#include "stdafx.h"
#include "HelloTexture.h"


HelloTexture::HelloTexture()
{
}

HelloTexture::~HelloTexture()
{
}


void HelloTexture::BuildAssets()
{
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPipelineState();
	BuildTriangleGeometry();
	BuildVertexBuffer();
	BuildTextureBufferAndView();
}
void HelloTexture::BuildRootSignature()
{
	// 需要采样器 - 适用静态采样器

	// 构建根签名
	{
		// D3D12SerializeRootSignature()	- 序列化根签名
		// m_device->CreateRootSignature()	- 创建根签名
	}

	D3D12_DESCRIPTOR_RANGE srvRange = {};
	{
		srvRange.RangeType= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 1;
		srvRange.BaseShaderRegister = 0;
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	D3D12_ROOT_DESCRIPTOR_TABLE srvTable = {};
	{
		srvTable.NumDescriptorRanges = 1;
		srvTable.pDescriptorRanges = &srvRange;
	}


	D3D12_ROOT_PARAMETER rootParameter = {};
	{
		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = srvTable;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	// 静态采样器
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	{
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;				// s0
		sampler.RegisterSpace = 0;				// space0
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignDesc = {};
	{
		rootSignDesc.NumParameters = 1;
		rootSignDesc.pParameters = &rootParameter;
		rootSignDesc.NumStaticSamplers = 1;
		rootSignDesc.pStaticSamplers = &sampler;
		rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}

	D3D_ROOT_SIGNATURE_VERSION version = D3D_ROOT_SIGNATURE_VERSION_1_0;
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignDesc, version, &signature, &error));

	ThrowIfFailed(m_device->CreateRootSignature(0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));

	return;
}
void HelloTexture::BuildShadersAndInputLayout()
{
	// Shaders
	{
		_tstring path = KApplication::GetApp()->Startup();
		_tstring file = _T("studyColors.hlsl");
		_tstring filePath = path + _T("Assets\\") + file;

		m_Shaders["VS"] = DXUtils::CompileShader(filePath.c_str(), nullptr, "VS_Tex", "vs_5_1");
		m_Shaders["PS"] = DXUtils::CompileShader(filePath.c_str(), nullptr, "PS_Tex", "ps_5_1");
	}

	// InputLyout
	{
		// 需要根据Vertex结构和 hlsl 中的顶点输入结构语义索引准备输入元素描述符
		m_InputLayout =
		{
			D3D12_INPUT_ELEMENT_DESC{
			"POSITION",									// SemanticName			- 语义名称：依据 HLSL
			0,											// SemanticIndex		- 语义索引：依据 HLSL
			DXGI_FORMAT_R32G32B32_FLOAT,				// Format				- 格式：依据 Vertex
			0,											// InputSlot			- ，？
			Vertex::OffsetPos,							// AlignedByteOffset	- 字节偏移，
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
void HelloTexture::BuildPipelineState()
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
		//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		// 深度模板状态 - D3D12_DEPTH_STENCIL_DESC
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		//psoDesc.DepthStencilState.DepthEnable = TRUE;
		//psoDesc.DepthStencilState.StencilEnable = TRUE;

		// 输入布局 - D3D12_INPUT_LAYOUT_DESC
		psoDesc.InputLayout = InputLayout();

		// ？ - D3D12_INDEX_BUFFER_STRIP_CUT_VALUE
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// 图元拓扑类型 - D3D12_PRIMITIVE_TOPOLOGY_TYPE
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// RTV，DXGI_FORMAT_R8G8B8A8_UNORM
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = m_BackBufferFormat;

		// DSV，DXGI_FORMAT_D24_UNORM_S8_UINT
		psoDesc.DSVFormat = m_DepthStencilFormat;

		// 采样器 - DXGI_SAMPLE_DESC
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
void HelloTexture::BuildTriangleGeometry()
{
	MeshFactory::CreateTriangle(1.0, &m_Triangle);
}
void HelloTexture::BuildVertexBuffer()
{
	// 不建议使用上传堆来传输静态数据（比如顶点数据），每次GPU需要它时，上传堆将被封送。
	// 请阅读默认堆的使用情况。
	// 这里使用上传堆是为了简化代码，并且因为实际传输的顶点很少。

	// 堆属性
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

	// 创建委员会资源？
	ThrowIfFailed(m_device->CreateCommittedResource(&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		initialResourceState,
		pOptimizedClearValue,
		IID_PPV_ARGS(&m_VertexBuffer)));

	// 复制顶点数据
	// 我们不打算从CPU上的这个资源中读取。

	void* pVertexData = nullptr;
	D3D12_RANGE readRange = { 0,0 };
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexData)));
	memcpy(pVertexData, m_Triangle.pvbInitData(), m_Triangle.vbSizeInBytes());
	m_VertexBuffer->Unmap(0, nullptr);

	return;
}
void HelloTexture::BuildTextureBufferAndView()
{
	// 创建描述符堆 - m_srvHeap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	{
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = 0;
	}
	ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

	//ComPtr是CPU对象，但该资源需要保留在作用域，直到引用它的命令列表在GPU上完成执行。
	//我们将在此方法的末尾刷新GPU，以确保资源不被过早销毁。

	// Create a Texture2D.
	D3D12_RESOURCE_DESC textureDesc = {};
	{
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Alignment = 0;
		textureDesc.Width = TextureWidth;
		textureDesc.Height = TextureHeight;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc = { 1, 0 };
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_Texture)));	// CPU

	// Returns required size of a buffer to be used for data upload(返回用于数据上载的缓冲区的所需大小。)
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture.Get(), 0, 1);

	// Create the GPU upload buffer.
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_TextureUploadHeap)));

	// 将数据复制到中间上载堆，然后将副本从上载堆调度到Texture2D。
	std::vector<UINT8> texture = GenerateTextureData();
	D3D12_SUBRESOURCE_DATA textureData = {};
	{
		textureData.pData = &texture[0];
		textureData.RowPitch = TextureWidth * TexturePixelSize;
		textureData.SlicePitch = textureData.RowPitch * TextureHeight;
	}
	UpdateSubresources(m_CommandList.Get(), m_Texture.Get(), m_TextureUploadHeap.Get(), 0, 0, 1, &textureData);

	// 资源屏障
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	// Create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	{
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
	}
	m_device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());

	return;
}




