/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

namespace dxCommon
{
	class D3DCOMMON_EXPORTS_CLASS Utils
	{
	public:
		Utils();
		~Utils();

	public:
		static HRESULT Trace(_In_z_ LPCTSTR lpszFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ LPCTSTR lpszMsg, _In_ bool bPopMsgBox);



	public:
		// 常量
		static const XMFLOAT4X4 Identity4x4;

		//@EndOf(Utils)
	};

}// dxCommon

 // ------------------------------
 // HR宏: Debug模式下的错误提醒与追踪
 // ------------------------------

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)																		\
	{																				\
		HRESULT hr = (x);															\
		if(FAILED(hr))																\
		{																			\
			dxCommon::Utils::Trace(__FILET__, (DWORD)__LINE__, hr, _T(#x), true);	\
		}																			\
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif 
#endif
