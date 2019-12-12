#include "stdafx.h"
#include "Dx12Book.h"


#pragma region --Constructors

Dx12Book::Dx12Book()
{
	m_useWarpDevice = false;

	m_4xMsaaState = false;
	m_4xMsaaQuality = 0;

	m_BackBufferIndex = 0;

	m_rtvDescriptorSize = 0;
	m_dsvDescriptorSize = 0;
	m_cbvDescriptorSize = 0;
	m_srvDescriptorSize = 0;
	m_uavDescriptorSize = 0;

	m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	m_World = DXUtils::Identity4x4;
	m_View = DXUtils::Identity4x4;
	m_Proj = DXUtils::Identity4x4;

	//ComPtr<X>
	m_Factory = nullptr;
	m_device = nullptr;

	m_rtvHeap = nullptr;
	m_dsvHeap = nullptr;

	m_Fence = nullptr;
	m_FenceEvent = nullptr;
	m_FenceValue = 0;

	m_CommandQueue = nullptr;
	m_CommandAllocator = nullptr;
	m_CommandList = nullptr;
	m_SwapChain = nullptr;

	for (int i = 0; i < BackBufferCount; i++)
	{
		m_RenderTargets[i] = nullptr;
	}

	m_DepthStencil = nullptr;

	m_Shaders.clear();
	m_InputLayout.clear();

	m_ScreenViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, 1.0f, 1.0f);
	m_ScissorRect = CD3DX12_RECT(0, 0, 1, 1);

	m_Theta = XM_PI * 3 / 2;
	m_Phi = XM_PI / 4;
	m_Radius = 5.0f;
}

Dx12Book::~Dx12Book()
{
}

#pragma endregion


#pragma region --�����Ϣ����

void Dx12Book::OnMouseDown(WPARAM wParam, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_pWindow->Handle());
}
void Dx12Book::OnMouseUp(WPARAM wParam, int x, int y)
{
	ReleaseCapture();
}
void Dx12Book::OnMouseMove(WPARAM wParam, int x, int y)
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

#pragma endregion


#pragma region --��Ⱦ��������

void Dx12Book::OnInitialize()
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

	WaitForGpu();	// in OnInit

#ifdef _DEBUG
	KUtil::Trace(_T("--��END��OnInit"));
#endif
}

void Dx12Book::OnResize()
{
	assert(m_device);
	assert(m_SwapChain);
	assert(m_CommandAllocator);

	WaitForGpu();

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

	// AfterResised
	Resized();

#ifdef _DEBUG
	KUtil::Trace(_T("--OnResize"));
#endif
}

void Dx12Book::OnUpdate()
{
	Update();

#ifdef _DEBUG
	KUtil::Trace(_T("--OnUpdate"));
#endif
}

void Dx12Book::OnRender()
{
	Draw();

#ifdef _DEBUG
	KUtil::Trace(_T("--OnRender"));
#endif
}

void Dx12Book::OnDestroy()
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

void Dx12Book::OnAppIdle()
{
	DoAppIdle();
}


#pragma endregion


#pragma region --��ʼ��֮ - CreateGraphics()

void Dx12Book::CreateGraphics()
{
	CreateDevice();

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();

	CreateSynchObjects();
	CreateDescriptorHeaps();

	CreateRenderTargets();
	CreateDepthStencilAndView();

	// ��ʼ���ڷ��״̬�������ǵ�һ�����������б�ʱ�����ǽ��Ḵλ��
	m_CommandList->Close();

	return;
}


// �豸
void Dx12Book::CreateDevice()
{
	CreateFactory();

	// Try to create hardware device.
	m_useWarpDevice = false;
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_device));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		m_useWarpDevice = true;

		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)));
	}

	Check4xMsaaQuality();
	StoreDescriptorSize();

	return;
}

// �������
void Dx12Book::CreateCommandObjects()
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

