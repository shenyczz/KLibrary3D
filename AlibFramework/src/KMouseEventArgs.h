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

#include "KEventArgs.h"

class ALIBFRAMEWORK_EXPORTS_CLASS KMouseEventArgs : public KEventArgs
{
public:
	KMouseEventArgs();
	KMouseEventArgs(WPARAM wParam, LPARAM lParam);
	~KMouseEventArgs();
	//KMouseEventArgs();
	//KMouseEventArgs(KMouseButtons button, int clicks, int X, int Y, int delta);
	//KMouseEventArgs(KMouseButtons button, int clicks, int X, int Y, int delta, void* tag);


public:
	WPARAM WParam;
	LPARAM LParam;

	//KMouseButtons MouseButtons;
	//int Clicks;
	//int Delta;
	//LONG X;
	//LONG Y;
	//POINT Location;

	//void* Tag;


	//@EndOf(KMouseEventArgs)
};

/*

//
// 摘要:
//     Initializes a new instance of the System.Windows.Forms.MouseEventArgs class.
//
// 参数:
//   button:
//     One of the System.Windows.Forms.MouseButtons values that indicate which mouse
//     button was pressed.
//
//   clicks:
//     The number of times a mouse button was pressed.
//
//   X:
//     The X-coordinate of a mouse click, in pixels.
//
//   Y:
//     The Y-coordinate of a mouse click, in pixels.
//
//   delta:
//     _Ty signed count of the number of detents the wheel has rotated.
public MouseEventArgs(MouseButtons button, int clicks, int X, int Y, int delta);

//
// 摘要:
//     Gets which mouse button was pressed.
//
// 返回结果:
//     One of the System.Windows.Forms.MouseButtons values.
public MouseButtons Button { get; }
//
// 摘要:
//     Gets the number of times the mouse button was pressed and released.
//
// 返回结果:
//     An System.Int32 that contains the number of times the mouse button was pressed
//     and released.
public int Clicks { get; }
//
// 摘要:
//     Gets the X-coordinate of the mouse during the generating mouse event.
//
// 返回结果:
//     The X-coordinate of the mouse, in pixels.
public int X { get; }
//
// 摘要:
//     Gets the Y-coordinate of the mouse during the generating mouse event.
//
// 返回结果:
//     The Y-coordinate of the mouse, in pixels.
public int Y { get; }
//
// 摘要:
//     Gets a signed count of the number of detents the mouse wheel has rotated, multiplied
//     by the WHEEL_DELTA constant. _Ty detent is one notch of the mouse wheel.
//
// 返回结果:
//     _Ty signed count of the number of detents the mouse wheel has rotated, multiplied
//     by the WHEEL_DELTA constant.
public int Delta { get; }
//
// 摘要:
//     Gets the location of the mouse during the generating mouse event.
//
// 返回结果:
//     _Ty System.Drawing.Point that contains the X- and Y- mouse coordinates, in pixels,
//     relative to the upper-left corner of the form.
public Point Location { get; }
}
*/