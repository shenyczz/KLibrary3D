#pragma once



template<typename _Ty>
struct TVector3
{
	TVector3() : TVector3(0, 0, 0) {}


	TVector3(_Ty _x, _Ty _y, _Ty _z) : x(_x), y(_y), z(_z) {}





	_Ty x, y, z;

};
