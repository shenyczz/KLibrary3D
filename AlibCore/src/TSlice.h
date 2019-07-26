/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: …Í”¿≥Ω.÷£÷› (shenyczz@163.com)
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

// CT «–∆¨
template<typename _Ty>
struct TSlice
{
	TSlice() = delete;
	TSlice(int width, int height)
		: Width(width), Height(height) {}

	_Ty GetValue(int x, int y)
	{
		return ElementArray.at(y * Width + x);
	}

	int Width;
	int Height;

	std::vector<_Ty> ElementArray;

};

