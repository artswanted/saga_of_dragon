// Debugger.cpp: implementation of the CDebugger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "Debugger.h"

#include "MainFrame.h"
#include "ScintillaView.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "pgserver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugger* CDebugger::m_pDebugger = NULL;

CDebugger::CDebugger()
{
	m_pDebugger = this;
	m_pThread = NULL;
	m_bIsGetCalltip = false;
}

CDebugger::~CDebugger()
{
	if(m_pThread!=NULL)
 		delete m_pThread;
}

BOOL CDebugger::Prepare()
{
	m_hWndMainFrame = AfxGetMainWnd()->GetSafeHwnd();

	CProject *pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	if ( pProject->PositionBreakPoints() )
		AfxMessageBox("하나 이상의 브레이크 포인트가 사용 가능한 곳에 지정되어 있지 않습니다. 그 포인터는 다른 줄로 옮겨집니다.", MB_OK);

	g_pkPgServer->Initialize();
	g_pkPgServer->Connect();
	g_pkPgServer->MakeThread();

	m_nMode = DMOD_NONE;
	
	return TRUE;
}

BOOL CDebugger::Start()
{
	//CProject *pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	//if ( pProject && pProject->PositionBreakPoints() )
	//	AfxMessageBox("하나 이상의 브레이크 포인트가 사용 가능한 곳에 지정되어 있지 않습니다. 그 포인터는 다른 줄로 옮겨집니다.", MB_OK);

	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);

	return true;
}

void CDebugger::ClearDebugEvalData()
{
	m_kDebugLocalList.clear();
	m_kDebugWatchList.clear();
	ClearLocalVariables();
	RedrawWatchList();
}

void CDebugger::DrawDebugData(char *pkString)
{
	PgDebugInfo *pkDebugInfo = (PgDebugInfo *)pkString;
	PgDebugInfo kDebugLocalInfo;
	
	char szCurDir[512];
	char szTemp[512];
	sprintf_s(szTemp, 512, "./%s", pkDebugInfo->acFilename);
	CDebugger::GetDebugger()->DebugBreak(
		szTemp, 
		pkDebugInfo->aiBreakLines[0]
	);

	kDebugLocalInfo.iLocalVariableCount = 0;
	for (int i=0 ; i<pkDebugInfo->iLocalVariableCount ; i++)
	{
		if (
			strcmp(pkDebugInfo->acLocalVariableName[i],
			"(*temporary)") )
		{
			AddLocalVariable(
				pkDebugInfo->acLocalVariableName[i],
				pkDebugInfo->acLocalVariableType[i],
				pkDebugInfo->acLocalVariableValue[i]);

			strcpy_s(kDebugLocalInfo.acLocalVariableName[i], 
				pkDebugInfo->acLocalVariableName[i]);
			strcpy_s(kDebugLocalInfo.acLocalVariableType[i], 
				pkDebugInfo->acLocalVariableType[i]);
			strcpy_s(kDebugLocalInfo.acLocalVariableValue[i], 
				pkDebugInfo->acLocalVariableValue[i]);

			kDebugLocalInfo.iLocalVariableCount += 1;
		}
	}

	m_kDebugLocalList.insert(std::make_pair(kDebugLocalInfo.acFilename, kDebugLocalInfo));
}

void CDebugger::AddWatchList(char *pkName, char *pkType, char *pkValue)
{
	PgDebugEvalInfo kDebugWatchInfo;
	strcpy_s(kDebugWatchInfo.acVariableName, sizeof(char)*64, 
		pkName);
	strcpy_s(kDebugWatchInfo.acVariableType, sizeof(char)*64,
		pkType);
	strcpy_s(kDebugWatchInfo.acVariableValue, sizeof(char)*64,
		pkValue);

	m_kDebugWatchList.push_back(kDebugWatchInfo);
}

void CDebugger::AddWatchListEnd()
{
	::SendMessage(m_hWndMainFrame, DMSG_REDRAW_WATCHES, 0, 0);
}

void CDebugger::RedrawWatchList()
{
	::SendMessage(m_hWndMainFrame, DMSG_RESEND_WATCHES, 0, 0);
}

