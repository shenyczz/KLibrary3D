#include "stdafx.h"
#include "Shapes.h"


Shapes::Shapes()
{
	m_IsWireframe = true;

	m_Theta = XM_PI * 3 / 2;
	m_Phi = XM_PI / 4;
	m_Radius = 20.0f;

	m_FrameResourceCount = 3;
}


Shapes::~Shapes()
{
}



//�������Ϣ��
void Shapes::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	Dx12Book::OnMouseDown(wParam, x, y);
}
void Shapes::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book::OnMouseUp(wParam, x, y);
}
void Shapes::OnMouseMove(WPARAM wParam, int x, int y)
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
		m_Radius = KMaths::Clamp(m_Radius, 5.0f, 150.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}


//�������ʲ���
void Shapes::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();

	BuildShapesGeometry();
	BuildRenderItems();
	BuildFrameResources();

	BuildConstantBufferAndView();

	return;
}
void Shapes::BuildRootSignature()
{
	// ��Ҫ2�����������������̳��������峣����ʹ����������
	const int numCbv = 2;

	// �����������������������������������
	CD3DX12_ROOT_PARAMETER slotRootParameter[numCbv];

	CD3DX12_DESCRIPTOR_RANGE cbvTable[numCbv];
	cbvTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	cbvTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	// ��������ʼ��Ϊ��������
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable[0]);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable[1]);

	// ��ǩ��������
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		numCbv, &slotRootParameter[0],
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// ���л���ǩ��������
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

	// ������ǩ��
	ThrowIfFailed(m_device->CreateRootSignature(
		0,								// nodeMask
		rootSig->GetBufferPointer(),	// pBlobWithRootSignature
		rootSig->GetBufferSize(),		// blobLengthInBytes
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void Shapes::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("Shapes.hlsl");
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
void Shapes::BuildPipelineState()
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
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// ���ģ��״̬ - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		//psoDesc.DepthStencilState.StencilEnable = FALSE;

		// ���벼�� - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };

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
void Shapes::BuildShapesGeometry()
{
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
	meshColor = XMFLOAT4(DirectX::Colors::Blue);
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
		boxSubmesh.IndexCount = boxIndexCount;
		boxSubmesh.StartIndexLocation = boxIndexOffset;
		boxSubmesh.BaseVertexLocation = boxVertexOffset;
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
		sphereSubmesh.IndexCount = sphereIndexCount;
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
	}

	// ������ - cylinder
	SubmeshGeometry cylinderSubmesh;
	{
		cylinderSubmesh.IndexCount = cylinderIndexCount;
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
	}


	// 4.����
	auto geo = std::make_unique<MeshGeometry>("shapesGeo");
	geo->Init(m_device.Get(), m_CommandList.Get(), &meshShapes);
	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	m_Geometries[geo->Name] = std::move(geo);

	return;
}
void Shapes::BuildRenderItems()
{
	// 1box�� 1grid��
	// 5 cylinder + 5 sphere -> left
	// 5 cylinder + 5 sphere -> right

	int objCBindex = 0;
	MeshGeometry* pMeshGeometry = m_Geometries["shapesGeo"].get();

	// box
	{
		_astring subname = "box";
		auto ritem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
		ritem->ObjCBIndex = objCBindex++;
		ritem->World = DXUtils::Identity4x4;
		ritem->Geo = pMeshGeometry;
		ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ritem->IndexCount = pMeshGeometry->DrawArgs[subname].IndexCount;
		ritem->StartIndexLocation = pMeshGeometry->DrawArgs[subname].StartIndexLocation;
		ritem->BaseVertexLocation = pMeshGeometry->DrawArgs[subname].BaseVertexLocation;
		XMStoreFloat4x4(&ritem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
		m_AllRitems.push_back(std::move(ritem));
	}


	// grid
	{
		_astring subname = "grid";
		auto ritem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
		ritem->ObjCBIndex = objCBindex++;
		ritem->World = DXUtils::Identity4x4;
		ritem->Geo = pMeshGeometry;
		ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ritem->IndexCount = pMeshGeometry->DrawArgs[subname].IndexCount;
		ritem->StartIndexLocation = pMeshGeometry->DrawArgs[subname].StartIndexLocation;
		ritem->BaseVertexLocation = pMeshGeometry->DrawArgs[subname].BaseVertexLocation;
		m_AllRitems.push_back(std::move(ritem));
	}

	// cylinder - left
	{
		_astring subname = "cylinder";
		for (int i = 0; i < 5; i++)
		{
			auto ritem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
			ritem->ObjCBIndex = objCBindex++;
			ritem->Geo = pMeshGeometry;
			ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			ritem->IndexCount = pMeshGeometry->DrawArgs[subname].IndexCount;
			ritem->StartIndexLocation = pMeshGeometry->DrawArgs[subname].StartIndexLocation;
			ritem->BaseVertexLocation = pMeshGeometry->DrawArgs[subname].BaseVertexLocation;
			XMStoreFloat4x4(&ritem->World, XMMatrixTranslation(-5.0f, 1.5f, -10.0f + 5.0f * i));
			m_AllRitems.push_back(std::move(ritem));
		}
	}

	// sphere - left
	{
		_astring subname = "sphere";
		for (int i = 0; i < 5; i++)
		{
			auto ritem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
			ritem->ObjCBIndex = objCBindex++;
			ritem->Geo = pMeshGeometry;
			ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			ritem->IndexCount = pMeshGeometry->DrawArgs[subname].IndexCount;
			ritem->StartIndexLocation = pMeshGeometry->DrawArgs[subname].StartIndexLocation;
			ritem->BaseVertexLocation = pMeshGeometry->DrawArgs[subname].BaseVertexLocation;
			XMStoreFloat4x4(&ritem->World, XMMatrixTranslation(-5.0f, 3.5f, -10.0f + 5.0f * i));
			m_AllRitems.push_back(std::move(ritem));
		}
	}


	// cylinder - right
	{
		_astring subname = "cylinder";
		for (int i = 0; i < 5; i++)
		{
			auto ritem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
			ritem->ObjCBIndex = objCBindex++;
			ritem->Geo = pMeshGeometry;
			ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			ritem->IndexCount = pMeshGeometry->DrawArgs[subname].IndexCount;
			ritem->StartIndexLocation = pMeshGeometry->DrawArgs[subname].StartIndexLocation;
			ritem->BaseVertexLocation = pMeshGeometry->DrawArgs[subname].BaseVertexLocation;
			XMStoreFloat4x4(&ritem->World, XMMatrixTranslation(+5.0f, 1.5f, -10.0f + 5.0f * i));
			m_AllRitems.push_back(std::move(ritem));
		}
	}


	// sphere - right
	{
		_astring subname = "sphere";
		for (int i = 0; i < 5; i++)
		{
			auto ritem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
			ritem->ObjCBIndex = objCBindex++;
			ritem->Geo = pMeshGeometry;
			ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			ritem->IndexCount = pMeshGeometry->DrawArgs[subname].IndexCount;
			ritem->StartIndexLocation = pMeshGeometry->DrawArgs[subname].StartIndexLocation;
			ritem->BaseVertexLocation = pMeshGeometry->DrawArgs[subname].BaseVertexLocation;
			XMStoreFloat4x4(&ritem->World, XMMatrixTranslation(+5.0f, 3.5f, -10.0f + 5.0f * i));
			m_AllRitems.push_back(std::move(ritem));
		}
	}

	return;


}
void Shapes::BuildFrameResources()
{
	for (int i = 0; i < m_FrameResourceCount; i++)
	{
		auto fr = std::make_unique<FrameResourceShapes>(m_device.Get(),
			1,
			(UINT)m_AllRitems.size());

		m_FrameResources.push_back(std::move(fr));
	}
}
void Shapes::BuildConstantBufferAndView()
{
	// 
	// ������3��֡��Դ��n����Ⱦ���ô��Ӧ�ô���3����Ⱦ���̳���������
	// �Լ�3n�����峣����������������Ҫ����3(n+1)��������������ͼ(CBV)
	// 

	// ������Ҫ��CBV����������
	UINT objCount = (UINT)m_AllRitems.size();
	UINT numDescriptors = (objCount + 1) * m_FrameResourceCount;

	m_PassCbvOffset = objCount * m_FrameResourceCount;

	// 1.����CBV���������Ա������㹻��������
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	{
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// ������
		cbvHeapDesc.NumDescriptors = numDescriptors;					// ����������������
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �ѱ��
		cbvHeapDesc.NodeMask = 0;
	}
	m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap));

	// 2.���CBV�ѣ�
	// ������0��n-1�ǵ�0��֡��Դ������CBV��n �� 2n-1 �����˵�1��֡��Դ������CBV��
	// 2n �� 3n-1 �����˵�2��֡��Դ������CBV�����3n��3n+1��3n+2�ֱ𱣴��0��1��2֡
	// ��Դ ����Ⱦ����CBV

	// ���峣����������256���룩�ֽڴ�С
	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::ObjectConstants));

	for (int frmindex = 0; frmindex < m_FrameResourceCount; frmindex++)
	{
		for (UINT i = 0; i < objCount; i++)
		{
			// ���峣����Դ
			auto objCB = m_FrameResources[frmindex]->ObjectCB->Resource();

			// ������������ַ
			D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = objCB->GetGPUVirtualAddress();
			bufferLocation += objCBByteSize * i;

			// CBV������
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			{
				cbvDesc.BufferLocation = bufferLocation;
				cbvDesc.SizeInBytes = objCBByteSize;
			}

			// D3D12_CPU_DESCRIPTOR_HANDLE
			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += m_cbvDescriptorSize * (objCount*frmindex + i);

			// ��������CBV
			m_device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}


	// ���̳�����������256���룩�ֽڴ�С
	UINT passCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::PassConstants));
	for (int frmindex = 0; frmindex < m_FrameResourceCount; frmindex++)
	{
		// ���̳�����Դ
		auto passCB = m_FrameResources[frmindex]->PassCB->Resource();

		// ������������ַ
		D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = passCB->GetGPUVirtualAddress();

		// CBV������
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		{
			cbvDesc.BufferLocation = bufferLocation;
			cbvDesc.SizeInBytes = passCBByteSize;
		}

		// D3D12_CPU_DESCRIPTOR_HANDLE
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += m_cbvDescriptorSize * (m_PassCbvOffset + frmindex);

		// ��������CBV
		m_device->CreateConstantBufferView(&cbvDesc, handle);
	}

	return;
}


