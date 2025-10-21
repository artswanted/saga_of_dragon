#include "defines.h"
#include "PatchProcess.h"
#include "PgEnvInfoParser.h"
#include "SiteControl.h"
#include "OriginalPatch.h"
#include "CTray.h"
//#define _TEST_

wchar_t const* const DAT = L".//res.dat";
wchar_t const* const RES = L"./Res/";
wchar_t const* const MAIN_BG = L"MAIN_BG";
wchar_t const* const PRGS_BAR1 = L"PROGRASS_BAR1";
wchar_t const* const PRGS_BAR2 = L"PROGRASS_BAR2";

CPatchProcess::CPatchProcess(void)
: m_pkPlay(NULL)
, m_bReloadResDat(false)
, m_bIsDestroying(false)
{	
//#define DEFAULT
//#define TAIWAN
//#define RUSSIA
#ifndef TAIWAN
	//기본 디폴트 메세지
	m_kContDefalutMsg.reserve(E_MSG_MAX);
	m_kContDefalutMsg.push_back(L"Failed to initialize Web Browser");	// 0
	m_kContDefalutMsg.push_back(L"Connecting to server");
	m_kContDefalutMsg.push_back(L"Patcher is already running.");
	m_kContDefalutMsg.push_back(L"Game client is already running.");
	m_kContDefalutMsg.push_back(L"Merging File : ");
	m_kContDefalutMsg.push_back(L"Receiving from Server List...");		// 5
	m_kContDefalutMsg.push_back(L"Creating Client List...");
	m_kContDefalutMsg.push_back(L"Comparing List...");
	m_kContDefalutMsg.push_back(L"Comparison completed. Execute Update.");
	m_kContDefalutMsg.push_back(L"Comparison completed. Let’s Play Game.");
	m_kContDefalutMsg.push_back(L"Calculating Total Size...");			// 10
	m_kContDefalutMsg.push_back(L"List Sorting...");
	m_kContDefalutMsg.push_back(L"Patching File :  ");
	m_kContDefalutMsg.push_back(L"Failed to Download ");
	m_kContDefalutMsg.push_back(L"Read file into memory");
	m_kContDefalutMsg.push_back(L"Move File to corresponding directory."); // 15
	m_kContDefalutMsg.push_back(L"Data size of Pack and Header Size...");
	m_kContDefalutMsg.push_back(L"Creating and Allocating files...");
	m_kContDefalutMsg.push_back(L"Downloading File : ");
	m_kContDefalutMsg.push_back(L"Data Load has been Failed...");
	m_kContDefalutMsg.push_back(L"Copy File To Pack..."); //20
	m_kContDefalutMsg.push_back(L"Merging Source File and Downloaded file");
	m_kContDefalutMsg.push_back(L"Compare list only");
	m_kContDefalutMsg.push_back(L"Start Patching");
	m_kContDefalutMsg.push_back(L"Patching Completed");
	m_kContDefalutMsg.push_back(L"Start Client Offline Patching"); //25
	m_kContDefalutMsg.push_back(L"Failed to read ini file");
	m_kContDefalutMsg.push_back(L"Failed to initialize Manager");
	m_kContDefalutMsg.push_back(L"CRC Info is broken");
	m_kContDefalutMsg.push_back(L"Cannot find corresponding launch folder for the program");
	m_kContDefalutMsg.push_back(L"Retrieved List is empty"); //30
	m_kContDefalutMsg.push_back(L"Calculating File Size");
	m_kContDefalutMsg.push_back(L"Initializing Patcher..");
	m_kContDefalutMsg.push_back(L"Failed to connect Server");
	m_kContDefalutMsg.push_back(L"Decoding Patch.ID password");
	m_kContDefalutMsg.push_back(L"Wait, sub Patcher not closed"); //35
	m_kContDefalutMsg.push_back(L"Failed to Patch");
	m_kContDefalutMsg.push_back(L"Patch Canceled");
	m_kContDefalutMsg.push_back(L"New version found. Do you want update?");
	m_kContDefalutMsg.push_back(L"Your client is latest version. Do you want recovery?");
	m_kContDefalutMsg.push_back(L"Patch canceled"); // 40
	m_kContDefalutMsg.push_back(L"Can't run game client");
	m_kContDefalutMsg.push_back(L"Can't find correct version info. Do you want to start recovery mode?");
	m_kContDefalutMsg.push_back(L"Change language"); // 43
	m_kContDefalutMsg.push_back(L"Failed initializate update");
	m_kContDefalutMsg.push_back(L"Allocating");
	m_kContDefalutMsg.push_back(L"Checking files...");
	m_kContDefalutMsg.push_back(L"Download...");
#else
	//대만 디폴트 메세지
	m_kContDefalutMsg.reserve(E_MSG_MAX);
	m_kContDefalutMsg.push_back(L"WEB瀏覽器初始化失敗");		// 0
	m_kContDefalutMsg.push_back(L"嘗試連線伺服器");
	m_kContDefalutMsg.push_back(L"Patcher已經執行時");
	m_kContDefalutMsg.push_back(L"Client已經執行中");
	m_kContDefalutMsg.push_back(L"檔案合併中 ");
	m_kContDefalutMsg.push_back(L"從伺服器列表取得中");			// 5
	m_kContDefalutMsg.push_back(L"遊戲正在更新中，請耐心等候，勿關閉視窗");
	m_kContDefalutMsg.push_back(L"列表比較中");
	m_kContDefalutMsg.push_back(L"完成比較。須更新");
	m_kContDefalutMsg.push_back(L"完成比較，可進行遊戲");
	m_kContDefalutMsg.push_back(L"計算總計大小");			// 10
	m_kContDefalutMsg.push_back(L"目錄分割");
	m_kContDefalutMsg.push_back(L"更新中的檔案");
	m_kContDefalutMsg.push_back(L"下載檔案失敗");
	m_kContDefalutMsg.push_back(L"記憶體中讀取檔案");
	m_kContDefalutMsg.push_back(L"將檔案移至相關目錄");		// 15
	m_kContDefalutMsg.push_back(L"封包的資料大小及檔頭大小");
	m_kContDefalutMsg.push_back(L"檔案生成及分割");
	m_kContDefalutMsg.push_back(L"接收檔案中 ");
	m_kContDefalutMsg.push_back(L"資料載入失敗");
	m_kContDefalutMsg.push_back(L"複製至封包");			// 20
	m_kContDefalutMsg.push_back(L"將原檔和下載檔合併");
	m_kContDefalutMsg.push_back(L"只比較列表");
	m_kContDefalutMsg.push_back(L"更新開始");
	m_kContDefalutMsg.push_back(L"更新完成");
	m_kContDefalutMsg.push_back(L"下線更新開始");			// 25
	m_kContDefalutMsg.push_back(L"讀取 ini 檔失敗");
	m_kContDefalutMsg.push_back(L"Manager初始化失敗");
	m_kContDefalutMsg.push_back(L"CRC 資訊被破壞");
	m_kContDefalutMsg.push_back(L"呼叫patcher替換程式失敗");
	m_kContDefalutMsg.push_back(L"取得的列表是空的");			// 30
	m_kContDefalutMsg.push_back(L"大小計算累積");
	m_kContDefalutMsg.push_back(L"Patcher 初始化");
	m_kContDefalutMsg.push_back(L"伺服器連線失敗");
	m_kContDefalutMsg.push_back(L"解讀更新ID暗號");
	m_kContDefalutMsg.push_back(L"下位Patcher尚未結束，請稍等。");				// 35
	m_kContDefalutMsg.push_back(L"更新失敗");
	m_kContDefalutMsg.push_back(L"更新被取消");
	m_kContDefalutMsg.push_back(L"找到新版本，確定要更新嗎?");
	m_kContDefalutMsg.push_back(L"Your client is latest version. Do you want recovery?");
	m_kContDefalutMsg.push_back(L"Patch canceled"); // 40
	m_kContDefalutMsg.push_back(L"Can't run game client");
	m_kContDefalutMsg.push_back(L"Can't find correct version info. Do you want to start recovery mode?");
#endif
}

