#ifndef __INCLUDED_KLIBRARY_3D_INL__
#define __INCLUDED_KLIBRARY_3D_INL__

#pragma pack(push,1)
#pragma pack(pop)

#pragma pack(push,1)
#pragma pack(pop)

#pragma warning (disable : 4005)		// warning C4005: “DXGI_STATUS_OCCLUDED”: 宏重定义
#pragma warning (disable : 4065)		// switch 语句包含“default”但是未包含“case”标签
#pragma warning (disable : 4251)
//#pragma warning (disable : 4275)
//#pragma warning (disable : 4482)
//#pragma warning (disable : 4996)		// unsafe function
//#pragma warning( disable : 4290)

//
// Windows 
//
#include <windows.h>
#include <windowsX.h>

//
// TCHAT
//
#include <tchar.h>

//
// stdlib
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//
// Gdiplus
//
//#include <comdef.h>						// Native C++ compiler COM support
//#include <Gdiplus.h>
//#pragma comment (lib, "Gdiplus.lib" )
//using namespace Gdiplus;

//
// STL
//
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

//using namespace std;

// COM
#include <comdef.h>

//---------------------------------------------------------

#define _astring std::string
#define _wstring std::wstring


#ifdef UNICODE
	#define _tstring _wstring
#else
	#define _tstring _astring
#endif

//---------------------------------------------------------
#define _delete(p) { if(p) {delete p; p = 0;} }
#define _deletea(p) { if(p) {delete []p; p = 0;} }
//---------------------------------------------------------
#define TOSTRING(s) s

#define LINK_MESSAGE	"Automatically linking with "

#define DLL_NAME		""			// 动态库名称
#define dll_sep			""			// 分隔符,比如"_"
#define dll_ver			""			// 动态库版本，建议和MFC库版本一致
#define dll_ext			".dll"		// 动态库扩展名
#define lib_ext			".lib"		// 连接库扩展名

// 运行环境标识
#ifdef _DEBUG
	#if defined(_UNICODE)
		#define dll_env ""			// Unicode Debug "ud"
	#else
		#define dll_env ""			// Debug "d"
	#endif
#else
	#if defined(_UNICODE)
		#define dll_env ""			// Unicode Release "ur"
	#else
		#define dll_env ""			// Release "r"
	#endif
#endif

#ifdef WIN32
	#define Platform_Architecture ""	// 平台结构：WIN32=>32，WIN64=>64
#endif

// 例子
//#define SFX_DLL	Dx11Base[dll_sep][dll_ver][dll_sep][dll_env][Platform_Architecture].dll
//					Dx11Base_120_ud_32.dll

#define SFX_DLL	\
	TOSTRING(DLL_NAME)\
	TOSTRING(dll_sep)\
	TOSTRING(dll_ver)\
	TOSTRING(dll_sep)\
	TOSTRING(dll_env)\
	TOSTRING(Platform_Architecture)\
	TOSTRING(dll_ext)

#define SFX_LIB \
	TOSTRING(DLL_NAME)\
	TOSTRING(dll_sep)\
	TOSTRING(dll_ver)\
	TOSTRING(dll_sep)\
	TOSTRING(dll_env)\
	TOSTRING(Platform_Architecture)\
	TOSTRING(lib_ext)

//---------------------------------------------------------
#include "Api.inl"
#include "std.inl"
#include "tchar.inl"
//---------------------------------------------------------


#endif	//#ifndef __INCLUDED_KLIBRARY_3D_INL__
