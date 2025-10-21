#include "stdafx.h"
#include "Variant/PgControlDefMgr.h"
#include "QCCore.h"
#include "LogMgr.h"
#include "CheckMgr.h"
#include "ListControl.h"
#include "TextViewer.h"
#include "ItemBag.h"
#include "BM/LocalMgr.h"

namespace QueryUtil
{
	CObserverMgr kObserverMgr;
}
namespace LocalQuery
{
	int const iBase = 10000;
	enum ELocalQueryType
	{
		LQT_None = 0 + iBase,
		LQT_ContainerMemo,
		LQT_BagMemo,
		LQT_BagGrpMemo,
	};

	bool OnDBExcute(CEL::DB_RESULT &rkResult)
	{
		switch( rkResult.QueryType() )
		{
		case LQT_BagMemo:
			{
				if( CEL::DR_SUCCESS == rkResult.eRet )
				{
					ItemBag::CONT_BAG_MEMO kTemp;
					CEL::DB_DATA_ARRAY::const_iterator iter = rkResult.vecArray.begin();
					while( rkResult.vecArray.end() != iter )
					{
						ItemBag::CONT_BAG_MEMO::key_type kKey;
						ItemBag::CONT_BAG_MEMO::mapped_type kVal;

						(*iter).Pop( kKey.first );		++iter;
						(*iter).Pop( kKey.second );		++iter;
						(*iter).Pop( kVal );			++iter;

						kTemp.insert( std::make_pair(kKey, kVal) );
					}
					ItemBag::kContBagMemo.swap(kTemp);
				}
			}break;
		case LQT_BagGrpMemo:
		case LQT_ContainerMemo:
			{
				if( CEL::DR_SUCCESS == rkResult.eRet )
				{
					ItemBag::CONT_MEMO kTemp;
					CEL::DB_DATA_ARRAY::const_iterator iter = rkResult.vecArray.begin();
					while( rkResult.vecArray.end() != iter )
					{
						ItemBag::CONT_MEMO::key_type kKey;
						ItemBag::CONT_MEMO::mapped_type kVal;

						(*iter).Pop( kKey );		++iter;
						(*iter).Pop( kVal );		++iter;

						kTemp.insert( std::make_pair(kKey, kVal) );
					}
					switch( rkResult.QueryType() )
					{
					case LQT_BagGrpMemo:		{ ItemBag::kContBagGrpMemo.swap(kTemp); }break;
					case LQT_ContainerMemo:		{ ItemBag::kContContainerMemo.swap( kTemp ); }break;
					}
				}
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}
}

extern bool ReadDBConfigINI(BM::vstring const vstrFileName, SERVER_IDENTITY const &rkSI, CONT_DB_INIT_DESC &rkContOut);

bool g_Asc = true;

int CALLBACK MyCompare( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	return g_Core.Sort( lParam1, lParam2, lParamSort );
}

int	CQCCore::Sort( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)lParamSort;
	TCHAR szItemText1[MAX_PATH] = {0};
	TCHAR szItemText2[MAX_PATH] = {0};

	/* lParamSort is the identify the subitem */
	/* lParam1 is the index of the first item and lParam2 the index of the second */

	ListView_GetItemText(pNMLV->hdr.hwndFrom, (INT)lParam1, pNMLV->iSubItem, szItemText1, MAX_PATH);
	ListView_GetItemText(pNMLV->hdr.hwndFrom, (INT)lParam2, pNMLV->iSubItem, szItemText2, MAX_PATH);

	int iRet = 0;
	switch( pNMLV->iSubItem )
	{
	case ST_ID:
	case ST_TXTID:
	case ST_AREAID:
	case ST_MINLEVEL:
	case ST_MAXLEVEL:
		{
			int iItem1 = _ttoi( szItemText1 );
			int iItem2 = _ttoi( szItemText2 );

			if( iItem1 == iItem2 )
				iRet = 0;
			else if( iItem1 < iItem2 )
				iRet = -1;
			else
				iRet = 1;
		}break;
	default:
		{
			iRet = pNMLV->lParam ? _tcscmp(szItemText1, szItemText2) : (-1) * _tcscmp(szItemText1, szItemText2);
		}break;
	}	

	if( g_Asc )
	{
		return iRet;
	}
	else
	{
		return -iRet;
	}
}

void CALLBACK AddError(PgQuestInfoVerifyUtil::SQuestInfoError const& rkNewError)
{
	BM::CAutoMutex kLock( g_kError );
	auto kRet = g_kErrorMsg.insert( std::make_pair( rkNewError.iQuestID ,ContErrorMsg::mapped_type() ) );
	if( g_kErrorMsg.end() != kRet.first )
	{
		(*kRet.first).second.push_back( rkNewError );
	}
}

void CQCCore::Start()
{
	if( !GetRegist() )
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"System Loading..."));
		return ;
	}

	if( GetWork() )
	{
		return;
	}

	Clear();
	Reload();
	g_kErrorMsg.clear();
	g_kCheckMgr.Clear();
	SetWork(true);

	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Connected DB"));
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Creating Dump File"));
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Wait for a moment please..."));

	wchar_t szNationCode[MAX_PATH] = {0, };
	int const iNationCode = BM::vstring(szNationCode);
	::GetWindowText(GetDlgItem(g_hDlgWnd, IDC_MAX_KILLCOUNT_EDIT), szNationCode, MAX_PATH);
	int const iMaxKillCount = BM::vstring(szNationCode);

	g_kCheckMgr.Start(iNationCode, iMaxKillCount);
}

