#include "stdafx.h"
#include "Ademo.h"


Ademo::Ademo()
{
	m_demoType = AdemoType::TRI;
	//m_demoType = AdemoType::QUAD;
	//m_demoType = AdemoType::TRICB;
	//m_demoType = AdemoType::TRIUV;	// 没太明白

	KMarchingCubesTriangleCases* pCases = KMarchingCubesTriangleCases::GetCases();
}
Ademo::~Ademo()
{
}



void Ademo::Resize()
{
}

void Ademo::Update()
{
	switch (m_demoType)
	{
		case AdemoType::TRICB:
			Update_tricb();
			break;
	}
}



void Ademo::CreateRootSignature()
{
	switch (m_demoType)
	{
		case AdemoType::TRI:
			CreateRootSignatureDefault();
			break;
		case AdemoType::QUAD:
			CreateRootSignatureDefault();
			break;

		case AdemoType::TRICB:
			CreateRootSignature_tricb();
			break;
		case AdemoType::TRIUV:
			CreateRootSignature_triuv();
			break;
	}
}

void Ademo::BuildShadersAndInputLayout()
{
	switch (m_demoType)
	{
		case AdemoType::TRI:
			BuildShadersAndInputLayout_tri();
			break;
		case AdemoType::QUAD:
			BuildShadersAndInputLayout_quad();
			break;
		case AdemoType::TRICB:
			BuildShadersAndInputLayout_tricb();
			break;
		case AdemoType::TRIUV:
			BuildShadersAndInputLayout_triuv();
			break;
	}
}

void Ademo::BuildPipelineStateObject()
{
	BuildPipelineStateObjectDefault();
}

void Ademo::BuildMeshData()
{
	switch (m_demoType)
	{
		case AdemoType::TRI:
			BuildMeshData_tri();
			break;
		case AdemoType::QUAD:
			BuildMeshData_quad();
			break;
		case AdemoType::TRICB:
			BuildMeshData_tricb();
			break;
		case AdemoType::TRIUV:
			BuildMeshData_triuv();
			break;
	}
}

void Ademo::BuildConstantBufferAndView()
{
	switch (m_demoType)
	{
		case AdemoType::TRICB:
			BuildConstantBufferAndView_tricb();
			break;
	}
}

void Ademo::BuildTextureBufferAndView()
{
	switch (m_demoType)
	{
		case AdemoType::TRIUV:
			BuildTextureBufferAndView_triuv();
			break;
	}
}

void Ademo::ReccorCommand()
{
	switch (m_demoType)
	{
		case AdemoType::TRI:
			ReccorCommand_tri();
			break;
		case AdemoType::QUAD:
			ReccorCommand_quad();
			break;
		case AdemoType::TRICB:
			ReccorCommand_tricb();
			break;
		case AdemoType::TRIUV:
			ReccorCommand_triuv();
			break;
	}
}




#pragma region DEFAULT

