#include "StdAfx.h"
#include <winsock2.h>
extern "C" 
{
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}
#include "LuaDebuggerLIB.h"

CPgLuaDebugger::CPgLuaDebugger(void)
{
	m_bIsToExit = false;
	m_eDebugType = DebugType_Go;
	m_bIsConnect = false;
	m_iStepOverCurrLine = 0;
	m_bIsRefreshBreakData = false;
	m_hWnd = 0;
}

CPgLuaDebugger::~CPgLuaDebugger(void)
{
}

bool CPgLuaDebugger::Initialize(lua_State *L, int iPort, const char* szIP, HWND hWnd, FuncDoFile pkFuncDoFile)
{
	if (!szIP || !strlen(szIP))
		return false;

	m_pkFuncDofile = pkFuncDoFile;

	m_iPort = iPort;
	m_strIP = szIP;
	m_hWnd = hWnd;

	m_bIsConnect = true;

	LinkLuaHook(L);
	Connect();

	return true;
}

void CPgLuaDebugger::Terminate()
{
	Disconnect();
	m_bIsToExit = true;
}

unsigned long __stdcall Thread( void *arg )
{
	if (!arg)
	{
		return 0;
	}

	CPgLuaDebugger *pkScriptDebug = (CPgLuaDebugger *)arg;

	WSANETWORKEVENTS event;
	WSAEVENT hRecvEvent = WSACreateEvent();
	int iRet = WSAEventSelect( pkScriptDebug->Socket , hRecvEvent, FD_READ | FD_CLOSE );

    while( pkScriptDebug &&
		!pkScriptDebug->m_bIsToExit )
    {
		Sleep( 1 );

		if (!pkScriptDebug)
			break;
	 
		WSAEnumNetworkEvents( pkScriptDebug->Socket , hRecvEvent, &event);
	 
		if((event.lNetworkEvents & FD_READ) == FD_READ)
		{
			int iLen=0;
			char kLen[4];
			int rettmp = recv(pkScriptDebug->Socket, (char *)&kLen, 4, 0 ); //데이터를 읽어들임.

			if (rettmp > 0)
			{
				char *pkLen = new char[4];
				memset(pkLen, 0, 4);
				char *pkLenTemp = new char[4];
				memset(pkLenTemp, 0, 4);

				memcpy(&(pkLen[0]), kLen, rettmp);
				memset(pkLenTemp, 0, 4);
				int iRecvsize = rettmp;

				// 덜 받았으면 받을때까지.
				while(iRecvsize < 4)
				{
					//(char *)&iLen
					int rettmp = recv(pkScriptDebug->Socket, pkLenTemp, 4-iRecvsize, 0 ); //데이터를 읽어들임.
					
					if (rettmp > 0)
					{
						memcpy(&(pkLen[iRecvsize]), pkLenTemp, rettmp);
						memset(pkLenTemp, 0, 4);
						iRecvsize += rettmp;
					}
				}
				int *piLen = (int *)pkLen;
				iLen = *piLen;
				delete[] pkLen;
				delete[] pkLenTemp;

				// 진짜 데이터 받는 부분.
				int iGetSize = 0;
				char *pcBuffer = new char[iLen];
				char *pcBufferTemp = new char[iLen];
				while(iGetSize < iLen)
				{
					int rettmp = recv(pkScriptDebug->Socket, pcBufferTemp, iLen-iGetSize, 0);

					if (rettmp > 0)
					{
						memcpy(&(pcBuffer[iGetSize]), pcBufferTemp, rettmp);
						memset(pcBufferTemp, 0, iLen);
						iGetSize += rettmp;
					}
				}

				// Buffer 가 데이터
				// 풀어 헤치는 함수.
				pkScriptDebug->PacketProcess(pcBuffer);

				delete[] pcBufferTemp;
				delete[] pcBuffer;
			}
		}
	 
		if((event.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
		{
			PgLuaDebugPacket kPacket;
			closesocket( pkScriptDebug->Socket  ); //해당 소켓이 닫혔음.
			pkScriptDebug->m_eDebugType = CPgLuaDebugger::DebugType_Go;
			pkScriptDebug->m_bIsToExit = true;
			break;
		}
    }

	WSACloseEvent( hRecvEvent );
    return 1;

}

bool CPgLuaDebugger::Connect()
{
	if (!m_bIsConnect)
		return false;

	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
	{
		WSACleanup();
		exit( 0 );
	}

	Socket = socket(AF_INET, SOCK_STREAM,0);

	if( Socket == INVALID_SOCKET )
	{
		WSACleanup();
		exit( 0 );
	}

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = inet_addr( m_strIP.c_str() );
	ServerAddress.sin_port = htons( m_iPort );

	ReturnVal = connect( Socket, (struct sockaddr*)&ServerAddress,
			sizeof(ServerAddress) );
	if( ReturnVal )
	{
		closesocket( Socket );
		WSACleanup();
		return false;
	}
	else
	{
		unsigned long TempValL;
		CreateThread( NULL, 0, Thread, this, 0, &TempValL );

		// Connected
		m_eDebugType = DebugType_Go;
	}

	return true;
}

bool CPgLuaDebugger::Disconnect()
{
	if (!m_bIsConnect)
		return true;

	closesocket( Socket ); //소켓을 닫습니다.
	WSACleanup();

	return true;
}

int CPgLuaDebugger::Send(int iLen, char *pkString)
{
	char *pkLen;
	pkLen = (char *)&iLen;

	int iSendsize=0;
	while(iSendsize < 4)
	{
		int ActualSendSize = send( Socket,
			pkLen+iSendsize,
			4-iSendsize,
			0);

		if (ActualSendSize > 0)
			iSendsize += ActualSendSize;

		if (m_bIsToExit)
			return 0;
	}

	iSendsize=0;
	while(iSendsize < iLen)
	{
		int ActualSendSize =
			send( Socket,
			pkString+iSendsize,
			iLen-iSendsize,
			0);

		if (ActualSendSize > 0)
			iSendsize += ActualSendSize;

		if (m_bIsToExit)
			return 0;
	}
	
	return iSendsize;
}

void CPgLuaDebugger::PacketProcess(char *pkString)
{
	PgLuaDebugPacket *pkPacket = (PgLuaDebugPacket *)pkString;

	if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_LineOnly)
	{
		PacketProcess_DebugLineOnly(pkString);
	}
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_LineClear)
	{
		m_bIsRefreshBreakData = true;
		m_kLineData.clear();
	}	
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_LineOnlyRefresh)
	{
		PacketProcess_DebugLineOnlyRefresh(pkString);
	}
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_LineOnlyRefreshEnd)
	{
		PacketProcess_DebugLineOnlyRefresh(pkString);
		m_bIsRefreshBreakData = false;
	}
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Command_All)
	{
		PacketProcess_CommandAll(pkString);
	}
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_EvalData)
	{
		Eval(pkString);
	}
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_Calltip)
	{
		Calltip(pkString);
	}
	else if (pkPacket->ePacketType ==
		PgLuaDebugPacket::PacketType_Debug_Dofile)
	{
		AddDofile(pkString);
	}
}

