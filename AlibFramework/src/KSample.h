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

class ALIBFRAMEWORK_EXPORTS_CLASS KSample
{
#pragma region --Constructor

protected:
	KSample();
	KSample(const KSample&) = delete;

public:
	~KSample() = default;

#pragma endregion


#pragma region --��Ⱦ��������

public:
	void Initialize();	// �� ʼ ��: ���� OnInit()
	void Resize();		// ���ڸı�: ���� OnResize()
	void Update();		// ϵͳ����: ���� OnUpdate()
	void Render();		// ϵͳ��Ⱦ: ���� OnRender()
	void Destroy();		// ϵͳ����: ���� OnDestroy()

protected:
	virtual void OnInit() = 0;
	virtual void OnResize() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

#pragma endregion


#pragma region --�ݺ��

public:
	float GetAspectRatio() { return m_fAspectRatio; }

protected:
	float m_fAspectRatio;

#pragma endregion

protected:
	KDocument * m_pDocument;
	KWindow * m_pWindow;


	//@EndOf(KSample)
};

