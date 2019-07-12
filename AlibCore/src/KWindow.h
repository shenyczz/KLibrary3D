/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: …Í”¿≥Ω.÷£÷› (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

#include "KObject.h"

class KApplication;
class KMouseListener;

class ALIBCORE_EXPORTS_CLASS KWindow : KObject
{
public:
	KWindow();
	KWindow(const KWindow&) = delete;
	~KWindow();

public:
	void Initialize(KApplication* pApplication, int width, int heightt, LPCTSTR lpszName);
	int MainLoop();

public:
	virtual void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnActivte(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnDeactivte(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnQuit(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam);

public:
	void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	//void MouseDBLClick(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseHWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);


public:
	HWND Handle() const { return m_hWnd; }
	UINT Width() const { return m_iWidth; }
	UINT Height() const { return m_iHeight; }
	float AaspectRatio() const { return m_fAspectRatio; }

	KMouseListener* GetMouseListener() { return m_pMouseListener; }


protected:
	HWND m_hWnd;

	UINT m_iWidth;
	UINT m_iHeight;

	float m_fAspectRatio;

	bool m_AppPaused;			// the application paused
	bool m_Minimized;			// the application minimized
	bool m_Maximized;			// the application maximized
	bool m_Resizing;			// the resize bars being dragged
	bool m_FullScreenEnalbe;	// fullscreen enabled

	_tstring m_WindowTitle;
	_tstring m_WindowClassName;

	KMouseListener* m_pMouseListener;

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

