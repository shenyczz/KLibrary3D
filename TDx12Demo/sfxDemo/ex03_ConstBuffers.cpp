#include "stdafx.h"
#include "ex03_ConstBuffers.h"


ConstBuffers::ConstBuffers()
	: m_ConstantBuffer(nullptr)
	, m_ConstantObject{}
	, m_pCbvDataBegin(nullptr)
{
}

ConstBuffers::~ConstBuffers()
{
}


void ConstBuffers::InitializeGraphics()
{
	Dx12Sample::InitializeGraphics();
}

void ConstBuffers::InitializeAssets()
{
	Dx12Sample::InitializeAssets();
}

void ConstBuffers::WindowResized()
{
	Dx12Sample::WindowResized();
}

void ConstBuffers::UpdateParameter()
{
	//Dx12Sample::UpdateParameter();
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	m_ConstantObject.offset.x += translationSpeed;
	if (m_ConstantObject.offset.x > offsetBounds)
	{
		m_ConstantObject.offset.x = -offsetBounds;
	}
	memcpy(m_pCbvDataBegin, &m_ConstantObject, sizeof(m_ConstantObject));
}

void ConstBuffers::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(m_CommandAllocator[m_frameIndex]->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator[m_frameIndex].Get(), m_PipelineState.Get()));

	// Set necessary state.
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	// m_cbvHeap
	ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// m_RtvHeap
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 输入装配
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

	m_CommandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
}




void ConstBuffers::CreateRootSignature()
{
	// 根签名的特征数据
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

	// 允许输入布局并拒绝对某些管道阶段的必要访问
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), &rootParameters[0], 0, nullptr, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
		featureData.HighestVersion, &signature, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
}

void ConstBuffers::BuildByteCode()
{
	UINT compileFlags = DXUtil::GetCompileFlags();

	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\ConstBuffers.hlsl");
	_tstring filePath = path + file;

	ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
		"VSMain", "vs_5_0", compileFlags, 0, &m_VSByteCode, nullptr));

	ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
		"PSMain", "ps_5_0", compileFlags, 0, &m_PSByteCode, nullptr));
}

void ConstBuffers::BuildVertexBufferAndView()
{
	float aspectRatio = this->GetAspectRatio();

	Vertex triangleVertices[] =
	{
		{ { 0.0f, 0.25f * aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.25f, -0.25f * aspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { -0.25f, -0.25f * aspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	const UINT vertexBufferSize = sizeof(triangleVertices);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)));

	// Initialize the vertex buffer view.
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = vertexBufferSize;

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
	m_VertexBuffer->Unmap(0, nullptr);
}

void ConstBuffers::BuildIndexBufferAndView()
{
	Dx12Sample::BuildIndexBufferAndView();
}

void ConstBuffers::BuildConstantBufferAndView()
{
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),			// ??
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_ConstantBuffer)));

	// Map and initialize the constant buffer.
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
	memcpy(m_pCbvDataBegin, &m_ConstantObject, sizeof(m_ConstantObject));
	// We don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.
	// Unmap 也 ok???
	m_ConstantBuffer->Unmap(0, nullptr);

	// Describe and create a constant buffer view.
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(m_ConstantBuffer) + 255) & ~255;	// CB size is required to be 256-byte aligned.
	m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	return;
}

void ConstBuffers::BuildInputElementDescrips()
{
	// 输入布局
	m_InputElementDescs =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void ConstBuffers::BuildPipelineStateObject()
{
	// 输入布局
	D3D12_INPUT_LAYOUT_DESC inputLayout = InputLayout();

	// pipeline state object desc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	{
		psoDesc.pRootSignature = m_RootSignature.Get();

		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_VSByteCode.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_PSByteCode.Get());

		//psoDesc.StreamOutput = D3D12_STREAM_OUTPUT_DESC();
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

		// DepthStencilState
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

		psoDesc.InputLayout = inputLayout;
		//psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;							// DXGI_FORMAT_R32G32B32_FLOAT
																					//psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;									// 深度模板格式1
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };								// 采样描述
		psoDesc.NodeMask = 0;
		//psoDesc.CachedPSO = D3D12_CACHED_PIPELINE_STATE();
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}

	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

	return;
}

