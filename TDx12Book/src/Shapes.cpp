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



//【鼠标消息】
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


//【构造资产】
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
	// 需要2个常量缓冲区：过程常量、物体常量，使用描述符表
	const int numCbv = 2;

	// 根参数可以是描述符表、根描述符或根常量
	CD3DX12_ROOT_PARAMETER slotRootParameter[numCbv];

	CD3DX12_DESCRIPTOR_RANGE cbvTable[numCbv];
	cbvTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	cbvTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	// 根参数初始化为描述符表
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable[0]);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable[1]);

	// 根签名描述符
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		numCbv, &slotRootParameter[0],
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// 序列化根签名描述符
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

	// 创建根签名
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
		// 需要根据Vertex结构和 hlsl 中的顶点输入结构语义索引准备输入元素描述符
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName，依据 HLSL
			0,											// SemanticIndex，依据 HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format，依据 Vertex
			0,											// InputSlot，？
			Vertex::OffsetPos,							// AlignedByteOffset，
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
	// 图形管线状态（PSO）描述符
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	{
		// 根签名指针 - ID3D12RootSignature*
		psoDesc.pRootSignature = m_RootSignature.Get();

		// 着色器字节码 - D3D12_SHADER_BYTECODE
		psoDesc.VS = { m_Shaders["VS"]->GetBufferPointer(), m_Shaders["VS"]->GetBufferSize() };
		psoDesc.PS = { m_Shaders["PS"]->GetBufferPointer(), m_Shaders["PS"]->GetBufferSize() };
		psoDesc.DS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
		psoDesc.HS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
		psoDesc.GS = D3D12_SHADER_BYTECODE{ nullptr, 0 };

		// 流输出 - D3D12_STREAM_OUTPUT_DESC
		psoDesc.StreamOutput = {};

		// 混合状态 - D3D12_BLEND_DESC
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		// 光栅输出状态 - D3D12_RASTERIZER_DESC
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// 深度模板状态 - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		//psoDesc.DepthStencilState.StencilEnable = FALSE;

		// 输入布局 - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };

		// ？ - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// 图元拓扑类型 - D3D12_PRIMITIVE_TOPOLOGY_TYPE
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// RTV，DXGI_FORMAT_R8G8B8A8_UNORM
		psoDesc.NumRenderTargets = BackBufferCount;
		for (int i = 0; i < BackBufferCount; i++)
		{
			psoDesc.RTVFormats[i] = m_BackBufferFormat;
		}

		// DSV，DXGI_FORMAT_D24_UNORM_S8_UINT
		psoDesc.DSVFormat = m_DepthStencilFormat;

		// 采样器 - DXGI_SAMPLE_DESC
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
	// 4种子网格：盒子、网格、球体、圆柱
	MeshData box, grid, sphere, cylinder;

	MeshFactory::CreateBox(1.5f, 0.5f, 1.5f, 3, &box);
	MeshFactory::CreateGrid(20.0f, 30.0f, 60, 40, &grid);
	MeshFactory::CreateSphere(0.5f, 20, 20, &sphere);
	MeshFactory::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, &cylinder);

	// 1、我们将所有几何体连接到一个大顶点/索引缓冲区中。

	// 1.1 将所有网格的顶点打包到一个顶点缓冲区中 - vertices
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

	// 1.2 将所有网格的索引打包到一个索引缓冲区中 - indices32
	std::vector<uint32> indices32;
	indices32.insert(indices32.end(), std::begin(box.Indices32), std::end(box.Indices32));
	indices32.insert(indices32.end(), std::begin(grid.Indices32), std::end(grid.Indices32));
	indices32.insert(indices32.end(), std::begin(sphere.Indices32), std::end(sphere.Indices32));
	indices32.insert(indices32.end(), std::begin(cylinder.Indices32), std::end(cylinder.Indices32));

	// 1.3 构造大网格
	MeshData meshShapes(vertices, indices32);

	// 2、定义缓冲区中每个子网格覆盖的区域

	// 索引数量
	UINT boxIndexCount = box.IndexCount();
	UINT gridIndexCount = grid.IndexCount();
	UINT sphereIndexCount = sphere.IndexCount();
	UINT cylinderIndexCount = cylinder.IndexCount();

	// 索引偏移地址
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = boxIndexOffset + boxIndexCount;
	UINT sphereIndexOffset = gridIndexOffset + gridIndexCount;
	UINT cylinderIndexOffset = sphereIndexOffset + sphereIndexCount;

	// 顶点偏移地址
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = boxVertexOffset + box.VertexCount();
	UINT sphereVertexOffset = gridVertexOffset + grid.VertexCount();
	UINT cylinderVertexOffset = sphereVertexOffset + sphere.VertexCount();

	// 子网格 - box
	SubmeshGeometry boxSubmesh;
	{
		boxSubmesh.IndexCount = boxIndexCount;
		boxSubmesh.StartIndexLocation = boxIndexOffset;
		boxSubmesh.BaseVertexLocation = boxVertexOffset;
	}

	// 子网格 - grid
	SubmeshGeometry gridSubmesh;
	{
		gridSubmesh.IndexCount = gridIndexCount;
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;
	}

	// 子网格 - sphere
	SubmeshGeometry sphereSubmesh;
	{
		sphereSubmesh.IndexCount = sphereIndexCount;
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
	}

	// 子网格 - cylinder
	SubmeshGeometry cylinderSubmesh;
	{
		cylinderSubmesh.IndexCount = cylinderIndexCount;
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
	}


	// 4.保存
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
	// 1box、 1grid、
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
	// 假设由3个帧资源与n个渲染项，那么就应该存在3个渲染过程常量缓冲区
	// 以及3n个物体常量缓冲区，我们需要创建3(n+1)个常量缓冲区视图(CBV)
	// 

	// 计算需要的CBV描述符数量
	UINT objCount = (UINT)m_AllRitems.size();
	UINT numDescriptors = (objCount + 1) * m_FrameResourceCount;

	m_PassCbvOffset = objCount * m_FrameResourceCount;

	// 1.创建CBV描述符堆以便容纳足够的描述符
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	{
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// 堆类型
		cbvHeapDesc.NumDescriptors = numDescriptors;					// 可容纳描述符数量
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// 堆标记
		cbvHeapDesc.NodeMask = 0;
	}
	m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap));

	// 2.填充CBV堆：
	// 描述符0至n-1是第0个帧资源的物体CBV，n 至 2n-1 容纳了第1个帧资源的物体CBV，
	// 2n 至 3n-1 容纳了第2个帧资源的物体CBV。最后3n、3n+1、3n+2分别保存第0、1、2帧
	// 资源 的渲染过程CBV

	// 物体常量缓冲区（256对齐）字节大小
	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::ObjectConstants));

	for (int frmindex = 0; frmindex < m_FrameResourceCount; frmindex++)
	{
		for (UINT i = 0; i < objCount; i++)
		{
			// 物体常量资源
			auto objCB = m_FrameResources[frmindex]->ObjectCB->Resource();

			// 常量缓冲区地址
			D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = objCB->GetGPUVirtualAddress();
			bufferLocation += objCBByteSize * i;

			// CBV描述符
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			{
				cbvDesc.BufferLocation = bufferLocation;
				cbvDesc.SizeInBytes = objCBByteSize;
			}

			// D3D12_CPU_DESCRIPTOR_HANDLE
			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += m_cbvDescriptorSize * (objCount*frmindex + i);

			// 创建物体CBV
			m_device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}


	// 过程常量缓冲区（256对齐）字节大小
	UINT passCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::PassConstants));
	for (int frmindex = 0; frmindex < m_FrameResourceCount; frmindex++)
	{
		// 过程常量资源
		auto passCB = m_FrameResources[frmindex]->PassCB->Resource();

		// 常量缓冲区地址
		D3D12_GPU_VIRTUAL_ADDRESS bufferLocation = passCB->GetGPUVirtualAddress();

		// CBV描述符
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		{
			cbvDesc.BufferLocation = bufferLocation;
			cbvDesc.SizeInBytes = passCBByteSize;
		}

		// D3D12_CPU_DESCRIPTOR_HANDLE
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += m_cbvDescriptorSize * (m_PassCbvOffset + frmindex);

		// 创建过程CBV
		m_device->CreateConstantBufferView(&cbvDesc, handle);
	}

	return;
}


