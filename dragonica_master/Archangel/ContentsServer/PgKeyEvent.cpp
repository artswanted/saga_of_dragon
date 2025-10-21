#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgEventView.h"
#include "Collins/Log.h"
#include "Contents/Global.h"
#include "PgDBProcess.h"
#include "JobDispatcher.h"
#include "PgEmporiaMgr.h"
#include "PgGuildMgr.h"

extern void ReloadAntiHackMgr();

bool CALLBACK OnEscape( WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	std::cout << "Press [F11] key to quit" << std::endl;
	return false;
}

bool CALLBACK OnF1( WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	S_STATE_LOG( BM::LOG_LV0, _T("=========================================="));
	g_kProcessCfg.Locked_DisplayState();
	g_kCoreCenter.DisplayState( g_kLogWorker, LT_S_STATE);
	g_kJobDispatcher.VDisplayState( g_kLogWorker, LT_S_STATE);
	S_STATE_LOG(BM::LOG_LV1, L"-- DB Worker Queue Size = "<<g_kCoreCenter.GetQueryCount(DT_PLAYER));

	return false;
}

bool CALLBACK OnF2(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	U_STATE_LOG( BM::LOG_LV0, _T("=========================================="));
	BM::MUTEX_PROFILE_STATS(LT_S_STATE);
	g_kRealmUserMgr.Locked_DisplayState(true);
	return false;
}

bool CALLBACK OnF3(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	S_STATE_LOG( BM::LOG_LV0, _T("=========================================="));
	g_kCoreCenter.DisplayStateDBRecord( g_kLogWorker, LT_S_STATE);

	return false;
}

bool CALLBACK OnF4( WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	ReloadData_Contents();

	INFO_LOG(BM::LOG_LV5, __FL__ << _T("BEGIN : Reloading AntiHack config"));
	if( g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Contents_constant.ini")) )
	{
		ReloadAntiHackMgr();
		PgGuildMgrUtil::InitGuildConstant();
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("END : Reloading AntiHack config"));
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("END : Failed Reloading AntiHack config"));
	
	return false;
}

bool CALLBACK OnF5(WORD const &InputKey)
{
	INFO_LOG(BM::LOG_LV5, __FL__ << _T("BEGIN : Reloading database table"));
	if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	{
		LoadDB(true);
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("END : Success reload database table!"));
	}
	else
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("END : This function isn't work in prodcution mode!"));
		

	/*CEL::DB_QUERY kQuery( DT_DEF, DQT_SELECT_TOP1_EMPORIA_FOR_CHECK, _T("SELECT TOP 1 [EmporiaID] FROM [DR2_User].[dbo].[TB_Emporia]"));
	g_kCoreCenter.PushQuery(kQuery, true);*/

	return false;
}

bool CALLBACK OnF6(WORD const &InputKey)
{
	std::wstring wConfig = g_kProcessCfg.ConfigDir() + _T("Contents_config.ini");
	ReadRealmStateFromIni(wConfig.c_str());
	return true;
}

