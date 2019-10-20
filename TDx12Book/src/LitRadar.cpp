#include "stdafx.h"
#include "LitRadar.h"

LitRadar::LitRadar()
{
	m_Theta = 1.5f*XM_PI;
	m_Phi = XM_PIDIV2 - 0.1f;
	m_Radius = 3.0f;

	m_IsWireframe = false;

	m_FrameResourceIndex = 0;
	m_FrameResourceCount = 3;

	m_EyePos = { 0.0f, 0.0f, 0.0f };
}


LitRadar::~LitRadar()
{
}

//�������Ϣ��
void LitRadar::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	Dx12Book::OnMouseDown(wParam, x, y);
}
void LitRadar::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book::OnMouseUp(wParam, x, y);
}
void LitRadar::OnMouseMove(WPARAM wParam, int x, int y)
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
		m_Radius = KMaths::Clamp(m_Radius, 2.0f, 15.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}
void LitRadar::OnKeyboardInput()
{
	KClock* pClock = KApplication::GetApp()->GetClock();
	const float dt = pClock->DeltaTime();

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{

	}


	//if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	//	m_SunTheta -= 1.0f*dt;

	//if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	//	m_SunTheta += 1.0f*dt;

	//if (GetAsyncKeyState(VK_UP) & 0x8000)
	//	m_SunPhi -= 1.0f*dt;

	//if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	//	m_SunPhi += 1.0f*dt;

	//m_SunPhi = KMaths::Clamp(m_SunPhi, 0.1f, XM_PIDIV2);
}


