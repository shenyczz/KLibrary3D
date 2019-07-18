#include "stdafx.h"
#include "MC.h"


MC::MC()
{
}


MC::~MC()
{
}


bool MC::LoadCTSlicerFromFiles(std::string fileNamePrefix, std::string fileNameSubfix, int sliceStartIndex, int sliceEndIndex, int pixelWidth, int pixelHeight)
{
	if (sliceEndIndex - sliceStartIndex< 1)
	{
		std::cout << "Error : number of CT slices should be more than 2��Load CT Slices failed." << std::endl;
		return false;
	}

	m_CTSlices.clear();

#if defined(_VAR_TYPE_CHAR_)
	int nBufferSize = pixelWidth * pixelHeight * sizeof(CHAR);
#elif defined(_VAR_TYPE_WORD_)
	int nBufferSize = pixelWidth * pixelHeight * sizeof(WORD);
#endif 

	 int sliceCount = sliceEndIndex - sliceStartIndex + 1;

	 for (int n = 0; n < sliceCount; n++)
	 {
		 string finalPath = fileNamePrefix;

		 //����ļ��������nλ���ֵģ���ô��ǰ���Ҫ����0
		 int i = sliceStartIndex + n;
		 if (i < 10)finalPath += "00";
		 else if (i < 100)finalPath += "0";

		 //���������ļ�·��
		 finalPath += std::to_string(i) + fileNameSubfix;

		 //���ļ�
		 std::ifstream inFile(finalPath, std::ios::binary);
		 if (!inFile.is_open())
		 {
			 std::cout << "File Load Error!! :  " << finalPath << std::endl;
		 }


		 TSlice<VarType> slice(pixelWidth, pixelHeight);

		 if (0)
		 {
			 char* fileBuff = new char[nBufferSize];

			 inFile.read(reinterpret_cast<char*>(fileBuff), nBufferSize);
			 slice.ElementArray.assign(fileBuff, fileBuff + pixelWidth * pixelHeight);

			 _deletea(fileBuff);
		 }
		 else
		 {
			 slice.ElementArray.resize(nBufferSize);
			 inFile.read(reinterpret_cast<char*>(slice.ElementArray.data()), nBufferSize);
		 }

		 // ������Ƭ
		 m_CTSlices.push_back(slice);

		 //�ر��ļ�
		 inFile.close();

		 std::cout << "File Loaded! : " << finalPath << std::endl;
	 }

	return true;
}

