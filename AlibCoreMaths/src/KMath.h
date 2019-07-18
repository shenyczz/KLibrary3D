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

class ALIBCOREMATHS_EXPORTS_CLASS KMath
{
private:
	KMath() = delete;

public:
	// 符号（-1，0，1）
	static int Sign(double X);

	// 相等判断
	static bool IsEqual(double v1, double v2);

	// 返回[0，1]之间的 double 随机数
	static double Rand();

	// 返回[a,b)之间的整型随机数
	static int Rand(int a, int b);

	// 返回[a,b)之间的 float 随机数
	static float Rand(float a, float b);

	// 返回[a, b)之间的 double 随机数
	static double Rand(double a, double b);

	// 两点间距离
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

	// 插值是根据Lerp(a,b,t)中的第三个参数t来返回a和b之间的值
	// t限制在0～1之间
	template<typename T>
	static T Lerp(const T& a, const T& b, const double t)
	{
		return (T)(a + (b - a)*t);
	}

	// Clamp - 钳位
	// 把 X 钳位在[low,high]区间
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
	static const double Infinity;	// 无穷大
};

