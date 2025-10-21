#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGCOUPLEMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGCOUPLEMGR_H

#include "PgServerSetMgr.h"
#include "JobDispatcher.h"

typedef struct tagCoupleInfo : public SCouple
{
	tagCoupleInfo(BM::GUID const &rkWhoGuid);
	void Accept(SContentsUser const &rkHim, BM::PgPackedTime const &rkNow, __int64 const iLimitTime);
	void Request(BM::GUID const &rkRequesterGuid);
	void CoupleLimitTime(BM::PgPackedTime const &rkLimitDate);
	void SweetHeartPanalty(BM::PgPackedTime const &rkNow);
} SCoupleInfo;

class PgCoupleMatcher
{
	typedef CONT_CENTER_PLAYER_BY_KEY TargetType;
	typedef std::pair< TargetType::key_type, TargetType::mapped_type > ContPair;

public:
	PgCoupleMatcher(size_t const iMaxCount, int const iLevelDiff, SContentsUser const &rkUser, ContContentsUser &rkOut, bool const bMinLevel = true);
	bool operator () (ContPair const &rkPair) const;

private:
	short m_sCurChannel;
	size_t const m_iMaxCount;
	int m_iTargetGender;
	int const m_iLevelDiff;
	int const m_iCurLevel;
	ContContentsUser &m_kOut;

	CONT_DEFMAP const* m_pkContDefMap;
};

//
class PgCoupleMgr
{	
	static int const iCoupleSkillNo[];// = {90000001, 100006301, 100006401, 100006501};
	typedef std::map< BM::GUID, SCoupleInfo > ContCouple;
	typedef ContCouple::iterator iterator;
	typedef ContCouple::const_iterator const_iterator;
	typedef ContCouple::key_type key_type;
	typedef ContCouple::mapped_type mapped_type;
	typedef std::list<int> COUPLE_SKILL_LIST;
public:
	PgCoupleMgr();
	~PgCoupleMgr();
	bool ProcessMsg(SEventMessage *pkMsg);
	void Tick();
public:
	static int iCoupleUse;
	static __int64 iPanaltyDay;
	static bool InitConstantValue(void);	
protected:

	bool ProcessCommand(BM::GUID const &rkCharGuid, BYTE const cCommand, BM::Stream &rkPacket);
	bool ProcessTCommand(BM::Stream &rkPacket);
	ECoupleCommandResult AnsCouple(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult ReqFind(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult BreakCouple(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult ReqCouple(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult ReqInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult ReqWarp(BM::GUID const &rkCharGuid, BM::Stream &rkPacket, BYTE const cCmdType);
	ECoupleCommandResult ReqSweetHeartQuest(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult AnsSweetHeartQuest(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult SweetHeartQuest_Cancel(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult SweetHeartQuestTimeInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult SweetHeartQuestComplete(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult ReqSweetHeartComplete(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult AnsSweetHeartComplete(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult SendGndCoupleStatusInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult CoupleSkillInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	ECoupleCommandResult ReqCoupleInit(BM::GUID const &rkCharGuid);
	void SweetHeartPanalty(BM::GUID const &rkCharGuid, BM::GUID const &rkGuid);

	inline bool Add(key_type const &rkKey, BM::GUID const &rkWhoGuid)
	{
		auto kRet = m_kMap.insert( std::make_pair(rkKey, mapped_type(rkWhoGuid)) );
		return kRet.second;
	}

	inline bool Check(key_type const &rkKey)
	{
		return m_kMap.end() != m_kMap.find(rkKey);
	}

	inline void Del(key_type const &rkKey)
	{
		m_kMap.erase(rkKey);
	}

	inline mapped_type* Get(key_type const &rkKey)
	{
		iterator iter = m_kMap.find(rkKey);
		if( m_kMap.end() == iter )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}
		return &(*iter).second;
	}

	inline mapped_type* GetMyInfo(key_type const &rkKey, BM::GUID& rkOutGuid)
	{
		iterator iter = m_kMap.begin();
		while( m_kMap.end() != iter )
		{
			if( (*iter).second.CoupleGuid() == rkKey )
			{
				rkOutGuid = (*iter).first;
				return &(*iter).second;
			}
			
			++iter;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	inline void RejectCouple(SContentsUser const &rkUser, mapped_type &rkCoupleInfo)
	{
		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_Couple);
		kPacket.Push( (BYTE)CoupleCR_Reject );
		kPacket.Push( rkUser.kName );								// 누구로 부터
		g_kRealmUserMgr.Locked_SendToUser(rkCoupleInfo.CoupleGuid(), kPacket, false);		// 거절 당했으

		mapped_type *pkRequesterCoupleInfo = Get(rkCoupleInfo.CoupleGuid());
		if( pkRequesterCoupleInfo )
		{
			pkRequesterCoupleInfo->Clear();
		}
		rkCoupleInfo.Clear();
	}

	inline void RejectCouplePanalty(mapped_type &rkCoupleInfo)
	{
		mapped_type *pkRequesterCoupleInfo = Get(rkCoupleInfo.CoupleGuid());
		if( pkRequesterCoupleInfo )
		{
			pkRequesterCoupleInfo->Clear();
		}
		rkCoupleInfo.Clear();
	}

	inline void SendCoupleInfo(SContentsUser const &rkUser)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Info);
		kPacket.Push( (BYTE)CoupleCR_Success );
		pkCoupleInfo->WriteToPacket( kPacket );
		g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kPacket);
	}

	inline void SendCoupleInfo(BM::GUID const &rkCharGuid)
	{
		BM::GUID kOwnerGuid;
		mapped_type *pkCoupleInfo = GetMyInfo(rkCharGuid, kOwnerGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Info);
		kPacket.Push( (BYTE)CoupleCR_Success );
		pkCoupleInfo->WriteToPacket( kPacket );
		g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid, kPacket, false);
	}

	inline void ReqDBCoupleInfo(SContentsUser const &rkUser)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}
		BM::Stream kPacket(PT_N_T_REQ_COUPLE_COMMAND, (BYTE)CC_Req_Info);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkUser.kMemGuid );
		kPacket.Push( rkUser.kCharGuid );
		SendToPacketHandler(kPacket);
	}

	inline void AddCoupleSkill(SContentsUser const &rkUser)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return;
		}

		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Nfy_AddSkill);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkUser.kCharGuid );
		COUPLE_SKILL_LIST kStack;
		int i = 0;
		while(iCoupleSkillNo[i])
		{
			kStack.push_back(iCoupleSkillNo[i]);
			++i;
		}

