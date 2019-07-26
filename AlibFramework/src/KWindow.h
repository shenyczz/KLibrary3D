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
******************************************************************************/
#pragma once

class KApplication;
class KMouseListener;

class ALIBFRAMEWORK_EXPORTS_CLASS KWindow : KObject
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
	virtual void OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnDeactivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnEnterSizeMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual void OnExitSizeMove(HWND hWnd, WPARAM wParam, LPARAM lParam);

public:
	void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseDblClick(HWND hWnd, WPARAM wParam, LPARAM lParam);

	void MouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseHWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);


public:
	HWND Handle() const { return m_hWnd; }
	UINT ClientWidth() const { return m_iClientWidth; }
	UINT ClientHeight() const { return m_iClientHeight; }
	float AspectRatio() const { return m_fAspectRatio; }

	LPCTSTR GetWindowTitle() const { return m_WindowTitle.c_str(); }
	void SetWindowTitle(LPCTSTR lpszTitle) { m_WindowTitle = lpszTitle; }

	KMouseListener* GetMouseListener() const { return m_pMouseListener; }


protected:
	HWND m_hWnd;						// 窗口句柄

	UINT m_iClientWidth;				// 窗口客户区宽度
	UINT m_iClientHeight;				// 窗口客户区高度

	bool m_AppPaused;					// the application paused
	bool m_Minimized;					// the application minimized
	bool m_Maximized;					// the application maximized
	bool m_Resizing;					// the resize bars being dragged
	bool m_FullScreenEnalbe;			// fullscreen enabled

	float m_fAspectRatio;				// 窗口纵横比

	KMouseListener* m_pMouseListener;	// 鼠标监听器

private:
	bool m_QuitMessage;					// WM_QUIT

	_tstring m_WindowTitle;				// 窗口标题
	_tstring m_WindowClassName;			// 窗口类

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

