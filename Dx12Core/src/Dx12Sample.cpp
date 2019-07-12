#include "stdafx.h"
#include "Dx12Sample.h"


namespace DX12
{

#pragma region Constructors


	Dx12Sample::Dx12Sample()
		: m_useWarpDevice(false)
		, m_4xMsaaState(false)
		, m_4xMsaaQuality(0)
		, m_frameIndex(0)
		, m_fenceValues{}
		, m_fenceEvent(nullptr)
		, m_rtvDescriptorSize(0)
		, m_dsvDescriptorSize(0)
		, m_cbvDescriptorSize(0)
		, m_srvDescriptorSize(0)
		, m_uavDescriptorSize(0)
		, m_BackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
		, m_DepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)
		, m_Viewport(0.0f, 0.0f, 1.0f, 1.0f)
		, m_ScissorRect(0, 0, 1, 1)
		, m_VSByteCode(nullptr)
		, m_PSByteCode(nullptr)
		, m_TextureBuffer(nullptr)
		, m_VertexBuffer(nullptr)
		, m_VertexBufferView{}
	{
		m_World = MathHelper::Identity4x4();
		m_View = MathHelper::Identity4x4();
		m_Proj = MathHelper::Identity4x4();
	}

	Dx12Sample::~Dx12Sample()
	{
		DXGI_FORMAT::DXGI_FORMAT_420_OPAQUE;
	}


#pragma endregion



