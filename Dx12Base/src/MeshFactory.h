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
/*
// All triangles are generated "outward" facing.  If you want "inward"
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
*/
#pragma once


#include "MeshData.h"

namespace DX12
{
	class DX12BASE_EXPORTS_CLASS MeshFactory
	{
	private:
		MeshFactory() = delete;
		~MeshFactory() = delete;

	public:
		///<summary>
		/// 创建一个三角形
		///</summary>
		static void CreateTriangle(const float side, MeshData* pMeshData);

		//<summary> CreateGridxz
		// 在 xz-plane 创建 m * n 的网格（m行n列），
		// 居中于指定宽度和深度的原点
		//</summary>
		// width - 宽度(X)
		// depth - 深度(Z)
		// m - 行
		// n - 列
		static void CreateGrid(float width, float depth, uint32 m, uint32 n, MeshData* pMeshData);


		///<summary>
		/// Creates a box centered at the origin with the given dimensions, where each
		/// face has m rows and n columns of vertices.
		///</summary>
		static void CreateBox(float width, float height, float depth, uint32 numSubdivisions, MeshData* pMeshData);

		///<summary>
		/// Creates a sphere centered at the origin with the given radius.  The
		/// slices and stacks parameters control the degree of tessellation.
		/// sliceCount - 面片数量（东西）
		/// stackCount - 堆叠层数（南北）
		///</summary>
		static void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount, MeshData* pMeshData);

		///<summary>
		/// Creates a geosphere centered at the origin with the given radius.  The
		/// depth controls the level of tessellation.
		///</summary>
		static void CreateGeosphere(float radius, uint32 numSubdivisions, MeshData* pMeshData);

		///<summary>
		/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
		///</summary>
		static void CreateQuad(float x, float y, float w, float h, float depth, MeshData* pMeshData);

		///<summary>
		/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
		/// The bottom and top radius can vary to form various cone shapes rather than true
		/// cylinders.  The slices and stacks parameters control the degree of tessellation.
		///</summary>
		static void CreateCylinder(float btmRadius, float topRadius, float height,
			uint32 sliceCount, uint32 stackCount, MeshData* pMeshData);

	private:
		static void Subdivide(MeshData& meshData);
		static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
		static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
		static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);


		//@EndOf(MeshFactory)
	};
}