void CPgLuaDebugger::PacketProcess_DebugLineOnly(char *pkString)
{
	PgDebugInfo kDebugInfo;
	memcpy(&kDebugInfo, pkString, sizeof(PgDebugInfo));
	m_kLineData.insert(std::make_pair(kDebugInfo.acFilename, kDebugInfo));
}

void CPgLuaDebugger::PacketProcess_DebugLineOnlyRefresh(char *pkString)
{
	PgDebugInfo kDebugInfo;
	memcpy(&kDebugInfo, pkString, sizeof(PgDebugInfo));
	m_kLineData.insert(std::make_pair(kDebugInfo.acFilename, kDebugInfo));
}

void CPgLuaDebugger::PacketProcess_CommandAll(char *pkString)
{
	PgCommandInfo *pkCmdInfo = (PgCommandInfo *)pkString;
	bool bIsWindowActive = false;
	
	if (pkCmdInfo->eCommandType == PgCommandInfo::CommandInfoType_Go)
	{
		m_eDebugType = DebugType_Go;
		bIsWindowActive = true;
	}
	else if (pkCmdInfo->eCommandType == PgCommandInfo::CommandInfoType_StepOver)
	{
		m_eDebugType = DebugType_StepOver;
		bIsWindowActive = true;
	}
	else if (pkCmdInfo->eCommandType == PgCommandInfo::CommandInfoType_StepInto)
	{
		m_eDebugType = DebugType_StepInto;
		bIsWindowActive = true;
	}
	else if (pkCmdInfo->eCommandType == PgCommandInfo::CommandInfoType_StepOut)
	{
		m_eDebugType = DebugType_StepOut;
		bIsWindowActive = true;
	}

	if (bIsWindowActive && m_hWnd)
	{
		SetForegroundWindow(m_hWnd);
	}
}

