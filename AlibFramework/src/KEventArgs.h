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

class ALIBFRAMEWORK_EXPORTS_CLASS KEventArgs : public KObject
{
public:
	KEventArgs();
	~KEventArgs();

};



enum class ALIBFRAMEWORK_EXPORTS_CLASS KMouseButtons
{
	// 0x100000
	// No mouse button was pressed.
	None = 0,

	// 0x100000
	// The left mouse button was pressed.
	Left = 1048576,

	// 0x200000
	// The right mouse button was pressed.
	Right = 2097152,

	// 0x400000
	// The middle mouse button was pressed.
	Middle = 4194304,

	// 0x800000
	// The first XButton was pressed.
	XButton1 = 8388608,

	// 0x1000000
	// The second XButton was pressed.
	XButton2 = 16777216
};

