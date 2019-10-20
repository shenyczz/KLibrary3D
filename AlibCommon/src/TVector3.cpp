#include "stdafx.h"
#include "TVector3.h"


#ifdef _DEBUG


ALIBAPI(void) alib_test_TVector3()
{
	TVector3<float> vf1(1, 2, 3);
	TVector3<float> vf2(2, 3, 4);
	TVector3<float> vf3 = vf1 + vf2;
	vf2.x = 11;

	vf2 = 2 * vf3;
	vf2 = vf3 * 2;

	//TVector3<float>::Distance
}


#endif // _DEBUG
