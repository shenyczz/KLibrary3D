/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
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
		// ����һ��������
		static void CreateTriangle(const float side, MeshData* pMesh);

		// ����һ��������
		static void CreateQuad(float width, float height, MeshData* pMesh);

		//����һ�������壺ָ����(X����)����(Y����)����(Z����)
		static void CreateBox(float width, float height, float depth, MeshData* pMesh);




		//static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData) {}

	};
}
