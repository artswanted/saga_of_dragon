// PatchExe.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "PatchExe.h"
#include <shlobj.h>
#include "shellapi.h"
#include "BM/ExceptionFilter.h"
//#include <Tlhelp32.h>

#define MAX_LOADSTRING 100

//#define CHINA_LOCAL
//#define TAIWAN_LOCAL
//#define FRANCE_LOCAL
//#define GERMANY_LOCAL

#ifdef CHINA_LOCAL	//중국
	WCHAR const* TEXT_PATCH					= _T("更新");
	WCHAR const* TEXT_CAN_NOT_READ_VERSION	= _T("无法读取版本信息");
	WCHAR const* TEXT_PATCH_VERSION			= _T("更新版本");
	WCHAR const* TEXT_HAVE_NOT_DATA			= _T("未找到数据文件");
	WCHAR const* TEXT_HAVE_NOT_CLIENT		= _T("未找到客户端");
	WCHAR const* TEXT_DIFFERENT_VERSION		= _T("客户端无法更新，请重新下载客户端");
	WCHAR const* TEXT_ALREADY_PATCHED		= _T("最新版本，无需更新");
	WCHAR const* TEXT_PATCH_COMPLETE		= _T("更新完成");
	WCHAR const* TEXT_DO_YOU_WANT_CANCEL	= _T("要取消更新吗？");
	WCHAR const* TEXT_READY					= _T("准备");
	WCHAR const* TEXT_NOW_PATCHING			= _T("正在更新……");
	WCHAR const* TEXT_UPDATE_CANCEL			= _T("取消更新");
	WCHAR const* TEXT_REQ_BETWEEN_VER		= _T("需要的Client版本 :");
	WCHAR const* TEXT_CUR_CLIENT_VER		= _T("目前Client版本是 :");
#else
	#ifdef TAIWAN_LOCAL		//대만
	WCHAR const* TEXT_PATCH					= _T("更新");
	WCHAR const* TEXT_CAN_NOT_READ_VERSION	= _T("無法讀取版本資訊");
	WCHAR const* TEXT_PATCH_VERSION			= _T("更新版本");
	WCHAR const* TEXT_HAVE_NOT_DATA			= _T("未找到數據文件");
	WCHAR const* TEXT_HAVE_NOT_CLIENT		= _T("未找到主程式");
	WCHAR const* TEXT_DIFFERENT_VERSION		= _T("主程式無法更新，請 重新下載 主程式");
	WCHAR const* TEXT_ALREADY_PATCHED		= _T("最新版本，無需更新");
	WCHAR const* TEXT_PATCH_COMPLETE		= _T("更新完成");
	WCHAR const* TEXT_DO_YOU_WANT_CANCEL	= _T("要取消更新嗎 ？");
	WCHAR const* TEXT_READY					= _T("準備");
	WCHAR const* TEXT_NOW_PATCHING			= _T("正在更新……");
	WCHAR const* TEXT_UPDATE_CANCEL			= _T("取消更新");
	WCHAR const* TEXT_REQ_BETWEEN_VER		= _T("需要的Client版本 :");
	WCHAR const* TEXT_CUR_CLIENT_VER		= _T("目前Client版本是 :");
	#else
		#ifdef FRANCE_LOCAL		//프랑스
			WCHAR const* TEXT_PATCH			= _T("Mise а jour");
			WCHAR const* TEXT_CAN_NOT_READ_VERSION	= _T("Impossible d'identifier la version");
			WCHAR const* TEXT_PATCH_VERSION		= _T("Version de mise а jour");
			WCHAR const* TEXT_HAVE_NOT_DATA		= _T("Aucune donnйe trouvйe");
			WCHAR const* TEXT_HAVE_NOT_CLIENT		= _T("Impossible de localiser le client");
			WCHAR const* TEXT_DIFFERENT_VERSION		= _T("Mauvaise information de version");
			WCHAR const* TEXT_ALREADY_PATCHED		= _T("Il s'agit dйjа d ela derniиre version du client");
			WCHAR const* TEXT_PATCH_COMPLETE		= _T("Mise а jour terminйe");
			WCHAR const* TEXT_DO_YOU_WANT_CANCEL		= _T("Souhaitez-vous annuler ?");
			WCHAR const* TEXT_READY			= _T("Prкt");
			WCHAR const* TEXT_NOW_PATCHING		= _T("Mise а jour en cours...");
			WCHAR const* TEXT_UPDATE_CANCEL		= _T("Mise а jour annulйe");
			WCHAR const* TEXT_REQ_BETWEEN_VER		= _T("Version nйcessaire entre :");
			WCHAR const* TEXT_CUR_CLIENT_VER		= _T("Version client actuelle :");
		#else
			#ifdef GERMANY_LOCAL		//독일
			WCHAR const* TEXT_PATCH			= _T("Patch");
			WCHAR const* TEXT_CAN_NOT_READ_VERSION	= _T("Version nicht erkannt");
			WCHAR const* TEXT_PATCH_VERSION		= _T("Patch Version");
			WCHAR const* TEXT_HAVE_NOT_DATA		= _T("Keine Daten gefunden");
			WCHAR const* TEXT_HAVE_NOT_CLIENT		= _T("Client nicht gefunden");
			WCHAR const* TEXT_DIFFERENT_VERSION		= _T("Falsche Versions Info");
			WCHAR const* TEXT_ALREADY_PATCHED		= _T("Client Dateien sind bereits neueste Version");
			WCHAR const* TEXT_PATCH_COMPLETE		= _T("Patch fertig");
			WCHAR const* TEXT_DO_YOU_WANT_CANCEL		= _T("Abbrechen?");
			WCHAR const* TEXT_READY			= _T("Bereit");
			WCHAR const* TEXT_NOW_PATCHING		= _T("Patch Vorgang..");
			WCHAR const* TEXT_UPDATE_CANCEL		= _T("Update abgebrochen");
			WCHAR const* TEXT_REQ_BETWEEN_VER		= _T("Versionen werden gebraucht, zwischen :");
			WCHAR const* TEXT_CUR_CLIENT_VER		= _T("Momentane Client Version :");
			#else
				//영어
				WCHAR const* TEXT_PATCH					= _T("Patch");
				WCHAR const* TEXT_CAN_NOT_READ_VERSION	= _T("Can not identify the version");
				WCHAR const* TEXT_PATCH_VERSION			= _T("Patch version");
				WCHAR const* TEXT_HAVE_NOT_DATA			= _T("No Data has been found");
				WCHAR const* TEXT_HAVE_NOT_CLIENT		= _T("Can not locate the client");
				WCHAR const* TEXT_DIFFERENT_VERSION		= _T("Wrong version info");
				WCHAR const* TEXT_ALREADY_PATCHED		= _T("Client data is already the latest version");
				WCHAR const* TEXT_PATCH_COMPLETE		= _T("Patch completed");
				WCHAR const* TEXT_DO_YOU_WANT_CANCEL	= _T("Do you want cancel?");
				WCHAR const* TEXT_READY					= _T("Ready");
				WCHAR const* TEXT_NOW_PATCHING			= _T("Now Patching..");
				WCHAR const* TEXT_UPDATE_CANCEL			= _T("Canceled updating");	
				WCHAR const* TEXT_REQ_BETWEEN_VER		= _T("Versions are needed, between :");
				WCHAR const* TEXT_CUR_CLIENT_VER		= _T("Current Client Version :");
			#endif
		#endif
	#endif
#endif



