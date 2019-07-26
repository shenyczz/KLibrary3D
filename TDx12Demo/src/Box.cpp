#include "stdafx.h"
#include "Box.h"
#include "MC.h"


#pragma region _getColor

void _getColor(float v, float& r, float& g, float& b)
{
	r = (float)0xC0 / 0xFF;		g = (float)0xC0 / 0xFF;		g = (float)0xFE / 0xFF;

	if (false) {}
	else if (v < 5)
	{
		r = (float)0x7A / 0xFF;		g = (float)0x72 / 0xFF;		g = (float)0xEE / 0xFF;
	}
	else if (v < 10)
	{
		r = (float)0x1E / 0xFF;		g = (float)0x1E / 0xFF;		g = (float)0xD0 / 0xFF;
	}
	else if (v < 15)
	{
		r = (float)0xA6 / 0xFF;		g = (float)0xFC / 0xFF;		g = (float)0xA8 / 0xFF;
	}
	else if (v < 20)
	{
		r = (float)0x00 / 0xFF;		g = (float)0xEA / 0xFF;		g = (float)0x00 / 0xFF;
	}
	else if (v < 25)
	{
		r = (float)0x10 / 0xFF;		g = (float)0x92 / 0xFF;		g = (float)0x1A / 0xFF;
	}
	else if (v < 30)
	{
		r = (float)0xFC / 0xFF;		g = (float)0xF4 / 0xFF;		g = (float)0x64 / 0xFF;
	}
	else if (v < 35)
	{
		r = (float)0xC8 / 0xFF;		g = (float)0xC8 / 0xFF;		g = (float)0x02 / 0xFF;
	}
	else if (v < 40)
	{
		r = (float)0x8C / 0xFF;		g = (float)0x8C / 0xFF;		g = (float)0x00 / 0xFF;
	}
	else if (v < 45)
	{
		r = (float)0xFE / 0xFF;		g = (float)0xAC / 0xFF;		g = (float)0xAC / 0xFF;
	}
	else if (v < 50)
	{
		r = (float)0xFE / 0xFF;		g = (float)0x64 / 0xFF;		g = (float)0x5C / 0xFF;
	}
	else if (v < 55)
	{
		r = (float)0xEE / 0xFF;		g = (float)0x02 / 0xFF;		g = (float)0x30 / 0xFF;
	}
	else if (v < 60)
	{
		r = (float)0xD4 / 0xFF;		g = (float)0x8E / 0xFF;		g = (float)0xFE / 0xFF;
	}
	else if (v < 65)
	{
		r = (float)0xAA / 0xFF;		g = (float)0x24 / 0xFF;		g = (float)0xFA / 0xFF;
	}


}

#pragma endregion












Box::Box()
{
	m_Theta = XM_PI * (3.0f / 2);	
	m_Phi = XM_PI / 4;
	m_Radius = 5.0f;

	m_bWireFrame = false;
}

Box::~Box()
{
}


void Box::OnResize()
{
	Dx12Book::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH((float)KMaths::PI / 4, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}
void Box::OnUpdate()
{
	Dx12Book::OnUpdate();

	m_psoCurrent = m_bWireFrame
		? m_psoMap[PsoStyle::WireFrame]
		: m_psoMap[PsoStyle::Solid];



	// Camera 
	{
		XMVECTOR pos = DXUtil::SphericalToCartesian(m_Radius, m_Theta, m_Phi);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view_temp = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&m_View, view_temp);
	}


	// 更新变换
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX worldViewProj = world * view * proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	FrameResourceBox::ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	m_ObjectCB->CopyData(0, objConstants);
}

void Box::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_bWireFrame = !m_bWireFrame;
	}

	Dx12Book::OnMouseDown(wParam, x, y);
}


void Box::RadarMesh(MeshData* pMesh)
{
	m_Radius = 30.0f;

	pMesh->Clear();

	{
		MC mc;
		mc.LoadCTSlicerFromFiles16(("D:\\sfxData\\VTKData\\Radar\\rad3d\\rad"), 1, 24, 68, 52);


		MCP mcp = {};
		{
			mcp.cubeCountX = 68 * 2;		// 150
			mcp.cubeCountY = 23;		// mc.m_Slices.size() - 1
			mcp.cubeCountZ = 52 * 2;		// 150

			mcp.cubeWidth = 0.15f;		// 0.23
			mcp.cubeHeight = 0.10f;		// 0.10
			mcp.cubeDepth = 0.15f;		// 0.23
		}
		//重建模型
		mc.Reconstruct16(mcp);

		std::vector<KVector3> outVertexList;
		mc.GetMesh(outVertexList);

		int nCount = outVertexList.size();
		float r, g, b;

		for (int i = 0; i < nCount; i++)
		{
			KVector3 vec = outVertexList[i];

			XMFLOAT3 pnt = { vec.x, vec.y*5.0f, vec.z };

			XMFLOAT4 clr = { 0.5f, 0.0f, 0.5f, 1.0f };	// RGBA

			_getColor(vec.V / 10, r, g, b);

			//if (i % 2 == 0)
			{
				clr = { r, g, b, 1.0f };	// RGBA
			}

			pMesh->Vertices.push_back(Vertex{ pnt, clr });
			pMesh->Indices32.push_back((uint32)i);
		}

		//pMesh->EnableUseIndex();
	}

	return;
}


