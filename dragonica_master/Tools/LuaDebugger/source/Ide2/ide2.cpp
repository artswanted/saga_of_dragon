// ide2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ide2.h"

#include "MainFrame.h"
#include "LuaFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"

#include "pgserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIdeApp

BEGIN_MESSAGE_MAP(CIdeApp, CWinApp)
	//{{AFX_MSG_MAP(CIdeApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdeApp construction

CIdeApp::CIdeApp()
{
	m_hScintilla = NULL;
	m_pkPgServer = 0;
}

CIdeApp::~CIdeApp()
{
	m_pkPgServer->DisConnect();
	m_pkPgServer->Terminate();
	delete m_pkPgServer;
	m_pkPgServer = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIdeApp object

CIdeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIdeApp initialization

BOOL CIdeApp::InitInstance()
{
	m_hScintilla = LoadLibrary("SciLexer.DLL");
	if ( !m_hScintilla )
	{
		AfxMessageBox("Can't load Scintilla dll 'SciLexer.dll'",
			MB_OK|MB_ICONERROR);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("LuaIde"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pLuaTemplate = new CMultiDocTemplate(
		IDR_IDE2TYPE,
		RUNTIME_CLASS(CLuaDoc),
		RUNTIME_CLASS(CLuaFrame), // custom MDI child frame
		RUNTIME_CLASS(CLuaView));
	AddDocTemplate(m_pLuaTemplate);

	m_pkPgServer = new CPgServer;
	m_pkPgServer->Initialize();

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	m_pMainWnd = pMainFrame;

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	pMainFrame->SetMode(CMainFrame::modeNoProject);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// must be here because load frame of child view takes accel table from parent
	// and we want it to be null then
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CIdeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CIdeApp message handlers


int CIdeApp::ExitInstance() 
{
	if ( m_hScintilla )
		FreeLibrary(m_hScintilla);		

	return CWinApp::ExitInstance();
}

CLuaView* CIdeApp::FindProjectFilesView(CProjectFile *pPF)
{
	POSITION pos = m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->GetNextDoc(pos);
		if ( pDoc->GetProjectFile() == pPF )
			return pDoc->GetView();
	}

	return NULL;
}

CLuaView* CIdeApp::LoadProjectFilesView(CProjectFile *pPF)
{
	CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->OpenDocumentFile(pPF->GetPathName(),TRUE);
	if ( pDoc )
		return pDoc->GetView();
	else
		return NULL;
}

CLuaView* CIdeApp::OpenProjectFilesView(CProjectFile *pPF, int nLine)
{
	CLuaView* pView = FindProjectFilesView(pPF);
	if ( !pView )
		pView = LoadProjectFilesView(pPF);

	if ( pView )
	{
		pView->Activate();

		if ( nLine>=0 )
			pView->GetEditor()->GotoLine(nLine);
	}

	return pView;
}

BOOL CIdeApp::SaveModifiedDocuments()
{
	BOOL bModified = FALSE;

	POSITION pos = m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->GetNextDoc(pos);
		if ( !pDoc->IsInProject() )
			continue;

		if ( pDoc->IsModified() )
		{
			pDoc->DoFileSave();
			bModified = TRUE;

			// writing new lines changes breakpoint positions
			pDoc->GetView()->GetEditor()->SetBreakPointsIn(pDoc->GetProjectFile());
		}
	}

	return bModified;
}

void CIdeApp::FormatMessage(char *pszAPI)
{
	LPVOID lpMsgBuf;
	::FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	CString str;
	str.Format("ERROR: API    = %s.\n   error code = %d.\n   message    = %s.\n",
		pszAPI, GetLastError(), lpMsgBuf);
	MessageBox( NULL, (LPCTSTR)str, "Error", MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

CString CIdeApp::GetModuleDir()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
   	char lpFilename[MAX_PATH];

	GetModuleFileName(GetModuleHandle(NULL), lpFilename, MAX_PATH);
	_splitpath( lpFilename, drive, dir, fname, ext );
	dir[strlen(dir)-1] = '\0';

	return CString(drive) + CString(dir);
}


void CIdeApp::DeleteAllFilesInCurrentDir()
{
	BOOL bFound;
	CFileFind ff;
	if ( ff.FindFile() )
		do
		{
			bFound = ff.FindNextFile();
			CString strPathName = ff.GetFilePath();
			if ( strPathName!=".." && strPathName!="." )
				DeleteFile(strPathName);
		}
		while ( bFound ) ;
}

BOOL CIdeApp::FirstFileIsNewer(CString strPathName1, CString strPathName2)
{
	WIN32_FILE_ATTRIBUTE_DATA attrFile1, attrFile2;

	if (! ::GetFileAttributesEx(strPathName1, GetFileExInfoStandard, &attrFile1) )
		return TRUE;

	if (! ::GetFileAttributesEx(strPathName2, GetFileExInfoStandard, &attrFile2) )
		return TRUE;

	ULARGE_INTEGER time1, time2;
	time1.LowPart = attrFile1.ftLastWriteTime.dwLowDateTime;
	time1.HighPart = attrFile1.ftLastWriteTime.dwHighDateTime;
	time2.LowPart = attrFile2.ftLastWriteTime.dwLowDateTime;
	time2.HighPart = attrFile2.ftLastWriteTime.dwHighDateTime;

	return ( time1.QuadPart > time2.QuadPart );
}
