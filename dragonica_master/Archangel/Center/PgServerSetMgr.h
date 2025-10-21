#ifndef CENTER_CENTERSERVER_SERVERSET_PGSERVERSETMGR_H
#define CENTER_CENTERSERVER_SERVERSET_PGSERVERSETMGR_H

#include <map>

#include "BM/GUID.h"
#include "BM/ClassSupport.h"
#include "BM/ObjectPool.h"
#include "Loki/singleton.h"
#include "Variant/pgtotalobjectmgr.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgMission.h"
#include "Lohengrin/packetstruct2.h"
#include "PgPortalMgr.h"
#include "Variant/PgConstellation.h"

#pragma pack (1)

int const CHECKNAME_DELAYTIME = 180000;// »зїл°ЎґЙЗС ДіёЇЕНАМё§АОБцё¦ DBїЎј­ ЖЗє°ЗС ИД ј­№цёЮёрё®їЎ ДіЅМµЗѕо АЦґВ ЅГ°Ј

typedef std::unordered_map< BM::GUID, PgPlayer*, BM::CGuid_hash_compare> CONT_CENTER_PLAYER_BY_KEY;// first key : member guid
typedef std::unordered_map< std::wstring, PgPlayer* > CONT_CENTER_PLAYER_BY_ID;
typedef std::unordered_map< BM::GUID, BM::GUID, BM::CGuid_hash_compare> CONT_PLAYER_MAPMOVE_LOCK;	// <CharacterGuid, MapMove_Lock_Key>

class SServerBalance
:	public SERVER_IDENTITY
{
public:
	SServerBalance()
	{
		Clear();
	}

	void Get(SERVER_IDENTITY& kSI)
	{
		kSI = (SERVER_IDENTITY)(*this);
	}

	void Clear()
	{
		SERVER_IDENTITY::Clear();
		m_iGroundPoint = 0;
		Live(false);
	}

	void IncGround(T_GNDATTR kGndAttr)
	{
		if ( kGndAttr & GATTR_INSTANCE )
		{
			++m_iGroundPoint;
		}
		else
		{
			m_iGroundPoint += 10;
		}
	}

	void DecGround(T_GNDATTR kGndAttr)
	{
		if ( kGndAttr & GATTR_INSTANCE )
		{
			--m_iGroundPoint;
		}
		else
		{
			m_iGroundPoint -= 10;
		}
	}

	// ґЩёҐ operator Б¤АЗ Аэґл ±ЭБц!!!!
	bool operator==(SERVER_IDENTITY const &rhs)const
	{
		return (SERVER_IDENTITY)(*this) == rhs;
	}

	int Point()const{return Live() ? m_iGroundPoint : INT_MAX;}
	CLASS_DECLARATION(bool,m_IsLiveServer,Live);

private:
	int m_iGroundPoint;
};

typedef struct tagSRealmUserInfo
{
	void Clear()
	{
		kCharGuid.Clear();
		kMemberGuid.Clear();
		sChannel = 0;
		wName = _T("");
	}

	tagSRealmUserInfo()
	{
		Clear();
	}

	tagSRealmUserInfo(tagSRealmUserInfo const& rhs)
	{
		kCharGuid = rhs.kCharGuid;
		kMemberGuid = rhs.kMemberGuid;
		sChannel = rhs.sChannel;
		wName = rhs.wName;
	}

	void Set( PgPlayer const &rkPlayer )
	{
		kCharGuid = rkPlayer.GetID();
		kMemberGuid = rkPlayer.GetMemberGUID();
		wName = rkPlayer.Name();
		sChannel = rkPlayer.GetChannel();
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kCharGuid);
		kPacket.Pop(kMemberGuid);
		kPacket.Pop(sChannel);
		kPacket.Pop(wName);
	}

	void WriteToPacket(BM::Stream &kPacket)
	{
		kPacket.Push(kCharGuid);
		kPacket.Push(kMemberGuid);
		kPacket.Push(sChannel);
		kPacket.Push(wName);
	}

	BM::GUID kCharGuid; // Characer Guid
	BM::GUID kMemberGuid;	// Member Guid
	short int sChannel;	// БўјУ БЯАО Channel №шИЈ
	std::wstring wName;	// Character Name (Game NickName)
} SRealmUserInfo;

typedef enum : PACKET_ID_TYPE
{
	ERealmUser_EnterUser = 1,
	ERealmUser_LeaveUser = 2,
	ERealmUser_Req_AllYourUser = 3,
	ERealmUser_Res_AllYourUser = 4,
} ERealmUser_Packet;