void Ademo::CreateRootSignatureDefault()
{
	// 这里创建一个空的根签名
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(
		0,									// UINT numParameters
		nullptr,							// D3D12_ROOT_PARAMETER* _pParameters
		0,									// UINT numStaticSamplers = 0
		nullptr,							// D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> signature;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	// 下面代码也 ok

	//ID3D12RootSignature* pID3D12RootSignature = nullptr;
	//ThrowIfFailed
	//(
	//	m_device->CreateRootSignature(0,
	//		signature->GetBufferPointer(),
	//		signature->GetBufferSize(),
	//		__uuidof(*pID3D12RootSignature),
	//		reinterpret_cast<void**>(&pID3D12RootSignature))
	//);
	//m_RootSignature.Attach(pID3D12RootSignature);

	return;
}
void Ademo::BuildPipelineStateObjectDefault()
{
	// 图形管线状态（PSO）描述符
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// 根签名指针 - ID3D12RootSignature*
	psoDesc.pRootSignature = m_RootSignature.Get();

	// 着色器字节码 - D3D12_SHADER_BYTECODE
	psoDesc.VS = { m_VSByteCode->GetBufferPointer(), m_VSByteCode->GetBufferSize() };
	psoDesc.PS = { m_PSByteCode->GetBufferPointer(), m_PSByteCode->GetBufferSize() };
	psoDesc.DS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
	psoDesc.HS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
	psoDesc.GS = D3D12_SHADER_BYTECODE{ nullptr, 0 };

	// 流输出 - D3D12_STREAM_OUTPUT_DESC
	psoDesc.StreamOutput = {};

	// 混合状态 - D3D12_BLEND_DESC
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// 光栅输出状态 - D3D12_RASTERIZER_DESC
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

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
		psoDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	// DSV 格式
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	// 采样器 - DXGI_SAMPLE_DESC
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };

	// ?
	psoDesc.NodeMask = 0;

	// D3D12_CACHED_PIPELINE_STATE
	psoDesc.CachedPSO = {};

	// ?
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create PSO
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

	return;
}
void Ademo::BuildShadersAndInputLayoutFromFile(LPCTSTR lpszFileName)
{
	// Shaders
	{
		_tstring filePath = lpszFileName;

		m_VSByteCode = DXUtil::CompileShader(lpszFileName, nullptr, "VS", "vs_5_0");
		m_PSByteCode = DXUtil::CompileShader(lpszFileName, nullptr, "PS", "ps_5_0");

		// m_VSByteCode 是根据顶点输入结构（VertexIn）编译乘的字节码，
		// VertexIn 定义的字段必须在定义的输入元素描述符中找到
		// 输入元素描述符中可以定义 VertexIn 没有声明的语义名称（SemanticName）
	}

	// InputLyout
	{
		// 需要根据Vertex结构和 hlsl 中的顶点输入结构准备输入元素描述符
		UINT offsetPos = Vertex::OffsetPos;
		UINT offsetClr = Vertex::OffsetClr;
		UINT offsetNom = Vertex::OffsetNom;
		UINT offsetTan = Vertex::OffsetTan;
		UINT offsetTex = Vertex::OffsetTex;
		m_InputElementDescs =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName，依据 HLSL
			0,											// SemanticIndex，依据 HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format，依据 Vertex
			0,											// InputSlot，？
			offsetPos,									// AlignedByteOffset，
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// InputSlotClass
			0 },										// InstanceDataStepRate

			D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, offsetClr, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, offsetNom, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			D3D12_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, offsetTan, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

			D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, offsetTex, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}

	return;
}

#pragma endregion




#pragma region DEMO - TRI