void CDebugger::AddStackList(PgDebugStackInfo *pkRecvStackInfo)
{
	ClearStackTrace();

	char szDesc[256];
	for (int i=0 ; i<pkRecvStackInfo->iLevel ; i++)
	{
		memset(szDesc, 0, sizeof(szDesc));

		szDesc[0] = '\0';
		if ( pkRecvStackInfo->acName[i] )
			strcat(szDesc, pkRecvStackInfo->acName[i]);
		strcat(szDesc, ",");
		if ( pkRecvStackInfo->acNamewhat[i] )
			strcat(szDesc, pkRecvStackInfo->acNamewhat[i]);
		strcat(szDesc, ",");
		if ( pkRecvStackInfo->acWhat[i] )
			strcat(szDesc, pkRecvStackInfo->acWhat[i]);
		strcat(szDesc, ",");
		if ( pkRecvStackInfo->acShort_src[i] )
			strcat(szDesc, pkRecvStackInfo->acShort_src[i]);

		AddStackTrace(szDesc, pkRecvStackInfo->acSource[i]+1, pkRecvStackInfo->aiCurrentline[i]);
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////
// thread functions
//////////////////////////////////////////////////////////////////////////////////////////////

UINT CDebugger::StartDebugger()
{
	//m_nLine = 0;
	//m_nLevel = 0;

	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);
	
	//
	//int nRet = m_lua.StartDebugger();
	//EndThread();

	return 0;
}

UINT CDebugger::StartDebugger( LPVOID pParam )
{	
	return ((CDebugger*)pParam)->StartDebugger();
}

void CDebugger::Write(const char* szMsg)
{
	::SendMessage(m_hWndMainFrame, DMSG_WRITE_DEBUG, (WPARAM)szMsg, 0);
}

void CDebugger::LineHook(const char *szFile, int nLine)
{
	if ( m_nMode == DMOD_STOP )
		EndThread();

	bool bCheck = ::SendMessage(m_hWndMainFrame, DMSG_HAS_BREAKPOINT, (WPARAM)szFile, (LPARAM)nLine);


	if ( ::SendMessage(m_hWndMainFrame, DMSG_HAS_BREAKPOINT, (WPARAM)szFile, (LPARAM)nLine) ||
		m_nMode==DMOD_STEP_INTO || 
		m_nMode==DMOD_BREAK ||
		(m_nMode==DMOD_STEP_OVER && m_nLevel<=0) || 
		(m_nMode==DMOD_STEP_OUT && m_nLevel<0) ||
		(m_nMode==DMOD_RUN_TO_CURSOR && m_strPathName.CompareNoCase(szFile)==0 && m_nLine==nLine) )
	{
		DebugBreak(szFile, nLine);
	}
}

void CDebugger::FunctionHook(const char *szFile, int nLine, BOOL bCall)
{
	if ( m_nMode == DMOD_STOP )
		EndThread();

	m_nLevel += (bCall?1:-1);
}

void CDebugger::DebugBreak(const char *szFile, int nLine)
{
	m_nMode = DMOD_NONE;

	::SendMessage(m_hWndMainFrame, DMSG_GOTO_FILELINE_FORDEBUG, (WPARAM)szFile, (LPARAM)nLine);

	// 따로 구현 해줘야함.
	//m_lua.DrawStackTrace();
	//m_lua.DrawGlobalVariables();
	//::SendMessage(m_hWndMainFrame, DMSG_REDRAW_WATCHES, 0, 0);
	//::SendMessage(m_hWndMainFrame, DMSG_GOTO_STACKTRACE_LEVEL, 0, 0);

	//m_event.ResetEvent();
	//
	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_BREAK, 0, 0);
	//CSingleLock lock(&m_event, TRUE);

	if ( m_nMode == DMOD_STOP )
		EndThread();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void CDebugger::SendCmdUntilBreak(PgCommandInfo::ECommandInfoType eType)
{
	CLuaView* pView = ((CMainFrame*)AfxGetMainWnd())->GetActiveView();
	pView->GetEditor()->ClearAllLine();
	// 새로운 break Data 전송
	g_pkPgServer->SendBreakData();
	
	// go data를 PG에
	PgCommandInfo kCmdInfo;
	kCmdInfo.ePacketType = PgDebugInfo::PacketType_Command_All;
	kCmdInfo.eCommandType = eType;
	g_pkPgServer->Send(sizeof(PgCommandInfo), ((char *)&kCmdInfo));
}

void CDebugger::Go()
{
	m_event.SetEvent();
	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);

	SendCmdUntilBreak(PgCommandInfo::CommandInfoType_Go);
}

