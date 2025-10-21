#include "StdAfx.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PacketStruct4Map.h"
#include "Lohengrin/packetstruct.h"
#include "Lohengrin/PgRealmManager.h"
#include "Collins/Log.h"
#include "Variant/PgLogUtil.h"
#include "Variant/Global.h"
#include "Variant/PgMctrl.h"
#include "Variant/PgEventview.h"
#include "Variant/PgMission.h"
#include "Variant/gm_const.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/ItemDefMgr.h"
#include "PgServerSetMgr.h"
#include "PgRecvFromItem.h"
#include "PgGMProcessMgr.h"
#include "Global.h"
#include "PgGuild.h"
#include "PgDBProcess.h"
#include "constant.h"
#include "JobDispatcher.h"
#include "PgActionEventProcess.h"
#include "Transaction.h"
#include "PgExpeditionListMgr.h"
#include "Variant/PgJumpingCharEventMgr.h"
#include "Variant/PgConstellation.h"
#include <hotmeta/hotmeta.h>
#include "Variant/PgBattlePassMgr.h"
#include "BM/ThreadObject.h"

namespace RealmUserManagerUtil
{
	static int iTutorialMapFighter			= 9910100;
	static int iTutorialMapMagician			= 9910100;
	static int iTutorialMapArcher			= 9910100;
	static int iTutorialMapThief			= 9910100;
	static int iEmergencyMap				= 9018210;
	static SPassTutorialInfo kStartVillageFighter(9018210);
	static SPassTutorialInfo kStartVillageMagician(9018220);
	static SPassTutorialInfo kStartVillageArcher(9018230);
	static SPassTutorialInfo kStartVillageThief(9018240);
	static SPassTutorialInfo kStartVillageSummoner(9010100);
	static SPassTutorialInfo kStartVillageTwins(9010100);
	static size_t iMinCharacterNameLen = 1;
	static size_t iMaxCharacterNameLen = 10;

	template< typename _T >
	inline HRESULT InitConstantValue(int const iKind, int const iID, _T& rkOut)
	{
		if( S_OK != g_kVariableContainer.Get(iKind, iID, rkOut) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		return true;
	}

	bool InitConstantValue()
	{
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_TUTORIAL_FIGHTER, iTutorialMapFighter) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_TUTORIAL_FIGHTER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_TUTORIAL_MAGICIAN, iTutorialMapMagician) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_TUTORIAL_MAGICIAN'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_TUTORIAL_ARCHER, iTutorialMapArcher) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_TUTORIAL_ARCHER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_TUTORIAL_THIEF, iTutorialMapThief) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_TUTORIAL_THIEF'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTVILLAGE_FIGHTER, kStartVillageFighter.iGroundNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTVILLAGE_FIGHTER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTVILLAGE_MAGICIAN, kStartVillageMagician.iGroundNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTVILLAGE_MAGICIAN'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTVILLAGE_ARCHER, kStartVillageArcher.iGroundNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTVILLAGE_ARCHER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTVILLAGE_THIEF, kStartVillageThief.iGroundNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTVILLAGE_THIEF'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTVILLAGE_SUMMONER, kStartVillageSummoner.iGroundNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTVILLAGE_SUMMONER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTVILLAGE_TWINS, kStartVillageTwins.iGroundNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTVILLAGE_TWINS'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_EMERGENCYMAP, iEmergencyMap) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_EMERGENCYMAP'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTPORTAL_FIGHTER, kStartVillageFighter.sPortalNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTPORTAL_FIGHTER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTPORTAL_MAGICIAN, kStartVillageMagician.sPortalNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTPORTAL_MAGICIAN'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTPORTAL_ARCHER, kStartVillageArcher.sPortalNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTPORTAL_ARCHER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTPORTAL_THIEF, kStartVillageThief.sPortalNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTPORTAL_THIEF'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTPORTAL_SUMMONER, kStartVillageSummoner.sPortalNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTPORTAL_SUMMONER'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DefaultMap, EVAR_STARTPORTAL_TWINS, kStartVillageTwins.sPortalNo) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVAR_STARTPORTAL_TWINS'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( !InitConstantValue(EVar_Kind_DBManager, EVar_DB_MinCharacterName, iMinCharacterNameLen) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVar_DB_MinCharacterName'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if( !InitConstantValue(EVar_Kind_DBManager, EVar_DB_MaxCharacterName, iMaxCharacterNameLen) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Find 'EVar_DB_MaxCharacterName'");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( 0 == kStartVillageFighter.sPortalNo )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"EVAR_STARTPORTAL_FIGHTER is can't 0");
			return false;
		}
		if( 0 == kStartVillageMagician.sPortalNo )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"EVAR_STARTPORTAL_MAGICIAN is can't 0");
			return false;
		}
		if( 0 == kStartVillageArcher.sPortalNo )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"EVAR_STARTPORTAL_ARCHER is can't 0");
			return false;
		}
		if( 0 == kStartVillageThief.sPortalNo )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"EVAR_STARTPORTAL_THIEF is can't 0");
			return false;
		}
		if( 0 == iMinCharacterNameLen )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Minimum Character Name Length is can't 0");
			return false;
		}
		if( 0 == iMaxCharacterNameLen )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Maximum Character Name Length is can't 0");
			return false;
		}
		if( iMinCharacterNameLen >= iMaxCharacterNameLen )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't equal greater than minimum name length["<<iMinCharacterNameLen<<L"] the maximmun["<<iMaxCharacterNameLen<<L"]");
			return false;
		}
		size_t const iMaxDBFieldLength = 20;
		if( iMaxDBFieldLength < iMaxCharacterNameLen )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't over than Maximum length["<<iMaxCharacterNameLen<<L"] the DB Field Length[20]");
			return false;
		}

		return true;
	}

	inline bool IsDeadPlayer(PgPlayer *pkPlayer)
	{
		int const iPlayerHP = pkPlayer->GetAbil(AT_HP);
		return 0 == iPlayerHP;
	}

	void GetPlayerClassByTutorialMap(PgPlayer *pkPlayer, SGroundKey &rkOut)
	{
		rkOut.Guid( BM::GUID::Create() );

		int const iPlayerClass = pkPlayer->GetAbil(AT_BASE_CLASS);
		switch( iPlayerClass )
		{
		case UCLASS_FIGHTER:
			{
				rkOut.GroundNo( iTutorialMapFighter );
			}break;
		case UCLASS_MAGICIAN:
			{
				rkOut.GroundNo( iTutorialMapMagician );
			}break;
		case UCLASS_ARCHER:
			{
				rkOut.GroundNo( iTutorialMapArcher );
			}break;
		case UCLASS_THIEF:
			{
				rkOut.GroundNo( iTutorialMapThief );
			}break;
		default:
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Wrong Player[Guid: " << pkPlayer->GetID() << L", Name: " << pkPlayer->Name() << L"] init class[" << iPlayerClass << L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
	}

	void GetPlayerClassByVillageMap(PgPlayer *pkPlayer, SGroundKey &rkOut, short &rsPortarNo)
	{
		rkOut.Guid( BM::GUID::NullData() );

		int const iPlayerClass = pkPlayer->GetAbil(AT_BASE_CLASS);
		switch( iPlayerClass )
		{
		case UCLASS_FIGHTER:
			{
				rkOut.GroundNo( kStartVillageFighter.iGroundNo );
				rsPortarNo = kStartVillageFighter.sPortalNo;
			}break;
		case UCLASS_MAGICIAN:
			{
				rkOut.GroundNo( kStartVillageMagician.iGroundNo );
				rsPortarNo = kStartVillageMagician.sPortalNo;
			}break;
		case UCLASS_ARCHER:
			{
				rkOut.GroundNo( kStartVillageArcher.iGroundNo );
				rsPortarNo = kStartVillageArcher.sPortalNo;
			}break;
		case UCLASS_THIEF:
			{
				rkOut.GroundNo( kStartVillageThief.iGroundNo );
				rsPortarNo = kStartVillageThief.sPortalNo;
			}break;
		case UCLASS_SHAMAN:
			{
				rkOut.GroundNo( kStartVillageSummoner.iGroundNo );
				rsPortarNo = kStartVillageSummoner.sPortalNo;
			}break;
		case UCLASS_DOUBLE_FIGHTER:
			{
				rkOut.GroundNo( kStartVillageTwins.iGroundNo );
				rsPortarNo = kStartVillageTwins.sPortalNo;
			}break;
		default:
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Wrong Player[Guid: " << pkPlayer->GetID() << L", Name: " << pkPlayer->Name() << L"] init class[" << iPlayerClass << L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
	}

	void ProcessKickUser( int const iCase, BM::GUID const &kReqGuid, SContentsUser const &kContUser )
	{
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->kGndKey = SRealmGroundKey(kContUser.sChannel, kContUser.kGndKey);
		pkActionOrder->InsertTarget(kContUser.kMemGuid);
		pkActionOrder->kCause = CNE_CONTENTS_EVENT;
		ContentsActionEvent kEvent(ECEvent_Kick_byGM);
		SPMO kOrder(IMET_CONTENTS_EVENT, kContUser.kMemGuid, kEvent);
		pkActionOrder->kContOrder.push_back(kOrder);
		pkActionOrder->kAddonPacket.Push(iCase);
		pkActionOrder->kAddonPacket.Push(kContUser.kMemGuid);
		pkActionOrder->kAddonPacket.Push(kReqGuid);
		//g_kRealmUserMgr.Locked_AddJob(pkActionOrder);
		g_kJobDispatcher.VPush(pkActionOrder);
	}

	void ProcessAddWorldMap( BM::GUID const &kCharGuid, short const nChannelNo, int const iGroundNo, SGroundKey const &kCasterGndKey )
	{
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		tagPlayerModifyOrderData_AddWorldMap kWorldMap(iGroundNo);
		pkActionOrder->InsertTarget(kCharGuid);
		pkActionOrder->kCause = IMEPT_WORLD_MAP;
		pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_WORLD_MAP, kCharGuid, kWorldMap) );
		pkActionOrder->kGndKey.Channel(nChannelNo);
		pkActionOrder->kGndKey.GroundKey(kCasterGndKey);
		g_kJobDispatcher.VPush(pkActionOrder);
	}

	void UpdateCharacterConnectionChannel(BM::GUID const& rkCharGuid, short const sConnectedChannel)
	{
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NO_OP, _T("EXEC [DBO].[UP_UPDATE_UserCharacter_Channel]"));
		kQuery.InsertQueryTarget( rkCharGuid );
		kQuery.PushStrParam( rkCharGuid );
		kQuery.PushStrParam( sConnectedChannel );
		g_kCoreCenter.PushQuery(kQuery);
	}
	void ClearCharacterConnectionChannel(BM::GUID const* pkMemberGuid)
	{
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NO_OP, _T("EXEC [DBO].[UP_CLEAR_UserCharacter_Channel]"));
		if( pkMemberGuid )
		{
			kQuery.InsertQueryTarget( *pkMemberGuid );
			kQuery.PushStrParam( *pkMemberGuid );
		}
		g_kCoreCenter.PushQuery(kQuery);
	}
}

namespace CHECK_ACHIEVENMENT
{
	void OnCheckRelation(BM::GUID const& kOwnerGuid, SPMOD_Complete_Achievement const& kData, PgAchievements const* pkAchievements)
	{
		if(kData.GroupNo() <= 0 || !pkAchievements)
		{
			return;
		}

		const CONT_DEF_ACHIEVEMENTS_SAVEIDX* pkContDef = NULL;
		g_kTblDataMgr.GetContDef( pkContDef );
		if( !pkContDef )
		{
			return;
		}

		// 1. 해당 GroupNo의 "모두달성" 업적을 찾음
		CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator c_find_iter = pkContDef->begin();
		while( c_find_iter != pkContDef->end() )
		{
			if( c_find_iter->second.iGroupNo == kData.GroupNo()
			&&	c_find_iter->second.iType == AT_ACHIEVEMENT_RELATION_CLAER )
			{
				break; // 이 때 c_find_iter는 연관업적 달성 업적
			}

			++c_find_iter;
		}

		if(c_find_iter == pkContDef->end())
		{
			return;
		}
		
		// 2. 지금 달성되는 업적의 GroupNo와 같은 업적들의 달성 여부를 체크
		bool bSuccessAchievementRelationClear = true;
		CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator c_loop_iter = pkContDef->begin();
		while( c_loop_iter != pkContDef->end() )
		{
			if( c_loop_iter->second.iType == AT_ACHIEVEMENT_RELATION_CLAER )	// 이건 당연히 체크하면 안되고.
			{
				++c_loop_iter;
				continue;
			}

			if( c_loop_iter->second.iGroupNo == kData.GroupNo() )
			{
				if( !pkAchievements->IsComplete( c_loop_iter->first ) )
				{// 하나라도 달성하지 않았으면 쫑
					bSuccessAchievementRelationClear = false;
					break;
				}
			}

			++c_loop_iter;
		}

		// 3. 연관업적 모두 달성하였으므로 "모두달성" 업적을 줌
		if( bSuccessAchievementRelationClear )
		{
			if(SActionOrder * pkActionOrder = PgJobWorker::AllocJob())
			{
				pkActionOrder->InsertTarget( kOwnerGuid );
				pkActionOrder->kCause = CAE_Achievement;
				pkActionOrder->kContOrder.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT, kOwnerGuid,
					SPMOD_Complete_Achievement(c_find_iter->first, c_find_iter->second.iCategory, c_find_iter->second.iRankPoint, c_find_iter->second.iUseTime, c_find_iter->second.iGroupNo)));
				g_kJobDispatcher.VPush(pkActionOrder);
				INFO_LOG( BM::LOG_LV8, __FL__ << "OwnerGuid: " << kOwnerGuid << L", SaveIdx: " << c_find_iter->second.iSaveIdx << L", GroupNo: " << c_find_iter->second.iGroupNo);
			}
		}
	}

	//업적 사용시간 체크
	void OnCheckUseTime( BM::GUID const& kOwnerGuid, SOD_ModifyAchievementTimeLimit const& kData, PgAchievements * pkAchieve )
	{
		if(NULL == pkAchieve)
		{
			return;
		}

		static BM::PgPackedTime kTimeLimit;
		if( pkAchieve->IsComplete(kData.SaveIdx()) && false == pkAchieve->GetAchievementTimeLimit(kData.SaveIdx(), kTimeLimit) )
		{
			if(SActionOrder * pkActionOrder = PgJobWorker::AllocJob())
			{
				pkActionOrder->InsertTarget( kOwnerGuid );
				pkActionOrder->kCause = CAE_Achievement;
				pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ACHIEVEMENT_TIMELIMIT, kOwnerGuid, kData));
				g_kJobDispatcher.VPush(pkActionOrder);
				INFO_LOG( BM::LOG_LV8, __FL__ << "OwnerGuid: " << kOwnerGuid << L", SaveIdx: " << kData.SaveIdx() << L", UseTime: " << kData.UseTime());
			}
		}			
	}

	typedef std::map<int,int> CONT_RELATION_CLAER;
	typedef std::map<int,int> CONT_USE_TIME;
	CONT_RELATION_CLAER GetRelationClaer()
	{
		CONT_RELATION_CLAER kCont;
		const CONT_DEF_ACHIEVEMENTS * pkContDef = NULL;
		g_kTblDataMgr.GetContDef( pkContDef );
		if( !pkContDef )
		{
			return kCont;
		}

		CONT_DEF_ACHIEVEMENTS::const_iterator iter = pkContDef->begin();
		while(iter != pkContDef->end())
		{
			if( AT_ACHIEVEMENT_RELATION_CLAER == (*iter).second.iType)
			{
				kCont.insert(std::make_pair((*iter).second.iSaveIdx, (*iter).second.iGroupNo));
			}
			++iter;
		}
		return kCont;
	}

	CONT_USE_TIME GetUseTime()
	{
		CONT_USE_TIME kCont;
		CONT_DEF_ACHIEVEMENTS const *pkContDef = NULL;
		g_kTblDataMgr.GetContDef(pkContDef);
		if(!pkContDef)
		{
			return kCont;
		}

		CONT_DEF_ACHIEVEMENTS::const_iterator iter = pkContDef->begin();
		while(iter != pkContDef->end())
		{
			if((*iter).second.iUseTime > 0)
			{
				kCont.insert(std::make_pair((*iter).second.iSaveIdx, (*iter).second.iUseTime));
			}
			++iter;
		}
		return kCont;
	}

	
	void Process(PgPlayer *const pkPlayer)
	{
		if(NULL == pkPlayer)
		{
			return;
		}
		
		{// 연관 업적 체크
			static const CONT_RELATION_CLAER kCont = GetRelationClaer();

			for(CONT_RELATION_CLAER::const_iterator it=kCont.begin(); it!=kCont.end(); ++it)
			{
				SPMOD_Complete_Achievement kData;
				kData.SaveIdx((*it).first);
				kData.GroupNo((*it).second);
				CHECK_ACHIEVENMENT::OnCheckRelation(pkPlayer->GetID(), kData, pkPlayer->GetAchievements());
			}
		}

		{// 유지시간 체크
			static const CONT_USE_TIME kCont = GetUseTime();

			for(CONT_USE_TIME::const_iterator it=kCont.begin(); it!=kCont.end(); ++it)
			{
				SOD_ModifyAchievementTimeLimit const kData((*it).first, (*it).second);
				CHECK_ACHIEVENMENT::OnCheckUseTime(pkPlayer->GetID(), kData, pkPlayer->GetAchievements());
			}
		}
	}
}//CHECK_ACHIEVENMENT

PgRealmUserManager::PgRealmUserManager(void)
:	m_eServerStatus(ESERVER_STATUS_NONE), m_iMyHomeUnitCount(0)
{
}

PgRealmUserManager::~PgRealmUserManager(void)
{
}

int Cmp_PlayerCount(void const *pOrg, void const *pTgt)
{//작은거 리턴.
	TBL_SERVERLIST const *pkOrgTbl = (TBL_SERVERLIST const *)pOrg;
	TBL_SERVERLIST const *pkTgtTbl = (TBL_SERVERLIST const *)pTgt;

	if(pkOrgTbl->nPlayerCount < pkTgtTbl->nPlayerCount)
	{
		return -1;
	}

	if(pkOrgTbl->nPlayerCount > pkTgtTbl->nPlayerCount)
	{
		return 1;
	}
	return 0;
}

HRESULT PgRealmUserManager::GetLoadBalanceSwitch(CONT_SERVER_HASH const &kContServer, SERVER_IDENTITY &rkSI)const
{
	bool bIsFirst = true;
	size_t nAccCount = 0;
	size_t nRetCount = 0;

	std::vector< TBL_SERVERLIST > kContPlayerCount;

	CONT_SERVER_HASH::const_iterator itor = kContServer.begin();
	while( kContServer.end() != itor )
	{
		CONT_SERVER_HASH::mapped_type const &element = (*itor).second;
		
		if(CEL::ST_SWITCH == element.nServerType
		&& element.pkSession)//세션 살아있어야함.
		{
			nAccCount += element.nPlayerCount;
			kContPlayerCount.push_back(element);
		}
		++itor;
	}

	if(kContPlayerCount.size())
	{
		::qsort(&kContPlayerCount.at(0), kContPlayerCount.size(), sizeof(TBL_SERVERLIST), Cmp_PlayerCount);
		if( MaxPlayerCount() <= nAccCount )
		{
		//	return false;
		}
			
		rkSI = kContPlayerCount.at(0);
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgRealmUserManager::Locked_Process_InsertSwitchWaitUser(SReqSwitchReserveMember const &rkRSRM/*, CEL:Stream &kRetPacket*/)//결과 패킷을 만들던가.
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if ( ESERVER_STATUS_STOPPING == m_eServerStatus )
	{
		// 서버를 닫고 있을 때는 거부해야 한다.
		BM::Stream kIMPacket( PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED, E_TLR_CHANNEL_FULLUSER );
		kIMPacket.Push( rkRSRM.guidMember );
		::SendToImmigration( kIMPacket );
		return false;
	}

	// Center의 동접수 검사
	if(		( 0 == rkRSRM.byGMLevel )	// GM계정은 무조건 통과
		&&	!CanReserveNewAccount()
	)
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T("Max ChannelUserCount :: Cannot Reserve New User") );
		BM::Stream kIMPacket( PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED, E_TLR_CHANNEL_FULLUSER );
		kIMPacket.Push(rkRSRM.guidMember);
		::SendToImmigration( kIMPacket );
		return false;
	}

	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkRSRM.guidMember, true, kLockPlayer) )
	{
		// 이미 있는 Player
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Alread exist LoginedUser [") << rkRSRM.ID() << _T("]") );

		// 이런일이 있을 수 있나? ㅡ_ㅡ
		// 만약 있다면 로그를 빡빡 남기고 있는 유저를 로그아웃 시켜야 하지 않을까?
		BM::Stream kIMPacket( PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED, E_TLR_ALREADY_LOGINED );
		kIMPacket.Push(rkRSRM.guidMember);
		::SendToImmigration( kIMPacket );
		return false;
	}


//	선택한 센터에게 보내야.
	CONT_SERVER_HASH kContServer;
	if( S_OK == g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CENTER, kContServer) )
	{
		CONT_SERVER_HASH::const_iterator server_itor = kContServer.begin();
		while( server_itor != kContServer.end() )
		{
			SERVER_IDENTITY const &kSI = (*server_itor).first;
			if(kSI.nChannel == rkRSRM.ChannelNo())
			{
				BM::Stream kPacket(PT_N_T_REQ_RESERVE_SWITCH_MEMBER);
				rkRSRM.WriteToPacket(kPacket);
				if(SendToServer(kSI, kPacket))
				{
					return true;
				}
			}

			++server_itor;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
	/*
__ROLL_BACK:
	{
//		m_ContSwitchWait.erase(rkRSRM.guidMember);	
	}
__FAILED:
	{
//		BM::Stream kRetPacket(PT_L_IM_RESERVED_4, rkRSRM.guidMember);//실패다.
//		실패를 보내야된다.
//		SendToImmigration(kRetPacket);
	}
	*/
}

void PgRealmUserManager::Locked_Recv_PT_S_T_ANS_RESERVE_MEMBER(BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	ESwitchReservePlayerResult eRet = SRPR_NONE;
	BM::GUID kSwitchKeyGuid;
	BM::GUID kMemberGuid;
	CEL::ADDR_INFO kSwitchAddr;

	pkPacket->Pop(eRet);
	pkPacket->Pop(kMemberGuid);
	pkPacket->Pop(kSwitchKeyGuid);
	pkPacket->Pop(kSwitchAddr);
	switch(eRet)
	{
	case SRPR_SUCEESS:
		{//Success
			SSwitchPlayerData kSPD;
			kSPD.ReadFromPacket( *pkPacket );

			INFO_LOG( BM::LOG_LV6, __FL__ << _T("<08> User Assign Success From Switch ID[") << kSPD.ID() << _T("], MemberKey[") << kSPD.guidMember.str().c_str() << _T("]") );

			SSwitchReserveResult kSRR = kSPD;
			kSRR.eRet = SRPR_SUCEESS;
			kSRR.addrSwitch	= kSwitchAddr;
			kSRR.guidSwitchKey = kSPD.guidOrderKey;

			INFO_LOG( BM::LOG_LV6, __FL__ << _T("<09> Send to LoginServer, the Switch Information") );
			
			BM::Stream kPacket(PT_T_IM_NFY_RESERVED_SWITCH_INFO);
			kSRR.WriteToPacket(kPacket);
			SendToImmigration(kPacket);
		}break;
	default:
		{	//실패 결과 전송
			INFO_LOG( BM::LOG_LV4, __FL__ << _T("<08-2> User Assign Fail From Switch") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
	//이런일이???
	//		SSwitchReserveResult;
	//		kSRR.eRet = eRet;
	//		BM::Stream kPacket(PT_T_IM_NFY_RESERVED_SWITCH_INFO, kSRR);
	//		SendToImmigration(kPacket);
		}break;
	}
	return;
}

void PgRealmUserManager::Locked_Recv_PT_IM_T_ANS_SWITCH_USER_ADD(HRESULT const hRecvRet, SSwitchPlayerData const &kSPD)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	//INFO_LOG(BM::LOG_LV7, _T("[%s] Member[%s], hRecvRet[%d]"), __FUNCTIONW__, kSPD.guidMember.str().c_str(), hRecvRet);
	if (kSPD.dtLastLogout.IsNull())
	{
		// 처음으로 Login 한 유저 이다.
		PgLogCont kLogCont(ELogMain_System_First_Logined, ELogSub_None);
		kLogCont.MemberKey(kSPD.guidMember);
		kLogCont.UID(kSPD.iUID);
		kLogCont.ID(kSPD.ID());
		PgLog kLog(ELOrderMain_None, ELOrderSub_None);
		kLog.Set(1, kSPD.addrRemote.ToString());
		kLog.Set(0, static_cast<int>(kSPD.ucGender));
		kLog.Set(1, static_cast<int>(kSPD.usAge));
		kLog.Set(2, static_cast<int>(kSPD.addrRemote.wPort));
		kLogCont.Add(kLog);
		kLogCont.Commit();
	}

	CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = new SCenterPlayerData(kSPD);

	HRESULT hRet = E_FAIL;
	if( SUCCEEDED(hRecvRet)
	&& pData)
	{
		auto const ret = m_kContPlayer_MemberKey.insert( std::make_pair( kSPD.guidMember, pData) );
		if( ret.second )//넣기 성공
		{
			const auto ret2 = m_kContPlayer_MemberID.insert( std::make_pair( kSPD.ID(), pData) );
			if( ret2.second )//넣기 성공
			{
				PgLogCont kLogCont(ELogMain_System_Login_User_Info);
				kLogCont.MemberKey(kSPD.guidMember);
				kLogCont.UID(kSPD.iUID);
				kLogCont.ID(kSPD.ID());
				PgLog kLog;
				//kLog.Set(1, kSPD.addrRemote.IP());
				kLog.Set(1, kSPD.addrRemote.ToString());
				kLog.Set(0, static_cast<int>(kSPD.ucGender));
				kLog.Set(1, static_cast<int>(kSPD.usAge));
				//kLog.Set(2, static_cast<int>(kSPD.addrRemote.wPort));

				kLogCont.Add(kLog);
				kLogCont.Commit();

				hRet = hRecvRet;
				goto __COMPLETE;
			}
			else
			{
				m_kContPlayer_MemberKey.erase( ret.first );
				goto __FAILED;
			}
		}
	}

	if(!pData)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, _T("[ERROR] [") << __FL__ << _T("] SCentekSPDData Memory Faied") );
		goto __FAILED;
	}
__FAILED:
	{
		CAUTION_LOG( BM::LOG_LV0, _T("[ERROR] [") << kSPD.ID() << _T("] Can't Insert Userlist") );
		INFO_LOG( BM::LOG_LV0, _T("[ERROR] [") << kSPD.ID() << _T("] Can't Insert Userlist") );
		SAFE_DELETE(pData);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
	}
__COMPLETE:
	{
		BM::Stream kRPacket(PT_T_S_ANS_LOGINED_PLAYER);
		kRPacket.Push(hRet);
		kRPacket.Push(kSPD.guidMember);
		kRPacket.Push(kSPD.guidOrderKey);

		SendToSwitch(kSPD.guidMember, kRPacket);
	}
	{
		BM::Stream kRPacket(PT_S_S_SEND_TO_SWITCH_USER);
		kRPacket.Push(kSPD.guidMember);
		kRPacket.Push(hotmeta::PT_I_M_HOTMETA_SYNC);
		hotmeta::write_to_packet(kRPacket);
		SendToSwitch(kSPD.guidMember, kRPacket);
	}
}

void PgRealmUserManager::ProcessRemoveUser_Common( CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData, bool bNotify, SReqSwitchReserveMember const *pkSRM )
{	
	if(!pCPData)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"pCPData is NULL!!!!");
		return;
	}

	__int64 const i64PlayerTime = _UpdatePlayTime(pCPData);

	if (bNotify)
	{
		BM::Stream kNfyPacket(PT_A_NFY_USER_DISCONNECT);
		kNfyPacket.Push(pCPData->guidMember);
		kNfyPacket.Push(pCPData->guidCharacter);
		pCPData->kSwitchServer.WriteToPacket(kNfyPacket);

		bool const bAutoLogin = ( NULL != pkSRM );
		kNfyPacket.Push( bAutoLogin );

		// 모든 채널로 보낸다.
		g_kProcessCfg.Locked_SendToChannelServerType( CEL::ST_CENTER, kNfyPacket, -1, true );

		if ( true == bAutoLogin )
		{
			kNfyPacket.Push( g_kProcessCfg.ServerIdentity() );
			SReqSwitchReserveMember kSRM(*pkSRM);
			kSRM.iAccConnSec = pCPData->iAccConSec;
			kSRM.iAccDisConnSec = pCPData->iAccDisSec;
			kSRM.i64TotalConnSec = pCPData->i64TotalConnSec;
			kSRM.WriteToPacket( kNfyPacket );
		}
		::SendToImmigration(kNfyPacket);
	}

	{//모든 Contents 클래스에 유저가 나감을 알리자
		BM::Stream kHPacket(PT_A_NFY_USER_DISCONNECT);
		SContentsUser kUserInfo;
		if( pCPData->Copy(kUserInfo) )
		{
			kUserInfo.kGndKey.Clear();// 로그아웃이니까 현재 위치는 클리어.

			kUserInfo.WriteToPacket(kHPacket);

			::SendToFriendMgr(kHPacket); kHPacket.PosAdjust();
			::SendToGuildMgr(kHPacket); kHPacket.PosAdjust();
			::SendToCoupleMgr(kHPacket);
			::SendToRealmContents(PMET_EVENTQUEST, kHPacket);
			::SendToRealmContents(PMET_BATTLESQUARE, kHPacket);
		}

		BM::Stream kNfyPacket(PT_NFY_LUCKYSTAR_LOGOUTUSER);
		kNfyPacket.Push(pCPData->guidMember);
		kNfyPacket.Push(pCPData->guidCharacter);
		SendToLuckyStarEvent(kNfyPacket);
	}

	//로그아웃 로그
	if(pCPData)
	{
		PgLogCont kLogCont(ELogMain_System_Login_LogOut);
		kLogCont.MemberKey(pCPData->guidMember);
		kLogCont.UID(pCPData->iUID);
		kLogCont.ChannelNo(pCPData->ChannelNo());
		kLogCont.ID(pCPData->kID);

		if(pCPData->pkPlayer)
		{
			PgDoc_Player* pkDocPlayer = pCPData->pkPlayer;

			kLogCont.CharacterKey(pkDocPlayer->GetID());
			kLogCont.Name(pkDocPlayer->Name());
			kLogCont.Class(pkDocPlayer->GetAbil(AT_CLASS));
			kLogCont.Level(pkDocPlayer->GetAbil(AT_LEVEL));
			kLogCont.GroundNo(pkDocPlayer->GetAbil(AT_RECENT));

			int const iNotConnection = 0;
			RealmUserManagerUtil::UpdateCharacterConnectionChannel(pkDocPlayer->GetID(), iNotConnection);
		}
		else
		{
			RealmUserManagerUtil::ClearCharacterConnectionChannel(&pCPData->guidMember); // 계정의 모든 캐릭터 채널 접속정보 초기화
		}
		PgLog kLog;
		kLog.Set(1, pCPData->addrRemote.ToString());
		kLog.Set(0, i64PlayerTime);
		kLogCont.Add(kLog);
		kLogCont.Commit();
	}

	SaveMemberData(pCPData);
// 플레이어 최신 정보가 저장되지 않을 경우는 없다고 봄
// 1. 로그아웃시 맵에서 받는다.
// 2. 맵 이동시 커낵션에 문제가 생겨도 이동전에 저장된다.
// 3. 최악의 시나리오 맵 서버가 최신 데이터를 보내다가 죽는다.
//    문제는 이상황에 죽었을경우 센터에서 가지고 있는 데이터도 이전 데이터 이므로 롤백은 막을수없다.
	
	//_ProcessRemoveCharacter( pCPData->pkPlayer );
	_ProcessRemoveCharacter( pCPData );
}

void PgRealmUserManager::_ProcessRemoveCharacter( SCenterPlayerData* pkCenterPlayer )
{
	if (pkCenterPlayer->pkPlayer != NULL)
	{
		SContentsUser kLogOutUser;
		pkCenterPlayer->Copy(kLogOutUser);
		m_kGambleUserMgr.LogOut(kLogOutUser, SHOP_GAMBLEMACHINE_RESULT_FROM, SHOP_GAMBLEMACHINE_RESULT_TITLENO, SHOP_GAMBLEMACHINE_RESULT_TEXTNO);
		m_kMixupUserMgr.LogOut(kLogOutUser, SHOP_GAMBLEMACHINE_MIXUP_RESULT_FROM, SHOP_GAMBLEMACHINE_MIXUP_RESULT_TITLENO, SHOP_GAMBLEMACHINE_MIXUP_RESULT_TEXTNO);
		Locked_LeaveEvent(pkCenterPlayer->pkPlayer->GetID());
		m_kOpenMarketMgr.Locked_ProcessDisableMarket(pkCenterPlayer->pkPlayer->GetID());
		ProcessUpdateCacheDB( pkCenterPlayer->pkPlayer );

		BM::Stream kPacket(PT_C_M_REQ_MYHOME_CHAT_EXIT);
		kPacket.Push(pkCenterPlayer->pkPlayer->GetID());
		SendToMyhomeMgr(kPacket);
		// SAFE_DELETE(pkCenterPlayer->pkPlayer); // remove later in tick or when a same user login
	}
	// SAFE_DELETE(pkCenterPlayer->pkBackupPlayer); // remove later in tick or when a same user login
	return;
}

bool PgRealmUserManager::ProcessRemoveUser(BM::GUID const &kMemberGuid, bool bNotify, SReqSwitchReserveMember const *pkSRM )
{
//	INFO_LOG(BM::LOG_LV7, _T("[%s] MemberGuid[%s], bNotify[%d]"), __FUNCTIONW__, kMemberGuid.str().c_str(), bNotify);
	CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.find(kMemberGuid);
	if( m_kContPlayer_MemberKey.end() != user_itor)
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = user_itor->second;//pData 는 pool 데이터이므로 user_itor 삭제해도 됨.
		RemovePlayer( pCPData );//ProcessRemoveUser_Common가 먼저 호출 되면 player 정보가 지워져서 안됨.
		ProcessRemoveUser_Common( pCPData, bNotify, pkSRM );

		// SAFE_DELETE(pCPData); // remove later in tick or when a same user login
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


//bool PgRealmUserManager::ProcessRemoveUser(SERVER_IDENTITY const &kSI)
//{
//	bool bNotify = false;
//	switch(kSI.nServerType)
//	{
//	case CEL::ST_LOG:
//	case CEL::ST_MAP:
//	case CEL::ST_ITEM:
//	case CEL::ST_MACHINE_CONTROL:
//		{
//		}break;
//	case CEL::ST_CENTER:
//	case CEL::ST_SWITCH:
//		{
//			_ProcessRemoveUser(kSI);
//			bNotify = true;
//		}break;
//	default:
//		{
//			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Unknown SeverType[") << kSI.nServerType << _T("]") );
//			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
//		}break;
//	}
//	return bNotify;
//}

//void PgRealmUserManager::_ProcessRemoveUser(SERVER_IDENTITY const &kSI)
//{
//	switch (kSI.nServerType)
//	{
//	case CEL::ST_SWITCH:
//		{
//			CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.begin();
//			while( m_kContPlayer_MemberKey.end() != user_itor)
//			{
//				CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData 는 pool 데이터이므로 user_itor 삭제해도 됨.
//
//				SERVER_IDENTITY const &kSwitchServer = pCPData->kSwitchServer;
//				if(kSI == kSwitchServer)
//				{
//					RemovePlayer(pCPData);//ProcessRemoveUser_Common가 먼저 호출 되면 player 정보가 지워져서 안됨.
//
//					ProcessRemoveUser_Common(pCPData);
//					SAFE_DELETE(pCPData);
//
//					user_itor = m_kContPlayer_MemberKey.begin();
//					continue;
//				}
//				++user_itor;
//			}
//		}break;
//	case CEL::ST_CENTER:
//		{
//			CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.begin();
//			while( m_kContPlayer_MemberKey.end() != user_itor)
//			{
//				CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData 는 pool 데이터이므로 user_itor 삭제해도 됨.
//
//				short const sChannel = pCPData->ChannelNo();
//				if(kSI.nChannel == sChannel)
//				{
//					RemovePlayer(pCPData);//ProcessRemoveUser_Common가 먼저 호출 되면 player 정보가 지워져서 안됨.
//
//					ProcessRemoveUser_Common(pCPData);
//					SAFE_DELETE(pCPData);
//
//					user_itor = m_kContPlayer_MemberKey.begin();
//					continue;
//				}
//				++user_itor;
//			}
//		}break;
//	default:
//		{
//			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled ServerType [") << C2L(kSI) << _T("]") );
//			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
//		}break;
//	}
//}

std::wstring PgRealmUserManager::LoginedPlayerName(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid)
{
	std::wstring wstrName;

	SCenterPlayer_Lock kLockPlayer;
	if (!GetPlayerInfo(rkMemberGuid, true, kLockPlayer))
	{
		return std::wstring();
	}
	if (kLockPlayer.pkCenterPlayerData == NULL || kLockPlayer.pkCenterPlayerData->pkPlayer == NULL)
	{
		return std::wstring();
	}
	PgDoc_Player *pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;	
	if(pkPlayer == NULL || pkPlayer->GetID() != rkCharacterGuid)
	{ 
		return std::wstring();
	}
	return pkPlayer->Name();
}

void PgRealmUserManager::Locked_UpdatePlayerTime(BM::GUID const &rkCharacterGuid, BM::Stream * pkPacket)
{
	if(!pkPacket)
	{
		return;
	}

	BM::CAutoMutex kLock(m_kMutex);

	SCenterPlayer_Lock kLockPlayer;
	if (!GetPlayerInfo(rkCharacterGuid, false, kLockPlayer))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Get PlayerInfo Failed"));
		return;
	}

	if (kLockPlayer.pkCenterPlayerData == NULL || kLockPlayer.pkCenterPlayerData->pkPlayer == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! PlayerData is NULL"));
		return;
	}
	PgDoc_Player *pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;	
	if(pkPlayer == NULL || pkPlayer->GetID() != rkCharacterGuid)
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! PlayerData is NULL"));
		return;
	}

	EWallowDefendMode eDefendMode = WDM_NONE;
	int iExpRate = 0;
	int iMoneyRate = 0;
	int iDropRate = 0;
	EPPTCondition eEtcBoolean = PPTC_NONE;
	pkPacket->Pop(eDefendMode);
	pkPacket->Pop(iExpRate);
	pkPacket->Pop(iMoneyRate);
	pkPacket->Pop(iDropRate);
	pkPacket->Pop(eEtcBoolean);

	pkPlayer->SetDefendMode(eDefendMode);
	pkPlayer->SetExpRate(iExpRate);
	pkPlayer->SetMoneyRate(iMoneyRate);
	pkPlayer->SetDropRate(iDropRate);
	pkPlayer->SetEtcBoolean(eEtcBoolean);
}