CPatchProcess::~CPatchProcess(void)
{
	CONT_IMG::iterator	img_iter = m_kContImg.begin();
	while( img_iter != m_kContImg.end() )
	{
		CONT_IMG::mapped_type& kElement = img_iter->second;
		SAFE_DELETE(kElement);
		img_iter = m_kContImg.erase(img_iter);
	}

	CONT_BUTTON::iterator	btn_iter = m_kContBtn.begin();
	while( btn_iter != m_kContBtn.end() )
	{
		CONT_BUTTON::mapped_type& kElement = btn_iter->second;
		SAFE_DELETE(kElement);
		btn_iter = m_kContBtn.erase(btn_iter);
	}

	CONT_EDIT_LIST::iterator edit_itr = m_kContEdit.begin();
	while( edit_itr != m_kContEdit.end() )
	{
		CONT_EDIT_LIST::mapped_type& kElement = edit_itr->second;
		SAFE_DELETE(kElement);
		edit_itr = m_kContEdit.erase(edit_itr);
	}

	SAFE_DELETE(m_pkPlay);
	SAFE_DELETE(m_kSubApp);
}

//	초기세팅
void CPatchProcess::Init(const std::wstring& CmdLine)
{	
	m_hWndOpDlg		= NULL;
	m_pkConfigXml	= NULL;	
	//	기본 정보
	InitVariable();
	ConnectInit();
	CmdLineParse(CmdLine);
	// ResDat 로드
	LoadResDat();
	MakeExeCRC(L"");	// 패쳐 CRC

	// 최초 실행시 Patch.ID 버전을 읽어와 저장한다
	BM::VersionInfo const kNowVersionInfo = GetPatchID(L"patch.id");
	wchar_t kMajorVer[30] = {0,};
	wchar_t kMinorVer[30] = {0,};
	wchar_t kTinyVer[30] = {0,};
	_i64tow_s(kNowVersionInfo.Version.i16Major, kMajorVer, 30, 10);
	_i64tow_s(kNowVersionInfo.Version.i16Minor, kMinorVer, 30, 10);
	_i64tow_s(kNowVersionInfo.Version.i32Tiny, kTinyVer, 30, 10);	
	SetIsOldVersion(kNowVersionInfo.iVersion);

	std::wstring wstrClientVersion;
	wstrClientVersion = WSTR_GAME_NAME + L" " + WSTR_VERSION + L": " + std::wstring(kMajorVer) + L"." + std::wstring(kMinorVer) + L"." + std::wstring(kTinyVer);
	SetIsClientVer(wstrClientVersion);
}

bool CPatchProcess::LoadRes(std::wstring filename, std::vector<char> &kData)
{
	if( m_bUsePack )
		return BM::PgDataPackManager::LoadFromPack(DAT, filename, kData);
	return BM::FileToMem((BM::vstring(RES) << filename), kData);
}

void CPatchProcess::LoadResDat()
{
	BM::PgDataPackManager::ConvertToFormattedDataFile(DAT);
	CONT_IMG::iterator	img_iter = m_kContImg.begin();
	while( img_iter != m_kContImg.end() )
	{
		CONT_IMG::mapped_type& kElement = img_iter->second;
		SAFE_DELETE(kElement);
		img_iter = m_kContImg.erase(img_iter);
	}

	CONT_BUTTON::iterator	btn_iter = m_kContBtn.begin();
	while( btn_iter != m_kContBtn.end() )
	{
		CONT_BUTTON::mapped_type& kElement = btn_iter->second;
		SAFE_DELETE(kElement);
		btn_iter = m_kContBtn.erase(btn_iter);
	}
	SAFE_DELETE(m_pkPlay);
	m_kEnviData.Clear();
	g_kSiteControl1.Clear();
	g_kSiteControl2.Clear();

	{// 팩된 설정값을 xml로 부터 정보를 얻어온다
		PgEnvInfoParser kParser;
		wchar_t const* const ENV_XML = L"environment.xml";
		kParser.ParseFromPack(ENV_XML, m_kEnviData);
	}
	std::vector<char> kData;	
	{// 배경 이미지	
		std::wstring const& kBGImg = m_kEnviData.kBgImg;
		LoadRes(kBGImg.c_str(), kData);
		POINT kPos = {0,0};
		BitmapLoadAndSetPos(HBT_DLG_NORMAL, kPos, kData);
	}
	{// 서브 배경 이미지	
		std::wstring const& kBGImg = m_kEnviData.kSubBgImg;	
		LoadRes(kBGImg.c_str(), kData);
		POINT kPos = {0,0};
		BitmapLoadAndSetPos(HBT_DLG_SUB, kPos, kData);
	}	
	{//프로그래스 바
		SPrograssBarInfo const& kPrgsImg1 = m_kEnviData.kProgressImg1;
		LoadRes(kPrgsImg1.kImg.c_str(), kData);
		BitmapLoadAndSetPos(HBT_PROGRESS1, kPrgsImg1.kPos, kData);
		SPrograssBarInfo const& kPrgsImg2 = m_kEnviData.kProgressImg2;
		LoadRes(kPrgsImg2.kImg.c_str(), kData);
		BitmapLoadAndSetPos(HBT_PROGRESS2, kPrgsImg2.kPos, kData);
	}
	SetWindowFrame();	
	{//	브라우저
		m_pkPlay = new PgAXPlay;
		SBRInfo& kBRInfo = m_kEnviData.kBRInfo;
		if( g_WinApp )
		{
			m_pkPlay->CreateWB(g_WinApp->GetHandle(), g_WinApp->GetInstance(), MB(kBRInfo.kLink), kBRInfo.kPos.x, kBRInfo.kPos.y, kBRInfo.iWidth, kBRInfo.iHeight);
		}
	}
	
	{// 버튼 등록
		SEnvData::CONT_BTNINFO const& rkContBtn = m_kEnviData.kContBtnInfo;
		SEnvData::CONT_BTNINFO::const_iterator itor = rkContBtn.begin();
		while(itor != rkContBtn.end())
		{
			if( CCTButton::eBTN_SUB_CANCEL == itor->second.iType )
			{
				++itor;
				continue;
			}

			int const& ID = itor->first;
			SBtnInfo const& kBtnInfo = (itor->second);
			LoadRes(kBtnInfo.kImg.c_str(), kData);
			ButtonInit(ID, kBtnInfo.iType, kBtnInfo.kPos.x, kBtnInfo.kPos.y, 
				kBtnInfo.iShapeNum, 0, kBtnInfo.kText, kBtnInfo.kLink, kData);	
			++itor;
		}
	}

	{//에디트 박스도 넣어보자.
		CONT_IMG::iterator img_itr = m_kContImg.find(HBT_DLG_NORMAL);
		if( img_itr != m_kContImg.end() )
		{
			SEnvData::CONT_EDITINFO const& rkContEdit = m_kEnviData.kContEditInfo;
			SEnvData::CONT_EDITINFO::const_iterator itor = rkContEdit.begin();
			while( itor != rkContEdit.end() )
			{
				SEnvData::CONT_EDITINFO::mapped_type const& kEditData = itor->second;

				CTEdit* pkEdit = new CTEdit();
				if( pkEdit )
				{
					pkEdit->Init( g_WinApp->GetInstance(), g_WinApp->GetHandle(), kEditData, img_itr->second );
					m_kContEdit.insert( std::make_pair( pkEdit->GetHandle(), pkEdit ) );
				}
				++itor;
			}			
		}
	}

	CONT_IMG::iterator bmp_iter = m_kContImg.find(GetLocalImageNumber());
	if( bmp_iter != m_kContImg.end() )
	{
		g_kSiteControl1.SetParentBitmap(bmp_iter->second);
		g_kSiteControl2.SetParentBitmap(bmp_iter->second);
	}	
	BM::PgDataPackManager::Clear();
}