void CALLBACK OnRegist(const CEL::SRegistResult &rkArg)
{	
	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		g_kCoreCenter.SvcStart();

		// DB 연결 유무 상관없이 TB_DefItemRarityControl은 메모리에 로드
		CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_RARITY_CONTROL, _T("EXEC [dbo].[UP_LoadDefItemRarityControl]"));
		g_kCoreCenter.PushQuery(kQuery, true);
	}
}

HRESULT CALLBACK OnDB_EXECUTE(CEL::DB_RESULT &rkResult)
{
	// DB 연결 유무 상관없이 TB_DefItemRarityControl은 메모리에 로드
	if( DQT_DEF_ITEM_RARITY_CONTROL == rkResult.QueryType() )
	{
		CONT_DEF_ITEM_RARITY_CONTROL map;

		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != itor )
		{
			CONT_DEF_ITEM_RARITY_CONTROL::mapped_type element;

			(*itor).Pop( element.iTypeNo );	++itor;
			(*itor).Pop( element.iSuccessRateControlNo );	++itor;

			map.insert( std::make_pair(element.iTypeNo, element) );
		}

		if( map.size() )
		{
			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(map);

			g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Quest checker initialized.."));
			g_Core.SetRegist( true );	// TB_DefItemRarityControl 로드 완료 시점이 Quest Checker 준비 완료
			::EnableWindow(GetDlgItem(g_hDlgWnd, IDC_BTN_START), TRUE);
			ItemBag::CreateItemBagDlg(g_hDlgWnd, TRUE);

			return S_OK;
		}		
		return E_FAIL;
	}

	if( !LocalQuery::OnDBExcute(rkResult) )
	{
		if( !PgDBCache::OnDBExcute( rkResult ) )
		{
			return E_FAIL;
		}
	}

	QueryUtil::kObserverMgr.Process(static_cast< EDBQueryType >(rkResult.QueryType()));
	return S_OK;
}

HRESULT CALLBACK OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &rkResult)
{
	return S_OK;
}

CQCCore::CQCCore(void)
{
	m_bRegist		= false;
	m_bWork			= false;
	m_pkQuestList	= NULL;
	m_pkQFailedList	= NULL;
	m_pkLog			= NULL;
	m_pkDBNotExist	= NULL;
	m_pkLogError	= NULL;
	m_kTextView		= NULL;
	m_pkEmptyList	= NULL;
	m_kState		= eWork_Prepare;
}

CQCCore::~CQCCore(void)
{
	
}

void CQCCore::End()
{
	CloseHandle(m_kTextView);
	SAFE_DELETE(m_pkEmptyList);
	SAFE_DELETE(m_pkQFailedList);
	SAFE_DELETE(m_pkDBNotExist);
	SAFE_DELETE(m_pkLogError);
	SAFE_DELETE(m_pkLog);	
	SAFE_DELETE(m_pkQuestList);		
}

