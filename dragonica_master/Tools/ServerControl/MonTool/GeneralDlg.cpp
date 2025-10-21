// GeneralDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "ServerControl.h"
#include "PgNetwork.h"
#include "PgServerStateDoc.h"
#include "PgRecvFromManagementServer.h"
#include "GeneralDlg.h"
#include "PgSendWrapper.h"
#include "ServerOnDlg.h"
#include "PgMCTTask.h"
#include "FileDownDlg.h"
#include "ChangeMaxUserDlg.h"
#include "NoticeDlg.h"
#include "PgServerCmdMgr.h"

// CGeneralDlg 대화 상자입니다.

#define INVAL_TIME 1

IMPLEMENT_DYNAMIC(CGeneralDlg, CDialog)

CGeneralDlg::CGeneralDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralDlg::IDD, pParent)
	, m_kStrCCU(_T(""))
	, m_iSelectTreeItem(0)
	, m_iCurFileCount(0)
	, m_dCurFileSize(0)
	, m_iTotalFileCount(0)
	, m_dTotalFileSize(0)
	, m_bIsDownload(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CGeneralDlg::~CGeneralDlg()
{
	m_kContStateWnd.clear();
	m_kContLogWnd.clear();
	m_kContProcessWnd.clear();
}

void CGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER, m_kListCtrl);
	DDX_Control(pDX, IDC_CONFIRM_KILL, m_kCheckBtnKillServer);
	DDX_Control(pDX, IDC_BTN_PROCESSKILL, m_kBtnKillServer);
	DDX_Control(pDX, IDC_CONFIRM_RESYNC, m_kCheckBtnReSync);
	DDX_Control(pDX, IDC_BTN_RESYNC, m_kBtnReSync);
	DDX_Control(pDX, IDC_DISP_1, m_kRadioBtn1);
	DDX_Control(pDX, IDC_DISP_2, m_kRadioBtn2);
	DDX_Control(pDX, IDC_DISP_3, m_kRadioBtn3);
	DDX_Control(pDX, IDC_DISP_4, m_kRadioBtn4);
	DDX_Control(pDX, IDD_TreeView, m_kTreeCtrl);
	DDX_Control(pDX, IDC_BTN_PATCH2, m_kBtnChangeMaxUser);
	DDX_Control(pDX, IDC_TAB4, m_kTabCtrl);	
	DDX_Text(pDX, IDC_TEXT_LIST_INFO, m_kStrCCU);
	DDX_Control(pDX, IDC_STATIC_VIEW, m_kGroupDisplayType);
	DDX_Control(pDX, IDC_GROUP_TERMINATE, m_kGroupTerminate);
	DDX_Control(pDX, IDC_GROUP_RESYNC, m_kGroupReSync);
	DDX_Control(pDX, IDC_GROUP_MAX_USER, m_kGroupMaxUser);
	DDX_Control(pDX, IDC_PROGRESS2, m_kProgressDownload);
	DDX_Control(pDX, IDC_BTN_ON3, m_kBtnDownload);
	DDX_Control(pDX, IDC_GROUP_DOWNLOAD, m_kGroupDownload);
	DDX_Control(pDX, IDC_TEXT_FILE_NAME, m_kStrFileName);
	DDX_Control(pDX, IDC_TEXT_FILE_SIZE, m_kStrFileSize);
	DDX_Control(pDX, IDC_BTN_ON, m_kBtnServerOn);
	DDX_Control(pDX, IDC_BTN_OFF, m_kBtnServerOff);
	DDX_Control(pDX, IDC_GROUP_SERVER_SWITCH, m_kGroupServerSwitch);
	DDX_Control(pDX, IDC_TEXT_FILE_COUNT, m_kStrFileCount);
	DDX_Control(pDX, IDC_TEXT_PROCESS, m_kStaticProcess);
	DDX_Control(pDX, IDC_JOB_PROGRESS, m_kProgressJob);
	DDX_Control(pDX, IDC_GROUP_PROCESS, m_kGroupProcess);
}


BEGIN_MESSAGE_MAP(CGeneralDlg, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY (LVN_COLUMNCLICK, IDC_LIST_SERVER, OnColumnclickServerList)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CGeneralDlg::OnBnClickedBtnRefresh)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SERVER, &CGeneralDlg::OnNMCustomdrawListServer)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CGeneralDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CGeneralDlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_ON, &CGeneralDlg::OnBnClickedBtnServerOn)
	ON_BN_CLICKED(IDC_BTN_OFF, &CGeneralDlg::OnBnClickedBtnServerOff)
	ON_BN_CLICKED(IDC_BTN_PROCESSKILL, &CGeneralDlg::OnBnClickedBtnProcesskill)
	ON_BN_CLICKED(IDC_CHECK_USEBEEP, &CGeneralDlg::OnBnClickedCheckUsebeep)
	ON_BN_CLICKED(IDC_CONFIRM_KILL, &CGeneralDlg::OnBnClickedConfirmKill)
	ON_COMMAND(ID_FILE_EXIT, &CGeneralDlg::OnFileExit)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDD_TreeView, &CGeneralDlg::OnTvnSelchangedTreeview)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SERVER, &CGeneralDlg::OnLvnItemchangedListServer)
	ON_BN_CLICKED(IDC_BTN_PATCH2, &CGeneralDlg::OnBnClickedBtnMaxUserChange)
	ON_BN_CLICKED(IDC_BTN_RESYNC, &CGeneralDlg::OnBnClickedBtnResync)
	ON_BN_CLICKED(IDC_CONFIRM_RESYNC, &CGeneralDlg::OnBnClickedConfirmResync)
	ON_BN_CLICKED(IDC_DISP_1, &CGeneralDlg::OnBnClickedDisp)
	ON_BN_CLICKED(IDC_DISP_2, &CGeneralDlg::OnBnClickedDisp)
	ON_BN_CLICKED(IDC_DISP_3, &CGeneralDlg::OnBnClickedDisp)
	ON_BN_CLICKED(IDC_DISP_4, &CGeneralDlg::OnBnClickedDisp)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB4, &CGeneralDlg::OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_BTN_ON3, &CGeneralDlg::OnBnClickedBtnDownload)
	ON_BN_CLICKED(IDC_BTN_OFF3, &CGeneralDlg::OnBnClickedBtnExpand)
	ON_BN_CLICKED(IDC_BTN_OFF4, &CGeneralDlg::OnBnClickedBtnFold)
	ON_BN_CLICKED(IDC_BTN_SELECT_ALL, &CGeneralDlg::OnBnClickedBtnSelectAll)
	ON_BN_CLICKED(IDC_BTN_SELECT_ROOT, &CGeneralDlg::OnBnClickedBtnSelectRoot)
	ON_BN_CLICKED(IDC_BTN_SELECT_REALM, &CGeneralDlg::OnBnClickedBtnSelectRealm)
	ON_BN_CLICKED(IDC_BTN_NOTICE, &CGeneralDlg::OnBnClickedBtnNotice)
END_MESSAGE_MAP()

#include <assert.h>
// CGeneralDlg 메시지 처리기입니다.
BOOL CGeneralDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_kMenu.DestroyMenu();
	SetMenu(&m_kMenu);

	RECT kSize;
	GetWindowRect(&kSize);

	m_kListCtrl.SetExtendedStyle(
		LVS_EX_SIMPLESELECT | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP |
		LVS_EX_LABELTIP | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES  | LVS_EX_SUBITEMIMAGES  );
		//LVS_EX_FULLROWSELECT);

	m_kTabCtrl.InsertItem(1, L"Server State");
	m_kTabCtrl.InsertItem(2, L"Log Info");

	SetColumnListCtrl();

	// Create ImageList
	m_kImageList.Create(16, 16, ILC_COLOR32, 2, 0);
	HICON hIcon;
	VERIFY((hIcon = AfxGetApp()->LoadIcon(IDI_STATE_X)) != 0);
	m_kImageList.Add(hIcon);
	VERIFY((hIcon = AfxGetApp()->LoadIcon(IDI_STATE_O)) != 0);
	m_kImageList.Add(hIcon);
	m_kImageList.Add(hIcon);
//	m_listServer.SetImageList(&m_kImageList, LVSIL_SMALL);

	m_hStateSelectTreeItem = m_hStateLastSelectTreeItem = 
	m_hLogSelectTreeItem = m_hLogLastSelectTreeItem = NULL;
	m_iSelectedListColum = 0;
	m_iSiteNo = 1;

	CallLoginDlg();

	m_kRadioBtn1.SetCheck(TRUE);

	// Log Group
	m_kContLogWnd.push_back(&m_kProgressDownload);
	m_kContLogWnd.push_back(&m_kBtnDownload);
	m_kContLogWnd.push_back(&m_kGroupDownload);
	m_kContLogWnd.push_back(&m_kStrFileName);
	m_kContLogWnd.push_back(&m_kStrFileSize);
	m_kContLogWnd.push_back(&m_kStrFileCount);

	// State Group
	m_kContStateWnd.push_back(&m_kGroupTerminate);
	m_kContStateWnd.push_back(&m_kCheckBtnKillServer);
	m_kContStateWnd.push_back(&m_kBtnKillServer);
	m_kContStateWnd.push_back(&m_kGroupReSync);
	m_kContStateWnd.push_back(&m_kCheckBtnReSync);
	m_kContStateWnd.push_back(&m_kBtnReSync);
	m_kContStateWnd.push_back(&m_kGroupMaxUser);
	m_kContStateWnd.push_back(&m_kBtnChangeMaxUser);
	m_kContStateWnd.push_back(&m_kGroupDisplayType);
	m_kContStateWnd.push_back(&m_kRadioBtn1);
	m_kContStateWnd.push_back(&m_kRadioBtn2);
	m_kContStateWnd.push_back(&m_kRadioBtn3);
	m_kContStateWnd.push_back(&m_kRadioBtn4);
	m_kContStateWnd.push_back(&m_kGroupServerSwitch);
	m_kContStateWnd.push_back(&m_kBtnServerOn);
	m_kContStateWnd.push_back(&m_kBtnServerOff);

	// Process Group
	m_kContProcessWnd.push_back(&m_kStaticProcess);
	m_kContProcessWnd.push_back(&m_kProgressJob);
	m_kContProcessWnd.push_back(&m_kGroupProcess);
		

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	m_bIsDownload = false;

	return TRUE;
}