void CDebugger::StepInto()
{
	m_nMode = DMOD_STEP_INTO;
	//Go();

	m_event.SetEvent();
	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);

	SendCmdUntilBreak(PgCommandInfo::CommandInfoType_StepInto);
}

void CDebugger::StepOver()
{
	m_nMode = DMOD_STEP_OVER;
	//m_strPathName = m_lua.GetSource();
	//m_nLevel = 0;
	//Go();

	m_event.SetEvent();
	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);

	SendCmdUntilBreak(PgCommandInfo::CommandInfoType_StepOver);
}

void CDebugger::StepOut()
{
	m_nMode = DMOD_STEP_OUT;
	//m_strPathName = m_lua.GetSource();
	//m_nLevel = 0;

	m_event.SetEvent();
	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);

	SendCmdUntilBreak(PgCommandInfo::CommandInfoType_StepOut);
}

void CDebugger::RunToCursor()
{
	CLuaView* pView = ((CMainFrame*)AfxGetMainWnd())->GetActiveView();
	CLuaDoc* pDoc = pView->GetDocument();


	if ( !pDoc->IsInProject() )
		return;

	m_nMode = DMOD_RUN_TO_CURSOR;
	
	CProjectFile* pPF = pDoc->GetProjectFile();
	m_strPathName = pPF->GetPathName();

	int nLine = pView->GetEditor()->GetCurrentLine();
	//m_nLine = pPF->GetNearestDebugLine(nLine);

	Go();
}

void CDebugger::Stop()
{
	m_nMode = DMOD_STOP;
	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_END, 0, 0);
	
	//CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	//if (pProject)
	//{
	//	CPgTree kRoot = pProject->GetFiles();

	//	std::list<CPgTree*>::iterator itr;
	//	std::list<CPgTree*> listTree = kRoot.m_listChild;
	//	for (itr = listTree.begin() ;
	//		itr != listTree.end() ;
	//		++itr)
	//	{
	//		CPgTree *pkChild = *itr;
	//		pkChild->pkProjectFile->RemoveAllDebugLines();
	//	}
	//}

	//Go();
	//g_pkPgServer->DisConnect();

	if (!m_pThread)
		return;

	//MSG msg;
	//while ( WaitForSingleObject (m_pThread->m_hThread, 1)==WAIT_TIMEOUT )
	//	if ( ::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE) )
	//		AfxGetThread()->PumpMessage ();

	//delete m_pThread;
 //	m_pThread=NULL;
}

void CDebugger::Break()
{
	m_nMode = DMOD_BREAK;
}

void CDebugger::ClearStackTrace()
{
	::SendMessage(m_hWndMainFrame, DMSG_CLEAR_STACKTRACE, 0, 0);
}

void CDebugger::AddStackTrace(const char* szDesc, const char* szFile, int nLine)
{
	StackTrace st;
	st.szDesc = szDesc;
	st.szFile = szFile;
	st.nLine = nLine;

	::SendMessage(m_hWndMainFrame, DMSG_ADD_STACKTRACE, (WPARAM)&st, 0);
}

int CDebugger::GetStackTraceLevel()
{
	return ::SendMessage(m_hWndMainFrame, DMSG_GET_STACKTRACE_LEVEL, 0, 0);
}

void CDebugger::StackLevelChanged()
{
	//m_lua.DrawLocalVariables();
}

void CDebugger::ClearLocalVariables()
{
	::SendMessage(m_hWndMainFrame, DMSG_CLEAR_LOCALVARIABLES, 0, 0);
}

void CDebugger::AddLocalVariable(const char *name, const char *type, const char *value)
{
	Variable var;
	var.szName = name;
	var.szType = type;
	var.szValue = value;

	::SendMessage(m_hWndMainFrame, DMSG_ADD_LOCALVARIABLE, (WPARAM)&var, 0);
}

void CDebugger::ClearGlobalVariables()
{
	::SendMessage(m_hWndMainFrame, DMSG_CLEAR_GLOBALVARIABLES, 0, 0);
}

void CDebugger::AddGlobalVariable(const char *name, const char *type, const char *value)
{
	Variable var;
	var.szName = name;
	var.szType = type;
	var.szValue = value;

	::SendMessage(m_hWndMainFrame, DMSG_ADD_GLOBALVARIABLE, (WPARAM)&var, 0);
}

