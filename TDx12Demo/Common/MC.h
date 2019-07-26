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

class MC
{
public:
	MC();
	~MC();

	bool LoadCTSlicerFromFiles16(string filePrefix, int indexBeg, int indexEnd, int width, int height);

	void Reconstruct16(MCP& mcp);

	void GetMesh(std::vector<KVector3>& output);

	bool ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<KVector3>& inVertexBuffer);


	std::vector<TSlice<VarType>>	m_Slices;
	std::vector<KVector3>			m_MeshOutput;

};