void CGeneralDlg::SetTitleText(std::wstring wstrPatchVer)
{//! 제목 표시줄에 각종 버전을 표시하자
	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	std::wstring wstrTitleText = L"[Tool Version] : ";
	wstrTitleText += chFileVersion;
	wstrTitleText += L" / [Product Version] : ";
	wstrTitleText += chProductVersion;
	wstrTitleText += L" / [Packet Version S] : ";
	wstrTitleText += PACKET_VERSION_S;
	wstrTitleText += L" / [Packet Version C] : ";
	wstrTitleText += PACKET_VERSION_C;
	wstrTitleText += L" / [Patch Version] : ";
	wstrTitleText += wstrPatchVer;
	SetWindowText(wstrTitleText.c_str());
}

void CGeneralDlg::SetColumnListCtrl(EDisplayMode eMode)
{
	//! Item / Column 삭제
	m_kListCtrl.DeleteAllItems();
	CHeaderCtrl* pHeader = (CHeaderCtrl*) m_kListCtrl.GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	for(int i=0; i<nColumnCount; ++i)
	{
		m_kListCtrl.DeleteColumn(0);
	}

	//! Column 초기화
	int iCol = 0;

	switch( eMode )
	{
	case DM_SERVER_STATE:
		{
			m_kListCtrl.InsertColumn(iCol++, L"", LVCFMT_CENTER, 20);
			m_kListCtrl.InsertColumn(iCol++, L"Server Name", LVCFMT_CENTER, 120);
			m_kListCtrl.InsertColumn(iCol++, L"Type", LVCFMT_CENTER, 85);
			m_kListCtrl.InsertColumn(iCol++, L"Machin IP", LVCFMT_CENTER, 100);
			m_kListCtrl.InsertColumn(iCol++, L"ServerNo", LVCFMT_CENTER, 65);
			m_kListCtrl.InsertColumn(iCol++, L"Channel", LVCFMT_CENTER, 60);
			m_kListCtrl.InsertColumn(iCol++, L"Connect", LVCFMT_CENTER, 60);
			m_kListCtrl.InsertColumn(iCol++, L"Process", LVCFMT_CENTER, 60);
			m_kListCtrl.InsertColumn(iCol++, L"ADDR", LVCFMT_CENTER, 120);
			m_kListCtrl.InsertColumn(iCol++, L"Now Users", LVCFMT_CENTER, 80);
			m_kListCtrl.InsertColumn(iCol++, L"Max Users", LVCFMT_CENTER, 80);
			m_kListCtrl.InsertColumn(iCol++, L"File Version", LVCFMT_CENTER, 80);
		}break;
	case DM_LOG_INFO:
		{
			STreeData kTreeData;
			GetTreeData(kTreeData, DM_LOG_INFO);

			m_kListCtrl.InsertColumn(iCol++, L"", LVCFMT_CENTER, 20);
			switch( kTreeData.nPriKey )
			{
			case TT_SITE_ROOT:
				{
					if( FT_DUMP == kTreeData.nSecKey )
					{
						m_kListCtrl.InsertColumn(iCol++, L"TYPE", LVCFMT_CENTER, 85);
						m_kListCtrl.InsertColumn(iCol++, L"Machin IP", LVCFMT_CENTER, 180);
						m_kListCtrl.InsertColumn(iCol++, L"Size / Count", LVCFMT_CENTER, 180);
					}
					else if( FT_CATION_HACK == kTreeData.nSecKey )
					{
						m_kListCtrl.InsertColumn(iCol++, L"DATE", LVCFMT_CENTER, 85);
						m_kListCtrl.InsertColumn(iCol++, L"CAUTION_LOG Size / Count", LVCFMT_CENTER, 180);
						m_kListCtrl.InsertColumn(iCol++, L"HACK_LOG Size / Count", LVCFMT_CENTER, 180);
					}
				}break;
			case TT_REALM_ROOT:
				{
					m_kListCtrl.InsertColumn(iCol++, L"Channel", LVCFMT_CENTER, 85);
					m_kListCtrl.InsertColumn(iCol++, L"Type", LVCFMT_CENTER, 85);
					m_kListCtrl.InsertColumn(iCol++, L"ServerNo", LVCFMT_CENTER, 120);
					m_kListCtrl.InsertColumn(iCol++, L"INFO_LOG Size / Count", LVCFMT_CENTER, 150);
					m_kListCtrl.InsertColumn(iCol++, L"CEL_LOG Size / Count", LVCFMT_CENTER, 150);
					m_kListCtrl.InsertColumn(iCol++, L"DUMP Size / Count", LVCFMT_CENTER, 150);
				}break;
			default :
				break;
			}
		}break;
	default :
		{
			m_kListCtrl.InsertColumn(iCol++, L"", LVCFMT_CENTER, 20);
			m_kListCtrl.InsertColumn(iCol++, L"TIME", LVCFMT_CENTER, 120);
			m_kListCtrl.InsertColumn(iCol++, L"CMD TYPE", LVCFMT_CENTER, 85);
			m_kListCtrl.InsertColumn(iCol++, L"VALUE", LVCFMT_CENTER, 100);
			m_kListCtrl.InsertColumn(iCol++, L"RESULT", LVCFMT_CENTER, 65);
		}break;
	}
}
void CGeneralDlg::OnColumnclickServerList(NMHDR* pNMHDR, LRESULT* pResult)
{
	BM::CAutoMutex kLock( m_kMutex );

	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	ESortType const eSortBy = static_cast<ESortType>(pNMListView->iSubItem);
	switch ( eSortBy )
	{
	case ST_CHECK:
		{
			// 전체 아이템 리스트 체크
			for ( int i=0 ; i<m_kListCtrl.GetItemCount() ; ++i )
			{
				BOOL const bCheck = m_kListCtrl.GetCheck(i);
				m_kListCtrl.SetCheck( i, !bCheck );
			}
		}break;
	}
	m_kListCtrl.SortItems( CompareServerList, eSortBy );

	*pResult = 0;
}

int CALLBACK CGeneralDlg::CompareServerList(LPARAM lParam1, LPARAM lParam2, 
                                      LPARAM lParamSort)
{
	bool    bComplementResult;
	int     result = 0;

	if (lParamSort < 0)
	{
		lParamSort = -lParamSort;
		bComplementResult = true;
	}
	else
	{
		bComplementResult = false;
	}


/*	SGameServerInfo kServerInfo1, kServerInfo2;

	int iServerIdx = (int)lParam1;
	bool bCheck1 = g_kGameServerMgr.GetServerInfo(iServerIdx, kServerInfo1);
	iServerIdx = (int)lParam2;
	bool bCheck2 = g_kGameServerMgr.GetServerInfo(iServerIdx, kServerInfo2);

	// 두개다 찾았으면 깨지지 않은 데이터다.
	// 두개 데이터를 비교하면서 정렬 해주자.
	// 리턴값이 0초과면 왼쪽값이 크고 0이면 같고 0보다 작으면 작다.
	if (bCheck1 && bCheck2)
	{
		int asdf = 0;
		switch (lParamSort)
		{
		case SERVER_NAME:
			result = _tcscmp(kServerInfo1.strName.c_str(), kServerInfo2.strName.c_str());
			break;
		case SERVER_TYPE:
			result = kServerInfo1.nServerType - kServerInfo2.nServerType;
			break;
		case MACHINIP:
			result = _tcscmp(kServerInfo1.strMachinIP.c_str(), kServerInfo2.strMachinIP.c_str());
			break;
		case SERVER_NO:
			result = kServerInfo1.nServerNo - kServerInfo2.nServerNo;
			break;
		case CHANNEL:
			result = kServerInfo1.nChannel - kServerInfo2.nChannel;
			break;
		case CONNECT:
			result = (kServerInfo1.bReadyToService && !kServerInfo2.bReadyToService) ? 1 : -1;
			break;
		case PROCESS:
			result = (kServerInfo1.bProcessHandle && !kServerInfo2.bProcessHandle) ? 1 : -1;
			//result = (kServerInfo1.bRunProcess && !kServerInfo2.bRunProcess) ? 1 : -1;
			break;
		case IP:
			result = _tcscmp(kServerInfo1.addrServerBind.StrIP().c_str(), kServerInfo2.addrServerBind.StrIP().c_str());
			break;
		case PORT:
			result = kServerInfo1.addrServerBind.wPort - kServerInfo2.addrServerBind.wPort;
			break;
		default :
			break;
		}
	}

	if (bComplementResult)
	{
		result = -result;
	}
*/
	return result;
}