////////////////////////////////////////////////////////////////////////////
// Dofile
void CPgLuaDebugger::AddDofile(char *pkString)
{
	if (!m_pkFuncDofile)
		return;

	PgDebugDofileInfo *pkDofileInfo = (PgDebugDofileInfo *)pkString;

	m_kDofileList.push_back(*pkDofileInfo);
}

void CPgLuaDebugger::Dofile()
{
	if (!m_pkFuncDofile)
		return;

	bool bIsDofile = false;
	for(LuaDofileList::iterator itr = m_kDofileList.begin();
		itr != m_kDofileList.end();
		++itr)
	{
		PgDebugDofileInfo kDofileInfo = *itr;
		std::string strFilename = kDofileInfo.acFilename;
		strFilename.erase(0, 10);

		m_pkFuncDofile(strFilename.c_str());
		bIsDofile = true;
	}

	if (bIsDofile)
		m_kDofileList.clear();
}

////////////////////////////////////////////////////////////////////////////
// Describe() : Lua의 변수에 대한 내용을 인자로 Out한다.
void CPgLuaDebugger::Describe(char *szType, char *szValue, int nIndex)
{
	int ntype = lua_type(m_pkL, nIndex);
	const char* type = lua_typename(m_pkL, ntype);
	char value[64];

	switch(ntype)
	{
	case LUA_TNUMBER:
		sprintf_s(value, "%f", lua_tonumber(m_pkL, nIndex));
		break;
	case LUA_TSTRING:
		sprintf_s(value, "%.63s", lua_tostring(m_pkL, nIndex));
		break;
	case LUA_TBOOLEAN:
		sprintf_s(value, "%s", lua_toboolean(m_pkL, nIndex) ? "true" : "false");
		break;
	case LUA_TUSERDATA:
		sprintf_s(value, "%p", lua_touserdata(m_pkL, nIndex));
		break;
		
	default:
		value[0] = '\0';
		break;
	}
	
	sprintf_s(szType, sizeof(char)*64, "%.64s", type);
	sprintf_s(szValue, sizeof(char)*64, "%.64s", value);
}

////////////////////////////////////////////////////////////////////////////
// Eval() : 필요한 데이터를 얻고 전송한다.
BOOL CPgLuaDebugger::Eval(char *pkString)
{
	char szName[64];
	char szType[64];
	char szValue[64];
	char szCode[64];

	PgDebugEvalInfo *pkRecvEvalInfo = (PgDebugEvalInfo *)pkString;
	strcpy_s(szName, pkRecvEvalInfo->acVariableName);

	strcpy_s(szCode, "return ");
	strcat_s(szCode, szName);

	int top = lua_gettop(m_pkL);	
	int status = luaL_loadbuffer(m_pkL, szCode, strlen(szCode), szCode);
	if ( status )
		sprintf_s(szValue, "%s", luaL_checkstring(m_pkL, -1));
	else
	{
		status = lua_pcall(m_pkL, 0, LUA_MULTRET, 0);  /* call main */
		if ( status )
		{
			const char* szErr = luaL_checkstring(m_pkL, -1);
			const char* szErr2 = strstr(szErr, ": ");
			sprintf_s(szValue, "%s", szErr2?(szErr2+2):szErr);
		}
		else
			Describe(szType, szValue, -1);
	}

	lua_settop(m_pkL, top);

	// Send
	PgDebugEvalInfo kToSendEvalInfo;
	kToSendEvalInfo.ePacketType = PgLuaDebugPacket::PacketType_Debug_EvalData;
	strcpy_s(kToSendEvalInfo.acVariableName, szName);
	strcpy_s(kToSendEvalInfo.acVariableType, szType);
	strcpy_s(kToSendEvalInfo.acVariableValue, szValue);
	kToSendEvalInfo.bIsLastEvalInfo = pkRecvEvalInfo->bIsLastEvalInfo;
	Send(sizeof(PgDebugEvalInfo), ((char *)&kToSendEvalInfo));

	return !status;
}


//////////////////////////////////////////////////////////////////
// DEBUG : 
CPgLuaDebugger *g_pkLuaScriptDebug;
void HookFunction(lua_State *L, lua_Debug *ar)
{
	lua_getinfo(L, "nSlu", ar);

	static int bStepIn = 0;

	if (!g_pkLuaScriptDebug)
		return;

	g_pkLuaScriptDebug->m_pkAr = ar;
	switch(ar->event)
	{
	case LUA_HOOKLINE:
		g_pkLuaScriptDebug->Line_Hook(L, ar);
		break;
	}
}

