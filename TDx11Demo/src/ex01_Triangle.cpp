#include "stdafx.h"
#include "ex01_Triangle.h"

using namespace dxCommon;

ex01_Triangle::ex01_Triangle()
{
	this->EnableFrameStats();
}


ex01_Triangle::~ex01_Triangle()
{
}


void ex01_Triangle::BuildAssets()
{
	BuildShadersAndInputLayout();

	// 设置三角形顶点
	Vertex vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }
	};

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(m_device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(Vertex);			// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	m_d3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_d3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_d3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	// ******************
	// 设置调试对象名
	//
	//D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
	//D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	//D3D11SetDebugObjectName(m_pVertexShader.Get(), "Trangle_VS");
	//D3D11SetDebugObjectName(m_pPixelShader.Get(), "Trangle_PS");


	return;
}
void ex01_Triangle::BuildShadersAndInputLayout()
{
	ComPtr<ID3DBlob> blob;

	_tstring path = KApplication::GetApp()->Startup();
	_tstring vsfile = _T("Triangle_VS.cso");
	_tstring psfile = _T("Triangle_PS.cso");

	_tstring vsfilePath = path + _T("Assets\\") + vsfile;
	_tstring psfilePath = path + _T("Assets\\") + psfile;

	// VS
	_wstring wvsfilePath = KString::WString(vsfilePath.c_str());
	ThrowIfFailed(D3DReadFileToBlob(wvsfilePath.c_str(), blob.ReleaseAndGetAddressOf()));
	ThrowIfFailed(m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		m_pVertexShader.GetAddressOf()));

	// vs 之 InputLayout
	HR(m_device->CreateInputLayout(Vertex11::InputLayout.data(), Vertex11::InputLayout.size(),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));


	// PS
	_wstring wpsfilePath = KString::WString(psfilePath.c_str());
	ThrowIfFailed(D3DReadFileToBlob(wpsfilePath.c_str(), blob.ReleaseAndGetAddressOf()));
	ThrowIfFailed(m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		m_pPixelShader.GetAddressOf()));

	return;
}




void ex01_Triangle::Resized()
{
	// 更行投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}
void ex01_Triangle::Update() {}
void ex01_Triangle::Draw()
{
	// For Example:
	assert(m_d3dImmediateContext);
	assert(m_SwapChain);

	if (m_RenderTargetView)
	{
		const float* clearColor = Colors::Black;
		m_d3dImmediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(clearColor));
		m_d3dImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_d3dImmediateContext->Draw(3, 0);
	}


	ThrowIfFailed(m_SwapChain->Present(0, 0));

	return;
}
