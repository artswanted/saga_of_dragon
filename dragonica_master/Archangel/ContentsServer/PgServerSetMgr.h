#ifndef CONTENTS_CONTENTSSERVER_SERVERSET_PGSERVERSETMGR_H
#define CONTENTS_CONTENTSSERVER_SERVERSET_PGSERVERSETMGR_H

#include <map>

#include "Loki/singleton.h"
#include "BM/GUID.h"
#include "BM/ClassSupport.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"
#include "Lohengrin/packetstruct2.h"
#include "Lohengrin/GameTime.h"
#include "Collins/Log.h"
#include "Variant/pgtotalobjectmgr.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgMission.h"
#include "Doc/PgDoc_Player.h"
#include "constant.h"
#include "OpenMarket.h"
#include "PgCharacterCardMgr.h"
#include "PgMyHomeMgr.h"
#include "PgGambleUserMgr.h"
#include "Variant/PgConstellation.h"

#pragma pack (1)

typedef std::vector< PgDoc_Player* > PLAYER_UNIT_ARRAY;

typedef std::unordered_map< BM::GUID, SCenterPlayerData*, BM::CGuid_hash_compare > CONT_CENTER_PLAYER_BY_KEY;// first key : member guid
typedef std::unordered_map< std::wstring, SCenterPlayerData* > CONT_CENTER_PLAYER_BY_ID;