bool CALLBACK OnF7(WORD const &InputKey)
{
	bool const bIsWriteLog = true;

	std::cout<< __FUNCTION__ << std::endl;
	std::cout << "Write ItemBagLog" << std::endl;

	GET_DEF(CItemBagMgr, kItemBagMgr );

	CONT_DEFMAP const *pkContMap = NULL;
	CONT_DEF_MAP_ITEM_BAG const *pkContMapBag = NULL;
	CONT_DEF_SUCCESS_RATE_CONTROL const * pContSRate = NULL;
	CONT_DEF_ITEM_BAG_GROUP const * pContItemBagGrp = NULL;
	CONT_DEF_ITEM_BAG const * pContItemBag = NULL;
	CONT_DEFMONSTER const * pContMonster = NULL;
	CONT_DEFMONSTERABIL const * pContMonsterAbil = NULL;

	CONT_DEF_MISSION_ROOT const * pContMissionRoot = NULL;
	CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;
	CONT_DEF_MISSION_RESULT const *pkContMissionResult = NULL;

	g_kTblDataMgr.GetContDef(pkContMap);
	g_kTblDataMgr.GetContDef(pkContMapBag);
	g_kTblDataMgr.GetContDef(pContSRate);
	g_kTblDataMgr.GetContDef(pContItemBagGrp);
	g_kTblDataMgr.GetContDef(pContItemBag);
	g_kTblDataMgr.GetContDef(pContMonster);
	g_kTblDataMgr.GetContDef(pContMonsterAbil);
	g_kTblDataMgr.GetContDef(pContMissionRoot);
	g_kTblDataMgr.GetContDef(pContContainer);
	g_kTblDataMgr.GetContDef(pkContMissionResult);



	GET_DEF(CItemDefMgr, kItemDefMgr);

	//	g_kTblDataMgr.GetContDef(pContSRate);
	//	g_kTblDataMgr.GetContDef(pContSRate);
	//	g_kTblDataMgr.GetContDef(pContSRate);
	//	g_kTblDataMgr.GetContDef(pkContMap);

	if(bIsWriteLog)
	{//맵.
		CONT_DEF_MAP_ITEM_BAG::const_iterator mapbag_itor = pkContMapBag->begin();
		while( mapbag_itor != pkContMapBag->end() )
		{
			CONT_DEF_MAP_ITEM_BAG::mapped_type const &kBagInfo = (*mapbag_itor).second;

			int const iMapNo = kBagInfo.iMapNo;
			CONT_DEFMAP::const_iterator map_itor = pkContMap->find(iMapNo);

			if( map_itor != pkContMap->end())
			{
				CONT_DEFMAP::mapped_type const &kTgtMap = (*map_itor).second;

				BM::vstring kMapName;
				GetDefString(kTgtMap.NameNo, kMapName);
				BM::vstring kRetStr;

				if(!(kTgtMap.iAttr & GATTR_INSTANCE))//일반
				{
					kRetStr << _T("N");
				}

				if(kTgtMap.iAttr & GATTR_FLAG_CHAOS_F)//카오스
				{
					kRetStr << _T("C");
				}
				kRetStr << _T(",");

				kRetStr << iMapNo << L"("<< kMapName << L")"<< _COMMA_;

				if(!(kTgtMap.iAttr & GATTR_INSTANCE)
					|| (kTgtMap.iAttr & GATTR_FLAG_CHAOS_F)	)//카오스
				{
					CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator srate_itor = pContSRate->find(kBagInfo.iSuccessRateNo);
					if(srate_itor  != pContSRate->end())
					{
						CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &kGrpSRate = (*srate_itor).second;
						int rate_i = 0;
						while(MAX_SUCCESS_RATE_ARRAY > rate_i)
						{//아이템 백 그룹 돌기.
							int const iGrpPopRate = kGrpSRate.aRate[rate_i];//그룹 Pop 확률
							int const iGrpTotalRate = kGrpSRate.iTotal;//그룹 토탈 확률
							int const iBagGrpNo = kBagInfo.aiBagGrpNo[rate_i];

							CONT_DEF_ITEM_BAG_GROUP::const_iterator baggrp_itor = pContItemBagGrp->find(iBagGrpNo);

							if(baggrp_itor != pContItemBagGrp->end())
							{
								CONT_DEF_ITEM_BAG_GROUP::mapped_type const &kItemBagGrp = (*baggrp_itor).second;

								int baggrp_i = 0;
								while(MAX_ITEM_BAG_ELEMENT > baggrp_i)
								{//아이템 백 내부 돌기

									int const iItemBagNo = kItemBagGrp.aiBagNo[baggrp_i];

									GET_DEF(CItemBagMgr, kItemBagMgr);

									PgItemBag kItemBag;
									if(S_OK == kItemBagMgr.GetItemBag( iItemBagNo, 1, kItemBag))
									{
										PgItemBag::BagElementCont kContItemElement;
										kContItemElement = kItemBag.GetElements();

										PgItemBag::BagElementCont::const_iterator bag_elem_itor = kContItemElement.begin();
										while(bag_elem_itor != kContItemElement.end())
										{
											PgItemBag::BagElementCont::value_type const &kitemElem = (*bag_elem_itor);
											//맵 이름. (아이템 확률) 반복?
											CItemDef const *pkDefItem = kItemDefMgr.GetDef(kitemElem.iItemNo);

											BM::vstring kItemName;
											GetDefString(pkDefItem->NameNo(), kItemName);

											double const dbOutRate = ((double)(iGrpPopRate*kitemElem.nRate) / (double)(iGrpTotalRate*kItemBag.TotalRaiseRate()));

											kRetStr << kitemElem.iItemNo << L"(" << kItemName << L")" << _COMMA_ << (float)dbOutRate << _COMMA_ ;
											++bag_elem_itor;
										}
									}
									++baggrp_i;
								}
							}
							++rate_i;
						}
					}
					//				kGrpSRate.iTotal

					//					srate_itor->second.aRate
					//				kBagInfo.aiBagGrpNo[];
					S_STATE_LOG(BM::LOG_LV0, kRetStr);
				}


				// 스태틱 맵의 경우
				// 아이템 백 그룹을 돌면서
				// 개별 백 그룸의 모든 아이템을 다 뽑아.
				// 그걸 로그로 기록.
				//			GetDefString(
			}
			++mapbag_itor;
		}
	}
	
	if(bIsWriteLog)
	{// 몬스터별
		GET_DEF(CMonsterDefMgr, kMonsterDefMgr);

		CONT_DEFMONSTER::const_iterator mon_itor = pContMonster->begin();
		while( mon_itor != pContMonster->end() )
		{
			int const iMonNo = mon_itor->second.MonsterNo;
			BM::vstring kMonName;
			
			GetDefString(mon_itor->second.Name, kMonName);
			BM::vstring kRetStr;
			kRetStr << iMonNo << L"("<< kMonName << L")"<< _COMMA_;

			CMonsterDef const *pkCaster = kMonsterDefMgr.GetDef(mon_itor->second.MonsterNo);

			int const iItemContainerNo = pkCaster->GetAbil(AT_ITEM_DROP_CONTAINER);
			int const iSuccessRateCtrlNo = pkCaster->GetAbil(AT_ITEM_BAG_SUCCESS_RATE_CONTROL);

			if(0 == iItemContainerNo)
			{
				kRetStr << "BagStyle" << _COMMA_; 

				CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator srate_itor = pContSRate->find(iSuccessRateCtrlNo);
				if(srate_itor  != pContSRate->end())
				{
					CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &kGrpSRate = (*srate_itor).second;

					int const iItemBagNo_1 = pkCaster->GetAbil(AT_ITEM_BAG_PRIMARY);
					int const iItemBagNo_2 = pkCaster->GetAbil(AT_ITEM_BAG_SECONDARY);

					PgItemBag akItemBag[3];
					kItemBagMgr.GetItemBag(iItemBagNo_1, 1, akItemBag[0]);
					kItemBagMgr.GetItemBag(iItemBagNo_2, 1, akItemBag[1]);

					int rate_i = 0;
					while(3 > rate_i)
					{
						int const iGrpPopRate = kGrpSRate.aRate[rate_i];//그룹 Pop 확률
						int const iGrpTotalRate = kGrpSRate.iTotal;//그룹 토탈 확률

						if( 2 == rate_i)
						{
							if(iGrpTotalRate)
							{
								double const dbOutRate = ((double)iGrpPopRate / (double)iGrpTotalRate);
								kRetStr << 0 << L"(MapBag)" << _COMMA_ << (float)dbOutRate << _COMMA_ ;
							}
						}
						else
						{
							PgItemBag &kItemBag = akItemBag[rate_i];
							PgItemBag::BagElementCont kContItemElement;
							kContItemElement = kItemBag.GetElements();
							
							PgItemBag::BagElementCont::const_iterator bag_elem_itor = kContItemElement.begin();
							while(bag_elem_itor != kContItemElement.end())
							{
								PgItemBag::BagElementCont::value_type const &kitemElem = (*bag_elem_itor);
								//맵 이름. (아이템 확률) 반복?
								CItemDef const *pkDefItem = kItemDefMgr.GetDef(kitemElem.iItemNo);

								if(pkDefItem)
								{
									BM::vstring kItemName;
									GetDefString(pkDefItem->NameNo(), kItemName);

									double const dbOutRate = ((double)(iGrpPopRate*kitemElem.nRate) / (double)(iGrpTotalRate*kItemBag.TotalRaiseRate()));

									kRetStr << kitemElem.iItemNo << L"(" << kItemName << L")" << _COMMA_ << (float)dbOutRate << _COMMA_ ;
								}
								++bag_elem_itor;
							}
						}
						++rate_i;
					}
				}
			}
			else
			{
				kRetStr << "ContStyle" << _COMMA_; 

				CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find(iItemContainerNo);
				if(item_cont_itor == pContContainer->end())
				{
					return false;
				}

				CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator srate_itor = pContSRate->find((*item_cont_itor).second.iSuccessRateControlNo);
				if(srate_itor  != pContSRate->end())
				{
					CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &kGrpSRate = (*srate_itor).second;
					int rate_i = 0;
					while(MAX_SUCCESS_RATE_ARRAY > rate_i)
					{//아이템 백 그룹 돌기.
						int const iGrpPopRate = kGrpSRate.aRate[rate_i];//그룹 Pop 확률
						int const iGrpTotalRate = kGrpSRate.iTotal;//그룹 토탈 확률
						int const iBagGrpNo = (*item_cont_itor).second.aiItemBagGrpNo[rate_i];

						CONT_DEF_ITEM_BAG_GROUP::const_iterator baggrp_itor = pContItemBagGrp->find(iBagGrpNo);

						if(baggrp_itor != pContItemBagGrp->end())
						{
							CONT_DEF_ITEM_BAG_GROUP::mapped_type const &kItemBagGrp = (*baggrp_itor).second;

							int baggrp_i = 0;
							while(MAX_ITEM_BAG_ELEMENT > baggrp_i)
							{//아이템 백 내부 돌기

								int const iItemBagNo = kItemBagGrp.aiBagNo[baggrp_i];

								GET_DEF(CItemBagMgr, kItemBagMgr);

								PgItemBag kItemBag;
								if(S_OK == kItemBagMgr.GetItemBag(iItemBagNo, 1, kItemBag))
								{
									PgItemBag::BagElementCont kContItemElement;
									kContItemElement = kItemBag.GetElements();

									PgItemBag::BagElementCont::const_iterator bag_elem_itor = kContItemElement.begin();
									while(bag_elem_itor != kContItemElement.end())
									{
										PgItemBag::BagElementCont::value_type const &kitemElem = (*bag_elem_itor);
										//맵 이름. (아이템 확률) 반복?
										CItemDef const *pkDefItem = kItemDefMgr.GetDef(kitemElem.iItemNo);

										if(pkDefItem)
										{
											BM::vstring kItemName;
											GetDefString(pkDefItem->NameNo(), kItemName);

											double const dbOutRate = ((double)(iGrpPopRate*kitemElem.nRate) / (double)(iGrpTotalRate*kItemBag.TotalRaiseRate()));

											kRetStr << kitemElem.iItemNo << L"(" << kItemName << L")" << _COMMA_ << (float)dbOutRate << _COMMA_ ;
										}
										++bag_elem_itor;
									}
								}
								++baggrp_i;
							}
						}

						++rate_i;
					}
				}
			}

			S_STATE_LOG(BM::LOG_LV0, kRetStr);
			++mon_itor;
		}
	}
	
	if(bIsWriteLog)
	{//미션

		CONT_DEF_MISSION_ROOT::const_iterator m_root_itor = pContMissionRoot->begin();
		while( m_root_itor != pContMissionRoot->end() )
		{
			CONT_DEF_MISSION_ROOT::mapped_type const &kRootInfo = (*m_root_itor).second;

			//			kRootInfo.iMissionNo

			//			int const iMapNo = kBagInfo.iMapNo;
			//			CONT_DEFMAP::const_iterator map_itor = pkContMap->find(iMapNo);

			//			if( map_itor != pkContMap->end())
			{
				//				CONT_DEFMAP::mapped_type const &kTgtMap = (*map_itor).second;

				//				BM::vstring kMapName;
				//				GetDefString(kTgtMap.NameNo, kMapName);
				int iMissionLv = 0;
				while(MAX_MISSION_LEVEL > iMissionLv )
				{
					int const iLvResultNo = kRootInfo.aiMissionResultNo[iMissionLv];

					CONT_DEF_MISSION_RESULT::const_iterator ret_itor = pkContMissionResult->find(iLvResultNo);

					if(ret_itor != pkContMissionResult->end() )
					{
						int iRank = 0;
						while(MAX_MISSION_RANK_CLEAR_LV > iRank)
						{
							BM::vstring kRetStr;
							kRetStr << kRootInfo.iMissionNo << L"("<< iMissionLv <<L"-"<< PgMission_Base::GetMissionRankName(iRank+1) << L")"<< _COMMA_;

							int const iItemContainerNo = ret_itor->second.aiResultContainer[iRank];

							CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find(iItemContainerNo);
							if(item_cont_itor != pContContainer->end())
							{
								CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator srate_itor = pContSRate->find((*item_cont_itor).second.iSuccessRateControlNo);
								if(srate_itor  != pContSRate->end())
								{
									CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &kGrpSRate = (*srate_itor).second;
									int rate_i = 0;
									while(MAX_SUCCESS_RATE_ARRAY > rate_i)
									{//아이템 백 그룹 돌기.
										int const iGrpPopRate = kGrpSRate.aRate[rate_i];//그룹 Pop 확률
										int const iGrpTotalRate = kGrpSRate.iTotal;//그룹 토탈 확률
										int const iBagGrpNo = (*item_cont_itor).second.aiItemBagGrpNo[rate_i];

										CONT_DEF_ITEM_BAG_GROUP::const_iterator baggrp_itor = pContItemBagGrp->find(iBagGrpNo);

										if(baggrp_itor != pContItemBagGrp->end())
										{
											CONT_DEF_ITEM_BAG_GROUP::mapped_type const &kItemBagGrp = (*baggrp_itor).second;

											int baggrp_i = 0;
											while(MAX_ITEM_BAG_ELEMENT > baggrp_i)
											{//아이템 백 내부 돌기

												int const iItemBagNo = kItemBagGrp.aiBagNo[baggrp_i];

												GET_DEF(CItemBagMgr, kItemBagMgr);

												PgItemBag kItemBag;
												if(S_OK == kItemBagMgr.GetItemBag(iItemBagNo, 1, kItemBag))
												{
													PgItemBag::BagElementCont kContItemElement;
													kContItemElement = kItemBag.GetElements();

													PgItemBag::BagElementCont::const_iterator bag_elem_itor = kContItemElement.begin();
													while(bag_elem_itor != kContItemElement.end())
													{
														PgItemBag::BagElementCont::value_type const &kitemElem = (*bag_elem_itor);
														//맵 이름. (아이템 확률) 반복?
														CItemDef const *pkDefItem = kItemDefMgr.GetDef(kitemElem.iItemNo);

														if(pkDefItem)
														{
															BM::vstring kItemName;
															GetDefString(pkDefItem->NameNo(), kItemName);

															double const dbOutRate = ((double)(iGrpPopRate*kitemElem.nRate) / (double)(iGrpTotalRate*kItemBag.TotalRaiseRate()));

															kRetStr << kitemElem.iItemNo << L"(" << kItemName << L")" << _COMMA_ << (float)dbOutRate << _COMMA_ ;
														}
														++bag_elem_itor;
													}
												}
												++baggrp_i;
											}
										}

										++rate_i;
									}
								}

							}
							++iRank;
							S_STATE_LOG(BM::LOG_LV0, kRetStr);
						}
					}
					else
					{
						BM::vstring kRetStr;
						kRetStr << kRootInfo.iMissionNo << _COMMA_<< L"(Error NoResult)"<< _COMMA_<<iLvResultNo;
						S_STATE_LOG(BM::LOG_LV0, kRetStr);
					}
					++iMissionLv;
				}


				// 스태틱 맵의 경우
				// 아이템 백 그룹을 돌면서
				// 개별 백 그룸의 모든 아이템을 다 뽑아.
				// 그걸 로그로 기록.
				//			GetDefString(
			}
			++m_root_itor;
		}
	}
	return true;
}

