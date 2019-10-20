#ifndef __INCLUDED_DX11LIB_COMMONCORE_H__
#define __INCLUDED_DX11LIB_COMMONCORE_H__

//-----------------------------------------------------------------------------
#include "dx11CommonCore.inl"
//-----------------------------------------------------------------------------
#include "Dx11Book.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_DX11LIB_COMMONCORE
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_DX11LIB_COMMONCORE_H__
