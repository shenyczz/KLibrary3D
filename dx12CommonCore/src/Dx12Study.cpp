#include "stdafx.h"
#include "Dx12Study.h"


Dx12Study::Dx12Study()
{
}

Dx12Study::~Dx12Study()
{
}


#pragma region --鼠标消息处理

void Dx12Study::OnMouseDown(WPARAM wParam, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_pWindow->Handle());
}
void Dx12Study::OnMouseUp(WPARAM wParam, int x, int y)
{
	ReleaseCapture();
}
void Dx12Study::OnMouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta -= dx;
		m_Phi -= dy;

		// Restrict the angle mPhi.
		m_Phi = KMaths::Clamp(m_Phi, 0.1f, (float)KMaths::PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = KMaths::Clamp(m_Radius, 3.0f, 150.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	return;
}
void Dx12Study::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{

}

#pragma endregion


#pragma region --渲染管线流程

void Dx12Study::OnInit()
{
#ifdef _DEBUG
	KUtil::Trace(_T("--【BEG】OnInit"));
#endif

	CreateGraphics();

	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	{
		BuildAssets();	// 纯虚函数
	}
	ThrowIfFailed(m_CommandList->Close());

	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	WaitForGpu();

#ifdef _DEBUG
	KUtil::Trace(_T("--【END】OnInit"));
#endif
}

void Dx12Study::OnResize()
{
	assert(m_device);
	assert(m_SwapChain);
	assert(m_CommandAllocator);

	WaitForGpu();

	// 复位命令分配器
	ThrowIfFailed(m_CommandAllocator->Reset());

	// 复位命令列表
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	// 复位 RenderTarget
	for (int i = 0; i < BackBufferCount; ++i)
		m_RenderTargets[i].Reset();

	// 复位 DepthStencil
	m_DepthStencil.Reset();

	// SwapChain
	ThrowIfFailed(m_SwapChain->ResizeBuffers(
		BackBufferCount,
		m_pWindow->ClientWidth(),
		m_pWindow->ClientHeight(),
		m_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	// 后台缓冲区索引
	m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// 重新创建
	CreateRenderTargets();
	CreateDepthStencilAndView();
	CreateViewportAndScissorRect();

	// 关闭命令列表
	ThrowIfFailed(m_CommandList->Close());

	// 
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// Wait until resize is complete.
	WaitForGpu();

	// 
	Resized();

#ifdef _DEBUG
	KUtil::Trace(_T("--OnResize"));
#endif
}

void Dx12Study::OnUpdate()
{
	Update();

#ifdef _DEBUG
	KUtil::Trace(_T("--OnUpdate"));
#endif
}

void Dx12Study::OnRender()
{
	Draw();

#ifdef _DEBUG
	KUtil::Trace(_T("--OnRender"));
#endif
}

void Dx12Study::OnDestroy()
{
#ifdef _DEBUG
	KUtil::Trace(_T("--【BEG】OnDestroy"));
#endif

	WaitForGpu();	// in OnDestroy
	CloseHandle(m_FenceEvent);

#ifdef _DEBUG
	KUtil::Trace(_T("--【END】OnDestroy"));
#endif
}

void Dx12Study::OnApplicationIdle()
{
	OnUpdate();
	OnRender();
	DoAppIdle();
}

#pragma endregion


#pragma region --初始化之 - CreateGraphics()

void Dx12Study::CreateGraphics()
{
	CreateDevice();
	CreateSynchObjects();
	CreateDescriptorHeaps();

	CreateCommandObjects();
	CreateSwapChain();

	CreateRenderTargets();
	CreateDepthStencilAndView();

	CreateViewportAndScissorRect();

	// 开始处于封闭状态；当我们第一次引用命令列表时，我们将会复位它
	m_CommandList->Close();

	return;
}



void Dx12Study::CreateDevice()
{
	CreateFactory();

	if (m_useWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		DXUtil::GetHardwareAdapter(m_Factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}

	//Check4xMsaaQuality();
	StoreDescriptorSize();

	return;
}

void Dx12Study::CreateCommandObjects()
{
	// 1.命令队列
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	{
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;
	}
	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc,
		IID_PPV_ARGS(&m_CommandQueue)));

	// 2.命令分配器（用于保存命令记录）
	ThrowIfFailed(m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_CommandAllocator)));

	// 3.命令队列
	ThrowIfFailed(m_device->CreateCommandList(
		0,									// nodeMask
		D3D12_COMMAND_LIST_TYPE_DIRECT,		// D3D12_COMMAND_LIST_TYPE
		m_CommandAllocator.Get(),			// Associated(关联) command allocator
		nullptr,							// Reset()、SetPipelineState()均可设置
		IID_PPV_ARGS(&m_CommandList)));

	// 开始处于封闭状态；当我们第一次引用命令列表时，我们将会复位它
	//m_CommandList->Close();

	return;
}
void Dx12Study::CreateSwapChain()
{
	HWND hWnd = m_pWindow->Handle();
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();

	// 释放之前的交换链重新创建.
	m_SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	{
		swapChainDesc.BufferDesc.Width = iClientWidth;
		swapChainDesc.BufferDesc.Height = iClientHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = m_BackBufferFormat;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc = { 1,0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = BackBufferCount;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	}

	ComPtr<IDXGISwapChain> swapChain;
	ThrowIfFailed(m_Factory->CreateSwapChain(
		m_CommandQueue.Get(),
		&swapChainDesc,
		swapChain.GetAddressOf()));

	// 此示例不支持全屏转换.
	ThrowIfFailed(m_Factory->MakeWindowAssociation(m_pWindow->Handle(), DXGI_MWA_NO_ALT_ENTER));

	//ThrowIfFailed(factory->CreateSwapChainForHwnd(
	//	m_commandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
	//	Win32Application::GetHwnd(),
	//	&swapChainDesc,
	//	nullptr,
	//	nullptr,
	//	&swapChain));

	ThrowIfFailed(swapChain.As(&m_SwapChain));

	// 
	m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	return;
}

void Dx12Study::CreateSynchObjects()
{
	// 为 CPU、GPU 同步创建围栏
	ThrowIfFailed(m_device->CreateFence(
		0,						// FenceValue
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_Fence)));

	// Create an event handle to use for frame synchronization.
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}

	// 围栏值
	m_FenceValue = 0;

	return;
}
void Dx12Study::CreateDescriptorHeaps()
{
	// RTV heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = BackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf())));

	// DSV heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf())));

	return;
}

