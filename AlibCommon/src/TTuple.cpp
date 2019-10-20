#include "stdafx.h"
#include "TTuple.h"

#ifdef _DEBUG


ALIBAPI(void) alib_test__TTuple()
{
	TTuple<int, 10> ia;
	TTuple<_tstring, 5> sa(_T("dsda"));

	//TTuple<10> a;

	int i = ia[8];
	int j = ia(0);

	//ia(0) = 0;
	ia[8] = -1;

	//string
	//TSlice<int> ic(1, 1);
	//int h = ic.Height;
}


#endif // _DEBUG
