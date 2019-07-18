#include "stdafx.h"
#include "KWindow.h"

#include "KApplication.h"
#include "KSample.h"
#include "KClock.h"
#include "KException.h"
#include "KUtil.h"

#include "KMouseListener.h"



KWindow::KWindow()
	: m_hWnd(NULL)
	, m_iWidth(1)
	, m_iHeight(1)
	, m_fAspectRatio(1.0f)
	, m_AppPaused(false)
	, m_Minimized(false)
	, m_Maximized(false)
	, m_Resizing(false)
	, m_FullScreenEnalbe(false)
	, m_pMouseListener(NULL)
	, m_WindowTitle(_T("MainWindow"))
	, m_WindowClassName(_T("__SFX_WINDOW_CLASS_NAME__"))
{
	m_pMouseListener = new KMouseListener(this);
}

KWindow::~KWindow()
{
	_delete(m_pMouseListener);
}



void KWindow::Initialize(KApplication* pApplication, int width, int height, LPCTSTR lpszName)
{
	m_iWidth = width;
	m_iHeight = height;

	m_WindowTitle = (lpszName) ? lpszName : m_WindowTitle;
	m_fAspectRatio = static_cast<float>(width) / static_cast<float>(height);

	HINSTANCE hInstance = pApplication->Instance();

	// 注册窗口类
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_WindowClassName.c_str();
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		throw KException(S_FALSE, __TFUNCTION__, __TFILE__, __LINE__);
	}

	// 创建窗口
	HWND hWnd = CreateWindow(
		m_WindowClassName.c_str(),
		m_WindowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_iWidth,
		m_iHeight,
		nullptr,		// We have no parent window.
		nullptr,		// We aren't using menus.
		hInstance,
		nullptr);		// pSample

	if (!hWnd)
	{
		throw KException(S_FALSE, __TFUNCTION__, __TFILE__, __LINE__);
	}

	m_hWnd = hWnd;
	UpdateWindow(hWnd);

	return;
}

// 主循环
int KWindow::MainLoop()
{
	KClock* pClock = KApplication::GetApp()->GetClock();
	KSample* pSample = KApplication::GetApp()->GetSample();

	pClock->Reset();

	MSG msg = {};

	while (msg.message != WM_QUIT)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//bool appIdle = !PeekMessage(&msg, NULL, 0, 0, 0);

#ifdef _DEBUG
			KUtil::Trace(_T("appIdle"));
#endif

			pClock->Tick();

			//m_AppPaused;

			if (true)
			{
				pSample->Update();
				pSample->Render();
			}
			else
			{
				Sleep(100);
			}
		}


	}//while()

	return static_cast<char>(msg.wParam);
}



#pragma region WndProc

//static
LRESULT CALLBACK KWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = S_OK;
	KWindow* pWindow = KApplication::GetApp()->GetWindow();

	if (!pWindow)
	{
		lResult = S_FALSE;
	}
	else
	{
		lResult = pWindow->MsgProc(hWnd, message, wParam, lParam);
	}

	return lResult;
}

