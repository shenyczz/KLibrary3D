#include "stdafx.h"
#include "Land.h"



Land::Land()
{
	m_IsWireframe = true;

	m_FrameResourceIndex = 0;
	m_FrameResourceCount = 3;
}

Land::~Land()
{
}


void Land::OnResize()
{
	Dx12Book2::OnResize();

	UpdateProj();
}
void Land::OnUpdate()
{
	// 循环帧资源数组
	CycleFrameResources();

#ifdef _DEBUG
	KString s = KString::Format(_T("Current FenceValue = %d"), CurrentFrameResource()->FenceValue);
	KUtil::Trace(s);
#endif

	// PSO
	UpdatePSO();

	// 摄像机
	UpdateCamera();

	// 项目常量
	UpdateObjectCBs();

	// 过程常量
	UpdatePassCBs();

	UpdateWaves();

	return;
}

void Land::OnRender()
{
	// 1.构造命令列表
	BuildCommandList();

	// 2.执行命令列表
	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// 3.呈现帧
	ThrowIfFailed(m_SwapChain->Present(1, 0));

	// 4.循环后台缓冲区索引
	m_BackBufferIndex = (m_BackBufferIndex + 1) % BackBufferCount;

	// 刷新命令队列

	//FlushCommandQueue();

	// 这里不再每次都刷新

	// Advance the fence value to mark commands up to this fence point.
	m_FenceValue++;
	CurrentFrameResource()->FenceValue = m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue));

	return;
}

void Land::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	Dx12Book2::OnMouseDown(wParam, x, y);
}
void Land::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book2::OnMouseUp(wParam, x, y);
}
void Land::OnMouseMove(WPARAM wParam, int x, int y)
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
		m_Radius += (dx - dy) * 20;

		// Restrict the radius.
		m_Radius = KMaths::Clamp(m_Radius, 5.0f, 280.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	return;
}

void Land::BuildRootSignature()
{
	// 构造根签名：两个常量寄存器 

	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBV.
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		_countof(slotRootParameter), slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

	return;
}
void Land::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("bookLand.hlsl");
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
void Land::BuildPipelineStateObject()
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
	psoDesc.NumRenderTargets = BackBufferCount;
	for (int i = 0; i < BackBufferCount; i++)
	{
		psoDesc.RTVFormats[i] = m_BackBufferFormat;
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
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSOs[PsoStyle::Default])));
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSOs[PsoStyle::Solid])));
	}

	{
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSOs[PsoStyle::WireFrame])));
	}

	m_psoCurrent = m_PSOs[PsoStyle::Default];

	return;
}
void Land::BuildMeshData()
{
	BuildLandGeometry();
	BuildWavesGeometry();
	BuildRenderItems();
	BuildFrameResources();
}
void Land::BuildOthers()
{
	KWindow* pWindow = KApplication::GetApp()->GetWindow();
	int iClientWidth = pWindow->ClientWidth();
	int iClientHeight = pWindow->ClientHeight();

	m_DepthStencil.Reset();

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = iClientWidth;
	depthStencilDesc.Height = iClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_DepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_DepthStencil.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_DepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencil.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	return;
}

void Land::BuildCommandList()
{
	ID3D12CommandAllocator* pCommandAllocator = CurrentFrameResource()->CmdListAllocator.Get();
	ID3D12PipelineState* pCurrentPso = m_psoCurrent.Get();

	// Reset 之前要先 Close()
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(pCommandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before re-recording.
	ThrowIfFailed(m_CommandList->Reset(pCommandAllocator, pCurrentPso));

	// 按照资源用途指示其状态的转变，此处将资源从呈现状态转换为渲染状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentRenderTarget(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	{
		DrawRenderItems();
	}
	// 按照资源用途指示其状态的转变，此处将资源从渲染状态转换为呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentRenderTarget(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());

	return;

}


// private:



void Land::UpdatePSO()
{
	m_psoCurrent = m_IsWireframe
		? m_PSOs[PsoStyle::WireFrame]
		: m_PSOs[PsoStyle::Solid];

	return;
}

void Land::UpdateProj()
{
	// 投影矩阵
	XMMATRIX proj = XMMatrixPerspectiveFovLH((float)KMaths::PI / 4, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, proj);
}

void Land::UpdateCamera()
{
	// 观察矩阵3要素
	XMVECTOR pos = DXUtil::SphericalToCartesian(m_Radius, m_Theta, m_Phi);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// 重新计算观察矩阵
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);

	return;
}

void Land::UpdateObjectCBs()
{
	auto currObjectCB = CurrentFrameResource()->ObjectCB.get();

	for (auto& ri : m_AllRitems)
	{
		if (ri->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&ri->World);

			FrameResourceLand::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(ri->ObjCBIndex, objConstants);

			ri->NumFramesDirty--;
		}
	}

	return;
}

void Land::UpdatePassCBs()
{
	auto passCBs = CurrentFrameResource()->PassCB.get();

	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	FrameResourceLand::PassConstants passConstants;
	XMStoreFloat4x4(&passConstants.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&passConstants.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(viewProj));

	passCBs->CopyData(0, passConstants);

	return;
}

