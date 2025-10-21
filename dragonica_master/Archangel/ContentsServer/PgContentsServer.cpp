#include "stdafx.h"
#include "CSMail/SendMail.h"
#include "Lohengrin/PgRealmManager.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgDBCache.h"
#include "Variant/PgEventview.h"
#include "Variant/PgMCtrl.h"
#include "Variant/PgClassPetDefMgr.h"
#include "PgKeyEvent.h"
#include "PgDBProcess.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromItem.h"
#include "PgGMProcessMgr.h"
#include "PgServerSetMgr.h"
#include "PgTimer.h"
#include "PgEventDoc.h"
#include "PgRecvFromImmigration.h"
#include "PgGuildMgr.h"
#include "PgRecvFromLog.h"
#include "PgRecvFromManagementServer.h"
#include "JobDispatcher.h"
#include "PgCoupleMgr.h"
#include "PgTask_Contents.h"
#include "PgEmporiaMgr.h"
#include "Variant/PgCheckMacAddress.h"

extern void ReloadAntiHackMgr();
extern HRESULT CALLBACK OnDB_EXECUTE( CEL::DB_RESULT &rkResult );
extern HRESULT CALLBACK OnDB_EXECUTE_TRAN( CEL::DB_RESULT_TRAN &rkResult );

const wchar_t* GetCsvPathByDBType(const int)
{
	return _T("Query/");
}

bool CsvQueryIsDBSupport(int iBDIndex)
{
	return iBDIndex == DT_DEF || iBDIndex == DT_LOCAL;
}

