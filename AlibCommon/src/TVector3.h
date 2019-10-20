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


template<typename _Ty>
struct TVector3
{
	_Ty x, y, z;

#pragma region Constructors - 构造函数

	TVector3() : TVector3(0, 0, 0) {}
	TVector3(const TVector3 &rhs) : TVector3(rhs.x, rhs.y, rhs.z) {}
	TVector3(_Ty _x, _Ty _y, _Ty _z) : x(_x), y(_y), z(_z) {}

#pragma endregion


#pragma region Operator overried - 操作符重载

	// 取负 -
	TVector3 operator -() const
	{
		return TVector3(-x, -y, -z);
	}
	TVector3& operator =(const TVector3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	TVector3 operator +(const TVector3& other) const
	{
		return TVector3(x + other.x, y + other.y, z + other.z);
	}
	TVector3 operator -(const TVector3& other) const
	{
		return TVector3(x - other.x, y - other.y, z - other.z);
	}
	TVector3 operator *(const double factor) const
	{
		double scale = factor;
		return TVector3(_Ty(x*scale), _Ty(y*scale), _Ty(z*scale));
	}
	TVector3 operator /(const double factor) const
	{
		double scale = IsEqual(scale, 0) ? 0 : 1.0f / factor;
		return TVector3(_Ty(x*scale), _Ty(y*scale), _Ty(z*scale));
	}

	TVector3& operator +=(const TVector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	TVector3& operator -=(const TVector3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	TVector3& operator *=(const double factor)
	{
		x *= factor;
		y *= factor;
		z *= factor;
		return *this;
	}
	TVector3& operator /=(const double factor)
	{
		float fact = IsEqual(factor, 0) ? 0 : 1.0f / factor;
		x *= fact;
		y *= fact;
		z *= fact;
		return *this;
	}

	bool operator ==(const TVector3& other) const
	{
		return true
			&& IsEqual(x, other.x)
			&& IsEqual(y, other.y)
			&& IsEqual(z, other.z)
			;
	}
	bool operator !=(const TVector3& other) const
	{
		return false
			|| !IsEqual(x, other.x)
			|| !IsEqual(y, other.y)
			|| !IsEqual(z, other.z)
			;
	}

	// 友元函数(不是该类的函数), vec1 = 2 * vec;
	friend TVector3 operator*(const float factor, const TVector3& other)
	{
		return TVector3(factor*other.x, factor* other.y, factor* other.z);
	}

#pragma endregion

	// 向量长度
	_Ty Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	// 求模
	_Ty Norm() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	// 标准化
	TVector3& Normalize()
	{
		_Ty len = Length();
		_Ty fact = (len > 0) ? (1.0f / len) : (1.0f);
		x *= fact;
		y *= fact;
		z *= fact;
		return *this;
	}



private:
	bool IsEqual(const double v1, const double v2)
	{
		return (fabs(v2 - v1) < 1.0e-12);
	}

	//@EndOf(TVector3)
};

typedef TVector3<double>	Vector3;
typedef TVector3<float>		Vector3F;




#ifdef _DEBUG

ALIBAPI(void) alib_test_TVector3();

#endif // _DEBUG

