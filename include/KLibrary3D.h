#ifndef __INCLUDED_KLIBRARY_3D_H__
#define __INCLUDED_KLIBRARY_3D_H__

#include "KLibrary3D.inl"
//---------------------------------------------------------
#include "AlibCommon.h"
#include "AlibCommonCore.h"
#include "AlibCommonDataModel.h"

#include "AlibData.h"
#include "AlibFramework.h"
//---------------------------------------------------------
#include "d3dxCommon.h"
//---------------------------------------------------------
#ifdef __DX11APP__
#include "dx11Common.h"
#include "dx11CommonCore.h"
using namespace dx11;
#endif
//---------------------------------------------------------
#ifdef __DX12APP__
#include "dx12Common.h"
#include "dx12CommonCore.h"
using namespace dx12;
#endif
//---------------------------------------------------------





#pragma pack(1)
#pragma pack()

#pragma pack(push,1)
#pragma pack(pop)


#endif	//#ifndef __INCLUDED_KLIBRARY_3D_H__
