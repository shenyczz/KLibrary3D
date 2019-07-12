#include "stdafx.h"
#include "KMouseEventArgs.h"



KMouseEventArgs::KMouseEventArgs()
	: MouseButtons(KMouseButtons::None)
	, Clicks(0), X(0), Y(0), Delta(0)
	, Tag(0)
{
}

KMouseEventArgs::KMouseEventArgs(KMouseButtons button, int clicks, int x, int y, int delta)
	: MouseButtons(button)
	, Clicks(clicks), X(x), Y(y), Delta(delta)
{

}

KMouseEventArgs::KMouseEventArgs(KMouseButtons button, int clicks, int x, int y, int delta, void* tag)
	: MouseButtons(button)
	, Clicks(clicks), X(x), Y(y), Delta(delta)
	, Tag(tag)
{

}


KMouseEventArgs::~KMouseEventArgs()
{
}
