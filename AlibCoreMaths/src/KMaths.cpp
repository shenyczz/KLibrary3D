#include "stdafx.h"
#include "KMaths.h"


const double KMaths::E = 2.17828;
const double KMaths::PI = 3.1415926535;
const double KMaths::EPS = 1.0e-12;
const double KMaths::Infinity = DBL_MAX;

//double KMath::EarthRadius = 6371.004;	// 地球平均半径，单位：公里(Km)
//double KMath::EarthRadius_Polar = 6356.755;	// 地球两极半径，单位：公里(Km)
//double KMath::EarthRadius_Equator = 6373.140;	// 地球赤道半径，单位：公里(Km)

int KMaths::Sign(double x)
{
	return x < 0 ? -1
		: x > 0 ? 1
		: 0;
}

bool KMaths::IsEqual(double v1, double v2)
{
	return fabs(v2 - v1) < EPS;
}


double KMaths::Rand()
{
	return (double)rand() / RAND_MAX;
}

//static 
int KMaths::Rand(int a, int b)
{
	if (a > b) std::swap(a, b);
	return a + rand() % ((b - a) + 1);
}

//static
float KMaths::Rand(float a, float b)
{
	if (a > b) std::swap(a, b);
	return a + (float)Rand()*(b - a);
}

//static
double KMaths::Rand(double a, double b)
{
	if (a > b) std::swap(a, b);
	return a + Rand()*(b - a);
}







// 计算两点间的距离(通用)
double KMaths::Distance(double x1, double y1, double x2, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}


//static 球坐标转换位笛卡尔坐标
// theta	- 与x轴夹角?
// phi		- 与y轴夹角
KVector3 KMaths::SphericalToCartesian(double radius, double theta, double phi)
{
	return KVector3(
		(float)(radius*sin(phi)*cos(theta)),	// x
		(float)(radius*cos(phi)),				// y
		(float)(radius*sin(phi)*sin(theta)));	// z
}