//�������ʲ���
void LitRadar::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();

	BuildRadarGeometry();
	BuildMaterials();

	BuildRenderItems();
	BuildFrameResources();

	return;
}
void LitRadar::BuildRootSignature()
{
	// ��Ҫ3�����������������塢���ʡ����̣����ݸ���Ƶ������
	const int NUM_CB = 3;
	CD3DX12_ROOT_PARAMETER rootParameters[NUM_CB];
	{
		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsConstantBufferView(1);
		rootParameters[2].InitAsConstantBufferView(2);
	}

	//KRootParameter rootParameters[NUM_CB];
	//{
	//	rootParameters[0].InitAsConstantBufferView(0);
	//	rootParameters[1].InitAsConstantBufferView(1);
	//	rootParameters[2].InitAsConstantBufferView(2);
	//}

	// ��ǩ���Ǹ���������
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		NUM_CB,
		rootParameters,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// ʹ�õ����۴�����ǩ�����ò�ָ���ɵ���������������ɵ���������Χ��
	ComPtr<ID3DBlob> rootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
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
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void LitRadar::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("LitRadar.hlsl");
		_tstring filePath = path + _T("Assets\\") + file;

		m_Shaders["VS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "VS", "vs_5_1");
		m_Shaders["PS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "PS", "ps_5_1");
	}

	// InputLyout
	{
		// ��Ҫ����Vertex�ṹ�� hlsl �еĶ�������ṹ��������׼������Ԫ��������
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName			- ���� HLSL
			0,											// SemanticIndex		- ���� HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format				- ���� Vertex(32λ������)
			0,											// InputSlot			- �����
			Vertex::OffsetPos,							// AlignedByteOffset	- �����ֽ�ƫ��
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// InputSlotClass		- ���������
			0 },										// InstanceDataStepRate	- ʵ�����ݲ���

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
void LitRadar::BuildPipelineState()
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
		{
			//psoDesc.BlendState.AlphaToCoverageEnable = TRUE;
			//BOOL AlphaToCoverageEnable;
			//BOOL IndependentBlendEnable;
			//D3D12_RENDER_TARGET_BLEND_DESC RenderTarget[8];
		}

		// ��դ���״̬ - D3D12_RASTERIZER_DESC
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		{
			//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}

		// ���ģ��״̬ - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		{
			//psoDesc.DepthStencilState.DepthEnable = FALSE;
			//psoDesc.DepthStencilState.StencilEnable = FALSE;
		}

		// ���벼�� - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };

		// todo:��������������ֵ - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// ͼԪ�������� - D3D12_PRIMITIVE_TOPOLOGY_TYPE
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// ��Ⱦ��ͼ��ʽ - RTV��DXGI_FORMAT_R8G8B8A8_UNORM
		psoDesc.NumRenderTargets = BackBufferCount;
		for (int i = 0; i < BackBufferCount; i++)
		{
			psoDesc.RTVFormats[i] = m_BackBufferFormat;
		}

		// ���ģ���ʽ - DSV��DXGI_FORMAT_D24_UNORM_S8_UINT
		psoDesc.DSVFormat = m_DepthStencilFormat;

		// ������ - DXGI_SAMPLE_DESC
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };
		psoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;

		// �ڵ�����
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
void LitRadar::BuildRadarGeometry()
{
	//m_Theta = 1.5f*XM_PI;
	//m_Phi = XM_PIDIV2 - 0.1f;
	//m_Radius = 3.0f;

	MeshData meshRadar;

	bool output = true;
	//bool output = false;

	_tstring rfile(_T("d:\\sfxData\\R3Data\\r050.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r100.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r150.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r200.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r250.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r300.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r350.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r400.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r450.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r500.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r550.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r600.txt"));
	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r650.txt"));


	if(output)
	{
		// װ������
		//d:\\sfxData\\R3Data\\mhd
		//LoadSlicerFromFiles16(_T("D:\\sfxData\\R3Data\\Radar\\rad3d\\rad"), 1, 24, 68, 52);
		LoadSlicerFromFiles16(_T("d:\\sfxData\\R3Data\\mhd\\rad.mhd"));

		vector<double> contourValues = { 450, 500 };
		//vector<double> contourValues = { 450,470,490, 500 };
		//vector<double> contourValues = { 150, 350 };

		//vector<double> contourValues = { 150, 550, 600, 650 };
		//vector<double> contourValues = { 500, 550, 600, 650 };
		//vector<double> contourValues = { 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 350, 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 300, 350, 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 250, 300, 350, 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };
		//vector<double> contourValues = { 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };

		// ��ά�ع�
		RebuildRadar3d(&m_Slices, contourValues);

		// ������������
		GenerateMesh(&meshRadar);

		// �����������
		_tstring outfile = rfile;
		//OutputMesh(outfile, &meshRadar);
	}

	// ��������ļ�
	if (!output)
	{
		_tstring infile = rfile;
		InputMesh(rfile, &meshRadar);
	}


	// MeshGeometry
	_astring meshName = "radar";
	auto geo = std::make_unique<MeshGeometry>();
	{
		geo->Init(m_device.Get(), m_CommandList.Get(), &meshRadar);
		geo->DrawArgs[meshName] = SubmeshGeometry(meshRadar.IndexCount(), 0, 0);
	}
	m_Geometries["radarGeo"] = std::move(geo);

	return;
}
void LitRadar::BuildMaterials()
{
	int index = 0;

	// �ݵز���
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
		//grass->Roughness = 0.125f;
		grass->Roughness = 0.853f;
	}
	m_Materials["grass"] = std::move(grass);

	// This is not a good water material definition, but we do not have all the rendering
	// tools we need (transparency, environment reflection), so we fake(α��) it for now.

	// ˮ�����
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
void LitRadar::BuildRenderItems()
{
	assert(m_FrameResourceCount > 0);

	m_AllRitems.clear();

	int index = 0;

	auto radarRitem = std::make_unique<FrameResourceLir::RenderItem>(m_FrameResourceCount);
	{
		radarRitem->ObjCBIndex = index++;
		radarRitem->World = DXUtil::Identity4x4;
		radarRitem->Mat = m_Materials["grass"].get();
		radarRitem->Geo = m_Geometries["radarGeo"].get();

		radarRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// ���Ʋ���
		radarRitem->IndexCount = radarRitem->Geo->DrawArgs["radar"].IndexCount;
		radarRitem->StartIndexLocation = radarRitem->Geo->DrawArgs["radar"].StartIndexLocation;
		radarRitem->BaseVertexLocation = radarRitem->Geo->DrawArgs["radar"].BaseVertexLocation;
	}
	m_AllRitems.push_back(std::move(radarRitem));

	return;
}
void LitRadar::BuildFrameResources()
{
	for (int i = 0; i < m_FrameResourceCount; ++i)
	{
		m_FrameResources.push_back
		(
			std::make_unique<FrameResourceLir>(m_device.Get(),
				1,							// pass count
				(UINT)m_AllRitems.size(),	// obj count
				(UINT)m_Materials.size())	// mat count
		);
	}
}


//�����ڴ�С�ı䡿
void LitRadar::Resized()
{
	// ����ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

//���������ݡ�
void LitRadar::Update()
{
	CircularFrameResource();

	OnKeyboardInput();
	UpdateCamera();
	UpdateObjectCB();
	UpdatePassCB();
	UpdateMaterialCB();
}
void LitRadar::UpdateCamera()
{
	// Convert Spherical to Cartesian coordinates.
	m_EyePos.x = m_Radius * sinf(m_Phi)*cosf(m_Theta);
	m_EyePos.z = m_Radius * sinf(m_Phi)*sinf(m_Theta);
	m_EyePos.y = m_Radius * cosf(m_Phi);

	// ������ͼ����
	XMVECTOR pos = XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// y������

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);
}
void LitRadar::UpdateObjectCB()
{
	auto currObjectCB = CurrentFrameResource()->ObjectCB.get();

	for (auto& e : m_AllRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->Dirtys > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			FrameResourceLir::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->Dirtys--;
		}
	}
}
void LitRadar::UpdatePassCB()
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

	FrameResourceLir::PassConstants passCB;
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

		passCB.AmbientLight = { 0.25f, 0.25f, 0.25f, 1.0f };


		float sss = 0.8f;
		float clr = 0.6f;
		for (int i = 0; i < 6; i++) { passCB.Lights[i].Strength = { sss, sss, sss }; }

		passCB.Lights[0].Direction = { clr, 0.0f, 0.0f };
		passCB.Lights[1].Direction = { 0.0f, clr,0.0f };
		passCB.Lights[2].Direction = { 0.0f, 0.0f, clr };
		passCB.Lights[3].Direction = { -clr, 0.0f, 0.0f };
		passCB.Lights[4].Direction = { 0.0f, -clr, 0.0f };
		passCB.Lights[5].Direction = { 0.0f, 0.0f, -clr };
	}

	auto currPassCB = CurrentFrameResource()->PassCB.get();
	currPassCB->CopyData(0, passCB);

	return;
}
void LitRadar::UpdateMaterialCB()
{
	auto currMaterialCB = CurrentFrameResource()->MaterialCB.get();
	for (auto& e : m_Materials)
	{
		Material* mat = e.second.get();

		if (mat->Dirtys > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			FrameResourceLir::MaterialConstants matConstants;
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


// ����Ⱦ���ơ�
void LitRadar::Draw()
{
	// ����׼��
	auto pCommandAllocator = CurrentFrameResource()->CmdListAllocator.Get();
	auto pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// ��λ������������������������е������¼
	ThrowIfFailed(pCommandAllocator->Reset());

	// ��λ�����б�����������¼������pCommandAllocator�У�
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, pCurrentPso));

	{
		// ��Դ���� - �ɳ���ת��Ϊ��Ⱦ״��
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// ��դ���׶�
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// ���װ��
		m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// �����Ⱦ����
		//const float* clearColor = Colors::LightSkyBlue;
		const float* clearColor = Colors::Black;
		m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// ������ģ����ͼ
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		{
			// ���ø�ǩ��
			m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

			// �󶨹�����Դ������������
			auto passCB = CurrentFrameResource()->PassCB->Resource();
			m_CommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());	// b2

			DrawRenderItems();
		}

		// ��Դ���� - ����Ⱦת��Ϊ����״̬
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	}

	// ��ɼ�¼����
	ThrowIfFailed(m_CommandList->Close());

	{
		// �������б�����������
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// ����ǰ��̨������
		ThrowIfFailed(m_SwapChain->Present(1, 0));
		m_BackBufferIndex = (m_BackBufferIndex + 1) % BackBufferCount;

		// ��Χ��ֵ��ǰ�ƽ����������ǵ���Χ����
		CurrentFrameResource()->FenceValue = ++m_FenceValue;

		// ��������������һ��ָ��������µ�Χ���㡣
		// ��Ϊ������GPUʱ�����ϣ���GPU������֮ǰ����������֮ǰ��
		// ��Signal()�����������µ�Χ���㡣
		m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
	}

	return;
}
void LitRadar::DrawRenderItems()
{
	UINT objCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceLir::ObjectConstants));
	UINT matCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceLir::MaterialConstants));

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

		// ����װ��
		m_CommandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		m_CommandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		m_CommandList->IASetPrimitiveTopology(ri->PrimitiveType);

		// ����
		m_CommandList->DrawIndexedInstanced(
			ri->IndexCount,			// ��������
			1,						// ʵ������
			ri->StartIndexLocation,	// ������ʼλ��
			ri->BaseVertexLocation,	// ���㿪ʼλ��
			0);						// ʵ����ʼλ��
	}

	return;
}

