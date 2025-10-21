// PatcherPatchDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include <shlwapi.h>
#include <Tlhelp32.h>
#include "PatcherPatch.h"
#include "PatcherPatchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPatcherPatchDlg 대화 상자




CPatcherPatchDlg::CPatcherPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatcherPatchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPatcherPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPatcherPatchDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CPatcherPatchDlg 메시지 처리기

BOOL CPatcherPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	CString strNewPatcherName;
	CString strOldPatcherName;
	CString strPatcherName;

	strNewPatcherName	= TEXT("Patcher2.exe");
	strPatcherName		= TEXT("Patcher.exe");
	size_t tryCount		= 10;

	// 1. 기존 패처가 살아 있다면 강제로 죽입니다.
	while(tryCount > 0 && FindProcess(strPatcherName) == TRUE)
	{
		KillProcess(strPatcherName);
		Sleep(1000);
		tryCount--;
	}

	if (FindProcess(strPatcherName) == TRUE)
	{
		MessageBox(LoadRCString(IDS_ERROR_STRING_4), TEXT("Failed to patch please restart patcher.exe"), 0);
		PostQuitMessage(WM_QUIT);
		return TRUE;
	}

	Sleep(1000);

	// 4. 패처를 변경할 파일이 있는지 확인합니다.
	if(PathFileExists(strNewPatcherName) == FALSE)
	{
		MessageBox(LoadRCString(IDS_ERROR_STRING_1), TEXT("PATCHER PATCH ERROR CODE : 1"), 0);

		RunPatcher();
		PostQuitMessage(WM_QUIT);
		return TRUE;
	}

	// 5. 기존 패처는 삭제
	if(DeleteFile(strPatcherName) == FALSE)
	{
		MessageBox(LoadRCString(IDS_ERROR_STRING_2), TEXT("PATCHER PATCH ERROR CODE : 2"), 0);

		// 실패 했다면 
		RunPatcher();
		PostQuitMessage(WM_QUIT);
		return TRUE;
	}

	// 6. 이름 변경을 합니다!
	if(_trename(strNewPatcherName, strPatcherName) != 0)
	{
		MessageBox(LoadRCString(IDS_ERROR_STRING_3), TEXT("PATCHER PATCH ERROR CODE : 3"), 0);
	}

	// 7. 패처 실행
	RunPatcher();

	// 8. 닫기
	PostQuitMessage(WM_QUIT);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPatcherPatchDlg::OnPaint()
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
HCURSOR CPatcherPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CPatcherPatchDlg::RunPatcher()
{
	TCHAR szDirectory[MAX_PATH] = {0,};
	GetCurrentDirectory(MAX_PATH, szDirectory);

	HINSTANCE kInstance = ShellExecute(	NULL,
										_T("open"),
										_T(".\\Patcher.exe"),
										NULL,
										szDirectory,
										SW_SHOWNORMAL);

	return TRUE;
}

BOOL CPatcherPatchDlg::FindProcess(CString sExeName)
{
	sExeName.MakeUpper();

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if ( (int)hSnapshot != -1 )
	{
		PROCESSENTRY32 pe32 ;
		pe32.dwSize=sizeof(PROCESSENTRY32);
		BOOL bContinue ;
		CString strProcessName;

		if(Process32First(hSnapshot, &pe32))
		{
			do
			{
				strProcessName = pe32.szExeFile; //strProcessName이 프로세스 이름;
				strProcessName.MakeUpper();
				if( ( strProcessName.Find(sExeName,0) != -1 ) )
				{
					HANDLE      hProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID );
					if( hProcess )
					{
						return TRUE;
					}

					return FALSE;
				}
				bContinue = Process32Next( hSnapshot, &pe32 );
			}while ( bContinue );
		}
		CloseHandle( hSnapshot );
	}
	return FALSE;
}



BOOL CPatcherPatchDlg::KillProcess(CString sExeName)
{
	sExeName.MakeUpper();

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if ( (int)hSnapshot != -1 )
	{
		PROCESSENTRY32 pe32 ;
		pe32.dwSize=sizeof(PROCESSENTRY32);
		BOOL bContinue ;
		CString strProcessName;

		if(Process32First(hSnapshot, &pe32))
		{
			do
			{
				strProcessName = pe32.szExeFile; //strProcessName이 프로세스 이름;
				strProcessName.MakeUpper();
				if( ( strProcessName.Find(sExeName,0) != -1 ) )
				{
					HANDLE      hProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID );
					if( hProcess )
					{
						DWORD       dwExitCode;
						GetExitCodeProcess( hProcess, &dwExitCode);
						TerminateProcess( hProcess, dwExitCode);
						CloseHandle(hProcess);
						CloseHandle( hSnapshot );
						return TRUE;
					}

					return FALSE;
				}
				bContinue = Process32Next( hSnapshot, &pe32 );
			}while ( bContinue );
		}
		CloseHandle( hSnapshot );
	}
	return FALSE;
}

LPTSTR CPatcherPatchDlg::LoadRCString(UINT wID)
{
	static TCHAR szBuf[512];	// 꼭 static 이어야한다!

	szBuf[0] = '\0';

	LoadString((HINSTANCE)GetModuleHandle(NULL), wID, szBuf, sizeof(szBuf));

	return szBuf;
}