void CQCCore::Clear()
{	
	if( m_pkQuestList )		m_pkQuestList->Clear();
	if( m_pkQFailedList	)	m_pkQFailedList->Clear();
	if( m_pkLog )			m_pkLog->Clear();
	if( m_pkLogError )		m_pkLogError->Clear();
	if( m_pkDBNotExist )	m_pkDBNotExist->Clear();
	if( m_pkEmptyList )		m_pkEmptyList->Clear();
	if( m_pkTextList )		m_pkTextList->Clear();
}



//-----------------------------------------------------------------------------
//	윈도우 커맨드
//-----------------------------------------------------------------------------
bool CQCCore::WMCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case ID_FOLDERSELECT:
		{			
			m_TargetFolder.clear();
			SetLocation();
			if( Reload() )
			{
				g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Change work Folder."));
			}
		}break;
	case IDC_BTN_VIEWTEXTLIST:
		{
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TEXTVIEW), g_hDlgWnd, TViewerProc);
		}break;
	case IDC_BTN_QUEST_EMPTY:
		{
			m_pkEmptyList->Clear();
			g_kCheckMgr.NotExistID();
		}break;
	case IDC_BTN_START:
		{
			Start();
		}break;
	case IDC_BUILD_BAG_RATE:
		{
			g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"ItemBag Rate Parsing.... Start"));
			TextHelper::Clear();
			TextHelper::Load();

			QueryUtil::CObserver kObserver(ItemBag::MakeItemBagResultProcess);
			kObserver << DQT_DEFSTRINGS << DQT_DEFITEM << DQT_DEF_ITEM_BAG_GROUP << DQT_DEFITEMBAG << DQT_DEF_ITEM_BAG_ELEMENTS;
			kObserver << DQT_DEF_SUCCESS_RATE_CONTROL << DQT_DEF_COUNT_CONTROL << DQT_DEF_DROP_MONEY_CONTROL << DQT_DEFITEMCONTAINER;
			kObserver << static_cast< EDBQueryType >(LocalQuery::LQT_ContainerMemo) << static_cast< EDBQueryType >(LocalQuery::LQT_BagMemo);
			kObserver << static_cast< EDBQueryType >(LocalQuery::LQT_BagGrpMemo);
			QueryUtil::kObserverMgr.Add(kObserver);

			{ CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEFSTRINGS, _T("EXEC [dbo].[UP_LoadDefStrings]"));									g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEM, _T("EXEC [dbo].[UP_LoadDefItem2]"));											g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_BAG_GROUP, _T("EXEC [dbo].[UP_LoadDefItemBagGroup]"));							g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMBAG, _T("EXEC [dbo].[UP_LoadDefItemBag]"));										g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_BAG_ELEMENTS, _T("EXEC [dbo].[UP_LoadDefItemBagElements]"));					g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_SUCCESS_RATE_CONTROL, _T("EXEC [dbo].[UP_LoadDefSuccessRateControl]"));				g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_COUNT_CONTROL, _T("EXEC [dbo].[UP_LoadDefCountControl]"));							g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_DROP_MONEY_CONTROL, _T("EXEC [dbo].[UP_LoadDefDropMoneyControl]"));					g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMCONTAINER, _T("EXEC [dbo].[UP_LoadDefItemContainer2]"));							g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, LocalQuery::LQT_ContainerMemo, _T("SELECT CONTAINERNO, MEMO FROM DBO.TB_DEFITEMCONTAINER"));	g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, LocalQuery::LQT_BagGrpMemo, _T("SELECT BAGGROUPNO, MEMO FROM DBO.TB_DEFITEMBAGGROUP")); 	g_kCoreCenter.PushQuery(kQuery, false); }
			{ CEL::DB_QUERY kQuery( DT_DEF, LocalQuery::LQT_BagMemo, _T("SELECT BAGNO, LEVEL, MEMO FROM DBO.TB_DEFITEMBAG")); 					g_kCoreCenter.PushQuery(kQuery, false); }
		}break;
	case IDC_RADIO_DB:
		{
			g_bDBCon = true;
		}break;
	case IDC_RADIO_LOCAL:
		{
			g_bDBCon = false;
		}break;
	case IDC_BTN_SIMULATION:
		{
		}break;	
	case IDC_CHK_QPTS_OPTION:
		{
		}break;
	case IDC_TXT_QUESTFINDER:
		{
		}break;
	case ID_OUTPUT:
		{
			Export();
		}break;
	case ID_EXIT:
	case IDCANCEL:
		{			
			g_kLogMgr.VDeactivate();
			g_kCheckMgr.VDeactivate();			
			g_kCoreCenter.Close();

			EndDialog(g_hLogWnd, 0);
			EndDialog(g_hDlgWnd, 0);		
			
		}break;
	default:
		{
		}break;
	}
	return	FALSE;
}