bool CPatchProcess::SubInit()
{
	if( m_kSubApp )
	{
		return false;
	}

	m_kSubApp = new Window(NULL, g_WinApp->GetInstance());
	if( !m_kSubApp )
	{
		return false;
	}

	BM::vstring title;
	#ifdef RUSSIA
	title = L"Драконика";
	#else
	title = WSTR_GAME_NAME;
	title += L" (";
	title += PG_PRODUCT_VERSION_STRW;
	title += L")";
	#endif

	m_kSubApp->Initialize(title, WS_POPUP|WS_CLIPCHILDREN, WS_EX_LAYERED);
	{
		SWindowIconInfo	kIconInfo;
		kIconInfo.hIcon		= LoadIcon(g_WinApp->GetInstance(), IDI_APPLICATION);
		kIconInfo.hIconSm	= LoadIcon(g_WinApp->GetInstance(), IDI_APPLICATION);
		if( !m_kSubApp->Register(kIconInfo, SubProc) )
		{
			WinMessageBox(g_PProcess.GetVisibleWindowHandle(), GetMsg(E_SUBWNDRUNNING), WSTR_GAME_NAME.c_str(), MB_OK);
			return false;
		}
	}

	if( !m_kSubApp->Create() )
	{
		return false;
	}

	SEnvData::CONT_BTNINFO::const_iterator env_iter = m_kEnviData.kContBtnInfo.begin();
	while( env_iter != m_kEnviData.kContBtnInfo.end() )
	{
		if( CCTButton::eBTN_SUB_CANCEL != env_iter->second.iType )
		{
			++env_iter;
			continue;
		}

		CCTButton *pkElement = new CCTButton;
		auto rst = m_kContBtn.insert(std::make_pair(env_iter->first, pkElement));
		if( rst.second )
		{
			CONT_BUTTON::mapped_type &kElement = rst.first->second;

			std::vector<char> kData;
			LoadRes(env_iter->second.kImg.c_str(), kData);
			kElement->Init(g_WinApp->GetInstance(), m_kSubApp->GetHandle(), env_iter->first, env_iter->second.iType, env_iter->second.kPos, 
				env_iter->second.iShapeNum, 0, env_iter->second.kText, env_iter->second.kLink, kData);

			CONT_IMG::iterator bmp_iter = m_kContImg.find(GetLocalImageNumber(true));
			if( bmp_iter != m_kContImg.end() )
			{
				kElement->SetParentBitmap(bmp_iter->second);
			}
		}
		else
		{
			SAFE_DELETE(pkElement);
		}

		break;
	}
	//다 사용한 res.dat 캐싱을 삭제(프로그램이 뜬후에도 res.dat를 삭제가능)
	std::wstring kTempName(DAT);
	BM::ConvFolderMark(kTempName);
	BM::PgDataPackManager::ClearBuffer(kTempName);

	CONT_IMG::iterator	img_iter = m_kContImg.find(GetLocalImageNumber(true));
	if( img_iter != m_kContImg.end() )
	{
		CONT_IMG::mapped_type &kBmpElement = img_iter->second;
		SetLayeredWindowAttributes(m_kSubApp->GetHandle(), RGB(255, 0, 255), 255, LWA_COLORKEY);
		m_kSubApp->SetSize(kBmpElement->GetWidth(), kBmpElement->GetHeight());
		return true;		
	}
	return false;
}

void CPatchProcess::SubDisplay(bool bDisplay)
{
	if( m_kSubApp )
	{
		m_kSubApp->Display((bDisplay)?(SW_SHOW):(SW_HIDE));
	}
}

HWND CPatchProcess::GetVisibleWindowHandle(void) const
{
	if( SubWindowVisible() )
	{
		HWND hSubWnd = g_PProcess.SubWindowHandle();
		if( NULL != hSubWnd )
		{
			return hSubWnd;
		}
	}
	return g_WinApp->GetHandle();
};

HWND CPatchProcess::SubWindowHandle() const
{
	if( m_kSubApp )
	{
		return m_kSubApp->GetHandle();
	}
	return NULL;
}

bool CPatchProcess::SubWindowVisible() const
{
	if( m_kSubApp )
	{
		return m_kSubApp->IsShow();
	}
	return false;
}

void CPatchProcess::SubDestroy()
{
	if(!m_kSubApp)
	{
		return;
	}

	CONT_BUTTON::iterator iter = m_kContBtn.begin();
	while( iter != m_kContBtn.end() )
	{
		CONT_BUTTON::mapped_type const &kElement = (iter->second);
		if( CCTButton::eBTN_SUB_CANCEL == kElement->GetType() )
		{
			m_kContBtn.erase(iter);
			break;
		}
		++iter;
	}
	DestroyWindow(m_kSubApp->GetHandle());
	m_kSubApp->Terminate();
	g_WinApp->Display(SW_SHOW);
	SAFE_DELETE(m_kSubApp);
}

//
void CPatchProcess::InitVariable()
{
	//	핸들
	m_PackThread	= INVALID_HANDLE_VALUE;

	//	프로세스 설정
	m_eProcess		= eProcess_Preparing;
	m_bIsAuto		= false;
	m_bIsRecover	= false;
	m_bIsListCheck	= false;
	m_UseGameGard	= false;
	m_bNotPatched	= false;
	m_bUsePatcherPatch = true;
	m_bUseNewPatchMethod = false;
	m_bUseLibUpdate = false;

	//	출력정보
	m_fTotalPercent = 0.0f;
	m_fFilePercent	= 0.0f;
	m_TotalSize	= 0;
	m_FileSize	= 0;
	m_CntFileSize	= 0;
	m_CntTotalSize	= 0;
}

static inline const wchar_t* NationCodeToLang(std::wstring const& kWstr)
{
	if (kWstr == _T("EUROPE"))
		return _T("ENG");
	if (kWstr == _T("RUSSIA"))
		return _T("RUS");
	if (kWstr == _T("FRANCE"))
		return _T("FRA");
	return _T("ENG");
}