typedef enum
{
	MMT_NONE,
	MMT_DEFAULT,
	MMT_FIRST,
	MMT_PUBLIC_PERSONAL,
	MMT_PUBLIC_PARTY,
} E_MAP_MOVE_TYPE;

#pragma pack ()

//ј­№ц БўјУ ёс·ПАє ProcessCfg °Ў °ЎБц°н.
//±Ч Б¤єёё¦ Едґл·О АЫѕчЗСґЩ
class PgServerSetMgr
{
	friend struct ::Loki::CreateStatic< PgServerSetMgr >;
public:
	typedef enum 
	{
		ESSMP_None = 0,
		ESSMP_SSwitchPlayerData = 1,
	}EServerSetMgrPacketOp;

protected:
	//typedef std::unordered_map< std::wstring, SCheckNameInfo* >	CONT_CHECK_NAME;// ДіёЇЕН »эјєЅГ »зїл°ЎґЙЗС ДіёЇЕНАМё§АОБц °Л»з (first key : MemberGuid)
	typedef std::unordered_map< BM::GUID, SReqSwitchReserveMember , BM::CGuid_hash_compare> CONT_SWITCH_WAIT;// ЅєА§ДЎїЎ ѕЦ ЗТґзЗШґЮ¶у°н єёі»іхАє ґл±вБЩ( first = АЇАъMember Key·О )

	mutable ACE_RW_Thread_Mutex m_kMutex;// LOCK(m_kContLoginedUserKey + m_kContPlayer_MemberID)
public:
	PgServerSetMgr(void);
	virtual ~PgServerSetMgr(void);

public:
	void Locked_Recv_PT_N_T_NFY_SELECT_CHARACTER( BM::Stream * const pkPacket );

	void Locked_Build(CONT_SERVER_HASH const &kContServerHash, CONT_MAP_CONFIG const &kContMapCfg_Static, CONT_MAP_CONFIG const &kContMapCfg_Mission);
	
	void Locked_ShutDown(void);
	void Locked_CheckShutDown(void);

//	HRESULT Locked_GetPlayerInfo_OtherChannel(std::wstring const &rkCharName, SContentsUser &rkOut) const;
	HRESULT Locked_GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut) const;
	HRESULT Locked_GetPlayerInfo(std::wstring const &rkCharName, SContentsUser &rkOut) const;
	bool Locked_IsPlayer(BM::GUID const &rkGuid, bool const bIsMemberGuid)const;

	bool Locked_SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid = true/* false ґВ ДіёЇЕНguid */)const;
	bool Locked_GetRealmUserByMemGuid(BM::GUID const &rkCharGuid, SRealmUserInfo& rkRealmInfo)const;
	bool Locked_GetRealmUserByNickname(std::wstring const& wstrNickname, SRealmUserInfo& rkRealmInfo)const;
	bool Locked_GetRealmUserByCharGuid(BM::GUID const &rkCharGuid, SRealmUserInfo& rkRealmInfo)const;
	bool Locked_SendToGround(SGroundKey const &kKey, BM::Stream const &rkPacket, bool const bIsGndWrap = true)const;
	bool Locked_SendToGround(BM::GUID const &kCharacterGuid, BM::Stream const &rkPacket, bool const bIsGndWrap = true)const;
