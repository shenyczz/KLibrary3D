#include "stdafx.h"
#include "LitRadar.h"

//const _astring MatName::All = "All";
//
//const _astring MatName::RL050 = "RL050";
//const _astring MatName::RL100 = "RL100";
//const _astring MatName::RL200 = "RL200";
//



LitRadar::LitRadar()
{
	m_Theta = 1.5f*XM_PI;
	m_Phi = XM_PI / 2 + 0.3f;
	m_Radius = 3.0f;

	m_IsWireframe = false;

	m_FrameResourceIndex = 0;
	m_FrameResourceCount = 3;

	m_EyePos = { 0.0f, 0.0f, 0.0f };

	m_MatNames[RLayer::All] = "All";
	m_MatNames[RLayer::RL050] = "RL050";
	m_MatNames[RLayer::RL100] = "RL100";
	m_MatNames[RLayer::RL150] = "RL150";
	m_MatNames[RLayer::RL200] = "RL200";
	m_MatNames[RLayer::RL250] = "RL250";
	m_MatNames[RLayer::RL300] = "RL300";
	m_MatNames[RLayer::RL350] = "RL350";
	m_MatNames[RLayer::RL400] = "RL400";
	m_MatNames[RLayer::RL450] = "RL450";
	m_MatNames[RLayer::RL500] = "RL500";
	m_MatNames[RLayer::RL550] = "RL550";
	m_MatNames[RLayer::RL600] = "RL600";
	m_MatNames[RLayer::RL650] = "RL650";

	//m_DiffuseAlbedos
	m_DiffuseAlbedos[RLayer::All] = XMFLOAT4(0.999f, 0.999f, 0.999f, 1.000f);
	m_DiffuseAlbedos[RLayer::RL050] = XMFLOAT4(0.478f, 0.447f, 0.933f, 0.050f);
	m_DiffuseAlbedos[RLayer::RL100] = XMFLOAT4(0.118f, 0.118f, 0.816f, 0.100f);
	m_DiffuseAlbedos[RLayer::RL150] = XMFLOAT4(0.651f, 0.988f, 0.659f, 0.150f);
	m_DiffuseAlbedos[RLayer::RL200] = XMFLOAT4(0.000f, 0.918f, 0.000f, 0.200f);
	m_DiffuseAlbedos[RLayer::RL250] = XMFLOAT4(0.063f, 0.573f, 0.102f, 0.250f);
	m_DiffuseAlbedos[RLayer::RL300] = XMFLOAT4(0.988f, 0.957f, 0.392f, 0.300f);
	m_DiffuseAlbedos[RLayer::RL350] = XMFLOAT4(0.784f, 0.784f, 0.008f, 0.350f);
	m_DiffuseAlbedos[RLayer::RL400] = XMFLOAT4(0.549f, 0.549f, 0.000f, 0.400f);
	m_DiffuseAlbedos[RLayer::RL450] = XMFLOAT4(0.996f, 0.675f, 0.675f, 0.450f);
	m_DiffuseAlbedos[RLayer::RL500] = XMFLOAT4(0.996f, 0.392f, 0.361f, 0.500f);
	m_DiffuseAlbedos[RLayer::RL550] = XMFLOAT4(0.933f, 0.008f, 0.188f, 0.550f);
	m_DiffuseAlbedos[RLayer::RL600] = XMFLOAT4(0.831f, 0.557f, 0.996f, 0.600f);
	m_DiffuseAlbedos[RLayer::RL650] = XMFLOAT4(0.667f, 0.141f, 0.980f, 0.650f);

}


LitRadar::~LitRadar()
{
}

//【鼠标消息】
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


// BuildRadarGeometry - xxx
// BuildMaterials - xxx
// BuildRenderItems - xxx
// Draw - xxx

static int flag_radar = 1;

