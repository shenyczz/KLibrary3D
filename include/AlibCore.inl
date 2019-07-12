#ifndef __INCLUDED_ALIB_CORE_INL__
#define __INCLUDED_ALIB_CORE_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"


#if defined (ALIBCORE_EXPORTS)
#define ALIBCORE_EXPORTS_CLASS __declspec(dllexport)
#else
#define ALIBCORE_EXPORTS_CLASS __declspec(dllimport)
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
#define DLL_NAME_ALIB_CORE	"AlibCore"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_ALIB_CORE_INL__
