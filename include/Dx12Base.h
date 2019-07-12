#ifndef __INCLUDED_DX12_BASE_H__
#define __INCLUDED_DX12_BASE_H__

//-----------------------------------------------------------------------------
#include "Dx12Base.inl"
//-----------------------------------------------------------------------------
#include "DXColors.h"
#include "DXUtil.h"

#include "MathHelper.h"
#include "UploadBuffer.h"

#include "MeshGeometry.h"

#include "DDSTextureLoader.h" 
//-----------------------------------------------------------------------------
#ifdef DLL_NAME
	#undef DLL_NAME
	#define DLL_NAME		DLL_NAME_DX12_BASE
#endif
//-----------------------------------------------------------------------------
#define LIB_COMMENT			SFX_LIB
#define DISPLAY_MESSAGE		TOSTRING(LINK_MESSAGE)SFX_DLL

#pragma comment(lib, LIB_COMMENT) 
#pragma message(DISPLAY_MESSAGE)
//-----------------------------------------------------------------------------

#endif // #ifndef __INCLUDED_DX12_BASE_H__
