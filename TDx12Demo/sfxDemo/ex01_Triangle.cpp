#include "stdafx.h"
#include "ex01_Triangle.h"


Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::InitializeGraphics()
{
	Dx12Sample::InitializeGraphics();
}

void Triangle::InitializeAssets()
{
	Dx12Sample::InitializeAssets();
}


void Triangle::WindowResized()
{
	Dx12Sample::WindowResized();
}

void Triangle::UpdateParameter()
{
	Dx12Sample::UpdateParameter();
}

void Triangle::PopulateCommandList()
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

	{
		// 设置根描述表
	}

	// 设置视图
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	{
		// 输出合并
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
		m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		m_CommandList->ClearRenderTargetView(rtvHandle, DXColors::ClearColorRGBA.data(), 0, nullptr);

		// 输入装配
		m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

		// 绘制
		m_CommandList->DrawInstanced(3, 1, 0, 0);
	}
	// Indicate that the back buffer will now be used to present.
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
	return;
}






void Triangle::CreateRootSignature()
{
	Dx12Sample::CreateRootSignature();
}

void Triangle::BuildByteCode()
{
	UINT compileFlags = DXUtil::GetCompileFlags();

	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\Triangle.hlsl");
	_tstring filePath = path + file;

	//ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
	//	"VSMain", "vs_5_0", compileFlags, 0, &m_VSByteCode, nullptr));

	m_VSByteCode = DXUtil::CompileShader(filePath, nullptr, "VSMain", "vs_5_0");

	ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
		"PSMain", "ps_5_0", compileFlags, 0, &m_PSByteCode, nullptr));

	return;
}

void Triangle::BuildInputElementDescrips()
{
	//struct D3D12_INPUT_ELEMENT_DESC
	//{
	//	LPCSTR SemanticName;						// 语义名称（HLSL中的定义）
	//	UINT SemanticIndex;							// 语义索引
	//	DXGI_FORMAT Format;							// DXGI_FORMAT
	//	UINT InputSlot;								// 输入插槽
	//	UINT AlignedByteOffset;						// 偏移字节数
	//	D3D12_INPUT_CLASSIFICATION InputSlotClass;	// 输入插槽类别
	//	UINT InstanceDataStepRate;
	//}

	UINT offsetPos = 0;
	UINT offsetClr = sizeof(Vertex::position);	// Vertex::position
	m_InputElementDescs =
	{
		D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetPos, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetClr, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return;
}

void Triangle::BuildPipelineStateObject()
{
	// 输入布局
	D3D12_INPUT_LAYOUT_DESC inputLayout = InputLayout();

	// pipeline state object desc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
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

	// Create PSO
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

	return;
}



void Triangle::BuildVertexBufferAndView()
{
	float fAaspectRatio = GetAspectRatio();
	fAaspectRatio = 1;
	// Create the vertex buffer.
	// Define the geometry for a triangle.
	Vertex vertices[] =
	{
		Vertex{ XMFLOAT3(+0.00f, +0.25f * fAaspectRatio, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		Vertex{ XMFLOAT3(+0.25f, -0.25f * fAaspectRatio, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		Vertex{ XMFLOAT3(-0.25f, -0.25f * fAaspectRatio, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	};

	const UINT vertexBufferSize = sizeof(vertices);

	// 不建议这么处理
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

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, vertices, sizeof(vertices));
	m_VertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.	VBV
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = vertexBufferSize;

	return;
}

void Triangle::BuildIndexBufferAndView()
{
	Dx12Sample::BuildIndexBufferAndView();
}

void Triangle::BuildConstantBufferAndView()
{
	Dx12Sample::BuildConstantBufferAndView();
}