#pragma region �������̣� Init -> Render -> Destroy


	void Dx12Sample::OnInit()
	{
		InitializeGraphics();
		InitializeAssets();

		WaitForGpu();
	}

	void Dx12Sample::OnResize()
	{
		WindowResized();
	}

	void Dx12Sample::OnUpdate()
	{
		// ���²�������
		UpdateParameter();
	}

	void Dx12Sample::OnRender()
	{
		// ��¼����
		PopulateCommandList();

		// ִ�������б�
		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// ����֡
		ThrowIfFailed(m_SwapChain->Present(1, 0));

		// ��һ֡
		MoveToNextFrame();

		return;
	}

	void Dx12Sample::OnDestroy()
	{
		WaitForGpu();				// ȷ��GPU�������ü����������Դ
		CloseHandle(m_fenceEvent);	// �ر�Χ���¼�
		return;
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

		CreateRootSignature();		// �����ʵ���������

		CreateCommandObjects();
		CreateSwapChain();

		CreateSynchObjects();

		CreateFrameResources();
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
		//ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_frameIndex].Get(), m_PipelineState.Get()));
		//{
		//	BuildByteCode();
		//	BuildVertexBufferAndView();
		//	BuildIndexBufferAndView();
		//	BuildTextureBufferAndView();
		//	BuildInputElementDescrips();
		//	BuildConstantBufferAndView();
		//	BuildPipelineStateObject();
		//}
		//ThrowIfFailed(m_CommandList->Close());
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_frameIndex].Get(), m_PipelineState.Get()));
		{
			BuildByteCode();
			BuildInputElementDescrips();
			BuildPipelineStateObject();

			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_CommandAllocator[m_frameIndex].Get(), m_PipelineState.Get(), IID_PPV_ARGS(&m_CommandList)));

			BuildVertexBufferAndView();
			BuildIndexBufferAndView();
			BuildConstantBufferAndView();
			BuildTextureBufferAndView();
		}
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	// ���ڴ�С�ı�
	void Dx12Sample::WindowResized()
	{
		/// PI = 3.1415926F
		/// OPI = PI; QPI = PI/4; HPI = PI/2
		/// ���û������˴��ڳߴ磬��

		// 1.�����ݺ��
		m_fAspectRatio = (m_pWindow) ? m_pWindow->AaspectRatio() : 1.0f;

		// 2.���¼���ͶӰ����;
		XMMATRIX prj = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_fAspectRatio, 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_Proj, prj);

		return;
	}

	// UpdateParameter
	void Dx12Sample::UpdateParameter()
	{

	}

	// PopulateCommandList, ���������б�
	void Dx12Sample::PopulateCommandList()
	{
		// Reset ֮ǰҪ�� Close()
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		ThrowIfFailed(m_CommandAllocator[m_frameIndex]->Reset());

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before re-recording.
		//ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_frameIndex].Get(), nullptr));	// pso

		// Indicate that the back buffer will be used as a render target.
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);

			const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
			m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}
		// Indicate that the back buffer will now be used to present.
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		ThrowIfFailed(m_CommandList->Close());

		return;
	}

	// Prepare to render the next frame.
	void Dx12Sample::MoveToNextFrame()
	{
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), currentFenceValue));

		// Update the frame index.
		m_frameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if (m_Fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
		{
			ThrowIfFailed(m_Fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
			WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_fenceValues[m_frameIndex] = currentFenceValue + 1;
	}

	// Wait for the command list to execute pending GPU work to complete.
	void Dx12Sample::WaitForGpu()
	{
		// Schedule a Signal command in the queue.
		UINT64 fenceValue = m_fenceValues[m_frameIndex];
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		m_fenceValues[m_frameIndex]++;
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
			GetHardwareAdapter(m_Factory.Get(), &hardwareAdapter);

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
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
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

		// UAV - ?
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
		// ���ﴴ��һ���յĸ�ǩ��(Flags �޸�Ϊ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)
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

		//  Command List��PSO��
		{
			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_CommandAllocator[m_frameIndex].Get(),	// ��ʱ m_frameIndex = 0
				m_PipelineState.Get(),
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
		int width = m_pWindow->Width();
		int height = m_pWindow->Height();

		// ����������
		{
			//[1]DXGI_MODE_DESC BufferDesc;
			DXGI_MODE_DESC BufferDesc = {};
			{
				BufferDesc.Width = width;
				BufferDesc.Height = height;
				BufferDesc.RefreshRate.Numerator = 60;	// DXGI_RATIONAL
				BufferDesc.RefreshRate.Denominator = 1;	// DXGI_RATIONAL
				BufferDesc.Format = m_BackBufferFormat;	// DXGI_FORMAT
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
		m_frameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// �ӿںͼ��о���
		// This sample does not support fullscreen transitions.
		ThrowIfFailed(m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

		return;
	}

	// ͬ��������Ҫ������У�
	void Dx12Sample::CreateSynchObjects()
	{
		// Ϊ CPU��GPU ͬ������Χ��
		ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_frameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_Fence)));

		// Χ��ֵ
		m_fenceValues[m_frameIndex]++;

		// ����֡ͬ���¼�
		m_fenceEvent = ::CreateEvent(
			nullptr,	// LPSECURITY_ATTRIBUTES
			FALSE,		// bManualReset
			FALSE,		// bInitialState
			nullptr);	// lpName

		if (m_fenceEvent == nullptr)
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
	void Dx12Sample::CreateFrameResources()
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
		int width = m_pWindow->Width();
		int height = m_pWindow->Height();
		//
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
			IID_PPV_ARGS(m_DepthStencil.GetAddressOf())));


		//Map and initialize the DepthStencil buffer.
		//UINT8* m_pDsvDataBegin;
		//CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		//ThrowIfFailed(m_DepthStencil->Map(0, &readRange, reinterpret_cast<void**>(&m_pDsvDataBegin)));
		//memcpy(m_pDsvDataBegin, m_DepthStencil, sizeof(??));
		//m_ConstantBuffer->Unmap(0, nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Describe and create a constant buffer view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		//dsvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		//cbvDesc.SizeInBytes = (sizeof(m_ConstantBuffer) + 255) & ~255;
		//m_device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc, dsvHandle);


		return;
	}

	// ��ͼ�ͼ��о��Σ�����ʵ�������
	void Dx12Sample::CreateViewportAndScissorRect()
	{
		m_Viewport = CD3DX12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>(m_pWindow->Width()),
			static_cast<float>(m_pWindow->Height()));
		//m_CommandList->RSSetViewports(1, &m_Viewport);

		m_ScissorRect = CD3DX12_RECT(
			1, 1,
			static_cast<long>(m_pWindow->Width()),
			static_cast<long>(m_pWindow->Height()));
		//m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		///Ϊ��ʵ�ָ߼�Ч������ʱ����ö�����о���
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
		qulityLevels.Format = m_BackBufferFormat;
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

	// ������ɫ���ֽ��루������ɫ����������ɫ����
	void Dx12Sample::BuildByteCode()
	{
		/* Example:
		//ComPtr<ID3DBlob> m_VSByteCode;
		//ComPtr<ID3DBlob> m_PSByteCode;

		UINT compileFlags = DXUtil::GetCompileFlags();

		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("Asserts\\Triangle.hlsl");
		_tstring filePath = path + file;

		ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
			"VSMain", "vs_5_0", compileFlags, 0, &m_VSByteCode, nullptr));

		ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
			"PSMain", "ps_5_0", compileFlags, 0, &m_PSByteCode, nullptr));
		return;

		*/
	}

	// ���ݶ���ṹ��������Ԫ������
	void Dx12Sample::BuildInputElementDescrips()
	{
		/* Example:

		m_InputElementDescs.clear();	// std::vector<D3D12_INPUT_ELEMENT_DESC>
		m_InputElementDescs = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_INPUT_LAYOUT_DESC InputLayout = InputLayout();

		*/
	}

	// ͼ�ι���״̬����������ǩ�������벼�֡�������ɫ��Դ��
	void Dx12Sample::BuildPipelineStateObject()
	{
		/* Example:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { NULL };
		psoDesc.pRootSignature = m_RootSignature.Get();								// ��ǩ��
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_VSByteCode.Get());					// vertexShader		(������ɫ��)
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_PSByteCode.Get());					// pixelShader		(������ɫ��)
		psoDesc.DS = CD3DX12_SHADER_BYTECODE(m_DSByteCode.Get());					// domainShader		(����ɫ��)
		psoDesc.HS = CD3DX12_SHADER_BYTECODE(m_HSByteCode.Get());					// hullShader		(����ɫ��)
		psoDesc.GS = CD3DX12_SHADER_BYTECODE(m_GSByteCode.Get());					// geometryShader	(������ɫ��)
		psoDesc.StreamOutput = {};
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);						// ���״̬
		psoDesc.SampleMask = UINT_MAX;												// ������ģ
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);			// դ��״̬
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);		// ���ģ��״̬
		psoDesc.InputLayout = nputLayout();											// ���벼��
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;		// ��
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// ͼԪ��������
		psoDesc.NumRenderTargets = 1;												// ��ȾĿ������
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;							// ��ȾĿ���ʽn
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;									// ���ģ���ʽ
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };								// ��������
		psoDesc.NodeMask = 0;														// �ڵ���ģ
		psoDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE();							// PSO Cached
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;								// ���(?)

		// Create PSO
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

		return;
		*/
	}

	// ���㻺��������ͼ
	void Dx12Sample::BuildVertexBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_VertexBuffer;
		//D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	}

	// ��������������ͼ
	void Dx12Sample::BuildIndexBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_IndexBuffer;
		//D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	}

	// ������������ͼ
	void Dx12Sample::BuildTextureBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_TextureBuffer;
		//ComPtr<ID3D12Resource> m_TextureBufferUpload;
	}

	// ��������������ͼ
	void Dx12Sample::BuildConstantBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_ConstantBuffer;
		//ComPtr<ID3D12Resource> m_ConstantBufferUpload;
	}

#pragma endregion




	//_Use_decl_annotations_
	void Dx12Sample::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}




}//DX12
