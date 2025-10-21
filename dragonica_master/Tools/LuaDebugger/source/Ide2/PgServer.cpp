#include "stdafx.h"
#include "ide2.h"
#include "MainFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include <vector>
using namespace std;
#include "PgServer.h"

CPgServer::CPgServer(void)
{
	m_bIsTermThread = false;
}

CPgServer::~CPgServer(void)
{
}

unsigned long __stdcall Thread( void *arg )
{
	// 소켓 생성
	g_pkPgServer->ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( g_pkPgServer->ServerSocket == INVALID_SOCKET )
	{
		ASSERT( !"소켓을 생성할수 없습니다." );
		WSACleanup();
		return 1;
	}

	// 포트 바인딩
	g_pkPgServer->ServerAddress.sin_family = AF_INET;
	g_pkPgServer->ServerAddress.sin_addr.s_addr = INADDR_ANY;
	g_pkPgServer->ServerAddress.sin_port = htons( g_pkPgServer->ServerPort );
	if( bind(g_pkPgServer->ServerSocket,(SOCKADDR*)&g_pkPgServer->ServerAddress,sizeof(g_pkPgServer->ServerAddress)) == SOCKET_ERROR )
	{
		ASSERT( !"bind() failed with error %d \n" );
		closesocket(g_pkPgServer->ServerSocket);
		return 2;
	}

	// 포트 열기
	if( listen(g_pkPgServer->ServerSocket,SOMAXCONN) == SOCKET_ERROR )
	{
		ASSERT( !"listen함수 설정을 실패했습니다." );
		closesocket(g_pkPgServer->ServerSocket);
		return 3;
	}

	// 여기서 부터 프로세싱
    while( !g_pkPgServer->m_bIsTermThread )
    {
		bool bDisconnected = false;

		// 접속 대기
		g_pkPgServer->ClientSocket =
			accept(g_pkPgServer->ServerSocket, 
				(struct sockaddr*)&g_pkPgServer->ClientAddress, 
				&g_pkPgServer->AddressSize); //접속을 받아들임.
		if(!g_pkPgServer ||
			g_pkPgServer->ClientSocket == INVALID_SOCKET)
		{
			break;
		}

		CDebugger::GetDebugger()->Start();

		// 브레이크-포인트 정보를 전송해주자
		g_pkPgServer->SendBreakData();

		// 여기서 부터 프로세싱
		while(!bDisconnected)
		{
			////
			// 총 데이터 길이를 받음
			int iRecvsize=0;
			int iLen=0;
			
			char *pkLen = new char[4];
			memset(pkLen, 0, 4);
			char *pkLenTemp = new char[4];
			memset(pkLenTemp, 0, 4);

			while(iRecvsize < 4 &&
				!g_pkPgServer->m_bIsTermThread)
			{
				//(char *)&iLen
				int rettmp = recv(g_pkPgServer->ClientSocket, pkLenTemp, 4-iRecvsize, 0 ); //데이터를 읽어들임.
				
				if (rettmp > 0)
				{
					memcpy(&(pkLen[iRecvsize]), pkLenTemp, rettmp);
					memset(pkLenTemp, 0, 4);
					iRecvsize += rettmp;
				}
				else if(rettmp <= 0)
				{
					bDisconnected = true;
					break;
				}
			}
			if(bDisconnected || g_pkPgServer->m_bIsTermThread)
			{
				g_pkPgServer->SendMessageDisConnect();
				break;
			}

			int *piLen = (int *)pkLen;
			iLen = *piLen;
			delete[] pkLen;
			delete[] pkLenTemp;

			////
			// 진짜 데이터를 받음
			int iGetSize = 0;
			char *pcBuffer = new char[iLen];	
			char *pcBufferTemp = new char[iLen];
			while(iGetSize < iLen &&
				!g_pkPgServer->m_bIsTermThread)
			{
				int rettmp = recv(g_pkPgServer->ClientSocket, pcBufferTemp, iLen-iGetSize, 0);

				if (rettmp > 0)
				{
					memcpy(&(pcBuffer[iGetSize]), pcBufferTemp, rettmp);
					memset(pcBufferTemp, 0, iLen);
					iGetSize += rettmp;
				}
				else if(rettmp <= 0)
				{
					bDisconnected = true;
				}
			}
			if (bDisconnected || g_pkPgServer->m_bIsTermThread)
			{
				break;
			}

			// 데이터 처리
			g_pkPgServer->PacketProcess(pcBuffer);

			delete[] pcBufferTemp;
			delete[] pcBuffer;
		}

		closesocket(g_pkPgServer->ClientSocket);
	}
	
	// 끝
	//CDebugger::GetDebugger()->ClearDebugEvalData();
	g_pkPgServer->m_bIsTermThread = false;
	closesocket( g_pkPgServer->ClientSocket ); //해당 소켓이 닫혔음.
	g_pkPgServer->DisConnect();

	CDebugger::GetDebugger()->Stop();

    return 1;
}

