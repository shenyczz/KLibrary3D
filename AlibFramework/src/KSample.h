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

class KDocument;
class KWindow;

class ALIBFRAMEWORK_EXPORTS_CLASS KSample : public KObject
{

#pragma region --Constructor

private:
	KSample(const KSample&) = delete;
	KSample& operator=(const KSample& rhs) = delete;

protected:
	KSample();

public:
	~KSample() = default;

#pragma endregion


#pragma region --渲染管线流程

public:
	void Initialize();	// 初 始 化: 调用 OnInit()
	void Resize();		// 窗口改变: 调用 OnResize()
	void Update();		// 系统更新: 调用 OnUpdate()
	void Render();		// 系统渲染: 调用 OnRender()
	void Destroy();		// 系统销毁: 调用 OnDestroy()

protected:
	virtual void OnInit() = 0;
	virtual void OnResize() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

#pragma endregion


#pragma region --鼠标消息

public:
	void MouseDown(WPARAM wParam, LPARAM lParam);
	void MouseUp(WPARAM wParam, LPARAM lParam);
	void MouseMove(WPARAM wParam, LPARAM lParam);
	void MouseWheel(WPARAM wParam, LPARAM lParam);
	

protected:
	// WPARAM wParam => UINT_PTR nType
	virtual void OnMouseDown(WPARAM wParam, int x, int y) = 0;
	virtual void OnMouseUp(WPARAM wParam, int x, int y) = 0;
	virtual void OnMouseMove(WPARAM wParam, int x, int y) = 0;
	virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam) = 0;


#pragma endregion


protected:
	float AspectRatio();


protected:
	KDocument* m_pDocument;
	KWindow* m_pWindow;


	//@EndOf(KSample)
};

