#include "stdafx.h"
#include "dx11Sample.h"


dx11Sample::dx11Sample()
{
	m_device = nullptr;
	m_d3dImmediateContext = nullptr;
	m_SwapChain = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;

	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
}

dx11Sample::~dx11Sample()
{
	ReleaseCom(m_DepthStencilView);
	ReleaseCom(m_RenderTargetView);
	ReleaseCom(m_DepthStencilBuffer);
	ReleaseCom(m_SwapChain);

	// Restore all default settings.
	if (m_d3dImmediateContext)
		m_d3dImmediateContext->ClearState();

	ReleaseCom(m_d3dImmediateContext);
	ReleaseCom(m_device);
}



#pragma region --Initialize

void dx11Sample::Initialize()
{
	CreateGraphics();
	BuildAssets();
}


void dx11Sample::CreateGraphics()
{
	CreateDevice();
	CreateSwapChain();
}

// 设备
void dx11Sample::CreateDevice()
{
	// 1.Create the device and device context.
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	//m_d3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;
	ThrowIfFailed(D3D11CreateDevice(
		0,                 // default adapter
		m_d3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&m_device,
		&featureLevel,
		&m_d3dImmediateContext));

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, _T("Direct3D Feature Level 11 unsupported."), 0, 0);
		return;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	Check4xMsaaQuality();

	return;
}

// 交换链
void dx11Sample::CreateSwapChain()
{
	HWND hWnd = m_pWindow->Handle();
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	{
		swapChainDesc.BufferDesc.Width = iClientWidth;
		swapChainDesc.BufferDesc.Height = iClientHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = m_BackBufferFormat;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = m_4xMsaaEnable ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = m_4xMsaaEnable ? m_4xMsaaQuality - 1 : 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;	// dx12 - DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	}

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = 0;
	(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory4* dxgiFactory = 0;
	(dxgiAdapter->GetParent(__uuidof(IDXGIFactory4), (void**)&dxgiFactory));

	ThrowIfFailed(dxgiFactory->CreateSwapChain(m_device, &swapChainDesc, &m_SwapChain));

	//CreateDXGIFactory2()

	ReleaseCom(dxgiDevice);
	ReleaseCom(dxgiAdapter);
	ReleaseCom(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.
}

// 检测用户设备对4X MSAA质量级别的支持情况

void dx11Sample::Check4xMsaaQuality()
{
	m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality > 0);
}

#pragma endregion


#pragma region --Resizing

void dx11Sample::Resizing()
{
	assert(m_d3dImmediateContext);
	assert(m_device);
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

	ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView));
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

	ThrowIfFailed(m_device->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
	ThrowIfFailed(m_device->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView));

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

#pragma endregion