/*
// 미션 아이템 드랍율 출력용 기획 요청 코드(박일환씨용)
void MakeItemDropFile()
{
	const CONT_DEF_MISSION_ROOT* pContDefMission = NULL;
	g_kTblDataMgr.GetContDef(pContDefMission);

	const CONT_DEF_MISSION_RESULT* pkContMissionResult = NULL;
	g_kTblDataMgr.GetContDef(pkContMissionResult);

	CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;
	g_kTblDataMgr.GetContDef(pContContainer);

	const CONT_DEF_SUCCESS_RATE_CONTROL* pContSuccess = NULL;
	g_kTblDataMgr.GetContDef(pContSuccess);

	const CONT_DEF_ITEM_BAG_GROUP* pContItemBagGroup = NULL;
	g_kTblDataMgr.GetContDef(pContItemBagGroup);

	const CONT_DEF_ITEM_BAG * pContItemBag = NULL;
	g_kTblDataMgr.GetContDef(pContItemBag);

	const CONT_DEF_ITEM_BAG_ELEMENTS * pContElement = NULL;
	g_kTblDataMgr.GetContDef(pContElement);

	std::string kMissionNo;
	std::string kMissionLevel;
	std::string kMissionRank;
	std::string kDropItemRate;

	std::ofstream kFile("missionitemdroprate.txt",std::ios::binary);
	
	for(CONT_DEF_MISSION_ROOT::const_iterator mission_itor = pContDefMission->begin();mission_itor != pContDefMission->end();++mission_itor)
	{
		const TBL_DEF_MISSION_ROOT* pMission = &(*mission_itor).second;

		kMissionNo = std::string(MB(BM::vstring(pMission->iMissionNo)));

		for(int ml = 0;ml < MAX_MISSION_LEVEL;++ml)
		{
			CONT_DEF_MISSION_RESULT::const_iterator rst_itr = pkContMissionResult->find(pMission->aiMissionResultNo[ml]);
			if ( rst_itr == pkContMissionResult->end() )
			{
				continue;
			}

			TBL_DEF_MISSION_RESULT const & kResult = (*rst_itr).second;

			kMissionLevel = std::string(MB(BM::vstring(ml)));

			for(int rl = 0;rl < MAX_MISSION_RANK_CLEAR_LV;++rl)
			{
				CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find(kResult.aiResultContainer[rl]);

				if(item_cont_itor == pContContainer->end())
				{
					continue;
				}

				TBL_DEF_ITEM_CONTAINER const & kItemCont = (*item_cont_itor).second;

				kMissionRank = std::string(MB(BM::vstring(rl)));

				CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator success_itor = pContSuccess->find((*item_cont_itor).second.iSuccessRateControlNo);
				if(success_itor == pContSuccess->end())
				{
					continue;
				}

				TBL_DEF_SUCCESS_RATE_CONTROL const & kSRate = (*success_itor).second;

				for(int sl = 0;sl < MAX_SUCCESS_RATE_ARRAY;++sl)
				{
					if(0 == kSRate.aRate[sl])
					{
						continue;
					}

					double const kBagGroupRate = static_cast<double>(kSRate.aRate[sl])/static_cast<double>(kSRate.iTotal);

					CONT_DEF_ITEM_BAG_GROUP::const_iterator baggroup_itor = pContItemBagGroup->find(kItemCont.aiItemBagGrpNo[sl]);
					if(baggroup_itor == pContItemBagGroup->end())
					{
						continue;
					}

					TBL_DQT_DEF_ITEM_BAG_GROUP const & kBGroup = (*baggroup_itor).second;

					CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator bagsuccess_itor = pContSuccess->find(kBGroup.iSuccessRateNo);
					if(bagsuccess_itor == pContSuccess->end())
					{
						continue;
					}

					TBL_DEF_SUCCESS_RATE_CONTROL const & kBSRate = (*bagsuccess_itor).second;

					for(int gsl = 0;gsl < MAX_SUCCESS_RATE_ARRAY;++gsl)
					{
						if(0 == kBSRate.aRate[gsl])
						{
							continue;
						}
						double const kBagRate = static_cast<double>(kBSRate.aRate[gsl])/static_cast<double>(kBSRate.iTotal);

						for(int bgl = 0;bgl < MAX_ITEM_BAG_ELEMENT;++bgl)
						{
							CONT_DEF_ITEM_BAG::const_iterator item_iter = pContItemBag->find(kBGroup.aiBagNo[bgl]);
							if(item_iter == pContItemBag->end())
							{
								continue;
							}

							TBL_DEF_ITEM_BAG const & kItemBag = (*item_iter).second;
							
							CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator Itemsuccess_itor = pContSuccess->find(kItemBag.iRaseRateNo);
							if(Itemsuccess_itor == pContSuccess->end())
							{
								continue;
							}

							TBL_DEF_SUCCESS_RATE_CONTROL const & kESRate = (*Itemsuccess_itor).second;
							CONT_DEF_ITEM_BAG_ELEMENTS::const_iterator element_itor = pContElement->find(kItemBag.iElementsNo);

							if(element_itor == pContElement->end())
							{
								continue;
							}

							TBL_DEF_ITEM_BAG_ELEMENTS const & kElement = (*element_itor).second;

							kDropItemRate.clear();

							for(int el = 0;el < MAX_SUCCESS_RATE_ARRAY;++el)
							{
								if(0 == kESRate.aRate[el])
								{
									continue;
								}
								double const kElementRate = static_cast<double>(kESRate.aRate[el])/static_cast<double>(kESRate.iTotal);
								int const iItemNo = kElement.aElement[el];// 아이템 번호
								double const kFinalDropRate = kBagGroupRate * kBagRate * kElementRate * 100.0;

								kDropItemRate += std::string(MB(BM::vstring(iItemNo)));
								kDropItemRate += ",";

								kDropItemRate += std::string(MB(BM::vstring(kFinalDropRate)));
								kDropItemRate += ",";
							}

							if(kDropItemRate.length() > 0)
							{
								kFile << kMissionNo.c_str();
								kFile << ",";
								kFile << kMissionLevel.c_str();
								kFile << ",";
								kFile << kMissionRank.c_str();
								kFile << ",";
								kFile << kDropItemRate.c_str() << std::endl;
							}
						}
					}
				}
			}
		}
	}

	kFile.close();
}
*/