bool CGeneralDlg::ServerRefresh(SLogCopyer::CONT_LIST_DUMP_INFO &rkContDumpInfo)
{
	m_kListCtrl.DeleteAllItems();
	SLogCopyer::CONT_LIST_DUMP_INFO::iterator ContDumpInfo_itor = rkContDumpInfo.begin();
	int iCount = 0;

	while(rkContDumpInfo.end() != ContDumpInfo_itor)
	{
		SLogCopyer::CONT_DUMP_INFO::iterator DumpInfo_Itor = (ContDumpInfo_itor->second).begin();
		while(ContDumpInfo_itor->second.end() != DumpInfo_Itor)
		{
			wchar_t wcString[1024] = {0, };
			short iContIdx = static_cast<short>(DumpInfo_Itor->first);

			int iImgIdx = 0;
			m_kListCtrl.InsertItem(LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, iCount, ServerConfigUtil::GetServerTypeName(DumpInfo_Itor->first).c_str(),
				INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, iImgIdx, (LPARAM)iContIdx );
			int iCol = 0;
			//! CheckBox
			m_kListCtrl.SetItemText(iCount, iCol++, _T("") );

			//! TypeName
			m_kListCtrl.SetItemText(iCount, iCol++, ServerConfigUtil::GetServerTypeName(DumpInfo_Itor->first).c_str());

			//! Machine IP
			m_kListCtrl.SetItemText(iCount, iCol++, ContDumpInfo_itor->first.c_str());

			//! Size / Count
			double dFileSize = 0.0f;
			int	iFileCount = 0;
			SLogCopyer::CONT_LOG_FILE_INFO kData;
			SLogCopyer::CONT_LOG_FILE_INFO::iterator FileInfo_Itor = DumpInfo_Itor->second.begin();
			while(DumpInfo_Itor->second.end() != FileInfo_Itor)
			{
				dFileSize += FileInfo_Itor->second;
				std::wstring wstrReName = ServerConfigUtil::GetServerTypeName(DumpInfo_Itor->first);
				wstrReName += L"\\";
				wstrReName += FileInfo_Itor->first;
				kData.insert(std::make_pair(wstrReName, FileInfo_Itor->second));

				++FileInfo_Itor;
				++iFileCount;
			}
			DumpInfo_Itor->second.swap(kData);

			std::wstring wstrSizeType = ::TansformFileSize(dFileSize);
			_stprintf_s(wcString, _countof(wcString), _T("%0.1f %s / %d Hit"), dFileSize, wstrSizeType.c_str(), iFileCount);

			m_kListCtrl.SetItemText(iCount, iCol++, wcString);
			++DumpInfo_Itor;
			++iCount;
		}
		++ContDumpInfo_itor;
	}

	return true;
}

bool CGeneralDlg::ServerRefresh(SLogCopyer::CONT_LOG_INFO const &rkContLogInfo, int iKeyValue)
{
	m_kListCtrl.DeleteAllItems();

	SLogCopyer::CONT_LOG_INFO::const_iterator LogInfo_itor = rkContLogInfo.begin();
	int iCount = static_cast<int>(rkContLogInfo.size());
	for(int i = 0; i < iCount; ++i, ++LogInfo_itor)
	{
		if( LogInfo_itor == rkContLogInfo.end() ) return false;

		wchar_t wcString[1024] = {0, };
		short iContIdx = static_cast<short>((*LogInfo_itor).first.nServerNo);

		int iImgIdx = 0;
		m_kListCtrl.InsertItem(LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, i, ServerConfigUtil::GetServerTypeName(LogInfo_itor->first.nServerType).c_str(),
									INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, iImgIdx, (LPARAM)iContIdx );
		//m_listServer.SetItemData(i, (LPARAM)iContIdx);
		
		int iCol = 0;
		//! CheckBox
		m_kListCtrl.SetItemText(i, iCol++, _T("") );

		if( iKeyValue == FT_CATION_HACK )
		{
			//! Time
			_ltot_s(LogInfo_itor->second.m_dwDate, wcString, 10);
			m_kListCtrl.SetItemText(i, iCol++, wcString);
		}
		else 
		{
			//! Channel
			_ltot_s(LogInfo_itor->first.nChannel, wcString, 10);
			m_kListCtrl.SetItemText(i, iCol++, wcString);

			//! TypeName
			m_kListCtrl.SetItemText(i, iCol++, ServerConfigUtil::GetServerTypeName(LogInfo_itor->first.nServerType).c_str());

			//! ServerNo
			_ltot_s(LogInfo_itor->first.nServerNo, wcString, 10);
			m_kListCtrl.SetItemText(i, iCol++, wcString);
		}

		//! [ Info Log ] Size / Count
		if(LogInfo_itor->second.m_dwSize1 == 0)
		{
			m_kListCtrl.SetItemText(i, iCol++, L" - ");
		}
		else
		{
			double dFileSize = static_cast<double>(LogInfo_itor->second.m_dwSize1);
			std::wstring wstrFileType = ::TansformFileSize(dFileSize);
			_stprintf_s(wcString, _countof(wcString), _T("%0.1f %s / %d Hit"), dFileSize, wstrFileType.c_str(), LogInfo_itor->second.m_dwCount1);

			m_kListCtrl.SetItemText(i, iCol++, wcString);
		}

		//! [ CEL Log ] Size / Count
		if(LogInfo_itor->second.m_dwSize2 == 0)
		{
			m_kListCtrl.SetItemText(i, iCol++, L" - ");
		}
		else
		{
			double dFileSize = static_cast<double>(LogInfo_itor->second.m_dwSize2);
			std::wstring wstrFileType = ::TansformFileSize(dFileSize);
			_stprintf_s(wcString, _countof(wcString), _T("%0.1f %s / %d Hit"), dFileSize, wstrFileType.c_str(), LogInfo_itor->second.m_dwCount2);
			m_kListCtrl.SetItemText(i, iCol++, wcString);
		}
	}
	m_kListCtrl.Invalidate();

	return true;
}

bool CGeneralDlg::ServerRefresh( CONT_SERVER_STATE_FOR_VIEW const & rkServerList)
{
	BM::CAutoMutex kLock( m_kMutex );

	// 선택 했던놈을 빽업 한다.
#ifdef USE_SELECT_BACKUP
	bool bSelected = false;
	int iSelected = m_kListCtrl.GetSelectionMark();
	SGameServerInfo kSelectedServer;
	if (iSelected != -1)
	{
		SGameServerInfo* pkSelectedServer = (int)m_kListCtrl.GetItemData(iSelected);
		memcpy_s(&kSelectedServer, sizeof(SGameServerInfo), pkSelectedServer, sizeof(SGameServerInfo));
		bSelected = true;
	}

	// 체크 했던놈을 빽업.
	std::vector<SGameServerInfo*> kCheckedList;
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; i++)
	{
		if (m_kListCtrl.GetCheck(i))
		{
			SGameServerInfo* pkServer = reinterpret_cast<SGameServerInfo*>( m_kListCtrl.GetItemData(i) );
			kCheckedList.push_back(pkServer);
		}
	}
#endif
	// Data Setting
	int iServerListCount = static_cast<int>(rkServerList.size());
	if( iServerListCount < m_kListCtrl.GetItemCount() )
	{
		for(int i = m_kListCtrl.GetItemCount(); i > iServerListCount; --i)
		{
			m_kListCtrl.DeleteItem(i - 1);
		}
	}

//	m_listServer.DeleteAllItems();
//	CONT_SERVER_STATE_FOR_VIEW rkServerList;
//	g_kGameServerMgr.GetServerList(rkServerList);

	CONT_SERVER_STATE_FOR_VIEW::const_iterator ServerList_itor = rkServerList.begin();
	for(int i = 0; i < iServerListCount; ++i, ++ServerList_itor)
	{
		if( ServerList_itor == rkServerList.end() )
		{
			assert(0);
		}
		CONT_SERVER_STATE_FOR_VIEW::mapped_type const& rkServerState = (*ServerList_itor).second;

		wchar_t wcString[1024] = {0, };
		const int iContIdx = (int)((*ServerList_itor).first);

		if( i >= m_kListCtrl.GetItemCount() )
		{
			int iImgIdx = 0;
			m_kListCtrl.InsertItem(LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, i, rkServerState.kTbl.strName.c_str(),
									INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, iImgIdx, (LPARAM)iContIdx );
		}
		else
		{
			m_kListCtrl.SetItemData(i, (LPARAM)iContIdx);
		}

		int iCol = 0;
		m_kListCtrl.SetItemText(i, iCol++, _T("") );
		m_kListCtrl.SetItemText(i, iCol++, rkServerState.kTbl.strName.c_str());
		m_kListCtrl.SetItemText(i, iCol++, ServerConfigUtil::GetServerTypeName(rkServerState.kTbl.nServerType).c_str());

		if ( true == g_kGameServerMgr.bControlLevel() )
		{
			m_kListCtrl.SetItemText(i, iCol++, rkServerState.kTbl.strMachinIP.c_str());
		}
		else
		{
			m_kListCtrl.SetItemText(i, iCol++, L"UnKnown" );
		}
		
		_ltot_s(rkServerState.kTbl.nServerNo, wcString, 10);
		m_kListCtrl.SetItemText(i, iCol++, wcString);

		_ltot_s(rkServerState.kTbl.nChannel, wcString, 10);
		m_kListCtrl.SetItemText(i, iCol++, wcString);

		wcscpy_s(wcString, rkServerState.bReadyToService ? L"O\0": L"X\0");
		m_kListCtrl.SetItemText(i, iCol++, wcString);

		wcscpy_s(wcString, rkServerState.kProcessInfo.IsCorrect() ? L"O\0": L"X\0");
		m_kListCtrl.SetItemText(i, iCol++, wcString);

		if ( true == g_kGameServerMgr.bControlLevel() )
		{
			if(rkServerState.kTbl.addrUserBind.ip.S_un.S_addr)
			{
				m_kListCtrl.SetItemText(i, iCol++, (LPCTSTR)rkServerState.kTbl.addrUserBind.ToString().c_str());
			}
			else
			{//(rkServerState.kTbl.addrServerBind.ip.S_un.S_addr)
				m_kListCtrl.SetItemText(i, iCol++, (LPCTSTR)rkServerState.kTbl.addrServerBind.ToString().c_str());
			}
		}
		else
		{
			m_kListCtrl.SetItemText(i, iCol++, L"UnKnown" );
		}
		
		wchar_t wcString2[1024] = {0,};

		switch(rkServerState.kTbl.nServerType)
		{
		case CEL::ST_CONTENTS:
		case CEL::ST_CENTER:
			{
				_itot_s(rkServerState.uiConnectionUsers, wcString, 10);
				_itot_s(rkServerState.uiMaxUsers, wcString2, 10);
			}break;
		case CEL::ST_SWITCH:
		case CEL::ST_LOGIN:
		case CEL::ST_MAP:
		case CEL::ST_IMMIGRATION:
		case CEL::ST_CONSENT:
			{
				_itot_s(rkServerState.uiConnectionUsers, wcString, 10);
				wcscpy_s(wcString2, 1024, L"-");
			}break;
		default:
			{
				wcscpy_s(wcString, 1024, L"-");
				wcscpy_s(wcString2, 1024, L"-");
			}break;
		}

		m_kListCtrl.SetItemText(i, iCol++, wcString);
		m_kListCtrl.SetItemText(i, iCol++, wcString2);
		m_kListCtrl.SetItemText(i, iCol++, wcString2);//Version
	}
	m_kListCtrl.Invalidate();

	return true;
}