//	연결 정보 얻기
void CPatchProcess::ConnectInit()
{
	wchar_t	szTemp[MAX_PATH] = {0,};
//	m_ContUrl.clear();
	static std::wstring kSystemNationCode = UNI(g_kLocal.GetSystemNationCode());
	static std::wstring kSystemLang = NationCodeToLang(kSystemNationCode);
	//	나라정보
	GetPrivateProfileString(L"LOCAL_INFO", L"NATION_CODE", kSystemNationCode.c_str(), szTemp, sizeof(szTemp), WSTR_LOCALCONFIGFILE.c_str());	
	SetLocale(szTemp);
	kSystemNationCode = szTemp;

	{// sub adress(하위 폴더)정보가 있다면
		GetPrivateProfileString(L"LOCAL_INFO", WSTR_SUB_ADRESS.c_str(), L"", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());	
		std::wstring kTemp = szTemp;
		if(!kTemp.empty())
		{// 저장해 둔다
			m_wstrSubAddress = L"/";
			m_wstrSubAddress+=szTemp;
			m_wstrSubAddress+= L"/";
		}
	}

	//	패치 서버 정보
	BM::VersionInfo kMinimumVersionInfo;
	kMinimumVersionInfo.Version.i16Major = ::GetPrivateProfileInt(L"PATCHER", L"MINIMUM_MAJOR", 0, WSTR_PATCHERCONFIGFILE.c_str());
	kMinimumVersionInfo.Version.i16Minor = ::GetPrivateProfileInt(L"PATCHER", L"MINIMUM_MINOR", 0, WSTR_PATCHERCONFIGFILE.c_str());
	kMinimumVersionInfo.Version.i32Tiny = ::GetPrivateProfileInt(L"PATCHER", L"MINIMUM_TINY", 0, WSTR_PATCHERCONFIGFILE.c_str());

	SetMinimumVersion(kMinimumVersionInfo.iVersion);

	//	패치 서버 정보
	::GetPrivateProfileString(L"LOCAL_INFO", L"ADDRESS", L"", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	m_wstrAddr	= szTemp;

	//	패치 서버 포트
	m_wPort = ::GetPrivateProfileInt(L"LOCAL_INFO", L"PORT", 80, WSTR_PATCHERCONFIGFILE.c_str());

	//	패치 서버 아이디
	::GetPrivateProfileString(L"LOCAL_INFO", L"LOGIN_ID", L"", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	m_wstrID	= szTemp;

	//	패치 서버 패스워드
	::GetPrivateProfileString(L"LOCAL_INFO", L"LOGIN_PW", L"", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	m_wstrPW	= szTemp;

	// Web Site
	::GetPrivateProfileString(L"LOCAL_INFO", L"URL", L"", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	m_wstrUrl	= szTemp;

	//	로그 남길래?
	::GetPrivateProfileString(L"LOG", L"REPORT", L"0", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	std::wstring	wstrResult = szTemp;
	if(wstrResult.size() && (wstrResult.compare(L"1") == 0))
	{
		InitLogReport();
	}


	::GetPrivateProfileString(L"DEBUG", L"PatcherPatch", L"1", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	wstrResult = szTemp;
	if(wstrResult.size() && (wstrResult.compare(L"0") == 0))
	{
		m_bUsePatcherPatch = false;
	}


	::GetPrivateProfileString(L"DEBUG", L"UseLibUpdate", L"0", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	m_bUseLibUpdate = (bool)_wtoi(szTemp);

#ifdef PATCHER_DEBUG
	::GetPrivateProfileString(L"DEBUG", L"NewPatchMethod", L"0", szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	wstrResult = szTemp;
	if(wstrResult.size() && (wstrResult.compare(L"1") == 0))
	{
		m_bUseNewPatchMethod= true;
	}
#endif

	// download 버퍼 사이즈
	int iBufferSize = ::GetPrivateProfileInt(L"LOCAL_INFO", L"BUFFER_SIZE", DEFAULT_BUFFER_SIZE, WSTR_PATCHERCONFIGFILE.c_str());
	if (iBufferSize != DEFAULT_BUFFER_SIZE)
	{
		g_FTPMgr.SetBufferSize(static_cast<unsigned int>(iBufferSize));
	}

	//	엔씨 게임가드
	::GetPrivateProfileString(_T("LOCAL_INFO"), _T("NCGG"), _T(""), szTemp, sizeof(szTemp), WSTR_PATCHERCONFIGFILE.c_str());
	wstrResult	= szTemp;
	if(wstrResult.size() && (wstrResult.compare(L"1") == 0))
	{
		m_UseGameGard = true;
	}

	m_bUsePack = static_cast<bool>(::GetPrivateProfileInt(L"LOCAL_INFO", L"USEPACK", 1, WSTR_PATCHERCONFIGFILE.c_str()));
	
	{
		std::wstring kLanguage;
		::GetPrivateProfileString(L"LOCAL_INFO", L"LANGUAGE", kSystemLang.c_str(), szTemp, sizeof(szTemp), WSTR_LOCALCONFIGFILE.c_str());
		kLanguage = szTemp;
		kSystemLang = kLanguage;
		LoadMsgFromINI(kLanguage);
	}
	ChangeLanguage(MB(kSystemNationCode), MB(kSystemNationCode), MB(kSystemLang), ELCM_NO);
}

//	클라이언트 및 패쳐 중복 실행 체크
CPatchProcess::EOperatedError CPatchProcess::OperatedCheck()
{
	if( g_WinApp )
	{
#ifndef _DEBUG
		// 패쳐의 Mutex 생성
		CreateMutex(NULL, true, L"OnePatcherExecute");
		if(GetLastError()==ERROR_ALREADY_EXISTS)
		{
			return EOperatedError::PatcherExist;
		}

		HANDLE	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"Dragonica_Client");
		if(hMutex != NULL)
		{
			return EOperatedError::ClientExist;
		}
		else
		{
			CloseHandle(hMutex);
		}
#endif
	}
	return EOperatedError::Ok;
}

//	리커버리
bool CPatchProcess::FullChecking()
{
	static	Loki::Mutex	local_mutex;
	BM::CAutoMutex		lock(local_mutex);
	if(m_eProcess == eProcess_Preparing)
	{
		unsigned	ThreadID;
		m_bIsRecover = true;
		m_PackThread = (HANDLE)_beginthreadex(NULL, 0, &OnlinePatch, this, 0, &ThreadID);
	}
	return true;
}

//	업데이트
bool CPatchProcess::UpdateClient()
{
	static	Loki::Mutex	local_mutex;
	BM::CAutoMutex		lock(local_mutex);

	if(m_eProcess == eProcess_Preparing)
	{
		unsigned	ThreadID;

		if(_access("m_patch.id", 0) == -1)
		{
			m_PackThread = (HANDLE)_beginthreadex(NULL, 0, &OnlinePatch, this, 0, &ThreadID);
		}
		else
		{ // 수동 패치는 직접 .EXE File 을 통해 진행
			//m_PackThread = (HANDLE)_beginthreadex(NULL, 0, &OfflinePatch, this, 0, &ThreadID);
		}
	}
	return true;
}

//	게임 시작
bool CPatchProcess::StartGame()
{
	if( m_bNotPatched )
	{
		UpdateClient();
		return false;
	}

	std::wstring kArgText = L"/NoGsm /LaunchByLauncher";
	switch(m_eLocal)
	{
	case LS_TAIWAN:
		{
			tagSiteInfo kSiteInfo = SiteControl::GetSelectedSiteInfo();
			if( kSiteInfo.IsEmpty() )
			{
				return false;
			}

			kArgText += L" ";
			kArgText += L"/ServerAddr:\"";
			kArgText += kSiteInfo.kSVIP;
			kArgText += L"\" ";
			kArgText += L"/ServerPort:\"";
			kArgText += BM::vstring(kSiteInfo.iSVPort).operator std::wstring const&();
			kArgText += L"\" ";
			kArgText += L"/SiteIndex:\"";
			kArgText += kSiteInfo.iSVIndex + L'0';
			kArgText += L"\" ";
			kArgText += L"/SiteName:\"";
			kArgText += kSiteInfo.kSVName;
			kArgText += L"\" ";
		}break;
	case LS_JAPAN:
		{
			extern std::wstring	g_CmdLine;
			kArgText += L" ";
			kArgText += g_CmdLine;
		}break;
	default:
		{
		}break;
	}
#ifdef AMERICA
	HINSTANCE	hInst = ShellExecute(NULL, L"open", L"dragonsaga.exe", kArgText.c_str(), L"release\\", SW_SHOWNORMAL);
#else
	HINSTANCE	hInst = ShellExecute(NULL, L"open", L"dragonica.exe", kArgText.c_str(), L"release\\", SW_SHOWNORMAL);
#endif
	if (SE_ERR_DLLNOTFOUND >= (int)(hInst)) // 32보다 큰 값이면 성공.
	{
		WinMessageBox(g_PProcess.GetVisibleWindowHandle(), g_PProcess.GetMsg(E_CANNOT_RUN_CLIENT), WSTR_GAME_NAME.c_str(), MB_OK);
	}
	::exit(0);
	return	true;
}

//	텍스트 갱신
void CPatchProcess::SetWorkMsg(const std::wstring& OutText)
{
	m_wstrTextState = OutText;
}

//	명령줄 파싱
bool CPatchProcess::CmdLineParse(const std::wstring& CmdLine)
{
	if(CmdLine.size())
	{
		//	있다 쪼개라	- 메인 명령어 >> 키워드|옵션@키워드|옵션
		typedef	std::list<std::wstring>	CUT_STRING;
		CUT_STRING	kMainList;
		BM::vstring::CutTextByKey(CmdLine, std::wstring(L"@"), kMainList);
		if(kMainList.size())
		{
			CUT_STRING::iterator	Main_iter = kMainList.begin();

			//	쪼개라 - 서브 명령어
			CUT_STRING	kSubList;
			BM::vstring::CutTextByKey((*Main_iter), std::wstring(L"="), kSubList);
			if(kSubList.size())
			{
				CUT_STRING::iterator	Sub_iter = kSubList.begin();

				while(Sub_iter != kSubList.end())
				{
					if((*Sub_iter).compare(L"-initmake") == 0)
					{
						// Version CRC 벡터로 변환.
						++Sub_iter;
						MakeExeCRC((*Sub_iter).c_str());
						exit(0);
					}
					else if((*Sub_iter).compare(L"-at") == 0)
					{
						m_bIsAuto = true;
					}
					else if((*Sub_iter).compare(L"-fc") == 0)
					{
						m_bIsRecover = true;
					}
					else if((*Sub_iter).compare(L"-lc") == 0)
					{
						m_bIsListCheck = true;
					}

					++Sub_iter;
				}
			}
		}
	}

	return	false;
}

//	실행파일 아이디
bool CPatchProcess::MakeExeCRC(const std::wstring& FileName)
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
		BM::EncSave(FileName, m_vecCRC);
	}
	return true;
}

bool CPatchProcess::ButtonInit(int BtnID, int iType, int iX, int iY, int iShapeCnt, int iInitShape, 
							   std::wstring const& wstrText, std::wstring const& kLink,
							   std::vector<char> const& kData)
{
	if( g_WinApp )
	{
		CCTButton *pkBtn = new CCTButton;
		auto rst = m_kContBtn.insert(std::make_pair(BtnID, pkBtn));
		if( rst.second )
		{
			CONT_BUTTON::mapped_type &kElement = rst.first->second;

			POINT	Pt = { iX, iY };
			kElement->Init(g_WinApp->GetInstance(), g_WinApp->GetHandle(), BtnID, iType, Pt, iShapeCnt, iInitShape, wstrText, kLink, kData);

			CONT_IMG::iterator bmp_iter = m_kContImg.find(GetLocalImageNumber());
			if( bmp_iter != m_kContImg.end() )
			{
				kElement->SetParentBitmap( bmp_iter->second );
			}
		}
		else
		{
			SAFE_DELETE(pkBtn);
		}
	}
	return	false;
}

void CPatchProcess::OwnerDraw(LPDRAWITEMSTRUCT lpDIS)
{
	CONT_BUTTON::iterator	iter = m_kContBtn.find(lpDIS->CtlID);
	if( iter != m_kContBtn.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;

		if( CCTButton::eBTN_SUB_CANCEL == kElement->GetType() )
		{
			if( !m_kSubApp || !m_kSubApp->IsShow() )
			{
				return;
			}
		}
		kElement->OwnerDraw(lpDIS);
		return;
	}

	if( CPatchProcess::LS_TAIWAN == g_PProcess.GetLocal() )
	{
		g_kSiteControl1.DrawItem(lpDIS);
		g_kSiteControl2.DrawItem(lpDIS);
	}
}

void CPatchProcess::CheckMouseOver(int iX, int iY)
{
	CONT_BUTTON::iterator	iter = m_kContBtn.begin();
	while( iter!= m_kContBtn.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		if( CCTButton::eBTN_SUB_CANCEL == kElement->GetType() )
		{
			++iter;
			continue;
		}
		if( kElement->CheckMouseOver(iX, iY) )
		{ //마우스 충돌이 되었다.
			break;
		}
		++iter;
	}

	if( CPatchProcess::LS_TAIWAN == g_PProcess.GetLocal() )
	{
		g_kSiteControl1.CheckMouseOver(iX, iY);
		g_kSiteControl2.CheckMouseOver(iX, iY);
	}
}

void CPatchProcess::SubCheckMouseOver(int iX, int iY)
{
	CONT_BUTTON::iterator	iter = m_kContBtn.begin();
	while( iter!= m_kContBtn.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		if( CCTButton::eBTN_SUB_CANCEL != kElement->GetType() )
		{
			++iter;
			continue;
		}

		kElement->CheckMouseOver(iX, iY);
		break;
	}
}

void CPatchProcess::ChangeLanguage(const char* nation_name, const char* service_region, const char* language, ELangChangeMessage eMsg)
{
	std::string localConfigPath = MB(WSTR_LOCALCONFIGFILE.c_str());
	WritePrivateProfileStringA("LOCAL_INFO", "NATION_CODE", nation_name, localConfigPath.c_str());
	WritePrivateProfileStringA("LOCAL_INFO", "SERVICE_REGION", service_region, localConfigPath.c_str());
	WritePrivateProfileStringA("LOCAL_INFO", "LANGUAGE", language, localConfigPath.c_str());
	SetLocale(UNI(nation_name));
	std::wstring wstrLocal = UNI(language);
	LoadMsgFromINI(wstrLocal);
	if (eMsg == ELCM_YES)
	{
		WinMessageBox(NULL, L"Success change language", g_PProcess.GetMsg(E_SUCCESS_CHANGE_LANG), MB_OK);
	}
}

void CPatchProcess::Exit()
{
	g_PProcess.SetDestroying(true);
	g_PProcess.SetEnd(true);
	if( m_PackThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject(m_PackThread, INFINITE);
	}

	if( g_WinApp )
	{
		g_WinApp->Display(SW_HIDE);
		PostMessage(g_WinApp->GetHandle(), WM_DESTROY, 0, 0L);
	}
}

void CPatchProcess::Command(WPARAM wParam, LPARAM lParam)
{// 버튼 클릭시 동작들
	int iCmdID = LOWORD(wParam);

	if( CPatchProcess::LS_TAIWAN == g_PProcess.GetLocal() )
	{
		if( g_kSiteControl1.Command(wParam, lParam) )
		{
			return;
		}

		if( g_kSiteControl2.Command(wParam, lParam) )
		{
			return;
		}
	}
	
	CONT_BUTTON::const_iterator itor = m_kContBtn.find(iCmdID);
	if(itor != m_kContBtn.end())
	{
		CONT_BUTTON::mapped_type const &kElement = itor->second;
		switch(kElement->GetType())
		{// 버튼의 타입이
		case CCTButton::eBTN_START:
			{// 게임 시작
				if(eProcess_Preparing != m_eProcess)
				{
					break;
				}
				if( !StartGame() )
				{
					break;
				}
			}break;
		case CCTButton::eBTN_EXIT:
			{// 종료
				CTray::Minimize();
			}break;
		case CCTButton::eBTN_RECOVERY:
			{
				if(!PgPatchCoreUtil::OriginalPatchIDCheck())
				{// 현재 버전이 패치할 버전 이상일때
					if( IDNO == WinMessageBox(g_PProcess.GetVisibleWindowHandle(), g_PProcess.GetMsg(E_RECOVERY_CONFIRM), WSTR_GAME_NAME.c_str(), MB_YESNO) )
					{// 풀체킹 하지 않는다면 리턴
						return; 
					}
				}
				FullChecking();
			}break;
		case CCTButton::eBTN_SUB_CANCEL:
			{
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_TRY_CANCEL));
				SetEnd(true);
			}break;
		case CCTButton::eBTN_OPTION:
			{
				InitOption();
			}break;
		case CCTButton::eBTN_DIVISION: { ShowWindow(g_WinApp->GetHandle(),SW_MINIMIZE); }break;
		case CCTButton::eBTN_LOCALE_EU: { ChangeLanguage("EUROPE", "EUROPE", "ENG"); } break;
		case CCTButton::eBTN_LOCALE_RU: { ChangeLanguage("RUSSIA", "RUSSIA", "RUS"); } break;
		case CCTButton::eBTN_LOCALE_FR: { ChangeLanguage("FRANCE", "FRANCE", "FRA"); } break;
		default:
			{// 일반 링크 버튼
				kElement->OpenLink();
			}break;
		}
	}

	switch(iCmdID)
	{
	case CCTButton::eTRAY_SHOW_WND:
		{
			CTray::Close();
		}
		break;
	case CCTButton::eTRAY_EXIT_APP:
		{
			Exit();
		}
		break;
	}
}

bool CPatchProcess::BitmapLoadAndSetPos(UINT Type, POINT const& kPos, std::vector<char> const& kData)
{
	CONT_IMG::iterator	iter = m_kContImg.find(Type);
	if( iter == m_kContImg.end() )
	{
		CHBitmap *pkBmp = new CHBitmap;
		auto	rst = m_kContImg.insert(std::make_pair(Type, pkBmp));
		if( rst.second )
		{
			CONT_IMG::mapped_type &kElement = rst.first->second;
			kElement->LoadBitmapToMemory(kData);
			kElement->SetPos(kPos);
			return true;
		}
		else
		{
			SAFE_DELETE(pkBmp);
		}
	}

	return false;
}

UINT CPatchProcess::GetLocalImageNumber(bool bSubType)
{
	switch(m_eLocal)
	{
	case	LS_CHINA:	
		{
			return HBT_DLG_CHS;
		}
	case	LS_TAIWAN:
		{
			if( bSubType )
			{
				return HBT_DLG_SUB;				
			}
		}
	}

	return HBT_DLG_NORMAL;
}

bool CPatchProcess::SetWindowFrame()
{
	CONT_IMG::iterator	bmp_iter = m_kContImg.find(GetLocalImageNumber());
	if( bmp_iter != m_kContImg.end() )
	{
		CONT_IMG::mapped_type &kElement = bmp_iter->second;
		if( g_WinApp )
		{
			g_WinApp->SetSize(kElement->GetWidth(), kElement->GetHeight());
		}
		return true;	
	}

	return	false;
}

bool	CPatchProcess::DrawDlgFrame(HDC hDC)
{
	return true;
}

void CPatchProcess::Paint(HDC hDC)
{
	CONT_IMG::iterator	bmp_iter = m_kContImg.find(GetLocalImageNumber());
	if( bmp_iter == m_kContImg.end() )
	{
		return;
	}

	CONT_IMG::iterator	Prog1_iter = m_kContImg.find(HBT_PROGRESS1);
	CONT_IMG::iterator	Prog2_iter = m_kContImg.find(HBT_PROGRESS2);
	if(	Prog1_iter == m_kContImg.end() 
	||	Prog2_iter == m_kContImg.end() )
	{
		return;
	}

	CONT_IMG::mapped_type &kElement = bmp_iter->second;
	
	CONT_IMG::mapped_type &kElem_Prog1 = Prog1_iter->second;
	CONT_IMG::mapped_type &kElem_Prog2 = Prog2_iter->second;

	int const iWidth = kElement->GetWidth();
	int const iHeight = kElement->GetHeight();

	HDC	MemDC = CreateCompatibleDC(hDC);
	HBITMAP	NewBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
	HBITMAP OldBitmap = static_cast<HBITMAP>(SelectObject(MemDC, NewBitmap));

	HFONT	NewFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
		DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
	HFONT	OldFont = static_cast<HFONT>(SelectObject(MemDC, NewFont));
	SetBkMode(MemDC, TRANSPARENT);

	kElement->DrawBitmap(MemDC, 0, 0, SRCCOPY);

	TCHAR	szTemp[MAX_PATH] = {0,};	
	RECT	rcDraw;

	g_kSiteControl1.Visible( CPatchProcess::LS_TAIWAN == g_PProcess.GetLocal() );
	g_kSiteControl2.Visible( g_kSiteControl1.Visible() );
	if( !g_kSiteControl1.Visible() )
	{
		{// 패치 메세지
			//SetRect(&rcDraw, 343, 506, 343+300, 506+66);
			POINT const& kPos = m_kEnviData.kMainMsgInfo.kPos;
			SetRect(&rcDraw, kPos.x, kPos.y, kPos.x+436, kPos.y+66);
			SetTextColor(MemDC,m_kEnviData.kMainMsgInfo.kTextColor);
			DrawText(MemDC, m_wstrTextState.c_str(), m_wstrTextState.size(), &rcDraw, DT_CENTER | DT_VCENTER);
		}

		{// 프로그래스 1
			DrawProgress(MemDC, *kElem_Prog1, m_fFilePercent, m_kEnviData.kProgressImg1 );
		}

		{// 프로그래스 2
			DrawProgress(MemDC, *kElem_Prog2, m_fTotalPercent, m_kEnviData.kProgressImg2 );
		}
	}

	//if((m_eLocal == LS_KOREA) || m_bIsRecover)
	//{// 이부분은 아직 수정이 안되어있음
	//	if(m_TotalSize)
	//	{
	//		std::wstring	Text = _T("[") + GetSizeState(m_CntTotalSize);
	//		Text += _T("/") + GetSizeState(m_TotalSize) + _T("]");
	//		SetRect(&rcDraw, 460, 615, 827, 630);
	//		DrawText(MemDC, Text.c_str(), Text.size(), &rcDraw, DT_LEFT | DT_VCENTER);	
	//	}
	//}

	// 버전 정보
	//SetRect(&rcDraw, 460, 545, 827, 557);	
	POINT const& kVerPos = m_kEnviData.kVerMsgInfo.kPos;
	SetRect(&rcDraw, kVerPos.x, kVerPos.y, kVerPos.x+100, kVerPos.y+12);
	SetTextColor(MemDC,m_kEnviData.kVerMsgInfo.kTextColor);
	DrawText(MemDC, m_wstrClientVersion.c_str(), m_wstrClientVersion.size(), &rcDraw, DT_CENTER | DT_VCENTER);

	BitBlt(hDC, 0, 0, iWidth, iHeight, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldFont);
	DeleteObject(NewFont);
	SelectObject(MemDC, OldBitmap);
	DeleteObject(NewBitmap);
	DeleteDC(MemDC);
}

void CPatchProcess::SubPaint(HDC hDC)
{
	CONT_IMG::iterator	bmp_iter = m_kContImg.find(GetLocalImageNumber(true));
	if( bmp_iter == m_kContImg.end() )
	{
		return;
	}

	CONT_IMG::iterator	Prog1_iter = m_kContImg.find(HBT_PROGRESS1);
	CONT_IMG::iterator	Prog2_iter = m_kContImg.find(HBT_PROGRESS2);
	if(	Prog1_iter == m_kContImg.end() 
	||	Prog2_iter == m_kContImg.end() )
	{
		return;
	}

	CONT_IMG::mapped_type &kElement = bmp_iter->second;
	CONT_IMG::mapped_type &kElem_Prog1 = Prog1_iter->second;
	CONT_IMG::mapped_type &kElem_Prog2 = Prog2_iter->second;


	int const iWidth = kElement->GetWidth();
	int const iHeight = kElement->GetHeight();

	HDC	MemDC = CreateCompatibleDC(hDC);
	HBITMAP	NewBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
	HBITMAP OldBitmap = static_cast<HBITMAP>(SelectObject(MemDC, NewBitmap));

	HFONT	NewFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
		DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
	HFONT	OldFont = static_cast<HFONT>(SelectObject(MemDC, NewFont));
	SetBkMode(MemDC, TRANSPARENT);

	kElement->DrawBitmap(MemDC, 0, 0, SRCCOPY);

	TCHAR	szTemp[MAX_PATH] = {0,};	
	RECT	rcDraw;

	{// 패치 메세지
		POINT const& kPos = m_kEnviData.kMainMsgInfo.kPos;
		SetRect(&rcDraw, kPos.x, kPos.y, kPos.x+436, kPos.y+66);
		SetTextColor(MemDC,m_kEnviData.kMainMsgInfo.kTextColor);
		DrawText(MemDC, m_wstrTextState.c_str(), m_wstrTextState.size(), &rcDraw, DT_CENTER | DT_VCENTER);
	}

	DrawProgress(MemDC, *kElem_Prog1, m_fFilePercent, m_kEnviData.kProgressImg1 );// 프로그래스 1
	DrawProgress(MemDC, *kElem_Prog2, m_fTotalPercent, m_kEnviData.kProgressImg2 );// 프로그래스 2

	BitBlt(hDC, 0, 0, iWidth, iHeight, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldFont);
	DeleteObject(NewFont);
	SelectObject(MemDC, OldBitmap);
	DeleteObject(NewBitmap);
	DeleteDC(MemDC);
}

void CPatchProcess::DrawProgress(HDC hDC, CHBitmap const& kImg, float const fPercent, SPrograssBarInfo const& ProgInfo )
{
	float fCalcPercent = fPercent * 100.0f;
	if(fCalcPercent > 100.0f) 
	{
		fCalcPercent = 100.0f;
	}
	kImg.DrawBitmap(hDC, fPercent, SRCCOPY);
	RECT	rcDraw = { kImg.GetPos().x, kImg.GetPos().y-1, kImg.GetPos().x + kImg.GetWidth(), kImg.GetPos().y + kImg.GetHeight() };
	SetTextColor(hDC, ProgInfo.kTextColor);
	wchar_t szTemp[ 1024 ] = {0,};
	swprintf(szTemp, MAX_PATH, L"%.1f / 100.0", fCalcPercent);
	DrawText(hDC, szTemp, -1, &rcDraw, DT_CENTER | DT_VCENTER);
}

std::wstring CPatchProcess::GetSizeState(size_t const& FileSize)
{
	wchar_t	szTemp[MAX_PATH] = {0,};
	// 텍스트를 이쁘게. 바꾸자.
	bool bIsFileSizeKB	= false;
	bool bIsFileSizeMB	= false;
	int iFileSizeKB		= 0;
	int iFileSizeMB 	= 0;
	float fFileSize 	= (float)FileSize;

	if (FileSize >= 100000)
	{
		iFileSizeMB = (int)(FileSize / 1000000);
		fFileSize	= (float)((int)fFileSize % 1000000);
		fFileSize	/= 100000;

		bIsFileSizeMB = true;
	}
	else if (FileSize >= 100)
	{
		iFileSizeKB = (int)(FileSize / 1000);
		fFileSize	= (float)((int)fFileSize % 1000);
		fFileSize	/= 100;

		bIsFileSizeKB = true;
	}

	if (bIsFileSizeMB)
	{
		swprintf(szTemp, MAX_PATH, _T("%d.%dMB"), iFileSizeMB, (int)fFileSize);
	}
	else if (bIsFileSizeKB)
	{
		swprintf(szTemp, MAX_PATH, _T("%d.%dKB"), iFileSizeKB, (int)fFileSize);
	}
	else
	{
		swprintf(szTemp, MAX_PATH, _T("%dByte"), FileSize);
	}

	return szTemp;
}

//	옵션 다이얼로그
bool CPatchProcess::InitOption()
{
	if(m_pkConfigXml)
	{
		delete m_pkConfigXml;
		m_pkConfigXml = NULL;
	}
	if(m_hWndOpDlg)
	{
		EndDialog(m_hWndOpDlg, 0);
		m_hWndOpDlg = NULL;
	}

	if( g_WinApp )
	{
		m_hWndOpDlg = CreateDialog(g_WinApp->GetInstance(), MAKEINTRESOURCE(IDD_OPTION), g_WinApp->GetHandle(), OptionProc);

		if(!m_hWndOpDlg){ return	false; }

		ShowWindow(m_hWndOpDlg, SW_SHOW);

		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SD_BGM_VOLUME), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SD_SE_VOLUME), PBM_SETRANGE, 0, MAKELPARAM(0, 100));

		m_pkConfigXml = new CConfigXml;

		ControlUpdate();
	}
	return	true;
}

