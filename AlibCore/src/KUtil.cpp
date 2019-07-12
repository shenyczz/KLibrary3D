#include "stdafx.h"
#include "KUtil.h"


KUtil::KUtil()
{
}

KUtil::~KUtil()
{
}







//static
_tstring KUtil::ToString(HRESULT hr)
{
	_com_error err(hr);
	_tstring errMsg(err.ErrorMessage());
	return errMsg;
}

//static
void KUtil::Trace(LPCTSTR lpszInfo)
{
	_tstring t = _tstring(lpszInfo) + _T("\n");
	OutputDebugString(t.c_str());
}

//static
std::string KUtil::AString(LPCSTR str)
{
	return std::string(str);
}
//static
std::string KUtil::AString(LPCWSTR str)
{
	std::string st = "";

	// 取得需要的最小缓冲区大小
	DWORD dwMinSize = WideCharToMultiByte(CP_OEMCP, 0, str, -1, NULL, 0, NULL, FALSE);
	CHAR* buffer = new CHAR[dwMinSize+1]{  };
	::WideCharToMultiByte(CP_OEMCP, 0, str, -1, buffer, dwMinSize, NULL, FALSE);
	st = std::string(buffer);
	_deletea(buffer);

	return st;
}


std::wstring KUtil::WString(LPCSTR str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buffer, 512);
	return std::wstring(buffer);
}

std::wstring KUtil::WString(LPCWSTR str)
{
	return std::wstring(str);
}

//static
_tstring KUtil::TString(LPCSTR lpsz)
{
	_tstring ts;

#ifdef _UNICODE
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpsz, -1, buffer, 512);
	ts = _tstring(buffer);
#else
	ts = _tstring(lpsz);
#endif

	return ts;
}

//static
_tstring KUtil::TString(LPCWSTR lpsz)
{
	_tstring ts;

#ifdef _UNICODE
	ts = _tstring(lpsz);
#else
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpsz, -1, buffer, 512);
	ts = _tstring(buffer);
#endif

	return ts;
}
