/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
* Usage:
*		1.ȡ�� Appliction ����ָ��
*		KApplication* pApp = KApplication::GetApp();
*
*		2.��ʼ��Ӧ��ʵ������ȡ������ָ��
*		_tstring szTitle = _tstring(_T("Ӧ�ñ���));
*		pApp->Initialize(hInstance, 1024, 768, szTitle.c_str());
*
*		3.��ʼ�����������봰����ָ��
*		KSmartPointer<KSample> sample = KUtil::CreateSample<Triangle>();
*
*		4.ִ������
*		pApp->PerformSample(sample.Get());
*
*		5.���У�������Ϣѭ����
*		int nExitCode = pApp->Run(nCmdShow);
*
*		6.Ӧ�ý���
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
	_tstring m_Startup;	// ����·��


	KDocument* m_pDocument;	// �ĵ�
	KWindow* m_pWindow;		// ����
	KClock* m_pClock;		// ʱ��

	KSample* m_pSample;		// ����(����ָ��)

public:
	static KApplication* GetApp() { return s_pApplication; }

private:
	static KApplication* s_pApplication;

};