struct SAFE_PLAYER_UNIT_ARRAY
	: public PLAYER_UNIT_ARRAY
{
	~SAFE_PLAYER_UNIT_ARRAY()
	{
		PLAYER_UNIT_ARRAY::iterator itor = begin();
		while(itor != end())
		{
			SAFE_DELETE(*itor);
			++itor;
		}
	}
};

bool PgRealmUserManager::Locked_Q_DQT_GET_MEMBER_PREMIUM_SERVICE(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	E_PREMIUM_QUERY_TYPE eQuaryType = EPQT_NONE;
	rkResult.contUserData.Pop(eQuaryType);
	if(EPQT_NONE==eQuaryType)
	{
		int const iJumpingEventNo = g_kJumpingCharEventMgr.GetNowEventNo();
		short nChannelNo = 0;
		rkResult.contUserData.Pop(nChannelNo);
		int iDrakanCreateItemNo = 0;
		g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Char_CreateDrakanItemNo, iDrakanCreateItemNo);

		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GET_PLAYER_LIST, L"EXEC [dbo].[up_SelectCharacter]");
		kQuery.QueryGuid( rkResult.QueryGuid() );
		kQuery.QueryOwner( rkResult.QueryOwner() );
		kQuery.InsertQueryTarget(rkResult.QueryOwner());
		kQuery.PushStrParam( rkResult.QueryOwner() );	// 인자값:멤버GUID
		kQuery.PushStrParam(PgClassDefMgr::s_sMaximumLevel);
		kQuery.PushStrParam(iJumpingEventNo);
		kQuery.PushStrParam(iDrakanCreateItemNo);
		kQuery.contUserData.Push(nChannelNo);
		kQuery.contUserData.Push(iJumpingEventNo);
		g_kCoreCenter.PushQuery(kQuery);
	}
	
	//INFO_LOG( BM::LOG_LV9, __FL__ << _T("1. Premium Query MemberKey[") << rkResult.QueryOwner() << _T("]") );

	if( CEL::DR_NO_RESULT == rkResult.eRet )
	{
		return true;
	}

	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Query failed. Get PremiumService ErrorCode[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	if( rkResult.vecArray.end() != itor )
	{
		CONT_MEMBER_PREMIUM::mapped_type kData;
		(*itor).Pop( kData.iServiceNo );			++itor;
		(*itor).Pop( kData.kStartDate );			++itor;
		(*itor).Pop( kData.kEndDate );				++itor;

		m_kContMemberPremium.erase(rkResult.QueryOwner());

		BM::DBTIMESTAMP_EX kNowTime;
		g_kEventView.GetLocalTime( kNowTime );
		if(kData.kEndDate > kNowTime)
		{
			m_kContMemberPremium.insert(std::make_pair(rkResult.QueryOwner(), kData));
		}
	}

	//
	if(EPQT_GM_ORDER==eQuaryType)
	{
		CONT_MEMBER_PREMIUM::const_iterator premium_it = m_kContMemberPremium.find( rkResult.QueryOwner() );
		if(premium_it != m_kContMemberPremium.end())
		{
			short eOrderType = ORDER_NONE;
			rkResult.contUserData.Pop(eOrderType);
			switch(eOrderType)
			{
			case ORDER_PREMIUM_SERVICE_MODIFY:
				{
					int iUseDate = 0;
					rkResult.contUserData.Pop(iUseDate);

					BM::PgPackedTime kEndDate( (*premium_it).second.kEndDate );
					CGameTime::AddTime(kEndDate, iUseDate * CGameTime::OneDay );

					CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_PREMIUM_SERVICE, L"EXEC [dbo].[UP_PremiumService_Update]");
					kQuery.InsertQueryTarget(rkResult.QueryOwner());
					kQuery.PushStrParam( rkResult.QueryOwner() );
					kQuery.PushStrParam( (*premium_it).second.iServiceNo );
					kQuery.PushStrParam( (*premium_it).second.kStartDate );
					kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kEndDate) );
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case ORDER_PREMIUM_SERVICE_REMOVE:
				{
					SYSTEMTIME kLocalTime;
					g_kEventView.GetLocalTime(&kLocalTime);
					BM::DBTIMESTAMP_EX kNowDate = BM::DBTIMESTAMP_EX(kLocalTime);

					CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_PREMIUM_SERVICE, L"EXEC [dbo].[UP_PremiumService_Update]");
					kQuery.InsertQueryTarget(rkResult.QueryOwner());
					kQuery.PushStrParam( rkResult.QueryOwner() );
					kQuery.PushStrParam( (*premium_it).second.iServiceNo );
					kQuery.PushStrParam( (*premium_it).second.kStartDate );
					kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kLocalTime) );
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			}
		}
	}
	return true;
}

void PgRealmUserManager::WriteToPacket_MEMBER_PREMIUM_SERVICE(BM::GUID const& kMemberID, BM::Stream & rkPacket)const
{
	CONT_MEMBER_PREMIUM::const_iterator c_it = m_kContMemberPremium.find(kMemberID);
	if(c_it!=m_kContMemberPremium.end())
	{
		rkPacket.Push( static_cast<bool>(true) );	//프리미엄서비스 사용
		rkPacket.Push((*c_it).second.iServiceNo);
		rkPacket.Push((*c_it).second.kStartDate);
		rkPacket.Push((*c_it).second.kEndDate);
	}
	else
	{
		rkPacket.Push( static_cast<bool>(false) );	//프리미엄서비스 미사용
	}
}

void PgRealmUserManager::SetPrimiumService(PgDoc_Player * pkPlayer)
{
	if(!pkPlayer)
	{
		return;
	}

	CONT_MEMBER_PREMIUM::const_iterator c_it = m_kContMemberPremium.find(pkPlayer->GetMemberGUID());
	if(c_it!=m_kContMemberPremium.end())
	{
		BM::Stream kCustomData = (*c_it).second.kCustomData;
		pkPlayer->GetPremium().SetupService((*c_it).second.iServiceNo, pkPlayer, kCustomData);
		pkPlayer->GetPremium().StartDate((*c_it).second.kStartDate);
		pkPlayer->GetPremium().EndDate((*c_it).second.kEndDate);
	}
}

void PgRealmUserManager::Locked_OnTick_Premium()
{
	BM::CAutoMutex kLock(m_kMutex);

	static DWORD s_dwHour = 0;
	if (BM::TimeCheck(s_dwHour, 3600000))
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator loop_iter = m_kContPlayer_MemberKey.begin();
		while( loop_iter != m_kContPlayer_MemberKey.end() )
		{
			SCenterPlayer_Lock kLockPlayer;
			if( GetPlayerInfo( (*loop_iter).first, true, kLockPlayer ) )
			{
				PgDoc_Player *pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
				if( pkPlayer && UT_PLAYER==pkPlayer->UnitType() )
				{
					//프리미엄 서비스 삭제 체크
					if(pkPlayer->GetPremium().GetServiceNo() && false==pkPlayer->GetPremium().IsUserService(true))
					{
						CONT_PLAYER_MODIFY_ORDER		kOrder;
						kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_REMOVE, pkPlayer->GetID()));

						SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
						pkActionOrder->InsertTarget(pkPlayer->GetMemberGUID());
						pkActionOrder->kCause = CIE_PREMIUM_SERVICE;
						pkActionOrder->kContOrder = kOrder;
						g_kJobDispatcher.VPush(pkActionOrder);
					}
				}
			}

			++loop_iter;
		}
	}
}

bool PgRealmUserManager::Locked_Q_DQT_GET_PLAYER_LIST(CEL::DB_RESULT &rkResult)
{//EXEC [dbo].[UP_SelectCharacterList]
	int i = 0;
	BM::CAutoMutex kLock(m_kMutex, true);
	
	//INFO_LOG( BM::LOG_LV9, __FL__ << _T("2. PlayerList Query MemberKey[") << rkResult.QueryOwner() << _T("]"));

	//CFunctionMonitor kFM(__FUNCTIONW__, __LINE__);
	SAFE_PLAYER_UNIT_ARRAY vtPlayers;

	CONT_DB_ITEM_LIST kItemList;
	int iJumpingCharEventNo = 0;
	int iJumpingCharEventRemainRewardCount = 0;
	int iDrakanCreateItemCount = 0;

	//
	short nChannelNo = 0;
	int iJumping_EventNo = 0;
	rkResult.contUserData.Pop(nChannelNo);
	rkResult.contUserData.Pop(iJumping_EventNo);

	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_RESULT_COUNT::const_iterator return_iter = rkResult.vecResultCount.begin();

		if(return_iter == rkResult.vecResultCount.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		int iCharacterCount = (*return_iter);	++return_iter;

		while( iCharacterCount )
		{
			if ( rkResult.vecArray.end() != itor )
			{
				DBTIMESTAMP kDate;
				BYTE byClass;
				BM::GUID guidMember;
				SPlayerInfo kPlayerInfo;
				std::wstring kName;
				double x = 0, y = 0, z = 0;
				BYTE byCharactorSlot = 0;
				
				(*itor).Pop( guidMember );						++itor;
				(*itor).Pop( kPlayerInfo.guidCharacter );		++itor;
				(*itor).Pop( kPlayerInfo.byState );				++itor;
				(*itor).Pop( kName );							++itor;
				(*itor).Pop( kPlayerInfo.byGender );			++itor;//5

				(*itor).Pop( kPlayerInfo.iRace );				++itor;
				// Class 정보는 1byte로 DB에 저장되어 있다.
				(*itor).Pop( byClass );							++itor;	kPlayerInfo.iClass = byClass;
				(*itor).Pop( kPlayerInfo.wLv );					++itor;
				(*itor).Pop( kPlayerInfo.i64Exp );				++itor;
				(*itor).Pop( kPlayerInfo.i64Money );			++itor;//10

				(*itor).Pop( kPlayerInfo.iHP );					++itor;
				(*itor).Pop( kPlayerInfo.iMP );					++itor;
				(*itor).Pop( kPlayerInfo.iHairColor );			++itor;
				(*itor).Pop( kPlayerInfo.iHairStyle );			++itor;
				(*itor).Pop( kPlayerInfo.iFace );				++itor;//15

				(*itor).Pop( kPlayerInfo.iJacket );				++itor;
				(*itor).Pop( kPlayerInfo.iPants );				++itor;
				(*itor).Pop( kPlayerInfo.iShoes );				++itor;
				(*itor).Pop( kPlayerInfo.iGloves );				++itor;
				(*itor).Pop( kPlayerInfo.kCoupleGuid );			++itor;//20

				(*itor).Pop( kPlayerInfo.kGuildGuid );			++itor;
				(*itor).Pop( kPlayerInfo.kRecentInfo.iMapNo );	++itor;
				(*itor).Pop( x );								++itor;	kPlayerInfo.kRecentInfo.ptPos3.x = static_cast<float>(x);
				(*itor).Pop( y );								++itor;	kPlayerInfo.kRecentInfo.ptPos3.y = static_cast<float>(y);
				(*itor).Pop( z );								++itor;	kPlayerInfo.kRecentInfo.ptPos3.z = static_cast<float>(z);
				
				(*itor).Pop( kPlayerInfo.kLastVillage.iMapNo );	++itor;
				(*itor).Pop( x );								++itor;	kPlayerInfo.kLastVillage.ptPos3.x = static_cast<float>(x);
				(*itor).Pop( y );								++itor;	kPlayerInfo.kLastVillage.ptPos3.y = static_cast<float>(y);
				(*itor).Pop( z );								++itor;	kPlayerInfo.kLastVillage.ptPos3.z = static_cast<float>(z);
				(*itor).Pop( kDate );							++itor;	kPlayerInfo.kBirthDate = kDate;

				(*itor).Pop( kPlayerInfo.i64TotalPlayTimeSec_Character ); ++itor;
				(*itor).Pop( kPlayerInfo.byFiveElementBody );	++itor;//32
				(*itor).Pop( kPlayerInfo.kCoupleColorGuid );	++itor;
				(*itor).Pop( kPlayerInfo.kCoupleStatus );		++itor;
				(*itor).Pop( byCharactorSlot );		++itor;

				SAFE_STRNCPY(kPlayerInfo.szName, kName);

		//		캐릭 목록을 쥘 필요가 없다.

				PgDoc_Player* pkPlayer = new PgDoc_Player;//dynamic_cast<PgDoc_Player*>(g_kTotalObjMgr.CreateUnit(UT_PLAYER, kPlayerInfo.guidCharacter));
				
				SPlayerBasicInfo kBasicInfo;
				GET_DEF(PgClassDefMgr, kClassDefMgr);
				kClassDefMgr.GetAbil(SClassKey(kPlayerInfo.iClass, kPlayerInfo.wLv), kBasicInfo);

				SPlayerBinaryData kBinaryData;

				pkPlayer->Create( guidMember, kPlayerInfo, kBasicInfo, kBinaryData);
				pkPlayer->SetAbil(AT_CHARACTOR_SLOT, static_cast<int>(byCharactorSlot));
				vtPlayers.push_back(pkPlayer);

				SRecentPlayerNormalMapData RecentChannelData;
				double MapX, MapY, MapZ;
				short State;
				(*itor).Pop( RecentChannelData.ChannelNo );			++itor;
				(*itor).Pop( RecentChannelData.GroundNo );			++itor;
				(*itor).Pop( MapX );								++itor;	RecentChannelData.RecentPos.x = static_cast<float>(MapX);
				(*itor).Pop( MapY );								++itor;	RecentChannelData.RecentPos.y = static_cast<float>(MapY);
				(*itor).Pop( MapZ );								++itor;	RecentChannelData.RecentPos.z = static_cast<float>(MapZ);
				(*itor).Pop( State );								++itor;

				CONT_DEFMAP const * pDefMap = NULL;
				g_kTblDataMgr.GetContDef(pDefMap);
				if( NULL != pDefMap )
				{
					CONT_DEFMAP::const_iterator itor = pDefMap->find(pkPlayer->GetRecentMapNo(GKIND_DEFAULT));
					if( itor != pDefMap->end() )
					{
						if( (itor->second.iAttr & GKIND_EXPEDITION) || (itor->second.iAttr & GKIND_EXPEDITION_LOBBY) || (itor->second.iAttr & GKIND_EVENT) )
						{
							pkPlayer->SetRecentMapInfo(RecentChannelData.GroundNo, RecentChannelData.RecentPos);
						}
					}
				}
				pkPlayer->SetRecentNormalMap(RecentChannelData);
			}
			else
			{
				assert(false);
			}

			--iCharacterCount;
		}

		int iItemCount = (*return_iter);	++return_iter;

		while( iItemCount )
		{
			if(rkResult.vecArray.end() != itor )
			{
				SDBItemData kDBItem;
				
				(*itor).Pop(kDBItem.ItemGuid);		++itor;
				(*itor).Pop(kDBItem.OwnerGuid);		++itor;
				(*itor).Pop(kDBItem.InvType);		++itor;
				(*itor).Pop(kDBItem.InvPos);		++itor;
				(*itor).Pop(kDBItem.ItemNo);		++itor;

				(*itor).Pop(kDBItem.Count);			++itor;
				(*itor).Pop(kDBItem.Enchant_01);	++itor;
				(*itor).Pop(kDBItem.Enchant_02);	++itor;
				(*itor).Pop(kDBItem.Enchant_03);	++itor;
				(*itor).Pop(kDBItem.Enchant_04);	++itor;
				(*itor).Pop(kDBItem.State);			++itor;
				(*itor).Pop(kDBItem.CreateDate);	++itor;

				PgBase_Item kItem;
				SItemPos kItemPos;
				CastSDBItemDataToSItem(kDBItem, kItem, kItemPos);

				if(!kItem.IsUseTimeOut())						// 로그인 캐릭 선택 화면에 사용 기간 만료된 캐시 아이템이 나오지 않게 하자!!
				{
					kItemList.push_back(kDBItem);
				}
			}

			--iItemCount;
		}

		int iAccountItemCount = (*return_iter);	++return_iter;
		for (; iAccountItemCount > 0; --iAccountItemCount)
		{
			if(rkResult.vecArray.end() != itor )
			{
				(*itor).Pop(iDrakanCreateItemCount);	++itor;
			}
		}

		if(iJumping_EventNo>0)
		{
			int const iJumpingCharEventCount = (*return_iter);		++return_iter;

			if(g_kJumpingCharEventMgr.GetMaxRewardCount(iJumping_EventNo) > 0)
			{
				if(rkResult.vecArray.end() != itor )
				{
					(*itor).Pop(iJumpingCharEventRemainRewardCount);	++itor;
				}
			}
		}
	}

	if(iJumping_EventNo>0)
	{
		int const iJumping_LevelLimit = g_kJumpingCharEventMgr.GetLevelLimit(iJumping_EventNo);

		if(iJumpingCharEventRemainRewardCount <= 0)
		{
			iJumpingCharEventRemainRewardCount = g_kJumpingCharEventMgr.GetMaxRewardCount(iJumping_EventNo);
		}

		bool bCheckLevel = false;
		if(iJumping_LevelLimit>0)
		{
			for(PLAYER_UNIT_ARRAY::const_iterator player_it=vtPlayers.begin(); player_it!=vtPlayers.end(); ++player_it)
			{
				if((*player_it) && (*player_it)->GetAbil(AT_LEVEL)>=iJumping_LevelLimit)
				{
					bCheckLevel = true;
					break;
				}
			}
		}
		else
		{
			bCheckLevel = true;
		}

		if(bCheckLevel)
		{
			iJumpingCharEventNo = iJumping_EventNo;
		}
	}

	SCenterPlayer_Lock kLockPlayer;
	if (GetPlayerInfo(rkResult.QueryOwner(), true, kLockPlayer))
	{
		SCenterPlayerData* pkMemberData = kLockPlayer.pkCenterPlayerData;
		if(!pkMemberData)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false; 
		}
		if ( pkMemberData->kGetListQueryGuid != rkResult.QueryGuid() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		//INFO_LOG( BM::LOG_LV6, _T("[%s]-[%d]<20> Send Login User's Character Data to Switch"), __FUNCTIONW__, __LINE__ );
	{
		PLAYER_UNIT_ARRAY::iterator player_itor = vtPlayers.begin();
		
		UNIT_PTR_ARRAY kUnitArray;//위에서 push_back을 하므로 안지워지게 해야한다.
		while( player_itor != vtPlayers.end() )
		{
			kUnitArray.Add((*player_itor));
			++player_itor;
		}

		BM::Stream kPacket(PT_T_S_ANS_CHARACTER_LIST);

		pkMemberData->kSwitchServer.WriteToPacket(kPacket);//

		kPacket.Push( pkMemberData->guidMember );
		kUnitArray.WriteToPacket(kPacket, WT_SIMPLE|WT_OP_DBPlayerState);
		kPacket.Push(kItemList);

		short sFlag = SOF_NONE;
		if(IsUseLevelRank())
		{
			sFlag |= SOF_LEVELRANK;
		}
		kPacket.Push(sFlag);

		//프리미엄서비스 정보
		WriteToPacket_MEMBER_PREMIUM_SERVICE(pkMemberData->guidMember, kPacket);

		//점핑 캐릭터 이벤트
		if(iJumpingCharEventNo)
		{
			kPacket.Push(true);
			kPacket.Push(iJumpingCharEventNo);
			kPacket.Push(iJumpingCharEventRemainRewardCount);
		}
		else
		{
			kPacket.Push(false);
		}
		
		{
			int iDrakanMinLv = 20;
			g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Char_MinLvToCreateDrakan, iDrakanMinLv);
			kPacket.Push(iDrakanMinLv);
		}

		{
			int iDrakanCreateItemNo = 0;
			g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Char_CreateDrakanItemNo, iDrakanCreateItemNo);
			kPacket.Push(iDrakanCreateItemNo);
		}
		kPacket.Push(iDrakanCreateItemCount);
		//맞는 센터를 통해서 줘야함?
		CONT_SERVER_HASH kContServer;
		g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CENTER, kContServer);

		CONT_SERVER_HASH::const_iterator server_itor = kContServer.begin();
		while(server_itor != kContServer.end())
		{
			if((*server_itor).first.nChannel == nChannelNo)
			{
				g_kProcessCfg.Locked_SendToServer((*server_itor).first, kPacket);
				break;
			}
			++server_itor;
		}

		return true;
	}
	}
	else
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T("Not Find Login User MemberGuid[") << rkResult.QueryOwner().str().c_str() << _T("]") );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::ProcessItemQuery(	CEL::DB_DATA_ARRAY::const_iterator &db_itr
										,	CEL::DB_DATA_ARRAY::const_iterator const db_end_itr
										,	CEL::DB_RESULT_COUNT::const_iterator &count_itr
										,	PgInventory &rkInven
										,	CONT_SELECTED_CHARACTER_ITEM_LIST *pOutItemList )
{
	if ( db_itr == db_end_itr )
	{
		return false;
	}

	DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime();
	__int64 const i64CurrentMSTime = g_kEventView.GetLocalSecTime( CGameTime::MILLISECOND );

	CONT_SELECTED_CHARACTER_ITEM_LIST kItemList;

	// 1. Item Base
	int iItemCount = (*count_itr);
	while( (iItemCount-- > 0) && db_itr != db_end_itr )
	{
		SDBItemData kDBItem;

		db_itr->Pop(kDBItem.ItemGuid);		++db_itr;
		db_itr->Pop(kDBItem.OwnerGuid);		++db_itr;
		db_itr->Pop(kDBItem.InvType);		++db_itr;
		db_itr->Pop(kDBItem.InvPos);		++db_itr;
		db_itr->Pop(kDBItem.ItemNo);		++db_itr;

		db_itr->Pop(kDBItem.Count);			++db_itr;
		db_itr->Pop(kDBItem.Enchant_01);	++db_itr;
		db_itr->Pop(kDBItem.Enchant_02);	++db_itr;
		db_itr->Pop(kDBItem.Enchant_03);	++db_itr;
		db_itr->Pop(kDBItem.Enchant_04);	++db_itr;
		for (int i = 0; i < MAX_STAT_TRACK_ABIL; ++i)
		{
			db_itr->Pop(kDBItem.StatTrackType[i]);	++db_itr;
			db_itr->Pop(kDBItem.StatTrackValue[i]);	++db_itr;
		}
		db_itr->Pop(kDBItem.State);			++db_itr;
		db_itr->Pop(kDBItem.CreateDate);	++db_itr;

		PgBase_Item kItem;
		SItemPos kItemPos;
		CastSDBItemDataToSItem(kDBItem, kItem, kItemPos);

		kItemList.insert(std::make_pair(kItem.Guid(),PgItemWrapper(kItem,kItemPos)));
	}
	++count_itr;

	// 2. Unbind Info
	int const iUnbindCount = (*count_itr); 
	for(int i = 0;i < iUnbindCount;++i)
	{
		BM::GUID kItemGuid;
		BM::DBTIMESTAMP_EX kUnbindDate;
		db_itr->Pop(kItemGuid);			++db_itr;
		db_itr->Pop(kUnbindDate);		++db_itr;
		rkInven.AddUnbindDate( kItemGuid, kUnbindDate );
	}
	++count_itr;// unbind info

	// 3. ExpCard
	int const iExpCardCount = (*count_itr);
	for( int i = 0;i < iExpCardCount; ++i )
	{
		BM::GUID kItemGuid;
		__int64 i64Value = 0;
		short	kExpPer = 0;
		SExpCard kExpCard;
		db_itr->Pop(kItemGuid);			++db_itr;
		db_itr->Pop(i64Value);			++db_itr;
		kExpCard.CurExp(i64Value);
		db_itr->Pop(i64Value);			++db_itr;
		kExpCard.MaxExp(i64Value);
		db_itr->Pop(kExpPer);			++db_itr;
		kExpCard.ExpPer(kExpPer);

		CONT_SELECTED_CHARACTER_ITEM_LIST::iterator itr = kItemList.find(kItemGuid);
		if( itr != kItemList.end())
		{
			itr->second.Set(kExpCard);
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("GetItem Error !![I:") << kItemGuid << _T("-C:") << rkInven.OwnerGuid() << _T("]") );
		}
	}
	++count_itr;// exp card info

	// 4. Pet Item
	int iPetCount = (*count_itr);
	while( (iPetCount-- > 0) && db_itr != db_end_itr )
	{
		BM::GUID kPetID;
		SClassKey kPetClassKey;
		std::wstring wstrPetName;
		__int64 i64PetExp = 0i64;
		db_itr->Pop( kPetID );						++db_itr;
		db_itr->Pop( kPetClassKey.iClass );			++db_itr;
		db_itr->Pop( kPetClassKey.nLv );			++db_itr;
		db_itr->Pop( wstrPetName );					++db_itr;
		db_itr->Pop( i64PetExp );					++db_itr;

		PgItem_PetInfo *pkPetInfo = NULL;
		CONT_SELECTED_CHARACTER_ITEM_LIST::iterator dbitemiter = kItemList.find(kPetID);
		if(		dbitemiter != kItemList.end() 
			&&	true == (*dbitemiter).second.GetExtInfo( pkPetInfo )
			)
		{
			pkPetInfo->ClassKey( kPetClassKey );
			pkPetInfo->Name( wstrPetName );
			pkPetInfo->Exp( i64PetExp );

			int iValue = 0;
			iValue = 0;db_itr->Pop( iValue );++db_itr;pkPetInfo->SetAbil( AT_MP, iValue );
			iValue = 0;db_itr->Pop( iValue );++db_itr;pkPetInfo->SetState_Health( iValue );
			iValue = 0;db_itr->Pop( iValue );++db_itr;pkPetInfo->SetState_Mental( iValue );

			char cValue = 0;
			cValue = 0;db_itr->Pop( cValue );++db_itr;pkPetInfo->SetAbil( AT_COLOR_INDEX, static_cast<int>(cValue) );

			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::MINUTE );
			PgItem_PetInfo::SStateValue kStateValue;
			for ( size_t i = 0; i<PgItem_PetInfo::MAX_SKILL_SIZE; ++i )
			{
				db_itr->Pop( kStateValue );++db_itr;
				__int64 const i64StateTime = static_cast<__int64>(kStateValue.Time()) + PgItem_PetInfo::SKILL_BASE_TIME;
				if ( i64NowTime < i64StateTime )
				{
					pkPetInfo->SetSkill( i, kStateValue );
				}
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("GetItem Error !![I:") << kPetID << _T("-C:") << rkInven.OwnerGuid() << _T("]") );

			int iValue;
			db_itr->Pop( iValue );++db_itr;//MP
			db_itr->Pop( iValue );++db_itr;//Health
			db_itr->Pop( iValue );++db_itr;//Mental

			char cValue;
			db_itr->Pop( cValue );++db_itr;//Color

			for ( size_t i = 0; i<PgItem_PetInfo::MAX_SKILL_SIZE; ++i )
			{
				db_itr->Pop( iValue );++db_itr;
			}
		}		
	}
	++count_itr;// pet data

	// 5. 기간제 몬스터 카드 정보
	int const iMonsterCount = (*count_itr);
	for( int i = 0;i < iMonsterCount; ++i )
	{
		BM::GUID kItemGuid;
		BM::DBTIMESTAMP_EX kLimitTime;
		SMonsterCardTimeLimit kMonsterCard;
		db_itr->Pop(kItemGuid);			++db_itr;
		db_itr->Pop(kLimitTime);		++db_itr;
		kMonsterCard.LimitTime(static_cast<BM::PgPackedTime>(kLimitTime));

		CONT_SELECTED_CHARACTER_ITEM_LIST::iterator itr = kItemList.find(kItemGuid);
		if( itr != kItemList.end())
		{
			itr->second.Set(kMonsterCard);
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("GetItem Error !![I:") << kItemGuid << _T("-C:") << rkInven.OwnerGuid() << _T("]") );
		}
	}
	++count_itr;// exp card info

	rkInven.InsertItemList( kItemList );
	
	if ( pOutItemList )
	{
		kItemList.swap( *pOutItemList );
	}

	return true;
}

void PgRealmUserManager::UpdateAchievementPoint(PgDoc_Player * pkPlayer)
{
	CONT_ACHIEVEMENT_POINT kResult;
	int iTotalPoint = 0;
	int iCompleteCount = 0;// 완료한 업적 개수

	CONT_DEF_CONT_ACHIEVEMENTS const *pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);
	if(!pkContDef)
	{
		return;
	}

	for(CONT_DEF_CONT_ACHIEVEMENTS::const_iterator iter = pkContDef->begin();iter != pkContDef->end(); ++iter)
	{
		CONT_DEF_CONT_ACHIEVEMENTS::mapped_type const & kCont = (*iter).second;
		PgAchievements const * pkAchievement = pkPlayer->GetAchievements();

		for(CONT_DEF_CONT_ACHIEVEMENTS::mapped_type::const_iterator a_iter = kCont.begin();a_iter != kCont.end();++a_iter)
		{	
			CONT_DEF_CONT_ACHIEVEMENTS::mapped_type::mapped_type const & kElement = (*a_iter).second;
			if(false == pkAchievement->IsComplete(kElement.iSaveIdx))
			{
				continue;
			}
			kResult[kElement.iCategory] += kElement.iRankPoint;
			iTotalPoint += kElement.iRankPoint;
			++iCompleteCount;
		}
	}

	{// 업적 모은 개수
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(pkPlayer->GetID());
		pkActionOrder->kCause = CAE_Achievement;
		pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL,pkPlayer->GetID(),SPMOD_AddAbil(AT_ACHIEVEMENT_COLLECT_COUNT, iCompleteCount)));
		g_kJobDispatcher.VPush(pkActionOrder);
	}

	for(CONT_ACHIEVEMENT_POINT::const_iterator iter = kResult.begin();iter != kResult.end();++iter)
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Set_UserAchievement]");
		kQuery.InsertQueryTarget(pkPlayer->GetID());
		kQuery.PushStrParam(pkPlayer->GetID());
		kQuery.PushStrParam((*iter).first);
		kQuery.PushStrParam((*iter).second);
		g_kCoreCenter.PushQuery(kQuery);
	}

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Set_UserAchievementTotal]");
	kQuery.InsertQueryTarget(pkPlayer->GetID());
	kQuery.PushStrParam(pkPlayer->GetID());
	kQuery.PushStrParam(iTotalPoint);
	g_kCoreCenter.PushQuery(kQuery);
}

bool PgRealmUserManager::Locked_Q_DQT_SELECT_PLAYER_DATA(CEL::DB_RESULT &rkResult)
{
	tagSafePlayer kSmartPtr_Player;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	CEL::DB_RESULT_COUNT::const_iterator count_itor = rkResult.vecResultCount.begin();

	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		if(count_itor == rkResult.vecResultCount.end())
		{
			return true;
		}

		if( rkResult.vecArray.end() == itor )
		{
			return true;
		}

		++count_itor;	// pass tutorial
		
		BYTE cPassTutorial = 0;
		DBTIMESTAMP kDate;
		BYTE byClass;
		BM::GUID guidMember;
		SPlayerDBData kPlayerInfo;
		SPlayerBinaryData kBinaryData;
		SPlayerDateContents& rkDateContents = kPlayerInfo.kDateContents;
		std::wstring kName;
		double x = 0, y = 0, z = 0;
		BYTE abyItemGroupCoolTime[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE];
		
		(*itor).Pop( cPassTutorial );					++itor;

		if( rkResult.vecArray.end() == itor )
		{
			// 중간에 Character가 block 되거나, 삭제되었을 경우 캐릭터가 없을 수 있다.
			return true;
		}
		if( (count_itor+1) == rkResult.vecResultCount.end() )
		{
			// @@ROWCOUNT <> 1 경우 Return 99 오류
			return true;
		}

		// 캐릭터 정보 시작
		(*itor).Pop( guidMember );						++itor;
		(*itor).Pop( kPlayerInfo.guidCharacter );		++itor;
		(*itor).Pop( kPlayerInfo.byState );				++itor;
		(*itor).Pop( kName );							++itor;
		(*itor).Pop( kPlayerInfo.byGender );			++itor;//5



		if ( g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_SelectCharacter) && guidMember != rkResult.QueryOwner() )
		{
			// 이 새끼 이거 해킹인데...
			//VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[HACKING][SelectCharacter] SelectCharacter Hacking MemberGuid<") << rkResult.QueryOwner() << _T("> Character<") << kName << _T("/") << kPlayerInfo.guidCharacter << _T(">'s OwnerMemberGuid<") << guidMember << _T(">") );
			HACKING_LOG( BM::LOG_LV5, _T("[HACKING][SelectCharacter] SelectCharacter Hacking MemberGuid[") << rkResult.QueryOwner() << _T("] USER[") << kName << _T("] GUID[") << kPlayerInfo.guidCharacter << _T("]'s OwnerMemberGuid<") << guidMember << _T(">") );
			return true;
		}

		// 오로지 Normal만 가능!
		if( CIDBS_Normal != kPlayerInfo.byState )
		{
			BM::Stream kPacket(PT_T_C_ANS_SELECT_CHARACTER);
			kPacket.Push( static_cast< ESelectCharacterRet >(E_SCR_ErrorState) );
			kPacket.Push( guidMember );
			SendToSwitch(guidMember, kPacket);

			//VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[HACKING][SelectCharacter] SelectCharacter Hacking MemberGuid<") << rkResult.QueryOwner() << _T("> Character<") << kName << _T("/") << kPlayerInfo.guidCharacter << _T(">'s OwnerMemberGuid<") << guidMember << _T(">, Can't select player DB State[") << kPlayerInfo.byState << L"]" );
			HACKING_LOG( BM::LOG_LV5, _T("[HACKING][SelectCharacter] SelectCharacter Hacking MemberGuid[") << rkResult.QueryOwner() << _T("] USER[") << kName << _T("] GUID[") << kPlayerInfo.guidCharacter << _T("]'s OwnerMemberGuid<") << guidMember << _T(">, Can't select player DB State[") << kPlayerInfo.byState << L"]" );
			return true;
		}

		(*itor).Pop( kPlayerInfo.iRace );				++itor;
		// Class 정보는 1byte로 DB에 저장되어 있다.
		(*itor).Pop( byClass );							++itor;	kPlayerInfo.iClass = byClass;
		(*itor).Pop( kPlayerInfo.wLv );					++itor;
		(*itor).Pop( kPlayerInfo.i64Exp );				++itor;
		(*itor).Pop( kPlayerInfo.i64Money );			++itor;//10

		(*itor).Pop( kPlayerInfo.iHP );					++itor;
		(*itor).Pop( kPlayerInfo.iMP );					++itor;
		(*itor).Pop( kPlayerInfo.iCP );					++itor;
		(*itor).Pop( kPlayerInfo.sSP );					++itor;
		(*itor).Pop( kBinaryData.abySkills );			++itor;//15

		(*itor).Pop( kBinaryData.abySkillExtends );		++itor;		
		(*itor).Pop( kPlayerInfo.Strategy_fatigability); ++itor;
		(*itor).Pop( kPlayerInfo.iHairColor );			++itor;
		(*itor).Pop( kPlayerInfo.iHairStyle );			++itor;
		(*itor).Pop( kPlayerInfo.iFace );				++itor;//20
		//20
		(*itor).Pop( kPlayerInfo.iJacket );				++itor;
		(*itor).Pop( kPlayerInfo.iPants );				++itor;
		(*itor).Pop( kPlayerInfo.iShoes );				++itor;
		(*itor).Pop( kPlayerInfo.iGloves );				++itor;
		(*itor).Pop( kPlayerInfo.kCoupleGuid );			++itor;//25

		(*itor).Pop( kPlayerInfo.kGuildGuid );			++itor;
		(*itor).Pop( kPlayerInfo.kRecentInfo.iMapNo );	++itor;

		(*itor).Pop( x );								++itor;	kPlayerInfo.kRecentInfo.ptPos3.x = static_cast<float>(x);
		(*itor).Pop( y );								++itor;	kPlayerInfo.kRecentInfo.ptPos3.y = static_cast<float>(y);
		(*itor).Pop( z );								++itor;	kPlayerInfo.kRecentInfo.ptPos3.z = static_cast<float>(z);
		
		(*itor).Pop( kPlayerInfo.kLastVillage.iMapNo );	++itor;
		(*itor).Pop( x );								++itor;	kPlayerInfo.kLastVillage.ptPos3.x = static_cast<float>(x);
		(*itor).Pop( y );								++itor;	kPlayerInfo.kLastVillage.ptPos3.y = static_cast<float>(y);
		(*itor).Pop( z );								++itor;	kPlayerInfo.kLastVillage.ptPos3.z = static_cast<float>(z);

		(*itor).Pop( kBinaryData.abyIngQuest );			++itor;//35
		(*itor).Pop( kBinaryData.abyEndQuest );			++itor;
		(*itor).Pop( kBinaryData.abyEndQuest2 );		++itor;
		(*itor).Pop( kBinaryData.abyEndQuestExt );		++itor;
		(*itor).Pop( kBinaryData.abyEndQuestExt2 );		++itor;


		(*itor).Pop( kBinaryData.abyQuickSlot );		++itor;// 40
		(*itor).Pop( kDate );							++itor;	kPlayerInfo.kBirthDate = kDate; 
		(*itor).Pop( kPlayerInfo.i64TotalPlayTimeSec_Character ); ++itor;
		(*itor).Pop( kPlayerInfo.byFiveElementBody );	++itor;
		(*itor).Pop( kPlayerInfo.i64Fran );				++itor;

		(*itor).Pop( kPlayerInfo.sTacticsLevel );		++itor;// 45
		(*itor).Pop( kPlayerInfo.i64TacticsExp );		++itor; 

		(*itor).Pop( kPlayerInfo.iRecommendPoint );		++itor;

		(*itor).Pop( kPlayerInfo.kRefreshDate );		++itor;
		(*itor).Pop( kBinaryData.abyEffect );			++itor;

		(*itor).Pop( kBinaryData.abyEmoticon );			++itor;	// 50
		(*itor).Pop( kBinaryData.abyEmotion );			++itor;	
		(*itor).Pop( kBinaryData.abyBalloon );			++itor;
		(*itor).Pop( kPlayerInfo.sBonusStatus );		++itor;
		(*itor).Pop( kPlayerInfo.sAddedStr );			++itor;	
		(*itor).Pop( kPlayerInfo.sAddedInt  );			++itor;	// 55	
		(*itor).Pop( kPlayerInfo.sAddedCon  );			++itor; 
		(*itor).Pop( kPlayerInfo.sAddedDex  );			++itor;
		(*itor).Pop( rkDateContents.kGuildLeaveDate  );	++itor;	
		(*itor).Pop( abyItemGroupCoolTime );			++itor; // 60
		(*itor).Pop( kBinaryData.dwJobSkillHistoryItem );	++itor; 

		SAFE_STRNCPY(kPlayerInfo.szName, kName);

		PgDoc_Player* pkPlayer = new PgDoc_Player;//dynamic_cast<PgDoc_Player*>(g_kTotalObjMgr.CreateUnit(UT_PLAYER, kPlayerInfo.guidCharacter));
