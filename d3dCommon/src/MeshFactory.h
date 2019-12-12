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

namespace dxCommon
{
	class D3DCOMMON_EXPORTS_CLASS MeshFactory
	{
	private:
		MeshFactory() = delete;
		~MeshFactory() = delete;

	public:
		//<summary>
		// 创建一个三角形
		//</summary>
		static void CreateTriangle(const float side, MeshData* pMeshData);

		// 在 xz-plane 创建 m * n 的网格（m行n列），
		// 居中于指定宽度和深度的原点
		// width	- 宽度 x
		// depth	- 深度 z
		// m		- 行数（对应深度 z）
		// n		- 列数（对应宽度 x）
		static void CreateGrid(float width, float depth, uint32 m, uint32 n, MeshData* pMeshData);

		// 
		static void CreateBox(MeshData* pMeshData);

		// 
		static void CreateBox(float width, float height, float depth, MeshData* pMeshData);

		// Creates a box centered at the origin with the given dimensions,
		// where each face has m rows and n columns of vertices.
		// width			- 宽度 x
		// height			- 高度 y
		// depth			- 深度 z
		// numSubdivisions	- 细分次数
		static void CreateBox(float width, float height, float depth, uint32 numSubdivisions, MeshData* pMeshData);

		// Creates a sphere centered at the origin with the given radius.
		// The slices and stacks parameters control the degree of tessellation.
		// radius		- 半径
		// sliceCount	- 切片数量（垂直切割片数）
		// stackCount	- 堆叠层数（横向切割层数）
		static void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount, MeshData* pMeshData);

		// Creates a geosphere centered at the origin with the given radius.
		// The depth controls the level of tessellation.
		static void CreateGeosphere(float radius, uint32 numSubdivisions, MeshData* pMeshData);

		// Creates a quad aligned with the screen.
		// This is useful for postprocessing and screen effects.
		static void CreateQuad(float x, float y, float w, float h, float depth, MeshData* pMeshData);

		// Creates a cylinder parallel to the y-axis, and centered about the origin.  
		// The bottom and top radius can vary to form various cone shapes rather than true
		// cylinders.  The slices and stacks parameters control the degree of tessellation.
		// btmRadius	- 底面半径
		// topRadius	- 顶面半径
		// height		- 高度 y
		// sliceCount	- 切片数量（垂直切割片数）
		// stackCount	- 堆叠层数（横向切割层数）
		static void CreateCylinder(float btmRadius, float topRadius, float height,
			uint32 sliceCount, uint32 stackCount, MeshData* pMeshData);

		// Cone - 圆锥
		static void CreateCone(float radius, float height, uint32 sliceCount, uint32 stackCount, MeshData* pMeshData);

		// Tetrahedron - 四面体
		static void CreateTetrahedron(float size, MeshData* pMeshData);

		// Octahedron - 八面体
		static void CreateOctahedron(float size, MeshData* pMeshData);


		// Dodecahedron - 十二面体
		static void CreateDodecahedron(float size, MeshData* pMeshData);

		// Icosahedron - 二十面体
		static void CreateIcosahedron(float size, MeshData* pMeshData);


		// Torus - 圆环面
		static void CreateTorus(float diameter, float thickness, size_t tessellation, MeshData* pMeshData);

		// 
		//static void CreateTeapot(float size, size_t tessellation, MeshData* pMeshData);





	private:
		static void Subdivide(MeshData& meshData);
		static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
		static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
		static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);

		//static void CreateCylinderCap(float radius, float height, size_t tessellation, bool isTop, MeshData* pMeshData);
		static XMVECTOR GetCircleVector(size_t i, size_t tessellation)
		{
			float angle = i * XM_2PI / tessellation;
			float dx, dz;

			XMScalarSinCos(&dx, &dz, angle);

			XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
			return v;
		}
		static XMVECTOR GetCircleTangent(size_t i, size_t tessellation)
		{
			float angle = (i * XM_2PI / tessellation) + XM_PIDIV2;
			float dx, dz;

			XMScalarSinCos(&dx, &dz, angle);

			XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
			return v;
		}


		//@EndOf(MeshFactory)
	};


}//dxCommon

