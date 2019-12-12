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

#include "KObject.h"
#include "KMouseEventArgs.h"

class ALIBFRAMEWORK_EXPORTS_CLASS KMouseListener : KObject
{
public:
	KMouseListener();
	KMouseListener(LPVOID pOwner);
	KMouseListener(const KMouseListener&) = delete;
	~KMouseListener();

public:
	// 鼠标按下
	void MouseDown(LPVOID pSender, KMouseEventArgs e) {}
	//void MouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam);











	POINT m_point;


	//@EndOf(KMouseListener)
};


/*

#region Public Functions - Mouse Events handdler

/// <summary>
/// 鼠标按下
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
public abstract void MouseDown(Object sender, EventArgs e);

/// <summary>
/// 鼠标释放
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
public abstract void MouseUp(Object sender, EventArgs e);

/// <summary>
/// 鼠标移动
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
public abstract void MouseMove(Object sender, EventArgs e);

/// <summary>
/// 鼠标转轮转动
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
public virtual void MouseWheel(Object sender, EventArgs e)
{
this.OnMouseWheel(e);
}

/// <summary>
/// 鼠标进入
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
public virtual void MouseEnter(Object sender, EventArgs e)
{
this.OnMouseEnter(e);
}

/// <summary>
/// 鼠标离开
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
public virtual void MouseLeave(Object sender, EventArgs e)
{
this.OnMouseLeave(e);
}

#endregion

#region Protected Functions

protected virtual void OnMouseLButtonDown(EventArgs e) { }
protected virtual void OnMouseLButtonUp(EventArgs e) { }
protected virtual void OnMouseLDoubleClick(EventArgs e) { }

protected virtual void OnMouseRButtonDown(EventArgs e) { }
protected virtual void OnMouseRButtonUp(EventArgs e) { }
protected virtual void OnMouseRDoubleClick(EventArgs e) { }

protected virtual void OnMouseMove(EventArgs e) { }
protected virtual void OnMouseHover(EventArgs e) { }
protected virtual void OnMouseDrag(EventArgs e) { }

protected abstract void OnMouseWheel(EventArgs e);
protected abstract void OnMouseEnter(EventArgs e);
protected abstract void OnMouseLeave(EventArgs e);


#endregion

*/