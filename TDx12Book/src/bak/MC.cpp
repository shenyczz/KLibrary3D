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

	int iSliceSize = width * height;
	int nBufferSize = iSliceSize * sizeof(VarType);

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


// ok
void MC::Reconstruct16(MCP& mcp)
{
	// 计算一堆小方块堆成的大方块大小
	float BigCubeWidth = mcp.BigCubeWidth();
	float BigCubeHeight = mcp.BigCubeHeight();
	float BigCubeDepth = mcp.BigCubeDepth();

	// 等高面值
	double step = 50;
	//double values[] = { 300 };
	//double values[] = { 300, 350, 400, 450, 500, 550 };
	double values[] = { 100, 150, 200, 250, 300, 350, 400, 450, 500, 550 };
	int numValues = ARRAYSIZE(values);


	KMarchingCubesTriangleCases* pTriCases = KMarchingCubesTriangleCases::GetCases();

	//Y：如果有n个切片，那么因为做了个“优化”就是相邻层图像求并集，所以只有n-1层图像
	//那么Y方向只需要遍历n-2个正方体
	for (int iy = 0; iy < mcp.cubeCountY; iy++)			// 高度
	{
		for (int iz = 0; iz < mcp.cubeCountZ; iz++)		// 深度
		{
			for (int ix = 0; ix < mcp.cubeCountX; ix++)	// 宽度
			{
				// 1.当前立方体的xyz的坐标偏移
				Vector3 basePosition =
				{
					ix * mcp.cubeWidth,
					iy * mcp.cubeHeight,
					iz * mcp.cubeDepth
				};

				// 2.计算小立方体的8个顶点坐标，顶点顺序跟1987年论文的定义一样 - vertex
				Vector3 v[8] =
				{
					basePosition + Vector3(0, 0 ,0) ,										// (0,0,0)
					basePosition + Vector3(mcp.cubeWidth, 0, 0),							// (x,0,0)
					basePosition + Vector3(mcp.cubeWidth, mcp.cubeHeight, 0),				// (x,y,0)
					basePosition + Vector3(0, mcp.cubeHeight, 0),							// (0,y,0)
					basePosition + Vector3(0, 0, mcp.cubeDepth),							// (0,0,z)
					basePosition + Vector3(mcp.cubeWidth, 0, mcp.cubeDepth),				// (x,0,z)
					basePosition + Vector3(mcp.cubeWidth, mcp.cubeHeight, mcp.cubeDepth),	// (x,y,z)
					basePosition + Vector3(0, mcp.cubeHeight, mcp.cubeDepth),				// (0,y,z)
				};

				double value = 250;

				for (int contNum = 0; contNum < numValues; contNum++)
				{
					value = values[contNum];

					// 3.根据8个顶点来计算要用256种情况的哪一种
					int triCaseIndex = 0;
					for (size_t i = 0; i < 8; i++)
					{
						int currentCubeIndexY = 0;

						//取当前切片层用以判断顶点是否在物体内部
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

						// 切片 Y (todo: y <--> Z)
						int pixelCoordZ = int((v[i].z / BigCubeDepth) * currentSlice.Height);
						if (pixelCoordZ == currentSlice.Height)
							pixelCoordZ--;



						// 顶点值
						//用CT图对应位置是否是白色（也就是-1）来判断此点是不是在物体内部
						short dwv = currentSlice.GetValue(pixelCoordX, pixelCoordZ);
						if (dwv >= value && dwv < (value+ step))
						{
							//设置第i个二进制位 0xFF
							triCaseIndex |= (1 << i);


							// 赋值
							//v[i].V = dwv;

						}
						else
						{
							//v[i].V = 2;
						}

					}//i(8)

					 //这两个case是不会生成任何三角形，干脆跳过
					if (triCaseIndex == 0 || triCaseIndex == 255)
						continue;

					//取 non-trivial 的 Triangle Case
					const KMarchingCubesTriangleCases& triCase = *(pTriCases + triCaseIndex);

					// 立方体：8个顶点、6个面、12条边、12个三角形、36个顶点索引
					//接下来就是12条棱上的迷之(中)点（讲道理是可以插值的，但是那个比例系数好像不太能获得，那就简单粗暴取中点）
					//
					//定义依旧和1987论文一样,12条边
					//static int edges[12][2] = { 
					//{ 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
					//{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
					//{ 0, 4 }, { 1, 5 }, { 3, 7 }, { 2, 6 } };


					// 计算坐标(简单取中点,Lerp以后再补)
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

					{
						// 为12条边上的点赋值（简单求取平均值）
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
						//pointOnEdge[11].V = (v[2].V + v[6].V) * 0.5f;

						//pointOnEdge[ 0].V = max(v[0].V , v[1].V);	//00
						//pointOnEdge[ 1].V = max(v[1].V , v[2].V);	//01
						//pointOnEdge[ 2].V = max(v[3].V , v[2].V);	//02
						//pointOnEdge[ 3].V = max(v[0].V , v[3].V);	//03
						//pointOnEdge[ 4].V = max(v[4].V , v[5].V);	//04
						//pointOnEdge[ 5].V = max(v[5].V , v[6].V);	//05
						//pointOnEdge[ 6].V = max(v[7].V , v[6].V);	//06
						//pointOnEdge[ 7].V = max(v[4].V , v[7].V);	//07
						//pointOnEdge[ 8].V = max(v[0].V , v[4].V);	//08
						//pointOnEdge[ 9].V = max(v[1].V , v[5].V);	//09
						//pointOnEdge[10].V = max(v[3].V , v[7].V);	//10
						//pointOnEdge[11].V = max(v[2].V , v[6].V);
					}



					//利用 triangleCase 来生成迷之三角形
					for (int i = 0; i < 16; i += 3)
					{
						if (triCase.Edges[i] == -1)
							break;

						// 减的那个东西是为了让模型靠近原点
						KVector3 v0(pointOnEdge[triCase.Edges[i + 0]] - KVector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
						KVector3 v1(pointOnEdge[triCase.Edges[i + 1]] - KVector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));
						KVector3 v2(pointOnEdge[triCase.Edges[i + 2]] - KVector3(BigCubeWidth*0.5f, BigCubeHeight*0.5f, BigCubeDepth*0.5f));

						// 为三角形顶点赋值
						v0.V = pointOnEdge[triCase.Edges[i + 0]].V;
						v1.V = pointOnEdge[triCase.Edges[i + 1]].V;
						v2.V = pointOnEdge[triCase.Edges[i + 2]].V;

						m_MeshOutput.push_back(v0);
						m_MeshOutput.push_back(v1);
						m_MeshOutput.push_back(v2);

					}//i(16)

				}//for all contour
			}//iz
		}//ix
	}//iy

	std::cout << "Marching Cubes had successfully finished!" << std::endl;

	return;
}
// ok
void MC::Reconstruct16_0_bak(MCP0& mcp)
{
	// 计算一堆小方块堆成的大方块大小
	float BigCubeSizeX = mcp.BigCubeSizeX();
	float BigCubeSizeY = mcp.BigCubeSizeY();
	float BigCubeSizeZ = mcp.BigCubeSizeZ();

	// 等高面值
	double step = 50;
	//double values[] = { 350 };
	//double values[] = { 500 };
	double values[] = { 300, 350, 400, 450, 500, 550 };
	//double values[] = { 100, 150, 200, 250, 300, 350, 400, 450, 500, 550 };
	int numValues = ARRAYSIZE(values);


	KMarchingCubesTriangleCases* pTriCases = KMarchingCubesTriangleCases::GetCases();

	//Y：如果有n个切片，那么因为做了个“优化”就是相邻层图像求并集，所以只有n-1层图像
	//那么Y方向只需要遍历n-2个正方体
	for (int k = 0; k < mcp.CubeCountZ; k++)
	{
		for (int j = 0; j < mcp.CubeCountY; j++)
		{
			for (int i = 0; i < mcp.CubeCountX; i++)
			{
				// 1.当前立方体的xyz的坐标偏移
				KVector3 basePosition =
				{
					// todo: ZY
					i * mcp.CubeSizeX,
					k * mcp.CubeSizeZ,
					j * mcp.CubeSizeY,
				};

				// 2.计算小立方体的8个顶点坐标，顶点顺序跟1987年论文的定义一样 - vertex
				KVector3 v[8] =
				{
					// todo: ZY
					basePosition + KVector3(0, 0 ,0) ,										// (0,0,0)
					basePosition + KVector3(mcp.CubeSizeX, 0, 0),							// (x,0,0)
					basePosition + KVector3(mcp.CubeSizeX, mcp.CubeSizeZ, 0),				// (x,y,0)
					basePosition + KVector3(0, mcp.CubeSizeZ, 0),							// (0,y,0)
					basePosition + KVector3(0, 0, mcp.CubeSizeY),							// (0,0,z)
					basePosition + KVector3(mcp.CubeSizeX, 0, mcp.CubeSizeY),				// (x,0,z)
					basePosition + KVector3(mcp.CubeSizeX, mcp.CubeSizeZ, mcp.CubeSizeY),	// (x,y,z)
					basePosition + KVector3(0, mcp.CubeSizeZ, mcp.CubeSizeY),				// (0,y,z)
				};

				double value = 250;

				for (int contNum = 0; contNum < numValues; contNum++)
				{
					value = values[contNum];

					// 3.根据8个顶点来计算要用256种情况的哪一种
					int triCaseIndex = 0;
					for (size_t vi = 0; vi < 8; vi++)
					{
						int currentCubeIndexK = 0;

						//取当前切片层用以判断顶点是否在物体内部
						if (vi == 0 || vi == 1 || vi == 4 || vi == 5)
						{
							currentCubeIndexK = k + 0;	//立方体下表面
						}
						else
						{
							currentCubeIndexK = k + 1;	//立方体上表面
						}

						//取CT切片
						TSlice<VarType>& currentSlice = m_Slices.at(currentCubeIndexK);

						//先求对应点的像素坐标，才能在CT图里面获得数据
						int pixelCoordX = int((v[vi].x / BigCubeSizeX) * currentSlice.Width);
						if (pixelCoordX == currentSlice.Width)
							pixelCoordX--;

						// todo: ZY
						int pixelCoordY = int((v[vi].z / BigCubeSizeY) * currentSlice.Height);
						if (pixelCoordY == currentSlice.Height)
							pixelCoordY--;


						// 用顶点值判断此点是不是在物体内部
						short dwv = currentSlice.GetValue(pixelCoordX, pixelCoordY);

						if (dwv >= value && dwv < (value + step))
						{
							//设置第i个二进制位 0xFF
							triCaseIndex |= (1 << vi);


							// 赋值
							v[vi].V = dwv;

						}
						else
						{
							v[vi].V = 2;
						}

					}//i(8)

					 //这两个case是不会生成任何三角形，干脆跳过
					if (triCaseIndex == 0 || triCaseIndex == 255)
						continue;

					//取 non-trivial 的 Triangle Case
					const KMarchingCubesTriangleCases& triCase = *(pTriCases + triCaseIndex);

					// 立方体：8个顶点、6个面、12条边、12个三角形、36个顶点索引
					//接下来就是12条棱上的迷之(中)点（讲道理是可以插值的，但是那个比例系数好像不太能获得，那就简单粗暴取中点）
					//
					//定义依旧和1987论文一样,12条边
					//static int edges[12][2] = { 
					//{ 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
					//{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
					//{ 0, 4 }, { 1, 5 }, { 3, 7 }, { 2, 6 } };


					// 计算坐标(简单取中点,Lerp以后再补)
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
						// 为12条边上的点赋值（简单求取平均值）
						pointOnEdge[0].V = (v[0].V + v[1].V) * 0.5f;	//00
						pointOnEdge[1].V = (v[1].V + v[2].V) * 0.5f;	//01
						pointOnEdge[2].V = (v[3].V + v[2].V) * 0.5f;	//02
						pointOnEdge[3].V = (v[0].V + v[3].V) * 0.5f;	//03
						pointOnEdge[4].V = (v[4].V + v[5].V) * 0.5f;	//04
						pointOnEdge[5].V = (v[5].V + v[6].V) * 0.5f;	//05
						pointOnEdge[6].V = (v[7].V + v[6].V) * 0.5f;	//06
						pointOnEdge[7].V = (v[4].V + v[7].V) * 0.5f;	//07
						pointOnEdge[8].V = (v[0].V + v[4].V) * 0.5f;	//08
						pointOnEdge[9].V = (v[1].V + v[5].V) * 0.5f;	//09
						pointOnEdge[10].V = (v[3].V + v[7].V) * 0.5f;	//10
						pointOnEdge[11].V = (v[2].V + v[6].V) * 0.5f;

						pointOnEdge[0].V = max(v[0].V, v[1].V);	//00
						pointOnEdge[1].V = max(v[1].V, v[2].V);	//01
						pointOnEdge[2].V = max(v[3].V, v[2].V);	//02
						pointOnEdge[3].V = max(v[0].V, v[3].V);	//03
						pointOnEdge[4].V = max(v[4].V, v[5].V);	//04
						pointOnEdge[5].V = max(v[5].V, v[6].V);	//05
						pointOnEdge[6].V = max(v[7].V, v[6].V);	//06
						pointOnEdge[7].V = max(v[4].V, v[7].V);	//07
						pointOnEdge[8].V = max(v[0].V, v[4].V);	//08
						pointOnEdge[9].V = max(v[1].V, v[5].V);	//09
						pointOnEdge[10].V = max(v[3].V, v[7].V);	//10
						pointOnEdge[11].V = max(v[2].V, v[6].V);
					}



					//利用 triangleCase 来生成迷之三角形
					for (int tri = 0; tri < 16; tri += 3)
					{
						if (triCase.Edges[tri] == -1)
							break;

						// todo: ZY

						// 减的那个东西是为了让模型靠近原点
						KVector3 v0(pointOnEdge[triCase.Edges[tri + 0]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeZ*0.5f, BigCubeSizeY*0.5f));
						KVector3 v1(pointOnEdge[triCase.Edges[tri + 1]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeZ*0.5f, BigCubeSizeY*0.5f));
						KVector3 v2(pointOnEdge[triCase.Edges[tri + 2]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeZ*0.5f, BigCubeSizeY*0.5f));

						// 为三角形顶点赋值
						v0.V = pointOnEdge[triCase.Edges[tri + 0]].V;
						v1.V = pointOnEdge[triCase.Edges[tri + 1]].V;
						v2.V = pointOnEdge[triCase.Edges[tri + 2]].V;

						m_MeshOutput.push_back(v0);
						m_MeshOutput.push_back(v1);
						m_MeshOutput.push_back(v2);

					}//for all triangle
				}//for all contour
			}//for i
		}//for j
	}//for k

	std::cout << "Marching Cubes had successfully finished!" << std::endl;

	return;
}

