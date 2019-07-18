#pragma once

using namespace std;

#define _VAR_TYPE_CHAR_
//#define _VAR_TYPE_WORD_

#if defined(_VAR_TYPE_CHAR_)
typedef UCHAR	VarType;
#elif defined(_VAR_TYPE_WORD_)
typedef WORD	VarType;
#else
typedef UCHAR	VarType;
#endif



// CT ��Ƭ
template<typename _Ty>
struct TSlice
{
	TSlice() = delete;
	TSlice(int width, int height)
		: Width(width), Height(height) {}

	_Ty GetValue(int x, int y)
	{
		return ElementArray.at(y * Width + x);
	}

	int Width;
	int Height;

	std::vector<_Ty> ElementArray;

};



// �ƶ����������
struct MCP
{
	int cubeCountX;		// X����С����������
	int cubeCountY;		// Y����С����������
	int cubeCountZ;		// Z����С����������
	
	float cubeWidth;	// С��������
	float cubeHeight;	// С������߶�
	float cubeDepth;	// С���������
};


typedef KVector3 Vector3;

class MC
{
public:
	MC();
	~MC();

	bool LoadCTSlicerFromFiles(string fileNamePrefix, string fileNameSubfix, int sliceStartIndex, int sliceEndIndex, int pixelWidth, int pixelHeight);

	void Reconstruct(const MCP& mcInfo);

	void GetMesh(std::vector<Vector3>& output);

	bool ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<Vector3>& inVertexBuffer);

//private:

	std::vector<TSlice<VarType>>	m_CTSlices;
	std::vector<Vector3>	m_MeshOutput;

};