class SServerBalance
:	public SERVER_IDENTITY
{
public:
	SServerBalance()
	{
		Clear();
	}

	void Get(SERVER_IDENTITY& kSI)const
	{
		kSI = *this;
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

	// 다른 operator 정의 절대 금지!!!!
	bool operator==(SERVER_IDENTITY const &rhs)const
	{
		return (SERVER_IDENTITY)(*this) == rhs;
	}

	int Point()const{return Live() ? m_iGroundPoint : INT_MAX;}
	CLASS_DECLARATION(bool,m_IsLiveServer,Live);

private:
	int m_iGroundPoint;
};

typedef struct tagActionOrder
{
	typedef std::set<BM::GUID> CONT_WORKDATA_TARGET;

	tagActionOrder() {}
	void Clear()
	{
		kGndKey.Clear();
		kCause = IMEPT_NONE;
		kContOrder.clear();
		kAddonPacket.Clear();
		kPacket2nd.Clear();
	}

	void InsertTarget(BM::GUID const &rkGuid)
	{
		if(rkGuid.IsNotNull())
		{
			m_kContTarget.insert(rkGuid);
		}
	}

	CONT_WORKDATA_TARGET m_kContTarget;
	SRealmGroundKey kGndKey;
	EItemModifyParentEventType kCause;
	CONT_PLAYER_MODIFY_ORDER kContOrder;
	BM::Stream kAddonPacket;
	BM::Stream kPacket2nd;
}SActionOrder;

typedef struct tagSRemoveWaiter
{
	tagSRemoveWaiter( bool bNotify=false, BM::GUID const &rkMemberID=BM::GUID::NullData() )
		:	bBroadcast(bNotify)
		,	kMemberGuid(rkMemberID)
	{
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(bBroadcast);
		rkPacket.Pop(kMemberGuid);
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(bBroadcast);
		rkPacket.Push(kMemberGuid);
	}

	bool bBroadcast;
	BM::GUID kMemberGuid;
} SRemoveWaiter;
//typedef std::vector<SRemoveWaiter> VECTOR_REMOVE_WAITER;

typedef struct tagSEventRwardTimeInfo
{
	tagSEventRwardTimeInfo()
	{
		PlayerGuid.Clear();
		LastRewardTime = 0;
		RewardCount = 0;
	}

	BM::GUID PlayerGuid;
	int RewardCount;
	__int64 LastRewardTime;

}SEventRwardTimeInfo;

namespace RealmUserManagerUtil
{
	void ClearCharacterConnectionChannel(BM::GUID const* pkMemberGuid = NULL);
};

#pragma pack ()

typedef std::map< BM::GUID, CONT_ITEM_CREATE_ORDER > CONT_GEN_SYS_ITEM;
typedef std::list< SActionOrder* > ListActionJob;
typedef std::set< BM::GUID > CONT_EVENT_USER;
typedef std::map< BM::GUID, PgDoc_Player* > CONT_PLAYER_WAITER;
//typedef std::unordered_map< BM::GUID, SCenterPlayerData*, BM::CGuid_hash_compare > CONT_CENTER_PLAYER_BY_KEY;// first key : member guid
//typedef std::unordered_map< std::wstring, SCenterPlayerData* > CONT_CENTER_PLAYER_BY_ID;
typedef std::map<BM::GUID,CONT_SELECTED_CHARACTER_ITEM_LIST> CONT_HOME_ITEM;
typedef std::map<int,int> CONT_ACHIEVEMENT_POINT;
typedef std::map<BM::GUID, SMemberPremiumData> CONT_MEMBER_PREMIUM;
typedef std::map<BM::GUID, SEventRwardTimeInfo> CONT_EVENTREWARD_TIMEINFO;

class PgRealmUserManager
{
	friend struct ::Loki::CreateStatic< PgRealmUserManager >;
public:
	typedef enum 
	{
		ESSMP_None = 0,
		ESSMP_SSwitchPlayerData = 1,
	}EServerSetMgrPacketOp;

	typedef enum : BYTE
	{
		ESERVER_STATUS_NONE = 0,
// 		ESERVER_STATUS_BEGIN = 1,
// 		ESERVER_STATUS_RUNNING = 2,
// 		ESERVER_STATUS_MUSTSTOP = 3,
		ESERVER_STATUS_STOPPING = 4,
	} ESERVER_STATUS;

	typedef std::map< short, size_t >	CONT_CHANNEL_USERCOUNT;

private:
	void SendRenameResult(BM::GUID const& kMemberGuid, BM::GUID const& kCharacterGuid, std::wstring const& kNewName, EUserCharacterRealmMergeReturn const eRet)
	{
		BM::Stream kPacket(PT_N_C_ANS_REALM_MERGE, kMemberGuid);
		kPacket.Push( eRet );
		kPacket.Push( kCharacterGuid );
		kPacket.Push( kNewName );
		SendToSwitch(kMemberGuid, kPacket);
	}

protected:
	typedef std::unordered_map< BM::GUID, SReqSwitchReserveMember, BM::CGuid_hash_compare > CONT_SWITCH_WAIT;// 스위치에 애 할당해달라고 보내놓은 대기줄( first = 유저Member Key로 )

	mutable PROFILE_MUTEX(ACE_RW_Thread_Mutex) m_kMutex;// LOCK(m_kContLoginedUserKey + m_kContLoginedUserID)
	//mutable Loki::Mutex m_kPlayerGroundKeyMutex; // LOCK (m_kContPlayer_GroundKey)
	//mutable Loki::Mutex m_kGetJobGroundUserContWithLock; // Function_CriticalSection(GetJobGroundUserContWithLock)
public:
	PgRealmUserManager(void);
	virtual ~PgRealmUserManager(void);

public:
	bool Locked_MarketHandleRecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	bool Locked_Q_DQT_GET_PLAYER_LIST(CEL::DB_RESULT &rkResult);
	bool Locked_Q_DQT_SELECT_PLAYER_DATA(CEL::DB_RESULT &rkResult);

//	bool Locked_Q_DQT_SELECT_PLAYER_DATA(CEL::DB_RESULT_TRAN & rkContResult);
	
	bool Locked_Q_DQT_CHECK_CHARACTERNAME_OVERLAP(CEL::DB_RESULT &rkResult);
	bool Locked_Q_DQT_CREATECHARACTER(CEL::DB_RESULT &rkResult);
	bool Locked_Q_DQT_DELETECHARACTER(CEL::DB_RESULT &rkResult);
	bool Locked_Q_DQT_REALMMERGE(CEL::DB_RESULT &rkResult);

	bool Locked_GetDoc_Player(BM::GUID const & rkGuid,bool const bMemberGuid,PgDoc_Player & rkCopyPlayer);

	HRESULT Locked_GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, SUserInfoExt* const pkExt = NULL) const;
	HRESULT Locked_GetPlayerInfo(std::wstring const &rkCharName, SContentsUser &rkOut) const;
	HRESULT Locked_GetPlayerInfo(BM::GUID const& rkGuid, bool const bIsMemberGuid, PgPremiumMgr & rkPremiumMgr) const;
	HRESULT Locked_GetPlayerInven(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::GUID const& kItemGuid, PgBase_Item & rkOutItem) const;

	bool Locked_SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid = true/* false 는 캐릭터guid */);

	bool Locked_Process_InsertSwitchWaitUser(const SReqSwitchReserveMember &rPlayer);
	void Locked_Recv_PT_S_T_ANS_RESERVE_MEMBER(BM::Stream * const pkPacket);
	void Locked_Recv_PT_IM_T_ANS_SWITCH_USER_ADD(HRESULT const hRecvRet, SSwitchPlayerData const &kSPD);