void CGeneralDlg::OnBnClickedBtnRefresh()
{
	EDisplayMode eMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
	switch( eMode )
	{
	case DM_LOG_INFO:
		{
			//! Refresh Log Info
			STreeData kTreeData;
			if(!GetTreeData(kTreeData, DM_LOG_INFO))
			{
				return;
			}
			g_kServerCmdMgr.OnRefreshLog(kTreeData.nPriKey, kTreeData.nSecKey);
		}break;
	case DM_CMD_HISTORY:
		{//! Refresh Command History
		}break;
	default :
		{//! Refresh Server State
			g_kServerCmdMgr.OnCommand(MCT_REFRESH_STATE);
		}break;
	}
}

void CGeneralDlg::OnNMCustomdrawListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	DWORD dwItem = (DWORD)pLVCD->nmcd.dwItemSpec;

	*pResult = 0;

    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
    {
	    *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
    {
		COLORREF crTextBG = RGB(255,255,255);
	    COLORREF crText = RGB(0,0,0);
	
		int iServerIdx = (int)pLVCD->nmcd.lItemlParam;
		EDisplayMode eCurMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
		switch( eCurMode )
		{
		case DM_LOG_INFO:
			{
				STreeData kTreeData;
				if(!GetTreeData(kTreeData, DM_LOG_INFO)) return;
				typedef std::vector <CString> CONT_ITEM_SIZE;
				CONT_ITEM_SIZE kContItemSize;
				if(kTreeData.nPriKey == TT_SITE_ROOT && kTreeData.nSecKey == FT_CATION_HACK)
				{
					kContItemSize.push_back(m_kListCtrl.GetItemText(dwItem, 2));
					kContItemSize.push_back(m_kListCtrl.GetItemText(dwItem, 3));
				}
				else if(kTreeData.nPriKey == TT_SITE_ROOT && kTreeData.nSecKey == FT_DUMP)
				{
					kContItemSize.push_back(m_kListCtrl.GetItemText(dwItem, 3));
				}
				else if(kTreeData.nPriKey == TT_REALM_ROOT)
				{
					kContItemSize.push_back(m_kListCtrl.GetItemText(dwItem, 4));
					kContItemSize.push_back(m_kListCtrl.GetItemText(dwItem, 5));
					kContItemSize.push_back(m_kListCtrl.GetItemText(dwItem, 6));
				}

				int sizeType = 0;
				for(int i=0; i<kContItemSize.size(); ++i)
				{
					if(-1 != kContItemSize[i].Find(L"GB"))
					{
						crTextBG = RGB(255,0,0);//Red
						crText = RGB(255,255,255);//white
						sizeType = 3;
					}
					else if(-1 != kContItemSize[i].Find(L"MB") && sizeType < 3)
					{
						crTextBG = RGB(255,255,0); //Yellow
						crText = RGB(0,0,0);//Black
						sizeType = 2;
					}
					else if(sizeType == 0)
					{
						crTextBG = RGB(255,255,255); //White
						crText = RGB(0,0,0);//Black
						sizeType = 1;
					}
				}

				kContItemSize.clear();
			}break;
		case DM_CMD_HISTORY:
			{
				crTextBG = RGB(255,255,255); //White
				crText = RGB(0,0,0);//Black
			}break;
		case DM_SERVER_STATE:
			{
				CONT_SERVER_STATE_EX::mapped_type kServerInfo;
				if (g_kGameServerMgr.GetServerInfo( m_iSiteNo, iServerIdx, kServerInfo))
				{
					if (	CEL::ST_SUB_MACHINE_CONTROL == kServerInfo.kTbl.nServerType
						||	g_kGameServerMgr.IsAliveSMC(kServerInfo.kTbl.strMachinIP)
						)
					{
						if( kServerInfo.bIsException || kServerInfo.bTickWarning )//Critical
						{//시간 오버나 뻑.
							crTextBG = RGB(99,99,99);//Silver
							crText = RGB(255,255,0); //Yellow
						}
						else if( kServerInfo.kProcessInfo.IsCorrect() && kServerInfo.bReadyToService )
						{//서비스 준비 완료
							crTextBG = RGB(255,255,255); //White
							crText = RGB(0,0,0);//Black
						}
						else if( kServerInfo.kProcessInfo.IsCorrect() || kServerInfo.bReadyToService )
						{//잘못 됐단거 같은데?
							crTextBG = RGB(255,255,0); //Yellow
							crText = RGB(0,0,0);//Black
						}
						else
						{	//서버 작동 가능하고 아무 서버도 안떠있는 기본 상태
							crTextBG = RGB(255,0,0);//Red
							crText = RGB(255,255,255);//white
						}
					}
					else
					{
						crTextBG = RGB(128,0,0);//Dark Red
						crText = RGB(255,255,255);//white
					}
				}
			}break;
		}

		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crTextBG;
		*pResult = CDRF_DODEFAULT;
    }
}

void CGeneralDlg::OnBnClickedBtnConnect()
{
	CallLoginDlg();
}

void CGeneralDlg::OnBnClickedBtnClose()
{
	g_kNetwork.DisConnectServer();
	m_kListCtrl.DeleteAllItems();
}

bool CGeneralDlg::GetCheckedList(int iKeyValue, CONT_SERVER_STRING& rkOutText, SLogCopyer::CONT_LOG_INFO& rkContLogInfo)
{
	bool bRet = false;
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; ++i)
	{
		if (m_kListCtrl.GetCheck(i))
		{
			BM::vstring vstrItemText; 
			vstrItemText = m_kListCtrl.GetItemText(i, 1).GetString();
			vstrItemText += L" / ";
			vstrItemText += m_kListCtrl.GetItemText(i, 2).GetString();
			vstrItemText += L" / ";
			vstrItemText += m_kListCtrl.GetItemText(i, 3).GetString();

			short iServerNo = static_cast<short>(m_kListCtrl.GetItemData(i));
			g_kLogCopyMgr.Locked_GetLogInfo(iKeyValue, iServerNo, rkContLogInfo);
			rkOutText.push_back(vstrItemText);

			bRet = true;
		}
	}

	return bRet;
}

bool CGeneralDlg::GetCheckedList(int iKeyValue, CONT_SERVER_STRING& rkOutText, SLogCopyer::CONT_DUMP_INFO& rkContDumpInfo)
{
	bool bRet = false;
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; ++i)
	{
		if (m_kListCtrl.GetCheck(i))
		{
			BM::vstring vstrItemText; 
			vstrItemText = m_kListCtrl.GetItemText(i, 1).GetString();
			vstrItemText += L" / ";
			vstrItemText += m_kListCtrl.GetItemText(i, 2).GetString();
			vstrItemText += L" / ";
			vstrItemText += m_kListCtrl.GetItemText(i, 3).GetString();

			short iServerType = static_cast<short>(m_kListCtrl.GetItemData(i));
			std::wstring wstrAddr = m_kListCtrl.GetItemText(i, 2).GetString();
			g_kLogCopyMgr.Locked_GetDumpInfo(wstrAddr, iServerType, rkContDumpInfo);
			rkOutText.push_back(vstrItemText);

			bRet = true;
		}
	}

	return bRet;
}


void CGeneralDlg::GetCheckedList(ContServerID& rkOutServerID, CONT_SERVER_STRING& rkOutText)const
{
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; i++)
	{
		if (m_kListCtrl.GetCheck(i))
		{
			int iServerIdx = (int)m_kListCtrl.GetItemData(i);
			CONT_SERVER_STATE_EX::mapped_type kServerInfo;
			if (g_kGameServerMgr.GetServerInfo( m_iSiteNo, iServerIdx, kServerInfo))
			{
				BM::vstring vstr;
				vstr << L"[R" << kServerInfo.kTbl.nRealm << L"C" << kServerInfo.kTbl.nChannel << L"] " << kServerInfo.kTbl.nServerNo << L"." << kServerInfo.kTbl.strName;
				rkOutText.push_back(vstr);

				SERVER_IDENTITY kCheckedServer = kServerInfo.kTbl;
				rkOutServerID.push_back(kCheckedServer);
			}
		}
	}
}

