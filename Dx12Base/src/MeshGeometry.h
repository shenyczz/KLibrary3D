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

namespace DX12
{
	struct SubmeshGeometry
	{
		// 索引数量
		UINT IndexCount = 0;

		// 索引开始位置
		UINT StartIndexLocation = 0;

		// 基础顶点位置
		INT BaseVertexLocation = 0;

		DirectX::BoundingBox Bounds;
	};

}//DX12



namespace DX12
{
	class DX12BASE_EXPORTS_CLASS MeshGeometry
	{
		MeshGeometry();
		~MeshGeometry();

		// 几何体网格名称，可以根据此名称找到它
		_tstring Name;

		// 系统内存中的副本。
		// 由于顶点（索引）可以是泛型格式（具体格式由用户定义），
		// 所以使用Blob类型来表示。用户使用时需将其转换为适当类型
		ComPtr<ID3DBlob> VertexBufferCPU;
		ComPtr<ID3DBlob> IndexBufferCPU;

		ComPtr<ID3D12Resource> VertexBufferGPU;
		ComPtr<ID3D12Resource> IndexBufferGPU;

		ComPtr<ID3D12Resource> VertexBufferUploader;
		ComPtr<ID3D12Resource> IndexBufferUploader;

		// 与缓冲区相关的数据

		// 顶点缓冲区字节大小 VBSizeInBytes
		UINT VertexBufferByteSize;

		// 顶点字节跨度 VBStrideInBytes
		UINT VertexByteStride;

		// 索引格式(DXGI_FORMAT_R16_UINT) => UINT16
		DXGI_FORMAT IndexFormat;

		// 索引缓冲区字节大小 IBSizeInBytes
		UINT IndexBufferByteSize;

		// 一个 MeshGeometry 结构体能够存储一组顶点/索引缓冲区中的多个几何体
		// 若利用下列容器来定义子网格几何体，就能单独绘制出其中的子网格（单个几何体）
		std::unordered_map<_tstring, SubmeshGeometry> DrawArgs;



		// 顶点缓冲区视图
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;

		// 索引缓冲区视图
		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

		void DisposeUploader();

		//@EndOf(MeshGeometry)
	};

}//DX12

// 几何体网格

