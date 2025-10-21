#include	"defines.h"
#include	"QCCore.h"
#include	"ListControl.h"
#include	"CheckMgr.h"

CWCCore::CWCCore(void)
{
	m_pkSystem		= NULL;
	m_pkErrLog		= NULL;
	m_pkLog			= NULL;
	hComboBOX_SELECT = NULL;
	i_ComboBOX_SELECT_iNdex = 0;
	m_StrComboBOX_SELECT = L"";
}

CWCCore::~CWCCore(void)
{
	SAFE_DELETE(m_pkSystem);
	SAFE_DELETE(m_pkLog);
	SAFE_DELETE(m_pkErrLog);
}

//-----------------------------------------------------------------------------
//	윈도우 커맨드
//-----------------------------------------------------------------------------
bool CWCCore::WMCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_BTN_DEVCHANGE:
		{
			m_pkSystem->Clear();
			m_pkErrLog->Clear();
			m_pkLog->Clear();
			g_kCheckMgr.Clear();
			m_TargetFolder.clear();
			//Reload();
		}break;
	case IDCANCEL:
		{
			g_kLogMgr.Stop();
			g_kCheckMgr.Stop();
			g_kLogMgr.VDeactivate();
			//g_kCheckMgr.VDeactivate();
			g_kCoreCenter.Close();
			EndDialog(g_hDlgWnd, 0);
		}break;
	case IDC_BUTTON_SELECT:
		{
			HWND hWndBtn;
			hWndBtn  = GetDlgItem(g_hDlgWnd, IDC_CMB_KIND);
			EnableWindow(hWndBtn, FALSE);
			hWndBtn  = GetDlgItem(g_hDlgWnd, IDC_BUTTON_SELECT);
			EnableWindow(hWndBtn, FALSE);

			m_pkSystem->Clear();
			m_pkErrLog->Clear();
			m_pkLog->Clear();
			g_kCheckMgr.Clear();

			TCHAR c_temp[256];
			i_ComboBOX_SELECT_iNdex = (int)SendMessage(hComboBOX_SELECT, CB_GETCURSEL, 0, 0); //선택한 iTem의 iNdex값 얻기
			SendMessage(hComboBOX_SELECT, CB_GETLBTEXT, i_ComboBOX_SELECT_iNdex, (LPARAM)c_temp); //iNdex에 위치한 TEXT, c_temp에 값 넣기
			m_StrComboBOX_SELECT = c_temp;
			Reload();
			SelectWorkMsg(i_ComboBOX_SELECT_iNdex);
		}break;
	}

	return	FALSE;
}


void CWCCore::NotifyMsg(LPARAM lParam)
{
	LPNMHDR	Hdr = (LPNMHDR)lParam;
	LPNMITEMACTIVATE	Hla = (LPNMITEMACTIVATE)lParam;

	if(m_pkErrLog)
	{
		if(Hdr->hwndFrom == m_pkErrLog->GetHandle())
		{			
			switch(Hdr->code)
			{
			case NM_DBLCLK:
				{			
					std::wstring::size_type start_pos = 0;
					std::wstring	szBuffer = L"";
					wchar_t	Caption[MAX_PATH] = {0,};

					if( Hla->iItem < 0 ) return;

					ListView_GetItemText(m_pkErrLog->GetHandle(), Hla->iItem, Hla->iSubItem, Caption, MAX_PATH);

					szBuffer = Caption;

					std::string::size_type index = szBuffer.find_last_of('\\');

					szBuffer = szBuffer.substr(start_pos, index);

					ShellExecute(m_pkErrLog->GetHandle(), _T("open"), _T("iexplore"), szBuffer.c_str() , NULL, SW_SHOWNORMAL);
				}break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
//  로드
//-----------------------------------------------------------------------------
bool CWCCore::CreateList(CListControl*& pList, HWND hWnd, DWORD dwID, std::wstring const& ColumnType)
{
	pList = new CListControl(hWnd, dwID);
	if( pList )
	{
		pList->SetColumn(ColumnType);
	}
	else
	{
		SAFE_DELETE(pList);
		return false;
	}

	return true;
}

bool CWCCore::Load()
{
	hComboBOX_SELECT  = GetDlgItem(g_hDlgWnd, IDC_CMB_KIND);

	for(int i=0; i<DTC_MAXTYPE; i++)
	{
		SendMessage(hComboBOX_SELECT, CB_ADDSTRING, 0, (LPARAM)ComboBOX_iTem[i]); //ComboBOX에 TEXT값 넣기
	}

	SendMessage(hComboBOX_SELECT, CB_SETCURSEL, 0, 1);


	CreateList(m_pkSystem, g_hDlgWnd, IDC_LIST_SYSTEM, WSTR_SYSTEM_LISTCOLUMN);
	CreateList(m_pkErrLog, g_hDlgWnd, IDC_LIST_ERRORTABLE, WSTR_ERRORLOG_LISTCOLUMN);
	CreateList(m_pkLog, g_hDlgWnd, IDC_LIST_LOG, WSTR_LOG_LISTCOLUMN);	

	//	쓰레드 관련 초기화
	CEL::INIT_CENTER_DESC	kCenterInit;
	kCenterInit.eOT	= BM::OUTPUT_JUST_TRACE;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.bIsUseDBWorker = true;
	kCenterInit.dwProactorThreadCount = 0;

	g_kCoreCenter.Init(kCenterInit);

	g_kCheckMgr.StartSvc(1);
	g_kLogMgr.StartSvc(1);

	CheckerInit();

	return	Reload();
}

bool CWCCore::CheckerInit()
{
	wchar_t	szTemp[MAX_PATH] = {0,};

	//	패치 서버 정보
	::GetPrivateProfileString(L"FORDER_INFO", L"DIR", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());
	m_TargetFolder = szTemp;

	return	true;
}

bool CWCCore::Reload()
{
	//	폴더 선택
	if( !m_TargetFolder.size() )
	{
		bool	bResult = SetFolder();
		if(!bResult)
		{
			return	false;
		}
	}
	SetWindowText(GetDlgItem(g_hDlgWnd, IDC_TXT_DEVFOLDER), m_TargetFolder.c_str());
	if(::SetCurrentDirectory(m_TargetFolder.c_str()) == FALSE)
	{
		return	false;
	}


	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);	

	sprintf_s(szErrorFileName, "\\%04u%02u%02u%02u%02u%02u_",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);	

	return	true;
}

void CWCCore::SelectWorkMsg(int iSelectValue)
{
	g_kCheckMgr.PutMsg(SCHECKMESSAGE());

	switch( iSelectValue )
	{
	case DTC_PATH_XML:
		{
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_XMLPATH, WSTR_XMLPATH));
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_GSAPATH, WSTR_NULLSTRING));
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_NIFPATH, WSTR_NULLSTRING));
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_COMPARE, WSTR_NULLSTRING));
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_FOLDER_LIST, m_TargetFolder));
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_COMPARELIST, m_TargetFolder));			
		}
		break;
	case DTC_EFFECT_XML:
		{
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_NIFEFFECT, WSTR_XMLEFFECT));
			g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_COMPARE_EFFECT, WSTR_NULLSTRING));			
		}break;
	}

	g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_QUIT, WSTR_NULLSTRING));
}

