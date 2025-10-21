// ServerControlDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ServerControl.h"
#include "GeneralDlg.h"
#include "PgServerStateDoc.h"
#include "PgNetwork.h"
#include "ServerControlDlg.h"
#include "GeneralDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CServerControlDlg 대화 상자




CServerControlDlg::CServerControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerControlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_kID);
	DDX_Control(pDX, IDC_EDIT2, m_kPW);
}

BEGIN_MESSAGE_MAP(CServerControlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CServerControlDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CServerControlDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT2, &CServerControlDlg::OnEnChangeEdit2)
END_MESSAGE_MAP()


// CServerControlDlg 메시지 처리기
bool GetMainDirectory(LPTSTR lpszPath, int iPathLen)
{
	TCHAR chPath[MAX_PATH];
	if (GetModuleFileName(NULL, chPath, MAX_PATH) <= 0)
	{
		return FALSE;
	}
	LPTSTR lpszFind = _tcsrchr(chPath, _T('\\'));
	if (lpszFind == NULL)
	{
		lpszPath[0] = _T('\0');
		return false;
	}
	*(lpszFind+1) = _T('\0');
	_tcscpy_s(lpszPath, iPathLen, chPath);
	return true;
}

BOOL CServerControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CServerControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CServerControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerControlDlg::OnBnClickedOk()
{
	//
	CString kID, kPW;
	m_kID.GetWindowTextW(kID);
	m_kPW.GetWindowTextW(kPW);
	m_kID.SetWindowTextW(_T(""));
	m_kPW.SetWindowTextW(_T(""));

	g_kMainDlg.m_wstrID = kID;
	g_kMainDlg.m_wstrPW = kPW;

	if( g_kMainDlg.m_wstrID.empty()
	||	g_kMainDlg.m_wstrPW.empty() )
	{
		MessageBox(_T("Empty ID or Password"), _T("Error"), MB_OK);
		return;
	}
	//로그인 시도는 GenenralDlg 가 다 한다. (얘는 단순 ID/PW 입력)
	OnOK();
}

void CServerControlDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CServerControlDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CServerControlDlg::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CAboutDlg::OnBnClickedOk()
{
	OnOK();
}
