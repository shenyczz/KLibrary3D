#ifndef __INCLUDED_ALIB_CORE_GEOS_H__
#define __INCLUDED_ALIB_CORE_GEOS_H__

//-----------------------------------------------------------------------------
#include "AlibCoreGeos.inl"
//-----------------------------------------------------------------------------
#include "KCoordinate.h"
#include "KGeometry.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_CORE_GEOS
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_GEOS_H__
