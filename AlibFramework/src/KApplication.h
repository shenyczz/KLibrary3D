/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
* Usage:
*		1.取得 Appliction 对象指针
*		KApplication* pApp = KApplication::GetApp();
*
*		2.初始化应用实例并获取窗口类指针
*		_tstring szTitle = _tstring(_T("应用标题));
*		pApp->Initialize(hInstance, 1024, 768, szTitle.c_str());
*
*		3.初始化样本，传入窗口类指针
*		KSmartPointer<KSample> sample = KUtil::CreateSample<Triangle>();
*
*		4.执行样本
*		pApp->PerformSample(sample.Get());
*
*		5.运行（进入消息循环）
*		int nExitCode = pApp->Run(nCmdShow);
*
*		6.应用结束
*		return nExitCode;
*
******************************************************************************/

#pragma once

class KDocument;
class KWindow;
class KSample;

class ALIBFRAMEWORK_EXPORTS_CLASS KApplication
{
public:
	KApplication();
	~KApplication();

public:
	void Initialize(HINSTANCE hInstance, UINT width, UINT height, LPCTSTR lpszName = NULL);
	void PerformSample(KSample* pSample);
	int Run(int nCmdShow);


public:
	HINSTANCE Instance() const { return m_hInstance; }
	LPCTSTR Startup() const { return m_Startup.c_str(); }

	KDocument* GetDocument() const { return m_pDocument; }
	KWindow* GetWindow() const { return m_pWindow; }
	KSample* GetSample() const { return m_pSample; }
	KClock* GetClock() const { return m_pClock; }

private:
	void BuildStartup(HINSTANCE hInstance);

protected:
	HINSTANCE m_hInstance;
	int m_nCmdShow;
	_tstring m_Startup;	// 启动路径


	KDocument* m_pDocument;	// 文档
	KWindow* m_pWindow;		// 窗口
	KClock* m_pClock;		// 时钟

	KSample* m_pSample;		// 样本(传入指针)

public:
	static KApplication* GetApp() { return s_pApplication; }

private:
	static KApplication* s_pApplication;

};

