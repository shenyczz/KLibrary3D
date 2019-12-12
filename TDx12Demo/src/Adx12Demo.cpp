#include "stdafx.h"
#include "Adx12Demo.h"


Adx12Demo::Adx12Demo()
{
	this->EnableFrameStats();
}

Adx12Demo::~Adx12Demo()
{
}





void Adx12Demo::BuildAssets() {}
void Adx12Demo::Resized()
{
	// 更行投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}
void Adx12Demo::Update() {}
void Adx12Demo::Draw()
{
	// For Example:
	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();
	ID3D12CommandQueue* pCommandQueue = m_CommandQueue.Get();
	IDXGISwapChain* pSwapChain = m_SwapChain.Get();

	//m_IsWireframe = true;
	ID3D12PipelineState* pCurrentPso = nullptr;

	// 复位命令分配器，清除其保存的命令记录
	ThrowIfFailed(pCommandAllocator->Reset());

	// 复位命令列表，开始记录命令
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// 栅格化阶段
		pCommandList->RSSetViewports(1, &m_ScreenViewport);
		pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出合并：RTV + DSV
		pCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// 清除渲染背景
		const float* clearColor = Colors::Black;
		pCommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		pCommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// 设置根签名
		//pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 描述符堆
		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
		//pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// 设置到b0
		//pCommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

		// 绘制...
		//DrawRenderItems();
	}

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 关闭命令列表，停止记录命令
	ThrowIfFailed(pCommandList->Close());

	// 把命令列表中的命令输入命令队列
	ID3D12CommandList* cmdLists[] = { pCommandList };
	pCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// 交换前后台缓冲区
	ThrowIfFailed(pSwapChain->Present(0, 0));

	// 同步
	WaitForGpu();

	return;
}
