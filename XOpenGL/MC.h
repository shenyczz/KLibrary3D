#pragma once

using namespace std;

//#define _VAR_TYPE_CHAR_
#define _VAR_TYPE_WORD_

#if defined(_VAR_TYPE_CHAR_)
typedef UCHAR	VarType;
#elif defined(_VAR_TYPE_WORD_)
typedef WORD	VarType;
#else
typedef UCHAR	VarType;
#endif



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


//typedef KVector3 Vector3;

class MC
{
public:
	MC();
	~MC();

	bool LoadCTSlicerFromFiles(string fileNamePrefix, string fileNameSubfix, int sliceStartIndex, int sliceEndIndex, int pixelWidth, int pixelHeight);
	bool LoadCTSlicerFromFiles16(string filePrefix, int indexBeg, int indexEnd, int width, int height);

	void Reconstruct(const MCP& mcp);
	void Reconstruct16(MCP& mcp);

	void GetMesh(std::vector<Vector3>& output);

	bool ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<Vector3>& inVertexBuffer);

//private:

	std::vector<TSlice<VarType>>	m_Slices;
	std::vector<Vector3>			m_MeshOutput;

};