bool CPgServer::Initialize()
{
	m_bIsTermThread = false;

	return true;
}

bool CPgServer::Terminate()
{
	closesocket( ServerSocket ); //소켓을 닫습니다.
	WSACleanup();

	// 쓰레드 종료
	::WaitForSingleObject(m_hThread, INFINITE); //쓰레드 종료를 기다림 
	::CloseHandle(m_hThread); // 쓰레드 종료

	return true;
}

void CPgServer::Connect()
{
	ClientSocket = 0;
	ServerPort = 5001;
	
	// WinSock 모듈 장착
	if (WSAStartup(MAKEWORD( 2, 2 ),&wsaData) == SOCKET_ERROR)
	{
		ASSERT(!"WSAStartup설정에서 문제 발생");
	}

	AddressSize = sizeof( ClientAddress ); //주소 크기를 저장하는

}

void CPgServer::MakeThread()
{
    unsigned long TempValL;
	m_hThread = CreateThread( NULL, 0, Thread, 0, 0, &TempValL );

}

void CPgServer::TermThread()
{
	m_bIsTermThread = true;
}

void CPgServer::SendMessageDisConnect()
{
	CDebugger::GetDebugger()->Stop();
}

void CPgServer::DisConnect()
{
	if (!this)
		return;

	closesocket( ServerSocket ); //소켓을 닫습니다.
	WSACleanup();

	TermThread();
}

int CPgServer::Send(int iLen, char *pkString)
{
	if (g_pkPgServer->m_bIsTermThread)
		return -1;

	char *pkLen;
	pkLen = (char *)&iLen;

	int iSendsize=0;
	while(iSendsize < 4)
	{
		int ActualSendSize = send( ClientSocket,
			pkLen+iSendsize,
			4-iSendsize,
			0);

		if (ActualSendSize > 0)
			iSendsize += ActualSendSize;

		if (ActualSendSize < 0)
			return 0;
	}

	iSendsize=0;
	while(iSendsize < iLen)
	{
		int ActualSendSize =
			send( ClientSocket,
			pkString+iSendsize,
			iLen-iSendsize,
			0);

		if (ActualSendSize > 0)
			iSendsize += ActualSendSize;

		if (ActualSendSize < 0)
			return 0;
	}

	return iSendsize;
}

int CPgServer::Receive(char *outpkString)
{
	return 0;
}

void CPgServer::Pulse()
{
}