void CPgLuaDebugger::LinkLuaHook(lua_State *L)
{
	if (!m_bIsConnect)
		return;

	g_pkLuaScriptDebug = this;

	m_pkL = L;
	lua_sethook(L, HookFunction, LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);
}

void CPgLuaDebugger::Line_Hook(lua_State *L, lua_Debug *ar)
{
	// 브레이크 포인터 새로고침중.
	while(m_bIsRefreshBreakData)
	{
		Sleep(1);
	}

	// file
	int iCount = 0;
	for(LuaDbgPacket::iterator itr = m_kLineData.begin();
		itr != m_kLineData.end() && !m_bIsRefreshBreakData;
		++itr)
	{
		iCount += 1;
		PgDebugInfo kDebugInfo = itr->second;

		// lines
		for (int iLineIdx=0 ;
			iLineIdx<kDebugInfo.iBreakLineCount ;
			++iLineIdx)
		{
			int iLine = kDebugInfo.aiBreakLines[iLineIdx];
			
			bool bDebug = false;
			if (m_eDebugType == DebugType_StepOut &&
				m_iI_ci > ar->i_ci)
			{
				bDebug = true;
			}
			else if (m_eDebugType == DebugType_StepInto)
			{
				bDebug = true;
			}
			else if (m_eDebugType == DebugType_StepOver &&
				m_iI_ci >= ar->i_ci)
			{
				bDebug = true;
			}
			
			if	(ar->source[0] == '@' &&
				_stricmp(&(ar->source[1]), kDebugInfo.acFilename) == 0 &&
				iLine == ar->currentline )
			{
				bDebug = true;
			}


			if (bDebug)
			{
				// 1.콜스텍, 변수들 전송
				SendDeleteEvalData();
				SendLuaDebugLocalData(L, ar);
				SendLuaDebugStackData(L);
				// 2.무한루프. 
				m_eDebugType = DebugType_Break;

				DebugLoop();

				if (m_eDebugType == DebugType_StepOver ||
					m_eDebugType == DebugType_StepOut)
				{
					strcpy_s(m_acStepOverCurr,
						sizeof(m_acStepOverCurr),
						&(ar->source[1]));
					m_iStepOverCurrLine = ar->currentline;
					m_iStepOverLineDef = ar->linedefined;
					m_iStepOverLastLineDef = ar->lastlinedefined;
					m_iI_ci = ar->i_ci;
				}

				iLineIdx = 0;
				return;
			}
		}

		if (m_bIsRefreshBreakData)
		{
			return ;
		}
	}

	if (m_bIsRefreshBreakData)
	{
		return ;
	}
}

void CPgLuaDebugger::DebugLoop()
{
	m_kLineData.clear();
	while(m_eDebugType == DebugType_Break)
	{
		Sleep(1);
	}
}

void CPgLuaDebugger::SendDeleteEvalData()
{
	// Clear Send
	PgLuaDebugPacket kPacket;
	kPacket.ePacketType = PgLuaDebugPacket::PacketType_Debug_EvalDataClear;
	Send(sizeof(PgLuaDebugPacket), ((char *)&kPacket));
}

void CPgLuaDebugger::SendLuaDebugStackData(lua_State *L)
{
	int nLevel = 0;
	lua_Debug ar;
	char szDesc[256];
	
	PgDebugStackInfo kStackInfo;
	kStackInfo.ePacketType = PgDebugInfo::PacketType_Debug_StackData;

	while ( lua_getstack (L, nLevel, &ar) )
	{
		lua_getinfo(L, "lnuS", &ar);
		if ( ar.source[0]=='@' )
		{
			szDesc[0] = '\0';
			if ( ar.name )
				strcpy_s(kStackInfo.acName[nLevel], ar.name);
			if ( ar.namewhat )
				strcpy_s(kStackInfo.acNamewhat[nLevel], ar.namewhat);
			if ( ar.what )
				strcpy_s(kStackInfo.acWhat[nLevel], ar.what);
			if ( ar.short_src )
				strcpy_s(kStackInfo.acShort_src[nLevel], ar.short_src);

			strcpy_s(kStackInfo.acSource[nLevel], ar.source);
			kStackInfo.aiCurrentline[nLevel] = ar.currentline;
		}

		++nLevel;
		
		if (nLevel >= 64)
			break;
	};

	kStackInfo.iLevel = nLevel;
	Send(sizeof(PgDebugStackInfo), ((char *)&kStackInfo));
}

