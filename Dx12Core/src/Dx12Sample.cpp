#include "stdafx.h"
#include "Dx12Sample.h"


namespace DX12
{

#pragma region Constructors


	Dx12Sample::Dx12Sample()
	{
		m_useWarpDevice = false;
		m_4xMsaaState = false;
		m_4xMsaaQuality = 0;

		m_FrameIndex = 0;

		m_rtvDescriptorSize = 0;
		m_dsvDescriptorSize = 0;
		m_cbvDescriptorSize = 0;
		m_srvDescriptorSize = 0;
		m_uavDescriptorSize = 0;

		m_RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		m_World = DXUtil::Identity4x4;
		m_View = DXUtil::Identity4x4;
		m_Proj = DXUtil::Identity4x4;

		m_Factory = nullptr;
		m_device = nullptr;

		m_rtvHeap = nullptr;
		m_dsvHeap = nullptr;
		m_cbvHeap = nullptr;
		m_srvHeap = nullptr;
		m_uavHeap = nullptr;

		m_RootSignature = nullptr;

		m_CommandQueue = nullptr;
		m_CommandList = nullptr;
		m_SwapChain = nullptr;
		m_Fence = nullptr;
		m_FenceEvent = nullptr;

		for (int i = 0; i < FrameCount; i++)
		{
			m_FenceValues[i] = 0;
			m_RenderTargets[i] = nullptr;
			m_CommandAllocator[i] = nullptr;
		}

		m_DepthStencil = nullptr;

		m_VSByteCode = nullptr;
		m_PSByteCode = nullptr;

		m_pso = nullptr;

		m_ScreenViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, 1.0f, 1.0f);
		m_ScissorRect = CD3DX12_RECT(0, 0, 1, 1);
	}

	Dx12Sample::~Dx12Sample()
	{
		DXGI_FORMAT::DXGI_FORMAT_420_OPAQUE;
	}


#pragma endregion


