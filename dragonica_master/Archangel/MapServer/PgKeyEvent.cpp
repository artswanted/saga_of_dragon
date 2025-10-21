#include "stdafx.h"
#include <NxBounds3.h> 
#include "NiCollision.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGroundMgr.h"
#include "BM/PgTask.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgEventView.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"

#include "constant.h"
#include "global.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgClassDefMgr.h"
#include "Variant/constant.h"
#include "Variant/MonsterDefMgr.h"

#include "Variant/PgNpc.h"
#include "PgAction.h"
#include "Variant/PgQuestInfo.h"
#include "PgQuest.h"
#include "variant/PgGroundItemBoxMgr.h"
#include "variant/ItemBagMgr.h"
#include "Lohengrin/VariableContainer.h"

bool CALLBACK OnEscape( WORD const &rkInputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	std::cout << "Press [F11] key to quit" << std::endl;
	
	//BM::GUID kNewGuid;
	//kNewGuid.Generate();
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Normal Guid[%s]"), __FUNCTIONW__, kNewGuid.str().c_str());
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Guid[%s]"), __FUNCTIONW__, kNewGuid);
	//char* chMessage = "Welcome to Exception";
	//INFO_LOG(BM::LOG_LV9, _T("[%s] char message output[%s]"), __FUNCTIONW__, chMessage);
	//char* pchMsg = NULL;
	//INFO_LOG(BM::LOG_LV9, _T("[%s] char message output[%s]"), __FUNCTIONW__, pchMsg);
	return true;
}

bool CALLBACK OnF1( WORD const &rkInputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	g_kCoreCenter.DisplayState( g_kLogWorker, LT_INFO);
	g_kProcessCfg.Locked_DisplayState();
	g_kGndMgr.DisplayState();
	return true;
}

#include "variant/PgItemOptionMgr.h"
#include "lohengrin/DBTables.h"

