#include "Window.h"

HINSTANCE Window::m_hInstance = NULL;

Window::Window(HWND hParentHandle, HINSTANCE hInstance)
	: m_hHandle(NULL)
	, m_hParentHandle(hParentHandle)
	, m_Style(0)
	, m_ExStyle(0)
{
	m_hInstance = hInstance;
}

Window::~Window(void)
{
}

void Window::Initialize(std::wstring const& WindowName, DWORD const Style, DWORD const ExStyle)
{
	m_strName	= WindowName;
	m_Style		= Style;
	m_ExStyle	= ExStyle;
}

bool Window::Register(SWindowIconInfo const& IconInfo, WNDPROC WndProc)
{
	WNDCLASSEX WndClass;
	WndClass.cbSize			= sizeof(WNDCLASSEX);
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;
	WndClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon			= IconInfo.hIcon;
	WndClass.hIconSm		= IconInfo.hIconSm;
	WndClass.hInstance		= m_hInstance;
	WndClass.lpszClassName	= m_strName.c_str();
	WndClass.lpszMenuName	= NULL;
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc	= WndProc;

	int const bRet = ::RegisterClassEx(&WndClass);
	if( !bRet )
	{
		return false;
	}
	return true;
}

bool Window::Create()
{
	m_hHandle = ::CreateWindowEx(m_ExStyle, m_strName.c_str(), m_strName.c_str(), m_Style, 0, 0, 
								 10, 10, m_hParentHandle, (HMENU)NULL, m_hInstance, NULL);

	if( m_hHandle )
	{
		SetClassLong(m_hHandle, GCL_HICON, (LONG)LoadIcon(m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME)));
		return true;
	}
	return false;
}

void Window::Display(int const iCmdShow)
{
	m_bDisplay = (iCmdShow == SW_SHOW);
	::ShowWindow(m_hHandle, iCmdShow);
	::UpdateWindow(m_hHandle);
}

void Window::OnLoop(IDLEFUNC IDLEFunc)
{
	MSG	Message;
	while( true )
	{
		if( ::PeekMessage(&Message, NULL, 0L, 0L, PM_REMOVE) )
		{
			if( Message.message == WM_QUIT )
			{
				break;
			}

			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}
		else
		{
			IDLEFunc(m_hHandle);
			Sleep(5);
		}

		if( CPatchProcess::eProcess_Preparing != g_PProcess.GetProcess() )
		{
			Sleep(15);
		}
	}
}

void Window::Terminate()
{
	::UnregisterClass(m_strName.c_str(), m_hInstance);
}

void Window::SetSize(unsigned short W, unsigned short H)
{
	m_PtSize.x = W;
	m_PtSize.y = H;

	RECT	rcWindow = { 0, 0, m_PtSize.x, m_PtSize.y };
	::AdjustWindowRectEx(&rcWindow, m_Style, FALSE, m_ExStyle);

	int const iWinSizeX = rcWindow.right - rcWindow.left;
	int const iWinSizeY = rcWindow.bottom - rcWindow.top;
	int const iStartX = (::GetSystemMetrics(SM_CXSCREEN) - iWinSizeX) / 2;
	int const iStartY = (::GetSystemMetrics(SM_CYSCREEN) - iWinSizeY) / 2;

	::MoveWindow(m_hHandle, iStartX, iStartY, iWinSizeX, iWinSizeY, FALSE);
	::SetWindowPos(m_hHandle, NULL, iStartX, iStartY, iWinSizeX, iWinSizeY, NULL);
}

void Window::SetSize(POINT const& Size)
{
	SetSize(Size.x, Size.y);
}

void Window::SetDirectory()
{
	wchar_t	szPath[MAX_PATH] = {0, };
	GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	if( 0 == wcslen(szPath) )
	{
		return;
	} 
	wcsrchr(szPath, L'\\')[1] = 0;
	SetCurrentDirectory(szPath);
}