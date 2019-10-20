#include "stdafx.h"
#include "Shapes.h"



Shapes::Shapes()
{
	m_Theta = XM_PI * (3.0f / 2);
	m_Phi = XM_PI / 5;
	m_Radius = 30.0f;

	m_IsWireframe = true;
	m_FrameResourceCount = 1;
}

Shapes::~Shapes()
{
}


void Shapes::OnResize()
{
	Dx12Book::OnResize();

	UpdateProj();	// 更新 Proj

	return;
}

void Shapes::OnUpdate()
{
	Dx12Book::OnUpdate();

	m_psoCurrent = m_IsWireframe
		? m_psoMap[PsoStyle::WireFrame]
		: m_psoMap[PsoStyle::Solid];

	// Cycle through the circular frame resource array.
	m_CurrFrameResourceIndex = (m_CurrFrameResourceIndex + 1) % m_FrameResourceCount;
	m_CurrFrameResource = m_FrameResources[m_CurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	//// If not, wait until the GPU has completed commands up to this fence point.
	//if (m_CurrFrameResource->FenceValue != 0 && m_Fence->GetCompletedValue() < m_CurrFrameResource->FenceValue)
	//{
	//	HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	//	ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrFrameResource->FenceValue, eventHandle));
	//	WaitForSingleObject(eventHandle, INFINITE);
	//	CloseHandle(eventHandle);
	//}

	//更新对象常量
	{
		// World
		UpdateObjectCBs();
		UpdatePassCBs();

		// View
		UpdateCamera();

		// Proj
		UpdateProj();
	}

	return;
}

void Shapes::OnRender()
{
	Dx12Book::OnRender();
}

void Shapes::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_pWindow->Handle());

	return;
}
void Shapes::OnMouseUp(WPARAM wParam, int x, int y)
{
	ReleaseCapture();
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
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += (dx - dy)*5;


		// Restrict the radius.
		m_Radius = KMaths::Clamp(m_Radius, 3.0f, 55.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	return;
}

void Shapes::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBVs.
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		2, slotRootParameter,	// 2个常量缓冲区
		0, nullptr,				// 无静态采样器
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot(??signal??)
	// which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> errorBlob;
	ComPtr<ID3DBlob> rootSigBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rootSigBlob.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_device->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

	return;
}

void Shapes::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("bookShapes.hlsl");
		_tstring filePath = path + _T("Asserts\\") + file;

		m_Shaders["VS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "VS", "vs_5_0");
		m_Shaders["PS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "PS", "ps_5_0");

		// m_VSByteCode 是根据顶点输入结构（VertexIn）编译乘的字节码，
		// VertexIn 定义的字段必须在定义的输入元素描述符中找到
		// 输入元素描述符中可以定义 VertexIn 没有声明的语义名称（SemanticName）
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
			Vertex::OffsetPos,									// AlignedByteOffset，
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

void Shapes::BuildMeshData()
{
	BuildShapesGeometry();
	BuildRenderItems();
	BuildFrameResources();

	return;
}

void Shapes::BuildConstantBufferAndView()
{
	BuildConstantBuffer();
	BuildConstantBufferView();
}

void Shapes::BuildPipelineStateObject()
{
	// 图形管线状态（PSO）描述符
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

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
	psoDesc.InputLayout = InputLayout();

	// ？ - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// 图元拓扑类型 - D3D12_PRIMITIVE_TOPOLOGY_TYPE
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// RTV
	psoDesc.NumRenderTargets = FrameCount;
	for (int i = 0; i < FrameCount; i++)
	{
		psoDesc.RTVFormats[i] = m_RenderTargetFormat;
	}

	/*
	D3D12 ERROR: ID3D12CommandList::DrawIndexedInstanced:
	The depth stencil format does not match that specified by the current pipeline state.
	A null depth stencil view may only be bound when the pipeline state depth stencil format is UNKNOWN.
	(pipeline state = D24_UNORM_S8_UINT, DSV ID3D12Resource* = 0x00000000:'(nullptr)')
	[ EXECUTION ERROR #615: DEPTH_STENCIL_FORMAT_MISMATCH_PIPELINE_STATE]
	*/
	// DSV 格式
	//psoDesc.DSVFormat = m_DepthStencilFormat;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	// 采样器 - DXGI_SAMPLE_DESC
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };
	psoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;

	// ?
	psoDesc.NodeMask = 0;

	// D3D12_CACHED_PIPELINE_STATE
	psoDesc.CachedPSO = {};

	// ?
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create PSO
	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_psoMap[PsoStyle::Default])));
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_psoMap[PsoStyle::Solid])));
	}

	{
		//auto psoDesc1 = psoDesc;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_psoMap[PsoStyle::WireFrame])));
	}

	m_psoCurrent = m_psoMap[PsoStyle::Default];

	return;
}