		PU::TWriteArray_A(kPacket, kStack);
		
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kMemGuid, kPacket, true, true);
	}

	inline void AddSweetHeartSkill(SContentsUser const &rkUser)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return;
		}

		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Nfy_AddSkill);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkUser.kCharGuid );
		COUPLE_SKILL_LIST kStack;
		int BaseSkill = iCoupleSkillNo[0];
		for(int i=BaseSkill; i <= pkCoupleInfo->LearnSkill(); ++i)
		{
			if( (BaseSkill+3) > i ) // 커플스킬 최대 3개밖에 음써
			{
				if( i >= BaseSkill )
				{
					kStack.push_back(i);
				}
			}
		}
		PU::TWriteArray_A(kPacket, kStack);
		
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kMemGuid, kPacket, true, true);
	}

	inline void SaveCoupleInfo(key_type const &rkKey, mapped_type *pkCoupleInfo, BYTE cStatus = 0)
	{
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		static BM::PgPackedTime const kEmptyTime;
		bool const bEmptyStartTime = kEmptyTime == pkCoupleInfo->StartDate();
		bool const bEmptyLimitTime = kEmptyTime == pkCoupleInfo->LimitDate();

		BYTE cSaveStatus = CoupleSI_None;
		if( !bEmptyStartTime )						{ cSaveStatus |= CoupleSI_StartTime; }
		if( !bEmptyLimitTime && !bEmptyStartTime )	{ cSaveStatus |= CoupleSI_LimitTime; }

		BM::Stream kPacket(PT_N_T_REQ_COUPLE_COMMAND, (BYTE)CC_Sys_Save);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkKey );
		kPacket.Push( pkCoupleInfo->CoupleGuid() );
		kPacket.Push( cStatus );
		kPacket.Push( cSaveStatus );
		if( !bEmptyStartTime )							{ kPacket.Push( pkCoupleInfo->StartDate() ); }
		if( !bEmptyLimitTime && !bEmptyLimitTime )		{ kPacket.Push( pkCoupleInfo->LimitDate() ); }
		SendToPacketHandler(kPacket);
	}

	inline void SaveDBSweetHeartInfo(key_type const &rkKey, mapped_type *pkCoupleInfo, BM::PgPackedTime const &rkNow = BM::PgPackedTime())
	{
		int					Quest = 0;
		__int64				SweetHeartQuestTime = 0;
		int					LearnSkill = 0;
		BM::PgPackedTime	CouplePanaltyLimitDate;
		BM::GUID			ColorGuid = BM::GUID::NullData();

		if( pkCoupleInfo )
		{
			Quest = pkCoupleInfo->Quest();
			SweetHeartQuestTime = pkCoupleInfo->SweetHeartQuestTime();
			LearnSkill = pkCoupleInfo->LearnSkill();
			CouplePanaltyLimitDate = pkCoupleInfo->CouplePanaltyLimitDate();
			pkCoupleInfo->SweetHeartQuestTimeGap( 0 );
			ColorGuid = pkCoupleInfo->ColorGuid();
		}
		else
		{
			if( !(CouplePanaltyLimitDate == rkNow) )
			{
				CouplePanaltyLimitDate = rkNow;
			}
		}

		BM::Stream kPacket(PT_N_T_REQ_COUPLE_COMMAND, (BYTE)CC_SweetHeartQuestTimeInfo);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkKey );
		kPacket.Push( Quest );
		kPacket.Push( SweetHeartQuestTime );
		kPacket.Push( LearnSkill );
		kPacket.Push( CouplePanaltyLimitDate );
		kPacket.Push( ColorGuid );
		SendToPacketHandler(kPacket);
	}

	inline void NotifyBreaked(SContentsUser const &rkUser)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}
		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return;
		}
		if( !pkCoupleInfo->Breaked() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleBreaked is false"));
			return;
		}
		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Break);
		kPacket.Push( (BYTE)CoupleCR_Notify );
		kPacket.Push( rkUser.kCharGuid );
		kPacket.Push( pkCoupleInfo->CharName() );
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kMemGuid, kPacket, true, true);

		pkCoupleInfo->Clear();

		SaveCoupleInfo(rkUser.kCharGuid, pkCoupleInfo, CoupleS_Normal);
		SetCoupleQuest(rkUser, true, 0, 0);
		SaveDBSweetHeartInfo(rkUser.kCharGuid, NULL);
	}

	inline void NotifyInfo(SContentsUser const &rkUser)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}
		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return;
		}
		if( pkCoupleInfo->Breaked() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleBreaked is false"));
			return;
		}

		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Info);
		kPacket.Push( (BYTE)CoupleCR_Notify );
		kPacket.Push( rkUser.kCharGuid );
		kPacket.Push( pkCoupleInfo->cStatus() );
		kPacket.Push( pkCoupleInfo->ColorGuid() );
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kMemGuid, kPacket, true, true);

		// 커플 업적
		if( (CoupleS_Normal | CoupleS_SweetHeart) & pkCoupleInfo->cStatus() )
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->InsertTarget(rkUser.kCharGuid);
			pkActionOrder->kCause = CAE_Achievement;
			pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL,rkUser.kCharGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_COUPLE_START_DATE,pkCoupleInfo->StartDate().GetTime())));
			g_kJobDispatcher.VPush(pkActionOrder);
		}
	}

	inline bool CheckLimitTime(key_type const &rkCharGuid)
	{
		mapped_type *pkCoupleInfo = Get(rkCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return false; // 커플 정보더 없어 ㅠㅠ
		}
		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return false; // 커플 없어 ㅠ_ㅠ
		}

		static BM::PgPackedTime const kNullTime;
		if( kNullTime == pkCoupleInfo->LimitDate() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleLimitData is NullTime"));
			return false; // 무한 커플이야
		}

		BM::PgPackedTime kNow = BM::PgPackedTime::LocalTime();
		bool const bLimited = kNow < pkCoupleInfo->LimitDate();	// 기간이 지났다?
		if( bLimited )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleLimitDate is True"));
			return false; // 안지났다
		}
		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Nfy_TimeLimit);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkCharGuid );
		kPacket.Push( pkCoupleInfo->CharName() );
		g_kRealmUserMgr.Locked_SendToUserGround(rkCharGuid, kPacket, false, true);
		
		pkCoupleInfo->Clear();
		SaveCoupleInfo(rkCharGuid, pkCoupleInfo, CoupleS_Normal);
		SaveDBSweetHeartInfo(rkCharGuid, NULL);
		return true; // 유통기간 지났어
	}

	inline void SendSweetHeartQuestTime(SContentsUser const &rkUser, ECoupleCommandResult eType)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return;
		}

		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_SweetHeartQuestTimeInfo);
		kPacket.Push( (BYTE)eType );
		kPacket.Push( pkCoupleInfo->SweetHeartQuestTime() );
		g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kPacket);
	}

	inline void SendSweetHeartQuestTime(key_type const &rkCharGuid, ECoupleCommandResult eType)
	{
		SContentsUser kUser;
		if( S_OK != ::GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! SContentsUser is NULL"));
			return;
		}

		SendSweetHeartQuestTime(kUser, eType);
	}

	inline void UserCoupleInfoTime(mapped_type &rkMyCoupleInfo, mapped_type &rkYouCoupleInfo)
	{
		mapped_type *pkMyCoupleInfo = Get(rkMyCoupleInfo.CoupleGuid());
		if( !pkMyCoupleInfo )
		{
			return;
		}

		mapped_type *pkYouCoupleInfo = Get(rkYouCoupleInfo.CoupleGuid());
		if( !pkYouCoupleInfo )
		{
			return;
		}

		int const iDiff = abs(static_cast<int>(pkMyCoupleInfo->SweetHeartQuestTime() - pkYouCoupleInfo->SweetHeartQuestTime()));
		if( 3 > iDiff )
		{
			return;
		}

		if( (0 < pkMyCoupleInfo->SweetHeartQuestTime()) && (0 < pkYouCoupleInfo->SweetHeartQuestTime()) )
		{
			if( pkMyCoupleInfo->SweetHeartQuestTime() != pkYouCoupleInfo->SweetHeartQuestTime() )
			{
				pkMyCoupleInfo->SweetHeartQuestTimeGap( 0i64 );
				pkYouCoupleInfo->SweetHeartQuestTimeGap( 0i64 );

				__int64 const iMyInfo = pkMyCoupleInfo->SweetHeartQuestTime();
				__int64 const iYouInfo = pkYouCoupleInfo->SweetHeartQuestTime();
				if( iMyInfo > iYouInfo )
				{
					pkMyCoupleInfo->SweetHeartQuestTime(iYouInfo);
					SendSweetHeartQuestTime(pkYouCoupleInfo->CoupleGuid(), CoupleCR_Success);
				}
				else
				{
					pkYouCoupleInfo->SweetHeartQuestTime(iMyInfo);
					SendSweetHeartQuestTime(pkMyCoupleInfo->CoupleGuid() , CoupleCR_Success);
				}
			}
		}
	}

	inline bool CheckCoupleGround(key_type const &rkCharGuid)
	{
		SContentsUser kUser;
		if( S_OK != ::GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! SContentsUser is NULL"));
			return false;
		}

		mapped_type *pkMyCoupleInfo = Get(kUser.kCharGuid);
		if( !pkMyCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return false; // 커플 정보더 없어 ㅠㅠ
		}
		if( BM::GUID::IsNull(pkMyCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return false; // 커플 없어 ㅠ_ㅠ
		}

		SContentsUser kCoupleUser;
		if( S_OK != ::GetPlayerByGuid(pkMyCoupleInfo->CoupleGuid(), false, kCoupleUser) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! SContentsUser is NULL"));
			return false;
		}

		mapped_type *pkYouCoupleInfo = Get(kCoupleUser.kCharGuid);
		if( !pkYouCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return false; // 커플 정보더 없어 ㅠㅠ
		}
		if( BM::GUID::IsNull(pkYouCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return false; // 커플 없어 ㅠ_ㅠ
		}

		if( 0 != pkMyCoupleInfo->QuestID()
		&&	0 != pkYouCoupleInfo->QuestID()
		&&	pkMyCoupleInfo->QuestID() == pkYouCoupleInfo->QuestID()
		&&	kUser.kGndKey.GroundNo() == kCoupleUser.kGndKey.GroundNo() )
		{
			return true;
		}
		return false;
	}

	inline bool CheckLimitTimeQuest(key_type const &rkCharGuid)
	{
		SContentsUser kUser;
		if( S_OK != ::GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! SContentsUser is NULL"));
			return false;
		}

		mapped_type *pkMyCoupleInfo = Get(kUser.kCharGuid);
		if( !pkMyCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return false; // 커플 정보더 없어 ㅠㅠ
		}
		if( BM::GUID::IsNull(pkMyCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return false; // 커플 없어 ㅠ_ㅠ
		}

		SContentsUser kCoupleUser;
		if( S_OK != ::GetPlayerByGuid(pkMyCoupleInfo->CoupleGuid(), false, kCoupleUser) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! SContentsUser is NULL"));
			return false;
		}

		mapped_type *pkYouCoupleInfo = Get(kCoupleUser.kCharGuid);
		if( !pkYouCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return false; // 커플 정보더 없어 ㅠㅠ
		}
		if( BM::GUID::IsNull(pkYouCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return false; // 커플 없어 ㅠ_ㅠ
		}

		// 커플 협동시간 똑같이 마춘다.
		UserCoupleInfoTime(*pkMyCoupleInfo, *pkYouCoupleInfo);

		// 커플이 같이 있는 경우만 시간 빼준다.
		if( 0 != pkMyCoupleInfo->QuestID()
		&&	0 != pkYouCoupleInfo->QuestID()
		&&	pkMyCoupleInfo->QuestID() == pkYouCoupleInfo->QuestID()
		&&	kUser.kGndKey.GroundNo() == kCoupleUser.kGndKey.GroundNo() )
		{
			__int64 const iPlayTime = pkMyCoupleInfo->SweetHeartQuestTimeGap();
			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::SECOND );
			if( 0 < iPlayTime )
			{
				__int64 const iPlayTimeGap = std::max(i64NowTime - iPlayTime, 0i64);
				if( 0 < iPlayTimeGap )
				{
					pkMyCoupleInfo->SweetHeartQuestTime( std::max( (pkMyCoupleInfo->SweetHeartQuestTime() - iPlayTimeGap), 0i64 ) );
					pkMyCoupleInfo->SweetHeartQuestTimeGap( i64NowTime );
				}

				if( false == pkMyCoupleInfo->SweetHeartQuestUpdate() )
				{
					pkMyCoupleInfo->SweetHeartQuestUpdate( true );
					SendSweetHeartQuestTime(kUser, CoupleCR_Success);
				}

				if( 0 >= pkMyCoupleInfo->SweetHeartQuestTime() )
				{
					CompleteCoupleQuestTime(kUser);
					CompleteCoupleQuestTime(kCoupleUser);
					return true;
				}
			}
			else
			{
				if( 0 < pkMyCoupleInfo->SweetHeartQuestTime() )
				{
					pkMyCoupleInfo->SweetHeartQuestTimeGap( i64NowTime );
				}
			}
		}
		else
		{
			if( true == pkMyCoupleInfo->SweetHeartQuestUpdate() )
			{
				pkMyCoupleInfo->SweetHeartQuestTimeGap( 0 );
				pkMyCoupleInfo->SweetHeartQuestUpdate( false );
				SendSweetHeartQuestTime(kUser,	CoupleCR_Failed);
			}
		}

		return false;
	}

	inline void CompleteCoupleQuestTime(SContentsUser const& rkUser)
	{
		// 연인 퀘스트 조건 완료 했다.
		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_SweetHeartQuest_Complete);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkUser.kCharGuid );
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kCharGuid, kPacket, false, true);

		SendSweetHeartQuestTime(rkUser, CoupleCR_Success);
		SetCoupleQuest(rkUser, true, 0, 0); // 달성 했다 초기화 하자
	}

	inline void RejectSweetHeartQuest(SContentsUser const &rkUser, mapped_type &rkCoupleInfo)
	{
		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_SweetHeart_Quest);
		kPacket.Push( (BYTE)CoupleCR_Reject );
		kPacket.Push( rkUser.kName );								// 누구로 부터
		g_kRealmUserMgr.Locked_SendToUser(rkCoupleInfo.CoupleGuid(), kPacket, false);		// 거절 당했으
	}

	inline void RejectSweetHeartComplete(SContentsUser const &rkUser, mapped_type &rkCoupleInfo)
	{
		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_SweetHeart_Complete);
		kPacket.Push( (BYTE)CoupleCR_Reject );
		kPacket.Push( rkUser.kName );								// 누구로 부터
		g_kRealmUserMgr.Locked_SendToUser(rkCoupleInfo.CoupleGuid(), kPacket, false);		// 거절 당했으

		/*mapped_type *pkRequesterCoupleInfo = Get(rkCoupleInfo.CoupleGuid());
		if( pkRequesterCoupleInfo )
		{
			pkRequesterCoupleInfo->Clear();
		}
		rkCoupleInfo.Clear();*/
	}

	inline bool SweetHeartQuestDelete(SContentsUser const &rkUser, mapped_type *pkCoupleInfo, int &iQuestValue)
	{
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return false;
		}

		bool bRet = false;

		if( 0 != pkCoupleInfo->Quest() )
		{
			iQuestValue = pkCoupleInfo->Quest();
			bRet = true;
		}
		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			iQuestValue = 0;
			bRet = true;
		}
		return bRet;
	}

	inline void SetCoupleQuest(SContentsUser const &rkUser, bool bInit, int const iQuestID, __int64 iPlayTime)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleGuid is NULL"));
			return;
		}

		if( false == bInit )
		{
			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::SECOND );

			pkCoupleInfo->QuestID( iQuestID );
			pkCoupleInfo->SweetHeartQuestTime( iPlayTime );
			pkCoupleInfo->SweetHeartQuestTimeGap( i64NowTime );
			pkCoupleInfo->SweetHeartQuestUpdate( false );

			SaveDBSweetHeartInfo(rkUser.kCharGuid, pkCoupleInfo);
		}
		else
		{
			pkCoupleInfo->QuestID( 0 );
			pkCoupleInfo->SweetHeartQuestTime( 0 );
			pkCoupleInfo->SweetHeartQuestTimeGap( 0 );
			pkCoupleInfo->SweetHeartQuestUpdate( false );
			SendSweetHeartQuestTime(rkUser, CoupleCR_None);
		}
		bool bRet = CheckLimitTimeQuest(rkUser.kCharGuid);
	}

	inline bool SweetHeartPanaltyCheck(mapped_type *pkCoupleInfo)
	{
		BM::PgPackedTime const kNullTime;
		if( !(kNullTime == pkCoupleInfo->CouplePanaltyLimitDate()) )
		{
			BM::PgPackedTime kNow = BM::PgPackedTime::LocalTime();
			bool const bLimited = kNow < pkCoupleInfo->CouplePanaltyLimitDate();
			if( bLimited )
			{
				return true;
			}
		}
		return false;
	}

	inline void CoupleStatusSave(SContentsUser const &rkUser, BYTE cStatus)
	{
		mapped_type *pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
			return;
		}

		pkCoupleInfo->cStatus( cStatus );

		SendCoupleInfo(rkUser);
	}

	inline void SendGndCoupleStatus(SContentsUser const& rkUser)
	{
		mapped_type* pkCoupleInfo = Get(rkUser.kCharGuid);
		if( !pkCoupleInfo )
		{
			return;
		}

		BM::Stream kStatusPacket(PT_N_M_ANS_COUPLE_COMMAND, static_cast< BYTE >(CC_NfyStatus));
		kStatusPacket.Push( static_cast< BYTE >(CoupleCR_Success) );
		kStatusPacket.Push( rkUser.kCharGuid );
		kStatusPacket.Push( pkCoupleInfo->cStatus() );
		kStatusPacket.Push( pkCoupleInfo->ColorGuid() );
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kMemGuid, kStatusPacket, true, true); // 바뀐 상태 알려
	}

	inline void GetCoupleColorGuid(BM::GUID const & kGuid, BM::GUID const & kCoupleGuid, BM::GUID & kCoupleColorGuid)
	{
		if(kCoupleGuid.IsNotNull())
		{//GUID 를  순방향, 역방향을 XOR로 만듬(시퀀셜 GUID 때문에. 순방, 역방을 섞어야함)
			BM::GUID const kOwnerGuid = kGuid;
			
			BYTE const *pkOwnerMem = reinterpret_cast<BYTE const*>(&kOwnerGuid);
			BYTE const *pkCoupleMem = reinterpret_cast<BYTE const*>(&kCoupleGuid)+sizeof(BM::GUID)-1;
			BYTE *pkRetMem = reinterpret_cast<BYTE *>(&kCoupleColorGuid);

			size_t i = 0;
			while(sizeof(BM::GUID) > i)
			{
				*(pkRetMem+i) = static_cast< BYTE >( static_cast<float>(*++pkOwnerMem ^ *--pkCoupleMem) );
				++i;
			}
		}
	}

private:
	Loki::Mutex m_kMutex;
	ContCouple m_kMap;	
};

//#define g_kCoupleMgr SINGLETON_STATIC(PgCoupleMgr)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGCOUPLEMGR_H