// ������
void Dx12Book::CreateSwapChain()
{
	HWND hWnd = m_pWindow->Handle();
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();

	// Release the previous swapchain we will be recreating.
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
		swapChainDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
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

	ThrowIfFailed(swapChain.As(&m_SwapChain));

	// This sample does not support fullscreen transitions.
	ThrowIfFailed(m_Factory->MakeWindowAssociation(m_pWindow->Handle(), DXGI_MWA_NO_ALT_ENTER));

	// 
	m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	return;
}

// ͬ������
void Dx12Book::CreateSynchObjects()
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

// ��������
void Dx12Book::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = BackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf())));

	return;
}

// ֡��Դ
void Dx12Book::CreateRenderTargets()
{
	// ȡ�����������е����������
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each frame.
	for (UINT i = 0; i < BackBufferCount; i++)
	{
		// ��ȡ�������еĻ�������Դ
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));

		// Ϊ��ȡ�ĺ�̨������������ȾĿ����ͼ��RTV������ = nullptr��
		m_device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);

		// ���ָ��ƫ�Ƶ�������������һ��������
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	return;
}

// ���ģ��
void Dx12Book::CreateDepthStencilAndView()
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
		depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;							// ��ÿ�����صĲ�������
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;	// ���ز�����������
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


	// DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = m_DepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencil.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	return;
}

// ��������
void Dx12Book::CreateFactory()
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

// ����û��豸��4X MSAA���������֧�����
void Dx12Book::Check4xMsaaQuality()
{
	//����û��豸��4X MSAA���������֧�������
	//��һ��֧��Direct3D 11���豸�����е���ȾĿ���ʽ���Ѿ�֧��4X MSAA�ˣ�
	//����ֻ������������ļ��
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qulityLevels = {};
	qulityLevels.Format = m_BackBufferFormat;
	qulityLevels.SampleCount = 4;	// check 4x MSAA
	qulityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	qulityLevels.NumQualityLevels = 0;	// _Out_

	ThrowIfFailed(m_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&qulityLevels,
		sizeof(qulityLevels)));

	m_4xMsaaQuality = qulityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	return;
}

// ��¼�����������Ĵ�С����Ҫʱ��ֱ��ʹ��
void Dx12Book::StoreDescriptorSize()
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
void Dx12Book::CreateViewportAndScissorRect()
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


void Dx12Book::BuildAssets() {}
void Dx12Book::Resized()
{
	// ����ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}
void Dx12Book::Update() {}
void Dx12Book::Draw()
{
	// For Example:
	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(pCommandAllocator->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, nullptr));

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Specify the buffers we are going to render to.
	m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	{
		//m_CommandList->SetGraphicsRootSignature(nullptr);
	}

	// �������
	{
		// �����Ⱦ����
		//const float* clearColor = Colors::Black;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// ������ģ����ͼ
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);
	}

	{
		//DrawRenderItems(m_CommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
		//DrawRenderItems2();
	}

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(m_CommandList->Close());

	// ���� - presentation

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(m_SwapChain->Present(0, 0));

	// 
	WaitForGpu();	// in Draw
}
void Dx12Book::DoAppIdle()
{
#ifdef _DEBUG
	CalculateFrameStats();
#endif
}


void Dx12Book::WaitForGpu()
{
	// ��������������ָ���������µ�Χ����.
	// ��Ϊ������GPUʱ�����ϣ���GPU���ǰ�����������ǰ���޷������µ�Χ����
	const UINT64 fenceValue = m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));
	m_FenceValue++;

	// �ȴ�GPU��ɵ����Χ���������
	if (m_Fence->GetCompletedValue() < fenceValue)
	{
		HANDLE hEvent = m_FenceEvent;

		// ��GPU���ﵱǰΧ�����ȼ�¼�
		ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, hEvent));
		WaitForSingleObject(hEvent, INFINITE);
	}

	m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	return;
}



#pragma region Private Functions

void Dx12Book::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (m_Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}
void Dx12Book::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, m_BackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}
void Dx12Book::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

void Dx12Book::CalculateFrameStats()
{
	HWND hWnd = m_pWindow->Handle();
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

		SetWindowText(hWnd, ss);

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

#pragma endregion