bool CWCCore::SetFolder()
{
	while(m_TargetFolder.size() == 0) 
	{ 
		if(!FolderSelect(g_hDlgWnd, m_TargetFolder))
		{
			MessageBox(g_hDlgWnd, L"Dragonica-Dev폴더를 지정해주세요.", L"Error", MB_OK);
		}
	}

	::WritePrivateProfileString(L"FORDER_INFO", L"DIR", m_TargetFolder.c_str(), WSTR_CONFIG_FILE.c_str());

	return	true;
}

bool CWCCore::AddListItem(const std::wstring& wstrItem, DWORD dwFlag)
{
	bool	bRet = false;
	switch(dwFlag)
	{
	case CHK_SYSTEM:
		{
			bRet = m_pkSystem->AddItem(wstrItem);
			ListView_EnsureVisible(m_pkSystem->GetHandle(), m_pkSystem->GetTapCol() - 1, FALSE);
		}break;
	case CHK_LOG:
		{
			bRet = m_pkLog->AddItem(wstrItem);
			ListView_EnsureVisible(m_pkLog->GetHandle(), m_pkLog->GetTapCol() - 1, FALSE);
		}break;
	case CHK_ERRLOG:
		{
			bRet = m_pkErrLog->AddItem(wstrItem);
			ListView_EnsureVisible(m_pkErrLog->GetHandle(), m_pkErrLog->GetTapCol() - 1, FALSE);
		}break;
	default:
		{
			bRet = m_pkErrLog->AddItem(wstrItem, dwFlag);
		}break;
	}

	return	bRet;
}

//-----------------------------------------------------------------------------
//  폴더 지정
//-----------------------------------------------------------------------------
bool CWCCore::SetDevFolder()
{
	while(m_TargetFolder.size() == 0) 
	{ 
		if(!FolderSelect(g_hDlgWnd, m_TargetFolder))
		{
			MessageBox(g_hDlgWnd, L"Dragonica-Dev폴더를 지정해주세요.", L"Error", MB_OK);
		}		
	}

	SetWindowText(GetDlgItem(g_hDlgWnd, IDC_TXT_DEVFOLDER), m_TargetFolder.c_str());
	if(::SetCurrentDirectory(m_TargetFolder.c_str()) == FALSE)
	{
		return	false;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);	

	sprintf_s(szErrorFileName, "\\%04u%02u%02u%02u%02u%02u_",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	return	true;
}

std::wstring CWCCore::GetDevFolder()
{
	return	m_TargetFolder;
}

int CWCCore::GetDevFolderLen()
{
	return m_TargetFolder.size();
}


char* CWCCore::GetErrorFileName()
{
	return szErrorFileName;
}

//-----------------------------------------------------------------------------
//  폴더를 선택
//-----------------------------------------------------------------------------
bool CWCCore::FolderSelect(const HWND& hWnd, std::wstring& wstrFolderName)
{
	BROWSEINFO			bi;

	bi.hwndOwner		= hWnd;	//부모윈도우핸들
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= NULL;
	bi.lpszTitle		= L"Dragonica-Dev폴더를 지정하세요.";
	bi.ulFlags			= 0;
	bi.lpfn				= NULL;
	bi.lParam			= 0;
	
	//폴더 선택 창을 만든다
	LPITEMIDLIST pidl	= SHBrowseForFolder(&bi);
	if(pidl == NULL) 
	{
		return	false; 
	}

	TCHAR	szTemp[MAX_PATH] = {0,};
	//폴더변수에저장한다
	SHGetPathFromIDList(pidl, szTemp);
	wstrFolderName = szTemp;

	return	true;
}