void Ademo::CreateRootSignature_tri()
{
	CreateRootSignatureDefault();
}
void Ademo::BuildShadersAndInputLayout_tri()
{
	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\ATriangle.hlsl");
	_tstring filePath = path + file;
	BuildShadersAndInputLayoutFromFile(filePath.c_str());
}
void Ademo::BuildPipelineStateObject_tri()
{
	BuildPipelineStateObjectDefault();
}
void Ademo::BuildMeshData_tri()
{
	// 网格数据
	//MeshFactory::CreateTriangle(1.0f, &m_mesh);
	//MeshFactory::CreateQuad(0.5f, 0.5f, &m_mesh);
	MeshFactory::CreateBox(1.0f, 1.0f, 1.0f, &m_mesh);

	// Vertex Buffer
	{
		const void* pInitData = m_mesh.VBInitData();
		UINT nSizeInBytes = m_mesh.VBSizeInBytes();
		UINT nStrideInBytes = m_mesh.VBStrideInBytes();

		// 不建议使用上传堆来传递静态资源（比如顶点缓冲区），每次GPU需要它，上传堆将被封送。
		// 请阅读默认堆的使用情况，这里使用上传堆是为了简化代码，并且因为实际传输的顶点很少。
		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(nSizeInBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin)));
		memcpy(pDataBegin, pInitData, nSizeInBytes);
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = nSizeInBytes;
		m_VertexBufferView.StrideInBytes = nStrideInBytes;
	}

	// IndexBuffder
	{
		const void* pInitData = m_mesh.IBInitData();
		UINT nSizeInBytes = m_mesh.IBSizeInBytes();

		// 不建议使用上传堆来传递静态资源（比如顶点缓冲区），每次GPU需要它，上传堆将被封送。
		// 请阅读默认堆的使用情况，这里使用上传堆是为了简化代码，并且因为实际传输的顶点很少。
		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(nSizeInBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_IndexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_IndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin)));
		memcpy(pDataBegin, pInitData, nSizeInBytes);
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the index buffer view.
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes = nSizeInBytes;
		m_IndexBufferView.Format = MeshData::IBVFormat;
	}

	return;
}
void Ademo::ReccorCommand_tri()
{
	// 设置根描述表
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex, m_rtvDescriptorSize);

	// 输出合并
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// 清除渲染背景
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 输入装配
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);

	// 绘制
	int indexCount = m_mesh.IndexCount();
	m_CommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

	return;
}

#pragma endregion

#pragma region DEMO - QUAD

void Ademo::CreateRootSignature_quad()
{
	CreateRootSignatureDefault();
}
void Ademo::BuildShadersAndInputLayout_quad()
{
	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\ATriangle.hlsl");
	_tstring filePath = path + file;
	BuildShadersAndInputLayoutFromFile(filePath.c_str());
}
void Ademo::BuildPipelineStateObject_quad()
{
	BuildPipelineStateObjectDefault();
}
void Ademo::BuildMeshData_quad()
{
	MeshFactory::CreateQuad(0.3f, 0.5f, &m_mesh);

	// Vertex Buffer
	{
		const void* pInitData = m_mesh.VBInitData();
		UINT nSizeInBytes = m_mesh.VBSizeInBytes();
		UINT nStrideInBytes = m_mesh.VBStrideInBytes();

		// 不建议使用上传堆来传递静态资源（比如顶点缓冲区），每次GPU需要它，上传堆将被封送。
		// 请阅读默认堆的使用情况，这里使用上传堆是为了简化代码，并且因为实际传输的顶点很少。
		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(nSizeInBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin)));
		memcpy(pDataBegin, pInitData, nSizeInBytes);
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = nSizeInBytes;
		m_VertexBufferView.StrideInBytes = nStrideInBytes;
	}

	// IndexBuffder
	{
		const void* pInitData = m_mesh.IBInitData();
		UINT nSizeInBytes = m_mesh.IBSizeInBytes();

		// 不建议使用上传堆来传递静态资源（比如顶点缓冲区），每次GPU需要它，上传堆将被封送。
		// 请阅读默认堆的使用情况，这里使用上传堆是为了简化代码，并且因为实际传输的顶点很少。
		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(nSizeInBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_IndexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_IndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin)));
		memcpy(pDataBegin, pInitData, nSizeInBytes);
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the index buffer view.
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes = nSizeInBytes;
		m_IndexBufferView.Format = MeshData::IBVFormat;
	}

	return;
}
void Ademo::ReccorCommand_quad()
{
	// 设置根描述表
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex, m_rtvDescriptorSize);

	// 输出合并
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// 清除渲染背景
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 输入装配
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);

	// 绘制
	int indexCount = m_mesh.IndexCount();
	m_CommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

	return;
}

#pragma endregion

#pragma region DEMO - TRICB