//	전역 옵션Dlg 프로시져
BOOL CALLBACK OptionProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_HSCROLL:
		if(g_PProcess.TrackCommand(wParam, lParam))
		{
			return TRUE;
		}
		break;
	case WM_COMMAND:
		if(g_PProcess.OptionCommand(wParam, lParam))
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}

//	컨트롤 업데이트
void CPatchProcess::ControlUpdate()
{
	SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SD_BGM_VOLUME), TBM_SETPOS, TRUE, m_pkConfigXml->GetUseBgmVolume());
	SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SD_SE_VOLUME), TBM_SETPOS, TRUE, m_pkConfigXml->GetUseSeVolume());


	if(m_pkConfigXml->GetUseGraphicMode()) 
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_HIGH), BM_SETCHECK, BST_CHECKED, 0); 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_LOW), BM_SETCHECK, BST_UNCHECKED, 0); 
	}
	else
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_HIGH), BM_SETCHECK, BST_UNCHECKED, 0); 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_LOW), BM_SETCHECK, BST_CHECKED, 0); 
	}

	if(m_pkConfigXml->GetUseScreenMode()) 
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_FULL), BM_SETCHECK, BST_CHECKED, 0); 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_WINDOW), BM_SETCHECK, BST_UNCHECKED, 0); 
	}
	else
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_FULL), BM_SETCHECK, BST_UNCHECKED, 0); 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_RDO_WINDOW), BM_SETCHECK, BST_CHECKED, 0); 
	}

	if(m_pkConfigXml->GetUseSeMode()) 
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SE_MUTE), BM_SETCHECK, BST_CHECKED, 0);
		EnableWindow(GetDlgItem(m_hWndOpDlg, IDC_SD_SE_VOLUME), false);
	}
	else
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SE_MUTE), BM_SETCHECK, BST_UNCHECKED, 0); 
		EnableWindow(GetDlgItem(m_hWndOpDlg, IDC_SD_SE_VOLUME), true);
	}

	if(m_pkConfigXml->GetUseBgmMode()) 
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_BGM_MUTE), BM_SETCHECK, BST_CHECKED, 0);
		EnableWindow(GetDlgItem(m_hWndOpDlg, IDC_SD_BGM_VOLUME), false);
	}
	else
	{ 
		SendMessage(GetDlgItem(m_hWndOpDlg, IDC_BGM_MUTE), BM_SETCHECK, BST_UNCHECKED, 0); 
		EnableWindow(GetDlgItem(m_hWndOpDlg, IDC_SD_BGM_VOLUME), true);
	}
}