bool CALLBACK OnF8(WORD const &InputKey)
{
	BM::Stream kSendPacket( PT_A_N_REQ_GAMETIME );
	SendToServerType( CEL::ST_CENTER, kSendPacket );

	TCHAR chTime[100];

	SYSTEMTIME kNowTime;
	g_kEventView.GetLocalTime( &kNowTime );
	CGameTime::SystemTime2String( kNowTime, chTime, 100 );

	INFO_LOG( BM::LOG_LV3,_T("[TESTNOWTIME] : ") << chTime << _T(" And ") << BM::GetTime32() );

	return false;
}

bool CALLBACK OnF9(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	if( g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Contents_constant.ini")) )
	{
		int iValue1 = 0;
		int iValue2 = 0;

		{// Log
			int iValue_OutPut = BM::OUTPUT_ALL;
			int iValue_Level = static_cast<int>(BM::LOG_LV9);

			g_kVariableContainer.Get(EVar_Kind_Log, EVar_LogOutputType, iValue_OutPut);
			g_kVariableContainer.Get(EVar_Kind_Log, EVar_LogLevelLimit, iValue_Level);

			iValue_OutPut &= BM::OUTPUT_ALL;

			SetLogOutPutType(static_cast<BM::E_OUPUT_TYPE>(iValue_OutPut));
			SetLogLeveLimit(static_cast<BM::E_LOG_LEVEL>(iValue_Level));

			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_LogOutputType = ") << iValue_OutPut);
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_LogLevelLimit = ") << iValue_Level);
		}

		{// 캐시샵 OPEN/CLOSE
			SSyncVariable kSyncVariable;
			if( S_OK != g_kVariableContainer.Get(Evar_Kind_CashShop, EVar_CashShop_Open, kSyncVariable.bCashShopOpen ) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'EVar_CashShop_Open'");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			g_kVariableContainer.Get(EVar_Kind_ExpBuff_Logoff, EVar_ExpAdd_MaxExperienceRate, kSyncVariable.iExpAdd_MaxExperienceRate);
			g_kVariableContainer.Get(EVar_Kind_ExpBuff_Logoff, EVar_ExpAdd_AddedExp_Rate_Hunting, kSyncVariable.iExpAdd_AddedExpRate_Hunting);
			g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Maximum_Level, kSyncVariable.iMaxCharacterLevel);
			g_kVariableContainer.Get(EVar_Kind_SpecificReward, EVar_Revive_Feather_LevelUp_Use, kSyncVariable.iReviveFeather_LevelUp_Use);
			g_kVariableContainer.Get(EVar_Kind_SpecificReward, EVar_Revive_Feather_Login_Time, kSyncVariable.iReviveFeather_Login_Time);

			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_CashShop_Open = ") << static_cast<int>(kSyncVariable.bCashShopOpen));
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_ExpAdd_MaxExperienceRate = ") << kSyncVariable.iExpAdd_MaxExperienceRate);
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_ExpAdd_AddedExp_Rate_Hunting = ") << kSyncVariable.iExpAdd_AddedExpRate_Hunting);
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_Maximum_Level = ") << kSyncVariable.iMaxCharacterLevel);			
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_Revive_Feather_LevelUp_Use = ") << kSyncVariable.iReviveFeather_LevelUp_Use);
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_Revive_Feather_Login_Time = ") << kSyncVariable.iReviveFeather_Login_Time);
			
			g_kEventView.VariableCont(kSyncVariable);
			BM::Stream kGPacket(PT_N_M_NFY_SYNC_CASHSHOP_ON_OFF);
			g_kEventView.WriteToPacket(false, kGPacket);
			::SendToServerType( CEL::ST_CENTER, kGPacket );
		}
	}
	return false;
}

bool CALLBACK OnF12(WORD const &kInputKey)
{
	INFO_LOG(BM::LOG_LV7, "Reload lua...");
	lua_tinker::call<void>("SelfReload");
	lua_tinker::call<void>("Reload");
	INFO_LOG(BM::LOG_LV5, "Reload success!");
	return true;
}

bool CALLBACK OnPlus(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	size_t const ret = __min(g_kRealmUserMgr.MaxPlayerCount()+100, 15000 );
	g_kRealmUserMgr.MaxPlayerCount( ret );
	INFO_LOG( BM::LOG_LV0, __FL__ << _T("UserCount ") << g_kRealmUserMgr.MaxPlayerCount() );
	return true;
}

bool CALLBACK OnMinus(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	
	size_t const ret = __max((int)g_kRealmUserMgr.MaxPlayerCount()-100, 0 );
	g_kRealmUserMgr.MaxPlayerCount( ret );
	INFO_LOG( BM::LOG_LV0, _T("UserCount ") << g_kRealmUserMgr.MaxPlayerCount() );
	return true;
}

bool CALLBACK OnTerminateServer(WORD const& rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, _T("Trying to [ContentsServer shutdown]"));

	if (false == g_kJobDispatcher.VReadyToStop( g_kLogWorker, LT_S_STATE))
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot stop now[Job Queue] .. wait a moment"));
		return false;
	}
	if (false == g_kCoreCenter.VReadyToStop( g_kLogWorker, LT_S_STATE) )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot stop now[DB Worker] .. Wait a moment"));
		return false;
	}

	g_kRealmUserMgr.Locked_SetServerState(PgRealmUserManager::ESERVER_STATUS_STOPPING);
	return true;
}

