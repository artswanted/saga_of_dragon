#include "StdAfx.h"
#include "IFBase.h"
#include "MainWorker.h"
#include <shlwapi.h>

void BreakSepW(const std::wstring &rkText, std::vector<std::wstring> &rkVec, const std::wstring kSep)
{
	std::wstring kTemp = rkText;
	wchar_t *pToken = NULL;
	wchar_t *pNextToken = NULL;
	pToken = wcstok_s(&kTemp[0], kSep.c_str(), &pNextToken);
	if(NULL == pToken)
	{
		rkVec.push_back(rkText);
		return;
	}
	rkVec.push_back(pToken);

	while(1)
	{
		pToken = wcstok_s(NULL, kSep.c_str(), &pNextToken);
		if( pToken != NULL )
		{
			rkVec.push_back( pToken );
		}
		else
		{
			break;
		}
	}
}

MainWorker::MainWorker(void)
	: m_kSelectJob(EPT_NONE)
	, m_bIsAutomatic(false)
	, m_dwWorkFlag(0)
	, m_pIFControl(NULL)
	, m_pkLogList(NULL)
	, m_pkOptionList(NULL)
	, m_eVersionType(EVT_PATCH_VERSION)
{
	m_kTaskWorker.SetMainWorker(this);
}

MainWorker::~MainWorker(void)
{
	SAFE_DELETE(m_pkLogList);
	SAFE_DELETE(m_pkOptionList);
	BM::PgDataPackManager::Clear();
}

bool MainWorker::Initialize(std::wstring const& kCmdLine)
{
	BM::CPackInfo	kPackInfo;
	if(!kPackInfo.Init())
	{
		return false;
	}

	if(!BM::PgDataPackManager::Init(kPackInfo))
	{
		return false;
	}

	if (ParseCmdLine(kCmdLine) == true)
		return false;

	return true;
}

void MainWorker::SetSelectedJob(E_PROCESS_TYPE const Type)
{
	m_kSelectJob = Type;
	IFBase* pNewIF = GetIFControl(m_kSelectJob);
	if( pNewIF )
	{
		CONT_IF_TEXTDATA::iterator txt_iter = m_kIFTextData.find(Type);
		if( txt_iter != m_kIFTextData.end() )
		{
			CONT_IF_TEXTDATA::mapped_type const& kTxtData = txt_iter->second;
			
			pNewIF->ChangeIF(kTxtData.kContents, kTxtData.kStatic1, kTxtData.kStatic2, kTxtData.kStatic3);

			IFBase* pTemp = m_pIFControl;
			m_pIFControl = pNewIF;
			delete pTemp;
		}
	}
}

bool MainWorker::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_RDO_DIFF:			{ SetSelectedJob(EPT_DIFF);				}break;
	case IDC_RDO_PACK:			{ SetSelectedJob(EPT_PACK);				}break;
	case IDC_RDO_CREATEHEADER:	{ SetSelectedJob(EPT_CREATELIST);		}break;
	case IDC_RDO_HDMERGE:		{ SetSelectedJob(EPT_HEADERMERGE);		}break;
	case IDC_RDO_BINDPACK:		{ SetSelectedJob(EPT_BIND);				}break;
	case IDC_RDO_UNPACK:		{ SetSelectedJob(EPT_UNPACK);			}break;
	case IDC_RDO_MAKEINB:		{ SetSelectedJob(EPT_MAKEINB);			}break;
	case IDC_RDO_EXPORT_DATLIST:{ SetSelectedJob(EPT_EXPORT_LIST);		}break;
	case IDC_RDO_MAKEID:		{ SetSelectedJob(EPT_CREATE_ID);		}break;
	case IDC_RDO_UPDATE_DAT_VER:{ SetSelectedJob(EPT_DAT_VERSION_UP);	}break;
	case IDC_RDO_DAT_VER_CHECK:	{ SetSelectedJob(EPT_DAT_VERSION_CHECK);}break;
	case IDC_RDO_CONVERT_DAT:	{ SetSelectedJob(EPT_DAT_CONVERT);		}break;
	case IDC_RDO_MAKEMANUALPATCH:	{ SetSelectedJob(EPT_MAKE_MANUAL_PATCH);		}break;
	case IDC_RDO_MAKEAUTOPATCH:	{ SetSelectedJob(EPT_MAKE_AUTO_PATCH);		}break;
	case IDC_RDO_PREVID:	 { 	m_eVersionType = EVT_PREV_VERSION;	}break;
	case IDC_RDO_PATCHID:	{  m_eVersionType = EVT_PATCH_VERSION; }break;
	case IDC_BTN_START:			{ StartWork(); }break;
	case IDC_BTN_FIXID:			{ UpdateVersion(); }break;
	case IDCANCEL:				{ EndDialog(GethWnd(), 0); }return TRUE;
	default:
		{
			if( m_pIFControl )
			{
				return m_pIFControl->Input(wParam, lParam);
			}
		}break;
	}
	return false;
}

