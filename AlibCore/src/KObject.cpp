#include "stdafx.h"
#include "KObject.h"


KObject::KObject()
	: m_pOwner(NULL)
	, m_szName(_T(""))
{
	//m_szName = typeid(*this).name;
}


KObject::~KObject()
{
}
