#include "stdafx.h"
#include "ex02_Cube.h"


ex02_Cube::ex02_Cube()
{
}


ex02_Cube::~ex02_Cube()
{
}


void ex02_Cube::BuildAssets()
{
	//BuildShadersAndInputLayout();
	//BuildGeometry0();
	//BuildGeometry1();
}
void ex02_Cube::BuildShadersAndInputLayout()
{
	ComPtr<ID3DBlob> blob;

	_tstring path = KApplication::GetApp()->Startup();
	_tstring vsfile = _T("Cube_VS.cso");
	_tstring psfile = _T("Cube_PS.cso");

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
void ex02_Cube::BuildGeometry0()
{
	// ******************
	// ���������嶥��
	//    5________ 6
	//    /|      /|
	//   /_|_____/ |
	//  1|4|_ _ 2|_|7
	//   | /     | /
	//   |/______|/
	//  0       3
	std::vector<Vertex> vertices =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
	};

	// ******************
	// ��������
	std::vector<uint32> indices =
	{
		// ����
		0, 1, 2,
		2, 3, 0,
		// ����
		4, 5, 1,
		1, 0, 4,
		// ����
		1, 5, 6,
		6, 2, 1,
		// ����
		7, 6, 5,
		5, 4, 7,
		// ����
		3, 2, 6,
		6, 7, 3,
		// ����
		4, 0, 3,
		3, 7, 4
	};

	mesh0 = MeshData(vertices, indices);

	MeshData* pMeshData = this->GetMeshData();




	// ���㻺��������
	D3D11_BUFFER_DESC vbd = {};
	{
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		//vbd.ByteWidth = sizeof vertices;
		//vbd.ByteWidth = vertices.size() * sizeof(Vertex);
		vbd.ByteWidth = pMeshData->vbSizeInBytes();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
	}

	// ���㻺����
	D3D11_SUBRESOURCE_DATA InitData = {};
	{
		//InitData.pSysMem = vertices.data();
		InitData.pSysMem = pMeshData->pvbInitData();
	}
	HR(m_device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// ������������������
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//ibd.ByteWidth = sizeof indices;
	ibd.ByteWidth = sizeof(uint32)*indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	// �½�����������
	InitData.pSysMem = indices.data();
	HR(m_device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// ����װ��׶ε���������������
	//m_d3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	m_d3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// ******************
	// ���ó�������������
	//
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// �½���������������ʹ�ó�ʼ����
	HR(m_device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.GetAddressOf()));

	// ��ʼ��������������ֵ
	m_CBuffer.world = XMMatrixIdentity();	// ��λ�����ת����������
	m_CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));
	m_CBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));


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
	// �����ºõĳ����������󶨵�������ɫ��
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

	m_d3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}

typedef Vertex VertexPosColor;
void ex02_Cube::BuildGeometry1()
{
	// ******************
	// ���������嶥��
	//    5________ 6
	//    /|      /|
	//   /_|_____/ |
	//  1|4|_ _ 2|_|7
	//   | /     | /
	//   |/______|/
	//  0       3
	VertexPosColor vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
	};
	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof vertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(m_device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// ******************
	// ��������
	//
	WORD indices[] = {
		// ����
		0, 1, 2,
		2, 3, 0,
		// ����
		4, 5, 1,
		1, 0, 4,
		// ����
		1, 5, 6,
		6, 2, 1,
		// ����
		7, 6, 5,
		5, 4, 7,
		// ����
		3, 2, 6,
		6, 7, 3,
		// ����
		4, 0, 3,
		3, 7, 4
	};
	// ������������������
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof indices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	// �½�����������
	InitData.pSysMem = indices;
	HR(m_device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// ����װ��׶ε���������������
	m_d3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);


	// ******************
	// ���ó�������������
	//
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// �½���������������ʹ�ó�ʼ����
	HR(m_device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.GetAddressOf()));

	// ��ʼ��������������ֵ
	m_CBuffer.world = XMMatrixIdentity();	// ��λ�����ת����������
	m_CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));
	m_CBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));


	// ******************
	// ����Ⱦ���߸����׶ΰ󶨺�������Դ
	//

	// ����װ��׶εĶ��㻺��������
	UINT stride = sizeof(VertexPosColor);	// ��Խ�ֽ���
	UINT offset = 0;						// ��ʼƫ����

	m_d3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// ����ͼԪ���ͣ��趨���벼��
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// ����ɫ���󶨵���Ⱦ����
	m_d3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	// �����ºõĳ����������󶨵�������ɫ��
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

	m_d3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}


void ex02_Cube::Resized()
{
	// ����ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

void ex02_Cube::Update()
{
	//UpdateCB();
}

void ex02_Cube::UpdateCB()
{
	static float theta = 0.0f, phi = 0.0f;
	theta += 0.0005f;
	phi += 0.0006f;

	m_CBuffer.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta));

	// ���³�������������������ת����
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_d3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	m_d3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);
}



void ex02_Cube::Draw()
{
	// For Example: 
	assert(m_d3dImmediateContext);
	assert(m_SwapChain);

	const float* clearColor = Colors::Blue;
	m_d3dImmediateContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(clearColor));
	m_d3dImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ����������
	//m_d3dImmediateContext->DrawIndexed(36, 0, 0);
	ThrowIfFailed(m_SwapChain->Present(0, 0));
}

