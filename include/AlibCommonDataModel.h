#ifndef __INCLUDED_ALIB_COMMONDATAMODEL_H__
#define __INCLUDED_ALIB_COMMONDATAMODEL_H__

//-----------------------------------------------------------------------------
#include "AlibCommonDataModel.inl"
//-----------------------------------------------------------------------------
#include "KDataObject.h"


#include "KCell.h"
#include "KMarchingCubes.h"
#include "KMarchingCubesTriangleCases.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_ALIB_COMMONDATAMODEL
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_ALIB_COMMONDATAMODEL_H__