//�����ڴ�С�ı䡿
void Shapes::Resized()
{
	UpdateProj();
}
void Shapes::UpdateProj()
{
	// ֻ�� OnResize() ����
	XMMATRIX proj = XMMatrixPerspectiveFovLH((float)KMaths::PI / 4, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, proj);
}


//���������ݡ�
void Shapes::Update()
{
	UpdateFrameResource();

	UpdateCamera();
	UpdateObjectCBs();
	UpdatePassCBs();
}
void Shapes::UpdateCamera()
{
	// ����۲��λ�ã��������
	m_EyePos = DXUtils::SphericalToCartesian(m_Radius, m_Theta, m_Phi);

	// �����۲���� - view matrix
	FXMVECTOR EyePosition = DirectX::XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	FXMVECTOR FocusPosition = DirectX::XMVectorZero();
	FXMVECTOR UpDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(EyePosition, FocusPosition, UpDirection);

	// ����۲����
	XMStoreFloat4x4(&m_View, view);
	return;
}
void Shapes::UpdateObjectCBs()
{
	auto objCB = CurrentFrameResource()->ObjectCB.get();
	for (auto &ritem : m_AllRitems)
	{
		if (ritem->Dirtys > 0)
		{
			// װ�������������
			XMMATRIX world = XMLoadFloat4x4(&ritem->World);

			FrameResourceShapes::ObjectConstants objConstants;
			//XMStoreFloat4x4(&objConstants.World, world);					// todo: ����û��ת��
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			objCB->CopyData(ritem->ObjCBIndex, objConstants);

			ritem->Dirtys--;
		}
	}

	return;
}
void Shapes::UpdatePassCBs()
{
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	//XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	//XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	//XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	FrameResourceShapes::PassConstants passConstants;
	XMStoreFloat4x4(&passConstants.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&passConstants.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(viewProj));

	//XMStoreFloat4x4(&passConstants.InvView, XMMatrixTranspose(invView));
	//XMStoreFloat4x4(&passConstants.InvProj, XMMatrixTranspose(invProj));
	//XMStoreFloat4x4(&passConstants.InvViewProj, XMMatrixTranspose(invViewProj));

	//passConstants.EyePosW = m_EyePos;
	//passConstants.RenderTargetSize = XMFLOAT2((float)m_pWindow->ClientWidth(), (float)m_pWindow->ClientHeight());
	//passConstants.InvRenderTargetSize = XMFLOAT2(1.0f / m_pWindow->ClientWidth(), 1.0f / m_pWindow->ClientHeight());
	//passConstants.NearZ = 1.0f;
	//passConstants.FarZ = 1000.0f;
	//passConstants.TotalTime = gt.TotalTime();
	//passConstants.DeltaTime = gt.DeltaTime();

	auto passCB = CurrentFrameResource()->PassCB.get();
	passCB->CopyData(0, passConstants);

	return;
}



