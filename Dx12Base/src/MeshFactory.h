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

#include "TVertex.h"

typedef TVertex<float> Vertex;

struct MeshData
{
	const void* VBInitData() const { return this->Vertices.data(); }
	const UINT VBSizeInBytes() const { return sizeof(Vertex) * Vertices.size(); }
	const UINT VBStrideInBytes() const { return sizeof(Vertex); }

	const void* IBInitData() const { return this->Indices.data(); }
	const UINT IBSizeInBytes() const { return Indices.size() * sizeof(std::uint32_t); }
	//const UINT IBStrideInBytes() const { return sizeof(std::uint32_t); }

	const UINT IndexCount() const { return Indices.size(); }


	std::vector<Vertex>	Vertices;
	std::vector<std::uint32_t> Indices;
	static const DXGI_FORMAT IBVFormat = DXGI_FORMAT_R32_UINT;

	//IndexFormat
};

class DX12BASE_EXPORTS_CLASS MeshFactory
{
	MeshFactory() = delete;
	~MeshFactory() = delete;

public:
	// 创建一个三角形
	static void CreateTriangle(const float side, MeshData* pMesh);

	// 创建一个长方形
	static void CreateQuad(float width, float height, MeshData* pMesh);

	//创建一个立方体：指定宽(X方向)、高(Y方向)、深(Z方向)
	static void CreateBox(float width, float height, float depth, MeshData* pMesh);




	//static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData) {}

};

