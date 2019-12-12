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
/******************************************************************************
*
* Usage:
*
* [01]创建类 Ademo, 基类为 Dx12Sample
* [02]修改构造函数为参数构造函数，因为基类 Dx12Sample 默认构造函数被删除
* [03]参数构造函数要运行基类的参数构造函数
* [04]重载以下函数
*
*	void BuildAssets() override;
*	void Resized() override;
*	void Update() override;
*	void Draw() override;
*	void DoAppIdle() override;
*
* [05]必要时重载以下函数
*
*	void OnInit() override;
*	void OnResize() override;
*	void OnUpdate() override;
*	void OnRender() override;
*
* [06]必要时重载以下函数 - 鼠标消息处理
*
*	void OnMouseDown(WPARAM wParam, int x, int y) override;
*	void OnMouseUp(WPARAM wParam, int x, int y) override;
*	void OnMouseMove(WPARAM wParam, int x, int y) override;
*
******************************************************************************/

#pragma once


class DX12LIBCOMMONCORE_CLASS  Dx12Book : public KSample
{
public:
	Dx12Book();
	~Dx12Book();

	LPVOID GetDevice3D() override
	{
		return m_device.Get();
	}

protected:
	// 后台缓冲区数量 - BackBufferCount
	static const int BackBufferCount = 2;
	int m_BackBufferIndex = 0;



#pragma region --鼠标消息处理

protected:
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;
	void OnMouseWheel(WPARAM wParam, LPARAM lParam) override {}

#pragma endregion

#pragma region --渲染管线流程

private:
	// 1.Initialize
	void OnInitialize() override;

	// 2.Resize
	void OnResize() override;

	// 3.Update
	void OnUpdate() override;

	// 4.Render
	void OnRender() override;

	// 5.Destory
	void OnDestroy() override;

	// 6.AppIdle() 
	void OnAppIdle() override;

#pragma endregion

#pragma region --初始化之 - CreateGraphics()

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

#pragma endregion

protected:
	// 重载下面4个函数
	virtual void BuildAssets();
	virtual void Resized();
	virtual void Update();
	virtual void Draw();
	virtual void DoAppIdle();


protected:
	void WaitForGpu();

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

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const
	{
		return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_INPUT_LAYOUT_DESC InputLayout()
	{
		return D3D12_INPUT_LAYOUT_DESC{ m_InputLayout.data(),
			(UINT)m_InputLayout.size() };
	}

private:
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	void CalculateFrameStats();

protected:
	//WARP（Windows Advanced Rasterization Platform）

	//提供了一种以软件方式代替显卡硬件方式完成渲染（用CPU模拟GPU功能）
	bool m_useWarpDevice;

	// 4X MSAA enabled
	bool m_4xMsaaState;

	// 4X MSAA quality level
	UINT m_4xMsaaQuality;

	UINT m_rtvDescriptorSize;
	UINT m_dsvDescriptorSize;
	UINT m_cbvDescriptorSize;
	UINT m_srvDescriptorSize;
	UINT m_uavDescriptorSize;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

	ComPtr<ID3D12Device> m_device;

	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<IDXGISwapChain3> m_SwapChain;

	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_FenceEvent;
	UINT64 m_FenceValue;

	ComPtr<ID3D12Resource> m_RenderTargets[BackBufferCount];
	ComPtr<ID3D12Resource> m_DepthStencil;

	// Viewport
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	// Derived class should set these in derived constructor to customize starting values.
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	XMFLOAT4X4 m_World = DXUtils::Identity4x4;
	XMFLOAT4X4 m_View = DXUtils::Identity4x4;
	XMFLOAT4X4 m_Proj = DXUtils::Identity4x4;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	std::unordered_map<_astring, ComPtr<ID3DBlob>> m_Shaders;
	std::unordered_map<_astring, ComPtr<ID3D12PipelineState>> m_PSOs;


	// other
	float m_Theta = XM_PI * 3 / 2;
	float m_Phi = XM_PI / 4;
	float m_Radius = 30.0f;

	POINT m_LastMousePos;


private:
	// 工厂
	ComPtr<IDXGIFactory4> m_Factory;

	//@EndOf(Dx12Book)
};