#pragma region �������̣�Init -> Render -> Destroy��


	void Dx12Sample::OnInit()
	{
#ifdef _DEBUG
		KUtil::Trace(_T("--��BEG��OnInit"));
#endif

		InitializeGraphics();
		InitializeAssets();		// 

#ifdef _DEBUG
		KUtil::Trace(_T("--��END��OnInit"));
#endif
	}

	void Dx12Sample::OnResize()
	{
		assert(m_device);
		assert(m_SwapChain);
		assert(m_CommandAllocator[0]);
		assert(m_CommandAllocator[1]);

		int width = m_pWindow->ClientWidth();
		int height = m_pWindow->ClientHeight();

		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), m_pso.Get()));

		// Release the previous resources we will be recreating.
		for (int i = 0; i < FrameCount; ++i)
		{
			m_RenderTargets[i].Reset();
		}

		m_DepthStencil.Reset();

		// Resize the swap chain.
		ThrowIfFailed(m_SwapChain->ResizeBuffers(
			FrameCount,
			width,
			height,
			m_RenderTargetFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		m_FrameIndex = 0;

		CreateRenderTargetsResources();
		CreateDepthStencilResources();

		// Execute the resize commands.
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		//
		WaitForGpu();

		// Update the viewport transform to cover the client area.
		m_ScreenViewport.TopLeftX = 0;
		m_ScreenViewport.TopLeftY = 0;
		m_ScreenViewport.Width = static_cast<float>(width);
		m_ScreenViewport.Height = static_cast<float>(height);
		m_ScreenViewport.MinDepth = 0.0f;
		m_ScreenViewport.MaxDepth = 1.0f;

		m_ScissorRect = { 0, 0, width, height };

#ifdef _DEBUG
		KUtil::Trace(_T("OnResize"));
#endif
	}

	void Dx12Sample::OnUpdate()
	{
		// ���²�������

#ifdef _DEBUG
		KUtil::Trace(_T("OnUpdate"));
#endif
	}

	void Dx12Sample::OnRender()
	{
#ifdef _DEBUG
		KUtil::Trace(_T("--��BEG��OnRender"));
#endif

		// ��¼����
		PopulateCommandList();

		// ִ�������б�
		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// ����֡
		ThrowIfFailed(m_SwapChain->Present(1, 0));

		// ��һ֡
		MoveToNextFrame();

#ifdef _DEBUG
		KUtil::Trace(_T("--��END��OnRender"));
#endif
	}

	void Dx12Sample::OnDestroy()
	{
		WaitForGpu();				// ȷ��GPU�������ü����������Դ
		CloseHandle(m_FenceEvent);	// �ر�Χ���¼�

#ifdef _DEBUG
		KUtil::Trace(_T("OnDestroy"));
#endif
	}


#pragma endregion



	// ��ʼ��ͼ�ι���
	// [01]�����豸������豸��4X MSAA���������֧�������
	// [02]������������
	// [03]������ǩ��
	// [04]�����������
	// [05]����������
	// [06]����ͬ������
	// [07]����֡��Դ
	// [08]�������ģ��Դ
	// [09]�����ӿںͼ��о���
	void Dx12Sample::InitializeGraphics()
	{
		CreateDevice();

		CreateDescriptorHeap();

		CreateRootSignature();

		CreateCommandObjects();
		CreateSwapChain();

		CreateSynchObjects();

		CreateRenderTargetsResources();
		CreateDepthStencilResources();

		CreateViewportAndScissorRect();
	}

	// ��ʼ����Դ
	// [xx]BuildByteCode()
	// [xx]BuildVertexBufferAndView()
	// [xx]BuildIndexBufferAndView()
	// [xx]BuildConstantBufferAndView()
	// [xx]BuildInputElementDescrips()
	// [xx]BuildPipelineStateObject()
	void Dx12Sample::InitializeAssets()
	{
		// ���������б��Ա��ʼ��
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), m_pso.Get()));
		{
			BuildRootSignature();
			BuildShadersAndInputLayout();
			BuildMeshData();
			BuildConstantBufferAndView();
			BuildTextureBufferAndView();
			BuildPipelineStateObject();
		}
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		return;
	}

	// ���������б�
	//
	void Dx12Sample::PopulateCommandList()
	{
		// Reset ֮ǰҪ�� Close()
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		ThrowIfFailed(m_CommandAllocator[m_FrameIndex]->Reset());

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before re-recording.
		//ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_pso.Get()));
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), m_pso.Get()));

		// ���ø�������
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// ��ͼ�ͼ��о���
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// ����ϲ�
		m_CommandList->OMSetRenderTargets(1, &RenderTargetView(), FALSE, nullptr);

		// ������Դ��;ָʾ��״̬��ת�䣬�˴�����Դ�ӳ���״̬ת��Ϊ��Ⱦ״̬
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		{
			// ��¼�����б�
			ReccorCommand();
		}
		// ������Դ��;ָʾ��״̬��ת�䣬�˴�����Դ����Ⱦ״̬ת��Ϊ����״̬
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		ThrowIfFailed(m_CommandList->Close());

		return;
	}

	// Prepare to render the next frame.
	//
	void Dx12Sample::MoveToNextFrame()
	{
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_FenceValues[m_FrameIndex];
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), currentFenceValue));

		// Update the frame index.
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_Fence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
		{
			ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent));
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_FenceValues[m_FrameIndex] = currentFenceValue + 1;
	}

	// Wait for the command list to execute pending GPU work to complete.
	//
	void Dx12Sample::WaitForGpu()
	{
		try
		{
			// Schedule a Signal command in the queue.
			UINT64 fenceValue = m_FenceValues[m_FrameIndex];
			ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));

			// Wait until the fence has been processed.
			ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

			// Increment the fence value for the current frame.
			m_FenceValues[m_FrameIndex]++;
		}
		catch (const std::exception&)
		{

		}
	}



