#ifndef __INCLUDED_ALIB_COMMON_H__
#define __INCLUDED_ALIB_COMMON_H__

//-----------------------------------------------------------------------------
#include "AlibCommon.inl"
//-----------------------------------------------------------------------------
#include "KObject.h"

#include "KSmartPointer.h"
#include "TSlice.h"
#include "TTuple.h"
#include "TVector3.h"


//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_COMMON
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_$_H__