bool CQCCore::TTVWMInit(const HWND hDlg)
{
	m_pkTextList	= new CListControl(hDlg, IDC_LIST_TEXTTABLE);
	if(m_pkTextList)
	{
		m_pkTextList->SetColumn(WSTR_TEXTLISTCOLUMN);
	}
	else
	{
		SAFE_DELETE(m_pkTextList);
		return	false;
	}

	wchar_t	szID[MAX_PATH] = {0,};
	GetDlgItemText(g_hDlgWnd, IDC_TXT_QUESTID, szID, MAX_PATH);
	g_kCheckMgr.PutMsg(SCHECKMESSAGE(LT_QTTV, szID));

	return	true;
}

bool CQCCore::TTVWMCommand(const HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDCANCEL:
		{
			SAFE_DELETE(m_pkTextList);
			EndDialog(hDlg, 0);
		}return FALSE;
	}

	return	FALSE;
}

void CQCCore::Export()
{
	if( !m_pkQuestList )
		return ;

	HANDLE hFile = CreateFile( L"QuestChecker.csv", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile )	
		return ;	

	wchar_t			wstrTemp[MAX_PATH] = {0,};
	char			strItemText[MAX_PATH] = {0,};
	DWORD			dwRead;

	/*char			strColName[][MAX_PATH] = { "ID", "TXTID", "NAME", "AreaID", "Area", "MinLevel", "MaxLevel" };

	for( int i = 0; i < m_pkQuestList->GetTapCnt(); i++ )
	{
		WriteFile( hFile, strColName[i], (DWORD)strlen(strColName[i]), &dwRead, NULL);
		
		if( i < m_pkQuestList->GetTapCnt() - 1 )
		{
			sprintf_s( strItemText, MAX_PATH, "," );
			WriteFile( hFile, strItemText, (DWORD)strlen(strItemText), &dwRead, NULL);
		}
	}

	sprintf_s( strItemText, MAX_PATH, "\r\n" );
	WriteFile( hFile, strItemText, (DWORD)strlen(strItemText), &dwRead, NULL );*/

	for( int i = 0; i < m_pkQuestList->GetTapCol(); i++ )
	{
		for( int j = 0; j < m_pkQuestList->GetTapCnt(); j++ )
		{
			ListView_GetItemText( m_pkQuestList->GetHandle(), i, j, wstrTemp, MAX_PATH );
			sprintf_s( strItemText, MAX_PATH, MB(wstrTemp) );
			WriteFile( hFile, strItemText, (DWORD)strlen(strItemText), &dwRead, NULL);

			if( j < m_pkQuestList->GetTapCnt() - 1 )
			{					
				sprintf_s( strItemText, MAX_PATH, "," );
				WriteFile( hFile, strItemText, (DWORD)strlen(strItemText), &dwRead, NULL);
			}
		}

		if( i < m_pkQuestList->GetTapCol() - 1 )
		{
			sprintf_s( strItemText, MAX_PATH, "\r\n" );
			WriteFile( hFile, strItemText, (DWORD)strlen(strItemText), &dwRead, NULL );
		}		
	}

	CloseHandle( hFile );

	::MessageBox(g_hDlgWnd, L"Success List Exported.", L"Success", MB_OK);
}