void MC::Reconstruct16_0(MCP0& mcp)
{
	// 计算一堆小方块堆成的大方块大小
	float BigCubeSizeX = mcp.BigCubeSizeX();
	float BigCubeSizeY = mcp.BigCubeSizeY();
	float BigCubeSizeZ = mcp.BigCubeSizeZ();

	// 等高面值
	double step = 50;
	//double values[] = { 350 };
	//double values[] = { 500 };
	double values[] = { 300, 350, 400, 450, 500, 550 };
	//double values[] = { 100, 150, 200, 250, 300, 350, 400, 450, 500, 550 };
	int numValues = ARRAYSIZE(values);


	KMarchingCubesTriangleCases* pTriCases = KMarchingCubesTriangleCases::GetCases();

	//Y：如果有n个切片，那么因为做了个“优化”就是相邻层图像求并集，所以只有n-1层图像
	//那么Y方向只需要遍历n-2个正方体
	for (int k = 0; k < mcp.CubeCountZ; k++)
	{
		for (int j = 0; j < mcp.CubeCountY; j++)
		{
			for (int i = 0; i < mcp.CubeCountX; i++)
			{
				// 1.当前立方体的xyz的坐标偏移
				KVector3 basePosition =
				{
					// todo: ZY
					i * mcp.CubeSizeX,
					j * mcp.CubeSizeY,
					k * mcp.CubeSizeZ,
				};

				// 2.计算小立方体的8个顶点坐标，顶点顺序跟1987年论文的定义一样 - vertex
				KVector3 v[8] =
				{
					basePosition + KVector3(0, 0 ,0) ,										// (0,0,0)
					basePosition + KVector3(mcp.CubeSizeX, 0, 0),							// (x,0,0)
					basePosition + KVector3(mcp.CubeSizeX, mcp.CubeSizeY, 0),				// (x,y,0)
					basePosition + KVector3(0, mcp.CubeSizeY, 0),							// (0,y,0)
					basePosition + KVector3(0, 0, mcp.CubeSizeZ),							// (0,0,z)
					basePosition + KVector3(mcp.CubeSizeX, 0, mcp.CubeSizeZ),				// (x,0,z)
					basePosition + KVector3(mcp.CubeSizeX, mcp.CubeSizeY, mcp.CubeSizeZ),	// (x,y,z)
					basePosition + KVector3(0, mcp.CubeSizeY, mcp.CubeSizeZ),				// (0,y,z)
				};


				// 遍历所有等高线
				double value = 250;
				for (int contNum = 0; contNum < numValues; contNum++)
				{
					value = values[contNum];

					// 3.根据8个顶点来计算要用256种情况的哪一种
					int triCaseIndex = 0;
					for (int vi = 0; vi < 8; vi++)
					{
						int currentCubeIndexK = 0;

						//取当前切片层用以判断顶点是否在物体内部
						if (vi == 0 || vi == 1 || vi == 2 || vi == 3)
						{
							currentCubeIndexK = k + 0;	//立方体下表面
						}
						else
						{
							currentCubeIndexK = k + 1;	//立方体上表面
						}

						//取CT切片
						TSlice<VarType>& currentSlice = m_Slices.at(currentCubeIndexK);

						//先求对应点的像素坐标，才能在CT图里面获得数据
						int pixelCoordX = int((v[vi].x / BigCubeSizeX) * currentSlice.Width);
						if (pixelCoordX == currentSlice.Width)
							pixelCoordX--;

						// todo: ZY
						int pixelCoordY = int((v[vi].y / BigCubeSizeY) * currentSlice.Height);
						if (pixelCoordY == currentSlice.Height)
							pixelCoordY--;


						// 用顶点值判断此点是不是在物体内部
						short dwv = currentSlice.GetValue(pixelCoordX, pixelCoordY);

						if (dwv >= value && dwv < (value + step))
						{
							//设置第i个二进制位 0xFF
							triCaseIndex |= (1 << vi);


							// 赋值
							v[vi].V = dwv;

						}
						else
						{
							v[vi].V = 2;
						}

					}//vi(8)

					 //这两个case是不会生成任何三角形，干脆跳过
					if (triCaseIndex == 0 || triCaseIndex == 255)
						continue;

					//取 non-trivial 的 Triangle Case
					const KMarchingCubesTriangleCases& triCase = *(pTriCases + triCaseIndex);

					// 立方体：8个顶点、6个面、12条边、12个三角形、36个顶点索引
					//接下来就是12条棱上的迷之(中)点（讲道理是可以插值的，但是那个比例系数好像不太能获得，那就简单粗暴取中点）
					//
					//定义依旧和1987论文一样,12条边
					//static int edges[12][2] = { 
					//{ 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
					//{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
					//{ 0, 4 }, { 1, 5 }, { 3, 7 }, { 2, 6 } };


					// 计算坐标(简单取中点,Lerp以后再补)
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
						// 为12条边上的点赋值（简单求取平均值）
						pointOnEdge[0].V = (v[0].V + v[1].V) * 0.5f;	//00
						pointOnEdge[1].V = (v[1].V + v[2].V) * 0.5f;	//01
						pointOnEdge[2].V = (v[3].V + v[2].V) * 0.5f;	//02
						pointOnEdge[3].V = (v[0].V + v[3].V) * 0.5f;	//03
						pointOnEdge[4].V = (v[4].V + v[5].V) * 0.5f;	//04
						pointOnEdge[5].V = (v[5].V + v[6].V) * 0.5f;	//05
						pointOnEdge[6].V = (v[7].V + v[6].V) * 0.5f;	//06
						pointOnEdge[7].V = (v[4].V + v[7].V) * 0.5f;	//07
						pointOnEdge[8].V = (v[0].V + v[4].V) * 0.5f;	//08
						pointOnEdge[9].V = (v[1].V + v[5].V) * 0.5f;	//09
						pointOnEdge[10].V = (v[3].V + v[7].V) * 0.5f;	//10
						pointOnEdge[11].V = (v[2].V + v[6].V) * 0.5f;

						pointOnEdge[0].V = max(v[0].V, v[1].V);	//00
						pointOnEdge[1].V = max(v[1].V, v[2].V);	//01
						pointOnEdge[2].V = max(v[3].V, v[2].V);	//02
						pointOnEdge[3].V = max(v[0].V, v[3].V);	//03
						pointOnEdge[4].V = max(v[4].V, v[5].V);	//04
						pointOnEdge[5].V = max(v[5].V, v[6].V);	//05
						pointOnEdge[6].V = max(v[7].V, v[6].V);	//06
						pointOnEdge[7].V = max(v[4].V, v[7].V);	//07
						pointOnEdge[8].V = max(v[0].V, v[4].V);	//08
						pointOnEdge[9].V = max(v[1].V, v[5].V);	//09
						pointOnEdge[10].V = max(v[3].V, v[7].V);	//10
						pointOnEdge[11].V = max(v[2].V, v[6].V);
					}

					//利用 triangleCase 来生成迷之三角形
					for (int tri = 0; tri < 16; tri += 3)
					{
						if (triCase.Edges[tri] == -1)
							break;

						// 减的那个东西是为了让模型靠近原点
						KVector3 v0(pointOnEdge[triCase.Edges[tri + 0]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeY*0.5f, BigCubeSizeZ*0.5f));
						KVector3 v1(pointOnEdge[triCase.Edges[tri + 1]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeY*0.5f, BigCubeSizeZ*0.5f));
						KVector3 v2(pointOnEdge[triCase.Edges[tri + 2]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeY*0.5f, BigCubeSizeZ*0.5f));

						// 为三角形顶点赋值
						v0.V = pointOnEdge[triCase.Edges[tri + 0]].V;
						v1.V = pointOnEdge[triCase.Edges[tri + 1]].V;
						v2.V = pointOnEdge[triCase.Edges[tri + 2]].V;


						swap(v0.y, v0.z);
						swap(v1.y, v1.z);
						swap(v2.y, v2.z);


						m_MeshOutput.push_back(v0);
						m_MeshOutput.push_back(v1);
						m_MeshOutput.push_back(v2);

					}//for all triangle
				}//for all contour
			}//for i
		}//for j
	}//for k

	std::cout << "Marching Cubes had successfully finished!" << std::endl;

	return;
}







