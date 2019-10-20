#ifndef __INCLUDED_DX12LIB_COMMONCORE_H__
#define __INCLUDED_DX12LIB_COMMONCORE_H__

//-----------------------------------------------------------------------------
#include "dx12CommonCore.inl"
//-----------------------------------------------------------------------------
#include "KRootParameter.h"

#include "Dx12Book.h"
#include "Dx12Study.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_DX12LIB_COMMONCORE
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_DX12LIB_COMMONCORE_H__