void CQCCore::NotifyMsg(LPARAM lParam)
{
	LPNMHDR	Hdr = (LPNMHDR)lParam;
	LPNMITEMACTIVATE	Hla = (LPNMITEMACTIVATE)lParam;
	LPNMLISTVIEW nlv;
	nlv = (LPNMLISTVIEW)lParam;

	if(m_pkQuestList && Hdr->hwndFrom == m_pkQuestList->GetHandle())
	{
		switch(Hdr->code)
		{
		case LVN_ITEMCHANGED:
			{
				wchar_t	Caption[MAX_PATH] = {0,};
				ListView_GetItemText(m_pkQuestList->GetHandle(), Hla->iItem, Hla->iSubItem, Caption, MAX_PATH);
				SetWindowText(GetDlgItem(g_hDlgWnd, IDC_TXT_QUESTID), Caption);
				ListView_GetItemText(m_pkQuestList->GetHandle(), Hla->iItem, Hla->iSubItem + 2, Caption, MAX_PATH);
				SetWindowText(GetDlgItem(g_hDlgWnd, IDC_TXT_QUESTNAME), Caption);
			}break;
		case LVN_COLUMNCLICK:
			{
				ListView_SortItemsEx( Hdr->hwndFrom, (PFNLVCOMPARE)MyCompare, nlv );
				g_Asc = !g_Asc;
			}break;
		case NM_DBLCLK:
			{				
				std::wstring	FILENAME = L"XML\\QUEST\\QUEST%08d.xml";
				wchar_t	szTemp[MAX_PATH] = {0,};
				wchar_t	Caption[MAX_PATH] = {0,};
				ListView_GetItemText(Hdr->hwndFrom, Hla->iItem, 0, Caption, MAX_PATH);
				swprintf(szTemp, MAX_PATH, FILENAME.c_str(), _wtoi(Caption));

				HWND handle = 0;
				HINSTANCE kIns = 		
					ShellExecute(handle,
					_T("open"),					
					szTemp,
					NULL,
					NULL,
					SW_SHOWNORMAL);
			}break;
		}
	}
	else if(m_pkQFailedList && Hdr->hwndFrom == m_pkQFailedList->GetHandle())
	{
		switch(Hdr->code)
		{
		case LVN_ITEMCHANGED:
			{
				m_pkLogError->Clear();
				wchar_t	Caption[MAX_PATH] = {0,};
				ListView_GetItemText(m_pkQFailedList->GetHandle(), Hla->iItem, Hla->iSubItem, Caption, MAX_PATH);
				g_kLogMgr.OutError(::_wtoi(Caption));
			}break;
		case LVN_COLUMNCLICK:
			{
				ListView_SortItemsEx( Hdr->hwndFrom, (PFNLVCOMPARE)MyCompare, nlv );
				g_Asc = !g_Asc;
			}break;
		case NM_DBLCLK:
			{				
				std::wstring	FILENAME = L"XML\\QUEST\\QUEST%08d.xml";
				wchar_t	szTemp[MAX_PATH] = {0,};
				wchar_t	Caption[MAX_PATH] = {0,};
				ListView_GetItemText(Hdr->hwndFrom, Hla->iItem, 0, Caption, MAX_PATH);
				swprintf(szTemp, MAX_PATH, FILENAME.c_str(), _wtoi(Caption));

				HWND handle = 0;
				HINSTANCE kIns = 		
					ShellExecute(handle,
					_T("open"),					
					szTemp,
					NULL,
					NULL,
					SW_SHOWNORMAL);
			}break;
		}
	}
	else if( m_pkDBNotExist && Hdr->hwndFrom == m_pkDBNotExist->GetHandle() )
	{
		switch(Hdr->code)
		{
		case LVN_ITEMCHANGED:
			{
				m_pkLogError->Clear();
				wchar_t	Caption[MAX_PATH] = {0,};
				ListView_GetItemText(m_pkDBNotExist->GetHandle(), Hla->iItem, Hla->iSubItem, Caption, MAX_PATH);
				g_kLogMgr.OutError(::_wtoi(Caption));
			}break;
		case LVN_COLUMNCLICK:
			{
				ListView_SortItemsEx( Hdr->hwndFrom, (PFNLVCOMPARE)MyCompare, nlv );
				g_Asc = !g_Asc;
			}break;
		case NM_DBLCLK:
			{				
				std::wstring	FILENAME = L"XML\\QUEST\\QUEST%08d.xml";
				wchar_t	szTemp[MAX_PATH] = {0,};
				wchar_t	Caption[MAX_PATH] = {0,};
				ListView_GetItemText(Hdr->hwndFrom, Hla->iItem, 0, Caption, MAX_PATH);
				swprintf(szTemp, MAX_PATH, FILENAME.c_str(), _wtoi(Caption));

				HWND handle = 0;
				HINSTANCE kIns = 		
					ShellExecute(handle,
					_T("open"),					
					szTemp,
					NULL,
					NULL,
					SW_SHOWNORMAL);
			}break;
		}
	}
}

