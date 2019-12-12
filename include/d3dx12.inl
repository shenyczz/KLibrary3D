#ifndef __INCLUDED_D3DX12__INL__
#define __INCLUDED_D3DX12__INL__

//
// h
//
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include "d3dx.inl"
#include "d3dx12.h"


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
