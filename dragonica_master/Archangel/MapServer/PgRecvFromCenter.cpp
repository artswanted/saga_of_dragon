#include "stdafx.h"
#include "BM/PgFilterString.h"
#include "FCS/AntiHack.h"
#include "variant/PgDynamicDefMgr.h"
#include "variant/PgClassDefMgr.h"
#include "Variant/PgPartyMgr.h"
#include "variant/ItemBagMgr.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/PgMissionInfo.h"
#include "Variant/PgMCtrl.h"
#include "variant/PgStoreMgr.h"
#include "variant/PgItemOptionMgr.h"
#include "variant/PgEventview.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "variant/MonsterDefMgr.h"
#include "Variant/PgDefSpendMoney.h"
#include "variant/PgMacroCheckTable.h"
#include "PgRecvFromCenter.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgIndun.h"
#include "PgGroundMgr.h"
#include "PgGenPointMgr.h"
#include "PgQuest.h"
#include "PgMissionMan.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "SkillEffectAbilSystem\PgEffectAbilHandleManager.h"
#include "SkillEffectAbilSystem\PgSkillAbilHandleManager.h"
#include "SkillEffectAbilSystem\PgSkillOnFindTargetManager.h"
#include "PgMonKillCountReward.h"
#include "variant/pggamblemachine.h"
#include "PgVolatileInven.h"
#include "PgBSMapGame.h"
#include "PgSkillLinkageInfo.h"
#include "PgPVPEffectSelector.h"
#include <hotmeta/hotmeta.h>
#include "ActorEventSystem.h"
#include "Variant/PgBattlePassMgr.h"

extern bool CALLBACK OnRecvFromCenter_01( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket, WORD const wkType );

void CALLBACK OnAcceptFromCenter( CEL::CSession_Base *pkSession )
{	//!  세션락
	static int i = 0;
	std::wcout<< __FUNCTIONW__ << _T("Accepted : ") << pkSession << _T(" [") << i++ << _T("]") <<std::endl;
}

void CALLBACK OnConnectToCenter( CEL::CSession_Base *pkSession )
{	//!  세션락
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Session is NULL");
		return;
	}

	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
		g_kProcessCfg.Locked_OnConnectServer(pkSession);
		INFO_LOG( BM::LOG_LV6, __FL__<<L"Success ["<<C2L(pkSession->Addr())<<L"]");
	}
	else
	{
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		std::wcout << L"[ConnectToCenter] Failed : " << pkSession->Addr().ToString().c_str() << std::endl;
		g_kProcessCfg.Locked_ConnectCenter();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("OnConnectToCenter is Failed!"));
	}
}

void CALLBACK OnDisConnectToCenter( CEL::CSession_Base *pkSession )
{//!  세션락
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Session is NULL");
		return;
	}

	SERVER_IDENTITY kSI;
	g_kProcessCfg.Locked_OnDisconnectServer( pkSession, &kSI );
	INFO_LOG(BM::LOG_LV1,__FL__<<L"CENTER Disconnected["<<C2L(pkSession->Addr())<<L"] !!!");

	g_kGndMgr.ProcessRemoveUser( kSI );

	g_kProcessCfg.Locked_ConnectCenter();
}

bool CheckMinRangeAndSkillRange()
{
	bool bReturn = true;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	GET_DEF(CMonsterDefMgr, kMonsterDefMgr);

	CMonsterDefMgr::CONT_DEF kContMonster;
	kMonsterDefMgr.GetCont(kContMonster);

	CSkillDefMgr::CONT_DEF kContSkill;
	kSkillDefMgr.GetCont(kContSkill);

	CMonsterDefMgr::CONT_DEF::const_iterator mon_it = kContMonster.begin();
	while(kContMonster.end() != mon_it)
	{
		CMonsterDef const* pDef = (*mon_it).second;
		if(pDef)
		{
			for(int i = 0; i < 10; ++i)
			{
				CSkillDefMgr::CONT_DEF::const_iterator skill_it = kContSkill.find(pDef->GetAbil(WORD(AT_MON_SKILL_01+i)));
				if( kContSkill.end() != skill_it )
				{
					CSkillDef const *pkSkillDef = (*skill_it).second;
					if( pkSkillDef && 0<(*skill_it).first )
					{
						int const iMinRange = pkSkillDef->GetAbil(AT_MON_MIN_RANGE);
						int const iAttkRange = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
						int const iDelta = iMinRange - iAttkRange;
						if(0<iMinRange)	//어빌이 있는 애들만 비교하자
						{
							if(AI_MONSTER_MIN_DISTANCE_CHASE_Q>=iDelta*iDelta)
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Monster["<<(*mon_it).first<<L"] Skill["<<(*skill_it).first<<L"] Delta["<<iDelta<<L"]");
								bReturn = false;
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
							}
						}
						else	//어빌이 없으면
						{
							if( AI_MONSTER_MIN_DISTANCE_FROM_TARGET >= iAttkRange )
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Monster["<<(*mon_it).first<<L"] Skill["<<(*skill_it).first<<L"] MinRange["<<iMinRange<<L"]");
								bReturn = false;
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
							}
						}
					}
				}
			}
		}
		++mon_it;
	}
	return bReturn;
}

