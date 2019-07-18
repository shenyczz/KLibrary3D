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

class ALIBCOREMATHS_EXPORTS_CLASS KMath
{
private:
	KMath() = delete;

public:
	// ���ţ�-1��0��1��
	static int Sign(double X);

	// ����ж�
	static bool IsEqual(double v1, double v2);

	// ����[0��1]֮��� double �����
	static double Rand();

	// ����[a,b)֮������������
	static int Rand(int a, int b);

	// ����[a,b)֮��� float �����
	static float Rand(float a, float b);

	// ����[a, b)֮��� double �����
	static double Rand(double a, double b);

	// ��������
	static double Distance(double x1, double y1, double x2, double y2);


	template<typename T>
	static void Swap(const T& a, const T& b)
	{
		std::swap(a, b);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	// ��ֵ�Ǹ���Lerp(a,b,t)�еĵ���������t������a��b֮���ֵ
	// t������0��1֮��
	template<typename T>
	static T Lerp(const T& a, const T& b, const double t)
	{
		return (T)(a + (b - a)*t);
	}

	// Clamp - ǯλ
	// �� X ǯλ��[low,high]����
	template<typename T>
	static T Clamp(const T& X, const T& low, const T& high)
	{
		return X < low ? low
			: X > high ? high
			: X;
	}


public:
	static const double E;
	static const double PI;
	static const double EPS;		// 
	static const double Infinity;	// �����
};