void MainWorker::StartWork()
{
	if( EWT_PROGRESS == m_kProgInfo.State )
	{
		::MessageBoxW(m_hMainWnd, GetMSG(EWMT_ALREADY_PROCESS).c_str(), GetMSG(EWMT_NOTICE_HEADER).c_str(), MB_OK);
		return;
	}

	S_PROCESS_MSG kMsg(GetSelectedJob());
	switch( kMsg.kType )
	{
	case EPT_DIFF:
	case EPT_HEADERMERGE:
	case EPT_MAKE_AUTO_PATCH:
		{
			kMsg.kCRTDir = GetWndText(IDC_EDT_TEXT2);

			if( kMsg.kType == EPT_DIFF )
			{
				kMsg.dwOption = (m_pIFControl)?(m_pIFControl->GetOption()):(0x00000000);
			}
		}//nonebreak;
	case EPT_PACK:
	case EPT_CREATELIST:
	case EPT_BIND:
	case EPT_UNPACK:
	case EPT_MAKEINB:
	case EPT_EXPORT_LIST:
	case EPT_DAT_VERSION_UP:
	case EPT_DAT_VERSION_CHECK:
	case EPT_DAT_CONVERT:
	case EPT_MAKE_MANUAL_PATCH:
		{
			kMsg.kTGTDir = GetWndText(IDC_EDT_TEXT1);
#ifdef _DEBUGa
			int iOption = m_pkOptionList->GetSelectedItemIndex();
			if (iOption == 0)
			{
				// crc
				BM::g_bCalcSHA2CheckSum = false;
			}
			else
			{
				// sha
				BM::g_bCalcSHA2CheckSum = true;
			}
#endif

			if( kMsg.kType == EPT_CREATELIST )
			{
				kMsg.dwOption = (m_pIFControl)?(m_pIFControl->GetOption()):(0x00000000);
			}
		}//nonebreak;
	case EPT_CREATE_ID:
		{
			kMsg.kOUTDir = GetWndText(IDC_EDT_TEXT3);
		}break;
	case EPT_NONE:
	default:
		{
			return;
		}
	}
	m_kTaskWorker.PutMsg(kMsg);
}

std::wstring MainWorker::GetWndText(DWORD ControlID)
{
	wchar_t	szTemp[MAX_PATH] = {0,};
	GetWindowText(GetDlgItem(GethWnd(), ControlID), szTemp, MAX_PATH);
	return std::wstring(szTemp);
}

