// MakeCRCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MakeCRC.h"
#include "MakeCRCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const BYTE PG_PATCHER_CRC_SIZE = 100;
const std::wstring PATCHER_FILE_NAME = L"Patcher.exe";
const std::wstring PATCHPATCHER_FILE_NAME = L"PatcherPatch.exe";

const std::wstring PATCHER_RESULT_FILE_NAME = L"patcher.id";
const std::wstring PATCHPATCHER_RESULT_FILE_NAME = L"patcherpatch.id";

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMakeCRCDlg dialog




CMakeCRCDlg::CMakeCRCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeCRCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakeCRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeCRCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CONV_TO_PATCHER, &CMakeCRCDlg::OnBnClickedBtnConvToPatcher)
	ON_BN_CLICKED(IDC_BTN_CONV_TO_PATCHPATCHER, &CMakeCRCDlg::OnBnClickedBtnConvToPatchPatcher)
END_MESSAGE_MAP()


// CMakeCRCDlg message handlers

BOOL CMakeCRCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMakeCRCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMakeCRCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMakeCRCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMakeCRCDlg::OnBnClickedBtnConvToPatcher()
{
	if( MakeExeCRC(PATCHER_FILE_NAME, PATCHER_RESULT_FILE_NAME) )
	{
		MessageBox(_T("Complete"));
	}
	else
	{
		MessageBox(_T("Failed"));
	}
}

void CMakeCRCDlg::OnBnClickedBtnConvToPatchPatcher()
{
	if( MakeExeCRC(PATCHPATCHER_FILE_NAME, PATCHPATCHER_RESULT_FILE_NAME) )
	{
		MessageBox(_T("Complete"));
	}
	else
	{
		MessageBox(_T("Failed"));
	}
}

bool CMakeCRCDlg::MakeExeCRC(const std::wstring& FileName, const std::wstring& ResultFileName)
{
	//	실행 파일의 절대 경로를 얻는다
	std::wstring wstrPath;
	wstrPath.resize(MAX_PATH);
	GetModuleFileName(GetModuleHandle(0), &wstrPath.at(0), MAX_PATH);
	if (wstrPath.size()==0)
	{
		return false;
	}

	//	해당 실행 파일의 CRC를 구한다
	m_vecCRC.resize(PG_PATCHER_CRC_SIZE);

	//	CRC정보를 문자열로
	if (BM::g_bCalcSHA2CheckSum)
	{
		unsigned char aucBuf[PG_SHA2_LENGTH];
		BM::GetFileSHA2(wstrPath, aucBuf);
		::memcpy(&m_vecCRC.at(0), aucBuf, PG_SHA2_LENGTH);
	}
	else
	{
		unsigned __int64 iCRC;
		BM::GetFileCRC(wstrPath, iCRC);
		_ui64toa_s(iCRC, &m_vecCRC.at(0), PG_PATCHER_CRC_SIZE, 10);
	}

	//	파일 이름을 검사하자
	if(FileName.size())
	{
		BM::EncSave(ResultFileName, m_vecCRC);
	}
	return true;
}