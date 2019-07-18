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
		std::cout << "Error : number of CT slices should be more than 2！Load CT Slices failed." << std::endl;
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

		 //如果文件标号是有n位数字的，那么在前面就要补够0
		 int i = sliceStartIndex + n;
		 if (i < 10)finalPath += "00";
		 else if (i < 100)finalPath += "0";

		 //计算最终文件路径
		 finalPath += std::to_string(i) + fileNameSubfix;

		 //打开文件
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

		 // 保存切片
		 m_CTSlices.push_back(slice);

		 //关闭文件
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

	//------------------开始用Marching Cube算法来生成迷之等值面们------------------

	//由一堆小方块堆成的大方块的尺寸
	float BigCubeWidth = mcp.cubeCountX * mcp.cubeWidth;
	float BigCubeHeight = mcp.cubeCountY * mcp.cubeHeight;
	float BigCubeDepth = mcp.cubeCountZ * mcp.cubeDepth;

	//Y：如果有n个切片，那么因为做了个“优化”就是相邻层图像求并集，所以只有n-1层图像
	//那么Y方向只需要遍历n-2个正方体
	for (int cubeIndexY = 0; cubeIndexY < mcp.cubeCountY; cubeIndexY++)
	{
		for (int cubeIndexX = 0; cubeIndexX < mcp.cubeCountX; cubeIndexX++)
		{
			for (int cubeIndexZ = 0; cubeIndexZ < mcp.cubeCountZ; cubeIndexZ++)
			{
				//接下来就是单个小立方体的逻辑：

				//当前立方体的xyz的坐标偏移
				Vector3 basePosition =
				{
					cubeIndexX * mcp.cubeWidth,
					cubeIndexY * mcp.cubeHeight,
					cubeIndexZ * mcp.cubeDepth
				};

				//计算小立方体的8个顶点坐标，顶点顺序跟1987年论文的定义一样
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

				//现在呢，我们根据8个顶点来计算要用256种情况的哪一种
				int triCaseIndex = 0;
				for (int i = 0; i < 8; ++i)
				{
					int currentCubeIndexY = 0;

					//取一波当前的CT切层用以判断顶点是否在物体内部
					if (i == 0 || i == 1 || i == 4 || i == 5)
					{
						currentCubeIndexY = cubeIndexY + 0;	//立方体下表面
					}
					else
					{
						currentCubeIndexY = cubeIndexY + 1;	//立方体上表面
					}

					//取CT切片
					TSlice<VarType>& currentSlice = m_CTSlices.at(currentCubeIndexY);

					//先求对应点的像素坐标，才能在CT图里面获得数据
					int pixelCoordX = int((v[i].X / BigCubeWidth) * currentSlice.Width);
					if (pixelCoordX == currentSlice.Width)
						pixelCoordX--;

					// 切片 Y 
					int pixelCoordY = int((v[i].Z / BigCubeDepth) * currentSlice.Height);
					if (pixelCoordY == currentSlice.Height)
						pixelCoordY--;

					// 顶点值
					//用CT图对应位置是否是白色（也就是-1）来判断此点是不是在物体内部
					//if (currentSlice.GetValue(pixelCoordX, pixelCoordY) == 255)
					if (currentSlice.GetValue(pixelCoordX, pixelCoordY) > 0)
					{
						//设置第i个二进制位 0xFF
						triCaseIndex |= (1 << i);
					}
				}

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

				// 插值根据顶点数值？

				// 12条边
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

				// triangleCase 右16种状态
				//利用 triangleCase 来生成迷之三角形
				for (int i = 0; i < 16; i += 3)
				{
					if (triCase.index[i] == -1)
						break;

					//减的那个东西是为了让模型靠近原点
					m_MeshOutput.push_back(pointOnEdge[triCase.index[i + 0]] - Vector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
					m_MeshOutput.push_back(pointOnEdge[triCase.index[i + 1]] - Vector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
					m_MeshOutput.push_back(pointOnEdge[triCase.index[i + 2]] - Vector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));

					// 这里可以计算每个顶点值？
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