void Box::BuildRootSignature()
{
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if(errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
}
void Box::BuildShadersAndInputLayout()
{

	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("bookBox.hlsl");
		_tstring filePath = path + _T("Asserts\\") + file;

		//auto geo = std::make_unique<MeshGeometry>();

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
void Box::BuildPipelineStateObject()
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
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

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

	// DSV 格式
	psoDesc.DSVFormat = m_DepthStencilFormat;

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
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_psoMap[PsoStyle::WireFrame])));
	}

	m_psoCurrent = m_psoMap[PsoStyle::Default];

	return;
}

void Box::BuildMeshData()
{
	bool clr = true;
	clr = !clr;

	if(clr)
	{

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

		std::array<uint16, 36> indices =
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

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16);

		// "boxGeo"
		unique_ptr<MeshGeometry> boxGeo = std::make_unique<MeshGeometry>();
		{
			boxGeo->Name = "boxGeo";

			ThrowIfFailed(D3DCreateBlob(vbByteSize, &boxGeo->VertexBufferCPU));
			CopyMemory(boxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

			boxGeo->VertexBufferGPU = DXUtil::CreateDefaultBuffer(m_device.Get(),
				m_CommandList.Get(), vertices.data(), vbByteSize, boxGeo->VertexBufferUploader);

			ThrowIfFailed(D3DCreateBlob(ibByteSize, &boxGeo->IndexBufferCPU));
			CopyMemory(boxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

			boxGeo->IndexBufferGPU = DXUtil::CreateDefaultBuffer(m_device.Get(),
				m_CommandList.Get(), indices.data(), ibByteSize, boxGeo->IndexBufferUploader);

			boxGeo->VBSizeInBytes = vbByteSize;
			boxGeo->VBStrideInBytes = sizeof(Vertex);

			boxGeo->IBSizeInBytes = ibByteSize;
			boxGeo->IBVFormat = DXGI_FORMAT_R16_UINT;

			SubmeshGeometry submesh;
			submesh.IndexCount = (UINT)indices.size();
			submesh.StartIndexLocation = 0;
			submesh.BaseVertexLocation = 0;

			boxGeo->DrawArgs["box"] = submesh;
		}

		// 
		m_Geometries["boxGeo"] = std::move(boxGeo);
	}
	else
	{
		MeshData mesh;

		RadarMesh(&mesh);
		//MeshFactory::CreateBox(1.0f, 1.0f, 1.0f, 3, &mesh);
		//mesh.OffsetZ(1.0f);

		unique_ptr<MeshGeometry> boxGeo = std::make_unique<MeshGeometry>();
		{
			boxGeo->Name = "boxGeo";

			size_t vbSizeInBytes = mesh.vbSizeInBytes();
			ThrowIfFailed(D3DCreateBlob(vbSizeInBytes, &boxGeo->VertexBufferCPU));						// 先创建顶点缓冲区(CPU内存)
			CopyMemory(boxGeo->VertexBufferCPU->GetBufferPointer(), mesh.pvbInitData(), vbSizeInBytes);	// 拷贝网格数据到顶点缓冲区

			//todo: VertexBufferGPU,VertexBufferUploader 各自用途？ VertexBufferGPU
			// 
			boxGeo->VertexBufferGPU = DXUtil::CreateDefaultBuffer(m_device.Get(), m_CommandList.Get(),
				mesh.pvbInitData(), vbSizeInBytes, boxGeo->VertexBufferUploader);

			size_t ibSizeInBytes = mesh.ibSizeInBytes();
			ThrowIfFailed(D3DCreateBlob(ibSizeInBytes, &boxGeo->IndexBufferCPU));						// IB
			CopyMemory(boxGeo->IndexBufferCPU->GetBufferPointer(), mesh.pibInitData(), ibSizeInBytes);	// 

			boxGeo->IndexBufferGPU = DXUtil::CreateDefaultBuffer(m_device.Get(), m_CommandList.Get(),	// 
				mesh.pibInitData(), ibSizeInBytes, boxGeo->IndexBufferUploader);

			boxGeo->VBSizeInBytes = vbSizeInBytes;
			boxGeo->VBStrideInBytes = mesh.vbStrideInBytes();

			boxGeo->IBSizeInBytes = ibSizeInBytes;
			boxGeo->IBVFormat = MeshData::IBVFormat;

			// 绘制参数
			boxGeo->DrawArgs["box"] = SubmeshGeometry(mesh.IndexCount(), 0, 0);

		}
		m_Geometries["boxGeo"] = std::move(boxGeo);
	}



	return;
}

void Box::BuildConstantBufferAndView()
{
	m_ObjectCB = std::make_unique<TUploadBuffer<FrameResourceBox::ObjectConstants>>(m_device.Get(), 1, true);

	UINT objCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceBox::ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();

	// Offset to the ith object constant buffer in the buffer.
	int objCBIndex = 0;
	cbAddress += objCBByteSize * objCBIndex;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceBox::ObjectConstants));

	int offsetInDescriptors = 0;
	auto cbvHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
	cbvHandle.ptr += m_cbvDescriptorSize * offsetInDescriptors;

	// 创建常量缓冲区
	m_device->CreateConstantBufferView(&cbvDesc, cbvHandle);

	return;
}

void Box::ReccorCommand()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

	// 清除渲染背景
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(RenderTargetView(), clearColor, 0, nullptr);

	m_CommandList->ClearDepthStencilView(DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// 输入装配
	MeshGeometry* pGeo = m_Geometries["boxGeo"].get();
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &pGeo->VertexBufferView());
	m_CommandList->IASetIndexBuffer(&pGeo->IndexBufferView());
	m_CommandList->DrawIndexedInstanced(pGeo->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

	return;
}