/*******************************
//
//		z
//		|
//		|    y
//		|   /
//		|  /
//		| /
//		0--------------x
//
********************************/
void MC::Reconstruct16_01(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues)
{
	// case 掩码
	static int CASE_MASK[8] = { 1,2,4,8,16,32,64,128 };

	// 每条边2个顶点
	static int s_edges[12][2] =
	{
		{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 },
	};

	// 3D网格维度
	int iOriginNX = dims[0];
	int iOriginNY = dims[1];
	int iOriginNZ = dims[2];

	// 切片尺寸
	int iSliceSize = iOriginNX * iOriginNY;

	int iAllSize = iOriginNX * iOriginNY * iOriginNZ;

	// mc 大小
	double dCubeSizeX = static_cast<float>(cubeSize[0]);
	double dCubeSizeY = static_cast<float>(cubeSize[1]);
	double dCubeSizeZ = static_cast<float>(cubeSize[2]);

	// mc 数量
	int iCubeCountX = cubeCount[0];
	int iCubeCountY = cubeCount[1];
	int iCubeCountZ = cubeCount[2];

	// 计算一堆小方块堆成的大方块大小
	double BigCubeSizeX = dCubeSizeX * iCubeCountX;
	double BigCubeSizeY = dCubeSizeY * iCubeCountY;
	double BigCubeSizeZ = dCubeSizeZ * iCubeCountZ;

	// TriangleCases
	KMarchingCubesTriangleCases* pTriCases = KMarchingCubesTriangleCases::GetCases();

	// 小立方体8个顶点坐标、梯度
	double pts[8][3];
	double offset[3];


	// 遍历所有的体素细胞
	for (int k = 0; k < iCubeCountZ; k++)
	{
		offset[2] = k * dCubeSizeZ;
		pts[0][2] = offset[2];
		double zp = pts[0][2] + dCubeSizeZ;

		for (int j = 0; j < iCubeCountY; j++)
		{
			offset[1] = j * dCubeSizeY;
			pts[0][1] = offset[1];
			double yp = pts[0][1] + dCubeSizeY;

			for (int i = 0; i < iCubeCountX; i++)
			{
				// 1.当前立方体的xyz的坐标偏移
				KVector3 basePosition =
				{
					(float)dCubeSizeX * i,
					(float)dCubeSizeY * j,
					(float)dCubeSizeZ * k,
				};


				// 2.计算小立方体的8个顶点坐标，顶点顺序跟1987年论文的定义一样 - vertex
				KVector3 v[8] =
				{
					// 逆时针: 0-1-2-3 
					basePosition + KVector3(0, 0 ,0) ,								// p0: (0,0,0)
					basePosition + KVector3(dCubeSizeX, 0, 0),						// p1: (x,0,0)
					basePosition + KVector3(dCubeSizeX, dCubeSizeY, 0),				// p2: (x,y,0)
					basePosition + KVector3(0, dCubeSizeY, 0),						// p3: (0,y,0)

					// 逆时针: 4-5-6-7
					basePosition + KVector3(0, 0, dCubeSizeZ),						// p4: (0,0,z)
					basePosition + KVector3(dCubeSizeX, 0, dCubeSizeZ),				// p5: (x,0,z)
					basePosition + KVector3(dCubeSizeX, dCubeSizeY, dCubeSizeZ),	// p6： (x,y,z)
					basePosition + KVector3(0, dCubeSizeY, dCubeSizeZ),				// p7: (0,y,z)
				};

				offset[0] = dCubeSizeX * i;
				pts[0][0] = offset[0];
				double xp = pts[0][0] + dCubeSizeX;

				// p1: (x,0,0)
				pts[1][0] = xp;
				pts[1][1] = pts[0][1];
				pts[1][2] = pts[0][2];

				// p2: (x,y,0)
				pts[2][0] = xp;
				pts[2][1] = yp;
				pts[2][2] = pts[0][2];

				// p3: (0,y,0)
				pts[3][0] = pts[0][0];
				pts[3][1] = yp;
				pts[3][2] = pts[0][2];

				// p4: (0,0,z)
				pts[4][0] = pts[0][0];
				pts[4][1] = pts[0][1];
				pts[4][2] = zp;

				// p5: (x,0,z)
				pts[5][0] = xp;
				pts[5][1] = pts[0][1];
				pts[5][2] = zp;

				// p6： (x,y,z)
				pts[6][0] = xp;
				pts[6][1] = yp;
				pts[6][2] = zp;

				// p7: (0,y,z)
				pts[7][0] = pts[0][0];
				pts[7][1] = yp;
				pts[7][2] = zp;

				// 遍历所有等高线
				double value = 250;
				for (int contNum = 0; contNum < numValues; contNum++)
				{
					value = values[contNum];

					// 根据8个顶点来计算要用256种情况的哪一种
					int triCaseIndex = 0;
					for (int vi = 0; vi < 8; vi++)
					{
						// 当前 z 向索引
						int idx_k = (vi == 0 || vi == 1 || vi == 2 || vi == 3) ? k : k + 1;

						// 当前 y 向索引
						int idx_j = int((v[vi].y / BigCubeSizeY) * iOriginNY);
						if (idx_j >= iOriginNY) idx_j--;

						// 当前 x 向索引
						int idx_i = int((v[vi].x / BigCubeSizeX) * iOriginNX);
						if (idx_i >= iOriginNX) idx_i--;

						//取CT切片
						TSlice<VarType>& currentSlice = m_Slices.at(idx_k);

						//先求对应点的像素坐标，才能在CT图里面获得数据
						int pixelCoordX = int((v[vi].x / BigCubeSizeX) * currentSlice.Width);
						if (pixelCoordX == currentSlice.Width)
							pixelCoordX--;

						int pixelCoordY = int((v[vi].y / BigCubeSizeY) * currentSlice.Height);
						if (pixelCoordY == currentSlice.Height)
							pixelCoordY--;


						int pos = idx_k * iOriginNX * iOriginNY + idx_j * iOriginNX + idx_i;
						if (pos >= iAllSize)
							continue;

						// 用顶点值判断此点是不是在物体内部
						short dwv0 = currentSlice.GetValue(pixelCoordX, pixelCoordY);
						short dwv1 = (short)(*(scalars + pos));

						if (dwv0 != dwv1)
						{
							int xxx = 0;
						}

						short dwv = dwv1;

						if (dwv >= value && dwv < (value + 50))
						{
							//设置第i个二进制位 0xFF
							triCaseIndex |= (1 << vi);
							// 赋值
							v[vi].V = dwv;
						}
						else
						{
							v[vi].V = 2;
						}

					}//for vi

					 //这两个case是不会生成任何三角形，干脆跳过
					if (triCaseIndex == 0 || triCaseIndex == 255)
						continue;

					//取 non-trivial 的 Triangle Case
					const KMarchingCubesTriangleCases& triCase = *(pTriCases + triCaseIndex);

					// 立方体：8个顶点、6个面、12条边、12个三角形、36个顶点索引
					//接下来就是12条棱上的迷之(中)点（讲道理是可以插值的，但是那个比例系数好像不太能获得，那就简单粗暴取中点）
					//
					//定义依旧和1987论文一样,12条边
					//static int edges[12][2] = { 
					//{ 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 },
					//{ 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 },
					//{ 0, 4 }, { 1, 5 }, { 3, 7 }, { 2, 6 } };


					// 计算坐标(简单取中点,Lerp以后再补)
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
						// 为12条边上的点赋值（简单求取平均值）
						pointOnEdge[0].V = (v[0].V + v[1].V) * 0.5f;	//00
						pointOnEdge[1].V = (v[1].V + v[2].V) * 0.5f;	//01
						pointOnEdge[2].V = (v[3].V + v[2].V) * 0.5f;	//02
						pointOnEdge[3].V = (v[0].V + v[3].V) * 0.5f;	//03
						pointOnEdge[4].V = (v[4].V + v[5].V) * 0.5f;	//04
						pointOnEdge[5].V = (v[5].V + v[6].V) * 0.5f;	//05
						pointOnEdge[6].V = (v[7].V + v[6].V) * 0.5f;	//06
						pointOnEdge[7].V = (v[4].V + v[7].V) * 0.5f;	//07
						pointOnEdge[8].V = (v[0].V + v[4].V) * 0.5f;	//08
						pointOnEdge[9].V = (v[1].V + v[5].V) * 0.5f;	//09
						pointOnEdge[10].V = (v[3].V + v[7].V) * 0.5f;	//10
						pointOnEdge[11].V = (v[2].V + v[6].V) * 0.5f;

						pointOnEdge[0].V = max(v[0].V, v[1].V);	//00
						pointOnEdge[1].V = max(v[1].V, v[2].V);	//01
						pointOnEdge[2].V = max(v[3].V, v[2].V);	//02
						pointOnEdge[3].V = max(v[0].V, v[3].V);	//03
						pointOnEdge[4].V = max(v[4].V, v[5].V);	//04
						pointOnEdge[5].V = max(v[5].V, v[6].V);	//05
						pointOnEdge[6].V = max(v[7].V, v[6].V);	//06
						pointOnEdge[7].V = max(v[4].V, v[7].V);	//07
						pointOnEdge[8].V = max(v[0].V, v[4].V);	//08
						pointOnEdge[9].V = max(v[1].V, v[5].V);	//09
						pointOnEdge[10].V = max(v[3].V, v[7].V);	//10
						pointOnEdge[11].V = max(v[2].V, v[6].V);
					}

					//利用 triangleCase 来生成迷之三角形
					for (int tri = 0; tri < 16; tri += 3)
					{
						if (triCase.Edges[tri] == -1)
							break;

						// 减的那个东西是为了让模型靠近原点
						KVector3 v0(pointOnEdge[triCase.Edges[tri + 0]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeY*0.5f, BigCubeSizeZ*0.5f));
						KVector3 v1(pointOnEdge[triCase.Edges[tri + 1]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeY*0.5f, BigCubeSizeZ*0.5f));
						KVector3 v2(pointOnEdge[triCase.Edges[tri + 2]] - KVector3(BigCubeSizeX*0.5f, BigCubeSizeY*0.5f, BigCubeSizeZ*0.5f));

						// 为三角形顶点赋值
						v0.V = pointOnEdge[triCase.Edges[tri + 0]].V;
						v1.V = pointOnEdge[triCase.Edges[tri + 1]].V;
						v2.V = pointOnEdge[triCase.Edges[tri + 2]].V;


						swap(v0.y, v0.z);
						swap(v1.y, v1.z);
						swap(v2.y, v2.z);


						m_MeshOutput.push_back(v0);
						m_MeshOutput.push_back(v1);
						m_MeshOutput.push_back(v2);

					}//for all triangle
				}// for all contour
			}//for i
		}//for j
	}//for k




	return;
}