// ����Ⱦ���ơ�
void Shapes::Draw()
{
	//ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12CommandAllocator* pCommandAllocator = CurrentFrameResource()->CmdListAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();

	ID3D12PipelineState* pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// ��λ���������������䱣��������¼
	ThrowIfFailed(pCommandAllocator->Reset());

	// ��λ�����б���ʼ��¼����
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// դ�񻯽׶�
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// ����ϲ���RTV + DSV
		m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// �����Ⱦ����
		//const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		const float* clearColor = Colors::LightSkyBlue;
		m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// ������ģ����ͼ
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// ���ø�ǩ��
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// ��������
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
		m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// �󶨹��̳�����b1
		D3D12_GPU_DESCRIPTOR_HANDLE passCbvGpuHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
		passCbvGpuHandle.ptr += m_cbvDescriptorSize * (m_PassCbvOffset + m_FrameResourceIndex);
		m_CommandList->SetGraphicsRootDescriptorTable(1, passCbvGpuHandle);

		DrawRenderItems();

	}

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// �ر������б�ֹͣ��¼����
	ThrowIfFailed(m_CommandList->Close());

	// ���� - presentation

	{
		// �������б�����������
		ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		// ����ǰ��̨������
		ThrowIfFailed(m_SwapChain->Present(0, 0));
		m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// ��Χ��ֵ��ǰ�ƽ����������ǵ���Χ����
		CurrentFrameResource()->FenceValue = ++m_FenceValue;

		// ��������������һ��ָ��������µ�Χ���㡣
		// ��Ϊ������GPUʱ�����ϣ���GPU������֮ǰ����������֮ǰ��
		// ��Signal()�����������µ�Χ���㡣
		m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);

