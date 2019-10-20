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

class DX12LIBCOMMONCORE_CLASS Dx12Study : public KSample
{
public:
	Dx12Study();
	~Dx12Study();

protected:
	// 后台缓冲区数量 - BackBufferCount
	static const int BackBufferCount = 2;
	int m_BackBufferIndex = 0;


#pragma region --鼠标消息处理

protected:
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;
	void OnMouseWheel(WPARAM wParam, LPARAM lParam) override;

#pragma endregion


#pragma region --渲染管线流程

protected:
	// 1.Initialize
	void OnInit() override;

	// 2.Resize
	void OnResize() override;

	// 3.Update
	void OnUpdate() override;

	// 4.Render
	void OnRender() override;

	// 5.Destory
	void OnDestroy() override;

	// 6.ApplicationIdle
	void OnApplicationIdle() override;

#pragma endregion


#pragma region --初始化之 - CreateGraphics()

private:
	void CreateGraphics();

	void CreateDevice();
	void CreateSynchObjects();
	void CreateDescriptorHeaps();

	void CreateCommandObjects();
	void CreateSwapChain();

	void CreateRenderTargets();
	void CreateDepthStencilAndView();

	void CreateViewportAndScissorRect();


	void CreateFactory();
	void StoreDescriptorSize();

#pragma endregion

protected:
	void WaitForGpu();

	ID3D12Resource* RenderTarget() const
	{
		return m_RenderTargets[m_BackBufferIndex].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_BackBufferIndex,
			m_rtvDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const
	{
		return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_INPUT_LAYOUT_DESC InputLayout()
	{
		return D3D12_INPUT_LAYOUT_DESC{ m_InputLayout.data(),
			(UINT)m_InputLayout.size() };
	}


protected:
	virtual void BuildAssets()= 0;
	virtual void Resized() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DoAppIdle()
	{
		CalculateFrameStats();
	}

	private:
		void CalculateFrameStats();

protected:
	//WARP（Windows Advanced Rasterization Platform）

	//提供了一种以软件方式代替显卡硬件方式完成渲染（用CPU模拟GPU功能）
	bool m_useWarpDevice;

	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvDescriptorSize = 0;
	UINT m_srvDescriptorSize = 0;
	UINT m_uavDescriptorSize = 0;

	ComPtr<ID3D12Device> m_device;

	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	ComPtr<IDXGISwapChain3> m_SwapChain;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_FenceEvent;
	UINT64 m_FenceValue;

	ComPtr<ID3D12Resource> m_RenderTargets[BackBufferCount];
	ComPtr<ID3D12Resource> m_DepthStencil;

	// Viewport
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 转换矩阵
	XMFLOAT4X4 m_World = DXUtil::Identity4x4;
	XMFLOAT4X4 m_View = DXUtil::Identity4x4;
	XMFLOAT4X4 m_Proj = DXUtil::Identity4x4;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	std::unordered_map<_astring, ComPtr<ID3DBlob>> m_Shaders;
	std::unordered_map<_astring, ComPtr<ID3D12PipelineState>> m_PSOs;


	// other
	float m_Theta = XM_PI * 3 / 2;
	float m_Phi = XM_PI / 4;
	float m_Radius = 5.0f;

	POINT m_LastMousePos;

private:
	ComPtr<IDXGIFactory4> m_Factory;	// 工厂

	//@EndOf(AstudyDemo)
};