bool CALLBACK OnF2( WORD const &rkInputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	if ( IDOK != ::MessageBoxA(NULL,"[WARNING] It will test Monster/ItemDrop\nIt will takes toooo~ long time\n Are you sure??","DRAGONICA_Center",MB_OKCANCEL) )
	{
		return true;
	}
{
/*
	CONT_DEFMAP const *pkContMap = NULL;
	CONT_DEF_MAP_ITEM_BAG const *pkContMapItemBag = NULL;
	CONT_DEF_MAP_REGEN_POINT const *pkContRegenPoint = NULL;
	CONT_DEF_MONSTER_BAG const *pkContMonsterBag;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pkContMonsterBagElement;
	
	g_kTblDataMgr.GetContDef(pkContMap);
	g_kTblDataMgr.GetContDef(pkContMapItemBag);
	g_kTblDataMgr.GetContDef(pkContMonsterBag);
	g_kTblDataMgr.GetContDef(pkContRegenPoint);
	g_kTblDataMgr.GetContDef(pkContMonsterBagElement);
	
	GET_DEF(CItemDefMgr, kItemDefMgr);
	GET_DEF(CMonsterDefMgr, kMonsterDefMgr);

	CONT_DEFMAP::const_iterator map_itor = pkContMap->begin();
	while(map_itor != pkContMap->end())
	{
		CONT_DEF_MAP_ITEM_BAG::const_iterator map_item_bag_Itor = pkContMapItemBag->find(map_itor->first);
		if(map_item_bag_Itor != pkContMapItemBag->end())
		{
			CONT_DEF_MAP_REGEN_POINT kContThisRegen;
			
			{
				CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = pkContRegenPoint->begin();
				while(regen_itor != pkContRegenPoint->end())
				{//지금맵용 리젠포인트. 셋팅.
					if( map_itor->first == regen_itor->second.iMapNo)
					{
						kContThisRegen.insert(std::make_pair(regen_itor->first, regen_itor->second));
					}
					++regen_itor;
				}
			}

			typedef std::map<int, int> CONT_DROPS;//아이템번호, 수량/
			CONT_DROPS kContDrops_Item;
			CONT_DROPS kContDrops_Mon;
			CONT_DROPS kContDrops_Money;
			
			CONT_DROPS kContDrops_ItemType;//장비. 소비 등등의 타입.
			CONT_DROPS kContDrops_ItemGrade;//아이템 등급..

			size_t loof_count = 10000;
			size_t const total_loof_count = loof_count;

			while(--loof_count)
			{
				CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = kContThisRegen.begin();
				while(regen_itor != kContThisRegen.end())
				{
					TBL_DEF_MAP_REGEN_POINT const &kTbl = (*regen_itor).second;
					CONT_DEF_MONSTER_BAG::const_iterator monbag_itor = pkContMonsterBag->find(kTbl.iBagControlNo);
					size_t iOutIDX = 0;
					if(	monbag_itor != pkContMonsterBag->end()//값 잘못 넣으면..
					&&	::RouletteRate((*monbag_itor).second.iRateNo, iOutIDX, 10))
					{
						CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itor = pkContMonsterBagElement->find((*monbag_itor).second.iElementNo);
						if(element_itor != pkContMonsterBagElement->end())
						{
							int const iMonNo = (*element_itor).second.aElement[iOutIDX];

							PgMonster kMon;
							kMon.SetAbil(AT_CLASS, iMonNo);

							PgAction_InitUnitDrop kAction((*map_item_bag_Itor).second);
							kAction.DoAction(&kMon, NULL);

//////////////////////////////////////////////////////////////////
//기록 몬스터 출현.
							{
								auto ret = kContDrops_Mon.insert(std::make_pair(iMonNo, 1));
								if(!ret.second)
								{//이미 있음.
									++(*ret.first).second;//수량 증가.
								}
							}
//////////////////////////////////////////////////////////////////
//기록 드랍아이템
							CONT_MONSTER_DROP_ITEM kOut;
							kMon.PopDropItem(kOut);

							CONT_MONSTER_DROP_ITEM::iterator ret_itor = kOut.begin();
							while(kOut.end() != ret_itor)
							{
								if((*ret_itor).ItemNo())
								{
									auto ret = kContDrops_Item.insert(std::make_pair((*ret_itor).ItemNo(), 1));
									if(!ret.second)
									{//이미 있음.
										++(*ret.first).second;//수량 증가.
									}
								}
								else
								{
									int const iMoney = (int)(*ret_itor).EnchantInfo().Field_1();
									auto ret = kContDrops_Money.insert(std::make_pair(iMoney, 1));
									if(!ret.second)
									{//이미 있음.
										++(*ret.first).second;//수량 증가.
									}
								}

								++ret_itor;
							}
//////////////////////////////////////////////////////////////////
						}
					}
				
					++regen_itor;
				}
			}

			BM::vstring kMapName((int)map_itor->first);
			kMapName += _T(".txt");

			BM::CDebugLog kLog;
			kLog.Init(BM::OUTPUT_JUST_FILE|BM::OUTPUT_IGNORE_LOG_HEAD, BM::LFC_WHITE, _T("./static"),	kMapName);
			kLog.SetLocale("kor");
			
			{
				CONT_DROPS::const_iterator drops_itor = kContDrops_Mon.begin();
				while(drops_itor != kContDrops_Mon.end())
				{
					CMonsterDef const * pkMonsterDef = kMonsterDefMgr.GetDef((*drops_itor).first);

					std::wstring kTemp;
					
					if(pkMonsterDef)
					{	
						GetDefString(pkMonsterDef->NameNo(), kTemp);
					}

					kLog.Log(BM::LOG_LV0, L"MonNo:	%s	%d	Count:	%d", kTemp.c_str(), (*drops_itor).first, (*drops_itor).second);

					++drops_itor;
				}
			}
			{
				CONT_DROPS::const_iterator drops_itor = kContDrops_Item.begin();
				while(drops_itor != kContDrops_Item.end())
				{
					CItemDef const *pkItemDef = kItemDefMgr.GetDef((*drops_itor).first);
					std::wstring kTemp;
					
					if(pkItemDef)
					{	
						GetDefString(pkItemDef->NameNo(), kTemp);
					}

					kLog.Log(BM::LOG_LV0, L"ItemNo:	%s	%d	Count:	%d", kTemp.c_str(), (*drops_itor).first, (*drops_itor).second);

					++drops_itor;
				}
			}
			{
				CONT_DROPS::const_iterator drops_itor = kContDrops_Money.begin();
				while(drops_itor != kContDrops_Money.end())
				{
					kLog.Log(BM::LOG_LV0, L"Money:		%d	Count:	%d",(*drops_itor).first, (*drops_itor).second);
					++drops_itor;
				}
			}
			
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Total
			
			kLog.Log(BM::LOG_LV0, L"Total Loof Count:	%d", total_loof_count);
			{
				CONT_DROPS::const_iterator drops_itor = kContDrops_Mon.begin();
				__int64 iTotal = 0;
				while(drops_itor != kContDrops_Mon.end())
				{
					iTotal += (*drops_itor).second;
					++drops_itor;
				}
				kLog.Log(BM::LOG_LV0, L"Total Mon Count:	%I64d",iTotal);
			}
			{
				CONT_DROPS::const_iterator drops_itor = kContDrops_Item.begin();
				__int64 iTotal = 0;
				while(drops_itor != kContDrops_Item.end())
				{
					iTotal += (*drops_itor).second;
					++drops_itor;
				}
				kLog.Log(BM::LOG_LV0, L"Total Item Count:	%I64d",iTotal);
			}
			{
				CONT_DROPS::const_iterator drops_itor = kContDrops_Money.begin();
				__int64 iTotal = 0;
				while(drops_itor != kContDrops_Money.end())
				{
					iTotal += ((*drops_itor).first * (*drops_itor).second);
					++drops_itor;
				}
				kLog.Log(BM::LOG_LV0, L"Total Money:	%I64d",iTotal);
			}
		}
		++map_itor;
	}
	*/
}
	return false;
}

