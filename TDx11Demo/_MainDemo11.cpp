//TDx11Demo.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"

#include "Adx11Demo.h"
#include "ex01_Triangle.h"
#include "ex02_Cube.h"


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// 这些参数不使用
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 允许在Debug版本进行运行时内存分配和泄漏检测
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//===========================
	dxCommon::alib_test_VertexBase();
	//===========================

	int nExitCode = 0;

	try
	{
		// 1.取得 Appliction 对象指针
		KApplication* pApp = KApplication::GetApp();

		//=================================================
		typedef	Adx11Demo	_dx11Sample;
		//typedef	ex01_Triangle	_dx11Sample;
		//typedef	ex02_Cube	_dx11Sample;
		//=================================================

		// 2.初始化
		_tstring szTitle = _tstring(_T("DirectX 11 in Win10 Code")) + _T(" --- ") + KString::TString(typeid(_dx11Sample).name());
		pApp->Initialize(hInstance, 1280, 720, szTitle.c_str());

		// 3.创建样本
		//KSmartPointer<KSample> sample = KObject::CreateObject<_dx11Sample>();
		KSample* pKSample = new _dx11Sample();

		// 4.执行样本 
		//pApp->PerformSample(sample.Get());
		pApp->PerformSample(pKSample);

		// 5.运行（进入消息循环）
		nExitCode = pApp->Run(nCmdShow);

		_delete(pKSample);
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
