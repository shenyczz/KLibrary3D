/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ÉêÓÀ³½.Ö£ÖÝ (shenyczz@163.com)
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

#ifndef ReleaseCom
#define ReleaseCom(X) { if(X){ (X)->Release(); (X) = 0; } }
#endif


class ALIBCORE_EXPORTS_CLASS KUtil
{
	KUtil();
	~KUtil();

public:
	static _tstring HR2String(HRESULT hr);
	static void Trace(LPCTSTR lpszInfo);


	static std::string AString(LPCSTR str);
	static std::string AString(LPCWSTR str);

	static std::wstring WString(LPCSTR str);
	static std::wstring WString(LPCWSTR str);

	static _tstring TString(LPCSTR lpsz);
	static _tstring TString(LPCWSTR lpsz);

public:
	template<class T>
	static T* CreateSample()
	{
		return new T();
	}

	//@EndOf(KUtil)
};

