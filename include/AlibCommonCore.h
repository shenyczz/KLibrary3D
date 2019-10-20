#ifndef __INCLUDED_ALIB_COMMONCORE_H__
#define __INCLUDED_ALIB_COMMONCORE_H__

//-----------------------------------------------------------------------------
#include "AlibCommonCore.inl"
//-----------------------------------------------------------------------------
#include "KMaths.h"

#include "KClock.h"
#include "KException.h"
#include "KString.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_COMMONCORE
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_COMMONCORE_H__
