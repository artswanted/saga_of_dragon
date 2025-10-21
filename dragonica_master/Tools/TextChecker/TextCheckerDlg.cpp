// TextCheckerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TextChecker.h"
#include "TextCheckerDlg.h"
#include "FolderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
typedef enum eResultType
{
	eUNKNOWN_ERROR =1,
	eHTML_MAKE_FAIL=2,
	eSUCCESS =3,
}EResultType;

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


// CTextCheckerDlg 대화 상자




CTextCheckerDlg::CTextCheckerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTextCheckerDlg::IDD, pParent)
, m_bErrChkMode(true)
{
	::setlocale(LC_ALL,"");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTextCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_XML1, m_EdtXML1);
	DDX_Control(pDX, IDC_EDIT_XML2, m_EdtXML2);
	DDX_Control(pDX, IDC_EDIT_FOLDER, m_EdtFolder);
	DDX_Control(pDX, IDC_EDIT_RESULT_FILE, m_EdtResultFile);
	DDX_Control(pDX, IDC_BUTTON_SET_XML2, m_SetXmlBtn2);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_kBtnRun);
}

BEGIN_MESSAGE_MAP(CTextCheckerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BUTTON_SET_XML1, &CTextCheckerDlg::OnBnClickedButtonSetXml1)	
	ON_BN_CLICKED(IDC_BUTTON_SET_XML2, &CTextCheckerDlg::OnBnClickedButtonSetXml2)
	ON_BN_CLICKED(IDC_BUTTON_SETFOLDER, &CTextCheckerDlg::OnBnClickedButtonSetfolder)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CTextCheckerDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_RADIO_ERRCHK, &CTextCheckerDlg::OnBnClickedRadioErrchk)
	ON_BN_CLICKED(IDC_RADIO_COMPARE, &CTextCheckerDlg::OnBnClickedRadioCompare)
END_MESSAGE_MAP()


// CTextCheckerDlg 메시지 처리기

BOOL CTextCheckerDlg::OnInitDialog()
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

void CTextCheckerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTextCheckerDlg::OnPaint()
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
HCURSOR CTextCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTextCheckerDlg::OnBnClickedButtonSetXml1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(FALSE);

	CFileDialog dlg(TRUE,
		L"xml",
		NULL,
		OFN_CREATEPROMPT | OFN_HIDEREADONLY,
		L"TextTable Files (*.xml)|*.xml|",		
		NULL);
	if (dlg.DoModal() == IDCANCEL) return;


	m_EdtXML1.SetWindowText(dlg.GetPathName());
	
	//std::wstring wstrFullName = dlg.GetPathName();
	////std::wstring wstrFileName;
	////std::wstring wstrFolderName;
	////BM::DivFolderAndFileName(wstrFullName, wstrFolderName, wstrFileName);
	////m_EdtXML1.SetWindowText(wstrFullName.c_str());

	//std::string const kFullPath = MB(wstrFullName);

	//char acDrive[_MAX_DRIVE];
	//char acDir[_MAX_DIR];
	//char acFname[_MAX_FNAME];
	//char acExt[_MAX_EXT];
	//_splitpath_s(kFullPath.c_str(), acDrive, acDir, acFname, acExt);
	//
	//std::string temp;
	//temp+=acDrive;
	//temp+=acDir;
	//temp+=acFname;
	//temp+=acExt;
	//m_EdtXML1.SetWindowText(UNI(temp.c_str()));

	UpdateData(TRUE);
}

void CTextCheckerDlg::OnBnClickedButtonSetXml2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(FALSE);

	CFileDialog dlg(TRUE,
		L"xml",
		NULL,
		OFN_CREATEPROMPT | OFN_HIDEREADONLY,
		L"TextTable Files (*.xml)|*.xml|",		
		NULL);
	if (dlg.DoModal() == IDCANCEL) return;


	m_EdtXML2.SetWindowText(dlg.GetPathName());
	UpdateData(TRUE);

}

