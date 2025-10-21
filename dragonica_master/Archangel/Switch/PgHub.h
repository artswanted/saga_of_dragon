#ifndef SWITCH_SWITCHSERVER_PGHUB_H
#define SWITCH_SWITCHSERVER_PGHUB_H

#include "ace/high_res_Timer.h"
#include "BM/ObjectPool.h"
#include "Loki/Singleton.h"
#include "PgReserveMemberData.h"

typedef enum eAntiHackType
{
	ANTIHACK_NONE,
	ANTIHACK_DUMMY,				// 개발 테스트용
	ANTIHACK_GAMEGUARD,			// INCA 게임가드
	ANTIHACK_AHN_HACKSHIELD,	// AhnLab 핵실드
}E_ANTIHACK_TYPE;

size_t const DELETE_RESERVED_USER_WAIT_TIME		= 30000;

HRESULT CALLBACK Check_PT_C_S_TRY_ACCESS_SWITCH_SECOND(BM::Stream &kPacket);
HRESULT CALLBACK Check_PT_C_S_ANS_GAME_GUARD_CHECK(BM::Stream &kPacket);

namespace PgHubUtil
{
	HRESULT UserDisconnectedLog(TCHAR const* szFunc, size_t const iFuncLine, int const iCause, PgReserveMemberData const& rkRMD, CEL::CSession_Base* pkSession = NULL, BM::vstring const& rkExtern = BM::vstring(), BYTE byReason = 0);
	bool IsCanTwiceCheckRegion();
	bool MakeCheckAntiHackQuery(BM::Stream &rkOut, PgReserveMemberData* pkRMD, DWORD const dwTime, wchar_t const* szFunc, size_t const iLine);
};

//!!!인원수 컨트롤은 스위치 서버가 하지 않습니다.
class PgHub
{
	friend struct ::Loki::CreateStatic< PgHub >;
public:
	static int		ms_iAntiHackCheck_Min;
	static int		ms_iAntiHackCheck_Max;
	static int		ms_iAntiHackCheck_Wait;
	static bool		ms_bDisconnectHackUser;
	static bool		ms_bUseAntiHackServerBind;
	static bool		ms_bUseAntiHackDisconnect2nd;

	typedef std::unordered_map< BM::GUID, PgReserveMemberData* ,BM::CGuid_hash_compare> CONT_SWITCH_PLAYER;		// <MemberGuid, SReservedMemberData*>
	typedef std::unordered_map< BM::GUID, PgReserveMemberData* ,BM::CGuid_hash_compare> CONT_SWITCH_RESERVER;	// <SwitchOrderKey, SReservedMemberData*>
	typedef std::map<BM::GUID, SChannelMapMove> CONT_CHANNEL_MAPMOVE_PLAYER;

protected:
	PgHub();
	virtual ~PgHub();

	//오더를 받은 유저 예약.
	//해당 유저는 제한 시간을 등록 해놓는다.
public:
	HRESULT InitAntiHack( E_ANTIHACK_TYPE const kAntiHackType );

public:
	void Locked_Recv_PT_T_S_ANS_LOGINED_PLAYER(HRESULT const hRecvRet, BM::GUID const& rkMemberGuid, BM::GUID const& rkSwitchKey);
	void Locked_OnRecvFromUser(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	void Locked_OnTimer_ClearTimeOverMember();
	void Locked_OnTimer_CheckPing();
	void Locked_OnDisconnectUser(CEL::CSession_Base *pkSession);
	void Locked_OnDisconnectFromCenter( bool const bPublic );
	bool Locked_SendPacketToUserFromMap(CONT_GUID_LIST const &kContTarget, BM::Stream const &rkPacket) const;
	
	bool Locked_RemoveMember(SERVER_IDENTITY const &kSI);
	void Locked_DisplayState() const;

	bool Locked_SendPacketToUser( BM::GUID const &rkMemberGuid, BM::Stream const &rkPacket) const;

	bool Locked_RemoveMember(EClientDisconnectedCause const eCause, BM::GUID const &kMemberGuid, BYTE byReason = 0 );
	bool Locked_RemoveMember(EClientDisconnectedCause const eCause, BM::GUID const &rkMemberGuid, BM::Stream &kCPacket );
	bool Locked_ChangeMemberMapServerNo( SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::GUID const& kMemberGuid, BM::GUID const& kCharGuid );
	bool Locked_ProcessReserveMember(const SReqSwitchReserveMember& rkRSRM);//쎈터로 부터 유저의 기본 정보를 모두셋팅해둔다.
	int  Locked_GetSwitchConnectionPlayerCount() const;
	void Locked_RegistChannelMapMoveUser(BM::Stream * const Packet);

	E_ANTIHACK_TYPE AntiHackType()const	{ return m_kAntiHackType; }
protected:
	bool SendPacketToUser( BM::GUID const &rkMemberGuid, BM::Stream const &rkPacket) const;
	void CALLBACK ProcessUserPacketForLogined( PgReserveMemberData const &rkRMD, unsigned short usType, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	void CALLBACK ProcessUserPacketForNotLogin( unsigned short usType, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	bool __RemoveMember(EClientDisconnectedCause const eCause, BM::GUID const &rkMemberGuid, BM::Stream& rkPacket = BM::Stream(), BYTE byReason=0);
	void SendNfyPlayerCount()const;

	bool TryAccessSwitch(CEL::CSession_Base *pkSession, STryAccessSwitch const &rkTAS);
	bool TryAccessSwitch2( CEL::CSession_Base *pkSession, BM::GUID const &guidSwitchKey, BM::Stream * const pkPacket );
protected:
	E_ANTIHACK_TYPE m_kAntiHackType;

	CONT_SWITCH_RESERVER m_kReserveMember;
	CONT_SWITCH_PLAYER m_kContMemberSession;//이게 진짜 할당된 친구들 //First = MemberGuid
	CONT_CHANNEL_MAPMOVE_PLAYER m_ContMapMoveUser;	// 채널이동하면서 맵을 바꿀 유저들. 이 컨테이너에 등록안되있는데 요청하면 거절해야됨.

	mutable ACE_RW_Thread_Mutex m_kMutex;

	CLASS_DECLARATION_S(CEL::ADDR_INFO, AcceptorAddr);

protected:
	// Ahn HackShield
	AHNHS_SERVER_HANDLE	m_kHackShieldHandle;
};

#define g_kHub SINGLETON_STATIC(PgHub)

#endif // SWITCH_SWITCHSERVER_PGHUB_H