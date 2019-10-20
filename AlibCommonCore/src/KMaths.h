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

//#include "KVector3.h"

class ALIBCOMMONCORE_EXPORTS_CLASS KMaths
{
private:
	KMaths() = delete;

public:
	// 符号（-1，0，1）
	static int Sign(double x);

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

	//球坐标转换位笛卡尔坐标
	static Vector3 SphericalToCartesian(double radius, double theta, double phi);



	// 两点间距离
	template<typename _Ty>
	static _Ty Distance(_Ty x1, _Ty y1, _Ty x2, _Ty y2)
	{
		_Ty dx = x2 - x1;
		_Ty dy = y2 - y1;
		return static_cast<_Ty>(sqrt(dx * dx + dy * dy));
	}

	// 求3维向量的模
	template<typename _Ty>
	static _Ty Norm(const _Ty v[3])
	{
		return static_cast<_Ty>(sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
	}

	template<typename _Ty>
	static _Ty Normalize(_Ty v[3])
	{
		_Ty den = Norm(v);

		if (den != 0.0)
		{
			for (int i = 0; i < 3; i++)
			{
				v[i] /= den;
			}
		}

		return den;
	}


	template<typename _Ty>
	static _Ty DotProduct(const TVector3<_Ty>& a, const TVector3<_Ty>& b)
	{
		return static_cast<_Ty>((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
	}

	template<typename _Ty>
	static TVector3<_Ty> CrossProduct(const TVector3<_Ty>& a, const TVector3<_Ty>& b)
	{
		return TVector3<_Ty>
			(
				static_cast<_Ty>(a.y*b.z - a.z*b.y),
				static_cast<_Ty>(a.z*b.x - a.x*b.z),
				static_cast<_Ty>(a.x*b.y - a.y*b.x)
			);
	}


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

