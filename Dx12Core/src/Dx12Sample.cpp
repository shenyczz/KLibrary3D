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


#pragma region 工作流程（Init -> Render -> Destroy）


	void Dx12Sample::OnInit()
	{
#ifdef _DEBUG
		KUtil::Trace(_T("--【BEG】OnInit"));
#endif

		InitializeGraphics();
		InitializeAssets();		// 

#ifdef _DEBUG
		KUtil::Trace(_T("--【END】OnInit"));
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
		// 更新参数数据

#ifdef _DEBUG
		KUtil::Trace(_T("OnUpdate"));
#endif
	}

	void Dx12Sample::OnRender()
	{
#ifdef _DEBUG
		KUtil::Trace(_T("--【BEG】OnRender"));
#endif

		// 记录命令
		PopulateCommandList();

		// 执行命令列表
		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// 呈现帧
		ThrowIfFailed(m_SwapChain->Present(1, 0));

		// 下一帧
		MoveToNextFrame();

#ifdef _DEBUG
		KUtil::Trace(_T("--【END】OnRender"));
#endif
	}

	void Dx12Sample::OnDestroy()
	{
		WaitForGpu();				// 确保GPU不再引用即将清理的资源
		CloseHandle(m_FenceEvent);	// 关闭围栏事件

#ifdef _DEBUG
		KUtil::Trace(_T("OnDestroy"));
#endif
	}


#pragma endregion



	// 初始化图形管线
	// [01]创建设备（检测设备对4X MSAA质量级别的支持情况）
	// [02]创建描述符堆
	// [03]创建根签名
	// [04]创建命令对象
	// [05]创建交换链
	// [06]创建同步对象
	// [07]创建帧资源
	// [08]创建深度模资源
	// [09]设置视口和剪切矩形
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

	// 初始化资源
	// [xx]BuildByteCode()
	// [xx]BuildVertexBufferAndView()
	// [xx]BuildIndexBufferAndView()
	// [xx]BuildConstantBufferAndView()
	// [xx]BuildInputElementDescrips()
	// [xx]BuildPipelineStateObject()
	void Dx12Sample::InitializeAssets()
	{
		// 重置命令列表以便初始化
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

	// 处理命令列表
	//
	void Dx12Sample::PopulateCommandList()
	{
		// Reset 之前要先 Close()
		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		ThrowIfFailed(m_CommandAllocator[m_FrameIndex]->Reset());

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before re-recording.
		//ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_pso.Get()));
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), m_pso.Get()));

		// 设置根描述表
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 视图和剪切矩形
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出合并
		m_CommandList->OMSetRenderTargets(1, &RenderTargetView(), FALSE, nullptr);

		// 按照资源用途指示其状态的转变，此处将资源从呈现状态转换为渲染状态
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		{
			// 记录命令列表
			ReccorCommand();
		}
		// 按照资源用途指示其状态的转变，此处将资源从渲染状态转换为呈现状态
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