void LitRadar::DoAppIdle()
{
	Dx12Book::DoAppIdle();
}


//������������
bool LitRadar::LoadSlicerFromFiles16(_tstring filePrefix, int indexBeg, int indexEnd, int width, int height)
{
	m_Slices.clear();

	int nSliceCount = indexEnd - indexBeg + 1;		// ��Ƭ����
	int nSliceSize = width * height;				// ��Ƭ��С
	int nBufferSize = nSliceSize * sizeof(uint16);	// ��������С(�ֽ�)

	for (int i = 0; i < nSliceCount; i++)
	{
		int n = i + indexBeg;
		_tstring finalPath = filePrefix + _T(".") + _ttostring(n);

		ifstream fin(finalPath, ios::binary);
		if (!fin.is_open())
		{
			std::cout << "File Load Error!! :  " << KString::AString(finalPath.c_str()) << std::endl;
			return false;
		}

		TSlice<uint16> slice(width, height);
		slice.ElementArray.resize(nBufferSize);
		fin.read(reinterpret_cast<char*>(slice.ElementArray.data()), nBufferSize);
		m_Slices.push_back(slice);

		//�ر��ļ�
		fin.close();
		std::cout << "File Loaded OK: " << KString::AString(finalPath.c_str()) << std::endl;
	}

	return true;
}

