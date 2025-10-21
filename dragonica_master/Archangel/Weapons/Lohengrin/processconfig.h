#ifndef WEAPON_LOHENGRIN_CONFIG_PROCESSCONFIG_H
#define WEAPON_LOHENGRIN_CONFIG_PROCESSCONFIG_H

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <list>
#include <map>
#include <vector>
#include <windows.h>
#include <tchar.h>

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "Loki/singleton.h"
#include "ServerType.h"
#include "PacketStruct.h"
#include "PacketStructLog.h"

typedef std::vector< CEL::ADDR_INFO >				CONT_ADDR_INFO;

#define ALL_CHANNEL_NUM		-1
#define PUBLIC_CHANNEL_NUM	0

typedef enum
{
	// Terminate type
	E_Terminate_Bitmask = 0x00FF,
	E_Terminate_By_Console = 0x0001,
	E_Terminate_By_MControl = 0x0002,

	// Service Exception type
	E_Exception_Bitmask = 0xFF00,
	E_Exception_Dis_Contents = 0x0100,
} E_ExitCode;


class CProcessConfig//센터서버는 ProcessConfig를 사용하지 않습니다.
{
	friend struct ::Loki::CreateStatic< CProcessConfig >;
	
	typedef struct tagConnectionObj
	{//이 이상 정보를 넣지 말 것.
		tagConnectionObj()
		{
		}
		BM::GUID kConnectObj;
		CEL::SESSION_KEY kSessionKey;
	}SConnectionObj;

	typedef struct tagSSession_Base_Info
	{
		tagSSession_Base_Info(SERVER_IDENTITY const& rkSI, CEL::CSession_Base* const pkSessionBase)
			: kSI(rkSI), pkSession(pkSessionBase)
		{}

		SERVER_IDENTITY const kSI;
		CEL::CSession_Base* const pkSession;
	} SSession_Base_Info;

public:
	typedef std::map< CEL::ADDR_INFO, SConnectionObj >	CONT_CONNECTION;
	//typedef std::list< CEL::CSession_Base * const >	CONT_BASE_SESSION;
	typedef std::list< SSession_Base_Info >	CONT_BASE_SESSION;

private:
	CProcessConfig();
	virtual ~CProcessConfig();

public:
	bool Locked_ParseArg(int const argc, TCHAR* argv[]);//파라메터 파싱.
	void Locked_SetConfigDirectory();
	bool Locked_SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)const;//연결된 세션으로 바로 보냄.
	bool Locked_SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, bool const bRandOne = false )const;//연결된 서버타입으로 바로 보냄.
	bool Locked_SendToChannelServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, short const nChannel, bool bPublic=false )const;//연결된 서버타입으로 바로 보냄(채널 한정)
	bool Locked_SendToContentsServer(short const nRealm, BM::Stream const &rkPacket)const;
	void Locked_SetServerType(CEL::E_SESSION_TYPE const eInType);//서비스중에는 서버 타입 바뀌지 않게해라

	bool Locked_ConnectImmigration();
	bool Locked_ConnectContents();
	bool Locked_ConnectCenter();
	bool Locked_ConnectPublicCenter();
	bool Locked_ConnectSwitch();
	bool Locked_ConnectLogin();
	bool Locked_ConnectLog();
