#ifndef __INCLUDED_DX11LIB_COMMON_H__
#define __INCLUDED_DX11LIB_COMMON_H__

//-----------------------------------------------------------------------------
#include "dx11Common.inl"
//-----------------------------------------------------------------------------
#include "dx11Utils.h"
#include "Vertex11.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_DX11LIB_COMMON
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_DX11LIB_COMMON_H__