void Shapes::BuildCommandList()
{
	ID3D12CommandAllocator* pCommandAllocator = CurrentCommandAllocator();
	ID3D12PipelineState* pCurrentPso = m_psoCurrent.Get();


	// Reset 之前要先 Close()
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(pCommandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before re-recording.
	//ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_pso.Get()));
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, pCurrentPso));

	// 按照资源用途指示其状态的转变，此处将资源从呈现状态转换为渲染状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	{
		DrawMeshData();
	}
	// 按照资源用途指示其状态的转变，此处将资源从渲染状态转换为呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());

	return;
}



// private:=============


void Shapes::UpdateProj()
{
	// 只在 OnResize() 调用
	XMMATRIX proj = XMMatrixPerspectiveFovLH((float)KMaths::PI / 4, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, proj);
}
void Shapes::UpdateCamera()
{
	//mEyePos

	XMVECTOR pos = DXUtil::SphericalToCartesian(m_Radius, m_Theta, m_Phi);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);	// 保存
}
void Shapes::UpdateObjectCBs()
{
	int objCount = m_AllRitems.size();
	auto objectCBs = m_CurrFrameResource->ObjectCBs.get();

	for (int i = 0; i < objCount; i++)
	{
		auto ri = m_AllRitems[i].get();
		if (ri->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&ri->World);

			FrameResourceShapes::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			objectCBs->CopyData(ri->ObjCBIndex, objConstants);

			// 物体世界坐标是物体相对位置，更新一次
			ri->NumFramesDirty--;
		}
	}

	return;
}
void Shapes::UpdatePassCBs()
{
	auto passCBs = m_CurrFrameResource->PassCBs.get();

	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	/*

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
	*/

	FrameResourceShapes::PassConstants passConstants;
	XMStoreFloat4x4(&passConstants.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&passConstants.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(viewProj));

	/*
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));

	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)m_ClientWidth, (float)m_ClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_ClientWidth, 1.0f / m_ClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	*/

	passCBs->CopyData(0, passConstants);
}



void Shapes::BuildBox2()
{
	m_Theta = XM_PI * (1.0f / 2);
	m_Phi = XM_PI / 3.5f;
	m_Radius = 10.0f;

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
	geo->RecordDrawArgs("box", SubmeshGeometry(indices.size(), 0, 0));

	m_Geometries["GEOs"] = std::move(geo);

	return;
}

