#include "stdafx.h"
#include "KMouseEventArgs.h"



KMouseEventArgs::KMouseEventArgs()
	: MouseButtons(KMouseButtons::None)
	, Clicks(0), X(0), Y(0), Delta(0)
	, Tag(0)
{
}

KMouseEventArgs::KMouseEventArgs(KMouseButtons button, int clicks, int X, int Y, int delta)
	: MouseButtons(button)
	, Clicks(clicks), X(X), Y(Y), Delta(delta)
{

}

KMouseEventArgs::KMouseEventArgs(KMouseButtons button, int clicks, int X, int Y, int delta, void* tag)
	: MouseButtons(button)
	, Clicks(clicks), X(X), Y(Y), Delta(delta)
	, Tag(tag)
{

}


KMouseEventArgs::~KMouseEventArgs()
{
}
