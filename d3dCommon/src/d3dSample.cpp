#include "stdafx.h"
#include "d3dSample.h"
#include "Utils.h"


namespace dxCommon
{

	d3dSample::d3dSample()
	{
		m_useWarpDevice = false;
		m_bFrameStats = false;

		m_4xMsaaEnable = false;
		m_4xMsaaQuality = 0;

		m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		m_World = Utils::Identity4x4;
		m_View = Utils::Identity4x4;
		m_Proj = Utils::Identity4x4;
	}

	d3dSample::~d3dSample()
	{
	}


	void d3dSample::OnInitialize()
	{
		Initialize();

#ifdef _DEBUG
		KUtil::Trace(_T("--OnInitialize"));
#endif
	}

	void d3dSample::OnResize()
	{
		Resizing();
		Resized();

#ifdef _DEBUG
		KUtil::Trace(_T("--OnResize"));
#endif
	}

	void d3dSample::OnUpdate()
	{
		Update();

#ifdef _DEBUG
		KUtil::Trace(_T("--OnUpdate"));
#endif
	}

	void d3dSample::OnRender()
	{
		Draw();

#ifdef _DEBUG
		KUtil::Trace(_T("--OnRender"));
#endif
	}

	void d3dSample::OnDestroy()
	{
		Shutdown();

#ifdef _DEBUG
		KUtil::Trace(_T("--OnDestroy"));
#endif
	}

	void d3dSample::OnAppIdle()
	{
		OnUpdate();
		OnRender();
		DoAppIdle();

#ifdef _DEBUG
		KUtil::Trace(_T("--OnAppIdle"));
#endif
	}
	void d3dSample::DoAppIdle()
	{
		if (m_bFrameStats)
		{
			CalculateFrameStats();
		}
	}











	// ¼ÆËãÖ¡×´Ì¬
	void d3dSample::CalculateFrameStats()
	{
		HWND hWnd = m_pWindow->Handle();
		KClock* pClock = KApplication::GetApp()->GetClock();

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((pClock->TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt;	// Ã¿ÃëÖ¡Êý
			float mspf = 1000.0f / fps;		// Ã¿Ö¡ºÁÃë

			KString ss = KString::Format(_T("%s : FPS = %4.1f, mspf = %f")
				, m_pWindow->GetWindowTitle()
				, fps
				, mspf);

			SetWindowText(hWnd, ss);

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}


}
