#ifndef __INCLUDED_DX12_CORE_H__
#define __INCLUDED_DX12_CORE_H__

//-----------------------------------------------------------------------------
#include "Dx12Core.inl"
//-----------------------------------------------------------------------------
#include "Dx12Sample.h"


//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_DX12_CORE
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_DX12_CORE_H__
