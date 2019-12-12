#include "stdafx.h"
#include "ex01_Box.h"


ex01_Box::ex01_Box()
{
	this->EnableFrameStats();
}

ex01_Box::~ex01_Box()
{
}




//【构造资产】
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
	// 着色器程序一般需要以资源作为输入（例如常量缓冲区、纹理、采样器）
	// 根签名则定义了着色器程序的具体资源
	// 如果把着色器程序看作一个函数，而将输入的资源当作向函数传递的参数数据，
	// 那么便可以认为根签名定义的都是函数签名

	// 1.根签名可以理解为根参数的数组
	// 2.根参数有三种形式：描述符表、根描述符、根常量

	DescriptorRange r;

	return;
}


void ex01_Box::Resized()
{
	// 更行投影矩阵
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