void Shapes::BuildShapesGeometry()
{
	// 4个几何体: box,grid,sphere,cylinder
	MeshData box, grid, sphere, cylinder;
	MeshFactory::CreateBox(1.5f, 0.5f, 1.5f, 3, &box);
	MeshFactory::CreateGrid(20, 30, 60, 50, &grid);
	MeshFactory::CreateSphere(0.5f, 20, 20, &sphere);
	MeshFactory::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, &cylinder);


	// 我们将所有的几何数据连接到一个大的顶点/索引缓冲区中，
	// 要定义每个子网格覆盖的缓冲区中的区域

	// 将顶点偏移缓存到连接顶点缓冲区中的每个对象

	// 并将所有网格的顶点打包到一个顶点缓冲区中
	auto totalVertexCount =
		box.VertexCount() +
		grid.VertexCount() +
		sphere.VertexCount() +
		cylinder.VertexCount();

	std::vector<Vertex> vertices(totalVertexCount);


	int k = 0;

	// 1.box
	for (size_t i = 0; i < box.Vertices.size(); i++, k++)
	{
		vertices[k].Position = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Blue);
	}

	// 2.grid
	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	// 3.sphere
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Crimson);
	}

	// 4.cylinder
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	// 索引
	std::vector<uint32> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	// 计算索引数量
	UINT boxIndexCount = box.IndexCount();
	UINT gridIndexCount = grid.IndexCount();
	UINT sphereIndexCount = sphere.IndexCount();
	UINT cylinderIndexCount = cylinder.IndexCount();

	// 根据添加顺序计算顶点偏移
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = boxVertexOffset + box.VertexCount();
	UINT sphereVertexOffset = gridVertexOffset + grid.VertexCount();
	UINT cylinderVertexOffset = sphereVertexOffset + sphere.VertexCount();

	// 根据添加顺序计算顶点偏移
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = boxIndexOffset + box.IndexCount();
	UINT sphereIndexOffset = gridIndexOffset + grid.IndexCount();
	UINT cylinderIndexOffset = sphereIndexOffset + sphere.IndexCount();


	// 定义绘制参数
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = boxIndexCount;
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = gridIndexCount;
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = sphereIndexCount;
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = cylinderIndexCount;
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;


	// 可以使用 MeshData meshAll
	MeshData meshAll(vertices, indices);

	auto geo = std::make_unique<MeshGeometry>("shapesGeo");
	geo->Init(m_device.Get(), m_CommandList.Get(), &meshAll);

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	m_Geometries["GEOs"] = std::move(geo);

	//end
}
void Shapes::BuildRenderItems()
{
	int objCBIndex = 0;

	auto boxRitem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
	{
		XMMATRIX boxWorld = XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f);

		boxRitem->ObjCBIndex = objCBIndex++;
		XMStoreFloat4x4(&boxRitem->World, boxWorld);
		boxRitem->Geo = m_Geometries["GEOs"].get();
		boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	}
	m_AllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
	{
		gridRitem->ObjCBIndex = objCBIndex++;
		gridRitem->World = DXUtil::Identity4x4;
		gridRitem->Geo = m_Geometries["GEOs"].get();
		gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
		gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
		gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	}
	m_AllRitems.push_back(std::move(gridRitem));


	for (int i = 0; i < 5; i++)
	{
		auto leftCylRitem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
		{
			XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + 5.0f * i);

			leftCylRitem->ObjCBIndex = objCBIndex++;
			XMStoreFloat4x4(&leftCylRitem->World, leftCylWorld);	// 
			leftCylRitem->Geo = m_Geometries["GEOs"].get();
			leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
			leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
			leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
		}
		m_AllRitems.push_back(std::move(leftCylRitem));

		auto rightCylRitem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
		{
			XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + 5.0f * i);

			rightCylRitem->ObjCBIndex = objCBIndex++;
			XMStoreFloat4x4(&rightCylRitem->World, rightCylWorld);
			rightCylRitem->Geo = m_Geometries["GEOs"].get();
			rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
			rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
			rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		}
		m_AllRitems.push_back(std::move(rightCylRitem));

		auto leftSphereRitem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
		{
			XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + 5.0f * i);

			leftSphereRitem->ObjCBIndex = objCBIndex++;
			XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
			leftSphereRitem->Geo = m_Geometries["GEOs"].get();
			leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
			leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
			leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
		}
		m_AllRitems.push_back(std::move(leftSphereRitem));

		auto rightSphereRitem = std::make_unique<FrameResourceShapes::RenderItem>(m_FrameResourceCount);
		{
			XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

			rightSphereRitem->ObjCBIndex = objCBIndex++;
			XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
			rightSphereRitem->Geo = m_Geometries["GEOs"].get();
			rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
			rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
			rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		}
		m_AllRitems.push_back(std::move(rightSphereRitem));

	}//for(i)

	return;
}
void Shapes::BuildFrameResources()
{
	int passCount = 1;
	int objectCount = m_AllRitems.size();

	for (int i = 0; i < m_FrameResourceCount; ++i)
	{
		m_FrameResources.push_back
		(
			std::make_unique<FrameResourceShapes>(
				m_device.Get(),
				passCount,
				objectCount)
		);
	}
}

