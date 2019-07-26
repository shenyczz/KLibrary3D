#include "stdafx.h"
#include "MC.h"


MC::MC()
{
}


MC::~MC()
{
}


bool MC::LoadCTSlicerFromFiles16(string filePrefix, int indexBeg, int indexEnd, int width, int height)
{
	m_Slices.clear();

	int nBufferSize = width * height * sizeof(VarType);
	int nCount = indexEnd - indexBeg + 1;

	//��ȡ����
	for (int i = 0; i < nCount; i++)
	{
		int n = i + indexBeg;
		string finalPath = filePrefix + "." + std::to_string(n);

		std::ifstream fin(finalPath, std::ifstream::binary);
		if (!fin.is_open())
		{
			std::cout << "File Load Error!! :  " << finalPath << std::endl;
			return false;
		}

		TSlice<VarType> slice(width, height);

		slice.ElementArray.resize(nBufferSize);
		fin.read(reinterpret_cast<char*>(slice.ElementArray.data()), nBufferSize);

		m_Slices.push_back(slice);

		//�ر��ļ�
		fin.close();

		std::cout << "File Loaded OK: " << finalPath << std::endl;
	}


	return true;
}

void MC::Reconstruct16(MCP& mcp)
{
	// ����һ��С����ѳɵĴ󷽿��С
	float BigCubeWidth = mcp.BigCubeWidth();
	float BigCubeHeight = mcp.BigCubeHeight();
	float BigCubeDepth = mcp.BigCubeDepth();

	//Y�������n����Ƭ����ô��Ϊ���˸����Ż����������ڲ�ͼ���󲢼�������ֻ��n-1��ͼ��
	//��ôY����ֻ��Ҫ����n-2��������
	for (int iy = 0; iy < mcp.cubeCountY; iy++)
	{
		for (int iz = 0; iz < mcp.cubeCountZ; iz++)
		{
			for (int ix = 0; ix < mcp.cubeCountX; ix++)
			{

				// 1.��ǰ�������xyz������ƫ��
				KVector3 basePosition =
				{
					ix * mcp.cubeWidth,
					iy * mcp.cubeHeight,
					iz * mcp.cubeDepth
				};

				// 2.����С�������8���������꣬����˳���1987�����ĵĶ���һ��
				KVector3 v[8] =
				{
					basePosition + KVector3(0, 0 ,0) ,										// (0,0,0)
					basePosition + KVector3(mcp.cubeWidth, 0, 0),							// (x,0,0)
					basePosition + KVector3(mcp.cubeWidth, mcp.cubeHeight, 0),				// (x,y,0)
					basePosition + KVector3(0, mcp.cubeHeight, 0),							// (0,y,0)
					basePosition + KVector3(0, 0, mcp.cubeDepth),							// (0,0,z)
					basePosition + KVector3(mcp.cubeWidth, 0, mcp.cubeDepth),				// (x,0,z)
					basePosition + KVector3(mcp.cubeWidth, mcp.cubeHeight, mcp.cubeDepth),	// (x,y,z)
					basePosition + KVector3(0, mcp.cubeHeight, mcp.cubeDepth),				// (0,y,z)
				};

				// 3.����8������������Ҫ��256���������һ��
				int triCaseIndex = 0;
				for (size_t i = 0; i < 8; i++)
				{
					int currentCubeIndexY = 0;

					//ȡһ����ǰ��CT�в������ж϶����Ƿ��������ڲ�
					if (i == 0 || i == 1 || i == 4 || i == 5)
					{
						currentCubeIndexY = iy + 0;	//�������±���
					}
					else
					{
						currentCubeIndexY = iy + 1;	//�������ϱ���
					}

					//ȡCT��Ƭ
					TSlice<VarType>& currentSlice = m_Slices.at(currentCubeIndexY);

					//�����Ӧ����������꣬������CTͼ����������
					int pixelCoordX = int((v[i].x / BigCubeWidth) * currentSlice.Width);
					if (pixelCoordX == currentSlice.Width)
						pixelCoordX--;

					// ��Ƭ Y (todo: y <--> Z ???)
					int pixelCoordY = int((v[i].z / BigCubeDepth) * currentSlice.Height);
					if (pixelCoordY == currentSlice.Height)
						pixelCoordY--;


					// ����ֵ
					//��CTͼ��Ӧλ���Ƿ��ǰ�ɫ��Ҳ����-1�����жϴ˵��ǲ����������ڲ�
					DWORD dwv = currentSlice.GetValue(pixelCoordX, pixelCoordY);
					//if (dwv >= 10 && dwv<25)
					if (dwv > 0)
					{
						//���õ�i��������λ 0xFF
						triCaseIndex |= (1 << i);

						v[i].V = currentSlice.GetValue(pixelCoordX, pixelCoordY);
					}
					else
					{
						v[i].V = 2;
					}

				}//i(8)

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
				// 12����
				KVector3 pointOnEdge[12] =
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

				{
					//��ֵ
					//pointOnEdge[0].V = (v[0].V + v[1].V) * 0.5f;	//00
					//pointOnEdge[1].V = (v[1].V + v[2].V) * 0.5f;	//01
					//pointOnEdge[2].V = (v[3].V + v[2].V) * 0.5f;	//02
					//pointOnEdge[3].V = (v[0].V + v[3].V) * 0.5f;	//03
					//pointOnEdge[4].V = (v[4].V + v[5].V) * 0.5f;	//04
					//pointOnEdge[5].V = (v[5].V + v[6].V) * 0.5f;	//05
					//pointOnEdge[6].V = (v[7].V + v[6].V) * 0.5f;	//06
					//pointOnEdge[7].V = (v[4].V + v[7].V) * 0.5f;	//07
					//pointOnEdge[8].V = (v[0].V + v[4].V) * 0.5f;	//08
					//pointOnEdge[9].V = (v[1].V + v[5].V) * 0.5f;	//09
					//pointOnEdge[10].V = (v[3].V + v[7].V) * 0.5f;	//10
					//pointOnEdge[11].V = (v[2].V + v[6].V) * 0.5f; 	//11

					pointOnEdge[ 0].V = KMaths::Max(v[0].V, v[1].V);	//00
					pointOnEdge[ 1].V = KMaths::Max(v[1].V, v[2].V);	//01
					pointOnEdge[ 2].V = KMaths::Max(v[3].V, v[2].V);	//02
					pointOnEdge[ 3].V = KMaths::Max(v[0].V, v[3].V);	//03
					pointOnEdge[ 4].V = KMaths::Max(v[4].V, v[5].V);	//04
					pointOnEdge[ 5].V = KMaths::Max(v[5].V, v[6].V);	//05
					pointOnEdge[ 6].V = KMaths::Max(v[7].V, v[6].V);	//06
					pointOnEdge[ 7].V = KMaths::Max(v[4].V, v[7].V);	//07
					pointOnEdge[ 8].V = KMaths::Max(v[0].V, v[4].V);	//08
					pointOnEdge[ 9].V = KMaths::Max(v[1].V, v[5].V);	//09
					pointOnEdge[10].V = KMaths::Max(v[3].V, v[7].V);	//10
					pointOnEdge[11].V = KMaths::Max(v[2].V, v[6].V);	//11
				}



				//���� triangleCase ��������֮������
				for (int i = 0; i < 16; i += 3)
				{
					if (triCase.index[i] == -1)
						break;

					KVector3 v0(pointOnEdge[triCase.index[i + 0]] - KVector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
					KVector3 v1(pointOnEdge[triCase.index[i + 1]] - KVector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
					KVector3 v2(pointOnEdge[triCase.index[i + 2]] - KVector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));

					v0.V = pointOnEdge[triCase.index[i + 0]].V;
					v1.V = pointOnEdge[triCase.index[i + 1]].V;
					v2.V = pointOnEdge[triCase.index[i + 2]].V;

					//�����Ǹ�������Ϊ����ģ�Ϳ���ԭ��
					m_MeshOutput.push_back(v0);
					m_MeshOutput.push_back(v1);
					m_MeshOutput.push_back(v2);

					// ������Լ���ÿ������ֵ��
				}//i(16)


			}//iz
		}//ix
	}//iy

	std::cout << "Marching Cubes had successfully finished!" << std::endl;

	return;
}

void MC::GetMesh(std::vector<KVector3>& output)
{
	output = std::move(m_MeshOutput);
};

bool  MC::ExportFile_STL_Binary(string filePath, const string & headerInfo, const vector<KVector3>& inVertexBuffer)
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
		KVector3 v1 = inVertexBuffer.at(3 * i + 0);
		KVector3 v2 = inVertexBuffer.at(3 * i + 1);
		KVector3 v3 = inVertexBuffer.at(3 * i + 2);
		KVector3 edge1 = v3 - v1;
		KVector3 edge2 = v2 - v1;
		KVector3 triNorm = KVector3(
			edge1.y * edge2.z - edge1.z * edge2.y,
			edge1.z * edge2.x - edge1.x * edge2.z,
			edge1.x * edge2.y - edge1.y * edge2.x)*(-1.0f);

		triNorm.Normalize();


		//a facet normal
		REINTERPRET_WRITE(triNorm.x);
		REINTERPRET_WRITE(triNorm.z);
		REINTERPRET_WRITE(triNorm.y);

		//3 vertices
		REINTERPRET_WRITE(v1.x);
		REINTERPRET_WRITE(v1.z);
		REINTERPRET_WRITE(v1.y);

		REINTERPRET_WRITE(v2.x);
		REINTERPRET_WRITE(v2.z);
		REINTERPRET_WRITE(v2.y);

		REINTERPRET_WRITE(v3.x);
		REINTERPRET_WRITE(v3.z);
		REINTERPRET_WRITE(v3.y);


		short faceAttr = 0;
		REINTERPRET_WRITE(faceAttr);
	}


	fileOut.close();

	return true;
}







