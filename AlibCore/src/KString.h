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


class ALIBCORE_EXPORTS_CLASS KString
{
public:
	KString();
	KString(const KString& rhs);
	KString(LPCTSTR pcsz);
	~KString();

public:


public:

	// ≤Ÿ◊˜∑˚÷ÿ‘ÿ []
	TCHAR operator[](int iChar) const;

	// un_explicit
	operator LPCTSTR();

public:
	static _astring AString(LPCSTR str);
	static _astring AString(LPCWSTR str);


	static _wstring WString(LPCSTR str);
	static _wstring WString(LPCWSTR str);

	static _tstring TString(LPCSTR lpsz);
	static _tstring TString(LPCWSTR lpsz);

	static KString& Format(LPCTSTR pszFormat, ...);


private:
	_tstring m_string;

	static KString s_string;

	//@EndOf(KString)
};

