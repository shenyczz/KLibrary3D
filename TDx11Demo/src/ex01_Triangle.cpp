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

	// ���������ζ���
	Vertex vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }
	};

	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(m_device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	// ******************
	// ����Ⱦ���߸����׶ΰ󶨺�������Դ
	//

	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(Vertex);			// ��Խ�ֽ���
	UINT offset = 0;						// ��ʼƫ����

	m_d3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// ����ͼԪ���ͣ��趨���벼��
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// ����ɫ���󶨵���Ⱦ����
	m_d3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_d3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);


	// ******************
	// ���õ��Զ�����
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

	// vs ֮ InputLayout
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
	// ����ͶӰ����
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