//	bool Locked_Recv_PT_I_T_NFY_INV_DATA(BM::Stream *pkPacket);
	bool Locked_Recv_PT_S_T_REQ_SELECT_CHARACTER(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid, bool const bPassTutorial);
	bool Locked_Recv_PT_T_N_NFY_SELECT_CHARACTER_FAILED( BM::GUID const &kCharGuid );
	bool Locked_Recv_PT_S_T_REQ_CREATE_CHARACTER(BM::GUID const &rkMemberGuid, SReqCreateCharacter const &kReqCreateCharacter);
	void Locked_Recv_PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP( const BM::GUID& rkMemberGuid, const std::wstring& rkCharacterName );
	bool Locked_Recv_PT_S_T_REQ_DELETE_CHARACTER(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid);
	bool Locked_Recv_PT_C_N_REQ_REALM_MERGE(BM::GUID const& rkMemberGuid, BM::GUID const& rkCharacterGuid, std::wstring const& rkNewName);
	void Locked_Recv_PT_M_T_REQ_SAVE_CHARACTER( BM::Stream * const pkPacket);
	void Locked_Recv_PT_C_T_REQ_SAVE_OPTION(BM::GUID const rkCharacterGuid, ClientOption const & rkOption);
	void Locked_Recv_PT_N_T_REQ_JOBSKILL3_HISTORYITEM(BM::GUID const rkCharacterGuid, int const iSaveIdx);
	void Locked_Recv_PT_C_T_REQ_SAVE_SKILLSET(BM::GUID const rkCharacterGuid, CONT_USER_SKILLSET const & rkSkillSet);
	bool Locked_Recv_PT_S_T_REQ_CHARACTER_LIST(BM::GUID const &rkMemberGuid, short const nChannelNo);

	bool Locked_Recv_PT_T_N_REQ_MAP_MOVE( BM::Stream * const pkPacket )const;
	void Locked_Recv_PT_T_N_NFY_DELETE_GROUND(short const sChannel, GND_KEYLIST const &rkGndList);
	void Locked_Recv_PT_A_A_SERVER_SHUTDOWN(SERVER_IDENTITY const rkSI);
	void Locked_Recv_PT_M_I_REQ_CASH_MODIFY( BM::Stream * const pkPacket ) const;
	
	HRESULT Locked_OnGreetingServer(SERVER_IDENTITY const &kRecvSI, CEL::CSession_Base *pkSession);
	
	void Locked_WriteToPacket_User(BM::Stream& rkPacket, EServerSetMgrPacketOp const eOp)const;
	size_t Locked_CurrentPlayerCount()const;
	size_t Locked_CurrentPlayerCount( CONT_CHANNEL_USERCOUNT &rkCont )const;
	void Locked_DisplayState(bool bDisplayUserID = true);
	bool Locked_NfyReloadGameData();

	bool Locked_SendToSwitch(BM::GUID const & MemberGuid, BM::Stream const & Packet);
	void Locked_SendWarnMessage( BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid=false);
	void Locked_SendWarnMessage2( BM::GUID const &rkCharGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid=false);
	bool Locked_SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap);
	bool Locked_SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid = true);
	void Locked_OnTick10s();
	void Locked_UpdatePlayerTime(BM::GUID const &rkCharacterGuid, BM::Stream * pkPacket);
	void Locked_ClearPlayerWaiter(ACE_Time_Value const &kNowTime);
	void Locked_RecvPT_A_NFY_USER_DISCONNECT(BM::Stream * const pkPacket, bool const bNotifyImm = false);
	bool Locked_SetServerState(ESERVER_STATUS const eNew);
	ESERVER_STATUS Locked_GetServerState() const;

	bool Recv_PT_C_T_REQ_CHANNLE_INFORMATION(BM::GUID const &kMemberGuid);
	bool Recv_PT_M_N_NOTIFY_FIRST_LOGIN(BM::Stream & kPacket);

	template < typename _T_ForFunc >
	void Locked_For_Each_Player(_T_ForFunc &rkFunc)
	{//문제는 외부함수가 Lock 을 걸어버리면 답이 없다는거.
		BM::CAutoMutex kLock(m_kMutex);
		CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_MemberKey.begin();
		while(m_kContPlayer_MemberKey.end() != iter)
		{
			BM::CAutoMutex kLock(iter->second->kMutex);
			if( !rkFunc(*iter) )
			{
				break;
			}
			++iter;
		}
	}

	bool Locked_EnterEvent(BM::GUID const & kCharGuId);
	void Locked_LeaveEvent(BM::GUID const & kCharGuId);
	DWORD Locked_ContentsEvent(SActionOrder const *pkActionOrder);
	void Locked_UpdateMissionReport(SActionOrder const *pkActionOrder);
	void Locked_CIE_LOAD_PET_ITEM(BM::Stream &rkPacket);
	HRESULT Locked_ProcessModifyPlayer(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder,
		SRealmGroundKey const &kGndKey, BM::Stream const &kAddonPacket, BM::Stream & rkPacket);
	void Locked_SendToLoginPlayer_LuckyStarOpen();
	void Locked_Recv_PT_N_M_REQ_COMPLETEACHIEVEMENT(BM::Stream * pkPacket)const;
	void Locked_RecvPT_C_M_REQ_CLIENT_CUSTOMDATA(BM::Stream * pkPacket);

	bool Locked_RegistGuildInfo(const BM::GUID &rkGuildGuid, const std::wstring rkGuildName, const BYTE* pkInvExtern, const BYTE* pkExternIdx, CONT_SELECTED_CHARACTER_ITEM_LIST *pkItemList = NULL, const __int64 iMoney = 0i64 );
	bool Locked_UnregistGuildInfo(BM::GUID const& rkGuildGuid);

	void Locked_Recv_PT_S_T_REQ_SAVE_CHARACTOR_SLOT(BM::GUID const& rkMemberGuid, BM::GUID const& rkCharacterGuid, int const iSlot);
	void Locked_Recv_PT_S_T_REQ_FIND_CHARACTOR_EXTEND_SLOT(BM::GUID const& rkMemberGuid, short const nChannelNo);