void Ademo::CreateRootSignature_tricb()
{
	// 创建由带有单个CBV的描述符表组成的根签名

	// 描述符范围
	D3D12_DESCRIPTOR_RANGE descRanges[1] = {};
	{
		descRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;		// => cbuffer
		descRanges[0].NumDescriptors = 1;								// 数量
		descRanges[0].BaseShaderRegister = 0;							// b0开始
		descRanges[0].RegisterSpace = 0;								// 注册空间 spce0
		//descRanges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;	// 
		descRanges[0].OffsetInDescriptorsFromTableStart =				// 
			D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	// 根描述符表
	D3D12_ROOT_DESCRIPTOR_TABLE rootDescriptorTable = {};
	{
		rootDescriptorTable.NumDescriptorRanges = 1;
		rootDescriptorTable.pDescriptorRanges = &descRanges[0];
	}

	// 根参数
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	{
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].DescriptorTable = rootDescriptorTable;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	// 根签名标志
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE
		| D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
		;

	// 根签名描述符
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	{
		rootSignatureDesc.NumParameters = 1;
		rootSignatureDesc.pParameters = &rootParameters[0];
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = rootSignatureFlags;
	}

	// 根签名特征数据
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_0 };

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> signature;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void Ademo::BuildShadersAndInputLayout_tricb()
{
	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\ATriangleCB.hlsl");
	_tstring filePath = path + file;
	BuildShadersAndInputLayoutFromFile(filePath.c_str());
}
void Ademo::BuildPipelineStateObject_tricb()
{
	BuildPipelineStateObjectDefault();
}
void Ademo::BuildMeshData_tricb()
{
	BuildMeshData_tri();
	//BuildMeshData_quad();
}
void Ademo::BuildConstantBufferAndView_tricb()
{
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),	// ?
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_ConstantBuffer)));

	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	//UINT8* m_pDataBegin = nullptr;
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));	// 映射
	memcpy(m_pCbvDataBegin, &m_cbData, sizeof(TCB));	// 拷贝到常量结构
	//m_ConstantBuffer->Unmap(0, nullptr);

	// Describe and create a constant buffer view.
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = DXUtil::CalcConstantBufferByteSize(sizeof(TCB));
	m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}
void Ademo::ReccorCommand_tricb()
{
	// 设置根描述表
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	// 设置描述符堆
	ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 设置描述符表
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

	// 渲染视图句柄
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex, m_rtvDescriptorSize);

	// 输出合并
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// 清除渲染背景
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 输入装配
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);

	// 绘制
	int indexCount = m_mesh.IndexCount();
	m_CommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

	return;
}
void Ademo::Update_tricb()
{
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	m_cbData.offset.x += translationSpeed;
	if (m_cbData.offset.x > offsetBounds)
	{
		m_cbData.offset.x= -offsetBounds;
	}

	memcpy(m_pCbvDataBegin, &m_cbData, sizeof(TCB));

	return;

}

#pragma endregion

#pragma region DEMO - TRIUV