//【构造资产】
void LitRadar::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();

	if (flag_radar == 0)
	{
		BuildRadarGeometry_0();
		BuildMaterials_0();
		BuildRenderItems_0();
	}
	else
	{
		BuildRadarGeometry();
		BuildMaterials();
		BuildRenderItems();
	}

	BuildFrameResources();

	return;
}
void LitRadar::BuildRootSignature()
{
	// 需要3个常量缓冲区：物体、材质、过程(Once)（依据更新频率排序）
	const int NUM_CB = 3;
	CD3DX12_ROOT_PARAMETER rootParameters[NUM_CB];
	{
		// b0,b1,b2
		rootParameters[0].InitAsConstantBufferView(0);	// b0 - 物体
		rootParameters[1].InitAsConstantBufferView(1);	// b1 - 材质
		rootParameters[2].InitAsConstantBufferView(2);	// b2 - 过程
	}

	// 根签名是根参数数组
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		NUM_CB,
		rootParameters,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// 使用单个槽创建根签名，该槽指向由单个常量缓冲区组成的描述符范围。
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

		m_Shaders["VS"] = DXUtils::CompileShader(filePath.c_str(), nullptr, "VS", "vs_5_1");
		m_Shaders["PS"] = DXUtils::CompileShader(filePath.c_str(), nullptr, "PS", "ps_5_1");
	}

	// InputLyout
	{
		// 需要根据Vertex结构和 hlsl 中的顶点输入结构语义索引准备输入元素描述符
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName			- 依据 HLSL
			0,											// SemanticIndex		- 依据 HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format				- 依据 Vertex(32位浮点数)
			0,											// InputSlot			- 输入槽
			Vertex::OffsetPos,							// AlignedByteOffset	- 对齐字节偏移
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// InputSlotClass		- 输入槽类型
			0 },										// InstanceDataStepRate	- 实例数据步长

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
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// 深度模板状态 - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

		// 输入布局 - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };

		// todo:索引缓冲区条切值 - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// 图元拓扑类型 - D3D12_PRIMITIVE_TOPOLOGY_TYPE
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// 渲染视图格式 - RTV，DXGI_FORMAT_R8G8B8A8_UNORM
		psoDesc.NumRenderTargets = BackBufferCount;
		psoDesc.RTVFormats[0] = m_BackBufferFormat;
		//for (int i = 0; i < BackBufferCount; i++)
		//{
		//	psoDesc.RTVFormats[i] = m_BackBufferFormat;
		//}

		// 深度模板格式 - DSV，DXGI_FORMAT_D24_UNORM_S8_UINT
		psoDesc.DSVFormat = m_DepthStencilFormat;

		// 采样器 - DXGI_SAMPLE_DESC
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };
		psoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;

		// 节点掩码
		psoDesc.NodeMask = 0;

		// ? D3D12_CACHED_PIPELINE_STATE
		psoDesc.CachedPSO = {};

		// ?
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}
	BuildPipelineStateSub(psoDesc);

	return;
}
void LitRadar::BuildPipelineStateSub(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
	// Default
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDefault = psoDesc;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDefault, IID_PPV_ARGS(&m_PSOs["Default"])));

	// Solid
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoSoild = psoDesc;
	psoSoild.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoSoild, IID_PPV_ARGS(&m_PSOs["Solid"])));

	// Wireframe
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoWireframe = psoDesc;
	psoWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoWireframe, IID_PPV_ARGS(&m_PSOs["Wireframe"])));

	// Transparent
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoTransparent = psoDesc;
	{
		psoTransparent.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		//psoTransparent.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		//psoTransparent.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

		D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc = {};
		transparencyBlendDesc.BlendEnable = true;
		transparencyBlendDesc.LogicOpEnable = false;

		transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			// 来自着色器
		transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// 来自后台缓冲区
		transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;

		// D3D12_BLEND_SRC_ALPHA
		transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

		transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		psoTransparent.BlendState.RenderTarget[0] = transparencyBlendDesc;
	}
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoTransparent,
		IID_PPV_ARGS(&m_PSOs["Transparent"])));


	// Alpha tested
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoAlphaTested = psoDesc;
	{
		//reinterpret_cast<BYTE*>(mShaders["alphaTestedPS"]->GetBufferPointer()),
		//	mShaders["alphaTestedPS"]->GetBufferSize()
	}
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoAlphaTested, IID_PPV_ARGS(&m_PSOs["AlphaTested"])));

	return;
}