#ifdef _USE_JOBCHECKER
		pkPlayer->CreateJobChecker();
#endif
		kSmartPtr_Player.SetPlayer(pkPlayer);
	
		GET_DEF(PgClassDefMgr, kClassDefMgr);

		bool const bPassTutorial = 0 != cPassTutorial;
		bool const bIsNewbiePlayer = PgPlayerUtil::IsNewbiePlayer(kPlayerInfo);
		if( bIsNewbiePlayer
		&&	bPassTutorial )
		{
			kBinaryData.ClearQuest();
		}

		BM::Stream kPremiumCustomData;
		(*itor).Pop( kPremiumCustomData.Data() );		++itor;
		kPremiumCustomData.PosAdjust();
		
		PgPremiumMgr kPremiumMgr;
		CONT_MEMBER_PREMIUM::iterator premium_it = m_kContMemberPremium.find(guidMember);
		if(premium_it!=m_kContMemberPremium.end())
		{
			(*premium_it).second.kCustomData = kPremiumCustomData;
			kPremiumMgr.SetService((*premium_it).second.iServiceNo, kPremiumCustomData);
			kPremiumMgr.StartDate((*premium_it).second.kStartDate);
			kPremiumMgr.EndDate((*premium_it).second.kEndDate);
		}

		SPlayerBasicInfo kBasicInfo;
		kClassDefMgr.GetAbil(SClassKey(kPlayerInfo.iClass, kPlayerInfo.wLv), kBasicInfo);
		pkPlayer->Create( guidMember, kPlayerInfo, kBasicInfo, kBinaryData);
		//
		{
			PgContLogMgr kContLogMgr;

			// 종료 퀘스트 마이그레이션
			pkPlayer->EndQuestMigration(bIsNewbiePlayer);

			//
			pkPlayer->RemoveNullQuest(kContLogMgr);
			pkPlayer->BuildGroupQuest(kContLogMgr);
			pkPlayer->BuildLoopQuest(kContLogMgr);

			//kContLogMgr.SendLog();
		}
		
		bool bRet = false;
		SPvPReport kPvPReport;
		PgPlayer_MissionReport	kMissionReport;

		// Load PvP Report
		itor->Pop(kPvPReport.m_iPoint);							++itor;
		itor->Pop(kPvPReport.m_iRecords[GAME_WIN]);				++itor;
		itor->Pop(kPvPReport.m_iRecords[GAME_DRAW]);			++itor;// 65
		itor->Pop(kPvPReport.m_iRecords[GAME_LOSE]);			++itor;
		itor->Pop(kPvPReport.m_iKill);							++itor; 
		itor->Pop(kPvPReport.m_iDeath);							++itor;
		itor->Pop(kPvPReport.m_iRecords[GAME_WIN_EXERCISE]);	++itor;
		itor->Pop(kPvPReport.m_iRecords[GAME_DRAW_EXERCISE]);	++itor;// 70
		itor->Pop(kPvPReport.m_iRecords[GAME_LOSE_EXERCISE]);	++itor;

		PgUserMapInfo<DWORD> kMapInfo;
		if( kMapInfo.LoadDB( itor++ ) )
		{
			kSmartPtr_Player()->SetWorldMap( kMapInfo );

			size_t const iSize = sizeof(kSmartPtr_Player()->m_kClientOption);
			if( itor->PopMemory( &(kSmartPtr_Player()->m_kClientOption), iSize))
			{
				++itor;
				kMissionReport.Load( SECTION_MISSIONAREA_CB1, itor );
				kMissionReport.Load( SECTION_MISSIONAREA_CB2, itor );	

				kSmartPtr_Player()->GetAchievements()->LoadDBAchievement(itor);

				// Ranking Point
				int iMaxComboCount = 0;
				itor->Pop(iMaxComboCount);					++itor;		
				kSmartPtr_Player()->SetAbil( AT_MAX_COMBO, iMaxComboCount );

				BM::DBTIMESTAMP_EX kRentalDate1,kRentalDate2,kRentalDate3;
				itor->Pop(kRentalDate1);					++itor;
				itor->Pop(kRentalDate2);					++itor;
				itor->Pop(kRentalDate3);					++itor; // 80

				BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE];
				itor->Pop(abyInvExtern);					++itor;

				int iMissionEvent = 0;
				itor->Pop(iMissionEvent);					++itor;		

				BYTE abyExternIdx[MAX_DB_INVEXTEND_SIZE];
				itor->Pop(abyExternIdx);					++itor;

				short sStreetNo = 0;
				int iHouseNo = 0;
				itor->Pop(sStreetNo);						++itor;
				itor->Pop(iHouseNo);						++itor; // 85

				itor->Pop( kPlayerInfo.kCoupleColorGuid );	++itor;
				kSmartPtr_Player()->ForceSetCoupleColorGuid(kPlayerInfo.kCoupleColorGuid);
				BYTE bExtendCharacterNum = 0;
				BM::DBTIMESTAMP_EX kShareRentalDate1,kShareRentalDate2,kShareRentalDate3,kShareRentalDate4;
				
				// 여긴 랠름 동일 계정에서 공통으로 사용하는 정보임
				itor->Pop( bExtendCharacterNum );			++itor;	
				itor->Pop( kShareRentalDate1 );				++itor;
				itor->Pop( kShareRentalDate2 );				++itor;
				itor->Pop( kShareRentalDate3 );				++itor; // 90
				itor->Pop( kShareRentalDate4 );				++itor;

				kSmartPtr_Player()->GetHiddenOpen()->LoadDB(itor); 

				BM::DBTIMESTAMP_EX kLastHiddenUpdate;
				itor->Pop( kLastHiddenUpdate );				++itor;

				BYTE bySkillSetItemCount = 0;
				itor->Pop( bySkillSetItemCount );			++itor;
				kSmartPtr_Player()->SetAbil(AT_SKILLSET_USE_COUNT, bySkillSetItemCount);

				BYTE byTabOpen = 0;
				BYTE byTabNo = 0;
				(*itor).Pop( byTabOpen );					++itor;// 95
				(*itor).Pop( byTabNo );						++itor;
				kSmartPtr_Player()->SetAbil(AT_STRATEGYSKILL_OPEN, byTabOpen);
				kSmartPtr_Player()->SetAbil(AT_STRATEGYSKILL_TABNO, byTabNo);

				SPlayerStrategySkillData kStrategySkillData;
				(*itor).Pop( kStrategySkillData.sSP );					++itor; 
				(*itor).Pop( kStrategySkillData.abySkills );			++itor;
				(*itor).Pop( kStrategySkillData.abySkillExtends );		++itor;
				(*itor).Pop( kStrategySkillData.abyQuickSlot );			++itor;
				kSmartPtr_Player()->StrategySkillInit(kStrategySkillData);

				SRecentPlayerNormalMapData RecentChannelData;
				double MapX, MapY, MapZ;
				short State;
				(*itor).Pop( RecentChannelData.ChannelNo );			++itor;
				(*itor).Pop( RecentChannelData.GroundNo );			++itor;
				(*itor).Pop( MapX );								++itor;	RecentChannelData.RecentPos.x = static_cast<float>(MapX);
				(*itor).Pop( MapY );								++itor;	RecentChannelData.RecentPos.y = static_cast<float>(MapY);
				(*itor).Pop( MapZ );								++itor;	RecentChannelData.RecentPos.z = static_cast<float>(MapZ);
				(*itor).Pop( State );								++itor;

				// 유저가 선택할 수 없는 채널에서 비정상 종료 되었을 경우 위치를 정상적으로 되돌려 주어야 함.
				if( SHRT_MIN == State )
				{
					// State는 정상적으로 원정대 채널이동이 성공 했을 경우에만 SHRT_MAX로 세팅된다.
					// 그 외에는 SHRT_MIN을 유지하고 있다.
					CONT_DEFMAP const * pDefMap = NULL;
					g_kTblDataMgr.GetContDef(pDefMap);
					if( NULL != pDefMap )
					{
						CONT_DEFMAP::const_iterator itor = pDefMap->find(kSmartPtr_Player()->GetRecentMapNo(GKIND_DEFAULT));
						if( itor != pDefMap->end() )
						{
							if( (itor->second.iAttr & GKIND_EXPEDITION) || (itor->second.iAttr & GKIND_EXPEDITION_LOBBY) || (itor->second.iAttr & GKIND_EVENT) )
							{
								kSmartPtr_Player()->SetRecentMapInfo(RecentChannelData.GroundNo, RecentChannelData.RecentPos);
							}
						}
					}
				}
				else
				{
					// 다시 되돌려놔야 함.
					CEL::DB_QUERY Query( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_RecentChannelState]");
					Query.InsertQueryTarget(kSmartPtr_Player()->GetID());
					Query.PushStrParam(kSmartPtr_Player()->GetID());

					Query.PushStrParam( SHRT_MIN );
					g_kCoreCenter.PushQuery( Query );
				}
				kSmartPtr_Player()->SetRecentNormalMap(RecentChannelData);
								
				kSmartPtr_Player()->ExtendCharacterNum(bExtendCharacterNum);
				kSmartPtr_Player()->HomeAddr(SHOMEADDR(sStreetNo, iHouseNo));

				SMYHOME kMyHome;
				if((false == kSmartPtr_Player()->HomeAddr().IsNull()) && (false == m_kMyHomeMgr.GetHome(sStreetNo, iHouseNo, kMyHome)))
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LOAD_MYHOME_INFO,L"EXEC [dbo].[up_LoadMyHomeInfo]");
					kQuery.InsertQueryTarget(kSmartPtr_Player()->GetID());
					kQuery.QueryOwner(kSmartPtr_Player()->GetID());
					kQuery.PushStrParam(sStreetNo);
					kQuery.PushStrParam(iHouseNo);
					kQuery.contUserData.Push(sStreetNo);
					kQuery.contUserData.Push(iHouseNo);
					kQuery.contUserData.Push(false);
					g_kCoreCenter.PushQuery(kQuery);
				}

				kSmartPtr_Player()->SetAbil( AT_MISSION_EVENT, iMissionEvent );

				kSmartPtr_Player()->SetExternDB( kPvPReport, kMissionReport );

				++count_itor;	// character data

				
				{// 현상수배 퀘스트 완료 카운트
					int iQuestID = 0, iClearCount = 0;
					ContQuestClearCount kContWantedQuestClearCount;

					int const iRows = *count_itor; // SELECT 결과 행의 수
					for( int iCur = 0; iRows > iCur; ++iCur )
					{
						itor->Pop(iQuestID);		++itor;
						itor->Pop(iClearCount);		++itor;
						
						kContWantedQuestClearCount.insert( std::make_pair( iQuestID, iClearCount ) );
					}

					kSmartPtr_Player()->SetWantedQuestClearCount(kContWantedQuestClearCount);
				}
				++count_itor; // 현상 수배 퀘스트

				PgInventory *pkInv = kSmartPtr_Player()->GetInven();
				pkInv->Init( abyInvExtern, abyExternIdx );//인벤 셋팅.
				pkInv->OwnerName(kName);

				CONT_SELECTED_CHARACTER_ITEM_LIST kItemList;

				// 캐릭터 전용 아이템 데이터 읽기
				{
					CONT_SELECTED_CHARACTER_ITEM_LIST kTmpItemList;
					ProcessItemQuery( itor, rkResult.vecArray.end(), count_itor, *pkInv, &kTmpItemList );
					kItemList.insert(kTmpItemList.begin(),kTmpItemList.end());
				}

				{
					CONT_SELECTED_CHARACTER_ITEM_LIST kTmpItemList;
					ProcessItemQuery( itor, rkResult.vecArray.end(), count_itor, *pkInv, &kTmpItemList );
					kItemList.insert(kTmpItemList.begin(),kTmpItemList.end());
				}

				pkInv->SetRentalSafeEnableTime(IT_RENTAL_SAFE1,kRentalDate1);
				pkInv->SetRentalSafeEnableTime(IT_RENTAL_SAFE2,kRentalDate2);
				pkInv->SetRentalSafeEnableTime(IT_RENTAL_SAFE3,kRentalDate3);

				pkInv->SetRentalSafeEnableTime(IT_SHARE_RENTAL_SAFE1,kShareRentalDate1);
				pkInv->SetRentalSafeEnableTime(IT_SHARE_RENTAL_SAFE2,kShareRentalDate2);
				pkInv->SetRentalSafeEnableTime(IT_SHARE_RENTAL_SAFE3,kShareRentalDate3);
				pkInv->SetRentalSafeEnableTime(IT_SHARE_RENTAL_SAFE4,kShareRentalDate4);

				int const iAchievementCount = (*count_itor);
				int iAchievementType = 0,
					iAchievementValue = 0;

				for(int i = 0;i < iAchievementCount;i++)
				{
					itor->Pop(iAchievementType);			++itor;
					itor->Pop(iAchievementValue);			++itor;
					kSmartPtr_Player()->SetAbil(iAchievementType,iAchievementValue);
				}

				++count_itor;	// achievement data

				int const iAchievementTimeLimit = (*count_itor);

				kSmartPtr_Player()->GetAchievements()->LoadDBAchievementTimeLimit(itor, iAchievementTimeLimit);

				++count_itor;

				int const iAchievementPointCount = (*count_itor); // achievement point
				kSmartPtr_Player()->GetAchievements()->LoadDBAchievementPoint(itor, iAchievementPointCount);

				++count_itor;	// character data

				CONT_USER_PORTAL kContPortal;
				CONT_USER_PORTAL::mapped_type kPortal;
				BM::GUID kPortalGuid;

				int const iPortalCount	= (*count_itor); ++count_itor;	// user portal 

				for(int i = 0;i < iPortalCount;++i)
				{
					(*itor).Pop(kPortalGuid);		++itor;
					(*itor).Pop(kPortal.kComment);	++itor;
					(*itor).Pop(kPortal.iGroundNo);	++itor;
					double DBFLOAT = 0.0;
					(*itor).Pop(DBFLOAT);	++itor;
					kPortal.kPos.x = static_cast<float>(DBFLOAT);
					(*itor).Pop(DBFLOAT);	++itor;
					kPortal.kPos.y = static_cast<float>(DBFLOAT);
					(*itor).Pop(DBFLOAT);	++itor;
					kPortal.kPos.z = static_cast<float>(DBFLOAT);
					kContPortal.insert(std::make_pair(kPortalGuid,kPortal));
				}

				kSmartPtr_Player()->ContPortal(kContPortal);

				int const iHasCard		= (*count_itor); ++count_itor;	// character card info

				if( iHasCard )
				{
					PgCharacterCard kCard;
					if( true == kCard.LoadDB(itor))
					{
						kCard.Name(kSmartPtr_Player()->Name());
						kSmartPtr_Player()->IsCreateCard(true);
						CONT_CARD_ABIL::mapped_type kAbil;
						if(true == kCard.GetCardAbil(kAbil))
						{
							// 혹시라도 여러개 만들걸 대비 하자. 안하면 ㅠㅠ;; 
							VEC_CARD_ABIL kContAbil;
							kContAbil.push_back(kAbil);
							kSmartPtr_Player()->SetCardAbil(kContAbil);
						}
						m_kCardMgr.Locked_RegCard(kCard);
					}
				}

				int const iSkillCount	= (*count_itor); ++count_itor;	//SkillSet

				CONT_USER_SKILLSET kContSkillSet;
				CONT_USER_SKILLSET kContSkillSet_Sec;
				for(int i=0; i<iSkillCount; ++i)
				{
					CONT_USER_SKILLSET::mapped_type element;
					BYTE byTabNo = 0;
					(*itor).Pop(element.bySetNo);		++itor;
					(*itor).Pop(byTabNo);				++itor;
					for(int j=0; j<MAX_SKILLSET_GROUP; ++j)
					{
						(*itor).Pop(element.byDelay[j]);	++itor;
						(*itor).Pop(element.byType[j]);		++itor;
						(*itor).Pop(element.iSkillNo[j]);	++itor;
					}

					if(ESTT_BASIC==byTabNo)
					{
						kContSkillSet.insert(std::make_pair(element.bySetNo,element));
					}
					else if(ESTT_SECOND==byTabNo)
					{
						kContSkillSet_Sec.insert(std::make_pair(element.bySetNo,element));
					}
				}
				kSmartPtr_Player()->StrategySkillSetInit(kContSkillSet, kContSkillSet_Sec);

				// 아래 두 개는 접속 시간 설정후에 사용.
				short IsKeepConn = 0;
				BM::DBTIMESTAMP_EX LastRewardTime;

				{
					BM::CAutoMutex kLock( m_kMutex, true );
					int const iSecondRows = (*count_itor); ++count_itor;
					{	// 로그인 후에 일정 시간 접속 시간 유지 할 때 아이템 지급하는 이벤트에서, 현재 캐릭터가 받을 수 있는 잔여 아이템 지급 횟수.
						BM::PgPackedTime CurTime;
						CurTime.SetLocalTime();

						if( iSecondRows > 0 )
						{
							short LastRewardCount = 0;
							itor->Pop(LastRewardCount); ++itor;

							itor->Pop(LastRewardTime);	++itor;
							itor->Pop(IsKeepConn);		++itor;

							BM::PgPackedTime NextTime(LastRewardTime); // 다음에 지급 받을 수 있는 시간( 마지막으로 받은 시간의 다음날 0시 )						
							CGameTime::AddTime( NextTime, CGameTime::OneDay );
							NextTime.Hour(0), NextTime.Min(0), NextTime.Sec(0);

							if( CurTime < NextTime )
							{
								// 남은 보상 횟수.
								kSmartPtr_Player()->SetAbil(AT_SPECIFIC_REWARD_EVENT, static_cast<int>(LastRewardCount));

								SEventRwardTimeInfo TimeInfo;
								TimeInfo.PlayerGuid = kSmartPtr_Player()->GetID();	// CharGuid.
								TimeInfo.RewardCount = LastRewardCount; // 보상횟수.

								if( IsKeepConn != 0 )
								{	// 일반 채널 이동 접속.
									__int64 LastRewardSec = 0;
									CGameTime::DBTimeEx2SecTime(LastRewardTime, LastRewardSec, CGameTime::SECOND);

									TimeInfo.LastRewardTime = LastRewardSec;	// 이전보상시간.

									CEL::DB_QUERY Query( DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_SpecificRewardKeepConn]" );
									Query.InsertQueryTarget(kSmartPtr_Player()->GetID());						
									Query.PushStrParam( static_cast<int>(AT_SPECIFIC_REWARD_EVENT) );
									Query.PushStrParam( kSmartPtr_Player()->GetID() );
									Query.PushStrParam( static_cast<short>(0) );
									g_kCoreCenter.PushQuery(Query);
								}
								else
								{	// 그냥 접속.
									TimeInfo.LastRewardTime = g_kEventView.GetLocalSecTime(CGameTime::SECOND);	// 이전보상시간.
								}

								m_ContEventRewardTimeInfo.insert( std::make_pair( TimeInfo.PlayerGuid, TimeInfo ) );
							}
							else
							{
								BM::DBTIMESTAMP_EX NowTime(CurTime);

								CEL::DB_QUERY Query( DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_SpecificReward2]" );
								Query.InsertQueryTarget(kSmartPtr_Player()->GetID());						
								Query.PushStrParam( static_cast<int>(AT_SPECIFIC_REWARD_EVENT) );
								Query.PushStrParam( kSmartPtr_Player()->GetID() );						
								Query.PushStrParam( NowTime );
								Query.PushStrParam( 0 );
								g_kCoreCenter.PushQuery(Query);

								kSmartPtr_Player()->SetAbil(AT_SPECIFIC_REWARD_EVENT, 0);

								SEventRwardTimeInfo TimeInfo;
								TimeInfo.PlayerGuid = kSmartPtr_Player()->GetID();	// CharGuid.
								TimeInfo.LastRewardTime = g_kEventView.GetLocalSecTime(CGameTime::SECOND);	// 이전보상시간.
								TimeInfo.RewardCount = 0; // 보상회수.

								m_ContEventRewardTimeInfo.insert( std::make_pair( TimeInfo.PlayerGuid, TimeInfo ) );
							}
						}
						else
						{	// 지급 받은 적이 없을 때는 접속하는 시간을 기록.
							BM::DBTIMESTAMP_EX NowTime(CurTime);

							CEL::DB_QUERY Query( DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_SpecificReward2]" );
							Query.InsertQueryTarget(kSmartPtr_Player()->GetID());						
							Query.PushStrParam( static_cast<int>(AT_SPECIFIC_REWARD_EVENT) );
							Query.PushStrParam( kSmartPtr_Player()->GetID() );						
							Query.PushStrParam( NowTime );
							Query.PushStrParam( 0 );
							g_kCoreCenter.PushQuery(Query);

							kSmartPtr_Player()->SetAbil(AT_SPECIFIC_REWARD_EVENT, 0);

							SEventRwardTimeInfo TimeInfo;
							TimeInfo.PlayerGuid = kSmartPtr_Player()->GetID();	// CharGuid.
							TimeInfo.LastRewardTime = g_kEventView.GetLocalSecTime(CGameTime::SECOND);	// 이전보상시간.
							TimeInfo.RewardCount = 0; // 보상회수.

							m_ContEventRewardTimeInfo.insert( std::make_pair( TimeInfo.PlayerGuid, TimeInfo ) );
						}
					}
				}

				int const iRows = (*count_itor); ++count_itor;
				{// 로그인 일정시간 유지 시 지급되는 아이템을 마지막으로 받은 시간
					if( iRows > 0 )
					{
						BM::DBTIMESTAMP_EX kLastRewardTime;
						itor->Pop(kLastRewardTime); ++itor;
						
						BM::PgPackedTime kNextTime(kLastRewardTime); // 다음에 지급 받을 수 있는 시간( 마지막으로 받은 시간의 다음날 0시 )						
						CGameTime::AddTime( kNextTime, CGameTime::OneDay );
						kNextTime.Hour(0), kNextTime.Min(0), kNextTime.Sec(0);

						BM::PgPackedTime kCurTime;
						kCurTime.SetLocalTime();

						if( kCurTime < kNextTime )
						{// 오늘은 받을 수 없다.
							kSmartPtr_Player()->SetAbil(AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN, 0);
						}
						else
						{
							kSmartPtr_Player()->SetAbil(AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN, 1);
						}
					}
					else
					{// 마지막으로 받은시간이 없다면 무조건 오늘은 받을 수 있음.
						kSmartPtr_Player()->SetAbil(AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN, 1);
					}
				}

				int iHavePenalty = (*count_itor); ++count_itor;// Penalty info
				++count_itor;// 

				BM::DBTIMESTAMP_EX kNowTime;
				g_kEventView.GetLocalTime( kNowTime );

				while ( iHavePenalty-- )
				{
					int iType = 0;
					int iValue = 0;
					BM::DBTIMESTAMP_EX kEndTime;
					itor->Pop( iType );		++itor;
					itor->Pop( iValue );	++itor;
					itor->Pop( kEndTime );	++itor;

					if ( kNowTime < kEndTime )
					{
						kSmartPtr_Player()->SetAbil( static_cast<WORD>(iType), iValue );
					}
					else
					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_PENALTY, L"EXEC [dbo].[up_DeletePenalty]" );
						kQuery.InsertQueryTarget( kPlayerInfo.guidCharacter );
						kQuery.QueryOwner( kPlayerInfo.guidCharacter );

						kQuery.PushStrParam( kPlayerInfo.guidCharacter );
						kQuery.PushStrParam( iType );

						g_kCoreCenter.PushQuery( kQuery );
					}
				}

				if( !((kLastHiddenUpdate.year == kNowTime.year) && (kLastHiddenUpdate.month == kNowTime.month) && (kLastHiddenUpdate.day == kNowTime.day)) )
				{
					kSmartPtr_Player()->GetHiddenOpen()->Init();

					BM::DBTIMESTAMP_EX kDBNowTime;
					CONT_HIDDENOPEN kHiddenOpen;
					
					kSmartPtr_Player()->GetHiddenOpen()->GetBuffer(kHiddenOpen);
					kSmartPtr_Player()->GetHiddenOpen()->GetDateBuffer(kDBNowTime);

					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Change_Character_HiddenOpen]");
					kQuery.InsertQueryTarget(kPlayerInfo.guidCharacter);
					kQuery.PushStrParam(kPlayerInfo.guidCharacter);
					kQuery.PushStrParam( &(kHiddenOpen.at(0)), kHiddenOpen.size() );
					kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kDBNowTime));

					g_kCoreCenter.PushQuery(kQuery);
				}

				int iSelectBuff = 0;
				itor->Pop( iSelectBuff );	++itor;
				if ( IsUseLevelRank() && (0 < iSelectBuff) )
				{
					kSmartPtr_Player()->SetAbil( AT_LEVEL_RANK, iSelectBuff );
				}

				// JobSkill Expertness, Exhaustion
				kSmartPtr_Player()->JobSkillExpertness().Init(itor, count_itor, kPremiumMgr);
				//로그아웃한 시점으로부터 지난 시간
				DWORD dwLogInGapTime = 0;
				{
					(*itor).Pop(dwLogInGapTime);	++itor;
					dwLogInGapTime *= 1000;
					++count_itor;
				}
				//SkillCoolTime
				{
					int iSkillNo = 0;
					DWORD dwCoolTime = 0;
					CSkill::MAP_COOLTIME kContCoolTime;
					int const iCooltimeCount	= (*count_itor); ++count_itor;
					for(int iCur = 0; iCooltimeCount > iCur; ++iCur )
					{
						(*itor).Pop(iSkillNo);		++itor;
						(*itor).Pop(dwCoolTime);	++itor;

						if( dwLogInGapTime < dwCoolTime )
						{
							kContCoolTime.insert( std::make_pair(iSkillNo, dwCoolTime - dwLogInGapTime) );
						}
					}
					if( 0 < iCooltimeCount )
					{
						kSmartPtr_Player()->GetSkill()->LoadFromDBCoolTimeMap(kContCoolTime);
					}
				}
				//Item CoolTime 쿼리는 --56 에서 받는다.
				{
					pkPlayer->GetInven()->LoadGroupCoolTime(dwLogInGapTime, abyItemGroupCoolTime);
				}

				{
					BYTE BlockType = 0;
					std::wstring BlockName;
					CONT_BLOCKCHAT_LIST BlockList;
					int BlockListCount = (*count_itor); ++count_itor; // Chat Block Info.
					for( int i = 0 ; i < BlockListCount ; ++i )
					{
						itor->Pop(BlockName); ++itor;
						itor->Pop(BlockType); ++itor;

						BlockList.insert( std::make_pair( BlockName, BlockType ) );
					}

					pkPlayer->SetChatBlockList(BlockList);
				}

				{
					const int iBattlePassCount = (*count_itor); ++count_itor;
					PgBattlePassInfo & kBattlePassInfo = pkPlayer->GetBattlePassInfo();
					kBattlePassInfo.Clear();
					for( int i = 0 ; i < iBattlePassCount ; ++i )
					{
						kBattlePassInfo.FromDB(itor);
					}
				}

				int const iHasMarket	= (*count_itor); ++count_itor;// market info
				int	const iArticleCount = (*count_itor); ++count_itor;// article info
				int const iDealingCount = (*count_itor); ++count_itor;// dealing info

				int const iMaxHP = pkPlayer->GetAbil(AT_C_MAX_HP);
				pkPlayer->NftChangedAbil(AT_REFRESH_ABIL_INV,SYNC_TYPE_NONE);
				pkPlayer->SetAbil(AT_HP, kPlayerInfo.iHP);

				BM::CAutoMutex kLock(m_kMutex, true);
				if( S_OK == ProcessSelectPlayer( kSmartPtr_Player(), rkResult.QueryGuid(), bPassTutorial ) )
				{
					if( iHasMarket )
					{
						PgOpenMarket kOpenMarket;
						if( true == kOpenMarket.LoadDB(itor,iArticleCount,iDealingCount, kItemList ) )
						{
							m_kOpenMarketMgr.Locked_InsertMarket(kOpenMarket);
						}
					}

					BM::Stream kSendPacket;
					kSendPacket.Push(E_NoticeMsg_UserLogin);
					kSendPacket.Push(kSmartPtr_Player()->GetMemberGUID());
					kSendPacket.Push(kSmartPtr_Player()->GetGuildGuid());
					SendToNotice( kSendPacket );

					kSmartPtr_Player.SetPlayer(NULL);//지워지지 않게.
				}				
			}
		}

		return true;
	}
	else if( CEL::DR_NO_RESULT == rkResult.eRet )
	{
		BM::Stream kPacket(PT_T_C_ANS_SELECT_CHARACTER);
		kPacket.Push( rkResult.QueryOwner() );
		kPacket.Push( static_cast< ESelectCharacterRet >(E_SCR_CHARACTERCOUNT_ERROR) );
		SendToSwitch( rkResult.QueryOwner(), kPacket );
		return false;
	}

	return true;
}

bool PgRealmUserManager::Locked_Recv_PT_S_T_REQ_SELECT_CHARACTER(BM::GUID const &kMemberGuid, BM::GUID const &kCharacterGuid, bool const bPassTutorial)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::GUID const kSelectQueryGuid(BM::GUID::Create());

	{
		SCenterPlayer_Lock kLockPlayer;
		if (!GetPlayerInfo(kMemberGuid, true, kLockPlayer))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		SCenterPlayerData* pkSelectedPlayerData = kLockPlayer.pkCenterPlayerData;
		if( pkSelectedPlayerData->pkPlayer 
		||	pkSelectedPlayerData->kSelectQueryGuid != BM::GUID::NullData()	)//쿼리 진행중이다.
		{//두번 따닥 클릭 막아야됨.
			if (pkSelectedPlayerData->pkPlayer)
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Already Selected Character[M:") << kMemberGuid.str().c_str() << _T("-C:") << pkSelectedPlayerData->pkPlayer->GetID().str().c_str() << _T("]") );
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Already Selected Character[M:") << kMemberGuid.str().c_str() << _T("-C:NULL]") );
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		pkSelectedPlayerData->kSelectQueryGuid = kSelectQueryGuid;
	}

	CONT_PLAYER_WAITER::iterator waiter_itor = m_kContPlayerWaiter.find(kCharacterGuid);
	if(waiter_itor != m_kContPlayerWaiter.end())
	{// 캐시가 있다면.
		tagSafePlayer kSmartPtr_Player;
		kSmartPtr_Player.SetPlayer((*waiter_itor).second, &m_kContPlayerWaiter);
		
		if(S_OK == ProcessSelectPlayer( kSmartPtr_Player(), kSelectQueryGuid, bPassTutorial ))
		{
			kSmartPtr_Player.SetPlayer(NULL);
			m_kContPlayerWaiter.erase(waiter_itor);// 지워줘야 해
		}
	}
	else
	{

		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SELECT_PLAYER_DATA, L"EXEC [dbo].[up_SelectedCharacter]");
			//INFO_LOG(BM::LOG_LV6, __FL__ << _T("up_SelectedCharacter Guid=") << kCharacterGuid);
			kQuery.InsertQueryTarget(kMemberGuid);
			kQuery.InsertQueryTarget(kCharacterGuid);
			kQuery.PushStrParam( kCharacterGuid );	// 인자값:캐릭GUID
			kQuery.PushStrParam( BYTE((bPassTutorial)? 1: 0) );
			kQuery.PushStrParam( kMemberGuid );
			kQuery.PushStrParam( g_kBattlePassMgr.Locked_SeasonNo() );
			kQuery.QueryOwner( kMemberGuid );
			
			kQuery.QueryGuid(kSelectQueryGuid);
			if(S_OK != g_kCoreCenter.PushQuery(kQuery))
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("[PT_S_T_REQ_SELECT_CHARACTER] PushQuery Error!! [M:") << kMemberGuid.str().c_str() << _T("-C:") << kCharacterGuid.str().c_str() << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}

		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Post_ReturnTimeOutMail]");
			kQuery.InsertQueryTarget(kCharacterGuid);
			kQuery.QueryOwner(kCharacterGuid);
			kQuery.PushStrParam( kCharacterGuid );	// 인자값:캐릭GUID
			g_kCoreCenter.PushQuery(kQuery);
		}
		
		return true;
	}
	return true;
}

bool PgRealmUserManager::Locked_Recv_PT_T_N_NFY_SELECT_CHARACTER_FAILED( BM::GUID const &kCharGuid )
{
	BM::CAutoMutex kLock(m_kMutex, true);

	{
		SCenterPlayer_Lock kLockPlayer;
		if ( GetPlayerInfo(kCharGuid, false, kLockPlayer) )
		{
			SCenterPlayerData* pkSelectedPlayerData = kLockPlayer.pkCenterPlayerData;

			if( pkSelectedPlayerData->pkPlayer )
			{
				if ( pkSelectedPlayerData->pkPlayer->GetID() == kCharGuid )
				{
					m_kContPlayer_CharKey.erase(kCharGuid);
					m_kContPlayer_CharName.erase( pkSelectedPlayerData->pkPlayer->Name() );
					_ProcessRemoveCharacter( pkSelectedPlayerData );
					return true;
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CharacterGUID [") << pkSelectedPlayerData->pkPlayer->GetID().str().c_str() << _T("]<->[") << kCharGuid.str().c_str() << _T("] Diffrent") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Matched CharGuid "));
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Don't Has Select Character[") << kCharGuid.str().c_str() << _T("] MemberGuid[") << pkSelectedPlayerData->guidMember.str().c_str() << _T("]") );
			}
		}	
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;

}

bool PgRealmUserManager::Recv_PT_C_T_REQ_CHANNLE_INFORMATION( BM::GUID const &kMemberGuid )
{//퍼블릭 채널의 경우 처리 때문에. 여기서 멤버 GUID체크 하지 않는다.
	BM::Stream kIMPacket(PT_T_IM_REQ_CHANNEL_INFORMATION, g_kProcessCfg.ServerIdentity());
	kIMPacket.Push(kMemberGuid);
	return SendToImmigration(kIMPacket);
}

