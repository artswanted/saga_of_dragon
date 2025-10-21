#pragma once

#include "LuaDev/LuaDebugPacketInfo.h"

class CPgLuaDebugger
{
public:
	typedef enum eDebugType
	{
		DebugType_Go,
		DebugType_Break,
		DebugType_StepOver,
		DebugType_StepInto,
		DebugType_StepOut,
	}EDebugType;

	EDebugType m_eDebugType;
	char m_acStepOverCurr[256];
	int m_iStepOverCurrLine;
	int m_iStepOverLineDef;
	int m_iStepOverLastLineDef;
	int m_iI_ci;

public:
	bool m_bIsConnect;
	bool m_bIsRefreshBreakData;
	std::string m_strIP;

	SOCKET         Socket;        //소켓을 선언합니다.
	WSADATA        wsaData;
	SOCKADDR_IN    ServerAddress; //소켓의 주소 정보를 넣는 구조체입
	int m_iPort;
	int            ReturnVal;
	bool			m_bIsToExit;
	HWND m_hWnd;

	// LuaState
	lua_State *m_pkL;
	lua_Debug *m_pkAr;

	// Lines
	LuaDbgPacket m_kLineData;

	// Dofile list
	LuaDofileList m_kDofileList;
	typedef void (*FuncDoFile)(const char *pkFilename);
	FuncDoFile m_pkFuncDofile;

public:
	CPgLuaDebugger(void);
	virtual ~CPgLuaDebugger(void);

public:
	// Base
	bool Initialize(lua_State *L, int iPort, const char* szIP,
		HWND hWnd = 0, void (*pkFuncDofile)(const char *pkFilename) = 0);
	void Terminate();
	
	//
	// Client - Server
	bool Connect();
	bool Disconnect();
	int Send(int iLen, char *pkString);
	void PacketProcess(char *pkString);
	void PacketProcess_DebugLineOnly(char *pkString);
	void PacketProcess_DebugLineOnlyRefresh(char *pkString);
	void PacketProcess_CommandAll(char *pkString);

	//
	// Debug
	void LinkLuaHook(lua_State *L);
	void Line_Hook(lua_State *L, lua_Debug *ar);
	void SendDeleteEvalData();
	void SendLuaDebugStackData(lua_State *L);
	void SendLuaDebugLocalData(lua_State *L, lua_Debug *ar);
	void Describe(char *szType, char *szValue, int nIndex);
	BOOL Eval(char *pkString);
	BOOL Calltip(char *pkString);
	void DebugLoop();

	//
	// Dofile
	void AddDofile(char *pkString);
	void Dofile();
};
