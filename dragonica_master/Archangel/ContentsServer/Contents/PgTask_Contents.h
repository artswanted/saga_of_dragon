#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGTASK_CONTENTS_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGTASK_CONTENTS_H

#include "BM/PgTask.h"
#include "Variant/PgCouponEventView.h"
#include "Variant/PgEventQuestBase.h"
#include "Variant/PgBattleSquare.h"
#include "PgEventQuestSystem.h"
#include "PgCoupleMgr.h"
#include "PgFriendMgr.h"
#include "PgNotice.h"
#include "PgOXQuizEvent.h"
#include "PgLuckyStarEvent.h"
#include "PgRankMgr.h"
#include "PgBSGame.h"
#include "PgCouponEventDoc.h"
#include "PgRealmEvent.h"
#include "PgPvPLeagueMgr.h"
#include "PgExpeditionListMgr.h"

class PgTask_RealmContents
	:public PgTask<>
{
public:
	PgTask_RealmContents();
	virtual ~PgTask_RealmContents();

public:
	virtual void HandleMessage(SEventMessage *pkMsg);
	void Start();
	void Close();

	void Timer1m();
	void Timer1s();
	void Timer10s();
	bool Build(CONT_DEF_MISSION_RESULT const &rkResult, CONT_DEF_MISSION_CANDIDATE const &rkCandi, 
		CONT_DEF_MISSION_ROOT const &rkRoot);	// m_kRankMgr build
	void Build(CONT_COUPON_EVENT_INFO const & kContEventInfo,CONT_COUPON_EVENT_REWARD const & kContEventReward); // m_kCouponEvnetDoc
	bool ParseXml();

private:
	PgCoupleMgr				m_kCoupleMgr;
	PgFriendMgr				m_kFriendMgr;
	PgNoticeSystem_Wrapper	m_kNoticeSystem;
	PgOXQuizEvent			m_kOXQuizEvent;
	PgLuckyStarEvent		m_kLuckyStarEvent;
	PgRankMgr				m_kRankMgr;
	PgCouponEventDoc		m_kCouponEventDoc;
	PgCouponEventView		m_kCouponEventView;
	PgEventQuestSystem		m_kEventQuest;
	PgBSGameMng				m_kBSGameMng;
	PgRealmEventMng			m_kRealmEventMng;
	PgPvPLeagueMgr			m_kPgPvPLeagueMgr;

private:
	
	CLASS_DECLARATION_S(__int64,LastUpdateAchievementRankTime);
	void UpdateAchievementRankView();

	CLASS_DECLARATION_S(BM::PgPackedTime, MisionRankClearTime);
	void CreateMissionRankClearTime();
	void UpdateMissionRankClearCheck();

	CLASS_DECLARATION_S(BM::PgPackedTime, StrategyFatigabilityInitTime);
	void CreateStrategyFatigabilityInitTime();	// 전략피로도 초기화 시간.
	void UpdateStrategyFatigabilityCheck(); // 전략피로도 초기화 체크.
};

#define g_kContentsTask SINGLETON_STATIC(PgTask_RealmContents)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGTASK_CONTENTS_H