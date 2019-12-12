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

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)												\
{																		\
    HRESULT hr_ = (x);													\
	_tstring _fun = __TFUNCTION__;										\
    _tstring _file = __TFILE__;											\
    if(FAILED(hr_)) { throw KException(hr_, _fun, _file, __LINE__); }	\
}
#endif
