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


#pragma region --��Ⱦ��������

private:
	void Initialize();						// �� ʼ ��: ���� OnInit()

protected:
	virtual void OnInit() = 0;				// �� ʼ ��
	virtual void OnResize() = 0;			// ���ڸı�
	virtual void OnUpdate() = 0;			// ϵͳ����
	virtual void OnRender() = 0;			// ϵͳ��Ⱦ
	virtual void OnDestroy() = 0;			// ϵͳ����
	virtual void OnApplicationIdle() = 0;	// Ӧ�ÿ���

#pragma endregion


#pragma region --�����Ϣ

private:
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
	const float& AspectRatio() const;
	const KWindow* GetWindow() const { return m_pWindow; }

protected:
	KDocument* m_pDocument;
	KWindow* m_pWindow;

private:
	friend class KWindow;
	friend class KApplication;

	//@EndOf(KSample)
};