void Land::UpdateWaves()
{
	KClock* pClock = KApplication::GetApp()->GetClock();

	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((pClock->TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = KMaths::Rand(4, m_Waves->RowCount() - 5);
		int j = KMaths::Rand(4, m_Waves->ColCount() - 5);

		float r = KMaths::Rand(0.2f, 1.5f);

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
		v.Color = XMFLOAT4(Colors::Blue);

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	m_WavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();

	return;
}

// 构造陆地几何体
void Land::BuildLandGeometry()
{
	m_meshName = "grid";

	m_Theta = XM_PI * (3.0f / 2);
	m_Phi = XM_PI / 5;
	m_Radius = 200.0f;

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

		// 根据顶点的高度给顶点涂上颜色
		if (false) {}
		else if (pVertex->Position.y < -10.0f)
		{
			// Sandy beach color.
			pVertex->Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (pVertex->Position.y < 5.0f)
		{
			// Light yellow-green.
			pVertex->Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (pVertex->Position.y < 12.0f)
		{
			// Dark yellow-green.
			pVertex->Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (pVertex->Position.y < 20.0f)
		{
			// Dark brown.
			pVertex->Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			pVertex->Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = grid.IndexCount();
	gridSubmesh.StartIndexLocation = 0;
	gridSubmesh.BaseVertexLocation = 0;

	auto geo = std::make_unique<MeshGeometry>();
	geo->Init(m_device.Get(), m_CommandList.Get(), &grid);
	geo->DrawArgs[m_meshName] = SubmeshGeometry(grid.IndexCount(), 0, 0);

	m_Geometries["LGEOs"] = std::move(geo);

	return;
}

// 构造波浪几何体
void Land::BuildWavesGeometry()
{
	m_meshName = "waves";

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
	geo->DrawArgs[m_meshName] = SubmeshGeometry(mesh.IndexCount(), 0, 0);

	// Set dynamically.(动态波浪)
	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	m_Geometries["WGEOs"] = std::move(geo);

	return;
}

// 把几何体转换为渲染项目，主要目的是重用几何体，

// 通过改变几何体的世界坐标组合构成更加复杂的几何体

// 以备将来绘制输出使用 - DrawRenderItems()
void Land::BuildRenderItems()
{
	int objCBIndex = 0;

	auto gridRitem = std::make_unique<RenderItem>(m_FrameResourceCount);
	{
		_astring daname = "grid";

		gridRitem->ObjCBIndex = objCBIndex++;
		gridRitem->World = DXUtil::Identity4x4;
		gridRitem->Geo = m_Geometries["LGEOs"].get();
		gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRitem->IndexCount = gridRitem->Geo->DrawArgs[daname].IndexCount;
		gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs[daname].StartIndexLocation;
		gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs[daname].BaseVertexLocation;
	}
	m_AllRitems.push_back(std::move(gridRitem));

	auto wavesRitem = std::make_unique<RenderItem>(m_FrameResourceCount);
	{
		_astring daname = "waves";

		wavesRitem->ObjCBIndex = objCBIndex++;
		wavesRitem->World = DXUtil::Identity4x4;
		wavesRitem->Geo = m_Geometries["WGEOs"].get();
		wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs[daname].IndexCount;
		wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs[daname].StartIndexLocation;
		wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs[daname].BaseVertexLocation;
	}
	m_WavesRitem = wavesRitem.get();
	m_AllRitems.push_back(std::move(wavesRitem));

	return;
}

// 构造帧资源
void Land::BuildFrameResources()
{
	for (int i = 0; i < m_FrameResourceCount; i++)
	{
		m_FrameResources.push_back
		(
			std::make_unique<FrameResourceLand>(m_device.Get(),
				m_AllRitems.size(),
				m_Waves->VertexCount()
				)
		);
	}

	return;
}


// 绘制渲染项
void Land::DrawRenderItems()
{
	// 必须
	{
		// 设置根描述表
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 视图和剪切矩形
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出合并
		m_CommandList->OMSetRenderTargets(1, &RenderTargetView(), FALSE, nullptr);
	}


	// 清除背景
	{
		// 清除渲染背景
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_CommandList->ClearRenderTargetView(RenderTargetView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(),
			clearFlags, 1.0f, 0, 0, nullptr);
	}

	// 绘制
	{
		// 绑定过程常量寄存器(b1)
		auto passCB = CurrentFrameResource()->PassCB->Resource();
		m_CommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	}


	// DrawRenderItems
	auto currObjCB = CurrentFrameResource()->ObjectCB->Resource();
	UINT objCBByteSize = DXUtil::CalculateConstantBufferByteSize(sizeof(FrameResourceLand::ObjectConstants));
	for (size_t i = 0; i < m_AllRitems.size(); i++)
	{
		auto ri = m_AllRitems[i].get();

		// 绑定对象常量寄存器(b0)
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = currObjCB->GetGPUVirtualAddress();
		objCBAddress += ri->ObjCBIndex * objCBByteSize;
		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

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


float Land::GetHillsHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}

XMFLOAT3 Land::GetHillsNormal(float x, float z) const
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