void CGeneralDlg::OnBnClickedBtnServerOn()
{
	ContServerID kContCheckedResult;
	CONT_SERVER_STRING kContServerString;

	GetCheckedList(kContCheckedResult, kContServerString);
	std::sort( kContCheckedResult.begin(), kContCheckedResult.end(), ContServerIDSort() );
	if( !kContCheckedResult.empty() )
	{
		g_kServerCmdMgr.OnCommand(MCT_SERVER_ON, kContCheckedResult, kContServerString);
	}
	else
	{
		if( g_kServerCmdMgr.OnCommand(MCT_SERVER_ON) )
		{
			m_kProgressJob.SetRange(1, g_kServerCmdMgr.GetMaxIndex());
		}
		else
		{
			//실패했다.
		}
	}
}

void CGeneralDlg::OnBnClickedBtnServerOff()
{
	ContServerID kContCheckedResult;
	CONT_SERVER_STRING kContServerString;
	GetCheckedList( kContCheckedResult, kContServerString);
	std::sort( kContCheckedResult.begin(), kContCheckedResult.end(), ContServerIDSort() );
	if( !kContCheckedResult.empty() )
	{
		g_kServerCmdMgr.OnCommand(MCT_SERVER_OFF, kContCheckedResult, kContServerString);
	}
	else
	{
		if( g_kServerCmdMgr.OnCommand(MCT_SERVER_OFF) )
		{
			m_kProgressJob.SetRange(1, g_kServerCmdMgr.GetMaxIndex());
		}
		else
		{
			//실패했다.
		}
	}
}

void CGeneralDlg::OnBnClickedBtnProcesskill()
{
	m_kCheckBtnKillServer.SetCheck(FALSE);
	m_kBtnKillServer.EnableWindow(FALSE);
	m_kCheckBtnReSync.SetCheck(FALSE);
	m_kBtnReSync.EnableWindow(FALSE);

	ContServerID kContCheckedResult;
	CONT_SERVER_STRING kContServerString;
	GetCheckedList(kContCheckedResult, kContServerString);
	std::sort( kContCheckedResult.begin(), kContCheckedResult.end(), ContServerIDSort() );
	if( !kContCheckedResult.empty() )
	{
		if( g_kServerCmdMgr.OnCommand(MCT_SERVER_SHUTDOWN, kContCheckedResult, kContServerString) )
		{
			for (int i=0 ; i<m_kListCtrl.GetItemCount() ; i++)
			{
				m_kListCtrl.SetCheck(i, false);
			}
		}
	}
}

void CGeneralDlg::OnBnClickedCheckUsebeep()
{
	//g_kGameServerMgr.m_bUseBeep = (bool)IsDlgButtonChecked(IDC_CHECK_USEBEEP);
}

void CGeneralDlg::OnBnClickedBtnLogClear()
{
	CString kString("");
	SetDlgItemText(IDC_EDIT_LOG, kString);
}

void CGeneralDlg::OnBnClickedConfirmKill()
{
	m_kBtnKillServer.EnableWindow(m_kCheckBtnKillServer.GetCheck());
}

bool CGeneralDlg::IsDisplay( CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement, EDisplayType const kDisplayType )const
{
	switch ( kDisplayType )
	{
	case DT_OPEN:
		{
			return kElement.bReadyToService && kElement.kProcessInfo.IsCorrect();
		}break;
	case DT_CLOSE:
		{
			return !kElement.bReadyToService && !kElement.kProcessInfo.IsCorrect();
		}break;
	case DT_ALERT:
		{
			return kElement.bReadyToService != kElement.kProcessInfo.IsCorrect();
		}break;
	case DT_ALL:
	default:
		{

		}break;
	}
	return true;
}

void CGeneralDlg::OnFileExit()
{
	g_kNetwork.DisConnectServer();
	SendMessage(WM_CLOSE, 0, 0);
}

void CGeneralDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 별로 이런거 안보여줘도 될듯..-_-...
	//CBitmap kNewBitMap;
	//bool const bLoadBitmap = kNewBitMap.LoadBitmapW(MAKEINTRESOURCE(IDB_BITMAP1));
	//if( !bLoadBitmap )
	//{
	//	MessageBox(L"Can't Load Bitmap");
	//}

	//CDC kBitMapDC;
	//bool const bCreateDC = kBitMapDC.CreateCompatibleDC(&dc);
	//if( !bCreateDC )
	//{
	//	MessageBox(L"Can't Create DC");
	//}
	//CBitmap* pkOldMap = kBitMapDC.SelectObject(&kNewBitMap);
	//BitBlt(dc, 840, 540, 400, 90, kBitMapDC, 0, 0, SRCCOPY);

	//kBitMapDC.SelectObject(pkOldMap);

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

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGeneralDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGeneralDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CGeneralDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//PostQuitMessage(0);
	CDialog::OnClose();
	g_kNetwork.IsServiceStart(false);
}

void CGeneralDlg::InvalidateServerStateToTreeView()
{
	//먼저 최상위 루트를 만들어주자
	
	typedef std::map< int, HTREEITEM > CONT_REALM_TREE_HANDLE;//RealmNo
	typedef std::map< STreeData, HTREEITEM > CONT_CHANNEL_TREE_HANDLE;//SI
	typedef std::map< std::wstring, HTREEITEM > CONT_MACHINE_TREE_HANDLE;//IP
	CONT_REALM_TREE_HANDLE kRealmTree;
	CONT_CHANNEL_TREE_HANDLE kChannelTree;

//렐름, 채널루트 
//>>
{
	BM::CAutoMutex kLock( g_kGameServerMgr.GetLock() );
	if ( g_kGameServerMgr.GetConsentSite() > 0 )
	{
		TV_INSERTSTRUCT kTvstruct;
		kTvstruct.hParent = 0;
		kTvstruct.hInsertAfter = TVI_LAST;
		kTvstruct.item.pszText = _T("Consent");
		kTvstruct.item.iImage = -1;
		kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;
		HTREEITEM hSiteRoot = m_kTreeCtrl.InsertItem(&kTvstruct);
		m_kTreeCtrl.SetItemData(hSiteRoot, (DWORD)STreeData( TT_CONSENT_ROOT, static_cast<short>(g_kGameServerMgr.GetConsentSite()) ));
	}

	CONT_CONTROL_INFO::const_iterator ControlInfo_itor = g_kGameServerMgr.GetControlInfo_Begin();
	for ( ; ControlInfo_itor != g_kGameServerMgr.GetControlInfo_End() ; ++ControlInfo_itor )
	{
		BM::vstring vstr( L"Site_" );
		vstr += ControlInfo_itor->first;

		TV_INSERTSTRUCT kTvstruct;
		kTvstruct.hParent = 0;
		kTvstruct.hInsertAfter = TVI_LAST;
		kTvstruct.item.pszText = (LPWSTR)vstr.operator wchar_t const*();
		kTvstruct.item.iImage = -1;
		kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;
		HTREEITEM hSiteRoot = m_kTreeCtrl.InsertItem(&kTvstruct);
		m_kTreeCtrl.SetItemData(hSiteRoot, (DWORD)STreeData( TT_SITE_ROOT, static_cast<short>(ControlInfo_itor->first) ));

		CONT_REALM kRealm;
		ControlInfo_itor->second.kRealmMgr.GetRealmCont( kRealm );

		CONT_REALM::iterator Realm_Itor = kRealm.begin();
		for ( ; Realm_Itor != kRealm.end() ; ++Realm_Itor )
		{
			CONT_REALM::key_type kRealmNo = Realm_Itor->first;
			CONT_REALM::mapped_type kElement = Realm_Itor->second;

			//렐름 셋팅
			kTvstruct.hParent = hSiteRoot;
			kTvstruct.hInsertAfter = TVI_LAST;
			kTvstruct.item.pszText = (LPWSTR)kElement.Name().c_str();
			kTvstruct.item.iImage = kRealmNo;
			kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;

			HTREEITEM const hRealm = m_kTreeCtrl.InsertItem(&kTvstruct);
			if(hRealm)
			{//렐름 루트
				m_kTreeCtrl.SetItemData(hRealm, (DWORD)STreeData(TT_REALM_ROOT, kRealmNo) );//숫자 셋팅
				//kRealmTree.insert(std::make_pair(kRealmNo, hRealm));
			}

			//채널 셋팅 
			CONT_CHANNEL kContChannel;
			kElement.GetChannelCont(kContChannel);

			CONT_CHANNEL::const_iterator channel_itor = kContChannel.begin();
			for( ; channel_itor != kContChannel.end() ; ++channel_itor )
			{
				CONT_CHANNEL::key_type const nChannelNo = channel_itor->first;
				kTvstruct.hParent = hRealm;
				kTvstruct.hInsertAfter = TVI_LAST;
				kTvstruct.item.pszText = (LPWSTR)channel_itor->second.ChannelName().c_str();
				kTvstruct.item.iImage = nChannelNo;
				kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;

				HTREEITEM const hChannel = m_kTreeCtrl.InsertItem(&kTvstruct);
				if(hChannel)
				{//채널 루트
					m_kTreeCtrl.SetItemData(hChannel, (DWORD)STreeData(kRealmNo, nChannelNo) );//숫자 셋팅
					/*
					SERVER_IDENTITY kSI;
					kSI.nRealm = kRealmNo;
					kSI.nChannel = nChannelNo;
					kChannelTree.insert(std::make_pair(kSI, hChannel));
					*/
				}
			}

			//퍼블릭 셋팅
			if(kRealmNo > 0)
			{
				kTvstruct.hParent = hRealm;
				kTvstruct.hInsertAfter = TVI_LAST;
				kTvstruct.item.pszText = _T("Public");
				kTvstruct.item.iImage = 0;//이게 클릭시 타입인듯
				kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;

				HTREEITEM const hPublicChannel = m_kTreeCtrl.InsertItem(&kTvstruct);
				if(hPublicChannel)
				{//채널이 없으면 퍼블릭 루트
					m_kTreeCtrl.SetItemData(hPublicChannel, (DWORD)STreeData(kRealmNo, 0));
				}
			}
			else
			{
				kTvstruct.hParent = hRealm;
				kTvstruct.hInsertAfter = TVI_LAST;
				kTvstruct.item.pszText = _T("SMC");
				kTvstruct.item.iImage = -1;
				kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;
				HTREEITEM const hSMCRoot = m_kTreeCtrl.InsertItem(&kTvstruct);
				m_kTreeCtrl.SetItemData(hSMCRoot, (DWORD)STreeData(TT_SMC_ROOT, kRealmNo));
			}
		}
	}
}

}

