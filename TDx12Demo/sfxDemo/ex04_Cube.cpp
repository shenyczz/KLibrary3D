#include "stdafx.h"
#include "ex04_Cube.h"


Cube::Cube()
{
}

Cube::~Cube()
{
}

void Cube::InitializeGraphics()
{
	Dx12Sample::InitializeGraphics();
}

void Cube::InitializeAssets()
{
	Dx12Sample::InitializeAssets();
}

void Cube::WindowResized()
{
	Dx12Sample::WindowResized();
}

void Cube::UpdateParameter()
{
	Dx12Sample::UpdateParameter();
}

void Cube::PopulateCommandList()
{
	Dx12Sample::PopulateCommandList();
}



void Cube::CreateRootSignature()
{
	Dx12Sample::CreateRootSignature();
	return;

	// ��ɫ������һ����Ҫ����Դ��Ϊ���루�����������������������ȣ�
	// ��ǩ����������ɫ�������������Դ
	// �������ɫ����������һ������������������Դ�����������ݵĲ�����
	// ��ô�������Ϊ��ǩ��������Ǻ���ǩ��

	// ��ǩ������������(�汾)
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// �������������������ꡢ���������������

	// �����ɵ���CBV��ɵ���������
	CD3DX12_DESCRIPTOR_RANGE rangs[1];
	rangs[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameters[1];
	slotRootParameters[0].InitAsDescriptorTable(1, &rangs[0]);

	
	// ��ǩ������һ�����������ɵĶ���
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(slotRootParameters), &slotRootParameters[0],
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);	// ��������װ������벼��

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed
	(
		D3D12SerializeRootSignature(&rootSignatureDesc,
		featureData.HighestVersion,
		signature.GetAddressOf(),
		error.GetAddressOf())
	);

	ThrowIfFailed
	(
		m_device->CreateRootSignature(0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_RootSignature))
	);

	return;
}


void Cube::BuildByteCode()
{
	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\color.hlsl");
	_tstring filePath = path + file;

	m_VSByteCode = DXUtil::CompileShader(filePath, nullptr, "VS", "vs_5_0");
	m_PSByteCode = DXUtil::CompileShader(filePath, nullptr, "PS", "ps_5_0");

	return;
}

void Cube::BuildInputElementDescrips()
{

	//typedef struct D3D12_INPUT_ELEMENT_DESC
	//{
	//	LPCSTR SemanticName;
	//	UINT SemanticIndex;
	//	DXGI_FORMAT Format;
	//	UINT InputSlot;
	//	UINT AlignedByteOffset;
	//	D3D12_INPUT_CLASSIFICATION InputSlotClass;
	//	UINT InstanceDataStepRate;
	//} 	D3D12_INPUT_ELEMENT_DESC;

	UINT offsetPos = 0;
	UINT offsetClr = sizeof(Vertex::Pos);	// Vertex::Pos
	m_InputElementDescs =
	{
		D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetPos, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetClr, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return;
}

void Cube::BuildPipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};


	return;
}


void Cube::BuildVertexBufferAndView()
{
	// ���֣�ԭ��������������
	std::array<Vertex, 8> vertices =
	{
		// ǰ���ı��� 0(���½�+˳ʱ��) -> 1 -> 2 -> 3
		Vertex{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) },
		Vertex{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) },
		Vertex{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
		Vertex{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) },

		// �������ı��� 4(���½�+˳ʱ��) -> 5 -> 6 -> 7
		Vertex{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) },
		Vertex{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
		Vertex{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) },
		Vertex{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }
	};

	// 8�����㣬6���棬12�������Σ�36����������
	std::array<std::uint16_t, 36> indices =
	{
		// front face��˳ʱ�룩
		0, 1, 2,
		0, 2, 3,

		// back face����ʱ�룩
		4, 6, 5,
		4, 7, 6,

		// left face����ʱ�룩
		4, 5, 1,
		4, 1, 0,

		// right face��˳ʱ�룩
		3, 2, 6,
		3, 6, 7,

		// top face��˳ʱ�룩
		1, 5, 6,
		1, 6, 2,

		// bottom face����ʱ�룩
		4, 0, 3,
		4, 3, 7
	};




	return;
}

void Cube::BuildIndexBufferAndView()
{
}

void Cube::BuildConstantBufferAndView()
{
	// UploadBuffer
	m_ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_device.Get(), 1, true);

	UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();

	// ƫ�Ƶ�������������i�������Ӧ�ĳ������ݣ�����ȡ i=0
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = objCBByteSize;
	m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	return;
}