void CALLBACK OnRegist( CEL::SRegistResult const &rkArg )
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("OnRegist Failed Type[") << rkArg.eType << L"], Ret[" << rkArg.iRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! RegistResult isn't Success"));
		return;
	}

	if( CEL::RT_CONNECTOR == rkArg.eType )
	{
		switch(rkArg.iIdentityValue)
		{
		case CEL::ST_LOG:
			{
				g_kProcessCfg.LogConnector(rkArg.guidObj);
			}break;
//		case CEL::ST_CENTER:
//			{	// Center 끼리 접속하기 위한 Connector 등록 완료
//				g_kRealmMgr.Connector(rkArg.guidObj);
//				g_kRealmMgr.ConnectAllCenter(rkArg.guidObj);
//			}break;
		case CEL::ST_IMMIGRATION:
			{
				g_kProcessCfg.ImmigrationConnector(rkArg.guidObj);
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ Connector!!!! ======================") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
	}

	if (CEL::RT_ACCEPTOR == rkArg.eType)
	{
		// Center 서버끼리 접속하기 위한 Connector 설정
		// Connector 가 ACCEPTOR Reg 이후에 호출되어야 접속 실패하는 Center가 발생하지 않는다.
//		CEL::INIT_CORE_DESC kCenterConnectorInit;
//		kCenterConnectorInit.OnSessionOpen	= OnConnectFromServer;
//		kCenterConnectorInit.OnDisconnect	= OnDisconnectFromServer;
//		kCenterConnectorInit.OnRecv			= OnRecvFromServer;
//		kCenterConnectorInit.bIsImmidiateActivate = true;
//		kCenterConnectorInit.kOrderGuid.Generate();
//		kCenterConnectorInit.IdentityValue(CEL::ST_CENTER);
//
//		g_kRealmMgr.ConnectorRegistOrder(kCenterConnectorInit.kOrderGuid);
		
//		g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kCenterConnectorInit, sizeof(kCenterConnectorInit));		
	}
	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		switch(rkArg.iIdentityValue)
		{
		case 1:
			{
				if(LoadDB())
				{
					CEL::REGIST_TIMER_DESC kInit100ms;
					kInit100ms.pFunc		= Timer100ms;
					kInit100ms.dwInterval	= 1000;

					CEL::REGIST_TIMER_DESC kInit1s;
					kInit1s.pFunc		= Timer1s;
					kInit1s.dwInterval	= 1000;

					CEL::REGIST_TIMER_DESC kInit10s;
					kInit10s.pFunc		= Timer10s;
					kInit10s.dwInterval	= 10000;

					CEL::REGIST_TIMER_DESC kInit1m;
					kInit1m.pFunc		= Timer1m;
					kInit1m.dwInterval	= 60000;

					CEL::REGIST_TIMER_DESC Init5m;
					Init5m.pFunc		= Timer5m;
					Init5m.dwInterval	= 300000;

					CEL::REGIST_TIMER_DESC kInit1m_2;
					kInit1m_2.pFunc		= Timer1m_2;
					kInit1m_2.dwInterval	= 60000;

					CEL::REGIST_TIMER_DESC kInitCounter;
					kInitCounter.pFunc		= Timer30s;
					kInitCounter.dwInterval	= 1000 * 30;		// 30초

					CEL::REGIST_TIMER_DESC kInitDay;
					kInitDay.pFunc = TimerDay;
					kInitDay.dwInterval = 60000;				// 1분

					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit100ms);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit1s);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit10s);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit1m);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &Init5m);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit1m_2);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInitCounter);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInitDay);

					CEL::INIT_FINAL_SIGNAL kInitFinal;
					kInitFinal.kIdentity = 2;
					g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

					INFO_LOG( BM::LOG_LV6, __FL__ << _T("Ready to Service Start") );
				}
				else
				{
					ASSERT_LOG(false, BM::LOG_LV1, __FL__ << _T("LoadDB failed"));
				}


				CleanDeleteCharacter();
				RealmUserManagerUtil::ClearCharacterConnectionChannel(); // 모든 캐릭터 채널 접속정보 초기화
			}break;
		case 2:
			{
				if(g_kCoreCenter.SvcStart())
				{
					INFO_LOG( BM::LOG_LV6, _T("==================================================") );
					INFO_LOG( BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
					INFO_LOG( BM::LOG_LV6, _T("==================================================") );

					g_kProcessCfg.Locked_ConnectImmigration();
					g_kContentsTask.Start();
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CoreCenter Connot Run!!!"));
					ASSERT_LOG(false, BM::LOG_LV1, __FL__ << _T("CoreCenter Cannot Run!!!"));
				}
			}break;
		}
	}
}

namespace open_market
{
extern void load_def_strings();
}

