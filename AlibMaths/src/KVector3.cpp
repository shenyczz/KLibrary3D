#include "stdafx.h"
#include "KVector3.h"



const KVector3 KVector3::ZERO = KVector3(0, 0, 0);



KVector3::KVector3()
	:X(0), Y(0), Z(0) {}

KVector3::KVector3(const KVector3 &rhs)
	: X(rhs.Z), Y(rhs.Z), Z(rhs.Z) {}

KVector3::KVector3(float x, float y, float z)
	: X(x), Y(y), Z(z) {}



void KVector3::Zero()
{
	X = Y = Z = 0.0f;
}

float KVector3::Length()
{
	return sqrtf(X*X + Y * Y + Z * Z);
}

void KVector3::Normalize()
{
	float len = Length();
	float fact = (len > 0) ? (1.0f / len) : (1.0f);
	X *= fact;
	Y *= fact;
	Z *= fact;
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
	float fact = factor;
	return KVector3(X*fact, Y*fact, Z*fact);
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
	float fact = factor;
	X *= fact;
	Y *= fact;
	Z *= fact;
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




// static
float KVector3::DotProduct(const KVector3& a, const KVector3& b)
{
	return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
}
KVector3 KVector3::CrossProduct(const KVector3& a, const KVector3& b)
{
	return KVector3(
		a.Y*b.Z - a.Z*b.Y,
		a.Z*b.X - a.X*b.Z,
		a.X*b.Y - a.Y*b.X);
}
// æ‡¿Î
float KVector3::Distance(const KVector3& a, const KVector3& b)
{
	float dx = a.X - b.X;
	float dy = a.Y - b.Y;
	float dz = a.Z - b.Z;
	return sqrtf(dx*dx + dy * dy + dz * dz);
}