BOOL CDebugger::GetCalltip(const char *szWord, char *szCalltip)
{
	PgDebugCalltipInfo kCalltipInfo;
	kCalltipInfo.ePacketType = PgLuaDebugPacket::PacketType_Debug_Calltip;
	strcpy_s(kCalltipInfo.acWord, szWord);
	//strcpy_s(kCalltipInfo.acCalltip, szCalltip);
	kCalltipInfo.iLevel = GetStackTraceLevel();
	g_pkPgServer->Send(sizeof(PgDebugCalltipInfo), ((char *)&kCalltipInfo));

	int iWaitcount = 0;
	while( !m_bIsGetCalltip &&
		iWaitcount < 100 )
	{
		Sleep(1);
		iWaitcount += 1;
	}
	if (iWaitcount>=100 ||
		!m_szCalltip[0])
		return false;

	strcpy_s(szCalltip, sizeof(char)*256, m_szCalltip);
	m_bIsGetCalltip = false;

	return true;
}

void CDebugger::SetCalltip(PgDebugCalltipInfo *pkCalltipInfo)
{
	if (pkCalltipInfo->bIsFound)
	{
		char szPrefix[32];
		if (pkCalltipInfo->bIsLocal)
			strcpy_s(szPrefix, "Local");
		else
			strcpy_s(szPrefix, "Global");
		// 합성
		sprintf_s(m_szCalltip, "%s %s: %s %s",
			szPrefix, pkCalltipInfo->acName,
			pkCalltipInfo->acType, pkCalltipInfo->acValue);
	}
	else
	{
		memset(m_szCalltip, 0, sizeof(m_szCalltip));
	}

	m_bIsGetCalltip = true;
}

void CDebugger::EndThread()
{
}

CString CDebugger::Eval(CString strCode)
{
 	for(LuaDbgPacket::iterator itr = m_kDebugLocalList.begin();
		itr != m_kDebugLocalList.end();
		++itr)
	{
		PgDebugInfo kDebugLocalInfo = itr->second;
		for (int i=0 ; i<kDebugLocalInfo.iLocalVariableCount ; i++)
		{
			if (strcmp(
				kDebugLocalInfo.acLocalVariableName[i],
				strCode) == 0)
			{
				return kDebugLocalInfo.acLocalVariableValue[i];
			}
		}
	}

 	for(LuaEvalPacket::iterator itr = m_kDebugWatchList.begin();
		itr != m_kDebugWatchList.end();
		++itr)
	{
		PgDebugEvalInfo kDebugWatchInfo = *itr;
		if (strcmp(
			kDebugWatchInfo.acVariableName,
			strCode) == 0)
		{
			char szRet[256];
			strcpy_s(szRet, kDebugWatchInfo.acVariableType);
			strcat_s(szRet, ": ");
			strcat_s(szRet, kDebugWatchInfo.acVariableValue);
			return szRet;
		}
	}

	return "";

	//strCode = "return " + strCode;
	//CString strRet;
	////m_lua.Eval(strCode.GetBuffer(0), strRet.GetBuffer(256));
	//strRet.ReleaseBuffer();

	//return strRet;
}

void CDebugger::Execute()
{
	char szHostname[20];
	hostent *phostent;
	gethostname(szHostname,20); //내 호스트 이름얻음 
	phostent=gethostbyname(szHostname);//호스트 이름을 통해서 내 ip 주소 얻음 
	char *szIP = inet_ntoa(*(IN_ADDR *)(phostent->h_addr));

	// Shifeed 여기서 클라이언트 접속하고 브레이크 정보 전달
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	CString strExecute = pProject->GetExecuteDir() + pProject->GetExecute();
	CString strParameters;
	strParameters = " -luadebug ";
	strParameters += szIP;
	strParameters += ",5001";

	CString test = "E:/Workpro/LuaIDE/LuaIDE/LuaIDE.exe";

	HINSTANCE kIns = ::ShellExecute(NULL, "open",
		pProject->GetExecute(),
		strParameters,
		pProject->GetExecuteDir(),
		SW_SHOW);

	if (!kIns)
	{
		g_pkPgServer->TermThread();
		g_pkPgServer->DisConnect();
	}

	//CString strInterpreter = "F:\\a ja tak\\lubie cie bardzo\\lua.exe"; //theApp.GetModuleDir() + "\\" + "lua.exe";
	//CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();

	//_spawnl( _P_NOWAIT, strInterpreter, strInterpreter, "\"" + pProject->GetDebugPathNameExt() + "\"", NULL );
}
