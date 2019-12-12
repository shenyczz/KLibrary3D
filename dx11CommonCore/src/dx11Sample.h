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
******************************************************************************/
#pragma once

class DX11LIBCOMMONCORE_CLASS dx11Sample : public dxCommon::d3dSample
{
public:
	dx11Sample();
	~dx11Sample();

	LPVOID GetDevice3D() override
	{
		return m_device;
	}

private:
	// 初始化
	void Initialize() override;
	// 调整大小
	void Resizing() override;

protected:
	// 重载函数样例
	virtual void BuildAssets() = 0;
	void Resized() override {}
	void Update() override {}
	void Draw() override {}
	void Shutdown() override {}
	void DoAppIdle() override { d3dSample::DoAppIdle(); }

private:
	void CreateGraphics();

	void CreateDevice();
	void CreateSwapChain();


	void Check4xMsaaQuality();


protected:
	ID3D11Device* m_device;
	IDXGISwapChain* m_SwapChain;
	ID3D11DeviceContext* m_d3dImmediateContext;

	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;

	ID3D11Texture2D* m_DepthStencilBuffer;

	D3D11_VIEWPORT m_ScreenViewport;


	//@EndOf(Dx11Sample)
};