void LitRadar::BuildRadarGeometry_0()
{
	//KString s = KString::Format(_T("%03d"), 50);
	//_astring  as = KString::AString(s);

	//m_Theta = 1.5f*XM_PI;
	//m_Phi = XM_PIDIV2 - 0.1f;
	//m_Radius = 3.0f;

	MeshData meshRadar;

	// 输出
	//bool output = true;
	bool output = false;

	//_tstring rfile(_T("d:\\temp\\2.txt"));
	_tstring rfile(_T("d:\\temp\\2_xyz.txt"));
	//_tstring rfile(_T("d:\\temp\\2_llh.txt"));

	//_tstring rfile(_T("d:\\sfxData\\R3Data\\r150.txt"));
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
		// 数据保存到 m_Slices
		LoadSlicerFromFiles16(_T("d:\\sfxData\\R3Data\\mhd\\rad.mhd"));

		//vector<double> contourValues = { 50 };
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

		vector<double> contourValues = { 350 };

		// 三维重构
		RebuildRadar3d(&m_Slices, contourValues);

		// 生成网格数据
		GenerateMeshData(&meshRadar);

		// 输出网格数据
		_tstring outfile = rfile;
		//OutputMesh(outfile, &meshRadar);
	}

	// 测试输出文件
	if (!output)
	{
		_tstring infile = rfile;
		InputMesh(rfile, &meshRadar);	// 使用xyz文件（可以来自 MTK 输出）
	}


	// MeshGeometry
	_astring dwaw_name = m_MatNames[RLayer::All];
	auto geo = std::make_unique<MeshGeometry>();
	{
		geo->Init(m_device.Get(), m_CommandList.Get(), &meshRadar);
		geo->DrawArgs[dwaw_name] = SubmeshGeometry(meshRadar.IndexCount(), 0, 0);
	}
	m_Geometries[RLayer::All] = std::move(geo);

	return;
}

// 读取雷达数据
void LitRadar::BuildRadarGeometry()
{
	// 数据保存到 m_Slices
	//LoadSlicerFromFiles16(_T("d:\\sfxData\\R3Data\\mhd\\rad.mhd"));
	//LoadSlicerFromFiles16(_T("d:\\sfxData\\R3Data\\mhd\\rad03.mhd"));
	LoadSlicerFromFiles16(_T("d:\\sfxData\\R3Data\\mhd\\rad36Layer.mhd"));

	//vector<double> CONTOUR_VALUES = { 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };

	vector<int> _contourValues = { 300, 500 };
	//vector<int> _contourValues = { 50,200,250,300, 450,500,550 };
	//vector<int> _contourValues = { 150, 300, 350, 450, 500, 550, 600, 650 };
	//vector<int> _contourValues = { 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };

	// 
	//vector<int> _contourValues = { 250, 300, 350, 400, 450, 500, 550, 600, 650 };

	//vector<int> _contourValues = { 300, 350, 400, 450, 500, 550, 600, 650 };
	//vector<int> _contourValues = { 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650 };
	for (size_t i = 0; i < _contourValues.size(); i++)
	{
		int icvalue = _contourValues[i];	// 每次一个曲面
		vector<double> contour_values = { static_cast<double>(icvalue) };

		// 三维重构
		RebuildRadar3d(&m_Slices, contour_values);

		// 生成网格数据
		auto radarMeshData = std::make_unique<MeshData>();
		auto pradarMeshData = radarMeshData.get();
		GenerateMeshData(pradarMeshData);

		if (pradarMeshData->VertexCount() == 0)
			continue;

		// 绘制参数名称
		RLayer eLayer = (RLayer)icvalue;
		_astring drawName = m_MatNames[eLayer];

		// 生成 MeshGeometry
		auto geo = std::make_unique<MeshGeometry>();
		{
			geo->Init(m_device.Get(), m_CommandList.Get(), pradarMeshData);
			geo->DrawArgs[drawName] = SubmeshGeometry(pradarMeshData->IndexCount(), 0, 0);
		}

		m_Geometries[eLayer] = std::move(geo);
		m_MeshDatas[eLayer] = std::move(radarMeshData);
	}

	// 输出数据
	//vector<int> _contourValues = { 150, 300, 450, 500, 550, 600, 650 };
	//int nCount = m_MeshDatas.size();

	XINFO xi = m_xinfo;
	for (size_t i = 0; i < _contourValues.size(); i++)
	{
		int icvalue = _contourValues[i];
		RLayer eLayer = (RLayer)icvalue;
		MeshData* pm = m_MeshDatas[eLayer].get();
	}

	return;

}

