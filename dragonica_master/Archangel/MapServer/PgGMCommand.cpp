#include "stdafx.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/AlramMissionMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgMissionGround.h"
#include "PublicMap/PgWarGround.h"
#include "PgQuest.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgAction_Pet.h"
#include "publicmap/PgBSGround.h"
#include "PgSuperGround.h"
#include "PgActionJobSkill.h"
#include "PgStaticEventGround.h"
#include "PgStaticRaceGround.h"
#include "PgConstellationMgr.h"

namespace GmCommandUtil
{
	void CopyItem(CONT_ITEM_CREATE_ORDER& rkContOut, CONT_ITEM_CREATE_ORDER& rkContItem)
	{
		CONT_ITEM_CREATE_ORDER::iterator item_iter = rkContItem.begin();
		while( rkContItem.end() != item_iter )
		{
			(*item_iter).Guid( BM::GUID::Create() ); // GUID 빼고 전부 복사!
			rkContOut.push_back( (*item_iter) );

			++item_iter;
		}
	}
}

bool PgGround::GMCommand( PgPlayer *pkPlayer, EGMCmdType const iCmdType, BM::Stream * const pkNfy )
{
	if ( pkPlayer )
	{
		CONT_DEFGMCMD const *pkGmCmd = NULL;
		g_kTblDataMgr.GetContDef(pkGmCmd);

		if ( pkGmCmd )
		{
			// 로그를 남겨
			PgLogCont kLogCont(ELogMain_GMCommand, ELogSub_GMCommand_Recv);
			kLogCont.MemberKey( pkPlayer->GetMemberGUID() );
			kLogCont.CharacterKey( pkPlayer->GetID() );
			kLogCont.ID( pkPlayer->MemberID() );
			kLogCont.UID( pkPlayer->UID() );
			kLogCont.Name( pkPlayer->Name() );
			kLogCont.Class( pkPlayer->GetAbil(AT_CLASS) );
			kLogCont.Level( pkPlayer->GetAbil(AT_LEVEL) );
			kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );
			kLogCont.GroundNo( GetGroundNo() );

			PgGMLevelChecker kGMLevelChecker( *pkGmCmd );
			if ( true == kGMLevelChecker.IsAccess( iCmdType, pkPlayer->GMLevel() ) )
			{
				PgLog kLog( ELOrderMain_GMCommand, ELOrderSub_Complete );
				kLog.Set( 0, static_cast<int>(iCmdType) );				// iValue1 GMCommandNo
				kLog.Set( 1, static_cast<int>(pkPlayer->GMLevel()) );	// iValue2 GMLevel
				kLogCont.Add( kLog );
				kLogCont.Commit();

				BM::CAutoMutex kLock( m_kRscMutex );
				switch ( iCmdType )
				{
				case GMCMD_MYHOME_AUCTION_TIME:
					{
						short	siStreetNo = 0;
						int		iHouseNo = 0;

						pkNfy->Pop(siStreetNo);
						pkNfy->Pop(iHouseNo);

						BM::PgPackedTime kNewTime;
						kNewTime.SetLocalTime();

						CONT_PLAYER_MODIFY_ORDER kCont;
						SPMO kIMO(IMET_MYHOME_AUCTION_TIME,pkPlayer->GetID(),SMOD_MyHome_Modify_Time(siStreetNo,iHouseNo,kNewTime));
						kCont.push_back(kIMO);
						PgAction_ReqModifyItem kItemModifyAction(CIE_GodCmd, GroundKey(), kCont);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}break;
				case GMCMD_MYHOME_TEX_TIME:
					{
						short	siStreetNo = 0;
						int		iHouseNo = 0;

						int iYear = 0;
						int iMon = 0;
						int iDay = 0;

						pkNfy->Pop(siStreetNo);
						pkNfy->Pop(iHouseNo);
						pkNfy->Pop(iYear);
						pkNfy->Pop(iMon);
						pkNfy->Pop(iDay);

						iYear = std::max(iYear - 2000,0);
						BM::PgPackedTime kNewTime;
						kNewTime.Year(iYear);
						kNewTime.Month(iMon);
						kNewTime.Day(iDay);

						CONT_PLAYER_MODIFY_ORDER kCont;
						SPMO kIMO(IMET_MYHOME_TEX_TIME,pkPlayer->GetID(),SMOD_MyHome_Modify_Time(siStreetNo,iHouseNo,kNewTime));
						kCont.push_back(kIMO);
						PgAction_ReqModifyItem kItemModifyAction(CIE_GodCmd, GroundKey(), kCont);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}break;
				case GMCMD_SET_GENTIME:
					{
						SItemPos kItemPos;
						int iYear = 0;
						int iMon = 0;
						int iDay = 0;
						pkNfy->Pop( kItemPos.x );
						pkNfy->Pop( kItemPos.y );
						pkNfy->Pop( iYear);
						pkNfy->Pop( iMon);
						pkNfy->Pop( iDay);

						int const iNewYear = std::max(0,iYear - BM::PgPackedTime::BASE_YEAR);

						BM::PgPackedTime kNewDate;
						kNewDate.Year(iNewYear);
						kNewDate.Month(iMon);
						kNewDate.Day(iDay);

						PgBase_Item kItem;
						if(S_OK == pkPlayer->GetInven()->GetItem(kItemPos,kItem))
						{
							CONT_PLAYER_MODIFY_ORDER kCont;
							SPMO kIMO(IMET_MODIFY_ENCHANT,pkPlayer->GetID(),SPMOD_Enchant(kItem,kItemPos,kItem.EnchantInfo(),kNewDate));
							kCont.push_back(kIMO);
							PgAction_ReqModifyItem kItemModifyAction(CIE_GodCmd, GroundKey(), kCont);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_SET_LIMITTIME:
					{
						SItemPos kItemPos;
						int iTimeType = 0;
						int iUseTime = 0;
						pkNfy->Pop(kItemPos.x);
						pkNfy->Pop(kItemPos.y);
						pkNfy->Pop(iTimeType);
						pkNfy->Pop(iUseTime);

						PgBase_Item kItem;
						if( S_OK == pkPlayer->GetInven()->GetItem( kItemPos ,kItem ) )
						{
							SEnchantInfo kEnchantInfo = kItem.EnchantInfo();
							kEnchantInfo.IsTimeOuted(0);
							kEnchantInfo.TimeType(iTimeType);

							if( 0 < iTimeType )
							{
								kEnchantInfo.IsTimeLimit(1);
								kEnchantInfo.UseTime( std::min<__int64>( static_cast<__int64>(iUseTime), MAX_CASH_ITEM_TIMELIMIT ) );
							}
							else
							{
								kEnchantInfo.IsTimeLimit(0);
								kEnchantInfo.UseTime(0i64);
							}

							BM::DBTIMESTAMP_EX kNewDate;
							g_kEventView.GetLocalTime(kNewDate);

							CONT_PLAYER_MODIFY_ORDER kCont;
							SPMO kIMO( IMET_MODIFY_ENCHANT, pkPlayer->GetID(), SPMOD_Enchant( kItem, kItemPos, kEnchantInfo, static_cast<BM::PgPackedTime>(kNewDate) ));
							kCont.push_back(kIMO);

							PgAction_ReqModifyItem kItemModifyAction(CIE_GodCmd, GroundKey(), kCont);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_OXQUIZ_OPEN:
					{
						BM::Stream kPacket(PT_M_I_GMCMD_OXQUIZ_OPEN);
						g_kProcessCfg.ServerIdentity().WriteToPacket(kPacket);
						kPacket.Push(GroundKey());
						kPacket.Push(*pkNfy);
						SendToOXQuizEvent(kPacket);
					}break;
				case GMCMD_OXQUIZ_STEP:
					{
						BM::Stream kPacket(PT_M_I_GMCMD_OXQUIZ_STEP);
						g_kProcessCfg.ServerIdentity().WriteToPacket(kPacket);
						kPacket.Push(GroundKey());
						kPacket.Push(*pkNfy);
						SendToOXQuizEvent(kPacket);
					}break;
				case GMCMD_LUCKYSTAR_OPEN:
					{
						BM::Stream kPacket(PT_M_I_GMCMD_LUCKYSTAR_OPEN);
						kPacket.Push(pkPlayer->GetID());
						kPacket.Push(*pkNfy);
						SendToLuckyStarEvent(kPacket);
					}break;
				case GMCMD_LUCKYSTAR_STEP:
					{
						BM::Stream kPacket(PT_M_I_GMCMD_LUCKYSTAR_STEP);
						kPacket.Push(pkPlayer->GetID());
						kPacket.Push(*pkNfy);
						SendToLuckyStarEvent(kPacket);
					}break;
				case GMCMD_SERVER_TIME:
					{
						bool bRet = false;
						int iTimeType = 0;
						pkNfy->Pop( iTimeType );

						switch( iTimeType )
						{
						case 0:// 일반적인 LocalTime(OS시간)
							{
								tm kServerTime;
								__time64_t lTime;
								_time64( &lTime );
								errno_t iErr = _localtime64_s( &kServerTime, &lTime );

								if( iErr )
								{
									break;
								}

								BM::Stream kPacket( PT_M_C_GMCMD_SERVERTIME );
								kPacket.Push( iTimeType );
								kPacket.Push( kServerTime.tm_hour );
								kPacket.Push( kServerTime.tm_min );
								kPacket.Push( kServerTime.tm_sec );
								pkPlayer->Send( kPacket );

							}break;
						case 1:// 동기화 시간
							{	
								SYSTEMTIME kSystemTime;

								bRet = g_kEventView.GetLocalTime( &kSystemTime );
								if( !bRet )
								{
									break;
								}

								BM::Stream kPacket( PT_M_C_GMCMD_SERVERTIME );
								kPacket.Push( iTimeType );
								kPacket.Push( kSystemTime.wHour );
								kPacket.Push( kSystemTime.wMinute );
								kPacket.Push( kSystemTime.wSecond );
								pkPlayer->Send( kPacket );
							}break;						
						}		
					}break;
				case GMCMD_GODHAND:
					{
						bool bSet = false;
						pkNfy->Pop( bSet );

						if( bSet )
						{
							pkPlayer->SetAbil(AT_GM_GODHAND, 1);
							BM::Stream kPacket( PT_M_C_GMCMD_GODHAND_ON );
							pkPlayer->Send( kPacket );
						}
						else
						{
							pkPlayer->SetAbil(AT_GM_GODHAND, 0);							
							BM::Stream kPacket( PT_M_C_GMCMD_GODHAND_OFF );
							pkPlayer->Send( kPacket );
						}
					}break;
				case GMCMD_CLEAR_INV:
					{
						BYTE bInv = 0;
						BYTE bPos = 0;
						pkNfy->Pop( bInv );
						pkNfy->Pop( bPos );

						PgBase_Item kItem;
						if(S_OK == pkPlayer->GetInven()->GetItem(SItemPos(bInv,bPos),kItem))
						{
							CONT_PLAYER_MODIFY_ORDER kCont;
							SPMOD_Modify_Count kDelData(kItem,SItemPos(bInv,bPos),0,true);
							SPMO kIMO(IMET_MODIFY_COUNT, pkPlayer->GetID(), kDelData);
							kCont.push_back(kIMO);
							PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, GroundKey(), kCont);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_SET_ACHIEVEMENT_TIMELIMIT:
					{
						int iSaveIdx = 0;
						int iUseTime = 0;
						pkNfy->Pop( iSaveIdx );
						pkNfy->Pop( iUseTime );
						CONT_PLAYER_MODIFY_ORDER kCont;
						kCont.push_back(SPMO(IMET_SET_ACHIEVEMENT_TIMELIMIT,pkPlayer->GetID(),SOD_ModifyAchievementTimeLimit(iSaveIdx, iUseTime)));
						PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, GroundKey(), kCont);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}break;
				case GMCMD_COMPLETE_ACHIEVEMENT:
					{
						int iSaveIdx = 0;
						pkNfy->Pop( iSaveIdx );

						CONT_DEF_ACHIEVEMENTS_SAVEIDX const * pkCont = NULL;
						g_kTblDataMgr.GetContDef(pkCont);
						if(pkCont)
						{
							CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator iter = pkCont->find(iSaveIdx);
							if(iter != pkCont->end())
							{
								CONT_PLAYER_MODIFY_ORDER kCont;
								kCont.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT,pkPlayer->GetID(),SPMOD_Complete_Achievement(iSaveIdx,(*iter).second.iCategory,(*iter).second.iRankPoint ,(*iter).second.iUseTime, (*iter).second.iGroupNo)));
								PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, GroundKey(), kCont);
								kItemModifyAction.DoAction(pkPlayer, NULL);
							}
						}
					}break;
				case GMCMD_SET_ACHIEVEMENT:
					{
						int iSaveIdx = 0;
						BYTE bValue = 0;
						pkNfy->Pop( iSaveIdx );
						pkNfy->Pop( bValue );
						CONT_PLAYER_MODIFY_ORDER kCont;
						kCont.push_back(SPMO(IMET_SET_ACHIEVEMENT,pkPlayer->GetID(),SPlayerModifyOrderData_ModifyAchievement(iSaveIdx,bValue)));
						PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, GroundKey(), kCont);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}break;
				case GMCMD_GIVEITEM:
					{
						int iItemNo = 0;
						int iCount = 0;
						int iRarityControl = 0;
						int isCurse = 0;
						int isSeal = 0;

						pkNfy->Pop( iItemNo );
						pkNfy->Pop( iCount );
						pkNfy->Pop( iRarityControl );
						pkNfy->Pop( isCurse );
						pkNfy->Pop( isSeal );

						if ( iCount > 0 )
						{
							CONT_ITEM_CREATE_ORDER kOrderList;
							PgBase_Item kItem;
							if(SUCCEEDED( ::CreateSItem(iItemNo, iCount, iRarityControl, kItem) ))
							{
								SEnchantInfo kEnchantInfo = kItem.EnchantInfo();
								kEnchantInfo.IsCurse((isCurse)?true:false);//저주, 봉인은 강제 컨트롤.
								kEnchantInfo.IsSeal((isSeal)?true:false);//

								kItem.EnchantInfo(kEnchantInfo);

								if( PushBSInvenItem(pkPlayer, kItem) )
								{
									return true;
								}
								else
								{
									kOrderList.push_back(kItem);
									PgAction_CreateItem kCreateAction(CIE_GodCmd, GroundKey(), kOrderList);
									return kCreateAction.DoAction( pkPlayer, NULL );	
								}
							}
						}

						INFO_LOG( BM::LOG_LV0, __FL__<<L"CmdType["<<iCmdType<<L"] Error ItemNo["<<iItemNo<<L"] Count["<<iCount<<L"] -- "<<pkPlayer->Name()<<L" --" );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Wrong iCount!"));
					}break;
				case GMCMD_COPY_THAT_FROM_DB:
					{
						int iClassNo = 0, iLevel = 0;
						SPlayerStrategySkillData kData;
						PgInventory kTempInven;

						pkNfy->Pop( iClassNo );
						pkNfy->Pop( iLevel );
						pkNfy->Pop( kData );
						kTempInven.ReadFromPacket( *pkNfy, WT_DEFAULT );

						// 재귀 호출로, 직업 레벨 변경(코드 재활용)
						{
							BM::Stream kChangeClassPacket;
							bool const bGod = false; // 다른 방법으로 스킬을 초기화
							kChangeClassPacket.Push( iClassNo );
							kChangeClassPacket.Push( iLevel );
							kChangeClassPacket.Push( bGod );
							GMCommand(pkPlayer, GMCMD_CLASSCHANGE, &kChangeClassPacket);
						}
						// 복사 아이템 지급
						{
							CONT_ITEM_CREATE_ORDER kOrderList;

							CONT_ITEM_CREATE_ORDER kContItem;
							kTempInven.GetItems(IT_FIT, kContItem); // 아이템을 얻고
							kTempInven.GetItems(IT_FIT_CASH, kContItem);
							kTempInven.GetItems(IT_FIT_COSTUME, kContItem);
							GmCommandUtil::CopyItem(kOrderList, kContItem); // 복사 아이템 생성

							if( 0 < kOrderList.size() )
							{
								PgAction_CreateItem kCreateAction(CIE_GodCmd, GroundKey(), kOrderList);
								kCreateAction.DoAction( pkPlayer, NULL );
							}
						}
						// 스킬 복사
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;

							SPMO kSPMO(IMET_GM_INIT_SKILL, pkPlayer->GetID());
							kSPMO.m_kPacket.Push( kData );
							kOrder.push_back( kSPMO );

							if( 0 < kOrder.size() )
							{
								PgAction_ReqModifyItem kItemModifyAction(IMEPT_GODCMD, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkPlayer, NULL);
							}
						}
					}break;
				case GMCMD_COPY_THAT:
					{
						std::wstring kCharName;
						pkNfy->Pop( kCharName );

						CUnit* pkTargetUnit = GetPlayer(kCharName);
						if( pkTargetUnit )
						{
							PgPlayer* pkTargetPlayer = dynamic_cast< PgPlayer* >(pkTargetUnit);
							if( pkTargetPlayer )
							{
								//
								BM::Stream kNfyPacket;
								// Class Level
								kNfyPacket.Push( pkTargetPlayer->GetAbil(AT_CLASS) );
								kNfyPacket.Push( pkTargetPlayer->GetAbil(AT_LEVEL) );

								// Skill
								SPlayerStrategySkillData kData;
								PgMySkill* pkSkill = pkTargetPlayer->GetMySkill();
								if( pkSkill )
								{
									pkSkill->Save(MAX_DB_SKILL_SIZE, kData.abySkills);
									pkSkill->SaveExtend(MAX_DB_SKILL_EXTEND_SIZE, kData.abySkillExtends);
								}
								kNfyPacket.Push( kData );

								// Equip Item
								PgInventory* pkInven = pkTargetPlayer->GetInven();
								if( pkInven )
								{
									pkInven->WriteToPacket( kNfyPacket, WT_DEFAULT );
								}
								else
								{
									PgInventory kTempInven;
									kTempInven.WriteToPacket( kNfyPacket, WT_DEFAULT );
								}
								GMCommand(pkPlayer, GMCMD_COPY_THAT_FROM_DB, &kNfyPacket); // 재귀 호출로 실제 처리
							}
						}
						else
						{
							BM::Stream kPacket(PT_A_N_REQ_COPY_THAT); // DB 갔다 와서 (GMCMD_COPY_THAT_FROM_DB 처리)
							kPacket.Push( pkPlayer->GetID() );
							kPacket.Push( kCharName );
							::SendToContents(kPacket);
						}
					}break;
				case GMCMD_LIMIT_HP:
					{
						int iLimitHP = 0;

						pkNfy->Pop(iLimitHP);

						iLimitHP = std::min(pkPlayer->GetAbil(AT_MAX_HP), iLimitHP);
						pkPlayer->SetAbil(AT_GMCMD_LIMIT_HP, iLimitHP);

						int const iCurHP = pkPlayer->GetAbil(AT_HP);
						if( iLimitHP > iCurHP )
						{
							pkPlayer->SetAbil(AT_HP, iLimitHP, true, true);
						}
					}break;

				case GMCMD_TELEPORT_TO_MON:
					{
						int iClassNo = 0;
						int iCount = 0;

						pkNfy->Pop( iClassNo );
						pkNfy->Pop( iCount );

						int iCurCount = 0;
						bool bFind = false;
						CUnit* pkMonster = NULL;
						CONT_OBJECT_MGR_UNIT::iterator kItor;
						PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
						while ((pkMonster = PgObjectMgr::GetNextUnit(UT_MONSTER, kItor)) != NULL)
						{
							++iCurCount;
							bFind = (pkMonster->GetAbil(AT_CLASS) == iClassNo) && (iCount == iCurCount) ;
							if( bFind)
							{
								POINT3 kPos = pkMonster->GetPos();
								SendToPosLoc( pkPlayer, kPos, MMET_Normal );
								break;
							}
						}
						if( !bFind )
						{
							int const iCantFindMonsterMsg = 20025;
							pkPlayer->SendWarnMessage(iCantFindMonsterMsg);
						}
					}break;

				case GMCMD_GIVEITEMSET:
					{
						int iItemSetNo = 0;
						pkNfy->Pop( iItemSetNo );

						std::vector<int> kVector;
						kVector.reserve(11);
						switch ( iItemSetNo )
						{
						case 2:
							{
								kVector.push_back(20106002);
								kVector.push_back(20106014);
								kVector.push_back(20120001);
								kVector.push_back(20121001);
								kVector.push_back(20120001);
								kVector.push_back(20122001);
								kVector.push_back(20123001);
								kVector.push_back(20124001);
							}break;
						case 3:
							{
								kVector.push_back(20106001);
								kVector.push_back(20120002);
								kVector.push_back(20121002);
								kVector.push_back(20122002);
								kVector.push_back(20123002);
								kVector.push_back(20124002);
								kVector.push_back(20103001);
							}break;
						case 4:
							{
								kVector.push_back(20106003);
								kVector.push_back(20106004);
								kVector.push_back(20120003);
								kVector.push_back(20121003);
								kVector.push_back(20122003);
								kVector.push_back(20123003);
								kVector.push_back(20124003);
								kVector.push_back(20103002);
							}break;
						case 5:
							{
								kVector.push_back(20106007);
								kVector.push_back(20106008);
								kVector.push_back(20120005);
								kVector.push_back(20121005);
								kVector.push_back(20122005);
								kVector.push_back(20123005);
								kVector.push_back(20124005);
								kVector.push_back(20103004);
							}break;
						case 6:
							{
								kVector.push_back(20106006);
								kVector.push_back(20106005);
								kVector.push_back(20120004);
								kVector.push_back(20121004);
								kVector.push_back(20122004);
								kVector.push_back(20123004);
								kVector.push_back(20124004);
								kVector.push_back(20103003);
							}break;
						case 7:
							{
								kVector.push_back(20106009);
								kVector.push_back(20106040);
								kVector.push_back(20120006);
								kVector.push_back(20121006);
								kVector.push_back(20122006);
								kVector.push_back(20123006);
								kVector.push_back(20124006);
								kVector.push_back(20103005);
							}break;
						case 8:
							{
								kVector.push_back(20106012);
								kVector.push_back(20106015);
								kVector.push_back(20120007);
								kVector.push_back(20121007);
								kVector.push_back(20122007);
								kVector.push_back(20123007);
								kVector.push_back(20124007);
							}break;
						case 9:
							{
								kVector.push_back(20106010);
								kVector.push_back(20120008);
								kVector.push_back(20121008);
								kVector.push_back(20122008);
								kVector.push_back(20123008);
								kVector.push_back(20124008);
								kVector.push_back(20103006);
							}break;
						case 10:
							{
								kVector.push_back(20106011);
								kVector.push_back(20106019);
								kVector.push_back(20120009);
								kVector.push_back(20121009);
								kVector.push_back(20122009);
								kVector.push_back(20123009);
								kVector.push_back(20124009);
								kVector.push_back(20103007);
							}break;
						case 11:
							{
								kVector.push_back(20106022);
								kVector.push_back(20106023);
								kVector.push_back(20120011);
								kVector.push_back(20121011);
								kVector.push_back(20122011);
								kVector.push_back(20123011);
								kVector.push_back(20124011);
								kVector.push_back(20103009);
							}break;
						case 12:
							{
								kVector.push_back(20106020);
								kVector.push_back(20106021);
								kVector.push_back(20120010);
								kVector.push_back(20121010);
								kVector.push_back(20122010);
								kVector.push_back(20123010);
								kVector.push_back(20124010);
								kVector.push_back(20103008);
							}break;
						case 13:
							{
								kVector.push_back(20106024);
								kVector.push_back(20106025);
								kVector.push_back(20120012);
								kVector.push_back(20121012);
								kVector.push_back(20122012);
								kVector.push_back(20123012);
								kVector.push_back(20124012);
								kVector.push_back(20103010);
							}break;
						case 14:
							{
								kVector.push_back(20106016);
								kVector.push_back(20106017);
								kVector.push_back(20120013);
								kVector.push_back(20121013);
								kVector.push_back(20122013);
								kVector.push_back(20122013);
								kVector.push_back(20123013);
								kVector.push_back(20124013);
							}break;
						case 15:
							{
								kVector.push_back(20106026);
								kVector.push_back(20120014);
								kVector.push_back(20121014);
								kVector.push_back(20122014);
								kVector.push_back(20123014);
								kVector.push_back(20124014);
								kVector.push_back(20103011);
							}break;
						case 16:
							{
								kVector.push_back(20106027);
								kVector.push_back(20106028);
								kVector.push_back(20120015);
								kVector.push_back(20121015);
								kVector.push_back(20122015);
								kVector.push_back(20123015);
								kVector.push_back(20124015);
								kVector.push_back(20103012);
							}break;
						case 17:
							{
								kVector.push_back(20106031);
								kVector.push_back(20106032);
								kVector.push_back(20120017);
								kVector.push_back(20121017);
								kVector.push_back(20122017);
								kVector.push_back(20123017);
								kVector.push_back(20124017);
								kVector.push_back(20103014);
							}break;
						case 18:
							{
								kVector.push_back(20106029);
								kVector.push_back(20106030);
								kVector.push_back(20120016);
								kVector.push_back(20121016);
								kVector.push_back(20122016);
								kVector.push_back(20123016);
								kVector.push_back(20124016);
								kVector.push_back(20103013);
							}break;
						case 19:
							{
								kVector.push_back(20106033);
								kVector.push_back(20106034);
								kVector.push_back(20120018);
								kVector.push_back(20121018);
								kVector.push_back(20122018);
								kVector.push_back(20123018);
								kVector.push_back(20124018);
								kVector.push_back(20103015);
							}break;
						case 20:
							{
								kVector.push_back(20106013);
								kVector.push_back(20106018);
								kVector.push_back(20120019);
								kVector.push_back(20121019);
								kVector.push_back(20122019);
								kVector.push_back(20123019);
								kVector.push_back(20124019);
							}break;
						case 21:
							{
								kVector.push_back(20106035);
								kVector.push_back(20120020);
								kVector.push_back(20121020);
								kVector.push_back(20122020);
								kVector.push_back(20123020);
								kVector.push_back(20124020);
								kVector.push_back(20124018);
							}break;
						case 22:
							{
								kVector.push_back(20106036);
								kVector.push_back(20106037);
								kVector.push_back(20120021);
								kVector.push_back(20121021);
								kVector.push_back(20122021);
								kVector.push_back(20123021);
								kVector.push_back(20124021);
								kVector.push_back(20103017);
							}break;
						case 23:
							{
								kVector.push_back(20106041);
								kVector.push_back(20106042);
								kVector.push_back(20120023);
								kVector.push_back(20121023);
								kVector.push_back(20122023);
								kVector.push_back(20123023);
								kVector.push_back(20124023);
								kVector.push_back(20103019);
							}break;
						case 24:
							{
								kVector.push_back(20106038);
								kVector.push_back(20106039);
								kVector.push_back(20120022);
								kVector.push_back(20121022);
								kVector.push_back(20122022);
								kVector.push_back(20123022);
								kVector.push_back(20124022);
								kVector.push_back(20103018);
							}break;
						case 25:
							{
								kVector.push_back(20106043);
								kVector.push_back(20106044);
								kVector.push_back(20120024);
								kVector.push_back(20121024);
								kVector.push_back(20122024);
								kVector.push_back(20123024);
								kVector.push_back(20124024);
								kVector.push_back(20103020);
							}break;
						}

						kVector.push_back(20200002);	// 회복물약(소)
						kVector.push_back(20200003);	// 고급회복물약(대)
						kVector.push_back(20200005);	// 마나물약(소)

						CONT_ITEM_CREATE_ORDER kOrderList;
						std::vector<int>::iterator no_itr =  kVector.begin();
						for ( ;no_itr != kVector.end() ; ++no_itr )
						{
							PgBase_Item kItem;
							if( S_OK == CreateSItem(*no_itr, 1, GIOT_NONE, kItem ) )
							{
								kOrderList.push_back(kItem);
							}
						}

						PgAction_CreateItem kCreateAction( CIE_GodCmd, GroundKey(), kOrderList);
						return kCreateAction.DoAction(pkPlayer, NULL);
					}break;

				case GMCMD_SETITEMCOUNT:
					{
						short sInvType = 0;
						short sInvPos = 0;
						int iCount = 0;
						pkNfy->Pop( sInvType );
						pkNfy->Pop( sInvPos );
						pkNfy->Pop( iCount );

						SItemPos const kItemPos((BYTE)sInvType, (BYTE)sInvPos);
						PgInventory *pkInv = pkPlayer->GetInven();
						if ( pkInv )
						{
							PgBase_Item kItem;
							if ( SUCCEEDED(pkInv->GetItem(kItemPos, kItem)) )
							{
								GET_DEF(CItemDefMgr, kItemDefMgr);
								CItemDef const *pkItemDef = kItemDefMgr.GetDef( kItem.ItemNo() );
								if ( !pkItemDef )
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot get User Item Def UserID["<<pkPlayer->Name()<<L"], ItemID["<<kItem.ItemNo()<<L"]");
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}

								bool bAmount = pkItemDef->IsAmountItem();
								int const iMaxCount = ((bAmount) ? pkItemDef->MaxAmount() : pkItemDef->MaxAmount());
								iCount = __max(0, __min(iMaxCount, iCount));	// 0 <= iCount <= iMaxCount
								int const iCurrentCount = kItem.Count();
								int const iDiff = iCount - iCurrentCount;
								if (iDiff == 0)
								{
									INFO_LOG(BM::LOG_LV0, __FL__<<L"Item Count value is same with source item count COUNT["<<iCurrentCount<<L"]");
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}

								SPMOD_Modify_Count kDelData(kItem, kItemPos, iDiff);
								SPMO kIMO(IMET_MODIFY_COUNT, pkPlayer->GetID(), kDelData);

								CONT_PLAYER_MODIFY_ORDER kOrder;
								kOrder.push_back(kIMO);

								PgAction_ReqModifyItem kItemModifyAction(CIE_CallGodCmd, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkPlayer, NULL);
							}
						}
					}break;
				case GMCMD_CLASSCHANGE:
					{
						int iClass = 0;
						int iLevel = 0;
						bool bGod = true;
						pkNfy->Pop( iClass );
						pkNfy->Pop( iLevel );
						pkNfy->Pop( bGod );

						SClassKey kNewClassLvKey(iClass, iLevel);

						GET_DEF( PgClassDefMgr, kClassDefMgr);
						int iGetAbilRet = kClassDefMgr.GetAbil( kNewClassLvKey, AT_MAX_HP );
						if( iGetAbilRet > 0 )// 해당클래스에 맞는 스탯찍기
						{
							PLAYER_LEVELUP_INFO kInfo;
							
							if( SUCCEEDED( kClassDefMgr.GetAbil( kNewClassLvKey, kInfo.kBasic ) ) )
							{
								__int64 const iExpValue64 = kClassDefMgr.GetAbil64( kNewClassLvKey, AT_EXPERIENCE );

								CONT_PLAYER_MODIFY_ORDER kOrder;

								SPMOD_AddAbil kSetExpData(AT_EXPERIENCE, iExpValue64);
								kOrder.push_back(SPMO(IMET_SET_ABIL64, pkPlayer->GetID(), kSetExpData));

								if ( pkPlayer->GetAbil(AT_LEVEL) > iLevel )
								{
									SPMOD_AddAbil kSetLvData(AT_LEVEL, iLevel);
									kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), kSetLvData));
								}

								SPMOD_AddAbil kSetClassData(AT_CLASS, iClass);
								kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), kSetClassData));

								EItemModifyParentEventType kCallType = CIE_ChangeClass_ByNormal;
								
								if(bGod)
								{
									kCallType = CIE_ChangeClass_ByGMCmd;
								}

								PgAction_ReqModifyItem kItemModifyAction(kCallType, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkPlayer, NULL);
								//RefreshGroundQuestInfo(pkPlayer);
								return true;
							}
						}
						INFO_LOG(BM::LOG_LV4, __FL__<<L"Error ClassDef Class["<<iClass<<L"] Level["<<iLevel<<L"] -- "<<pkPlayer->Name()<<L" --" );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Wrong iGetAbilRet!"));
					}break;
				case GMCMD_SKILLPOINT:
					{
						short sNewPoint = 0;
						pkNfy->Pop( sNewPoint );

						CONT_PLAYER_MODIFY_ORDER kOrder;

						SPMOD_AddAbil kAddAbilData(AT_SP, sNewPoint);
						kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), kAddAbilData));

						PgAction_ReqModifyItem kItemModifyAction(IMEPT_GODCMD, GroundKey(), kOrder);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}break;
				case GMCMD_GMABIL:
					{
						int iAbil = 0;
						BYTE byAdd = 0;
						pkNfy->Pop( iAbil );
						pkNfy->Pop( byAdd );

						GMABIL_TYPE const kGMAbilValue = static_cast<GMABIL_TYPE>(iAbil);
						bool const bAdd = (byAdd > 0);

						pkPlayer->GmAbilAdd( kGMAbilValue, bAdd );

						if ( GMABIL_HIDDEN & kGMAbilValue )
						{
							if ( true == bAdd )
							{
								pkPlayer->SetSyncType( SYNC_TYPE_RECV_ADD );// AddUnit 받기만 한다.
							}
							else
							{
								pkPlayer->SetSyncType( SYNC_TYPE_DEFAULT );// Default로 되돌린다.
							}	
						}

						if ( GMABIL_CANNOT_DAMAGE & kGMAbilValue )
						{
							pkPlayer->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_GM, bAdd, E_SENDTYPE_NONE );
						}
					}break;

				case GMCMD_HEALME:
					{
						if( pkPlayer->IsDead() )
						{
							if( GetAttr() & GATTR_EVENT_GROUND )
							{
								PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(this);
								if( pStaticGround && (pStaticGround->GetEventGroundState() & EGS_PLAY) )
								{
									pStaticGround->RevivePlayer();
								}
							}
						}

						pkPlayer->Alive( EALIVE_PVP, E_SENDTYPE_BROADALL );

						if( pkPlayer->HaveExpedition() )	// 원정대에 가입되어 있다면, 자신의 상태 변화를 알려야 함.
						{
							BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
							Packet.Push(pkPlayer->ExpeditionGuid());
							Packet.Push(pkPlayer->GetID());
							Packet.Push(pkPlayer->IsAlive());
							::SendToGlobalPartyMgr(Packet);
						}

						PgDeathPenalty kAction(GroundKey(), LURT_GMCommand);
						kAction.DoAction(pkPlayer, NULL);
					}break;
				case GMCMD_QUEST:
					{
						short sStartID = 0;
						short sEndID = 0;
						bool bComplete = false, bForceRebuild = false;
						pkNfy->Pop( sStartID );
						pkNfy->Pop( sEndID );
						pkNfy->Pop( bComplete );
						pkNfy->Pop( bForceRebuild );

						CONT_PLAYER_MODIFY_ORDER kOrder;

						ContQuestID kVec;
						for( ; sStartID<=sEndID; ++sStartID )
						{
							SPMOD_SetEndQuest kAddQuestData(sStartID, bComplete);
							kOrder.push_back(SPMO(IMET_ADD_ENDQUEST, pkPlayer->GetID(), kAddQuestData));
						}
						if( bForceRebuild )
						{
							kOrder.push_back( SPMO(IMET_BUILD_DAYLOOP_FORCE, pkPlayer->GetID()) );
						}
						else
						{
							kOrder.push_back( SPMO(IMET_BUILD_DAYLOOP, pkPlayer->GetID()) );
						}

						PgAction_ReqModifyItem kItemModifyAction(IMEPT_GODCMD, GroundKey(), kOrder);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}break;

				case GMCMD_GMABIL64:
					{
						WORD wType = 0;
						__int64 i64Value = 0i64;
						pkNfy->Pop( wType );
						pkNfy->Pop( i64Value );

						CONT_PLAYER_MODIFY_ORDER kOrder;
						switch( wType )
						{
						case AT_EXPERIENCE:
							{
								SPMOD_AddAbil kModifyAbilData(AT_EXPERIENCE, i64Value);
								kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkPlayer->GetID(), kModifyAbilData));
							}break;
						case AT_MONEY:
							{
								SPMOD_Add_Money kModifyMoneyData(i64Value);
								kOrder.push_back(SPMO(IMET_ADD_MONEY, pkPlayer->GetID(), kModifyMoneyData));
							}break;
						case AT_FRAN:
							{
								SPMOD_AddAbil kModifyFranData(AT_FRAN, i64Value);
								kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkPlayer->GetID(), kModifyFranData));
							}break;
						}

						if( !kOrder.empty() )
						{
							PgAction_ReqModifyItem kItemModifyAction(MCE_GodCmd, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;

				case GMCMD_DROPITEM:
					{
						int iItemNo = 0;
						int iMoney = 0;

						pkNfy->Pop( iItemNo );
						pkNfy->Pop( iMoney );

						PgCreateSpreadPos kPosAction(pkPlayer->GetPos());
						POINT3BY OrientedBy = pkPlayer->GetOrientedVector();
						POINT3 Oriented(OrientedBy.x, OrientedBy.y, OrientedBy.z);
						Oriented.Normalize();
						kPosAction.AddDir( PhysXScene()->GetPhysXScene(), Oriented, 100 );

						VEC_GUID kOwnerVec;
						kOwnerVec.push_back( pkPlayer->GetID() );

						if ( iItemNo )
						{
							PgBase_Item kDropItem;
							if( SUCCEEDED( ::CreateSItem( iItemNo, 1, GIOT_FIELD, kDropItem)) )
							{
								POINT3 kDropPos;
								kPosAction.PopPos( kDropPos );

								InsertItemBox( kDropPos, kOwnerVec, pkPlayer, kDropItem, 0i64, kLogCont );
							}
							else
							{
								INFO_LOG( BM::LOG_LV0, __FL__<<L"CmdType["<<iCmdType<<L"] Error ItemNo["<<iItemNo<<L"] -- "<<pkPlayer->Name()<<L" --" );
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Wrong iCount!"));
							}
						}

						if ( 0 < iMoney )
						{
							POINT3 kDropPos;
							kPosAction.PopPos( kDropPos );

							InsertItemBox( kDropPos, kOwnerVec, pkPlayer, PgBase_Item::NullData(), static_cast<__int64>(iMoney), kLogCont );
						}
					}break;

				case GMCMD_MAPMOVE:
					{						
						int iGroundNo = 0;
						short nTargetPortal = 0;
						pkNfy->Pop( iGroundNo );
						pkNfy->Pop( nTargetPortal );

						CONT_DEFMAP const *pkContDefMap = NULL;
						g_kTblDataMgr.GetContDef( pkContDefMap );
						if ( pkContDefMap )
						{
							PgMapMoveChecker kMapMoveChecker( *pkContDefMap );
							EPartyMoveType const kMapMoveType = kMapMoveChecker.GetMoveType( iGroundNo );
							if ( E_MOVE_LOCK != kMapMoveType )
							{
								EMapMoveEventType eType = MMET_None;
								if( 0 != (GetAttr() & GATTR_BATTLESQUARE) )
								{
									eType = MMET_BackToChannel;
								}
								{
									CONT_DEFMAP::const_iterator find_iter = pkContDefMap->find(iGroundNo);
									if( pkContDefMap->end() != find_iter )
									{
										if( 0 != ((*find_iter).second.iAttr & GATTR_BATTLESQUARE) )
										{
											eType = MMET_BATTLESQUARE;
										}
									}
								}

								SReqMapMove_MT kRMM(eType);
								kRMM.kTargetKey.GroundNo(iGroundNo);
								kRMM.nTargetPortal = nTargetPortal;

								PgReqMapMove kMapMove( this, kRMM, NULL );
								if ( kMapMove.Add( pkPlayer ) )
								{
									if ( E_MOVE_PARTYMASTER & kMapMoveType )
									{
										// 인던인 경우는.. 파티원도 다 넣어주어야지
										if ( pkPlayer->HaveParty() )
										{
											this->AddPartyMember( pkPlayer, kMapMove );
										}
									}
									else if( E_MOVE_ANY_PARTYMEMMBER & kMapMoveType )
									{
										if ( pkPlayer->HaveParty() )
										{
											this->AddAnyPartyMember( pkPlayer, kMapMove );											
										}
									}
									kMapMove.DoAction();
								}
							}
							else
							{
								pkPlayer->SendWarnMessage( 1 );
							}
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkcontDefMap is NULL"));
						}
					}break;

				case GMCMD_RESET_MONEY:
					{
						__int64 i64NewMoney = 0i64;
						pkNfy->Pop(i64NewMoney);
						__int64 const i64Money = pkPlayer->GetAbil64(AT_MONEY);
						i64NewMoney-=i64Money;
						PgAction_ReqAddMoney kAction(IMEPT_GODCMD, i64NewMoney, GroundKey());
						kAction.DoAction(pkPlayer, NULL);
					}break;

				case GMCMD_NOTICE_ALL:
					{
						std::wstring wstrNotice;
						bool bSendToAllChannel = false;
						pkNfy->Pop( wstrNotice );
						pkNfy->Pop( bSendToAllChannel );

						short const nChannelNo = ( true == bSendToAllChannel ? ALL_CHANNEL_NUM : g_kProcessCfg.ChannelNo() );

						BM::Stream kPacket(PT_GM_A_REQ_GMCOMMAND, EGMC_NOTICE);
						kPacket.Push(pkPlayer->GetID());
						kPacket.Push(nChannelNo);
						kPacket.Push(wstrNotice);
						SendToContents(kPacket);
					}break;

				case GMCMD_BLOCKUSER:
					{
						std::wstring wstrName;
						int iBlockState = 0;
						int iEndYear = 1901;
						int iEndMonth = 0;
						int iEndDay = 0;

						pkNfy->Pop( wstrName );
						pkNfy->Pop( iBlockState );
						pkNfy->Pop( iEndYear );
						pkNfy->Pop( iEndMonth );
						pkNfy->Pop( iEndDay );

						BM::DBTIMESTAMP_EX kStart, kEnd;
						kStart.SetLocalTime();
						kEnd.year = iEndYear;
						kEnd.month = iEndMonth;
						kEnd.day = iEndDay;

						BM::GUID kGuid;
						PgPlayer* pkBlockPlayer = dynamic_cast<PgPlayer*>(GetPlayer(BM::vstring::ConvToMultiByte(wstrName).c_str()));
						if( pkBlockPlayer )
						{// 접속 한 유저라면, 멤버 GUID를 넣고..(이걸로 Kick한다)
							kGuid = pkBlockPlayer->GetMemberGUID();
						}
						else
						{// 접속 안했다면 NULL GUID 넣어주자.
							kGuid = BM::GUID::NullData();
						}

						BM::Stream kPacket( PT_GM_A_REQ_GMCOMMAND, EGMC_FREEZEACCOUNT );
						kPacket.Push(kGuid);// 멤버 GUID
						kPacket.Push(ECK_GMC);
						kPacket.Push(wstrName);// 계정명
						kPacket.Push(iBlockState);// 블럭 값. 100이상이면 블럭임.
						kPacket.Push(kStart);// 블럭 시작시간 (현재부터)
						kPacket.Push(kEnd);// X년 X월 X일 까지 블럭.
						SendToContents(kPacket);
					}break;

				case GMCMD_QUESTPARAM:
					{
						int iQuestID = 0;
						int iParamNo = 0;
						int iCount = 0;
						pkNfy->Pop( iQuestID );
						pkNfy->Pop( iParamNo );
						pkNfy->Pop( iCount );

						PgAction_SetQuestParam kSetParamAction(GroundKey(), iQuestID, iParamNo, iCount);
						kSetParamAction.DoAction( pkPlayer, NULL );
					}break;

				case GMCMD_SET_REALM_QUEST:
					{
						std::wstring kRealmQuestID;
						size_t iCount = 0;
						pkNfy->Pop( kRealmQuestID );
						pkNfy->Pop( iCount );

						BM::Stream kPacket(PT_N_N_REQ_REALM_QUEST_ADD_COUNT);
						kPacket.Push( pkPlayer->GetID() );
						kPacket.Push( kRealmQuestID );
						kPacket.Push( iCount );
						::SendToRealmContents(PMET_REALM_EVENT, kPacket);
					}break;

				case GMCMD_SPEEDCHANGE:
					{
						int iSpeed = 0;
						pkNfy->Pop( iSpeed );

						pkPlayer->SetAbil(AT_MOVESPEED, iSpeed, true, true);
						pkPlayer->SetAbil(AT_C_MOVESPEED, iSpeed, true, true);
					}break;

				case GMCMD_COOLTIMEINIT:
					{
						if ( pkPlayer->GetInven() )
						{
							pkPlayer->GetInven()->ClearCoolTime();
						}

						PgPet *pkPet = GetPet( pkPlayer );
						if(pkPet && pkPet->GetSkill())
						{
							pkPet->GetSkill()->ClearCoolTime();
						}

						CSkill *pkSkill = pkPlayer->GetSkill();
						if ( pkSkill )
						{
							pkSkill->ClearCoolTime();
							BM::Stream kPacket( PT_M_C_NFY_COOLTIME_INIT, pkPlayer->GetID() );
							pkPlayer->Send( kPacket, E_SENDTYPE_SELF );
						}
					}break;
				case GMCMD_KILLUNIT:
					{
						BM::GUID kGuid;
						pkNfy->Pop( kGuid );
						if ( BM::GUID::IsNull( kGuid ) )
						{
							RemoveAllMonster(true, dynamic_cast<CUnit*>(pkPlayer) );

							bool bNoRegen = false;
							pkNfy->Pop( bNoRegen );
							if ( bNoRegen )
							{
								m_kContGenPoint_Monster.clear();
							}
						}
						else
						{
							CUnit *pkUnit = GetUnit( kGuid );
							if ( pkUnit )
							{
								switch( pkUnit->UnitType() )
								{
								case UT_PET:
									{
									}break;
								default:
									{
										::OnDamaged(NULL, pkUnit, 0, pkUnit->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());
									}break;
								}
							}				
						}
					}break;

				case GMCMD_ADDMONSTER:
					{
						enum EGMAddMonType
						{
							GMAMT_Monster		= 0,
							GMAMT_Object		= 1,
							GMAMT_MonGenGroup	= 2,
							GMAMT_SummonNPC		= 3,
						};

						int iMonsterNo = 0;
						int iMonsterCount = 1;
						int iType = 0;
						bool bDropAllItem = false;
						int iEnchantGradeNo = 0;
						pkNfy->Pop( iMonsterNo );
						pkNfy->Pop( iMonsterCount );
						pkNfy->Pop( iType );
						pkNfy->Pop( bDropAllItem );
						pkNfy->Pop( iEnchantGradeNo );
						if(iMonsterCount < 1 )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						TBL_DEF_MAP_REGEN_POINT kRegenInfo;
						kRegenInfo.iMapNo = GetGroundNo();
						kRegenInfo.pt3Pos = pkPlayer->GetPos();
						BM::GUID kMonsterGuid;
						bool bRtn = false;
						for( int i=0; i<iMonsterCount; ++i)
						{
							switch( iType )
							{
							case GMAMT_Monster:
								{
									bRtn = SUCCEEDED(InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid, NULL, bDropAllItem, iEnchantGradeNo));
								}break;
							case GMAMT_Object:
								{
									SObjUnitBaseInfo kTemp(SObjUnitGroupIndex(), iMonsterNo);
									kTemp.pt3Pos = pkPlayer->GetPos();
									bRtn = SUCCEEDED(InsertObjectUnit( kRegenInfo, kTemp, kMonsterGuid ));
								}break;
							case GMAMT_MonGenGroup:
								{
									int const iGenGroup = iMonsterNo;
									ActivateMonsterGenGroup( iGenGroup, false, false );
								}break;
							case GMAMT_SummonNPC:
								{
									SClassKey const kKey(iMonsterNo, 1);

									SCreateSummoned kCreateInfo;
									kCreateInfo.kClassKey.iClass = kKey.iClass;
									kCreateInfo.kClassKey.nLv = kKey.nLv;
									kCreateInfo.bNPC = true;
									kCreateInfo.kGuid.Generate();			// GUID 생성									

									CUnit * pkSummonNpc = CreateSummoned(pkPlayer, &kCreateInfo, L"GM_SummonedNpc", SSummonedMapMoveData::NullData(), pkPlayer->GetPos());
									if(pkSummonNpc)
									{
										bRtn = true;
									}
								}break;
							}
							if(bRtn == false)
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}
						}
						return true;
					}break;

				case GMCMD_MUTECHAT:
					{
						std::wstring wstrName;
						int iMinTime = 0;
						pkNfy->Pop( wstrName );
						pkNfy->Pop( iMinTime );
						iMinTime = std::min( iMinTime, 24 * 366 );// 1년이상 설정안되게		

						int const iSecTime = ( 0 > iMinTime ? iMinTime : static_cast<int>(iMinTime) * 60 );
						BM::Stream kPacket( PT_GM_A_REQ_GMCOMMAND, EGMC_MUTE );
						kPacket.Push( pkPlayer->GetID() );
						kPacket.Push( wstrName );
						kPacket.Push( iSecTime );
						::SendToContents( kPacket );
					}break;

				case GMCMD_QUESTBEGIN:
					{
						short sQuestID = 0;
						pkNfy->Pop( sQuestID );

						CONT_PLAYER_MODIFY_ORDER kOrder;

						SPMOD_AddIngQuest kAddQuestData(sQuestID, QS_Begin);
						kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );

						BM::Stream kAddonPacket(PT_M_I_REQ_AFTER_QUEST_ACCEPT);
						kAddonPacket.Push( static_cast< int >(sQuestID) );

						PgAction_ReqModifyItem kAction(IMEPT_GODCMD, GroundKey(), kOrder, kAddonPacket);
						kAction.DoAction(pkPlayer, NULL);

						//PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
						//if( pkMyQuest )
						//{
						//	PgQuestInfo const *pkQuestInfo = NULL;
						//	if ( g_kQuestMan.GetQuest(sQuestID, pkQuestInfo) )
						//	{
						//		SUserQuestState* pkState = pkMyQuest->BeginQuest( sQuestID, pkQuestInfo->Type() );
						//		if( pkState )
						//		{
						//			//PgAction_NfyChangeState kNfyQuestAction(pkState);
						//			//kNfyQuestAction.DoAction( pkPlayer, NULL );

						//			RefreshGroundQuestInfo( pkPlayer );
						//		}
						//	}
						//}	
					}break;
				case GMCMD_CLEAR_ING_QUEST:
					{
						CONT_PLAYER_MODIFY_ORDER kOrder;
						kOrder.push_back( SPMO(IMET_CLEAR_ING_QUEST, pkPlayer->GetID(), SPMOD_AddIngQuest(0, QS_None)) );
						PgAction_ReqModifyItem kAction(IMEPT_GODCMD, GroundKey(), kOrder);
						kAction.DoAction(pkPlayer, NULL);
					}break;
				case GM_CMD_GUILD_BOARD_REFRESH:
					{
						bool bIsGMCommand = true;
						const BM::GUID kCharGuid = pkPlayer->GetID();

						BM::Stream kPacket(PT_A_N_REQ_GET_ENTRANCE_OPEN_GUILD);
						kPacket.Push(bIsGMCommand);
						kPacket.Push(kCharGuid);
						SendToGuildMgr(kPacket);
					}break;
				case GMCMD_SETABIL:
					{
						WORD wAbil = 0;
						int iValue = 0;
						pkNfy->Pop(wAbil);
						pkNfy->Pop(iValue);
						switch(wAbil)
						{
						case AT_CP:
							{
								PgAction_ReqAddCp kModify(CPE_GodCmd, iValue, GroundKey());
								kModify.DoAction( pkPlayer, NULL );
							}break;
						default:
							{
								pkPlayer->SetAbil(wAbil, iValue, true, true);

								if((AT_ACHIEVEMENT_DATA_MIN <= wAbil) && (wAbil <= AT_ACHIEVEMENT_DATA_MAX))
								{
									SPMOD_AddAbil kSetData(wAbil, iValue);
									CONT_PLAYER_MODIFY_ORDER kOrder;
									kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), kSetData));
									PgAction_ReqModifyItem kAction(CAE_Achievement, this->GroundKey(), kOrder);
									kAction.DoAction(pkPlayer, NULL);
								}
								
								//INFO_LOG(BM::LOG_LV5, _T("[%s]Unknown Abil[%hd]"), __FUNCTIONW__, wAbil);
								//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
							}break;
						}
					}break;
				case GMCMD_UNIT_SETABIL:
					{
						BM::GUID kGuid;
						WORD wAbil = 0;
						int iValue = 0;
						pkNfy->Pop(kGuid);
						pkNfy->Pop(wAbil);
						pkNfy->Pop(iValue);

						CUnit *pkUnit = GetUnit( kGuid );
						if(pkUnit)
						{
							pkUnit->SetAbil(wAbil, iValue, true, true);
						}
					}break;

				case GMCMD_SETENCHANT:
					{
						BYTE Inv = 0,
							 Pos = 0;
						int iModifyType = 0;
						int iValue1 = 0;
						int iValue2 = 0;

						pkNfy->Pop( Inv );
						pkNfy->Pop( Pos );
						pkNfy->Pop( iModifyType );
						pkNfy->Pop( iValue1 );
						pkNfy->Pop( iValue2 );

						PgInventory * pkInv = pkPlayer->GetInven();

						SItemPos kItemPos = SItemPos( Inv, Pos );
						PgBase_Item kItem;
						if( SUCCEEDED(pkInv->GetItem(kItemPos, kItem)) )
						{
							SEnchantInfo kNewEnchantInfo = kItem.EnchantInfo();

							switch(iModifyType)
							{
							case 0:
								{
									kNewEnchantInfo.Rarity(static_cast<__int64>(iValue1));//1024
								}
								break;
							case 1:
								{
									kNewEnchantInfo.PlusType(static_cast<__int64>(iValue1));//128 종
									kNewEnchantInfo.PlusLv(static_cast<BYTE>(iValue2));//15종
								}break;
							case 2:
								{
									kNewEnchantInfo.BasicType1(static_cast<__int64>(iValue1));
									kNewEnchantInfo.BasicLv1(static_cast<__int64>(iValue2));
								}break;
							case 3:
								{
									kNewEnchantInfo.BasicType2(static_cast<__int64>(iValue1));
									kNewEnchantInfo.BasicLv2(static_cast<__int64>(iValue2));
								}break;
							case 4:
								{
									kNewEnchantInfo.BasicType3(static_cast<__int64>(iValue1));
									kNewEnchantInfo.BasicLv3(static_cast<__int64>(iValue2));
								}break;
							case 5:
								{
									kNewEnchantInfo.BasicType4(static_cast<__int64>(iValue1));
									kNewEnchantInfo.BasicLv4(static_cast<__int64>(iValue2));
								}break;
							case 6:
								{
									kNewEnchantInfo.IsCurse(static_cast<__int64>(iValue1));//저주.
									kNewEnchantInfo.IsSeal(static_cast<__int64>(iValue2));//봉인됨.
								}break;
							case 7:
								{
									kNewEnchantInfo.Attr(static_cast<__int64>(iValue1));//속성. 화수목금토,  EPropertyType
									kNewEnchantInfo.AttrLv(static_cast<__int64>(iValue2));//15
								}break;
							case 8:
								{
									kNewEnchantInfo.IsAttached(static_cast<__int64>(iValue1));
								}break;
							}

							PgBase_Item kOldItem = kItem;

							kItem.EnchantInfo(kNewEnchantInfo);

							CONT_PLAYER_MODIFY_ORDER kOrder;

							SPMOD_Enchant kEnchantData( kOldItem, kItemPos, kItem.EnchantInfo());//변경될 인첸트
							SPMO kIMO(IMET_MODIFY_ENCHANT, pkPlayer->GetID(), kEnchantData);
							kOrder.push_back(kIMO);

							PgAction_ReqModifyItem kAction(CIE_GodCmd, GroundKey(), kOrder);
							kAction.DoAction(pkPlayer, NULL);
						}
					}break;

				case GMCMD_MISSIONMOVE:
					{
						int iEvent = 0;
						SMissionKey kMissionKey;
						pkNfy->Pop( kMissionKey.iKey );
						pkNfy->Pop( kMissionKey.iLevel );
						pkNfy->Pop( iEvent );

						--kMissionKey.iLevel;//Client는 레벨 1부터 온다.

						Constellation::SConstellationMission constellationMission;
						GetConstellationMission( GetConstellationKey(), constellationMission );
						BM::Stream kPacket( PT_C_M_MISSION_TEST, kMissionKey );
						kPacket.Push( GroundKey() );
						kPacket.Push( iEvent );
						kPacket.Push( pkPlayer->GetID() );	// Owner
						kPacket.Push (pkPlayer->GetAbil(AT_LEVEL) );	// Contents Server가 레벨을 검사하여야 한다.
						constellationMission.WriteToPacket(kPacket);
						SendToMissionMgr( kPacket );
					}break;

				case GMCMD_COUPLE_INIT:
					{
						BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND);
						kPacket.Push( pkPlayer->GetID() );
						kPacket.Push( (BYTE) CC_CoupleInit );
						SendToCoupleMgr( kPacket );
					}break;

				case GMCMD_MISSIONSTATGE:
					{
						int iMissionStage = 0;
						pkNfy->Pop(  iMissionStage );

						if ( GKIND_MISSION == this->GetKind() )
						{
							PgMissionGround *pkMissionGround = dynamic_cast<PgMissionGround*>(this);
							if ( pkMissionGround )
							{
								if( pkMissionGround->IsDefenceGameMode() )
								{
									pkMissionGround->GM_DefenceNowStage(iMissionStage-1);
								}
								else
								{
									return pkMissionGround->SwapStage_Before( iMissionStage, NULL );
								}
							}
						}
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}break;
				case GMCMD_MISSIONSCORE:
					{
						int iScore = 0;
						pkNfy->Pop( iScore );

						if( GKIND_MISSION == this->GetKind() )
						{
							PgMissionGround *pkMissionGround = dynamic_cast<PgMissionGround*>(this);
							if( pkMissionGround )
							{
								pkMissionGround->SetGMMissionScore(iScore);
							}
						}
					}break;
				case GMCMD_ADD_EFFECT:
					{
						int iEffectNo = 0;
						pkNfy->Pop( iEffectNo );
						SActArg kArg;
						kArg.Set(ACTARG_GROUND, this);
						pkPlayer->AddEffect(iEffectNo, 0, &kArg, pkPlayer);
					}break;
				case GMCMD_DELETE_EFFECT:
					{
						int iEffectNo = 0;
						pkNfy->Pop( iEffectNo );				
						pkPlayer->DeleteEffect(iEffectNo);
					}break;
				case GMCMD_INDUN_START_NOW:
					{
						PgIndun *pkIndun = dynamic_cast<PgIndun*>(this);
						if ( pkIndun )
						{
							if ( pkIndun->GetState() == INDUN_STATE_WAIT )
							{
								pkIndun->SetState( INDUN_STATE_READY );
							}
						}
					}break;
				case GMCMD_INDUN_END_NOW:
					{
						PgIndun *pkIndun = dynamic_cast<PgIndun*>(this);
						if ( pkIndun )
						{
							if ( pkIndun->GetState() == INDUN_STATE_PLAY )
							{
								pkIndun->SetState( INDUN_STATE_RESULT_WAIT );
							}
						}
					}break;
				case GMCMD_INDUN_ADD_POINT:
					{
						int iPoint = 0;
						pkNfy->Pop( iPoint );
						if ( iPoint )
						{
							this->GMCommand_RecvGamePoint( pkPlayer, iPoint );
						}
					}break;
				case GMCMD_KOH_TRIGGER_POINT:
					{
						std::string kTriggerID;
						int iPoint = 0;
						pkNfy->Pop( kTriggerID );
						pkNfy->Pop( iPoint );
						if ( !kTriggerID.empty() )
						{
							if(PgWarGround * pkWarGnd = dynamic_cast<PgWarGround*>(this))
							{
								pkWarGnd->KOH_SetTriggerUnitPoint( kTriggerID, iPoint );
							}
						}
					}break;
				case GMCMD_LOVEROUNDWIN:
					{
						int WinTeam = 0;
						pkNfy->Pop(WinTeam);
						if( 0 != WinTeam )
						{
							PgWarGround * pWarGnd = NULL;
							pWarGnd = dynamic_cast<PgWarGround*>(this);
							if( pWarGnd )
							{
								pWarGnd->RoundWin(WinTeam);
							}
						}
					}break;
				case GMCMD_LOVEGAMEWIN:
					{
					}break;
				case GMCMD_GUILD_ADDEXP:
					{
						int iAddExp = 0;

						pkNfy->Pop( iAddExp );

						if( iAddExp )
						{
							BM::Stream kNPacket(PT_M_N_REQ_GUILD_COMMAND, pkPlayer->GetID());
							kNPacket.Push( (BYTE)GC_AddExp );
							kNPacket.Push( iAddExp );
							SendToGuildMgr(kNPacket);
						}
					}break;
				case GMCMD_QUEST_FAILDAILYQUEST:
					{
						// 강제로 하루 전 시간을 만든다. 
						BM::PgPackedTime const kLocalTime = BM::PgPackedTime::LocalTime();
						BM::PgPackedTime kFailPrevTime(kLocalTime), kNextTime(kLocalTime);
						CGameTime::AddTime(kFailPrevTime, -CGameTime::OneDay);
						kFailPrevTime.Hour(0), kFailPrevTime.Min(0), kFailPrevTime.Sec(0);
						kNextTime.Hour(0), kNextTime.Min(0), kNextTime.Min(0);

						CheckDayLoopQuestTime(GroundKey(), pkPlayer, kFailPrevTime, kNextTime);
					}break;
				case GMCMD_GROUNDMUTE:
					{
						bool bSet = false;

						pkNfy->Pop( bSet );

						if( true == bSet )
						{
							m_kGroundMute = pkPlayer->GetID();
						}
						else
						{
							m_kGroundMute.Clear();
						}
					}break;
				case GMCMD_SET_DGATTR:
					{
						int iAddDynamicAttr = 0;
						bool bSet = false;

						pkNfy->Pop( iAddDynamicAttr );
						pkNfy->Pop( bSet );

						if( bSet )
						{
							iAddDynamicAttr = DynamicGndAttr() | iAddDynamicAttr;
						}
						else
						{
							iAddDynamicAttr = DynamicGndAttr() & (DGATTR_FLAG_ALL ^ iAddDynamicAttr);
						}

						DynamicGndAttr( static_cast< EDynamicGroundAttr >(iAddDynamicAttr) );
					}break;
				case GMCMD_ADD_BS_POINT:
					{
						if( PgGroundUtil::IsBSGround(GetAttr())
						&&	PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
						{
							BM::Stream kNfyPacket(PT_U_G_GM_ADDSCORE);
							kNfyPacket.Push( *pkNfy );
							pkPlayer->VNotify(&kNfyPacket);
						}
					}break;
				case GMCMD_SET_BATTLESQUARE_STATE:
					{
						int iGameIDX = 0;
						int iStatus = 0;

						pkNfy->Pop( iGameIDX );
						pkNfy->Pop( iStatus );

						BM::Stream kPacket(PT_GM_N_REQ_SET_GAME_STATUS);
						kPacket.Push( iGameIDX );
						kPacket.Push( iStatus );
						::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
					}break;
				case GMCMD_PET_CLASSCHANGE:
					{
						int iGrade = 0;//
						short nLv = 0;
						pkNfy->Pop( iGrade );
						pkNfy->Pop( nLv );

						CONT_PLAYER_MODIFY_ORDER kOrder;

						PgActPet_MakeClassChangeOrder kAction( kOrder, true );
						if ( true == kAction.DoAction( pkPlayer, iGrade, nLv ) )
						{
							PgAction_ReqModifyItem kItemModifyAction( CIE_SetAbilPet, GroundKey(), kOrder, BM::Stream(), true );//죽음무시
							kItemModifyAction.DoAction( pkPlayer->GetID() );
						}
					}break;
				case GMCMD_PET_SETABIL:
					{
						WORD wType = 0;
						int iValue = 0;
						pkNfy->Pop( wType );
						pkNfy->Pop( iValue );

						PgPet * pkPet = GetPet( pkPlayer );
						if ( pkPet )
						{
							switch( wType )
							{
							case AT_CLASS:
							case AT_LEVEL:
							case AT_HEALTH:
							case AT_MENTAL:
								{// 이건 이걸로 금지
								}break;
							case AT_EXPERIENCE:
								{
									CONT_PLAYER_MODIFY_ORDER kOrder;

									__int64 const i64OldValue = pkPet->GetAbil64( wType );
									__int64 const i64AddExp = static_cast<__int64>(iValue) - i64OldValue;
									if ( 0i64 < i64AddExp )
									{
										GET_DEF(PgClassPetDefMgr,kClassPetDefMgr);
										PgActPet_AddExp kAction(kClassPetDefMgr);

										SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
										if ( true == kAction.PopAbilList( pkPet, i64AddExp, kAddAbilPet.kAbilList ) )
										{
											SPMO kIMO( IMET_PET, pkPlayer->GetID(), kAddAbilPet );
											kOrder.push_back( kIMO );
										}
									}
									else if ( 0i64 > i64AddExp )
									{
										SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
										kAddAbilPet.kAbilList.push_back( SPMOD_SAbil( AT_EXPERIENCE, i64AddExp ) );

										SPMO kIMO( IMET_PET, pkPlayer->GetID(), kAddAbilPet );
										kOrder.push_back( kIMO );
									}

									if ( !kOrder.empty() )
									{
										PgAction_ReqModifyItem kItemModifyAction( CIE_SetAbilPet, GroundKey(), kOrder, BM::Stream(), true );//죽음무시
										kItemModifyAction.DoAction( pkPet->Caller() );
									}
								}break;
							case AT_COLOR_INDEX:
								{// Set Abil
									SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos);
									kAddAbilPet.kAbilList.push_back( SPMOD_SAbil(wType, static_cast<__int64>(iValue)) );

									CONT_PLAYER_MODIFY_ORDER kOrder;

									SPMO kIMO( IMET_PET, pkPlayer->GetID(), kAddAbilPet );
									kOrder.push_back( kIMO );

									PgAction_ReqModifyItem kItemModifyAction( CIE_SetAbilPet, GroundKey(), kOrder, BM::Stream(), true );//죽음무시
									kItemModifyAction.DoAction( pkPet->Caller() );
								}break;
							default:
								{
									pkPet->SetAbil( wType, iValue, true );
								}break;
							}
						}
					}break;
				case GMCMD_PET_ADDABIL:
					{
						WORD wType = 0;
						int iAddValue = 0;
						pkNfy->Pop( wType );
						pkNfy->Pop( iAddValue );

						PgPet * pkPet = GetPet( pkPlayer );
						if ( pkPet )
						{
							switch( wType )
							{
							case AT_CLASS:
							case AT_LEVEL:
							case AT_HEALTH:
							case AT_MENTAL:
								{// 이건 이걸로 금지
								}break;
							case AT_EXPERIENCE:
								{
									CONT_PLAYER_MODIFY_ORDER kOrder;
									if ( 0 < iAddValue )
									{
										GET_DEF(PgClassPetDefMgr,kClassPetDefMgr);
										PgActPet_AddExp kAction(kClassPetDefMgr);

										SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
										if ( true == kAction.PopAbilList( pkPet, static_cast<__int64>(iAddValue), kAddAbilPet.kAbilList ) )
										{
											SPMO kIMO( IMET_PET, pkPlayer->GetID(), kAddAbilPet );
											kOrder.push_back( kIMO );
										}
									}
									else if ( 0 > iAddValue )
									{
										SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
										kAddAbilPet.kAbilList.push_back( SPMOD_SAbil( AT_EXPERIENCE, static_cast<__int64>(iAddValue) ) );

										SPMO kIMO( IMET_PET, pkPlayer->GetID(), kAddAbilPet );
										kOrder.push_back( kIMO );
									}

									if ( !kOrder.empty() )
									{
										PgAction_ReqModifyItem kItemModifyAction( CIE_SetAbilPet, GroundKey(), kOrder, BM::Stream(), true );//죽음무시
										kItemModifyAction.DoAction( pkPet->Caller() );
									}
								}break;
							default:
								{
								}break;
							}
						}
					}break;
				case GMCMD_PLAYERPLAYTIMESTEP:
					{
						BM::Stream kPacket(PT_M_I_GMCMD_PLAYERPLAYTIMESTEP);
						kPacket.Push(*pkNfy);
						SendToContents(kPacket);
					}break;
				case GMCMD_SETPLAYERPLAYTIME:
					{
						BM::Stream kPacket(PT_M_I_GMCMD_SETPLAYERPLAYTIME);
						kPacket.Push(*pkNfy);
						SendToContents(kPacket);
					}break;
				case GMCMD_SUPERGROUNDMOVE:
					{
						int iSuperGroundNo = 0, iSuperGroundMode = 0;
						pkNfy->Pop(iSuperGroundNo);
						pkNfy->Pop(iSuperGroundMode);
						SuperGroundUtil::ReqEnterSuperGround(pkPlayer, GroundKey(), iSuperGroundNo, iSuperGroundMode);
					}break;
				case GMCMD_SUPERGROUNDFLOOR:
					{
						int iNextFloor = 0;
						int iSpawnNo = 0;
						bool bForce = false;
						pkNfy->Pop(iNextFloor);
						pkNfy->Pop(iSpawnNo);
						pkNfy->Pop(bForce);
						if( 0 >= iSpawnNo )
						{
							iSpawnNo = 1;
						}
						PgSuperGround* pkSuperGround = dynamic_cast< PgSuperGround* >(this);
						if( pkSuperGround )
						{
							pkSuperGround->NextFloor(iNextFloor, iSpawnNo, bForce);
						}
					}break;
				case GMCMD_NEW_ALRAMMISSION:
					{
						int iMissionNo = 0;
						pkNfy->Pop( iMissionNo );

						if ( true == this->IsAlramMission() )
						{
							PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();
							if ( SUCCEEDED(m_pkAlramMissionMgr->GetNewActionByID( iMissionNo, rkAlramMission)) )
							{
								BM::Stream kNewAlraMPacket( PT_M_C_NFY_ALRAMMISSION_BEGIN, rkAlramMission.GetID() );
								kNewAlraMPacket.Push( rkAlramMission.GetBeginTime() );
								pkPlayer->Send( kNewAlraMPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );
							}
							else
							{
								pkPlayer->SendWarnMessage(120108);
							}
						}
						else
						{
							pkPlayer->SendWarnMessage(120107);// 받을 수 없는 상태
						}
					}break;
				case GMCMD_JOBSKILL_ADD_EXPERTNESS:
					{
						int iSkillNo = 0;
						int iAddValue = 0;
						
						pkNfy->Pop( iSkillNo );
						pkNfy->Pop( iAddValue );

						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							
							SPMOD_JobSkillExpertness kAddExpertnessData(iSkillNo, iAddValue);
							kOrder.push_back(SPMO(IMET_JOBSKILL_ADD_EXPERTNESS, pkPlayer->GetID(), kAddExpertnessData));

							PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_JOBSKILL_ADD_EXHAUSTION:
					{
						int iAddValue = 0;
						pkNfy->Pop( iAddValue );

						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							
							SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pkPlayer->GetID(), SPMOD_JobSkillExhaustion(iAddValue)); // 피로도 소모
							kOrder.push_back(kIMO);

							PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_JOBSKILL_RESET_EXHAUSTION:
					{
						int iResetMinute = 0;
						pkNfy->Pop( iResetMinute );

						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							
							SPMO kIMO(IMET_JOBSKILL_RESET_EXHAUSTION, pkPlayer->GetID(), SPMOD_JobSkillExhaustion(iResetMinute)); // 피로도 리셋
							kOrder.push_back(kIMO);

							PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_JOBSKILL_ALWAYS_USE_SUBTOOL:
					{
						bool bAlwaysUseSubTool = false;
						pkNfy->Pop( bAlwaysUseSubTool );

						if( bAlwaysUseSubTool )
						{
							pkPlayer->SetAbil(AT_GM_JOBSKILL_USE_SUBTOOL, 1);
							BM::Stream kPacket( PT_M_C_GMCMD_GODHAND_JOBSKILL_SUBTOOL_ON );
							pkPlayer->Send( kPacket );
						}
						else
						{
							pkPlayer->SetAbil(AT_GM_JOBSKILL_USE_SUBTOOL, 0);							
							BM::Stream kPacket( PT_M_C_GMCMD_GODHAND_JOBSKILL_SUBTOOL_OFF );
							pkPlayer->Send( kPacket );
						}
					}break;
				case GMCMD_PREMIUM_SERVICE_INSERT:
					{
						std::wstring kName;
						int iServiceNo = 0;
						pkNfy->Pop( kName );
						pkNfy->Pop( iServiceNo );

						PgPlayer * pkTarget = (false==kName.empty()) ? dynamic_cast<PgPlayer*>(GetPlayer(kName)) : pkPlayer;
						if(pkTarget)
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_INSERT, pkPlayer->GetID(), SPMOD_PremiumService(iServiceNo)));

							PgAction_ReqModifyItem kItemModifyAction(CIE_PREMIUM_SERVICE, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}break;
				case GMCMD_PREMIUM_SERVICE_MODIFY:
					{
						std::wstring kName;
						int iUseDate = 0;
						pkNfy->Pop( kName );
						pkNfy->Pop( iUseDate );//분단위

						PgPlayer * pkTarget = (false==kName.empty()) ? dynamic_cast<PgPlayer*>(GetPlayer(kName)) : pkPlayer;
						if(pkTarget)
						{
							if(pkTarget->GetPremium().IsUserService())
							{
								int const iServiceNo = pkTarget->GetPremium().GetServiceNo();

								CONT_PLAYER_MODIFY_ORDER kOrder;
								kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_MODIFY, pkTarget->GetID(), SPMOD_PremiumServiceModify(iServiceNo,iUseDate)));

								PgAction_ReqModifyItem kItemModifyAction(CIE_PREMIUM_SERVICE, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkTarget, NULL);
							}
						}
					}break;
				case GMCMD_PREMIUM_SERVICE_REMOVE:
					{
						std::wstring kName;
						pkNfy->Pop( kName );

						PgPlayer * pkTarget = (false==kName.empty()) ? dynamic_cast<PgPlayer*>(GetPlayer(kName)) : pkPlayer;
						if(pkTarget)
						{
							if(pkTarget->GetPremium().IsUserService())
							{
								CONT_PLAYER_MODIFY_ORDER kOrder;
								kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_REMOVE, pkTarget->GetID()));

								PgAction_ReqModifyItem kItemModifyAction(CIE_PREMIUM_SERVICE, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkTarget, NULL);
							}
						}
					}break;
				case GMCMD_DEFENCE_POINT_COPY:
					{
						int Count = 0;
						pkNfy->Pop(Count);
						if( this->GetAttr() & GATTR_MISSION )
						{
							PgMissionGround * pMissionGround = dynamic_cast<PgMissionGround*>(this);
							if( pMissionGround )
							{
								if( pMissionGround->IsDefenceMode7() )
								{
									pMissionGround->PointAccumCount(Count);
									pMissionGround->AccumMonsterKillableCount(Count);
									pMissionGround->OccupiedPointCopy();
								}
							}
						}
					}break;	
				case GMCMD_MOVE_TO_COMMUNITYEENT_MAP:
					{	// GM 커맨드로 이동할 때는 모든 조건 무시. 인원 제한 늘리지 않음.
						int EventNo = 0;
						pkNfy->Pop(EventNo);

						CONT_DEF_EVENT_SCHEDULE const * pEventSchedule = NULL;
						g_kTblDataMgr.GetContDef(pEventSchedule);

						if( NULL == pEventSchedule )
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("EventSchedule Table NULL!!"));
							return true;
						}

						CONT_DEF_EVENT_SCHEDULE::const_iterator schedule_iter = pEventSchedule->find(EventNo);
						if( schedule_iter == pEventSchedule->end() )
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("There is no EventNo[") << EventNo << _T("] in EventSchedule Table!!"));
							return true;
						}

						SEventScheduleData const & ScheduleData = schedule_iter->second;

						if( ScheduleData.EventGround == this->GetGroundNo() )
						{
							pkPlayer->SendWarnMessage(302026);
							return true;
						}

						if( g_kProcessCfg.ChannelNo() == EXPEDITION_CHANNEL_NO )
						{
							int iGroundNo = ScheduleData.EventGround;
							short nTargetPortal = 1;

							CONT_DEFMAP const *pkContDefMap = NULL;
							g_kTblDataMgr.GetContDef( pkContDefMap );
							if ( pkContDefMap )
							{
								PgMapMoveChecker kMapMoveChecker( *pkContDefMap );
								EPartyMoveType const kMapMoveType = kMapMoveChecker.GetMoveType( iGroundNo );
								if ( E_MOVE_LOCK != kMapMoveType )
								{
									EMapMoveEventType eType = MMET_None;
									if( 0 != (GetAttr() & GATTR_BATTLESQUARE) )
									{
										eType = MMET_BackToChannel;
									}
									{
										CONT_DEFMAP::const_iterator find_iter = pkContDefMap->find(iGroundNo);
										if( pkContDefMap->end() != find_iter )
										{
											if( 0 != ((*find_iter).second.iAttr & GATTR_BATTLESQUARE) )
											{
												eType = MMET_BATTLESQUARE;
											}
										}
									}

									SReqMapMove_MT kRMM(eType);
									kRMM.kTargetKey.GroundNo(iGroundNo);
									kRMM.nTargetPortal = nTargetPortal;

									PgReqMapMove kMapMove( this, kRMM, NULL );
									if ( kMapMove.Add( pkPlayer ) )
									{
										if ( E_MOVE_PARTYMASTER & kMapMoveType )
										{
											// 인던인 경우는.. 파티원도 다 넣어주어야지
											if ( pkPlayer->HaveParty() )
											{
												this->AddPartyMember( pkPlayer, kMapMove );
											}
										}
										else if( E_MOVE_ANY_PARTYMEMMBER & kMapMoveType )
										{
											if ( pkPlayer->HaveParty() )
											{
												this->AddAnyPartyMember( pkPlayer, kMapMove );											
											}
										}
										kMapMove.DoAction();
									}
								}
								else
								{
									pkPlayer->SendWarnMessage( 1 );
								}
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkcontDefMap is NULL"));
							}
							return true;
						}

						MoveToEventGround(EventNo, pkPlayer, ScheduleData.EventGround);
					}break;
				case GMCMD_COMMUNITY_EVENT_END:
					{	// 이벤트 종료는 이벤트 맵에서만.
						if( GetAttr() & GKIND_RACE )
						{
							PgStaticRaceGround * pStaticGround = dynamic_cast<PgStaticRaceGround *>(this);
							if( pStaticGround )
							{
								pStaticGround->GMCommandEndEvent();
							}
						}
						else if( GetAttr() & GKIND_EVENT )
						{
							PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(this);
							if( pStaticGround )
							{
								pStaticGround->GMCommandEndEvent();
							}
						}
					}break;
				case GMCMD_CART_OBJECT_MOVE_TO_PLAYER_POS:
					{
                        int iMoveSpeed = 100;
                        pkNfy->Pop(iMoveSpeed);
						pkPlayer->SendWarnMessageStr(BM::vstring() << "[DEBUG] Move cart to pos " << pkPlayer->GetPos());
						UNIT_PTR_ARRAY kUnits;
						if (0 == GetUnitByType(EUnitType::UT_OBJECT, kUnits))
						{
							pkPlayer->SendWarnMessageStr(BM::vstring() << "Can't find any EUnitType::UT_OBJECT");
							break;
						}

						for (UNIT_PTR_ARRAY::iterator unit_itr = kUnits.begin(); unit_itr != kUnits.end(); ++unit_itr)
						{
							PgObjectUnit* pkObject =  dynamic_cast<PgObjectUnit*>(unit_itr->pkUnit);
							pkObject->SetState(US_MOVE);
							pkObject->GoalPos(pkPlayer->GetPos());
							pkObject->SetAbil(AT_MOVESPEED, iMoveSpeed, true, true);
							pkObject->SetAbil(AT_C_MOVESPEED, iMoveSpeed, true, true);
							break; // not need search any more
						}

						pkPlayer->SendWarnMessageStr(BM::vstring() << "Can't cart unit");
					}break;
				case GMCMD_GROUND_PAUSE:
					{
						pkNfy->Pop(m_kPaused);
						pkPlayer->SendWarnMessageStr(BM::vstring() << "Ground paused: " << m_kPaused);
					}break;
				default:
					{
						GMCommandToContents( pkPlayer, iCmdType, pkNfy );
					}break;
				}
				return true;
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("GMLevel Error CommandNo<") << iCmdType <<_T("> User<") << pkPlayer->Name().c_str() << _T("/") << pkPlayer->GetID() << _T(">") );

				PgLog kLog( ELOrderMain_GMCommand, ELOrderSub_Warning );
				kLog.Set( 0, static_cast<int>(iCmdType) );				// iValue1 GMCommandNo
				kLog.Set( 1, static_cast<int>(pkPlayer->GMLevel()) );	// iValue2 GMLevel
				kLogCont.Add( kLog );
				kLogCont.Commit();
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::GMCommandToContents( PgPlayer *pkPlayer, EGMCmdType const iCmdType, BM::Stream * const pkNfy )
{
	bool bSendToContents = true;

	BM::Stream kGodCmdPacket( PT_M_A_GODCMD, iCmdType );
	switch( iCmdType )
	{
	case GMCMD_KICKUSER:
		{
			kGodCmdPacket.Push( pkPlayer->GetID() );
			kGodCmdPacket.Push( *pkNfy );
		}break;
	case GMCMD_MAPMOVE_RECALL:
		{
			switch( this->GetAttr() )
			{
			case GATTR_PVP:
			case GATTR_EMPORIABATTLE:
			case GATTR_EMPORIA:
				{
					// 여기서는 사용 하면 안된다.
					pkPlayer->SendWarnMessage( 98, EL_GMMsg );
					return false;
				}break;
			default:
				{
					POINT3 pt3Pos;
					pkNfy->Pop( pt3Pos );// 포지션이 서버에 있는게 안맞을 수 있으니까 클라이언트로 부터 받는다.

					SReqMapMove_CM kRMMC( MMET_GM_ReCall );
					kRMMC.kGndKey = GroundKey();
					kRMMC.pt3TargetPos = pt3Pos;

					kGodCmdPacket.Push(pkPlayer->GetID());
					kRMMC.WriteToPacket(kGodCmdPacket);
					kGodCmdPacket.Push(*pkNfy);
					bSendToContents = false;// 센터로 보내야 해
				}break;
			}
		}break;
	case GMCMD_MAPMOVE_TARGET:			
	case GMCMD_MAPMOVE_TARGET_TOMAP:
		{
			switch( this->GetAttr() )
			{
			case GATTR_PVP:
				{
					// 여기서는 사용 하면 안된다.
					pkPlayer->SendWarnMessage( 98, EL_GMMsg );
					return false;
				}break;
			default:
				{
					kGodCmdPacket.Push(pkPlayer->GetID());
					kGodCmdPacket.Push(*pkNfy);
				}break;
			}
			
			bSendToContents = false;// 센터로 보내야 해
		}break;
	case GODCMD_MTS_UM_ARTICLE_STATE:
		{
			kGodCmdPacket.Push(pkPlayer->GetMemberGUID());
			kGodCmdPacket.Push(*pkNfy);
		}break;
	case GODCMD_MTS_PVP_ROOMMODIFY:
		{
		}break;
	case GMCMD_EMPORIA_OPEN:
	case GMCMD_EMPORIA_CLOSE:
	case GMCMD_EMPORIA_BATTLE_START:
		{
			kGodCmdPacket.Push( *pkNfy );
		}break;	
	case GMCMD_CASHITEMGIFT_ADD:
	case GMCMD_CASHITEMGIFT_DEL:
		{
			kGodCmdPacket.Push( pkPlayer->GetID() );
			kGodCmdPacket.Push( *pkNfy );
		}break;
	case GMCMD_PVPLEAGUE_SETEVENT:
		{
			kGodCmdPacket.Push( pkPlayer->GetID() );
			kGodCmdPacket.Push( *pkNfy );
		}break;
	case GMCMD_COMMUNITY_EVENT_START:
		{
			int EventNo = 0;
			pkNfy->Pop(EventNo);

			CONT_DEF_EVENT_SCHEDULE const * pEventSchedule = NULL;
			g_kTblDataMgr.GetContDef(pEventSchedule);

			if( NULL == pEventSchedule )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("EventSchedule Table NULL!!"));
				return true;
			}

			CONT_DEF_EVENT_SCHEDULE::const_iterator schedule_iter = pEventSchedule->find(EventNo);
			if( schedule_iter == pEventSchedule->end() )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("There is no EventNo[") << EventNo << _T("] in EventSchedule Table!!"));
				return true;
			}

			SEventScheduleData const & ScheduleData = schedule_iter->second;

			if( GetAttr() & GATTR_RACE_GROUND )
			{
				PgStaticRaceGround* pkStaticGround = dynamic_cast<PgStaticRaceGround *>(this);
				if(pkStaticGround)
				{
					pkStaticGround->GMCommandStartEvent(EventNo);
				}
			}
			else if( GetAttr() & GATTR_EVENT_GROUND )
			{
				PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(this);
				if( pStaticGround )
				{
					pStaticGround->GMCommandStartEvent(EventNo);
				}
			}
			else
			{
				kGodCmdPacket.Push(EXPEDITION_CHANNEL_NO);
				kGodCmdPacket.Push(ScheduleData.EventGround);
				kGodCmdPacket.Push(EventNo);

				bSendToContents = false;
			}
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Bad GodCommand<") << iCmdType << _T("> User<") << pkPlayer->Name() << _T("/") << pkPlayer->GetID() << _T(">") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	if ( true == bSendToContents )
	{
		return SendToContents( kGodCmdPacket );
	}
	return SendToCenter( kGodCmdPacket );
}