// 전역 변수:
HWND g_hWnd;	
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
HANDLE g_hImgFile = NULL;
std::vector<std::wstring>	g_kPackFolders;
size_t g_nPatchHeaderSize = 0;
CPatch g_Patch;
const std::wstring g_kMon2FolderName = L".\\data\\2_mon2";


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	PgExceptionFilter::Install(PgExceptionFilter::DUMP_LEVEL_HEAVY);
	BM::g_bCalcSHA2CheckSum = true;
	BM::g_bUseFormattedFile = true;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PATCHEXE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PATCHEXE));

	if(!g_Patch.VersionRead())
	{
		//MessageBoxW(g_hWnd, _T("Can not read Version"), _T("Patch"), MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
		return FALSE;
	}

	TCHAR szPath[MAX_PATH];
	HRESULT hModuleName = GetModuleFileNameW(GetModuleHandle(0), szPath, MAX_PATH);
	std::wstring strPath = szPath;
	strPath = strPath.substr(0,strPath.rfind(_T('\\')));
	SetCurrentDirectoryW(strPath.c_str());

	if(_access("PATCH.ID", 0) == -1)
	{
		if(!g_Patch.SetFolder())
		{
			return FALSE;
		}
	}
	else
	{
		std::wstring strPatchID = g_Patch.GetPatchID(_T("patch.id"));
		if(!strPatchID.size())
		{
			if(!g_Patch.SetFolder())
			{
				return FALSE;
			}
		}
	}

	if(!g_Patch.ClientVersionCheck())
	{
		return FALSE;
	}

