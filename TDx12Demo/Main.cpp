//TDx12Demo.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"

#include "KApplication.h"
#include "KDocument.h"

// sfxDemo
#include "Ademo.h"



int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int nExitCode = 0;

	try
	{
		// 1.取得 Appliction 对象指针
		KApplication* pApp = KApplication::GetApp();

		_tstring s1 = KUtil::TString(typeid(KApplication).name());
		_tstring s2 = KUtil::TString(typeid(pApp).name());

		std::ifstream inFile("", std::ios::binary);

		//std::string s = (typeid(Vertex_tri)).name();

		TTuple_test();

		// 2.初始化
		_tstring szTitle = _tstring(_T("大风起兮云飞扬"));
		pApp->Initialize(hInstance, 900, 600, szTitle.c_str());

		// 3.创建样本
		KSmartPointer<KSample> sample
			= KUtil::CreateSample<Ademo>()
			;

		// 4.执行样本 
		pApp->PerformSample(sample.Get());

		// 5.运行（进入消息循环）
		nExitCode = pApp->Run(nCmdShow);

	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "HR Failed", MB_OK);
	}
	catch (KException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), _T("HR Failed"), MB_OK);
	}

	return nExitCode;
}








void a()
{
	std::string finalPath = "";
	std::ifstream inFile(finalPath, std::ios::binary);
}