bool CALLBACK OnEnd(WORD const& rkInputKey)
{
	// Ctrl + Shift + END
	std::cout << __FUNCTION__ << " / Key:" << rkInputKey << std::endl;
	SHORT sState = GetKeyState(VK_SHIFT);
	if (HIBYTE(sState) == 0)
	{
		return false;
	}
	sState = GetKeyState(VK_CONTROL);
	if (HIBYTE(sState) == 0)
	{
		return false;
	}
	std::cout << "SUCCESS " << __FUNCTION__ << " / Key:" << rkInputKey << std::endl;

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] Contents Server will be CRASHED.. are you sure??","DRAGONICA_Contents",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] Contents Server will be CRASHED.. are you sure??","DRAGONICA_Contents",MB_OKCANCEL) )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Server crashed by INPUT") );
//			::RaiseException(1,  EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
			__asm int 3;
		}
	}
	return false;
}

bool CALLBACK OnTestEventQuest(WORD const& rkInputKey)
{
	//if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	//{
		BM::Stream kPacket(PT_N_N_REQ_RELOAD_EVENT_QUEST);
		::SendToRealmContents(PMET_EVENTQUEST, kPacket);
	//}
	return false;
}
bool CALLBACK OnTestBattleSquare(WORD const& rkInputKey)
{
	//if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	//{
		BM::Stream kPacket(PT_A_N_REQ_RELOAD_BS_GAME);
		::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
	//}
	return false;
}

