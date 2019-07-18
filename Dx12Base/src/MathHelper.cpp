#include "stdafx.h"
#include "MathHelper.h"


namespace DX12
{

	// Returns the polar angle of the point (X,Y) in [0, 2*PI).
	// ·½Î»½Ç
	float MathHelper::AngleFromXY(float X, float Y)
	{
		float theta = 0.0f;

		// Quadrant I or IV
		if (X >= 0.0f)
		{
			// If X = 0, then atanf(Y/X) = +pi/2 if Y > 0
			//                atanf(Y/X) = -pi/2 if Y < 0
			theta = atanf(Y / X); // in [-pi/2, +pi/2]

			if (theta < 0.0f)
				theta += 2.0f*(float)KMath::PI; // in [0, 2*pi).
		}

		// Quadrant II or III
		else
			theta = atanf(Y / X) + (float)KMath::PI; // in [0, 2*pi).

		return theta;
	}


	XMVECTOR MathHelper::RandUnitVec3()
	{
		XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR Zero = XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			XMVECTOR v = XMVectorSet(KMath::Rand(-1.0f, 1.0f),
				KMath::Rand(-1.0f, 1.0f),
				KMath::Rand(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (XMVector3Greater(XMVector3LengthSq(v), One))
				continue;

			return XMVector3Normalize(v);
		}
	}

	XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
	{
		XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR Zero = XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			XMVECTOR v = XMVectorSet(KMath::Rand(-1.0f, 1.0f),
				KMath::Rand(-1.0f, 1.0f),
				KMath::Rand(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (XMVector3Greater(XMVector3LengthSq(v), One))
				continue;

			// Ignore points in the bottom hemisphere.
			if (XMVector3Less(XMVector3Dot(n, v), Zero))
				continue;

			return XMVector3Normalize(v);
		}
	}

}//DirectX12









/*

using namespace DirectX;



XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One  = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while(true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.
		
		if( XMVector3Greater( XMVector3LengthSq(v), One) )
			continue;

		// Ignore points in the bottom hemisphere.
		if( XMVector3Less( XMVector3Dot(n, v), Zero ) )
			continue;

		return XMVector3Normalize(v);
	}
}
}*/