#include "stdafx.h"
#include "KUtil.h"


KUtil::KUtil()
{
}

KUtil::~KUtil()
{
}







//static
_tstring KUtil::HR2String(HRESULT hr)
{
	_com_error err(hr);
	_tstring errMsg(err.ErrorMessage());
	return errMsg;
}

//static
void KUtil::Trace(LPCTSTR lpszInfo)
{
	if (lpszInfo)
	{
		_tstring t = _tstring(lpszInfo) + _T("\n");
		OutputDebugString(t.c_str());
	}
	else
	{
		//OutputDebugString(_T(""));
	}
}
