/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
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

// ŷ����
// ����λ�ֽ�Ϊ���������ഹֱ�����ת��
// Լ��Ϊ  "heading-pitch-bank" y-x-z
//         "roll-pitch-yaw"		z-x-y
// heading�����ƴ�ֱ�����ת
// pitch����ˮƽ�������ǣ�
// bank������z�����ת��
// roll��	����
// pitch��	ǰ��
// yaw��	ƫ��
class ALIBMATHS_EXPORTS_CLASS KEulerAngles
{
public:
	KEulerAngles();
	~KEulerAngles();

	float heading;	// ����������ϵy����ת����һ����ת��
	float pitch;	// ����������ϵx����ת���ڶ�����ת��
	float bank;		// ����������ϵz����ת����������ת��


};

