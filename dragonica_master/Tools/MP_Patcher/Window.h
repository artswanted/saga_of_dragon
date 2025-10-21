#pragma once

#include "defines.h"

enum
{
	WINDOW_DEFAULT		= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
	WINDOW_SIZE			= WINDOW_DEFAULT | WS_THICKFRAME,
	WINDOW_POPUP		= WS_POPUP | WS_BORDER,

	WINDOW_EX_DEFAULT	= 0,
};

struct SWindowIconInfo
{
	HICON	hIcon;
	HICON	hIconSm;
};

typedef void (* IDLEFUNC)(HWND);

class Window
{
public:
	void Initialize(std::wstring const& WindowName, DWORD const Style = WINDOW_DEFAULT, DWORD const ExStyle = WINDOW_EX_DEFAULT);
	bool Register(SWindowIconInfo const& IconInfo, WNDPROC WndProc);
	bool Create();
	void Display(int const iCmdShow);
	bool IsShow() const { return m_bDisplay; };
	void OnLoop(IDLEFUNC IDLEFunc);
	void Terminate();

	void SetSize(unsigned short W, unsigned short H);
	void SetSize(POINT const& Size);
	void SetDirectory();
	
	HINSTANCE	GetInstance(void)		{ return m_hInstance;		};
	HWND		GetHandle(void)	const	{ return m_hHandle;			};
	HWND		GetParentHandle(void)	{ return m_hParentHandle;	};

	explicit Window(HWND hParentHandle, HINSTANCE hInstance);
	virtual ~Window(void);

private:
	HWND		m_hHandle;
	HWND		m_hParentHandle;
	DWORD		m_Style;
	DWORD		m_ExStyle;

	std::wstring	m_strName;
	POINT			m_PtSize;

	bool			m_bDisplay;

	static HINSTANCE	m_hInstance;
};
