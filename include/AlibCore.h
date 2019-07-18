#ifndef __INCLUDED_ALIB_CORE_H__
#define __INCLUDED_ALIB_CORE_H__

//-----------------------------------------------------------------------------
#include "AlibCore.inl"
//-----------------------------------------------------------------------------
#include "KSmartPointer.h"

#include "KObject.h"
#include "KString.h"
#include "KException.h"

#include "KClock.h"

#include "KUtil.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_CORE
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_CORE_H__
