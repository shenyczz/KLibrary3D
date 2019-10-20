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

#include "KUtil.h"

class ALIBCOMMONCORE_EXPORTS_CLASS KException
{
public:
	KException() = default;
	KException(HRESULT hr, const _tstring& functionName, const _tstring& filename, int lineNumber);
	KException(HRESULT hr, const _tstring& functionName, const _tstring& filename, int lineNumber, const _tstring& customMessage);

	_tstring ToString() const;

	HRESULT ErrorCode = S_FALSE;
	_tstring FunctionName;
	_tstring Filename;
	int LineNumber = -1;

private:
	_tstring CustomMessage;
};

inline void ThrowIfFailed_22(HRESULT hr)
{
	if (FAILED(hr))
	{
		_tstring s = KUtil::HR2String(hr);
		throw KException();
	}
}

#ifndef ThrowIfFailed
//#define ThrowIfFailed(x)	\
//{							\
//	ThrowIfFailed_22(x);	\
//}
#define ThrowIfFailed(x)			\
{									\
    HRESULT hr__ = (x);				\
	_tstring fun = __TFUNCTION__;	\
    _tstring wfn = __TFILE__;		\
    if(FAILED(hr__)) { throw KException(hr__, fun, wfn, __LINE__); } \
}
#endif


/*
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

*/
