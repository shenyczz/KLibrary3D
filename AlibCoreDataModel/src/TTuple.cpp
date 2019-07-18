#include "stdafx.h"
#include "TTuple.h"


ALIBAPI(void) TTuple_test()
{
	TTuple<int, 10> ia(0);
	TTuple<_tstring, 5> sa(_T("dsda"));

	int i = ia[8];
	int j = ia(0);

	//ia(0) = 0;
	ia[8] = -1;

	//string

}
