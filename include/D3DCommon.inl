#ifndef __INCLUDED_D3D_COMMON_INL__
#define __INCLUDED_D3D_COMMON_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"


#if defined (D3DCOMMON_EXPORTS)
#define D3DCOMMON_EXPORTS_CLASS __declspec(dllexport)
#else
#define D3DCOMMON_EXPORTS_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_D3D_COMMON	"D3DCommon"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_D3D_COMMON_INL__