void PgRealmUserManager::Locked_Recv_PT_M_T_REQ_SAVE_CHARACTER( BM::Stream *pkPacket)
{
	// 여기서는 Lock을 걸지 않는다!!
	BM::GUID kCharGuid;
	SGroundKey kGndKey;
	POINT3 pt3Pos;
	int iHP = 0;
	int iMP = 0;
	short FatigueDegree = 0;
	__int64 i64RestExp = 0;
	PgQuickInventory kQuickInv;
	SPlayerBinaryData kBinary;
	SRecentPlayerNormalMapData MapData;
	BYTE abyItemGroupCoolTime[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE] = {0,};
	CSkill::MAP_COOLTIME kContCoolTime;
	bool bSelectChannel = false;
	pkPacket->Pop( kCharGuid );
	pkPacket->Pop( bSelectChannel );
	pkPacket->Pop( kGndKey );
	pkPacket->Pop( MapData );
	pkPacket->Pop( pt3Pos );
	pkPacket->Pop( iHP );
	pkPacket->Pop( iMP );
	kQuickInv.ReadFromPacket( *pkPacket );
	pkPacket->Pop(FatigueDegree);
	pkPacket->Pop(kBinary.abyEffect);
	pkPacket->Pop(i64RestExp);
	pkPacket->Pop(abyItemGroupCoolTime);
	pkPacket->Pop(kContCoolTime);
	

	// 1 최근 위치 저장
	T_GNDATTR kGndAttr = GATTR_INSTANCE;//인스턴스로 초기해 놔야 오류를 방지 할 수 있다!!

	CONT_DEFMAP const *pkDefMap;
	g_kTblDataMgr.GetContDef(pkDefMap);
	if( pkDefMap )
	{
		CONT_DEFMAP::const_iterator map_itr = pkDefMap->find( kGndKey.GroundNo() );
		if(	map_itr != pkDefMap->end() )
		{	
			kGndAttr = (T_GNDATTR)(map_itr->second.iAttr);
			if ( GATTR_FLAG_VILLAGE & kGndAttr )
			{
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_LastVillage]");
				kQuery.InsertQueryTarget(kCharGuid);
				kQuery.PushStrParam(kCharGuid);

				kQuery.PushStrParam( kGndKey.GroundNo() );
				kQuery.PushStrParam( pt3Pos.x );
				kQuery.PushStrParam( pt3Pos.y );
				kQuery.PushStrParam( pt3Pos.z );
				g_kCoreCenter.PushQuery( kQuery );
			}
			else if ( 0 == (GATTR_FLAG_NOSAVE_RECENT & kGndAttr) )
			{
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_RecentMap]");
				kQuery.InsertQueryTarget(kCharGuid);
				kQuery.PushStrParam(kCharGuid);

				kQuery.PushStrParam( kGndKey.GroundNo() );
				kQuery.PushStrParam( pt3Pos.x );
				kQuery.PushStrParam( pt3Pos.y );
				kQuery.PushStrParam( pt3Pos.z );
				g_kCoreCenter.PushQuery( kQuery );
			}
		}
	}

	{
		// 이전 채널 정보.
		CEL::DB_QUERY Query( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_RecentChannel]");
		Query.InsertQueryTarget(kCharGuid);
		Query.PushStrParam(kCharGuid);

		Query.PushStrParam( MapData.ChannelNo );
		Query.PushStrParam( MapData.GroundNo );
		Query.PushStrParam( MapData.RecentPos.x );
		Query.PushStrParam( MapData.RecentPos.y );
		Query.PushStrParam( MapData.RecentPos.z );
		g_kCoreCenter.PushQuery( Query );
	}

	{
		// 정상적으로 저장 됨.
		CEL::DB_QUERY Query( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_RecentChannelState]");
		Query.InsertQueryTarget(kCharGuid);
		Query.PushStrParam(kCharGuid);

		Query.PushStrParam( SHRT_MAX );
		g_kCoreCenter.PushQuery( Query );
	}

	{// QuickInvSave
		BYTE abyQuickSlot[MAX_DB_QUICKSLOT_SIZE] = {0,};
		kQuickInv.Save(abyQuickSlot);

		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVECHARACTER_EXTERN, L"EXEC [dbo].[up_Update_TB_UserCharacter_QuickSlot2]" );
		kQuery.InsertQueryTarget(kCharGuid);				
		kQuery.PushStrParam(kCharGuid);
		kQuery.PushStrParam(static_cast<BYTE>(false));//활성화된 QuickInv을 QuickSlot컬럼에 추가
		kQuery.PushStrParam( abyQuickSlot, MAX_DB_QUICKSLOT_SIZE );
		g_kCoreCenter.PushQuery(kQuery);
	}

	if (iHP >= 0)
	{// HP Save
		//INFO_LOG(BM::LOG_LV6, __FL__ << _T("HP SAVE Guid=") << kCharGuid << _T(", HP=") << iHP);
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_Field2]" );
		kQuery.InsertQueryTarget(kCharGuid);
		kQuery.PushStrParam(kCharGuid);
		kQuery.PushStrParam(std::wstring(_T("HP")));
		kQuery.PushStrParam(iHP);
		g_kCoreCenter.PushQuery(kQuery);
	}

	if (iMP >= 0)
	{// MP Save
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_Field2]" );
		kQuery.InsertQueryTarget(kCharGuid);
		kQuery.PushStrParam(kCharGuid);
		kQuery.PushStrParam(std::wstring(_T("MP")));
		kQuery.PushStrParam(iMP);
		g_kCoreCenter.PushQuery(kQuery);
	}

	if( FatigueDegree >= 0 )
	{// FatigueDegree Save
		CEL::DB_QUERY Query( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_Field2]" );
		Query.InsertQueryTarget(kCharGuid);
		Query.PushStrParam(kCharGuid);
		Query.PushStrParam(std::wstring(_T("Strategyfatigability")));
		Query.PushStrParam(FatigueDegree);
		g_kCoreCenter.PushQuery(Query);
	}

	{	// Effect Save
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_PLAYER_EFFECT, L"EXEC [dbo].[up_SaveEffect]" );
		kQuery.InsertQueryTarget(kCharGuid);				
		kQuery.PushStrParam(kCharGuid);
		kQuery.PushStrParam( kBinary.abyEffect, sizeof(kBinary.abyEffect) );
		g_kCoreCenter.PushQuery(kQuery);
	}

	{// 마지막 로그 아웃 타임 저장
		BM::PgPackedTime kCurTime;
		kCurTime.SetLocalTime();
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Update_UserAchievements]");
		kQuery.InsertQueryTarget(kCharGuid);
		kQuery.PushStrParam(kCharGuid);
		kQuery.PushStrParam(static_cast<int>(AT_ACHIEVEMENT_LONGTIMENOSEE));
		kQuery.PushStrParam(static_cast<int>(kCurTime.GetTime()));
		g_kCoreCenter.PushQuery(kQuery);
	}

	{
		DWORD dwLow32, dwHigh32;
		SplitInt64(i64RestExp, dwLow32, dwHigh32);
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Update_UserAchievements]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.PushStrParam(kCharGuid);
			kQuery.PushStrParam(static_cast<int>(AT_REST_EXP_ADD_MAX_LOW32));
			kQuery.PushStrParam(static_cast<int>(dwLow32));	// DWORD 이지만, DB는 int type 으로만 저장가능하므로
			g_kCoreCenter.PushQuery(kQuery);
		}
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Update_UserAchievements]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.PushStrParam(kCharGuid);
			kQuery.PushStrParam(static_cast<int>(AT_REST_EXP_ADD_MAX_HIGH32));
			kQuery.PushStrParam(static_cast<int>(dwHigh32)); // DWORD 이지만, DB는 int type 으로만 저장가능하므로
			g_kCoreCenter.PushQuery(kQuery);
		}
		{
			// Character logout time
			BM::PgPackedTime kNow;
			kNow.SetLocalTime();
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Update_UserAchievements]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.PushStrParam(kCharGuid);
			kQuery.PushStrParam(static_cast<int>(AT_CHARACTER_LAST_LOGOUT_TIME));
			kQuery.PushStrParam(static_cast<int>(kNow.GetTime()));
			g_kCoreCenter.PushQuery(kQuery);
		}
	}

	{
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NO_OP, L"EXEC [dbo].[UP_Update_ItemGroupCoolTime]");
		kQuery.InsertQueryTarget(kCharGuid);
		kQuery.PushStrParam(kCharGuid);
		kQuery.PushStrParam(abyItemGroupCoolTime, MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE);
		g_kCoreCenter.PushQuery(kQuery);
	}
	//SkillCoolTime
	{//일단 기존 정보 날리고
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NO_OP, L"EXEC [dbo].[UP_User_Skill_CoolTime_Delete]");
		kQuery.InsertQueryTarget(kCharGuid);
		kQuery.PushStrParam(kCharGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
	{//새로 넣자고
		CSkill::MAP_COOLTIME::const_iterator iter = kContCoolTime.begin();
		while( iter != kContCoolTime.end() )
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NO_OP, L"EXEC [dbo].[UP_User_Skill_CoolTime_Update]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.PushStrParam(kCharGuid);
			kQuery.PushStrParam(static_cast<int>((*iter).first));//SkillNo
			kQuery.PushStrParam(static_cast<int>((*iter).second));//CoolTime
			g_kCoreCenter.PushQuery(kQuery);
			++iter;
		}
	}
	// Pet
	BM::GUID kPetID;
	pkPacket->Pop( kPetID );
	if ( BM::GUID::IsNotNull(kPetID) )
	{
		int iPetMP = 0;
		if ( true == pkPacket->Pop( iPetMP ) )
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SETABIL_PET, L"EXEC [dbo].[up_Update_Pet]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.PushStrParam(kPetID);
			kQuery.PushStrParam(std::wstring(L"MP"));
			kQuery.PushStrParam(iPetMP);
			g_kCoreCenter.PushQuery( kQuery );
		}
		
// 		bool bUpdateCoolTime = false;
// 		pkPacket->Pop( bUpdateCoolTime );
// 		if ( true == bUpdateCoolTime )
// 		{
// 			BYTE byCoolTime[MAX_DB_PET_COOLTIME_SIZE] = {0,};
// 			if ( pkPacket->PopMemory( byCoolTime, sizeof(byCoolTime) ) )
// 			{
// 				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SETABIL_PET, L"EXEC [dbo].[up_Update_Pet_CoolTime]");
// 				kQuery.InsertQueryTarget(kCharGuid);
// 				kQuery.PushStrParam( kPetID );
// 				kQuery.PushStrParam( byCoolTime, sizeof(byCoolTime) );
// 				g_kCoreCenter.PushQuery( kQuery );
// 			}
// 		}
	}

	bool bAutoLogin = false;
	pkPacket->Pop( bAutoLogin );

	if( !bAutoLogin )
	{
		BM::CAutoMutex Lock( m_kMutex, true );
		// 자동 로그인 아니면 그냥 보상 받는 정보를 삭제해 주어야 한다.
		m_ContEventRewardTimeInfo.erase(kCharGuid);
	}
	

	if ( true == bAutoLogin )
	{// 채널 Change일 경우에~~
		BM::CAutoMutex kLock( m_kMutex, true );

		SCenterPlayer_Lock kLockPlayer;
		if ( GetPlayerInfo( kCharGuid, false, kLockPlayer) )
		{
			if ( kLockPlayer.pkCenterPlayerData )
			{
				PgDoc_Player *pkDocPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
				if ( pkDocPlayer )
				{
					if ( pkDocPlayer->GetID() == kCharGuid )
					{
						pkDocPlayer->GroundKey( kGndKey );
						pkDocPlayer->SetPos( pt3Pos );
						pkDocPlayer->UpdateRecent( kGndKey, kGndAttr );
						pkDocPlayer->SetRecentNormalMap(MapData);

						SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
						pkActionOrder->InsertTarget(pkDocPlayer->GetMemberGUID());
						pkActionOrder->InsertTarget(pkDocPlayer->GetID());
						pkActionOrder->kCause = CNE_CONTENTS_EVENT;
						ContentsActionEvent kEvent(ECEvent_PT_I_T_KICKUSER);

						SPMO kOrder( IMET_CONTENTS_EVENT, pkDocPlayer->GetMemberGUID(), kEvent );
						pkActionOrder->kContOrder.push_back(kOrder);

						SRemoveWaiter kWaiter( true, pkDocPlayer->GetMemberGUID() );
						kWaiter.WriteToPacket( pkActionOrder->kAddonPacket );
						pkActionOrder->kAddonPacket.Push( *pkPacket );;//SReqSwitchReserveMember 이게 들어있음.

						g_kJobDispatcher.VPush( pkActionOrder );

						if( bSelectChannel )
						{
							// 로그인 이벤트를 위해 접속을 끊은게 아니고 채널 이동이라는 것을 저장해 두어야함.
							CEL::DB_QUERY Query( DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_SpecificRewardKeepConn]" );
							Query.InsertQueryTarget(pkDocPlayer->GetID());						
							Query.PushStrParam( static_cast<int>(AT_SPECIFIC_REWARD_EVENT) );
							Query.PushStrParam( pkDocPlayer->GetID() );
							Query.PushStrParam( static_cast<short>(1) );
							g_kCoreCenter.PushQuery(Query);
						}
						else
						{	// 채널 이동이 아니고 그냥 로그아웃인 경우에는 보상 받는 정보를 삭제해 주어야 한다.
							m_ContEventRewardTimeInfo.erase(kCharGuid);
						}
					}
					else
					{
						INFO_LOG( BM::LOG_LV5, __FL__ << _T("Different CharacterGuid ") << pkDocPlayer->GetID() << _T("/") << kCharGuid );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Matched CharGuid "));
					}
				}	
			}
		}
	}
}

bool PgRealmUserManager::ReqMapMove( UNIT_PTR_ARRAY const &rkUnitArray, SReqMapMove_MT const &rkRMM, Constellation::SConstellationMission & constellationMission, CONT_PET_MAPMOVE_DATA &rkContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &rkContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER& kModifyItemOrder )const
{
	switch( rkRMM.cType )
	{
	case MMET_PvP:
		{
			BM::Stream kSelectPacket( PT_N_T_NFY_SELECT_CHARACTERS, rkRMM );
			rkUnitArray.WriteToPacket( kSelectPacket, WT_MAPMOVE_FIRST );
			PU::TWriteTable_AM( kSelectPacket, rkContPetMapMoveData );
			kModifyItemOrder.WriteToPacket( kSelectPacket );

			if ( !SendToCenter( CProcessConfig::GetPublicChannel(), kSelectPacket ) )
			{
				// 돌려보내...
				SReqMapMove_MT kRMM(rkRMM);
				if ( kRMM.SetBackHome(MMET_PublicChannelFailed) )
				{
					return ReqMapMove( rkUnitArray, kRMM, constellationMission, rkContPetMapMoveData, rkContUnitSummonedMapMoveData, kModifyItemOrder );
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!!!!!") );
				}
			}

			OnEnterGround(rkUnitArray, SRealmGroundKey(CProcessConfig::GetPublicChannel(), rkRMM.kTargetKey) );
		}break;
	case MMET_BackToPvP:
	case MMET_KickToPvP:
		{
			BM::Stream kMovePacket(PT_N_T_RES_MAP_MOVE, rkRMM);
			constellationMission.WriteToPacket(kMovePacket);
			rkUnitArray.WriteToPacket(kMovePacket, rkUnitArray.GetWriteType());
			PU::TWriteTable_AM( kMovePacket, rkContPetMapMoveData );
			PU::TWriteTable_AM( kMovePacket, rkContUnitSummonedMapMoveData );
			kModifyItemOrder.WriteToPacket( kMovePacket );
			if ( !SendToChannel( CProcessConfig::GetPublicChannel(), kMovePacket ) )
			{
				// 어쩌냐..ㅠㅠ
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("[MMET_BackToPvP] How to Handle ?") );
			}

			OnEnterGround(rkUnitArray, SRealmGroundKey(CProcessConfig::GetPublicChannel(), rkRMM.kTargetKey) );
		}break;
	case MMET_GoToPublicGround:
	case MMET_GoTopublicGroundParty:
	case MMET_BATTLESQUARE:
	case MMET_BackToChannel:
	case MMET_PublicChannelFailed:
		{	
			SReqMapMove_MT kRMM( rkRMM );
			ReqFirstMapMove( rkUnitArray, kRMM, rkContPetMapMoveData, &kModifyItemOrder );
		}break;
	default:
		{
			BM::Stream kMovePacket(PT_N_T_RES_MAP_MOVE, rkRMM);
			constellationMission.WriteToPacket(kMovePacket);
			rkUnitArray.WriteToPacket(kMovePacket, rkUnitArray.GetWriteType());
			PU::TWriteTable_AM( kMovePacket, rkContPetMapMoveData );
			PU::TWriteTable_AM( kMovePacket, rkContUnitSummonedMapMoveData );
			kModifyItemOrder.WriteToPacket( kMovePacket );
			SendToChannel( rkRMM.kTargetSI.nChannel, kMovePacket );

			OnEnterGround(rkUnitArray, SRealmGroundKey(rkRMM.kTargetSI.nChannel, rkRMM.kTargetKey));

			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return true;
}

bool PgRealmUserManager::ReqFirstMapMove( UNIT_PTR_ARRAY const &kUnitArray, SReqMapMove_MT &rkRMM, CONT_PET_MAPMOVE_DATA const &kContPetMapMoveData, CONT_PLAYER_MODIFY_ORDER * pkModifyItemOrder, bool const bReCall )const
{
	if ( kUnitArray.empty() )
	{
		return false;
	}

	PgPlayer *pkFirstPlayer = dynamic_cast<PgPlayer*>(kUnitArray.begin()->pkUnit);
	if ( pkFirstPlayer )
	{
		rkRMM.kTargetSI.nChannel = pkFirstPlayer->GetChannel();
		if ( rkRMM.kTargetKey.IsEmpty() )
		{
			T_GNDATTR kRecentType = GATTR_DEFAULT;
			if( RealmUserManagerUtil::IsDeadPlayer( pkFirstPlayer ) )
			{
				kRecentType = GATTR_VILLAGE;
			}

			rkRMM.pt3TargetPos = pkFirstPlayer->GetRecentPos(kRecentType);
			pkFirstPlayer->GetRecentGround( rkRMM.kTargetKey, kRecentType );

			if ( rkRMM.kTargetKey.IsEmpty() )
			{
				rkRMM.kTargetKey.GroundNo( RealmUserManagerUtil::iEmergencyMap );
				rkRMM.kTargetKey.Guid( BM::GUID::NullData() );
			}

			rkRMM.nTargetPortal = 0;
		}

		CONT_DEFMAP const * pkContDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkContDefMap);

		if ( pkContDefMap )
		{
			CONT_DEFMAP::const_iterator itr = pkContDefMap->find( rkRMM.kTargetKey.GroundNo() );
			if ( itr != pkContDefMap->end() )
			{
				if ( GATTR_FLAG_PUBLIC_CHANNEL & itr->second.iAttr )
				{
					rkRMM.kTargetSI.nChannel = CProcessConfig::GetPublicChannel();
				}
			}
		}

		BM::Stream kSelectPacket( PT_N_T_NFY_SELECT_CHARACTERS, rkRMM );
		kUnitArray.WriteToPacket( kSelectPacket, WT_MAPMOVE_FIRST );
		
		PU::TWriteTable_AM( kSelectPacket, kContPetMapMoveData );
		if ( pkModifyItemOrder )
		{
			pkModifyItemOrder->WriteToPacket( kSelectPacket );
		}
		else
		{
			kSelectPacket.Push( static_cast<size_t>(0) );
		}

		if ( true == ::SendToCenter( rkRMM.kTargetSI.nChannel, kSelectPacket ) )
		{
			OnEnterGround( kUnitArray, SRealmGroundKey(rkRMM.kTargetSI.nChannel, rkRMM.kTargetKey) );
			return true;
		}
		else
		{
			if ( true == CProcessConfig::IsPublicChannel(rkRMM.kTargetSI.nChannel) && !bReCall )
			{
				// 돌려보내...
				if ( rkRMM.SetBackHome(MMET_PublicChannelFailed) )
				{
					return ReqFirstMapMove( kUnitArray, rkRMM, kContPetMapMoveData, NULL, true );
				}
			}
		}
	}
	return false;
}

DWORD PgRealmUserManager::RecvMapMove( BM::Stream * const pkPacket )
{
	SReqMapMove_MT kRMM;
	Constellation::SConstellationMission constellationMission;
	if( pkPacket->Pop(kRMM) )
	{
		constellationMission.ReadFromPacket(*pkPacket);

		short nChannelNo = 0;
		pkPacket->Pop(nChannelNo);

		UNIT_PTR_ARRAY kUnitArray;
		EWRITETYPE const kWriteType = kUnitArray.ReadFromPacket( *pkPacket, true );// 맵에서 보낸 유저정보를 싹 흡수 해버림

		CONT_PET_MAPMOVE_DATA kContPetMapMoveData;
		PU::TLoadTable_AM( *pkPacket, kContPetMapMoveData );

		CONT_UNIT_SUMMONED_MAPMOVE_DATA kContUnitSummonedMapMoveData;
		PU::TLoadTable_AM( *pkPacket, kContUnitSummonedMapMoveData );

		UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
		while ( unit_itr!=kUnitArray.end() )
		{
			switch (unit_itr->pkUnit->UnitType())
			{
			case UT_MYHOME:
				{
				}break;
			case UT_PLAYER:
				{
					PgPlayer *pkMapUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
					if ( !pkMapUser )
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("Ptr Error") );
						kContPetMapMoveData.erase(unit_itr->pkUnit->GetID());
						kContUnitSummonedMapMoveData.erase(unit_itr->pkUnit->GetID());
						unit_itr = kUnitArray.erase(unit_itr);
						continue;
					}

					SCenterPlayer_Lock kLockPlayer;
					if (!GetPlayerInfo(pkMapUser->GetMemberGUID(), true, kLockPlayer)
						|| kLockPlayer.pkCenterPlayerData == NULL
						|| kLockPlayer.pkCenterPlayerData->pkPlayer == NULL)
					{	// 헛~없다니....
						kContPetMapMoveData.erase(pkMapUser->GetID());
						kContUnitSummonedMapMoveData.erase(pkMapUser->GetID());
						unit_itr = kUnitArray.erase(unit_itr);
						continue;
					}

					PgDoc_Player *pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
					if ( pkPlayer->VolatileID() == pkMapUser->VolatileID() )
					{
						pkMapUser->CopyTo(*pkPlayer, kWriteType|WT_OP_No_Contents);
						if ( true == pkPlayer->UpdateWorldMap(kRMM.kTargetKey.GroundNo() ) )
						{
							using namespace RealmUserManagerUtil;
							ProcessAddWorldMap( pkMapUser->GetID(), kRMM.kCasterSI.nChannel, kRMM.kTargetKey.GroundNo(), kRMM.kCasterKey );
						}

						UpdateRecentPos(pkPlayer, nChannelNo, &kRMM );
						ProcessUpdateCacheDB( pkPlayer );

					//	Contents Player정보로 덮어 쓴 후, 다른 서버/클라이언트로 Player 정보를 보내야 한다.
						PgPlayer* pkDynamicCastPlayer = dynamic_cast<PgPlayer*>(pkPlayer);
						if (pkDynamicCastPlayer != NULL)
						{
							pkDynamicCastPlayer->CopyTo(*pkMapUser, kWriteType);
						}

						BM::GUID const kSelectedPetID = pkPlayer->SelectedPetID();
						if ( kSelectedPetID.IsNotNull() )
						{
							CONT_PET_MAPMOVE_DATA::iterator pet_data_itr = kContPetMapMoveData.find( kSelectedPetID );
							if ( pet_data_itr != kContPetMapMoveData.end() )
							{
								if ( FAILED( pkPlayer->UpdatePetInfo( kSelectedPetID, pet_data_itr->second, false )) )
								{
									kContPetMapMoveData.erase( pet_data_itr );
								}
							}
							else
							{
								CONT_PET_MAPMOVE_DATA::mapped_type kPetMapMoveData;
								if ( SUCCEEDED( pkPlayer->UpdatePetInfo( kSelectedPetID, kPetMapMoveData, true )) )
								{
									kContPetMapMoveData.insert( std::make_pair( kSelectedPetID, kPetMapMoveData) );
								}
							}
						}
					}
					else
					{
						CAUTION_LOG( BM::LOG_LV3, __FL__ << L" Character<" << pkPlayer->Name() << L"/" << pkPlayer->GetID() << L"> LogOut->OtherJoin->OldMapMove Process Excute!!" );
						kContPetMapMoveData.erase(pkPlayer->GetID());
						kContUnitSummonedMapMoveData.erase(pkPlayer->GetID());
						unit_itr = kUnitArray.erase(unit_itr);
						continue;
					}
				}break;
			default:
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Error UnitType = " << unit_itr->pkUnit->UnitType() );
				}break;
			}
			++unit_itr;
		}

		CONT_PLAYER_MODIFY_ORDER kModifyItemOrder;
		kModifyItemOrder.ReadFromPacket( *pkPacket );

		ReqMapMove( kUnitArray, kRMM, constellationMission, kContPetMapMoveData, kContUnitSummonedMapMoveData, kModifyItemOrder );
	}
	return 0;
}


bool PgRealmUserManager::Locked_NfyReloadGameData()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::Stream kPacket(PT_T_M_NFY_RELOAD_GAMEDATA);

	g_kTblDataMgr.DataToPacket(kPacket);
	
	SendToServerType(CEL::ST_MAP, kPacket);
	SendToServerType(CEL::ST_ITEM, kPacket);

	return true;
}

void PgRealmUserManager::Locked_Recv_PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP( const BM::GUID& rkMemberGuid, const std::wstring& rkCharacterName )
{
	BM::CAutoMutex kLock(m_kMutex);

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHECK_CHARACTERNAME_OVERLAP, L"EXEC [dbo].[up_CheckCharacterNameOverlap]");
	kQuery.InsertQueryTarget( rkMemberGuid );
	kQuery.QueryOwner( rkMemberGuid );
	kQuery.PushStrParam( rkCharacterName );
	g_kCoreCenter.PushQuery(kQuery);
}

bool PgRealmUserManager::Locked_Recv_PT_S_T_REQ_CREATE_CHARACTER(BM::GUID const &rkMemberGuid, SReqCreateCharacter const &kReqCreateCharacter)
{//글자수 제한 해야됨.
//	디폴트 캐릭터 디비 값을 넣을것.
	BM::CAutoMutex kLock(m_kMutex);//Create 시도니까 다른게 바뀔건 없다.

	SCenterPlayer_Lock kLockPlayer;
	if ( !GetPlayerInfo(rkMemberGuid, true, kLockPlayer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;// 로그인 하지 않은 유저
	}

	SCenterPlayerData* pPlayerData = kLockPlayer.pkCenterPlayerData;
	if(!pPlayerData)
	{	
		INFO_LOG( BM::LOG_LV4, __FL__ << _T("Memory error") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;// 메모리 에러
	}

	if (pPlayerData->kSelectQueryGuid != BM::GUID::NullData())
	{
		// kSelectQueryGuid 가 있다는 것은, SELECT_CHARACTER,CREATE_CHARACTER,DELETE_CHARACTER 중에 한개가 진행중 이라는 중이다.
		// DOS 공격에 대비하여 바로 return
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Query Inprocessing...waiting DB_Result Member=") << pPlayerData->kID << _T(", M-G=") << pPlayerData->guidMember
			<< _T(", QueryGuid=") << pPlayerData->kSelectQueryGuid);
		return false;
	}

	// 데드락을 피하기 위해서 m_kMutexLoginedUser를 {}안에 넣었다
	// {}를 빠져나온후 로그인유저의 무결성을 보장하지 못하지만 크게 문제될건 없다

	bool bCheckInMemory = false;
	bool bUseable = false;
	BYTE byCreateMode = ECHAR_CREATE_CHECK_ALL_NAME;
	static LOCAL_MGR::NATION_CODE const eNation = static_cast<LOCAL_MGR::NATION_CODE>(g_kLocal.ServiceRegion());
	switch(eNation)
	{
	case LOCAL_MGR::NC_SINGAPORE:
	case LOCAL_MGR::NC_THAILAND:
	case LOCAL_MGR::NC_INDONESIA:
	case LOCAL_MGR::NC_PHILIPPINES:
	case LOCAL_MGR::NC_VIETNAM:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_TAIWAN:
		{
			byCreateMode = ECHAR_CREATE_NOT_CHECK_DEL_CHAR;	// Name 바꾸고 State = 1
		}break;
	case LOCAL_MGR::NC_CHINA:
	default:
		{
			byCreateMode = ECHAR_CREATE_CHECK_ALL_NAME;	// Name 유지하고 State = 1
		}break;
	}

	BM::Stream kErrPacket(PT_T_S_ANS_CREATE_CHARACTER, rkMemberGuid);

	EKindCharacterBaseWear eWrongWear = KCBW_NONE;

	std::wstring kTempName(kReqCreateCharacter.szName);
	int const iMinCharacterNameLen = RealmUserManagerUtil::iMinCharacterNameLen;
	if( kTempName.size() < iMinCharacterNameLen )
	{
		kErrPacket.Push(CCR_MIN_LENGTH);
		SendToSwitch(rkMemberGuid, kErrPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	//캐릭터 이름길이 재확인
	std::wstring::const_iterator itorCharName = kTempName.begin();
	int iCharNameCount = 0;
	while( kTempName.end() != itorCharName)
	{
		++iCharNameCount;
		++itorCharName;
	}
	if( iMinCharacterNameLen > iCharNameCount
		|| kTempName.size() != iCharNameCount)
	{
		kErrPacket.Push(CCR_FILTER_CODE);
		SendToSwitch(rkMemberGuid, kErrPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( kTempName.size() > RealmUserManagerUtil::iMaxCharacterNameLen )
	{
		kErrPacket.Push(CCR_MAX_LENGTH);
		SendToSwitch(rkMemberGuid, kErrPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	int iEnableCreate = 1;
	if (S_OK == g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_Eable_CreateCharacter, iEnableCreate))
	{
		if (iEnableCreate == 0)
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << _T("CreateCharacter function disabled.."));
			kErrPacket.Push(CCR_DISABLED_FUNCTION);
			SendToSwitch(rkMemberGuid, kErrPacket);
			return false;
		}
	}

	int iCreateCharacterNum = DEFAULT_CHARACTER_COUNT;
	if (S_OK == g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_MaxCreateCharacterNum, iCreateCharacterNum))
	{
		iCreateCharacterNum = (0 < iCreateCharacterNum ? iCreateCharacterNum : DEFAULT_CHARACTER_COUNT);
	}

	if( false == g_kUnicodeFilter.IsCorrect(UFFC_CHARACTER_NAME, kReqCreateCharacter.szName) )
	{
		kErrPacket.Push(CCR_FILTER_CODE);
		SendToSwitch(rkMemberGuid, kErrPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( true == g_kFilterString.Filter(kTempName, false, FST_ALL) )
	{
		kErrPacket.Push(CCR_FILTER_NAME);
		SendToSwitch(rkMemberGuid, kErrPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pPlayerData->kGetListQueryGuid.Clear();

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CREATECHARACTER, L"EXEC [dbo].[UP_CreateCharacter]");//goto 문 때문. 2번째 줄은 DefaultValue
	kQuery.InsertQueryTarget(rkMemberGuid);
	CONT_DEF_BASE_CHARACTER const *pkContBaseChar = NULL;
	g_kTblDataMgr.GetContDef(pkContBaseChar);

	CONT_DEF_BASE_CHARACTER::const_iterator base_char_itor = pkContBaseChar->find(kReqCreateCharacter.iClass);
	if(base_char_itor == pkContBaseChar->end())	// 보안 검사
	{
		HACKING_LOG(BM::LOG_LV2, __FL__ << _T("[Hacking] CreateCharacter wrong Class no request Class[") << kReqCreateCharacter.iClass << _T("]"));
		INFO_LOG(BM::LOG_LV2, __FL__ << _T("[Hacking] CreateCharacter wrong Class no request Class[") << kReqCreateCharacter.iClass << _T("]"));
		return false;
	}
	if( (*base_char_itor).second.f_Class != UCLASS_SUMMONER
		&&(*base_char_itor).second.f_Class != UCLASS_TWINS )
	{
		if ((*base_char_itor).second.f_Lv > 1)
		{
			HACKING_LOG(BM::LOG_LV2, __FL__ << _T("[Hacking] CreateCharacter wrong Class no request Class=") << kReqCreateCharacter.iClass << _T(", Lv=") << (*base_char_itor).second.f_Lv);
			INFO_LOG(BM::LOG_LV2, __FL__ << _T("[Hacking] CreateCharacter wrong Class no request Class=") << kReqCreateCharacter.iClass << _T(", Lv=") << (*base_char_itor).second.f_Lv);
			return false;
		}
	}

	//JumpingCharEvent
	SCreateCharacterInfo kCreateCharacterInfo;
	if(kReqCreateCharacter.kJumpingCharEvent.iEventNo>0)
	{
		if(false==g_kJumpingCharEventMgr.GetEvent(kReqCreateCharacter.kJumpingCharEvent.iEventNo, kReqCreateCharacter.iClass, kReqCreateCharacter.kJumpingCharEvent.iClass, kCreateCharacterInfo))
		{
			BM::Stream kPacket(PT_T_S_ANS_CREATE_CHARACTER, rkMemberGuid);
			kPacket.Push(CCR_NOT_JUMPINGEVENT);
			SendToSwitch(rkMemberGuid, kPacket);
			return false;
		}
	}
	else
	{
		kCreateCharacterInfo.iClass = kReqCreateCharacter.iClass;
		kCreateCharacterInfo.iLv = (*base_char_itor).second.f_Lv;
		kCreateCharacterInfo.iExp = (*base_char_itor).second.f_Exp;
		kCreateCharacterInfo.iHP = (*base_char_itor).second.f_HP;
		kCreateCharacterInfo.iMP = (*base_char_itor).second.f_MP;
		kCreateCharacterInfo.iExtraSkillPoint = (*base_char_itor).second.f_ExtraSkillPoint;
		kCreateCharacterInfo.pkIngQuest = (*base_char_itor).second.f_IngQuest;
		kCreateCharacterInfo.pkEndQuest = (*base_char_itor).second.f_EndQuest;
		kCreateCharacterInfo.pkEndQuest2 = (*base_char_itor).second.f_EndQuest2;
		kCreateCharacterInfo.pkEndQuestExt = (*base_char_itor).second.f_EndQuestExt;
		kCreateCharacterInfo.pkEndQuestExt2 = (*base_char_itor).second.f_EndQuestExt2;
	}


	std::wstring kConvName = kReqCreateCharacter.szName;
  /*
	if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU))
	{
		if(kConvName.size())
		{
			std::wstring kHeadName(kConvName.begin(), kConvName.begin()+1);
			std::wstring kBodyName(kConvName.begin()+1, kConvName.end());
			
			UPR(kHeadName);
			LWR(kBodyName);

			kConvName = kHeadName + kBodyName;
		}
	}
	*/
	// 착용가능한 아이템인지 검사
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultHair,	KCBW_HAIRSTYLE))	{	eWrongWear = KCBW_HAIRSTYLE;	goto __WEAR_FAILED;	}
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultHairColor, KCBW_HAIRCOLOR))	{	eWrongWear = KCBW_HAIRCOLOR;	goto __WEAR_FAILED;	}
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultFace,	KCBW_FACE))			{	eWrongWear = KCBW_FACE;			goto __WEAR_FAILED;	}
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultShirts,	KCBW_JACKET))		{	eWrongWear = KCBW_JACKET;		goto __WEAR_FAILED;	}
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultGlove,	KCBW_GLOVES))		{	eWrongWear = KCBW_GLOVES;		goto __WEAR_FAILED;	}
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultPants,	KCBW_PANTS))		{	eWrongWear = KCBW_PANTS;		goto __WEAR_FAILED;	}
	if(!CheckValidBaseWear(kReqCreateCharacter.iDefaultBoots,	KCBW_SHOES))		{	eWrongWear = KCBW_SHOES;		goto __WEAR_FAILED;	}
	// TODO: Check for correct setNo!
	if (kReqCreateCharacter.iSetNo != 0)
	{
		CONT_DEF_CHARACTER_CREATE_SET const* pContDefCharList = NULL;
		TBL_DEF_CHARACTER_CREATE_SET kFind;

		g_kTblDataMgr.GetContDef(pContDefCharList);
		kFind.iSetNo = kReqCreateCharacter.iSetNo;
		kFind.byGender = kReqCreateCharacter.byGender;
		kFind.byClassNo = BYTE(kReqCreateCharacter.iClass);
		CONT_DEF_CHARACTER_CREATE_SET::const_iterator it = pContDefCharList->find(kFind);
		if (it == pContDefCharList->end())
		{
			BM::Stream kPacket(PT_T_S_ANS_CREATE_CHARACTER, rkMemberGuid);
			kPacket.Push(CCR_SET_NOT_FOUND);
			SendToSwitch(rkMemberGuid, kPacket);
			return false;
		}
	}

	int iDrakanMinLv = 20;
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Char_MinLvToCreateDrakan, iDrakanMinLv);

	int iDrakanCreateItemNo = 0;
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Char_CreateDrakanItemNo, iDrakanCreateItemNo);

	// Todo : 최초 맵번호는 어디 ? -> DB:StoredProcedure 에 있더군....옮겨야 겠지
	kQuery.QueryOwner(rkMemberGuid);

	kQuery.PushStrParam(byCreateMode);
	kQuery.PushStrParam(rkMemberGuid);
	kQuery.PushStrParam(kConvName);
	kQuery.PushStrParam(kReqCreateCharacter.byGender);
	kQuery.PushStrParam(kReqCreateCharacter.iClass);
	kQuery.PushStrParam(kCreateCharacterInfo.iClass);
	kQuery.PushStrParam(kReqCreateCharacter.iDefaultHair);

	kQuery.PushStrParam(kReqCreateCharacter.iDefaultHairColor);
	kQuery.PushStrParam(kReqCreateCharacter.iDefaultFace);
	kQuery.PushStrParam(kReqCreateCharacter.iDefaultShirts);
	kQuery.PushStrParam(kReqCreateCharacter.iDefaultGlove);
	kQuery.PushStrParam(kReqCreateCharacter.iDefaultPants);

	kQuery.PushStrParam(kReqCreateCharacter.iDefaultBoots);
	kQuery.PushStrParam(kReqCreateCharacter.byFiveElement_Body);


// 아래는 디폴트 값.
	kQuery.PushStrParam((*base_char_itor).second.f_State);
