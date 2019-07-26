#ifndef __INCLUDED_DX12_BASE_INL__
#define __INCLUDED_DX12_BASE_INL__

//
// KLibrary3D
//
#include "KLibrary3D.inl"
#include "d3dx12.inl"

namespace DX12 {}


#if defined (DX12BASE_EXPORTS)
#define DX12BASE_EXPORTS_CLASS __declspec(dllexport)
#else
#define DX12BASE_EXPORTS_CLASS __declspec(dllimport)
#endif

//---------------------------------------------------------
#define DLL_NAME_DX12_BASE	"Dx12Base"
//---------------------------------------------------------

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;
using namespace DX12;

#endif	//#ifndef __INCLUDED_DX12_BASE_INL__