bool LoadXML()
{
	if( !g_kFilterString.ParseXml(_T("XML/BadWord.xml")) )
	{ 
		VERIFY_INFO_LOG( false, BM::LOG_LV1, _T("Cannot Read XML/BadWord.xml") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Cannot Read XML/BadWord.xml"));
	}
	//if( !g_kFilterString.ParseXml(_T("XML/GoodWord.XML")) )			
	//{ 
	//	VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("Cannot Read XML/BadWord.xml"));
	//	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Cannot Read XML/BadWord.xml"));
	//}
	//if( !g_kFilterString.ParseXml(_T("XML/IgnoreWord.xml")) )			
	//{ 
	//	VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("Cannot Read XML/BadWord.xml"));
	//	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Cannot Read XML/BadWord.xml"));
	//}
	if( !g_kFilterString.ParseXml(_T("XML/BlockNickName.xml")) )	
	{ 
		VERIFY_INFO_LOG( false, BM::LOG_LV1, _T("Cannot Read XML/BlockNickName.xml") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Cannot Read XML/BlockNickName.xml"));
	}

	g_kTblDataMgr.LoadAllDefStringXMLInFolder(L"XML\\DefStrings\\");
	open_market::load_def_strings();
	return true;
}

HRESULT ReadConfigFile(LPCTSTR lpszFileName)
{
	BM::vstring kCategory(L"REALM_");
	kCategory += g_kProcessCfg.RealmNo();
	int const iValue = GetPrivateProfileInt(kCategory.operator const wchar_t *(), _T("MAX_USER_COUNT"), 3000, lpszFileName);
	g_kRealmUserMgr.MaxPlayerCount(iValue);
	
	INFO_LOG( BM::LOG_LV7, __FL__ << _T("USER_COUNT_MAX_NUM_CHANNEL : ") << iValue );

	return S_OK;
}

bool GlobalInit()
{
	g_kTerminateFunc = OnTerminateServer;//Init MCtrl	
	g_kGetMaxUserFunc = GetMaxUser;
	g_kGetNowUserFunc = GetConnectionUser;
	g_kSetMaxUserFunc = SetMaxUser;

	//CEL_LOG_INIT();

	if( !g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Contents_constant.ini")) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't load Contents_constant.ini file") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !RealmUserManagerUtil::InitConstantValue() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !PgQueryChecker::InitConstantValue() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !PgCoupleMgr::InitConstantValue() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !PgGuildMgrUtil::InitGuildConstant() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iValue = 0;
	if ( S_OK == g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Maximum_Level, iValue) && iValue > 0)
	{
		PgClassDefMgr::s_sMaximumLevel = iValue;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot read MaxLevel in INI file"));
		return false;
	}

	if( S_OK == g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Item_NationCode, iValue) && 0 < iValue )
	{
		DBCacheUtil::iForceNationCode = iValue;
	}

	SSyncVariable kSyncVariable;
	g_kVariableContainer.Get(EVar_Kind_ExpBuff_Logoff, EVar_ExpAdd_MaxExperienceRate, kSyncVariable.iExpAdd_MaxExperienceRate);
	g_kVariableContainer.Get(EVar_Kind_ExpBuff_Logoff, EVar_ExpAdd_AddedExp_Rate_Hunting, kSyncVariable.iExpAdd_AddedExpRate_Hunting);
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_Maximum_Level, kSyncVariable.iMaxCharacterLevel);
	g_kVariableContainer.Get(Evar_Kind_CashShop, EVar_CashShop_Open, kSyncVariable.bCashShopOpen );	
	g_kVariableContainer.Get(EVar_Kind_SpecificReward, EVar_Revive_Feather_Login_Time, kSyncVariable.iReviveFeather_Login_Time);
	g_kVariableContainer.Get(EVar_Kind_SpecificReward, EVar_Revive_Feather_LevelUp_Use, kSyncVariable.iReviveFeather_LevelUp_Use);
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_JobSkill_ExhaustionGap, kSyncVariable.iJobSkill_ExhaustionGap);
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_JobSkill_ExhaustionRestore, kSyncVariable.iJobSkill_ExhaustionRestore);
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_JobSkill_BlessRate , kSyncVariable.iJobSkill_BlessRate);
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_JobSkill_BlessRate_DurationSec , kSyncVariable.iJobSkill_BlessRate_DurationSec);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Max_StrategyFatigability, kSyncVariable.iMaxStrategyFatigability);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Dec_Per_Stage_1, kSyncVariable.iDecPerStageFatigability_1);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Dec_Per_Stage_2, kSyncVariable.iDecPerStageFatigability_2);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Dec_Per_Stage_3, kSyncVariable.iDecPerStageFatigability_3);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Dec_Per_Stage_4, kSyncVariable.iDecPerStageFatigability_4);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Bonus_Exp_Rate, kSyncVariable.fBonusExpRate);
	g_kVariableContainer.Get(EVar_Kind_StrategyFatigability, EVar_Fatigability_Div_Value, kSyncVariable.iFatigabilityDivValue);
	g_kVariableContainer.Get(EVar_Kind_LoginEventSpecific, EVar_SpecificRewardNotifyMessageInterval, kSyncVariable.SpecificRewardNotifyMessageInterval);
	g_kVariableContainer.Get(EVar_Kind_Community_Event_Time, eVar_StartWaitTime, kSyncVariable.CommunityEventWaitTime);
	g_kVariableContainer.Get(EVar_Kind_Community_Event_Time, eVar_ReadyTime, kSyncVariable.CommunityEventReadyTime);

	if( 0 == kSyncVariable.CommunityEventWaitTime )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<< L"Can't find EVar_Kind_Community_Event_Time, eVar_StartWaitTime Value");
		kSyncVariable.CommunityEventWaitTime = 300;
	}

	if( 0 == kSyncVariable.CommunityEventReadyTime )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't find EVar_Kind_Community_Event_Time, eVar_ReadyTime Value");
		kSyncVariable.CommunityEventReadyTime = 10;
	}

	g_kEventView.VariableCont(kSyncVariable);

	{
		if( 0 == kSyncVariable.iJobSkill_ExhaustionGap )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't find 'EVar_JobSkill_ExhaustionGap' Value");
			return false;
		}
		if( 0 == kSyncVariable.iJobSkill_ExhaustionRestore )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't find 'EVar_JobSkill_ExhaustionRestore' Value");
			return false;
		}
		if( 0 == kSyncVariable.iJobSkill_BlessRate )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't find 'EVar_JobSkill_BlessRate' Value");
			return false;
		}
		if( 0 == kSyncVariable.iJobSkill_BlessRate_DurationSec )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't find 'EVar_JobSkill_BlessRate_DurationSec' Value");
			return false;
		}
		if( 0 == kSyncVariable.iMaxStrategyFatigability )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't find 'EVar_StrategyFatigability' Value");
			return false;
		}
	}

	ReloadAntiHackMgr();

	PgEmporiaMgr::LoadEmporiaBattleTime();
	ItemCountLogHelper::Init();

	{// Log
		int iValue_OutPut = BM::OUTPUT_ALL;
		int iValue_Level = static_cast<int>(BM::LOG_LV9);

		g_kVariableContainer.Get(EVar_Kind_Log, EVar_LogOutputType, iValue_OutPut);
		g_kVariableContainer.Get(EVar_Kind_Log, EVar_LogLevelLimit, iValue_Level);

		iValue_OutPut &= BM::OUTPUT_ALL;

		SetLogOutPutType(static_cast<BM::E_OUPUT_TYPE>(iValue_OutPut));
		SetLogLeveLimit(static_cast<BM::E_LOG_LEVEL>(iValue_Level));

		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Load EVar_LogOutputType = ") << iValue_OutPut);
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Load EVar_LogLevelLimit = ") << iValue_Level);
	}
	
