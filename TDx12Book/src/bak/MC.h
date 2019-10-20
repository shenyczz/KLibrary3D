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



// �ƶ����������
struct MCP
{
	int cubeCountX;		// X����С����������
	int cubeCountY;		// Y����С����������
	int cubeCountZ;		// Z����С����������
	
	float cubeWidth;	// С��������
	float cubeHeight;	// С������߶�
	float cubeDepth;	// С���������

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
	int CubeCountX;			// X����С����������
	int CubeCountY;			// Y����С����������
	int CubeCountZ;			// Z����С����������

	float CubeSizeX;		// С��������
	float CubeSizeY;		// С���������
	float CubeSizeZ;		// С������߶�

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
	// scalars		- 3D�����Ҫ��ֵ
	// dims[3]		- 3D����ά��(x,y,z)
	// origin[3]	- ԭ������
	// spacing[3]	- �ƶ��������С(cx,cy,cz)
	// 
	void Reconstruct16_1(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues);
	void Reconstruct16_2(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues);

	// scalars		- 3D�����Ҫ��ֵ
	// dims[3]		- 3D����ά��(nx,ny,nz)
	// cubeCount[3]	- �ƶ�����������
	// cubeSize[3]	- �ƶ��������С(cx,cy,cz)
	void Reconstruct16_3(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues);

	void GetMesh(std::vector<Vector3>& output);

	bool ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<Vector3>& inVertexBuffer);


	std::vector<TSlice<VarType>>	m_Slices;
	std::vector<Vector3>			m_MeshOutput;

};

