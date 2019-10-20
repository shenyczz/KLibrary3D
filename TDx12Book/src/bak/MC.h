#pragma once

using namespace std;

//#define _VAR_TYPE_CHAR_
#define _VAR_TYPE_WORD_

#if defined(_VAR_TYPE_CHAR_)
typedef UCHAR	VarType;
#elif defined(_VAR_TYPE_WORD_)
//typedef WORD	VarType;
typedef SHORT	VarType;
#else
typedef UCHAR	VarType;
#endif

// vtkMarchingCubesTriangleCases
// vtkHexahedron
// vtkVoxel
// vtkFlyingEdges3DAlgorithm<T>
// vtkMarchingCubes
// vtkDiscreteMarchingCubes
// vtkImageMarchingCubes
// vtkSliceCubes



// 移动立方体参数
struct MCP
{
	int cubeCountX;		// X方向小正方体数量
	int cubeCountY;		// Y方向小正方体数量
	int cubeCountZ;		// Z方向小正方体数量
	
	float cubeWidth;	// 小正方体宽度
	float cubeHeight;	// 小正方体高度
	float cubeDepth;	// 小正方体深度

	float BigCubeWidth() { return cubeWidth * cubeCountX; }
	float BigCubeHeight() { return cubeCountY * cubeHeight; }
	float BigCubeDepth() { return cubeCountZ * cubeDepth; }

	//@EndOf(MCP)
};

/*******************************
//
//  z
//	|
//	|    y
//	|   /
//	|  /
//	| /
//	0--------------x
//
********************************/
struct MCP0
{
	int CubeCountX;			// X方向小正方体数量
	int CubeCountY;			// Y方向小正方体数量
	int CubeCountZ;			// Z方向小正方体数量

	float CubeSizeX;		// 小正方体宽度
	float CubeSizeY;		// 小正方体深度
	float CubeSizeZ;		// 小正方体高度

	float BigCubeSizeX() { return CubeSizeX * CubeCountX; }
	float BigCubeSizeY() { return CubeSizeY * CubeCountY; }
	float BigCubeSizeZ() { return CubeSizeZ * CubeCountZ; }

	//@EndOf(MCP)
};


class MC
{
public:
	MC();
	~MC();

	bool LoadCTSlicerFromFiles16(string filePrefix, int indexBeg, int indexEnd, int width, int height);

	// ok
	void Reconstruct16(MCP& mcp);

	void Reconstruct16_0_bak(MCP0& mcp);

	void Reconstruct16_0(MCP0& mcp);

	void Reconstruct16_01(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues);

	// 
	// scalars		- 3D网格点要素值
	// dims[3]		- 3D网格维度(x,y,z)
	// origin[3]	- 原点坐标
	// spacing[3]	- 移动立方体大小(cx,cy,cz)
	// 
	void Reconstruct16_1(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues);
	void Reconstruct16_2(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues);

	// scalars		- 3D网格点要素值
	// dims[3]		- 3D网格维度(nx,ny,nz)
	// cubeCount[3]	- 移动立方体数量
	// cubeSize[3]	- 移动立方体大小(cx,cy,cz)
	void Reconstruct16_3(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues);

	void GetMesh(std::vector<Vector3>& output);

	bool ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<Vector3>& inVertexBuffer);


	std::vector<TSlice<VarType>>	m_Slices;
	std::vector<Vector3>			m_MeshOutput;

};

