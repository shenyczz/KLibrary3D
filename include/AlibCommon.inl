#ifndef __INCLUDED_ALIB_COMMOM_INL__
#define __INCLUDED_ALIB_COMMOM_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"


#if defined (ALIBCOMMON_EXPORTS)
#define ALIBCOMMOM_EXPORTS_CLASS __declspec(dllexport)
#else
#define ALIBCOMMOM_EXPORTS_CLASS __declspec(dllimport)
#endif

template<class _Ty>
inline _tstring _ttostring(_Ty _Val)
{
#ifdef _UNICODE
	return std::to_wstring(_Val);
#else
	return std::to_string(_Val);
#endif // _UNICODE
}

//---------------------------------------------------------
#define DLL_NAME_ALIB_COMMON	"AlibCommon"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_ALIB_COMMOM_INL__