void HelloTexture::Update()
{
	// no body
}

void HelloTexture::Draw()
{
	ID3D12CommandAllocator* pCommandAllocator = m_CommandAllocator.Get();
	ID3D12GraphicsCommandList* pCommandList = m_CommandList.Get();

	ID3D12PipelineState* pCurrentPso = m_IsWireframe ? m_PSOs["Wireframe"].Get() : m_PSOs["Solid"].Get();

	// 复位命令分配器，清除其保存的命令记录
	ThrowIfFailed(pCommandAllocator->Reset());

	// 复位命令列表，开始记录命令
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator, pCurrentPso));

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(RenderTarget(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	{
		// 设置根签名
		pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// 设置描述符堆
		ID3D12DescriptorHeap* ppHeaps[] = { m_srvHeap.Get() };
		pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// 设置描述符表 - t0
		pCommandList->SetGraphicsRootDescriptorTable(0, m_srvHeap->GetGPUDescriptorHandleForHeapStart());

		// 光栅化阶段 - 设置视口和剪切矩形
		pCommandList->RSSetViewports(1, &m_ScreenViewport);
		pCommandList->RSSetScissorRects(1, &m_ScissorRect);

		// 输出合并：RTV + DSV
		//pCommandList->OMSetRenderTargets(1, &RenderTargetView(), true, &DepthStencilView());
		pCommandList->OMSetRenderTargets(1, &RenderTargetView(), FALSE, &DepthStencilView());

		// 清除渲染背景
		const float* clearColor = Colors::LightSkyBlue;
		pCommandList->ClearRenderTargetView(RenderTargetView(), clearColor, 0, nullptr);

		// 清除深度模板视图
		const auto clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		pCommandList->ClearDepthStencilView(DepthStencilView(), clearFlags, 1.0f, 0, 0, nullptr);

		// 绘制
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->IASetVertexBuffers(0, 1, &VertexBufferView());
		pCommandList->DrawInstanced(m_Triangle.VertexCount(), 1, 0, 0);

	}

	// Indicate a state transition on the resource usage.
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(RenderTarget(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 关闭命令列表，停止记录命令
	ThrowIfFailed(pCommandList->Close());

	// 呈现 - presentation

	// 把命令列表中的命令输入命令队列
	ID3D12CommandList* cmdLists[] = { pCommandList };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// 交换前后台缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));

	// 
	WaitForGpu();
}







std::vector<UINT8> HelloTexture::GenerateTextureData()
{
	const UINT rowPitch = TextureWidth * TexturePixelSize;
	const UINT cellPitch = rowPitch >> 3;					// The width of a cell in the checkboard texture.
	const UINT cellHeight = TextureWidth >> 3;				// The height of a cell in the checkerboard texture.
	const UINT textureSize = rowPitch * TextureHeight;

	std::vector<UINT8> data(textureSize);

	// 填充数据
	UINT8* pData = &data[0];
	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
	{
		UINT x = n % rowPitch;
		UINT y = n / rowPitch;
		UINT i = x / cellPitch;
		UINT j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n] = 0x00;		// R
			pData[n + 1] = 0x00;	// G
			pData[n + 2] = 0x00;	// B
			pData[n + 3] = 0xff;	// A
		}
		else
		{
			pData[n] = 0xff;		// R
			pData[n + 1] = 0xff;	// G
			pData[n + 2] = 0xff;	// B
			pData[n + 3] = 0xff;	// A
		}
	}

	return data;
}
