/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

namespace dx12
{

	class DX12LIBCOMMONCORE_CLASS dx12Sample : public dxCommon::d3dSample
	{
	public:
		dx12Sample();
		~dx12Sample();

		LPVOID GetDevice3D() override
		{
			return m_device.Get();
		}

	private:
		// 初始化
		void Initialize() override;
		// 调整大小
		void Resizing() override;

	protected:
		// 重载函数样例
		virtual void BuildAssets() = 0;
		void Resized() override {}
		void Update() override {}
		void Draw() override {}
		void Shutdown() override;
		void DoAppIdle() override { d3dSample::DoAppIdle(); }


	private:
		void CreateGraphics();

		void CreateDevice();
		void CreateCommandObjects();
		void CreateSwapChain();

		void CreateSynchObjects();
		void CreateDescriptorHeaps();

		void CreateRenderTargets();
		void CreateDepthStencilAndView();

		void CreateViewportAndScissorRect();

		void CreateFactory();
		void Check4xMsaaQuality();
		void StoreDescriptorSize();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	protected:
		// 后台缓冲区数量 - BackBufferCount
		static const int BackBufferCount = 2;
		int m_BackBufferIndex;

		// ID3D12Device 生成后存储各种描述符的大小需要时可直接使用
		UINT m_rtvDescriptorSize;
		UINT m_dsvDescriptorSize;
		UINT m_cbvDescriptorSize;
		UINT m_srvDescriptorSize;
		UINT m_uavDescriptorSize;

		// 设备
		ComPtr<IDXGIFactory4> m_Factory;
		ComPtr<ID3D12Device> m_device;

		// 命令对象
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		// 交换链
		ComPtr<IDXGISwapChain3> m_SwapChain;

		// 同步对象
		ComPtr<ID3D12Fence> m_Fence;
		HANDLE m_FenceEvent;
		UINT64 m_FenceValue;

		// 描述符堆
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

		// 帧资源
		ComPtr<ID3D12Resource> m_RenderTargets[BackBufferCount];

		// 深度模板
		ComPtr<ID3D12Resource> m_DepthStencil;

		// Viewport - 视口和剪切矩形
		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;

		// 输入布局
		//std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		// todo: 会有内存泄漏 - dx12Sample
		//std::unordered_map<_astring, ComPtr<ID3DBlob>> m_Shaders;
		//std::unordered_map<_astring, ComPtr<ID3D12PipelineState>> m_PSOs;

	protected:
		void WaitForGpu();

		// 当前后台缓冲区
		ID3D12Resource* CurrentBackBuffer() const
		{
			return m_RenderTargets[m_BackBufferIndex].Get();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(
				m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
				m_BackBufferIndex,
				m_rtvDescriptorSize);
		}

		// 深度模板视图
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const
		{
			return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		}

		// 输入布局
		//D3D12_INPUT_LAYOUT_DESC InputLayout()
		//{
		//	return D3D12_INPUT_LAYOUT_DESC{ m_InputLayout.data(),
		//		(UINT)m_InputLayout.size() };
		//}


	private:


		//@EndOf(dx12Sample)
	};

}
