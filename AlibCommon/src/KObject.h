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

class ALIBCOMMOM_EXPORTS_CLASS KObject
{
public:
	KObject();
	~KObject();

public:
	LPVOID GetOwner() const { return m_pOwner; }
	void SetOwner(LPVOID pOwner) { m_pOwner = pOwner; }

	LPCTSTR GetName() const { return m_szName.c_str(); }
	void SetName(LPCTSTR lpszName) { m_szName = (lpszName != NULL) ? lpszName : _T(""); }

protected:
	LPVOID m_pOwner;
	_tstring m_szName;


public:
	template<typename T>
	static T* CreateObject()
	{
		return new T();
	}

};