void CPgLuaDebugger::SendLuaDebugLocalData(lua_State *L, lua_Debug *ar)
{
	PgDebugInfo kDebugInfo;
	if (ar->source[0] == '@')
		strcpy_s(kDebugInfo.acFilename, &(ar->source[1]));
	else
		strcpy_s(kDebugInfo.acFilename, &(ar->source[0]));
	kDebugInfo.ePacketType = PgDebugInfo::PacketType_Debug_LocalData;

	int iIdx = 0;
	kDebugInfo.iLocalVariableCount = 0;
	int i = 1;
	const char *name;
    while ((name = lua_getlocal(L, ar, i++)) != NULL)
	{
		strcpy_s(kDebugInfo.acLocalVariableName[iIdx], name);

		int ntype = lua_type(L, -1);
		strcpy_s(kDebugInfo.acLocalVariableType[iIdx], lua_typename(L, ntype));
		
		switch(ntype)
		{
		case LUA_TNUMBER:
			sprintf_s(kDebugInfo.acLocalVariableValue[iIdx],
				"%f", lua_tonumber(L, -1));
			break;
		case LUA_TSTRING:
			sprintf_s(kDebugInfo.acLocalVariableValue[iIdx],
				"%.63s", lua_tostring(L, -1));
			break;
		case LUA_TBOOLEAN:
			sprintf_s(kDebugInfo.acLocalVariableValue[iIdx],
				"%s", lua_toboolean(L, -1) ? "true" : "false");
			break;
		case LUA_TUSERDATA:
			sprintf_s(kDebugInfo.acLocalVariableValue[iIdx],
				"%p", lua_touserdata(L, -1));
			break;
		default:
			kDebugInfo.acLocalVariableValue[iIdx][0] = '\0';
			break;
		}

		kDebugInfo.iLocalVariableCount += 1;
		iIdx += 1;

		lua_pop(L, 1);  /* remove variable value */
    }

	kDebugInfo.iBreakLineCount = 1;
	kDebugInfo.aiBreakLines[0] = ar->currentline;
	Send(sizeof(PgDebugInfo), ((char *)&kDebugInfo));
}

BOOL CPgLuaDebugger::Calltip(char *pkString)
{
	PgDebugCalltipInfo kInfo = *((PgDebugCalltipInfo *)pkString);
	char *szWord = kInfo.acWord;
	int nLevel = kInfo.iLevel;
	
	lua_Debug ar = *m_pkAr;
	int i = 1;
    const char *name;
    while ((name = lua_getlocal(m_pkL, &ar, i++)) != NULL) {
		if ( strcmp(name, szWord)==0 )
		{
			char szType[64];
			char szValue[64];
			Describe(szType, szValue, -1);
			strcpy_s(kInfo.acName, name);
			strcpy_s(kInfo.acType, szType);
			strcpy_s(kInfo.acValue, szValue);
			kInfo.bIsLocal = true;
			kInfo.bIsFound = true;

			lua_pop(m_pkL, 1);  /* remove variable value */

			Send(sizeof(PgDebugCalltipInfo), ((char *)&kInfo));

			return TRUE;
		}

		lua_pop(m_pkL, 1);  /* remove variable value */
    }

	lua_pushvalue(m_pkL, LUA_GLOBALSINDEX);

	lua_pushnil(m_pkL);  /* first key */
	while (lua_next(m_pkL, -2))
	{
		const char* name = lua_tostring(m_pkL, -2);
		if ( strcmp(name, szWord)==0 )
		{
			char szType[64];
			char szValue[64];
			Describe(szType, szValue, -1);
			strcpy_s(kInfo.acName, name);
			strcpy_s(kInfo.acType, szType);
			strcpy_s(kInfo.acValue, szValue);
			kInfo.bIsLocal = false;
			kInfo.bIsFound = true;

			lua_pop(m_pkL, 3);  /* remove table, key, value */

			Send(sizeof(PgDebugCalltipInfo), ((char *)&kInfo));

			return TRUE;
		}
		
		lua_pop(m_pkL, 1); // pop value, keep key for next iteration;
	}
	lua_pop(m_pkL, 1); // pop table of globals;

	kInfo.bIsFound = false;
	Send(sizeof(PgDebugCalltipInfo), ((char *)&kInfo));

	return FALSE;
}