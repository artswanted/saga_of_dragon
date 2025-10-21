#ifndef SWITCH_SWITCHSERVER_PGRESERVEMEMBERDATA_H
#define SWITCH_SWITCHSERVER_PGRESERVEMEMBERDATA_H

#include "GameGuard/ggsrv25.h"

#ifdef HACKSHIELD_AUTO	//중국/일본용
#include "HackShield_AUTO/AntiCpXSvr.h"
#else	//기본 설정
#include "HackShield_UNAUTO/AntiCpXSvr.h"
#endif

typedef struct tagSwitchPingInfo
{//클라이언트 마지막 타임은 어디에 기록 하나.
	tagSwitchPingInfo()
		:m_kAceTime(0,0)
	{
		dwClientTime = 0;
		dwServerTime = 0;
		dwPingTime = 0;
//		m_kSimpleData.m_kAceTime = ACE_OS::gettimeofday();
//		BM::Rand_Range(SHRT_MAX, SHRT_MIN);
	}

	tagSwitchPingInfo(tagSwitchPingInfo const& rhs)
		: m_kGuid(rhs.m_kGuid), m_kAceTime(rhs.m_kAceTime), dwClientTime(rhs.dwClientTime), dwServerTime(rhs.dwServerTime), dwPingTime(rhs.dwPingTime)
	{
	}

	bool Build()
	{
		m_kGuid.Generate();
		dwServerTime = BM::GetTime32();
		m_kAceTime = ACE_OS::gettimeofday();
		dwClientTime = 0;
		return true;
	}

	bool IsEmpty()const
	{
		if( m_kGuid.IsNull() )
		{
			return true;
		}

		return false; 
	}

	BM::GUID m_kGuid;//GUID 만 주고 받고 하자.

	ACE_Time_Value m_kAceTime;//기록 시간. (시스템타임)
	DWORD dwServerTime;//기록 시간.(TimeGetTime)
	DWORD dwClientTime;//기록 시간.(TimeGetTime)
	DWORD dwPingTime;//Ping 길이
//	SPingInfo m_kSimpleData;
}SSwitchPingInfo;

class PgReserveMemberData
	:	public SSwitchPlayerData
{
public:
	static int	ms_iSpeedCheckGap;

	PgReserveMemberData(void);
	explicit PgReserveMemberData( SSwitchPlayerData const & ); 
	virtual ~PgReserveMemberData(void);

	PgReserveMemberData& operator = ( tagReqSwitchReserveMember const &rhs );

	virtual HRESULT MakePacket_CheckAntiHack( BM::Stream &rkPacket ){return E_FAIL;}
	virtual HRESULT CheckAntiHack( BM::Stream &rkPacket ){return S_OK;}
	virtual bool IsCheckAntiHack(void)const{return false;}

	void WriteToPacket( BM::Stream &rkPacket )const;
	bool ReadFromPacket( BM::Stream &rkPacket );

	HRESULT DoSendPing(int const iCallType);
	HRESULT RecvPing(BM::GUID const &kRecvGuid/*, DWORD const &dwClientTime*/);
	bool DropLastRecvPing(BM::GUID const &kRecvGuid, DWORD &dwOutLatency);

public:
	SERVER_IDENTITY kSI;//접속중인 서버
	SGroundKey kGroundKey;
	SGroundKey kClientGroundKey;	// 클라이언트가 존재하는 GroundKey
	BM::GUID guidCharacter;//사용중인 캐릭터 GUID-> 맵서버와 통신용
	CEL::CSession_Base *m_pkSession;
	bool m_bPublicConnect;// 공용채널에 접속중이냐?
	DWORD m_dwLastAntiHackCheckTime;	// 다음 AntiHack Check 시간
	DWORD m_dwNextAntiHackCheckTime;	// 다음 AntiHack Check 시간
	bool  m_bWaitAntiHackCheck;		// AntiHack 응답을 기다리는 중이다.	

protected:
	typedef std::list< SSwitchPingInfo > CONT_PING_INFO;
	CONT_PING_INFO m_kContPingSendTime;
	SSwitchPingInfo m_kLastRecvPing;//마지막 받았던 기록.
	SSwitchPingInfo m_kLastSendPing;//마지막 받았던 기록.
	SSwitchPingInfo m_kPingStartLine;//핑 체크의 기준.

	mutable Loki::Mutex m_kMutex;

	int m_kSendCount;
};

class PgReserveMemberData_Dummy
	:	public PgReserveMemberData
{
	public:
		PgReserveMemberData_Dummy();
		explicit PgReserveMemberData_Dummy(SSwitchPlayerData const& rkSPD); 
		virtual HRESULT MakePacket_CheckAntiHack(BM::Stream &rkPacket);
		virtual HRESULT CheckAntiHack(BM::Stream &rkPacket);
		virtual bool IsCheckAntiHack() const;
};

class PgReserveMemberData_GameGuard
	:	public PgReserveMemberData
{
public:
	PgReserveMemberData_GameGuard(void);
	explicit PgReserveMemberData_GameGuard( SSwitchPlayerData const & ); 
	virtual ~PgReserveMemberData_GameGuard(void);

	virtual HRESULT MakePacket_CheckAntiHack( BM::Stream &rkPacket );
	virtual HRESULT CheckAntiHack( BM::Stream &rkPacket );
	virtual bool IsCheckAntiHack(void)const{return 0 == byGMLevel;}

protected:
	CCSAuth2	m_kCsa;
};

class PgReserveMemberData_AhnHackShield
	:	public PgReserveMemberData
{
public:
	PgReserveMemberData_AhnHackShield(void);
	explicit PgReserveMemberData_AhnHackShield( SSwitchPlayerData const & ); 
	virtual ~PgReserveMemberData_AhnHackShield(void);

	void SetHandle( AHNHS_CLIENT_HANDLE const kHandle ){m_kHackShieldHandle=kHandle;}

	virtual HRESULT MakePacket_CheckAntiHack( BM::Stream &rkPacket );
	virtual HRESULT CheckAntiHack( BM::Stream &rkPacket );
	virtual bool IsCheckAntiHack(void)const{return ANTICPX_INVALID_HANDLE_VALUE != m_kHackShieldHandle;}

protected:
	AHNHS_CLIENT_HANDLE		m_kHackShieldHandle;
};

#endif // SWITCH_SWITCHSERVER_PGRESERVEMEMBERDATA_H