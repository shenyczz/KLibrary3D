#ifndef __INCLUDED_ALIB_FRAMEWORK_INL__
#define __INCLUDED_ALIB_FRAMEWORK_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"


#if defined (ALIBFRAMEWORK_EXPORTS)
#define ALIBFRAMEWORK_EXPORTS_CLASS __declspec(dllexport)
#else
#define ALIBFRAMEWORK_EXPORTS_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_ALIB_FRAMEWORK	"AlibFramework"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_ALIB_FRAMEWORK_INL__
