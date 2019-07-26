#include "stdafx.h"
#include "KMouseEventArgs.h"



KMouseEventArgs::KMouseEventArgs()
	: WParam(0), LParam(0) {}

KMouseEventArgs::KMouseEventArgs(WPARAM wParam, LPARAM lParam)
	: WParam(wParam), LParam(lParam) {}


KMouseEventArgs::~KMouseEventArgs()
{
}
