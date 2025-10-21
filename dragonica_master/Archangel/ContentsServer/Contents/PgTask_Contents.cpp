#include "StdAfx.h"
#include <set>
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Lohengrin/VariableContainer.h"
#include "PgGuildMgr.h"
#include "PgRecvFromServer.h"
#include "PgGMProcessMgr.h"
#include "RealmChatMgr.h"
#include "PgDBProcess.h"
#include "PgTask_Contents.h"
#include "PgEmporiaMgr.h"
#include "PgHardCoreDungeonSystemMgr.h"
#include "PgSocialDBUtil.h"
#include "PgStatTrackMgr.h"

PgTask_RealmContents::PgTask_RealmContents(void)
{
	LastUpdateAchievementRankTime(g_kEventView.GetLocalSecTime());
	CreateMissionRankClearTime();
	CreateStrategyFatigabilityInitTime();
}

PgTask_RealmContents::~PgTask_RealmContents(void)
{
}

__int64 const UPDATEACHIEVEMENTRANKVIEW_TIME = 60i64 * 60i64;//1시간에 한번

void PgTask_RealmContents::UpdateAchievementRankView()
{
	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

	if(LastUpdateAchievementRankTime() > i64CurTime)
	{
		return;
	}

	LastUpdateAchievementRankTime(i64CurTime+UPDATEACHIEVEMENTRANKVIEW_TIME);

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_UserAchievementRankView]");
	BM::GUID const kGuid = BM::GUID::Create();
	kQuery.InsertQueryTarget( kGuid );
	kQuery.QueryOwner( kGuid );
	g_kCoreCenter.PushQuery( kQuery );
}

void PgTask_RealmContents::CreateMissionRankClearTime()
{
	BM::PgPackedTime kLocalTime = BM::PgPackedTime::LocalTime();

	CGameTime::AddTime(kLocalTime, CGameTime::OneDay);
	kLocalTime.Hour(0), kLocalTime.Min(0), kLocalTime.Sec(0);

	m_kMisionRankClearTime = kLocalTime;
}

void PgTask_RealmContents::UpdateMissionRankClearCheck()
{
	BM::PgPackedTime kLocalTime = BM::PgPackedTime::LocalTime();

	if( m_kMisionRankClearTime < kLocalTime )
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_ClearMissionRank]");
		BM::GUID const kGuid = BM::GUID::Create();
		kQuery.InsertQueryTarget( kGuid );
		kQuery.QueryOwner( kGuid );
		kQuery.PushStrParam( -1 );
		g_kCoreCenter.PushQuery( kQuery );

		CreateMissionRankClearTime();
		
		BM::Stream kNPacket(PT_T_N_NFY_MISSION_RANK_CLEAR);
		SendToRankMgr(kNPacket);
	}
}

void PgTask_RealmContents::CreateStrategyFatigabilityInitTime()
{
	BM::PgPackedTime LocalTime = BM::PgPackedTime::LocalTime();

	int InitHour = 0, InitMin = 0, InitSec = 0;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Init_Time_HOUR, InitHour)
		|| S_OK != g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Init_Time_MIN, InitMin)
		|| S_OK != g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Init_Time_SEC, InitSec) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << "Can't Init Time, EVar_Init_Time_HOUR, EVar_Init_Time_MIN, EVar_Init_Time_SEC");
	}

	CGameTime::AddTime(LocalTime, CGameTime::OneDay);
	LocalTime.Hour(InitHour), LocalTime.Min(InitMin), LocalTime.Sec(InitSec);

	m_kStrategyFatigabilityInitTime = LocalTime;
}

void PgTask_RealmContents::UpdateStrategyFatigabilityCheck()
{
	BM::PgPackedTime LocalTime = BM::PgPackedTime::LocalTime();

	if( m_kStrategyFatigabilityInitTime < LocalTime )
	{
		int MaxStrategyFatigability = 0;
		if( S_OK != g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Max_StrategyFatigability, MaxStrategyFatigability) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << "Can't Init StrategyFatigability, [EVar_StrategyFatigability] can't find");
			return;
		}

		CEL::DB_QUERY Query(DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_InitializeStractgyFatigability]");
		BM::GUID const Guid = BM::GUID::Create();
		Query.InsertQueryTarget(Guid);
		Query.QueryOwner(Guid);
		Query.PushStrParam(MaxStrategyFatigability);

		g_kCoreCenter.PushQuery(Query);

		CreateStrategyFatigabilityInitTime();

		BM::Stream NfyPacket(PT_N_T_NFY_RECOVERY_STRATEGY_FATIGUABILITY);
		NfyPacket.Push(MaxStrategyFatigability);
		SendToChannel(-1, NfyPacket);	// 전 채널 BroadCast.
	}
}

void PgTask_RealmContents::Start()
{
	this->m_kNoticeSystem.Load();
	this->StartSvc();
}

void PgTask_RealmContents::Close()
{
	this->m_kNoticeSystem.Save();
	this->VDeactivate();
}