//	kQuery.PushStrParam((*base_char_itor).second.f_GMLevel);
	kQuery.PushStrParam((*base_char_itor).second.f_Race);

	kQuery.PushStrParam(kCreateCharacterInfo.iLv);	
	kQuery.PushStrParam(kCreateCharacterInfo.iExp);	
	kQuery.PushStrParam((*base_char_itor).second.f_Money);
	kQuery.PushStrParam(kCreateCharacterInfo.iHP);
	kQuery.PushStrParam(kCreateCharacterInfo.iMP);

	kQuery.PushStrParam((*base_char_itor).second.f_CP);
	kQuery.PushStrParam(kCreateCharacterInfo.iExtraSkillPoint);
	kQuery.PushStrParam((*base_char_itor).second.f_Skills, MAX_DB_SKILL_SIZE);
	kQuery.PushStrParam((*base_char_itor).second.f_QuickSlot, MAX_DB_QUICKSLOT_SIZE);
	kQuery.PushStrParam(kCreateCharacterInfo.pkIngQuest, MAX_DB_INGQUEST_SIZE);

	kQuery.PushStrParam(kCreateCharacterInfo.pkEndQuest, MAX_DB_ENDQUEST_SIZE);
	kQuery.PushStrParam(kCreateCharacterInfo.pkEndQuest2, MAX_DB_ENDQUEST_SIZE);
	kQuery.PushStrParam(kCreateCharacterInfo.pkEndQuestExt, MAX_DB_ENDQUEST_EXT_SIZE);
	kQuery.PushStrParam(kCreateCharacterInfo.pkEndQuestExt2, MAX_DB_ENDQUEST_EXT2_SIZE);	
	kQuery.PushStrParam((*base_char_itor).second.f_InvExtend, MAX_DB_INVEXTEND_SIZE);

	kQuery.PushStrParam(kReqCreateCharacter.kJumpingCharEvent.iEventNo);
	kQuery.PushStrParam(kCreateCharacterInfo.iJumpingCharEvent_MaxRewardCount);
	kQuery.PushStrParam(kCreateCharacterInfo.iJumpingCharEvent_ItemGroupNo);
	kQuery.PushStrParam(kCreateCharacterInfo.iMapNo);

	kQuery.PushStrParam(iCreateCharacterNum);	// 최대 생성 캐릭터 개수

	kQuery.PushStrParam(kReqCreateCharacter.iSetNo);
	kQuery.PushStrParam(iDrakanMinLv);
	kQuery.PushStrParam(iDrakanCreateItemNo);

	pPlayerData->kSelectQueryGuid = BM::GUID::Create();
	kQuery.QueryGuid(pPlayerData->kSelectQueryGuid);
	g_kCoreCenter.PushQuery(kQuery);

	return true;
__WEAR_FAILED:
	{	
		if(KCBW_NONE != eWrongWear)
		{
			kErrPacket.Push(CCR_WRONG_WEAR_ITEM);
			kErrPacket.Push(eWrongWear);// 어떤것을 잘못입었니?
			SendToSwitch(rkMemberGuid, kErrPacket);
			
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::CheckValidBaseWear(int const iWearNo, const EKindCharacterBaseWear eType)
{
	CONT_DEFCHARACTER_BASEWEAR const* pContDefCharacterBaseWear = NULL;
	g_kTblDataMgr.GetContDef(pContDefCharacterBaseWear);

	CONT_DEFCHARACTER_BASEWEAR::const_iterator wearItor = pContDefCharacterBaseWear->find(iWearNo);
	if(pContDefCharacterBaseWear->end() != wearItor)
	{
		if(eType == (*wearItor).second.iWearType)
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool PgRealmUserManager::Locked_Recv_PT_S_T_REQ_DELETE_CHARACTER(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SCenterPlayer_Lock kLockPlayer;
	if ( !GetPlayerInfo(rkMemberGuid, true, kLockPlayer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (kLockPlayer.pkCenterPlayerData->kSelectQueryGuid != BM::GUID::NullData())
	{
		// kSelectQueryGuid 가 있다는 것은, SELECT_CHARACTER,CREATE_CHARACTER,DELETE_CHARACTER 중에 한개가 진행중 이라는 중이다.
		// DOS 공격에 대비하여 바로 return
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Query Inprocessing...waiting DB_Result Member=") << kLockPlayer.pkCenterPlayerData->kID << _T(", M-G=") << kLockPlayer.pkCenterPlayerData->guidMember
			<< _T(", C-G=") << rkCharacterGuid << _T(", QueryGuid=") << kLockPlayer.pkCenterPlayerData->kSelectQueryGuid);
		return false;
	}
	kLockPlayer.pkCenterPlayerData->kSelectQueryGuid = BM::GUID::Create();

	int iDeleteMode = ECHAR_DEL_NAME_UNCHANGED; // 기본은 항상, 이름을 남기고, State를 1로 바꾼다. (모든 국가는 ICEE 측의 형태를 따라야 한다)
	static LOCAL_MGR::NATION_CODE const eNation = static_cast<LOCAL_MGR::NATION_CODE>(g_kLocal.ServiceRegion());
	switch(eNation)
	{
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_SINGAPORE:
	case LOCAL_MGR::NC_THAILAND:
	case LOCAL_MGR::NC_INDONESIA:
	case LOCAL_MGR::NC_PHILIPPINES:
	case LOCAL_MGR::NC_VIETNAM:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_TAIWAN:
		{
			iDeleteMode = ECHAR_DEL_NAME_CHANGED_TO_DUMMY;	// Name 바꾸고 State = 1
		}break;
	case LOCAL_MGR::NC_CHINA:
	default:
		{
			iDeleteMode = ECHAR_DEL_NAME_UNCHANGED;	// Name 유지하고 State = 1
		}break;
	}

	m_kOpenMarketMgr.Locked_RemoveMarket(rkCharacterGuid);

	m_kCardMgr.Locked_UnRegCard(rkCharacterGuid);

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_DELETECHARACTER, L"EXEC [dbo].[UP_DeleteUserCharacter]");
	kQuery.InsertQueryTarget(rkMemberGuid);
	kQuery.InsertQueryTarget(rkCharacterGuid);
	kQuery.QueryOwner(rkMemberGuid);
	kQuery.PushStrParam( rkCharacterGuid );
	kQuery.PushStrParam( int(1) );
	kQuery.PushStrParam( std::wstring() );
	kQuery.PushStrParam( iDeleteMode );
	kQuery.QueryGuid(kLockPlayer.pkCenterPlayerData->kSelectQueryGuid);
	g_kCoreCenter.PushQuery(kQuery);
	return true;
}

bool PgRealmUserManager::Locked_Recv_PT_C_N_REQ_REALM_MERGE(BM::GUID const& rkMemberGuid, BM::GUID const& rkCharacterGuid, std::wstring const& rkNewName)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SCenterPlayer_Lock kLockPlayer;
	if( !GetPlayerInfo(rkMemberGuid, true, kLockPlayer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRMR_Failed);
		return false;
	}

	if( kLockPlayer.pkCenterPlayerData->kSelectQueryGuid != BM::GUID::NullData() )
	{
		// kSelectQueryGuid 가 있다는 것은, SELECT_CHARACTER,CREATE_CHARACTER,DELETE_CHARACTER 중에 한개가 진행중 이라는 중이다.
		// DOS 공격에 대비하여 바로 return
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Query Inprocessing...waiting DB_Result Member=") << kLockPlayer.pkCenterPlayerData->kID << _T(", M-G=") << kLockPlayer.pkCenterPlayerData->guidMember
			<< _T(", C-G=") << rkCharacterGuid << _T(", QueryGuid=") << kLockPlayer.pkCenterPlayerData->kSelectQueryGuid);
		SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRMR_None);
		return false;
	}

	//if( !PgPlayerUtil::IsNeedRename(kLockPlayer.pkCenterPlayerData->pkPlayer->GetDBPlayerState) )
	//{
	//	SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRR_Failed);
	//	return false;
	//}

	if( rkNewName.empty() )
	{
		SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRMR_EmptyName);
		return false;
	}

	if( MAX_CHARACTER_NAME_LEN < rkNewName.size() )
	{
		SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRMR_MaxLen);
		return false;
	}

	std::wstring kTempStr(rkNewName);
	if( false == g_kUnicodeFilter.IsCorrect(UFFC_CHARACTER_NAME, kTempStr)
	||	true == g_kFilterString.Filter(kTempStr, false, FST_ALL) )
	{
		SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRMR_BadName);
		return false;
	}

	{
		SCenterPlayer_Lock kTempPlayer; // 중복 이름 체크를 위해서
		if( GetPlayerInfo(rkNewName, kTempPlayer) )
		{
			SendRenameResult(rkMemberGuid, rkCharacterGuid, rkNewName, UCRMR_Duplicate);
			return false;
		}
	}

	kLockPlayer.pkCenterPlayerData->kSelectQueryGuid = BM::GUID::Create();

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_REALMMERGE, L"EXEC [dbo].[UP_RealmMergeCharacter]");
	kQuery.InsertQueryTarget(rkMemberGuid);
	kQuery.InsertQueryTarget(rkCharacterGuid);
	kQuery.QueryOwner(rkMemberGuid);
	kQuery.PushStrParam( rkMemberGuid );
	kQuery.PushStrParam( rkCharacterGuid );
	kQuery.PushStrParam( rkNewName );
	kQuery.QueryGuid(kLockPlayer.pkCenterPlayerData->kSelectQueryGuid);
	g_kCoreCenter.PushQuery(kQuery);
	return true;
}

void PgRealmUserManager::Locked_DisplayState(bool bDisplayUserID)
{
	BM::CAutoMutex kLock(m_kMutex);

	if (bDisplayUserID)
	{
		U_STATE_LOG(BM::LOG_LV1, _T("------------ Logined User List -----------"));

		CONT_CENTER_PLAYER_BY_ID::const_iterator user_itor = m_kContPlayer_MemberID.begin();
		int iMyhomeCount = 0;
		while(user_itor != m_kContPlayer_MemberID.end())
		{
			CONT_CENTER_PLAYER_BY_ID::mapped_type pElement = (*user_itor).second;
			BM::CAutoMutex kLock((*user_itor).second->kMutex);
			if (pElement->pkPlayer != NULL)
			{
				if(pElement->pkPlayer->IsUnitType(UT_MYHOME))
				{
					++iMyhomeCount;
				}
				U_STATE_LOG(BM::LOG_LV6, L"LoginedUser ID["<<(*user_itor).first<<L"] "<<C2L(pElement->pkPlayer->GroundKey()));
			}
			else
			{
				U_STATE_LOG(BM::LOG_LV6, L"LoginedUser ID["<<(*user_itor).first<<L"] pkPlayer is NULL");
			}
			++user_itor;
		}
		CONT_CENTER_PLAYER_BY_KEY::const_iterator kOfflineUser = m_kContPlayerOffline_CharKey.begin();
		while(kOfflineUser != m_kContPlayerOffline_CharKey.end())
		{
			SCenterPlayer_Lock kLock;
			kLock.Set(kOfflineUser->second);
			if (kOfflineUser->second->pkPlayer)
			{
				U_STATE_LOG(BM::LOG_LV6, L"Offline User CharName["<<kOfflineUser->second->pkPlayer->Name()<<L"]");
			}
			else
			{
				U_STATE_LOG(BM::LOG_LV6, L"Offline User ID["<<kOfflineUser->first<<L"] is null");
			}
			++kOfflineUser;
		}
		U_STATE_LOG(BM::LOG_LV1, L"TotalUser Count["<<m_kContPlayer_MemberID.size()-iMyhomeCount<<L"],  Myhome Count["<<iMyhomeCount <<L"]");
		U_STATE_LOG(BM::LOG_LV1, L"TotalUser Member Count["<<m_kContPlayer_MemberID.size()<<L"],  m_Myhome Count["<<m_iMyHomeUnitCount <<L"]");
		U_STATE_LOG(BM::LOG_LV1, _T("--------------------- Cut Line ---------------------"));
	}
}

void PgRealmUserManager::SaveMemberSkillSetData(BM::GUID const & kID, CONT_USER_SKILLSET const & kContSkillSet, ESkillTabType const eType)const
{
	for(CONT_USER_SKILLSET::const_iterator c_iter = kContSkillSet.begin();
		c_iter!= kContSkillSet.end();++c_iter)
	{
		CEL::DB_QUERY kSaveSetQuery(DT_PLAYER, DQT_SAVE_SKILLSET, L"EXEC [dbo].[UP_SaveSkillSet]" );
		kSaveSetQuery.InsertQueryTarget(kID);
		kSaveSetQuery.QueryOwner(kID);
		kSaveSetQuery.PushStrParam(kID);

		CONT_USER_SKILLSET::mapped_type const & element = c_iter->second;
		kSaveSetQuery.PushStrParam(element.bySetNo);
		kSaveSetQuery.PushStrParam(static_cast<BYTE>(eType));
		for(int i=0;i<MAX_SKILLSET_GROUP;++i)
		{
			kSaveSetQuery.PushStrParam(element.byDelay[i]);
			kSaveSetQuery.PushStrParam(element.byType[i]);
			kSaveSetQuery.PushStrParam(element.iSkillNo[i]);
		}
		g_kCoreCenter.PushQuery(kSaveSetQuery);
	}
}

void PgRealmUserManager::SaveStrategySkillData(PgDoc_Player const * pkPlayer)const
{
	if( !pkPlayer->IsOpenStrategySkill(ESTT_SECOND) ){ return; }

	{// Second MySkillExtend
		BYTE abySkillExtends[MAX_DB_SKILL_EXTEND_SIZE];
		pkPlayer->GetStrategyMySkill()->SaveExtend(MAX_DB_SKILL_EXTEND_SIZE, abySkillExtends);

		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_SkillExtend]");
		kQuery.InsertQueryTarget(pkPlayer->GetID());
		kQuery.PushStrParam(pkPlayer->GetID());
		kQuery.PushStrParam(static_cast<BYTE>(true));
		kQuery.PushStrParam(abySkillExtends, MAX_DB_SKILL_EXTEND_SIZE);
		g_kCoreCenter.PushQuery(kQuery);
	}
	{// Second QuickInvSave
		BYTE abyQuickSlot[MAX_DB_QUICKSLOT_SIZE] = {0,};
		pkPlayer->GetStrategyQInv()->Save(abyQuickSlot);

		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVECHARACTER_EXTERN, L"EXEC [dbo].[up_Update_TB_UserCharacter_QuickSlot2]" );
		kQuery.InsertQueryTarget(pkPlayer->GetID());
		kQuery.PushStrParam(pkPlayer->GetID());
		kQuery.PushStrParam(static_cast<BYTE>(true));
		kQuery.PushStrParam( abyQuickSlot, MAX_DB_QUICKSLOT_SIZE );
		g_kCoreCenter.PushQuery(kQuery);
	}
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Field2]");
		kQuery.InsertQueryTarget(pkPlayer->GetID());
		kQuery.QueryOwner(pkPlayer->GetID());
		kQuery.PushStrParam(pkPlayer->GetID());
		kQuery.PushStrParam(std::wstring(_T("SkillTabNo")));
		kQuery.PushStrParam(pkPlayer->GetAbil(AT_STRATEGYSKILL_TABNO));
		g_kCoreCenter.PushQuery(kQuery);
	}
}

void PgRealmUserManager::SaveMemberData( CONT_CENTER_PLAYER_BY_KEY::mapped_type pData )
{
	if( pData )
	{
		BM::Stream kIMPacket( PT_T_IM_NFY_USER_LOGOUT, pData->guidMember );
		kIMPacket.Push(pData->i64TotalConnSec);
		kIMPacket.Push(pData->iAccConSec);
		kIMPacket.Push(pData->iAccDisSec);

		if(pData->pkPlayer)
		{
			if(UT_PLAYER != pData->pkPlayer->UnitType())
			{
				return;
			}

			SendToImmigration( kIMPacket );

			PgDoc_Player *pkPlayer =  pData->pkPlayer;

//			pkPlayer->ClearAllEffect(true,true);	// 이펙트 걸린거 지우고
//			pkPlayer->GetInven()->ClearCoolTime();	// 쿨타임 남은거 지운다.

			{//정보 저장.
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVECHARACTER, L"EXEC [dbo].[UP_SaveCharacter6]" );
				kQuery.InsertQueryTarget(pkPlayer->GetID());
				pkPlayer->WriteToQuery( kQuery, EDBSAVE_DEFAULT );
				g_kCoreCenter.PushQuery(kQuery);
			}
			SaveStrategySkillData(pkPlayer);

			//옵션 저장.
			size_t const iSize = sizeof(pkPlayer->m_kClientOption);

			CEL::DB_QUERY kOptQuery(DT_PLAYER, DQT_SAVE_CLIENTOPTION, L"EXEC [dbo].[UP_SaveClientOption]" );
			kOptQuery.InsertQueryTarget(pkPlayer->GetID());
			kOptQuery.QueryOwner(pkPlayer->GetID());
			kOptQuery.PushStrParam(pkPlayer->GetID());
			kOptQuery.PushStrParam(reinterpret_cast<BYTE*>(&(pkPlayer->m_kClientOption)), iSize);
			g_kCoreCenter.PushQuery(kOptQuery);

			{//스킬세트 저장.
				BYTE const bySkillSetItemCount = static_cast<BYTE>(pkPlayer->GetAbil(AT_SKILLSET_USE_COUNT));
				CEL::DB_QUERY kCountQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_SkillSetItemCount]");
				kCountQuery.InsertQueryTarget(pkPlayer->GetID());
				kCountQuery.PushStrParam(pkPlayer->GetID());
				kCountQuery.PushStrParam(bySkillSetItemCount);
				g_kCoreCenter.PushQuery(kCountQuery);

				SaveMemberSkillSetData(pkPlayer->GetID(), pkPlayer->GetMySkill()->ContSkillSet(), ESTT_BASIC);
				SaveMemberSkillSetData(pkPlayer->GetID(), pkPlayer->GetStrategyMySkill()->ContSkillSet(), ESTT_SECOND);
			}

			{//직업3차 최근제작 아이템정보 저장.
				CONT_HISTORYJOBITEM const& rkContJSHistoryItem = pkPlayer->ContJobSkillHistoryItem();
				std::vector<CONT_HISTORYJOBITEM::value_type> kCont(rkContJSHistoryItem.rbegin(),rkContJSHistoryItem.rend());
				kCont.resize(MAX_JOBSKILL_HISTORYITEM);

				CEL::DB_QUERY kOptQuery(DT_PLAYER, DQT_UPDATE_USER_JOBSKILL_HISTORYITEM, L"EXEC [dbo].[UP_UserJobSkill_HistoryItem_Update]" );
				kOptQuery.InsertQueryTarget(pkPlayer->GetID());
				kOptQuery.QueryOwner(pkPlayer->GetID());
				kOptQuery.PushStrParam(pkPlayer->GetID());
				kOptQuery.PushStrParam(reinterpret_cast<BYTE*>(&kCont.at(0)), MAX_DB_JOBKSILL_HISTORYITEM_SIZE);
				g_kCoreCenter.PushQuery(kOptQuery);
			}

			if( pkPlayer->GetPremium().IsUserService(true) )
			{//프리미엄 서비스 저장
				BM::Stream kCustomData;
				pkPlayer->GetPremium().ToDB(kCustomData);

				if(false==kCustomData.IsEmpty())
				{
					CEL::DB_QUERY kOptQuery(DT_PLAYER, DQT_UPDATE_PREMIUM_CUSTOM_DATA, L"EXEC [dbo].[UP_Update_PremiumCustomData]" );
					kOptQuery.InsertQueryTarget(pkPlayer->GetID());
					kOptQuery.QueryOwner(pkPlayer->GetID());
					kOptQuery.PushStrParam(pkPlayer->GetID());
					kOptQuery.PushStrParam(kCustomData.Data());
					g_kCoreCenter.PushQuery(kOptQuery);
				}
			}
		}
		else
		{
			SendToImmigration( kIMPacket );
		}
	}
}

void PgRealmUserManager::Locked_Recv_PT_C_T_REQ_SAVE_OPTION(BM::GUID const rkCharacterGuid, ClientOption const & rkOption)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkCharacterGuid, false, kLockPlayer) )
	{
		PgDoc_Player* pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
		if (pkPlayer != NULL)
		{
			pkPlayer->m_kClientOption = rkOption;
		}
	}
}

void PgRealmUserManager::Locked_Recv_PT_N_T_REQ_JOBSKILL3_HISTORYITEM(BM::GUID const rkCharacterGuid, int const iSaveIdx)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkCharacterGuid, false, kLockPlayer) )
	{
		PgDoc_Player* pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
		if (pkPlayer != NULL)
		{
			if( pkPlayer->AddJobSkillHistoryItem(iSaveIdx) )
			{
				BM::Stream kPacket(PT_M_C_NFY_JOBSKILL3_HISTORYITEM);
				kPacket.Push(pkPlayer->ContJobSkillHistoryItem());
				SendToUser( pkPlayer->GetMemberGUID(), kPacket, true );
			}
		}
	}
}

void PgRealmUserManager::Locked_Recv_PT_C_T_REQ_SAVE_SKILLSET(BM::GUID const rkCharacterGuid, CONT_USER_SKILLSET const & rkSkillSet)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkCharacterGuid, false, kLockPlayer) )
	{
		PgDoc_Player* pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
		if (pkPlayer != NULL)
		{
			pkPlayer->GetMySkill()->ContSkillSet(rkSkillSet);
			BM::Stream kPacket(PT_M_C_ANS_SAVE_SKILLSET);
			SendToUser(rkCharacterGuid,kPacket,false);
		}
	}
}

bool PgRealmUserManager::Locked_Recv_PT_S_T_REQ_CHARACTER_LIST(BM::GUID const &rkMemberGuid, short const nChannelNo)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	
	BM::GUID kQueryGuid = BM::GUID::Create();
	{
		SCenterPlayer_Lock kLockPlayer;
		if(GetPlayerInfo(rkMemberGuid, true, kLockPlayer) 
		&&	kLockPlayer.pkCenterPlayerData->kGetListQueryGuid == BM::GUID::NullData())
		{//쿼리 셋팅.
			kLockPlayer.pkCenterPlayerData->kGetListQueryGuid = kQueryGuid;
		}
		else
		{//유저 없거나. 쿼리 시행중이면 에러.
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	CEL::DB_QUERY kQueryPremium(DT_MEMBER, DQT_GET_MEMBER_PREMIUM_SERVICE, L"EXEC [dbo].[up_GetPremiumService]");
	kQueryPremium.QueryGuid(kQueryGuid);
	kQueryPremium.QueryOwner(rkMemberGuid);
	kQueryPremium.InsertQueryTarget(rkMemberGuid);
	kQueryPremium.PushStrParam( rkMemberGuid );	// 인자값:멤버GUID
	kQueryPremium.contUserData.Push(EPQT_NONE);
	kQueryPremium.contUserData.Push(nChannelNo);
	g_kCoreCenter.PushQuery(kQueryPremium);
	return true;
}

bool PgRealmUserManager::CanReserveNewAccount()const
{
	size_t const iCurrent = m_kContPlayer_MemberKey.size() - m_iMyHomeUnitCount;
	if ( MaxPlayerCount() < (iCurrent + 5) )	// 약간의 여유를 두어야 MaxPlayerCount()를 넘어서는 일이 없다.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgRealmUserManager::Locked_Q_DQT_DELETECHARACTER(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::Stream kPacket(PT_T_S_ANS_DELETE_CHARACTER); 
	BM::GUID kCharacterGuid;// 삭제된 캐릭터의 캐릭터GUID
	int iErrNo = 0;// 1 : 삭제성공

	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Query failed. Delete character ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		
		kPacket.Push(rkResult.QueryOwner());
		kPacket.Push((BYTE)UCDR_Failed);
		kPacket.Push(BM::GUID::NullData());
		SendToSwitch(rkResult.QueryOwner(), kPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	std::wstring kDeletePlayerName;
	BM::GUID kGuildGuid, kCoupleGuid;
	short sClass = 0;
	short sLevel = 0;
	__int64 i64Exp = 0;
	char cGender = 0;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	if(rkResult.vecArray.end() != itor)
	{
		//로그를 남겨야하기 때문에 성공 실패 여부에 관계 없이 일단 데이터 가져옴.
		(*itor).Pop(kCharacterGuid);		++itor;
		(*itor).Pop(iErrNo);				++itor;
		(*itor).Pop( kDeletePlayerName );	++itor;
		(*itor).Pop( kGuildGuid );			++itor;
		(*itor).Pop( kCoupleGuid );			++itor;
		(*itor).Pop( sClass );				++itor;
		(*itor).Pop( sLevel );				++itor;
		(*itor).Pop( i64Exp );				++itor;
		(*itor).Pop( cGender );				++itor;
	}

	m_kOpenMarketMgr.Locked_RemoveMarket(kCharacterGuid);

	if(UCDR_Success == iErrNo )
	{	//성공
		if( BM::GUID::IsNotNull(kGuildGuid) )//길드 탈퇴 알림
		{
			BM::Stream kGuildPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
			kGuildPacket.Push((BYTE)GC_Leave);//삭제 == Leave
			kGuildPacket.Push((BYTE)GCR_Success);//성공
			kGuildPacket.Push(kGuildGuid);
			kGuildPacket.Push(kCharacterGuid);
			SendToGuildMgr(kGuildPacket);
		}

		{// 길드가입 신청 취소
			BM::Stream kGuildPacket(PT_C_N_REQ_GUILD_ENTRANCE_CANCEL);
			kGuildPacket.Push(kCharacterGuid);			
			SendToGuildMgr(kGuildPacket);
		}

		if( BM::GUID::IsNotNull(kCoupleGuid) )//커플 해어짐 알림
		{
			BM::Stream kCouplePacket(PT_T_N_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Break);
			kCouplePacket.Push( (BYTE)CoupleCR_Success );
			kCouplePacket.Push( kCoupleGuid );
			kCouplePacket.Push( kCharacterGuid );
			kCouplePacket.Push( kDeletePlayerName );
			SendToCoupleMgr(kCouplePacket);
		}

		{
			BM::Stream kPacket(PT_S_T_REQ_DELETE_CHARACTER);
			kPacket.Push( rkResult.QueryOwner() );
			kPacket.Push( kCharacterGuid );
			::SendToRealmContents(PMET_WEB_HELPER, kPacket);
		}
	}
	else
	{//오류 발생
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Delete character error. ErrNo[") << iErrNo << _T("]") );// return 하지않음
	}

	kPacket.Push(rkResult.QueryOwner());
	kPacket.Push((BYTE)iErrNo);
	kPacket.Push(kCharacterGuid);
	
	SendToSwitch(rkResult.QueryOwner(), kPacket);//삭제 결과를 전달 할 수 있게 해야함.

	SCenterPlayer_Lock kPlayer_Lock;
	GetPlayerInfo(rkResult.QueryOwner(), true, kPlayer_Lock);
	if(kPlayer_Lock.pkCenterPlayerData != NULL && kPlayer_Lock.pkCenterPlayerData->kSelectQueryGuid == rkResult.QueryGuid())
	{
		kPlayer_Lock.pkCenterPlayerData->kSelectQueryGuid.Clear();
	}
	//캐릭터 삭제 로그
	PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Create);
	kLogCont.MemberKey(rkResult.QueryOwner());
	kLogCont.CharacterKey(kCharacterGuid);

	if(kPlayer_Lock.pkCenterPlayerData != NULL)
	{
		kLogCont.ID(kPlayer_Lock.pkCenterPlayerData->kID);	
		kLogCont.ChannelNo(kPlayer_Lock.pkCenterPlayerData->ChannelNo());
		kLogCont.UID(kPlayer_Lock.pkCenterPlayerData->iUID);
	}

	kLogCont.Name(kDeletePlayerName);

	kLogCont.Class(sClass);
	kLogCont.Level(sLevel);

	GET_DEF(PgClassDefMgr, kClassDefMgr);
	SPlayerBasicInfo kBasicInfo;
	kClassDefMgr.GetAbil(SClassKey(static_cast<int>(sClass), static_cast<int>(sLevel)), kBasicInfo);
	std::wstring kState;
	std::wstring kQwner = L"GAME";
	wchar_t wstrBuff[100];
	swprintf_s(wstrBuff, L"%hd / %hd / %hd / %hd / %hd / %hd", kBasicInfo.sInitStr,	kBasicInfo.sInitInt, 
		kBasicInfo.sInitDex, kBasicInfo.sInitCon, kBasicInfo.sPhysicsDefence, kBasicInfo.sMagicDefence);
	kState = wstrBuff;
	
	PgLog kLog(ELOrderMain_Character, ELOrderSub_Delete);
	kLog.Set(0, kQwner);
	kLog.Set(1, kState);
	kLog.Set(0, iErrNo);
	kLog.Set(1, static_cast<int>(cGender) );
	kLog.Set(1, i64Exp);

	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}

bool PgRealmUserManager::Locked_Q_DQT_REALMMERGE(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::GUID kCharacterGuid; // 이름이 변경된 캐릭터GUID
	std::wstring kNewName; // 새로운 이름
	int iErrNo = 0;// 1 : 삭제성공

	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Query failed. Delete character ErrorCode[") << rkResult.eRet << _T("]") );
		
		SendRenameResult(rkResult.QueryOwner(), BM::GUID::NullData(), std::wstring(), UCRMR_None);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring kOrgName;
	short sClass = 0;
	short sLevel = 0;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	if( rkResult.vecArray.end() != itor )
	{
		(*itor).Pop( iErrNo );				++itor;
		(*itor).Pop( kCharacterGuid );		++itor;
		(*itor).Pop( kNewName );			++itor;
		(*itor).Pop( kOrgName );			++itor;
		(*itor).Pop( sClass );				++itor;
		(*itor).Pop( sLevel );				++itor;
	}

	if( UCRMR_Success == iErrNo )
	{
		// 각각 컨텐츠의 메모리 정보(하지 않는다.)
		// 오픈 마켓
		// 길드, 친구, 커플는 로그인/로그아웃시에 만 하도록
	}

	SendRenameResult(rkResult.QueryOwner(), kCharacterGuid, kNewName, static_cast< EUserCharacterRealmMergeReturn >(iErrNo));

	SCenterPlayer_Lock kPlayer_Lock;
	GetPlayerInfo(rkResult.QueryOwner(), true, kPlayer_Lock);
	if(kPlayer_Lock.pkCenterPlayerData != NULL && kPlayer_Lock.pkCenterPlayerData->kSelectQueryGuid == rkResult.QueryGuid())
	{
		kPlayer_Lock.pkCenterPlayerData->kSelectQueryGuid.Clear();
	}

	// 이름 변경 로그
	PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Merge);
	kLogCont.MemberKey(rkResult.QueryOwner());
	kLogCont.CharacterKey(kCharacterGuid);

	if( NULL != kPlayer_Lock.pkCenterPlayerData )
	{
		kLogCont.ID(kPlayer_Lock.pkCenterPlayerData->kID);
		kLogCont.ChannelNo(kPlayer_Lock.pkCenterPlayerData->ChannelNo());
		kLogCont.UID(kPlayer_Lock.pkCenterPlayerData->iUID);
	}

	kLogCont.Name(kNewName);
	kLogCont.Class(sClass);
	kLogCont.Level(sLevel);
	
	PgLog kLog(ELOrderMain_Character, ELOrderSub_Modify);
	kLog.Set(0, kOrgName);
	kLog.Set(0, iErrNo);

	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}

bool PgRealmUserManager::Locked_Q_DQT_CHECK_CHARACTERNAME_OVERLAP(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Query failed ErrorCode[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iErrNo = 0;
	std::wstring kCharacterName;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itor)
	{
		(*itor).Pop(iErrNo);			++itor;
		(*itor).Pop(kCharacterName);	++itor;
	}

	BM::Stream kPacket(PT_N_C_ANS_CHECK_CHARACTERNAME_OVERLAP, rkResult.QueryOwner());

	if( 0 == iErrNo )
	{
		kPacket.Push(UCRMR_None);
	}
	else
	{
		kPacket.Push( iErrNo );
	}

	kPacket.Push(kCharacterName);
	SendToSwitch(rkResult.QueryOwner(), kPacket);

	return true;
}

bool PgRealmUserManager::Locked_Q_DQT_CREATECHARACTER(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Query failed. Create character ErrorCode[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kCharacterGuid;
	int iErrNo = 0;// ECreateCharacterResult 의 값을 사용함
	unsigned char ucGender;	// 성별
	int iClassNo = 0;		// 클래스번호
	std::wstring kName;		// 캐릭터 이름
	int iJumpingEventNo = 0;	//점핑 캐릭터 이벤트 번호

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itor)
	{
		(*itor).Pop(iErrNo);			++itor;
		(*itor).Pop(kCharacterGuid);	++itor;
		(*itor).Pop(ucGender);			++itor;
		(*itor).Pop(iClassNo);			++itor;
		(*itor).Pop(kName);				++itor;
		(*itor).Pop(iJumpingEventNo);	++itor;
	}

	// 생성을 성공하건 실패하건 아래의 패킷을 클라에게 보내면 클라는 캐릭터생성창에서 캐릭터선택창으로 이동해야 한다
	// 이유 : 캐릭터가 생성된 직후 서버메모리에 추가된  캐릭터 갯수가 갱신되지 않으므로
	//		  캐릭터선택창에 나가서 DB로 부터 내 캐릭터목록을 쭈루룩 받아서 서버메모리를 갱신시켜야 함!

	BM::Stream kPacket(PT_T_S_ANS_CREATE_CHARACTER, rkResult.QueryOwner());

	if(0 == iErrNo)
	{
		kPacket.Push(CCR_SUCCESS);
		kPacket.Push(kCharacterGuid);
	}
	else
	{
		kPacket.Push(iErrNo);//ECreateCharacterResult의 값과 동일
	}

	SendToSwitch(rkResult.QueryOwner(), kPacket);

	SCenterPlayer_Lock kLockPlayer;
	if(GetPlayerInfo(rkResult.QueryOwner(), true, kLockPlayer) )
	{
		if (kLockPlayer.pkCenterPlayerData->kSelectQueryGuid == rkResult.QueryGuid())
		{
			kLockPlayer.pkCenterPlayerData->kSelectQueryGuid.Clear();
		}

		// 캐릭터 생성 로그
		PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Create);
		kLogCont.MemberKey(kLockPlayer.pkCenterPlayerData->guidMember);
		kLogCont.UID(kLockPlayer.pkCenterPlayerData->iUID);
		kLogCont.CharacterKey(kCharacterGuid);
		kLogCont.ID(kLockPlayer.pkCenterPlayerData->kID);
		kLogCont.Name(kName);
		kLogCont.Class(iClassNo);
		kLogCont.Level(1);	// 추후 수정할 수 있음.
		kLogCont.ChannelNo(kLockPlayer.pkCenterPlayerData->sChannelNo);

		std::wstring kState;
		if(0 == iErrNo)
		{
			GET_DEF(PgClassDefMgr, kClassDefMgr);
			SPlayerBasicInfo kBasicInfo;
			kClassDefMgr.GetAbil(SClassKey(iClassNo, 1), kBasicInfo);
			wchar_t wstrBuff[100];
			swprintf_s(wstrBuff, L"%hd / %hd / %hd / %hd / %hd / %hd", kBasicInfo.sInitStr,	kBasicInfo.sInitInt, 
				kBasicInfo.sInitDex, kBasicInfo.sInitCon, kBasicInfo.sPhysicsDefence, kBasicInfo.sMagicDefence);
			kState = wstrBuff;
		}

		PgLog kLog(ELOrderMain_Character, ELOrderSub_Create);
		std::wstring kQwner = L"GAME";
		kLog.Set(0, kQwner);
		kLog.Set(0, iErrNo);
		kLog.Set(1, static_cast<int>(ucGender));
		kLog.Set(2, iJumpingEventNo);
		kLog.Set(1, kState);

		kLogCont.Add(kLog);
		kLogCont.Commit();
	}
	
	return true;
}

HRESULT PgRealmUserManager::Locked_OnGreetingServer(SERVER_IDENTITY const &kRecvSI, CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	/*
	if ( kRecvSI.nServerType == CEL::ST_MAP )
	{
		SServerBalance* pkBalance = GetServerBalance(kRecvSI);
		if ( pkBalance )
		{
			pkBalance->Live(true);
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1,_T("[%s] Not Found Realm[%d] Channel[%d] ServerNo[%d]"), __FUNCTIONW__, kRecvSI.nRealm, kRecvSI.nChannel, kRecvSI.nServerNo );
			return E_FAIL;
		}
	}
	*/
	return S_OK;
}

HRESULT PgRealmUserManager::Locked_GetPlayerInfo(BM::GUID const& rkGuid, bool const bIsMemberGuid, PgPremiumMgr & rkPremiumMgr) const
{
	BM::CAutoMutex kLock(m_kMutex);

	SCenterPlayer_Lock kLockPlayer;
	if (!GetPlayerInfo(rkGuid, bIsMemberGuid, kLockPlayer))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return E_FAIL;
	}

	SCenterPlayerData * pkCenterPlayerData = kLockPlayer.pkCenterPlayerData;
	if(!pkCenterPlayerData || !pkCenterPlayerData->pkPlayer)
	{
		return E_FAIL;
	}

	PgDoc_Player * pkPlayer = pkCenterPlayerData->pkPlayer;

	pkPlayer->GetPremium().CopyTo(rkPremiumMgr);
	return S_OK;
}

HRESULT PgRealmUserManager::Locked_GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, SUserInfoExt* const pkExt) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetPlayerInfo(rkGuid, bIsMemberGuid, rkOut, pkExt);
}

