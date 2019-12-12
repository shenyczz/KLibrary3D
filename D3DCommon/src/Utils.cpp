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
	// 在调试输出窗口中输出格式化错误信息，可选的错误窗口弹出(已汉化)
	// [In]lpszFile			当前文件名，通常传递宏__FILEW__
	// [In]hlslFileName     当前行号，通常传递宏__LINE__
	// [In]hr				函数执行出现问题时返回的HRESULT值
	// [In]lpszMsg			用于帮助调试定位的字符串，通常传递L#x(可能为NULL)
	// [In]bPopMsgBox       如果为TRUE，则弹出一个消息弹窗告知错误信息
	// 返回值: 形参hr
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

		// Windows SDK 8.0起DirectX的错误信息已经集成进错误码中，可以通过FormatMessageW获取错误信息字符串
		// 不需要分配字符串内存
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			strBufferError, 256, nullptr);

		TCHAR* errorStr = _tcsrchr(strBufferError, _T('\r'));
		if (errorStr)
		{
			errorStr[0] = _T('\0');	// 擦除FormatMessageW带来的换行符(把\r\n的\r置换为\0即可)
		}

		_stprintf_s(strBufferHR, 40, _T(" (0x%0.8x)"), hr);
		_tcscat_s(strBufferError, strBufferHR);
		_stprintf_s(strBuffer, 3000, _T("错误码含义：%ls"), strBufferError);
		OutputDebugString(strBuffer);
		OutputDebugString(_T("\n"));

		if (bPopMsgBox)
		{
			_tcscpy_s(strBufferFile, MAX_PATH, _T(""));
			if (lpszFile)_tcscpy_s(strBufferFile, MAX_PATH, lpszFile);
			_tcscpy_s(strBufferMsg, 1024, _T(""));

			if (nMsgLen > 0) _stprintf_s(strBufferMsg, 1024, _T("当前调用：%ls\n"), lpszMsg);
			_stprintf_s(strBuffer, 3000, _T("文件名：%ls\n行号：%ls\n错误码含义：%ls\n%ls您需要调试当前应用程序吗？"),
				strBufferFile, strBufferLine, strBufferError, strBufferMsg);

			int nResult = MessageBox(GetForegroundWindow(), strBuffer, _T("错误"), MB_YESNO | MB_ICONERROR);
			if (nResult == IDYES)
			{
				DebugBreak();
			}
		}

		return hr;
	}




}// dxCommon

