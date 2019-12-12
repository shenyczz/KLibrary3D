#include "stdafx.h"
#include "Abook11Demo.h"


Abook11Demo::Abook11Demo()
{
	this->EnableFrameStats();
}

Abook11Demo::~Abook11Demo()
{
}



void Abook11Demo::BuildAssets() {}

void Abook11Demo::Resized() {}

void Abook11Demo::Update() {}

void Abook11Demo::Draw()
{
	// this is example codes
	assert(m_d3dImmediateContext);
	assert(m_SwapChain);

	const float* clearColor = Colors::Blue;
	m_d3dImmediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(clearColor));
	m_d3dImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ThrowIfFailed(m_SwapChain->Present(0, 0));
}

void Abook11Demo::Shutdown() {}

