#include "stdafx.h"
#include "Adx11Demo.h"


Adx11Demo::Adx11Demo()
{
	this->EnableFrameStats();
}

Adx11Demo::~Adx11Demo()
{
}


void Adx11Demo::BuildAssets() {}
void Adx11Demo::Resized()
{
	// 更行投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}
void Adx11Demo::Update() {}
void Adx11Demo::Draw()
{
	// For Example:
	assert(m_d3dImmediateContext);
	assert(m_SwapChain);

	const float* clearColor = Colors::Blue;
	m_d3dImmediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(clearColor));
	m_d3dImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ThrowIfFailed(m_SwapChain->Present(0, 0));
}

