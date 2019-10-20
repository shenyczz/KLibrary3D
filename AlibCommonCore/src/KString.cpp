#include "stdafx.h"
#include "KString.h"

//static
KString KString::s_string = (_T(""));


#pragma region --Constuctor

KString::KString()
	:m_string(_T("")) {}

KString::KString(const KString& rhs)
	: m_string(rhs.m_string) {}

KString::KString(const _tstring& rhs)
	: m_string(rhs) {}

KString::KString(LPCTSTR pcsz)
	: m_string(pcsz) {}

KString::~KString()
{}

#pragma endregion


#pragma region Public Functions

KString& KString::Append(LPCTSTR lpszAppend)
{
	m_string.append(lpszAppend);
	return *this;
}
KString& KString::Replace(LPCTSTR lpszSource, LPCTSTR lpszDest)
{
	_tstring strSource(lpszSource);
	_tstring strDest(lpszDest);

	_tstring::size_type pos = 0;
	_tstring::size_type lenSource = strSource.size();

	while (1)
	{
		pos = m_string.find(strSource);
		if (_tstring::npos == pos)
			break;

		m_string.replace(pos, lenSource, strDest);
	}

	return *this;
}

KString& KString::Upper()
{
	UpperString((LPTSTR)this->GetString());
	return (*this);
}
KString& KString::Lower()
{
	KString::LowerString((LPTSTR)this->GetString());
	return (*this);
}


KString& KString::Trim()
{
	TrimLeft();
	TrimRight();
	return (*this);
}
KString& KString::TrimLeft()
{
	LPTSTR pszString = (LPTSTR)this->GetString();

	while (*pszString)
	{
		if (*pszString != _T(' '))
			break;

		pszString++;
	}

	(*this) = pszString;
	return (*this);
}
KString& KString::TrimRight()
{
	LPTSTR pszString = (LPTSTR)this->GetString();
	int len = (int)_tcslen(pszString);
	pszString += len - 1;

	while (*pszString)
	{
		if (*pszString != _T(' '))
			break;

		*pszString = _T('\0');
		pszString--;
	}

	return (*this);
}

KString KString::Mid(int iFirst)
{
	TCHAR szBuffer[1024] = _T("");
	int len = (int)m_string.length();

	if (iFirst >= len)
		return NULL;

	if (iFirst < 0)
		iFirst = 0;

	//_tcscpy(szBuffer, m_string.substr(iFirst).c_str());
	_tcscpy_s(szBuffer, 1024, m_string.substr(iFirst).c_str());
	KString strBuffer(szBuffer);
	return strBuffer;

}
KString KString::Mid(int iFirst, int iCount)
{
	TCHAR szBuffer[1024] = _T("");
	int len = (int)m_string.length();

	if (iFirst >= len)
		return NULL;

	if (iFirst < 0)
		iFirst = 0;

	//_tcscpy(szBuffer, m_string.substr(iFirst, iCount).c_str());
	_tcscpy_s(szBuffer, 1024, m_string.substr(iFirst, iCount).c_str());
	KString strBuffer(szBuffer);
	return strBuffer;
}

KString KString::Left(_In_ int nCount) const
{
	if (nCount < 0)
		nCount = 0;

	int nLength = GetLength();
	if (nCount >= nLength)
	{
		return(*this);
	}

	return KString(m_string.substr(0, nCount).c_str());
}
KString KString::Right(_In_ int nCount) const
{
	if (nCount < 0)
		nCount = 0;

	int nLength = GetLength();
	if (nCount >= nLength)
	{
		return(*this);
	}

	return KString(m_string.substr(nLength - nCount, nCount).c_str());
}

int KString::Find(LPCTSTR lpszSearch)
{
	_tstring::size_type pos = m_string.find(lpszSearch);
	return (int)(_tstring::npos == pos ? -1 : pos);
}
int KString::ReverseFind(LPCTSTR lpszSearch)
{
	_tstring::size_type pos = m_string.rfind(lpszSearch);
	return (int)(_tstring::npos == pos ? -1 : pos);
}

LPCTSTR KString::GetString()
{
	return m_string.c_str();
}
int KString::GetLength() const
{
	return (int)m_string.length();
}

#pragma endregion


#pragma region --operator overrider 操作符重载

// 默认转换
KString::operator LPTSTR()
{
	return (LPTSTR)m_string.c_str();
}
KString::operator LPCTSTR()
{
	return m_string.c_str();
}

LPCTSTR KString::operator ()(void)
{
	return m_string.c_str();
}

// 操作符重载 +
KString KString::operator +(LPCTSTR lpsz)
{
	KString strReturn(*this);
	strReturn.Append(lpsz);
	return strReturn;
}

// 操作符重载 +=
KString& KString::operator +=(LPCTSTR lpsz)
{
	this->Append(lpsz);
	return *this;
}

// 操作符重载 >
bool KString::operator >(LPCTSTR lpsz)
{
	return this->m_string > _tstring(lpsz);
}

// 操作符重载 >=
bool KString::operator >=(LPCTSTR lpsz)
{
	return this->m_string >= _tstring(lpsz);
}