void LitRadar::BuildMaterials_0()
{
	int index = 0;

	// mat_All
	{
		auto mat_All = std::make_unique<Material>(m_FrameResourceCount);
		auto pMat = mat_All.get();

		XMFLOAT4 diffuseAlbedo = XMFLOAT4(1.000f, 1.000f, 1.000f, 1.000f);

		pMat->Name = m_MatNames[RLayer::All];
		pMat->MatCBIndex = index++;
		pMat->DiffuseAlbedo = diffuseAlbedo;
		pMat->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
		pMat->Roughness = 0.853f;

		m_Materials[RLayer::All] = std::move(mat_All);
	}

	return;
}
void LitRadar::BuildMaterials()
{
	int index = 0;
	m_Materials.clear();

	BuildMaterials(index, RLayer::RL050);
	BuildMaterials(index, RLayer::RL100);
	BuildMaterials(index, RLayer::RL150);
	BuildMaterials(index, RLayer::RL200);
	BuildMaterials(index, RLayer::RL250);
	BuildMaterials(index, RLayer::RL300);
	BuildMaterials(index, RLayer::RL350);
	BuildMaterials(index, RLayer::RL400);
	BuildMaterials(index, RLayer::RL450);
	BuildMaterials(index, RLayer::RL500);
	BuildMaterials(index, RLayer::RL550);
	BuildMaterials(index, RLayer::RL600);
	BuildMaterials(index, RLayer::RL650);

	return;
}
void LitRadar::BuildMaterials(int& index, RLayer eLayer)
{
	auto mat = std::make_unique<Material>(m_FrameResourceCount);

	auto pMat = mat.get();
	pMat->Name = m_MatNames[eLayer];
	pMat->MatCBIndex = index++;
	pMat->DiffuseAlbedo = m_DiffuseAlbedos[eLayer];
	pMat->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	pMat->Roughness = 0.853f;

	m_Materials[eLayer] = std::move(mat);
}

void LitRadar::BuildRenderItems_0()
{
	assert(m_FrameResourceCount > 0);

	int index = 0;
	m_AllRitems.clear();

	auto radarRitem = std::make_unique<FrameResourceLir::RenderItem>(m_FrameResourceCount);
	{
		_astring mat_name = m_MatNames[RLayer::All];

		auto pGeo = m_Geometries[RLayer::All].get();

		radarRitem->ObjCBIndex = index++;
		radarRitem->World = DXUtils::Identity4x4;
		radarRitem->Mat = m_Materials[RLayer::All].get();
		radarRitem->Geo = pGeo;

		radarRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// 绘制参数
		radarRitem->IndexCount = pGeo->DrawArgs[mat_name].IndexCount;
		radarRitem->StartIndexLocation = pGeo->DrawArgs[mat_name].StartIndexLocation;
		radarRitem->BaseVertexLocation = pGeo->DrawArgs[mat_name].BaseVertexLocation;

		// 调整世界坐标
		//XMStoreFloat4x4(&radarRitem->World, XMMatrixTranslation(-0.5f, -0.5f, -0.5f));
	}

	// 保存数据
	m_AllRitems.push_back(std::move(radarRitem));

	return;
}
void LitRadar::BuildRenderItems()
{
	assert(m_FrameResourceCount > 0);

	int index = 0;
	m_AllRitems.clear();

	BuildRenderItems(index, RLayer::RL050);
	BuildRenderItems(index, RLayer::RL100);
	BuildRenderItems(index, RLayer::RL150);
	BuildRenderItems(index, RLayer::RL200);
	BuildRenderItems(index, RLayer::RL250);
	BuildRenderItems(index, RLayer::RL300);
	BuildRenderItems(index, RLayer::RL350);
	BuildRenderItems(index, RLayer::RL400);
	BuildRenderItems(index, RLayer::RL450);
	BuildRenderItems(index, RLayer::RL500);
	BuildRenderItems(index, RLayer::RL550);
	BuildRenderItems(index, RLayer::RL600);
	BuildRenderItems(index, RLayer::RL650);

	return;
}
void LitRadar::BuildRenderItems(int& index, RLayer eLayer)
{
	RLayer rlayer = eLayer;			// 

	auto drw_name = m_MatNames[rlayer];

	auto pMat = (m_Materials.end() != m_Materials.find(rlayer))
		? m_Materials.at(rlayer).get()
		: nullptr;

	auto pGeo = (m_Geometries.end() != m_Geometries.find(rlayer))
		? m_Geometries.at(rlayer).get() : nullptr;

	if (pGeo && pMat)
	{
		auto ritem = std::make_unique<FrameResourceLir::RenderItem>(m_FrameResourceCount);
		auto pRitem = ritem.get();

		pRitem->ObjCBIndex = index++;
		pRitem->World = DXUtils::Identity4x4;
		pRitem->Mat = pMat;
		pRitem->Geo = pGeo;
		pRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		pRitem->IndexCount = pGeo->DrawArgs[drw_name].IndexCount;
		pRitem->StartIndexLocation = pGeo->DrawArgs[drw_name].StartIndexLocation;
		pRitem->BaseVertexLocation = pGeo->DrawArgs[drw_name].BaseVertexLocation;

		m_RitemLayer[0].push_back(pRitem);
		m_AllRitems.push_back(std::move(ritem));
	}
}

