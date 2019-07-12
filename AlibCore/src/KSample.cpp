#include "stdafx.h"
#include "KSample.h"
#include "KWindow.h"
#include "KApplication.h"


KSample::KSample()
	: m_pWindow(nullptr)
	, m_pDocument(nullptr)
	, m_fAspectRatio(1.0F)
{

}


void KSample::Initialize()
{
	m_pWindow = KApplication::GetApp()->GetWindow();
	m_pDocument = KApplication::GetApp()->GetDocument();

	OnInit();
}

void KSample::Resize()
{
	OnResize();
}

void KSample::Update()
{
	OnUpdate();
}

void KSample::Render()
{
	OnRender();
}
void KSample::Destroy()
{
	OnDestroy();
}
