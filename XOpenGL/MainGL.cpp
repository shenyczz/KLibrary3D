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
	cy = 0;			// cubeCountY -> ��Ƭ����
	cz = 150;		// cubeCountZ

	w = 0.23f;		// С������ width
	h = 0.1f;		// С������ height
	d = 0.23f;		// С������ depth

	begID = 15;		// ��ʼ ID
	endID = 179;	// ���� ID


	MC mc;

	//����CT��Ƭ
	mc.LoadCTSlicerFromFiles("D:\\sfxData\\VTKData\\CT\\", ".dat", begID, endID, 512, 512);

	//��mc�㷨�ؽ���ģ��
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

	//����ģ�͵�stl�ļ�
	//mc.ExportFile_STL_Binary("d:\\temp\\333.stl", "sfx333aflsdjl", outVertexList);
	mc.ExportFile_STL_Binary("d:\\temp\\888.stl", "sfx888", outVertexList);

	MessageBox(NULL, _T("OK"), _T(""), MB_OK);

	return true;
}