void LitRadar::BuildFrameResources()
{
	if (m_AllRitems.size() > 0)
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

	return;
}


//【窗口大小改变】
void LitRadar::Resized()
{
	// 更新投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

//【更新数据】
void LitRadar::Update()
{
	CircularFrameResource();

	OnKeyboardInput();
	UpdateCamera();
	UpdateObjectCB();
	UpdateMaterialCB();
	UpdatePassCB();
}
void LitRadar::UpdateCamera()
{
	// Convert Spherical to Cartesian coordinates.
	m_EyePos.x = m_Radius * sinf(m_Phi)*cosf(m_Theta);
	m_EyePos.z = m_Radius * sinf(m_Phi)*sinf(m_Theta);
	m_EyePos.y = m_Radius * cosf(m_Phi);

	// 构造视图矩阵
	XMVECTOR pos = XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// y轴向上

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);
}
void LitRadar::UpdateObjectCB()
{
	auto pFrameResource = CurrentFrameResource();
	if (pFrameResource == nullptr)		return;

	auto currObjectCB = pFrameResource->ObjectCB.get();

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
void LitRadar::UpdateMaterialCB()
{
	auto pFrameResource = CurrentFrameResource();
	if (pFrameResource == nullptr) return;

	auto currMaterialCB = pFrameResource->MaterialCB.get();
	for (auto& e : m_Materials)
	{
		Material* mat = e.second.get();

		//if (mat && mat->Dirtys > 0)
		if (mat->Dirtys > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&(mat->MatTransform));

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
void LitRadar::UpdatePassCB()
{
	auto pFrameResource = CurrentFrameResource();
	if (pFrameResource == nullptr) return;

	// 只更新1次
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

		passCB.AmbientLight = { 0.26f, 0.26f, 0.26f, 1.0f };

		float sss = 0.6f, dir = 0.576f;
		for (int i = 0; i < 6; i++)
		{
			passCB.Lights[i].Strength = { sss, sss, sss };
		}

		passCB.Lights[0].Direction = { dir, 0.0f, 0.0f };
		passCB.Lights[1].Direction = { 0.0f, dir,0.0f };
		passCB.Lights[2].Direction = { 0.0f, 0.0f, dir };
		passCB.Lights[3].Direction = { -dir, 0.0f, 0.0f };
		passCB.Lights[4].Direction = { 0.0f, -dir, 0.0f };
		passCB.Lights[5].Direction = { 0.0f, 0.0f, -dir };

		//passCB.Lights[0].Direction = { dir, dir, dir };
		//passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
		//passCB.Lights[1].Direction = { -dir, -dir, -dir };
		//passCB.Lights[1].Strength = { 0.6f, 0.6f, 0.6f };
	}

	auto currPassCB = pFrameResource->PassCB.get();
	currPassCB->CopyData(0, passCB);

	return;
}


// 【渲染绘制】
void LitRadar::Draw()
{
	auto pFrameResource = CurrentFrameResource();
	if (pFrameResource == nullptr) return;

	// 数据准备
	auto pCommandAllocator = pFrameResource->CmdListAllocator.Get();
	auto pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();
	auto pCommandList = m_CommandList.Get();
	auto pCommandQueue = m_CommandQueue.Get();
	auto pSwapChain = m_SwapChain.Get();

	// 复位命令分配器，清空命令分配器中的命令记录
	ThrowIfFailed(pCommandAllocator->Reset());

	// 复位命令列表（后面的命令记录保存在pCommandAllocator中）
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator, pCurrentPso));

	{
		// 资源屏障 - 由呈现转换为渲染状体
		pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// 光栅化阶段
		pCommandList->RSSetViewports(1, &m_ScreenViewport);
		pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出装配
		pCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// 清除渲染背景
		const float* clearColor = Colors::Black;
		//const float* clearColor = Colors::White;
		//const float* clearColor = Colors::DarkGray;
		//const float* clearColor = Colors::Blue;
		//const float* clearColor = Colors::LightBlue;
		//const float* clearColor = Colors::LightSkyBlue;
		pCommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		pCommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		{
			// 设置根签名
			pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

			// 绑定过程资源到常量缓冲区
			auto passCB = pFrameResource->PassCB->Resource();
			pCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());	// b2

			// 绘制
			if (flag_radar == 0)
			{
				DrawRenderItems_0();
			}
			else
			{
				pCommandList->SetPipelineState(m_PSOs["Transparent"].Get());
				DrawRenderItems(pCommandList, m_RitemLayer[0]);
			}
		}

		// 资源屏障 - 由渲染转换为呈现状态
		pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	}

	// 完成记录命令
	ThrowIfFailed(pCommandList->Close());

	{
		// 把命令列表加入命令队列
		ID3D12CommandList* cmdsLists[] = { pCommandList };
		pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// 交换前后台缓冲区
		ThrowIfFailed(pSwapChain->Present(1, 0));
		m_BackBufferIndex = (m_BackBufferIndex + 1) % BackBufferCount;

		// 将围栏值向前推进，将命令标记到此围栏点
		pFrameResource->FenceValue = ++m_FenceValue;

		// 向命令队列中添加一条指令，以设置新的围栏点。
		// 因为我们在GPU时间轴上，在GPU处理完之前的所有命令之前，
		// 此Signal()将不会设置新的围栏点。
		pCommandQueue->Signal(m_Fence.Get(), m_FenceValue);
	}

	return;
}

