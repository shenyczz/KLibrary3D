#ifndef __INCLUDED_DX12LIB_COMMON_H__
#define __INCLUDED_DX12LIB_COMMON_H__

//-----------------------------------------------------------------------------
#include "DX12LibCommon.inl"
//-----------------------------------------------------------------------------
#include "TUploadBuffer.h"

#include "DXColors.h"
#include "DXUtil.h"
#include "CommonStructs.h"


#include "Vertex.h"
#include "MeshData.h"
#include "MeshFactory.h"
#include "MeshGeometry.h"


#include "DDSTextureLoader.h" 
//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_DX12LIB_COMMON
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_DX12LIB_COMMON_H__
