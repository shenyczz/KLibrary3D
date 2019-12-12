#ifndef __INCLUDED_D3DX_COMMON_INL__
#define __INCLUDED_D3DX_COMMON_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"
#include "d3dx.inl"

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

#include <wrl.h>
using Microsoft::WRL::ComPtr;


#if defined (D3DCOMMON_EXPORTS)
#define D3DCOMMON_EXPORTS_CLASS __declspec(dllexport)
#else
#define D3DCOMMON_EXPORTS_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_D3DX_COMMON	"d3dCommon"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_D3DX_COMMON_INL__