HRESULT PgRealmUserManager::GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, SUserInfoExt* const pkExt) const
{
	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkGuid, bIsMemberGuid, kLockPlayer) )
	{
		SCenterPlayerData const *pkElement = kLockPlayer.pkCenterPlayerData;
		if( pkElement
		&&	pkElement->pkPlayer )
		{
			if( false == pkElement->Copy(rkOut) )
			{
				return E_FAIL;
			}
			if (pkExt != NULL)
			{
				if (false == pkElement->Copy(*pkExt))
				{
					return E_FAIL;
				}
			}
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgRealmUserManager::Locked_GetPlayerInfo(std::wstring const &rkCharName, SContentsUser &rkOut) const
{
	BM::CAutoMutex kLock(m_kMutex);
	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkCharName, kLockPlayer) )
	{
		SCenterPlayerData* const pkElement = kLockPlayer.pkCenterPlayerData;
		if( pkElement
		&&	pkElement->pkPlayer )
		{
			if( pkElement->Copy(rkOut) )
			{
				return S_OK;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgRealmUserManager::Locked_GetPlayerInven(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::GUID const& kItemGuid, PgBase_Item & rkOutItem) const
{
	BM::CAutoMutex kLock(m_kMutex);
	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(rkGuid, bIsMemberGuid, kLockPlayer) )
	{
		SCenterPlayerData* const pkElement = kLockPlayer.pkCenterPlayerData;
		if( pkElement
		&&	pkElement->pkPlayer )
		{
			return pkElement->pkPlayer->GetInven()->GetItem(kItemGuid, rkOutItem);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgRealmUserManager::Locked_WriteToPacket_User(BM::Stream& rkPacket, EServerSetMgrPacketOp const eOp)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CENTER_PLAYER_BY_KEY::const_iterator itor = m_kContPlayer_MemberKey.begin();
	VEC_SwitchPlayerData kPlayerVector;
	while(m_kContPlayer_MemberKey.end() != itor)
	{
		BM::CAutoMutex kLock(itor->second->kMutex);
		SSwitchPlayerData kInfo;
		itor->second->Copy(kInfo);
		kPlayerVector.push_back(kInfo);
		++itor;
	}
	PU::TWriteArray_M(rkPacket, kPlayerVector);
}

size_t PgRealmUserManager::Locked_CurrentPlayerCount()const
{
	BM::CAutoMutex kLock(m_kMutex);
	int iHomeSize = 0;
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itor_pl = m_kContPlayer_MemberKey.begin();
	while (m_kContPlayer_MemberKey.end() != itor_pl)
	{
		BM::CAutoMutex kLock2((*itor_pl).second->kMutex);
		if (NULL != (*itor_pl).second && NULL != (*itor_pl).second->pkPlayer)
		{
			EUnitType const eType = (*itor_pl).second->pkPlayer->UnitType();
			if (eType == UT_MYHOME)
			{
				++iHomeSize;	
			}
		}
		++itor_pl;
	}
	return m_kContPlayer_MemberKey.size() - iHomeSize;
}

size_t PgRealmUserManager::Locked_CurrentPlayerCount( CONT_CHANNEL_USERCOUNT &rkCont )const
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t	iCount = 0;

	CONT_CENTER_PLAYER_BY_KEY::const_iterator key_itr = m_kContPlayer_MemberKey.begin();
	for ( ; key_itr != m_kContPlayer_MemberKey.end() ; ++key_itr )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type const pCData = key_itr->second;
		if ( pCData )
		{
			BM::CAutoMutex kLock2(pCData->kMutex);
			if ( pCData->pkPlayer )
			{
				if ( UT_PLAYER == pCData->pkPlayer->UnitType() )
				{
					auto kPair = rkCont.insert( std::make_pair( pCData->pkPlayer->GetChannel(), 0 ) );
					++(kPair.first->second);
					++iCount;
				}
			}
			else
			{
				auto kPair = rkCont.insert( std::make_pair( pCData->kSwitchServer.nChannel, 0 ) );
				++(kPair.first->second);
				++iCount;
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"CONT_CENTER_PLAYER_BY_KEY::mapped_type is NULL");
		}
	}

	return iCount;
}

void PgRealmUserManager::_ClearSelectCharacter( CONT_CENTER_PLAYER_BY_KEY::mapped_type pkSelectedPlayerData )
{
	// PkPlayer를 초기화 해줌으로써 
	// 다시 시도 할 수 있게 해준다.
	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" ") << pkSelectedPlayerData->pkPlayer->Name().c_str() << _T("[") << pkSelectedPlayerData->pkPlayer->GetID().str().c_str() << _T("]") );

	pkSelectedPlayerData->guidCharacter.Clear();
	pkSelectedPlayerData->pkPlayer = NULL;
}

__int64 PgRealmUserManager::_UpdatePlayTime( CONT_CENTER_PLAYER_BY_KEY::mapped_type pData )
{//4줄 주석.
	if( (pData == NULL) || (pData->pkPlayer == NULL) )
	{
		return 0;

	}
	__int64 const i64ThisConnSec = pData->pkPlayer->GetThisConnSec_Character();
	pData->pkPlayer->Update_PlayTime(BM::GetTime32());
	pData->iAccConSec = pData->pkPlayer->GetAccConnSec_Member();
	pData->iAccDisSec = pData->pkPlayer->GetAccDisConnSec_Member();
	pData->i64TotalConnSec += i64ThisConnSec;
	return i64ThisConnSec;
}

bool PgRealmUserManager::RemovePlayer(CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPD)
{
	if( m_kContPlayer_MemberKey.end() != m_kContPlayer_MemberKey.find(pCPD->guidMember)	
		&&	m_kContPlayer_MemberID.end() != m_kContPlayer_MemberID.find(pCPD->kID))
	{
		m_kContPlayer_MemberKey.erase(pCPD->guidMember);
		m_kContPlayer_MemberID.erase(pCPD->kID);

		PgDoc_Player* pkPlayer = pCPD->pkPlayer;
		if (!pkPlayer)
			return true;

		CONT_CENTER_PLAYER_BY_KEY::iterator pkPlayerByIdIter = m_kContPlayer_CharKey.find(pkPlayer->GetID());
		CONT_CENTER_PLAYER_BY_ID::iterator pkPlayerByNameIter = m_kContPlayer_CharName.find(pkPlayer->Name());
		if( m_kContPlayer_CharKey.end() != pkPlayerByIdIter && m_kContPlayer_CharName.end() != pkPlayerByNameIter)
		{
			m_kContPlayerOffline_CharKey.insert(std::make_pair(pkPlayer->GetID(), pkPlayerByIdIter->second));
			pkPlayerByIdIter->second->dwLogoutTime = BM::GetTime32();
			m_kContPlayer_CharKey.erase(pkPlayer->GetID());
			m_kContPlayer_CharName.erase(pkPlayer->Name());
		}

		/*
		if (pkPlayer != NULL)
		{
			SRealmGroundKey kRGroundKey(pCPD->ChannelNo(), pCPD->pkPlayer->GroundKey());
			CONT_CENTER_PLAYER_BY_GROUNDKEY::iterator itor_groundkey = m_kContPlayer_GroundKey.find(kRGroundKey);
			if (itor_groundkey == m_kContPlayer_GroundKey.end())
			{
				if (kRGroundKey.GroundKey().GroundNo() != PvP_Lobby_GroundNo)
				{
					INFO_LOG(BM::LOG_LV5, _T("[%s] Cannot find RGroundKey[%s]"), __FUNCTIONW__, kRGroundKey.str().c_str());
				}
			}
			else
			{
				if (bDoGndLock)
				{
					if ( !itor_groundkey->second->RemoveUser_MemberKey(pCPD->guidMember) )
					{
						INFO_LOG(BM::LOG_LV5, _T("[%s] Cannot Remove from GroundCont RGroundKey[%s], MemberGuid[%s]"), __FUNCTIONW__,
							kRGroundKey.str().c_str(), pCPD->guidMember.str().c_str());
					}				}
				else
				{
					if ( !itor_groundkey->second->Locked_RemoveUser_MemberKey(pCPD->guidMember) )
					{
						INFO_LOG(BM::LOG_LV5, _T("[%s] Cannot Remove from GroundCont RGroundKey[%s], MemberGuid[%s]"), __FUNCTIONW__,
							kRGroundKey.str().c_str(), pCPD->guidMember.str().c_str());
					}
				}
			}
		}
		*/
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgRealmUserManager::Locked_ClearPlayerWaiter(ACE_Time_Value const &kNowTime)
{//속도 저하가 올 수도 있으니. 주의.
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_PLAYER_WAITER::iterator player_waiter_itor = m_kContPlayerWaiter.begin();
	while(player_waiter_itor != m_kContPlayerWaiter.end())
	{
		CONT_PLAYER_WAITER::mapped_type pkPlayer = (*player_waiter_itor).second;

		if(pkPlayer->CanDelete(kNowTime))
		{
			SAFE_DELETE(pkPlayer);
			m_kContPlayerWaiter.erase(player_waiter_itor++);
			continue;
		}
		
		++player_waiter_itor;
	}
}

void PgRealmUserManager::ProcessInventoryTimeOut(PgDoc_Player *pkPlayer)
{
	{
		CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
		//기간제 캐시 아이템 삭제 요청 처리 
		pkPlayer->GetInven()->ProcessTimeOutedItem(kContModifyOrder, pkPlayer);
		if(false == kContModifyOrder.empty())
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->InsertTarget(pkPlayer->GetID());
			pkActionOrder->kCause = CIE_TimeOut;
			g_kJobDispatcher.VPush(pkActionOrder);
		}
	}
	{
		CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
		//기간제 몬스터 카드 처리 요청
		pkPlayer->GetInven()->ProcessTimeOutedMonsterCard(kContModifyOrder, pkPlayer);

		if(false == kContModifyOrder.empty())
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->InsertTarget(pkPlayer->GetID());
			pkActionOrder->kCause = CIE_TimeOut;
			g_kJobDispatcher.VPush(pkActionOrder);
		}
	}
}

void PgRealmUserManager::RemoveOfflinePlayer(CONT_CENTER_PLAYER_BY_KEY::key_type kPlayerKey)
{
	CONT_CENTER_PLAYER_BY_KEY::iterator kFindOfflinePlayer = m_kContPlayerOffline_CharKey.find(kPlayerKey);
	if (kFindOfflinePlayer == m_kContPlayerOffline_CharKey.end())
		return;
	SCenterPlayer_Lock kLock;
	kLock.Set(kFindOfflinePlayer->second);
	SAFE_DELETE(kFindOfflinePlayer->second->pkPlayer);
	SAFE_DELETE(kFindOfflinePlayer->second->pkBackupPlayer);
	SAFE_DELETE(kFindOfflinePlayer->second);
	m_kContPlayerOffline_CharKey.erase(kFindOfflinePlayer);
}

HRESULT PgRealmUserManager::ProcessSelectPlayer(PgDoc_Player *pkPlayer, BM::GUID const &kSelectGuid, bool const bPassTutorial)
{
	if(!pkPlayer)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"pkPlayer is NULL!!!!");
		return E_FAIL;
	}

	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(pkPlayer->GetMemberGUID(), true, kLockPlayer) )
	{
		SCenterPlayerData* pkMemberData = kLockPlayer.pkCenterPlayerData;
		if(!pkMemberData)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL; 
		}

		//INFO_LOG(BM::LOG_LV6, __FL__ << _T("HP=") << pkPlayer->GetAbil(AT_HP));
		if ( kSelectGuid != kLockPlayer.pkCenterPlayerData->kSelectQueryGuid )
		{
			CAUTION_LOG( BM::LOG_LV5, _T("Player<") << pkPlayer->GetMemberGUID() << _T("> SelectQueryGuid is Different : ") << kLockPlayer.pkCenterPlayerData->kSelectQueryGuid << _T(" <-> ") << kSelectGuid );
			return E_FAIL;
		}

		kLockPlayer.pkCenterPlayerData->kSelectQueryGuid.Clear();//쿼리 진행 삭제.
		
		if ( pkMemberData->pkPlayer )
		{
			CAUTION_LOG( BM::LOG_LV5, _T("Player is NOT NULL OldCharacter<") << pkMemberData->pkPlayer->Name() << _T("/") << pkMemberData->pkPlayer->GetID() << _T("> NewCharacter<") << pkPlayer->Name() << _T("/") << pkPlayer->GetID() << _T(">") );
			return E_FAIL;
		}

		if( g_kDefPlayTime.IsUse() 
		&& (g_kDefPlayTime.PlayTimeSec() < pkMemberData->iAccConSec) )
		{
			if( !g_kDefPlayTime.IsEnable(PgDefPlayerPlayTimeImpl::EDPTF_SELECTCHARACTER) )
			{
				int iRemainSec = g_kDefPlayTime.CalcRemainSec(pkMemberData->iAccConSec,pkMemberData->iAccDisSec);

				BM::Stream kPacket( PT_T_C_ANS_SELECT_CHARACTER, E_SCR_PLAYTIMEOVER );
				kPacket.Push(pkPlayer->GetMemberGUID());
				kPacket.Push(iRemainSec);
				SendToSwitch(pkPlayer->GetMemberGUID(), kPacket);
				return E_FAIL;
			}
		}

		// 휴식 경험치 계산
		static int s_iExpAdd_MaxExpRate = -1;	// 얻을수 있는 최대 경험치 %
		if (s_iExpAdd_MaxExpRate == -1)
		{
			s_iExpAdd_MaxExpRate = 0;
			g_kVariableContainer.Get(EVar_Kind_ExpBuff_Logoff, EVar_ExpAdd_MaxExperienceRate, s_iExpAdd_MaxExpRate);
		}

		__int64 i64RestExp = MakeInt64(static_cast<DWORD>(pkPlayer->GetAbil(AT_REST_EXP_ADD_MAX_LOW32)), static_cast<DWORD>(pkPlayer->GetAbil(AT_REST_EXP_ADD_MAX_HIGH32)));
		pkPlayer->SetAbil64(AT_REST_EXP_ADD_MAX, i64RestExp);
		// 휴식 경험치 추가 될 수 있는지 검사
		int const iRecentMapNo = pkPlayer->GetAbil(AT_RECENT);
		GET_DEF(PgDefMapMgr, kDefMapMgr);
		float fOfflineRate = kDefMapMgr.GetAbil(iRecentMapNo, AT_BONUS_EXP_RATE_OFFLINE) / 1000000.0f;	// 백만분율 값이므로
		int iOfflineIntervalMin = kDefMapMgr.GetAbil(iRecentMapNo, AT_BONUS_EXP_INTERVAL_OFFLINE);
		int const iRestExpLevelLimit = kDefMapMgr.GetAbil(iRecentMapNo, AT_BONUS_EXP_LEVELLIMIT);
		if ( fOfflineRate > 0 && iOfflineIntervalMin > 0 && (0 < iRestExpLevelLimit && pkPlayer->GetAbil(AT_LEVEL) <= iRestExpLevelLimit) )
		{
			BM::PgPackedTime kLastLogout;
			kLastLogout.SetTime(static_cast<DWORD>(pkPlayer->GetAbil(AT_CHARACTER_LAST_LOGOUT_TIME)));
			if (false == kLastLogout.IsNull())
			{
				BM::DBTIMESTAMP_EX kNow;
				kNow.SetLocalTime();
				BM::DBTIMESTAMP_EX kLastLogout_db(kLastLogout);
				__int64 const i64ElapsedMin = CGameTime::GetElapsedTime(kNow, kLastLogout_db, CGameTime::MINUTE);

				GET_DEF(PgClassDefMgr, kClassDef);
				int const iClass = pkPlayer->GetAbil(AT_CLASS);
				__int64 i64LvExp = kClassDef.GetExperience4Levelup(SClassKey(iClass, pkPlayer->GetAbil(AT_LEVEL)));
				__int64 i64CurExp = pkPlayer->GetAbil64(AT_EXPERIENCE);
				double const fOldAddExpRate = std::max(0.0e0, static_cast<double>((pkPlayer->GetAbil64(AT_REST_EXP_ADD_MAX)-i64CurExp) / static_cast<double>(i64LvExp)));

				float fAddExpRate = (fOfflineRate * static_cast<int>(i64ElapsedMin/iOfflineIntervalMin)) + fOldAddExpRate;
				if (fAddExpRate > 0)
				{
					__int64 i64RestExpNew = std::min<__int64>(kClassDef.GetMaxExperience(iClass), i64CurExp + std::min<double>(s_iExpAdd_MaxExpRate / 100.0e0, fAddExpRate) * i64LvExp);
					if (i64RestExpNew <= i64CurExp)
					{
						i64RestExpNew = 0;	// 만랩~
					}
					pkPlayer->SetAbil64(AT_REST_EXP_ADD_MAX, i64RestExpNew);
				}
			}
		}
		RemoveOfflinePlayer(pkPlayer->GetID());

		auto kPair = m_kContPlayer_CharKey.insert(std::make_pair(pkPlayer->GetID(), pkMemberData));
		if ( true == kPair.second )
		{
			auto kPair2 = m_kContPlayer_CharName.insert(std::make_pair(pkPlayer->Name(), pkMemberData));
			if ( true == kPair2.second )
			{
				pkPlayer->VolatileID( BM::GUID::Create() );
				pkPlayer->addrRemote(pkMemberData->addrRemote);
				pkPlayer->SetSwitchServer( pkMemberData->kSwitchServer );
				pkPlayer->ConnArea( pkMemberData->kConnArea );
				pkPlayer->SetAccTime( pkMemberData->usAge , pkMemberData->dtUserBirth, pkMemberData->byGMLevel, pkMemberData->iAccConSec, pkMemberData->iAccDisSec, BM::PgPackedTime(pkMemberData->dtLastLogout) );
				pkPlayer->MemberID(pkMemberData->kID);
				pkPlayer->UID(pkMemberData->iUID);
				pkPlayer->SetGMLevel( pkMemberData->byGMLevel );

				switch( g_kLocal.ServiceRegion() )
				{
					// 연동으로 인해 PC방 정보가 넘어오는 국가들
				case LOCAL_MGR::NC_KOREA:
				case LOCAL_MGR::NC_USA:
					{
						pkPlayer->SetPCCafe( pkMemberData->bIsPCCafe );
						pkPlayer->SetPCCafeGrade( pkMemberData->byPCCafeGrade );
					}break;
				default:
					{
						const CONT_PCCAFE* pkPCCafe = NULL;
						g_kTblDataMgr.GetContDef( pkPCCafe );
						if( pkPCCafe )
						{
							CONT_PCCAFE::const_iterator c_iter = pkPCCafe->find( pkMemberData->addrRemote.IP() );
							if( c_iter != pkPCCafe->end() )
							{
								pkPlayer->SetPCCafe( true );
								pkPlayer->SetPCCafeGrade( c_iter->second );
							}
						}
					}break;
				}

				pkMemberData->pkPlayer = pkPlayer;//컨텐츠 서버에 플레이어 올리고.
				if (pkMemberData->pkBackupPlayer == NULL)
				{
					pkMemberData->pkBackupPlayer = new PgDoc_Player;
				}

				pkMemberData->guidCharacter = pkPlayer->GetID();

				SReqMapMove_MT kRMM( MMET_Login );

				if ( true == PgPlayerUtil::IsNewbiePlayer( pkPlayer ) )
				{
					if( true == bPassTutorial )
					{
						RealmUserManagerUtil::GetPlayerClassByVillageMap( pkPlayer, kRMM.kTargetKey, kRMM.nTargetPortal );
					}
					else
					{
						RealmUserManagerUtil::GetPlayerClassByTutorialMap( pkPlayer, kRMM.kTargetKey );
					}

					if ( true == pkPlayer->UpdateWorldMap( kRMM.kTargetKey.GroundNo() ) )
					{
						RealmUserManagerUtil::ProcessAddWorldMap( pkPlayer->GetID(), kRMM.kTargetSI.nChannel, kRMM.kTargetKey.GroundNo(), kRMM.kCasterKey );
					}
				}

				pkPlayer->SetAbil(AT_CLIENT_OPTION_SAVE, pkPlayer->m_kClientOption.dwShowOption);

				//프리미엄 서비스 처리
				SetPrimiumService(pkPlayer);
				pkMemberData->pkBackupPlayer->GetPremium() = pkPlayer->GetPremium();


				// Select daily reward info sync!
				// This code maybe be laggy, be careful!
				{
					CEL::DB_QUERY kQuery(DT_MEMBER, DQT_GET_MEMBER_DAILY_REWARD, L"EXEC [dbo].[up_GetDailyReward]");
					kQuery.QueryOwner( pkPlayer->GetMemberGUID() );
					kQuery.PushStrParam( pkPlayer->GetMemberGUID() );
					g_kCoreCenter.PushQuery(kQuery, /*bIsImmidiate*/ true);
				}

				UNIT_PTR_ARRAY kUnitArray;
				kUnitArray.Add( pkPlayer );

				CONT_PET_MAPMOVE_DATA kContPetMoveData;
				BM::GUID const kSelectedPetID = pkPlayer->SelectedPetID();
				if ( BM::GUID::IsNotNull(kSelectedPetID) )
				{
					PgDoc_PetInfo *pkDocPetInfo = pkPlayer->LoadDocPetInfo( kSelectedPetID, true );
					if ( pkDocPetInfo )
					{
						PgInventory *pkInv = pkDocPetInfo->GetInven();
						if ( pkInv )
						{
							CONT_PET_MAPMOVE_DATA::mapped_type kElement;
							kElement.kInventory = *pkInv;
							kContPetMoveData.insert( std::make_pair( kSelectedPetID, kElement ) );
						}
					}
				}

				if ( true == ReqFirstMapMove( kUnitArray, kRMM, kContPetMoveData ) )
				{
					RealmUserManagerUtil::UpdateCharacterConnectionChannel(pkPlayer->GetID(), kRMM.kTargetSI.nChannel);

					BM::Stream kOptionPacket(PT_T_C_NFY_OPTION);
					kOptionPacket.Push( &(pkPlayer->m_kClientOption), sizeof(pkPlayer->m_kClientOption) );
					SendToUser( pkPlayer->GetMemberGUID(), kOptionPacket, true );

					{
						BM::Stream kBattlePassInfo(PT_M_C_NFY_BATTLE_PASS_INFO);
						g_kBattlePassMgr.Locked_WriteToPacket(kBattlePassInfo);
						SendToUser( pkPlayer->GetMemberGUID(), kBattlePassInfo, /*IsMemberGuid*/true );
					}
				}

				//기간제 캐시 아이템 삭제 요청 처리 
				//기간제 몬스터 카드 처리 요청
				ProcessInventoryTimeOut(pkPlayer);

				//캐릭터 선택 로그
				PgLogCont kLogCont(ELogMain_System_Login_SelecteChar);
				kLogCont.MemberKey(pkPlayer->GetMemberGUID());
				kLogCont.UID(pkPlayer->UID());
				kLogCont.CharacterKey(pkPlayer->GetID());
				kLogCont.ID(pkPlayer->MemberID());
				kLogCont.Name(pkPlayer->Name());
				kLogCont.Class(pkPlayer->GetAbil(AT_CLASS));
				kLogCont.Level(pkPlayer->GetAbil(AT_LEVEL));
				kLogCont.ChannelNo(pkPlayer->GetChannel());
				kLogCont.GroundNo(pkPlayer->GetAbil(AT_RECENT));
				PgLog kLog;
				kLog.Set(2, pkPlayer->GetAbil(AT_GENDER));
				kLogCont.Add(kLog);
				kLogCont.Commit();

				PgActionCouponEvent kEvent(ET_CONNECT_COUNT,pkPlayer);

				bool bJoinedUser = false;
				BM::Stream kPacket(PT_NFY_LUCKYSTAR_LOGINUSER);
				kPacket.Push(pkPlayer->GetMemberGUID());
				kPacket.Push(pkPlayer->GetID());
				kPacket.Push(bJoinedUser);
				SendToLuckyStarEvent(kPacket);

				BM::Stream kDefPlayTimePacket(PT_SYNC_DEF_PLAYERPLAYTIME);
				g_kDefPlayTime.WriteToPacket(kDefPlayTimePacket);
				SendToUser( pkPlayer->GetMemberGUID(), kDefPlayTimePacket, true );

				BM::Stream kNfyJB3HistoryItemPacket(PT_M_C_NFY_JOBSKILL3_HISTORYITEM);
				kNfyJB3HistoryItemPacket.Push(pkPlayer->ContJobSkillHistoryItem());
				SendToUser( pkPlayer->GetMemberGUID(), kNfyJB3HistoryItemPacket, true );

				{// 그룹 메일 발송
					__int64 const iClass = GET_CLASS_LIMIT(pkPlayer->GetAbil(AT_CLASS));
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_GROUP_MAIL, L"EXEC [dbo].[up_Select_NewGroupMail]");
					kQuery.InsertQueryTarget( pkPlayer->GetID() );
					kQuery.QueryOwner( pkPlayer->GetID() );
					kQuery.PushStrParam( pkPlayer->GetID() );
					kQuery.PushStrParam( pkPlayer->GetAbil(AT_LEVEL) );
					kQuery.PushStrParam( iClass );
					g_kCoreCenter.PushQuery( kQuery );
				}

				{// 퀘스트 완료 유무
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SELECT_USER_QUEST_COMPLETE, L"EXEC [dbo].[up_UserQuestComplete_Select]");
					kQuery.InsertQueryTarget( pkPlayer->GetID() );
					kQuery.QueryOwner( pkPlayer->GetID() );
					kQuery.PushStrParam( pkPlayer->GetID() );
					g_kCoreCenter.PushQuery( kQuery );
				}

				UpdateAchievementPoint(pkPlayer);
				CHECK_ACHIEVENMENT::Process(pkPlayer);

				return S_OK;
			}
			else
			{
				m_kContPlayer_CharKey.erase( kPair.first );
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!! PlayerName<") << pkPlayer->Name() << _T(">") );
			}
		}
		else
		{//플레이어 삭제됨.
			CAUTION_LOG( BM::LOG_LV5, _T("Already Logined Character ") << pkPlayer->Name() << _COMMA_ << pkPlayer->GetID() );
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgRealmUserManager::Locked_Recv_PT_T_N_REQ_MAP_MOVE( BM::Stream * const pkPacket )const
{
	SReqMapMove_MT kRMM;
	Constellation::SConstellationMission constellationMission;
	short sChannel = 0;
	UNIT_PTR_ARRAY kUnitArray;
	pkPacket->Pop(kRMM);
	constellationMission.ReadFromPacket(*pkPacket);
	pkPacket->Pop(sChannel);
	EWRITETYPE const kWriteType = kUnitArray.ReadFromPacket( *pkPacket );// 맵에서 보낸 유저정보를 싹 흡수 해버림

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	if ( pkActionOrder )
	{
		BM::CAutoMutex kLock(m_kMutex);

		UNIT_PTR_ARRAY::iterator itr_unit = kUnitArray.begin();
		while ( itr_unit != kUnitArray.end() )
		{
			if ( itr_unit->pkUnit )
			{
				if ( true == IsLoginPlayer( itr_unit->pkUnit->GetID(), false ) )
				{
					pkActionOrder->InsertTarget( itr_unit->pkUnit->GetID() );
					++itr_unit;
					continue;
				}

				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot Find Character = ") << itr_unit->pkUnit->GetID() );	
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("PgPlayer is NULL") );
			}

			itr_unit = kUnitArray.erase( itr_unit );
		}

		if ( kUnitArray.empty() )
		{
			PgJobWorker::FreeJob( pkActionOrder );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		pkActionOrder->kCause = CNE_CONTENTS_EVENT;
		pkActionOrder->kGndKey.Set( sChannel, kRMM.kCasterKey );
		ContentsActionEvent kEvent(ECEvent_PT_T_N_REQ_MAP_MOVE);
		SPMO kOrder(IMET_CONTENTS_EVENT, BM::GUID::NullData(), kEvent);
		pkActionOrder->kContOrder.push_back(kOrder);
		pkActionOrder->kAddonPacket.Push( kRMM );
		constellationMission.WriteToPacket(pkActionOrder->kAddonPacket);
		pkActionOrder->kAddonPacket.Push( sChannel );
		kUnitArray.WriteToPacket( pkActionOrder->kAddonPacket, kWriteType );
		pkActionOrder->kAddonPacket.Push(*pkPacket);
		return g_kJobDispatcher.VPush(pkActionOrder);
	}
	return false;
}

DWORD PgRealmUserManager::OnContentsActionEvent(ContentsActionEvent const &rkEvent, BM::Stream * const pkAddonPacket)
{
	DWORD dwResult = 0;
	switch (rkEvent.Type())
	{
	case ECEvent_Kick_byGM:
		{
			dwResult = OnGMKickUser(pkAddonPacket);
		}break;
	case ECEvent_PT_I_T_KICKUSER:
		{
			SRemoveWaiter kWaiter;
			kWaiter.ReadFromPacket(*pkAddonPacket);

			if ( pkAddonPacket->RemainSize() )
			{
				SReqSwitchReserveMember kSRM;
				if ( true == kSRM.ReadFromPacket( *pkAddonPacket ) )
				{
					ProcessRemoveUser(kWaiter.kMemberGuid, true, &kSRM );
					break;
				}
			}

			ProcessRemoveUser(kWaiter.kMemberGuid, kWaiter.bBroadcast, NULL );
		}break;
	case ECEvent_PT_T_N_REQ_MAP_MOVE:
		{
			dwResult = RecvMapMove(pkAddonPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled EventType[") << rkEvent.Type() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return dwResult;
}

void PgRealmUserManager::Locked_OnTick10s()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if ( ESERVER_STATUS_STOPPING == m_eServerStatus )
	{
		size_t const iRemainUserCount = m_kContPlayer_MemberKey.size();
		if ( iRemainUserCount > 0 )
		{
			INFO_LOG( BM::LOG_LV5, L"Server Terminate Wait Remain User Count : " << iRemainUserCount );
		}
		else
		{
			INFO_LOG( BM::LOG_LV6, _T("===========================================================") );
			INFO_LOG( BM::LOG_LV6, _T("[ContentsServer] will be shutdown") );
			INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING...") );
			INFO_LOG( BM::LOG_LV6, _T("===========================================================") );
			g_kConsoleCommander.StopSignal(true);
			INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
		}
	}

	static DWORD s_dwLoginEvent = 0;
	if (BM::TimeCheck(s_dwLoginEvent, 300000))	
	{ // 로그인 특정시간 유지 시, 부활 깃털이 들어있는 상자지급( 하루 한번 )

		if( 0 == g_kEventView.VariableCont().iReviveFeather_Login_Time )
		{
			//INFO_LOG( BM::LOG_LV7, _T("EVar_Revive_Feather_Login_Time's value is 0.") );
			return ;
		}

		CONT_DEF_SPECIFIC_REWARD const *pkContSpecificReward = NULL;
		g_kTblDataMgr.GetContDef( pkContSpecificReward );
		if( !pkContSpecificReward ) { return ; }

		const __int64 i64CheckTimeSec = (g_kEventView.VariableCont().iReviveFeather_Login_Time) * 60;

		CONT_CENTER_PLAYER_BY_KEY::const_iterator loop_iter = m_kContPlayer_MemberKey.begin();
		while( loop_iter != m_kContPlayer_MemberKey.end() )
		{
			SCenterPlayer_Lock kLockPlayer;
			if( GetPlayerInfo( (*loop_iter).first, true, kLockPlayer ) )
			{
				PgDoc_Player *pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
				if( pkPlayer && UT_PLAYER==pkPlayer->UnitType() )
				{
					const __int64 iConnSec = pkPlayer->GetThisConnSec_Character();

					if( i64CheckTimeSec <= iConnSec
					&&	pkPlayer->GetAbil(AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN) )
					{
						const int iLevel = pkPlayer->GetAbil(AT_LEVEL);
						int iSubType = iLevel % 10 ? iLevel / 10 : ( iLevel / 10 ) - 1;

						CONT_DEF_SPECIFIC_REWARD::const_iterator iter = pkContSpecificReward->find( POINT2(AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN, iSubType) );
						if( iter != pkContSpecificReward->end() )
						{
							CONT_DEF_SPECIFIC_REWARD::mapped_type const& kValue = (*iter).second;

							CONT_ITEM_CREATE_ORDER kOrder;
							if( OrderCreateItem(kOrder, kValue.iRewardItemNo, kValue.i64RewardCount) )
							{
								// 아이템 지급 오더
								SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
								pkActionOrder->InsertTarget(pkPlayer->GetID());
								pkActionOrder->kCause = CIE_Revive_Feather_Login;
								for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
								{
									pkActionOrder->kContOrder.push_back( SPMO(IMET_INSERT_FIXED, pkPlayer->GetID(), SPMOD_Insert_Fixed(*c_it, SItemPos(), true)) );
								}
								g_kJobDispatcher.VPush(pkActionOrder);

								// 아이템 지급 기록
								BM::DBTIMESTAMP_EX kNowTime;
								kNowTime.SetLocalTime();
								CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_SPECIFIC_REWARD, L"EXEC [dbo].[up_Update_SpecificReward]" );
								kQuery.InsertQueryTarget(pkPlayer->GetID());						
								kQuery.PushStrParam( static_cast<int>(AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN) );
								kQuery.PushStrParam( pkPlayer->GetID() );						
								kQuery.PushStrParam( kNowTime );
								g_kCoreCenter.PushQuery(kQuery);

								pkPlayer->SetAbil( AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN, 0 );								
							}
							else
							{
								INFO_LOG( BM::LOG_LV2, _T("CreateSItem failed!!") );
							}
						}
						else
						{
							//INFO_LOG( BM::LOG_LV7, _T("Can't find Type=[") << AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN << _T("], SubType=[") << iSubType << _T("] from TB_SpecificReward.") );
						}
					}
					else
					{
						if( pkPlayer->IsPCCafe() )
						{
							INFO_LOG( BM::LOG_LV2, _T("PCCafe player's guid=[") << pkPlayer->GetID() << _T("], CheckTimeSec=[") << i64CheckTimeSec << _T("s], connect sec=[") << iConnSec << _T("s]") );
						}
					}
				}
			}

			++loop_iter;
		}
	}

	auto kOfflinePlayerIt = m_kContPlayerOffline_CharKey.begin();
	std::vector<CONT_CENTER_PLAYER_BY_KEY::key_type> kOfflinePlayerNames(m_kContPlayerOffline_CharKey.size() / 2);
	while (kOfflinePlayerIt != m_kContPlayerOffline_CharKey.end())// todo: rework not thread safty!
	{
		if (BM::TimeCheck(kOfflinePlayerIt->second->dwLogoutTime, 10000)) // we set big timeout for sync!
			kOfflinePlayerNames.push_back(kOfflinePlayerIt->first);
		++kOfflinePlayerIt;
	}

	for(size_t i = 0; i < kOfflinePlayerNames.size(); ++i)
		RemoveOfflinePlayer(kOfflinePlayerNames[i]);
}

void PgRealmUserManager::Locked_OnTick_SpecificReward_Event()
{	// 로그인 후 접속 유지시, 일정 간격으로 아이템 지급.
	BM::CAutoMutex Lock(m_kMutex);

	static DWORD s_dwSpecificRewardEvent = 0;

	if( BM::TimeCheck(s_dwSpecificRewardEvent, 300000) )
	{
		BM::DBTIMESTAMP_EX NowTime;
		NowTime.SetLocalTime();

		int SpecificRewardNotifyMessageInterval = 0;
		g_kVariableContainer.Get(EVar_Kind_LoginEventSpecific, EVar_SpecificRewardNotifyMessageInterval, SpecificRewardNotifyMessageInterval);

		CONT_DEF_SPECIFIC_REWARD_EVENT const * pContSpecificRewardEvent = NULL;
		g_kTblDataMgr.GetContDef( pContSpecificRewardEvent );
		if( !pContSpecificRewardEvent )	
		{ 
			INFO_LOG(BM::LOG_LV1, __FL__ << L"CONT_DEF_SPECIFIC_REWARD_EVENT is NULL!" );
			return ; 
		}

		CONT_DEF_EVENT_REWARD_ITEM_GROUP const * pRewardItemGroup = NULL;
		g_kTblDataMgr.GetContDef(pRewardItemGroup);
		if( !pRewardItemGroup ) 
		{ 
			INFO_LOG(BM::LOG_LV1, __FL__ << L"CONT_DEF_EVENT_REWARD_ITEM_GROUP is NULL!" );
			return ; 
		}

		CONT_DEF_SPECIFIC_REWARD_EVENT::const_iterator event_iter = pContSpecificRewardEvent->begin();
		for( ; event_iter != pContSpecificRewardEvent->end() ; ++event_iter)
		{
			CONT_DEF_SPECIFIC_REWARD_EVENT::mapped_type const & EventElement = event_iter->second;

			__int64 const i64CheckTimeSec = EventElement.RewardPeriod * 60;	// 보상 간격.

			if( (EventElement.StartTime < NowTime) && (NowTime < EventElement.EndTime) )	// 이벤트 시간일 때만.
			{
				CONT_DEF_EVENT_REWARD_ITEM_GROUP::const_iterator item_iter = pRewardItemGroup->find(EventElement.RewardItemGroupNo);	// 지급할 아이템 그룹.
				if( item_iter != pRewardItemGroup->end() )
				{
					CONT_CENTER_PLAYER_BY_KEY::const_iterator loop_iter = m_kContPlayer_MemberKey.begin();
					for( ; loop_iter != m_kContPlayer_MemberKey.end() ; ++loop_iter)
					{
						SCenterPlayer_Lock LockPlayer;
						if( GetPlayerInfo( (*loop_iter).first, true, LockPlayer ) )
						{
							PgDoc_Player * pPlayer = LockPlayer.pkCenterPlayerData->pkPlayer;
							if( pPlayer && (UT_PLAYER == pPlayer->UnitType()) )
							{
								int LevelLimit = 0;
								switch( pPlayer->UnitRace() )	// 종족 별로 이벤트 레벨 제한이 다름.
								{
								case ERACE_HUMAN:
									{
										LevelLimit = EventElement.LevelLimit;
									}break;
								case ERACE_DRAGON:
									{
										LevelLimit = EventElement.DraLevelLimit;
									}break;
								default:
									{
										continue;
									}break;
								}

								if( pPlayer->GetAbil(AT_LEVEL) >= LevelLimit )	// 1. 레벨 제한 검사.
								{
									CONT_EVENTREWARD_TIMEINFO::iterator time_iter = m_ContEventRewardTimeInfo.find( pPlayer->GetID() );
									if( time_iter == m_ContEventRewardTimeInfo.end() )
									{	// 못 찾았으면 더 이상 진행하지 않음.
										continue;
									}

									int RewardCount = (*time_iter).second.RewardCount;
									__int64 const iConnSec = g_kEventView.GetLocalSecTime(CGameTime::SECOND) - (*time_iter).second.LastRewardTime;

									BM::DBTIMESTAMP_EX kLastRewardTime;
									CGameTime::SecTime2DBTimeEx((*time_iter).second.LastRewardTime, kLastRewardTime, CGameTime::SECOND);

									BM::PgPackedTime NextTime(kLastRewardTime); // 다음에 지급 받을 수 있는 시간( 마지막으로 받은 시간의 다음날 0시 )						
									CGameTime::AddTime( NextTime, CGameTime::OneDay );
									NextTime.Hour(0), NextTime.Min(0), NextTime.Sec(0);
									if( false == (BM::PgPackedTime(NowTime) < NextTime) )
									{
										RewardCount = 0;
									}

									//int RewardCount = pPlayer->GetAbil(AT_SPECIFIC_REWARD_EVENT);	// 아이템 지급 받은 수.
									//__int64 const iConnSec = pPlayer->GetLastSpecificReward();		// 이전 보상 받은 시간.

									if( RewardCount < EventElement.RewardCount )	// 2. 더 받을 수 있는지 검사.
									{
										if(i64CheckTimeSec <= iConnSec )			// 3. 접속 시간 검사.							
										{
											PgLogCont kLogCont(ELogMain_System_Event, ELogSub_Event_Keep_Access);
											kLogCont.MemberKey(pPlayer->GetMemberGUID());
											kLogCont.CharacterKey(pPlayer->GetID());
											kLogCont.UID(pPlayer->UID());
											kLogCont.ID(pPlayer->MemberID());
											kLogCont.Name(pPlayer->Name());
											kLogCont.Class(pPlayer->GetAbil(AT_CLASS));
											kLogCont.Level(pPlayer->GetAbil(AT_LEVEL));
											kLogCont.GroundNo(pPlayer->GetAbil(AT_RECENT));

											bool ItemSend = false;
											// 아이템 지급. 아이템은 지정 된 그룹에 있는 아이템을 모두 지급.
											VEC_EVENTREWARDITEMGROUP::const_iterator reward_iter = (*item_iter).second.begin();
											for( ; reward_iter != (*item_iter).second.end() ; ++reward_iter )
											{
												int const RewardItemNo = (*reward_iter).ItemNo;
												int const RewardItemCount = (*reward_iter).Count;
												PgBase_Item Item;
												if( S_OK == CreateSItem( RewardItemNo, RewardItemCount, GIOT_NONE, Item) )
												{	// 아이템 지급 오더
													SActionOrder * pActionOrder = PgJobWorker::AllocJob();
													pActionOrder->InsertTarget(pPlayer->GetID());
													pActionOrder->kCause = CIE_Login_Event;
													pActionOrder->kContOrder.push_back(SPMO(IMET_INSERT_FIXED, pPlayer->GetID(), SPMOD_Insert_Fixed( Item, SItemPos(), true)));
													g_kJobDispatcher.VPush(pActionOrder);
													ItemSend = true;

													PgLog kLog(ELOrderMain_Event, ELOrderSub_Reward);
													kLog.Set(0, EventElement.EventNo);
													kLog.Set(1, EventElement.RewardItemGroupNo);
													kLog.Set(2, RewardItemNo);
													kLog.Set(3, EventElement.RewardCount-(RewardCount+1));
													kLogCont.Add(kLog);
												}
											}

											if( ItemSend )
											{
												kLogCont.Commit();
												++RewardCount;	// 아이템 지급 카운트.

												// 아이템 지급 기록
												CEL::DB_QUERY Query( DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_SpecificReward2]" );
												Query.InsertQueryTarget(pPlayer->GetID());						
												Query.PushStrParam( static_cast<int>(AT_SPECIFIC_REWARD_EVENT) );
												Query.PushStrParam( pPlayer->GetID() );						
												Query.PushStrParam( NowTime );
												Query.PushStrParam( static_cast<short>(RewardCount) );
												g_kCoreCenter.PushQuery(Query);

												//pPlayer->SetAbil(AT_SPECIFIC_REWARD_EVENT, RewardCount);	// 보상 카운트.
												//pPlayer->SetLasSpecificRewardSec(g_kEventView.GetLocalSecTime(CGameTime::SECOND));	// 보상 시간.
												(*time_iter).second.RewardCount = RewardCount;
												(*time_iter).second.LastRewardTime = g_kEventView.GetLocalSecTime(CGameTime::SECOND);
												if( EventElement.RewardCount == RewardCount )
												{
													SendWarnMessage(pPlayer->GetID(), 1968, EL_Notice3, false);
												}
											}
										}
										else
										{
											if( 0 != SpecificRewardNotifyMessageInterval )
											{
												int calcRemainMin = static_cast<int>((i64CheckTimeSec - iConnSec) / 60i64);
												int diffMin = 5 - static_cast<int>(calcRemainMin % 5i64);
												int restSec = static_cast<int>((i64CheckTimeSec - iConnSec) % 60i64);
												if( 0 == restSec )
												{
													diffMin = 0;
												}
												int calcResult = calcRemainMin + diffMin;
												if( (calcResult % SpecificRewardNotifyMessageInterval) == 0 )
												{
													SendWarnMessage2(pPlayer->GetID(), 1967, calcResult, EL_Notice3, false);
												}
											}
										}
									}								
								}
							}
						}
					}
				}
			}
		}
	}
}

DWORD PgRealmUserManager::Locked_ContentsEvent(SActionOrder const *pkActionOrder)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	DWORD dwReturn = 0;
	CONT_PLAYER_MODIFY_ORDER::const_iterator order_itor =  pkActionOrder->kContOrder.begin();
	while(order_itor !=  pkActionOrder->kContOrder.end())
	{
		SPMO const &kOrder = (*order_itor);

		HRESULT hSubRet = E_FAIL;
		switch (kOrder.Cause())
		{
		case IMET_CONTENTS_EVENT:
			{
				ContentsActionEvent kData;
				kOrder.Read(kData);
				dwReturn = OnContentsActionEvent(kData, (BM::Stream*)&(pkActionOrder->kAddonPacket));
			}break;
		case IMET_REQ_REMOVE_GROUND:
			{
				Remove_Home_Unit(pkActionOrder->kGndKey);
			}break;
		default:
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled Cause[") << kOrder.Cause() << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			};
		}

		++order_itor;
	}
	return dwReturn;
}

DWORD PgRealmUserManager::OnGMKickUser(BM::Stream* const pkPacket)
{
	SendToRealmContents(PMET_GMProcess, *pkPacket);
//보내기 전에 Pop 해버리면 안됨;
	int iCase;
	BM::GUID kMemberGuid;
	BM::GUID kReqGuid;

	pkPacket->Pop(iCase);
	pkPacket->Pop(kMemberGuid);
	pkPacket->Pop(kReqGuid);

	ProcessRemoveUser( kMemberGuid, true, NULL );
	return 0;
}

void PgRealmUserManager::Locked_CIE_LOAD_PET_ITEM(BM::Stream &rkPacket)
{
	BM::GUID kCharGuid;
	BM::GUID kPetID;
	rkPacket.Pop( kCharGuid );
	rkPacket.Pop( kPetID );

	BM::CAutoMutex kLock(m_kMutex);

	SCenterPlayer_Lock kLockPlayer;
	if ( true == GetPlayerInfo( kCharGuid, false, kLockPlayer) )
	{
		PgInventory kPetInven;
		kPetInven.OwnerGuid( kPetID );
		kPetInven.ReadFromPacket( rkPacket, WT_DEFAULT );

		PgDoc_PetInfo *pkDocPetInfo = kLockPlayer.pkCenterPlayerData->pkPlayer->SetDocPetInfo( kPetID, kPetInven );
		if ( pkDocPetInfo )
		{
			BM::Stream kMPacket( PT_N_M_NFY_CREATE_PET, kCharGuid );
			kMPacket.Push( kPetID );
			pkDocPetInfo->WriteToPacket( kMPacket );

			SendToGround( kLockPlayer.pkCenterPlayerData->sChannelNo, kLockPlayer.pkCenterPlayerData->pkPlayer->GroundKey(), kMPacket );
		}
	}
}

void PgRealmUserManager::Locked_UpdateMissionReport(SActionOrder const *pkActionOrder)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_PLAYER_MODIFY_ORDER::const_iterator order_itor =  pkActionOrder->kContOrder.begin();
	while (order_itor !=  pkActionOrder->kContOrder.end())
	{
		SPMO const &kOrder = (*order_itor);
		MissionReport_Update kData;
		kOrder.Read(kData);

		HRESULT hSubRet = E_FAIL;
		switch (kOrder.Cause())
		{
		case IMET_END_MISSION:
		case IMET_SWAP_MISSION:
			{
				BM::Stream kResPacket(PT_N_M_RES_MISSIONUPDATE, kOrder.Cause());
				kResPacket.Push(kData.DoUpdate());
				size_t iWrPos = kResPacket.WrPos();
				size_t iOrgVecSize = kData.GuidList().size();
				kResPacket.Push(iOrgVecSize);

				int iWritePacketCount = 0;
				MissionReport_Update kData;
				kOrder.Read(kData);
				VEC_GUID::const_iterator itor_guid = kData.GuidList().begin();
				while (itor_guid != kData.GuidList().end())
				{
					SCenterPlayer_Lock kLockPlayer;
					if ( !GetPlayerInfo(*itor_guid, false, kLockPlayer) )
					{
						INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Player CharGuid[") << (*itor_guid).str().c_str() << _T("]") );
						++itor_guid;
						continue;
					}
					PgDoc_Player *pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
					if (pkPlayer == NULL)
					{
						INFO_LOG( BM::LOG_LV5, __FL__ << _T("pkPlayer is NULL Name[") << kLockPlayer.pkCenterPlayerData->kID << _T("], CharGuid[") << (*itor_guid).str().c_str() << _T("]") );
						++itor_guid;
						continue;
					}
					if (kData.DoUpdate())
					{
						pkPlayer->UpdateMission(kData.MissionKey(), kData.StageBitFlag());

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVECHARACTER_EXTERN, L"EXEC [dbo].[UP_SaveCharacterMission]" );
						kQuery.InsertQueryTarget(pkPlayer->GetID());
						pkPlayer->WriteToQuery( kQuery, EDBSAVE_MISSION );
						g_kCoreCenter.PushQuery(kQuery);
					}

					// Res Packet
					kResPacket.Push(*itor_guid);
					pkPlayer->WriteToPacket(kResPacket, WT_OP_MISSION);
					++iWritePacketCount;

					++itor_guid;
				}

				if (iWritePacketCount != iOrgVecSize)
				{
					kResPacket.ModifyData(iWrPos, &iWritePacketCount, sizeof(iWritePacketCount));
				}
				if (pkActionOrder->kAddonPacket.Size() > 0)
				{
					kResPacket.Push(pkActionOrder->kAddonPacket.Size());
					kResPacket.Push(pkActionOrder->kAddonPacket);
				}
				else
				{
					kResPacket.Push((size_t)0);
				}
				SendToGround(pkActionOrder->kGndKey.Channel(), pkActionOrder->kGndKey.GroundKey(), kResPacket);
			}break;
		default:
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled Cause[") << kOrder.Cause() << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			};
		}

		++order_itor;
	}
}

void PgRealmUserManager::UpdateRecentPos(PgDoc_Player *pkPlayer, short const nChannelNo, SReqMapMove_MT const* pkRMM)
{
	CONT_DEFMAP const *pkDefMap;
	g_kTblDataMgr.GetContDef(pkDefMap);
	if( pkDefMap )
	{
		SGroundKey kRecentGndKey = pkPlayer->GroundKey();
		if(pkRMM)
		{//맵이동
			kRecentGndKey = pkRMM->kCasterKey;
		}

		CONT_DEFMAP::const_iterator map_itr = pkDefMap->find( kRecentGndKey.GroundNo() );
		if(	map_itr != pkDefMap->end() )
		{
			if ( true == pkPlayer->UpdateRecent( kRecentGndKey, map_itr->second.iAttr) )
			{		
				if ( GATTR_FLAG_VILLAGE & (map_itr->second.iAttr) )
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_LastVillage]");
					kQuery.InsertQueryTarget(pkPlayer->GetID());
					kQuery.PushStrParam(pkPlayer->GetID());

					POINT3 ptPos = pkPlayer->GetRecentPos(GATTR_VILLAGE);
					kQuery.PushStrParam( kRecentGndKey.GroundNo() );
					kQuery.PushStrParam( ptPos.x );
					kQuery.PushStrParam( ptPos.y );
					kQuery.PushStrParam( ptPos.z );
					g_kCoreCenter.PushQuery( kQuery );
				}
				else
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_RecentMap]");
					kQuery.InsertQueryTarget(pkPlayer->GetID());
					kQuery.PushStrParam(pkPlayer->GetID());

					POINT3 ptPos = pkPlayer->GetRecentPos(GATTR_DEFAULT);
					kQuery.PushStrParam( kRecentGndKey.GroundNo() );
					kQuery.PushStrParam( ptPos.x );
					kQuery.PushStrParam( ptPos.y );
					kQuery.PushStrParam( ptPos.z );
					g_kCoreCenter.PushQuery( kQuery );
				}
			}
		}
	}
}