void PgTask_RealmContents::HandleMessage(SEventMessage *pkMsg)
{
	//INFO_LOG(BM::LOG_LV8, _T("START : PgTask_RealmContents::HandleMessage : Message Type[%d][%d]"), pkMsg->PriType(), pkMsg->SecType());

	switch(pkMsg->PriType())
	{
	case PMET_MYHOME:
		{
			g_kRealmUserMgr.RecvTaskProcessMyHome(pkMsg);
		}break;
	case PMET_SendToPacketHandler:
		{
			OnRecvFromServer(NULL, pkMsg);
		}break;
	//case PMET_PARTY:
	//	{
	//		bRet = g_kPartyMgr.ProcessMsg(pkMsg);
	//	}break;
	case PMET_RealmChat:
		{
			g_kRealmChatMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_FRIEND:
		{
			m_kFriendMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_GUILD:
		{
			g_kGuildMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_EMPORIA:
		{
			g_kEmporiaMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_COUPLE:
		{
			m_kCoupleMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_MissionRank:
		{
			m_kRankMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_GMProcess:
		{
			int iCase;
			BM::GUID kMemberGuid;
			BM::GUID kReqGuid;

			pkMsg->Pop(iCase);
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kReqGuid);

			g_kGMProcessMgr.OnGMKickUser(true, iCase, kReqGuid, kMemberGuid);
		}break;
	case PMET_Notice:
		{
			m_kNoticeSystem.ProcessPacket( pkMsg );
		}break;
	case PMET_OXQuizEvent:
		{
			SERVER_IDENTITY kSI;
			SGroundKey kGndKey;
			BM::Stream::DEF_STREAM_TYPE kPacketType;

			pkMsg->Pop(kPacketType);
			kSI.ReadFromPacket(*(dynamic_cast<BM::Stream*>(pkMsg)));
			pkMsg->Pop(kGndKey);
			m_kOXQuizEvent.Locked_RecvMessage(kPacketType, kSI, kGndKey, pkMsg);
		}break;
	case PMET_LuckyStarEvent:
		{
			BM::Stream::DEF_STREAM_TYPE kPacketType;

			pkMsg->Pop(kPacketType);
			m_kLuckyStarEvent.Locked_RecvMessage(kPacketType, pkMsg);
		}break;
	case PMET_CouponEventDoc:
		{
			m_kCouponEventDoc.ProcessMsg(pkMsg);
		}break;
	case PMET_CouponEventView:
		{
			m_kCouponEventView.ProcessMsg(dynamic_cast<BM::Stream*>(pkMsg));
		}break;
	case PMET_EVENTQUEST:
		{
			BM::Stream* pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				m_kEventQuest.ProcessMsg(*pkPacket);
			}
		}break;
	case PMET_BATTLESQUARE:
		{
			BM::Stream* pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				m_kBSGameMng.ProcessMsg(*pkPacket);
			}
		}break;
	case PMET_RELOAD_DATA:
		{//리로드
			ReloadData_Contents();//타입으로. 캐시샵 뿐만 아니라 게임내 상점도.

			BM::GUID kOrderGuid;
			pkMsg->Pop(kOrderGuid);
			g_kGMProcessMgr.SendOrderState( kOrderGuid, OS_DONE);
		}break;
	case PMET_REALM_EVENT:
		{
			BM::Stream* pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				m_kRealmEventMng.ProcessPacket(*pkPacket);
			}
		}break;
	case PMET_HARDCORE_DUNGEON:
		{
			g_kHardCoreDungeonMgr.ProcessPacket( pkMsg );
		}break;
	case PMET_WEB_HELPER:
		{
			BM::Stream* pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				BM::Stream::DEF_STREAM_TYPE wType = 0;
				pkPacket->Pop( wType );
				SocialDBUtil::ProcessPacket(wType, *pkPacket);
			}
		}break;
	case PMET_PVP_LEAGUE:
		{
			BM::Stream* pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				m_kPgPvPLeagueMgr.ProcessMsg(*pkPacket);
			}
		}break;
	case PMET_EXPEDITION:
		{
			BM::Stream * pPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pPacket )
			{
				g_ExpeditionListMgr.ProcessMsg(*pPacket);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV3, __FL__ << _T("invalid Task primary type[") << pkMsg->PriType() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	//INFO_LOG(BM::LOG_LV8, _T("ENDING Message Type[%d][%d]"), pkMsg->PriType(), pkMsg->SecType());
}

void PgTask_RealmContents::Timer1m()
{
	m_kNoticeSystem.OnTick();
	g_kGuildMgr.Locked_Tick();
	g_kHardCoreDungeonMgr.OnTick();
	UpdateAchievementRankView();
	g_kGuildMgr.CheckNewDay();
	UpdateMissionRankClearCheck();
	UpdateStrategyFatigabilityCheck();
}

void PgTask_RealmContents::Timer1s()
{
	DWORD const dwCurTime = BM::GetTime32();
	m_kOXQuizEvent.Locked_Tick();
	m_kRankMgr.OnRegisterMemoTick(dwCurTime);
	m_kCoupleMgr.Tick();
	m_kBSGameMng.OnTick();
	m_kRealmEventMng.Tick();
	m_kPgPvPLeagueMgr.OnTick();
}

void PgTask_RealmContents::Timer10s()
{
	DWORD const dwCurTime = BM::GetTime32();
	m_kRankMgr.OnTick(dwCurTime);
	m_kEventQuest.OnTick();
	m_kLuckyStarEvent.Locked_Tick(dwCurTime);
	g_kStatTrackMgr.Locked_FlushInfo();
}

bool PgTask_RealmContents::Build(CONT_DEF_MISSION_RESULT const &rkResult, CONT_DEF_MISSION_CANDIDATE const &rkCandi, 
		CONT_DEF_MISSION_ROOT const &rkRoot)
{
	return m_kRankMgr.Build(rkResult, rkCandi, rkRoot);
}

void PgTask_RealmContents::Build(CONT_COUPON_EVENT_INFO const & kContEventInfo,CONT_COUPON_EVENT_REWARD const & kContEventReward)
{
	return m_kCouponEventDoc.Build(kContEventInfo, kContEventReward);
}

bool PgTask_RealmContents::ParseXml()
{
	// Def 로드 이후
	bool bRet = true;
	bRet = m_kRealmEventMng.ParseXml() && bRet;
	return bRet;
}