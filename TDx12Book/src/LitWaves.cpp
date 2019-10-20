#include "stdafx.h"
#include "LitWaves.h"


LitWaves::LitWaves()
{
	m_Theta = 1.5f*XM_PI;
	m_Phi = XM_PIDIV2 - 0.1f;
	m_Radius = 50.0f;

	m_IsWireframe = false;

	m_FrameResourceCount = 3;
}


LitWaves::~LitWaves()
{
}



//【鼠标消息】 - Mouse
void LitWaves::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}
	
	Dx12Book::OnMouseDown(wParam, x, y);
}
void LitWaves::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book::OnMouseUp(wParam, x, y);
}
void LitWaves::OnMouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta -= dx;
		m_Phi -= dy;

		// Restrict the angle m_Phi.
		m_Phi = KMaths::Clamp(m_Phi, 0.1f, (float)KMaths::PI - 0.1f);
		//m_Phi = KMaths::Clamp(m_Phi, 0.1f, (float)KMaths::PI/3);
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


//【构造资产】 - Build
void LitWaves::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();

	BuildLandGeometry();
	BuildWavesGeometry();
	BuildMaterials();

	BuildRenderItems();
	BuildFrameResources();

	return;
}
void LitWaves::BuildRootSignature()
{
	const int NUM_CB = 3;

	// 需要3个常量缓冲区：物体、材质、过程（依据更新频率排序）
	CD3DX12_ROOT_PARAMETER slotRootParameter[NUM_CB];
	{
		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsConstantBufferView(2);
	}

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		NUM_CB,
		slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// 使用单个槽创建根签名，该槽指向由单个常量缓冲区组成的描述符范围。
	ComPtr<ID3DBlob> rootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		rootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_device->CreateRootSignature(
		0,
		rootSig->GetBufferPointer(),
		rootSig->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

	return;
}
void LitWaves::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("LitWaves.hlsl");
		_tstring filePath = path + _T("Assets\\") + file;

		m_Shaders["VS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "VS", "vs_5_0");
		m_Shaders["PS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "PS", "ps_5_0");
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
void LitWaves::BuildPipelineState()
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
		{
			//psoDesc.BlendState.AlphaToCoverageEnable = TRUE;
			//BOOL AlphaToCoverageEnable;
			//BOOL IndependentBlendEnable;
			//D3D12_RENDER_TARGET_BLEND_DESC RenderTarget[8];
		}

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
void LitWaves::BuildLandGeometry()
{
	_astring meshName = "grid";

	MeshData grid;
	MeshFactory::CreateGrid(160, 160, 50, 50, &grid);

	// 提取我们感兴趣的顶点元素，并将高度函数应用于每个顶点。
	// 此外，根据顶点的高度给顶点涂上颜色，
	// 这样我们就可以看到沙滩、青草低矮的小山和雪山山峰。
	for (size_t i = 0; i < grid.VertexCount(); i++)
	{
		Vertex* pVertex = &grid.Vertices[i];

		// 高度函数应用于每个顶点
		pVertex->Position.y = GetHillsHeight(pVertex->Position.x, pVertex->Position.z);

		// 计算顶点法线
		pVertex->Normal = GetHillsNormal(pVertex->Position.x, pVertex->Position.z);

		pVertex->Color = XMFLOAT4(0.9f, 0.1f, 0.0f, 1.0f);
	}

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = grid.IndexCount();
	gridSubmesh.StartIndexLocation = 0;
	gridSubmesh.BaseVertexLocation = 0;

	auto geo = std::make_unique<MeshGeometry>();
	geo->Init(m_device.Get(), m_CommandList.Get(), &grid);
	geo->DrawArgs[meshName] = SubmeshGeometry(grid.IndexCount(), 0, 0);

	m_Geometries["landGeo"] = std::move(geo);

	return;
}
void LitWaves::BuildWavesGeometry()
{
	_astring meshName = "grid";

	//Waves(int rows, int cols, float dx, float dt, float speed, float damping);
	m_Waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	// 3 indices per face
	std::vector<uint32> indices(m_Waves->TriangleCount() * 3);

	// ？？？
	assert(m_Waves->VertexCount() < 0x0000ffff);

	// Iterate over each quad.
	int k = 0;
	int m = m_Waves->RowCount();
	int n = m_Waves->ColCount();
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k + 0] = (i + 0)*n + j;
			indices[k + 1] = (i + 0)*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = (i + 0)*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	std::vector<Vertex> vertices(m_Waves->VertexCount());
	for (int i = 0; i < m_Waves->VertexCount(); i++)
	{
		XMFLOAT3 pos = m_Waves->Position(i);
		vertices[i] = Vertex(pos);
	}

	// 网格数据
	MeshData mesh(vertices, indices);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Init(m_device.Get(), m_CommandList.Get(), &mesh);
	geo->DrawArgs[meshName] = SubmeshGeometry(mesh.IndexCount(), 0, 0);

	// Set dynamically.(动态波浪)
	//geo->VertexBufferCPU = nullptr;
	//geo->VertexBufferGPU = nullptr;

	m_Geometries["waterGeo"] = std::move(geo);

	return;
}
void LitWaves::BuildMaterials()
{
	int index = 0;

	// 草地材质
	auto grass = std::make_unique<Material>(m_FrameResourceCount);
	{
		XMFLOAT4 Grass = XMFLOAT4(0.2f, 0.6f, 0.2f, 1.0f);
		XMFLOAT4 SandyBeach = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		XMFLOAT4 LightYellowGreen = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		XMFLOAT4 DarkYellowGreen = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		XMFLOAT4 Darkbrown = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		XMFLOAT4 WhiteSnow = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		grass->Name = "grass";
		grass->MatCBIndex = index++;

		grass->DiffuseAlbedo = Grass;
		grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
		grass->Roughness = 0.125f;
	}
	m_Materials["grass"] = std::move(grass);

	// This is not a good water material definition, but we do not have all the rendering
	// tools we need (transparency, environment reflection), so we fake(伪造) it for now.

	// 水体材质
	auto water = std::make_unique<Material>(m_FrameResourceCount);
	{
		water->Name = "water";
		water->MatCBIndex = index++;
		water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
		water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
		water->Roughness = 0.0f;
	}
	m_Materials["water"] = std::move(water);

	return;
}
void LitWaves::BuildRenderItems()
{
	assert(m_FrameResourceCount > 0);

	int index = 0;

	auto wavesRitem = std::make_unique<FrameResourceLiw::RenderItem>(m_FrameResourceCount);
	{
		wavesRitem->ObjCBIndex = index++;
		wavesRitem->World = DXUtil::Identity4x4;
		wavesRitem->Mat = m_Materials["water"].get();
		wavesRitem->Geo = m_Geometries["waterGeo"].get();
		wavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
		wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
		wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	}
	m_WavesRitem = wavesRitem.get();
	m_AllRitems.push_back(std::move(wavesRitem));

	auto gridRitem = std::make_unique<FrameResourceLiw::RenderItem>(m_FrameResourceCount);
	{
		gridRitem->ObjCBIndex = index++;
		gridRitem->World = DXUtil::Identity4x4;
		gridRitem->Mat = m_Materials["grass"].get();
		gridRitem->Geo = m_Geometries["landGeo"].get();
		gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
		gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
		gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	}
	m_AllRitems.push_back(std::move(gridRitem));

	return;
}
void LitWaves::BuildFrameResources()
{
	for (int i = 0; i < m_FrameResourceCount; ++i)
	{
		m_FrameResources.push_back
		(
			std::make_unique<FrameResourceLiw>(m_device.Get(),
				1,
				(UINT)m_AllRitems.size(),
				(UINT)m_Materials.size(),
				(UINT)m_Waves->VertexCount())
		);
	}
}

//【窗口改变】 - Resized
void LitWaves::Resized()
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*(float)KMaths::PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}


//【数据更新】 - Update
void LitWaves::Update()
{
	CircularFrameResource();

	OnKeyboardInput();
	UpdateCamera();
	UpdateObjectCBs();
	UpdateMaterialCBs();
	UpdatePassCB();
	UpdateWaves();
}
void LitWaves::UpdateCamera()
{
	// Convert Spherical to Cartesian coordinates.
	m_EyePos.x = m_Radius * sinf(m_Phi)*cosf(m_Theta);
	m_EyePos.z = m_Radius * sinf(m_Phi)*sinf(m_Theta);
	m_EyePos.y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);
}
void LitWaves::UpdateObjectCBs()
{
	auto currObjectCB = CurrentFrameResource()->ObjectCB.get();
	for (auto& e : m_AllRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->Dirtys > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			FrameResourceLiw::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->Dirtys--;
		}
	}
}
void LitWaves::UpdateMaterialCBs()
{
	auto currMaterialCB = CurrentFrameResource()->MaterialCB.get();
	for (auto& e : m_Materials)
	{
		Material* mat = e.second.get();

		if (mat->Dirtys > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			FrameResourceLiw::MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, matTransform);

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			// Next FrameResource need to be updated too.
			mat->Dirtys--;
		}
	}
}
void LitWaves::UpdatePassCB()
{
	KClock* pClock = KApplication::GetApp()->GetClock();
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();


	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	FrameResourceLiw::PassConstants passCB;
	{
		XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&passCB.InvViewProj, XMMatrixTranspose(invViewProj));

		passCB.EyePosW = m_EyePos;

		passCB.RenderTargetSize = XMFLOAT2((float)iClientWidth, (float)iClientHeight);
		passCB.InvRenderTargetSize = XMFLOAT2(1.0f / iClientWidth, 1.0f / iClientHeight);

		passCB.NearZ = 1.0f;
		passCB.FarZ = 1000.0f;
		passCB.TotalTime = pClock->TotalTime();
		passCB.DeltaTime = pClock->DeltaTime();

		// 环境光
		passCB.AmbientLight = { 0.25f, 0.25f, 0.25f, 1.0f };

		// 灯管方向
		Vector3 v3 = KMaths::SphericalToCartesian(1.0f, m_SunTheta, m_SunPhi);
		XMVECTOR lightDir = XMVectorSet((float)v3.x, (float)v3.y, (float)v3.z, 1.0f);
		lightDir = -lightDir;

		XMStoreFloat3(&passCB.Lights[0].Direction, lightDir);
		passCB.Lights[0].Strength = { 1.0f, 1.0f, 1.0f };
	}

	auto currPassCB = CurrentFrameResource()->PassCB.get();
	currPassCB->CopyData(0, passCB);

	return;
}
void LitWaves::UpdateWaves()
{
	KClock* pClock = KApplication::GetApp()->GetClock();

	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((pClock->TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = KMaths::Rand(4, m_Waves->RowCount() - 5);
		int j = KMaths::Rand(4, m_Waves->ColCount() - 5);

		float r = KMaths::Rand(0.2f, 0.5f);

		m_Waves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	m_Waves->Update(pClock->DeltaTime());

	// Update the wave vertex buffer with the new solution.
	auto currWavesVB = CurrentFrameResource()->WavesVB.get();
	for (int i = 0; i < m_Waves->VertexCount(); ++i)
	{
		Vertex v;

		v.Position = m_Waves->Position(i);
		v.Normal = m_Waves->Normal(i);

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	m_WavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();
}


//【渲染绘制】 - Draw
void LitWaves::Draw()
{
	// 数据准备
	auto pCommandAllocator = CurrentFrameResource()->CmdListAllocator.Get();
	auto pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// 复位命令分配器，清空命令分配器中的命令记录
	ThrowIfFailed(pCommandAllocator->Reset());

	// 复位命令列表（后面的命令记录保存在pCommandAllocator中）
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, pCurrentPso));

	{
		// 资源屏障 - 由呈现转换为渲染状体
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// 光栅化阶段
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出装配
		m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// 清除渲染背景
		const float* clearColor = Colors::LightSkyBlue;
		m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		{
			// 设置根签名
			m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

			// 绑定过程资源到常量缓冲区
			auto passCB = CurrentFrameResource()->PassCB->Resource();
			m_CommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());	// b2

			DrawRenderItems();
		}

		// 资源屏障 - 由渲染转换为呈现状态
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	}

	// 完成记录命令
	ThrowIfFailed(m_CommandList->Close());

	{
		// 把命令列表加入命令队列
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// 交换前后台缓冲区
		ThrowIfFailed(m_SwapChain->Present(0, 0));
		m_BackBufferIndex = (m_BackBufferIndex + 1) % BackBufferCount;

		// 将围栏值向前推进，将命令标记到此围栏点
		CurrentFrameResource()->FenceValue = ++m_FenceValue;

		// 向命令队列中添加一条指令，以设置新的围栏点。
		// 因为我们在GPU时间轴上，在GPU处理完之前的所有命令之前，
		// 此Signal()将不会设置新的围栏点。
		m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
	}

	return;
}
void LitWaves::DrawRenderItems()
{
	UINT objCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceLiw::ObjectConstants));
	UINT matCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceLiw::MaterialConstants));

	auto objCB = CurrentFrameResource()->ObjectCB->Resource();
	auto matCB = CurrentFrameResource()->MaterialCB->Resource();

	for (size_t i = 0; i < m_AllRitems.size(); i++)
	{
		auto ri = m_AllRitems[i].get();

		// b0
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		// b1
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;
		m_CommandList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		// 输入装配
		m_CommandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		m_CommandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		m_CommandList->IASetPrimitiveTopology(ri->PrimitiveType);

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



//【其他事项】 - Misc
void LitWaves::OnKeyboardInput()
{
	KClock* pClock = KApplication::GetApp()->GetClock();

	const float dt = pClock->DeltaTime();

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_SunTheta -= 1.0f*dt;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_SunTheta += 1.0f*dt;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_SunPhi -= 1.0f*dt;

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_SunPhi += 1.0f*dt;

	m_SunPhi = KMaths::Clamp(m_SunPhi, 0.1f, XM_PIDIV2);
}
float LitWaves::GetHillsHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}
XMFLOAT3 LitWaves::GetHillsNormal(float x, float z) const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}