protected:
	HRESULT GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, SUserInfoExt* const pkExt = NULL) const;
	DWORD RecvMapMove( BM::Stream * const pkPacket );
//	bool UpdatePlayerData( UNIT_PTR_ARRAY& rkUnitArray, BM::Stream * const pkPacket, SReqMapMove_MT const* pkRMM  = NULL);
	void UpdateRecentPos(PgDoc_Player *pkPlayer, short const nChannelNo, SReqMapMove_MT const* pkRMM);

	std::wstring LoginedPlayerName(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid);
	bool RecvReqPreCreateCharacter(BM::GUID const &rkMemberGuid, int const iSwitchNo);
	bool ReqFirstMapMove( UNIT_PTR_ARRAY const &kUnitArray, SReqMapMove_MT &rkRMM, CONT_PET_MAPMOVE_DATA const &kContPetMapMoveData, CONT_PLAYER_MODIFY_ORDER * pkModifyItemOrder = NULL, bool const bReCall = false )const;
	bool ReqMapMove( UNIT_PTR_ARRAY const &rkUnitArray, SReqMapMove_MT const &rkRMM, Constellation::SConstellationMission & constellationMission, CONT_PET_MAPMOVE_DATA &rkContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &rkContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER& kModifyItemOrder )const;
	bool RecvAnsMapMove( UNIT_PTR_ARRAY &rkUnitArray, SAnsMapMove_MT const &rkAMM );
	bool CheckValidBaseWear(int const iWearNo, const EKindCharacterBaseWear eType);
	void SaveMemberData(CONT_CENTER_PLAYER_BY_KEY::mapped_type pData);
	void SaveMemberSkillSetData(BM::GUID const & kID, CONT_USER_SKILLSET const & kContSkillSet, ESkillTabType const eType)const;
	void SaveStrategySkillData(PgDoc_Player const * pkPlayer)const;
	bool CanReserveNewAccount()const;
	HRESULT GetLoadBalanceSwitch(CONT_SERVER_HASH const &kContServer, SERVER_IDENTITY &rkSI)const;
	//bool ProcessRemoveUser(SERVER_IDENTITY const &kSI);
	//void _ProcessRemoveUser(SERVER_IDENTITY const &kSI);
	//void _ProcessRemoveCharacter( PgDoc_Player*& pkDocPlayer );
	void _ProcessRemoveCharacter( SCenterPlayerData* pkCenterPlayer );
	void ProcessRemoveUser_Common( CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData, bool const bNotify=true, SReqSwitchReserveMember const *pkSRM = NULL );
	bool ProcessRemoveUser(BM::GUID const &kMemgerGuid, bool const bNotify, SReqSwitchReserveMember const *pkSRM );
	//void AddRemoveWaiter( SRemoveWaiter const &kWaiter );
	//void ClearRemoveWaiter();

	enum E_FAIL_LOG_TYPE
	{
		EFLT_SYS_INV	= 0,
		EFLT_RESULT		= 1,
	};
	typedef std::map<E_FAIL_LOG_TYPE, PgContLogMgr> CONT_FAIL_LOG_MGR;

	typedef struct _SProcessModifyResult
	{
		_SProcessModifyResult(HRESULT const _1st, HRESULT const _2nd)
			: hResult(_1st), hFailProcessResult(_2nd)
		{
		}
		HRESULT hResult;
		HRESULT hFailProcessResult;
	} SProcessModifyResult;

	SProcessModifyResult ProcessModifyPlayer(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
		CONT_DOC_PLAYER const &kContCopyInventory, CONT_DOC_PLAYER const &kContOrgInv, BM::Stream const &kAddonPacket, BM::Stream & rkPacket,PgContLogMgr & kContLogMgr);
    SProcessModifyResult ProcessFailModifyPlayer(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
		CONT_DOC_PLAYER const &kContCopyPlayer, CONT_DOC_PLAYER const &kContOrgPlayer, BM::Stream const &kAddonPacket, BM::Stream & rkPacket,PgContLogMgr & kContLogMgr);
	HRESULT ProcessModifyPlayer_D(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, CONT_DOC_PLAYER &kContPlayer, BM::Stream const &kAddonPacket, BM::Stream & rkPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr & kContLogMgr);//당분간. 직접 처리.
	void ProcessModifyPlayer_LogOut(EItemModifyParentEventType const kCause, SPMO const &kOrder, SRealmGroundKey const &kGndKey);
	HRESULT ProcessModifyItemToDB(EItemModifyParentEventType const kEventCause,DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, BM::Stream & rkPacket, CONT_DOC_PLAYER const &kContOrgInv);
	void ProcessModifyItemFailSub(SPMO const & rkOrder, CONT_ITEM_CREATE_ORDER & rkCreateItem, PgContLogMgr & kContLogMgr);
	HRESULT ProcessModifyItemFail(EItemModifyParentEventType const kCause, SPMO const & rkOrder, CONT_GEN_SYS_ITEM & rkGenSysItem, CONT_DOC_PLAYER const &kContOrgInv, CONT_FAIL_LOG_MGR & kContFailLogMgr);
	void ProcessModifyItemFail_GambleMachine_CashShop(SPMO const & rkOrder, PgContLogMgr & kContLogMgr);
	void ProcessModifyItemFail_GambleMachine_Mixup(SPMO const & rkOrder, PgContLogMgr & kContLogMgr);

	void ProcessUpdateCacheDB( PgDoc_Player *pkDocPlayer );
