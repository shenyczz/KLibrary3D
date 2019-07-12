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



#pragma region 工作流程： Init -> Render -> Destroy


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
		// 更新参数数据
		UpdateParameter();
	}

	void Dx12Sample::OnRender()
	{
		// 记录命令
		PopulateCommandList();

		// 执行命令列表
		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// 呈现帧
		ThrowIfFailed(m_SwapChain->Present(1, 0));

		// 下一帧
		MoveToNextFrame();

		return;
	}

	void Dx12Sample::OnDestroy()
	{
		WaitForGpu();				// 确保GPU不再引用即将清理的资源
		CloseHandle(m_fenceEvent);	// 关闭围栏事件
		return;
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

		CreateRootSignature();		// 需根据实际情况重载

		CreateCommandObjects();
		CreateSwapChain();

		CreateSynchObjects();

		CreateFrameResources();
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

	// 窗口大小改变
	void Dx12Sample::WindowResized()
	{
		/// PI = 3.1415926F
		/// OPI = PI; QPI = PI/4; HPI = PI/2
		/// 若用户调整了窗口尺寸，则

		// 1.更新纵横比
		m_fAspectRatio = (m_pWindow) ? m_pWindow->AaspectRatio() : 1.0f;

		// 2.重新计算投影矩阵;
		XMMATRIX prj = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_fAspectRatio, 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_Proj, prj);

		return;
	}

	// UpdateParameter
	void Dx12Sample::UpdateParameter()
	{

	}

	// PopulateCommandList, 处理命令列表
	void Dx12Sample::PopulateCommandList()
	{
		// Reset 之前要先 Close()
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

	// 根签名（根据实际情况重载）
	void Dx12Sample::CreateRootSignature()
	{
		// 这里创建一个空的根签名(Flags 修改为 D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)
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

		//  Command List（PSO）
		{
			ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_CommandAllocator[m_frameIndex].Get(),	// 此时 m_frameIndex = 0
				m_PipelineState.Get(),
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
		int width = m_pWindow->Width();
		int height = m_pWindow->Height();

		// 创建交换链
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
				m_CommandQueue.Get(),	// 交换链需要队列
				&swapChainDesc,
				swapChain.GetAddressOf()));

			ThrowIfFailed(swapChain.As(&m_SwapChain));
		}

		// 取得当前背景缓冲区索引
		m_frameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// 视口和剪切矩形
		// This sample does not support fullscreen transitions.
		ThrowIfFailed(m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

		return;
	}

	// 同步对象（需要命令队列）
	void Dx12Sample::CreateSynchObjects()
	{
		// 为 CPU、GPU 同步创建围栏
		ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_frameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_Fence)));

		// 围栏值
		m_fenceValues[m_frameIndex]++;

		// 创建帧同步事件
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

	// 帧资源
	void Dx12Sample::CreateFrameResources()
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
		int width = m_pWindow->Width();
		int height = m_pWindow->Height();
		//
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

	// 视图和剪切矩形（根据实际情况）
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

		///为了实现高级效果，有时会采用多个剪切矩形
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

	// 构建着色器字节码（顶点着色器、像素着色器）
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

	// 根据顶点结构建造输入元素描述
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

	// 图形管线状态（参数：根签名、输入布局、各种着色资源）
	void Dx12Sample::BuildPipelineStateObject()
	{
		/* Example:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { NULL };
		psoDesc.pRootSignature = m_RootSignature.Get();								// 根签名
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_VSByteCode.Get());					// vertexShader		(顶点着色器)
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_PSByteCode.Get());					// pixelShader		(像素着色器)
		psoDesc.DS = CD3DX12_SHADER_BYTECODE(m_DSByteCode.Get());					// domainShader		(域着色器)
		psoDesc.HS = CD3DX12_SHADER_BYTECODE(m_HSByteCode.Get());					// hullShader		(壳着色器)
		psoDesc.GS = CD3DX12_SHADER_BYTECODE(m_GSByteCode.Get());					// geometryShader	(几何着色器)
		psoDesc.StreamOutput = {};
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);						// 混合状态
		psoDesc.SampleMask = UINT_MAX;												// 采样掩模
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);			// 栅格化状态
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);		// 深度模板状态
		psoDesc.InputLayout = nputLayout();											// 输入布局
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;		// ？
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;		// 图元拓扑类型
		psoDesc.NumRenderTargets = 1;												// 渲染目标数量
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;							// 渲染目标格式n
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;									// 深度模板格式
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };								// 采样描述
		psoDesc.NodeMask = 0;														// 节点掩模
		psoDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE();							// PSO Cached
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;								// 标记(?)

		// Create PSO
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

		return;
		*/
	}

	// 顶点缓冲区和视图
	void Dx12Sample::BuildVertexBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_VertexBuffer;
		//D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	}

	// 索引缓冲区和视图
	void Dx12Sample::BuildIndexBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_IndexBuffer;
		//D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	}

	// 纹理缓冲区和视图
	void Dx12Sample::BuildTextureBufferAndView()
	{
		//ComPtr<ID3D12Resource> m_TextureBuffer;
		//ComPtr<ID3D12Resource> m_TextureBufferUpload;
	}

	// 常量缓冲区和视图
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
