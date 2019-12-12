#include "stdafx.h"
#include "ex01_Box.h"


ex01_Box::ex01_Box()
{
	this->EnableFrameStats();
}

ex01_Box::~ex01_Box()
{
}




//�������ʲ���
void ex01_Box::BuildAssets()
{
	BuildRootSignature();

	/*
	BuildShaders();
	BuildInputLayout();
	BuildPipelineState();

	BuildConstantBufferAndView();

	BuildGeometry();

	*/
}
void ex01_Box::BuildRootSignature()
{
	// ��ɫ������һ����Ҫ����Դ��Ϊ���루���糣����������������������
	// ��ǩ����������ɫ������ľ�����Դ
	// �������ɫ��������һ�������������������Դ�����������ݵĲ������ݣ�
	// ��ô�������Ϊ��ǩ������Ķ��Ǻ���ǩ��

	// 1.��ǩ���������Ϊ������������
	// 2.��������������ʽ��������������������������

	DescriptorRange r;

	return;
}


void ex01_Box::Resized()
{
	// ����ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}


void ex01_Box::Update() {}


void ex01_Box::Draw()
{
	// For Example:
	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();
	ID3D12CommandQueue* pCommandQueue = m_CommandQueue.Get();
	IDXGISwapChain* pSwapChain = m_SwapChain.Get();

	//m_IsWireframe = true;
	ID3D12PipelineState* pCurrentPso = nullptr;

	// ��λ���������������䱣��������¼
	ThrowIfFailed(pCommandAllocator->Reset());

	// ��λ�����б���ʼ��¼����
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// դ�񻯽׶�
		pCommandList->RSSetViewports(1, &m_ScreenViewport);
		pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		// ����ϲ���RTV + DSV
		pCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// �����Ⱦ����
		const float* clearColor = Colors::Black;
		pCommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// ������ģ����ͼ
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		pCommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// ���ø�ǩ��
		//pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// ��������
		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
		//pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// ���õ�b0
		//pCommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

		// ����...
		//DrawRenderItems();
	}

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// �ر������б�ֹͣ��¼����
	ThrowIfFailed(pCommandList->Close());

	// �������б��е����������������
	ID3D12CommandList* cmdLists[] = { pCommandList };
	pCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// ����ǰ��̨������
	ThrowIfFailed(pSwapChain->Present(0, 0));

	// ͬ��
	WaitForGpu();

	return;
}
