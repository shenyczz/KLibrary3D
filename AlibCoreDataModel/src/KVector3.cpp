#include "stdafx.h"
#include "KVector3.h"


//static
const KVector3 KVector3::ZERO = KVector3(0, 0, 0);


KVector3::KVector3()
	:X(0), Y(0), Z(0) {}

KVector3::KVector3(const KVector3 &rhs)
	: X(rhs.X), Y(rhs.Y), Z(rhs.Z) {}

KVector3::KVector3(float _x, float _y, float _z)
	: X(_x), Y(_y), Z(_z) {}

KVector3::KVector3(const float *pArray)
	: X(pArray[0]), Y(pArray[1]), Z(pArray[2]) {}




float KVector3::Length() const
{
	return sqrtf(X*X + Y * Y + Z * Z);
}

KVector3& KVector3::Normalize()
{
	float len = Length();
	float fact = (len > 0)
		? (1.0f / len)
		: (1.0f);
	X *= fact;
	Y *= fact;
	Z *= fact;
	return *this;
}




KVector3 KVector3::operator -() const
{
	return KVector3(-X, -Y, -Z);
}

KVector3& KVector3::operator =(const KVector3& other)
{
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	return *this;
}

KVector3 KVector3::operator +(const KVector3& other) const
{
	return KVector3(X + other.X, Y + other.Y, Z + other.Z);
}
KVector3 KVector3::operator -(const KVector3& other) const
{
	return KVector3(X - other.X, Y - other.Y, Z - other.Z);
}

KVector3 KVector3::operator *(const float factor) const
{
	return KVector3(X*factor, Y*factor, Z*factor);
}
KVector3 KVector3::operator /(const float factor) const
{
	float fact = IsEqual(factor, 0) ? 0 : 1.0f / factor;
	return KVector3(X*fact, Y*fact, Z*fact);
}

KVector3& KVector3::operator +=(const KVector3& other)
{
	X += other.X;
	Y += other.Y;
	Z += other.Z;
	return *this;
}
KVector3& KVector3::operator -=(const KVector3& other)
{
	X -= other.X;
	Y -= other.Y;
	Z -= other.Z;
	return *this;
}
KVector3& KVector3::operator *=(const float factor)
{
	X *= factor;
	Y *= factor;
	Z *= factor;
	return *this;
}
KVector3& KVector3::operator /=(const float factor)
{
	float fact = IsEqual(factor, 0) ? 0 : 1.0f / factor;
	X *= fact;
	Y *= fact;
	Z *= fact;
	return *this;
}

bool KVector3::operator ==(const KVector3& other) const
{
	return true
		&& IsEqual(X, other.X)
		&& IsEqual(Y, other.Y)
		&& IsEqual(Z, other.Z)
		;
}
bool KVector3::operator !=(const KVector3& other) const
{
	return false
		|| !IsEqual(X, other.X)
		|| !IsEqual(Y, other.Y)
		|| !IsEqual(Z, other.Z)
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
	float dx = a.X - b.X;
	float dy = a.Y - b.Y;
	float dz = a.Z - b.Z;
	return sqrtf(dx*dx + dy * dy + dz * dz);
}

// static
float KVector3::DotProduct(const KVector3& a, const KVector3& b)
{
	return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
}

// static
KVector3 KVector3::CrossProduct(const KVector3& a, const KVector3& b)
{
	return KVector3(
		a.Y*b.Z - a.Z*b.Y,
		a.Z*b.X - a.X*b.Z,
		a.X*b.Y - a.Y*b.X);
}

// friend
KVector3 operator*(const float factor, const KVector3& other)
{
	return KVector3(factor*other.X, factor* other.Y, factor* other.Z);
}