bool LitRadar::LoadSlicerFromFiles16(_tstring fileName)
{
	KString mhdFile = fileName;
	KString rawFile = KString::Replace(mhdFile, _T(".mhd"), _T(".raw"));

	LoadSlicerFromFiles16_mhd(_tstring(mhdFile));
	LoadSlicerFromFiles16_raw((_tstring)rawFile);

	return true;
}
void LitRadar::LoadSlicerFromFiles16_mhd(_tstring fileName)
{
	_astring ignore_str;
	int n;
	int dim[3];
	float elementSize[3];
	float elememtSpace[3];
	_astring elementType;
	_astring elementByteOrderMSB;
	_astring elementDataFile;
	float lon, lat;
	float xInterval, yInterval;

	// �����������ڴ����룩
	ifstream fin(fileName);
	{
		fin >> ignore_str >> ignore_str >> n;														// NDims = 3
		fin >> ignore_str >> ignore_str >> dim[0] >> dim[1] >> dim[2];								// DimSize = 65 69 24
		fin >> ignore_str >> ignore_str >> elementSize[0] >> elementSize[1] >> elementSize[2];		// ElementSize = 1.0 1.0 1.0
		fin >> ignore_str >> ignore_str >> elememtSpace[0] >> elememtSpace[1] >> elememtSpace[2];	// ElementSpacing = 1.0 1.0 1.0
		fin >> ignore_str >> ignore_str >> elementType;												// ElementType = MET_SHORT
		fin >> ignore_str >> ignore_str >> elementByteOrderMSB;										// ElementByteOrderMSB = False
		fin >> ignore_str >> ignore_str >> elementDataFile;											// ElementDataFile = rad.raw
		fin >> ignore_str >> ignore_str >> lon >> lat;												// LL1 = 111.494918823242 35.0724334716797
		fin >> ignore_str >> ignore_str >> xInterval >> yInterval;									// LL2 = 0.009876986621 0.009876986621
	}
	fin.close();

	for (int i = 0; i < 3; i++)
	{
		m_xinfo.dim[i] = dim[i];
		m_xinfo.elementSize[i] = elementSize[i];
		m_xinfo.elememtSpace[i] = elememtSpace[i];
	}
	m_xinfo.lon0 = lon;
	m_xinfo.lat0 = lat;
	m_xinfo.xInterval = xInterval;
	m_xinfo.yInterval = yInterval;

	return;
}
void LitRadar::LoadSlicerFromFiles16_raw(_tstring fileName)
{
	m_Slices.clear();

	int nSliceWidth = m_xinfo.dim[0];		// 57
	int nSliceHeight = m_xinfo.dim[1];		// 47
	int nSliceCount = m_xinfo.dim[2];		// ��Ƭ���� 24

	int nSliceSize = nSliceWidth * nSliceHeight;				// ��Ƭ��С
	int nBufferSize = nSliceSize * sizeof(uint16);	// ��������С(�ֽ�)


	ifstream fin(fileName, ios::binary);
	bool b = fin.is_open();

	for (int i = 0; i < nSliceCount; i++)
	{
		TSlice<uint16> slice(nSliceWidth, nSliceHeight);
		slice.ElementArray.resize(nBufferSize);
		fin.read(reinterpret_cast<char*>(slice.ElementArray.data()), nBufferSize);
		m_Slices.push_back(slice);
	}

	fin.close();

	// ����ÿ��ͼ��
	for (int i = 0; i < nSliceCount; i++)
	{
		TSlice<uint16> slice = m_Slices[i];

		uint16 v = 0;
		for (int j = 0; j < nBufferSize; j++)
		{
			v += slice.ElementArray[j];
		}

		KUtil::Trace(KString::Format(_T("%d = %d"), i, v));
	}

	return;
}


