#include "stdafx.h"
#include "KCoordinate.h"


KCoordinate g_Coordinate;

KCoordinate::KCoordinate()
	: KCoordinate(0, 0, 0) {}

KCoordinate::KCoordinate(const KCoordinate& rhs)
	: KCoordinate(rhs.X, rhs.Y, rhs.Z) {}


KCoordinate::KCoordinate(double x, double y, double z)
	: X(x), Y(y), Z(z) {}


KCoordinate::~KCoordinate()
{
}













KCoordinate& KCoordinate::Clone()
{
	g_Coordinate = KCoordinate(*this);
	return g_Coordinate;
}