void CGeneralDlg::InvalidateLogInfoToTreeView()
{
	//먼저 최상위 루트를 만들어주자
	
	typedef std::map< int, HTREEITEM > CONT_REALM_TREE_HANDLE;//RealmNo
	typedef std::map< STreeData, HTREEITEM > CONT_CHANNEL_TREE_HANDLE;//SI
	typedef std::map< std::wstring, HTREEITEM > CONT_MACHINE_TREE_HANDLE;//IP
	CONT_REALM_TREE_HANDLE kRealmTree;
	CONT_CHANNEL_TREE_HANDLE kChannelTree;

//렐름, 채널루트 
//>>
{
	BM::CAutoMutex kLock( g_kGameServerMgr.GetLock() );

	HTREEITEM hSiteRoot ;
	CONT_CONTROL_INFO::const_iterator ControlInfo_itor = g_kGameServerMgr.GetControlInfo_Begin();
	for ( ; ControlInfo_itor != g_kGameServerMgr.GetControlInfo_End() ; ++ControlInfo_itor )
	{
		BM::vstring vstr( L"Site_" );
		vstr += ControlInfo_itor->first;

		TV_INSERTSTRUCT kTvstruct;
		kTvstruct.hParent = 0;
		kTvstruct.hInsertAfter = TVI_LAST;
		kTvstruct.item.pszText = (LPWSTR)vstr.operator wchar_t const*();
		kTvstruct.item.iImage = FT_CATION_HACK;
		kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;
		/*HTREEITEM */hSiteRoot = m_kTreeCtrl.InsertItem(&kTvstruct);
		m_kTreeCtrl.SetItemData(hSiteRoot, (DWORD)STreeData( TT_SITE_ROOT, static_cast<short>(ControlInfo_itor->first) ));

		CONT_REALM kRealm;
		ControlInfo_itor->second.kRealmMgr.GetRealmCont( kRealm );

		kTvstruct.hParent = hSiteRoot;
		kTvstruct.hInsertAfter = TVI_LAST;
		kTvstruct.item.pszText = L"Crash Dump";
		kTvstruct.item.iImage = FT_DUMP;
		kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;

		HTREEITEM hItem = m_kTreeCtrl.InsertItem(&kTvstruct);
		if(hItem) m_kTreeCtrl.SetItemData(hItem, (DWORD)STreeData(TT_SITE_ROOT, FT_DUMP) );//숫자 셋팅

		kTvstruct.hParent = hSiteRoot;
		kTvstruct.hInsertAfter = TVI_LAST;
		kTvstruct.item.pszText = L"Caution/Hack Log";
		kTvstruct.item.iImage = FT_CATION_HACK;
		kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;

		hItem = m_kTreeCtrl.InsertItem(&kTvstruct);
		if(hItem) m_kTreeCtrl.SetItemData(hItem, (DWORD)STreeData(TT_SITE_ROOT, FT_CATION_HACK) );//숫자 셋팅

		std::wstring wstrName;
		CONT_REALM::iterator Realm_Itor = kRealm.begin();
		for ( ; Realm_Itor!=kRealm.end() ; ++Realm_Itor )
		{
			CONT_REALM::key_type kRealmNo = Realm_Itor->first;
			CONT_REALM::mapped_type kElement = Realm_Itor->second;

			
			if(Realm_Itor == kRealm.begin())
			{
				wstrName = L"Public Realm Log";
			}
			else
			{
				wstrName = kElement.Name() + L" Log";
			}

			//렐름 셋팅
			kTvstruct.hParent = hSiteRoot;
			kTvstruct.hInsertAfter = TVI_LAST;
			kTvstruct.item.pszText = (LPWSTR)wstrName.c_str();
			kTvstruct.item.iImage = kRealmNo;
			kTvstruct.item.mask = TVIF_TEXT | TVIF_IMAGE;

			HTREEITEM const hRealm = m_kTreeCtrl.InsertItem(&kTvstruct);
			if(hRealm)
			{//렐름 루트
				m_kTreeCtrl.SetItemData(hRealm, (DWORD)STreeData(TT_REALM_ROOT, kRealmNo) );//숫자 셋팅
				//kRealmTree.insert(std::make_pair(kRealmNo, hRealm));
			}
		}
	}
}
}

void CGeneralDlg::InvalidateTreeView(EDisplayMode eMode)
{
	EDisplayMode eCurMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
	if(eCurMode != eMode)
	{
		return;
	}

	//초기화
	m_kTreeCtrl.DeleteAllItems();

	//! 현재 활성화된 탭으로 업데이트
	if(eCurMode == DM_LOG_INFO)
	{//! Log
		InvalidateLogInfoToTreeView();
	}
	else if(eCurMode == DM_CMD_HISTORY)
	{//! History
	}
	else
	{//! State
		InvalidateServerStateToTreeView();
	}
}