void MC::Reconstruct16_1(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues)
{
	static int CASE_MASK[8] = { 1,2,4,8,16,32,64,128 };

	// 每条边2个顶点
	static int s_edges[12][2] =
	{
		{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 },
	};


	// 小立方体8个顶点坐标、梯度
	double pts[8][3];

	// 小立方体8个顶点梯度
	double gradients[8][3];

	// 切片尺寸
	int sliceSize = dims[0] * dims[1];

	// 小立方体8个顶点要素值
	double s[8];
	//
	double x[3];

	KMarchingCubesTriangleCases* triCases = KMarchingCubesTriangleCases::GetCases();

	// 最大最小值
	double min = 100;
	double max = 550;

	// 遍历所有的体素细胞，使用MC算法生成三角形和点梯度
	for (int k = 0; k < (dims[2] - 1); k++)
	{
		// 面偏移
		int kOffset = k * sliceSize;

		// z坐标
		//pts[0][2] = origin[2] + (j + extent[4]) * spacing[2];
		pts[0][2] = origin[2] + k * spacing[2];
		double zp = pts[0][2] + spacing[2];

		for (int j = 0; j < (dims[1] - 1); j++)
		{
			// 行偏移
			int jOffset = j * dims[0];

			// y坐标
			//pts[0][1] = origin[1] + (j + extent[2]) * spacing[1];
			pts[0][1] = origin[1] + j * spacing[1];
			double yp = pts[0][1] + spacing[1];

			for (int i = 0; i < (dims[0] - 1); i++)
			{
				// 取得小立方体的8个顶点的要素值
				int idx = i + jOffset + kOffset;
				s[0] = scalars[idx];
				s[1] = scalars[idx + 1];
				s[2] = scalars[idx + 1 + dims[0]];
				s[3] = scalars[idx + dims[0]];
				s[4] = scalars[idx + sliceSize];
				s[5] = scalars[idx + 1 + sliceSize];
				s[6] = scalars[idx + 1 + dims[0] + sliceSize];
				s[7] = scalars[idx + dims[0] + sliceSize];

				// 要素值不在范围内
				if ((s[0] < min && s[1] < min && s[2] < min && s[3] < min &&
					s[4] < min && s[5] < min && s[6] < min && s[7] < min) ||
					(s[0] > max && s[1] > max && s[2] > max && s[3] > max &&
						s[4] > max && s[5] > max && s[6] > max && s[7] > max))
				{
					continue; // no contours possible
				}

				//create voxel points
				//pts[0][0] = origin[0] + (i + extent[0]) * spacing[0];
				pts[0][0] = origin[0] + i * spacing[0];
				double xp = pts[0][0] + spacing[0];

				pts[1][0] = xp;
				pts[1][1] = pts[0][1];
				pts[1][2] = pts[0][2];

				pts[2][0] = xp;
				pts[2][1] = yp;
				pts[2][2] = pts[0][2];

				pts[3][0] = pts[0][0];
				pts[3][1] = yp;
				pts[3][2] = pts[0][2];

				pts[4][0] = pts[0][0];
				pts[4][1] = pts[0][1];
				pts[4][2] = zp;

				pts[5][0] = xp;
				pts[5][1] = pts[0][1];
				pts[5][2] = zp;

				pts[6][0] = xp;
				pts[6][1] = yp;
				pts[6][2] = zp;

				pts[7][0] = pts[0][0];
				pts[7][1] = yp;
				pts[7][2] = zp;

				// 计算梯度。。。

				double value = 300;

				// 等值线数量
				for (int contNum = 0; contNum < numValues; contNum++) { value = values[contNum];

					// 根据8个顶点来计算要用256种情况的哪一种
					int index = 0;
					for (int ii = 0; ii < 8; ii++)
					{
						int currentCubeIndexK = 0;

						//取当前切片层用以判断顶点是否在物体内部
						if (ii == 0 || ii == 1 || ii == 4 || ii == 5)
						{
							currentCubeIndexK = k + 0;	//立方体下表面
						}
						else
						{
							currentCubeIndexK = k + 1;	//立方体上表面
						}

						//if (s[ii] >= value && s[ii] < value + 50)
						if (s[ii] >= value)
						{
							//index |= CASE_MASK[ii];		// 1 << ii
							index |= 1 << ii;
						}
					}

					//这两个case不会生成任何三角形
					if (index == 0 || index == 255) //no surface
					{
						continue;
					}

					KMarchingCubesTriangleCases* triCase = triCases + index;
					int* aedge = triCase->Edges;

					// for each triangle
					for (; aedge[0] > -1; aedge += 3)
					{
						// insert a triangle
						for (int ii = 0; ii < 3; ii++)
						{
							// 线性插值计算坐标 LERP
							int* vert = s_edges[aedge[ii]];
							if (KMaths::IsEqual(s[vert[1]], s[vert[0]]))
								break;

							double t = (value - s[vert[0]]) / (s[vert[1]] - s[vert[0]]);

							double* x1 = pts[vert[0]];
							double* x2 = pts[vert[1]];

							// 坐标插值
							x[0] = x1[0] + t * (x2[0] - x1[0]);		// x
							x[1] = x1[1] + t * (x2[1] - x1[1]);		// y
							x[2] = x1[2] + t * (x2[2] - x1[2]);

							//KVector3 v = KVector3((float)x[0], (float)x[1], (float)x[2]);
							KVector3 v = KVector3((float)x[0], (float)x[2], (float)x[1]);
							//v.V = (float)value;
							v.V = (float)KMaths::Lerp(s[vert[0]], s[vert[1]], t);

							m_MeshOutput.push_back(v);

						}//for(ii)

					}//for each triangle
				}//for(all contours)
			}//for(i)
		}//for(j)
	}//for(k)

	return;
}


