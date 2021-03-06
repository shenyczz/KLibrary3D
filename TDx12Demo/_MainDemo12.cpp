//TDx12Demo.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"

#include "Adx12Demo.h"
#include "ex01_Box.h"



int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// 这些参数不使用
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 允许在Debug版本进行运行时内存分配和泄漏检测
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int nExitCode = 0;

	//===============
	alib_test_TVector3();
	alib_test__TTuple();
	//===============

	try
	{
		// 1.取得 Appliction 对象指针
		KApplication* pApp = KApplication::GetApp();

		//=================================================
		//typedef	Adx12Demo	_dx12Sample;
		typedef	ex01_Box	_dx12Sample;
		//=================================================

		// 2.初始化
		_tstring szTitle = _tstring(_T("DirectX 12 in Win10 Code")) + _T(" --- ") + KString::TString(typeid(_dx12Sample).name());
		pApp->Initialize(hInstance, 1280, 720, szTitle.c_str());

		// 3.创建样本
		KSmartPointer<KSample> sample = KObject::CreateObject<_dx12Sample>();

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