void PgRealmUserManager::Locked_Recv_PT_T_N_NFY_DELETE_GROUND(short const sChannel, GND_KEYLIST const &rkGndList) 
{
	BM::CAutoMutex kLock(m_kMutex);

	GND_KEYLIST::const_iterator itor_gnd = rkGndList.begin();
	while (itor_gnd != rkGndList.end())
	{
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget((*itor_gnd).Guid());
		pkActionOrder->kGndKey = SRealmGroundKey(sChannel, *itor_gnd);
		pkActionOrder->kCause = CNE_CONTENTS_EVENT;
		ContentsActionEvent kEvent(ECEvent_None);
		SPMO kOrder(IMET_REQ_REMOVE_GROUND, BM::GUID::NullData(), kEvent);
		pkActionOrder->kContOrder.push_back(kOrder);
		g_kJobDispatcher.VPush(pkActionOrder);

		++itor_gnd;
	}
}

void PgRealmUserManager::Locked_Recv_PT_A_A_SERVER_SHUTDOWN(SERVER_IDENTITY const rkSI)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(rkSI.nServerType)
	{
	case CEL::ST_CENTER:
		{
			CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.begin();
			while( m_kContPlayer_MemberKey.end() != user_itor)
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData 는 pool 데이터이므로 user_itor 삭제해도 됨.

				BM::CAutoMutex kLock(pCPData->kMutex);
				if(rkSI.nChannel == pCPData->sChannelNo)
				{
					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(pCPData->guidMember);
					if (pCPData->pkPlayer != NULL)
					{
						pkActionOrder->InsertTarget(pCPData->pkPlayer->GetID());
					}
					pkActionOrder->kCause = CNE_CONTENTS_EVENT;
					ContentsActionEvent kEvent(ECEvent_PT_I_T_KICKUSER);

					SPMO kOrder(IMET_CONTENTS_EVENT, BM::GUID::NullData(), kEvent);
					pkActionOrder->kContOrder.push_back(kOrder);

					SRemoveWaiter kWaiter( false, pCPData->guidMember );
					kWaiter.WriteToPacket(pkActionOrder->kAddonPacket);
					if (pCPData->pkPlayer != NULL)
					{
						pkActionOrder->kGndKey = SRealmGroundKey(pCPData->sChannelNo, pCPData->pkPlayer->GroundKey());
					}
					else
					{
						// pkPlayer == NULL 이라고 하더라도, Disconnect는 해야 한다.
						pkActionOrder->kGndKey = Static_Default_RealmGroundKey;
					}
					g_kJobDispatcher.VPush(pkActionOrder);
				}
				++user_itor;
			}
		}break;
	case CEL::ST_SWITCH:
		{
			CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.begin();
			while( m_kContPlayer_MemberKey.end() != user_itor)
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData 는 pool 데이터이므로 user_itor 삭제해도 됨.

				BM::CAutoMutex kLock(pCPData->kMutex);
				SERVER_IDENTITY const &kSwitchServer = pCPData->kSwitchServer;
				if(rkSI == kSwitchServer)
				{
					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(pCPData->guidMember);
					if (pCPData->pkPlayer != NULL)
					{
						pkActionOrder->InsertTarget(pCPData->pkPlayer->GetID());
					}
					pkActionOrder->kCause = CNE_CONTENTS_EVENT;
					ContentsActionEvent kEvent(ECEvent_PT_I_T_KICKUSER);

					SPMO kOrder(IMET_CONTENTS_EVENT, BM::GUID::NullData(), kEvent);
					pkActionOrder->kContOrder.push_back(kOrder);

					SRemoveWaiter kWaiter( false, pCPData->guidMember );
					kWaiter.WriteToPacket(pkActionOrder->kAddonPacket);
					if (pCPData->pkPlayer != NULL)
					{
						pkActionOrder->kGndKey = SRealmGroundKey(pCPData->sChannelNo, pCPData->pkPlayer->GroundKey());
					}
					else
					{
						// pkPlayer == NULL 이라고 하더라도, Disconnect는 해야 한다.
						pkActionOrder->kGndKey = Static_Default_RealmGroundKey;
					}
					g_kJobDispatcher.VPush(pkActionOrder);
				}
				++user_itor;
			}
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unhandled Server[") << C2L(rkSI) << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return;
		}break;
	}
}

bool PgRealmUserManager::Locked_EnterEvent(BM::GUID const & kCharGuId)
{
	BM::CAutoMutex kLock(m_kEventMutex);
	return EnterEvent(kCharGuId);
}

bool PgRealmUserManager::EnterEvent(BM::GUID const & kCharGuId)
{
	auto kRet = m_kContEventUser.insert(kCharGuId);
	return kRet.second;
}

void PgRealmUserManager::Locked_LeaveEvent(BM::GUID const & kCharGuId)
{
	BM::CAutoMutex kLock(m_kEventMutex);
	LeaveEvent(kCharGuId);
}

void PgRealmUserManager::LeaveEvent(BM::GUID const & kCharGuId)
{
	m_kContEventUser.erase(kCharGuId);
}

void PgRealmUserManager::OnEnterGround(UNIT_PTR_ARRAY const &rkUnitArray, SRealmGroundKey const &rkGroundKey) const
{
	UNIT_PTR_ARRAY::const_iterator itor_unitptr = rkUnitArray.begin();
	while (itor_unitptr != rkUnitArray.end())
	{
		PgPlayer* const pkUnit_Ptr = dynamic_cast<PgPlayer*>((*itor_unitptr).pkUnit);
		if (pkUnit_Ptr == NULL)
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Typecasting failed") );
			++itor_unitptr;
			continue;
		}

		SCenterPlayer_Lock kLockPlayer;
		if (!GetPlayerInfo(pkUnit_Ptr->GetMemberGUID(), true, kLockPlayer))
		{
			INFO_LOG( BM::LOG_LV4, __FL__ << _T("Cannot find UserInfo MemberGuid[") << pkUnit_Ptr->GetMemberGUID().str().c_str() << _T("]") );
			++itor_unitptr;
			continue;;
		}
		PgDoc_Player* pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
		if (pkPlayer == NULL)
		{
			INFO_LOG( BM::LOG_LV4, __FL__ << _T("Player is NULL MemberGuid[") << pkUnit_Ptr->GetMemberGUID().str().c_str() << _T("]") );
			++itor_unitptr;
			continue;;
		}

		/*
		if ( !pkPlayer->GroundKey().IsEmpty() )
		{
			short sOldChannel = itor_memberkey->second->sChannelNo;
			SGroundKey kOldGroundKey = pkPlayer->GroundKey();
	
			// 이전 GroundCont 에서 삭제
			SRealmGroundKey kRGroundKey(sOldChannel, kOldGroundKey);
			CONT_CENTER_PLAYER_BY_GROUNDKEY::iterator itor_groundkey = m_kContPlayer_GroundKey.find(kRGroundKey);
			if (itor_groundkey == m_kContPlayer_GroundKey.end())
			{
				INFO_LOG(BM::LOG_LV5, _T("[%s] Cannot find RGroundKey[%s]"), __FUNCTIONW__, kRGroundKey.str().c_str());
			}
			else
			{
				if ( !itor_groundkey->second->RemoveUser_MemberKey(kMemberGuid) )
				{
					INFO_LOG(BM::LOG_LV5, _T("[%s] Cannot Remove from GroundCont RGroundKey[%s], MemberGuid[%s]"), __FUNCTIONW__,
						kRGroundKey.str().c_str(), kMemberGuid.str().c_str());
				}
			}
		}
		*/

		kLockPlayer.pkCenterPlayerData->sChannelNo = rkGroundKey.Channel();
		pkPlayer->GroundKey(rkGroundKey.GroundKey());

		// 새로운 GroundCont에 insert
		/*
		{
			CONT_CENTER_PLAYER_BY_GROUNDKEY::iterator itor_groundkey = m_kContPlayer_GroundKey.find(rkGroundKey);
			PgGroundUserCont* pkTargetGround = NULL;
			if (itor_groundkey != m_kContPlayer_GroundKey.end())
			{
				pkTargetGround = itor_groundkey->second;	
				pkTargetGround->AddUser(*(itor_memberkey->second));
			}
			else
			{
				INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find GroundKey[%s]"), __FUNCTIONW__, kNewKey.str().c_str());
			}
			//pkTargetGround->Locked_AddUser(*(itor_memberkey->second));
		}
		*/

		++itor_unitptr;
	}
}

bool PgRealmUserManager::Locked_GetDoc_Player(BM::GUID const & rkGuid,bool const bMemberGuid,PgDoc_Player & rkCopyPlayer)
{
	BM::CAutoMutex kLock( m_kMutex, false );
	SCenterPlayer_Lock rkLockInfo;
	if(GetPlayerInfo(rkGuid, bMemberGuid, rkLockInfo) && rkLockInfo.pkCenterPlayerData->pkPlayer)
	{
		rkLockInfo.pkCenterPlayerData->pkPlayer->CopyTo(rkCopyPlayer,WT_DEFAULT);
		return true;
	}
	return false;
}

bool PgRealmUserManager::IsLoginPlayer( BM::GUID const &rkGuid, bool const bMemberGuid )const
{
	if ( true == bMemberGuid )
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContPlayer_MemberKey.find( rkGuid );
		return itr != m_kContPlayer_MemberKey.end();
	}

	CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContPlayer_CharKey.find(rkGuid);
	return itr != m_kContPlayer_CharKey.end();
}

bool PgRealmUserManager::GetOfflineInfo(SModifyOrderOwner const& rkOrderOwner, SCenterPlayer_Lock& rkLockInfo)
{
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itor_char = m_kContPlayerOffline_CharKey.find(rkOrderOwner.kOwnerGuid);
	if (itor_char == m_kContPlayerOffline_CharKey.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	rkLockInfo.Set(itor_char->second);
	return true;
}

bool PgRealmUserManager::GetInfo(SModifyOrderOwner const& rkOrderOwner, SCenterPlayer_Lock& rkLockInfo, bool const bMemberGuid)
{
	PgRealmUserManager const* pkThis = this;
	return pkThis->GetInfo(rkOrderOwner, rkLockInfo, bMemberGuid);
}
bool PgRealmUserManager::GetInfo(SModifyOrderOwner const& rkOrderOwner, SCenterPlayer_Lock& rkLockInfo, bool const bMemberGuid) const
{
	switch( rkOrderOwner.eOwnerType )
	{
	case OOT_Guild:
		{
			return GetGuildInfo(rkOrderOwner.kOwnerGuid, rkLockInfo);
		}break;
	case OOT_Player: // 기본은 플레이어
	default:
		{
			return GetPlayerInfo(rkOrderOwner.kOwnerGuid, bMemberGuid, rkLockInfo);
		}break;
	}
	return false;
}

bool PgRealmUserManager::GetPlayerInfo(BM::GUID const &rkGuid, bool const bMemberGuid, SCenterPlayer_Lock& rkLockInfo)
{
	// 동시에 두개이상의 PlayerInfo 요청하는 일이 없어야 한다. (Deadlock 발생)
	// ActionJob 에서는 Ground에서 Lock이 잡히므로, 두개이상의 Player 정보를 요청하더라도 오직 한개의 Thread 만이 그러한 요청을 할 수 있으므로 가능
	if (bMemberGuid)
	{
		CONT_CENTER_PLAYER_BY_KEY::iterator itor_member = m_kContPlayer_MemberKey.find(rkGuid);
		if (itor_member == m_kContPlayer_MemberKey.end() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		//if (itor_member->second->bWaitingDelete)
		//{
		//	return false;
		//}
		rkLockInfo.Set(itor_member->second);
		return true;
	}
	else
	{
		CONT_CENTER_PLAYER_BY_KEY::iterator itor_char = m_kContPlayer_CharKey.find(rkGuid);
		if (itor_char == m_kContPlayer_CharKey.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		//if (itor_char->second->bWaitingDelete)
		//{
		//	return false;
		//}

		rkLockInfo.Set(itor_char->second);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::GetPlayerInfo(BM::GUID const &rkGuid, bool const bMemberGuid, SCenterPlayer_Lock& rkLockInfo) const
{
	// 동시에 두개이상의 PlayerInfo 요청하는 일이 없어야 한다. (Deadlock 발생)
	// ActionJob 에서는 Ground에서 Lock이 잡히므로, 두개이상의 Player 정보를 요청하더라도 오직 한개의 Thread 만이 그러한 요청을 할 수 있으므로 가능
	if (bMemberGuid)
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itor_member = m_kContPlayer_MemberKey.find(rkGuid);
		if (itor_member == m_kContPlayer_MemberKey.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		//if (itor_member->second->bWaitingDelete)
		//{
		//	return false;
		//}
		rkLockInfo.Set(itor_member->second);
		return true;
	}
	else
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itor_char = m_kContPlayer_CharKey.find(rkGuid);
		if (itor_char == m_kContPlayer_CharKey.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		//if (itor_char->second->bWaitingDelete)
		//{
		//	return false;
		//}
		rkLockInfo.Set(itor_char->second);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::GetPlayerInfo(std::wstring const &rkCharName, SCenterPlayer_Lock& rkLockInfo)
{
	CONT_CENTER_PLAYER_BY_ID::iterator itor_char = m_kContPlayer_CharName.find(rkCharName);
	if (itor_char == m_kContPlayer_CharName.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	//if (itor_char->second->bWaitingDelete)
	//{
	//	return false;
	//}
	rkLockInfo.Set(itor_char->second);
	return true;
}

bool PgRealmUserManager::GetPlayerInfo(std::wstring const &rkCharName, SCenterPlayer_Lock& rkLockInfo) const
{
	CONT_CENTER_PLAYER_BY_ID::const_iterator itor_char = m_kContPlayer_CharName.find(rkCharName);
	if (itor_char == m_kContPlayer_CharName.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	//if (itor_char->second->bWaitingDelete)
	//{
	//	return false;
	//}
	rkLockInfo.Set(itor_char->second);
	return true;
}

void PgRealmUserManager::Locked_RecvPT_A_NFY_USER_DISCONNECT(BM::Stream * const pkPacket, bool const bNotifyImm)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kMemberGuid;
	BM::GUID kCharacterGuid;
	SERVER_IDENTITY kSI;
//	size_t iRdPos = pkPacket->RdPos();
	pkPacket->Pop(kMemberGuid);
	pkPacket->Pop(kCharacterGuid);
	kSI.ReadFromPacket(*pkPacket);
//	INFO_LOG(BM::LOG_LV7, _T("[%s] Member[%s], Char[%s]"), __FUNCTIONW__, kMemberGuid.str().c_str(), kCharacterGuid.str().c_str());

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kMemberGuid);
	if(kCharacterGuid.IsNotNull())
	{
		pkActionOrder->InsertTarget(kCharacterGuid);
	}
	pkActionOrder->kCause = CNE_CONTENTS_EVENT;
	ContentsActionEvent kEvent(ECEvent_PT_I_T_KICKUSER);

	SPMO kOrder(IMET_CONTENTS_EVENT, kMemberGuid, kEvent);
	pkActionOrder->kContOrder.push_back(kOrder);

	SRemoveWaiter kWaiter( true, kMemberGuid );
	kWaiter.WriteToPacket(pkActionOrder->kAddonPacket);

	g_kJobDispatcher.VPush(pkActionOrder);
	
// 	pkPacket->RdPos(iRdPos);
// 	BM::Stream kBPacket(PT_A_NFY_USER_DISCONNECT);
// 	kBPacket.Push(*pkPacket);
// 	kBPacket.Push(false);
// 	// 모든 채널로 보낸다.
// 	g_kProcessCfg.Locked_SendToChannelServerType( CEL::ST_CENTER, kBPacket, -1, true );
// 	if (bNotifyImm)
// 	{
// 		SendToImmigration(kBPacket);
// 	}
}

bool PgRealmUserManager::Locked_SetServerState(ESERVER_STATUS const eNew)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch ( m_eServerStatus )
	{
	case ESERVER_STATUS_STOPPING:
		{
			if ( ESERVER_STATUS_STOPPING != eNew )
			{
				return false;
			}

			// 스탑 명령을 여러번 보내도 처리가 되게 하자...

		}break;
	default:
		{
		}break;
	}

	m_eServerStatus = eNew;

	switch ( m_eServerStatus )
	{
	case ESERVER_STATUS_STOPPING:
		{
			CONT_SERVER_HASH kCont;
			g_kProcessCfg.Locked_GetServerInfo( CEL::ST_CENTER, kCont );

			CONT_SERVER_HASH::const_iterator server_itr = kCont.begin();
			for ( ; server_itr != kCont.end() ; ++server_itr )
			{
				SERVER_IDENTITY const &kSI = server_itr->first;
				if( g_kProcessCfg.ServerIdentity().nRealm == kSI.nRealm )
				{
					BM::Stream kPacket(PT_MCTRL_MMC_A_NFY_SERVER_COMMAND);
					kPacket.Push(MCC_Shutdown_Service);
					kSI.WriteToPacket(kPacket);

					g_kProcessCfg.Locked_SendToServer( kSI, kPacket );
				}
			}
		}break;
	default:
		{

		}break;
	}

	return true;
}

PgRealmUserManager::ESERVER_STATUS PgRealmUserManager::Locked_GetServerState() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_eServerStatus;
}

bool PgRealmUserManager::Recv_PT_M_N_NOTIFY_FIRST_LOGIN(BM::Stream & kPacket)
{
	BM::GUID kOwnerGuid;
	kPacket.Pop( kOwnerGuid );

	SCenterPlayer_Lock kLockPlayer;
	if (!GetPlayerInfo(kOwnerGuid, false, kLockPlayer))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SCenterPlayerData * pkCenterPlayerData = kLockPlayer.pkCenterPlayerData;

	if(!pkCenterPlayerData || !pkCenterPlayerData->pkPlayer)
	{
		return false;
	}

	PgDoc_Player * pkPlayer = pkCenterPlayerData->pkPlayer;

	PgActionCouponEvent kEvent( ET_NEW_CHARACTER, pkPlayer );

	CONT_CCE_REWARD_TABLE const * pkCont = NULL;
	g_kTblDataMgr.GetContDef(pkCont);
	if(!pkCont)
	{
		return false;
	}

	CONT_CCE_REWARD_TABLE::const_iterator iter = pkCont->find(kLockPlayer.pkCenterPlayerData->iEventRewardKey);
	if(iter == pkCont->end())
	{
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	CONT_CCE_REWARD const & kRewards = (*iter).second.kRewards;

	for(CONT_CCE_REWARD::const_iterator rewarditer = kRewards.begin();rewarditer != kRewards.end();++rewarditer)
	{
		PgBase_Item kItem;
		if(S_OK == CreateSItem((*rewarditer).iItemNo,(*rewarditer).siCount,(*rewarditer).iRarityControlNo,kItem))
		{
			kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkPlayer->GetID(), SPMOD_Insert_Fixed(kItem, SItemPos(), true)));
		}
	}

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(pkPlayer->GetID());
	pkActionOrder->kGndKey.Set(pkPlayer->GetChannel(), pkPlayer->GroundKey());
	pkActionOrder->kCause = CIE_CCE_Reward;
	pkActionOrder->kContOrder = kOrder;
	g_kJobDispatcher.VPush(pkActionOrder);

	{// 그룹 메일 발송
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_GROUP_MAIL, L"EXEC [dbo].[up_Select_NewGroupMail]");
		kQuery.InsertQueryTarget( pkPlayer->GetID() );
		kQuery.QueryOwner( pkPlayer->GetID() );
		kQuery.PushStrParam( pkPlayer->GetID() );
		kQuery.PushStrParam( static_cast<int>(0) );
		kQuery.PushStrParam( static_cast<__int64>(0) );
		g_kCoreCenter.PushQuery( kQuery );
	}

	return true;
}

void PgRealmUserManager::Locked_Recv_PT_M_I_REQ_CASH_MODIFY( BM::Stream * const pkPacket ) const
{
	BM::CAutoMutex kLock(m_kMutex);	// READ_LOCK
	SPMO kCashOrder;
	SPMOD_AddCash kAddCash;
	EItemModifyParentEventType eCause;
	BM::GUID kCasterGuid;
	kCashOrder.ReadFromPacket(*pkPacket);
	pkPacket->Pop(eCause);
	pkPacket->Pop(kCasterGuid);
	kCashOrder.Read(kAddCash);

	SCenterPlayer_Lock kLockPlayer;
	if ( false == GetPlayerInfo(kCasterGuid, false, kLockPlayer))
	{
		return;
	}
	if (kLockPlayer.pkCenterPlayerData == NULL || kLockPlayer.pkCenterPlayerData->pkPlayer == NULL)
	{
		return;
	}

	PgDoc_Player* pkPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;

	PgTranPointer kTran(ECASH_TRAN_ADD_CASH, pkPlayer->GetMemberGUID(), kCasterGuid, pkPlayer->GroundKey(), pkPlayer->GetSwitchServer(), kAddCash.Cause());
	PgAddCashTran* pkTran = dynamic_cast<PgAddCashTran*>(kTran.GetTran());
	if (pkTran != NULL)
	{
		pkTran->UID(pkPlayer->UID());
		pkTran->AccountID(pkPlayer->MemberID());
		pkTran->CharacterName(pkPlayer->Name());
		pkTran->RemoteAddr(pkPlayer->addrRemote());
		pkTran->Cash(kAddCash.AddCash());
		pkTran->AddedPacket().Push(*pkPacket);	// AddedPacket 에 ORDER 를 넣어 둔다.
	}
	kTran.RequestPayCash();

}

void PgRealmUserManager::Locked_Add_Home_Unit(BM::GUID const & kGuid,SMYHOME const kHome, CONT_HOME_ITEM const & kContItem, BM::Stream const & kAddonPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = new SCenterPlayerData;

	if(NULL == pData)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, _T("[ERROR] [") << __FL__ << _T("] SCentekSPDData Memory Faied") );
		return;
	}

	SPlayerDBData kData;
	kData.guidCharacter = kGuid;

	kData.iClass= MYHOME_CLASS_INSIDE_NO;
	_snwprintf_s(kData.szName,MAX_CHARACTER_NAME_LEN,L"%d-%d:IN",kHome.siStreetNo,kHome.iHouseNo);

	TCHAR chTempID[100];
	_snwprintf_s(chTempID,20,L"M%d-%d:IN",kHome.siStreetNo,kHome.iHouseNo);
	pData->kID = chTempID;

	auto const ret = m_kContPlayer_MemberKey.insert( std::make_pair( kGuid, pData) );
	if(true == ret.second )//넣기 성공
	{
		const auto ret2 = m_kContPlayer_MemberID.insert( std::make_pair( pData->kID, pData) );
		if( true == ret2.second )//넣기 성공
		{
			auto ret3 = m_kContPlayer_CharKey.insert(std::make_pair(kGuid, pData));
			if ( true == ret3.second )
			{
				auto ret4 = m_kContPlayer_CharName.insert(std::make_pair(std::wstring(kData.szName), pData));
				if ( false == ret4.second )
				{
					delete pData;
					m_kContPlayer_MemberKey.erase( ret.first );
					m_kContPlayer_MemberID.erase( ret2.first );
					m_kContPlayer_CharKey.erase( ret3.first );
					return;
				}
			}
			else
			{
				delete pData;
				m_kContPlayer_MemberKey.erase( ret.first );
				m_kContPlayer_MemberID.erase( ret2.first );
				return;
			}
		}
		else
		{
			delete pData;
			m_kContPlayer_MemberKey.erase( ret.first );
			return;
		}
	}
	else
	{
		delete pData;
		return;
	}

	++m_iMyHomeUnitCount;

	pData->guidMember = kGuid;
	pData->guidCharacter = kGuid;
	pData->pkPlayer = new PgDoc_MyHome;
	pData->pkBackupPlayer = new PgDoc_MyHome;
	pData->pkPlayer->VolatileID( BM::GUID::Create() );
	pData->pkPlayer->MemberID(pData->kID);

	kData.wLv	= 1;
	kData.iRace = 1;
	kData.iHairColor	=	kHome.iHomeColor;
	kData.iHairStyle	=	kHome.iHomeStyle;
	kData.iFace			=	kHome.iHomeFence;
	kData.iJacket		=	kHome.iHomeGarden;
	kData.iPants		=	kHome.iRoomWall;
	kData.iShoes		=	kHome.iRoomFloor;
	kData.iGloves		=	0;
	kData.iHP			=	100;

	GET_DEF(PgClassDefMgr, kClassDefMgr);
	SPlayerBasicInfo kBasicInfo;
	kClassDefMgr.GetAbil(SClassKey(kData.iClass, kData.wLv), kBasicInfo);
	pData->pkPlayer->Create( kGuid, kData, kBasicInfo, SPlayerBinaryData());
	pData->pkPlayer->SetID(kGuid);
	PgDoc_MyHome * pkMyHome = dynamic_cast<PgDoc_MyHome*>(pData->pkPlayer);
	pkMyHome->OwnerGuid(kHome.kOwnerGuid);
	pkMyHome->HomeAddr(SHOMEADDR(kHome.siStreetNo,kHome.iHouseNo));
	pkMyHome->OwnerName(kHome.kName);

	pkMyHome->SetAbil(AT_MYHOME_VISITFLAG,kHome.bEnableVisitBit);
	pkMyHome->SetAbil(AT_MYHOME_STATE,kHome.bAuctionState);
	pkMyHome->SetAbil(AT_MYHOME_VISITLOGCOUNT,kHome.iTotalVisitLogCount);

	pkMyHome->SetAbil(AT_MYHOME_TODAYHIT,kHome.iTodayHitCount);
	pkMyHome->SetAbil(AT_MYHOME_TOTALHIT,kHome.iTotalHitCount);
	
	int iSideJob = 0;
	for(CONT_MYHOME_SIDE_JOB::const_iterator job_iter = kHome.kContSideJob.begin();job_iter != kHome.kContSideJob.end();++job_iter)
	{
		iSideJob = iSideJob | (*job_iter).first;
	}

	pkMyHome->SetAbil(AT_HOME_SIDEJOB, iSideJob);

	PgInventory * pkInv = pData->pkPlayer->GetInven();
	CONT_HOME_ITEM::const_iterator iter = kContItem.find(kGuid);
	if(iter != kContItem.end())
	{
		BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE] = {0,};
		BYTE abyExternIdx[MAX_DB_INVEXTEND_SIZE] = {0,};

		pkInv->Init( abyInvExtern, abyExternIdx );//인벤 셋팅.
		pkInv->InsertItemList( iter->second );
	}

	pkInv->OwnerName(pData->pkPlayer->Name());
	pkMyHome->IsInSide(true);

	SGroundMakeOrder kOrder;
	kOrder.kKey.GroundNo( kHome.iGroundNo );
	kOrder.kKey.Guid( kGuid );

	BM::Stream kPacket( PT_N_T_REQ_CREATE_PUBLICMAP, PMET_MYHOME );
	kOrder.WriteToPacket(kPacket);
	kPacket.Push(kAddonPacket);
	::SendToCenter( CProcessConfig::GetPublicChannel(), kPacket );
}