//	HRESULT ProcessModifyItemSub(SPMO const &kOrder, PgDoc_Player *pkTargetInv, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray);
	void ProcessModifyPlayer_D_ResultProcess(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
											BM::Stream const &kAddonPacket, BM::Stream & rkPacket,HRESULT const hRet);

	virtual HRESULT ProcessModifyToDBSub(BM::Stream & rkPacket, CEL::DB_QUERY_TRAN & kContItemQuery);
	void ProcessModify_End(SProcessModifyResult const& kResult, EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
		BM::Stream const &kAddonPacket, BM::Stream & rkPacket);

	HRESULT ProcessUnEquipPet( PgDoc_Player *pkDocPlayer, BM::Stream &kPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr &kContLogMgr );
	HRESULT ProcessModifyPetAbil( PgDoc_Player *pkDocPlayer, SPMOD_AddAbilPet &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr &kContLogMgr );

	bool const BuildLogType(EItemModifyParentEventType const kCause,PgDoc_Player * const pkPlayer, PgContLogMgr & kLogContMgr, int const iPlayerIndex = 0, BM::Stream const* pkAddonPacket = NULL);

	void Remove_Home_Unit(SRealmGroundKey const & kGndKey);
	void Reset_MyHome_Unit(PgDoc_MyHome * pkMyHome);

	CLASS_DECLARATION(size_t, m_nMaxPlayerCount, MaxPlayerCount );
	HRESULT CashResult(SProcessModifyResult const& rkResult)
	{
		if (rkResult.hResult == S_OK)
		{
			return S_OK;
		}
		if (rkResult.hFailProcessResult == E_ADDED_SYSTEM_INVEN)
		{
			return S_OK;
		}
		return rkResult.hResult;
	}
	
