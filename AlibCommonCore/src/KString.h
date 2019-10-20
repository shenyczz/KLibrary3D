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


class ALIBCOMMONCORE_EXPORTS_CLASS KString
{
public:
	KString();
	KString(const KString& rhs);
	KString(const _tstring& rhs);
	KString(LPCTSTR pcsz);
	~KString();

public:
	KString& Append(LPCTSTR lpszAppend);
	KString& Replace(LPCTSTR lpszSource, LPCTSTR lpszDest);

	KString& Upper();
	KString& Lower();

	KString& Trim();
	KString& TrimLeft();
	KString& TrimRight();

	KString Mid(int iFirst);
	KString Mid(int iFirst, int iCount);

	KString Left(_In_ int nCount) const;
	KString Right(_In_ int nCount) const;

	int Find(LPCTSTR lpszSearch);
	int ReverseFind(LPCTSTR lpszSearch);

	LPCTSTR GetString();
	int GetLength() const;

public:
	// ���������� LPTSTR; LPCTSTR
	operator LPTSTR();
	operator LPCTSTR();

	// ���������� ()
	LPCTSTR operator ()(void);

	// ���������� +; +=
	KString operator +(LPCTSTR lpsz);
	KString& operator +=(LPCTSTR lpsz);

	// ���������� >; >=
	bool operator >(LPCTSTR lpsz);
	bool operator >=(LPCTSTR lpsz);

	// ���������� <; <=
	bool operator <(LPCTSTR lpsz);
	bool operator <=(LPCTSTR lpsz);

	// ���������� ==; !=
	bool operator ==(LPCTSTR lpsz);
	bool operator !=(LPCTSTR lpsz);

	// ���������� []
	TCHAR operator [](int iChar) const;

public:
	static _astring AString(LPCSTR lpsz);
	static _astring AString(LPCWSTR lpsz);


	static _wstring WString(LPCSTR lpsz);
	static _wstring WString(LPCWSTR lpwsz);

	static _tstring TString(LPCSTR lpsz);
	static _tstring TString(LPCWSTR lpwsz);

	// �ַ�����Сдת��
	static LPCTSTR UpperString(LPTSTR pszString);
	static LPCTSTR LowerString(LPTSTR pszString);

	// ȥ���ַ����ո�
	static LPCTSTR TrimString(LPTSTR pszString);
	static LPCTSTR TrimStringLeft(LPTSTR pszString);
	static LPCTSTR TrimStringRight(LPTSTR pszString);

	static KString Replace(LPCTSTR lpszReplaceIn, LPCTSTR lpszSource, LPCTSTR lpszDest);

	// ��ʽ��
	static KString Format(LPCTSTR pszFormat, ...);

private:
	_tstring m_string;

	static KString s_string;

	//@EndOf(KString)
};

