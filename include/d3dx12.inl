#ifndef __INCLUDED_D3DX12__INL__
#define __INCLUDED_D3DX12__INL__

//
// h
//
#include "d3dx12.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

#include <wrl.h>
using Microsoft::WRL::ComPtr;


// lib
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#else
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#endif



#endif	//#ifndef __INCLUDED_D3DX12__INL__
