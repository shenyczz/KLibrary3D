#ifndef __INCLUDED_D3D_COMMON_H__
#define __INCLUDED_D3D_COMMON_H__

//-----------------------------------------------------------------------------
#include "d3dCommon.inl"
//-----------------------------------------------------------------------------
#include "Utils.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_D3D_COMMON
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_D3D_COMMON_H__
