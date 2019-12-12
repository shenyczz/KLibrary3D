#include "stdafx.h"
#include "Box.h"


const int box = 0;
const int car = 1;
const int skull = 2;
const int shapes = 3;
const int radar = 4;


Box::Box()
{
	//m_Theta = 1.5f*XM_PI;
	m_Theta = 0;
	m_Phi = XM_PIDIV4;
	m_Radius = 5.0f;

	m_GeoFlag = box;
	//m_GeoFlag = car;
	//m_GeoFlag = skull;
	//m_GeoFlag = shapes;
	//m_GeoFlag = radar;
}

Box::~Box()
{
}



//�������Ϣ�� - Mouse
void Box::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	Dx12Book::OnMouseDown(wParam, x, y);
}
void Box::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book::OnMouseUp(wParam, x, y);
}
void Box::OnMouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta -= dx;
		m_Phi -= dy;

		// Restrict the angle mPhi.
		m_Phi = KMaths::Clamp(m_Phi, 0.1f, (float)KMaths::PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = KMaths::Clamp(m_Radius, 1.0f, 50.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}


//�������ʲ���
void Box::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();
	BuildConstantBufferAndView();

	BuildBoxGeometry();
	BuildCarGeometry();
	BuildSkullGeometry();
	BuildShapesGeometry();
	BuildBoxRadar();

	return;
}
void Box::BuildRootSignature()
{
	// ��ɫ������һ����Ҫ����Դ��Ϊ���루���糣����������������������
	// ��ǩ����������ɫ������ľ�����Դ
	// �������ɫ��������һ�������������������Դ�����������ݵĲ������ݣ�
	// ��ô�������Ϊ��ǩ������Ķ��Ǻ���ǩ��

	// �����������������������������������
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// ������1��CBV����ɵ���������ע�ᵽ0����� - b0
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	// ��������ʼ��Ϊ��������
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// ��ǩ��������
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		1, slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// ������ǩ��
	ComPtr<ID3DBlob> rootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rootSig.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_device->CreateRootSignature(
		0,								// nodeMask
		rootSig->GetBufferPointer(),	// pBlobWithRootSignature
		rootSig->GetBufferSize(),		// blobLengthInBytes
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void Box::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("Box.hlsl");
		_tstring filePath = path + _T("Assets\\") + file;

		m_Shaders["VS"] = DXUtils::CompileShader(filePath.c_str(), nullptr, "VS", "vs_5_0");
		m_Shaders["PS"] = DXUtils::CompileShader(filePath.c_str(), nullptr, "PS", "ps_5_0");
	}

	// InputLyout
	{
		// ��Ҫ����Vertex�ṹ�� hlsl �еĶ�������ṹ��������׼������Ԫ��������
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName������ HLSL
			0,											// SemanticIndex������ HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format������ Vertex
			0,											// InputSlot����
			Vertex::OffsetPos,							// AlignedByteOffset��
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// InputSlotClass
			0 },										// InstanceDataStepRate

			D3D12_INPUT_ELEMENT_DESC{
			"NORMAL", 0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0, Vertex::OffsetNom,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			D3D12_INPUT_ELEMENT_DESC{
			"TANGENT", 0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0, Vertex::OffsetTan,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			D3D12_INPUT_ELEMENT_DESC{
			"TEXCOORD", 0,
			DXGI_FORMAT_R32G32_FLOAT,
			0, Vertex::OffsetTex,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			D3D12_INPUT_ELEMENT_DESC{
			"COLOR", 0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, Vertex::OffsetClr,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}

	return;
}
void Box::BuildPipelineState()
{
	// ͼ�ι���״̬��PSO��������
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	{
		// ��ǩ��ָ�� - ID3D12RootSignature*
		psoDesc.pRootSignature = m_RootSignature.Get();

		// ��ɫ���ֽ��� - D3D12_SHADER_BYTECODE
		psoDesc.VS = { m_Shaders["VS"]->GetBufferPointer(), m_Shaders["VS"]->GetBufferSize() };
		psoDesc.PS = { m_Shaders["PS"]->GetBufferPointer(), m_Shaders["PS"]->GetBufferSize() };
		psoDesc.DS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
		psoDesc.HS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
		psoDesc.GS = D3D12_SHADER_BYTECODE{ nullptr, 0 };

		// ����� - D3D12_STREAM_OUTPUT_DESC
		psoDesc.StreamOutput = {};

		// ���״̬ - D3D12_BLEND_DESC
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		// ��դ���״̬ - D3D12_RASTERIZER_DESC
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// ���ģ��״̬ - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		//psoDesc.DepthStencilState.StencilEnable = FALSE;

		// ���벼�� - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
		//psoDesc.InputLayout = { Vertex::InputLayout.data(), (UINT)Vertex::InputLayout.size() };

		// �� - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// ͼԪ�������� - D3D12_PRIMITIVE_TOPOLOGY_TYPE
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// RTV��DXGI_FORMAT_R8G8B8A8_UNORM
		psoDesc.NumRenderTargets = BackBufferCount;
		for (int i = 0; i < BackBufferCount; i++)
		{
			psoDesc.RTVFormats[i] = m_BackBufferFormat;
		}

		// DSV��DXGI_FORMAT_D24_UNORM_S8_UINT
		psoDesc.DSVFormat = m_DepthStencilFormat;

		// ������ - DXGI_SAMPLE_DESC
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };
		psoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;

		// ?
		psoDesc.NodeMask = 0;

		// ? D3D12_CACHED_PIPELINE_STATE
		psoDesc.CachedPSO = {};

		// ?
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}

	// Create PSO
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSOs["Solid"])));

		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSOs["Wireframe"])));
	}

	return;
}
void Box::BuildBoxGeometry()
{
	if (m_GeoFlag != box)
		return;

	//m_Theta = 1.5f*XM_PI;
	m_Theta = 0;
	m_Phi = XM_PI / 3;
	m_Radius = 5.0f;

	m_IsWireframe = false;

	if (false)
	{
		MeshData mesh;
		MeshFactory::CreateBox(1.0f, 1.0f, 1.0f, &mesh);			// 
		//MeshFactory::CreateTetrahedron(1.0f, &mesh);
		//MeshFactory::CreateDodecahedron(1.0f, &mesh);
		//MeshFactory::CreateOctahedron(1.0f, &mesh);
		//MeshFactory::CreateIcosahedron(1.0f, &mesh);
		//MeshFactory::CreateTorus(2.0f, 1.0f, 30, &mesh);
		//MeshFactory::CreateCone(1.0f, 2.0f, 20, 20, &mesh);


		auto geo = std::make_unique<MeshGeometry>("boxGeo");
		geo->Init(m_device.Get(), m_CommandList.Get(), &mesh);
		geo->DrawArgs["box"] = SubmeshGeometry((int)mesh.IndexCount(), 0, 0);

		m_Geometries[geo->Name] = std::move(geo);

		return;
	}


	// 
	std::vector<Vertex> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),

		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};

	std::vector<uint32> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	MeshData mesh(vertices, indices);

	auto geo = std::make_unique<MeshGeometry>("boxGeo");
	geo->Init(m_device.Get(), m_CommandList.Get(), &mesh);
	geo->DrawArgs["box"] = SubmeshGeometry((int)indices.size(), 0, 0);

	m_Geometries[geo->Name] = std::move(geo);

	return;
}
void Box::BuildCarGeometry()
{
	if (m_GeoFlag != car)
		return;

	m_Theta = XM_PI * 0 / 2;
	m_Phi = XM_PI / 4;
	m_Radius = 20.0f;

	m_IsWireframe = false;


	//_tstring path = _tstring(KApplication::GetApp()->Startup()) + _T("Models\\");
	//_tstring file = _T("car.dat");
	_tstring path = _T("d:\\temp\\");
	_tstring file = _T("1.txt");

	_tstring filePath = path + file;

	std::ifstream fin(filePath);

	if (!fin)
	{
		MessageBox(0, filePath.c_str(), 0, 0);
		return;
	}

	UINT vcount = 0;	// VertexCount
	UINT tcount = 0;	// TriangleCount
	_astring ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;

	// VertexList (pos, normal)
	fin >> ignore >> ignore >> ignore;
	fin >> ignore;	// {

	std::vector<Vertex> vertices(vcount);
	for (UINT i = 0; i < vcount; i++)
	{
		fin >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		float r = KMaths::Rand(0.0f, 1.0f);
		float g = KMaths::Rand(0.0f, 1.0f);
		float b = KMaths::Rand(0.0f, 1.0f);
		vertices[i].Color = XMFLOAT4(r, g, b, 1.0f);
	}

	fin >> ignore;

	// TriangleList
	fin >> ignore;
	fin >> ignore;	// {

	std::vector<uint32> indices(3 * tcount);
	for (UINT i = 0; i < tcount; i++)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin >> ignore;	// }

	fin.close();

	MeshData mesh(vertices, indices);
	auto geo = std::make_unique<MeshGeometry>("carGeo");
	geo->Init(m_device.Get(), m_CommandList.Get(), &mesh);
	geo->DrawArgs["car"] = SubmeshGeometry((int)indices.size(), 0, 0);

	m_Geometries[geo->Name] = std::move(geo);

	return;
}
void Box::BuildSkullGeometry()
{
	if (m_GeoFlag != skull)
		return;

	m_Theta = XM_PI * 3 / 2;
	m_Phi = XM_PI / 4;
	m_Radius = 20.0f;

	m_IsWireframe = true;


	_tstring path = _tstring(KApplication::GetApp()->Startup()) + _T("Models\\");
	_tstring file = _T("skull.dat");
	_tstring filePath = path + file;

	std::ifstream fin(filePath);

	if (!fin)
	{
		MessageBox(0, filePath.c_str(), 0, 0);
		return;
	}

	UINT vcount = 0;	// VertexCount
	UINT tcount = 0;	// TriangleCount
	_astring ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;

	// VertexList (pos, normal)
	fin >> ignore >> ignore >> ignore;
	fin >> ignore;	// {

	std::vector<Vertex> vertices(vcount);
	for (UINT i = 0; i < vcount; i++)
	{
		fin >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	fin >> ignore;

	// TriangleList
	fin >> ignore;
	fin >> ignore;	// {

	std::vector<uint32> indices(3 * tcount);
	for (UINT i = 0; i < tcount; i++)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin >> ignore;	// }

	fin.close();

	MeshData mesh(vertices, indices);
	auto geo = std::make_unique<MeshGeometry>("skullGeo");
	geo->Init(m_device.Get(), m_CommandList.Get(), &mesh);
	geo->DrawArgs["skull"] = SubmeshGeometry((int)indices.size(), 0, 0);

	m_Geometries[geo->Name] = std::move(geo);

	return;
}
void Box::BuildShapesGeometry()
{
	if (m_GeoFlag != shapes)
		return;

	m_IsWireframe = true;

	m_Theta = XM_PI * 0 / 2;
	m_Phi = XM_PI / 4;
	m_Radius = 20.0f;

	// 4�������񣺺��ӡ��������塢Բ��
	MeshData box, grid, sphere, cylinder;

	MeshFactory::CreateBox(1.5f, 0.5f, 1.5f, 3, &box);
	MeshFactory::CreateGrid(20.0f, 30.0f, 60, 40, &grid);
	MeshFactory::CreateSphere(0.5f, 20, 20, &sphere);
	MeshFactory::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, &cylinder);

	// 1�����ǽ����м��������ӵ�һ���󶥵�/�����������С�

	// 1.1 ����������Ķ�������һ�����㻺������ - vertices
	UINT totalVertexCount = box.VertexCount()
		+ grid.VertexCount()
		+ sphere.VertexCount()
		+ cylinder.VertexCount();


	int k = 0;
	XMFLOAT4 meshColor = {};
	MeshData* pMeshData = nullptr;
	std::vector<Vertex> vertices(totalVertexCount);

	// box
	pMeshData = &box;
	meshColor = XMFLOAT4(DirectX::Colors::DarkGreen);
	for (UINT i = 0; i < pMeshData->VertexCount(); i++, k++)
	{
		vertices[k].Position = pMeshData->Vertices[i].Position;
		vertices[k].Color = meshColor;
	}

	// grid
	pMeshData = &grid;
	meshColor = XMFLOAT4(DirectX::Colors::ForestGreen);
	for (UINT i = 0; i < pMeshData->VertexCount(); i++, k++)
	{
		vertices[k].Position = pMeshData->Vertices[i].Position;
		vertices[k].Color = meshColor;
	}

	// sphere
	pMeshData = &sphere;
	meshColor = XMFLOAT4(DirectX::Colors::Crimson);
	for (UINT i = 0; i < pMeshData->VertexCount(); i++, k++)
	{
		vertices[k].Position = pMeshData->Vertices[i].Position;
		vertices[k].Color = meshColor;
	}

	// cylinder
	pMeshData = &cylinder;
	meshColor = XMFLOAT4(DirectX::Colors::SteelBlue);
	for (UINT i = 0; i < pMeshData->VertexCount(); i++, k++)
	{
		vertices[k].Position = pMeshData->Vertices[i].Position;
		vertices[k].Color = meshColor;
	}

	// 1.2 ��������������������һ�������������� - indices32
	std::vector<uint32> indices32;
	indices32.insert(indices32.end(), std::begin(box.Indices32), std::end(box.Indices32));
	indices32.insert(indices32.end(), std::begin(grid.Indices32), std::end(grid.Indices32));
	indices32.insert(indices32.end(), std::begin(sphere.Indices32), std::end(sphere.Indices32));
	indices32.insert(indices32.end(), std::begin(cylinder.Indices32), std::end(cylinder.Indices32));

	// 1.3 ���������
	MeshData meshShapes(vertices, indices32);

	// 2�����建������ÿ�������񸲸ǵ�����

	// ��������
	UINT boxIndexCount = box.IndexCount();
	UINT gridIndexCount = grid.IndexCount();
	UINT sphereIndexCount = sphere.IndexCount();
	UINT cylinderIndexCount = cylinder.IndexCount();

	// ����ƫ�Ƶ�ַ
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = boxIndexOffset + boxIndexCount;
	UINT sphereIndexOffset = gridIndexOffset + gridIndexCount;
	UINT cylinderIndexOffset = sphereIndexOffset + sphereIndexCount;

	// ����ƫ�Ƶ�ַ
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = boxVertexOffset + box.VertexCount();
	UINT sphereVertexOffset = gridVertexOffset + grid.VertexCount();
	UINT cylinderVertexOffset = sphereVertexOffset + sphere.VertexCount();

	// ������ - box
	SubmeshGeometry boxSubmesh;
	{
		boxSubmesh.IndexCount			= boxIndexCount;
		boxSubmesh.StartIndexLocation	= boxIndexOffset;
		boxSubmesh.BaseVertexLocation	= boxVertexOffset;
	}

	// ������ - grid
	SubmeshGeometry gridSubmesh;
	{
		gridSubmesh.IndexCount = gridIndexCount;
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;
	}

	// ������ - sphere
	SubmeshGeometry sphereSubmesh;
	{
		sphereSubmesh.IndexCount			= sphereIndexCount;
		sphereSubmesh.StartIndexLocation	= sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation	= sphereVertexOffset;
	}

	// ������ - cylinder
	SubmeshGeometry cylinderSubmesh;
	{
		cylinderSubmesh.IndexCount			= cylinderIndexCount;
		cylinderSubmesh.StartIndexLocation	= cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation	= cylinderVertexOffset;
	}


	// 4.����
	auto geo = std::make_unique<MeshGeometry>("shapesGeo");
	geo->Init(m_device.Get(), m_CommandList.Get(), &meshShapes);
	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	geo->DrawArgs["shapes"] = sphereSubmesh;

	m_Geometries[geo->Name] = std::move(geo);

	return;
}
void Box::BuildConstantBufferAndView()
{
	// ����������������
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	{
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
	}
	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));

	// ��������������
	m_ObjectCB = std::make_unique<TUploadBuffer<ObjectConstants>>(m_device.Get(), 1, true);

	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();

	// Offset to the ith object constant buffer in the buffer.
	int objCBIndex = 0;
	cbAddress += objCBByteSize * objCBIndex;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = DXUtils::CalculateConstantBufferByteSize(sizeof(ObjectConstants));

	int offsetInDescriptors = 0;
	auto cbvHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
	cbvHandle.ptr += m_cbvDescriptorSize * offsetInDescriptors;

	// ����
	m_device->CreateConstantBufferView(&cbvDesc, cbvHandle);

	return;
}
void Box::BuildBoxRadar()
{
	// �� LitRadar ����
	if (m_GeoFlag != radar)
		return;

	m_Theta = XM_PI * (1.0f / 2);
	m_Phi = XM_PI * (0.1f / 2 );
	m_Radius = 10.0f;

	MeshData mesh;
	//RadarMesh0(&mesh);	// ok
	//RadarMesh1(&mesh);
	//RadarMesh2(&mesh);
	//RadarMesh3(&mesh);		// ok

	//mesh.ReverseWinding();	// ?

	std::unique_ptr<MeshGeometry> boxGeo = std::make_unique<MeshGeometry>();
	{
		boxGeo->Name = "radarGeo";

		UINT vbSizeInBytes = (UINT)mesh.vbSizeInBytes();
		ThrowIfFailed(D3DCreateBlob(vbSizeInBytes, &boxGeo->VertexBufferCPU));						// �ȴ������㻺����(CPU�ڴ�)
		CopyMemory(boxGeo->VertexBufferCPU->GetBufferPointer(), mesh.pvbInitData(), vbSizeInBytes);

		//todo: VertexBufferGPU,VertexBufferUploader ������;�� VertexBufferGPU
		boxGeo->VertexBufferGPU = DXUtils::CreateDefaultBuffer(m_device.Get(), m_CommandList.Get(),
			mesh.pvbInitData(), vbSizeInBytes, boxGeo->VertexBufferUploader);

		UINT ibSizeInBytes = (UINT)mesh.ibSizeInBytes();
		ThrowIfFailed(D3DCreateBlob(ibSizeInBytes, &boxGeo->IndexBufferCPU));						// IB
		CopyMemory(boxGeo->IndexBufferCPU->GetBufferPointer(), mesh.pibInitData(), ibSizeInBytes);

		boxGeo->IndexBufferGPU = DXUtils::CreateDefaultBuffer(m_device.Get(), m_CommandList.Get(),	// 
			mesh.pibInitData(), ibSizeInBytes, boxGeo->IndexBufferUploader);

		boxGeo->VBSizeInBytes = vbSizeInBytes;
		boxGeo->VBStrideInBytes = mesh.vbStrideInBytes();

		boxGeo->IBSizeInBytes = ibSizeInBytes;
		boxGeo->IBVFormat = MeshData::IBVFormat;

		// ���Ʋ���
		boxGeo->DrawArgs["radar"] = SubmeshGeometry(mesh.IndexCount(), 0, 0);
	}
	m_Geometries[boxGeo->Name] = std::move(boxGeo);
}