//	옵션 다이얼로그 드래그바
bool CPatchProcess::TrackCommand(WPARAM wParam, LPARAM lParam)
{
	HWND	hTrack = GetDlgItem(m_hWndOpDlg, IDC_SD_BGM_VOLUME);
	if((HWND)lParam == hTrack)
	{
		const int Pos = (int)SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SD_BGM_VOLUME), TBM_GETPOS, 0, 0); 
		m_pkConfigXml->SetUseBgmVolume(Pos);
	}

	hTrack = GetDlgItem(m_hWndOpDlg, IDC_SD_SE_VOLUME);
	if((HWND)lParam == hTrack)
	{
		const int Pos = (int)SendMessage(GetDlgItem(m_hWndOpDlg, IDC_SD_SE_VOLUME), TBM_GETPOS, 0, 0); 
		m_pkConfigXml->SetUseSeVolume(Pos);
	}

	ControlUpdate();

	return	true;
}

//	옵션 다이얼로그 커맨드 입력 처리
bool CPatchProcess::OptionCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_RDO_FULL:
		m_pkConfigXml->SetUseScreenMode(true);
		break;
	case IDC_RDO_WINDOW:
		m_pkConfigXml->SetUseScreenMode(false);
		break;
	case IDC_RDO_HIGH:
		m_pkConfigXml->SetUseGraphicMode(true);
		break;
	case IDC_RDO_LOW:
		m_pkConfigXml->SetUseGraphicMode(false);
		break;
	case IDC_BGM_MUTE:
		if(!m_pkConfigXml->GetUseBgmMode())
		{
			m_pkConfigXml->SetUseBgmMode(true);
		}
		else
		{
			m_pkConfigXml->SetUseBgmMode(false);
		}
		break;
	case IDC_SE_MUTE:
		if(!m_pkConfigXml->GetUseSeMode())
		{
			m_pkConfigXml->SetUseSeMode(true);
		}
		else
		{
			m_pkConfigXml->SetUseSeMode(false);
		}
		break;
	case IDC_BTN_OK:
		m_pkConfigXml->Save();
		SAFE_DELETE(m_pkConfigXml);
		EndDialog(m_hWndOpDlg, 0);
		return	true;
	case IDC_BTN_CANCEL:
	case IDCANCEL:
		SAFE_DELETE(m_pkConfigXml);
		EndDialog(m_hWndOpDlg, 0);
		return	true;
	}

	ControlUpdate();

	return	true;
}

