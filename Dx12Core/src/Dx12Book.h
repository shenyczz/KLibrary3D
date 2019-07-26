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
/******************************************************************************
*
* Usage:
*
* [01]������ Ademo, ����Ϊ Dx12Sample
* [02]�޸Ĺ��캯��Ϊ�������캯������Ϊ���� Dx12Sample Ĭ�Ϲ��캯����ɾ��
* [03]�������캯��Ҫ���л���Ĳ������캯��
* [04]�������º���
*
*	void OnResize() override;
*	void OnUpdate() override;
*
*	// �����Ϣ����
*	void OnMouseDown(WPARAM wParam, int x, int y) override;
*	void OnMouseUp(WPARAM wParam, int x, int y) override;
*	void OnMouseMove(WPARAM wParam, int x, int y) override;
*
*	// BuildAssets()����
*	void BuildRootSignature() override;
*	void BuildShadersAndInputLayout() override;
*	void BuildPipelineStateObject() override;
*	void BuildMeshData() override;
*	void BuildConstantBufferAndView() override;
*	void BuildTextureBufferAndView() override;
*
*	// OnRender()����
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
		// ֡����
		static const UINT FrameCount = 2;


#pragma region --��Ⱦ��������

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


#pragma region --�����Ϣ����

	protected:
		virtual void OnMouseDown(WPARAM wParam, int x, int y) override;
		virtual void OnMouseUp(WPARAM wParam, int x, int y) override;
		virtual void OnMouseMove(WPARAM wParam, int x, int y) override;
		virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam) override {}

#pragma endregion


#pragma region --��ʼ��֮ - CreateGraphics()

	protected:
		virtual void CreateGraphics();					// OnInit()����

		virtual void CreateDevice();					// InitializeGraphics()����
		virtual void CreateDescriptorHeap();			// InitializeGraphics()����
		virtual void CreateRootSignature();				// InitializeGraphics()����
		virtual void CreateCommandObjects();			// InitializeGraphics()����
		virtual void CreateSwapChain();					// InitializeGraphics()����
		virtual void CreateSynchObjects();				// InitializeGraphics()����
		virtual void CreateRenderTargetsResources();	// InitializeGraphics()����
		virtual void CreateDepthStencilResources();		// InitializeGraphics()����
		virtual void CreateViewportAndScissorRect();	// InitializeGraphics()����

#pragma endregion

#pragma region --��ʼ��֮ - BuildAssets()

	protected:
		virtual void BuildAssets();						// OnInit()����

		virtual void BuildRootSignature();				// InitializeAssets()����,����
		virtual void BuildShadersAndInputLayout();		// InitializeAssets()����,����
		virtual void BuildPipelineStateObject();		// InitializeAssets()����,����

		virtual void BuildMeshData();					// InitializeAssets()����,����
		virtual void BuildConstantBufferAndView();		// InitializeAssets()����,����
		virtual void BuildTextureBufferAndView();		// InitializeAssets()����,����

#pragma endregion


#pragma region --��Ⱦ֮ - ��¼����

	protected:
		virtual void ReccorCommand();					// OnRender()����,����

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
		void CreateFactory();			// ��������
		void Check4xMsaaQuality();		// ����û��豸��4X MSAA���������֧�����
		void RecordDescriptorSize();	// ��¼��������С

		void MoveToNextFrame();			// OnRender() ����
		void WaitForGpu();				// 

		UINT GetCompileFlags();			// �����־


#pragma endregion



#pragma region --Protected Variables

	protected:

		//WARP��Windows Advanced Rasterization Platform��

		//�ṩ��һ���������ʽ�����Կ�Ӳ����ʽ�����Ⱦ����CPUģ��GPU���ܣ�
		bool m_useWarpDevice;

		// 4X MSAA
		bool m_4xMsaaState;
		UINT m_4xMsaaQuality;

		// ֡���� [0,FrameCount-1]
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

		// �任����
		XMFLOAT4X4 m_World;
		XMFLOAT4X4 m_View;
		XMFLOAT4X4 m_Proj;

		// ���
		float m_Theta;		// ��X��н�
		float m_Phi;		// ��Y��н�
		float m_Radius;		// ��뾶

		POINT m_LastMousePos;

	protected:
		// Device - �豸
		ComPtr<ID3D12Device> m_device;

		// Descriptor heap - ��������
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;	// 
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;	//
		ComPtr<ID3D12DescriptorHeap> m_cbvHeap;	// CBV_SRV_UAV
		ComPtr<ID3D12DescriptorHeap> m_srvHeap;	// SHADER_RESOURCE_VIEW
		ComPtr<ID3D12DescriptorHeap> m_uavHeap;

		// Root Signature - ��ǩ��
		ComPtr<ID3D12RootSignature> m_RootSignature;

		// Swap Chain - ������
		ComPtr<IDXGISwapChain3> m_SwapChain;

		// Synchronization objects - ͬ������
		ComPtr<ID3D12Fence> m_Fence;		// Χ��
		HANDLE m_FenceEvent;				// Χ���¼�
		UINT64 m_FenceValues[FrameCount];	// Χ��ֵ

		// Commnd objects - �������
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator[FrameCount];

		// Resources
		ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
		ComPtr<ID3D12Resource> m_DepthStencil;

		// ���벼������
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		// Pipeline State object - pso
		ComPtr<ID3D12PipelineState> m_psoCurrent;

		// ��ɫ���ֽ���
		//ComPtr<ID3DBlob> m_xxByteCode;
		//ComPtr<ID3DBlob> m_VSByteCode;	// Vertex Shader	()
		//ComPtr<ID3DBlob> m_PSByteCode;	// Pixel Shader		()
		//ComPtr<ID3DBlob> m_DSByteCode;	// Domain Shader	(����ɫ��)
		//ComPtr<ID3DBlob> m_HSByteCode;	// Hull Shader		(����ɫ��)
		//ComPtr<ID3DBlob> m_GSByteCode;	// Geometry Shader	(������ɫ��)
		//ComPtr<ID3DBlob> m_CSByteCode;	// Computer Shader	(������ɫ��)
		//ComPtr<ID3DBlob> m_zzByteCode;
		// VS��PS��DS��HS��GS��CS
		std::unordered_map<_astring, ComPtr<ID3DBlob>> m_Shaders;

		// Viewport
		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;


#pragma endregion



#pragma region --Private Variables

		ComPtr<IDXGIFactory4> m_Factory;	// ����

		D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandleCPU;	// RenderTargetView()
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandleCPU;	// DepthStencilView()

#pragma endregion




		//@EndOf(Dx12Book)
	};


}


