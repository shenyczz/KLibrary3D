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

// a simple 3D vector class
class ALIBCOMMONCORE_EXPORTS_CLASS KVector3
{
public:
	KVector3();
	KVector3(const KVector3 &rhs);
	KVector3(float _x, float _y, float _z);
	explicit KVector3(const float *pArray);

public:
	float x, y, z;
	float V;

	KVector3 operator -() const;
	KVector3& operator =(const KVector3& other);

	KVector3 operator +(const KVector3& other) const;
	KVector3 operator -(const KVector3& other) const;
	KVector3 operator *(const float factor) const;
	KVector3 operator /(const float factor) const;

	KVector3& operator +=(const KVector3& other);
	KVector3& operator -=(const KVector3& other);

	KVector3& operator *=(const float factor);
	KVector3& operator /=(const float factor);

	bool operator ==(const KVector3& other) const;
	bool operator !=(const KVector3& other) const;


public:
	float Length() const;
	KVector3& Normalize();

public:
	static const KVector3 ZERO;

	static float Distance(const KVector3& a, const KVector3& b);
	static float DotProduct(const KVector3& a, const KVector3& b);
	static KVector3 CrossProduct(const KVector3& a, const KVector3& b);

private:
	static bool IsEqual(const double v1, const double v2);


public:
	// 友元函数(不是该类的函数)
	friend ALIBCOMMONCORE_EXPORTS_CLASS KVector3 operator*(const float factor, const KVector3& other);

	//@EndOf(KVector3)
};
