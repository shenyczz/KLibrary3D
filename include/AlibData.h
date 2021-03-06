#ifndef __INCLUDED_ALIB_DATA_H__
#define __INCLUDED_ALIB_DATA_H__

//-----------------------------------------------------------------------------
#include "AlibData.inl"
//-----------------------------------------------------------------------------
#include "KDataInfo.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_DATA
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_DATA_H__