void MC::Reconstruct(const MCP & mcp)
{
	/*
	struct MCP
	{
		int cubeCountX;
		int cubeCountY;
		int cubeCountZ;

		float cubeWidth;
		float cubeHeight;
		float cubeDepth;
	};
	*/

	//------------------��ʼ��Marching Cube�㷨��������֮��ֵ����------------------

	//��һ��С����ѳɵĴ󷽿�ĳߴ�
	float BigCubeWidth = mcp.cubeCountX * mcp.cubeWidth;
	float BigCubeHeight = mcp.cubeCountY * mcp.cubeHeight;
	float BigCubeDepth = mcp.cubeCountZ * mcp.cubeDepth;

	//Y�������n����Ƭ����ô��Ϊ���˸����Ż����������ڲ�ͼ���󲢼�������ֻ��n-1��ͼ��
	//��ôY����ֻ��Ҫ����n-2��������
	for (int cubeIndexY = 0; cubeIndexY < mcp.cubeCountY; cubeIndexY++)
	{
		for (int cubeIndexX = 0; cubeIndexX < mcp.cubeCountX; cubeIndexX++)
		{
			for (int cubeIndexZ = 0; cubeIndexZ < mcp.cubeCountZ; cubeIndexZ++)
			{
				//���������ǵ���С��������߼���

				//��ǰ�������xyz������ƫ��
				Vector3 basePosition =
				{
					cubeIndexX * mcp.cubeWidth,
					cubeIndexY * mcp.cubeHeight,
					cubeIndexZ * mcp.cubeDepth
				};

				//����С�������8���������꣬����˳���1987�����ĵĶ���һ��
				Vector3 v[8] =
				{
					basePosition + Vector3(0,0,0) ,
					basePosition + Vector3(mcp.cubeWidth,0,0),
					basePosition + Vector3(mcp.cubeWidth,mcp.cubeHeight,0),
					basePosition + Vector3(0,mcp.cubeHeight,0),
					basePosition + Vector3(0,0,mcp.cubeDepth),
					basePosition + Vector3(mcp.cubeWidth,0,mcp.cubeDepth),
					basePosition + Vector3(mcp.cubeWidth,mcp.cubeHeight,mcp.cubeDepth),
					basePosition + Vector3(0,mcp.cubeHeight,mcp.cubeDepth),
				};

				//�����أ����Ǹ���8������������Ҫ��256���������һ��
				int triCaseIndex = 0;
				for (int i = 0; i < 8; ++i)
				{
					int currentCubeIndexY = 0;

					//ȡһ����ǰ��CT�в������ж϶����Ƿ��������ڲ�
					if (i == 0 || i == 1 || i == 4 || i == 5)
					{
						currentCubeIndexY = cubeIndexY + 0;	//�������±���
					}
					else
					{
						currentCubeIndexY = cubeIndexY + 1;	//�������ϱ���
					}

					//ȡCT��Ƭ
					TSlice<VarType>& currentSlice = m_CTSlices.at(currentCubeIndexY);

					//�����Ӧ����������꣬������CTͼ����������
					int pixelCoordX = int((v[i].X / BigCubeWidth) * currentSlice.Width);
					if (pixelCoordX == currentSlice.Width)
						pixelCoordX--;

					// ��Ƭ Y 
					int pixelCoordY = int((v[i].Z / BigCubeDepth) * currentSlice.Height);
					if (pixelCoordY == currentSlice.Height)
						pixelCoordY--;

					// ����ֵ
					//��CTͼ��Ӧλ���Ƿ��ǰ�ɫ��Ҳ����-1�����жϴ˵��ǲ����������ڲ�
					//if (currentSlice.GetValue(pixelCoordX, pixelCoordY) == 255)
					if (currentSlice.GetValue(pixelCoordX, pixelCoordY) > 0)
					{
						//���õ�i��������λ 0xFF
						triCaseIndex |= (1 << i);
					}
				}

				//������case�ǲ��������κ������Σ��ɴ�����
				if (triCaseIndex == 0 || triCaseIndex == 255)
					continue;

				//ȡ non-trivial �� Triangle Case
				const KMarchingCubesTriangleCases& triCase = KMarchingCubesTriangleCases::GetCases()[triCaseIndex];

				// �����壺8�����㡢6���桢12���ߡ�12�������Ρ�36����������
				//����������12�����ϵ���֮(��)�㣨�������ǿ��Բ�ֵ�ģ������Ǹ�����ϵ������̫�ܻ�ã��Ǿͼ򵥴ֱ�ȡ�е㣩
				//
				//�������ɺ�1987����һ��
				//static int edges[12][2] = { 
				//{ 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
				//{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
				//{ 0, 4 }, { 1, 5 }, { 3, 7 }, { 2, 6 } };

				// ��ֵ���ݶ�����ֵ��

				// 12����
				Vector3 pointOnEdge[12] =
				{
					(v[0] + v[1]) * 0.5f,	//00
					(v[1] + v[2]) * 0.5f,	//01
					(v[3] + v[2]) * 0.5f,	//02
					(v[0] + v[3]) * 0.5f,	//03
					(v[4] + v[5]) * 0.5f,	//04
					(v[5] + v[6]) * 0.5f,	//05
					(v[7] + v[6]) * 0.5f,	//06
					(v[4] + v[7]) * 0.5f,	//07
					(v[0] + v[4]) * 0.5f,	//08
					(v[1] + v[5]) * 0.5f,	//09
					(v[3] + v[7]) * 0.5f,	//10
					(v[2] + v[6]) * 0.5f, 	//11
				};

				// triangleCase ��16��״̬
				//���� triangleCase ��������֮������
				for (int i = 0; i < 16; i += 3)
				{
					if (triCase.index[i] == -1)
						break;

					//�����Ǹ�������Ϊ����ģ�Ϳ���ԭ��
					m_MeshOutput.push_back(pointOnEdge[triCase.index[i + 0]] - Vector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
					m_MeshOutput.push_back(pointOnEdge[triCase.index[i + 1]] - Vector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
					m_MeshOutput.push_back(pointOnEdge[triCase.index[i + 2]] - Vector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));

					// ������Լ���ÿ������ֵ��
				}

			}
		}
	}

	std::cout << "Marching Cubes had successfully finished!" << std::endl;
}

void MC::GetMesh(std::vector<Vector3>& output)
{
	output = std::move(m_MeshOutput);
};


bool  MC::ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<Vector3>& inVertexBuffer)
{
	std::ofstream fileOut(filePath, std::ios::binary);

	if (!fileOut.is_open())
	{
		std::cout << ("Export STL Binary : Open/Create File Failed! File path :" + filePath) << std::endl;
		return false;
	}

	/*STL: Baidu encyclopedia

	binary STL use fixed length of bit patterns to store vertex information,
	At the beginning, 80 bytes of header will be some custom info,
	Right behind the header , next 4 bytes will be the total number of triangles;

	the rest of the file will represent every triangles in 50 bytes blocks:

	3xfloat = 12bytes ------- Facet Normal
	3xfloat = 12bytes ------- Vertex1
	3xfloat = 12bytes ------- Vertex2
	3xfloat = 12bytes ------- Vertex3
	2 byte ------ face attribute info (I don't know what's the use...)

	the length of a complete STL will be 50 *(triangleCount) + 84  */

	//file header
	std::string finalHeaderInfo = headerInfo;
	//fill in the header to ensure that it's 80 bytes in length
	if (finalHeaderInfo.size() < 80)finalHeaderInfo.append(80 - headerInfo.size(), ' ');
	fileOut.write(headerInfo.c_str(), 80);

	//move reading cursor
	fileOut.seekp(80);

	//write bit patterns, and reinterpret the data to interested type
	//using decltype()
#define REINTERPRET_WRITE(var) \
	fileOut.write((char*)&var,sizeof(var));\

	//a char array used to store bit pattern (used in REINTERPRET_WRITE)
	//char dataBlock[5] = {};


	_Uint32t triangleCount = inVertexBuffer.size() / 3;
	REINTERPRET_WRITE(triangleCount);

	for (_Uint32t i = 0; i<triangleCount; ++i)
	{
		//3 vertices of a triangle
		Vector3 v1 = inVertexBuffer.at(3 * i + 0);
		Vector3 v2 = inVertexBuffer.at(3 * i + 1);
		Vector3 v3 = inVertexBuffer.at(3 * i + 2);
		Vector3 edge1 = v3 - v1;
		Vector3 edge2 = v2 - v1;
		Vector3 triNorm = Vector3(
			edge1.Y * edge2.Z - edge1.Z * edge2.Y,
			edge1.Z * edge2.X - edge1.X * edge2.Z,
			edge1.X * edge2.Y - edge1.Y * edge2.X)*(-1.0f);

		triNorm.Normalize();


		//a facet normal
		REINTERPRET_WRITE(triNorm.X);
		REINTERPRET_WRITE(triNorm.Z);
		REINTERPRET_WRITE(triNorm.Y);

		//3 vertices
		REINTERPRET_WRITE(v1.X);
		REINTERPRET_WRITE(v1.Z);
		REINTERPRET_WRITE(v1.Y);

		REINTERPRET_WRITE(v2.X);
		REINTERPRET_WRITE(v2.Z);
		REINTERPRET_WRITE(v2.Y);

		REINTERPRET_WRITE(v3.X);
		REINTERPRET_WRITE(v3.Z);
		REINTERPRET_WRITE(v3.Y);


		short faceAttr = 0;
		REINTERPRET_WRITE(faceAttr);
	}


	fileOut.close();

	return true;
}













