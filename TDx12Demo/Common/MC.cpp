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

	//读取数据
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

		//关闭文件
		fin.close();

		std::cout << "File Loaded OK: " << finalPath << std::endl;
	}


	return true;
}

void MC::Reconstruct16(MCP& mcp)
{
	// 计算一堆小方块堆成的大方块大小
	float BigCubeWidth = mcp.BigCubeWidth();
	float BigCubeHeight = mcp.BigCubeHeight();
	float BigCubeDepth = mcp.BigCubeDepth();

	//Y：如果有n个切片，那么因为做了个“优化”就是相邻层图像求并集，所以只有n-1层图像
	//那么Y方向只需要遍历n-2个正方体
	for (int iy = 0; iy < mcp.cubeCountY; iy++)
	{
		for (int iz = 0; iz < mcp.cubeCountZ; iz++)
		{
			for (int ix = 0; ix < mcp.cubeCountX; ix++)
			{

				// 1.当前立方体的xyz的坐标偏移
				KVector3 basePosition =
				{
					ix * mcp.cubeWidth,
					iy * mcp.cubeHeight,
					iz * mcp.cubeDepth
				};

				// 2.计算小立方体的8个顶点坐标，顶点顺序跟1987年论文的定义一样
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

				// 3.根据8个顶点来计算要用256种情况的哪一种
				int triCaseIndex = 0;
				for (size_t i = 0; i < 8; i++)
				{
					int currentCubeIndexY = 0;

					//取一波当前的CT切层用以判断顶点是否在物体内部
					if (i == 0 || i == 1 || i == 4 || i == 5)
					{
						currentCubeIndexY = iy + 0;	//立方体下表面
					}
					else
					{
						currentCubeIndexY = iy + 1;	//立方体上表面
					}

					//取CT切片
					TSlice<VarType>& currentSlice = m_Slices.at(currentCubeIndexY);

					//先求对应点的像素坐标，才能在CT图里面获得数据
					int pixelCoordX = int((v[i].x / BigCubeWidth) * currentSlice.Width);
					if (pixelCoordX == currentSlice.Width)
						pixelCoordX--;

					// 切片 Y (todo: y <--> Z ???)
					int pixelCoordY = int((v[i].z / BigCubeDepth) * currentSlice.Height);
					if (pixelCoordY == currentSlice.Height)
						pixelCoordY--;


					// 顶点值
					//用CT图对应位置是否是白色（也就是-1）来判断此点是不是在物体内部
					DWORD dwv = currentSlice.GetValue(pixelCoordX, pixelCoordY);
					//if (dwv >= 10 && dwv<25)
					if (dwv > 0)
					{
						//设置第i个二进制位 0xFF
						triCaseIndex |= (1 << i);

						v[i].V = currentSlice.GetValue(pixelCoordX, pixelCoordY);
					}
					else
					{
						v[i].V = 2;
					}

				}//i(8)

				//这两个case是不会生成任何三角形，干脆跳过
				if (triCaseIndex == 0 || triCaseIndex == 255)
					continue;

				//取 non-trivial 的 Triangle Case
				const KMarchingCubesTriangleCases& triCase = KMarchingCubesTriangleCases::GetCases()[triCaseIndex];

				// 立方体：8个顶点、6个面、12条边、12个三角形、36个顶点索引
				//接下来就是12条棱上的迷之(中)点（讲道理是可以插值的，但是那个比例系数好像不太能获得，那就简单粗暴取中点）
				//
				//定义依旧和1987论文一样
				//static int edges[12][2] = { 
				//{ 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
				//{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
				//{ 0, 4 }, { 1, 5 }, { 3, 7 }, { 2, 6 } };
				// 12条边
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
					//赋值
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



				//利用 triangleCase 来生成迷之三角形
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

					//减的那个东西是为了让模型靠近原点
					m_MeshOutput.push_back(v0);
					m_MeshOutput.push_back(v1);
					m_MeshOutput.push_back(v2);

					// 这里可以计算每个顶点值？
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







