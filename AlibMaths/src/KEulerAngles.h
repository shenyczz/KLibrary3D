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

// 欧拉角
// 将方位分解为绕三个互相垂直轴的旋转，
// 约定为  "heading-pitch-bank" y-x-z
//         "roll-pitch-yaw"		z-x-y
// heading度量绕垂直轴的旋转
// pitch度量水平方向的倾角，
// bank度量绕z轴的旋转量
// roll：	翻滚
// pitch：	前倾
// yaw：	偏航
class ALIBMATHS_EXPORTS_CLASS KEulerAngles
{
public:
	KEulerAngles();
	~KEulerAngles();

	float heading;	// 绕物体坐标系y轴旋转（第一个旋转）
	float pitch;	// 绕物体坐标系x轴旋转（第二个旋转）
	float bank;		// 绕物体坐标系z轴旋转（第三个旋转）


};