#pragma region InitializeGraphics() 调用

	// 设备
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

	// 描述符堆
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

		// UAV - 无序存取 View
		D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
		uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavHeapDesc.NumDescriptors = 1;
		uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_uavHeap)));

		return;
	}

	// 根签名（根据实际情况重载）
	void Dx12Sample::CreateRootSignature()
	{
		// 这里创建一个空的根签名
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

	// 命令对象（需要根签名）
	void Dx12Sample::CreateCommandObjects()
	{
		// Command Queue
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDesc.Priority = 0;		// 优先权
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
				m_CommandAllocator[m_FrameIndex].Get(),	// 此时 m_FrameIndex = 0
				m_pso.Get(),
				IID_PPV_ARGS(&m_CommandList)));

			// Command lists are created in the recording state,
			// but there is nothing to record yet.
			// The main loop expects it to be closed now.
			ThrowIfFailed(m_CommandList->Close());
		}

		return;
	}

	// 交换链（需根据实际情况）
	void Dx12Sample::CreateSwapChain()
	{
		// 创建交换链需要窗口参数和命令队列
		HWND hWnd = m_pWindow->Handle();
		int width = m_pWindow->ClientWidth();
		int height = m_pWindow->ClientHeight();

		// 创建交换链
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
				m_CommandQueue.Get(),	// 交换链需要队列
				&swapChainDesc,
				swapChain.GetAddressOf()));

			ThrowIfFailed(swapChain.As(&m_SwapChain));
		}

		// 取得当前背景缓冲区索引
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// 视口和剪切矩形
		// This sample does not support fullscreen transitions.
		ThrowIfFailed(m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

		return;
	}

	// 同步对象（需要命令队列）
	void Dx12Sample::CreateSynchObjects()
	{
		// 为 CPU、GPU 同步创建围栏
		ThrowIfFailed(m_device->CreateFence(m_FenceValues[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_Fence)));

		// 围栏值
		m_FenceValues[m_FrameIndex]++;

		// 创建帧同步事件
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

	// 帧资源
	void Dx12Sample::CreateRenderTargetsResources()
	{
		// 取得描述符堆中的描述符句柄
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT i = 0; i < FrameCount; i++)
		{
			// 获取交换链中的缓冲区资源
			ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));

			// 为获取的后台缓冲区创建渲染目标视图（RTV描述符 = nullptr）
			m_device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);

			// 句柄指针偏移到描述符堆中下一个缓冲区
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}

		return;
	}

	// 深度模板（需根据实际情况）
	void Dx12Sample::CreateDepthStencilResources()
	{
		int width = m_pWindow->ClientWidth();
		int height = m_pWindow->ClientHeight();

		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;					// 资源维度
		depthStencilDesc.Alignment = 0;														// ?对齐
		depthStencilDesc.Width = width;														// 纹理宽度
		depthStencilDesc.Height = height;													// 纹理高度
		depthStencilDesc.DepthOrArraySize = 1;												// 纹理深度
		depthStencilDesc.MipLevels = 1;														// mipmap层级的数量(深度模板缓冲区只能有1个层级)
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;								// 深度模板格式
		depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;							// 对每个像素的采样次数
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;	// 多重采样质量级别
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;								// 纹理布局
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;					// ?
		/*
		// 关于 depthStencilDesc.Format 的说明
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
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),		// 堆属性（默认堆只有GPU能访问，上传堆、回读堆、客户堆则不行）
			D3D12_HEAP_FLAG_NONE,									// 堆额外标志
			&depthStencilDesc,										// 资源描述符指针
			D3D12_RESOURCE_STATE_COMMON,							// 资源状态
			&optClear,												// 清除资源的优化值
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

	// 视图和剪切矩形（根据实际情况）
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

		//为了实现高级效果，有时会采用多个剪切矩形
	}


	// 创建工厂
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
	// 检测用户设备对4X MSAA质量级别的支持情况
	void Dx12Sample::Check4xMsaaQuality()
	{
		//检测用户设备对4X MSAA质量级别的支持情况。
		//在一切支持Direct3D 11的设备上所有的渲染目标格式都已经支持4X MSAA了，
		//这里只进行质量级别的检测
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
	// 记录各种描述符的大小，需要时可直接使用
	void Dx12Sample::RecordDescriptorSize()
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


#pragma endregion


#pragma region InitializeAssets() 调用

	// 构建根签名
	void Dx12Sample::BuildRootSignature()
	{
		// 重载
	}

	// 构建着色器（顶点着色器、像素着色器）和输入布局
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

	// 图形管线状态（参数：根签名、输入布局、各种着色资源）
	void Dx12Sample::BuildPipelineStateObject()
	{
		/* Example:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { };
		psoDesc.pRootSignature = m_RootSignature.Get();								// 根签名
		psoDesc.InputLayout = InputLayout();										// 输入布局
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_VSByteCode.Get());					// vertexShader		(顶点着色器)
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_PSByteCode.Get());					// pixelShader		(像素着色器)
		psoDesc.DS = CD3DX12_SHADER_BYTECODE(m_DSByteCode.Get());					// domainShader		(域着色器)
		psoDesc.HS = CD3DX12_SHADER_BYTECODE(m_HSByteCode.Get());					// hullShader		(壳着色器)
		psoDesc.GS = CD3DX12_SHADER_BYTECODE(m_GSByteCode.Get());					// geometryShader	(几何着色器)
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);						// 混合状态
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);			// 栅格化状态
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);		// 深度模板状态
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;		// ？
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// 图元拓扑类型
		psoDesc.NumRenderTargets = 1;												// 渲染目标数量
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;							// 渲染目标格式n
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;									// 深度模板格式
		psoDesc.SampleMask = UINT_MAX;												// 采样掩模
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };								// 采样描述

		psoDesc.StreamOutput = {};													// 流输出
		psoDesc.NodeMask = 0;														// 节点掩模
		psoDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE();							// PSO Cached
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;								// 标记(?)

		// Create PSO
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso)));

		return;
		*/
	}

	// 构造网格数据
	void Dx12Sample::BuildMeshData()
	{
		/*
		//ComPtr<ID3D12Resource> m_VertexBuffer;
		//D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		// 1.1 顶点数据
		// 1.2 顶点缓冲区
		// 1.3 顶点缓冲区视图

		//ComPtr<ID3D12Resource> m_IndexBuffer;
		//D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		// 2.1 索引数据
		// 2.2 索引缓冲区
		// 2.3 索引缓冲区视图
		*/
	}

	// 常量缓冲区和视图
	void Dx12Sample::BuildConstantBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_ConstantBuffer;
		//ComPtr<ID3D12Resource> m_ConstantBufferUpload;
	}


	// 纹理缓冲区和视图
	void Dx12Sample::BuildTextureBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_Texture;
		//ComPtr<ID3D12Resource> m_TextureUpload;
	}

#pragma endregion


#pragma region OnRender() 调用

	void Dx12Sample::ReccorCommand()
	{
		/* Example:

		// 清除渲染背景
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_CommandList->ClearRenderTargetView(RtvHandleCPU(), clearColor, 0, nullptr);

		// 输入装配
		m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		m_CommandList->IASetIndexBuffer(&m_IndexBufferView);


		// 绘制
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

		。。。

		*/
	}

#pragma endregion







}//DX12