#pragma region InitializeGraphics() ����

	// �豸
	void Dx12Sample::CreateDevice()
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

		Check4xMsaaQuality();
		RecordDescriptorSize();

		return;
	}

	// ��������
	void Dx12Sample::CreateDescriptorHeap()
	{
		// RTV - Render Target View descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		// DSV - Depth Stencil view descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

		// SRV - Shader Resource View heap for the texture.
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

		// CBV - Constant Buffer View (CBV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));

		// UAV - �����ȡ View
		D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
		uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavHeapDesc.NumDescriptors = 1;
		uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_uavHeap)));

		return;
	}

	// ��ǩ��������ʵ��������أ�
	void Dx12Sample::CreateRootSignature()
	{
		// ���ﴴ��һ���յĸ�ǩ��
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(
			0,									// UINT numParameters
			nullptr,							// D3D12_ROOT_PARAMETER* _pParameters
			0,									// UINT numStaticSamplers = 0
			nullptr,							// D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> error;
		ComPtr<ID3DBlob> signature;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));

		ThrowIfFailed(m_device->CreateRootSignature(0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_RootSignature)));

		return;
	}

	// ���������Ҫ��ǩ����
	void Dx12Sample::CreateCommandObjects()
	{
		// Command Queue
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDesc.Priority = 0;		// ����Ȩ
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.NodeMask = 0;
			ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
		}

		// Command Allocator
		for (UINT i = 0; i < FrameCount; i++)
		{
			// Command Allocator
			ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&m_CommandAllocator[i])));
		}

		//  Command List
		{
			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_CommandAllocator[m_FrameIndex].Get(),	// ��ʱ m_FrameIndex = 0
				m_pso.Get(),
				IID_PPV_ARGS(&m_CommandList)));

			// Command lists are created in the recording state,
			// but there is nothing to record yet.
			// The main loop expects it to be closed now.
			ThrowIfFailed(m_CommandList->Close());
		}

		return;
	}

	// �������������ʵ�������
	void Dx12Sample::CreateSwapChain()
	{
		// ������������Ҫ���ڲ������������
		HWND hWnd = m_pWindow->Handle();
		int width = m_pWindow->ClientWidth();
		int height = m_pWindow->ClientHeight();

		// ����������
		{
			//[1]DXGI_MODE_DESC BufferDesc;
			DXGI_MODE_DESC BufferDesc = {};
			{
				BufferDesc.Width = width;
				BufferDesc.Height = height;
				BufferDesc.RefreshRate.Numerator = 60;	// DXGI_RATIONAL
				BufferDesc.RefreshRate.Denominator = 1;	// DXGI_RATIONAL
				BufferDesc.Format = m_RenderTargetFormat;	// DXGI_FORMAT
				BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// DXGI_MODE_SCANLINE_ORDER
				BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// DXGI_MODE_SCALING
			}
			//[2]DXGI_SAMPLE_DESC SampleDesc;
			DXGI_SAMPLE_DESC SampleDesc = {};
			{
				SampleDesc.Count = m_4xMsaaState ? 4 : 1;
				SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
			}
			//[3]DXGI_USAGE BufferUsage;
			DXGI_USAGE BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			//[4]UINT BufferCount;
			UINT BufferCount = FrameCount;
			// 5]HWND OutputWindow;
			HWND OutputWindow = hWnd;
			//[6]BOOL Windowed;
			BOOL Windowed = TRUE;
			//[7]DXGI_SWAP_EFFECT SwapEffect;
			DXGI_SWAP_EFFECT SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			//[8]UINT Flags;
			UINT Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			DXGI_SWAP_CHAIN_DESC swapChainDesc =
			{
				BufferDesc,		//[1]DXGI_MODE_DESC BufferDesc;
				SampleDesc,		//[2]DXGI_SAMPLE_DESC SampleDesc;
				BufferUsage,	//[3]DXGI_USAGE BufferUsage;
				BufferCount,	//[4]UINT BufferCount;
				OutputWindow,	//[5]HWND OutputWindow;
				Windowed,		//[6]BOOL Windowed;
				SwapEffect,		//[7]DXGI_SWAP_EFFECT SwapEffect;
				Flags			//[8]UINT Flags;
			};

			ComPtr<IDXGISwapChain> swapChain;
			ThrowIfFailed(m_Factory->CreateSwapChain(
				m_CommandQueue.Get(),	// ��������Ҫ����
				&swapChainDesc,
				swapChain.GetAddressOf()));

			ThrowIfFailed(swapChain.As(&m_SwapChain));
		}

		// ȡ�õ�ǰ��������������
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// �ӿںͼ��о���
		// This sample does not support fullscreen transitions.
		ThrowIfFailed(m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

		return;
	}

	// ͬ��������Ҫ������У�
	void Dx12Sample::CreateSynchObjects()
	{
		// Ϊ CPU��GPU ͬ������Χ��
		ThrowIfFailed(m_device->CreateFence(m_FenceValues[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_Fence)));

		// Χ��ֵ
		m_FenceValues[m_FrameIndex]++;

		// ����֡ͬ���¼�
		m_FenceEvent = ::CreateEvent(
			nullptr,	// LPSECURITY_ATTRIBUTES
			FALSE,		// bManualReset
			FALSE,		// bInitialState
			nullptr);	// lpName

		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute;
		// we are reusing the same command list in our main loop but for now,
		// we just want to wait for setup t complete before continuing.
		WaitForGpu();

		return;
	}

	// ֡��Դ
	void Dx12Sample::CreateRenderTargetsResources()
	{
		// ȡ�����������е����������
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT i = 0; i < FrameCount; i++)
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

	// ���ģ�壨�����ʵ�������
	void Dx12Sample::CreateDepthStencilResources()
	{
		int width = m_pWindow->ClientWidth();
		int height = m_pWindow->ClientHeight();

		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;					// ��Դά��
		depthStencilDesc.Alignment = 0;														// ?����
		depthStencilDesc.Width = width;														// ������
		depthStencilDesc.Height = height;													// ����߶�
		depthStencilDesc.DepthOrArraySize = 1;												// �������
		depthStencilDesc.MipLevels = 1;														// mipmap�㼶������(���ģ�建����ֻ����1���㼶)
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;								// ���ģ���ʽ
		depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;							// ��ÿ�����صĲ�������
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;	// ���ز�����������
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;								// ������
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;					// ?
		/*
		// ���� depthStencilDesc.Format ��˵��
		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		*/

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = m_DepthStencilFormat;	// 
		optClear.DepthStencil.Depth = 1.0f;		// 
		optClear.DepthStencil.Stencil = 0;		// 

		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),		// �����ԣ�Ĭ�϶�ֻ��GPU�ܷ��ʣ��ϴ��ѡ��ض��ѡ��ͻ������У�
			D3D12_HEAP_FLAG_NONE,									// �Ѷ����־
			&depthStencilDesc,										// ��Դ������ָ��
			D3D12_RESOURCE_STATE_COMMON,							// ��Դ״̬
			&optClear,												// �����Դ���Ż�ֵ
			IID_PPV_ARGS(&m_DepthStencil)));
		//	IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = m_DepthStencilFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;

		// hDsv
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		m_device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc, dsvHandle);

		// Transition the resource from its initial state to be used as a depth buffer.
		//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencil.Get(),
		//	D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		return;
	}

	// ��ͼ�ͼ��о��Σ�����ʵ�������
	void Dx12Sample::CreateViewportAndScissorRect()
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
		//m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		//Ϊ��ʵ�ָ߼�Ч������ʱ����ö�����о���
	}


	// ��������
	void Dx12Sample::CreateFactory()
	{
		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
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
	void Dx12Sample::Check4xMsaaQuality()
	{
		//����û��豸��4X MSAA���������֧�������
		//��һ��֧��Direct3D 11���豸�����е���ȾĿ���ʽ���Ѿ�֧��4X MSAA�ˣ�
		//����ֻ������������ļ��
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qulityLevels = {};
		qulityLevels.Format = m_RenderTargetFormat;
		qulityLevels.SampleCount = 4;	// check 4x MSAA
		qulityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		qulityLevels.NumQualityLevels = 0;	// _Out_

		ThrowIfFailed(m_device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&qulityLevels,
			sizeof(qulityLevels)));

		m_4xMsaaQuality = qulityLevels.NumQualityLevels;

		return;
	}
	// ��¼�����������Ĵ�С����Ҫʱ��ֱ��ʹ��
	void Dx12Sample::RecordDescriptorSize()
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


