#ifndef __INCLUDED_ALIB_$_H__
#define __INCLUDED_ALIB_$_H__

//-----------------------------------------------------------------------------
#include "Alib$.inl"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_$
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_$_H__