#ifndef CHINA_LOCAL
	std::wstring strVersion = TEXT_PATCH_VERSION;
	strVersion = strVersion + _T("\n") + g_Patch.m_kClientVer + _T(" -> ") + g_Patch.m_szNewVersion;
	int iRtn = MessageBoxW(g_hWnd, strVersion.c_str(), TEXT_PATCH, MB_OKCANCEL|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
	if(iRtn == IDCANCEL)
	{
		return FALSE;
	}
#endif

	g_Patch.Init();

	wchar_t	szTemp[MAX_PATH] = {0,};
#ifdef CHINA_LOCAL
	GetPrivateProfileString(L"LOCAL_INFO", L"NATION_NAME", L"CHS", szTemp, sizeof(szTemp), L"PATCHERCONFIG.INI");
#else
	GetPrivateProfileString(L"LOCAL_INFO", L"NATION_NAME", L"USA", szTemp, sizeof(szTemp), L"PATCHERCONFIG.INI");
#endif
	setlocale(LC_ALL, MB(szTemp));

	g_Patch.CreatePatchThread();
	//g_Patch.ReadChangeLog();

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		InvalidateRect(g_hWnd, NULL, false);

		static DWORD Time = GetTickCount();
		DWORD	NewTime = GetTickCount();
		if(NewTime > Time)
		{
			Time = NewTime + 500;
			g_Patch.CheckFinish();
		}
	}

	BM::PgDataPackManager::Clear();

	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PATCHEXE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_PATCHEXE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP|WS_CLIPCHILDREN,
      0, 0, 10, 10, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		g_Patch.Command(wParam, lParam);
		// 메뉴의 선택 영역을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DRAWITEM:
		{
			g_Patch.OwnerDraw((LPDRAWITEMSTRUCT)lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		g_Patch.Paint(hdc);
		EndPaint(hWnd, &ps);
		UpdateWindow(g_hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



//////////////////////////////////////////////////////
CPatch::CPatch()
{
	m_dwOffset = 0;
	m_dwRead = 0;
	m_bylen2 = 0;
	m_bylen3 = 0;
	m_bThreadEnd = false;
	m_bFinish = false;
	memset(m_szOldVersion, 0, sizeof(m_szOldVersion));
	memset(m_szNewVersion, 0, sizeof(m_szNewVersion));
}
CPatch::~CPatch()
{
}

void CPatch::Init()
{
	//	툴 모양 관련.
	BitmapLoad(HBT_DLG_CHS,	hInst, IDB_BITMAP_BGCHS, 515, 395);
	BitmapLoad(HBT_PROGRESS1, hInst, IDB_BITMAP_PROGRESS1, 445, 12);
	BitmapLoad(HBT_PROGRESS2, hInst, IDB_BITMAP_PROGRESS2, 445, 12);

	InitVriable();
	SetWindowFrame();

	ButtonInit(1004, 443, 363, 61, 22, L"Exit", hInst, IDB_BITMAP_BUTTON);
	//ListBoxInit(1101, 5, 5, 505, 258);

	SetWorkState(TEXT_READY);
}

bool CPatch::SetFolder()
{
	TCHAR szPath[MAX_PATH];
	HRESULT hModuleName = GetModuleFileNameW(GetModuleHandle(0), szPath, MAX_PATH);
	std::wstring strPath = szPath;
	strPath = strPath.substr(0,strPath.rfind(_T('\\')));
	SetCurrentDirectoryW(strPath.c_str());

	if(_access("patch.id", 0) != -1)
	{
		return true;
	}

	WCHAR szKeyName[MAX_PATH] = _T("SOFTWARE\\ICEE\\Dragonica");
	WCHAR szHomeDir[MAX_PATH]= {0,};
	
	// 게임폴더 위치
	LONG iter = MAX_PATH;
	if(ERROR_SUCCESS!=RegQueryValueW(HKEY_LOCAL_MACHINE, szKeyName, szHomeDir, &iter))
	{
		if(FolderSelect(NULL, szHomeDir))
		{
			SetWindowText(NULL, szHomeDir);
		}
		else
		{
			return false;
		}
	}
	SetCurrentDirectoryW(szHomeDir);

	return true;
}

bool CPatch::VersionRead()
{
	m_pNtHdrs = (PIMAGE_NT_HEADERS)((DWORD)hInst + ((PIMAGE_DOS_HEADER)hInst)->e_lfanew);
	GetModuleFileNameW(NULL, m_szImgName, MAX_PATH);

	// 데이타 체크
	g_hImgFile = CreateFileW(m_szImgName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	SetFilePointer(g_hImgFile, m_pNtHdrs->FileHeader.PointerToSymbolTable, NULL, FILE_BEGIN);
	m_dwOffset = m_pNtHdrs->FileHeader.PointerToSymbolTable;
	if(!m_dwOffset)
	{
		MessageBoxW(g_hWnd, TEXT_HAVE_NOT_DATA, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
		return false;
	}

	ReadFile(g_hImgFile, &m_bylen2, sizeof(m_bylen2), &m_dwRead, NULL);
	ReadFile(g_hImgFile, m_szOldVersion, m_bylen2, &m_dwRead, NULL);
	m_szOldVersion[m_bylen2] = '\0';
	m_kClientVer = m_szOldVersion;

	ReadFile(g_hImgFile, &m_bylen3, sizeof(m_bylen3), &m_dwRead, NULL);
	ReadFile(g_hImgFile, m_szNewVersion, m_bylen3, &m_dwRead, NULL);
	m_szNewVersion[m_bylen3] = '\0';

	m_kNewVersion.iVersion = ReadPatchVer(m_szNewVersion);

	return true;
}

bool CPatch::Process()
{
	DWORD dwLen = 0;
	ReadFile(g_hImgFile, &dwLen, sizeof(dwLen), &m_dwRead, NULL);
	char* szText = new char[dwLen];
	ReadFile(g_hImgFile, szText, dwLen, &m_dwRead, NULL);
	std::wstring strText = UNI(szText);
	typedef	std::list<std::wstring>	CUT_STRING;
	CUT_STRING	kText;
	BM::vstring::CutTextByKey<std::wstring>(strText, L"\n", kText);
	CUT_STRING::iterator itr = kText.begin();
	const int TEXTLENG = 29;
	//for(int i=0; itr != kText.end(); ++itr)
	//{
	//	std::wstring strLine = (*itr);
	//	bool bRoop = true;
	//	while(bRoop)
	//	{
	//		std::wstring strTemp;
	//		if(strLine.size() < TEXTLENG)
	//		{
	//			bRoop = false;
	//			strTemp = strLine;
	//		}
	//		else
	//		{
	//			strTemp = strLine.substr(0, TEXTLENG);
	//			strLine = strLine.substr(TEXTLENG);
	//			if(strLine.size() == 0)
	//			{
	//				bRoop = false;
	//			}
	//		}

	//		SendMessage(m_ListBoxChangeLog.GetWnd(), LB_ADDSTRING, i, (LPARAM)strTemp.c_str());  
	//		++i;
	//	}
	//}
	delete[] szText;

	m_dwOffset = m_dwOffset + m_bylen2 + m_bylen3 + (sizeof(BYTE)*2) + sizeof(DWORD) + dwLen;

	// 데이타 크기
	LARGE_INTEGER iFileSize;
	GetFileSizeEx(g_hImgFile, &iFileSize);

	DWORD dwSize = iFileSize.LowPart-m_dwOffset;
	char* szData = new char[dwSize];

	// 데이타 읽기
	ReadFile(g_hImgFile, szData, sizeof(char)*dwSize, &m_dwRead, NULL);

	// 패치
	PackFolderInit();
	if( !PackPatch(szData, dwSize))
	{
		delete[] szData;
		if(!m_wstrTextState.empty())
		{
			Sleep(100);
			DeleteFile(m_wstrTextState.c_str());
		}
		return false;
	}

	delete[] szData;

	if(m_bThreadEnd)
	{
		return false;
	}

	//HWND MessageBoxWnd = FindWindow(NULL, _T("Patch")) ;
	//if(MessageBoxWnd)
	//{
	//	EndDialog(MessageBoxWnd, TRUE);
	//}

	//MakePatchID(MB(m_szNewVersion));

	//MessageBoxW(g_hWnd, _T("Patch Complete"), _T("Patch"), MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);

	//HWND handle = 0;
	//HINSTANCE kIns = ShellExecute(handle, L"open", L"Launcher.exe", L"", L"", SW_SHOWNORMAL);

	return true;
}

bool CPatch::IsVersionCompare(std::wstring const& strClient, std::wstring const& strOldVersion)
{// 같아야 패치한다.
	return true;
}

std::wstring CPatch::GetPatchID(std::wstring const& strFileName)
{
	std::vector<char>	vec_PatchID;	//암호화된 ID
	BM::FileToMem(strFileName, vec_PatchID);

	if(vec_PatchID.size())
	{
		BM::SimpleDecrypt(vec_PatchID, vec_PatchID);
		vec_PatchID.push_back('\0');

		char*	pIDSrc	= new char[vec_PatchID.size()];
		memcpy(pIDSrc, &vec_PatchID.at(0), vec_PatchID.size());
		std::wstring	wstrID = UNI(pIDSrc);
		delete [] pIDSrc;

		return	wstrID;
	}
	
	return	L"";
}

void CPatch::MakePatchID(std::string strPatchID)
{
	std::vector<char>	vecPatchID;
	vecPatchID.resize(strPatchID.size());
	::memcpy(&vecPatchID.at(0), strPatchID.c_str(), strPatchID.size());
	//	저장
	BM::SimpleEncrypt(vecPatchID, vecPatchID);
	BM::MemToFile(L"PATCH.ID", vecPatchID, false);
	//BM::MemToFileNotReserveFolder(L"PATCH.ID", vecPatchID);
}

bool CPatch::FolderSelect(const HWND& hWnd, TCHAR* szFolderName, const int& Length)
{
	memset(szFolderName, 0, Length);

	BROWSEINFO			bi;

	bi.hwndOwner		= hWnd;	//부모윈도우핸들
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= NULL;
	bi.lpszTitle		= L"Select Folder";
	bi.ulFlags			= 0;
	bi.lpfn				= NULL;
	bi.lParam			= 0;
	
	//폴더 선택 창을 만든다
	LPITEMIDLIST pidl	= SHBrowseForFolder(&bi);
	if(pidl == NULL)	return	false;

	//폴더변수에저장한다
	SHGetPathFromIDList(pidl, szFolderName);

	return	true;
}

void CPatch::PackFolderInit()
{
	g_kPackFolders.push_back(L".\\xml");
	g_kPackFolders.push_back(L".\\script");
	g_kPackFolders.push_back(L".\\bgm");
	g_kPackFolders.push_back(L".\\ui");
	g_kPackFolders.push_back(L".\\data\\1_cha");
	g_kPackFolders.push_back(L".\\data\\2_mon2");
	g_kPackFolders.push_back(L".\\data\\2_mon");
	g_kPackFolders.push_back(L".\\data\\3_world");
	g_kPackFolders.push_back(L".\\data\\3_world2");
	g_kPackFolders.push_back(L".\\data\\4_item");
	g_kPackFolders.push_back(L".\\data\\5_effect");
	g_kPackFolders.push_back(L".\\data\\6_ui");

	BM::CPackInfo kPackInfo;//Init
	if(!kPackInfo.Init())
	{
		assert(NULL);
	}
	BM::PgDataPackManager::Init(kPackInfo);
}

bool CPatch::PackPatch(char* szData, DWORD dwSize)
{
	SetWorkState(TEXT_NOW_PATCHING);
		//	패치 헤더를 구한다
	BM::FolderHash	kPatchList;
	g_nPatchHeaderSize = (BM::PgDataPackManager::ReadHeader(szData, kPatchList));

	//	패치 리스트를 나눈다
	BM::FolderHash	kPatchPackList;
	BM::FolderHash	kPatchNonPackList;
	if(!DividePatchList(kPatchPackList, kPatchNonPackList, kPatchList, g_kPackFolders))
	{
		return	false;
	}

	//	총 크기 계산
	size_t	TotalSize = 0;
	//TotalSize	+= GetListDataSize(kPatchPackList);
	TotalSize	+= GetListDataSize(kPatchNonPackList);
	SetFileSize(TotalSize);
	SetTotalSize(kPatchPackList.size() + 1);

	//	NonPack 업데이트
	if(!AmendNonPackUpdate(kPatchNonPackList, szData))
	{
		return	false;
	}
	SetTotalCnt(1);

	//	Pack 업데이트
	if(!AmendPackUpdate(kPatchPackList, szData))
	{
		return	false;
	}

	BM::FolderHash		kClientPackList;
	BM::FolderHash		kClientNonPackList;
	if(!GetClientList(kClientPackList, kClientNonPackList))
	{
		return false;
	}
	if(!UpdatePackVer(kClientPackList))
	{
		return false;
	}

	SetTotalPer(1.0f);
	SetWorkState(TEXT_PATCH_COMPLETE);

	return	true;
}

bool CPatch::DividePatchList(BM::FolderHash& kResultPackList, BM::FolderHash& kResultNonPackList,
						const BM::FolderHash& kPackList, const std::vector<std::wstring>& kPackFolderList)
{
	//	리스트가 없네 에러다
	if(!kPackList.size())		{ return	false; }

	//	패치 리스트 폴더 해쉬 기본적으로 하나지만. 루프는 돌아주자
	BM::FolderHash::const_iterator	Main_iter = kPackList.begin();
	while(Main_iter != kPackList.end())
	{
		//	패치 리스트의 파일 해쉬 본격적으로 나누는거다
		BM::FileHash::const_iterator	Main_file_iter = (*Main_iter).second->kFileHash.begin();
		while(Main_file_iter != (*Main_iter).second->kFileHash.end())
		{
			//	팩인지 아닌지 스위치
			bool	bIsPack = false;
			BM::PgFolderInfo* pFolderInfo = NULL; 

			//	폴더이름
			std::wstring	wstrFolderName = L".\\" + (*Main_file_iter).first;

			//const std::wstring wstrTemp1 = wstrFolderName.substr(0, g_kMon2FolderName.size());
			//if(wcscmp(wstrTemp1.c_str(), g_kMon2FolderName.c_str()) == 0)
			//{//2_Mon2 폴더 예외처리.. 말안하고 맘대로 추가한 기획자 누구야.. 아 짱나 죽겄네 -_
			//	bIsPack = false;
			//}
			//else
			{//	팩 폴더 리스트를 돌아서 골라내는거다
				std::vector<std::wstring>::const_iterator	kFolder_iter = kPackFolderList.begin();
				while(kFolder_iter != kPackFolderList.end())
				{
					//	이제 패치리스트에서 찾아보자
					const std::wstring wstrTemp = wstrFolderName.substr(0, (*kFolder_iter).size());
					if(wcscmp(wstrTemp.c_str(), (*kFolder_iter).c_str()) == 0)
					{
						//	파일명 잘라야지
						const std::wstring	wstrFileName = (*Main_file_iter).first.substr(((*kFolder_iter).size() - 1), (*Main_file_iter).first.size());

						//	팩폴더가 결과 리스트에 추가된적이 있는지 확인하자
						BM::FolderHash::iterator	RstPack_iter = kResultPackList.find((*kFolder_iter) + L"\\");
						if(RstPack_iter == kResultPackList.end())
						{
							//	팩폴더없네 - 추가
							pFolderInfo = BM::PgFolderInfo::New();
							pFolderInfo->bIsUsePack = true;
							kResultPackList.insert(make_pair((*kFolder_iter) + L"\\", BM::PgFolderInfoPtr(pFolderInfo)));
							RstPack_iter = kResultPackList.find((*kFolder_iter) + L"\\");
						}
						//	파일 정보를 받고
						BM::PgFolderInfoPtr RstFolderInfo = ((*RstPack_iter).second);

						//	파일 넣자
						RstFolderInfo->kFileHash.insert(make_pair(wstrFileName, (*Main_file_iter).second));
						bIsPack = true;	//	팩이다 스위치 온
						break;
					}

					++kFolder_iter;
				}
			}

			//	팩이 아니랬나?
			if(!bIsPack)
			{
				//	파일명 잘라야지
				std::wstring	wstrFolderName;
				std::wstring	wstrFileName;
				BM::DivFolderAndFileName((*Main_file_iter).first, wstrFolderName, wstrFileName);

				//	폴더가 추가된 적이 있나?
				BM::FolderHash::iterator	RstNonPack_iter = kResultNonPackList.find(wstrFolderName);
				if(RstNonPack_iter == kResultNonPackList.end())
				{
					//	NonPack폴더 없네 - 추가
					pFolderInfo = BM::PgFolderInfo::New();
					pFolderInfo->bIsUsePack = false;
					kResultNonPackList.insert(make_pair(wstrFolderName, BM::PgFolderInfoPtr(pFolderInfo)));
					RstNonPack_iter = RstNonPack_iter = kResultNonPackList.find(wstrFolderName);
				}
				//	파일 정보를 받고
				BM::PgFolderInfoPtr	RstFolderInfo = (*RstNonPack_iter).second;
	
				//	파일 넣자
				RstFolderInfo->kFileHash.insert(make_pair(wstrFileName, (*Main_file_iter).second));
			}

			++Main_file_iter;
		}

		++Main_iter;
	}

	return	true;
}

bool CPatch::AmendNonPackUpdate(const BM::FolderHash& PatchList, char* szData)
{
	//	리스트가 있을때만 해야겠지
	if(PatchList.size())
	{
		char* szData2 = szData;

		//	폴더를 먼저 선택하고
		BM::FolderHash::const_iterator	Folder_iter = PatchList.begin();
		while(Folder_iter != PatchList.end())
		{
			//	파일이 있다면
			if((*Folder_iter).second->kFileHash.size())
			{
				//	돌자
				BM::FileHash::const_iterator	File_iter = (*Folder_iter).second->kFileHash.begin();
				while(File_iter != (*Folder_iter).second->kFileHash.end())
				{
					CheckRun();
					if(m_bThreadEnd)
					{
						return false;
					}
					if((*File_iter).first == _T("patch.id"))
					{
						++File_iter;
						continue;
					}
					char*	pPatchData = szData2 + g_nPatchHeaderSize + (*File_iter).second.offset;
					size_t	pDataSize = 0;
					std::vector<char>	vecData;

					//	사이즈 알아보자 - 압축?
					if((*File_iter).second.bIsZipped)
					{
						pDataSize = (*File_iter).second.zipped_file_size;
					}
					else
					{
						pDataSize = (*File_iter).second.org_file_size;
					}

					SetFileCnt(pDataSize);

					//	사이즈 알아보자 - 암호화?
					if((*File_iter).second.bIsEncrypt)
					{
						pDataSize += BM::ENC_HEADER_SIZE;
					}

					//	파일을 메모리에 복사하자
					vecData.resize(pDataSize);
					::memcpy(&vecData.at(0), pPatchData, pDataSize);

					//	암호화되있음 푼다
					if((*File_iter).second.bIsEncrypt)
					{
						if( !DecFile(vecData, (*File_iter).second) )
						{
							return false;
						}
					}
					else
					{
						//	압축되있음 푼다
						if((*File_iter).second.bIsZipped)
						{
							UnCompFile(vecData, (*File_iter).second.org_file_size);
						}
					}

					//	해당 디렉토리로 파일을 옮긴다
					std::wstring	wstrOrgFileName = (*Folder_iter).first + (*File_iter).first;
					BM::ReserveFolder((*Folder_iter).first);
					DeleteFile(wstrOrgFileName.c_str());
					BM::MemToFile(wstrOrgFileName, vecData, false);
					//BM::MemToFileNotReserveFolder(wstrOrgFileName, vecData);

					++File_iter;
				}
			}

			++Folder_iter;
		}
	}

	return	true;
}

bool CPatch::AmendPackUpdate(const BM::FolderHash& PatchList, char* szData)
{
	//	리스트가 있을때만 해야겠지
	if(PatchList.size())
	{
		//	폴더를 먼저 선택하고
		BM::FolderHash::const_iterator	Folder_iter = PatchList.begin();
		while(Folder_iter != PatchList.end())
		{
			//	파일이 있다면
			if((*Folder_iter).second->kFileHash.size())
			{
				//	빼서 넘겨야 겠지?
				BM::FolderHash	kPatchList;
				kPatchList.insert(std::make_pair(L".\\", (*Folder_iter).second));
				if(!AmendPatchToOrgMerge((*Folder_iter).first, kPatchList, szData))
				{
					return false;
				}
			}

			++Folder_iter;
		}
	}

	return	true;
}

//	클라이언트 리스트 생성
bool CPatch::GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList)
{
	//	클라이언트 Pack 리스트
	std::vector<std::wstring>::iterator		FolderName_iter = g_kPackFolders.begin();
	while(FolderName_iter != g_kPackFolders.end())
	{
		std::wstring	FileName = (*FolderName_iter) + L".dat";

		if(_access(MB(FileName), 0) != -1)
		{//이전 포맷 이라면 컨버팅부터 하고 작업한다
			BM::PgDataPackManager::ConvertToFormattedDataFile(FileName);

			BM::PgDataPackManager::PackFileCache_new kCache;
			if(kCache.Open(FileName))
			{
				BM::PgDataPackManager::ReadHeader(kCache);
			}
			BM::FolderHash	kFolderList;
			kFolderList = kCache.mFolderHash;
			//BM::PgDataPackManager::ReadHeader(FileName, kFolderList);
			PackList.insert(std::make_pair((*FolderName_iter) + L"\\", kFolderList.begin()->second));
		}

		++FolderName_iter;
	}

	return	true;
}

size_t CPatch::GetListDataSize(BM::FolderHash const& List, EREQUESTTARGET const Value)
{
	if(!List.size())
	{
		return	0;
	}

	size_t	TotalSize = 0;

	BM::FolderHash::const_iterator	iter = List.begin();
	while(iter != List.end())
	{
		if((*iter).second->kFileHash.size())
		{
			BM::FileHash::const_iterator	File_iter = (*iter).second->kFileHash.begin();
			while(File_iter != (*iter).second->kFileHash.end())
			{
				if((*File_iter).second.bIsZipped)
				{
					TotalSize += (*File_iter).second.zipped_file_size;
				}
				else
				{
					TotalSize += (*File_iter).second.org_file_size;
				}

				if(Value != ERT_DOWNLOAD)
				{
					if((*File_iter).second.bIsEncrypt)
					{
						TotalSize += BM::ENC_HEADER_SIZE;
					}
				}

				++File_iter;
			}
		}

		++iter;
	}

	return	TotalSize;
}

bool CPatch::DecFile(std::vector<char>& vecData, BM::PgFileInfo kFileInfo)
{
	std::vector<char>	vecSrc;
	vecSrc.resize(vecData.size());
	memcpy(&vecSrc.at(0), &vecData.at(0), vecData.size());
	return BM::DecLoadFromMem(kFileInfo.org_file_size, kFileInfo.zipped_file_size, kFileInfo.bIsZipped, kFileInfo.bIsEncrypt, vecSrc, vecData);
}
void CPatch::UnCompFile(std::vector<char>& vecData, size_t const& OriginalSize)
{
	std::vector<char>	vecSrc;
	vecSrc.resize(OriginalSize);
	BM::UnCompress(vecSrc, vecData);
	vecData = vecSrc;
}

bool CPatch::AmendPatchToOrgMerge(const std::wstring& TargetDir, const BM::FolderHash& kPatchHash, char* szData)
{
	//	원본 파일
	//PgFileMapping	kOrgFile;
	BM::FolderHash	kOrgHash;
	std::wstring	wstrFileName = TargetDir;
	BM::DetachFolderMark(wstrFileName);
	wstrFileName += L".dat";
	if(_access(MB(wstrFileName), 0) == -1)
	{
		BM::ReserveFolder(wstrFileName);
		CopyFile(L".\\patch.tmp", wstrFileName.c_str(), false);
		return	true;
	}

	std::wstring	wstrNewName;
	
	{
		//이전 포맷 이라면 컨버팅부터 하고 작업한다
		BM::PgDataPackManager::ConvertToFormattedDataFile(wstrFileName);

		BM::PgDataPackManager::PackFileCache_new kOrgCache;
		if(!kOrgCache.Open(wstrFileName))
		{
			return false;
		}
		const size_t	OrgHeaderSize = BM::PgDataPackManager::ReadHeader(kOrgCache);
		kOrgHash = kOrgCache.mFolderHash;
		//const size_t	OrgHeaderSize = BM::PgDataPackManager::ReadHeader(wstrFileName, kOrgHash);
		//if(kOrgFile.Init(wstrFileName) != S_OK)
		//{
		//	return false;
		//}

		//	패치 파일
		char* szData2 = szData;

		//	원본에서 패치된 파일을 업데이트합니다.
		//	결합 파일
		BM::FolderHash	kNewHash;	

		//	리스트 생성 및 수정
		MergeList(kPatchHash, kOrgHash, kNewHash);

		size_t TotalSize = GetListDataSize(kNewHash);
		SetFileSize(TotalSize);
		//	결과 파일 생성
		//PgFileMapping	kNewFile;
		wstrNewName = TargetDir;
		BM::DetachFolderMark(wstrNewName);
		wstrNewName += L"_new.dat";
		const unsigned __int64	NewDataSize	= BM::PgDataPackManager::BuildOffset(kNewHash);
		std::fstream ffile;
		ffile.open(wstrNewName.c_str(), std::ios_base::in | std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);

		if(!ffile.is_open())
		{
			return false;
		}
		const size_t	NewHeaderSize = BM::PgDataPackManager::MakeHeader(kNewHash, ffile, true);
		//const size_t	NewHeaderSize = BM::PgDataPackManager::MakeHeader(wstrNewName, kNewHash, kNewFile, true);
		//if(kNewFile.Init(wstrNewName, OPEN_EXISTING, NewHeaderSize + NewDataSize) != S_OK)
		//{
		//	return false;
		//}
		
		//	결과대로 묶자
		BM::FolderHash::iterator	New_iter = kNewHash.begin();
		while(New_iter != kNewHash.end())
		{
			//	폴더를 찾아
			BM::FolderHash::const_iterator	Patch_iter = kPatchHash.find((*New_iter).first);
			BM::FolderHash::iterator	Org_iter = kOrgHash.find((*New_iter).first);

			//	파일을 돌려
			BM::FileHash::iterator	New_file_iter = (*New_iter).second->kFileHash.begin();
			while(New_file_iter != (*New_iter).second->kFileHash.end())
			{
				CheckRun();
				if(m_bThreadEnd)
				{
					m_wstrTextState = wstrNewName;
					//BOOL bRtn = DeleteFile(wstrNewName.c_str());
					return false;
				}
				const BM::PgFileInfo	*pFileInfo = NULL;
				char*	pData = NULL;
				bool bFind = false;
				size_t	pDataSize = 0;
				bool	bInPatch = true;
				//char*	pNewData = kNewFile.Data() + NewHeaderSize + (*New_file_iter).second.offset;
				ffile.seekg(NewHeaderSize + (*New_file_iter).second.offset);

				//	검사 있냐?
				if(Patch_iter == kPatchHash.end())
				{
					bInPatch = false;
				}
				else
				{
					//	있네. 찾아라
					if((*Patch_iter).second->Find((*New_file_iter).first, pFileInfo))
					{
						pData = szData2 + g_nPatchHeaderSize + pFileInfo->offset;
						bFind = true;
					}
					else
					{
						bInPatch = false;
					}
				}

				//	없다.
				if(bInPatch == false)
				{
					//	오리지날에서 찾자
					if(Org_iter == kOrgHash.end())
					{
						return	false;
					}
					else
					{
						//	있다. 찾아라
						if((*Org_iter).second->Find((*New_file_iter).first, pFileInfo))
						{
							//pData = kOrgFile.Data() + OrgHeaderSize + pFileInfo->offset;
							bFind = true;
						}
						else
						{
							//	없다 에러
							return	false;
						}
					}
				}

				//	사이즈 알아보자 - 압축?
				if(pFileInfo->bIsZipped)
				{
					pDataSize = pFileInfo->zipped_file_size;
				}
				else
				{
					pDataSize = pFileInfo->org_file_size;
				}

				SetFileCnt(pDataSize);


				//	사이즈 알아보자 - 암호화?
				if(pFileInfo->bIsEncrypt)
				{
					pDataSize += BM::ENC_HEADER_SIZE;
				}

				//	복사하자
				if(bFind && bInPatch)
				{
					ffile.write(pData, pDataSize);
				}
				else if(bFind && !bInPatch)
				{
					std::vector<char> vecTemp;
					vecTemp.resize(pDataSize);
					kOrgCache.ReadData(OrgHeaderSize + pFileInfo->offset, &vecTemp.at(0), pDataSize);
					ffile.write(&vecTemp.at(0), pDataSize);
				}
				//if(pData)
				//{
				//	::memcpy(pNewData, pData, pDataSize);
				//	pNewData += pDataSize;
				//}

				++New_file_iter;
			}

			++New_iter;
		}

		//kOrgFile.Clear();
		//kNewFile.Clear();
		ffile.close();
	}

	DeleteFile(wstrFileName.c_str());
	_wrename(wstrNewName.c_str(), wstrFileName.c_str());

	SetTotalCnt(1);

	return	true;
}

bool CPatch::MergeList(BM::FolderHash const& PatchList, BM::FolderHash& OrgList, BM::FolderHash& kResultList)
{
	//	둘중하나라도 비면 할필요가 있나?
	if(PatchList.size() || OrgList.size())
	{
		// 원본을 리설트에 넣고.
		// SmartPtr이어서 그냥 집어 넣으면 안되고 복사를 해서 넣어줘야 한다. 아래는 예전코드
		//kResultList.insert(OrgList.begin(), OrgList.end());	
		BM::FolderHash::iterator org_iter = OrgList.begin();
		while (org_iter != OrgList.end())
		{
			BM::PgFolderInfo* kNewInfo = BM::PgFolderInfo::New();
			*kNewInfo = *((*org_iter).second); // make a deepcopy
			kResultList.insert(std::make_pair((*org_iter).first, BM::PgFolderInfoPtr(kNewInfo)));
			++org_iter;
		}

		//	폴더를 돌자
		BM::FolderHash::const_iterator	Patch_iter = PatchList.begin();
		while(Patch_iter != PatchList.end())
		{
			//	폴더를 찾아라
			BM::FolderHash::iterator	Result_iter = kResultList.find((*Patch_iter).first);

			//	찾았냐?
			if(Result_iter != kResultList.end())
			{
				//	파일을 검사하자.
				BM::FileHash::const_iterator	Patch_file_iter = (*Patch_iter).second->kFileHash.begin();
				while(Patch_file_iter != (*Patch_iter).second->kFileHash.end())
				{
					//	파일을 찾아라
					BM::FileHash::iterator	Result_file_iter = (*Result_iter).second->kFileHash.find((*Patch_file_iter).first);
					//	찾았냐?
					if(Result_file_iter != (*Result_iter).second->kFileHash.end())
					{
						//	갱신 및 삭제
						(*Result_file_iter).second = (*Patch_file_iter).second;

					}
					else
					{
						//	추가
						(*Result_iter).second->kFileHash.insert(std::make_pair((*Patch_file_iter).first, (*Patch_file_iter).second));
					}

					++Patch_file_iter;
				}
			}
			else
			{
				//	추가
				kResultList.insert(std::make_pair((*Patch_iter).first, (*Patch_iter).second));
			}

			++Patch_iter;
		}

		return	true;
	}

	return	false;
}

bool CPatch::BitmapLoad(UINT Type, std::vector<char> const& kData)
{
	kImageCont::iterator	iter = m_kImageContainer.find(Type);
	if( iter == m_kImageContainer.end() )
	{
		CHBitmap	Bitmap;
		auto	rst = m_kImageContainer.insert(std::make_pair(Type, Bitmap));
		if( rst.second )
		{
			rst.first->second.LoadBitmapToMemory(kData);
			return true;
		}			
	}

	return false;
}

bool CPatch::BitmapLoad(UINT Type, HINSTANCE hInst, UINT uiID, int iWidth, int iHeight)
{
	kImageCont::iterator	iter = m_kImageContainer.find(Type);
	if( iter == m_kImageContainer.end() )
	{
		CHBitmap	Bitmap;
		auto	rst = m_kImageContainer.insert(std::make_pair(Type, Bitmap));
		if( rst.second )
		{
			rst.first->second.LoadToResource(hInst, uiID);
			rst.first->second.SetWidth(iWidth);
			rst.first->second.SetHeight(iHeight);
			return true;
		}			
	}

	return false;
}

void CPatch::InitVriable()
{
	//	핸들
	m_PackThread	= INVALID_HANDLE_VALUE;

	//	출력정보
	m_fTotalPercent = 0.0f;
	m_fFilePercent	= 0.0f;
	m_TotalSize	= 0;
	m_FileSize	= 0;
	m_CntFileSize	= 0;
	m_CntTotalSize	= 0;
}

bool CPatch::SetWindowFrame()
{
	kImageCont::iterator	iter = m_kImageContainer.find(1);
	if( iter == m_kImageContainer.end() )
	{
		return	false;
	}

	int const iWidth = iter->second.GetWidth();
	int const iHeight = iter->second.GetHeight();
	int const PosX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (iWidth / 2);
	int const PosY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (iHeight / 2);

	SetWindowPos(g_hWnd, NULL, PosX, PosY, iWidth, iHeight, NULL);

	return true;
}

bool CPatch::ButtonInit(int BtnID, int iX, int iY, std::wstring const& wstrText, std::vector<char> const& kData)
{
	kButtonCont::iterator	iter = m_kButtonContainer.find(BtnID);
	if( iter == m_kButtonContainer.end() )
	{
		CCTButton	Button;
		auto rst = m_kButtonContainer.insert(std::make_pair(BtnID, Button));
		if( rst.second )
		{
			POINT	Pt = { iX, iY };
			rst.first->second.Init(hInst, g_hWnd, BtnID, Pt, wstrText, kData);
		}
	}

	return	false;
}

bool CPatch::ButtonInit(int BtnID, int iX, int iY, int iWidth, int iHeight, std::wstring const& wstrText, HINSTANCE hInst, UINT uiID)
{
	kButtonCont::iterator	iter = m_kButtonContainer.find(BtnID);
	if( iter == m_kButtonContainer.end() )
	{
		CCTButton	Button;
		auto rst = m_kButtonContainer.insert(std::make_pair(BtnID, Button));
		if( rst.second )
		{
			RECT rt = { iX, iY, iWidth, iHeight };
			rst.first->second.Init(hInst, g_hWnd, BtnID, rt, wstrText, hInst, uiID);
		}
	}

	return	false;
}

bool CPatch::ListBoxInit(int iListBoxID, int iX, int iY, int iWidth, int iHeight)
{
	RECT rect = { iX, iY, iWidth, iHeight};
	m_ListBoxChangeLog.Init(hInst, g_hWnd, iListBoxID, rect);

	//HFONT hFont = CreateFontW(
	//		16, //Height
	//		0, //Width
	//		0, //Escapement
	//		0, //Orientation
	//		FW_DONTCARE, //Weight (0~1000)
	//		FALSE, //Italic Flag
	//		FALSE, //Underline Flag
	//		FALSE, //Strikeout Flag
	//		DEFAULT_CHARSET, //CHAR SET
	//		OUT_DEFAULT_PRECIS, //Output Precision
	//		CLIP_DEFAULT_PRECIS, //Clip Precision
	//		DEFAULT_QUALITY, //Quality
	//		DEFAULT_PITCH, //Pitch and Family
	//		L"GulimChe" //Face, Fontname 
	//		);
	//SendMessageW(m_ListBoxChangeLog.GetWnd(), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	
	return	false;
}

unsigned __stdcall PatchRun(void* pArgument)
{
	bool bRtn = g_Patch.Process();

	static	Loki::Mutex	local_mutex;
	BM::CAutoMutex		lock(local_mutex);
	g_Patch.m_bFinish = true;
	g_Patch.m_bFinishResult = bRtn;
	//std::wstring strText;
	//if(g_Patch.m_bThreadEnd && !bRtn)
	//{
	//	strText = _T("Update Cancel");
	//	MessageBoxW(g_hWnd, strText.c_str(), _T("Patch"), MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
	//}
	//else if(!g_Patch.m_bThreadEnd && bRtn)
	//{
	//	strText = _T("Patch Complete");
	//	g_Patch.MakePatchID(MB(g_Patch.m_szNewVersion));
	//	MessageBoxW(g_hWnd, strText.c_str(), _T("Patch"), MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
	//	HWND handle = 0;
	//	HINSTANCE kIns = ShellExecute(handle, L"open", L"Launcher.exe", L"", L"", SW_SHOWNORMAL);
	//}
	//
	//SendMessage(g_hWnd, WM_DESTROY, 0, 0);
	return true;
}

bool CPatch::CreatePatchThread()
{
	static	Loki::Mutex	local_mutex;
	BM::CAutoMutex		lock(local_mutex);

	unsigned	ThreadID;
		
	m_bThreadRun = true;
	m_PackThread = (HANDLE)_beginthreadex(NULL, 0, &PatchRun, this, 0, &ThreadID);

	return	true;
}

void CPatch::OwnerDraw(LPDRAWITEMSTRUCT lpDIS)
{
	kButtonCont::iterator	iter = m_kButtonContainer.find(lpDIS->CtlID);
	if( iter == m_kButtonContainer.end() )
	{
		return;
	}

	iter->second.OwnerDraw(lpDIS);
}

void CPatch::Paint(HDC hDC)
{
	kImageCont::iterator	iter = m_kImageContainer.find(1);
	if( iter == m_kImageContainer.end() )
	{
		return;
	}

	kImageCont::iterator	Prog1_iter = m_kImageContainer.find(HBT_PROGRESS1);
	kImageCont::iterator	Prog2_iter = m_kImageContainer.find(HBT_PROGRESS2);
	if( Prog1_iter == m_kImageContainer.end() || Prog2_iter == m_kImageContainer.end() )
	{
		return;
	}

	int const iWidth = iter->second.GetWidth();
	int const iHeight = iter->second.GetHeight();

	HDC	MemDC = CreateCompatibleDC(hDC);
	HBITMAP	NewBitmap = CreateCompatibleBitmap(hDC, iWidth, iHeight);
	HBITMAP OldBitmap = static_cast<HBITMAP>(SelectObject(MemDC, NewBitmap));

	HFONT	NewFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
								DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
								DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
	HFONT	OldFont = static_cast<HFONT>(SelectObject(MemDC, NewFont));
	SetBkMode(MemDC, TRANSPARENT);

	iter->second.DrawBitmap(MemDC, 0, 0, SRCCOPY);

	TCHAR	szTemp[MAX_PATH] = {0,};

	RECT	rcDraw = {22, 272, 492, 294 };
	DrawText(MemDC, m_wstrTextState.c_str(), m_wstrTextState.size(), &rcDraw, DT_LEFT | DT_VCENTER);

	OffsetRect(&rcDraw, 0, 21);
	swprintf(szTemp, MAX_PATH, L"%.1f / 100.0", (m_fFilePercent * 100.0f));
	Prog1_iter->second.DrawBitmap(MemDC, 49, 294, m_fFilePercent, SRCCOPY);
	DrawText(MemDC, szTemp, -1, &rcDraw, DT_CENTER | DT_VCENTER);

	OffsetRect(&rcDraw, 0, 15);
	swprintf(szTemp, MAX_PATH, L"%.1f / 100.0", (m_fTotalPercent * 100.0f));
	Prog2_iter->second.DrawBitmap(MemDC, 49, 309, m_fTotalPercent, SRCCOPY);
	DrawText(MemDC, szTemp, -1, &rcDraw, DT_CENTER | DT_VCENTER);

	SetRect(&rcDraw, 22, 342, 492, 364);

	BitBlt(hDC, 0, 0, iWidth, iHeight, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldFont);
	DeleteObject(NewFont);
	SelectObject(MemDC, OldBitmap);
	DeleteObject(NewBitmap);
	DeleteDC(MemDC);
}

void CPatch::SetWorkState(const std::wstring& OutText)
{
	m_wstrTextState = OutText;
}

void CPatch::ReadChangeLog()
{
	std::vector<char> vec_ChangeLog;
	BM::FileToMem(L"changelog.txt", vec_ChangeLog);
	vec_ChangeLog.push_back('\0');
	char* szText = new char[vec_ChangeLog.size()];
	memcpy(szText, &vec_ChangeLog.at(0), vec_ChangeLog.size());
	std::wstring strText = UNI(szText);

	typedef	std::list<std::wstring>	CUT_STRING;
	CUT_STRING	kText;
	BM::vstring::CutTextByKey<std::wstring>(strText, L"\r\n", kText);
	CUT_STRING::iterator itr = kText.begin();
	for(int i=0; itr != kText.end(); ++itr, ++i)
	{
		SendMessage(m_ListBoxChangeLog.GetWnd(), LB_ADDSTRING, i, (LPARAM)(*itr).c_str());  
	}

	//SendMessage(m_ListBoxChangeLog.GetWnd(), LB_ADDSTRING, 0, (LPARAM)vec_ChangeLog[0]);  
	//GetDlgItem(g_hDlgWnd, IDC_LIST_LOG);
	//SendMessage(GetDlgItem(g_hDlgWnd, IDC_LIST_LOG), LB_ADDSTRING, 0, vec_ChangeLog[0]);  
}

void CPatch::Command(WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD(wParam) )
	{
	case 1004: 
		{
			PatchCancel();
		}
		break;
	default:
		{
		}
		break;
	}
}

void CPatch::PatchCancel()
{
	BM::CAutoMutex kLock(m_kMutex);
	HWND MessageBoxWnd = FindWindow(NULL, TEXT_PATCH) ;
	if(MessageBoxWnd)
	{
		return;
	}
	m_bThreadRun = false;
	int iRtn = MessageBoxW(g_hWnd, TEXT_DO_YOU_WANT_CANCEL, TEXT_PATCH, MB_OKCANCEL|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);

	if(iRtn == IDOK)
	{
		m_bThreadRun = true;
		m_bThreadEnd = true;
	}
	else
	{
		m_bThreadRun = true;
	}
}

void CPatch::CheckRun()
{
	while(!m_bThreadRun)
	{
		Sleep(100);
	}
}

bool CPatch::ClientVersionCheck()
{
	// 클라이언트 버전
	std::wstring strPatchID;
	if(_access("patch.id", 0) == -1)
	{
		MessageBoxW(g_hWnd, TEXT_HAVE_NOT_CLIENT, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
		return false;
	}
	else
	{
		strPatchID = GetPatchID(_T("patch.id"));
		m_kClientVer = strPatchID;					// 클라이언트 버전 저장
		if(!strPatchID.size())
		{
			MessageBoxW(g_hWnd, TEXT_HAVE_NOT_CLIENT, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
			return false;
		}
	}

	std::wstring strNewVersion = m_szNewVersion;
	if(!strPatchID.compare(strNewVersion))
	{// 패치할 New 버전과 현재 클라이언트 버전이 같음
		MessageBoxW(g_hWnd, TEXT_ALREADY_PATCHED, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
		return false;
	}

	if(!IsAbleToPatch(strPatchID))
	{// 최소 버전과, New 버전 사이의 클라이언트 버전이 아님		
		return false;
	}	
	return true;
}

bool CPatch::IsAbleToPatch(std::wstring const& kCurVer)
{	
	BM::VersionInfo const kOldVerInfo = GetVerInfo(m_szOldVersion);	
	BM::VersionInfo const kNewVerInfo = GetVerInfo(m_szNewVersion);
	BM::VersionInfo const kNowVerInfo = GetVerInfo(kCurVer);	
	
	{// 최소 버전보다는 같거나 커야한다.
		bool bRet = true;
		if(kOldVerInfo.Version.i16Major > kNowVerInfo.Version.i16Major)
		{// Major 버전 보다 더 작으면 최소 요구버전 보다 이전버전
			bRet = false;
		}
		else if(kOldVerInfo.Version.i16Major == kNowVerInfo.Version.i16Major)
		{// Major이 같은데
			if(kOldVerInfo.Version.i16Minor > kNowVerInfo.Version.i16Minor)
			{// Minor 버전이 더 작으면 최소 요구버전 보다 이전버전
				bRet = false;
			}
			else if(kOldVerInfo.Version.i16Minor == kNowVerInfo.Version.i16Minor)
			{// Major, Minor 버전이 같은데
				if(kOldVerInfo.Version.i32Tiny > kNowVerInfo.Version.i32Tiny)
				{// Tiny 버전이 더 작으면 최소 요구 버전 보다 이전버전
					bRet = false;
				}
			}
		}
		if(false == bRet)
		{
			std::wstring kStr = TEXT_DIFFERENT_VERSION;
			kStr+=L"\n";
			kStr+=TEXT_REQ_BETWEEN_VER;
			kStr+=m_szOldVersion;
			kStr+=L" ~ ";
			kStr+=m_szNewVersion;
			kStr+=L"\n";
			kStr+=TEXT_CUR_CLIENT_VER+kCurVer;
			MessageBoxW(g_hWnd, kStr.c_str(), TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
			return false;
		}
	}

	{// New 버전 보다는 작아야 한다
		if(kNewVerInfo.Version.i16Major > kNowVerInfo.Version.i16Major)
		{// New 버전 보다 현재 버전이 작으면 패치 가능
			return true;
		}
		else if(kNewVerInfo.Version.i16Major < kNowVerInfo.Version.i16Major)
		{
			MessageBoxW(g_hWnd, TEXT_ALREADY_PATCHED, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
			return false;
		}
		if(kNewVerInfo.Version.i16Minor > kNowVerInfo.Version.i16Minor)
		{
			return true;
		}
		else if(kNewVerInfo.Version.i16Minor < kNowVerInfo.Version.i16Minor)
		{
			MessageBoxW(g_hWnd, TEXT_ALREADY_PATCHED, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
			return false;
		}
		if(kNewVerInfo.Version.i32Tiny > kNowVerInfo.Version.i32Tiny)
		{
			return true;
		}
		else if(kNewVerInfo.Version.i32Tiny < kNowVerInfo.Version.i32Tiny)
		{
			MessageBoxW(g_hWnd, TEXT_ALREADY_PATCHED, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
			return false;
		}
	}
	// 클라이언트 현재 버전이 New버전과 같은 경우이나, ClientVersionCheck()에서 같은 경우는 미리 체크하고있다.
	return false;
}

BM::VersionInfo CPatch::GetVerInfo(std::wstring const& kVerStr)
{
	BM::VersionInfo kVerInfo;
	kVerInfo.iVersion = 0;
	if(kVerStr.empty())
	{
		return kVerInfo;
	}

	typedef	std::list<std::wstring>	CUT_STRING;
	
	CUT_STRING	kNumberList;
	BM::vstring::CutTextByKey(kVerStr, std::wstring(L"."), kNumberList);
	
	std::wstring	wstrClient = L"";
	std::wstring	wstrCount = L"";
	std::wstring	wstrLimit = L"";

	if( kNumberList.size() == 3 )
	{
		wstrClient = kNumberList.begin()->c_str(); 
		kNumberList.pop_front();
		wstrCount = kNumberList.begin()->c_str(); 
		kNumberList.pop_front();
		wstrLimit = kNumberList.begin()->c_str(); 
		kNumberList.pop_front();

		kVerInfo.Version.i16Major = _wtoi64(wstrClient.c_str());
		kVerInfo.Version.i16Minor = _wtoi64(wstrCount.c_str());
		kVerInfo.Version.i32Tiny = _wtoi64(wstrLimit.c_str());
	}
	return kVerInfo;
}

void CPatch::CheckFinish()
{
	if(!m_bFinish)
		return;

	m_bFinish = false;// 한번이상 들어오지 않게

	if(m_bThreadEnd && !m_bFinishResult)
	{
		MessageBoxW(g_hWnd, TEXT_UPDATE_CANCEL, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
	}
	else if(!m_bThreadEnd && m_bFinishResult)
	{
		HWND MessageBoxWnd = FindWindow(NULL, TEXT_PATCH) ;
		if(MessageBoxWnd)
		{
			EndDialog(MessageBoxWnd, TRUE);
		}
		MakePatchID(MB(m_szNewVersion));
#ifndef CHINA_LOCAL
		MessageBoxW(g_hWnd, TEXT_PATCH_COMPLETE, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
#endif
	}

	//if(!FindProcess(L"Launcher.exe"))
	{
		HWND handle = 0;
		HINSTANCE kIns = ShellExecute(handle, L"open", L"Launcher.exe", L"DD=1", L"", SW_SHOWNORMAL);
	}

	SendMessage(g_hWnd, WM_DESTROY, 0, 0);
}

bool CPatch::UpdatePackVer(BM::FolderHash& kAllPackList)
{
	//	버젼만 바꿀 팩 리스트
	BM::FolderHash::iterator	Cl_iter = kAllPackList.begin();
	while(Cl_iter != kAllPackList.end())
	{
		std::wstring	wstrFileName = Cl_iter->first;
		BM::DetachFolderMark(wstrFileName);
		wstrFileName += L".DAT";
		if(!BM::PgDataPackManager::UpdatePackVersion(wstrFileName, m_kNewVersion))
		{
			return false;
		}

		++Cl_iter;
	}

	return	true;
}

__int64 CPatch::ReadPatchVer(std::wstring strVersion)
{
	typedef	std::list<std::wstring>	CUT_STRING;

	CUT_STRING	kNumberList;
	std::wstring	wstrKey = strVersion;
	BM::vstring::CutTextByKey<std::wstring>(wstrKey, L".", kNumberList);

	if(kNumberList.size() != 3){ return 0; }

	CUT_STRING::iterator	iter = kNumberList.begin();

	BM::VersionInfo	Info;
	Info.Version.i16Major = (WORD)_wtoi(iter->c_str());	++iter;
	Info.Version.i16Minor = (WORD)_wtoi(iter->c_str()); ++iter;
	Info.Version.i32Tiny = _wtoi(iter->c_str());
	return Info.iVersion;
}

//bool CPatch::FindProcess(std::wstring strExeName)
//{
//	std::wstring strLauncher = strExeName;
//
//	UPR(strLauncher);
//
//	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//
//	if ( (int)hSnapshot != -1 )
//	{
//		PROCESSENTRY32 pe32 ;
//		pe32.dwSize=sizeof(PROCESSENTRY32);
//		BOOL bContinue ;
//		std::wstring strProcessName;
//
//		if(Process32First(hSnapshot, &pe32))
//		{
//			do
//			{
//				strProcessName = pe32.szExeFile; //strProcessName이 프로세스 이름;
//				UPR(strProcessName);
//				if(strProcessName == strLauncher)
//				{
//					HANDLE      hProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID );
//					if( hProcess )
//					{
//						return true;
//					}
//
//					return false;
//				}
//				bContinue = Process32Next( hSnapshot, &pe32 );
//			}while ( bContinue );
//		}
//		CloseHandle( hSnapshot );
//	}
//	return false;
//}