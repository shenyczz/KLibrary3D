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

#include "MeshData.h"

namespace DX12
{
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
}