//�����ڴ�С�ı䡿
void Box::Resized()
{
	// ����ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*(float)KMaths::PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}



//���������ݡ�
void Box::Update()
{
	UpdateCamera();
	UpdateObjectCBs();
}
void Box::UpdateCamera()
{
	// �۲��
	m_EyePos = DXUtils::SphericalToCartesian(m_Radius, m_Theta, m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);

	return;
}
void Box::UpdateObjectCBs()
{
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX worldViewProj = world * view * proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	m_ObjectCB->CopyData(0, objConstants);

	return;
}



//����Ⱦ���ơ�
void Box::Draw()
{
	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();

	//m_IsWireframe = true;
	ID3D12PipelineState* pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// ��λ���������������䱣��������¼
	ThrowIfFailed(pCommandAllocator->Reset());

	// ��λ�����б���ʼ��¼����
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// դ�񻯽׶�
		pCommandList->RSSetViewports(1, &m_ScreenViewport);
		pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		// ����ϲ���RTV + DSV
		pCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// �����Ⱦ����
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		pCommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// ������ģ����ͼ
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		pCommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// ���ø�ǩ��
		pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// ��������
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
		pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// ���õ�b0
		pCommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

		// ����װ��
		MeshGeometry* pGeo = nullptr;
		SubmeshGeometry submesh;

		if (m_GeoFlag == box)
		{
			pGeo = m_Geometries["boxGeo"].get();
			submesh = pGeo->DrawArgs["box"];
		}
		else if (m_GeoFlag == car)
		{
			pGeo = m_Geometries["carGeo"].get();
			submesh = pGeo->DrawArgs["car"];
		}
		else if (m_GeoFlag == skull)
		{
			pGeo = m_Geometries["skullGeo"].get();
			submesh = pGeo->DrawArgs["skull"];
		}
		else if (m_GeoFlag == shapes)
		{
			pGeo = m_Geometries["shapesGeo"].get();
			submesh = pGeo->DrawArgs["shapes"];
		}
		else if (m_GeoFlag == radar)
		{
			pGeo = m_Geometries["radarGeo"].get();
			submesh = pGeo->DrawArgs["radar"];
		}

		if (pGeo != nullptr)
		{
			//D3D_PRIMITIVE_TRIANGLE
			// D3D11_PRIMITIVE_TOPOLOGY_POINTLIST

			pCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pCommandList->IASetVertexBuffers(0, 1, &pGeo->VertexBufferView());
			pCommandList->IASetIndexBuffer(&pGeo->IndexBufferView());

			pCommandList->DrawIndexedInstanced(
				submesh.IndexCount,				// IndexCountPerInstance
				1,								// InstanceCount = 1
				submesh.StartIndexLocation,		// StartIndexLocation
				submesh.BaseVertexLocation,		// BaseVertexLocation
				0);								// StartInstanceLocation = 0

		}//pGeo != nullptr

	}

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// �ر������б�ֹͣ��¼����
	ThrowIfFailed(pCommandList->Close());

	// ���� - presentation

	// �������б��е����������������
	ID3D12CommandList* cmdLists[] = { pCommandList };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// ����ǰ��̨������
	ThrowIfFailed(m_SwapChain->Present(0, 0));

	// 
	WaitForGpu();
}