void LitRadar::RebuildRadar3d(vector<TSlice<uint16>>* pSlices, const vector<double>& contourValues)
{
	// dims - 68,52,24
	int x = pSlices->at(0).Width;
	int y = pSlices->at(0).Height;
	int z = pSlices->size();

	int dims[3] = { x, y, z };

	// scalars
	std::vector<double> scalars;
	for (int k = 0; k < z; k++)
	{
		auto slice = m_Slices[k];
		for (int j = 0; j < y; j++)
		{
			for (int i = 0; i < x; i++)
			{
				double s = slice.GetValue(i, j);
				scalars.push_back(s);
			}
		}
	}

	// origin
	double origin[3] = { 0, 0, 0 };

	// spacing
	m_xinfo.elememtSpace[0];
	//double spacing[3] = { 0.10, 0.10, 0.10 };
	double spacing[3] = { 1.0, 1.0, 1.0 };


	// values
	//contourValues

	KMarchingCubes* pmc = &m_KMarchingCubes;
	pmc->Clear();
	pmc->Building_0(scalars.data(), dims, origin, spacing, (double*)contourValues.data(), contourValues.size());

	return;
}
void LitRadar::GenerateMesh(MeshData* pMeshData)
{
	KMarchingCubes* pmc = &m_KMarchingCubes;

	// Vertices - ����
	std::vector<Vector3> vertices;
	pmc->GetVertices(vertices);

	// Indices - ����
	std::vector<uint32> indices;
	pmc->GetIndices(indices);

	// Normals - ����
	std::vector<Vector3> normals;
	pmc->GetNormals(normals);

	// VertexValues - ����ֵ
	std::vector<double> vertexValues;
	pmc->GetVertexValues(vertexValues);


	// ��������
	float r, g, b;
	int nCount = (int)vertices.size();

	for (int i = 0; i < nCount; i++)
	{
		Vector3 v = vertices[i];
		//XMFLOAT3 pnt = { (float)v.x, (float)-v.y, (float)v.z };
		XMFLOAT3 pnt = { (float)v.x, (float)-v.z, (float)-v.y };

		Vector3 n = normals[i];
		XMFLOAT3 nom = { (float)n.x, (float)n.y, (float)n.z };

		float sv = (float)vertexValues[i];	// ����ֵ
		//if (sv > 200)
		{
			GetRadarColor(sv, r, g, b);
			XMFLOAT4 clr = { r, g, b, 1.0f };

			pMeshData->vertex_push_bak(Vertex(pnt, nom, clr));
			pMeshData->index_push_back(i);
		}
	}

	return;
}
void LitRadar::OutputMesh(_tstring fileName, const MeshData* pMeshData)
{
	ofstream out(fileName);
	if (out.fail())
		return;

	/*
	Points 34157
	Normals 34157
	Colors 34157
	Triangles 34157

	ÿ���ļ��洢��һ��dbzֵ����Ƭ����
	����r150.txt������ dbz=15 ����Ƭ���ݣ��Դ����ơ�

	�ļ����ݰ������ʽ��˳��洢��

	1���������꣺p(x��y��z),ÿ�д洢һ������
	Points 240
	x0 y0 z0
	...

	2�����㷨�ߣ�n(x��y��z),ÿ�д洢һ��������
	Normals 240
	x0 y0 z0
	...

	3��������ɫ��c(r��g��b),ÿ�д洢һ����ɫ����
	Colors 240
	ro g0 b0
	...

	4��������������(idx0 idx1 idx2)����������˳ʱ�����򣩾���һ��������
	Triangles: 80
	idx0 idx1 idx2
	...

	*/

	int iVertexCount = pMeshData->VertexCount();
	int IndexCount = pMeshData->IndexCount();

	string _blank_space_ = " ";

	out << "Points " << iVertexCount << endl;
	for (int i = 0; i < iVertexCount; i++)
	{
		Vertex vertex = pMeshData->Vertices[i];
		XMFLOAT3 vec = vertex.Position;
		out << vec.x << _blank_space_ << vec.y << _blank_space_ << vec.z << endl;
	}

	out << "Normals " << iVertexCount << endl;
	for (int i = 0; i < iVertexCount; i++)
	{
		Vertex vertex = pMeshData->Vertices[i];
		XMFLOAT3 vec = vertex.Normal;
		out << vec.x << _blank_space_ << vec.y << _blank_space_ << vec.z << endl;
	}

	out << "Colors " << iVertexCount << endl;
	for (int i = 0; i < iVertexCount; i++)
	{
		Vertex vertex = pMeshData->Vertices[i];
		XMFLOAT4 vec = vertex.Color;
		out << vec.x << _blank_space_ << vec.y << _blank_space_ << vec.z << endl;
	}

	uint32 idx[3];
	out << "Triangles " << IndexCount / 3 << endl;
	for (int i = 0; i < iVertexCount; i+=3)
	{
		idx[0] = pMeshData->Indices32[i + 0];
		idx[1] = pMeshData->Indices32[i + 1];
		idx[2] = pMeshData->Indices32[i + 2];

		out << idx[0] << _blank_space_ << idx[1] << _blank_space_ << idx[2] << endl;
	}


	out.close();
	return;
}
void LitRadar::InputMesh(_tstring fileName, MeshData* pMeshData)
{
	ifstream fin(fileName);
	if (fin.fail())
		return;

	/*
	Points 71562
	Normals 71562
	Colors 71562
	Triangles 23854
	*/

	int nCount = 0;
	_astring ignore_str;

	vector<XMFLOAT3> points;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT4> colors;

	vector<Vertex> vertices;
	vector<uint32> indices32;

	// Points 71562
	// 0.117647 -0.426117 -0.480769
	fin >> ignore_str >> nCount;
	for (int i = 0; i < nCount; i++)
	{
		XMFLOAT3 vec;
		fin >> vec.x >> vec.y >> vec.z;
		points.push_back(vec);
	}

	// Normals 71562
	// -0.303774 -0.00379718 -0.952737
	fin >> ignore_str >> nCount;
	for (int i = 0; i < nCount; i++)
	{
		XMFLOAT3 vec;
		fin >> vec.x >> vec.y >> vec.z;
		normals.push_back(vec);
	}

	// Colors 71562
	// 0 0.917647 0
	fin >> ignore_str >> nCount;
	for (int i = 0; i < nCount; i++)
	{
		XMFLOAT4 vec(0, 0, 0, 1);
		fin >> vec.x >> vec.y >> vec.z;
		colors.push_back(vec);
	}

	//Triangles 23854
	// 0 1 2
	// 3 4 5
	fin >> ignore_str >> nCount;
	for (int i = 0; i < nCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			uint32 idx = 0;
			fin >> idx;
			indices32.push_back(idx);
		}
	}

	// ��������
	nCount = points.size();
	for (int i = 0; i < nCount; i++)
	{
		XMFLOAT3 pnt = points[i];
		XMFLOAT3 nom = normals[i];
		XMFLOAT4 clr = colors[i];

		Vertex vertex = { pnt, nom, clr };
		vertices.push_back(vertex);
	}

	//MeshData
	*pMeshData = MeshData(vertices, indices32);


	fin.close();
	return;
}

