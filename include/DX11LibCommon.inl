#ifndef __INCLUDED_DX11LIB_COMMON_INL__
#define __INCLUDED_DX11LIB_COMMON_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"
#include "d3dx11.inl"


#if defined (DX11LIBCOMMON_EXPORTS)
#define DX11LIBCOMMON_EXPORTS_CLASS __declspec(dllexport)
#else
#define DX11LIBCOMMON_EXPORTS_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_DX11LIB_COMMON	"DX11LibCommon"
//---------------------------------------------------------

namespace dx11 {}


#endif	//#ifndef __INCLUDED_DX11LIB_COMMON_INL__
