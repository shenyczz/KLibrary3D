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
*	void OnResize() override;
*	void OnUpdate() override;
*
*	// 鼠标消息处理
*	void OnMouseDown(WPARAM wParam, int x, int y) override;
*	void OnMouseUp(WPARAM wParam, int x, int y) override;
*	void OnMouseMove(WPARAM wParam, int x, int y) override;
*
*	// BuildAssets()调用
*	void BuildRootSignature() override;
*	void BuildShadersAndInputLayout() override;
*	void BuildPipelineStateObject() override;
*	void BuildMeshData() override;
*	void BuildConstantBufferAndView() override;
*	void BuildTextureBufferAndView() override;
*
*	// OnRender()调用
*	void ReccorCommand() override;
*
******************************************************************************/
#pragma once

using namespace std;


namespace DX12
{

	class DX12CORE_EXPORTS_CLASS Dx12Book : public KSample
	{
	public:
		Dx12Book();
		~Dx12Book();

	protected:
		// 帧数量
		static const UINT FrameCount = 2;


#pragma region --渲染管线流程

	protected:
		// 1.Initialize
		virtual void OnInit() override;

		// 2.Resize
		virtual void OnResize() override;

		// 3.Update
		virtual void OnUpdate() override;

		// 4.Render
		virtual void OnRender() override;

		// 5.Destory
		virtual void OnDestroy() override;

#pragma endregion


#pragma region --鼠标消息处理

	protected:
		virtual void OnMouseDown(WPARAM wParam, int x, int y) override;
		virtual void OnMouseUp(WPARAM wParam, int x, int y) override;
		virtual void OnMouseMove(WPARAM wParam, int x, int y) override;
		virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam) override {}

#pragma endregion


#pragma region --初始化之 - CreateGraphics()

	protected:
		virtual void CreateGraphics();					// OnInit()调用

		virtual void CreateDevice();					// InitializeGraphics()调用
		virtual void CreateDescriptorHeap();			// InitializeGraphics()调用
		virtual void CreateRootSignature();				// InitializeGraphics()调用
		virtual void CreateCommandObjects();			// InitializeGraphics()调用
		virtual void CreateSwapChain();					// InitializeGraphics()调用
		virtual void CreateSynchObjects();				// InitializeGraphics()调用
		virtual void CreateRenderTargetsResources();	// InitializeGraphics()调用
		virtual void CreateDepthStencilResources();		// InitializeGraphics()调用
		virtual void CreateViewportAndScissorRect();	// InitializeGraphics()调用

#pragma endregion

#pragma region --初始化之 - BuildAssets()

	protected:
		virtual void BuildAssets();						// OnInit()调用

		virtual void BuildRootSignature();				// InitializeAssets()调用,重载
		virtual void BuildShadersAndInputLayout();		// InitializeAssets()调用,重载
		virtual void BuildPipelineStateObject();		// InitializeAssets()调用,重载

		virtual void BuildMeshData();					// InitializeAssets()调用,重载
		virtual void BuildConstantBufferAndView();		// InitializeAssets()调用,重载
		virtual void BuildTextureBufferAndView();		// InitializeAssets()调用,重载

#pragma endregion


#pragma region --渲染之 - 记录命令

	protected:
		virtual void ReccorCommand();					// OnRender()调用,重载

#pragma endregion


#pragma region --Protected Functions