Window const* CPatchProcess::GetPopPatchDlg() const
{
	return m_kSubApp;
}

void CPatchProcess::SetTotalSize(size_t size)	
{ 
	m_TotalSize = size; 
	m_CntTotalSize = 0;	
}
void CPatchProcess::SetFileSize(size_t size)	
{ 
	m_FileSize = size; 
	m_CntFileSize = 0;
}

void CPatchProcess::SetTotalCnt(size_t size)	
{
	m_CntTotalSize += size;
	if(0 == m_TotalSize)
	{
		return;
	}
	m_fTotalPercent = static_cast<float>(m_CntTotalSize) / static_cast<float>(m_TotalSize);
}

void CPatchProcess::SetFileCnt(size_t size)		
{ 
	m_CntFileSize += size; 
	m_fFilePercent = (float)m_CntFileSize / m_FileSize;
}

void CPatchProcess::ThreadClear()	
{
	if(m_PackThread != INVALID_HANDLE_VALUE)
	{
		_endthreadex(0);
		m_PackThread = INVALID_HANDLE_VALUE; 
	}
}

bool CPatchProcess::CheckMsgLanguageType(std::wstring& kLanguage)
{
	if(kLanguage == WSTR_ENG)		{ return true; }
	else if(kLanguage == WSTR_KOR)	{ return true; }
	else if(kLanguage == WSTR_CHT)	{ return true; }
	else if(kLanguage == WSTR_CHS)	{ return true; }
	else if(kLanguage == WSTR_DEU)	{ return true; }
	else if(kLanguage == WSTR_FRA)	{ return true; }
	else if(kLanguage == WSTR_RUS)	{ return true; }
	else if(kLanguage == WSTR_BRA)	{ return true; }
	else if(kLanguage == WSTR_JPN)	{ return true; }

	switch( m_eLocal )
	{
	case LS_TAIWAN:		{ kLanguage = WSTR_CHT; return true; }break;
	default:			{}break;
	}
	kLanguage = WSTR_ENG;	//set default
	return false;
}

