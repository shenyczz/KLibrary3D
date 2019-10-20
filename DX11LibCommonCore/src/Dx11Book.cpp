#include "stdafx.h"
#include "Dx11Book.h"


Dx11Book::Dx11Book()
{
	m_4xMsaaEnable = false;
	m_4xMsaaQuality = 0;
	m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE,

	m_d3dDevice = nullptr;
	m_d3dImmediateContext = nullptr;
	m_SwapChain = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;

	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));

}

Dx11Book::~Dx11Book()
{
	ReleaseCom(m_DepthStencilView);
	ReleaseCom(m_RenderTargetView);
	ReleaseCom(m_DepthStencilBuffer);
	ReleaseCom(m_SwapChain);

	// Restore all default settings.
	if (m_d3dImmediateContext)
		m_d3dImmediateContext->ClearState();

	ReleaseCom(m_d3dImmediateContext);
	ReleaseCom(m_d3dDevice);
}


void Dx11Book::OnInit()
{
	CreateGraphics();
}

void Dx11Book::OnApplicationIdle()
{
	OnUpdate();
	OnRender();
	//DoAppIdle();
}





void Dx11Book::CreateGraphics()
{
	// 1.Create the device and device context.
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		m_d3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&m_d3dDevice,
		&featureLevel,
		&m_d3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, _T("D3D11CreateDevice Failed."), 0, 0);
		return;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, _T("Direct3D Feature Level 11 unsupported."), 0, 0);
		return;
	}


	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	Check4xMsaaQuality();

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	KWindow* pWindow = KApplication::GetApp()->GetWindow();

	DXGI_SWAP_CHAIN_DESC sd;
	{
		sd.BufferDesc.Width = pWindow->ClientWidth();
		sd.BufferDesc.Height = pWindow->ClientHeight();
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		if (m_4xMsaaEnable)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = pWindow->Handle();
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;
	}

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = 0;
	(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice, &sd, &m_SwapChain));


	ReleaseCom(dxgiDevice);
	ReleaseCom(dxgiAdapter);
	ReleaseCom(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	OnResize();

	return;
}

void Dx11Book::Check4xMsaaQuality()
{
	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality > 0);
}

void Dx11Book::OnResize()
{
	assert(m_d3dImmediateContext);
	assert(m_d3dDevice);
	assert(m_SwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCom(m_RenderTargetView);
	ReleaseCom(m_DepthStencilView);
	ReleaseCom(m_DepthStencilBuffer);

	KWindow* pWindow = KApplication::GetApp()->GetWindow();
	UINT iClientWidth = pWindow->ClientWidth();
	UINT iClientHeight = pWindow->ClientHeight();

	// Resize the swap chain and recreate the render target view.
	ThrowIfFailed(m_SwapChain->ResizeBuffers(1,
		iClientWidth,
		iClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	ID3D11Texture2D* backBuffer;
	ThrowIfFailed(m_SwapChain->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer)));

	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView));
	ReleaseCom(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	{

		depthStencilDesc.Width = iClientWidth;
		depthStencilDesc.Height = iClientHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		if (m_4xMsaaEnable)
		{
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;
	}

	ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
	ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView));

	// Bind the render target view and depth/stencil view to the pipeline.
	m_d3dImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	// Set the viewport transform.
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(iClientWidth);
	m_ScreenViewport.Height = static_cast<float>(iClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;


	// RS
	m_d3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);

	return;
}

void Dx11Book::OnRender()
{
	assert(m_d3dImmediateContext);
	assert(m_SwapChain);

	const float* clearColor = Colors::Blue;
	//const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_d3dImmediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(clearColor));
	m_d3dImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ThrowIfFailed(m_SwapChain->Present(0, 0));
}


/*
void D3DApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (mTimer.TotalTime() - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			 << L"FPS: " << fps << L"    " 
			 << L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
*/