#ifdef _DEBUG
		KUtil::Trace(KString::Format(_T("BackBufferIndex = %d, FrameIndex = %d")
			, m_BackBufferIndex
			, m_FrameResourceIndex));
#endif // DEBUG

	}

	return;
}
void Shapes::DrawRenderItems()
{
	// ������Ⱦ��
	UINT objCount = (UINT)m_AllRitems.size();
	auto objectCB = CurrentFrameResource()->ObjectCB->Resource();
	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::ObjectConstants));
	for (auto& ritem : m_AllRitems)
	{
		// �����峣����b0
		D3D12_GPU_DESCRIPTOR_HANDLE objCbvGpuHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
		objCbvGpuHandle.ptr += m_cbvDescriptorSize * (objCount * m_FrameResourceIndex + ritem->ObjCBIndex);
		m_CommandList->SetGraphicsRootDescriptorTable(0, objCbvGpuHandle);

		// ����װ��
		m_CommandList->IASetVertexBuffers(0, 1, &ritem->Geo->VertexBufferView());
		m_CommandList->IASetIndexBuffer(&ritem->Geo->IndexBufferView());
		m_CommandList->IASetPrimitiveTopology(ritem->PrimitiveType);

		// ����
		m_CommandList->DrawIndexedInstanced(
			ritem->IndexCount,			// IndexCountPerInstance
			1,							// InstanceCount
			ritem->StartIndexLocation,	// StartIndexLocation
			ritem->BaseVertexLocation,	// BaseVertexLocation
			0);
	}
}