bool CALLBACK OnF3( WORD const &rkInputKey)
{
/*	std::cout<< __FUNCTION__ << std::endl;

	PgPlayer kPlayer;
	kPlayer.SetID(BM::GUID::Create());
	kPlayer.SetRandomSeed(g_kEventView.GetServerElapsedTime());
	std::vector<int> kSaveRandom;
	kSaveRandom.resize(100);
	for (int i=0; i<100; i++)
	{
		kSaveRandom.at(i) = 0;
	}
	for (int j=0; j<10000; j++)
	{
		DWORD dwRandom = kPlayer.GetRandom();
		INFO_LOG(BM::LOG_LV6, _T("Random Value = ") << dwRandom);
		dwRandom = dwRandom % 100;
		kSaveRandom.at(dwRandom) += 1;
	}
	for (int k=0; k<100; k++)
	{
		INFO_LOG(BM::LOG_LV6, _T("Random Value Index[") << k << _T("]=") << kSaveRandom.at(k));
	}*/
	return false;
}
bool CALLBACK OnF4( WORD const &rkInputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	SHORT sState = GetKeyState(VK_SHIFT);
	if (HIBYTE(sState) == 0)
	{
		S_STATE_LOG(BM::LOG_LV0, _T("---Memory Observe---"));
#ifdef _MEMORY_TRACKING
		g_kObjObserver.DisplayState(g_kLogWorker, LT_S_STATE);
		if (g_pkMemoryTrack)
		{
			g_pkMemoryTrack->DisplayState(g_kLogWorker, LT_S_STATE);
		}
#endif
	}
	else
	{
#ifdef _MEMORY_TRACKING
		sState = GetKeyState(VK_CONTROL);
		if (HIBYTE(sState) == 0)
		{
			return false;
		}
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING] MapServer will display MemoryAlloced, It takes some time OK??","DRAGONICA_Map",MB_OKCANCEL) )
		{
			g_kObjObserver.DisplayState(g_kLogWorker, LT_S_STATE);
			if (g_pkMemoryTrack)
			{
				g_pkMemoryTrack->DisplayState(g_kLogWorker, LT_S_STATE, true);
			}
		}
#endif
	}


//	while()
//	{
//		
//	}

	return false;
}