void CPgServer::SendBreakData(PgLuaDebugPacket::ELuaDebugPacketType eType)
{
	CWinApp *p = AfxGetApp();
	CMainFrame* pkMainWnd = (CMainFrame*)p->GetMainWnd();	
	CProject* pProject = pkMainWnd->GetProject();
	if ( pProject->PositionBreakPoints() )
		AfxMessageBox("하나 이상의 브레이크 포인트가 사용 가능한 곳에 지정되어 있지 않습니다. 그 포인터는 다른 줄로 옮겨집니다.", MB_OK);

	//int iFileCount = pProject->NofFiles();

	LuaDbgPacket kPacket;

	CPgTree kFiles = pProject->GetFiles();

	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = kFiles.m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		CProjectFile* pPF = pkChild->pkProjectFile;
	//}

	//for (int i=0 ; i<iFileCount ; i++)
	//{
		//CProjectFile* pPF = pProject->GetFile(i);
		int iBreakCount = pPF->GetBreakPointCount();
		if (!iBreakCount)
			continue;

		CString strFilename = pPF->GetPathName();
		CString strSendFilepath = 
			ConvertAbsolutepathToRelativepath(pProject->GetExecuteDir(), strFilename);

		if (!strSendFilepath.Find(".LUA"))
		{
			strSendFilepath += pPF->GetNameExt();
		}
		
		// 패킷 저장.
		PgDebugInfo kDebugInfo;
		strcpy_s(kDebugInfo.acFilename, strSendFilepath);
		kDebugInfo.ePacketType = eType;
		kDebugInfo.iBreakLineCount = iBreakCount;
		for (int j=0 ; j<iBreakCount ; j++)
		{
			int iLine = pPF->GetBreakPointToIdx(j);
			kDebugInfo.aiBreakLines[j] = iLine;
		}

		kPacket.insert(std::make_pair(kDebugInfo.acFilename, kDebugInfo));
	}

	// 브레이크 데이터 보내주기.
	int iCount = 0;
 	for(LuaDbgPacket::iterator itr = kPacket.begin();
		itr != kPacket.end();
		++itr)
	{
		PgDebugInfo kDebugInfo = itr->second;

		// 새로고침 데이터는 마지막 End가 필요하다.
		if (iCount == kPacket.size()-1 &&
			kDebugInfo.ePacketType == PgLuaDebugPacket::PacketType_Debug_LineOnlyRefresh)
		{
			kDebugInfo.ePacketType = PgLuaDebugPacket::PacketType_Debug_LineOnlyRefreshEnd;
		}
		Send(sizeof(PgDebugInfo), ((char *)&kDebugInfo));

		iCount += 1;
	}
}

//---------------------------------------------------------------------------
// Start에서 End 경로까지 상대경로 만들어서 리턴.
// c:/windows/system/command   ->   c:/windows/font/
// ../../font/

