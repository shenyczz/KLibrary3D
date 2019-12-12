/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

//#include "MeshData.h"

namespace dx12
{
	using namespace dxCommon;

	struct SubmeshGeometry
	{
		SubmeshGeometry()
			: IndexCount(0)
			, StartIndexLocation(0)
			, BaseVertexLocation(0) {}

		SubmeshGeometry(int _indexCount, int _startIndexLocation, int _baseVertexLocation)
			: IndexCount(_indexCount)
			, StartIndexLocation(_startIndexLocation)
			, BaseVertexLocation(_baseVertexLocation) {}

		SubmeshGeometry(int _indexCount, int _startIndexLocation, int _baseVertexLocation, const BoundingBox& bbox)
			: IndexCount(_indexCount)
			, StartIndexLocation(_startIndexLocation)
			, BaseVertexLocation(_baseVertexLocation)
			, Bounds(bbox) {}

		// 索引数量
		UINT IndexCount;

		// 索引开始位置
		UINT StartIndexLocation;

		// 基准顶点位置
		UINT BaseVertexLocation;

		// 包围盒
		BoundingBox Bounds;

		//@EndOf(SubmeshGeometry)
	};

}//dx12



namespace dx12
{
	// 网格几何体
	// MeshGeometry 可以把多个几何体的顶点数据和索引数据分别存储在大的顶点缓冲区和索引缓冲区
	//
	struct DX12LIBCOMMON_EXPORTS_CLASS MeshGeometry
	{
	public:
		MeshGeometry()
			: MeshGeometry("") {}

		MeshGeometry(_astring name)
			: Name(name)
			, VertexBufferCPU(nullptr), IndexBufferCPU(nullptr)
			, VertexBufferGPU(nullptr), IndexBufferGPU(nullptr)
			, VertexBufferUploader(nullptr), IndexBufferUploader(nullptr)
			, VBSizeInBytes(0), VBStrideInBytes(0)
			, IBSizeInBytes(0), IBVFormat(DXGI_FORMAT_UNKNOWN) {}

		~MeshGeometry() { DisposeUploader(); }

	public:
		void Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList, MeshData* pMeshData);

		// 顶点缓冲区视图
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv = {};

			try
			{
				vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
				vbv.SizeInBytes = VBSizeInBytes;
				vbv.StrideInBytes = VBStrideInBytes;
			}
			catch (const std::exception&)
			{
			}

			return vbv;
		}

		// 索引缓冲区视图
		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
		{
			D3D12_INDEX_BUFFER_VIEW ibv = {};

			try
			{
				ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
				ibv.SizeInBytes = IBSizeInBytes;
				ibv.Format = IBVFormat;
			}
			catch (const std::exception&)
			{
			}

			return ibv;
		}

		void DisposeUploader()
		{
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}



	public:
		// 几何体网格名称，可以根据此名称找到它
		_astring Name;

		// 系统内存中的副本。
		// 由于顶点（索引）可以是泛型格式（具体格式由用户定义），
		// 所以使用Blob类型来表示。用户使用时需将其转换为适当类型

		UINT VBSizeInBytes;								// 顶点缓冲区字节大小
		UINT VBStrideInBytes;							// 顶点跨度字节数
		ComPtr<ID3DBlob> VertexBufferCPU;				// VBCPU
		ComPtr<ID3D12Resource> VertexBufferGPU;			// VBGPU


		UINT IBSizeInBytes;								// 索引缓冲区字节大小
		DXGI_FORMAT IBVFormat;							// 索引格式
		ComPtr<ID3DBlob> IndexBufferCPU;				// IBCPU
		ComPtr<ID3D12Resource> IndexBufferGPU;			// IBGPU


		ComPtr<ID3D12Resource> VertexBufferUploader;	// VBUpload
		ComPtr<ID3D12Resource> IndexBufferUploader;		// IBUpload

		// 把多个网格的顶点（索引）数据合并，使用下列
		// 容器来定义子网格索引属性，就能单独绘制出其中的子网格

		std::unordered_map<_astring, SubmeshGeometry> DrawArgs;



		//@EndOf(MeshGeometry)
	};

}//dx12


