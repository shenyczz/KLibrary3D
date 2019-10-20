#include "stdafx.h"
#include "HelloConstBuffers.h"


HelloConstBuffers::HelloConstBuffers()
{
}


HelloConstBuffers::~HelloConstBuffers()
{
	m_ConstantBuffer->Unmap(0, nullptr);
}


void HelloConstBuffers::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();
	BuildTriangleGeometry();
	BuildVertexBuffer();
	BuildConstantBufferAndView();
}
void HelloConstBuffers::BuildRootSignature()
{
	// ��Ҫһ�������Ĵ���

	// ��������Χ
	D3D12_DESCRIPTOR_RANGE range = {};
	{
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	// ��������
	D3D12_ROOT_DESCRIPTOR_TABLE cbvTable = {};
	{
		cbvTable.NumDescriptorRanges = 1;
		cbvTable.pDescriptorRanges = &range;
	}

	// ������
	D3D12_ROOT_PARAMETER rootParameters = {};
	{
		rootParameters.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters.DescriptorTable = cbvTable;
		rootParameters.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	}

	// ��ǩ��������
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	{
		rootSigDesc.NumParameters = 1;
		rootSigDesc.pParameters = &rootParameters;
		rootSigDesc.NumStaticSamplers = 0;
		rootSigDesc.pStaticSamplers = nullptr;
		rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}
	
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&signature, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void HelloConstBuffers::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("studyColors.hlsl");
		_tstring filePath = path + _T("Assets\\") + file;

		m_Shaders["VS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "VS_CB", "vs_5_1");
		m_Shaders["PS"] = DXUtil::CompileShader(filePath.c_str(), nullptr, "PS", "ps_5_1");
	}

	// InputLyout
	{
		// ��Ҫ����Vertex�ṹ�� hlsl �еĶ�������ṹ��������׼������Ԫ��������
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName			- �������ƣ����� HLSL
			0,											// SemanticIndex		- �������������� HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format				- ��ʽ������ Vertex
			0,											// InputSlot			- ����
			Vertex::OffsetPos,							// AlignedByteOffset	- �ֽ�ƫ�ƣ�
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
void HelloConstBuffers::BuildPipelineState()
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

		// ��դ���״̬ - D3D12_RASTERIZER_DESC
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// ���ģ��״̬ - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		//psoDesc.DepthStencilState.DepthEnable = FALSE;
		//psoDesc.DepthStencilState.StencilEnable = FALSE;

		// ���벼�� - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = InputLayout();

		// �� - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// ͼԪ�������� - D3D12_PRIMITIVE_TOPOLOGY_TYPE
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// RTV��DXGI_FORMAT_R8G8B8A8_UNORM
		psoDesc.NumRenderTargets = BackBufferCount;
		for (int i = 0; i < BackBufferCount; i++)
		{
			psoDesc.RTVFormats[i] = m_BackBufferFormat;
		}

		// DSV��DXGI_FORMAT_D24_UNORM_S8_UINT
		psoDesc.DSVFormat = m_DepthStencilFormat;

		// ������ - DXGI_SAMPLE_DESC
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };

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
void HelloConstBuffers::BuildTriangleGeometry()
{
	MeshFactory::CreateTriangle(1.0, &m_Triangle);
}
void HelloConstBuffers::BuildVertexBuffer()
{
	// ������ʹ���ϴ��������侲̬���ݣ����綥�����ݣ���ÿ��GPU��Ҫ��ʱ���ϴ��ѽ������͡�
	// ���Ķ�Ĭ�϶ѵ�ʹ�������
	// ����ʹ���ϴ�����Ϊ�˼򻯴��룬������Ϊʵ�ʴ���Ķ�����١�

	// ������
	D3D12_HEAP_PROPERTIES heapProperty;
	{
		heapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperty.CreationNodeMask = 1;
		heapProperty.VisibleNodeMask = 1;
	}

	D3D12_RESOURCE_DESC resourceDesc = {};
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = sizeof(Vertex) * m_Triangle.VertexCount();
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc = { 1,0 };
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

	D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr;

	// ����ίԱ����Դ��
	ThrowIfFailed(m_device->CreateCommittedResource(&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		initialResourceState,
		pOptimizedClearValue,
		IID_PPV_ARGS(&m_VertexBuffer)));

	// ���ƶ�������
	// ���ǲ������CPU�ϵ������Դ�ж�ȡ��

	void* pVertexData = nullptr;
	D3D12_RANGE readRange = { 0,0 };
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexData)));
	memcpy(pVertexData, m_Triangle.pvbInitData(), m_Triangle.vbSizeInBytes());
	m_VertexBuffer->Unmap(0, nullptr);

	return;
}
void HelloConstBuffers::BuildConstantBufferAndView()
{
	// 1.�������������� - ConstantBuffer
	UINT nCbvBytesSize = DXUtil::CalculateConstantBufferByteSize(sizeof(SceneConstants));
	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(nCbvBytesSize),	// 256 * 128
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_ConstantBuffer));

	// 2 ����������������ͼ - ConstantBufferView

	// 2.1�ȴ���������������ͼ��CBV�������������ڱ���������
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	{
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
	}
	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));

	// 2.2������������ͼ������
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	{
		cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = nCbvBytesSize;
	}
	m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	// �ϴ�����
	ThrowIfFailed(m_ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pCbvData)));
	//memcpy(m_pCbvData, &m_SceneConstants, sizeof(SceneConstants));
	//m_ConstantBuffer->Unmap(0, nullptr);

	return;
}


void HelloConstBuffers::Update()
{
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	m_SceneConstants.offset.x += translationSpeed;
	if (m_SceneConstants.offset.x > offsetBounds)
	{
		m_SceneConstants.offset.x = -offsetBounds;
	}

	memcpy(m_pCbvData, &m_SceneConstants, sizeof(m_SceneConstants));
}

void HelloConstBuffers::Draw()
{
	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();

	ID3D12PipelineState* pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// ��λ���������������䱣��������¼
	ThrowIfFailed(pCommandAllocator->Reset());

	// ��λ�����б���ʼ��¼����
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(RenderTarget(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// դ�񻯽׶�
		pCommandList->RSSetViewports(1, &m_ScreenViewport);
		pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		// ����ϲ���RTV + DSV
		pCommandList->OMSetRenderTargets(1, &RenderTargetView(), true, &DepthStencilView());

		// �����Ⱦ����
		const float* clearColor = Colors::LightSkyBlue;
		pCommandList->ClearRenderTargetView(RenderTargetView(), clearColor, 0, nullptr);

		// ������ģ����ͼ
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		pCommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// ���ø�ǩ��
		pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 
		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
		pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		pCommandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->IASetVertexBuffers(0, 1, &VertexBufferView());
		pCommandList->DrawInstanced(m_Triangle.VertexCount(), 1, 0, 0);
	}

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(RenderTarget(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// �ر������б�ֹͣ��¼����
	ThrowIfFailed(pCommandList->Close());

	// ���� - presentation

	// �������б��е����������������
	ID3D12CommandList* cmdLists[] = { pCommandList };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// ����ǰ��̨������
	ThrowIfFailed(m_SwapChain->Present(1, 0));

	// 
	WaitForGpu();
}
