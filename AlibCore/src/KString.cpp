#include "stdafx.h"
#include "KString.h"

//static
KString KString::s_string = (_T(""));


KString::KString()
	:m_string(_T("")) {}

KString::KString(const KString& rhs)
	: m_string(rhs.m_string) {}

KString::KString(LPCTSTR pcsz)
	: m_string(pcsz) {}

KString::~KString()
{}





// 操作符重载 []
TCHAR KString::operator[](int iChar) const
{
	if ((iChar < 0) || (iChar > (int)m_string.length()))
		return _T('\0');

	return m_string[iChar];
}


KString::operator LPCTSTR()
{
	return m_string.c_str();
}


//static
_astring KString::AString(LPCSTR str)
{
	return _astring(str);
}

//static
_astring KString::AString(LPCWSTR str)
{
	_astring astr = "";

	// 取得需要的最小缓冲区大小
	DWORD dwMinSize = WideCharToMultiByte(CP_OEMCP, 0, str, -1, NULL, 0, NULL, FALSE);
	CHAR* buffer = new CHAR[dwMinSize + 1]{};
	::WideCharToMultiByte(CP_OEMCP, 0, str, -1, buffer, dwMinSize, NULL, FALSE);
	astr = std::string(buffer);
	_deletea(buffer);

	return astr;
}

//static
_wstring KString::WString(LPCSTR str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buffer, 512);
	return _wstring(buffer);
}

//static
_wstring KString::WString(LPCWSTR str)
{
	return _wstring(str);
}

//static
_tstring KString::TString(LPCSTR lpsz)
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
_tstring KString::TString(LPCWSTR lpsz)
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

//static
KString& KString::Format(LPCTSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	{
		int len = _vsctprintf(pszFormat, argList)	// _vscprintf doesn't count
			+ 1;	// terminating '\0'

		TCHAR* pszBuffer = (TCHAR*)malloc(len * sizeof(TCHAR));
		_vstprintf_s(pszBuffer, len, pszFormat, argList);
		s_string = pszBuffer;
		free(pszBuffer);
		pszBuffer = NULL;
	}
	va_end(argList);

	return s_string;
}