#pragma endregion


#pragma region InitializeAssets() ����

	// ������ǩ��
	void Dx12Sample::BuildRootSignature()
	{
		// ����
	}

	// ������ɫ����������ɫ����������ɫ���������벼��
	void Dx12Sample::BuildShadersAndInputLayout()
	{
		/* Example:

		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("Asserts\\Triangle.hlsl");
		_tstring filePath = path + file;

		m_VSByteCode = DXUtil::CompileShader(filePath, nullptr, "VS", "vs_5_0");
		m_PSByteCode = DXUtil::CompileShader(filePath, nullptr, "PS", "ps_5_0");


		UINT offsetPos = 0;
		UINT offsetClr = sizeof(Vertex::position);
		m_InputLayout.clear();
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetPos,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 },

			D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetClr, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		*/
	}

	// ͼ�ι���״̬����������ǩ�������벼�֡�������ɫ��Դ��
	void Dx12Sample::BuildPipelineStateObject()
	{
		/* Example:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { };
		psoDesc.pRootSignature = m_RootSignature.Get();								// ��ǩ��
		psoDesc.InputLayout = InputLayout();										// ���벼��
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_VSByteCode.Get());					// vertexShader		(������ɫ��)
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_PSByteCode.Get());					// pixelShader		(������ɫ��)
		psoDesc.DS = CD3DX12_SHADER_BYTECODE(m_DSByteCode.Get());					// domainShader		(����ɫ��)
		psoDesc.HS = CD3DX12_SHADER_BYTECODE(m_HSByteCode.Get());					// hullShader		(����ɫ��)
		psoDesc.GS = CD3DX12_SHADER_BYTECODE(m_GSByteCode.Get());					// geometryShader	(������ɫ��)
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);						// ���״̬
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);			// դ��״̬
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);		// ���ģ��״̬
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;		// ��
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// ͼԪ��������
		psoDesc.NumRenderTargets = 1;												// ��ȾĿ������
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;							// ��ȾĿ���ʽn
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;									// ���ģ���ʽ
		psoDesc.SampleMask = UINT_MAX;												// ������ģ
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };								// ��������

		psoDesc.StreamOutput = {};													// �����
		psoDesc.NodeMask = 0;														// �ڵ���ģ
		psoDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE();							// PSO Cached
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;								// ���(?)

		// Create PSO
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso)));

		return;
		*/
	}

	// ������������
	void Dx12Sample::BuildMeshData()
	{
		/*
		//ComPtr<ID3D12Resource> m_VertexBuffer;
		//D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		// 1.1 ��������
		// 1.2 ���㻺����
		// 1.3 ���㻺������ͼ

		//ComPtr<ID3D12Resource> m_IndexBuffer;
		//D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		// 2.1 ��������
		// 2.2 ����������
		// 2.3 ������������ͼ
		*/
	}

	// ��������������ͼ
	void Dx12Sample::BuildConstantBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_ConstantBuffer;
		//ComPtr<ID3D12Resource> m_ConstantBufferUpload;
	}


	// ������������ͼ
	void Dx12Sample::BuildTextureBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_Texture;
		//ComPtr<ID3D12Resource> m_TextureUpload;
	}

#pragma endregion


#pragma region OnRender() ����

	void Dx12Sample::ReccorCommand()
	{
		/* Example:

		// �����Ⱦ����
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_CommandList->ClearRenderTargetView(RtvHandleCPU(), clearColor, 0, nullptr);

		// ����װ��
		m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		m_CommandList->IASetIndexBuffer(&m_IndexBufferView);


		// ����
		if (m_mesh.IsUseIndex())
		{
			int indexCount = m_mesh.IndexCount();
			m_CommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		}
		else
		{
			int vertexCount = m_mesh.VertexCount();
			m_CommandList->DrawInstanced(vertexCount, 1, 0, 0);
		}

		������

		*/
	}

#pragma endregion







}//DX12