void CTextCheckerDlg::OnBnClickedButtonSetfolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(FALSE);

	CFolderDlg dlg(TRUE, _T(""), this);
	dlg.SetTitle(_T("Set folder to save"));
	if (dlg.DoModal() == IDCANCEL) return;
	m_EdtFolder.SetWindowText(dlg.GetFolderName());

	UpdateData(TRUE);
}

void CTextCheckerDlg::OnBnClickedButtonRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	::setlocale(LC_ALL,"");

	int const ZERO=0;

	CString kUpXmlName;	
	m_EdtXML1.GetWindowText(kUpXmlName);
	if(ZERO == kUpXmlName.GetLength())
	{// 대상 xml이 지정 되어 있어야 하고,
		MessageBox(L"Set first TextTable xml file");
		return;
	}

	CString kResultFolderName;
	m_EdtFolder.GetWindowText(kResultFolderName);	
	if(ZERO == kResultFolderName.GetLength())
	{// 결과 폴더가 지정 되어 있어야 하고,
		MessageBox(L"Set result folder");
		return;
	}
	
	CString kResultFileName;
	m_EdtResultFile.GetWindowText(kResultFileName);
	if(ZERO == kResultFileName.GetLength())
	{// 결과 파일 이름이 지정되어 있어야 한다. 
		MessageBox(L"Set result file name");
		return;
	}
	
	
	EResultType eResult = eUNKNOWN_ERROR;
	m_kBtnRun.EnableWindow(false);	
	if(true == m_bErrChkMode)
	{
		std::wstring kTemp = kUpXmlName;
		if( !m_kTxtChkMain.LoadXML(MB(kTemp)) )
		{
			MessageBox(L"Can't load xml file");
			m_kBtnRun.EnableWindow(true);
			return;
		}

		kTemp = kResultFolderName;
		kTemp+=L"\\";
		kTemp+=kResultFileName;
		kTemp+=L".html";

		if(m_kTxtChkMain.ShowErrorToHTML(MB(kTemp)) )
		{
			eResult = eSUCCESS;	
		}
		else
		{
			eResult = eHTML_MAKE_FAIL;
		}
	}
	else
	{
		CString kDnXmlName;		
		m_EdtXML2.GetWindowText(kDnXmlName);
		if(ZERO == kDnXmlName.GetLength())
		{// 두번째 대상 xml이 지정 되어 있어야 한다
			MessageBox(L"Set second TextTable xml file");
			m_kBtnRun.EnableWindow(true);
			return;
		}

		std::wstring kTemp1 = kUpXmlName;
		std::wstring kTemp2 = kDnXmlName;
		
		if( !m_kTxtChkMain.LoadXML(MB(kTemp1), MB(kTemp2)) )
		{
			MessageBox(L"Can't load xml file");
			m_kBtnRun.EnableWindow(true);
			return;
		}

		kTemp1 = kResultFolderName;
		kTemp1+=L"\\";
		kTemp1+=kResultFileName;
		kTemp1+=L".html";
		if(m_kTxtChkMain.ShowAllResultToHTML(MB(kTemp1)))
		{
			eResult = eSUCCESS;
		}
		else
		{
			eResult = eHTML_MAKE_FAIL;			
		}
	}
	
	switch(eResult)
	{
	case eSUCCESS:
		{
			MessageBox(L"Done");		
		}break;
	case eHTML_MAKE_FAIL:
		{
			MessageBox(L"HTML create failed");
		}break;
	default:
		{
			MessageBox(L"Unknown error");
		}break;
	}
	m_kBtnRun.EnableWindow(true);
}

void CTextCheckerDlg::OnBnClickedRadioErrchk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_EdtXML2.SetWindowTextW(L"");
	m_SetXmlBtn2.EnableWindow(false);
	m_bErrChkMode = true;
}

void CTextCheckerDlg::OnBnClickedRadioCompare()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_SetXmlBtn2.EnableWindow(true);
	m_bErrChkMode = false;
}
BOOL CTextCheckerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if((pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN))
	{
		OnBnClickedButtonRun();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}