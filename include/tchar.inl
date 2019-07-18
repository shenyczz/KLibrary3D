#ifndef __INCLUDED_SFX_TCHAR__INL__
#define __INCLUDED_SFX_TCHAR__INL__


#if defined(_UNICODE)
#define __TFILE__		__FILEW__
#define __TFUNCTION__	__FUNCTIONW__
#else
#define __TFILE__		__FILE__
#define __TFUNCTION__	__FUNCTION__
#endif


#endif	//#ifndef __INCLUDED_SFX_TCHAR__INL__
