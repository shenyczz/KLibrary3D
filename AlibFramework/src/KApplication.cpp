//*********************************************************
//
// Copyright (c) ShenYongchen. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR _Ty PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "stdafx.h"

#include "KApplication.h"

#include "KClock.h"
#include "KSample.h"
#include "KWindow.h"
#include "KDocument.h"
#include "KException.h"
#include "KMouseListener.h"

//static
KApplication* KApplication::s_pApplication = nullptr;
const KApplication __ebbe9801_bafa_4917_8dc9_5835f4ecb731_theApplication__;


KApplication::KApplication()
	: m_hInstance(nullptr)
	, m_pWindow(nullptr)
	, m_pDocument(nullptr)
	, m_pClock(nullptr)
	, m_pSample(nullptr)
	, m_nCmdShow(SW_SHOW)
	, m_Startup(_T(""))
{
	s_pApplication = this;
}

KApplication::~KApplication()
{
	_delete(m_pDocument);
	_delete(m_pWindow);
	_delete(m_pClock);
}



#pragma region 1.Initialize

void KApplication::Initialize(HINSTANCE hInstance, UINT width, UINT height, LPCTSTR lpszName)
{
	try
	{
		m_hInstance = hInstance;

		// 构造启动路径
		BuildStartup(hInstance);

		m_pClock = new KClock();
		m_pClock->Reset();

		m_pDocument = new KDocument();

		m_pWindow = new KWindow();
		m_pWindow->Initialize(this, width, height, lpszName);
	}
	catch (...)
	{
		throw KException(S_FALSE, __TFUNCTION__, __TFILE__, __LINE__);
	}
}
void KApplication::BuildStartup(HINSTANCE hInstance)
{
	TCHAR path[MAX_PATH] = _T("");
	DWORD size = GetModuleFileName(hInstance, path, MAX_PATH);
	if (size == 0 || size == MAX_PATH)
	{
		throw KException();
	}

	TCHAR* lastSlash = _tcsrchr(path, _T('\\'));
	if (lastSlash)
	{
		*(lastSlash + 1) = _T('\0');
	}

	m_Startup = path;

	return;
}

#pragma endregion

#pragma region 2.PerformSample

void KApplication::PerformSample(KSample* pSample)
{
	try
	{
		m_pSample = pSample;
		m_pSample->Initialize();
	}
	catch (...)
	{
		throw KException(S_FALSE, __TFUNCTION__, __TFILE__, __LINE__);
	}
}

#pragma endregion

#pragma region 3.Run

int KApplication::Run(int nCmdShow)
{
	m_nCmdShow = nCmdShow;

	HWND hWnd = m_pWindow->Handle();

	UpdateWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);

	return m_pWindow->MainLoop();
}

#pragma endregion






