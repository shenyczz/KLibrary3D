#include "stdafx.h"
#include "KSample.h"
#include "KWindow.h"
#include "KApplication.h"


KSample::KSample()
	: m_pWindow(nullptr)
	, m_pDocument(nullptr) {}


void KSample::Initialize()
{
	m_pWindow = KApplication::GetApp()->GetWindow();
	m_pDocument = KApplication::GetApp()->GetDocument();

	OnInit();
}




void KSample::MouseDown(WPARAM wParam, LPARAM lParam)
{
	OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
}
void KSample::MouseUp(WPARAM wParam, LPARAM lParam)
{
	OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
}
void KSample::MouseMove(WPARAM wParam, LPARAM lParam)
{
	OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
}
void KSample::MouseWheel(WPARAM wParam, LPARAM lParam)
{
	OnMouseWheel(wParam, lParam);
}





const float& KSample::AspectRatio() const
{
	return m_pWindow->AspectRatio();
}