//-----------------------------------------------------------------------------
//  로드
//-----------------------------------------------------------------------------
bool CQCCore::CreateList(CListControl*& pList, HWND hWnd, DWORD dwID, std::wstring const& ColumnType)
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

bool CQCCore::Load()
{
	g_kCoreCenter;

	CreateList(m_pkQuestList, g_hDlgWnd, IDC_LIST_QUESTTABLE, WSTR_QUESTLISTCOLUMN);
	CreateList(m_pkQFailedList, g_hDlgWnd, IDC_LIST_QUESTERROR, WSTR_QUESTLISTCOLUMN);
	CreateList(m_pkDBNotExist, g_hDlgWnd, IDC_LIST_DBNOTEXIST, WSTR_QUESTLISTCOLUMN);
	CreateList(m_pkLog, g_hLogWnd, IDC_LIST_LOG, WSTR_LOGLISTCOLUMN);	
	CreateList(m_pkLogError, g_hLogWnd, IDC_LIST_ERROR, WSTR_LOGERRLISTCOLUMN);
	CreateList(m_pkEmptyList, g_hDlgWnd, IDC_LIST_QUEST_EMPTY, WSTR_QUESTEMPTY);

	g_kCheckMgr.PutMsg( SCHECKMESSAGE( LT_INIT ) );

	if( CheckerInit() )
	{
		Reload();
	}
	
	PgQuestInfoVerifyUtil::g_lpAddErrorFunc = AddError;

	//	쓰레드 관련 초기화
	CEL::INIT_CENTER_DESC	kCenterInit;
	kCenterInit.eOT	= BM::OUTPUT_JUST_TRACE;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.bIsUseDBWorker = true;
	kCenterInit.dwProactorThreadCount = 0;

	g_kCoreCenter.Init(kCenterInit);

	// Module Full Path.
	TCHAR szModulePath[ _MAX_PATH ];
	ZeroMemory( szModulePath, sizeof( szModulePath ) );
	::GetModuleFileName( NULL, szModulePath, sizeof( szModulePath ) );

	// FileName 제외한 Path.
	CString strModulePath = szModulePath;
	int iPos = strModulePath.ReverseFind(_T('\\') );
	strModulePath = strModulePath.Left( iPos + 1 );
	BM::vstring strPath( strModulePath );

	// Local.ini
	if( !g_kLocal.LoadFromINI( strPath + L"Local.ini") )
	{
		g_kLogMgr.PutMsg( SLOGMESSAGE( 0, LT_COMPLATE, L"Local.ini read fail!!" ) );
	}

	// Read DB_Config.ini
	CONT_DB_INIT_DESC kContDBInit;
	if( !ReadDBConfigINI( strPath + L"DB_Config.ini", g_kProcessCfg.ServerIdentity(), kContDBInit ) )
	{
		g_kLogMgr.PutMsg( SLOGMESSAGE( 0, LT_COMPLATE, L"DB_Config.ini read fail!!" ) );
	}

	CONT_DB_INIT_DESC::iterator dbinit_itor = kContDBInit.begin();
	CEL::INIT_DB_DESC &kDefDBInit = (*dbinit_itor);
	CEL::INIT_DB_DESC &kLocalDBInit = *(++dbinit_itor);

	kDefDBInit.bUseConsoleLog = false;
	kDefDBInit.OnDBExecute = OnDB_EXECUTE;
	kDefDBInit.OnDBExecuteTran = OnDB_EXECUTE_TRAN;	
	kDefDBInit.dwWorkerCount = 3;

	kLocalDBInit.bUseConsoleLog = false;
	kLocalDBInit.OnDBExecute = OnDB_EXECUTE;
	kLocalDBInit.OnDBExecuteTran = OnDB_EXECUTE_TRAN;	
	kLocalDBInit.dwWorkerCount = 3;

	g_kCoreCenter.Regist( CEL::RT_DB_WORKER, &kDefDBInit );
	g_kCoreCenter.Regist( CEL::RT_DB_WORKER, &kLocalDBInit );

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 1;

	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	g_kCheckMgr.StartSvc();
	g_kLogMgr.StartSvc(1);

	return true;
}

