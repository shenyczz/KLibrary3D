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
* Usage:
*
* [01]创建类 Ademo, 基类为 Dx12Sample
* [02]修改构造函数为参数构造函数，因为基类 Dx12Sample 默认构造函数被删除
* [03]参数构造函数要运行基类的参数构造函数
* [04]重载以下函数
*
*	virtual void Resize();						// OnResize() 调用,重载
*	virtual void Update();						// OnUpdate() 调用,重载
*
*	virtual void CreateRootSignature();			// InitializeGraphics()调用,重载
*
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

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace DX12;

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


#pragma region --OnInit()

	protected:
		virtual void InitializeGraphics();				// OnInit()调用
		virtual void InitializeAssets();				// OnInit()调用

		virtual void CreateDevice();					// InitializeGraphics()调用
		virtual void CreateDescriptorHeap();			// InitializeGraphics()调用
		virtual void CreateRootSignature();				// InitializeGraphics()调用,重载
		virtual void CreateCommandObjects();			// InitializeGraphics()调用
		virtual void CreateSwapChain();					// InitializeGraphics()调用
		virtual void CreateSynchObjects();				// InitializeGraphics()调用
		virtual void CreateRenderTargetsResources();	// InitializeGraphics()调用
		virtual void CreateDepthStencilResources();		// InitializeGraphics()调用
		virtual void CreateViewportAndScissorRect();	// InitializeGraphics()调用

		virtual void BuildShadersAndInputLayout();		// InitializeAssets()调用,重载
		virtual void BuildPipelineStateObject();		// InitializeAssets()调用,重载

		virtual void BuildMeshData();					// InitializeAssets()调用,重载
		virtual void BuildConstantBufferAndView();		// InitializeAssets()调用,重载
		virtual void BuildTextureBufferAndView();		// InitializeAssets()调用,重载

		virtual void ReccorCommand();				// PopulateCommandList()调用,重载

#pragma endregion

	protected:
		virtual void PopulateCommandList();				// OnRender() 调用,重载

		virtual void Resize();							// OnResize() 调用,重载
		virtual void Update();							// OnUpdate() 调用,重载
		virtual void MoveToNextFrame();					// OnRender() 调用
		virtual void WaitForGpu();						// OnInit、OnDestroy()、CreateSynchObjects(？) 调用


	protected:
		// 取得硬件适配器
		void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

	private:
		void CreateFactory();			// 创建工厂
		void Check4xMsaaQuality();		// 检测用户设备对4X MSAA质量级别的支持情况
		void RecordDescriptorSize();	// 记录描述符大小

	protected:
		// 帧数量(交换缓冲区数量)
		static const UINT FrameCount = 2;

		// Factory
		ComPtr<IDXGIFactory4> m_Factory;

		//WARP（Windows Advanced Rasterization Platform）
		//提供了一种以软件方式代替显卡硬件方式完成渲染（用CPU模拟GPU功能）
		bool m_useWarpDevice;

		// 记录当前后台缓冲区索引 [0,FrameCount-1]
		UINT m_frameIndex;

		// 4X MSAA
		BOOL m_4xMsaaState;
		UINT m_4xMsaaQuality;

		// Descriptor size
		UINT m_rtvDescriptorSize;
		UINT m_dsvDescriptorSize;
		UINT m_cbvDescriptorSize;
		UINT m_srvDescriptorSize;
		UINT m_uavDescriptorSize;

		// DXGI_FORMAT
		DXGI_FORMAT m_BackBufferFormat;
		DXGI_FORMAT m_DepthStencilFormat;

		// 变换矩阵
		XMFLOAT4X4 m_World;
		XMFLOAT4X4 m_View;
		XMFLOAT4X4 m_Proj;

	protected:
		// Device
		ComPtr<ID3D12Device> m_device;

		// Descriptor heap
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
		ComPtr<ID3D12DescriptorHeap> m_srvHeap;	// SHADER_RESOURCE_VIEW
		ComPtr<ID3D12DescriptorHeap> m_uavHeap;

		// RootSignature
		ComPtr<ID3D12RootSignature> m_RootSignature;

		// Commnd objects
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator[FrameCount];
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		// Swap Chain
		ComPtr<IDXGISwapChain3> m_SwapChain;


		// Synchronization objects.
		ComPtr<ID3D12Fence> m_Fence;		/// 围栏
		HANDLE m_fenceEvent;				/// 围栏事件
		UINT64 m_fenceValues[FrameCount];

		// Resources1 (View) m_device->Create_xxx_View(...)
		ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];


		ComPtr<ID3D12Resource> m_DepthStencil;

		ComPtr<ID3D12Resource> m_Texture;
		ComPtr<ID3D12Resource> m_TextureUpload;

		ComPtr<ID3D12Resource> m_ConstantBuffer;
		ComPtr<ID3D12Resource> m_ConstantBufferUpload;

		// Resources2 (View)
		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

		ComPtr<ID3D12Resource> m_IndexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

		// 着色器字节码
		ComPtr<ID3DBlob> m_VSByteCode;
		ComPtr<ID3DBlob> m_PSByteCode;


		// 输入布局描述
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDescs;
		D3D12_INPUT_LAYOUT_DESC InputLayout()
		{
			return D3D12_INPUT_LAYOUT_DESC
			{
				(D3D12_INPUT_ELEMENT_DESC*)m_InputElementDescs.data(),
				(UINT)m_InputElementDescs.size()
			};
		}

		// Pipeline State object
		ComPtr<ID3D12PipelineState> m_PipelineState;


		// Viewport
		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;

		//@EndOf(Dx12Sample)
	};

}//DX12