// 操作符重载 <
bool KString::operator <(LPCTSTR lpsz)
{
	return this->m_string < _tstring(lpsz);
}

// 操作符重载 <=
bool KString::operator <=(LPCTSTR lpsz)
{
	return this->m_string <= _tstring(lpsz);
}

// 操作符重载 ==
bool KString::operator ==(LPCTSTR lpsz)
{
	return this->m_string == _tstring(lpsz);
}

// 操作符重载 !=
bool KString::operator !=(LPCTSTR lpsz)
{
	return this->m_string != _tstring(lpsz);
}

// 操作符重载 []
TCHAR KString::operator[](int iChar) const
{
	if ((iChar < 0) || (iChar > (int)m_string.length()))
		return _T('\0');

	return m_string[iChar];
}

#pragma endregion


#pragma region --Static Functions 静态函数

//static
_astring KString::AString(LPCSTR lpsz)
{
	return _astring(lpsz);
}

//static
_astring KString::AString(LPCWSTR lpwsz)
{
	_astring astr = "";

	// 取得需要的最小缓冲区大小
	DWORD dwMinSize = WideCharToMultiByte(CP_OEMCP, 0, lpwsz, -1, NULL, 0, NULL, FALSE);
	CHAR* buffer = new CHAR[dwMinSize + 1]{};
	::WideCharToMultiByte(CP_OEMCP, 0, lpwsz, -1, buffer, dwMinSize, NULL, FALSE);
	astr = std::string(buffer);
	_deletea(buffer);

	return astr;
}

//static
_wstring KString::WString(LPCSTR lpsz)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, lpsz, -1, buffer, 512);
	return _wstring(buffer);
}

//static
_wstring KString::WString(LPCWSTR lpwsz)
{
	return _wstring(lpwsz);
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
_tstring KString::TString(LPCWSTR lpwsz)
{
	_tstring ts;

#ifdef _UNICODE
	ts = _tstring(lpwsz);
#else
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpwsz, -1, buffer, 512);
	ts = _tstring(buffer);
#endif

	return ts;
}

//static
// 字符串转换为大写
LPCTSTR KString::UpperString(LPTSTR pszString)
{
	//#define _toupper(_Char)    ( (_Char)-'a'+'A' )

	TCHAR *p = pszString;

	while (*p)
	{
		// 是字母则转换
		if (*p >= _T('a') && *p <= _T('z'))
		{
			*p = *p + _T('A') - _T('a');
		}

		p++;
	}

	return pszString;
}

//static
// 字符串转换为小写(改变原字符串)
LPCTSTR KString::LowerString(LPTSTR pszString)
{
	TCHAR *p = pszString;

	while (*p)
	{
		// 是字母则转换
		if (*p >= _T('A') && *p <= _T('Z'))
		{
			*p = *p + _T('a') - _T('A');
		}

		p++;
	}

	return pszString;
}

//static
// 去除字符串前导和尾部空格(改变原字符串)
LPCTSTR KString::TrimString(LPTSTR pszString)
{
	return TrimStringLeft((LPTSTR)TrimStringRight(pszString));
}

//static
// 去除字符串前导空格(改变原字符串)
LPCTSTR KString::TrimStringLeft(LPTSTR pszString)
{
	TCHAR *p = pszString;

	while (*p)
	{
		if (*p != _T(' '))
			break;

		p++;
	}

	return pszString;
}

//static
// 去除字符串尾部空格(改变原字符串)
LPCTSTR KString::TrimStringRight(LPTSTR pszString)
{
	TCHAR *p = pszString;
	int len = (int)_tcslen(pszString);
	p += len - 1;

	while (*p)
	{
		if (*p != _T(' '))
			break;

		*p = _T('\0');
		p--;
	}

	return pszString;
}

//static
// 替换字符串()
KString KString::Replace(LPCTSTR lpszReplaceIn, LPCTSTR lpszSource, LPCTSTR lpszDest)
{
	_tstring strReplaceIn(lpszReplaceIn);
	_tstring strSource(lpszSource);
	_tstring strDest(lpszDest);

	_tstring::size_type pos = 0;
	_tstring::size_type lenSource = strSource.size();

	while (true)
	{
		pos = strReplaceIn.find(strSource);
		if (_tstring::npos == pos)
			break;

		strReplaceIn.replace(pos, lenSource, strDest);
	}

	return strReplaceIn;
}

//static
/*
//产生"YYYY-MM-DDhh:mm:ss"格式的字符串。
char s[32];
strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&t));
*/
KString KString::Format(LPCTSTR pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	{
		int len = _vsctprintf(pszFormat, argList)	// _vscprintf doesn't count
			+ 1;									// terminating '\0'

		TCHAR* pszBuffer = (TCHAR*)malloc(len * sizeof(TCHAR));
		_vstprintf_s(pszBuffer, len, pszFormat, argList);
		s_string = pszBuffer;
		free(pszBuffer);
		pszBuffer = NULL;
	}
	va_end(argList);

	return s_string;
}

#pragma endregion