bool CGeneralDlg::GetTreeData(STreeData& kTreeData, EDisplayMode eMode)
{
	HTREEITEM hSelectTreeItem = NULL;
	HTREEITEM hLastSelectTreeItem = NULL;
	if(eMode == DM_SERVER_STATE)
	{
		hSelectTreeItem = m_hStateSelectTreeItem;
		hLastSelectTreeItem = m_hStateLastSelectTreeItem;
	}
	else if(eMode == DM_LOG_INFO)
	{
		hSelectTreeItem = m_hLogSelectTreeItem;
		hLastSelectTreeItem = m_hLogLastSelectTreeItem;
	}

	hSelectTreeItem = m_kTreeCtrl.GetSelectedItem();

	if( NULL == hSelectTreeItem )
	{
		if( NULL == hLastSelectTreeItem )
		{
			return false;
		}
		else
		{
			hSelectTreeItem = hLastSelectTreeItem;
		}
	}
	else
	{
		hLastSelectTreeItem = hSelectTreeItem;
	}

	HTREEITEM hParent = hSelectTreeItem;
	HTREEITEM hParentOfParent = m_kTreeCtrl.GetParentItem( hSelectTreeItem );
	while ( hParentOfParent )
	{
		hParent = hParentOfParent;
		hParentOfParent = m_kTreeCtrl.GetParentItem( hParent );
	}

	{
		STreeData kTreeData( (DWORD)m_kTreeCtrl.GetItemData(hParent) );
		m_iSiteNo = static_cast<int>( kTreeData.nSecKey );
	}

	kTreeData = ((DWORD)m_kTreeCtrl.GetItemData(hSelectTreeItem));

	if(eMode == DM_SERVER_STATE)
	{
		m_hStateSelectTreeItem = hSelectTreeItem;
		m_hStateLastSelectTreeItem = hLastSelectTreeItem;
	}
	else if(eMode == DM_LOG_INFO)
	{
		m_hLogSelectTreeItem = hSelectTreeItem;
		m_hLogLastSelectTreeItem = hLastSelectTreeItem;
	}
	return true;
}
void CGeneralDlg::InvalidateServerStateToListView(STreeData& rkTreeData)
{
	m_kServerStateInfo.Reset();

	CONT_SERVER_STATE_FOR_VIEW kServerList;
	g_kGameServerMgr.GetServerList( m_iSiteNo, kServerList );//다 갖고 와서.

	EDisplayType kDisplayType = DT_ALL;
	if ( TRUE == m_kRadioBtn2.GetCheck() )
	{
		kDisplayType = DT_OPEN;
	}
	else if ( TRUE == m_kRadioBtn3.GetCheck() )
	{
		kDisplayType = DT_CLOSE;
	}
	else if ( TRUE == m_kRadioBtn4.GetCheck() )
	{
		kDisplayType = DT_ALERT;
	}

	BM::vstring kStrCCU("All Server = ");
	kStrCCU << m_kServerStateInfo.m_iAll;
	kStrCCU << L" / Open Server = ";
	kStrCCU << m_kServerStateInfo.m_iOpen;
	kStrCCU << L" / Close Server = ";
	kStrCCU << m_kServerStateInfo.m_iClose;
	kStrCCU << L" / Alert Server = ";
	kStrCCU << m_kServerStateInfo.m_iAlert;
	kStrCCU << L" / Site CCU = ";
	kStrCCU << g_kGameServerMgr.GetCCU(CEL::ST_IMMIGRATION);

	switch( rkTreeData.nPriKey )//루트 렐름
	{
	case TT_SITE_ROOT:
		{
			CONT_SERVER_STATE_FOR_VIEW::iterator ServerList_Itor = kServerList.begin();
			while ( ServerList_Itor != kServerList.end() )
			{
				CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = ServerList_Itor->second;
				m_kServerStateInfo.AddInfo(kElement);
				if(		kElement.kTbl.nServerType == CEL::ST_MACHINE_CONTROL
					||	kElement.kTbl.nServerType == CEL::ST_SUB_MACHINE_CONTROL
					||	kElement.kTbl.nServerType == CEL::ST_CONSENT
					)
				{
					ServerList_Itor = kServerList.erase( ServerList_Itor );
				}
				else
				{
					if ( true == IsDisplay( kElement, kDisplayType ) )
					{
						++ServerList_Itor;
					}
					else
					{
						ServerList_Itor = kServerList.erase( ServerList_Itor );
					}
				}
			}
		}break;
	case TT_REALM_ROOT:
		{
			CONT_SERVER_STATE_FOR_VIEW::iterator ServerList_Itor = kServerList.begin();
			while ( ServerList_Itor != kServerList.end() )
			{
				CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = ServerList_Itor->second;
				m_kServerStateInfo.AddInfo(kElement);
				if(		kElement.kTbl.nRealm != rkTreeData.nSecKey
					||	kElement.kTbl.nServerType == CEL::ST_MACHINE_CONTROL
					||	kElement.kTbl.nServerType == CEL::ST_SUB_MACHINE_CONTROL
					||	kElement.kTbl.nServerType == CEL::ST_CONSENT
					)
				{
					ServerList_Itor = kServerList.erase( ServerList_Itor );
				}
				else
				{
					if ( true == IsDisplay( kElement, kDisplayType ) )
					{
						++ServerList_Itor;
					}
					else
					{
						ServerList_Itor = kServerList.erase( ServerList_Itor );
					}
				}
			}	
			kStrCCU << L" / Realm CCU = ";
			kStrCCU << g_kGameServerMgr.GetCCU(CEL::ST_CONTENTS, 1, rkTreeData.nSecKey);
		}break;
	case TT_SMC_ROOT:
		{
			CONT_SERVER_STATE_FOR_VIEW::iterator ServerList_Itor = kServerList.begin();
			while ( ServerList_Itor != kServerList.end() )
			{
				CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = ServerList_Itor->second;
				m_kServerStateInfo.AddInfo(kElement);
				if(		kElement.kTbl.nRealm != rkTreeData.nSecKey
					||	kElement.kTbl.nServerType != CEL::ST_SUB_MACHINE_CONTROL )
				{
					ServerList_Itor = kServerList.erase( ServerList_Itor );
				}
				else
				{
					if ( true == IsDisplay( kElement, kDisplayType ) )
					{
						++ServerList_Itor;
					}
					else
					{
						ServerList_Itor = kServerList.erase( ServerList_Itor );
					}
				}
			}
		}break;
	case TT_CONSENT_ROOT:
		{
			CONT_SERVER_STATE_FOR_VIEW::iterator ServerList_Itor = kServerList.begin();
			while ( ServerList_Itor != kServerList.end() )
			{
				CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = ServerList_Itor->second;
				m_kServerStateInfo.AddInfo(kElement);
				if(	kElement.kTbl.nServerType != CEL::ST_CONSENT )
				{
					ServerList_Itor = kServerList.erase( ServerList_Itor );
				}
				else
				{
					if ( true == IsDisplay( kElement, kDisplayType ) )
					{
						++ServerList_Itor;
					}
					else
					{
						ServerList_Itor = kServerList.erase( ServerList_Itor );
					}
				}
			}
		}break;
	default://첫 키가 양수면 렐름 번호.
		{
			CONT_SERVER_STATE_FOR_VIEW::iterator ServerList_Itor = kServerList.begin();
			while ( ServerList_Itor != kServerList.end() )
			{
				CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = ServerList_Itor->second;
				m_kServerStateInfo.AddInfo(kElement);
				if(		kElement.kTbl.nRealm != rkTreeData.nPriKey
					||	kElement.kTbl.nChannel != rkTreeData.nSecKey
					||	kElement.kTbl.nServerType == CEL::ST_MACHINE_CONTROL
					||	kElement.kTbl.nServerType == CEL::ST_SUB_MACHINE_CONTROL
					||	kElement.kTbl.nServerType == CEL::ST_CONSENT
					/*||	kElement.kTbl.nServerType == CEL::ST_CONTENTS*/
					)
				{
					ServerList_Itor = kServerList.erase( ServerList_Itor );
				}
				else
				{
					if ( true == IsDisplay( kElement, kDisplayType ) )
					{
						++ServerList_Itor;
					}
					else
					{
						ServerList_Itor = kServerList.erase( ServerList_Itor );
					}
				}
			}
			kStrCCU << L" / Realm CCU = ";
			kStrCCU << g_kGameServerMgr.GetCCU(CEL::ST_CONTENTS, 1, rkTreeData.nPriKey);
			kStrCCU << L" / Channel CCU = ";
			kStrCCU << g_kGameServerMgr.GetCCU(CEL::ST_CENTER, 1, rkTreeData.nPriKey, rkTreeData.nSecKey);
		}break;
	}

	m_kStrCCU = kStrCCU;
	UpdateData(false);
	ServerRefresh(kServerList);
}
void CGeneralDlg::InvalidateLogInfoToListView(STreeData& rkTreeData)
{
	int iKeyValue = 0;

	if( (rkTreeData.nPriKey == TT_SITE_ROOT && rkTreeData.nSecKey == FT_CATION_HACK) 
		|| (rkTreeData.nPriKey == TT_REALM_ROOT) )
	{//! 로그 파일 뷰
		iKeyValue = rkTreeData.nSecKey;
		SLogCopyer::CONT_LOG_INFO kContLogInfo;
		if(g_kLogCopyMgr.Locked_GetLogInfo(iKeyValue, kContLogInfo))
		{
			ServerRefresh(kContLogInfo, iKeyValue);
		}
		else
		{//! 데이터가 없다.
			m_kListCtrl.DeleteAllItems();
			return;
		}
	}
	else if(rkTreeData.nPriKey == TT_SITE_ROOT && rkTreeData.nSecKey == FT_DUMP) 
	{//! 덤프 파일 뷰
		SLogCopyer::CONT_LIST_DUMP_INFO kContListDumpInfo;
		if(g_kLogCopyMgr.Locked_GetDumpInfo(kContListDumpInfo))
		{
			ServerRefresh(kContListDumpInfo);
			g_kLogCopyMgr.Locked_SetDumpInfo(kContListDumpInfo);
		}
		else
		{//! 데이터가 없다.
			m_kListCtrl.DeleteAllItems();
			return;
		}
	}
}

void CGeneralDlg::InvalidateListView(bool bRecvData, EDisplayMode eMode)
{
	EDisplayMode eCurMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());

	STreeData kTreeData;
	if(!GetTreeData(kTreeData, eCurMode))
	{
		return;
	}

	if(bRecvData && eMode != eCurMode)
	{//! 데이터를 받았지만, 모드가 현재 활성화된 탭과 다르다면, 업데이트 하지 않음.
		return;
	}
	
	m_kStrCCU = "";
	//! 현재 활성화된 탭으로 업데이트
	if(eCurMode == DM_LOG_INFO)
	{
		InvalidateLogInfoToListView(kTreeData);
	}
	else if(eCurMode == DM_CMD_HISTORY)
	{
		m_kListCtrl.DeleteAllItems();
	}
	else
	{
		InvalidateServerStateToListView(kTreeData);
	}
	UpdateData(false);
}

void CGeneralDlg::OnTvnSelchangedTreeview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	EDisplayMode eCurMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
	*pResult = 0;

	SetCheckListCtrl(false);
	STreeData kTreeData;
	if(!GetTreeData(kTreeData, eCurMode))
	{
		return;
	}
	else
	{
		SetColumnListCtrl(eCurMode);
		InvalidateListView();
	}
}

void CGeneralDlg::CallLoginDlg()
{
	if(!g_kNetwork.IsConnect())
	{
		ShowWindow(SW_HIDE);

		bool bRet = false;
		while ( !bRet )
		{
			while ( !g_kNetwork.IsServiceStart() )
			{
				Sleep(300);
			}

			if( IDOK == m_kLoginDlg.DoModal() )
			{
				bRet = g_kNetwork.TryConnectServer();
				if ( !bRet )
				{
					MessageBox(_T("Try Again, Please"), _T("Warning"), MB_OK);
				}
			}
			else
			{
				g_kNetwork.DisConnectServer();
				OnOK();
				break;
			}
		}
	}
	else
	{
		MessageBox(_T("Already Connected Server!!"), _T("Warning"), MB_OK);
	}
}

void CGeneralDlg::Close()
{
	OnOK();
}

void CGeneralDlg::Clear()
{
	BM::CAutoMutex kLock( m_kMutex );
	m_kListCtrl.DeleteAllItems();
}

void CGeneralDlg::OnLvnItemchangedListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	m_iSelectedListColum = (int)pNMLV->lParam;

	CONT_SERVER_STATE_EX::mapped_type kServerInfo;
	if (g_kGameServerMgr.GetServerInfo( m_iSiteNo, m_iSelectedListColum, kServerInfo))
	{
		bool bChange = false;
		if( kServerInfo.kTbl.nServerType == CEL::ST_CONTENTS
			|| kServerInfo.kTbl.nServerType == CEL::ST_CENTER )
		{
			bChange = true;			
		}
	}
}

void CGeneralDlg::OnBnClickedBtnMaxUserChange()
{
	ChangeMaxUserDlg kChangeMaxUserDlg;
	kChangeMaxUserDlg.DoModal();
}