// 
// scalars		- 3D网格点要素值
// dims[3]		- 3D网格维度(x,y,z)
// origin[3]	- 原点坐标
// spacing[3]	- 移动立方体大小(cx,cy,cz)
// 
void MC::Reconstruct16_2(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues)
{
	static int CASE_MASK[8] = { 1,2,4,8,16,32,64,128 };

	// 每条边2个顶点
	static int s_edges[12][2] =
	{
		{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 },
	};


	// 小立方体8个顶点坐标、梯度
	double pts[8][3];

	// 小立方体8个顶点梯度
	double gradients[8][3];

	// 小立方体8个顶点要素值
	double s[8];

	// 立方体6个面范围?
	int extent[6] = { 0,67,0,51,0,0 };
	//vtkInformation *inInfo = self->GetExecutive()->GetInputInformation(0, 0);
	//inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);


	// 
	double x[3];

	// 
	KMarchingCubesTriangleCases* triCases = KMarchingCubesTriangleCases::GetCases();

	// 切片尺寸
	int sliceSize = dims[0] * dims[1];

	// 最大最小值
	double min = 100;
	double max = 550;

	// 原点坐标
	double x0 = origin[0];
	double y0 = origin[1];
	double z0 = origin[2];

	// 小立方体尺寸
	double cx = spacing[0];
	double cy = spacing[1];
	double cz = spacing[2];

	// 遍历所有的体素细胞，使用MC算法生成三角形和点梯度
	for (int k = 0; k < (dims[2] - 1); k++)
	{
		// 面偏移
		int kOffset = k * sliceSize;

		// z坐标
		//pts[0][2] = origin[2] + (k + extent[4]) * spacing[2];
		pts[0][2] = z0 + k * cz;
		double zp = pts[0][2] + cz;

		for (int j = 0; j < (dims[1] - 1); j++)
		{
			int jOffset = j * dims[0];

			// y坐标
			//pts[0][1] = origin[1] + (j + extent[2]) * spacing[1];
			pts[0][1] = y0 + j * cy;
			double yp = pts[0][1] + cy;

			for (int i = 0; i < (dims[0] - 1); i++)
			{
				// 移动立方体顶点顺序跟1987年论文的定义一样
				// 底面 p0->p1->p2->p3
				// 顶面 p4->p5->p6->p7

				// 取得小立方体的8个顶点的标量值
				int idx = kOffset + jOffset + i;

				// 底面
				s[0] = scalars[idx];
				s[1] = scalars[idx + 1];
				s[2] = scalars[idx + dims[0] + 1];
				s[3] = scalars[idx + dims[0]];

				// 顶面
				s[4] = scalars[idx + sliceSize];
				s[5] = scalars[idx + sliceSize + 1];
				s[6] = scalars[idx + sliceSize + dims[0] + 1];
				s[7] = scalars[idx + sliceSize + dims[0]];

				// 要素值不在范围内
				if ((s[0] < min && s[1] < min && s[2] < min && s[3] < min &&
					s[4] < min && s[5] < min && s[6] < min && s[7] < min) ||
					(s[0] > max && s[1] > max && s[2] > max && s[3] > max &&
						s[4] > max && s[5] > max && s[6] > max && s[7] > max))
				{
					continue; // no contours possible
				}

				//create voxel points
				//pts[0][0] = origin[0] + (i + extent[0]) * spacing[0];
				pts[0][0] = x0 + i * cx;
				double xp = pts[0][0] + cx;

				// p1 
				pts[1][0] = xp;
				pts[1][1] = pts[0][1];
				pts[1][2] = pts[0][2];

				// p2
				pts[2][0] = xp;
				pts[2][1] = yp;
				pts[2][2] = pts[0][2];

				// p3
				pts[3][0] = pts[0][0];
				pts[3][1] = yp;
				pts[3][2] = pts[0][2];

				// p4
				pts[4][0] = pts[0][0];
				pts[4][1] = pts[0][1];
				pts[4][2] = zp;

				// p5
				pts[5][0] = xp;
				pts[5][1] = pts[0][1];
				pts[5][2] = zp;

				// p6
				pts[6][0] = xp;
				pts[6][1] = yp;
				pts[6][2] = zp;

				// p7
				pts[7][0] = pts[0][0];
				pts[7][1] = yp;
				pts[7][2] = zp;

				// 计算梯度。。。

				// 等值线数量
				for (int contNum = 0; contNum < numValues; contNum++)
				{
					double value = values[contNum];

					// 根据8个顶点来计算要用256种情况的哪一种
					int index = 0;
					for (int ii = 0; ii < 8; ii++)
					{
						if (s[ii] >= value)
						{
							//index |= CASE_MASK[ii];		// 1 << ii
							index |= 1 << ii;
						}
					}

					//这两个case不会生成任何三角形
					if (index == 0 || index == 255) //no surface
					{
						continue;
					}

					int* aedge = triCases[index].Edges;

					// for each triangle
					for (; aedge[0] > -1; aedge += 3)
					{
						// insert a triangle
						for (int ii = 0; ii < 3; ii++)
						{
							// 线性插值计算坐标 LERP
							int* vert = s_edges[aedge[ii]];
							double t = (value - s[vert[0]]) / (s[vert[1]] - s[vert[0]]);
							double* x1 = pts[vert[0]];
							double* x2 = pts[vert[1]];
							x[0] = x1[0] + t * (x2[0] - x1[0]);		// x
							x[1] = x1[1] + t * (x2[1] - x1[1]);		// y
							x[2] = x1[2] + t * (x2[2] - x1[2]);		// z

							//KVector3 v = KVector3((float)x[0], (float)x[1], (float)x[2]);
							KVector3 v = KVector3((float)x[0], (float)x[2], (float)x[1]);	// x,z,y
							v.V = (float)value;

							m_MeshOutput.push_back(v);

						}//for(ii)

					}//for each triangle
				}//for(all contours)
			}//for(i)
		}//for(j)
	}//for(k)

	return;
}