protected :
	// Send 관련 함수
	void SendWarnMessage( BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid);
	void SendWarnMessage2( BM::GUID const &rkCharGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid );
	bool SendToUser(std::wstring const &kStrNick, BM::Stream const &rkPacket);
	bool SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid = true/* false 는 캐릭터guid */);
	bool SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid);
	bool SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap);
	bool SendToSwitch(BM::GUID const &kMemberGuid, BM::Stream const &Packet)const;

	__int64 _UpdatePlayTime( CONT_CENTER_PLAYER_BY_KEY::mapped_type pData );
	void _ClearSelectCharacter( CONT_CENTER_PLAYER_BY_KEY::mapped_type pkSelectedPlayerData );
	//void SaveMapContents( PgDoc_Player * const pkDocPlayer ) const;

	bool GetInfo(SModifyOrderOwner const& rkOrderOwner, SCenterPlayer_Lock& rkLockInfo, bool const bMemberGuid); // PgRealmUserManager 전용 (Out: Player, Guild ...)
	bool GetInfo(SModifyOrderOwner const& rkOrderOwner, SCenterPlayer_Lock& rkLockInfo, bool const bMemberGuid) const;

	bool GetPlayerInfo(BM::GUID const &rkGuid, bool const bMemberGuid, SCenterPlayer_Lock& rkLockInfo);
	bool GetPlayerInfo(BM::GUID const &rkGuid, bool const bMemberGuid, SCenterPlayer_Lock& rkLockInfo) const;
	bool GetPlayerInfo(std::wstring const &rkCharName, SCenterPlayer_Lock& rkLockInfo);
	bool GetPlayerInfo(std::wstring const &rkCharName, SCenterPlayer_Lock& rkLockInfo) const;
	bool IsLoginPlayer( BM::GUID const &rkGuid, bool const bMemberGuid )const;
	bool EnterEvent(BM::GUID const & kCharGuId);
	void LeaveEvent(BM::GUID const & kCharGuId);

	//
	bool GetGuildInfo(BM::GUID const &rkGuid, SCenterPlayer_Lock& rkLockInfo) const;

protected:
	bool RemovePlayer(CONT_CENTER_PLAYER_BY_KEY::mapped_type pkCPD);
	bool GetOfflineInfo(SModifyOrderOwner const& rkOrderOwner, SCenterPlayer_Lock& rkLockInfo);
	void RemoveOfflinePlayer(CONT_CENTER_PLAYER_BY_KEY::key_type kPlayerKey);
	HRESULT ProcessSelectPlayer(PgDoc_Player *pkPlayer, BM::GUID const &kSelectGuid, bool const bPassTutorial);
	DWORD OnContentsActionEvent(ContentsActionEvent const &rkEvent, BM::Stream * const pkAddonPacket);
	//DWORD OnContentsEvent(SActionOrder const *pkActionOrder);
	//void OnUpdateMissionReport(SActionOrder const *pkActionOrder);
	//void OnUserEnterGround(BM::Stream* const pkPacket);
	DWORD OnGMKickUser(BM::Stream* const pkPacket);
	void OnEnterGround(UNIT_PTR_ARRAY const &rkUnitArray, SRealmGroundKey const &rkGroundKey) const;

