#include "stdafx.h"
#include "MC.h"

int main(int argc, char* argv[])
{
	std::cout << "please enter : cubeCountX,cubeCountZ, width, height,depth,startID,endID" << std::endl;
	std::cout << "default parameters: 150 150 0.23 0.1 0.23 15 179" << std::endl;
	int cx, cy, cz, begID, endID;
	float w, h, d;
	//std::cin >> cx >> cz >> w >> h >> d >> startID >> endID; 

	cx = 150;		// cubeCountX
	cy = 0;			// cubeCountY -> 切片数量
	cz = 150;		// cubeCountZ

	w = 0.23f;		// 小立方体 width
	h = 0.1f;		// 小立方体 height
	d = 0.23f;		// 小立方体 depth

	begID = 15;		// 开始 ID
	endID = 179;	// 结束 ID


	MC mc;

	//加载CT切片
	mc.LoadCTSlicerFromFiles("D:\\sfxData\\VTKData\\CT\\", ".dat", begID, endID, 512, 512);

	//用mc算法重建出模型
	MCP mcp = {};
	{
		mcp.cubeCountX = cx;	// 150
		mcp.cubeCountY = mc.m_CTSlices.size() - 1;
		mcp.cubeCountZ = cz;	// 150
		mcp.cubeWidth = w;		// 0.23
		mcp.cubeHeight = h;		// 0.10
		mcp.cubeDepth = d;		// 0.23
	}

	mc.Reconstruct(mcp);

	std::vector<Vector3> outVertexList;
	mc.GetMesh(outVertexList);

	//保存模型到stl文件
	//mc.ExportFile_STL_Binary("d:\\temp\\333.stl", "sfx333aflsdjl", outVertexList);
	mc.ExportFile_STL_Binary("d:\\temp\\888.stl", "sfx888", outVertexList);

	MessageBox(NULL, _T("OK"), _T(""), MB_OK);

	return true;
}