void Shapes::BuildConstantBuffer()
{
	// 帧资源数量
	int frameResCount = m_FrameResources.size();

	// 过程对象数量(每个帧资源有1个过程常量)
	int passCount = 1;

	// 绘制对象数量(每个帧资源有n个对象常量)
	int objectCount = m_AllRitems.size();

	// 第二个常量寄存器对应常量缓冲区的偏移地址
	m_PassCbvOffset = objectCount * frameResCount;

	// 描述符总数
	// 每个 obj 需要一个 Descriptor
	// + 1个 passCB
	UINT numDescriptors = (passCount + objectCount) * frameResCount;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	{
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.NumDescriptors = numDescriptors;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
	}

	// ID3D12DescriptorHeap - ConstantBuffer
	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&m_cbvHeap)));

	return;
}
void Shapes::BuildConstantBufferView()
{
	// 帧资源数量
	int frameResCount = m_FrameResources.size();

	// 对象数量
	UINT objCount = (UINT)m_AllRitems.size();

	// 常量缓冲区字节大小
	UINT objectCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::ObjectConstants));

	// Need a CBV descriptor for each object for each frame resource.
	for (int frmIndex = 0; frmIndex < frameResCount; frmIndex++)
	{
		auto objectCBs = m_FrameResources[frmIndex]->ObjectCBs->Resource();

		// 为每个对象创建一个常量缓冲区视图
		for (UINT i = 0; i < objCount; i++)
		{
			// 计算 ith 缓冲区地址
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCBs->GetGPUVirtualAddress();
			cbAddress += objectCBByteSize * i;

			// 常量缓冲区描述符
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objectCBByteSize;

			// 计算描述符句柄
			int heapIndex = frmIndex * objCount + i;
			D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
			cbvHandle.ptr += m_cbvDescriptorSize * heapIndex;

			// 创建常量缓冲区视图（CBV）
			m_device->CreateConstantBufferView(&cbvDesc, cbvHandle);

		}//i


	}// frmIndex


	UINT passCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceShapes::PassConstants));
	for (int frmIndex = 0; frmIndex < frameResCount; frmIndex++)
	{
		//auto passCB = m_PassCBs->Resource();
		auto passCB = m_FrameResources[frmIndex]->PassCBs->Resource();

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();
		cbAddress += passCBByteSize * frmIndex;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		// Offset to the pass cbv in the descriptor heap.
		int heapIndex = m_PassCbvOffset + frmIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, m_cbvDescriptorSize);

		m_device->CreateConstantBufferView(&cbvDesc, handle);
	}

	return;
}


void Shapes::DrawMeshData()
{
	// 设置根描述表
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	// 视图和剪切矩形
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// 输出合并
	m_CommandList->OMSetRenderTargets(1, &RenderTargetView(), FALSE, nullptr);

	// 设置描述符堆
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// 清除渲染背景
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(RenderTargetView(), clearColor, 0, nullptr);

	// 清除深度模板视图
	auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
	m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

	int b1 = 1;	// 对应 m_PassCBs
	int cbvIndex_b1 = m_PassCbvOffset + m_CurrFrameResourceIndex;
	D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle_b1 = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
	cbvHandle_b1.ptr += m_cbvDescriptorSize * cbvIndex_b1;
	m_CommandList->SetGraphicsRootDescriptorTable(b1, cbvHandle_b1);	// b1的描述符表

	int objCount = m_AllRitems.size();
	for (int i = 0; i < objCount; i++)
	{
		auto ri = m_AllRitems[i].get();

		int b0 = 0;	// 对应 m_ObjectCBs
		int cbvIndex_b0 = ri->ObjCBIndex;
		D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle_b0 = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
		cbvHandle_b0.ptr += m_cbvDescriptorSize * cbvIndex_b0;
		m_CommandList->SetGraphicsRootDescriptorTable(b0, cbvHandle_b0);	// b0的描述符表

		m_CommandList->IASetPrimitiveTopology(ri->PrimitiveType);
		m_CommandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());	// ok
		m_CommandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());

		// 绘制
		m_CommandList->DrawIndexedInstanced(
			ri->IndexCount,			// 索引数量
			1,						// 实例数量
			ri->StartIndexLocation,	// 索引开始位置
			ri->BaseVertexLocation,	// 顶点开始位置
			0);						// 实例开始位置
	}

	return;
}



