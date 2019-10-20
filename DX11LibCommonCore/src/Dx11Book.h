/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: …Í”¿≥Ω.÷£÷› (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

class DX11LIBCOMMONCORE_CLASS Dx11Book : public KSample
{
public:
	Dx11Book();
	~Dx11Book();

protected:
	void OnMouseDown(WPARAM wParam, int x, int y) override {}
	void OnMouseUp(WPARAM wParam, int x, int y) override {}
	void OnMouseMove(WPARAM wParam, int x, int y) override {}
	void OnMouseWheel(WPARAM wParam, LPARAM lParam) override {}

protected:
	// 1.Initialize
	void OnInit() override;

	// 2.Resize
	void OnResize() override;

	// 3.Update
	void OnUpdate() override {}

	// 4.Render
	void OnRender() override;

	// 5.Destory
	void OnDestroy() override {}

	// 6.ApplicationIdle
	void OnApplicationIdle() override;

private:
	void CreateGraphics();

	void Check4xMsaaQuality();

protected:
	// 4X MSAA enabled
	bool m_4xMsaaEnable;
	UINT m_4xMsaaQuality;


	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dImmediateContext;
	IDXGISwapChain* m_SwapChain;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;

	ID3D11Texture2D* m_DepthStencilBuffer;

	D3D11_VIEWPORT m_ScreenViewport;
	D3D_DRIVER_TYPE m_d3dDriverType;



	//@EndOf(Dx11Book)
};