//	g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("./Contents_Constant.ini"));
//	ReloadAntiHackMgr();
	return true;
}

int CALLBACK ExceptionTerminate(void)
{
	g_kLogWorker.VDeactivate();
	return 0;
}

class PgLogDeactivator
{
public:
	PgLogDeactivator(){}
	~PgLogDeactivator()
	{
		g_kLogWorker.VDeactivate();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	g_kCoreCenter;//인스턴스 생성.

	PgLogDeactivator kLogDeactivator;

	PgItem_PetInfo::SStateValue kValue;
	kValue.Time(INT_MAX);

	SYSTEMTIME kTime;
	kTime.wYear = 2010;
	kTime.wMonth = 4;
	kTime.wDay = 1;
	kTime.wHour = 0;
	kTime.wMinute = 0;
	kTime.wSecond = 0;
	kTime.wMilliseconds = 0;
	kTime.wDayOfWeek =0;

	__int64 i64Time;
	CGameTime::SystemTime2SecTime( kTime, i64Time, CGameTime::MINUTE );

//	int iValue = 134217727;

	CGameTime::SecTime2SystemTime( static_cast<__int64>(kValue.Time()) + i64Time, kTime, CGameTime::MINUTE );

#ifndef _DEBUG
	PgExceptionFilter::Install( PgExceptionFilter::DUMP_LEVEL_HEAVY, PgExceptionFilter::DUMP_OP_Exit_Program|PgExceptionFilter::DUMP_OP_UseTimeInDumpFile, ExceptionTerminate );
#endif

	g_kProcessCfg.Locked_SetConfigDirectory();
	if(!g_kLocal.LoadFromINI(g_kProcessCfg.ConfigDir() + L"Local.ini"))
	{
		std::cout << "Load From \"Config/local.ini\" Error!!" << std::endl;
		system("pause");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	g_kProcessCfg.Locked_SetServerType(CEL::ST_CONTENTS);
	if( !g_kProcessCfg.Locked_ParseArg(argc, argv) )
	{
		BM::vstring arg;
		for ( int i = 1; i< argc ; ++i )
		{
			arg += argv[i];
			arg += _T(" ");
		}
		CAUTION_LOG( BM::LOG_LV0, _T("Argment Error! ") << arg );
		system("pause");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	std::wstring kLogFolder;
	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_CONTENTS%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL );

#ifndef _DEBUG
		if ( !(CProcessConfig::E_RunMode_Debug & g_kProcessCfg.RunMode()) )
		{
			INFO_LOG_LEVEL(BM::LOG_LV7);
		}
#endif
	}

	if( !GlobalInit() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	if(!LoadXML())
	{
		CAUTION_LOG( BM::LOG_LV0, _T("Load XML Failed") );
		system("pause");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	
	if (!lwOnibal::InitOnibal())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"failed to initializing Onibal Scripting");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	if (!lwOnibal::InitOnibalServerSide())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"failed to initializing Onibal Server Scripting");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	lua_wrapper_user kLua(g_kLuaTinker);
	lua_tinker::dofile(*kLua, "./Script/init.lua");

	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	INFO_LOG( BM::LOG_LV6, _T("==================== Server Initialize Start ====================") );
	INFO_LOG( BM::LOG_LV6, _T("Product Version : ") << chProductVersion );
	INFO_LOG( BM::LOG_LV6, _T("File Version : ") << chFileVersion );
	INFO_LOG( BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S );

	CEL::INIT_CENTER_DESC kCenterInit;

	kCenterInit.eOT = BM::OUTPUT_ALL;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.bIsUseDBWorker = true;
	kCenterInit.dwProactorThreadCount = 15;//유저 서비스 
	kCenterInit.m_kCelLogFolder = kLogFolder;

	g_kCoreCenter.Init(kCenterInit);

	g_kJobDispatcher.VInit(15);
	g_kJobDispatcher.VActivate();

	CEL::INIT_CORE_DESC kLogConnectorInit;
	kLogConnectorInit.OnSessionOpen	= OnConnectToLog;
	kLogConnectorInit.OnDisconnect	= OnDisconnectFromLog;
	kLogConnectorInit.OnRecv			= OnRecvFromLog;
	kLogConnectorInit.kOrderGuid.Generate();
	kLogConnectorInit.IdentityValue(CEL::ST_LOG);
	kLogConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);