bool RegistKeyEvent()
{
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	g_kConsoleCommander.Regist( VK_F1,		OnF1 );
	g_kConsoleCommander.Regist( VK_F2,		OnF2 );
	g_kConsoleCommander.Regist( VK_F3,		OnF3 );
	g_kConsoleCommander.Regist( VK_F4,		OnF4 );
	g_kConsoleCommander.Regist( VK_F5,		OnF5 );
	g_kConsoleCommander.Regist( VK_F6,		OnF6 );
	g_kConsoleCommander.Regist( VK_F7,		OnF7 );
	g_kConsoleCommander.Regist( VK_F8,		OnF8 );
	g_kConsoleCommander.Regist( VK_F9,		OnF9 );
	g_kConsoleCommander.Regist( VK_F11,		OnTerminateServer );
	g_kConsoleCommander.Regist( VK_F12,		OnF12 );
	
	g_kConsoleCommander.Regist( VK_ADD,			OnPlus );//숫자판
	g_kConsoleCommander.Regist( VK_SUBTRACT,	OnMinus );//숫자판
	g_kConsoleCommander.Regist( VK_END,			OnEnd );

	// Test
	g_kConsoleCommander.Regist( VK_OEM_4,	OnTestEventQuest );		// [
	g_kConsoleCommander.Regist( VK_OEM_6,	OnTestBattleSquare );	// ]
	return true;
}