	protected:
		// 
		const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTargetView()
		{
			m_rtvHandleCPU = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
			m_rtvHandleCPU.ptr += m_rtvDescriptorSize * m_FrameIndex;
			return m_rtvHandleCPU;
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE& DepthStencilView()
		{
			m_dsvHandleCPU = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
			return m_dsvHandleCPU;
		}

		ID3D12CommandAllocator* CurrentCommandAllocator() const
		{
			return m_CommandAllocator[m_FrameIndex].Get();
		}

		ID3D12Resource* CurrentBackBuffer() const
		{
			return m_RenderTargets[m_FrameIndex].Get();
		}

		UINT64 CurrentFenceValue() const
		{
			return m_FenceValues[m_FrameIndex];
		}

		D3D12_INPUT_LAYOUT_DESC InputLayout()
		{
			return D3D12_INPUT_LAYOUT_DESC
			{
				(D3D12_INPUT_ELEMENT_DESC*)m_InputLayout.data(),
				(UINT)m_InputLayout.size()
			};
		}


#pragma endregion


#pragma region --Private Functions

	private:
		void CreateFactory();			// 创建工厂
		void Check4xMsaaQuality();		// 检测用户设备对4X MSAA质量级别的支持情况
		void RecordDescriptorSize();	// 记录描述符大小

		void MoveToNextFrame();			// OnRender() 调用
		void WaitForGpu();				// 

		UINT GetCompileFlags();			// 编译标志


#pragma endregion



#pragma region --Protected Variables

	protected:

		//WARP（Windows Advanced Rasterization Platform）

		//提供了一种以软件方式代替显卡硬件方式完成渲染（用CPU模拟GPU功能）
		bool m_useWarpDevice;

		// 4X MSAA
		bool m_4xMsaaState;
		UINT m_4xMsaaQuality;

		// 帧索引 [0,FrameCount-1]
		UINT m_FrameIndex;

		// Descriptor size
		UINT m_rtvDescriptorSize;
		UINT m_dsvDescriptorSize;
		UINT m_cbvDescriptorSize;
		UINT m_srvDescriptorSize;
		UINT m_uavDescriptorSize;

		// DXGI_FORMAT
		DXGI_FORMAT m_RenderTargetFormat;
		DXGI_FORMAT m_DepthStencilFormat;

		// 变换矩阵
		XMFLOAT4X4 m_World;
		XMFLOAT4X4 m_View;
		XMFLOAT4X4 m_Proj;

		// 鼠标
		float m_Theta;		// 与X轴夹角
		float m_Phi;		// 与Y轴夹角
		float m_Radius;		// 球半径

		POINT m_LastMousePos;

	protected:
		// Device - 设备
		ComPtr<ID3D12Device> m_device;

		// Descriptor heap - 描述符堆
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;	// 
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;	//
		ComPtr<ID3D12DescriptorHeap> m_cbvHeap;	// CBV_SRV_UAV
		ComPtr<ID3D12DescriptorHeap> m_srvHeap;	// SHADER_RESOURCE_VIEW
		ComPtr<ID3D12DescriptorHeap> m_uavHeap;

		// Root Signature - 根签名
		ComPtr<ID3D12RootSignature> m_RootSignature;

		// Swap Chain - 交换链
		ComPtr<IDXGISwapChain3> m_SwapChain;

		// Synchronization objects - 同步对象
		ComPtr<ID3D12Fence> m_Fence;		// 围栏
		HANDLE m_FenceEvent;				// 围栏事件
		UINT64 m_FenceValues[FrameCount];	// 围栏值

		// Commnd objects - 命令对象
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator[FrameCount];

		// Resources
		ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
		ComPtr<ID3D12Resource> m_DepthStencil;

		// 输入布局描述
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		// Pipeline State object - pso
		ComPtr<ID3D12PipelineState> m_psoCurrent;

		// 着色器字节码
		//ComPtr<ID3DBlob> m_xxByteCode;
		//ComPtr<ID3DBlob> m_VSByteCode;	// Vertex Shader	()
		//ComPtr<ID3DBlob> m_PSByteCode;	// Pixel Shader		()
		//ComPtr<ID3DBlob> m_DSByteCode;	// Domain Shader	(域着色器)
		//ComPtr<ID3DBlob> m_HSByteCode;	// Hull Shader		(壳着色器)
		//ComPtr<ID3DBlob> m_GSByteCode;	// Geometry Shader	(几何着色器)
		//ComPtr<ID3DBlob> m_CSByteCode;	// Computer Shader	(计算着色器)
		//ComPtr<ID3DBlob> m_zzByteCode;
		// VS、PS、DS、HS、GS、CS
		std::unordered_map<_astring, ComPtr<ID3DBlob>> m_Shaders;

		// Viewport
		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;


#pragma endregion



#pragma region --Private Variables

		ComPtr<IDXGIFactory4> m_Factory;	// 工厂

		D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandleCPU;	// RenderTargetView()
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandleCPU;	// DepthStencilView()

#pragma endregion




		//@EndOf(Dx12Book)
	};


}