	CONT_DB_INIT_DESC kContDBInit;

	std::wstring kPatch = g_kProcessCfg.ConfigDir() + _T("Contents_Config.ini");
	if(FAILED(ReadConfigFile(kPatch.c_str())))
	{
		CAUTION_LOG( BM::LOG_LV0, _T("Load From \"Config/Contents_Config.ini\" Error!!") );
		system("pause");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	if (!g_kEventView.ReadConfigFile(kPatch.c_str()))
	{
		CAUTION_LOG( BM::LOG_LV0, _T("Load From \"Config/Contents_Config.ini\" Error!!") );
		system("pause");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	if(g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_ReadIni)
	{
		if( !ReadDBConfig( EConfigType_Ini, g_kProcessCfg.ConfigDir(), g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Load From \"Config/Contents_DB_Config.ini\" Error!!") );
			system("pause");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0;
		}
	}
	else
	{
		if( !ReadDBConfig( EConfigType_Inb, g_kProcessCfg.ConfigDir(), g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Load From \"Config/Contents_DB_Config.ini\" Error!!") );
			system("pause");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0;
		}
	}
	
	CONT_DB_INIT_DESC::iterator dbinit_itor = kContDBInit.begin();
	while(dbinit_itor != kContDBInit.end())
	{
		CEL::INIT_DB_DESC &kDBInit = (*dbinit_itor);

		if(kDBInit.kDBInfo.DBIndex() == DT_PLAYER)	// 임시 처리 
		{
			//			kDBInit.dwThreadCount = 1;
			kDBInit.dwConnectionPoolCount =  30;//커넥션풀을 100개 가지도록.
		}

		kDBInit.bUseConsoleLog = true;
		kDBInit.OnDBExecute = OnDB_EXECUTE;
		kDBInit.OnDBExecuteTran = OnDB_EXECUTE_TRAN;
		kDBInit.m_kLogFolder = kLogFolder;

		g_kCoreCenter.Regist(CEL::RT_DB_WORKER, &kDBInit);
		++dbinit_itor;
	}