void PgRealmUserManager::Remove_Home_Unit(SRealmGroundKey const & kGndKey)
{
	SContentsUser rkOut;
	if(S_OK == GetPlayerInfo(kGndKey.GroundKey().Guid(), false, rkOut))
	{
		BM::Stream kPacket(PT_I_M_REQ_HOME_DELETE);
		kPacket.Push(kGndKey.GroundKey().Guid());
		::SendToCenter( kGndKey.Channel(), kPacket);
	}
}

void PgRealmUserManager::Locked_Release_Home_Unit(BM::GUID const & kHomeGuid)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::Stream kPacket(PT_I_M_MYHOME_MOVE_TO_HOMETOWN);
	kPacket.Push(kHomeGuid);
	SendToUserGround(kHomeGuid,kPacket,false,true);

	SContentsUser rkOut;
	if(S_OK == GetPlayerInfo(kHomeGuid, false, rkOut))
	{
		BM::Stream kPacket(PT_I_M_REQ_HOME_DELETE);
		kPacket.Push(kHomeGuid);
		::SendToCenter( rkOut.sChannel, kPacket);
	}

	CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.find(kHomeGuid);
	if(iter != m_kContPlayer_CharKey.end())
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = iter->second;
		RemovePlayer( pCPData );
		_ProcessRemoveCharacter( pCPData );
		SAFE_DELETE(pCPData);
	}
	if( 0 < m_iMyHomeUnitCount )
	{
		--m_iMyHomeUnitCount;
	}
}

bool PgRealmUserManager::Q_DQT_MYHOME_REMOVE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		BM::GUID kHomeGuid;
		rkResult.contUserData.Pop(kHomeGuid);
		Locked_Release_Home_Unit(kHomeGuid); // 아파트 반납시
		//친구 갱신 요청
		{
			BM::Stream kRefresh(PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_FRIEND);
			kRefresh.Push(rkResult.QueryOwner());
			::SendToFriendMgr(kRefresh);
		}
		{//길드 갱신 요청
			BM::Stream kRefresh(PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_GUILD);
			kRefresh.Push(rkResult.QueryOwner());
			::SendToGuildMgr(kRefresh);
		}
		{//커플 갱신 요청
			BM::Stream kRefresh(PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_COUPLE);
			kRefresh.Push(rkResult.QueryOwner());
			::SendToCoupleMgr(kRefresh);
		}
		{//파티 갱신 요청
			SContentsUser rkUser;
			if(S_OK ==  GetPlayerByGuid(rkResult.QueryOwner(), false, rkUser) )
			{
				BM::Stream kRefresh(PT_N_T_NFY_COMMUNITY_STATE_HOMEADDR_PARTY);
				kRefresh.Push(rkUser.kCharGuid);
				::SendToGlobalPartyMgr(rkUser.sChannel, kRefresh);
			}
		}
		return true;
	}
	return false;
}

bool PgRealmUserManager::Q_DQT_LOAD_MYHOME_INFO(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)	
	{
		return false;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator countiter = rkResult.vecResultCount.begin();

	if((iter == rkResult.vecArray.end()) || (countiter == rkResult.vecResultCount.end()))
	{
		return true;
	}

	short sStreetNo = 0;
	int iHouseNo = 0;

	rkResult.contUserData.Pop(sStreetNo);
	rkResult.contUserData.Pop(iHouseNo);

	BM::Stream kAddonPacket;
	kAddonPacket.Push(rkResult.contUserData);

	CONT_MYHOME kCont;
	CONT_HOME_ITEM kItemList;

	int const iHomeCount = (*countiter); ++countiter;

	//INFO_LOG( BM::LOG_LV6, __FL__<<"sStreetNo : "<<sStreetNo<<"  iHouseNo : "<<iHouseNo<<" iHomeCount : "<<iHomeCount);

	DWORD const dwMSecPerSec = 1000;
	SYSTEMTIME kNowDateTime;
	::GetLocalTime(&kNowDateTime);
	__int64 iNowTime = 0;
	CGameTime::SystemTime2SecTime(kNowDateTime, iNowTime);

	iNowTime = iNowTime-60i64*60i64*24i64*30i64;//현재 날짜로 부터 30일

	BM::DBTIMESTAMP_EX ex;
	CGameTime::SecTime2DBTimeEx(iNowTime, ex);

	bool bNeedSkip = (g_kLocal.IsServiceRegion(LOCAL_MGR::NC_RUSSIA) && 7==sStreetNo);

	for(int i = 0;i < iHomeCount;++i)
	{
		SMYHOME kValue;
		kValue.MyHomeReadFromDB(iter);

		if(true == bNeedSkip && BM::DBTIMESTAMP_EX(kValue.kPayTexNotiTime) < ex )
		{
			continue;
		}

		kCont.insert(std::make_pair(CONT_MYHOME::key_type(kValue.siStreetNo,kValue.iHouseNo),kValue));

		if(kValue.kBidderGuid.IsNotNull())
		{
			m_kMyHomeMgr.AddBidder(kValue.kBidderGuid);
		}
	}

	int const iSideJobCount = (*countiter); ++countiter;

	std::map<BM::GUID,CONT_MYHOME_SIDE_JOB> kContSideJob;

	for(int i = 0;i < iSideJobCount;++i)
	{
		BM::GUID kKey;
		eMyHomeSideJob kSideJob;
		CONT_MYHOME_SIDE_JOB::mapped_type kValue;

		(*iter).Pop(kKey);						++iter;
		(*iter).Pop(kSideJob);					++iter;
		BM::DBTIMESTAMP_EX kEndTime;
		(*iter).Pop(kEndTime);					++iter; kValue.kEndTime = static_cast<BM::PgPackedTime>(kEndTime);
		(*iter).Pop(kValue.i64TotalSellCost);	++iter;
		(*iter).Pop(kValue.kJobRateType);		++iter;
		(*iter).Pop(kValue.iJobRate);			++iter;

		kContSideJob[kKey].insert(std::make_pair(kSideJob,kValue));
	}

	int const iItemCount = (*countiter); ++countiter;
	
	for(int i = 0;i < iItemCount;++i)
	{
		SDBItemData kDBItem;
		
		(*iter).Pop(kDBItem.ItemGuid);		++iter;
		(*iter).Pop(kDBItem.OwnerGuid);		++iter;
		(*iter).Pop(kDBItem.InvType);		++iter;
		(*iter).Pop(kDBItem.InvPos);		++iter;
		(*iter).Pop(kDBItem.ItemNo);		++iter;

		(*iter).Pop(kDBItem.Count);			++iter;
		(*iter).Pop(kDBItem.Enchant_01);	++iter;
		(*iter).Pop(kDBItem.Enchant_02);	++iter;
		(*iter).Pop(kDBItem.Enchant_03);	++iter;
		(*iter).Pop(kDBItem.Enchant_04);	++iter;
		(*iter).Pop(kDBItem.State);			++iter;
		(*iter).Pop(kDBItem.CreateDate);	++iter;

		double dValue = 0;
		POINT3 kPos;
		short siDir = 0;
		BM::GUID kParentGuid;
		BYTE bLinkIdx = 0;

		(*iter).Pop(dValue);		++iter;	kPos.x = static_cast<float>(dValue);
		(*iter).Pop(dValue);		++iter;	kPos.y = static_cast<float>(dValue);
		(*iter).Pop(dValue);		++iter;	kPos.z = static_cast<float>(dValue);
		(*iter).Pop(siDir);			++iter;
		(*iter).Pop(kParentGuid);	++iter;
		(*iter).Pop(bLinkIdx);		++iter;

		SHomeItem kHomeItem;
		kHomeItem.Pos(kPos);
		kHomeItem.Dir(siDir);
		kHomeItem.ParentGuid(kParentGuid);
		kHomeItem.LinkIdx(bLinkIdx);

		PgBase_Item kItem;
		SItemPos kItemPos;
		CastSDBItemDataToSItem(kDBItem, kItem, kItemPos);

		kItem.Set(kHomeItem);

		kItemList[kDBItem.OwnerGuid].insert(std::make_pair(kItem.Guid(),PgItemWrapper(kItem,kItemPos)));
	}

	//INFO_LOG( BM::LOG_LV6, __FL__<<"iSideJobCount : "<<iSideJobCount<<"  iItemCount : "<<iItemCount);

	GET_DEF(CItemDefMgr, kItemDefMgr);
	for(CONT_MYHOME::iterator home_iter = kCont.begin();home_iter != kCont.end();++home_iter)
	{
		SMYHOME & kMyHome = (*home_iter).second;

		CONT_HOME_ITEM::const_iterator item_iter = kItemList.find(kMyHome.kHomeInSideGuid);
		if(item_iter != kItemList.end())
		{
			std::set<int> kHomeEquip;
			for(CONT_HOME_ITEM::mapped_type::const_iterator equip_iter = (*item_iter).second.begin();equip_iter != (*item_iter).second.end();++equip_iter)
			{
				if(false == (*equip_iter).second.IsUseTimeOut())
				{
					kHomeEquip.insert((*equip_iter).second.ItemNo());
				}
			}
			kMyHome.iEquipItemCount = kHomeEquip.size(); // 내부 배치 아이템 수량을 업데이트 한다.
		}

		std::map<BM::GUID,CONT_MYHOME_SIDE_JOB>::const_iterator sidejob_iter = kContSideJob.find(kMyHome.kHomeInSideGuid);
		if(sidejob_iter != kContSideJob.end()) // 아르바이트 정보 업데이트
		{
			kMyHome.kContSideJob = (*sidejob_iter).second;
		}

		Locked_Add_Home_Unit(kMyHome.kHomeInSideGuid, kMyHome, kItemList, kAddonPacket);
	}

	m_kMyHomeMgr.AddMyHome(sStreetNo, kCont);
	return true;
}

void PgRealmUserManager::Reset_MyHome_Unit(PgDoc_MyHome * pkMyHome)
{
	BM::Stream kPacket(PT_I_M_REQ_HOME_DELETE);
	kPacket.Push(pkMyHome->GetMemberGUID());
	SendToCenter( pkMyHome->GetChannel(), kPacket);

	SReqMapMove_MT kRMM( MMET_GoToPublicGround );
	kRMM.kTargetKey = pkMyHome->GroundKey();

	if(false == pkMyHome->IsInSide())
	{
		kRMM.kTargetSI.nChannel = 0;
		kRMM.nTargetPortal = 0;
		kRMM.pt3TargetPos = pkMyHome->GetPos();
	}
	else
	{
		kRMM.kTargetSI.nChannel = 0;
		kRMM.nTargetPortal = 1;
	}

	UNIT_PTR_ARRAY kUnitArray;
	kUnitArray.Add( pkMyHome, false );
	OnEnterGround( kUnitArray, SRealmGroundKey(pkMyHome->GetChannel(),pkMyHome->GroundKey()) );
	BM::Stream kSelectPacket( PT_I_M_REQ_HOME_CREATE, kRMM );
	pkMyHome->WriteToPacket( kSelectPacket, WT_MAPMOVE_FIRST );
	SendToCenter( pkMyHome->GetChannel(), kSelectPacket );
}

void PgRealmUserManager::RecvMyHomePacketHandler(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	m_kMyHomeMgr.RecvPacketHandler(kPacketType,kSI,kGndKey,pkPacket);
}

void PgRealmUserManager::RecvTaskProcessMyHome(BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE kType = 0;
	pkPacket->Pop( kType );

	switch ( kType )
	{
	case PT_C_M_REQ_CHAT:
	case PT_C_M_REQ_MYHOME_CHAT_ENTER:
	case PT_C_M_REQ_MYHOME_CHAT_EXIT:
	case PT_C_M_REQ_MYHOME_CHAT_MODIFY_ROOM:
	case PT_C_M_REQ_MYHOME_CHAT_MODIFY_GUEST:
	case PT_C_M_REQ_MYHOME_CHAT_ROOM_LIST:
	case PT_C_M_REQ_MYHOME_CHAT_ROOM_CREATE:
	case PT_C_M_REQ_MYHOME_CHAT_SET_ROOMMASTER:
	case PT_C_M_REQ_MYHOME_CHAT_KICK_GEUST:
		{
			m_kMyHomeMgr.RecvHomeChatHandler(kType, pkPacket);
		}break;
	case PT_T_N_ANS_CREATE_PUBLICMAP:
		{
			SGroundMakeOrder kOrder;
			HRESULT hRet = E_FAIL;
			pkPacket->Pop( hRet );
			kOrder.ReadFromPacket(*pkPacket);

			switch(hRet)
			{
			case S_OK:
				{
					BM::CAutoMutex kLock( m_kMutex );

					SCenterPlayer_Lock kLockPlayer;
					if (!GetPlayerInfo(kOrder.kKey.Guid(), false, kLockPlayer))
					{
						return;
					}

					SCenterPlayerData * pkCenterPlayerData = kLockPlayer.pkCenterPlayerData;

					if(!pkCenterPlayerData || !pkCenterPlayerData->pkPlayer)
					{
						return;
					}

					PgDoc_MyHome * pkHome = dynamic_cast<PgDoc_MyHome *>(pkCenterPlayerData->pkPlayer);
					if(!pkHome)
					{
						return;
					}

					UNIT_PTR_ARRAY kUnitArray;
					kUnitArray.Add( pkHome, false );

					OnEnterGround( kUnitArray, SRealmGroundKey(g_kProcessCfg.GetPublicChannel(), kOrder.kKey) );

					SReqMapMove_MT kRMMIN( MMET_GoToPublicGround );
					kRMMIN.kTargetSI.nChannel = 0;
					kRMMIN.nTargetPortal = 1;
					kRMMIN.kTargetKey = kOrder.kKey;

					BM::Stream kSelectPacket( PT_I_M_REQ_HOME_CREATE, kRMMIN );
					pkHome->WriteToPacket( kSelectPacket, WT_MAPMOVE_FIRST );
					SendToCenter( pkHome->GetChannel(), kSelectPacket );
				}break;
			default:
				{
				}break;
			}

			bool bIsHasRequester = false;
			pkPacket->Pop(bIsHasRequester);

			if(true == bIsHasRequester)
			{
				BM::GUID kGuid;
				pkPacket->Pop(kGuid);
				BM::Stream kAddonPacket;
				pkPacket->Pop(kAddonPacket.Data());
				kAddonPacket.PosAdjust();
				Locked_SendToUserGround(kGuid,kAddonPacket,false,true);
			}
		}break;
	case PT_N_T_NFY_COMMUNITY_STATE_HOMEADDR_PARTY:
		{
			BM::GUID kPreOwner;
			SHOMEADDR addr;
			pkPacket->Pop(kPreOwner);
			pkPacket->Pop(addr);

			SContentsUser rkUser;
			if(S_OK==Locked_GetPlayerInfo(kPreOwner, false, rkUser))
			{
				BM::Stream kRefresh(PT_N_T_NFY_COMMUNITY_STATE_HOMEADDR_PARTY);
				kRefresh.Push(rkUser.kCharGuid);
				kRefresh.Push(addr);
				::SendToGlobalPartyMgr(rkUser.sChannel, kRefresh);
			}
		}break;
	}
}

bool PgRealmUserManager::SetPlayerPlayTime( SCenterPlayerData * pkData, int const iAccConSec, int const iAccDicSec )
{
	if( NULL==pkData )
	{
		return false;
	}
	if( NULL==pkData->pkPlayer )
	{
		pkData->iAccConSec = iAccConSec;
		pkData->iAccDisSec = iAccDicSec;
		return true;
	}

	__int64 i64SelectSec = g_kEventView.GetLocalSecTime(CGameTime::SECOND);
	pkData->i64TotalConnSec += pkData->pkPlayer->GetThisConnSec_Character();
	pkData->iAccConSec = iAccConSec;
	pkData->iAccDisSec = iAccDicSec;
	pkData->pkPlayer->SetPlayTime(iAccConSec,iAccDicSec);
	pkData->pkPlayer->SetSelectCharacterSec(i64SelectSec);
	pkData->pkPlayer->SetLasSpecificRewardSec(i64SelectSec);

	BM::Stream kPacket(PT_M_M_UPDATE_PLAYERPLAYTIME);
	kPacket.Push(pkData->pkPlayer->GroundKey());
	kPacket.Push(pkData->pkPlayer->GetID());
	kPacket.Push(iAccConSec);
	kPacket.Push(iAccDicSec);
	kPacket.Push(i64SelectSec);
	SendToServerType(CEL::ST_CENTER, kPacket);
	return true;
}

void PgRealmUserManager::ResetPlayerPlayTime()
{
	for(CONT_CENTER_PLAYER_BY_KEY::iterator iter = m_kContPlayer_MemberKey.begin();
		iter!=m_kContPlayer_MemberKey.end();++iter)
	{
		SCenterPlayer_Lock kLockPlayer;
		if( GetPlayerInfo((*iter).first, true, kLockPlayer) )
		{
			SetPlayerPlayTime(kLockPlayer.pkCenterPlayerData, 0, 0);
		}
	}
}

void PgRealmUserManager::Locked_RecvTaskProcessPlayerPlayTime(WORD const usType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(NULL==pkPacket)
	{
		return;
	}

	switch ( usType )
	{
	case PT_M_I_GMCMD_PLAYERPLAYTIMESTEP:
		{
			BM::Stream kPacket(PT_M_I_GMCMD_PLAYERPLAYTIMESTEP);
			kPacket.Push(*pkPacket);
			::SendToImmigration( kPacket );
		}break;
	case PT_I_M_GMCMD_RESETPLAYERPLAYTIME:
		{
			BM::PgPackedTime kTime;
			pkPacket->Pop( kTime );
			g_kDefPlayTime.LastApplyTime(kTime);

			ResetPlayerPlayTime();
		}break;
	case PT_M_I_GMCMD_SETPLAYERPLAYTIME:
		{
			std::wstring kSearchID;
			int iAccConSec = 0;
			int iAccDicSec = 0;
			bool bIsMember;
			pkPacket->Pop( kSearchID );
			pkPacket->Pop( iAccConSec );
			pkPacket->Pop( iAccDicSec );
			pkPacket->Pop( bIsMember );
			
			std::transform(kSearchID.begin(), kSearchID.end(), kSearchID.begin(), towupper);	//대문자변환

			if(!bIsMember)
			{
				BM::CAutoMutex kLock(m_kMutex);
				SCenterPlayer_Lock kLockPlayer;
				if( GetPlayerInfo(kSearchID, kLockPlayer) )
				{
					SetPlayerPlayTime(kLockPlayer.pkCenterPlayerData, iAccConSec, iAccDicSec);
				}
				else
				{ //접속하지 않은 캐릭터인 경우 DB에서 초기화
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_PROCESS_SETPLAYERPLAYTIME, L"EXEC [dbo].[up_ProcessSetPlayerPlayTime]");
					kQuery.PushStrParam( kSearchID );
					kQuery.contUserData.Push( iAccConSec );
					kQuery.contUserData.Push( iAccDicSec );
					g_kCoreCenter.PushQuery(kQuery);
				}
			}
			else
			{
				BM::Stream kPacket(PT_M_I_GMCMD_SETPLAYERPLAYTIME);
				kPacket.Push( kSearchID );
				kPacket.Push( iAccConSec );
				kPacket.Push( iAccDicSec );
				::SendToImmigration( kPacket );

				INFO_LOG( BM::LOG_LV6, __FL__ << _T("PT_M_I_GMCMD_SETPLAYERPLAYTIME Member Find: ") << kSearchID << _T("(") << iAccConSec << _T(",") << iAccDicSec << _T(")") );
			}
		}break;
	case PT_I_M_GMCMD_SETPLAYERPLAYTIME:
		{
			std::wstring kID;
			int iAccConSec = 0;
			int iAccDicSec = 0;
			pkPacket->Pop( kID );
			pkPacket->Pop( iAccConSec );
			pkPacket->Pop( iAccDicSec );
			
			std::transform(kID.begin(), kID.end(), kID.begin(), towupper);	//대문자변환

			BM::CAutoMutex kLock(m_kMutex);
			CONT_CENTER_PLAYER_BY_ID::iterator iter = m_kContPlayer_MemberID.find( kID );
			if(m_kContPlayer_MemberID.end() != iter)
			{
				SetPlayerPlayTime((*iter).second, iAccConSec, iAccDicSec);
			}

			std::wstring kTemp;
			if(m_kContPlayer_MemberID.end() != iter)
			{
				kTemp = L" Find";
			}
			else
			{
				kTemp = L" Not Find";
			}
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("PT_I_M_GMCMD_SETPLAYERPLAYTIME SerchID: ") << kID << _T("(") << iAccConSec << _T(",") << iAccDicSec << _T(")") << kTemp );
		}break;
	}
}
 
//이 함수의 용도는 접속중인 캐릭터들에게 럭키스타 접속요청 패킷을 전송하기 위한 용도
void PgRealmUserManager::Locked_SendToLoginPlayer_LuckyStarOpen()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.begin();
	while(iter != m_kContPlayer_CharKey.end())
	{
		PgDoc_Player * const pkPlayer =  (*iter).second->pkPlayer;
		if(pkPlayer)
		{		
			bool bJoinedUser = true;
			BM::Stream kPacket(PT_NFY_LUCKYSTAR_LOGINUSER);
			kPacket.Push(pkPlayer->GetMemberGUID());
			kPacket.Push(pkPlayer->GetID());
			kPacket.Push(bJoinedUser);
			SendToLuckyStarEvent(kPacket);
		}

		++iter;
	}
}

void PgRealmUserManager::Locked_Recv_PT_N_M_REQ_COMPLETEACHIEVEMENT(BM::Stream * pkPacket)const
{
	VEC_GUID kContList;
	int iAchieveSaveIdx = 0;
	pkPacket->Pop(kContList);
	pkPacket->Pop(iAchieveSaveIdx);

	if( kContList.empty() ){ return; }

	CONT_DEF_ACHIEVEMENTS_SAVEIDX const * pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);
	if( !pkContDef ){ return; }

	CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator c_iter = pkContDef->find(iAchieveSaveIdx);
	if( c_iter==pkContDef->end() ){ return; }

	TBL_DEF_ACHIEVEMENTS const & kDefAchieve = (*c_iter).second;

	BM::CAutoMutex kLock(m_kMutex);
	CONT_PLAYER_MODIFY_ORDER kContOrder;
	for(VEC_GUID::const_iterator c_iter=kContList.begin();c_iter!=kContList.end();++c_iter)
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.find(*c_iter);
		if( iter==m_kContPlayer_CharKey.end() ){ continue; }

		BM::CAutoMutex kLock((*iter).second->kMutex);
		PgDoc_Player * const pkPlayer =  (*iter).second->pkPlayer;
		if( !pkPlayer ){ continue; }

		PgAchievements const * pkAchievements = pkPlayer->GetAchievements();

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		if(pkActionOrder)
		{
			pkActionOrder->InsertTarget(pkPlayer->GetID());
			pkActionOrder->kCause = CAE_Achievement;
			if( pkAchievements->IsComplete(kDefAchieve.iSaveIdx) )
			{
				//timelimit setting
				pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ACHIEVEMENT_TIMELIMIT,pkPlayer->GetID(),SOD_ModifyAchievementTimeLimit(kDefAchieve.iSaveIdx, kDefAchieve.iUseTime)));
			}
			else
			{
				//Achievement complete
				pkActionOrder->kContOrder.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT,pkPlayer->GetID(),SPMOD_Complete_Achievement(kDefAchieve.iSaveIdx, kDefAchieve.iCategory, kDefAchieve.iRankPoint, kDefAchieve.iUseTime, kDefAchieve.iGroupNo)));
			}
			g_kJobDispatcher.VPush(pkActionOrder);
		}
	}
}

void PgRealmUserManager::Locked_RecvPT_C_M_REQ_CLIENT_CUSTOMDATA(BM::Stream * pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kCharacterGuid;
	EClientCustomDataType eType = ECCDT_NONE;
	int iValue = 0;
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(eType);
	pkPacket->Pop(iValue);

	switch(eType)
	{
	case ECCDT_SKILLTABNO:
		{
		}break;
	}
}

bool PgRealmUserManager::GetGuildInfo(BM::GUID const &rkGuid, SCenterPlayer_Lock& rkLockInfo) const
{
	CONT_CENTER_PLAYER_BY_KEY::const_iterator guild_iter = m_kContPlayer_Guild.find(rkGuid);
	if( m_kContPlayer_Guild.end() == guild_iter )
	{
		return false;
	}
	rkLockInfo.Set(guild_iter->second);
	return true;
}

bool PgRealmUserManager::Locked_RegistGuildInfo(const BM::GUID &rkGuildGuid, const std::wstring rkGuildName, const BYTE* pkInvExtern, const BYTE* pkExternIdx, CONT_SELECTED_CHARACTER_ITEM_LIST *pkItemList, const __int64 iMoney )
{
 	BM::CAutoMutex kLock(m_kMutex, true);
	SCenterPlayerData* pData = new SCenterPlayerData();
	if( !pData )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, _T("[ERROR] [") << __FL__ << _T("] SCentekSPDData Memory Faied") );
		return false;
	}

	auto const kRet = m_kContPlayer_Guild.insert( std::make_pair( rkGuildGuid, pData) );
	if( !kRet.second )
	{
		SAFE_DELETE(pData);
		return true; // 실패처리는 하지 않는다.
	}

	{
		pData->kID = rkGuildName;
		pData->guidMember = rkGuildGuid;
		pData->guidCharacter = rkGuildGuid;

		pData->pkBackupPlayer = new PgDoc_Player;
		pData->pkPlayer = new PgDoc_Player;
		pData->pkPlayer->VolatileID( BM::GUID::Create() );
		pData->pkPlayer->MemberID(pData->kID);
		pData->pkPlayer->SetID(rkGuildGuid);
	}

	{
		SPlayerDBData kData;
		_snwprintf_s(kData.szName, MAX_CHARACTERNAME_LEN, L"%s", rkGuildName.c_str());
		kData.guidCharacter = rkGuildGuid;
		kData.iClass = 1;
		kData.wLv	= 1;
		kData.iRace = 1;
        kData.i64Money = iMoney;
		
		GET_DEF(PgClassDefMgr, kClassDefMgr);
		SPlayerBasicInfo kBasicInfo;
		kClassDefMgr.GetAbil(SClassKey(kData.iClass, kData.wLv), kBasicInfo);
		pData->pkPlayer->Create( rkGuildGuid, kData, kBasicInfo, SPlayerBinaryData());

		CUnit* pkUnit = dynamic_cast<CUnit*>(pData->pkPlayer);
		if( pkUnit )
		{
			PgInventory* pkInv = pkUnit->GetInven();
			if( pkInv )
			{
				pkInv->Init(pkInvExtern, pkExternIdx);

				if( NULL != pkItemList )
				{
					pkInv->InsertItemList(*pkItemList);
				}
			}
		}
	}
	
	// 길드 컨텐츠로 알림
	return true;
}

bool PgRealmUserManager::Locked_UnregistGuildInfo(BM::GUID const& rkGuildGuid)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	CONT_CENTER_PLAYER_BY_KEY::iterator find_iter = m_kContPlayer_Guild.find( rkGuildGuid );
	if( m_kContPlayer_Guild.end() == find_iter )
	{
		return false;
	}

	SCenterPlayerData* pData = (*find_iter).second;

	SAFE_DELETE(pData->pkPlayer);
	SAFE_DELETE(pData->pkBackupPlayer);
	m_kContPlayer_Guild.erase(find_iter);
	return true;
}

void PgRealmUserManager::Locked_Recv_PT_S_T_REQ_SAVE_CHARACTOR_SLOT(BM::GUID const& rkMemberGuid, BM::GUID const& rkCharacterGuid, int const iSlot)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	
	BM::GUID kQueryGuid = BM::GUID::Create();
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SAVE_CHARACTOR_SLOT, L"EXEC [dbo].[up_SaveSelectedSlot]");
	kQuery.InsertQueryTarget(rkCharacterGuid);
	kQuery.QueryOwner( rkMemberGuid );
	kQuery.QueryGuid(kQueryGuid);
	kQuery.PushStrParam( rkCharacterGuid );
	kQuery.PushStrParam( iSlot );
	g_kCoreCenter.PushQuery(kQuery);
}

void PgRealmUserManager::Locked_Recv_PT_S_T_REQ_FIND_CHARACTOR_EXTEND_SLOT(BM::GUID const& rkMemberGuid, short const nChannelNo)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	
	BM::GUID kQueryGuid = BM::GUID::Create();
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_FIND_CHARACTOR_EXTEND_SLOT, L"EXEC [dbo].[up_Select_Member_ExtendSlot]");
	kQuery.InsertQueryTarget(rkMemberGuid);
	kQuery.QueryOwner( rkMemberGuid );
	kQuery.PushStrParam( rkMemberGuid );
	kQuery.contUserData.Push(nChannelNo);
	g_kCoreCenter.PushQuery(kQuery);
}


bool PgRealmUserManager::Locked_Q_DQT_FIND_CHARACTOR_EXTEND_SLOT(CEL::DB_RESULT &rkResult)
{//EXEC [dbo].[up_select_member_ExtendSlot]
	BM::CAutoMutex kLock(m_kMutex, true);
	
	//INFO_LOG( BM::LOG_LV9, __FL__ << _T("Member ExtendSlot Query") );

	//CFunctionMonitor kFM(__FUNCTIONW__, __LINE__);
	SAFE_PLAYER_UNIT_ARRAY vtPlayers;

	CONT_DB_ITEM_LIST kItemList;

	BM::GUID kMemberGuid;
	BYTE byExtendSlot = 0;

	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if( iter != rkResult.vecArray.end() )
		{
			(*iter).Pop(kMemberGuid);	++iter;
			(*iter).Pop(byExtendSlot);	++iter;
		}
	}

	SCenterPlayer_Lock kLockPlayer;
	if (GetPlayerInfo(rkResult.QueryOwner(), true, kLockPlayer))
	{
		SCenterPlayerData* pkMemberData = kLockPlayer.pkCenterPlayerData;
		if(!pkMemberData)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false; 
		}

		//INFO_LOG( BM::LOG_LV6, _T("[%s]-[%d]<20> Send Login User's Character Data to Switch"), __FUNCTIONW__, __LINE__ );
		{
			BM::Stream kPacket(PT_T_S_ANS_FIND_CHARACTOR_EXTEND_SLOT);

			pkMemberData->kSwitchServer.WriteToPacket(kPacket);//
			kPacket.Push( pkMemberData->guidMember );
			kPacket.Push(byExtendSlot);

			short nChannelNo;
			rkResult.contUserData.Pop(nChannelNo);

			//맞는 센터를 통해서 줘야함?
			CONT_SERVER_HASH kContServer;
			g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CENTER, kContServer);

			CONT_SERVER_HASH::const_iterator server_itor = kContServer.begin();
			while(server_itor != kContServer.end())
			{
				if((*server_itor).first.nChannel == nChannelNo)
				{
					g_kProcessCfg.Locked_SendToServer((*server_itor).first, kPacket);
					break;
				}
				++server_itor;
			}

			return true;
		}
	}
	else
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T("Not Find Login User MemberGuid[") << rkResult.QueryOwner().str().c_str() << _T("]") );
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgRealmUserManager::Locked_RegistChatBlock(BM::Stream * const pPacket)
{
	BYTE BlockType = 0;
	BM::GUID CharGuid;
	std::wstring Name;

	pPacket->Pop(CharGuid);
	pPacket->Pop(Name);
	pPacket->Pop(BlockType);

	BM::Stream ResultPacket(PT_T_M_NFY_RESULT_REGIST_CHAT_BLOCK);
	ResultPacket.Push(CharGuid);	// 차단을 요청했던 캐릭터 GUID.
	ResultPacket.Push(Name);		// 차단 등록을 실패한 캐릭터 이름(차단 당하는 캐릭터).
	ResultPacket.Push(BlockType);
	
	// 현재 접속중인지 확인.
	SCenterPlayer_Lock LockPlayer;
	if( GetPlayerInfo( Name, LockPlayer ) )
	{	// 접속 중인 것을 확인. DB 갱신.
		BM::GUID QueryGuid = BM::GUID::Create();
		CEL::DB_QUERY Query(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_User_ChatBlock]");
		Query.InsertQueryTarget(CharGuid);
		Query.QueryOwner( CharGuid );
		Query.QueryGuid(QueryGuid);
		Query.PushStrParam(CharGuid);
		Query.PushStrParam(Name);
		Query.PushStrParam(BlockType);
		Query.PushStrParam(1);		// INSERT INTO
		g_kCoreCenter.PushQuery(Query);

		ResultPacket.Push(true);
	}
	else
	{	// 차단하려는 캐릭터가 게임내에 존재 하지않음. Map으로 실패 전송.
		ResultPacket.Push(false);
	}

	SCenterPlayer_Lock LockReceiver;
	if( GetPlayerInfo( CharGuid, false, LockReceiver ) )
	{
		SendToGround( LockReceiver.pkCenterPlayerData->ChannelNo(), LockReceiver.pkCenterPlayerData->pkPlayer->GroundKey(), ResultPacket);
	}
}

void PgRealmUserManager::Locked_UnRegistChatBlock(BM::Stream * const pPacket)
{
	BM::GUID CharGuid;
	std::wstring Name;

	pPacket->Pop(CharGuid);
	pPacket->Pop(Name);

	BM::GUID QueryGuid = BM::GUID::Create();
	CEL::DB_QUERY Query(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_User_ChatBlock]");
	Query.InsertQueryTarget(CharGuid);
	Query.QueryOwner( CharGuid );
	Query.QueryGuid(QueryGuid);
	Query.PushStrParam(CharGuid);
	Query.PushStrParam(Name);
	Query.PushStrParam(0);		// 안씀.
	Query.PushStrParam(3);		// DELETE FROM
	g_kCoreCenter.PushQuery(Query);
}

void PgRealmUserManager::Locked_ChangeChatBlockOption(BM::Stream * const pPacket)
{
	BYTE BlockType = 0;
	BM::GUID CharGuid;
	std::wstring Name;

	pPacket->Pop(CharGuid);
	pPacket->Pop(Name);
	pPacket->Pop(BlockType);

	BM::GUID QueryGuid = BM::GUID::Create();
	CEL::DB_QUERY Query(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC [dbo].[up_Update_User_ChatBlock]");
	Query.InsertQueryTarget(CharGuid);
	Query.QueryOwner( CharGuid );
	Query.QueryGuid(QueryGuid);
	Query.PushStrParam(CharGuid);
	Query.PushStrParam(Name);
	Query.PushStrParam(BlockType);
	Query.PushStrParam(2);		// UPDATE
	g_kCoreCenter.PushQuery(Query);
}

void PgRealmUserManager::Locked_Q_DQT_GET_MEMBER_DAILY_REWARD(CEL::DB_RESULT &rkResult)
{
	BM::GUID kOwner = rkResult.QueryOwner();
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed get reward for player " << kOwner);
		return;
	}

    int DayNo;
    BM::DBTIMESTAMP_EX RewardDate;
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itor)
	{
		(*itor).Pop(DayNo);			++itor;
		(*itor).Pop(RewardDate);	++itor;
	}


	SCenterPlayer_Lock kLockPlayer;
	if (!GetPlayerInfo(kOwner, true, kLockPlayer) || !kLockPlayer.pkCenterPlayerData->pkPlayer)
	{
		INFO_LOG(BM::LOG_LV3, __FL__<<L"Failed get player " << kOwner);
		return;
	}

	PgDailyReward & kDailyReward = kLockPlayer.pkCenterPlayerData->pkPlayer->GetDailyReward();
	kDailyReward.RewardDate(RewardDate);
	// Rest day no, too late take a reward
	kDailyReward.DayNo(kDailyReward.IsExpired() ? 0 : DayNo);
}