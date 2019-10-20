#ifndef __INCLUDED_DX11LIB_COMMONCORE_INL__
#define __INCLUDED_DX11LIB_COMMONCORE_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"


#if defined (DX11LIBCOMMONCORE_EXPORTS)
#define DX11LIBCOMMONCORE_CLASS __declspec(dllexport)
#else
#define DX11LIBCOMMONCORE_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_DX11LIB_COMMONCORE	"dx11CommonCore"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_DX11LIB_COMMONCORE_INL__