void LitRadar::GetRadarColor(float v, float& r, float& g, float& b)
{
	r = (float)0xC0 / 0xFF;		g = (float)0xC0 / 0xFF;		b = (float)0xFE / 0xFF;

	if (false) {}
	else if (v < 50)
	{
		// 5	122	114	238 => ()
		r = (float)0x7A / 0xFF;		g = (float)0x72 / 0xFF;		b = (float)0xEE / 0xFF;
	}
	else if (v < 100)
	{
		// 10	30	30	208
		r = (float)0x1E / 0xFF;		g = (float)0x1E / 0xFF;		b = (float)0xD0 / 0xFF;
	}
	else if (v < 150)
	{
		// 15	166	252	168
		r = (float)0xA6 / 0xFF;		g = (float)0xFC / 0xFF;		b = (float)0xA8 / 0xFF;
	}
	else if (v < 200)
	{
		// 20	0	234	0
		r = (float)0x00 / 0xFF;		g = (float)0xEA / 0xFF;		b = (float)0x00 / 0xFF;
	}
	else if (v < 250)
	{
		// 25	16	146	26
		r = (float)0x10 / 0xFF;		g = (float)0x92 / 0xFF;		b = (float)0x1A / 0xFF;
	}
	else if (v < 300)
	{
		// 30	252	244	100
		r = (float)0xFC / 0xFF;		g = (float)0xF4 / 0xFF;		b = (float)0x64 / 0xFF;
	}
	else if (v < 350)
	{
		// 35	200	200	2
		r = (float)0xC8 / 0xFF;		g = (float)0xC8 / 0xFF;		b = (float)0x02 / 0xFF;
	}
	else if (v < 400)
	{
		// 40	140	140	0
		r = (float)0x8C / 0xFF;		g = (float)0x8C / 0xFF;		b = (float)0x00 / 0xFF;
	}
	else if (v < 450)
	{
		// 45	254	172	172
		r = (float)0xFE / 0xFF;		g = (float)0xAC / 0xFF;		b = (float)0xAC / 0xFF;
	}
	else if (v < 500)
	{
		// 50	254	100	92
		r = (float)0xFE / 0xFF;		g = (float)0x64 / 0xFF;		b = (float)0x5C / 0xFF;
	}
	else if (v < 550)
	{
		// 55	238	2	48
		r = (float)0xEE / 0xFF;		g = (float)0x02 / 0xFF;		b = (float)0x30 / 0xFF;
	}
	else if (v < 600)
	{
		// 60	212	142	254
		r = (float)0xD4 / 0xFF;		g = (float)0x8E / 0xFF;		b = (float)0xFE / 0xFF;
	}
	else if (v < 650)
	{
		// 65	170	36	250
		r = (float)0xAA / 0xFF;		g = (float)0x24 / 0xFF;		b = (float)0xFA / 0xFF;
	}
	else if (v < 700)
	{
		r = (float)0xFE / 0xFF;		g = (float)0xFE / 0xFF;		b = (float)0x00 / 0xFF;
	}
	else
	{
		r = (float)0xC0 / 0xFF;		g = (float)0xC0 / 0xFF;		b = (float)0xFE / 0xFF;
	}

	return;
}