// ok

// scalars		- 3D网格点要素值
// dims[3]		- 3D网格维度(x,y,z)
// origin[3]	- 原点坐标
// cubenums[3]	- 移动立方体数量
// cubesize[3]	- 移动立方体大小(cx,cy,cz)
void MC::Reconstruct16_3(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues)
{
	// CASE_MASK
	static int CASE_MASK[8] = { 1,2,4,8,16,32,64,128 };

	// 每条边2个顶点
	static int s_edges[12][2] =
	{
		{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 },
	};

	// 三维格点维度
	int iSliceNX = dims[0];
	int iSliceNY = dims[1];
	int iSliceNZ = dims[2];

	// 切片尺寸
	int sliceSize = iSliceNX * iSliceNY;
	int allSize = iSliceNX * iSliceNY * iSliceNZ;

	// 计算一堆小方块堆成的大方块大小
	int iCubeNX = cubeCount[0];
	int iCubeNY = cubeCount[1];
	int iCubeNZ = cubeCount[2];

	double cx = cubeSize[0];
	double cy = cubeSize[1];
	double cz = cubeSize[2];

	// 大方块大小
	double dBigCubeCX = cx * iCubeNX;
	double dBigCubeCY = cy * iCubeNY;
	double dBigCubeCZ = cz * iCubeNZ;

	double x0 = 0;
	double y0 = 0;
	double z0 = 0;

	// 移动立方体8个顶点标量值
	double s[8];

	double x[3];

	// 小立方体8个顶点坐标、梯度
	double pts[8][3];


	// 
	KMarchingCubesTriangleCases* triCases = KMarchingCubesTriangleCases::GetCases();

	// 遍历所有的体素细胞使用MC算法生成三角形和点梯度
	for (int k = 0; k < iCubeNZ; k++)
	{
		pts[0][2] = z0 + k * cz;
		double zp = pts[0][2] + cz;

		for (int j = 0; j < iCubeNY; j++)
		{
			pts[0][1] = y0 + j * cy;
			double yp = pts[0][1] + cy;

			for (int i = 0; i < iCubeNX; i++)
			{
				// 小立方体8个顶点坐标
				pts[0][0] = x0 + i * cx;
				double xp = pts[0][0] + cx;

				// p1 
				pts[1][0] = xp;
				pts[1][1] = pts[0][1];
				pts[1][2] = pts[0][2];

				// p2
				pts[2][0] = xp;
				pts[2][1] = yp;
				pts[2][2] = pts[0][2];

				// p3
				pts[3][0] = pts[0][0];
				pts[3][1] = yp;
				pts[3][2] = pts[0][2];

				// p4
				pts[4][0] = pts[0][0];
				pts[4][1] = pts[0][1];
				pts[4][2] = zp;

				// p5
				pts[5][0] = xp;
				pts[5][1] = pts[0][1];
				pts[5][2] = zp;

				// p6
				pts[6][0] = xp;
				pts[6][1] = yp;
				pts[6][2] = zp;

				// p7
				pts[7][0] = pts[0][0];
				pts[7][1] = yp;
				pts[7][2] = zp;

				// 等值线数量
				double value = 50;
				for (int contNum = 0; contNum < numValues; contNum++)
				{
					value = values[contNum];

					// 根据8个顶点来计算要用256种情况的哪一种
					int index = 0;
					for (int ii = 0; ii < 8; ii++)
					{
						//先求对应点的像素坐标
						int xindex = (int)(pts[ii][0] / dBigCubeCX * iSliceNX);
						if (xindex >= iSliceNX) xindex--;

						int yindex = (int)(pts[ii][1] / dBigCubeCY * iSliceNY);
						if (yindex >= iSliceNY) yindex--;

						int zindex = (int)(pts[ii][2] / dBigCubeCZ * iSliceNZ);
						if (zindex >= iSliceNZ) zindex--;

						int pos = zindex * iSliceNX*iSliceNY + yindex * iSliceNX + xindex;
						if (pos >= allSize)
							continue;

						// 8个顶点标量值(等待计算)
						s[ii] = scalars[pos];

						//if (s[ii] >= value && s[ii]<value + 50)
						if (s[ii] >= value)
						{
							index |= 1 << ii;
						}
					}

					//这两个case不会生成任何三角形
					if (index == 0 || index == 255) //no surface
					{
						continue;
					}

					int* aedge = triCases[index].Edges;

					// for each triangle
					for (; aedge[0] > -1; aedge += 3)
					{
						//for one triangle
						for (int ii = 0; ii < 3; ii++)
						{
							// 线性插值计算坐标 LERP
							int* vert = s_edges[aedge[ii]];
							if (KMaths::IsEqual(s[vert[1]], s[vert[0]]))
								break;

							double t = (value - s[vert[0]]) / (s[vert[1]] - s[vert[0]]);

							double* pt1 = pts[vert[0]];
							double* pt2 = pts[vert[1]];

							double x = pt1[0] + t * (pt2[0] - pt1[0]);
							double y = pt1[1] + t * (pt2[1] - pt1[1]);
							double z = pt1[2] + t * (pt2[2] - pt1[2]);


							// 靠近中心点
							x -= dBigCubeCX / 2;
							y -= dBigCubeCY / 2;
							z -= dBigCubeCZ / 2;

							// 标准化处理
							//x /= dBigCubeCX;
							//y /= dBigCubeCY;
							//z /= dBigCubeCZ;

							//KVector3 vec = KVector3((float)x, (float)y, (float)z);
							KVector3 vec = KVector3((float)x, (float)z, (float)y);
							vec.V = (float)value;

							m_MeshOutput.push_back(vec);

						}// for one triangle
					}//for each triangle
				}// for all contour
			}//for i
		}//for j
	}//for k

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


	uint32 triangleCount = (uint32)inVertexBuffer.size() / 3;
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