bool RecvGameData(BM::Stream &rkPacket, bool bReload = false)
{//
	BM::GUID kShopValueKeyGuid;
	rkPacket.Pop(kShopValueKeyGuid);

	INFO_LOG(BM::LOG_LV1, _T("RecvGameData SetStoreKey::")<< kShopValueKeyGuid);
	g_kControlDefMgr.StoreValueKey(kShopValueKeyGuid);

	bool bReturn = true;
{//블럭 없에면 데드락
	//BM::CAutoMutex kLock(g_kTblDataMgr.GetLock()->kObjectLock);
	g_kTblDataMgr.PacketToData(rkPacket);
}

	SReloadDef kReloadDef;
	int iLoadDef = 
		PgControlDefMgr::EDef_MonsterDefMgr
		|	PgControlDefMgr::EDef_SkillDefMgr
		|	PgControlDefMgr::EDef_ItemDefMgr
		|	PgControlDefMgr::EDef_EffectDefMgr
		|	PgControlDefMgr::EDef_RareDefMgr
		|	PgControlDefMgr::EDef_ItemEnchantDefMgr
		|	PgControlDefMgr::EDef_ClassDefMgr
		|	PgControlDefMgr::EDef_ItemBagMgr
		|	PgControlDefMgr::EDef_GenPointMgr
		|	PgControlDefMgr::EDef_StoreMgr
		|	PgControlDefMgr::EDef_SpendMoneyMgr
		|	PgControlDefMgr::EDef_DynamicDefMgr
		|	PgControlDefMgr::EDef_ItemOptionMgr
		|	PgControlDefMgr::EDef_ExtMgr
		|	PgControlDefMgr::EDef_ItemSetDefMgr
		|	PgControlDefMgr::EDef_ObjectDefMgr
		|	PgControlDefMgr::EDef_PropertyMgr
		|	PgControlDefMgr::EDef_GroundEffect
		|	PgControlDefMgr::EDef_RecommendationItem
		|	PgControlDefMgr::EDef_Pet
		|	PgControlDefMgr::EDef_Default
		|	PgControlDefMgr::EDef_PremiumMgr
		|	PgControlDefMgr::EDef_MapDefMgr;

	if(!bReload)
	{	
		g_kControlDefMgr.AddCustomDef<PgGenPointMgr>(PgControlDefMgr::EDef_GenPointMgr);
		iLoadDef |= PgControlDefMgr::EDef_OnlyFirst;
	}

	g_kTblDataMgr.GetReloadDef(kReloadDef, iLoadDef);
	if ( !g_kControlDefMgr.Update(kReloadDef, iLoadDef) )
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("PgControlDefMgr Update failed") );
		LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	{
		//임시로 이 서버가 가진 맵 몹록들을 뽑자
		CONT_MAP_CONFIG kMapCfg_Static;
		CONT_MAP_CONFIG kMapCfg_Mission;
		g_kProcessCfg.Locked_GetMapServerCfg(&kMapCfg_Static,&kMapCfg_Mission);

		CONT_MAP_CONFIG kMyMapConfig;
		CONT_MAP_BUILD_DATA kMapBuildData;
		PgGroundRscMgr::GetMapBuildData(	g_kProcessCfg.ServerIdentity(),	kMapCfg_Static, kMapCfg_Mission,
			kReloadDef.pkContMap, kReloadDef.pkMissionRoot,	kReloadDef.pkMissionCandi, kReloadDef.pkMissionBonusMap, kReloadDef.pkSuperGroundGrp, kMapBuildData);

		// Custom Def Build
		GET_DEF_CUSTOM(PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
		if ( ! const_cast<PgGenPointMgr*>(&kGenPointMgr)->Build(*kReloadDef.pkRegenPPoint, *kReloadDef.pContDefMonsterBagControl, *kReloadDef.pContDefMonsterBag,
			*kReloadDef.pkMonsterBagElements, *kReloadDef.pkSuccessRateControl, *kReloadDef.pkDefObjectBag, *kReloadDef.pkDefObjectBagElements, *kReloadDef.pkItemBag, *kReloadDef.pkItemBagElements, kMapBuildData ))
		{
			bReturn = false;
			ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("PgGenPointMgr Build failed") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false") );
		}
	}
	//g_kRegenPointMgr.Build(*kReloadDef.pkRegenPPoint, *kReloadDef.pContDefMonsterBagControl, *kReloadDef.pContDefMonsterBag,
	//	*kReloadDef.pkMonsterBagElements, *kReloadDef.pkSuccessRateControl );

	if (!g_kMonKillCountReward.Init())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("MonKillCountReward Init failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	if (!g_kQuestMan.Build(kReloadDef.pkQuestReward))// 리로드 기능 OK
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("QuestMan Build failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	g_kEffectAbilHandleMgr.Release();
	g_kEffectAbilHandleMgr.Init();
	if (!g_kEffectAbilHandleMgr.Build())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("EffectAbilHandleMgr Build failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	g_kSkillAbilHandleMgr.Release();
	g_kSkillAbilHandleMgr.Init();
	if (!g_kSkillAbilHandleMgr.Build())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("SkillAbilHandleMgr Build failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	g_kSkillOnFindTargetMgr.Release();
	g_kSkillOnFindTargetMgr.Init();
	if (!g_kSkillOnFindTargetMgr.Build())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("SkillOnFindTargetMgr Build failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	g_kSkillLinkageInfo.Release();
	if(!g_kSkillLinkageInfo.Build())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("SkillLinkageInfo Build failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	g_kPVPEffectSlector.Release();
	if(!g_kPVPEffectSlector.Build())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("PVPEffectSelector Build failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	if (!CheckMinRangeAndSkillRange())
	{
		bReturn = false;
		ASSERT_LOG( false, BM::LOG_LV4, __FL__ << _T("CheckMinRangeAndSkillRange failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	if( !bReload )
	{
		CONT_DEF_FILTER_UNICODE const* pkFilterUnicode = NULL;
		g_kTblDataMgr.GetContDef(pkFilterUnicode);
		if( pkFilterUnicode->empty() )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"[DR2_Def].[dbo].[TB_DefFilterUnicode] row count is 0");
		}

		CONT_DEF_FILTER_UNICODE::const_iterator filter_iter = pkFilterUnicode->begin();
		while( pkFilterUnicode->end() != filter_iter )
		{
			CONT_DEF_FILTER_UNICODE::value_type const& rkFilter = (*filter_iter);
			g_kUnicodeFilter.AddRange(rkFilter.iFuncCode, rkFilter.bFilterType, rkFilter.cStart, rkFilter.cEnd);
			++filter_iter;
		}
	}

	if (!bReload)
	{
		CONT_MAP_CONFIG kMapCfg_Static;
		CONT_MAP_CONFIG kMapCfg_Mission;
		g_kProcessCfg.Locked_GetMapServerCfg(&kMapCfg_Static,&kMapCfg_Mission);

		CONT_MAP_CONFIG kMyMapConfig;
		CONT_MAP_BUILD_DATA kMapBuildData;
		bReturn = PgGroundRscMgr::GetMapBuildData(	g_kProcessCfg.ServerIdentity(),	kMapCfg_Static, kMapCfg_Mission,
													kReloadDef.pkContMap, kReloadDef.pkMissionRoot,	kReloadDef.pkMissionCandi, kReloadDef.pkMissionBonusMap, kReloadDef.pkSuperGroundGrp, kMapBuildData) && bReturn;
		
		INFO_LOG(BM::LOG_LV1, _T("---- LoadMap Info Start---- "));
		CONT_MAP_BUILD_DATA::const_iterator maps_itor = kMapBuildData.begin();
		while(maps_itor != kMapBuildData.end())
		{
			INFO_LOG(BM::LOG_LV1, (*maps_itor).iGroundNo);
			++maps_itor;
		}
		INFO_LOG(BM::LOG_LV1, _T("---- LoadMap Info End---- TotalCount = ")<< kMapBuildData.size());

		bReturn = g_kGndMgr.Reserve( kMapBuildData ) && bReturn;
		bReturn = g_kMissionContMgr.Build(*kReloadDef.pkMissionResult, *kReloadDef.pkMissionCandi, *kReloadDef.pkMissionRoot) && bReturn;

		// Mission Xml Loading
		//INFO_LOG(BM::LOG_LV0, _T("Start DefMission_LevelRoot-loading Mission ..."));
		bReturn = g_kMissionMan.Create() && bReturn;
	}

	bReturn = PgWorldEventUtil::DisplayResult() && bReturn;

	return bReturn;
}

bool IsCashShopOrOpenMarketPacketType(BM::Stream::DEF_STREAM_TYPE wkType)
{	
	switch(wkType)
	{
	//캐시샵 관련 패킷 추가
	case PT_C_M_CS_REQ_ENTER_CASHSHOP:
	//case PT_C_M_CS_REQ_EXIT_CASHSHOP:
	case PT_C_M_CS_REQ_RECV_GIFT:
	case PT_C_M_CS_REQ_MODIFY_VISABLE_RANK:
	case PT_C_M_CS_REQ_SEND_GIFT:
	case PT_C_M_CS_REQ_ADD_TIMELIMIT:
	//case PT_C_M_CS_REQ_LAST_RECVED_GIFT:
	case PT_C_M_CS_REQ_BUY_ARTICLE:

	// 오픈마켓 관련 패킷 추가
	case PT_C_M_UM_REQ_MARKET_ENTER:
	//case PT_C_M_UM_REQ_MARKET_EXIT:
	case PT_C_M_UM_REQ_ARTICLE_REG:
	case PT_C_M_UM_REQ_MARKET_CLOSE:
	case PT_C_M_UM_REQ_BEST_MARKET_LIST:
	case PT_C_M_UM_REQ_ARTICLE_DEREG:
	case PT_C_M_UM_REQ_MARKET_QUERY:
	case PT_C_M_UM_REQ_ARTICLE_BUY:
	case PT_C_M_UM_REQ_DEALINGS_READ:
	case PT_C_M_UM_REQ_MINIMUM_COST_QUERY:
	case PT_C_M_UM_REQ_MY_MARKET_QUERY:
	case PT_C_M_UM_REQ_MARKET_OPEN:
	case PT_C_M_UM_REQ_USE_MARKET_MODIFY_ITEM:
	case PT_C_M_UM_REQ_MARKET_ARTICLE_QUERY:
	case PT_C_M_UM_REQ_MARKET_MODIFY_STATE:
	//노점 관련 패킷 추가
	case PT_C_M_REQ_VENDOR_CREATE:
	case PT_C_M_REQ_VENDOR_RENAME:
	case PT_C_M_REQ_VENDOR_DELETE:
	case PT_C_M_REQ_VENDOR_STATE:
	case PT_C_M_UM_REQ_MY_VENDOR_QUERY:
	case PT_C_M_UM_REQ_VENDOR_ENTER:
	case PT_C_M_UM_REQ_VENDOR_EXIT:
	case PT_C_M_UM_REQ_VENDOR_REFRESH_QUERY:
		{
			return true;
		}break;
	default:
		{
		}break;
	}

	return false;
}

bool IsPacketType(BM::Stream::DEF_STREAM_TYPE wkType)
{// 공용맵서버에서 허용 되지 않는 패킷은 여기서 걸러주자.
	switch( wkType )
	{
	case 0:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Invalid packet type[0]"));
			return false;
		}break;
	case PT_T_M_NFY_PREPARE_MISSION:// 미션맵은 공용맵서버에 만들 수 없다.
		case PT_T_M_NFY_ENTER_SUPER_GROUND:

	//	파티관련 패킷은 공용맵서버에서는 불가능함.
	case PT_C_N_REQ_JOIN_PARTY://Route packet to Contents
	case PT_C_N_REQ_JOIN_PARTYFIND:
	case PT_C_N_ANS_JOIN_PARTY:
	case PT_C_N_ANS_JOIN_PARTYFIND:
//	case PT_C_N_REQ_LEAVE_PARTY:
	case PT_C_N_REQ_KICKOUT_PARTY_USER:
//	case PT_C_N_REQ_PARTY_CHANGE_MASTER:
//	case PT_C_N_REQ_PARTY_CHANGE_OPTION:
	case PT_C_N_REQ_CREATE_PARTY:
//	case PT_C_N_REQ_PARTY_RENAME:
	case PT_C_N_REQ_PARTY_RENAME_GM:
	case PT_C_M_REQ_PARTY_LIST:

	// 아이템 거래 공용맵서버에서는 불가능함
	case PT_C_M_REQ_EXCHANGE_ITEM_REQ:
		{
			if ( g_kProcessCfg.IsPublicChannel() )
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__<<L"This Server's Recv Impossible Packet["<<wkType<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}
	return true;
}

void CALLBACK OnRecvFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	WORD wkType = 0;
	pkPacket->Pop(wkType);

	//std::cout<< "패킷 받음 T:[" << wkType <<"] Size["<< pkPacket->Size() << "]"<< std::endl;
	if ( !IsPacketType(wkType) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("IsPacketType is false"));
		return;
	}

	switch( wkType )
	{
	case PT_SYNC_EVENT_ITEM_REWARD:
		{
			CONT_EVENT_ITEM_REWARD kCont;
			PU::TLoadTable_AM(*pkPacket,kCont);
			g_kTblDataMgr.SetContDef(kCont);
		}break;
	case PT_SYNC_GAMBLEMACHINE:
		{
			g_kGambleMachine.ReadFromPacket(*pkPacket);
		}break;
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{//	
			INFO_LOG( BM::LOG_LV6, __FL__<<L"PT_A_S_ANS_GREETING" );

			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);
			
			g_kProcessCfg.Locked_Read_ServerList(*pkPacket);
			g_kProcessCfg.Locked_ConnectSwitch();
			g_kProcessCfg.Locked_ConnectLog();	
			if ( S_OK == SetSendWrapper( kRecvSI) )
			{
				if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
				{
					if (!RecvGameData(*pkPacket))
					{
						ASSERT_LOG(false, BM::LOG_LV2, __FL__ << _T("GameData build failed"));
					}
					
					g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);
					PgGround::SetAntiHackVariable();

					g_kGambleMachine.ReadFromPacket(*pkPacket);
					g_kDefPlayTime.ReadFromPacket(*pkPacket);

					//					ReadyToService();//모든 서버용(센터 제외)
					INFO_LOG(BM::LOG_LV6, _T("=========================================="));
					INFO_LOG(BM::LOG_LV6, _T("=========== MAP SERVER START!! ==========="));
					INFO_LOG(BM::LOG_LV6, _T("=========================================="));
				}
				else
				{
					g_kGndMgr.SendAllGround();
				}

				BM::Stream kPacket(PT_A_N_REQ_GAMETIME);
				pkSession->VSend(kPacket);
				pkSession->VSend(BM::Stream(PT_A_N_REQ_BATTLE_PASS_INFO));
				return;
			}
			
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Add Server Session Failed Realm["<<kRecvSI.nRealm<<L"] Chn["<<kRecvSI.nChannel<<L"] No["<<kRecvSI.nServerNo<<L"] Type["<<kRecvSI.nServerType<<L"] ");
		}break;
// 	case PT_T_M_CONNECT_NEW_SWITCH:
// 		{
// 			INFO_LOG( BM::LOG_LV6, _T("[PT_T_M_CONNECT_NEW_SWITCH] Recved") );
// 			g_kProcessCfg.Locked_ConnectSwitch();
// 		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;

			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharacterGuid);
			g_kGndMgr.ProcessRemoveUser(kCharacterGuid);
		}break;
	case PT_T_M_NFY_RELOAD_GAMEDATA:
		{
			INFO_LOG(BM::LOG_LV5, _T("Reload GameData from Center"));
			RecvGameData(*pkPacket, true);
			INFO_LOG(BM::LOG_LV5, _T("Reload Success"));
			//else INFO_LOG(BM::LOG_LV5, _T("Reload Faild"));
		}break;
	case PT_M_T_REQ_INDUN_PARTY_LIST:
	case PT_T_C_NFY_BULLHORN:
	case PT_T_C_NFY_NOTICE:
	case PT_N_C_NFY_NOTICE_PACKET:
	case PT_T_C_NFY_MARRY:
		{
			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(wkType);
			kEventMsg.Push(*pkPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	case PT_T_A_EVENT_SYNC:
		{
			g_kEventView.RecvSync(*pkPacket);
		}break;
	case PT_T_A_COUPON_EVENT_SYNC:
		{
			BM::Stream kEvent;
			kEvent.Push(E_CouponEvent_ReadFromPacket);
			kEvent.Push(*pkPacket);
			SendToCouponEventView_Map(kEvent);
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	case PT_I_M_MACRO_CHECK_TABLE_SYNC:
		{
			g_kMacroCheckTable.ReadFromPacket(*pkPacket);
		}break;
	case PT_M_C_NFY_GAMBLEMACHINE_MIXUP_RESULT:
	case PT_M_C_NOTI_ACHIEVEMENT_COMPLETE_FIRST:
	case PT_M_C_NFY_GENERIC_ACHIEVEMENT_NOTICE:
	case PT_M_C_NFY_GAMBLEMACHINE_RESULT:
	case PT_M_C_NFY_ITEM_MAKING_SUCCESS:
	case PT_M_C_NOTI_OPEN_GAMBLE:
	case PT_M_C_NFY_USEITEM:
	case PT_I_M_NOTI_OXQUIZ_NOTI:
	case PT_M_C_NOTI_OXQUIZ_OPEN:
	case PT_M_C_NOTI_OXQUIZ_LOCK:
	case PT_M_C_LUCKYSTAR_RESULT:
	case PT_M_C_LUCKYSTAR_CLOSE:
	case PT_M_C_NFY_EVENT_QUEST_INFO:
	case PT_M_C_NFY_EVENT_QUEST_NOTICE:
	case PT_N_C_NFY_BS_NOTICE:
	case PT_N_M_NFY_ALL_USER_ADDEFFECT:
	case PT_N_M_NFY_ALL_GROUND_WORLD_ENVIRONMENT_STATUS:
	case PT_N_C_NFY_REALM_QUEST_INFO:
	case PT_N_C_NFY_REALM_QUEST_REWARD:
	case PT_M_C_NOTI_EVENT_ITEM_REWARD:
	case PT_M_C_NOTI_TREASURE_CHEST:
	case PT_T_M_NFY_RECOVERY_STRATEGY_FATIGUABILITY:
	case PT_T_M_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE:
	case PT_T_M_NFY_EVENT_GROUND_USER_COUNT_MODIFY:
		{
			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(wkType);
			kEventMsg.Push(*pkPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	case PT_IM_A_NFY_ANTIHACK_CONTROL:
		{
			g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);
			PgGround::SetAntiHackVariable();
		}break;
	case PT_A_A_NFY_REFRESH_DB_DATA:
		{
			BM::GUID kValueKey;
			CONT_DEF_CASH_SHOP kContShopMain;//.clear();
			CONT_DEF_CASH_SHOP_ARTICLE kContShopArticle;//.clear();
			CONT_CASH_SHOP_ITEM_LIMITSELL kContShopLimitSell;
            CONT_TBL_SHOP_IN_EMPORIA kShopInEmporia;
			CONT_TBL_SHOP_IN_GAME kShopInGame;
			CONT_TBL_SHOP_IN_STOCK kShopInStock;
			CONT_SHOPNPC_GUID kShopNpcGuid;

			///////////////////////////////////////////////////////
			//Read Part
			{
				INFO_LOG(BM::LOG_LV1, _T("RefreshDBData SetStoreKey::")<< kValueKey);
				pkPacket->Pop(kValueKey);
				g_kControlDefMgr.StoreValueKey(kValueKey);

				PU::TLoadTable_MM(*pkPacket, kContShopMain);
				PU::TLoadTable_MM(*pkPacket, kContShopArticle);
				PU::TLoadTable_MM(*pkPacket, kContShopLimitSell);

				g_kTblDataMgr.SetContDef(kContShopMain);
				g_kTblDataMgr.SetContDef(kContShopArticle);
				g_kTblDataMgr.SetContDef(kContShopLimitSell);

                PU::TLoadTable_MM(*pkPacket, kShopInEmporia);
				PU::TLoadTable_MM(*pkPacket, kShopInGame);
				PU::TLoadTable_MM(*pkPacket, kShopInStock);
				PU::TLoadTable_AA(*pkPacket, kShopNpcGuid);

				SReloadDef kReloadDef;
				int const iLoadDef = PgControlDefMgr::EDef_StoreMgr;
                kReloadDef.pkShopInEmporia = &kShopInEmporia;
				kReloadDef.pkShopInGame = &kShopInGame;
				kReloadDef.pkShopInStock = &kShopInStock;
				kReloadDef.pkShopNpcGuid = &kShopNpcGuid;
				if ( !g_kControlDefMgr.Update(kReloadDef, iLoadDef) )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot update StoreMgr"));
				}
			}
		}break;
	case PT_SYNC_DEF_PLAYERPLAYTIME:
		{
			g_kDefPlayTime.ReadFromPacket(*pkPacket);
			
			BM::Stream kPacket;
			g_kDefPlayTime.WriteToPacket(kPacket);

			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(PT_SYNC_DEF_PLAYERPLAYTIME);
			kEventMsg.Push(kPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	case PT_M_M_UPDATE_PLAYERPLAYTIME:
		{
			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(PT_M_M_UPDATE_PLAYERPLAYTIME);
			kEventMsg.Push(*pkPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	case PT_N_T_NFY_TRADE:
		{
			SEventMessage kEventMsg;

			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(wkType);
			kEventMsg.Push(*pkPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	default:
		{
			if(OnRecvFromCenter_01(pkSession, pkPacket, wkType)){break;}

			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << wkType << _T("]"));
		}break;
	}
}

void ProcessReqMapMove( CUnit* pkCaster, SReqMapMove_CM const &kRMM )
{
	if ( pkCaster )
	{
		BM::Stream kNfyPacket( PT_A_M_NFY_REQ_MAP_MOVE, kRMM );
		pkCaster->VNotify(&kNfyPacket);
	}
}

void ProcessReqMapMove(CUnit* pkCaster,SReqMapMove_MT const& kRMM)
{
	if ( pkCaster )
	{
		BM::Stream kNfyPacket(PT_U_G_REQ_MAP_MOVE,kRMM);
		pkCaster->VNotify(&kNfyPacket);
	}
}

bool CALLBACK OnRecv_PT_A_A_WRAPPED_PACKET( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket  )
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	SGroundKey kKey;

	pkPacket->Pop(kKey);
	pkPacket->Pop(usType);

	switch(usType)
	{
	case PT_N_M_ANS_MISSION_RESTART:
		{
			// 미션맵 다시 시작
			g_kGndMgr.RestartMission(pkPacket);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}
	return true;
}

bool CALLBACK OnRecvFromCenter_01( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket, WORD const wkType )
{
	//size_t const user_packet_pos = pkPacket->RdPos() - sizeof(unsigned short);

	switch( wkType )
	{
	case PT_A_GND_WRAPPED_PACKET:
		{	
			g_kGndMgr.RecvGndWrapped(pkPacket);
		}break;
	case PT_A_A_WRAPPED_PACKET:
		{
			OnRecv_PT_A_A_WRAPPED_PACKET(pkSession, pkPacket);
		}break;
//	case PT_T_M_NFY_CONNECT_CONTENTS:
//		{// Contents가 새로 접속했다.
// 			INFO_LOG(BM::LOG_LV6,_T("[%s] Connect Contents"), __FUNCTIONW__);
// 			g_kGndMgr.SendAllBalancingGnd();
// 		}break;
// 	case PT_T_M_NFY_DISCONNECT_CONTENTS:
// 		{
// 			INFO_LOG(BM::LOG_LV2,_T("[%s] DisConnect Contents"), __FUNCTIONW__);
// 
// 			// 인던은 삭제
// 			g_kGndMgr.Release(GATTR_INSTANCE);
// 
// 			// 현재 존재하는 모든 파티를 끊어 버려
// 			SEventMessage kEventMsg;
// 			kEventMsg.PriType(PMET_PARTY);
// 			kEventMsg.SecType(PT_T_M_NFY_DISCONNECT_CONTENTS);
// 			g_kTask.PutMsg(kEventMsg);
// 		}break;
	case PT_N_M_NFY_SYNC_CASHSHOP_ON_OFF:
		{			
			g_kEventView.ReadFromPacket(*pkPacket);
		}break;
	case PT_N_A_NFY_BATTLE_PASS_INFO:
		{
			g_kBattlePassMgr.Locked_ReadFromPacket(*pkPacket);
		}break;
	case PT_N_A_NFY_GAMETIME:
		{
			//INFO_LOG(BM::LOG_LV9, _T("[%s] TickCount[%u]"), __FUNCTIONW__, BM::GetTime32());
			HRESULT hReturn = g_kEventView.ReadFromPacket(*pkPacket);
			switch ( hReturn )
			{
			case E_SYNCHED_FIRST_GAMETIME:
			case E_SYNCHED_AGAIN_GAMETIME:
				{
					BM::Stream kAgainPacket( PT_A_N_REQ_GAMETIME );
					pkSession->VSend( kAgainPacket );
				}break;		
			case E_SYNCHED_GAMETIME:
				{
					// 모든 Client에게 GameTime 시간을 다시 맞춘다.
					SEventMessage kMsg(PMET_GROUND_MGR, PT_M_C_SYNC_GAMETIME);
					kMsg.Push(PT_M_C_SYNC_GAMETIME);
					g_kEventView.WriteToPacket(true, kMsg);
					g_kTask.PutMsg(kMsg);
				}break;
			}
			if (PgClassDefMgr::s_sMaximumLevel <= 0)
			{
				// MAX LEVEL setting
				GET_DEF(PgClassDefMgr, kClassDef);
				const_cast<PgClassDefMgr&>(kClassDef).BuildLastLevel(g_kEventView.VariableCont().iMaxCharacterLevel);
			}

		}break;
	//-->
	case PT_T_M_REQ_CREATE_GROUND:
		{
			SGroundMakeOrder kOrder;
			kOrder.ReadFromPacket( *pkPacket );

			bool bAddData = false;
			pkPacket->Pop( bAddData );

			HRESULT const hRet = g_kGndMgr.OrderCreate( kOrder, bAddData ? pkPacket : NULL );
			if ( FAILED(hRet) )
			{	
				BM::Stream kFailedPacket( PT_M_T_ANS_CREATE_GROUND, kOrder.kKey );
				kFailedPacket.Push(hRet);
				SendToChannelContents( PMET_Portal, kFailedPacket );
			}
		}break;
	case PT_T_M_NFY_PREPARE_MISSION:
		{//미션맵을 만들자!.
			PgMission kMission;
			kMission.ReadFromPacket(*pkPacket);

			SGroundKey kCreateGndKey;
			kMission.GetGroundKey(kCreateGndKey);

			BM::Stream kRetPacket( PT_M_T_ANS_PREPARE_MISSION, kMission.GetID() );

			if ( g_kGndMgr.OrderCreateMission( &kMission ) )
			{
				kRetPacket.Push( true );
			}
			else
			{
				kRetPacket.Push( false );	

				BM::Stream kFailedPacket( PT_M_T_ANS_CREATE_GROUND, kCreateGndKey );
				kFailedPacket.Push(E_FAIL);
				SendToChannelContents( PMET_Portal, kFailedPacket );
			}

			SendToMissionMgr( kRetPacket );

		}break;
	case PT_T_M_NFY_ENTER_SUPER_GROUND:
		{
			SGroundMakeOrder kCreateOrder;
			kCreateOrder.kKey.ReadFromPacket(*pkPacket);
			pkPacket->Pop(kCreateOrder.iOwnerLv);
			
			if( S_OK == g_kGndMgr.OrderCreate(kCreateOrder, pkPacket) )
			{
				SGroundKey kOrgGndKey;
				BM::Stream kOrgGndPacket;
				kOrgGndKey.ReadFromPacket(*pkPacket);
				pkPacket->Pop( kOrgGndPacket.Data() );
				kOrgGndPacket.PosAdjust();
				::SendToGround(kOrgGndKey, kOrgGndPacket);
			}
			else
			{
				BM::Stream kFailedPacket( PT_M_T_ANS_CREATE_GROUND, kCreateOrder.kKey );
				kFailedPacket.Push(E_FAIL);
				::SendToChannelContents( PMET_Portal, kFailedPacket );
			}
		}break;
	case PT_N_T_REQ_CREATE_PUBLICMAP:
		{
			EContentsMessageType kMsgType = PMET_NONE;
			SGroundMakeOrder kGndMakeOrder;
			pkPacket->Pop( kMsgType );
			kGndMakeOrder.ReadFromPacket( *pkPacket );

			size_t const iRDPos = pkPacket->RdPos();

			HRESULT const hRet = g_kGndMgr.OrderCreate( kGndMakeOrder, pkPacket );

			pkPacket->RdPos( iRDPos );

			BM::Stream kAnsPacket( PT_T_N_ANS_CREATE_PUBLICMAP, hRet );
			kGndMakeOrder.WriteToPacket( kAnsPacket );
			kAnsPacket.Push( *pkPacket );
			::SendToRealmContents( kMsgType, kAnsPacket );
		}break;
	case PT_T_M_NFY_DELETE_GROUND:
		{
			bool bAttribute = true;
			pkPacket->Pop(bAttribute);
			if ( bAttribute )
			{
				T_GNDATTR kGndAttr = GATTR_DEFAULT;
				pkPacket->Pop(kGndAttr);
				g_kGndMgr.Release(kGndAttr);
			}
			else
			{

			}
		}break;
	default:
		{
			if (AES::Exec(wkType, pkPacket, NULL, NULL))
				return true;
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << wkType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

int GetConnectionUserCount()
{
	return g_kGndMgr.Locked_GetConnectionMapUserCount();
}