CString CPgServer::ConvertAbsolutepathToRelativepath(
	CString kStartPath, CString kEndPath, CString kSlash)
{
	char acTemp[256];
	strcpy_s(acTemp, kStartPath);
	kStartPath = acTemp;
	strcpy_s(acTemp, kEndPath);
	kEndPath = acTemp;

	if (kSlash == "\\")
	{
		kStartPath.Replace("/", "\\");
		kEndPath.Replace("/", "\\");
	}
	else if (kSlash == "/")
	{
		kStartPath.Replace("\\", "/");
		kEndPath.Replace("\\", "/");
	}
	else
		return "0";

	kStartPath.MakeUpper();
	kEndPath.MakeUpper();

	vector<CString> arStartPath;
	vector<CString> arEndPath;

 	CString kPath = kStartPath;
	int iFirstIdx = 0;
	int iSecondIdx = 0;
	while (kPath.Find(kSlash) > 0)
	{
		CString kString = kPath;

		int iSecondIdx = kPath.FindOneOf(kSlash);
		kString.GetBufferSetLength(iSecondIdx);
		arStartPath.push_back(kString);

		kPath = kPath.Right(kPath.GetLength() - (kString.GetLength()+1));
	}
	if (kPath.GetLength() > 0)
		arStartPath.push_back(kPath);

 	kPath = kEndPath;
	while (kPath.Find(kSlash) > 0)
	{
		CString kString = kPath;

		int iSecondIdx = kPath.FindOneOf(kSlash);
		kString.GetBufferSetLength(iSecondIdx);
		arEndPath.push_back(kString);

		kPath = kPath.Right(kPath.GetLength() - (kString.GetLength()+1));
	}
	if (kPath.GetLength() > 0)
		arEndPath.push_back(kPath);


	int iSameCount = 0;
	int iMoveupCount = 0;
	while( iSameCount < (int)arStartPath.size() && iSameCount < (int)arEndPath.size() &&
		arStartPath[iSameCount] == arEndPath[iSameCount] )
		iSameCount += 1;
	iMoveupCount = (int)arStartPath.size() - iSameCount;

	
	CString strResult;
	// 두번째 경로가 첫번째 경로의 처음부터 끝까지를 가지고 있을때.
	if (iMoveupCount == 0)
	{
		strResult = "";
		CString strTemp2 = kSlash;
		for (int i=iSameCount ; i<(int)arEndPath.size() ; i++)
		{
			if ( i!=iSameCount )
				strResult += strTemp2;
			strResult += arEndPath[i];
			//strResult = CString::Concat(strResult, arEndPath->get_Item(i), strTemp2);
		}
	}
	// 완전 새경로 (C: 와 D: 정도로 서로 중복되는 디렉토리가 없을) 일때.
	else if (iMoveupCount == (int)arStartPath.size())
	{
		strResult = kEndPath;
	}
	// 조금 중복이 있을때
	else
	{
		strResult = "";
		CString strTemp = ".." + kSlash;
		CString strTemp2 = kSlash;
		for (int i=0 ; i<iMoveupCount ; i++)
		{
			strResult += strTemp;
		}
		for (int i=iSameCount ; i<(int)arEndPath.size() ; i++)
		{
			strResult += arEndPath[i];

			if (i != (int)arEndPath.size()-1)
				strResult += strTemp2;
		}
	}

	return strResult;
}

void CPgServer::PacketProcess(char *pkString)
{
	PgLuaDebugPacket *pkPacket = (PgLuaDebugPacket *)pkString;

	// Debug
	if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_LineOnly)
	{
	}
	// Packet Quit
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Quit)
	{
		CDebugger::GetDebugger()->Stop();
	}
	// Local Variable
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_LocalData)
	{
		HWND hWnd = CDebugger::GetDebugger()->GetMainWnd();
		SetForegroundWindow(hWnd);

		PacketProcess_DebugLocal(pkString);
	}
	// Eval
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_EvalData)
	{
		PacketProcess_EvalData(pkString);
	}
	// Eval Clear
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_EvalDataClear)
	{
		CDebugger::GetDebugger()->ClearDebugEvalData();
	}
	// Stack
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_StackData)
	{
		PacketProcess_StackData(pkString);
	}
	// Calltip
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_Calltip)
	{
		PacketProcess_Calltip(pkString);
	}
}

void CPgServer::PacketProcess_DebugLocal(char *pkString)
{
	CDebugger::GetDebugger()->DrawDebugData(pkString);

	return;
}

void CPgServer::PacketProcess_EvalData(char *pkString)
{
	PgDebugEvalInfo *pkRecvEvalInfo = (PgDebugEvalInfo *)pkString;

	CDebugger::GetDebugger()->AddWatchList(
		pkRecvEvalInfo->acVariableName,
		pkRecvEvalInfo->acVariableType,
		pkRecvEvalInfo->acVariableValue
		);

	if (pkRecvEvalInfo->bIsLastEvalInfo)
	{
		CDebugger::GetDebugger()->AddWatchListEnd();
	}
}

void CPgServer::PacketProcess_StackData(char *pkString)
{
	PgDebugStackInfo *pkRecvStackInfo = (PgDebugStackInfo *)pkString;
	CDebugger::GetDebugger()->AddStackList(pkRecvStackInfo);
}

void CPgServer::PacketProcess_Calltip(char *pkString)
{
	PgDebugCalltipInfo *pkCalltipInfo = (PgDebugCalltipInfo *)pkString;
	CDebugger::GetDebugger()->SetCalltip(pkCalltipInfo);
}
