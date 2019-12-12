#include "stdafx.h"
#include "Abook12Demo.h"


Abook12Demo::Abook12Demo()
{
	m_IsWireframe = false;

	m_Theta = XM_PI * 3 / 2;
	m_Phi = XM_PI / 4;
	m_Radius = 5.0f;
}

Abook12Demo::~Abook12Demo()
{
}

//【鼠标消息】
void Abook12Demo::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	Dx12Book::OnMouseDown(wParam, x, y);
}
void Abook12Demo::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book::OnMouseUp(wParam, x, y);
}
void Abook12Demo::OnMouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta -= dx;
		m_Phi -= dy;

		// Restrict the angle mPhi.
		m_Phi = KMaths::Clamp(m_Phi, 0.1f, (float)KMaths::PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = KMaths::Clamp(m_Radius, 5.0f, 150.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}


//【构造资产】
void Abook12Demo::BuildAssets()
{
	//BuildRootSignature();
	//BuildShadersAndInputLayout();
	//BuildPipelineState();
	//BuildBoxGeometry();
	//BuildConstantBufferAndView();

	return;
}

//【窗口大小改变】
void Abook12Demo::Resized()
{
	// 更行投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

//【更新数据】
void Abook12Demo::Update()
{
	//UpdateCamera();
	//UpdateObjectCBs();
}

// 【渲染绘制】
void Abook12Demo::Draw()
{
	// For Example:

	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();

	ID3D12PipelineState* pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

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
		const float* clearColor = Colors::LightSkyBlue;
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

		// 输入装配
		//MeshGeometry* pGeo = m_Geometries["boxGeo"].get();
		//UINT indexCount = pGeo->DrawArgs["box"].IndexCount;

		//if (pGeo != nullptr && indexCount > 0)
		//{
		//	pCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//	pCommandList->IASetVertexBuffers(0, 1, &pGeo->VertexBufferView());
		//	pCommandList->IASetIndexBuffer(&pGeo->IndexBufferView());

		//	pCommandList->DrawIndexedInstanced(
		//		indexCount,
		//		1,
		//		0, 0, 0);
		//}

	}

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 关闭命令列表，停止记录命令
	ThrowIfFailed(pCommandList->Close());

	// 呈现 - presentation

	// 把命令列表中的命令输入命令队列
	ID3D12CommandList* cmdsLists[] = { pCommandList };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 交换前后台缓冲区
	ThrowIfFailed(m_SwapChain->Present(0, 0));

	// 
	WaitForGpu();
}

//【辅助函数】