void LitRadar::DrawRenderItems_0()
{
	auto pFrameResource = CurrentFrameResource();
	if (pFrameResource == nullptr) return;

	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceLir::ObjectConstants));
	UINT matCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceLir::MaterialConstants));

	auto objCB = pFrameResource->ObjectCB->Resource();
	auto matCB = pFrameResource->MaterialCB->Resource();

	for (size_t i = 0; i < m_AllRitems.size(); i++)
	{
		auto ri = m_AllRitems[i].get();

		// b0 - 物体
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		// b1 - 材质
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
void LitRadar::DrawRenderItems(ID3D12GraphicsCommandList* pCommandList,
	const std::vector<FrameResourceLir::RenderItem*>& ritems)
{
	auto pFrameResource = CurrentFrameResource();
	if (pFrameResource == nullptr) return;

	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceLir::ObjectConstants));
	UINT matCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceLir::MaterialConstants));

	auto objCB = pFrameResource->ObjectCB->Resource();
	auto matCB = pFrameResource->MaterialCB->Resource();

	size_t count = ritems.size();
	for (size_t i = 0; i < count; ++i)
	{
		int ii = (count - 1) - i;	// 倒序
		auto ri = ritems[ii];

		// b0
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		pCommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		// b1
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;
		pCommandList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		// 输入装配
		pCommandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		pCommandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		pCommandList->IASetPrimitiveTopology(ri->PrimitiveType);

		// 绘制
		pCommandList->DrawIndexedInstanced(
			ri->IndexCount,			// 索引数量
			1,						// 实例数量
			ri->StartIndexLocation,	// 索引开始位置
			ri->BaseVertexLocation,	// 顶点开始位置
			0);						// 实例开始位置
	}

	return;
}



void LitRadar::DoAppIdle()
{
	Dx12Book::DoAppIdle();
}


//【辅助函数】
bool LitRadar::LoadSlicerFromFiles16(_tstring filePrefix, int indexBeg, int indexEnd, int width, int height)
{
	m_Slices.clear();

	int nSliceCount = indexEnd - indexBeg + 1;		// 切片数量
	int nSliceSize = width * height;				// 切片大小
	int nBufferSize = nSliceSize * sizeof(uint16);	// 缓冲区大小(字节)

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

		//关闭文件
		fin.close();
		std::cout << "File Loaded OK: " << KString::AString(finalPath.c_str()) << std::endl;
	}

	return true;
}