//	bool Locked_ConnectAllServers();

	void ImmigrationConnector(BM::GUID const& rkGuid) { m_kImmigrationSession.kSessionKey.Set(rkGuid, BM::GUID::NullData()); }
	void ContentsConnector(BM::GUID const& rkGuid) { m_kContentsSession.kSessionKey.Set(rkGuid, BM::GUID::NullData()); }
	void CenterConnector(BM::GUID const& rkGuid)
	{
		m_kCenterSession.kSessionKey.Set(rkGuid, BM::GUID::NullData());
		m_kPublicCenterSession.kSessionKey.Set(rkGuid, BM::GUID::NullData());
	}
	BM::GUID const& ImmigrationConnector()const { return m_kImmigrationSession.kSessionKey.WorkerGuid(); }

	bool Locked_AddServerInfo(TBL_SERVERLIST const &rkInfo);

	HRESULT Locked_AddMapCfg(TBL_MAP_CONFIG const &rkKey);
	HRESULT Locked_AddMapCfg_Mission(TBL_MAP_CONFIG const &rkKey);

	HRESULT Locked_Read_ServerList(BM::Stream &rPacket);//서버 리스트 받기.
	bool Locked_Write_ServerList( BM::Stream &rPacket, short const nRealm = 0);//서버 리스트 쓰기.

	void Locked_DisplayState()const;
	HRESULT Locked_GetServerInfo(short const nServerType, CONT_SERVER_HASH &rkCont)const;//TBL_SERVERLIST 내부에 string이 있다는걸 명심하고 써라.
	void Locked_SetConsoleTitle() const;

	void Locked_GetVersion(int const iFileVersionSize, const LPTSTR lpszFileVersion, int const iProductVersionSize = 0, const LPTSTR lpszProductVersion = NULL)const;

	HRESULT Locked_OnDisconnectServer(CEL::CSession_Base * const pkSession, SERVER_IDENTITY *pkOutDisconnectSI=NULL);
	
	HRESULT Locked_OnGreetingServer(SERVER_IDENTITY const& rkSI, CEL::CSession_Base * const pkSession);//서버만. ->Greeting 용

	HRESULT Locked_GetServerInfo(SERVER_IDENTITY const &kSI, TBL_SERVERLIST &rkOut)const;

	HRESULT Locked_OnConnectServer(CEL::CSession_Base * const pkSession);
	
	HRESULT Locked_GetMapServerCfg(CONT_MAP_CONFIG* pkMapCfgStatic,CONT_MAP_CONFIG* pkMapCfgMission)const;
	HRESULT Locked_GetServerIdentity(short const sRealm, short const sChannel, short const nServerType, ContServerID &rkCont)const;
	//void Locked_OnConnectConsentServer(CEL::CSession_Base * const pkSession);
	bool Locked_SendToConsentServer(BM::Stream const &rkPacket) const;

	void Locked_SetLogServer();

	HRESULT Locked_SetPlayerCount(SERVER_IDENTITY const &kSI, size_t const iValue);
	void SetServerNo(short const nServerNo);//서비스중에는 서버 번호 바뀌지 않게해라
	static std::wstring PatchVersionS() { return ms_wstrPatchVersionS; }

protected://스스로 사용(컨텐츠 유지)
	void SetLogServer();
	void SendIdentity(CEL::CSession_Base *pSession, SERVER_IDENTITY const *pkForceSI = NULL);//! 내 정보를 날려주자.
	
	bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)const;//연결된 세션으로 바로 보냄.
	bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, bool const bRandOne=false )const;//연결된 서버타입으로 바로 보냄.
	bool SendToChannelServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, short const nChannel, bool const bPublic=false )const;//연결된 서버타입으로 바로 보냄(채널 한정)
	bool SendToContentsServer(short const nRealm, BM::Stream const &rkPacket)const;

	void _GetConsoleTitle(std::wstring &rkTitleOut, int const iServerType, short const nRealmNo, short const nChannelNo, short const nServerNo)const;//API 함수와 이름이 같다.

	HRESULT GetServerInfo(short const nServerType, CONT_SERVER_HASH &rkCont)const;//TBL_SERVERLIST 내부에 string이 있다는걸 명심하고 써라.
	HRESULT GetServerInfo(SERVER_IDENTITY const &kSI, TBL_SERVERLIST &rkOut)const;

	
	HRESULT GetSessionCont(CEL::E_SESSION_TYPE const eServerType, CONT_BASE_SESSION const *&pkOut)const;
	HRESULT GetSessionCont(CEL::E_SESSION_TYPE const eServerType, CONT_BASE_SESSION *&pkOut);

protected:
	BOOL GetFileVersionString(LPCTSTR lpszFile, int iFileVersionSize, LPTSTR lpszFileVersion, int iProductVersionSize = 0, LPTSTR lpszProductVersion = 0)const;
	
	bool CheckArg();
	int  ParseArg_Sub( std::wstring const &wstrText );

	//CLASS_DECLARATION_S(BM::GUID, ImmigrationConnector);
	//CLASS_DECLARATION_S(BM::GUID, ContentsConnector);
	//CLASS_DECLARATION_S(BM::GUID, CenterConnector);
	CLASS_DECLARATION_S(BM::GUID, LogConnector);
	CLASS_DECLARATION_S(BM::GUID, SwitchConnector);
	CLASS_DECLARATION_S(BM::GUID, LoginConnector);
//	CLASS_DECLARATION_S(BM::GUID, AllServerConnector);
	