void Dx12Study::CreateRenderTargets()
{
	// 取得描述符堆中的描述符句柄
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Create a RTV for each frame.
	for (UINT i = 0; i < BackBufferCount; i++)
	{
		// 获取交换链中的缓冲区资源
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));

		// 句柄指针偏移到描述符堆中下一个缓冲区
		rtvHandle.ptr += m_rtvDescriptorSize * i;

		// 为获取的后台缓冲区创建渲染目标视图（RTV ）
		m_device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
	}

	return;
}
void Dx12Study::CreateDepthStencilAndView()
{
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();

	D3D12_RESOURCE_DESC depthStencilDesc;
	{
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;					// 资源维度
		depthStencilDesc.Alignment = 0;														// ?对齐
		depthStencilDesc.Width = iClientWidth;												// 纹理宽度
		depthStencilDesc.Height = iClientHeight;											// 纹理高度
		depthStencilDesc.DepthOrArraySize = 1;												// 纹理深度
		depthStencilDesc.MipLevels = 1;														// mipmap层级的数量(深度模板缓冲区只能有1个层级)
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;								// 深度模板格式
		depthStencilDesc.SampleDesc = { 1,0 };												// 对每个像素的采样次数
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;								// 纹理布局
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// 关于 depthStencilDesc.Format 的说明
		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.
	}

	D3D12_CLEAR_VALUE optClear;
	{
		optClear.Format = m_DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
	}

	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),		// 堆属性（默认堆只有GPU能访问，上传堆、回读堆、客户堆则不行）
		D3D12_HEAP_FLAG_NONE,									// 堆额外标志
		&depthStencilDesc,										// 资源描述符指针
		D3D12_RESOURCE_STATE_COMMON,							// 资源状态
		&optClear,												// 清除资源的优化值
		IID_PPV_ARGS(&m_DepthStencil)));


	// DSV 描述符
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	{
		dsvDesc.Format = m_DepthStencilFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;
	}

	m_device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc,
		m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// Transition the resource from its initial state to be used as a depth buffer.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencil.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	return;
}


// 工厂
void Dx12Study::CreateFactory()
{
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_Factory)));

	return;
}

// 记录各种描述符的大小，需要时可直接使用
void Dx12Study::StoreDescriptorSize()
{
	// 记录各种描述符的大小，需要时可直接使用
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_uavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
	// D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES
}

// 视图和剪切矩形（根据实际情况）
void Dx12Study::CreateViewportAndScissorRect()
{
	m_ScreenViewport = CD3DX12_VIEWPORT(
		0.0f, 0.0f,
		static_cast<float>(m_pWindow->ClientWidth()),
		static_cast<float>(m_pWindow->ClientHeight()));
	//m_CommandList->RSSetViewports(1, &m_ScreenViewport);

	m_ScissorRect = CD3DX12_RECT(
		1, 1,
		static_cast<long>(m_pWindow->ClientWidth()),
		static_cast<long>(m_pWindow->ClientHeight()));

	//为了实现高级效果，有时会采用多个剪切矩形
}

#pragma endregion


void Dx12Study::WaitForGpu()
{
	// 向命令队列中添加指令以设置新的围栏点.
	// 因为我们在GPU时间线上，在GPU完成前面的所有命令前，无法设置新的围栏点
	const UINT64 fenceValue = m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));
	m_FenceValue++;

	// 等待GPU完成到这个围栏点的命令
	if (m_Fence->GetCompletedValue() < fenceValue)
	{
		// 当GPU到达当前围栏点点燃事件
		ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	return;
}





void Dx12Study::CalculateFrameStats()
{
	KClock* pClock = KApplication::GetApp()->GetClock();

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((pClock->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;	// 每秒帧数
		float mspf = 1000.0f / fps;		// 每帧毫秒

		KString ss = KString::Format(_T("%s : fps = %4.1f, mspf = %f")
			, m_pWindow->GetWindowTitle()
			, fps
			, mspf);

		SetWindowText(m_pWindow->Handle(), ss);

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
