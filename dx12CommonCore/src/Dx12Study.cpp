#include "stdafx.h"
#include "Dx12Study.h"


Dx12Study::Dx12Study()
{
}

Dx12Study::~Dx12Study()
{
}


#pragma region --�����Ϣ����

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


#pragma region --��Ⱦ��������

void Dx12Study::OnInit()
{
#ifdef _DEBUG
	KUtil::Trace(_T("--��BEG��OnInit"));
#endif

	CreateGraphics();

	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	{
		BuildAssets();	// ���麯��
	}
	ThrowIfFailed(m_CommandList->Close());

	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	WaitForGpu();

#ifdef _DEBUG
	KUtil::Trace(_T("--��END��OnInit"));
#endif
}

void Dx12Study::OnResize()
{
	assert(m_device);
	assert(m_SwapChain);
	assert(m_CommandAllocator);

	WaitForGpu();

	// ��λ���������
	ThrowIfFailed(m_CommandAllocator->Reset());

	// ��λ�����б�
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	// ��λ RenderTarget
	for (int i = 0; i < BackBufferCount; ++i)
		m_RenderTargets[i].Reset();

	// ��λ DepthStencil
	m_DepthStencil.Reset();

	// SwapChain
	ThrowIfFailed(m_SwapChain->ResizeBuffers(
		BackBufferCount,
		m_pWindow->ClientWidth(),
		m_pWindow->ClientHeight(),
		m_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	// ��̨����������
	m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// ���´���
	CreateRenderTargets();
	CreateDepthStencilAndView();
	CreateViewportAndScissorRect();

	// �ر������б�
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
	KUtil::Trace(_T("--��BEG��OnDestroy"));
#endif

	WaitForGpu();	// in OnDestroy
	CloseHandle(m_FenceEvent);

#ifdef _DEBUG
	KUtil::Trace(_T("--��END��OnDestroy"));
#endif
}

void Dx12Study::OnApplicationIdle()
{
	OnUpdate();
	OnRender();
	DoAppIdle();
}

#pragma endregion


#pragma region --��ʼ��֮ - CreateGraphics()

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

	// ��ʼ���ڷ��״̬�������ǵ�һ�����������б�ʱ�����ǽ��Ḵλ��
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
	// 1.�������
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	{
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;
	}
	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc,
		IID_PPV_ARGS(&m_CommandQueue)));

	// 2.��������������ڱ��������¼��
	ThrowIfFailed(m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_CommandAllocator)));

	// 3.�������
	ThrowIfFailed(m_device->CreateCommandList(
		0,									// nodeMask
		D3D12_COMMAND_LIST_TYPE_DIRECT,		// D3D12_COMMAND_LIST_TYPE
		m_CommandAllocator.Get(),			// Associated(����) command allocator
		nullptr,							// Reset()��SetPipelineState()��������
		IID_PPV_ARGS(&m_CommandList)));

	// ��ʼ���ڷ��״̬�������ǵ�һ�����������б�ʱ�����ǽ��Ḵλ��
	//m_CommandList->Close();

	return;
}
void Dx12Study::CreateSwapChain()
{
	HWND hWnd = m_pWindow->Handle();
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();

	// �ͷ�֮ǰ�Ľ��������´���.
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

	// ��ʾ����֧��ȫ��ת��.
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
	// Ϊ CPU��GPU ͬ������Χ��
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

	// Χ��ֵ
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
	// ȡ�����������е����������
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Create a RTV for each frame.
	for (UINT i = 0; i < BackBufferCount; i++)
	{
		// ��ȡ�������еĻ�������Դ
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));

		// ���ָ��ƫ�Ƶ�������������һ��������
		rtvHandle.ptr += m_rtvDescriptorSize * i;

		// Ϊ��ȡ�ĺ�̨������������ȾĿ����ͼ��RTV ��
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
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;					// ��Դά��
		depthStencilDesc.Alignment = 0;														// ?����
		depthStencilDesc.Width = iClientWidth;												// ������
		depthStencilDesc.Height = iClientHeight;											// ����߶�
		depthStencilDesc.DepthOrArraySize = 1;												// �������
		depthStencilDesc.MipLevels = 1;														// mipmap�㼶������(���ģ�建����ֻ����1���㼶)
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;								// ���ģ���ʽ
		depthStencilDesc.SampleDesc = { 1,0 };												// ��ÿ�����صĲ�������
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;								// ������
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// ���� depthStencilDesc.Format ��˵��
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
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),		// �����ԣ�Ĭ�϶�ֻ��GPU�ܷ��ʣ��ϴ��ѡ��ض��ѡ��ͻ������У�
		D3D12_HEAP_FLAG_NONE,									// �Ѷ����־
		&depthStencilDesc,										// ��Դ������ָ��
		D3D12_RESOURCE_STATE_COMMON,							// ��Դ״̬
		&optClear,												// �����Դ���Ż�ֵ
		IID_PPV_ARGS(&m_DepthStencil)));


	// DSV ������
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


// ����
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

// ��¼�����������Ĵ�С����Ҫʱ��ֱ��ʹ��
void Dx12Study::StoreDescriptorSize()
{
	// ��¼�����������Ĵ�С����Ҫʱ��ֱ��ʹ��
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_uavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
	// D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES
}

// ��ͼ�ͼ��о��Σ�����ʵ�������
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

	//Ϊ��ʵ�ָ߼�Ч������ʱ����ö�����о���
}

#pragma endregion


void Dx12Study::WaitForGpu()
{
	// ��������������ָ���������µ�Χ����.
	// ��Ϊ������GPUʱ�����ϣ���GPU���ǰ�����������ǰ���޷������µ�Χ����
	const UINT64 fenceValue = m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));
	m_FenceValue++;

	// �ȴ�GPU��ɵ����Χ���������
	if (m_Fence->GetCompletedValue() < fenceValue)
	{
		// ��GPU���ﵱǰΧ�����ȼ�¼�
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
		float fps = (float)frameCnt;	// ÿ��֡��
		float mspf = 1000.0f / fps;		// ÿ֡����

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
