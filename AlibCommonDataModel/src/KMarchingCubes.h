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

class ALIBCOMMONDATAMODEL_EXPORTS_CLASS KMarchingCubes
{
public:
	KMarchingCubes();
	~KMarchingCubes();


public:
	// scalars		- 3D网格点要素值
	// dims[3]		- 3D网格维度(nx,ny,nz)
	// values		- 等值面值
	// numValues	- 等值面数量
	void Building(double *scalars, int dims[3], double *values, int numValues);

	// 原点加间距决定了点在空间中的位置
	// scalars		- 3D网格点要素值
	// dims[3]		- 3D网格维度(nx,ny,nz)
	// origin[3]	- 原点
	// spacing[3]	- 间距
	// values		- 等值面值
	// numValues	- 等值面数量
	void Building(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues);


	// scalars		- 3D网格点要素值
	// dims[3]		- 3D网格维度(nx,ny,nz)
	// origin[3]	- 源点？
	// spacing[3]	- ??
	// values		- 等值面值
	// numValues	- 等值面数量
	void Building_bak(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues);


	// scalars		- 3D网格点要素值
	// dims[3]		- 3D网格维度(nx,ny,nz)
	// cubeSize[3]	- 移动立方体大小(cx,cy,cz)
	// cubeCount[3]	- 移动立方体数量
	// values		- 等值面值
	// numValues	- 等值面数量
	void Building_1(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues);



	void Clear()
	{
		m_Vertices.clear();	
		m_Normals.clear();
		m_Indices.clear();
		m_VertexValues.clear();
	}

	void GetVertices(std::vector<Vector3>& outVertices)
	{
		outVertices = std::move(m_Vertices);
	}

	void GetIndices(std::vector<uint32>& outIndices)
	{
		outIndices = std::move(m_Indices);
	}

	void GetNormals(std::vector<Vector3>& outNormals)
	{
		outNormals = std::move(m_Normals);
	}

	void GetVertexValues(std::vector<double>& outVertexValues)
	{
		outVertexValues = std::move(m_VertexValues);
	}

private:
	bool IsUniquePoint(double xyz[3]);
	void ComputePointGradient(int i, int j, int k, int sliceSize,
		double *scalars, int dims[3], double spacing[3], OUT double n[3]);

private:
	std::vector<Vector3> m_Vertices;		// 顶点
	std::vector<Vector3> m_Normals;			// 法线
	std::vector<uint32> m_Indices;			// 索引
	std::vector<double> m_VertexValues;

	//@EndOf(KMarchingCubes)
};

