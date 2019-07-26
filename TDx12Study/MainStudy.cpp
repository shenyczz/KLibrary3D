//TDx12Study.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"

#include "KApplication.h"
#include "KDocument.h"

#include "Ademo.h"


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int nExitCode = 0;

	try
	{
		// 1.取得 Appliction 对象指针
		KApplication* pApp = KApplication::GetApp();

		// 2.初始化
		_tstring szTitle = _tstring(_T("Directx 12 Study"));
		pApp->Initialize(hInstance, 900, 600, szTitle.c_str());

		// 3.创建样本
		KSmartPointer<KSample> sample
			= KUtil::CreateObject<Ademo>()
			//= KUtil::CreateObject<Box>()
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

