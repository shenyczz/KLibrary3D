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

class ALIBCORE_EXPORTS_CLASS KObject
{
public:
	KObject();
	~KObject();

public:
	LPVOID GetOwner() { return m_pOwner; }

	LPCTSTR GetName() { return m_szName.c_str(); }
	void SetName(LPCTSTR lpszName) { m_szName = (lpszName) ? lpszName : _T(""); }

protected:
	LPVOID m_pOwner;
	_tstring m_szName;

};