//	void Locked_RecvRealmUserMgrPacket(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	HRESULT Locked_RegistGnd( SGround const &rkGnd );
	HRESULT Locked_UnregistGnd(const GND_KEYLIST &kGndKeyList);
	//HRESULT Locked_UnregistGnd(SERVER_IDENTITY const &kSI);
	HRESULT Locked_ReqUnregistGnd(SERVER_IDENTITY const &kSI);

	bool Locked_ProcessRemoveUser(SERVER_IDENTITY const &kSI);//ј­№ц°Ў ¶іѕоБіАЅ.
	bool Locked_ProcessRemoveUser(BM::GUID const &kMemgerGuid, bool bNotify = true, bool bNotifyMap = false );//GUID·О АЇАъ БўјУ Іч±в

	bool Locked_Recv_PT_M_T_REQ_MAP_MOVE( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_T_T_REQ_MAP_MOVE( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_T_T_REQ_RECENT_MAP_MOVE( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_T_T_ANS_MAP_MOVE_RESULT( BM::GUID const &kCharGuid, SAnsMapMove_MT& rkAMM );
	bool Locked_Recv_PT_M_T_ANS_MAP_MOVE_RESULT( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_M_T_REQ_Kick_User( BM::Stream * const pkPacket  );
	void Locked_Recv_PT_M_T_REFRESH_LEVELUP(BM::Stream* const pkPacket);
	bool Locked_PT_T_T_REQ_EXIT_LOBBY( BM::Stream* const pkPacket );
	void Locked_PT_T_T_REQ_KICK_ROOM_USER( BM::GUID const& kCharGuid, SGroundKey const &kGndKey )const;
	void Locked_Recv_PT_M_T_REQ_MAP_MOVE_COME( SReqMapMove_CM const &kRMMC, VEC_GUID const &kUserList, bool const bMemberGuid );
	bool Locked_Recv_PT_N_T_RES_MAP_MOVE( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_I_M_REQ_HOME_CREATE(BM::Stream * pkPacket);
	bool Locked_Recv_PT_I_M_REQ_HOME_DELETE( BM::Stream * const pkPacket  );
	void Locked_Recv_PT_M_T_REFRESH_CLASS_CHANGE(BM::Stream* const pkPacket);
	bool Locked_Recv_PT_M_M_UPDATE_PLAYERPLAYTIME( BM::Stream * const pkPacket );
	void Recv_PT_M_T_NFY_CREATE_GROUND( BM::Stream &rkPacket );

//	HRESULT Locked_UserOpenGround(BM::GUID const &rkMemberGuid, int const iGroundNo);
	HRESULT Locked_OnGreetingServer(SERVER_IDENTITY const &kRecvSI, CEL::CSession_Base *pkSession);

	HRESULT Locked_GroundLoadBalance( SGroundKey const &rkKey, SERVER_IDENTITY &kOutSI )const;
	bool Locked_IsAliveGround(SGroundKey const &rkKey)const;
	
//	void Locked_WriteToPacket_User(BM::Stream& rkPacket, EServerSetMgrPacketOp const eOp)const;
	size_t Locked_CurrentPlayerCount()const;
	void Locked_DisplayState();
	bool Locked_NfyReloadGameData();

	void Locked_SendWarnMessage(BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid=false)const;
	void Locked_SendWarnMessage2(BM::GUID const &rkCharGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid=false)const;
	bool Locked_SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap)const;
	bool Locked_SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid = true)const;

	bool Locked_TargetMapMoveByGuid( bool const bGMCommand, BM::GUID const &kCharGuid, BM::GUID const &kTargetGuid, bool const bMemberGuid )const;
	bool Locked_TargetMapMoveByName( bool const bGMCommand, BM::GUID const &kCharGuid, std::wstring const &wstrTargetName, bool const bID )const;

	// LockАМ ЗКїдѕшґЩ.
	void Recv_PT_S_T_ANS_RESERVE_MEMBER(BM::Stream * const pkPacket)const;
	void Recv_PT_M_T_ANS_MAP_MOVE_TARGET_FAILED( BM::Stream * const pkPacket )const;

protected:
	void Clear();
	bool Recv_PT_N_T_NFY_SELECT_CHARACTER(PgPlayer *pkPlayer, SReqMapMove_MT & rkRMM, EWRITETYPE const eWriteType, CONT_PET_MAPMOVE_DATA::mapped_type const * const pkPetMapMoveData, CONT_PLAYER_MODIFY_ORDER &kContModifyItemOrder );

	bool IsPlayer(BM::GUID const &rkGuid, bool const bIsMemberGuid)const;
	HRESULT GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut) const;
	bool RecvMapMove( BM::Stream * const pkPacket, bool bResMovePacket = false);
	bool UpdatePlayerData( UNIT_PTR_ARRAY& rkUnitArray, SPortalWaiter *pkPortalWaiter=NULL, bool const bClearGroundKey = false )const;
	
	bool IsAliveGround(SGroundKey const &rkKey)const;
	HRESULT GroundLoadBalance( SGroundKey const &kKey, SERVER_IDENTITY &kOutSI )const;

	HRESULT GetServerIdentity(SGroundKey const &kGndKey, SERVER_IDENTITY &rkOutSI)const;
	bool SendToGround(SGroundKey const &kKey, BM::Stream const &rkPacket, bool const bIsGndWrap = true)const;
	bool SendToGround(BM::GUID const &kCharacterGuid, BM::Stream const &rkPacket, bool const bIsGndWrap = true)const;
	bool SendToSwitch(BM::GUID const &kMemberGuid, BM::Stream const &Packet)const;
	std::wstring LoginedPlayerName(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid);
	bool RecvReqPreCreateCharacter(BM::GUID const &rkMemberGuid, int const iSwitchNo);
	bool ReqMapMove( UNIT_PTR_ARRAY& rkUnitArray, SReqMapMove_MT &rkRMM, Constellation::SConstellationMission &constellationMission, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER &kContModifyOrder );
	bool ReqTargetMapMoveByGuid( bool const bGMCommand, BM::GUID const &kCharGuid, BM::GUID const &kTargetGuid, bool const bMemberGuid )const;
	bool ReqTargetMapMoveByName( bool const bGMCommand, BM::GUID const &kCharGuid, std::wstring const &wstrTargetName, bool const bID )const;
	bool FindRandomMap( SGroundKey& kGndKey )const;

	bool RecvAnsMapMove( BM::GUID const &kCharGuid, SAnsMapMove_MT const &rkAMM, SGroundKey const &kRealGndKey );
	bool RecvAnsMapMove( UNIT_PTR_ARRAY &rkUnitArray, SAnsMapMove_MT const &rkAMM, SGroundKey const &kRealGndKey );

	bool CheckValidBaseWear(int const iWearNo, const EKindCharacterBaseWear eType);
	SGroundKey GetPlayerKey(BM::GUID const &rkMember);
	SGroundKey GetPlayerKey(std::wstring const &kId);
	void ProcessRemoveUser_BySwitch(SERVER_IDENTITY const &kSI);
	size_t ProcessRemoveUser_ByGround(SGroundKey const &kGndKey);
	void ProcessRemoveUser_Common(CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData, bool const bNotify = true);
	void ProcessRemoveUser_NotifyOtherMgr( CONT_CENTER_PLAYER_BY_KEY::mapped_type const &pCPData, bool const bParty )const;

	bool InsertPlayer(PgPlayer *pkPlayer);
	bool RemovePlayer(PgPlayer *pkPlayer);
	bool InsertMapMoveLock(BM::GUID const& rkMoveGuid, BM::GUID const& rkCharacterGuid);
	void RemoveMapMoveLock(BM::GUID const& rkCharacterGuid);
	bool GetMapMoveLock(BM::GUID const& rkCharacterGuid, BM::GUID& rkOutLock);

	void UnregistGnd( SGround const &kGnd );

protected:
	bool GetRealmUserByMemGuid(BM::GUID const &rkCharGuid, SRealmUserInfo& rkRealmInfo)const;
	bool GetRealmUserByCharGuid(BM::GUID const &rkCharGuid, SRealmUserInfo& rkRealmInfo)const;
	
	// Send °ь·Г ЗФјц
	void SendWarnMessage(BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid)const;
	void SendWarnMessage2(BM::GUID const &rkCharGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid )const;
	bool SendToUser(std::wstring const &kStrNick, BM::Stream const &rkPacket)const;
	bool SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid = true/* false ґВ ДіёЇЕНguid */)const;
	bool SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid)const;
	bool SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap)const;
	bool SendToChannel(short int sChannelNo, BM::Stream const &rkPacket)const;
