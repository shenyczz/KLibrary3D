#include "stdafx.h"
#include "KWindow.h"

#include "KApplication.h"
#include "KSample.h"
#include "KClock.h"
#include "KException.h"
#include "KUtil.h"

#include "KMouseListener.h"



KWindow::KWindow()
{
	m_hWnd = nullptr;

	m_iClientWidth = 1;
	m_iClientHeight = 1;
	m_fAspectRatio = 1.0f;

	m_AppPaused = false;
	m_Minimized = false;
	m_Maximized = false;
	m_Resizing = false;
	m_FullScreenEnalbe = false;

	m_QuitMessage = false;

	m_WindowTitle = _T("MainWindow");
	m_WindowClassName = _T("__SFX_WINDOW_CLASS_NAME__");

	m_pMouseListener = new KMouseListener(this);
}

KWindow::~KWindow()
{
	_delete(m_pMouseListener);
}



void KWindow::Initialize(KApplication* pApplication, int width, int height, LPCTSTR lpszName)
{
	m_iClientWidth = width;
	m_iClientHeight = height;

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
		m_iClientWidth,
		m_iClientHeight,
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
	
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) !=0 )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//bool AppIdle = PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) == 0;
		else
		{
			pClock->Tick();

			if (!m_AppPaused)
			{
				pSample->ApplicationIdle();
			}
			else
			{
				Sleep(100);
			}


#ifdef _DEBUG
			KUtil::Trace(_T("Application Idle"));
#endif
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

/*
OnGetMinMaxInfo
OnCreate
OnActivate
OnSize
OnMove
OnGetMinMaxInfo
OnPaint
{
	AppIdle
	...
}
OnGetMinMaxInfo
OnEnterSizeMove
{
	OnGetMinMaxInfo
	OnSize
	OnPaint
	...
}
OnExitSizeMove
{
	AppIdle
	...
}
OnDeactivate
{
	AppIdle
	...
}
OnClose
OnDeactivate
OnDestroy
*/

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
			break;
		}
		case WM_MOVE:			// 0x0003
		{
			OnMove(hWnd, wParam, lParam);
			break;
		}
		case WM_SIZE:			// 0x0005
		{
			OnSize(hWnd, wParam, lParam);
			break;
		}
		case WM_ACTIVATE:		// 0x0006
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				OnDeactivate(hWnd, wParam, lParam);
			}
			else
			{
				OnActivate(hWnd, wParam, lParam);
			}
			break;
		}

		case WM_PAINT:			// 0x000F
		{
			OnPaint(hWnd, wParam, lParam);
			break;
		}
		case WM_CLOSE:			// 0x0010
		{
			OnClose(hWnd, wParam, lParam);
			break;
		}
		case WM_QUIT:			// 0x0012
		{
			// 不会到 WM_QUIT,因为在消息循环中碰到 WM_QUIT 就结束了
			break;
		}
		case WM_GETMINMAXINFO:	// 0x0024
		{
			OnGetMinMaxInfo(hWnd, wParam, lParam);
			break;
		}
		case WM_ENTERSIZEMOVE:	// 0x0231
		{
			// WM_ENTERSIZEMOVE is sent when the user grabs the resize bars.
			OnEnterSizeMove(hWnd, wParam, lParam);
			break;
		}
		case WM_EXITSIZEMOVE:	// 0x0232
		{
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			OnExitSizeMove(hWnd, wParam, lParam);
			break;
		}

#pragma region --菜单消息

		case WM_MENUCHAR:	// 0x0120
		{
			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
			// Don't beep when we alt-enter.
			//return MAKELRESULT(0, MNC_CLOSE);
			break;
		}

#pragma endregion

#pragma region -键盘消息

		case WM_KEYDOWN:	// 0x0100
		{
			break;
		}
		case WM_KEYUP:		// 0x0101
		{
			if (wParam == VK_ESCAPE)	// 0x1B
			{
				//::PostMessage(hWnd, WM_DESTROY, 0, 0);
			}
			break;
		}

#pragma endregion