//【窗口大小改变】
void Shapes::Resized()
{
	UpdateProj();
}
void Shapes::UpdateProj()
{
	// 只在 OnResize() 调用
	XMMATRIX proj = XMMatrixPerspectiveFovLH((float)KMaths::PI / 4, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, proj);
}


//【更新数据】
void Shapes::Update()
{
	UpdateFrameResource();

	UpdateCamera();
	UpdateObjectCBs();
	UpdatePassCBs();
}
void Shapes::UpdateCamera()
{
	// 计算观察点位置（摄像机）
	m_EyePos = DXUtils::SphericalToCartesian(m_Radius, m_Theta, m_Phi);

	// 构建观察矩阵 - view matrix
	FXMVECTOR EyePosition = DirectX::XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	FXMVECTOR FocusPosition = DirectX::XMVectorZero();
	FXMVECTOR UpDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(EyePosition, FocusPosition, UpDirection);

	// 保存观察矩阵
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
			// 装载物体世界矩阵
			XMMATRIX world = XMLoadFloat4x4(&ritem->World);

			FrameResourceShapes::ObjectConstants objConstants;
			//XMStoreFloat4x4(&objConstants.World, world);					// todo: 错误，没有转置
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



// 【渲染绘制】
void Shapes::Draw()
{
	//ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12CommandAllocator* pCommandAllocator = CurrentFrameResource()->CmdListAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();

	ID3D12PipelineState* pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// 复位命令分配器，清除其保存的命令记录
	ThrowIfFailed(pCommandAllocator->Reset());

	// 复位命令列表，开始记录命令
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// 栅格化阶段
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出合并：RTV + DSV
		m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// 清除渲染背景
		//const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		const float* clearColor = Colors::LightSkyBlue;
		m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// 设置根签名
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 描述符堆
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
		m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// 绑定过程常量到b1
		D3D12_GPU_DESCRIPTOR_HANDLE passCbvGpuHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
		passCbvGpuHandle.ptr += m_cbvDescriptorSize * (m_PassCbvOffset + m_FrameResourceIndex);
		m_CommandList->SetGraphicsRootDescriptorTable(1, passCbvGpuHandle);

		DrawRenderItems();

	}

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 关闭命令列表，停止记录命令
	ThrowIfFailed(m_CommandList->Close());

	// 呈现 - presentation

	{
		// 把命令列表加入命令队列
		ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		// 交换前后台缓冲区
		ThrowIfFailed(m_SwapChain->Present(0, 0));
		m_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// 将围栏值向前推进，将命令标记到此围栏点
		CurrentFrameResource()->FenceValue = ++m_FenceValue;

		// 向命令队列中添加一条指令，以设置新的围栏点。
		// 因为我们在GPU时间轴上，在GPU处理完之前的所有命令之前，
		// 此Signal()将不会设置新的围栏点。
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
	// 绘制渲染项
	UINT objCount = (UINT)m_AllRitems.size();
	auto objectCB = CurrentFrameResource()->ObjectCB->Resource();
	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::ObjectConstants));
	for (auto& ritem : m_AllRitems)
	{
		// 绑定物体常量到b0
		D3D12_GPU_DESCRIPTOR_HANDLE objCbvGpuHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
		objCbvGpuHandle.ptr += m_cbvDescriptorSize * (objCount * m_FrameResourceIndex + ritem->ObjCBIndex);
		m_CommandList->SetGraphicsRootDescriptorTable(0, objCbvGpuHandle);

		// 输入装配
		m_CommandList->IASetVertexBuffers(0, 1, &ritem->Geo->VertexBufferView());
		m_CommandList->IASetIndexBuffer(&ritem->Geo->IndexBufferView());
		m_CommandList->IASetPrimitiveTopology(ritem->PrimitiveType);

		// 绘制
		m_CommandList->DrawIndexedInstanced(
			ritem->IndexCount,			// IndexCountPerInstance
			1,							// InstanceCount
			ritem->StartIndexLocation,	// StartIndexLocation
			ritem->BaseVertexLocation,	// BaseVertexLocation
			0);
	}
}