// 数据保存到 m_Slices
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

	// 输入流（向内存输入）
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
		m_xinfo.dims[i] = dim[i];
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

	int nSliceWidth = m_xinfo.dims[0];		// 57
	int nSliceHeight = m_xinfo.dims[1];		// 47
	int nSliceCount = m_xinfo.dims[2];		// 切片数量 24

	int nSliceSize = nSliceWidth * nSliceHeight;				// 切片大小
	int nBufferSize = nSliceSize * sizeof(uint16);	// 缓冲区大小(字节)


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

	// 生成每层图像
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

// pSlices -> 数据源
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



	KMarchingCubes* pmc = &m_KMarchingCubes;
	pmc->Clear();

	// pmc->Building()
	pmc->Building(scalars.data(), dims, (double*)contourValues.data(), contourValues.size());


	//double origin[3] = { 0, 0, 0 };
	//double spacing[3] = { 1.0, 1.0, 1.0 };
	//pmc->Building(scalars.data(), dims, origin, spacing, (double*)contourValues.data(), contourValues.size());


	return;
}
void LitRadar::GenerateMeshData(MeshData* pMeshData)
{
	KMarchingCubes* pmc = &m_KMarchingCubes;

	// Vertices - 顶点
	std::vector<Vector3> vertices;
	pmc->GetVertices(vertices);

	// Indices - 索引
	std::vector<uint32> indices;
	pmc->GetIndices(indices);

	// Normals - 法线
	std::vector<Vector3> normals;
	pmc->GetNormals(normals);

	// VertexValues - 顶点值
	std::vector<double> vertexValues;
	pmc->GetVertexValues(vertexValues);


	// 顶点数量
	float r, g, b, a;
	int nCount = (int)vertices.size();

	for (int i = 0; i < nCount; i++)
	{
		Vector3 v = vertices[i];
		//XMFLOAT3 pnt = { (float)v.x, (float)v.y, (float)v.z };
		//XMFLOAT3 pnt = { (float)v.x, (float)-v.y, (float)v.z };
		XMFLOAT3 pnt = { (float)v.x, (float)-v.z, (float)-v.y };

		Vector3 n = normals[i];
		XMFLOAT3 nom = { (float)n.x, (float)n.y, (float)n.z };

		a = 1.0f;
		float sv = (float)vertexValues[i];	// 顶点值
		//if (sv = 350) a = 0.2f;
		//if (sv = 450) a = 1.0f;

		GetRadarColor(sv, r, g, b);
		XMFLOAT4 clr = { r, g, b, a };

		pMeshData->vertex_push_bak(Vertex(pnt, nom, clr));
		pMeshData->index_push_back(i);
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

	每个文件存储了一个dbz值的面片数据
	不如r150.txt村输了 dbz=15 的面片数据，以此类推。

	文件数据按下面格式和顺序存储：

	1、顶点坐标：p(x，y，z),每行存储一个顶点
	Points 240
	x0 y0 z0
	...

	2、顶点法线：n(x，y，z),每行存储一个法向量
	Normals 240
	x0 y0 z0
	...

	3、顶点颜色：c(r，g，b),每行存储一个颜色向量
	Colors 240
	ro g0 b0
	...

	4、三角形索引：(idx0 idx1 idx2)三个索引（顺时针绕序）决定一个三角形
	Triangles 80
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
		_astring sx = KString::AString((LPCTSTR)KString::Format(_T("%8.5f"), vec.x));
		_astring sy = KString::AString((LPCTSTR)KString::Format(_T("%8.5f"), vec.y));
		_astring sz = KString::AString((LPCTSTR)KString::Format(_T("%8.5f"), vec.z));

		//out << vec.x << _blank_space_ << vec.y << _blank_space_ << vec.z << endl;
		out << sx << _blank_space_ << sy << _blank_space_ << sz << endl;
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
	Triangles 23854
	*/

	int nCount = 0;
	_astring ignore_str;

	vector<XMFLOAT3> points;
	vector<XMFLOAT3> normals;
	//vector<XMFLOAT4> colors;

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
	//fin >> ignore_str >> nCount;
	//for (int i = 0; i < nCount; i++)
	//{
	//	XMFLOAT4 vec(0, 0, 0, 1);
	//	fin >> vec.x >> vec.y >> vec.z;
	//	colors.push_back(vec);
	//}

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

	// 构建顶点
	nCount = points.size();
	for (int i = 0; i < nCount; i++)
	{
		XMFLOAT3 pnt = points[i];
		XMFLOAT3 nom = normals[i];
		XMFLOAT4 clr = { 0.0f, 0.0f, 0.0f, 1.0f };

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
