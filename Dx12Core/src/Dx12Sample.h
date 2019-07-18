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
* Usage:
*
* [01]������ Ademo, ����Ϊ Dx12Sample
* [02]�޸Ĺ��캯��Ϊ�������캯������Ϊ���� Dx12Sample Ĭ�Ϲ��캯����ɾ��
* [03]�������캯��Ҫ���л���Ĳ������캯��
* [04]�������º���
*
*	virtual void Resize();						// OnResize() ����,����
*	virtual void Update();						// OnUpdate() ����,����
*
*	virtual void CreateRootSignature();			// InitializeGraphics()����,����
*
*	virtual void BuildShadersAndInputLayout();	// InitializeAssets()����,����
*	virtual void BuildPipelineStateObject();	// InitializeAssets()����,����
*	virtual void BuildMeshData();				// InitializeAssets()����,����
*	virtual void BuildConstantBufferAndView();	// InitializeAssets()����,����
*	virtual void BuildTextureBufferAndView();	// InitializeAssets()����,����
*
*	virtual void ReccorCommand();				// PopulateCommandList()����,����
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
		virtual void InitializeGraphics();				// OnInit()����
		virtual void InitializeAssets();				// OnInit()����

		virtual void CreateDevice();					// InitializeGraphics()����
		virtual void CreateDescriptorHeap();			// InitializeGraphics()����
		virtual void CreateRootSignature();				// InitializeGraphics()����,����
		virtual void CreateCommandObjects();			// InitializeGraphics()����
		virtual void CreateSwapChain();					// InitializeGraphics()����
		virtual void CreateSynchObjects();				// InitializeGraphics()����
		virtual void CreateRenderTargetsResources();	// InitializeGraphics()����
		virtual void CreateDepthStencilResources();		// InitializeGraphics()����
		virtual void CreateViewportAndScissorRect();	// InitializeGraphics()����

		virtual void BuildShadersAndInputLayout();		// InitializeAssets()����,����
		virtual void BuildPipelineStateObject();		// InitializeAssets()����,����

		virtual void BuildMeshData();					// InitializeAssets()����,����
		virtual void BuildConstantBufferAndView();		// InitializeAssets()����,����
		virtual void BuildTextureBufferAndView();		// InitializeAssets()����,����

		virtual void ReccorCommand();				// PopulateCommandList()����,����

#pragma endregion

	protected:
		virtual void PopulateCommandList();				// OnRender() ����,����

		virtual void Resize();							// OnResize() ����,����
		virtual void Update();							// OnUpdate() ����,����
		virtual void MoveToNextFrame();					// OnRender() ����
		virtual void WaitForGpu();						// OnInit��OnDestroy()��CreateSynchObjects(��) ����


	protected:
		// ȡ��Ӳ��������
		void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

	private:
		void CreateFactory();			// ��������
		void Check4xMsaaQuality();		// ����û��豸��4X MSAA���������֧�����
		void RecordDescriptorSize();	// ��¼��������С

	protected:
		// ֡����(��������������)
		static const UINT FrameCount = 2;

		// Factory
		ComPtr<IDXGIFactory4> m_Factory;

		//WARP��Windows Advanced Rasterization Platform��
		//�ṩ��һ���������ʽ�����Կ�Ӳ����ʽ�����Ⱦ����CPUģ��GPU���ܣ�
		bool m_useWarpDevice;

		// ��¼��ǰ��̨���������� [0,FrameCount-1]
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

		// �任����
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
		ComPtr<ID3D12Fence> m_Fence;		/// Χ��
		HANDLE m_fenceEvent;				/// Χ���¼�
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

		// ��ɫ���ֽ���
		ComPtr<ID3DBlob> m_VSByteCode;
		ComPtr<ID3DBlob> m_PSByteCode;


		// ���벼������
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


