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
*	virtual void OnResize();
*	virtual void OnUpdate();
*
*	// 鼠标族
*	virtual void OnMouseDown(WPARAM wParam, int x, int y);
*	...
*
*	virtual void BuildRootSignature();			// InitializeAssets()调用,重载
*	virtual void BuildShadersAndInputLayout();	// InitializeAssets()调用,重载
*	virtual void BuildPipelineStateObject();	// InitializeAssets()调用,重载
*	virtual void BuildMeshData();				// InitializeAssets()调用,重载
*	virtual void BuildConstantBufferAndView();	// InitializeAssets()调用,重载
*	virtual void BuildTextureBufferAndView();	// InitializeAssets()调用,重载
*
*	virtual void ReccorCommand();				// PopulateCommandList()调用,重载
*
******************************************************************************/

#pragma once

namespace DX12
{
	class DX12CORE_EXPORTS_CLASS Dx12Sample : public KSample
	{
	protected:
		Dx12Sample();
		Dx12Sample(const Dx12Sample&) = delete;

	public:
		~Dx12Sample();

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

	protected:
		virtual void OnMouseDown(WPARAM wParam, int x, int y) override {}
		virtual void OnMouseUp(WPARAM wParam, int x, int y) override {}
		virtual void OnMouseMove(WPARAM wParam, int x, int y) override {}
		virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam) override {}

#pragma region --OnInit()

	protected:
		virtual void InitializeGraphics();				// OnInit()调用
		virtual void InitializeAssets();				// OnInit()调用

		virtual void CreateDevice();					// InitializeGraphics()调用
		virtual void CreateDescriptorHeap();			// InitializeGraphics()调用
		virtual void CreateRootSignature();				// InitializeGraphics()调用
		virtual void CreateCommandObjects();			// InitializeGraphics()调用
		virtual void CreateSwapChain();					// InitializeGraphics()调用
		virtual void CreateSynchObjects();				// InitializeGraphics()调用
		virtual void CreateRenderTargetsResources();	// InitializeGraphics()调用
		virtual void CreateDepthStencilResources();		// InitializeGraphics()调用
		virtual void CreateViewportAndScissorRect();	// InitializeGraphics()调用

	protected:
		virtual void BuildRootSignature();				// InitializeAssets()调用,重载
		virtual void BuildShadersAndInputLayout();		// InitializeAssets()调用,重载
		virtual void BuildPipelineStateObject();		// InitializeAssets()调用,重载

		virtual void BuildMeshData();					// InitializeAssets()调用,重载
		virtual void BuildConstantBufferAndView();		// InitializeAssets()调用,重载
		virtual void BuildTextureBufferAndView();		// InitializeAssets()调用,重载

		virtual void ReccorCommand();					// PopulateCommandList()调用,重载

#pragma endregion

	protected:
		virtual void PopulateCommandList();				// OnRender() 调用
		virtual void MoveToNextFrame();					// OnRender() 调用
		virtual void WaitForGpu();						// 

	protected:
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

		D3D12_INPUT_LAYOUT_DESC InputLayout()
		{
			return D3D12_INPUT_LAYOUT_DESC
			{
				(D3D12_INPUT_ELEMENT_DESC*)m_InputLayout.data(),
				(UINT)m_InputLayout.size()
			};
		}


	private:
		void CreateFactory();				// 创建工厂
		void Check4xMsaaQuality();			// 检测用户设备对4X MSAA质量级别的支持情况
		void RecordDescriptorSize();		// 记录描述符大小

	protected:
		// 帧数量(交换缓冲区数量)
		static const UINT FrameCount = 2;


		//WARP（Windows Advanced Rasterization Platform）
		//提供了一种以软件方式代替显卡硬件方式完成渲染（用CPU模拟GPU功能）
		bool m_useWarpDevice;

		// 4X MSAA
		BOOL m_4xMsaaState;
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

	protected:
		// Factory
		ComPtr<IDXGIFactory4> m_Factory;

		// Device
		ComPtr<ID3D12Device> m_device;

		// Descriptor heap - 描述符堆
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;	// 
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;	//
		ComPtr<ID3D12DescriptorHeap> m_cbvHeap;	//
		ComPtr<ID3D12DescriptorHeap> m_srvHeap;	// SHADER_RESOURCE_VIEW
		ComPtr<ID3D12DescriptorHeap> m_uavHeap;	//

		// Root Signature - 根签名
		ComPtr<ID3D12RootSignature> m_RootSignature;

		// Commnd objects - 命令对象
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator[FrameCount];

		// Swap Chain - 交换链
		ComPtr<IDXGISwapChain3> m_SwapChain;


		// Synchronization objects - 同步对象
		ComPtr<ID3D12Fence> m_Fence;		// 围栏
		HANDLE m_FenceEvent;				// 围栏事件
		UINT64 m_FenceValues[FrameCount];	// 围栏值

		// Resources
		ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
		ComPtr<ID3D12Resource> m_DepthStencil;

		// 着色器字节码
		ComPtr<ID3DBlob> m_xxByteCode;
		ComPtr<ID3DBlob> m_VSByteCode;	// Vertex Shader	()
		ComPtr<ID3DBlob> m_PSByteCode;	// Pixel Shader		()
		ComPtr<ID3DBlob> m_DSByteCode;	// Domain Shader	(域着色器)
		ComPtr<ID3DBlob> m_HSByteCode;	// Hull Shader		(壳着色器)
		ComPtr<ID3DBlob> m_GSByteCode;	// Geometry Shader	(几何着色器)
		ComPtr<ID3DBlob> m_CSByteCode;	// Computer Shader	(计算着色器)
		ComPtr<ID3DBlob> m_zzByteCode;

		// 输入布局描述
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		// Pipeline State object - pso
		ComPtr<ID3D12PipelineState> m_pso;

		// Viewport
		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;


		// other - 下面变量不需要（？）
		ComPtr<ID3D12Resource> m_Texture;
		ComPtr<ID3D12Resource> m_TextureUpload;

		ComPtr<ID3D12Resource> m_ConstantBuffer;
		ComPtr<ID3D12Resource> m_ConstantBufferUpload;

		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

		ComPtr<ID3D12Resource> m_IndexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;


	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandleCPU;
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandleCPU;


		//@EndOf(Dx12Sample)
	};

}//DX12


