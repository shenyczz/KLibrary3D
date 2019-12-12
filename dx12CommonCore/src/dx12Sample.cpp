#include "stdafx.h"
#include "dx12Sample.h"

namespace dx12
{

	dx12Sample::dx12Sample()
	{
		m_BackBufferIndex = 0;

		m_rtvDescriptorSize = 0;
		m_dsvDescriptorSize = 0;
		m_cbvDescriptorSize = 0;
		m_srvDescriptorSize = 0;
		m_uavDescriptorSize = 0;

		m_ScreenViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, 1.0f, 1.0f);
		m_ScissorRect = D3D12_RECT{ 0, 0, 1, 1 };

		m_FenceEvent = nullptr;
	}

	dx12Sample::~dx12Sample()
	{
	}



#pragma region --Initialize

	void dx12Sample::Initialize()
	{
		CreateGraphics();

		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
		{
			BuildAssets();	// 纯虚函数
		}
		ThrowIfFailed(m_CommandList->Close());

		ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		WaitForGpu();
	}


	void dx12Sample::CreateGraphics()
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

		CreateViewportAndScissorRect();

		Check4xMsaaQuality();
		StoreDescriptorSize();

		// 开始处于封闭状态；当我们第一次引用命令列表时，我们将会复位它
		m_CommandList->Close();
	}

	// 设备
	void dx12Sample::CreateDevice()
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

	// 命令对象
	void dx12Sample::CreateCommandObjects()
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

	// 交换链
	void dx12Sample::CreateSwapChain()
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
			swapChainDesc.SampleDesc.Count = m_4xMsaaEnable ? 4 : 1;
			swapChainDesc.SampleDesc.Quality = m_4xMsaaEnable ? (m_4xMsaaQuality - 1) : 0;
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

	// 同步对象
	void dx12Sample::CreateSynchObjects()
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

	// 描述符堆
	void dx12Sample::CreateDescriptorHeaps()
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

	// 帧资源
	void dx12Sample::CreateRenderTargets()
	{
		// 取得描述符堆中的描述符句柄
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT i = 0; i < BackBufferCount; i++)
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

	// 深度模板
	void dx12Sample::CreateDepthStencilAndView()
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
			depthStencilDesc.SampleDesc.Count = m_4xMsaaEnable ? 4 : 1;							// 对每个像素的采样次数
			depthStencilDesc.SampleDesc.Quality = m_4xMsaaEnable ? (m_4xMsaaQuality - 1) : 0;	// 多重采样质量级别
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

	// 视口和剪切矩形（根据实际情况）
	void dx12Sample::CreateViewportAndScissorRect()
	{
		m_ScreenViewport = CD3DX12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>(m_pWindow->ClientWidth()),
			static_cast<float>(m_pWindow->ClientHeight()));

		m_ScissorRect = CD3DX12_RECT(
			1, 1,
			static_cast<long>(m_pWindow->ClientWidth()),
			static_cast<long>(m_pWindow->ClientHeight()));

		//m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		//为了实现高级效果，有时会采用多个剪切矩形
	}

	// 创建工厂
	void dx12Sample::CreateFactory()
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
	// 检测用户设备对4X MSAA质量级别的支持情况
	void dx12Sample::Check4xMsaaQuality()
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
		assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

		return;
	}
	// 存储各种描述符的大小，需要时可直接使用
	void dx12Sample::StoreDescriptorSize()
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

	void dx12Sample::LogAdapters()
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
	void dx12Sample::LogAdapterOutputs(IDXGIAdapter* adapter)
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
	void dx12Sample::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
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

#pragma endregion


#pragma region --Resizing

	void dx12Sample::Resizing()
	{
		assert(m_device);
		assert(m_SwapChain);
		assert(m_CommandAllocator);

		WaitForGpu();

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
	}

#pragma endregion


#pragma region --Shutdown

	// 关闭
	void dx12Sample::Shutdown()
	{
		WaitForGpu();
		if (m_FenceEvent)
		{
			CloseHandle(m_FenceEvent);
		}
	}

#pragma endregion


#pragma region --WaitForGpu

	void dx12Sample::WaitForGpu()
	{
		// 向命令队列中添加指令以设置新的围栏点.
		// 因为我们在GPU时间线上，在GPU完成前面的所有命令前，无法设置新的围栏点
		const UINT64 fenceValue = m_FenceValue;
		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValue));
		m_FenceValue++;

		// 等待GPU完成到这个围栏点的命令
		if (m_Fence->GetCompletedValue() < fenceValue)
		{
			HANDLE hEvent = m_FenceEvent;

			// 当GPU到达当前围栏点点燃事件
			ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, hEvent));
			WaitForSingleObject(hEvent, INFINITE);
		}

		m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		return;
	}

#pragma endregion


}
