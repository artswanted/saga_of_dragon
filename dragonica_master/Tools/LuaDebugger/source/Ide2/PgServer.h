#pragma once

#include "Winsock2.h"

class CPgServer :
	public CSingleton<CPgServer>
{
public:
	WSADATA        wsaData;
	SOCKET         ServerSocket;   //소켓을 선언합니다.
	SOCKADDR_IN    ServerAddress;  //소켓의 주소 정보를 넣는 구조체입니다.
	unsigned short ServerPort;

	SOCKADDR_IN   ClientAddress; //접속한 클라이언트의 주소정보를 담을 구조체
	SOCKET        ClientSocket;  //클라이언트 소켓.
	int AddressSize;			 //주소 크기를 저장하는

	HANDLE m_hThread;
	bool m_bIsTermThread;

public:
	CPgServer(void);
	virtual ~CPgServer(void);

public:
	bool Initialize();
	bool Terminate();
	void Connect();
	void MakeThread();
	void TermThread();
	void SendMessageDisConnect();
	void DisConnect();
	int Send(int iLen, char *pkString);
	int Receive(char *outpkString);
	void Pulse();
	// Data Processing
	CString ConvertAbsolutepathToRelativepath(CString kStartPath, CString kEndPath, CString kAbleSlash = "/");
	void SendBreakData(PgLuaDebugPacket::ELuaDebugPacketType eType = PgDebugInfo::PacketType_Debug_LineOnly);
	// Debug
	void PacketProcess(char *pkString);
	void PacketProcess_DebugLocal(char *pkString);
	void PacketProcess_EvalData(char *pkString);
	void PacketProcess_StackData(char *pkString);
	void PacketProcess_Calltip(char *pkString);
};

#define g_pkPgServer	CPgServer::GetSingleton()