#pragma region --鼠标消息


		case WM_MOUSEMOVE:			// 0x0200
		{
			// wParam = 0x0000000000000000
			WORD lwx = LOWORD(lParam);	// x
			WORD hiy = HIWORD(lParam);	// y
			MouseMove(hWnd, wParam, lParam);
			if (wParam > 0)
			{
				// 按下鼠标拖动
				int x = 0;
			}
			break;
		}
		case WM_LBUTTONDOWN:		// 0x0201
		case WM_RBUTTONDOWN:		// 0x0204
		case WM_MBUTTONDOWN:		// 0x0207
		case WM_XBUTTONDOWN:		// 0x020B
		{
			// wParam = 0x0000000000000010
			WORD lwx = LOWORD(lParam);	// x
			WORD hiy = HIWORD(lParam);	// y
			MouseDown(hWnd, wParam, lParam);
			break;
		}
		case WM_LBUTTONUP:			// 0x0202
		case WM_RBUTTONUP:			// 0x0205
		case WM_MBUTTONUP:			// 0x0208
		case WM_XBUTTONUP:			// 0x020C
		{
			// wParam = 0x0000000000000000
			MouseUp(hWnd, wParam, lParam);
			break;
		}
		case WM_LBUTTONDBLCLK:		// 0x0203
		case WM_RBUTTONDBLCLK:		// 0x0206
		case WM_MBUTTONDBLCLK:		// 0x0209
		case WM_XBUTTONDBLCLK:		// 0x020D
		{
			// todo: MouseDblClick
			MouseDblClick(hWnd, wParam, lParam);
			break;
		}
		case WM_MOUSEWHEEL:			// 0x020A
		{
			// todo: MouseWheel() - wParam = 0x00000000ff880000
			MouseWheel(hWnd, wParam, lParam);
			break;
		}
		case WM_MOUSEHWHEEL:		// 0x020E
		{
			// todo: MouseHWheel()
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
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnCreate"));
#endif
}

void KWindow::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		pSample->OnDestroy();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnDestroy"));
#endif
}

void KWindow::OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnMove"));
#endif
}

void KWindow::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();

		m_iClientWidth = LOWORD(lParam);
		m_iClientHeight = HIWORD(lParam);
		m_fAspectRatio = static_cast<float>(m_iClientWidth) / static_cast<float>(m_iClientHeight);

		//if (md3dDevice)
		if (pSample->GetDevice3D())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;

				pSample->OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_Minimized)
				{
					// Restoring from minimized state
					m_AppPaused = false;
					m_Minimized = false;

					pSample->OnResize();
				}
				else if (m_Maximized)
				{
					m_AppPaused = false;
					m_Maximized = false;

					pSample->OnResize();
				}
				else if (m_Resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else
				{
					// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					pSample->OnResize();
				}
			}

		}//

	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnSize"));
#endif
}

void KWindow::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		KClock* pClock = KApplication::GetApp()->GetClock();

		m_AppPaused = false;
		pClock->Start();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnActivate"));
#endif
}

void KWindow::OnDeactivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		KClock* pClock = KApplication::GetApp()->GetClock();

		m_AppPaused = true;
		pClock->Stop();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnDeactivate"));
#endif
}

void KWindow::OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();

		if (pSample->GetDevice3D())
		{
			//pSample->OnUpdate();
			//pSample->OnRender();
		}
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnPaint"));
#endif
}

void KWindow::OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnClose"));
#endif
}

void KWindow::OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();

		MINMAXINFO* pMinMaxInfo;
		pMinMaxInfo = (PMINMAXINFO)lParam;
		pMinMaxInfo->ptMinTrackSize = POINT{ 300,300 };	// 设置最小拖动范围
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnGetMinMaxInfo"));
#endif
}

void KWindow::OnEnterSizeMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		KClock* pClock = KApplication::GetApp()->GetClock();

		m_AppPaused = true;
		m_Resizing = true;
		pClock->Stop();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnEnterSizeMove"));
#endif
}

void KWindow::OnExitSizeMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		KClock* pClock = KApplication::GetApp()->GetClock();

		m_AppPaused = false;
		m_Resizing = false;

		pClock->Start();
		pSample->OnResize();

	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("OnExitSizeMove"));
#endif
}

#pragma endregion


#pragma region On Windows Mouse Message - 鼠标消息

void KWindow::MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		pSample->MouseMove(wParam, lParam);
	}
	catch (const std::exception&)
	{
		// no body
	}
}
void KWindow::MouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		pSample->MouseDown(wParam, lParam);
	}
	catch (const std::exception&)
	{
		// no body
	}
}
void KWindow::MouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		pSample->MouseUp(wParam, lParam);
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("MouseUp"));
#endif
}
void KWindow::MouseDblClick(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{

	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("MouseDblClick"));
#endif
}
void KWindow::MouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
		pSample->MouseWheel(wParam, lParam);
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("MouseWheel"));
#endif
}
void KWindow::MouseHWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	try
	{
		KSample* pSample = KApplication::GetApp()->GetSample();
	}
	catch (const std::exception&) {}

#ifdef _DEBUG
	KUtil::Trace(_T("MouseWheel"));
#endif
}




#pragma endregion