void Ademo::CreateRootSignature_triuv()
{
	// 描述范围
	D3D12_DESCRIPTOR_RANGE ranges[1] = {};
	{
		ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// => Texture2D
		ranges[0].NumDescriptors = 1;							// 1个
		ranges[0].BaseShaderRegister = 0;						// t0
		ranges[0].RegisterSpace = 0;							// space0
		ranges[0].OffsetInDescriptorsFromTableStart = -1;		// ( 0xffffffff ) D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
	}

	// 根描述符表
	D3D12_ROOT_DESCRIPTOR_TABLE desTable = {};
	{
		desTable.NumDescriptorRanges = 1;
		desTable.pDescriptorRanges = &ranges[0];
	}

	// 跟参数
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	{
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].DescriptorTable = desTable;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	// 静态采样器描述 => SamplerState
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	{
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;	// ?
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;									// ?
		sampler.MaxAnisotropy = 0;								// 异项性
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// 
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;				// s0
		sampler.RegisterSpace = 0;				// space0
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	// 跟签名标志
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE
		| D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		//| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		//| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		//| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		//| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
		;

	// 根签名描述
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	{
		rootSignatureDesc.NumParameters = 1;
		rootSignatureDesc.pParameters = &rootParameters[0];
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.pStaticSamplers = &sampler;
		rootSignatureDesc.Flags = rootSignatureFlags;
	}

	// 根特征数据
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_0 };

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> signature;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void Ademo::BuildShadersAndInputLayout_triuv()
{
	_tstring path = KApplication::GetApp()->Startup();
	_tstring file = _T("Asserts\\ATriangleUV.hlsl");
	_tstring filePath = path + file;
	BuildShadersAndInputLayoutFromFile(filePath.c_str());

}
void Ademo::BuildPipelineStateObject_triuv()
{
	BuildPipelineStateObjectDefault();
}
void Ademo::BuildMeshData_triuv()
{
	BuildMeshData_tri();
}
void Ademo::BuildTextureBufferAndView_triuv()
{
	// 如何 Upload Texture?

	// Describe and create a Texture2D.
	D3D12_RESOURCE_DESC textureDesc = {};
	{
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//textureDesc.Alignment = 0;
		textureDesc.Width = TextureWidth;
		textureDesc.Height = TextureHeight;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc = { 1, 0 };
		//textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_Texture)));

	// 262144
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture.Get(), 0, 1);

	// Create the GPU upload buffer.
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_TextureUpload)));

	// Copy data to the intermediate upload heap and then schedule a copy 
	// from the upload heap to the Texture2D.
	std::vector<UINT8> texture = GenerateTextureData();

	D3D12_SUBRESOURCE_DATA textureData = {};
	{
		textureData.pData = &texture[0];
		textureData.RowPitch = TextureWidth * TexturePixelSize;
		textureData.SlicePitch = textureData.RowPitch * TextureHeight;
	}

	// 
	UpdateSubresources(m_CommandList.Get(), m_Texture.Get(), m_TextureUpload.Get(), 0, 0, 1, &textureData);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// Describe and create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	{
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
	}

	// SRV
	m_device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());

	return;
}
void Ademo::ReccorCommand_triuv()
{
	// 设置根描述表
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	// 设置描述符堆
	ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_srvHeap->GetGPUDescriptorHandleForHeapStart());

	// 设置描述符表
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_srvHeap->GetGPUDescriptorHandleForHeapStart());

	// 渲染视图句柄
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex, m_rtvDescriptorSize);

	// 输出合并
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// 清除渲染背景
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// 输入装配
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);

	// 绘制
	int indexCount = m_mesh.IndexCount();
	m_CommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

	return;
}




// Generate a simple black and white checkerboard texture.
std::vector<UINT8> Ademo::GenerateTextureData()
{
	// 256 X 256

	// 行距(每个像素4字节->RGBA)
	const UINT rowPitch = TextureWidth * TexturePixelSize;

	// 纹理字节大小
	const UINT textureSize = rowPitch * TextureHeight;

	// The width of a cell in the checkboard texture（?）单元个间距
	const UINT cellPitch = rowPitch >> 3;

	// The height of a cell in the checkerboard texture.
	//const UINT cellHeight = TextureWidth >> 3;
	const UINT cellHeight = TextureHeight >> 3;

	std::vector<UINT8> data(textureSize);
	UINT8* pData = &data[0];

	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
	{
		UINT X = n % rowPitch;	// 列
		UINT Y = n / rowPitch;	// 行

		UINT j = X / cellPitch;
		UINT i = Y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n + 0] = 0x00;	// R
			pData[n + 1] = 0x00;	// G
			pData[n + 2] = 0x00;	// B
			pData[n + 3] = 0xff;	// _Ty
		}
		else
		{
			pData[n + 0] = 0xff;	// R
			pData[n + 1] = 0xff;	// G
			pData[n + 2] = 0xff;	// B
			pData[n + 3] = 0xff;	// _Ty
		}

	}

	return data;
}


#pragma endregion

