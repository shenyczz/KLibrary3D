#ifndef __INCLUDED_DX12LIB_COMMON_INL__
#define __INCLUDED_DX12LIB_COMMON_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"
#include "d3dx12.inl"


#if defined (DX12LIBCOMMON_EXPORTS)
#define DX12LIBCOMMON_EXPORTS_CLASS __declspec(dllexport)
#else
#define DX12LIBCOMMON_EXPORTS_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_DX12LIB_COMMON	"DX12LibCommon"
//---------------------------------------------------------

namespace dx12 {}


#endif	//#ifndef __INCLUDED_DX12LIB_COMMON_INL__