	CEL::INIT_CORE_DESC kImmConnectorInit;
	kImmConnectorInit.OnSessionOpen	= OnConnectFromImmigration;
	kImmConnectorInit.OnDisconnect	= OnDisconnectFromImmigration;
	kImmConnectorInit.OnRecv		= OnRecvFromImmigration;
	kImmConnectorInit.kOrderGuid.Generate();
	kImmConnectorInit.IdentityValue(CEL::ST_IMMIGRATION);
	kImmConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);

	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kLogConnectorInit);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kImmConnectorInit);

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 1;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	RegistKeyEvent();

	g_kProcessCfg.Locked_SetConsoleTitle();

	std::cout<< "-- Started --" << std::endl;

	g_kConsoleCommander.MainLoof();

	g_kContentsTask.Close();
	g_kJobDispatcher.VDeactivate();
	//g_kItemMgr.Close();
	//g_kInvenSerializer.Stop();
	// 서비스 종료
	//g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();

	return 0;
}
//*
typedef enum
{
	EVKind_None = 0,
	EVKind_AntiHack = 5,
} EVariable_Kind;

void ReloadAntiHackMgr()
{
	int iMax = 0;
	if (g_kVariableContainer.Get(EVKind_AntiHack, 1, iMax) == S_OK)
	{
		g_kAntiHackMgr.Locked_SetMaxHackIndex(iMax);
	}
	for (int i=1; i<=EAHP_POLICY_MAX; i++)
	{
		int index = i*100;
		SHackPolicy kPolicy;
		int iValue;
		float fValue;
		if (g_kVariableContainer.Get(EVKind_AntiHack, index++, iValue) == S_OK)
		{
			// EHack_Var_Use
			kPolicy.bUse = (iValue==1) ? true : false;
		}
		if (g_kVariableContainer.Get(EVKind_AntiHack, index++, fValue) == S_OK)
		{
			// EHack_Var_Mutiflier
			kPolicy.fHackIndexMultiflier = fValue;
		}
		if (g_kVariableContainer.Get(EVKind_AntiHack, index++, iValue) == S_OK)
		{
			// EHack_Var_Add
			kPolicy.sHackIndexAdd = iValue;
		}
		if (g_kVariableContainer.Get(EVKind_AntiHack, index++, iValue) == S_OK)
		{
			// EHack_Var_BlockDurationTime
			kPolicy.iBlockTimeSec = iValue;
		}
		if (g_kVariableContainer.Get(EVKind_AntiHack, index++, iValue) == S_OK)
		{
			// EHack_Var_MaxIndex
			kPolicy.sMaxHackIndex = iValue;
		}

		// custom value loading
		index = i*100;
		for (int j=10; j<100; ++j)
		{
			if (g_kVariableContainer.Get(EVKind_AntiHack, index+j, iValue) == S_OK)
			{
				kPolicy.kCustomValue.Set(index+j, iValue);
			}
		}

		g_kAntiHackMgr.Locked_SetPolicy(static_cast<EAntiHack_Policy>(i), kPolicy);
	}

	BM::Stream kVPacket(PT_IM_A_NFY_ANTIHACK_CONTROL);
	g_kAntiHackMgr.Locked_WriteToPacket(kVPacket);
	SendToServerType(CEL::ST_CENTER, kVPacket);
}
//*/