//������������

/*
void Box::RadarMesh0(MeshData* pMesh)
{
	m_Radius = 15.0f;

	pMesh->Clear();

	MC mc;

	// layer	= 24
	// width	= 68
	// height	= 52
	mc.LoadCTSlicerFromFiles16(("D:\\sfxData\\VTKData\\Radar\\rad3d\\rad"), 1, 24, 68, 52);

	MCP0 mcp = {};
	{
		mcp.CubeCountX = 68 - 1;
		mcp.CubeCountY = 52 - 1;
		mcp.CubeCountZ = 24 - 1;

		mcp.CubeSizeX = 0.10f;
		mcp.CubeSizeY = 0.10f;
		mcp.CubeSizeZ = 0.20f;
	}

	//�ؽ�ģ��
	//mc.Reconstruct16(mcp);
	//mc.Reconstruct16_0_bak(mcp);
	mc.Reconstruct16_0(mcp);

	std::vector<Vector3> outVertexList;
	mc.GetMesh(outVertexList);

	// ����


	// ��������
	int nCount = (int)outVertexList.size();
	//float r, g, b;

	for (int i = 0; i < nCount; i++)
	{
		Vector3 vec = outVertexList[i];

		XMFLOAT3 pnt = { (float)vec.x, (float)vec.y, (float)vec.z };

		float a = 0.1f;
		XMFLOAT4 clr = { 0.5f, 0.0f, 0.5f, 1.0f };	// RGBA

		//GetRadarColor(vec.V, r, g, b);
		//clr = { r, g, b, a };	// RGBA

		pMesh->Vertices.push_back(Vertex{ pnt, clr });
		pMesh->Indices32.push_back((uint32)i);
	}

	return;
}

void Box::RadarMesh1(MeshData* pMesh)
{
	m_Radius = 30.0f;

	pMesh->Clear();

	MC mc;

	// layer	= 24
	// width	= 68
	// height	= 52
	mc.LoadCTSlicerFromFiles16(("D:\\sfxData\\VTKData\\Radar\\rad3d\\rad"), 1, 24, 68, 52);

	int x = mc.m_Slices[0].Width;
	int y = mc.m_Slices[0].Height;
	int z = mc.m_Slices.size();
	int dims[3] = { x, y, z };

	vector<double> scalars;
	for (int k = 0; k < z; k++)
	{
		TSlice<VarType> slice = mc.m_Slices[k];
		for (int j = 0; j < y; j++)
		{
			for (int i = 0; i < x; i++)
			{
				double v = slice.GetValue(i, j);
				scalars.push_back(v);
			}
		}
	}

	double origin[3] = { 0, 0, 0 };
	double spacing[3] = { 0.10, 0.10, 0.10 };
	double values[6] = { 150, 200, 300, 350, 400, 450 };
	int n = ARRAYSIZE(values);

	mc.Reconstruct16_1(scalars.data(), dims, origin, spacing, values, n);

	// x-> 0-67
	// y-> 0-52
	// z-> 0-23
	std::vector<Vector3> outVertexList;
	mc.GetMesh(outVertexList);

	// ��������
	int nCount = (int)outVertexList.size();
	float r, g, b;

	for (int i = 0; i < nCount; i++)
	{
		Vector3 vec = outVertexList[i];

		XMFLOAT3 pnt = { (float)vec.x, (float)vec.y, (float)vec.z };

		XMFLOAT4 clr = { 0.5f, 0.0f, 0.5f, 1.0f };	// RGBA

		//GetRadarColor(vec.V, r, g, b);

		//if (i % 2 == 0)
		{
			clr = { r, g, b, 1.0f };	// RGBA
		}

		pMesh->Vertices.push_back(Vertex{ pnt, clr });
		pMesh->Indices32.push_back((uint32)i);
	}

	return;
}

void Box::RadarMesh2(MeshData* pMesh)
{
	m_Radius = 50.0f;

	pMesh->Clear();

	MC mc;

	// layer	= 24
	// width	= 68
	// height	= 52
	mc.LoadCTSlicerFromFiles16(("D:\\sfxData\\VTKData\\Radar\\rad3d\\rad"), 1, 24, 68, 52);

	int x = mc.m_Slices[0].Width;
	int y = mc.m_Slices[0].Height;
	int z = mc.m_Slices.size();
	int dims[3] = { x, y, z };

	vector<double> scalars;
	for (int k = 0; k < z; k++)
	{
		TSlice<VarType> slice = mc.m_Slices[k];
		for (int j = 0; j < y; j++)
		{
			for (int i = 0; i < x; i++)
			{
				double v = slice.GetValue(i, j);
				scalars.push_back(v);
			}
		}
	}

	double origin[3] = { 0, 0, 0 };
	double spacing[3] = { 0.10, 0.10, 0.20 };				// mc -> x,y,z
	double values[6] = { 150, 200, 300, 350, 400, 450 };

	mc.Reconstruct16_2(scalars.data(), dims, origin, spacing, values, 6);

	// x-> 0-67
	// y-> 0-52
	// z-> 0-23
	std::vector<Vector3> outVertexList;
	mc.GetMesh(outVertexList);

	// ��������
	int nCount = (int)outVertexList.size();
	float r, g, b;

	for (int i = 0; i < nCount; i++)
	{
		Vector3 vec = outVertexList[i];

		XMFLOAT3 pnt = { (float)vec.x, (float)vec.y, (float)vec.z };

		XMFLOAT4 clr = { 0.5f, 0.0f, 0.5f, 1.0f };	// RGBA

		//GetRadarColor(vec.V, r, g, b);

		//if (i % 2 == 0)
		{
			clr = { r, g, b, 1.0f };	// RGBA
		}

		pMesh->Vertices.push_back(Vertex{ pnt, clr });
		pMesh->Indices32.push_back((uint32)i);
	}

	return;
}

// ok
void Box::RadarMesh3(MeshData* pMesh)
{
	m_Theta = XM_PI * (-1.0f / 2);
	m_Phi = XM_PI * (0.9f / 2);
	m_Radius = 3.0f;

	pMesh->Clear();

	//MC mc;

	// layer	= 24
	// width	= 68
	// height	= 52
	//mc.LoadCTSlicerFromFiles16(("D:\\sfxData\\VTKData\\Radar\\rad3d\\rad"), 1, 24, 68, 52);


	int x = mc.m_Slices[0].Width;
	int y = mc.m_Slices[0].Height;
	int z = mc.m_Slices.size();
	int dims[3] = { x, y, z };

	std::vector<double> scalars;
	for (int k = 0; k < z; k++)
	{
		TSlice<WORD> slice = mc.m_Slices[k];
		for (int j = 0; j < y; j++)
		{
			for (int i = 0; i < x; i++)
			{
				double v = slice.GetValue(i, j);
				scalars.push_back(v);
			}
		}
	}

	// 68,52,24
	double origin[3] = { 0, 0, 0 };
	//int cubeCount[3] = { 68,52,24 };
	int cubeCount[3] = { 136, 104, 48 };
	double cubeSize[] = { 0.10, 0.10, 0.10 };
	double spacing[3] = { 0.10, 0.10, 0.10 };
	//double dContourValues[] = { 50, 100, 150, 200,300,350,400,450, 500 };
	double dContourValues[] = { 150, 200,300,350,400,450, 500 };
	//double dContourValues[] = { 350,400,450, 500 };

	std::vector<Vector3> outVertexList_old;


	//mc.Reconstruct16_3(scalars.data(), dims, cubeSize, cubeCount, values, ARRAYSIZE(values));
	//mc.GetMesh(outVertexList);


	KMarchingCubes mc2;
	mc2.Building_0(scalars.data(), dims, origin, spacing, dContourValues, ARRAYSIZE(dContourValues));
	//mc2.Building(scalars.data(), dims, cubeSize, cubeCount, values, ARRAYSIZE(values));

	std::vector<Vector3> vecs;
	mc2.GetMesh(vecs);

	// ����
	std::vector<Vector3> noms;
	mc2.GetNormals(noms);

	// ����ֵ
	std::vector<double> svalues;
	mc2.GetValues(svalues);



	// ��������
	int nCount = (int)vecs.size();
	float r, g, b;

	for (int i = 0; i < nCount; i++)
	{
		Vector3 vec = vecs[i];
		//XMFLOAT3 pnt = { (float)vec.x, (float)vec.y, (float)vec.z };
		XMFLOAT3 pnt = { (float)vec.x, (float)vec.z, (float)vec.y };

		Vector3 n = noms[i];
		XMFLOAT3 nom = { (float)n.x, (float)n.y, (float)n.z };
		XMFLOAT4 clr = { 0.5f, 0.0f, 0.5f, 1.0f };	// RGBA

		float scalar = (float)svalues[i];
		GetRadarColor(scalar, r, g, b);
		clr = { r, g, b, 1.0f };	// RGBA
		pMesh->Vertices.push_back(Vertex{ pnt, nom, clr });
		pMesh->Indices32.push_back((uint32)i);
	}

	//pMesh->InvertNormals();
	//pMesh->ReverseWinding();

	return;
}


void Box::GetRadarColor(float v, float& r, float& g, float& b)
{
	r = (float)0xC0 / 0xFF;		g = (float)0xC0 / 0xFF;		b = (float)0xFE / 0xFF;

	if (false) {}
	else if (v < 50)
	{
		r = (float)0x7A / 0xFF;		g = (float)0x72 / 0xFF;		b = (float)0xEE / 0xFF;
	}
	else if (v < 100)
	{
		r = (float)0x1E / 0xFF;		g = (float)0x1E / 0xFF;		b = (float)0xD0 / 0xFF;
	}
	else if (v < 150)
	{
		r = (float)0xA6 / 0xFF;		g = (float)0xFC / 0xFF;		b = (float)0xA8 / 0xFF;
	}
	else if (v < 200)
	{
		r = (float)0x00 / 0xFF;		g = (float)0xEA / 0xFF;		b = (float)0x00 / 0xFF;
	}
	else if (v < 250)
	{
		r = (float)0x10 / 0xFF;		g = (float)0x92 / 0xFF;		b = (float)0x1A / 0xFF;
	}
	else if (v < 300)
	{
		r = (float)0xFC / 0xFF;		g = (float)0xF4 / 0xFF;		b = (float)0x64 / 0xFF;
	}
	else if (v < 350)
	{
		r = (float)0xC8 / 0xFF;		g = (float)0xC8 / 0xFF;		b = (float)0x02 / 0xFF;
	}
	else if (v < 400)
	{
		r = (float)0x8C / 0xFF;		g = (float)0x8C / 0xFF;		b = (float)0x00 / 0xFF;
	}
	else if (v < 450)
	{
		r = (float)0xFE / 0xFF;		g = (float)0xAC / 0xFF;		b = (float)0xAC / 0xFF;
	}
	else if (v < 500)
	{
		r = (float)0xFE / 0xFF;		g = (float)0x64 / 0xFF;		b = (float)0x5C / 0xFF;
	}
	else if (v < 550)
	{
		r = (float)0xEE / 0xFF;		g = (float)0x02 / 0xFF;		b = (float)0x30 / 0xFF;
	}
	else if (v < 600)
	{
		r = (float)0xD4 / 0xFF;		g = (float)0x8E / 0xFF;		b = (float)0xFE / 0xFF;
	}
	else if (v < 650)
	{
		r = (float)0xAA / 0xFF;		g = (float)0x24 / 0xFF;		b = (float)0xFA / 0xFF;
	}
	else if (v < 700)
	{
		r = (float)0xFE / 0xFF;		g = (float)0xFE / 0xFF;		b = (float)0x00 / 0xFF;
	}
}

bool MC::LoadCTSlicerFromFiles16(string filePrefix, int indexBeg, int indexEnd, int width, int height)
{
	m_Slices.clear();

	int iSliceSize = width * height;
	int nBufferSize = iSliceSize * sizeof(VarType);

	int nCount = indexEnd - indexBeg + 1;

	//��ȡ����
	for (int i = 0; i < nCount; i++)
	{
		int n = i + indexBeg;
		string finalPath = filePrefix + "." + std::to_string(n);

		std::ifstream fin(finalPath, std::ifstream::binary);
		if (!fin.is_open())
		{
			std::cout << "File Load Error!! :  " << finalPath << std::endl;
			return false;
		}

		TSlice<VarType> slice(width, height);

		slice.ElementArray.resize(nBufferSize);
		fin.read(reinterpret_cast<char*>(slice.ElementArray.data()), nBufferSize);

		m_Slices.push_back(slice);

		//�ر��ļ�
		fin.close();

		std::cout << "File Loaded OK: " << finalPath << std::endl;
	}


	return true;
}

*/