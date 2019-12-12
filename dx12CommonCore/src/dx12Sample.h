/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
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
		// ��ʼ��
		void Initialize() override;
		// ������С
		void Resizing() override;

	protected:
		// ���غ�������
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
		// ��̨���������� - BackBufferCount
		static const int BackBufferCount = 2;
		int m_BackBufferIndex;

		// ID3D12Device ���ɺ�洢�����������Ĵ�С��Ҫʱ��ֱ��ʹ��
		UINT m_rtvDescriptorSize;
		UINT m_dsvDescriptorSize;
		UINT m_cbvDescriptorSize;
		UINT m_srvDescriptorSize;
		UINT m_uavDescriptorSize;

		// �豸
		ComPtr<IDXGIFactory4> m_Factory;
		ComPtr<ID3D12Device> m_device;

		// �������
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		// ������
		ComPtr<IDXGISwapChain3> m_SwapChain;

		// ͬ������
		ComPtr<ID3D12Fence> m_Fence;
		HANDLE m_FenceEvent;
		UINT64 m_FenceValue;

		// ��������
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

		// ֡��Դ
		ComPtr<ID3D12Resource> m_RenderTargets[BackBufferCount];

		// ���ģ��
		ComPtr<ID3D12Resource> m_DepthStencil;

		// Viewport - �ӿںͼ��о���
		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;

		// ���벼��
		//std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		// todo: �����ڴ�й© - dx12Sample
		//std::unordered_map<_astring, ComPtr<ID3DBlob>> m_Shaders;
		//std::unordered_map<_astring, ComPtr<ID3D12PipelineState>> m_PSOs;

	protected:
		void WaitForGpu();

		// ��ǰ��̨������
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

		// ���ģ����ͼ
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const
		{
			return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		}

		// ���벼��
		//D3D12_INPUT_LAYOUT_DESC InputLayout()
		//{
		//	return D3D12_INPUT_LAYOUT_DESC{ m_InputLayout.data(),
		//		(UINT)m_InputLayout.size() };
		//}


	private:


		//@EndOf(dx12Sample)
	};

}
