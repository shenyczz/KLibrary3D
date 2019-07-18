#ifndef __INCLUDED_ALIB_CORE_DATAMODEL_H__
#define __INCLUDED_ALIB_CORE_DATAMODEL_H__

//-----------------------------------------------------------------------------
#include "AlibCoreDataModel.inl"
//-----------------------------------------------------------------------------
#include "TTuple.h"		// template

#include "KCell.h"
#include "KVector3.h"

#include "KMarchingCubesTriangleCases.h"
#include "KMarchingCubes.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_CORE_DATAMODEL
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_CORE_DATAMODEL_H__
