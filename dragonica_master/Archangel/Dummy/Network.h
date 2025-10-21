#pragma once
#include "BM/GUID.h"
#include "lohengrin/PacketStruct.h"
#include "lohengrin/PacketType.h"
#include "Loki/singleton.h"
#include "constant.h"
#include "datapack/pgdatapackmanager.h"

typedef struct
{
	TCHAR chID[20];
	short int sBeginIndex;
	short int sEndIndex;
	TCHAR chPassword[20];
	CEL::ADDR_INFO sLoginServer;
	BYTE byTimer50ms;

	short sRealm;
	short sChannel;
}TEST_INFO;

class CClientNetwork
{
	friend struct ::Loki::CreateStatic< CClientNetwork >;
protected:
	CClientNetwork()
	{
		m_vtRegist.clear();
	}
	virtual ~CClientNetwork()
	{
	}

protected:
	typedef struct tagRegistState
	{
		tagRegistState()
		{
			bIsComplete = false;
		}

		BM::GUID guidOrder;
		bool bIsComplete;
	}SRegistState;
	typedef std::vector< SRegistState > REGIST_ARRAY;

public:
	bool Init();

	//bool ConnectLoginServer(const CEL::ADDR_INFO &addr);

	bool AddRegist( const BM::GUID &guidOrder );
	bool SetRegistComplete( const BM::GUID &guidOrder );
	
	bool IsRegistAllComplete()const;
	bool ParseArg(const int argc, _TCHAR* argv[]);
	const TEST_INFO* GetTestInfo()const{ return &m_kTestInfo; }

	bool ReadConfigFile(LPCTSTR lpszFile);
public:
	// TODO Function
	bool DoConnectLogin(CEL::SESSION_KEY &rkOutSessionGuid);
	bool DoConnectSwitch(const CEL::ADDR_INFO &rkSwitchAddr, CEL::SESSION_KEY &rkOutSessionGuid);
	bool SendToSwitch(const CEL::SESSION_KEY &rkSessionKey, const BM::Stream &rkPacket);
	bool SendToLogin(const CEL::SESSION_KEY &rkSessionKey, const BM::Stream &rkPacket);

	BM::VersionInfo const & GetPatchVersion(){return m_kVersionInfo;}

protected:
	bool RegistLoginConnector();
	bool RegistSwitchConnector();
	bool RegistTimer();

	bool ParseArg_Sub( const std::wstring &wstrText );
	

	static void CALLBACK OnRegist(const CEL::SRegistResult &rArg);

	static void CALLBACK OnConnectFromLogin( CEL::CSession_Base *pSession );
	static void CALLBACK OnDisconnectFromLogin( CEL::CSession_Base *pSession );
	static void CALLBACK OnRecvFromLogin( CEL::CSession_Base *pSession, BM::Stream * const pPacket );

	static void CALLBACK OnConnectFromSwitch( CEL::CSession_Base *pSession );
	static void CALLBACK OnDisconnectFromSwitch( CEL::CSession_Base *pSession );
	static void CALLBACK OnRecvFromSwitch( CEL::CSession_Base *pSession, BM::Stream * const pPacket );

protected:
	SSwitchReserveResult m_SRR;

	REGIST_ARRAY m_vtRegist;

	CLASS_DECLARATION_S(BM::GUID, LoginConnector);
	CLASS_DECLARATION_S(BM::GUID, SwitchConnector);

	TEST_INFO m_kTestInfo;
	mutable Loki::Mutex m_kMutex;
	BM::VersionInfo m_kVersionInfo;

//아래 얘는 임시
protected:
	void RegistAcceptor();
	static void CALLBACK OnConnectFromX( CEL::CSession_Base *pSession );
	static void CALLBACK OnDisconnectFromX( CEL::CSession_Base *pSession );
	static void CALLBACK OnRecvFromX( CEL::CSession_Base *pSession, BM::Stream * const pPacket );
public:
	static int m_iTotalSessionCount;
};

#define g_kNetwork SINGLETON_STATIC(CClientNetwork)
