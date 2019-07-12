#include "stdafx.h"
#include "KException.h"



KException::KException(HRESULT hr, const _tstring& functionName, const _tstring& filename, int lineNumber)
	: KException(hr, functionName, filename, lineNumber, _T(""))
{

}

KException::KException(HRESULT hr, const _tstring& functionName, const _tstring& filename, int lineNumber, const _tstring& customMessage)
	: ErrorCode(hr)
	, FunctionName(functionName)
	, Filename(filename)
	, LineNumber(lineNumber)
	, CustomMessage(customMessage)
{

}

_tstring KException::ToString() const
{
	// Get the string description of the error code.
	_com_error err(ErrorCode);
	_tstring msg = err.ErrorMessage();
	return FunctionName + _T(" failed in ")
		+ Filename + _T("; line ")
		+ _ttostring(LineNumber) + _T("; error: ")
		+ msg + _T(" ")
		+ CustomMessage;
}