void CGeneralDlg::SetControlBtn()
{
	// Control Level
	BOOL const bOpen = ( g_kGameServerMgr.bControlLevel() ? TRUE : FALSE );

	CWnd* pkWnd = GetDlgItem(IDC_BTN_ON);
	if (pkWnd) pkWnd->EnableWindow(bOpen);
	pkWnd = GetDlgItem(IDC_BTN_ON);
	if (pkWnd) pkWnd->EnableWindow(bOpen);
	pkWnd = GetDlgItem(IDC_BTN_OFF);
	if (pkWnd) pkWnd->EnableWindow(bOpen);
	pkWnd = GetDlgItem(IDC_CONFIRM_KILL);
	if (pkWnd) pkWnd->EnableWindow(bOpen);
	pkWnd = GetDlgItem(IDC_BTN_PATCH);
	if (pkWnd) pkWnd->EnableWindow(bOpen);
	pkWnd = GetDlgItem(IDC_EDIT1);
	if (pkWnd) pkWnd->EnableWindow(bOpen);

	m_kCheckBtnKillServer.EnableWindow(bOpen);
	m_kBtnKillServer.EnableWindow(FALSE);;
	m_kCheckBtnReSync.EnableWindow(bOpen);;
	m_kBtnReSync.EnableWindow(FALSE);;
}
void CGeneralDlg::OnBnClickedBtnResync()
{
	std::wstring const kMessage = _T("Server Resync?\n Will All Server Terminate!!");
	if( IDOK == MessageBox(kMessage.c_str(), L"Server On warning!!", MB_OKCANCEL) )
	{
		g_kServerCmdMgr.OnCommand(MCT_RE_SYNC);
		m_kProgressJob.SetRange(1, g_kServerCmdMgr.GetMaxIndex());
	}
}

void CGeneralDlg::OnBnClickedConfirmResync()
{
	m_kBtnReSync.EnableWindow(m_kCheckBtnReSync.GetCheck());
}


void CGeneralDlg::OnBnClickedDisp()
{
	InvalidateListView();
}

void CGeneralDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	EDisplayMode eCurMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
	if( DM_CMD_HISTORY == eCurMode )
	{//! 아직 지원 안함.
		return;
	}

	HTREEITEM* hSelectTreeItem = NULL;
	HTREEITEM* hLastSelectTreeItem = NULL;

	//! 컬럼 업데이트
	SetColumnListCtrl(eCurMode);

	//! 탭에 따라 쓰지 않는 컨트롤은 숨김
	for(size_t i=0; i<m_kContLogWnd.size(); ++i)
	{
		m_kContLogWnd[i]->ShowWindow(eCurMode);
	}
	for(size_t i=0; i<m_kContStateWnd.size(); ++i)
	{
		m_kContStateWnd[i]->ShowWindow(!eCurMode);
	}

	//! 트리/리스트 컨트롤 업데이트
	InvalidateTreeView(eCurMode);
	InvalidateListView();

	*pResult = 0;
}

void CGeneralDlg::OnBnClickedBtnDownload()
{
	if(m_bIsDownload)
	{//! 다운로드 중에 누르면 Cancle시키자.
		g_kLogCopyMgr.IsStopDownload(true);
		return;
	}

	//! 아니라면 다운로드 시작.
	CONT_SERVER_STRING kWarningList;

	STreeData kTreeData;
	EDisplayMode eMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
	if(!GetTreeData(kTreeData, eMode)) 
	{
		return;
	}

	CFileDownDlg kFileDownDlg;
	kFileDownDlg.Init(kTreeData.nSecKey);
	if(kTreeData.nSecKey == FT_DUMP)
	{
		SLogCopyer::CONT_DUMP_INFO kContDumpInfo;
		if( GetCheckedList(kTreeData.nSecKey, kWarningList, kContDumpInfo) )
		{
			kFileDownDlg.SetDownloadList(kWarningList, kContDumpInfo);
		}
	}
	else
	{
		SLogCopyer::CONT_LOG_INFO kContLogInfo;
		if( GetCheckedList(kTreeData.nSecKey, kWarningList, kContLogInfo) )
		{
			kFileDownDlg.SetDownloadList(kWarningList, kContLogInfo);
		}
	}

	kFileDownDlg.DoModal();
}

void CGeneralDlg::InitDownloadInfo(size_t usTotalCount, double dTotalsize)
{//! 다운로드 관련 UI그룹 초기화
	g_kLogCopyMgr.IsStopDownload(false);
	m_bIsDownload = true;
	m_kBtnDownload.SetWindowTextW(L"Cancel");
	m_kProgressDownload.SetRange(1, usTotalCount);
	m_kProgressDownload.SetStep(1);
	m_iTotalFileCount = usTotalCount;
	m_dTotalFileSize = dTotalsize;
	m_iCurFileCount = 1;
	m_dCurFileSize = 0;

	WCHAR wstrFileData[64] = {0,};
	_stprintf_s(wstrFileData, _countof(wstrFileData), _T("File Download Ready..." ));
	m_kStrFileName.SetWindowTextW(wstrFileData);
	
	_stprintf_s(wstrFileData, _countof(wstrFileData), _T("File Count : 1 / %d"), usTotalCount);
	m_kStrFileSize.SetWindowTextW(wstrFileData);

	m_wstrTotalSize = ::TansformFileSize(m_dTotalFileSize);
	_stprintf_s(wstrFileData, _countof(wstrFileData), _T("%0.1f %s"), m_dTotalFileSize, m_wstrTotalSize.c_str());
	m_wstrTotalSize = wstrFileData;
	_stprintf_s(wstrFileData, _countof(wstrFileData), _T("File Size : 1 / %s"), m_wstrTotalSize.c_str());
	m_kStrFileSize.SetWindowTextW(wstrFileData);
}

void CGeneralDlg::EndDownload()
{
	m_kBtnDownload.SetWindowTextW(L"Download");
	m_kStrFileSize.SetWindowTextW(L"");
	m_kStrFileCount.SetWindowTextW(L"");
	m_kProgressDownload.SetRange(0, 1);
	m_kProgressDownload.SetPos(0);
	m_bIsDownload = false;

	if(g_kLogCopyMgr.IsStopDownload())
	{
		m_kStrFileName.SetWindowTextW(L"Download Cancel!");
	}
	else
	{
		m_kStrFileName.SetWindowTextW(L"Download Complete!");
	}

}

void CGeneralDlg::UpdateDownloadInfo(std::wstring wstrFileName, double dCurSize)
{
	m_dCurFileSize += dCurSize;

	WCHAR wstrFileData[256] = {0,};
	_stprintf_s(wstrFileData, _countof(wstrFileData), _T("File Count : %d / %d"), m_iCurFileCount, m_iTotalFileCount);
	m_kStrFileCount.SetWindowTextW(wstrFileData);

	std::wstring wstrCurSize = ::TansformFileSize(dCurSize);
	_stprintf_s(wstrFileData, _countof(wstrFileData), _T("File Size : %0.1f %s / %s"), dCurSize, wstrCurSize.c_str(), m_wstrTotalSize.c_str());
	m_kStrFileSize.SetWindowTextW(wstrFileData);

	m_kStrFileName.SetWindowTextW(wstrFileName.c_str());
	if(m_iCurFileCount != 1)
	{
		m_kProgressDownload.StepIt();
	}
	++m_iCurFileCount;
		
}
void CGeneralDlg::OnBnClickedBtnExpand()
{
	m_kTreeCtrl.Expand(m_kTreeCtrl.GetRootItem(), TVE_EXPAND);
	//m_kTree.GetFirstVisibleItem
}

void CGeneralDlg::OnBnClickedBtnFold()
{
	EDisplayMode eCurMode = static_cast<EDisplayMode>(m_kTabCtrl.GetCurSel());
	InvalidateTreeView(eCurMode);
}

void CGeneralDlg::OnBnClickedBtnSelectAll()
{
	ContServerID kCheckedList;
	CONT_SERVER_STRING kWarningList;
	GetCheckedList(kCheckedList, kWarningList);
	bool bIsCheck = kCheckedList.empty();
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; i++)
	{
		m_kListCtrl.SetCheck( i, bIsCheck );
	}
}

void CGeneralDlg::OnBnClickedBtnSelectRoot()
{
	ContServerID kCheckedList;
	CONT_SERVER_STRING kWarningList;
	GetCheckedList(kCheckedList, kWarningList);
	bool bIsCheck = kCheckedList.empty();
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; i++)
	{
		int iServerIdx = (int)m_kListCtrl.GetItemData(i);
		CONT_SERVER_STATE_EX::mapped_type kServerInfo;
		if (g_kGameServerMgr.GetServerInfo( m_iSiteNo, iServerIdx, kServerInfo))
		{
			if( 0 == kServerInfo.kTbl.nRealm || kServerInfo.kTbl.nServerType == CEL::ST_CONTENTS )
			{
				m_kListCtrl.SetCheck( i, bIsCheck );
			}
		}
	}
}

void CGeneralDlg::OnBnClickedBtnSelectRealm()
{
	ContServerID kCheckedList;
	CONT_SERVER_STRING kWarningList;
	GetCheckedList(kCheckedList, kWarningList);
	bool bIsCheck = kCheckedList.empty();
	for (int i=0 ; i<m_kListCtrl.GetItemCount() ; i++)
	{
		int iServerIdx = (int)m_kListCtrl.GetItemData(i);
		CONT_SERVER_STATE_EX::mapped_type kServerInfo;
		if (g_kGameServerMgr.GetServerInfo( m_iSiteNo, iServerIdx, kServerInfo))
		{
			if( kServerInfo.kTbl.nRealm && kServerInfo.kTbl.nServerType != CEL::ST_CONTENTS )
			{
				m_kListCtrl.SetCheck( i, bIsCheck );
			}
		}
	}
}

void CGeneralDlg::OnBnClickedBtnNotice()
{
	NoticeDlg kNoticeDlg;
	kNoticeDlg.DoModal();
}

void CGeneralDlg::SetCheckListCtrl( bool bCheck )
{
	int iIndex = 0;
	while( m_kListCtrl.GetItemCount() > iIndex )
	{
		m_kListCtrl.SetCheck(iIndex, bCheck);
		++iIndex;
	}
}