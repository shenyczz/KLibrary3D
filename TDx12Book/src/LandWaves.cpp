#include "stdafx.h"
#include "LandWaves.h"


LandWaves::LandWaves()
{
	m_Theta = 1.5f*XM_PI;
	m_Phi = XM_PIDIV2 - 0.1f;
	m_Radius = 50.0f;

	m_FrameResourceCount = 3;
}

LandWaves::~LandWaves()
{
}


//【鼠标消息】 - Mouse
void LandWaves::OnMouseDown(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) && (wParam & MK_CONTROL))
	{
		m_IsWireframe = !m_IsWireframe;
	}

	Dx12Book::OnMouseDown(wParam, x, y);
}
void LandWaves::OnMouseUp(WPARAM wParam, int x, int y)
{
	Dx12Book::OnMouseUp(wParam, x, y);
}
void LandWaves::OnMouseMove(WPARAM wParam, int x, int y)
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

//【构造资产】 - Build
void LandWaves::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();

	BuildLandGeometry();
	BuildWavesGeometry();

	BuildRenderItems();
	BuildFrameResources();

	return;
}
void LandWaves::BuildRootSignature()
{
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBV.
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		2, slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
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
		0,							// nodeMask
		rootSig->GetBufferPointer(),
		rootSig->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}
void LandWaves::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("LandAndWaves.hlsl");
		_tstring filePath = path + _T("Assets\\") + file;

		//auto geo = std::make_unique<MeshGeometry>();

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
void LandWaves::BuildPipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	opaquePsoDesc.pRootSignature = m_RootSignature.Get();
	opaquePsoDesc.VS = { m_Shaders["VS"]->GetBufferPointer(), m_Shaders["VS"]->GetBufferSize() };
	opaquePsoDesc.PS = { m_Shaders["PS"]->GetBufferPointer(), m_Shaders["PS"]->GetBufferSize() };
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_BackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = m_DepthStencilFormat;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["opaque"])));

	//
	// PSO for opaque wireframe objects.
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&m_PSOs["opaque_wireframe"])));
}
void LandWaves::BuildLandGeometry()
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
	geo->DrawArgs[meshName] = SubmeshGeometry(grid.IndexCount(), 0, 0);

	m_Geometries["landGeo"] = std::move(geo);

	return;
}
void LandWaves::BuildWavesGeometry()
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
void LandWaves::BuildRenderItems()
{
	int index = 0;

	auto wavesRitem = std::make_unique<FrameResourceLaw::RenderItem>(m_FrameResourceCount);
	{
		wavesRitem->ObjCBIndex = index++;
		wavesRitem->World = DXUtils::Identity4x4;
		wavesRitem->Geo = m_Geometries["waterGeo"].get();
		wavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
		wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
		wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	}

	// 保存指针用于设置动态顶点
	m_WavesRitem = wavesRitem.get();
	m_AllRitems.push_back(std::move(wavesRitem));

	auto gridRitem = std::make_unique<FrameResourceLaw::RenderItem>(m_FrameResourceCount);
	{
		gridRitem->ObjCBIndex = index++;
		gridRitem->World = DXUtils::Identity4x4;
		gridRitem->Geo = m_Geometries["landGeo"].get();
		gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
		gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
		gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	}

	m_AllRitems.push_back(std::move(gridRitem));

	return;
}
void LandWaves::BuildFrameResources()
{
	for (int i = 0; i < m_FrameResourceCount; ++i)
	{
		m_FrameResources.push_back
		(
			std::make_unique<FrameResourceLaw>(m_device.Get(),
				1, (UINT)m_AllRitems.size(), m_Waves->VertexCount())
		);
	}
}

//【窗口改变】 - Resized
void LandWaves::Resized()
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*(float)KMaths::PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}


//【数据更新】 - Update
void LandWaves::Update()
{
	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % m_FrameResourceCount;
	mCurrFrameResource = m_FrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->FenceValue != 0 && m_Fence->GetCompletedValue() < mCurrFrameResource->FenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(mCurrFrameResource->FenceValue, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateCamera();
	UpdateObjectCBs();
	UpdateMainPassCB();
	UpdateWaves();
}
void LandWaves::UpdateCamera()
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
void LandWaves::UpdateObjectCBs()
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : m_AllRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->Dirtys > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			FrameResourceLaw::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->Dirtys--;
		}
	}
}
void LandWaves::UpdateMainPassCB()
{
	KClock* pClock = KApplication::GetApp()->GetClock();
	int iClientWidth = m_pWindow->ClientWidth();
	int iClientHeight = m_pWindow->ClientHeight();

	auto currPassCB = mCurrFrameResource->PassCB.get();

	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	FrameResourceLaw::PassConstants passCB;
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

	currPassCB->CopyData(0, passCB);
}
void LandWaves::UpdateWaves()
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
	auto currWavesVB = mCurrFrameResource->WavesVB.get();
	for (int i = 0; i < m_Waves->VertexCount(); ++i)
	{
		Vertex v;

		v.Position = m_Waves->Position(i);
		v.Color = XMFLOAT4(DirectX::Colors::Blue);

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	m_WavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();
}

//【渲染绘制】 - Draw
void LandWaves::Draw()
{
	// 数据准备
	auto pCommandAllocator = mCurrFrameResource->CmdListAllocator.Get();
	auto pCurrentPso = m_IsWireframe ? m_PSOs["opaque_wireframe"].Get() : m_PSOs["opaque"].Get();

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

		// 设置根签名
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 输出装配
		m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// 清除渲染背景
		const float* clearColor = Colors::LightSkyBlue;
		m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		m_CommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		{
			// 绑定过程资源到常量缓冲区
			auto passCB = mCurrFrameResource->PassCB->Resource();
			m_CommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());	// b1

			DrawRenderItems();
		}

		// 资源屏障 - 由渲染转换为呈现状态
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}

	// 完成记录命令
	ThrowIfFailed(m_CommandList->Close());

	// 把命令列表加入命令队列
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 交换前后台缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	m_BackBufferIndex = (m_BackBufferIndex + 1) % BackBufferCount;

	// 将围栏值向前推进，将命令标记到此围栏点
	mCurrFrameResource->FenceValue = ++m_FenceValue;

	// 向命令队列中添加一条指令，以设置新的围栏点。
	// 因为我们在GPU时间轴上，在GPU处理完之前的所有命令之前，
	// 此Signal()将不会设置新的围栏点。
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);

	return;
}
void LandWaves::DrawRenderItems()
{
	auto currObjCB = mCurrFrameResource->ObjectCB->Resource();
	UINT objCBByteSize = DXUtils::CalculateConstantBufferByteSize(sizeof(FrameResourceLaw::ObjectConstants));

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

//【其他功能】
float LandWaves::GetHillsHeight(float x, float z)const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}
XMFLOAT3 LandWaves::GetHillsNormal(float x, float z)const
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