bool CQCCore::CheckerInit()
{
	wchar_t	szTemp[MAX_PATH] = {0,};

	//	패치 서버 정보
	::GetPrivateProfileString(L"FORDER_INFO", L"DIR", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());
	m_TargetFolder = szTemp;
	::GetPrivateProfileString(L"FORDER_INFO", L"NOTE", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());
	m_NotePadDir = szTemp;
	//::GetPrivateProfileString(L"TEST_SERVER", L"IP", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());
	//::GetPrivateProfileString(L"TEST_SERVER", L"PORT", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());
	//::GetPrivateProfileString(L"TEST_SERVER", L"ID", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());
	//::GetPrivateProfileString(L"TEST_SERVER", L"PW", L"", szTemp, sizeof(szTemp), WSTR_CONFIG_FILE.c_str());

	return	true;
}

bool CQCCore::Reload()
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

	return	true;
}

bool CQCCore::AddListItem(std::wstring const& wstrItem, EOUTTYPE OutType , DWORD dwFlag)
{
	bool	bRet = false;
	switch(dwFlag)
	{
	case CHK_LOG:
		{
			if( OT_SUCCESS == OutType )
			{
				bRet = m_pkLog->AddItem(wstrItem);
				ListView_EnsureVisible(m_pkLog->GetHandle(), m_pkLog->GetTapCol() - 1, FALSE);
			}
			else if( OT_FAIL == OutType)
			{
				bRet = m_pkLogError->AddItem(wstrItem);
				ListView_EnsureVisible(m_pkLogError->GetHandle(), m_pkLogError->GetTapCol() - 1, FALSE);
			}
			else
			{
				bRet = m_pkDBNotExist->AddItem(wstrItem);
				ListView_EnsureVisible(m_pkDBNotExist->GetHandle(), m_pkDBNotExist->GetTapCol() - 1, FALSE);
			}
		}break;
	case CHK_QTVIEWER:
		{
			if(m_pkTextList)
			{
				bRet = m_pkTextList->AddItem(wstrItem);
			}
		}break;
	case CHK_EMPTYID:
		{
			if( m_pkEmptyList )
			{
				bRet = m_pkEmptyList->AddItem(wstrItem);
			}
		}break;
	default:
		{
			if( OT_SUCCESS == OutType )
			{
				bRet = m_pkQuestList->AddItem(wstrItem, dwFlag);
			}
			else if( OT_FAIL == OutType )
			{
				bRet = m_pkQFailedList->AddItem(wstrItem, dwFlag);
			}
			else
			{
				bRet = m_pkDBNotExist->AddItem(wstrItem, dwFlag);
			}
		}break;
	}

	return	bRet;
}

//-----------------------------------------------------------------------------
//  폴더 지정
//-----------------------------------------------------------------------------
bool CQCCore::SetFolder()
{
	if( m_TargetFolder.size() == 0 ) 
	{
		if(!FolderSelect(g_hDlgWnd, m_TargetFolder))
		{			
			g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Fail Set Dragonica-Dev Folder."));
			return false;
		}
		else
		{
			::WritePrivateProfileString(L"FORDER_INFO", L"DIR", m_TargetFolder.c_str(), WSTR_CONFIG_FILE.c_str());
		}
	}	

	return	true;
}

//-----------------------------------------------------------------------------
//  폴더를 선택
//-----------------------------------------------------------------------------
bool CQCCore::FolderSelect(const HWND& hWnd, std::wstring& wstrFolderName)
{
	BROWSEINFO			bi;

	bi.hwndOwner		= hWnd;	//부모윈도우핸들
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= NULL;
	bi.lpszTitle		= L"Set Dragonica-Dev Folder.";
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
