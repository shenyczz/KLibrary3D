#ifndef __INCLUDED_D3DX11__INL__
#define __INCLUDED_D3DX11__INL__

//
// include
//
#include <d3d11.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include "d3dx.inl"
#include "d3dx11.h"


//
// lib
//
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#else
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#endif


#endif	//#ifndef __INCLUDED_D3DX11__INL__
