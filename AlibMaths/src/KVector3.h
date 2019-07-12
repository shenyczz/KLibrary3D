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

// a simple 3D vector class
class ALIBMATHS_EXPORTS_CLASS KVector3
{
public:
	KVector3();
	KVector3(const KVector3 &rhs);
	KVector3(float x, float y, float z);

public:
	float X, Y, Z;

public:
	void Zero();
	float Length();
	void Normalize();

public:
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


private:
	static bool IsEqual(double v1, double v2)
	{
		return (fabs(v2 - v1) < 1.0e-12);
	}

public:
	static const KVector3 ZERO;

public:
	static float DotProduct(const KVector3& a, const KVector3& b);
	static KVector3 CrossProduct(const KVector3& a, const KVector3& b);

	static float Distance(const KVector3& a, const KVector3& b);




	//@EndOf(KVector3)
};