//	__int64 _UpdatePlayTime( CONT_CENTER_PLAYER_BY_KEY::mapped_type pData );
//	void _ClearSelectCharacter( CONT_CENTER_PLAYER_BY_KEY::mapped_type pkSelectedPlayerData );

	CLASS_DECLARATION_S(CEL::SESSION_KEY, ContentsServer);
	CLASS_DECLARATION_S(CEL::SESSION_KEY, ItemServer);
	CLASS_DECLARATION_S(CEL::SESSION_KEY, MCtrlServer);
	

protected:
	CONT_CENTER_PLAYER_BY_KEY m_kContPlayer_MemberKey;//Member
	CONT_CENTER_PLAYER_BY_KEY m_kContPlayer_CharKey;//Guid
	CONT_CENTER_PLAYER_BY_ID m_kContPlayer_MemberID;//MemberID
	CONT_CENTER_PLAYER_BY_ID m_kContPlayer_CharName;//Name

	CONT_PLAYER_MAPMOVE_LOCK m_kPlayerMapMoveLock;	// MapMoveБЯАО АЇАъё¦ LockАв±в(АМБЯАё·О MapMove µЗґВ°Н №жБц)
	
	CONT_GROUND			m_kContAliveGround;//ЗцАз µо·ПµИ Ground

	// Server Balance
	typedef std::vector< SServerBalance* >					CONT_SERVER_BALANCE;//!!!!ї©±вїЎ µйАє ЖчАОЕНґВ Аэґл·О »иБ¦ЗПБц ё»АЪ!!!
	typedef std::map<int, std::set< SServerBalance* > >		CONT_GROUND2SERVER_BALANCE;//Key = GroundNo
	CONT_SERVER_BALANCE										m_kContServerBalance;
	CONT_GROUND2SERVER_BALANCE								m_kContGround2ServerBalance;

	bool			m_bShutDownServer;

private:
	SServerBalance* GetServerBalance(SERVER_IDENTITY const &rhs)const;
};

#define g_kServerSetMgr SINGLETON_STATIC(PgServerSetMgr)

#endif // CENTER_CENTERSERVER_SERVERSET_PGSERVERSETMGR_H