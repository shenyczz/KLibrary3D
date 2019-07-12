#include "stdafx.h"
#include "MeshGeometry.h"

namespace DX12
{

	MeshGeometry::MeshGeometry()
		: Name(_T(""))
		, VertexBufferCPU(nullptr)
		, IndexBufferCPU(nullptr)
		, VertexBufferGPU(nullptr)
		, IndexBufferGPU(nullptr)
		, VertexBufferUploader(nullptr)
		, IndexBufferUploader(nullptr)
		, VertexByteStride(0)
		, VertexBufferByteSize(0)
		, IndexBufferByteSize(0)
		, IndexFormat(DXGI_FORMAT_R16_UINT)	// R16
	{
	}


	MeshGeometry::~MeshGeometry()
	{
	}


	D3D12_VERTEX_BUFFER_VIEW MeshGeometry::VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};

		try
		{
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.SizeInBytes = VertexBufferByteSize;
			vbv.StrideInBytes = VertexByteStride;
		}
		catch (const std::exception&)
		{
		}

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW MeshGeometry::IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv = {};

		try
		{
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.SizeInBytes = IndexBufferByteSize;
			ibv.Format = IndexFormat;
		}
		catch (const std::exception&)
		{
		}

		return ibv;
	}

	void MeshGeometry::DisposeUploader()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}


}//DX12