bool CPatchProcess::LoadMsgFromINI(std::wstring& kLanguage)
{
	CheckMsgLanguageType(kLanguage);
	wchar_t	szTemp[MAX_MSG_CHAR] = {0,};
	
	for(int i=E_FAILEDINITWEBBR; i<E_MSG_MAX; ++i)
	{
		BM::vstring vStr(_T("MSG"));
		vStr+=i;
		std::wstring kItem = static_cast<std::wstring>(vStr);
		::GetPrivateProfileString(kLanguage.c_str(), kItem.c_str(), L"", szTemp, sizeof(szTemp), WSTR_MSG_INIFILE.c_str());
		std::wstring kTemp = szTemp;
		if(kTemp.empty())
		{
			if(m_kContDefalutMsg.size() > i)
			{
				kTemp = m_kContDefalutMsg.at(i);
			}			
		}
		auto ret = m_kContMsg.insert(std::make_pair(i, kTemp));
		if(!ret.second)
		{// 존재하면
#ifdef _DEBUG
			kItem+=_T(" is already exist");
			WinMessageBox(NULL, L"LoadMsgFromINI", kItem.c_str() , MB_ICONERROR);
#endif
		}
	}
	return true;
}

wchar_t const * const CPatchProcess::GetMsg(EMsg const& eMsgID)
{
	CONT_MSG::const_iterator itor = m_kContMsg.find(eMsgID);
	if(itor != m_kContMsg.end())
	{
		std::wstring const& kMsgStr = itor->second;
		return kMsgStr.c_str();
	}
	return NULL;
}
void CPatchProcess::SetLocale(std::wstring kNationName)
{
	UPR(kNationName);

	if(kNationName.compare(L"CHS") == 0)				 
	{
		m_eLocal = LS_CHINA;		
		::setlocale(LC_ALL,"CHS");	
	}
	else if(kNationName.compare(L"CHINA") == 0)		 
	{ 
		m_eLocal = LS_CHINA;		
		::setlocale(LC_ALL,"CHS");	
	}
	else if(kNationName.compare(L"SGP") == 0)		 
	{ 
		m_eLocal = LS_SINGAPORE;	
		::setlocale(LC_ALL,"ENG");	
	}
	else if(kNationName.compare(L"SINGAPORE") == 0)	 
	{ 
		m_eLocal = LS_SINGAPORE;	
		::setlocale(LC_ALL,"ENG");	
	}
	else if(kNationName.compare(L"EUROPE") == 0)		 
	{ 
		m_eLocal = LS_EUROPE;	
		::setlocale(LC_ALL,"ENG");	
	}
	else if(kNationName.compare(L"FRANCE") == 0)		 
	{ 
		m_eLocal = LS_FRANCE;	
		::setlocale(LC_ALL,"FRANCE");	
	}
	else if(kNationName.compare(L"GERMANY") == 0)	 
	{ 
		m_eLocal = LS_GERMANY;	
		::setlocale(LC_ALL,"GERMANY");	
	}
	else if(kNationName.compare(L"TAIWAN") == 0)		 
	{ 
		m_eLocal = LS_TAIWAN;	
		::setlocale(LC_ALL,"CHT");	
	}
	else if(kNationName.compare(L"CHT") == 0)		 
	{ 
		m_eLocal = LS_TAIWAN;	
		::setlocale(LC_ALL,"CHT");
	}
	else if(kNationName.compare(L"KOREA") == 0)		 
	{ 
		m_eLocal = LS_KOREA;		
		::setlocale(LC_ALL,"KOREA");
	}
	else if(kNationName.compare(L"AMERICA") == 0)
	{ 
		m_eLocal = LS_AMERICA;	
		::setlocale(LC_ALL,"ENG");
	}
	else if(kNationName.compare(L"JAPAN") == 0)
	{ 
		m_eLocal = LS_JAPAN;
		::setlocale(LC_ALL,"JPN");
	}
	else if(kNationName.compare(L"THAILAND") == 0)
	{ 
		m_eLocal = LS_THAILAND;
		::setlocale(LC_ALL,"THA");
	}
	else if(kNationName.compare(L"VIETNAM") == 0)
	{ 
		m_eLocal = LS_VIETNAM;
		::setlocale(LC_ALL,"VIT");
		::setlocale(LC_NUMERIC,"C");
	}
	else if(kNationName.compare(L"RUSSIA") == 0)
	{ 
		m_eLocal = LS_RUSSIA;
		::setlocale(LC_ALL,"RUS");
		::setlocale(LC_NUMERIC,"C");
	}
	else
	{ 
		m_eLocal = LS_KOREA;		
		::setlocale(LC_ALL, "");	
	}
}

CTEdit* CPatchProcess::GetEdit( HWND ControlWnd )
{
	CONT_EDIT_LIST::iterator edit_itor = m_kContEdit.find(ControlWnd);
	if( edit_itor != m_kContEdit.end() )
	{
		return edit_itor->second;
	}
	return NULL;
}

CTEdit* CPatchProcess::GetNextEdit( HWND ControlWnd )
{
	if( m_kContEdit.empty() )
	{
		return NULL;
	}

	CONT_EDIT_LIST::iterator edit_itor = m_kContEdit.find(ControlWnd);
	if( edit_itor != m_kContEdit.end() )
	{
		++edit_itor;
		if( edit_itor == m_kContEdit.end() )
		{
			edit_itor = m_kContEdit.begin();
		}
		return edit_itor->second;
	}
	return NULL;
}

SEditInfo const& CPatchProcess::GetEditInfo( HWND ControlWnd )
{
	static SEditInfo kInfo;
	CTEdit* pkEdit = GetEdit( ControlWnd );
	if( pkEdit )
	{
		return pkEdit->GetEditInfo();
	}
	return kInfo;
}

int const CPatchProcess::GetButtonID( int const Type )
{
	CONT_BUTTON::const_iterator itr = m_kContBtn.begin();
	while( itr != m_kContBtn.end() )
	{
		if( Type == itr->second->GetType() )
		{
			return itr->second->GetControlID();
		}
		++itr;
	}
	return 0;
}

LRESULT	CALLBACK SubProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_CREATE:
		{

		}break;
	case WM_DRAWITEM:
		{
			g_PProcess.OwnerDraw((LPDRAWITEMSTRUCT)lParam);
		}break;
	case WM_PAINT:
		{
			POINT kMousePt;
			GetCursorPos(&kMousePt);
			ScreenToClient(hWnd, &kMousePt);
			g_PProcess.SubCheckMouseOver(kMousePt.x, kMousePt.y);

			PAINTSTRUCT		ps;
			HDC	hDC = BeginPaint(hWnd, &ps);
			g_PProcess.SubPaint(hDC);
			EndPaint(hWnd, &ps);
		}break;
	case WM_COMMAND:
		{
			g_PProcess.Command(wParam, lParam);
		}break;
	case WM_NCHITTEST:
		{
			//	Window Draging
			UINT	hRet = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
			if(hRet == HTCLIENT)
			{
				return	HTCAPTION;
			}
			return	hRet;
		}break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return	0;
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void OnSubIDLE(HWND hWnd)
{
	static DWORD Time = GetTickCount();
	DWORD	NewTime = GetTickCount();
	if(NewTime > Time)
	{
		Time = NewTime + (1000 / 30);
		InvalidateRect(hWnd, NULL, false);
	}
}