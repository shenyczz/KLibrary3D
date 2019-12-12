#ifndef __INCLUDED_D3D_COMMON_H__
#define __INCLUDED_D3D_COMMON_H__

//-----------------------------------------------------------------------------
#include "d3dxCommon.inl"
//-----------------------------------------------------------------------------
#include "d3dSample.h"

#include "Utils.h"
#include "VertexBase.h"
#include "MeshData.h"
#include "MeshFactory.h"

//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_D3DX_COMMON
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_D3D_COMMON_H__
