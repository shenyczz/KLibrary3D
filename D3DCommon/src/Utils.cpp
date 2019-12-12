#include "stdafx.h"
#include "Utils.h"

namespace dxCommon
{
	//static
	const XMFLOAT4X4 Utils::Identity4x4 =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};


	Utils::Utils()
	{
	}


	Utils::~Utils()
	{
	}


	//static
	// �ڵ�����������������ʽ��������Ϣ����ѡ�Ĵ��󴰿ڵ���(�Ѻ���)
	// [In]lpszFile			��ǰ�ļ�����ͨ�����ݺ�__FILEW__
	// [In]hlslFileName     ��ǰ�кţ�ͨ�����ݺ�__LINE__
	// [In]hr				����ִ�г�������ʱ���ص�HRESULTֵ
	// [In]lpszMsg			���ڰ������Զ�λ���ַ�����ͨ������L#x(����ΪNULL)
	// [In]bPopMsgBox       ���ΪTRUE���򵯳�һ����Ϣ������֪������Ϣ
	// ����ֵ: �β�hr
	//dxCommon::Utils::Trace(__FILET__, (DWORD)__LINE__, hr, _T(#x), true);
	HRESULT Utils::Trace(_In_z_ LPCTSTR lpszFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ LPCTSTR lpszMsg, _In_ bool bPopMsgBox)
	{
		TCHAR strBufferFile[MAX_PATH];
		TCHAR strBufferLine[128];
		TCHAR strBufferError[300];
		TCHAR strBufferMsg[1024];
		TCHAR strBufferHR[40];
		TCHAR strBuffer[3000];

		_stprintf_s(strBufferLine, 128, _T("%lu"), dwLine);

		if (lpszFile)
		{
			_stprintf_s(strBuffer, 3000, _T("%ls(%ls): "), lpszFile, strBufferLine);
			OutputDebugString(strBuffer);
		}

		size_t nMsgLen = (lpszMsg) ? _tcsnlen(lpszMsg, 1024) : 0;
		if (nMsgLen > 0)
		{
			OutputDebugString(lpszMsg);
			OutputDebugString(_T(" "));
		}

		// Windows SDK 8.0��DirectX�Ĵ�����Ϣ�Ѿ����ɽ��������У�����ͨ��FormatMessageW��ȡ������Ϣ�ַ���
		// ����Ҫ�����ַ����ڴ�
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			strBufferError, 256, nullptr);

		TCHAR* errorStr = _tcsrchr(strBufferError, _T('\r'));
		if (errorStr)
		{
			errorStr[0] = _T('\0');	// ����FormatMessageW�����Ļ��з�(��\r\n��\r�û�Ϊ\0����)
		}

		_stprintf_s(strBufferHR, 40, _T(" (0x%0.8x)"), hr);
		_tcscat_s(strBufferError, strBufferHR);
		_stprintf_s(strBuffer, 3000, _T("�����뺬�壺%ls"), strBufferError);
		OutputDebugString(strBuffer);
		OutputDebugString(_T("\n"));

		if (bPopMsgBox)
		{
			_tcscpy_s(strBufferFile, MAX_PATH, _T(""));
			if (lpszFile)_tcscpy_s(strBufferFile, MAX_PATH, lpszFile);
			_tcscpy_s(strBufferMsg, 1024, _T(""));

			if (nMsgLen > 0) _stprintf_s(strBufferMsg, 1024, _T("��ǰ���ã�%ls\n"), lpszMsg);
			_stprintf_s(strBuffer, 3000, _T("�ļ�����%ls\n�кţ�%ls\n�����뺬�壺%ls\n%ls����Ҫ���Ե�ǰӦ�ó�����"),
				strBufferFile, strBufferLine, strBufferError, strBufferMsg);

			int nResult = MessageBox(GetForegroundWindow(), strBuffer, _T("����"), MB_YESNO | MB_ICONERROR);
			if (nResult == IDYES)
			{
				DebugBreak();
			}
		}

		return hr;
	}




}// dxCommon

