#ifndef __INCLUDED_ALIB_API_INL__
#define __INCLUDED_ALIB_API_INL__


#ifdef ALIBAPI_EXPORTS
	#define ALIBAPI_EXPORTS_API __declspec(dllexport)
#else
	#define ALIBAPI_EXPORTS_API __declspec(dllimport)
#endif

#ifdef __cplusplus
	#define __EXTERN_C extern "C"
	#define __EXTERN_C_REGIONBEGIN	__EXTERN_C {
	#define __EXTERN_C_REGIONEND               }
#else
	#define _EXTERN_C_
	#define _EXTERN_C_REGIONBEGIN
	#define _EXTERN_C_REGIONEND
#endif


#define __ALIBAPI	__EXTERN_C ALIBAPI_EXPORTS_API


#ifdef _STDCALL
	#undef _STDCALL
	#define _STDCALL __stdcall
#else
	#define _STDCALL __stdcall
#endif

#ifdef _CDECL
	#undef _CDECL
	#define _CDECL __cdecl
#else
	#define _CDECL __cdecl
#endif






#define ALIBAPI(type)	__ALIBAPI type WINAPI


#endif	//#ifndef __INCLUDED_ALIB_API_INL__

