#ifndef __INCLUDED_ALIB_FRAMEWORK_H__
#define __INCLUDED_ALIB_FRAMEWORK_H__

//-----------------------------------------------------------------------------
#include "AlibFramework.inl"
//-----------------------------------------------------------------------------
#include "KApplication.h"
#include "KDocument.h"
#include "KWindow.h"
#include "KSample.h"

#include "KEventArgs.h"
#include "KMouseEventArgs.h"

#include "KMouseListener.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_FRAMEWORK
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_FRAMEWORK_H__