public:	
	static HRESULT Recv_PT_A_ENCRYPT_KEY(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);//공통 패킷.
	static bool IsPublicChannel(short const nChannelNo){ return (PUBLIC_CHANNEL_NUM==nChannelNo);}
	static short GetPublicChannel() { return PUBLIC_CHANNEL_NUM; }
	static bool IsCorrectVersion( std::wstring const &wstrVersion, bool const bClient );

	//얘네는 락 안잡아도 됨.
	short SiteNo() const {return 1;}// 이녀석은 용민씨가 작업 할때까진 임시임 무조건 사이트 1번으로 고정
	short RealmNo() const {return m_kServerIdentity.nRealm;}
	short ServerNo() const {return m_kServerIdentity.nServerNo;}
	short ChannelNo()const{return m_kServerIdentity.nChannel;}
	bool IsPublicChannel()const{return IsPublicChannel(ChannelNo()); }// 공용 채널이냐.
	CEL::E_SESSION_TYPE ServerType()const{return (CEL::E_SESSION_TYPE)m_kServerIdentity.nServerType;}

protected:
	CONT_CONNECTION m_kContSwitchSession;//N개의 스위치의 커넥터. 어떤게 시도중인지를 몰라서.(굉장히 예외)
	CONT_CONNECTION m_kContLoginSession;//N개의 스위치의 커넥터. 어떤게 시도중인지를 몰라서.(굉장히 예외)
	CONT_CONNECTION m_kContLogSession;	//N개의 로그 커넥터
	CONT_CONNECTION::mapped_type m_kImmigrationSession;
	CONT_CONNECTION::mapped_type m_kContentsSession;
	CONT_CONNECTION::mapped_type m_kCenterSession;
	CONT_CONNECTION::mapped_type m_kPublicCenterSession;
//	CONT_CONNECTION m_kAllServerSession;//로그서버에서만 사용

	CLASS_DECLARATION_S(CEL::ADDR_INFO, ImmigrationServerAddr);//인자로 들어온 서버 주소
	CLASS_DECLARATION_S(CEL::ADDR_INFO, ContentsServerAddr);//인자로 들어온 서버 주소
	CLASS_DECLARATION_S(CEL::ADDR_INFO, CenterServerAddr);
	CLASS_DECLARATION_S(CEL::ADDR_INFO, PublicCenterServerAddr);// 공용센터 : Switch만 사용
	CLASS_DECLARATION_S(CEL::ADDR_INFO, MMCServerAddr);//인자로 들어온 서버 주소
	

	CLASS_DECLARATION_S(SERVER_IDENTITY, ServerIdentity);//스스로의 ServerIdentity
	CLASS_DECLARATION_S(int, LuaDebugPort);

	CLASS_DECLARATION_S(short int, RunMode);	// E_RunMode값
	CLASS_DECLARATION_S(WORD, ExitCode);	// E_ExitCode값

	CLASS_DECLARATION_S(std::wstring, ConfigDir);	//Config 디렉토리 값

protected:
	CONT_SERVER_HASH m_kServerHash;//스레드 세이프 때문에 CLASS_DECLARATION을 쓰지 않는다.
	//속도를 위한 컨테이너. m_kServerHash 와 동기화가 되어야한다.
	CONT_BASE_SESSION m_kSession_Switch;
	CONT_BASE_SESSION m_kSession_Map;
	CONT_BASE_SESSION m_kSession_Center;
	CONT_BASE_SESSION m_kSession_Consent;
	CONT_BASE_SESSION m_kSession_Immigration;	// ConsentServer 는 여러개의 Immigration 연결을 가진다.
	
	CONT_MAP_CONFIG m_kContMapCfg_Static;
	CONT_MAP_CONFIG m_kContMapCfg_Mission;

	static const std::wstring	ms_wstrPatchVersionC;
	static const std::wstring	ms_wstrPatchVersionS;

public://싱크에 사용
	typedef enum : short int 
	{
		E_RunMode_Normal = 0x0000,
		E_RunMode_Debug = 0x0001,
		E_RunMode_ReadIni = 0x0002,
		E_RunMode_ReadInb = 0x0004,
	} E_RunMode;

protected:
	mutable ACE_RW_Thread_Mutex m_kMutex;
};

#define g_kProcessCfg SINGLETON_STATIC(CProcessConfig)

#endif // WEAPON_LOHENGRIN_CONFIG_PROCESSCONFIG_H