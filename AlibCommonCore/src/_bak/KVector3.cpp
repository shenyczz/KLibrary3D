#include "stdafx.h"
#include "KVector3.h"


//static
const KVector3 KVector3::ZERO = KVector3(0, 0, 0);


KVector3::KVector3()
	: KVector3(0, 0, 0) {}

KVector3::KVector3(const KVector3 &rhs)
	: KVector3(rhs.x, rhs.y, rhs.z)
{
	V = rhs.V;
}

KVector3::KVector3(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z) {}

KVector3::KVector3(const float *pArray)
	: KVector3(pArray[0], pArray[1], pArray[2]) {}



float KVector3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

KVector3& KVector3::Normalize()
{
	float len = Length();
	float fact = (len > 0)
		? (1.0f / len)
		: (1.0f);
	x *= fact;
	y *= fact;
	z *= fact;
	return *this;
}




KVector3 KVector3::operator -() const
{
	return KVector3(-x, -y, -z);
}

KVector3& KVector3::operator =(const KVector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

KVector3 KVector3::operator +(const KVector3& other) const
{
	return KVector3(x + other.x, y + other.y, z + other.z);
}
KVector3 KVector3::operator -(const KVector3& other) const
{
	return KVector3(x - other.x, y - other.y, z - other.z);
}

KVector3 KVector3::operator *(const float factor) const
{
	return KVector3(x*factor, y*factor, z*factor);
}
KVector3 KVector3::operator /(const float factor) const
{
	float fact = IsEqual(factor, 0) ? 0 : 1.0f / factor;
	return KVector3(x*fact, y*fact, z*fact);
}

KVector3& KVector3::operator +=(const KVector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}
KVector3& KVector3::operator -=(const KVector3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
KVector3& KVector3::operator *=(const float factor)
{
	x *= factor;
	y *= factor;
	z *= factor;
	return *this;
}
KVector3& KVector3::operator /=(const float factor)
{
	float fact = IsEqual(factor, 0) ? 0 : 1.0f / factor;
	x *= fact;
	y *= fact;
	z *= fact;
	return *this;
}

bool KVector3::operator ==(const KVector3& other) const
{
	return true
		&& IsEqual(x, other.x)
		&& IsEqual(y, other.y)
		&& IsEqual(z, other.z)
		;
}
bool KVector3::operator !=(const KVector3& other) const
{
	return false
		|| !IsEqual(x, other.x)
		|| !IsEqual(y, other.y)
		|| !IsEqual(z, other.z)
		;
}



//static
bool KVector3::IsEqual(const double v1, const double v2)
{
	return (fabs(v2 - v1) < 1.0e-12);
}


// static ¾àÀë
float KVector3::Distance(const KVector3& a, const KVector3& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrtf(dx*dx + dy * dy + dz * dz);
}

// static
float KVector3::DotProduct(const KVector3& a, const KVector3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

// static
KVector3 KVector3::CrossProduct(const KVector3& a, const KVector3& b)
{
	return KVector3(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x);
}

// friend
KVector3 operator*(const float factor, const KVector3& other)
{
	return KVector3(factor*other.x, factor* other.y, factor* other.z);
}