private:

	Loki::Mutex		m_kEventMutex;
	CONT_EVENT_USER m_kContEventUser;

	CONT_CENTER_PLAYER_BY_KEY m_kContPlayer_MemberKey;// <MemberGuid, SCentPlayerData*>
	CONT_CENTER_PLAYER_BY_ID m_kContPlayer_MemberID;// <AccountID, SCentPlayerData*>

	// Character정보로 Hash -> Character 안고르면, 없을 수도 있다.
	CONT_CENTER_PLAYER_BY_KEY m_kContPlayer_CharKey;// <CharacterGuid, SCentPlayerData*>
	CONT_CENTER_PLAYER_BY_ID m_kContPlayer_CharName;//<CharacterName, SCentPlayerData*>

	// In some order we have a problem when a user logout and not all order is complete,
	// so we need copy user after logout and complete all order
	CONT_CENTER_PLAYER_BY_KEY m_kContPlayerOffline_CharKey;// <CharacterGuid, SCentPlayerData*>

	CONT_PLAYER_WAITER	m_kContPlayerWaiter;//플레이어 지워지기전.
	ESERVER_STATUS m_eServerStatus;

	// 기타 유사 Player 정보(ex: Guild)
	CONT_CENTER_PLAYER_BY_KEY m_kContPlayer_Guild;

	//VECTOR_REMOVE_WAITER m_kRemoveWaiter;	// SCenterPlayerData 삭제 대기자


	typedef struct tagSafePlayer
	{
	private:
		// 복사생성자와 =은 절대로 호출하면 안된다.
		tagSafePlayer( tagSafePlayer const &rhs ):	pkPlayer(NULL),	pkContWaiter(NULL){}
		tagSafePlayer& operator=( tagSafePlayer const &rhs ){return *this;}

	public:
		tagSafePlayer()
			:	pkPlayer(NULL),	pkContWaiter(NULL)
		{}

		~tagSafePlayer()
		{
			if(pkPlayer)
			{
				if(pkContWaiter)
				{//되돌려주기.
					pkContWaiter->insert(std::make_pair(pkPlayer->GetID(), pkPlayer));
				}
				else
				{
					SAFE_DELETE(pkPlayer);
				}
			}
		}

		void SetPlayer(PgDoc_Player* const pkInPlayer, CONT_PLAYER_WAITER * const pkInCont = NULL)
		{///.. 여기서 컨테이너로 어떻게 돌려줘???
			pkPlayer= pkInPlayer;
			pkContWaiter = pkInCont;
		}

		PgDoc_Player* operator ()()
		{
			return pkPlayer;
		}

	protected:
		PgDoc_Player* pkPlayer;
		CONT_PLAYER_WAITER *pkContWaiter;//
	}SSafePlayer;

/*==================================================================================================================================================
																오픈 마켓 코드 이동 
====================================================================================================================================================*/