// 消息处理
LRESULT KWindow::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

	switch (message)
	{
		case WM_CREATE:			// 0x0001
		{
			OnCreate(hWnd, wParam, lParam);
			break;
		}

		case WM_DESTROY:		// 0x0002
		{
			OnDestroy(hWnd, wParam, lParam);
			PostQuitMessage(0);
			return 0;
		}
		case WM_MOVE:			// 0x0003
		{
			OnMove(hWnd, wParam, lParam);
			break;
		}
		case WM_SIZE:			// 0x0005
		{
			OnSize(hWnd, wParam, lParam);
			return 0;
		}
		case WM_ACTIVATE:		// 0x0006
		{
			// OnActivte(hWnd, wParam, lParam)
			// OnDeactivate(hWnd, wParam, lParam)
//#define     WA_INACTIVE     0
//#define     WA_ACTIVE       1
//#define     WA_CLICKACTIVE  2
			break;
		}
		case WM_PAINT:			// 0x000F
		{
			OnPaint(hWnd, wParam, lParam);
			return 0;
		}
		case WM_CLOSE:			// 0x0010
		{
			OnClose(hWnd, wParam, lParam);
			break;
		}
		case WM_QUIT:			// 0x0012
		{
			OnQuit(hWnd, wParam, lParam);
			break;
		}
		case WM_GETMINMAXINFO:	// 0x0024
		{
			OnGetMinMaxInfo(hWnd, wParam, lParam);
			break;
		}


#pragma region --鼠标消息


		case WM_MOUSEMOVE:			// 0x0200
		{
			WORD lwx = LOWORD(lParam);	// X
			WORD hwy = HIWORD(lParam);	// Y
			MouseMove(hWnd, wParam, lParam);		// wParam = 0x0000000000000000
			if (wParam > 0)
			{
				// 按下鼠标拖动
				int X = 0;
			}
			break;
		}
		case WM_LBUTTONDOWN:		// 0x0201
		{
			WORD lwx = LOWORD(lParam);	// X
			WORD hwy = HIWORD(lParam);	// Y
			MouseDown(hWnd, wParam, lParam);		// wParam = 0x0000000000000010
			break;
		}
		case WM_LBUTTONUP:			// 0x0202
		{
			MouseUp(hWnd, wParam, lParam);			// wParam = 0x0000000000000000
			break;
		}
		case WM_LBUTTONDBLCLK:		// 0x0203
		{
			//MouseDown(hWnd, wParam, lParam);
			break;
		}
		case WM_RBUTTONDOWN:		// 0x0204
		{
			MouseDown(hWnd, wParam, lParam);		// wParam = 0x0000000000000002
			break;
		}
		case WM_RBUTTONUP:			// 0x0205
		{
			MouseUp(hWnd, wParam, lParam);			// wParam = 0x0000000000000000
			break;
		}
		case WM_RBUTTONDBLCLK:		// 0x0206
		{
			break;
		}
		case WM_MBUTTONDOWN:		// 0x0207
		{
			MouseDown(hWnd, wParam, lParam);		// wParam = 0x0000000000000010
			break;
		}
		case WM_MBUTTONUP:			// 0x0208
		{
			MouseUp(hWnd, wParam, lParam);			// wParam = 0x000000000000000
			break;
		}
		case WM_MBUTTONDBLCLK:		// 0x0209
		{
			break;
		}
		case WM_MOUSEWHEEL:			// 0x020A
		{
			MouseWheel(hWnd, wParam, lParam);		// wParam = 0x00000000ff880000
			break;
		}
		case WM_XBUTTONDOWN:		// 0x020B
		{
			MouseDown(hWnd, wParam, lParam);
			break;
		}
		case WM_XBUTTONUP:			// 0x020C
		{
			MouseUp(hWnd, wParam, lParam);
			break;
		}
		case WM_XBUTTONDBLCLK:		// 0x020D
		{
			break;
		}
		case WM_MOUSEHWHEEL:		// 0x020E
		{
			MouseHWheel(hWnd, wParam, lParam);
			break;
		}

#pragma endregion



		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

#pragma endregion


#pragma region On Windows Message


void KWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

#ifdef _DEBUG
	KUtil::Trace(_T("OnSize"));
#endif
}

void KWindow::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();
	pSample->Destroy();

#ifdef _DEBUG
	KUtil::Trace(_T("OnDestroy"));
#endif
}

void KWindow::OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

#ifdef _DEBUG
	KUtil::Trace(_T("OnMove"));
#endif
}

void KWindow::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

	m_iWidth = LOWORD(lParam);
	m_iHeight = HIWORD(lParam);
	m_fAspectRatio = static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight);

	pSample->Resize();

#ifdef _DEBUG
	KUtil::Trace(_T("OnSize"));
#endif
}

void KWindow::OnActivte(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

#ifdef _DEBUG
	KUtil::Trace(_T("OnActivte"));
#endif
}

void KWindow::OnDeactivte(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

#ifdef _DEBUG
	KUtil::Trace(_T("OnDeactivte"));
#endif
}

void KWindow::OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

	if (pSample)
	{
		pSample->Update();
		pSample->Render();
	}

#ifdef _DEBUG
	KUtil::Trace(_T("OnPaint"));
#endif
}

void KWindow::OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

#ifdef _DEBUG
	KUtil::Trace(_T("OnClose"));
#endif
}

void KWindow::OnQuit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

#ifdef _DEBUG
	KUtil::Trace(_T("OnQuit"));
#endif
}

void KWindow::OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	KSample* pSample = KApplication::GetApp()->GetSample();

	MINMAXINFO* pMinMaxInfo;
	pMinMaxInfo = (PMINMAXINFO)lParam;
	pMinMaxInfo->ptMinTrackSize = POINT{ 300,300 };	// 设置最小拖动范围

#ifdef _DEBUG
	KUtil::Trace(_T("OnGetMinMaxInfo"));
#endif
}


#pragma endregion


#pragma region On Windows Mouse Message - 鼠标消息

void KWindow::MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}
void KWindow::MouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		/*
		KMouseButtons MouseButtons;
		int Clicks;
		int Delta;
		LONG X;
		LONG Y;
		POINT Location;

		void* Tag;

		*/
		// hWnd - 监听对象
		// wParam - 按键状态
		// lParam - 当前位置 POINT
		KMouseListener* pMouseListener = m_pMouseListener;
		pMouseListener->MouseDown(this, KMouseEventArgs());
	}
	catch (const std::exception&)
	{
		// no body
	}
}
void KWindow::MouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}
void KWindow::MouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

void KWindow::MouseHWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}




#pragma endregion