bool CALLBACK OnF5( WORD const &rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, __FL__);

	/*
	ConStage kConStage;

	SMissionStageKey kStage1;
	kStage1.kBitFalg = 1;
	kStage1.iGroundNo = 9010302;

	SMissionStageKey kStage2;
	kStage2.kBitFalg = 0;
	kStage2.iGroundNo = 9010309;

	kConStage.push_back(kStage1); 
	kConStage.push_back(kStage2); 

	int i = 1000;
	while(i--)
	{
		PgMissionGround *pkMission = new PgMissionGround;

		pkMission->GroundKey( 9010302 );
		pkMission->SetMapLevel( 1 );
		pkMission->GroundWeight( 0 );

		PgGroundResource const *pkGndRsc = NULL;
		g_kGndMgr.m_kGndRscMgr.Locked_GetGroundResource( kConStage.begin()->iGroundNo, pkGndRsc );

		pkMission->CloneResource( pkGndRsc );

		pkMission->Init(2, false);

		ConStage::const_iterator stage_itr = kConStage.begin();
		for( ; stage_itr!=kConStage.end(); ++stage_itr )
		{
			if(S_OK == g_kGndMgr.m_kGndRscMgr.Locked_GetGroundResource(stage_itr->iGroundNo, pkGndRsc))
			{
//				pkMission->AddStage( pkGndRsc );
			}
		}

		delete pkMission;
	}
	*/
	return true;
}

bool CALLBACK OnF6( WORD const &rkInputKey)
{
	if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	{
#ifndef _MDo_
		INFO_LOG(BM::LOG_LV1, __FL__<<L"0. Start Reload Quest");
		INFO_LOG(BM::LOG_LV1, __FL__<<L"1. Resync XML");

		g_kQuestMan.Reload();
#endif
	}
	return true;
}

bool CALLBACK OnF7( WORD const &rkInputKey)
{
	//INFO_LOG(BM::LOG_LV4, _T("Quest Memory Status ------------------------------------------"));

	//g_kQuestMan.TotalSize();

	return true;
}

bool CALLBACK OnF8( WORD const &rkInputKey)
{
	INFO_LOG(BM::LOG_LV7, "Reload lua...");
	lua_tinker::call<void>("SelfReload");
	lua_tinker::call<void>("Reload");
	INFO_LOG(BM::LOG_LV5, "Reload success!");
	return true;
}

extern bool GlobalInit(bool const bKey);
bool CALLBACK OnF9( WORD const &rkInputKey)
{
	GlobalInit(true);
	return false;
}

bool CALLBACK OnF10(WORD const& rkInputKey)
{
	return false;
}	

bool CALLBACK OnTerminateServer(WORD const& rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG(BM::LOG_LV6, _T("[MapServer] will be shutdown"));
	INFO_LOG(BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));
	g_kGndMgr.Release();
	g_kConsoleCommander.StopSignal(true);
	INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
	return false;
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] Map Server will be CRASHED.. are you sure??","DRAGONICA_Map",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] Map Server will be CRASHED.. are you sure??","DRAGONICA_Map",MB_OKCANCEL) )
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Server crashed by INPUT");
//			::RaiseException(1,  EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
			int * p = NULL;
			*p = 1;
		}
	}
	return false;
}

bool RegistKeyEvent()
{
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	g_kConsoleCommander.Regist( VK_F1,	OnF1 );
	g_kConsoleCommander.Regist( VK_F2,	OnF2 );
	g_kConsoleCommander.Regist( VK_F3,	OnF3 );
	g_kConsoleCommander.Regist( VK_F4,	OnF4 );
	g_kConsoleCommander.Regist( VK_F5,	OnF5 );
	g_kConsoleCommander.Regist( VK_F6,	OnF6 );
	g_kConsoleCommander.Regist( VK_F7, OnF7 );
	g_kConsoleCommander.Regist( VK_F8, OnF8 );
	g_kConsoleCommander.Regist( VK_F9, OnF9 );
	g_kConsoleCommander.Regist( VK_F10, OnF10 );
	g_kConsoleCommander.Regist( VK_F11, OnTerminateServer );
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}