bool MainWorker::SetIniData()
{
	m_pkLogList = new LogList(GethWnd(), IDC_LST_LOG);
	if( !m_pkLogList || !m_pkLogList->Initialize() )
	{
		return false;
	}

	m_pkOptionList = new OptionList(GethWnd(), IDC_LST_OPTION);
	if( !m_pkOptionList || !m_pkOptionList->Initialize() )
	{
		return false;
	}

	m_pkOptionList->AddItem(std::wstring(L"@CRC"));
	m_pkOptionList->AddItem(std::wstring(L"@SHA512"));

	const wchar_t APP_TEXTTABLE_FILE[] =  L".\\TextTable.ini";

	if( !::PathFileExistsW(APP_TEXTTABLE_FILE) )
	{
		return false;
	}
	
	wchar_t szTemp[ 2048 ] = {0,};

	::GetPrivateProfileStringW(L"TEXT", L"JOB", L"Job", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_JOB), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"CONTENTS", L"Contents", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_CONTENTS), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"OPTION", L"Option", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_OPTION), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"PATCHID", L"PatchID", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_PATCHOUT), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"MAJOR", L"Major", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_MAJOR), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"MINOR", L"Minor", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_MINOR), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"TINY", L"Tiny", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_TINY), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"START", L"Start", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_BTN_START), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"UPDATE", L"ID Fix", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_BTN_FIXID), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"DIFF", L"Diff", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_DIFF), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"PACK", L"Pack", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_PACK), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"CREATELIST", L"Create Header File", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_CREATEHEADER), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"HEADERMERGE", L"HeaderMerge", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_HDMERGE), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"BIND", L"BindPack", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_BINDPACK), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"UNPACK", L"ID Fix", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_UNPACK), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"MAKEINB", L"Convert INI To INB", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_MAKEINB), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"EXPORT_LIST", L"Export DAT File List", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_EXPORT_DATLIST), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"CREATE_ID", L"Create Patch.ID", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_MAKEID), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"DAT_VERSION_UP", L"DAT Version Up", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_UPDATE_DAT_VER), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"FIX_PATCH_ID", L"Fixed Patch ID", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_FIXEDID), szTemp);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_PATCHID), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"PREV_PATCH_ID", L"Prev Patch ID", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_PREVID), szTemp);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_PREVID), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"DAT_VERSION_CHECK", L"DAT Version Check", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_DAT_VER_CHECK), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"CONVERT_DAT", L"Convert DAT", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_CONVERT_DAT), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"MAKEMANUALPATCH", L"Make Manual Patch", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_MAKEMANUALPATCH), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"MAKEAUTOPATCH", L"Make Auto Patch", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_RDO_MAKEAUTOPATCH), szTemp);

	::GetPrivateProfileStringW(L"TEXT", L"LOG", L"Log", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
	::SetWindowText(GetDlgItem(GethWnd(), IDC_STC_LOG), szTemp);

	for( int i = 0; i < EPT_END; ++i )
	{
		BM::vstring vStr(L"TEXT_JOB");
		vStr += i;

		SIFTextData kTextData;

		if( EPT_NONE == i )
		{
			kTextData.kJobName = L"";
		}
		else
		{
			GetWindowText(GetDlgItem(GethWnd(), IDC_RDO_DIFF + (i - 1)), szTemp, 2048);
			kTextData.kJobName = szTemp;
		}
		::GetPrivateProfileStringW(vStr.operator const std::wstring &().c_str(), L"CONTENTS", L"", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
		kTextData.kContents = szTemp;
		::GetPrivateProfileStringW(vStr.operator const std::wstring &().c_str(), L"STATIC1", L"", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
		kTextData.kStatic1 = szTemp;
		::GetPrivateProfileStringW(vStr.operator const std::wstring &().c_str(), L"STATIC2", L"", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
		kTextData.kStatic2 = szTemp;
		::GetPrivateProfileStringW(vStr.operator const std::wstring &().c_str(), L"STATIC3", L"", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);
		kTextData.kStatic3 = szTemp;

		m_kIFTextData.insert(std::make_pair(static_cast<E_PROCESS_TYPE>(i), kTextData));
	}

	for( int i = 0; i < EWMT_END; ++i )
	{
		BM::vstring vStr(L"MESSAGE");
		vStr += i;

		::GetPrivateProfileStringW(L"MESSAGE", vStr.operator const std::wstring &().c_str(), L"", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);

		m_kMessage.insert(std::make_pair(i, szTemp));
	}

	for( int i = 0; i < ELM_ENUM_END; ++i )
	{
		BM::vstring vStr(L"MESSAGE");
		vStr += i;

		::GetPrivateProfileStringW(L"LOG_MESSAGE", vStr.operator const std::wstring &().c_str(), L"", szTemp, sizeof(szTemp), APP_TEXTTABLE_FILE);

		m_kLogMessage.insert(std::make_pair(i, szTemp));
	}

	return true;
}

std::wstring const& MainWorker::GetMSG(E_WORK_MESSAGE_TYPE const Type)
{
	static std::wstring	kText;
	CONT_MESSAGE::iterator msg_itor = m_kMessage.find(Type);
	if( msg_itor != m_kMessage.end() )
	{
		return msg_itor->second;
	}
	return kText;
}

void MainWorker::StartButtonLock(bool bDisable)
{
	::EnableWindow(::GetDlgItem(GethWnd(), IDC_BTN_START), !bDisable);
}

void MainWorker::ShowMessage(E_WORK_STATE const State, E_PROCESS_TYPE const Type, std::wstring const& kMessage)
{
	E_WORK_MESSAGE_TYPE MsgType = EWMT_END;
	std::wstring kNoticeMessage;
	std::wstring kAddon;
	wchar_t szTemp[MAX_PATH] = {0,};

	CONT_IF_TEXTDATA::iterator txt_itor = m_kIFTextData.find(Type);
	if( txt_itor != m_kIFTextData.end() )
	{
		swprintf_s(szTemp, MAX_PATH, GetMSG(MsgType).c_str(), txt_itor->second.kJobName.c_str());
		kNoticeMessage += szTemp;
	}

	if( !kAddon.empty() )
	{
		swprintf_s(szTemp, MAX_PATH, kAddon.c_str(), kMessage.c_str());

		kNoticeMessage += L"\n";
		kNoticeMessage += szTemp;
	}

	::MessageBoxW(m_hMainWnd, kNoticeMessage.c_str(), GetMSG(EWMT_NOTICE_HEADER).c_str(), MB_OK);
}

void MainWorker::LogMessage(E_PROCESS_TYPE const Type, E_LOG_MSG const LogTypeNo, std::wstring const& kAddonText)
{
	switch( LogTypeNo )
	{
	case ELM_START:
		{ 
			m_kProgInfo.State = EWT_PROGRESS;
			StartButtonLock(true);
		}break;
	case ELM_END:
	case ELM_FAIL:
		{ 
			m_kProgInfo.State = EWT_PREPARE; 
			StartButtonLock(false);
		}break;
	}

	std::wstring kProcessName;

	CONT_IF_TEXTDATA::iterator if_itor = m_kIFTextData.find(Type);
	if( if_itor != m_kIFTextData.end() )
	{
		CONT_IF_TEXTDATA::mapped_type const& kData = if_itor->second;
		kProcessName = kData.kJobName;
	}

	std::wstring kMessage;
	CONT_MESSAGE::iterator txt_iter = m_kLogMessage.find(LogTypeNo);
	if( txt_iter != m_kLogMessage.end() )
	{
		CONT_MESSAGE::mapped_type const& kMsg = txt_iter->second;
		kMessage = kMsg + kAddonText;
	}

	wchar_t szTemp[ 2048 ] = {0,};
	swprintf_s(szTemp, 2048, m_pkLogList->ColumnValueDesign().c_str(), kProcessName.c_str(), kMessage.c_str());
	m_pkLogList->AddItem(std::wstring(szTemp));

	for(int i = m_pkLogList->GetTapCol(); i > 500; --i)
	{
		m_pkLogList->DelItem(0);
	}
}

bool MainWorker::ParseCmdLine(std::wstring const& kCmdLine)
{
	if (kCmdLine.size() > 0)
	{
		std::vector<std::wstring> kVec;
		BreakSepW(kCmdLine, kVec, L" ");
		const std::wstring wstrCmd = kVec.at(0);
		if (wstrCmd == TEXT("-automake_onepack"))
		{
			if (kVec.size() < 2)
			{
				return false;
			}

			PackTask kTask;
			S_PROCESS_MSG kMsg;
			kMsg.kType = EPT_BIND;
			kMsg.kTGTDir = kVec.at(1);
			kTask.HandleMessage(&kMsg);
			return true;
		}
		else if (wstrCmd == TEXT("-create_id"))
		{
			// Target dir
			// Major
			// Minor
			// Tiny
			if (kVec.size() < 5)
			{
				return false;
			}

			PackTask kTask;
			S_PROCESS_MSG kMsg;
			BM::VersionInfo kVer;

			kVer.Version.i16Major = _wtoi(kVec.at(2).c_str());
			kVer.Version.i16Minor = _wtoi(kVec.at(3).c_str());
			kVer.Version.i32Tiny = _wtoi(kVec.at(4).c_str());

			kMsg.kType = EPT_CREATE_ID;
			kMsg.kOUTDir = kVec.at(1);

			kTask.SetPatchVersionInfo(kVer);
			kTask.HandleMessage(&kMsg);
			return true;
		}
		else if(wstrCmd == TEXT("-pack"))
		{
			// Target dir
			// Out dir
			// Major
			// Minor
			// Tiny
			if (kVec.size() < 6)
			{
				return false;
			}
			
			PackTask kTask;
			S_PROCESS_MSG kMsg;
			BM::VersionInfo kVer;

			kVer.Version.i16Major = _wtoi(kVec.at(3).c_str());
			kVer.Version.i16Minor = _wtoi(kVec.at(4).c_str());
			kVer.Version.i32Tiny = _wtoi(kVec.at(5).c_str());

			kMsg.kType = EPT_PACK;
			kMsg.kTGTDir = kVec.at(1);
			kMsg.kOUTDir = kVec.at(2);

			kTask.SetPatchVersionInfo(kVer);
			kTask.HandleMessage(&kMsg);
		}
	}
	return false;
}

IFBase* MainWorker::GetIFControl(E_PROCESS_TYPE const Type)
{
	switch( Type )
	{
	case EPT_DIFF:				{ return new IFDiff(GethWnd());			}
	case EPT_PACK:				{ return new IFPack(GethWnd());			}
	case EPT_CREATELIST:		{ return new IFCreateList(GethWnd());	}
	case EPT_HEADERMERGE:		{ return new IFHeaderMerge(GethWnd());	}
	case EPT_BIND:				{ return new IFBind(GethWnd());			}
	case EPT_UNPACK:			{ return new IFUnBind(GethWnd());		}
	case EPT_MAKEINB:			{ return new IFDMakeINB(GethWnd());		}
	case EPT_EXPORT_LIST:		{ return new IFExportList(GethWnd());	}
	case EPT_CREATE_ID:			{ return new IFCreateID(GethWnd());		}
	case EPT_DAT_VERSION_UP:	{ return new IFDatVersionUp(GethWnd()); }
	case EPT_DAT_VERSION_CHECK:	{ return new IFDatVersionCheck(GethWnd()); }
	case EPT_DAT_CONVERT:		{ return new IFDatConvert(GethWnd()); }
	case EPT_MAKE_MANUAL_PATCH:	{ return new IFMakeManualPatch(GethWnd()); }
	case EPT_MAKE_AUTO_PATCH:	{ return new IFMakeAutoPatch(GethWnd()); }
	case EPT_NONE:
	default:
		{
			 return new IFDefault(GethWnd());
		}
	}
	return NULL;
}

void MainWorker::UpdateVersion()
{
	BM::VersionInfo VInfo;
	VInfo.Version.i16Major = _wtoi(GetWndText(IDC_EDT_MAJOR).c_str());
	VInfo.Version.i16Minor = _wtoi(GetWndText(IDC_EDT_MINOR).c_str());
	VInfo.Version.i32Tiny = _wtoi(GetWndText(IDC_EDT_TINY).c_str());

	if( EVT_PATCH_VERSION == m_eVersionType )
	{
		UpdatePatchVersion(VInfo);
	}
	else
	{
		UpdatePrevVersion(VInfo);
	}

	::SetWindowTextW(GetDlgItem(GethWnd(), IDC_EDT_MAJOR), L"");
	::SetWindowTextW(GetDlgItem(GethWnd(), IDC_EDT_MINOR), L"");
	::SetWindowTextW(GetDlgItem(GethWnd(), IDC_EDT_TINY), L"");
}

void MainWorker::UpdatePrevVersion(BM::VersionInfo VInfo)
{
	wchar_t szTemp[MAX_PATH] = {0,};
	swprintf_s(szTemp, MAX_PATH, L"%d.%d.%d", static_cast<WORD>(VInfo.Version.i16Major), static_cast<WORD>(VInfo.Version.i16Minor), static_cast<UINT>(VInfo.Version.i32Tiny));

	m_kTaskWorker.SetPrevVersionInfo(VInfo);
	::SetWindowTextW(GetDlgItem(GethWnd(), IDC_STC_PREVPATCHID), szTemp);
}
void MainWorker::UpdatePatchVersion(BM::VersionInfo VInfo)
{
	wchar_t szTemp[MAX_PATH] = {0,};
	swprintf_s(szTemp, MAX_PATH, L"%d.%d.%d", static_cast<WORD>(VInfo.Version.i16Major), static_cast<WORD>(VInfo.Version.i16Minor), static_cast<UINT>(VInfo.Version.i32Tiny));

	m_kTaskWorker.SetPatchVersionInfo(VInfo);
	::SetWindowTextW(GetDlgItem(GethWnd(), IDC_STC_PATCHID), szTemp);
}