public:

	void Locked_InsertMarket(PgOpenMarket const & kUserMarket);
	void Locked_Tick();

	void Locked_RecvPT_M_I_UM_REQ_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPakcet);
	void Locked_RecvPT_M_I_UM_REQ_MINIMUM_COST_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MY_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MY_VENDOR_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_VENDOR_ENTER(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_VENDOR_REFRESH_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MARKET_ARTICLE_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_BEST_MARKET_LIST(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvORDER_MARKET_CLOSE(BM::GUID const & kGuid);
	EUserMarketResult Locked_OpenMarketVerifyArticlePrice(BM::GUID const& kBuyerGuid, BM::GUID const& kMarketGuid, WORD const wBuyCount, BM::GUID const& kArticleGuid,
		PgOpenMarketMgr::SCostInfo& rkCost) const;
	EUserMarketResult Locked_Cash_Article_Reg(PgTranPointer const& rkTran);
	bool Locked_IsOpenMarketSameGrade(BM::GUID const& kOwnerGuid, int const iGrade)const;


	// STATIC 함수들.................... (STATIC 제거 하지 말것 )))))))))))))))))))))))))))))))))
	//	LOCK 기능이 없으므로 
	static void RecvPT_M_I_UM_REQ_MARKET_OPEN(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	static void RecvPT_M_I_UM_REQ_ARTICLE_REG(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPakcet);
	static void RecvPT_M_I_UM_REQ_ARTICLE_DEREG(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPakcet);
	static void RecvPT_M_I_UM_REQ_DEALINGS_READ(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	static void RecvPT_M_I_UM_REQ_USE_MARKET_MODIFY_ITEM(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	static void RecvPT_M_I_UM_REQ_MARKET_MODIFY_STATE(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	static void RecvPT_M_I_UM_REQ_ARTICLE_BUY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPakcet);
	static void RecvPT_M_I_UM_REQ_MARKET_CLOSE(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	static bool ProcessItemQuery( CEL::DB_DATA_ARRAY::const_iterator &db_itr, CEL::DB_DATA_ARRAY::const_iterator const db_end_itr, CEL::DB_RESULT_COUNT::const_iterator &count_itr, PgInventory &rkInven, CONT_SELECTED_CHARACTER_ITEM_LIST *pOutItemList = NULL );
public:

/*==================================================================================================================================================
																캐릭터 카드 정보
====================================================================================================================================================*/

	void Locked_RecvPT_M_I_REQ_SEARCH_MATCH_CARD(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_REQ_CHARACTER_CARD_INFO(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

/*==================================================================================================================================================
																마이홈 정보
====================================================================================================================================================*/
	void Locked_Add_Home_Unit(BM::GUID const & kGuid,SMYHOME const kHome, CONT_HOME_ITEM const & kContItem, BM::Stream const & kAddonPacket);
	void Locked_Release_Home_Unit(BM::GUID const & kHomeGuid);

	void RecvMyHomePacketHandler(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void RecvTaskProcessMyHome(BM::Stream * const pkPacket);

	bool Q_DQT_LOAD_MYHOME_INFO(CEL::DB_RESULT &rkResult);

	bool Q_DQT_MYHOME_REMOVE(CEL::DB_RESULT &rkResult);

/*==================================================================================================================================================
																피로도 정보
====================================================================================================================================================*/
	void Locked_RecvTaskProcessPlayerPlayTime(WORD const usType, BM::Stream * const pkPacket);
protected:
	void ResetPlayerPlayTime();
	bool SetPlayerPlayTime(SCenterPlayerData * pkData, int const iAccConSec, int const iAccDicSec);

/*==================================================================================================================================================
																채팅 블럭(차단).
====================================================================================================================================================*/

public:

	void Locked_RegistChatBlock(BM::Stream * const pPacket);
	void Locked_UnRegistChatBlock(BM::Stream * const pPacket);
	void Locked_ChangeChatBlockOption(BM::Stream * const pPacket);

private:

	PgOpenMarketMgr		m_kOpenMarketMgr;
	PgCharacterCardMgr	m_kCardMgr;

private:

	PgMyHomeMgr			m_kMyHomeMgr;

	PgGambleUserMgr		m_kGambleUserMgr;

	PgGambleUserMgr		m_kMixupUserMgr;

private:
	size_t				m_iMyHomeUnitCount;

	HRESULT OnProcessDBAddAbil64(PgDoc_Player * pkCopyPlayer, int const iAbilType, __int64 const i64AbilValue, PgContLogMgr & rkContLogMgr, DB_ITEM_STATE_CHANGE_ARRAY & kChangeArray, bool& bIsQuery);
	HRESULT OnProcessAhievement2Inv(SPMO const &kOrder, PgDoc_Player * pkCopyPlayer, DB_ITEM_STATE_CHANGE_ARRAY & kChangeArray, PgContLogMgr & kContLogMgr);
	void ProcessInventoryTimeOut(PgDoc_Player *pkPlayer);
	void UpdateAchievementPoint(PgDoc_Player * pkPlayer);
    void ProcessEmporiaReserve(HRESULT const hRet, BM::Stream kAddonPacket, BYTE byErrorType = (BYTE)GCR_Money);

/*==================================================================================================================================================
																캐선창 캐쉬 슬롯
====================================================================================================================================================*/
public:
	bool Locked_Q_DQT_FIND_CHARACTOR_EXTEND_SLOT(CEL::DB_RESULT &rkResult);

/*==================================================================================================================================================
																프리미엄 서비스
====================================================================================================================================================*/
public:
	bool Locked_Q_DQT_GET_MEMBER_PREMIUM_SERVICE(CEL::DB_RESULT &rkResult);
	void Locked_OnTick_Premium();

public:
	void Locked_Q_DQT_GET_MEMBER_DAILY_REWARD(CEL::DB_RESULT &rkResult);

public:
	void Locked_OnTick_SpecificReward_Event();

private:
	void WriteToPacket_MEMBER_PREMIUM_SERVICE(BM::GUID const& kMemberID, BM::Stream & rkPacket)const;
	void SetPrimiumService(PgDoc_Player * pkPlayer);

	CONT_MEMBER_PREMIUM m_kContMemberPremium;

/*====================================================================================================================================*/

	CONT_EVENTREWARD_TIMEINFO m_ContEventRewardTimeInfo;
};

#define g_kRealmUserMgr SINGLETON_STATIC(PgRealmUserManager)

#endif // CONTENTS_CONTENTSSERVER_SERVERSET_PGSERVERSETMGR_H