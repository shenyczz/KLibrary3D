#ifndef __INCLUDED_D3DX11__INL__
#define __INCLUDED_D3DX11__INL__

//
// h
//
#include <d3d11.h>
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

#include "d3dx11Effect.h"
//#include <dxerr.h>		// 没有对应


//
// lib
//
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Effects11d.lib")
#else
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Effects11.lib")
#endif

/*

Effects11d.lib		
dxerr.lib
dxguid.lib			// y

*/


#endif	//#ifndef __INCLUDED_D3DX11__INL__
