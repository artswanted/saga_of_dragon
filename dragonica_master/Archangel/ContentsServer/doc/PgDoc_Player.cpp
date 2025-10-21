#include "StdAfx.h"
#include <Mmsystem.h>
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgLogUtil.h"
#include "variant/Global.h"
#include "variant/TableDataManager.h"
#include "variant/PgClassDefMgr.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgSystemInvManager.h"
#include "Variant/PgQuestResetSchedule.h"
#include "PgDoc_Player.h"
#include "Contents/PgQuestRandom.h"
#include "Contents/PgQuestMng.h"
#include "PgServerSetMgr.h"
#include "Global.h"
#include "PgActionEventProcess.h"
#include "PgGuildMgr.h"
#include "PgAchievement.h"

#ifdef _USE_JOBCHECKER
#include "PgJobChecker.h"
#endif

// ---------------------------------------
// Query Checker
// ---------------------------------------
unsigned short PgQueryChecker::CHK_EXP = 60;
unsigned short PgQueryChecker::CHK_FRAN = 60;
unsigned short PgQueryChecker::CHK_MONEY = 10;

namespace DocPlayerUtil
{
	//
	SOrderInfo::SOrderInfo(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder, PgContLogMgr& rkContLogMgr)
		: m_rkChangeArray(rkChangeArray), m_rkOrder(rkOrder), m_rkContLogMgr(rkContLogMgr)
	{
	}
	SOrderInfo::SOrderInfo(SOrderInfo const& rhs)
		: m_rkChangeArray(rhs.m_rkChangeArray), m_rkOrder(rhs.m_rkOrder), m_rkContLogMgr(rhs.m_rkContLogMgr)
	{
	}
	//
	void CreateQuestEndOrder(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder, int const iQuestID, int const iClearQuest, PgContLogMgr& rkContLogMgr, PgMyQuest const* pkMyQuest)
	{
		if( NULL == pkMyQuest
		||	((0 != iClearQuest)? false == pkMyQuest->IsEndedQuest(iQuestID): true == pkMyQuest->IsEndedQuest(iQuestID)) )
		{
			tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_END, rkOrder.Cause(), rkOrder.OwnerGuid());
			kDBData.kAddonData.Push(iQuestID);
			kDBData.kAddonData.Push(iClearQuest);
			rkChangeArray.push_back(kDBData);

			PgQuestLogUtil::EndLog(rkContLogMgr, ((0 != iClearQuest)? ELOrderSub_End: ELOrderSub_DeEnd), rkOrder.OwnerGuid(), iQuestID);
		}
	}
	//
	void CreateQuestEndOrder(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder, ContQuestID const& rkVec, int const iClearQuest, PgContLogMgr& rkContLogMgr, PgMyQuest const* pkMyQuest)
	{
		ContQuestID::const_iterator iter = rkVec.begin();
		while( rkVec.end() != iter )
		{
			CreateQuestEndOrder(rkChangeArray, rkOrder, (*iter), iClearQuest, rkContLogMgr, pkMyQuest);
			++iter;
		}
	}
}

bool PgQueryChecker::InitConstantValue(void)
{
	int iValue = 60;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_HalfDelay, EVar_ExpSaveCycle , iValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Get constant value [EVar_Kind_HalfDelay][EVar_ExpSaveCycle]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CHK_EXP = (unsigned short)iValue;

	iValue = 60;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_HalfDelay, EVar_FranSaveCycle , iValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Get constant value [EVar_Kind_HalfDelay][EVar_FranSaveCycle]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CHK_FRAN = (unsigned short)iValue;

	iValue = 10;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_HalfDelay, EVar_MoneySaveCycle , iValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Can't Get constant value [EVar_Kind_HalfDelay][EVar_MoneySaveCycle]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CHK_MONEY = (unsigned short)iValue;
	return true;
}

PgQueryChecker::PgQueryChecker(void)
{
	ClearAllChecker();
}

PgQueryChecker::~PgQueryChecker(void)
{
}

void PgQueryChecker::CopyTo(PgQueryChecker& rkChecker)const
{
	rkChecker.m_nChk_Exp = m_nChk_Exp;
	rkChecker.m_nChk_Exp_Pet = m_nChk_Exp_Pet;
	rkChecker.m_nChk_Fran = m_nChk_Fran;
	rkChecker.m_nChk_Money = m_nChk_Money;
}

void PgQueryChecker::ClearAllChecker()
{
	m_nChk_Exp = 0;
	m_nChk_Exp_Pet = 0;
	m_nChk_Fran = 0;
	m_nChk_Money = 0;
}

void PgQueryChecker::AddChecker( WORD const Type )
{
	switch( Type )
	{
	case QCHK_EXPERANCE:
		{
			if ( m_nChk_Exp < USHRT_MAX )
			{
				++m_nChk_Exp;
			}
		}break;
	case QCHK_EXPERANCE_PET:
		{
			if ( m_nChk_Exp_Pet < USHRT_MAX )
			{
				++m_nChk_Exp_Pet;
			}
		}break;
	case QCHK_MONEY:
		{
			if ( m_nChk_Money < USHRT_MAX )
			{
				++m_nChk_Money;
			}
		}break;
	case QCHK_FRAN:
		{
			if ( m_nChk_Fran < USHRT_MAX )
			{
				++m_nChk_Fran;
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

unsigned short PgQueryChecker::GetCheckerValue( WORD const Type )const
{
	switch( Type )
	{
	case QCHK_EXPERANCE:	{return m_nChk_Exp;			}break;
	case QCHK_EXPERANCE_PET:{return m_nChk_Exp_Pet;		}break;	
	case QCHK_MONEY:		{return m_nChk_Money;		}break;
	case QCHK_FRAN:			{return m_nChk_Fran;		}break;
	default:{LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));}break;
	}
	return 0;
}

void PgQueryChecker::SetCheckerMax( WORD const Type )
{
	switch( Type )
	{
	case QCHK_EXPERANCE:
		{
			m_nChk_Exp = CHK_EXP;
		}break;
	case QCHK_EXPERANCE_PET:
		{
			m_nChk_Exp_Pet = CHK_EXP;
		}break;
	case QCHK_MONEY:
		{
			m_nChk_Money = CHK_MONEY;
		}break;
	case QCHK_FRAN:
		{
			m_nChk_Fran = CHK_FRAN;
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

void PgQueryChecker::ClearChecker( WORD const Type )
{
	switch( Type )
	{
	case QCHK_EXPERANCE:
		{
			m_nChk_Exp = 0;
		}break;
	case QCHK_EXPERANCE_PET:
		{
			m_nChk_Exp_Pet = 0;
		}break;
	case QCHK_MONEY:
		{
			m_nChk_Money = 0;
		}break;
	case QCHK_FRAN:
		{
			m_nChk_Fran = 0;
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}
	
bool PgQueryChecker::IsQuery( WORD const Type )const
{
	switch( Type )
	{
	case QCHK_EXPERANCE:
		{
			return CHK_EXP <= m_nChk_Exp;
		}break;
	case QCHK_EXPERANCE_PET:
		{
			return CHK_EXP <= m_nChk_Exp_Pet;
		}break;
	case QCHK_MONEY:
		{
			return CHK_MONEY <= m_nChk_Money;
		}break;
	case QCHK_FRAN:
		{
			return CHK_FRAN <= m_nChk_Fran;
		}break;
	case QCHK_SIDEJOB:
		{
			return false;// 이 녀석은 무조건 저장되지 않는다. 단지 맵 컨텐츠 동기화를 위해서 사용된다.
		}break;
	default:
		{
			// 업적 어빌은 바로 저장하지 않는다.
			if(QCHK_ACHIEVEMENT_MIN <= Type && QCHK_ACHIEVEMENT_MAX >= Type)
			{
				return false;
			}

			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return true;
}

PgDoc_Player::PgDoc_Player(void)
#ifdef _USE_JOBCHECKER
:	m_pkJobChecker(NULL)
#endif
{
	::memset(&m_kClientOption, 0, sizeof(m_kClientOption));
}

PgDoc_Player::~PgDoc_Player(void)
{
#ifdef _USE_JOBCHECKER
	SAFE_DELETE(m_pkJobChecker);
#endif
}

void PgDoc_Player::GetQuery( DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray )const
{
	PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_DBSave, GetMemberGUID(), GetID(), MemberID(), Name(), GetAbil(AT_CLASS), GetAbil(AT_LEVEL), 0, static_cast<EUnitGender>(GetAbil(AT_GENDER)));
	if ( 0 < m_nChk_Exp )
	{
		DB_ITEM_STATE_CHANGE kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL64, GetID() );
		kDBData.kAddonData.Push(AT_EXPERIENCE);
		__int64 const i64Value = GetAbil64(AT_EXPERIENCE);
		kDBData.kAddonData.Push(i64Value);
		kChangeArray.push_back(kDBData);
		{
			PgLog kLog(ELOrderMain_Exp, ELOrderSub_DBSave);
			kLog.Set(0, i64Value);
			kLogCont.Add(kLog);
		}
	}

	if ( 0 < m_nChk_Fran )
	{
		DB_ITEM_STATE_CHANGE kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL64, GetID());
		kDBData.kAddonData.Push(AT_FRAN);
		__int64 const i64Value = GetAbil64(AT_FRAN);
		kDBData.kAddonData.Push(i64Value);
		kChangeArray.push_back(kDBData);
		{
			PgLog kLog(ELOrderMain_Fran, ELOrderSub_DBSave);
			kLog.Set(0, i64Value);
			kLogCont.Add(kLog);
		}
	}

	if ( 0 < m_nChk_Money )
	{
		DB_ITEM_STATE_CHANGE kDBData(DISCT_MODIFY_MONEY, IMET_ADD_MONEY, GetID());
		__int64 const i64Value = m_kInv.Money();
		kDBData.kAddonData.Push(i64Value);
		kChangeArray.push_back(kDBData);
		{
			PgLog kLog(ELOrderMain_Money, ELOrderSub_DBSave);
			kLog.Set(0, i64Value);			
			kLogCont.Add(kLog);
		}
	}

	if ( 0 < m_nChk_Exp_Pet )
	{
		PgBase_Item kPetItem;
		if ( S_OK == m_kInv.GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, kPetItem ) )
		{
			PgItem_PetInfo *pkPetInfo = NULL;
			if ( true == kPetItem.GetExtInfo( pkPetInfo ) )
			{
				DB_ITEM_STATE_CHANGE kDBData( DISCT_SETABIL_PET, IMET_PET, GetID() );
				kDBData.ItemWrapper( PgItemWrapper( kPetItem, PgItem_PetInfo::ms_kPetItemEquipPos ) );
				kDBData.kAddonData.Push( static_cast<size_t>(1) );
				kDBData.kAddonData.Push( SPMOD_SAbil( AT_EXPERIENCE, pkPetInfo->Exp() ) );
				kChangeArray.push_back(kDBData);
			}
		}
	}

	kLogCont.Commit();
}

void PgDoc_Player::UpdateModifyAchievementsAbil(WORD const wAbilType)
{
	if((AT_ACHIEVEMENT_DATA_MIN <= wAbilType) && (wAbilType <= AT_ACHIEVEMENT_DATA_MAX))
	{
		m_kContModifyAchievementAbil.insert(wAbilType);
	}
}

void PgDoc_Player::OnAchievementAbilSaveDB(DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray)
{
	for(CONT_ACHIEVEMENT_ABIL::const_iterator iter = m_kContModifyAchievementAbil.begin();iter != m_kContModifyAchievementAbil.end();++iter)
	{
		tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID());
		kDBData.kAddonData.Push( static_cast<int>(*iter));
		kDBData.kAddonData.Push( static_cast<__int64>(GetAbil((*iter))));
		kChangeArray.push_back(kDBData);
	}
	m_kContModifyAchievementAbil.clear();
}

bool PgDoc_Player::SetAbil(WORD const Type, int const iValue, bool const bIsSend, bool const bBroadcast)
{
	SetUsedAchievementAbilList(Type);

	switch(Type)
	{
	case AT_LEVEL:
		{
			if (iValue > PgClassDefMgr::s_sMaximumLevel)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot set LEVEL to be more MaximumLevel=") << PgClassDefMgr::s_sMaximumLevel);
				return false;
			}
		}// break을 사용하지 않는다.
	default:
		{
			PgPlayer::SetAbil(Type, iValue, bIsSend, bBroadcast);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}

	AddChecker( Type );
	return true;
}

void PgDoc_Player::CopyAbilFromClassDef()
{
	//if (pkDef == NULL)
	//{
	//	return;
	//}
	GET_DEF(PgClassDefMgr, kClassDefMgr);
	SetAbil(AT_UNIT_SIZE, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_UNIT_SIZE));
	SetAbil(AT_MAX_HP_EXPAND_RATE, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_MAX_HP_EXPAND_RATE));
	SetAbil(AT_MAX_MP_EXPAND_RATE, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_MAX_MP_EXPAND_RATE));
	SetAbil(AT_DODGE_SUCCESS_VALUE, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_DODGE_SUCCESS_VALUE));
	SetAbil(AT_HIT_SUCCESS_VALUE, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_HIT_SUCCESS_VALUE));
	SetAbil(AT_ATTACK_SPEED, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_ATTACK_SPEED));
	if( 0 >= GetAbil(AT_C_ATTACK_SPEED) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("Player AttackSpeed must be more than ZERO Value[") << GetAbil(AT_C_ATTACK_SPEED) << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! AttackSpeed must be more than ZERO"));
	}
	//CLASSDEF_ABIL_CONT::const_iterator itor = pkDef->kAbil.begin();
	//while (itor != pkDef->kAbil.end())
	//{
	//	SetAbil(itor->first, itor->second);

	//	++itor;
	//}
}

// 센터에서 사용
void PgDoc_Player::WriteToQuery( CEL::DB_QUERY &rkQuery, EDBSaveType const kWriteType ) const
{
	if( UCHAR_MAX > m_kInfo.iClass )
	{
		rkQuery.QueryOwner(GetID());
		switch ( kWriteType )
		{
		case EDBSAVE_DEFAULT:
			{
				SPlayerBinaryData kBinaryData;
				
				// Buffer
				m_kMySkill.Save(MAX_DB_SKILL_SIZE, kBinaryData.abySkills);
				m_kQuest.WriteToBuff(PgQuestIngBuffer(kBinaryData.abyIngQuest), PgQuestEndBuffer(kBinaryData.abyEndQuest), PgQuestEndBuffer(kBinaryData.abyEndQuest2), PgQuestEndExtBuffer(kBinaryData.abyEndQuestExt), PgQuestEndExt2Buffer(kBinaryData.abyEndQuestExt2));
				//m_kQInv.Save(kBinaryData.abyQuickSlot); //Locked_Recv_PT_M_T_REQ_SAVE_CHARACTER에서 저장하고 있음
				BYTE abyStrategySkills[MAX_DB_SKILL_SIZE];
				m_kStrategySkill.kMySkill.Save(MAX_DB_SKILL_SIZE, abyStrategySkills);

				// write data to Query
				rkQuery.PushStrParam( GetID() );
				rkQuery.PushStrParam( m_kInfo.iClass );
				rkQuery.PushStrParam( m_kInfo.wLv );
				rkQuery.PushStrParam( GetAbil64(AT_EXPERIENCE) );
				rkQuery.PushStrParam( m_kInfo.sSP );
				rkQuery.PushStrParam( kBinaryData.abySkills, MAX_DB_SKILL_SIZE );
				rkQuery.PushStrParam( kBinaryData.abyIngQuest, MAX_DB_INGQUEST_SIZE );
				rkQuery.PushStrParam( kBinaryData.abyEndQuest, MAX_DB_ENDQUEST_SIZE );
				rkQuery.PushStrParam( kBinaryData.abyEndQuest2, MAX_DB_ENDQUEST_SIZE );
				rkQuery.PushStrParam( kBinaryData.abyEndQuestExt, MAX_DB_ENDQUEST_EXT_SIZE );
				rkQuery.PushStrParam( kBinaryData.abyEndQuestExt2, MAX_DB_ENDQUEST_EXT2_SIZE );

				rkQuery.PushStrParam( GetTotalConnSec_Character() );
				rkQuery.PushStrParam( GetAbil64(AT_FRAN) );
				rkQuery.PushStrParam( GetAbil(AT_TACTICS_LEVEL) );
				rkQuery.PushStrParam( GetAbil64(AT_TACTICS_EXP) );

				rkQuery.PushStrParam( GetStrategySP() );
				rkQuery.PushStrParam( abyStrategySkills, MAX_DB_SKILL_SIZE );
			}break;
		case EDBSAVE_PVP:
			{
				rkQuery.PushStrParam(GetID());
				rkQuery.PushStrParam(m_iPoint);
				rkQuery.PushStrParam(m_iRecords[GAME_WIN]);
				rkQuery.PushStrParam(m_iRecords[GAME_DRAW]);
				rkQuery.PushStrParam(m_iRecords[GAME_LOSE]);			//5

				rkQuery.PushStrParam((int)0);
				rkQuery.PushStrParam((__int64)m_iKill);
				rkQuery.PushStrParam((__int64)m_iDeath);	//8
			}break;
		case EDBSAVE_MISSION:
			{
				rkQuery.PushStrParam(GetID());
				m_kMissionReport.WriteToQuery(rkQuery);
			}break;
		case EDBSAVE_EXTERN:
			{
				WriteToQuery( rkQuery, EDBSAVE_PVP );//8
				m_kMissionReport.WriteToQuery(rkQuery);
			}break;
		}
		return;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Player Class is Invalid Class[") << m_kInfo.iClass << _T("], Guid[") << GetID().str().c_str() << _T("]") );
}

void PgDoc_Player::SetExternDB( SPvPReport const &kPvPReport, PgPlayer_MissionReport const &kMissionReport )
{
	SPvPReport::operator = (kPvPReport);
	m_kMissionReport = kMissionReport;
}

void PgDoc_Player::CopyTo(PgDoc_Player& rkPlayer, EWRITETYPE const eType) const
{
//	BM::Stream kCopyPacket;
//	WriteToPacket(kCopyPacket, eType);
	//rkPlayer.Init();
//	rkPlayer.ReadFromPacket(kCopyPacket);

//	class CAbilObject
	rkPlayer.m_mapAbils = m_mapAbils;
	rkPlayer.m_mapAbils64 = m_mapAbils64;

//	class IDObject
	rkPlayer.m_kGuid = m_kGuid;
	rkPlayer.m_Pos = m_Pos;

//	class CUnit
//	rkPlayer.m_dwElapsedTimeCheckZone = m_dwElapsedTimeCheckZone;
//	rkPlayer.m_dwHPElapsedTime = m_dwHPElapsedTime;
//	rkPlayer.m_dwMPElapsedTime = m_dwMPElapsedTime;
	rkPlayer.m_kCommon = m_kCommon;
	::memcpy( &rkPlayer.m_kBasic, &m_kBasic, sizeof(SPlayerBasicInfo) );

//	rkPlayer.m_State = m_State;
//	rkPlayer.m_iDelay = m_iDelay;
//	rkPlayer.m_iAttackDelay = m_iAttackDelay;
//	rkPlayer.m_kTargetDeque = m_kTargetDeque;

	rkPlayer.m_kInv = m_kInv;
	rkPlayer.m_Skill = m_Skill;
	rkPlayer.Name( Name() );
	///////////////////////////////////////
	//rkPlayer.m_kEffect = m_kEffect;
	///////////////////////////////////////

//	rkPlayer.m_iActionInstanceID = m_iActionInstanceID;
//	rkPlayer.m_iRandomSeed = m_iRandomSeed;
//	rkPlayer.m_iRandomHelper = m_iRandomHelper;
//	rkPlayer.m_bSummoned = m_bSummoned;
//	rkPlayer.m_kOrientedVector = m_kOrientedVector;
//	rkPlayer.m_guidBlowAttacker = m_guidBlowAttacker;
//	rkPlayer.m_kProjectile = m_kProjectile;
//	rkPlayer.FrontDirection(FrontDirection());
//	rkPlayer.LastAreaIndex(LastAreaIndex());
//	rkPlayer.LastTickTime(LastTickTime());
//	rkPlayer.TickNum(TickNum());
//	rkPlayer.DeathTime(DeathTime());
//	rkPlayer.GoalPos(GoalPos());
	rkPlayer.m_pkAggroMeter = NULL;

	rkPlayer.WeaponDecCount( WeaponDecCount() );
	rkPlayer.ArmorDecCount( ArmorDecCount() );
//	rkPlayer.ActionID( ActionID() );
//	rkPlayer.RelativeGoalPos( RelativeGoalPos() );
//	rkPlayer.PathNormal( PathNormal() );
//	rkPlayer.LastPos( LastPos() );

//	SUnit_AI_Info m_kAI;
//	rkPlayer.m_bSendSyncPacket = m_bSendSyncPacket;
//	rkPlayer.m_kSummonUnit = m_kSummonUnit;
//	rkPlayer.m_kCaller = m_kCaller;
//	rkPlayer.m_kDeliverDamageUnit = m_kDeliverDamageUnit;
	rkPlayer.m_bySyncType = m_bySyncType;

//	class Other
	rkPlayer.PgPlayerPlayTime::operator=(*this);
	rkPlayer.SPvPReport::operator=(*this);

//	class PgControlUnit
	rkPlayer.m_kMySkill = m_kMySkill;
	rkPlayer.m_kLevelRank = m_kLevelRank;
	rkPlayer.m_kStrategySkill = m_kStrategySkill;

//	class PgPlayer
	rkPlayer.m_kNetModule = m_kNetModule;
	rkPlayer.m_kVolatileID = m_kVolatileID;
	rkPlayer.m_kWorldMapFlag = m_kWorldMapFlag;
	rkPlayer.m_kPartyGuid = m_kPartyGuid;
	rkPlayer.m_kExpeditionGuid = m_kExpeditionGuid;
	rkPlayer.m_kExpeditionPartyGuid = m_kExpeditionPartyGuid;
	rkPlayer.m_kClientOption = m_kClientOption;
	::memcpy( &(rkPlayer.m_kInfo), &m_kInfo, sizeof(SPlayerDBData) );

//	rkPlayer.m_bReadyNetwork = m_bReadyNetwork;
//	rkPlayer.m_bDoSimulate = m_bDoSimulate;
//	rkPlayer.m_iMonsterKillCount = m_iMonsterKillCount;
	rkPlayer.m_byGMLevel = m_byGMLevel;
	rkPlayer.m_bIsPCCafe = m_bIsPCCafe;
	rkPlayer.m_byPCCafeGrade = m_byPCCafeGrade;
	rkPlayer.m_kGmAbil = m_kGmAbil;
//	rkPlayer.m_kActionInfo = m_kActionInfo;
	rkPlayer.MemberID( MemberID() );

//	rkPlayer.NotifyTime( NotifyTime() );
//	rkPlayer.LastLatencyTime( LastLatencyTime() );
//	rkPlayer.Latency( Latency() );
//	rkPlayer.OldPos( OldPos() );
//	rkPlayer.MoveDirection( MoveDirection() );
//	rkPlayer.FollowingHeadGuid( FollowingHeadGuid() );
	rkPlayer.ConnArea( ConnArea() );
	rkPlayer.GroundKey( GroundKey() );
	rkPlayer.Cash( Cash() );

	rkPlayer.m_ksJumpHeight = m_ksJumpHeight;
	rkPlayer.m_ksCJumpHeight = m_ksCJumpHeight;
	rkPlayer.m_ksAddMoneyPer = m_ksAddMoneyPer;

	rkPlayer.m_kMissionReport = m_kMissionReport;
	rkPlayer.m_kQInv = m_kQInv;
	if(WT_OP_COPY_PREMIUM & eType)
	{
		m_kPremium.CopyTo(rkPlayer.m_kPremium);
	}
	else
	{
		rkPlayer.m_kPremium = m_kPremium;
	}
	///////////////////////////////////////
	m_kQuest.Copy( rkPlayer.m_kQuest );
	///////////////////////////////////////
	rkPlayer.m_kAchievements = m_kAchievements;
//	rkPlayer.m_kPacketBuffer = m_kPacketBuffer;
	rkPlayer.m_kContAchievementAbil = m_kContAchievementAbil;

	BM::Stream kCopyPacket;
	m_kEffect.WriteToPacket(kCopyPacket, false);
	rkPlayer.m_kEffect.ReadFromPacket(kCopyPacket, false);

	rkPlayer.ContPortal(ContPortal());

	rkPlayer.m_kCardAbilObj = m_kCardAbilObj;

	rkPlayer.Emoticon(Emoticon());
	rkPlayer.Emotion(Emotion());
	rkPlayer.Balloon(Balloon());

	rkPlayer.HomeAddr(HomeAddr());

	//	class PgDoc
	rkPlayer.PgDoc::operator = (*this);

	//	class PgDocPlayer
	rkPlayer.m_kContDocPetInfo = m_kContDocPetInfo;

	rkPlayer.m_kExtendCharacterNum = m_kExtendCharacterNum;	// 캐릭터 슬롯 확장 개수

	rkPlayer.m_kContModifyAchievementAbil = m_kContModifyAchievementAbil;

	//	class PgQueryChecker
	PgQueryChecker *pkChecker = (PgQueryChecker*)(&rkPlayer);
	PgQueryChecker::CopyTo( *pkChecker );

#ifdef _USE_JOBCHECKER
	if ( m_pkJobChecker )
	{
		if ( !rkPlayer.m_pkJobChecker )
		{
			rkPlayer.m_pkJobChecker = new PgJobChecker;
		}

		if ( rkPlayer.m_pkJobChecker )
		{
			*(rkPlayer.m_pkJobChecker) = *m_pkJobChecker;
		}
	}
#endif
	m_kJobSkillExpertness.CopyTo(rkPlayer.m_kJobSkillExpertness);
	rkPlayer.m_kContJobSkillHistoryItem = m_kContJobSkillHistoryItem;

	rkPlayer.IsCreateCard(IsCreateCard());
}


HRESULT PgDoc_Player::AddExp(__int64 const& iAddValue, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray)
{
//	if(IsDead())//경험치 깔때 괜찮을까??;
//	{
//		return S_OK;//죽었으면 경험치 없음
//	}
	__int64 iRetValue = iAddValue;

	int iAddRate = GetAbil(AT_ADD_EXPERIENCE_RATE);
	if( S_PST_AddExperience const* pkPremiumAddExp = m_kPremium.GetType<S_PST_AddExperience>() )
	{
		iAddRate += pkPremiumAddExp->iRate;
	}
	if( 0 != iAddRate )
	{
		iRetValue += SRateControl::GetValueRate<int>( iAddValue, iAddRate );
		iRetValue = __max(iRetValue,0);
	}

	CalcExpRate(iRetValue);
	if ( iRetValue != 0 )
	{
		return SetExp(GetAbil64(AT_EXPERIENCE) + iRetValue, kChangeArray);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgDoc_Player::SetExp(__int64 const iValue, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray)
{
	int const iClass = GetAbil(AT_CLASS);
	int const iNowLv = GetAbil(AT_LEVEL);
	
	SClassKey kKey(iClass, iNowLv);//
	SClassKey kResultKey;//

	__int64 const i64NowExp = GetAbil64(AT_EXPERIENCE);

	__int64 i64RetExp = iValue;
	GET_DEF(PgClassDefMgr, kClassDefMgr);
	if(S_OK == kClassDefMgr.RevisionClassKey(kKey, i64RetExp))
	{
		m_kInfo.i64Exp = i64RetExp;//경험치 대입.

		PgClassDefMgr::SLvUpAddValue kAddedValue;
		if(kClassDefMgr.IsLvUp(kKey, i64RetExp, kResultKey, kAddedValue))
		{
			SPlayerBasicInfo kResultBasic;
			if(S_OK == kClassDefMgr.GetAbil(kResultKey, kResultBasic))
			{
//				m_kInfo. += iAddedSP;//SP 더해주는건 여기서.
				SetAbil(AT_SP, GetAbil(AT_SP) + kAddedValue.sSP);
				SetAbil(AT_STATUS_BONUS, GetAbil(AT_STATUS_BONUS) + kAddedValue.sBonusStatus);
				SetAbil(AT_CLASS, kResultKey.iClass);
				SetAbil(AT_LEVEL, kResultKey.nLv);

				// SendToAchievementMgr(kResultKey.nLv);

				if(IsOpenStrategySkill(ESTT_SECOND))
				{
					m_kStrategySkill.iSP += kAddedValue.sSP;
				}

				//레벨업 로그
				PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Change_Info);
				kLogCont.MemberKey(GetMemberGUID());
				kLogCont.UID(UID());
				kLogCont.CharacterKey(GetID());
				kLogCont.ID(MemberID());
				kLogCont.Name(Name());
				kLogCont.Class(GetAbil(AT_CLASS));
				kLogCont.Level(GetAbil(AT_LEVEL));
				kLogCont.ChannelNo(GetChannel());
				kLogCont.GroundNo(GetAbil(AT_RECENT));

				SPlayerBasicInfo kBasicInfo;
				kClassDefMgr.GetAbil(kResultKey, kBasicInfo);
				std::wstring kState;
				wchar_t wstrBuff[100];
				swprintf_s(wstrBuff, L"%hd / %hd / %hd / %hd / %hd / %hd", kBasicInfo.sInitStr+GetAbil(AT_STR_ADD),	kBasicInfo.sInitInt+GetAbil(AT_INT_ADD),
					kBasicInfo.sInitDex+GetAbil(AT_DEX_ADD), kBasicInfo.sInitCon+GetAbil(AT_CON_ADD), kBasicInfo.sPhysicsDefence, kBasicInfo.sMagicDefence);
				kState = wstrBuff;

				{
					PgLog kLvLog(ELOrderMain_Level, ELOrderSub_Modify);
					kLvLog.Set(1, kState);
					kLvLog.Set(0, iNowLv);
					kLvLog.Set(1, kAddedValue.sSP);
					kLvLog.Set(2, GetAbil(AT_GENDER) );
					kLvLog.Set(3, kAddedValue.sBonusStatus);
					kLvLog.Set(0, GetAbil64(AT_EXPERIENCE));
					kLvLog.Set(4, this->GetTotalConnSec_Character() );
					kLogCont.Add(kLvLog);
				}
				{
					int const iBS = GetAbil(AT_STATUS_BONUS);
					PgLog kBSLog(ELOrderMain_AS_BonusStatus, ELOrderSub_Modify);
					kBSLog.Set(0, static_cast<__int64>(iBS));
					kBSLog.Set(1, static_cast<__int64>(iBS-kAddedValue.sBonusStatus));
					kBSLog.Set(2, static_cast<__int64>(kAddedValue.sBonusStatus));
					kLogCont.Add(kBSLog);
				}
				kLogCont.Commit();

				if (kAddedValue.sSP != 0)
				{
					tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID());
					kDBData.kAddonData.Push(AT_SP);
					kDBData.kAddonData.Push((__int64)GetAbil(AT_SP));
					kChangeArray.push_back(kDBData);
				}

				if(iClass != kResultKey.iClass)
				{
					tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID());
					kDBData.kAddonData.Push(AT_CLASS);
					kDBData.kAddonData.Push((__int64)kResultKey.iClass);
					kDBData.kAddonData.Push((__int64)iClass);
					kChangeArray.push_back(kDBData);
				}

				{
					tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID());
					kDBData.kAddonData.Push(AT_LEVEL);
					kDBData.kAddonData.Push((__int64)kResultKey.nLv);
					kDBData.kAddonData.Push(GetMemberGUID());
					kDBData.kAddonData.Push(PgPlayerPlayTime::GetTotalConnSec_Character());
					kDBData.kAddonData.Push(GetAbil(AT_LEVEL_RANK));
					kDBData.kAddonData.Push((__int64)iNowLv);
					kChangeArray.push_back(kDBData);
				}

				if (kAddedValue.sBonusStatus != 0)
				{
					tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID());
					kDBData.kAddonData.Push(AT_STATUS_BONUS);
					kDBData.kAddonData.Push((__int64)GetAbil(AT_STATUS_BONUS));
					kChangeArray.push_back(kDBData);
				}

				PgActionCouponEvent kEvent( ET_CHANGE_LEVEL, this );
				SetCheckerMax(AT_EXPERIENCE);
				return S_OK;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Character[") << Name() << _COMMA_ << GetID() << _T(", Req Bad ClassInfo=(") << kResultKey.iClass
				<< _COMMA_ << kResultKey.nLv << _T(")"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		AddChecker(AT_EXPERIENCE);
		return S_OK;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Character[") << Name() << _COMMA_ << GetID() << _T(", Bad Class=(") << kKey.iClass
		<< _COMMA_ << kKey.nLv << _T(")"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgDoc_Player::AddTacticsExp(__int64 const iAddVal, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray)
{
	if( !iAddVal )
	{
		return S_OK;
	}

	__int64 iCalcValue = iAddVal;

	int iAddRate = GetAbil(AT_ADD_EXPERIENCE_RATE);
	if( S_PST_AddExperience const* pkPremiumAddExp = m_kPremium.GetType<S_PST_AddExperience>() )
	{
		iAddRate += pkPremiumAddExp->iRate;
	}
	if( 0 != iAddRate )
	{
		iCalcValue += SRateControl::GetValueRate<int>( iAddVal, iAddRate );
		iCalcValue = __max(iCalcValue,0);
	}

	CalcExpRate(iCalcValue);
	if( 0 < iCalcValue )
	{
		return SetTacticsExp(GetAbil64(AT_TACTICS_EXP) + iCalcValue, kChangeArray);
	}
	return S_OK;
}

HRESULT PgDoc_Player::SetTacticsExp(__int64 const iVal, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray)
{
	__int64 const iOldExp = GetAbil64(AT_TACTICS_EXP);

	CONT_DEF_TACTICS_LEVEL const *pkDefTacticsLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefTacticsLevel);
	
	if( pkDefTacticsLevel )
	{
		CONT_DEF_TACTICS_LEVEL::const_iterator find_iter = pkDefTacticsLevel->find( GetAbil(AT_TACTICS_LEVEL)+1 ); // 다음 레벨
		if( pkDefTacticsLevel->end() != find_iter )
		{
			CONT_DEF_TACTICS_LEVEL::mapped_type const &rkNextTacticsLevel = (*find_iter).second;
			SetAbil64(AT_TACTICS_EXP, iVal);

			__int64 const iCurExp = GetAbil64(AT_TACTICS_EXP);
			if( rkNextTacticsLevel.iTacticsExp <= iCurExp )
			{
				int const iOldTLv = GetAbil(AT_TACTICS_LEVEL);
				//Do Levelup
				SetAbil(AT_TACTICS_LEVEL, rkNextTacticsLevel.sTacticsLevel);

				__int64 const iNowTLv = (__int64)GetAbil(AT_TACTICS_LEVEL);

				tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID());
				kDBData.kAddonData.Push(AT_TACTICS_LEVEL);
				kDBData.kAddonData.Push(iNowTLv);
				kChangeArray.push_back(kDBData);

				VOnChangeTactics();

				//용병 레벨업 로그
				PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Change_Info);
				kLogCont.MemberKey(GetMemberGUID());
				kLogCont.UID(UID());
				kLogCont.CharacterKey(GetID());
				kLogCont.ID(MemberID());
				kLogCont.Name(Name());
				kLogCont.Class(GetAbil(AT_CLASS));
				kLogCont.Level(GetAbil(AT_LEVEL));
				kLogCont.ChannelNo(GetChannel());
				kLogCont.GroundNo(GetAbil(AT_RECENT));


				PgLog kLvUpLog(ELOrderMain_TacticsExp, ELOrderSub_Modify);
				kLvUpLog.Set(0, iNowTLv);
				kLvUpLog.Set(1, iOldTLv);
				kLvUpLog.Set(0, rkNextTacticsLevel.iTacticsExp);
				kLogCont.Add(kLvUpLog);
				kLogCont.Commit();
			}
		}
	}
	
	return S_OK;
}

HRESULT PgDoc_Player::AddFran(__int64 const& iAddValue)
{
//	if(IsDead())
//	{
//		return S_OK;
//	}

	__int64 iRetValue = iAddValue;

	if(0 > (iRetValue + GetAbil64(AT_FRAN)))
	{
		return E_FAIL;
	}

	CalcExpRate(iRetValue);

	return SetFran(GetAbil64(AT_FRAN) + iRetValue);
}

HRESULT PgDoc_Player::SetFran(__int64 const iFran)
{
	int const iNowLv = GetAbil(AT_LEVEL);

	if (iNowLv >= PgClassDefMgr::s_sMaximumLevel)
	{
		return E_FAIL;
	}

	__int64 iOldFran = GetAbil64(AT_FRAN);

	SetAbil64(AT_FRAN, iFran);
	//	kChangeArray.push_back(tagDBItemStateChange(DISCT_MODIFY_VALUE, IMET_SET_ABIL, GetID(), AT_SP, GetAbil(AT_SP)));

	return S_OK;
}

bool PgDoc_Player::AddAbil(WORD const Type, int const iValue)
{
	SetUsedAchievementAbilList(Type);

	switch(Type)
	{//플레이어는 SetAbil 쓸 것.
	case AT_SP:
		{
			int const iPrevValue =  m_kInfo.sSP + iValue;
			if(0 <= iPrevValue)
			{//0 이하가 되면 안된다.
				m_kInfo.sSP += iValue;
				return true;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	//여기는 디비값
	case AT_LEVEL:
	case AT_CLASS:
	case AT_GENDER:	
	case AT_RACE:
	case AT_FIVE_ELEMENT_TYPE_AT_BODY:
	//여기는 실제값.
	case AT_JUMP_HEIGHT:
	case AT_C_JUMP_HEIGHT:
	case AT_ADD_MONEY_PER:
		{//플레이어 정보는 Setabil로.
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Do not use this in AddAbil(use SetAbil)") );
		}break;
	case AT_HP:
		{
			// Max 값을 체크 해야 한다.
			int const iRet = std::min( iValue + GetAbil( AT_HP ), GetAbil(AT_C_MAX_HP) );
			CUnit::SetAbil( AT_HP, iRet );
		}break;
	case AT_MP:
		{
			// Max 값을 체크 해야 한다.
			int const iRet = std::min( iValue + GetAbil( AT_MP ), GetAbil(AT_C_MAX_MP) );
			CUnit::SetAbil( AT_MP, iRet );
		}break;
	default:
		{
			return CUnit::AddAbil(Type, iValue);
		}break;
	}

	AddChecker( Type );
	return true;
}


// int Quest
bool PgDoc_Player::AddIngQuest(int const iQuestID)
{
	CONT_DEF_QUEST_REWARD const *pkQuestDefReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestDefReward);

	CONT_DEF_QUEST_REWARD::const_iterator def_iter = pkQuestDefReward->find( iQuestID );
	if( pkQuestDefReward->end() == def_iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CONT_DEF_QUEST_REWARD::mapped_type const &rkQuestRewardDef = (*def_iter).second;

	SUserQuestState kNewState(iQuestID, QS_Ing);
	
	if( NULL != m_kQuest.AddIngQuest(&kNewState, rkQuestRewardDef.iDBQuestType) )
	{
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

bool PgDoc_Player::DelIngQuest(int const iQuestID, bool const bComplete, ContQuestID &rkOut, int &iOut)
{
	CONT_DEF_QUEST_REWARD const *pkQuestDefReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestDefReward);

	CONT_DEF_QUEST_REWARD::const_iterator def_iter = pkQuestDefReward->find( iQuestID );
	if( pkQuestDefReward->end() == def_iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CONT_DEF_QUEST_REWARD::mapped_type const &rkQuestRewardDef = (*def_iter).second;

	bool bRet = false;
	bRet = m_kQuest.DropQuest(iQuestID); // 무조건 버린다.

	if( bComplete )
	{
		EQuestType eQuestType = static_cast<EQuestType>( PgQuestInfoUtil::GetQuestDefType( iQuestID ) );
		
		switch( eQuestType )
		{
		case QT_Wanted:
			{
				CONT_DEF_QUEST_WANTED const *pkContWantedQuest = NULL;
				g_kTblDataMgr.GetContDef( pkContWantedQuest );
				if( pkContWantedQuest )
				{
					int iRewardClearCount = 0;

					CONT_DEF_QUEST_WANTED::const_iterator find_iter = pkContWantedQuest->find( iQuestID );
					if( pkContWantedQuest->end() != find_iter )
					{
						CONT_DEF_QUEST_WANTED::mapped_type const& rkQuestWanted = (*find_iter).second;
						iRewardClearCount = rkQuestWanted.iClearCount;
					}

					if( GetWantedQuestClearCount(iQuestID) >= ( iRewardClearCount - 1 ) )
					{//이번에 완료하면 최종 완료다
						m_kQuest.AddEndQuest(iQuestID, __FUNCTIONW__);
					}
				}
			}break;
		default:
			{
				m_kQuest.AddEndQuest(iQuestID, __FUNCTIONW__);			
			}break;
		}
	}

	if( bComplete
	&&	rkQuestRewardDef.iGroupNo )
	{
		ContQuestID const *pkVec = g_kQuestMng.GetGroup(rkQuestRewardDef.iGroupNo);
		if( pkVec )
		{
			rkOut = *pkVec;
			m_kQuest.DeEndQuest(rkOut, __FUNCTIONW__);
			iOut = BuildGroupQuest(rkQuestRewardDef.iGroupNo);
		}
	}
	return bRet;
}

bool PgDoc_Player::SetIngQuest(int const iQuestID, int const iTargetState)
{
	SUserQuestState const *pkState = m_kQuest.Get(iQuestID);
	if( !pkState )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SUserQuestState kNewState = *pkState;
	kNewState.byQuestState = (BYTE)iTargetState;

	return m_kQuest.UpdateQuest(kNewState);
}

bool PgDoc_Player::AddIngQuestParam(int const iQuestID, int const iParamNo, int const iParamCount, int const iMaxCount, bool const bSet, int &iResultOut, int &iPreOut)
{
	SUserQuestState const *pkState = m_kQuest.Get(iQuestID);
	if( !pkState )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	iPreOut = pkState->byParam[iParamNo];
	
	SUserQuestState kNewState = *pkState;
	if( bSet )
	{
		kNewState.byParam[iParamNo] = (BYTE)iParamCount;
	}
	else
	{
		kNewState.byParam[iParamNo] += (BYTE)iParamCount;
		kNewState.byParam[iParamNo] = __min(kNewState.byParam[iParamNo], (BYTE)iMaxCount);
	}

	iResultOut = kNewState.byParam[iParamNo];

	return m_kQuest.UpdateQuest(kNewState);
}

void PgDoc_Player::AddEndQuest(int const iQuestID, bool const bClear)
{
	if( bClear )
	{
		m_kQuest.AddEndQuest(iQuestID, __FUNCTIONW__);
	}
	else
	{
		m_kQuest.DeEndQuest(iQuestID, __FUNCTIONW__);
	}
}

void PgDoc_Player::RemoveNullQuest(PgContLogMgr &rkLogMgr) // DB에 표시 된 퀘스트를 삭제 한다.
{
	int const iClass = GetAbil(AT_CLASS);
	int const iLevel = GetAbil(AT_LEVEL);

	ContQuestID const& rkVec = g_kQuestMng.DeletedQuestAll();
	ContQuestID::const_iterator loop_iter = rkVec.begin();
	while( rkVec.end() != loop_iter )
	{
		int const iQuestID = (*loop_iter);

		if( m_kQuest.DropQuest(iQuestID) ) // 삭제 되면 로그를 남긴다.
		{
			PgQuestLogUtil::IngLog(rkLogMgr, ELOrderSub_Delete, GetID(), iQuestID, iClass, iLevel);
			PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_DeEnd, GetID(), iQuestID);
		}

		++loop_iter;
	}
	m_kQuest.DeEndQuest(rkVec, __FUNCTIONW__);
}

void PgDoc_Player::RemoveCoupleQuest(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder)
{
	bool bRet = false;

	ContQuestID const& rkVec = g_kQuestMng.CoupleQuestAll();
	ContQuestID::const_iterator loop_iter = rkVec.begin();
	while( rkVec.end() != loop_iter )
	{
		int const iQuestID = (*loop_iter);

		//SUserQuestState const* pkUserState = m_kQuest.Get(iQuestID);
		//if( pkUserState && QS_End != pkUserState->byQuestState )
		{
			bRet = m_kQuest.DropQuest(iQuestID);

			if( bRet )
			{
				tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_ING, rkOrder.Cause(), rkOrder.OwnerGuid());
				kDBData.kAddonData.Push(iQuestID);
				kDBData.kAddonData.Push(QS_None);
				rkChangeArray.push_back(kDBData);
			}

			{
				AddEndQuest(iQuestID, false);

				tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_END, rkOrder.Cause(), rkOrder.OwnerGuid());
				kDBData.kAddonData.Push(iQuestID);
				kDBData.kAddonData.Push(static_cast<int>(0));
				rkChangeArray.push_back(kDBData);
			}
		}

		++loop_iter;
	}
	m_kQuest.DeEndQuest(rkVec, __FUNCTIONW__);
}

void PgDoc_Player::BuildGroupQuest(PgContLogMgr &rkLogMgr)
{
	ContQuestGroup const &rkMap = g_kQuestMng.QuestGroup();
	ContQuestGroup::const_iterator loop_iter = rkMap.begin();
	while(rkMap.end() != loop_iter)
	{
		int const iGroupNo = (*loop_iter).first;
		int const iSelectedQuestID = BuildGroupQuest( iGroupNo );

		// Log 출력용
		if( 0 != iSelectedQuestID )
		{
			ContQuestID const *pkVec = g_kQuestMng.GetGroup( iGroupNo );
			if( pkVec
			&&	1 < pkVec->size() ) // 1개 이상의 그룹일 때
			{
				ContQuestID::const_iterator id_iter = pkVec->begin();
				while( pkVec->end() != id_iter )
				{
					int const iQuestID = (*id_iter);
					PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_End, GetID(), iQuestID);
					++id_iter;
				}
			}
			PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_DeEnd, GetID(), iSelectedQuestID);
		}

		++loop_iter;
	}
}

int PgDoc_Player::BuildGroupQuest(int const iGroupNo)
{
	if( !iGroupNo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	ContQuestID const *pkVec = g_kQuestMng.GetGroup(iGroupNo);
	if( !pkVec )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	ContQuestID const &rkVec = *pkVec;
	if( 1 == rkVec.size() )
	{
		int const iQuestID = rkVec.front();
		m_kQuest.DeEndQuest(iQuestID, __FUNCTIONW__);
		return iQuestID;
	}

	size_t const iUnflagedCount = m_kQuest.GetFlagQusetCount(rkVec, false);
	if( 1 == iUnflagedCount )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int const iSelected = BM::Rand_Index( static_cast< int >(rkVec.size()) );
	m_kQuest.AddEndQuest(rkVec, __FUNCTIONW__, iSelected);
	m_kQuest.DeEndQuest(iSelected, __FUNCTIONW__);
	return iSelected;
}

void PgDoc_Player::StartRandomQuest()
{	
	ContQuestID kVec;
	m_kQuest.AddEndQuest( g_kQuestMng.QuestRandomAll(), __FUNCTIONW__ ); // 랜덤 퀘스트 전부 초기화
	g_kQuestMng.GetQuestRandom(GetAbil(AT_LEVEL), kVec);
	m_kQuest.ContRandomQuest(kVec);
	
	m_kQuest.DeEndQuest( m_kQuest.ContRandomQuest(), __FUNCTIONW__ );
	m_kQuest.BuildedRandomQuest(true);
	m_kQuest.BuildedRandomQuestPlayerLevel(GetAbil(AT_LEVEL));
}
void PgDoc_Player::StartTactcisRandomQuest()
{	
	ContQuestID kVec;
	m_kQuest.AddEndQuest( g_kQuestMng.QuestTacticsRandomAll(), __FUNCTIONW__ ); // 랜덤 퀘스트 전부 초기화
	g_kQuestMng.GetQuestTacticsRandom(GetAbil(AT_LEVEL), kVec);
	m_kQuest.ContTacticsQuest(kVec);
	
	m_kQuest.DeEndQuest( m_kQuest.ContTacticsQuest(), __FUNCTIONW__ );
	m_kQuest.BuildedTacticsQuest(true);
	m_kQuest.BuildedTacticsQuestPlayerLevel(GetAbil(AT_LEVEL));
}
void PgDoc_Player::StartWantedQuest()
{	
	ContQuestID kVec;
	m_kQuest.AddEndQuest( g_kQuestMng.QuestWantedAll(), __FUNCTIONW__ ); // 현상 수배 퀘스트 전부 초기화
	g_kQuestMng.GetQuestWanted(GetAbil(AT_LEVEL), kVec);
	m_kQuest.ContWantedQuest(kVec);
	
	m_kQuest.DeEndQuest( m_kQuest.ContWantedQuest(), __FUNCTIONW__ );
	m_kQuest.BuildedWantedQuest(true);
	m_kQuest.BuildedWantedQuestPlayerLevel(GetAbil(AT_LEVEL));

	ContQuestClearCount kTemp;
	m_kQuest.ContWantedQuestClearCount(kTemp);
}

bool PgDoc_Player::BuildDayLoopQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo)
{
	bool const bNeedFresh = PgQuestInfoUtil::NeedRefreshDayQuest(m_kQuest, rkFailPrevTime, rkNextTime);
	if( !bNeedFresh )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if( pkOrderInfo )
	{
		int const iNotClearQuest = 0;
		DocPlayerUtil::CreateQuestEndOrder(pkOrderInfo->m_rkChangeArray, pkOrderInfo->m_rkOrder, g_kQuestMng.DayQuestAll(), iNotClearQuest, pkOrderInfo->m_rkContLogMgr, &m_kQuest);
	}
	m_kQuest.DeEndQuest( g_kQuestMng.DayQuestAll(), __FUNCTIONW__ ); // 1일 퀘스트 전부 초기화
	return true;
}

bool PgDoc_Player::BuildWeekLoopQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, DocPlayerUtil::SOrderInfo* pkOrderInfo)
{
	bool const bNeedFresh = PgQuestInfoUtil::NeedRefreshDayQuest(m_kQuest, rkFailPrevTime, rkNextTime);
	if( !bNeedFresh )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if( pkOrderInfo )
	{
		int const iNotClearQuest = 0;
		DocPlayerUtil::CreateQuestEndOrder(pkOrderInfo->m_rkChangeArray, pkOrderInfo->m_rkOrder, g_kQuestMng.WeekQuestAll(), iNotClearQuest, pkOrderInfo->m_rkContLogMgr, &m_kQuest);
	}
	m_kQuest.DeEndQuest( g_kQuestMng.WeekQuestAll(), __FUNCTIONW__ ); // 주간 퀘스트 전부 초기화
	return true;
}

bool PgDoc_Player::BuildRandomQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo)
{
	if( !bForce )
	{
		bool const bNeedFresh = PgQuestInfoUtil::NeedRefreshDayQuest(m_kQuest, rkFailPrevTime, rkNextTime);
		if( !bNeedFresh )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( pkOrderInfo )
	{
		int const iClearQuest = 1;
		DocPlayerUtil::CreateQuestEndOrder(pkOrderInfo->m_rkChangeArray, pkOrderInfo->m_rkOrder, g_kQuestMng.QuestRandomAll(), iClearQuest, pkOrderInfo->m_rkContLogMgr, &m_kQuest);
	}

	ContQuestID kContTemp;
	m_kQuest.AddEndQuest( g_kQuestMng.QuestRandomAll(), __FUNCTIONW__ ); // 랜덤 퀘스트 전부 초기화
	g_kQuestMng.GetQuestRandom(GetAbil(AT_LEVEL), kContTemp);

	if( !kContTemp.size() )
	{
		return false;
	}
	
	m_kQuest.ContRandomQuest(kContTemp);
	m_kQuest.BuildedRandomQuest(false);
	m_kQuest.BuildedRandomQuestPlayerLevel(GetAbil(AT_LEVEL));
	return true;
}

bool PgDoc_Player::BuildTacticsRandomQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo)
{
	if( !bForce )
	{
		bool const bNeedFresh = PgQuestInfoUtil::NeedRefreshDayQuest(m_kQuest, rkFailPrevTime, rkNextTime);
		if( !bNeedFresh )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( pkOrderInfo )
	{
		int const iClearQuest = 1;
		DocPlayerUtil::CreateQuestEndOrder(pkOrderInfo->m_rkChangeArray, pkOrderInfo->m_rkOrder, g_kQuestMng.QuestTacticsRandomAll(), iClearQuest, pkOrderInfo->m_rkContLogMgr, &m_kQuest);
	}
	
	ContQuestID kContTemp;
	m_kQuest.AddEndQuest( g_kQuestMng.QuestTacticsRandomAll(), __FUNCTIONW__ ); // 랜덤 퀘스트 전부 초기화
	g_kQuestMng.GetQuestTacticsRandom(GetAbil(AT_LEVEL), kContTemp);

	if( !kContTemp.size() )
	{
		return false;
	}

	m_kQuest.ContTacticsQuest(kContTemp);
	m_kQuest.BuildedTacticsQuest(false);
	m_kQuest.BuildedTacticsQuestPlayerLevel(GetAbil(AT_LEVEL));
	return true;
}

bool PgDoc_Player::BuildWantedQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo)
{	
	ContQuestID kVec;
	g_kQuestMng.GetQuestWanted(GetAbil(AT_LEVEL), kVec);
	if( !kVec.empty() )
	{
		m_kQuest.ExistCanBuildWantedQuest(true);
	}

	if( !bForce )
	{
		bool const bNeedFresh = PgQuestInfoUtil::NeedRefreshDayQuest( m_kQuest, rkFailPrevTime, rkNextTime );
		if( !bNeedFresh )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	{// 클리어 카운트 초기화
		InitWantedQuestClearCount();
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_INIT_QUEST_CLEAR_COUNT,L"EXEC [dbo].[up_UserWantedQuest_Init]");
		kQuery.InsertQueryTarget(GetID());
		kQuery.PushStrParam(GetID());
		g_kCoreCenter.PushQuery(kQuery);
	}

	if( pkOrderInfo )
	{
		int const iClearQuest = 1;
		DocPlayerUtil::CreateQuestEndOrder(pkOrderInfo->m_rkChangeArray, pkOrderInfo->m_rkOrder, g_kQuestMng.QuestWantedAll(), iClearQuest, pkOrderInfo->m_rkContLogMgr, &m_kQuest);
	}

	ContQuestID kContTemp;
	m_kQuest.AddEndQuest( g_kQuestMng.QuestWantedAll(), __FUNCTIONW__ ); // 현상수배 퀘스트 전부 초기화
	g_kQuestMng.GetQuestWanted(GetAbil(AT_LEVEL), kContTemp);

	if( !kContTemp.size() )
	{
		return false;
	}

	m_kQuest.ContWantedQuest(kContTemp);
	m_kQuest.BuildedWantedQuest(false);
	m_kQuest.BuildedWantedQuestPlayerLevel(GetAbil(AT_LEVEL));
	return true;
}

bool PgDoc_Player::BuildScheduledQuest(BM::PgPackedTime const& rkNowTime, ContQuestID& rkOut, DocPlayerUtil::SOrderInfo* pkOrderInfo)
{
	PgQuestResetScheduleUtil::CheckResetSchedule(m_kQuest.DayLoopQuestTime(), rkNowTime, g_kQuestMng.ResetSchedule(), rkOut);
	if( !rkOut.empty() )
	{
		if( pkOrderInfo )
		{
			int const iNotClearQuest = 0;
			DocPlayerUtil::CreateQuestEndOrder(pkOrderInfo->m_rkChangeArray, pkOrderInfo->m_rkOrder, rkOut, iNotClearQuest, pkOrderInfo->m_rkContLogMgr, &m_kQuest);
		}
		m_kQuest.DeEndQuest( rkOut, __FUNCTIONW__ );
		return true;
	}
	return false;
}

bool PgDoc_Player::BuildLoopQuest(PgContLogMgr &rkLogMgr)
{
	ContQuestID kResetSchedule;
	bool const bRet = ProcessBuildLoopQuest(kResetSchedule, false, NULL, BLQF_ALL);
	if( bRet ) // 너무 많은 로그가 남아서 로그를 남기지 않는다
	{
		//PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_DeEnd, GetID(), g_kQuestMng.DayQuestAll());
		//PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_DeEnd, GetID(), g_kQuestMng.WeekQuestAll());
		//PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_End, GetID(), g_kQuestMng.QuestRandomAll());
		//PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_End, GetID(), g_kQuestMng.QuestTacticsRandomAll());
		//PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_End, GetID(), g_kQuestMng.QuestWantedAll());
		//PgQuestLogUtil::EndLog(rkLogMgr, ELOrderSub_DeEnd, GetID(), kRandomQuestID);
	}
	return bRet;
}

bool PgDoc_Player::ProcessBuildLoopQuest(ContQuestID& rkScheduleOut, bool const bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo, int const iFlag)
{
	BM::PgPackedTime const kLocalTime(BM::PgPackedTime::LocalTime());
	BM::PgPackedTime kFailPrevTime, kNextTime, WeekFailPrevTime, WeekNextTime;
	PgQuestInfoUtil::MakeDayLoopQuestTime(kFailPrevTime, kNextTime);
	PgQuestInfoUtil::MakeWeekLoopQuestTime(WeekFailPrevTime, WeekNextTime);

	bool const bRet1 = iFlag&BLQF_DAY_LOOP_QUEST			? BuildDayLoopQuest(kFailPrevTime, kNextTime, bForce, pkOrderInfo) : false;
	bool const bRet2 = iFlag&BLQF_RANDOM_QUEST				? BuildRandomQuest(kFailPrevTime, kNextTime, bForce, pkOrderInfo) : false;
	bool const bRet3 = iFlag&BLQF_TACTICS_RANDOM_QUEST 		? BuildTacticsRandomQuest(kFailPrevTime, kNextTime, bForce, pkOrderInfo) : false;
	bool const bRet4 = iFlag&BLQF_WANTED_QUEST				? BuildWantedQuest(kFailPrevTime, kNextTime, bForce, pkOrderInfo) : false;
	bool const bRet5 = iFlag&BLQF_SCHEDULE_QUEST			? BuildScheduledQuest(kLocalTime, rkScheduleOut, pkOrderInfo) : false;
	bool const bRet6 = iFlag&BLQF_WEEK_LOOP_QUEST			? BuildWeekLoopQuest(WeekFailPrevTime, WeekNextTime, pkOrderInfo) : false;

	if( bRet1
	||	bRet2
	||	bRet3 
	||  bRet4 
	||  bRet5
	||	bRet6 )
	{
		m_kQuest.DayLoopQuestTime( kLocalTime );
		return true;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgDoc_Player::SetRecentPos(int const iPointType, POINT3 const &ptPos)
{
	switch(iPointType )
	{
	case AT_RECENT:
		{
			m_kInfo.kRecentInfo.ptPos3 = ptPos;
		}break;
	case AT_LAST_VILLAGE:
		{
			m_kInfo.kLastVillage.ptPos3 = ptPos;
		}break;
	}
}

HRESULT PgDoc_Player::ItemProcess( SPMO const &kOrder, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)//modify를 요구함. 전체가 다 되어야. 응답을 S_OK로 해줌,
{
	PgItemWrapper kItemWrapper;
	HRESULT hReturn = E_FAIL;
	__int64 const iCause = kOrder.Cause();
	switch( IMET_CMP_BASE & iCause )
	{
	case IMET_MODIFY_EXTEND_DATA:
		{
			SPMOD_ExtendData kExtend;
			kOrder.Read(kExtend);

			if(S_OK == (hReturn = m_kInv.PopItem(iCause, kExtend.Pos(), kItemWrapper, true, kChangeArray, kContLogMgr)))
			{
				PgBase_Item const &rkMyItem = kItemWrapper;

				PgBase_Item kTempItem = rkMyItem;

				if(true == kExtend.Remove())
				{
					kTempItem.Remove(kExtend.Type());

					DB_ITEM_STATE_CHANGE kItemChange(DISCT_REMOVE_ITEM_EXTEND_DATA, iCause, m_kInv.OwnerGuid());
					kItemChange.kAddonData.Push(rkMyItem.Guid());
					kItemChange.kAddonData.Push(kExtend.Type());
					kChangeArray.push_back(kItemChange);
				}
				else
				{
					switch(kExtend.Type())
					{
					case IEDT_EXPCARD:
						{
							SExpCard kNewExpCard;
							kExtend.m_kExtendData.Get(kNewExpCard);

							SExpCard kOldExpCard,kOldExpCard2;
							if(false == rkMyItem.Get(kOldExpCard) || false == kExtend.Item().Get(kOldExpCard2))
							{
								return E_INCORRECT_ITEM;
							}

							__int64 const i64NewExp = std::min(kNewExpCard.CurExp() + kOldExpCard.CurExp(), kOldExpCard.MaxExp());
							kNewExpCard.CurExp( i64NewExp );

							kTempItem.Set(kNewExpCard);
						}break;
					case IEDT_HOME:
						{
							SHomeItem kkData;
							kExtend.m_kExtendData.Get(kkData);
							kTempItem.Set(kkData);
						}break;
					case IEDT_MONSTERCARD:
						{
							SMonsterCardTimeLimit kCard;
							kExtend.m_kExtendData.Get(kCard);
							kTempItem.Set(kCard);
						}break;
					default:
						{
							CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Unhanded Extend Item Data[") << static_cast<int>(kExtend.Type()) << _T("] OwnerID[") << Name().c_str() << _T("]") );
							return E_INCORRECT_ITEM;
						}break;
					}
				}

				kItemWrapper.SetItem(kTempItem);

				hReturn = m_kInv.OverWriteItem(iCause, kItemWrapper, kChangeArray, kContLogMgr);
				if(S_OK == hReturn)
				{
					return S_OK;
				}
			}
		}break;
	case IMET_DELETE_SYSITEM:
		{
			g_kSysInvMgr.ClearTimeOutSysItem(kOrder.OwnerGuid());

			SPMOD_SysItemRemove kData;
			kOrder.Read(kData);

			PgBase_Item kItem = g_kSysInvMgr.GetSysItem(kOrder.OwnerGuid(),kData.ItemGuid());

			kItem.Guid(kData.ItemGuid());

			PgItemWrapper kBlankItemWrapper(kItem, SItemPos()), kTempItemWrapper;

			DB_ITEM_STATE_CHANGE kItemChange(DISCT_REMOVE_SYSTEM, iCause, m_kInv.OwnerGuid(), m_kInv.OwnerGuid(),kBlankItemWrapper, kTempItemWrapper); 
			kChangeArray.push_back(kItemChange);

			PgItemLogUtil::DeleteLog(GetID(),kContLogMgr,kTempItemWrapper);
			return S_OK;
		}break;
	case IMET_MODIFY_SYS2INV:
		{
			g_kSysInvMgr.ClearTimeOutSysItem(kOrder.OwnerGuid());

			SPMOD_Sys2Inv kData;
			kOrder.Read(kData);
			PgBase_Item kItem = g_kSysInvMgr.GetSysItem(kOrder.OwnerGuid(),kData.ItemGuid());

			if(kItem.IsEmpty())
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_ITEM"));
				return E_NOT_FOUND_ITEM;
			}

			PgItemWrapper kCasterWrapper(kItem, kData.Pos());

			bool const bAnyPos = kData.AnyPos();

			if(!bAnyPos)
			{
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const *pItemDef = kItemDefMgr.GetDef(kCasterWrapper.ItemNo());
				if(pItemDef)
				{
					if(pItemDef->PrimaryInvType() != kCasterWrapper.Pos().x)
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_INVENTYPE"));
						return E_INVALID_INVENTYPE;
					}
				}
				else
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_ITEM"));
					return E_NOT_FOUND_ITEM;
				}
			}

			hReturn = m_kInv.InsertItem(iCause, kCasterWrapper, bAnyPos, false, kChangeArray, kContLogMgr);
			if(S_OK != hReturn)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hReturn);
				return hReturn;
			}

			PgItemWrapper kBlankItemWrapper(kItem, SItemPos()), kTempItemWrapper;
			DB_ITEM_STATE_CHANGE kItemChange(DISCT_REMOVE_SYSTEM, iCause, m_kInv.OwnerGuid(), m_kInv.OwnerGuid(), kBlankItemWrapper,kTempItemWrapper); 
			kChangeArray.push_back(kItemChange);

			PgItemLogUtil::DeleteLog(GetID(),kContLogMgr,kCasterWrapper);
			return S_OK;
		}break;
	case IMET_MODIFY_ENCHANT://타겟 대상으로 작업
		{
			SPMOD_Enchant kData;
			kOrder.Read(kData);

			if(S_OK == (hReturn = m_kInv.PopItem(iCause, kData.Pos(), kItemWrapper, false, kChangeArray, kContLogMgr)))
			{
				PgBase_Item const &rkMyItem = kItemWrapper;

				if(rkMyItem.EnchantInfo() != kData.Item().EnchantInfo())
				{//수리가 되면서.. 아이템이 안맞을 수도 있네.
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
					return E_INCORRECT_ITEM;
				}

				PgBase_Item kTempItem = rkMyItem;
				kTempItem.EnchantInfo(kData.NewEnchantInfo());
				kTempItem.CreateDate(kData.NewGenDate());
				kItemWrapper.SetItem(kTempItem);

				hReturn = m_kInv.OverWriteItem(iCause, kItemWrapper, kChangeArray, kContLogMgr);
				if(S_OK == hReturn)
				{
					if ( iCause & IMET_PET )
					{
						PgDoc_PetInfo * pkDocPetInfo = LoadDocPetInfo( kItemWrapper.Guid(), true );
						if ( pkDocPetInfo )
						{
							DB_ITEM_STATE_CHANGE kDBData( DISCT_CREATE_PET, IMET_PET, GetID() );
							kDBData.ItemWrapper(kItemWrapper);
							kDBData.kAddonData.Push( kItemWrapper.Guid());
							pkDocPetInfo->RefreshSkillCoolTimeMap();
							pkDocPetInfo->WriteToPacket( kDBData.kAddonData );
							kChangeArray.push_back(kDBData);
						}
					}

					return S_OK;
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	case IMET_MODIFY_COUNT://타겟 대상으로 작업, 까기 올리기 둘다 지원.
		{
			SPMOD_Modify_Count kData;
			kOrder.Read(kData);
			
			hReturn = m_kInv.ModifyCount(iCause, kData, kChangeArray, kContLogMgr);			
			if(S_OK == hReturn)
			{
				return S_OK;
			}
		}break;
	case IMET_MODIFY_POS:
		{
			SPMOD_Modify_Pos kData;
			kOrder.Read(kData);

			PgItemWrapper kCasterItem;
			PgItemWrapper kTargetItem;

			SItemPos const &rkCasterPos = kData.OrgPos();
			SItemPos const &rkTargetPos = kData.TgtPos();

			//			DB_ITEM_STATE_CHANGE_ARRAY	kTempChangeArray;//Modify시 Pop을 기록할 이유는 없음. -> 아님. 빈자리에 갈 경우 Pop이 기록 되어야함. 그래야 깨끗함
			if(	S_OK == (hReturn = m_kInv.PopItem(iCause, rkCasterPos, kCasterItem, false, kChangeArray, kContLogMgr))
				&&	S_OK == (hReturn = m_kInv.PopItem(iCause, rkTargetPos, kTargetItem, false, kChangeArray, kContLogMgr)) )
			{//둘다 뺄 수 있어야지.
				if(	!kCasterItem.IsEmpty()
					||	!kTargetItem.IsEmpty())
				{//아이템이 둘다 비었어.
					if(	S_OK == (hReturn = m_kInv.PopItem(iCause, rkCasterPos, kCasterItem, true, kChangeArray, kContLogMgr))
						&&	S_OK == (hReturn = m_kInv.PopItem(iCause, rkTargetPos, kTargetItem, true, kChangeArray, kContLogMgr)) )
					{
						// 실제 ItemGuid와 MapServer가 요청한 ItemGuid를 한번 더 검사한다.
						//	이유 : 검사하지 않을 경우, 장착할 수 없는 아이템을 장착할 수 있는 버그 발생 (ContentsServer가 느릴 경우)
						BM::GUID const& rkOrgGuid = kData.OrgItemGuid();
						BM::GUID const& rkTgtGuid = kData.TgtItemGuid();
						if (rkOrgGuid.IsNotNull() && rkOrgGuid != kCasterItem.Guid()) { return E_FAIL; }
						if (rkTgtGuid.IsNotNull() && rkTgtGuid != kTargetItem.Guid()) { return E_FAIL; }

						{
							__int64 iCause2 = iCause;
							if(S_OK != (hReturn = PgInventory::ItemContainer::MergeItem(kCasterItem, kTargetItem, iCause2)))
							{//머징이 안되면 //위치 교환
								kCasterItem.Pos(rkTargetPos);
								kTargetItem.Pos(rkCasterPos);
							}

							if(	S_OK == (hReturn = m_kInv.OverWriteItem(iCause2, kCasterItem, kChangeArray, kContLogMgr))
								&&	S_OK == (hReturn = m_kInv.OverWriteItem(iCause2, kTargetItem, kChangeArray, kContLogMgr)))
							{
								if ( PgItem_PetInfo::ms_kPetItemEquipPos == rkCasterPos )
								{
									if ( 0 < GetCheckerValue(QCHK_EXPERANCE_PET) )
									{
										PgItem_PetInfo *pkPetInfo = NULL;
										if ( true == kCasterItem.GetExtInfo( pkPetInfo ) )
										{
											DB_ITEM_STATE_CHANGE kDBData( DISCT_SETABIL_PET, IMET_PET, GetID() );
											kDBData.ItemWrapper(kCasterItem);
											kDBData.kAddonData.Push( static_cast<size_t>(1) );
											kDBData.kAddonData.Push( SPMOD_SAbil( AT_EXPERIENCE, pkPetInfo->Exp() ) );
											kChangeArray.push_back(kDBData);
										}
										ClearChecker(QCHK_EXPERANCE_PET);
									}

									if ( !kTargetItem.IsEmpty() )
									{
										PgDoc_PetInfo * pkDocPetInfo = LoadDocPetInfo( kTargetItem.Guid(), true );
										if ( pkDocPetInfo )
										{
											DB_ITEM_STATE_CHANGE kDBData2( DISCT_CREATE_PET, IMET_PET, GetID() );
											kDBData2.ItemWrapper(kTargetItem);
											kDBData2.kAddonData.Push( kTargetItem.Guid() );
											pkDocPetInfo->RefreshSkillCoolTimeMap();
											pkDocPetInfo->WriteToPacket( kDBData2.kAddonData );
											kChangeArray.push_back(kDBData2);
										}
									}
								}
								else if ( PgItem_PetInfo::ms_kPetItemEquipPos == rkTargetPos )
								{
									if ( 0 < GetCheckerValue(QCHK_EXPERANCE_PET) )
									{
										PgItem_PetInfo *pkPetInfo = NULL;
										if ( true == kTargetItem.GetExtInfo( pkPetInfo ) )
										{
											DB_ITEM_STATE_CHANGE kDBData( DISCT_SETABIL_PET, IMET_PET, GetID() );
											kDBData.ItemWrapper(kTargetItem);
											kDBData.kAddonData.Push( static_cast<size_t>(1) );
											kDBData.kAddonData.Push( SPMOD_SAbil( AT_EXPERIENCE, pkPetInfo->Exp() ) );
											kChangeArray.push_back(kDBData);
										}
										ClearChecker(QCHK_EXPERANCE_PET);
									}

									if ( !kCasterItem.IsEmpty() )
									{
										PgDoc_PetInfo * pkDocPetInfo = LoadDocPetInfo( kCasterItem.Guid(), true );
										if ( pkDocPetInfo )
										{
											DB_ITEM_STATE_CHANGE kDBData2( DISCT_CREATE_PET, IMET_PET, GetID() );
											kDBData2.ItemWrapper(kTargetItem);
											kDBData2.kAddonData.Push( kCasterItem.Guid() );
											pkDocPetInfo->RefreshSkillCoolTimeMap();
											pkDocPetInfo->WriteToPacket( kDBData2.kAddonData );
											kChangeArray.push_back(kDBData2);
										}
									}
								}

								return S_OK;
							}
						}
					}
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	case IMET_INSERT_FIXED:
		{
			SPMOD_Insert_Fixed kData;
			kOrder.Read(kData);

			PgItemWrapper kCasterWrapper(kData.Item(), kData.Pos());
			const bool bIsAnyPos = kData.IsAnyPos();

			hReturn = m_kInv.InsertItem(iCause, kCasterWrapper, bIsAnyPos, false, kChangeArray, kContLogMgr);
			
			if(S_OK == hReturn)
			{
				return hReturn;
			}
		}break;
	case IMET_MODIFY_DB2INV:
		{
			SPMOD_DB2Inv kData;
			kOrder.Read(kData);

			PgItemWrapper kCasterWrapper(kData.Item(), kData.OrgPos());

			PgItemWrapper kBlankItemWrapper;
			DB_ITEM_STATE_CHANGE kItemChange(DISCT_REMOVE, iCause, kOrder.OwnerGuid(), m_kInv.OwnerGuid(), kCasterWrapper, kBlankItemWrapper); 
			kChangeArray.push_back(kItemChange);

			PgItemLogUtil::DeleteLog(GetID(),kContLogMgr,kCasterWrapper);

			hReturn = m_kInv.InsertItem(iCause, kCasterWrapper, true, true, kChangeArray, kContLogMgr);
			if(S_OK == hReturn)
			{
				return hReturn;
			}
		}break;
	case IMET_ADD_ANY:
		{
			SPMOD_Add_Any kData;
			kOrder.Read(kData);
			return m_kInv.ModifyItemAny(iCause, kData, kChangeArray, kContLogMgr);
		}break;
	case IMET_ADD_MONEY:
		{//돈 수정도 되네.
			SPMOD_Add_Money kData;
			kOrder.Read(kData);

			__int64 iBefore = m_kInv.Money();

			if( S_OK == (hReturn = m_kInv.AddMoney(kData.AddMoney(), false)))
			{//
				AddChecker( QCHK_MONEY );

				bool const bQuery = IsQuery( QCHK_MONEY );

				DB_ITEM_STATE_CHANGE kState(DISCT_MODIFY_MONEY, iCause, m_kInv.OwnerGuid());
				kState.kAddonData.Push(m_kInv.Money());
				kState.IsQuery( bQuery );

				if( iCause & IMC_GUILD_INV )
				{
					kState.State( DISCT_MODIFY_GUILD_MONEY );
					kState.IsQuery(true); // 길드금고의 골드는 바로 쿼리 한다.
				}

				PgBase_Item kItem;
				SEnchantInfo kEnchantInfo;
				kEnchantInfo.Field_1(iBefore);
				kEnchantInfo.Field_2(m_kInv.Money());
				kItem.EnchantInfo(kEnchantInfo);
				kItemWrapper.SetItem(kItem);
				kState.ItemWrapper(kItemWrapper);
				kChangeArray.push_back(kState);

				if ( bQuery )
				{
					ClearChecker( QCHK_MONEY );

					PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_DBSave, GetMemberGUID(), GetID(), MemberID(), Name(), GetAbil(AT_CLASS), GetAbil(AT_LEVEL), 0, static_cast<EUnitGender>(GetAbil(AT_GENDER)));
					PgLog kLog2(ELOrderMain_Money, ELOrderSub_DBSave);
					kLog2.Set(0, m_kInv.Money());
					kLogCont.Add(kLog2);
					kLogCont.Commit();
				}

				{
					PgLog kLog( ELOrderMain_Money,ELOrderSub_Modify);
					kLog.Set(0,iBefore);
					kLog.Set(1,m_kInv.Money());
					kLog.Set(2,kData.AddMoney());
					kContLogMgr.AddLog(GetID(),kLog);
				}

				return S_OK;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	case IMET_ADD_CP:
		{
			SPMOD_Add_CP kData;
			kOrder.Read(kData);

			int iBefore = m_kInv.CP();

			if( S_OK == (hReturn = m_kInv.AddCP(kData.AddCP(), false)))
			{
				DB_ITEM_STATE_CHANGE kState(DISCT_MODIFY_CP, iCause, m_kInv.OwnerGuid());
				kState.kAddonData.Push(m_kInv.CP());

				PgBase_Item kItem;
				SEnchantInfo kEnchantInfo;
				kEnchantInfo.Field_1(iBefore);
				kEnchantInfo.Field_2(m_kInv.CP());
				kItem.EnchantInfo(kEnchantInfo);
				kItemWrapper.SetItem(kItem);
				kState.ItemWrapper(kItemWrapper);

				kChangeArray.push_back(kState);
				return S_OK;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unhanded Cause[") << (kOrder.Cause() & IMET_CMP_BASE) << _T("] OwnerID[") << Name().c_str() << _T("]") );
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Unhanded Cause[") << (kOrder.Cause() & IMET_CMP_BASE) << _T("] OwnerID[") << Name().c_str() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}

	//	CAUTION_LOG(BM::LOG_LV5, _T("[%s-%d] Can't Process Cause[%I64d], ErrorCode[0x%08X] Owner[%s]"), __FUNCTIONW__, __LINE__, kOrder.Cause(), hReturn, m_kOwnerName.c_str());
	return hReturn;
}

HRESULT PgDoc_Player::UpdatePetInfo( BM::GUID const &kPetID, SPetMapMoveData &rkPetMapMoveData, bool const bGetSkillCoolTimeMap )
{
	PgBase_Item kPetItem;
	if (	S_OK == m_kInv.GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, kPetItem ) 
		&&	kPetID == kPetItem.Guid() )
	{
		PgDoc_PetInfo *pkDocPetInfo = LoadDocPetInfo( kPetID, true );
		if ( pkDocPetInfo )
		{
			PgInventory *pkInv = pkDocPetInfo->GetInven();
			if ( pkInv )
			{
				rkPetMapMoveData.kInventory = *pkInv;
			}
			
			if ( true == bGetSkillCoolTimeMap )
			{
				pkDocPetInfo->RefreshSkillCoolTimeMap();
				pkDocPetInfo->GetSkillCoolTimeMap( rkPetMapMoveData.kSkillCoolTime );
			}
		}

		if ( true == bGetSkillCoolTimeMap )
		{
			PgItem_PetInfo *pkPetInfo = NULL;
			if ( true == kPetItem.GetExtInfo( pkPetInfo ) )
			{
				if ( true == pkPetInfo->SetAbil( AT_MP, rkPetMapMoveData.iMP ) )
				{
					m_kInv.SwapItem( PgItem_PetInfo::ms_kPetItemEquipPos, kPetItem );//swap

					// Pet's MP Save
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SETABIL_PET, L"EXEC [dbo].[up_Update_Pet]");
					kQuery.InsertQueryTarget(GetID());
					kQuery.PushStrParam(kPetID);
					kQuery.PushStrParam(std::wstring(L"MP"));
					kQuery.PushStrParam(rkPetMapMoveData.iMP);
					g_kCoreCenter.PushQuery( kQuery );
				}
			}
		}
		return S_OK;
	}
	return E_FAIL;
}

PgInventory* PgDoc_Player::GetPetInven( BM::GUID const &kPetID )
{
	CONT_DOC_PET_INFO::iterator pet_info_itr = m_kContDocPetInfo.find( kPetID );
	if ( pet_info_itr != m_kContDocPetInfo.end() )
	{
		return pet_info_itr->second.GetInven();
	}
	return NULL;
}

PgDoc_PetInfo * PgDoc_Player::SetDocPetInfo( BM::GUID const &kPetID, PgInventory &kPetInv )
{
	bool bSelectedPet = false;
	PgBase_Item kPetItem;
	if ( S_OK == m_kInv.GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, kPetItem ) )
	{
		if ( kPetID == kPetItem.Guid() )
		{
			bSelectedPet = true;
		}
	}

	if (	!bSelectedPet &&
			(S_OK != m_kInv.GetItem( kPetID, kPetItem ))
		)
	{
		return NULL;
	}
	
	PgItem_PetInfo *pkPetInfo = NULL;
	if ( true == kPetItem.GetExtInfo( pkPetInfo ) )
	{
		CONT_DOC_PET_INFO::iterator pet_info_itr = m_kContDocPetInfo.find( kPetItem.Guid() );
		if ( pet_info_itr == m_kContDocPetInfo.end() )
		{
			auto kPair = m_kContDocPetInfo.insert( std::make_pair( kPetItem.Guid(), CONT_DOC_PET_INFO::mapped_type() ) );
			pet_info_itr = kPair.first;
		}

		kPetInv.OwnerName( pkPetInfo->Name() );
		pet_info_itr->second.SetInven(kPetInv);
		return &(pet_info_itr->second);
	}
	return NULL;
}

PgDoc_PetInfo * PgDoc_Player::LoadDocPetInfo( BM::GUID const &kPetID, bool const bIsLoadDB )
{
	CONT_DOC_PET_INFO::iterator pet_info_itr = m_kContDocPetInfo.find( kPetID );
	if ( pet_info_itr != m_kContDocPetInfo.end() )
	{
		if ( true == pet_info_itr->second.IsLoadedInven() )
		{
			return &(pet_info_itr->second);
		}
	}

	if ( true == bIsLoadDB )
	{
		// DB에서 불러와야 한다.
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LOAD_PET_ITEM, L"EXEC [dbo].[up_Item_Select]");
		kQuery.InsertQueryTarget( GetID() );
		kQuery.QueryOwner( GetID() );
		kQuery.PushStrParam( kPetID );

		kQuery.contUserData.Push( GetID() );
		kQuery.contUserData.Push( kPetID );

		g_kCoreCenter.PushQuery( kQuery );
	}

	return NULL;
}

void PgDoc_Player::RemovePetInfo( BM::GUID const &kPetID )
{
	m_kContDocPetInfo.erase( kPetID );
}

void PgDoc_Player::StrategySkillInit(SPlayerStrategySkillData const & kData)
{
	m_kStrategySkill.iSP = kData.sSP;

	m_kStrategySkill.kMySkill.Init();
	m_kStrategySkill.kMySkill.Add(MAX_DB_SKILL_SIZE, kData.abySkills);
	m_kStrategySkill.kMySkill.InitSkillExtend(MAX_DB_SKILL_EXTEND_SIZE, kData.abySkillExtends);
	m_kStrategySkill.kQInv.Init(kData.abyQuickSlot);
}

void PgDoc_Player::StrategySkillSetInit(CONT_USER_SKILLSET const & kContSkillSet, CONT_USER_SKILLSET const & kContSkillSet_Sec)
{
	m_kMySkill.ContSkillSet(kContSkillSet);
	m_kStrategySkill.kMySkill.ContSkillSet(kContSkillSet_Sec);
}

bool PgDoc_Player::SwapMySkill(ESkillTabType const eTabType)
{
	if( false==IsOpenStrategySkill(eTabType) )
	{
		return false;
	}

	if( GetAbil(AT_STRATEGYSKILL_TABNO) == eTabType )
	{
		return false;
	}

	int const iSP = GetAbil(AT_SP);
	SetAbil(AT_SP, m_kStrategySkill.iSP);
	m_kStrategySkill.iSP = iSP;

	SetAbil(AT_STRATEGYSKILL_TABNO, eTabType);

	m_kMySkill.Swap(m_kStrategySkill.kMySkill);
	m_kQInv.Swap(m_kStrategySkill.kQInv);
	return true;
}

void PgDoc_Player::SetStrategyMySkill(PgMySkill const * pkMySkill) 
{ 
	if( pkMySkill )
	{
		m_kStrategySkill.kMySkill = *pkMySkill;
	}
}

void PgDoc_Player::SendToAchievementMgr(short const Level)
{
	// switch( g_kLocal.ServiceRegion() )
	// {
	// case LOCAL_MGR::NC_CHINA:
	// case LOCAL_MGR::NC_DEVELOP:
		// {
			switch( Level )
			{// 레벨업 시 전체 유저의 알리미에 표시
			case 50: case 60: case 70: case 80: case 90: case 100:
				{
					BM::Stream Packet;
					Packet.Push( this->Name() );
					Packet.Push( Level );

					g_kAchievementMgr.Broadcast(EAchievementType::E_ACHI_LEVELUP, Packet);
				}break;
			}
		// }break;
	// }
}

#ifdef _USE_JOBCHECKER
void PgDoc_Player::AddJobChecker( __int64 const iType )
{
	if ( m_pkJobChecker )
	{
		m_pkJobChecker->Add( iType );
	}
}

void PgDoc_Player::DisplayJobChecker()const
{
	if ( m_pkJobChecker )
	{
		INFO_LOG( BM::LOG_YELLOW, __FL__ << _T("-------- ") << Name().c_str() << _T(" -----------") );
		m_pkJobChecker->Display();
	}
}

void PgDoc_Player::CreateJobChecker()
{
	if ( !m_pkJobChecker )
	{
		m_pkJobChecker = new PgJobChecker;
	}
}
#endif


void PgDoc_MyHome::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket,kWriteType);

	m_kNetModule.GetNet().WriteToPacket( rkPacket );
	rkPacket.Push( m_kVolatileID );
	PgPlayerPlayTime::WriteToPacket_PlayTime(rkPacket);

	switch( kWriteType&WTCHK_TYPE )
	{
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
		{
			//SPlayerDBData
			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				rkPacket.Push( GetMemberGUID() );
				rkPacket.Push( m_kInfo );
			}

			rkPacket.Push( GetPos().x );
			rkPacket.Push( GetPos().y );
			rkPacket.Push( GetPos().z );

			rkPacket.Push( m_kBasic );//SPlayerBasicInfo

			m_kInv.WriteToPacket(rkPacket, kWriteType);

			rkPacket.Push( ConnArea() );
			rkPacket.Push( m_kMemberID );

			rkPacket.Push( GetAbil(AT_MYHOME_TODAYHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_TOTALHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_VISITLOGCOUNT) );

		}break;
	case WT_SIMPLE:
		{
//			SPlayerDBSimpleData kSimpleData;
//			GetInfo(kSimpleData);
//			rkPacket.Push( kSimpleData );
//			이전에 SPlayerDBSimpleData로 되어있던것
			rkPacket.Push( GetMemberGUID() );
			rkPacket.Push( GetID() );
			rkPacket.Push( PartyGuid() );
			rkPacket.Push( GuildGuid() );
			rkPacket.Push( CoupleGuid() );//5
			rkPacket.Push( Name() );
			rkPacket.Push( m_kInfo.byGender );
			rkPacket.Push( m_kInfo.iRace );
			rkPacket.Push( m_kInfo.iClass );
			rkPacket.Push( m_kInfo.wLv );//10
			rkPacket.Push( m_kInfo.sTacticsLevel );
			rkPacket.Push( GetPos() );
			rkPacket.Push( m_kInfo.iHairColor );
			rkPacket.Push( m_kInfo.iHairStyle );
			rkPacket.Push( m_kInfo.iFace );//15
			rkPacket.Push( m_kInfo.iJacket );
			rkPacket.Push( m_kInfo.iPants );
			rkPacket.Push( m_kInfo.iShoes );
			rkPacket.Push( m_kInfo.iGloves );
			rkPacket.Push( m_kInfo.byFiveElementBody );//20
			rkPacket.Push( m_kInfo.kBirthDate );
			rkPacket.Push( GetAbil(AT_RECENT) );
			rkPacket.Push( m_byGMLevel );
			rkPacket.Push( m_bIsPCCafe );
			rkPacket.Push( m_byPCCafeGrade );
			rkPacket.Push( m_kIsCreateCard);
			rkPacket.Push( GetAbil(AT_HP) );// 24
			
			//SIMPLE_EXTRA_USERINFO
			rkPacket.Push( GetActionID() );

			rkPacket.Push( (BYTE)GetAbil(AT_TEAM) );//팀

			m_kInv.WriteToPacket(IT_HOME, rkPacket);

			rkPacket.Push( GetAbil(AT_CLIENT_OPTION_SAVE) );

			m_kCardAbilObj.WriteToPacket(rkPacket);

			rkPacket.Push( GetAbil(AT_MYHOME_TODAYHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_TOTALHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_VISITLOGCOUNT) );

			rkPacket.Push( ExpeditionGuid() );
			rkPacket.Push( ExpeditionPartyGuid() );

		}break;
	default:
		{
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled WriteType[%d]"), __FUNCTIONW__, kWriteType);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning! Invalid CaseType"));
		}break;
	}

	if (WT_DEFAULT_EXCLUDE & kWriteType)
	{
		m_kGroundKey.WriteToPacket(rkPacket);
	}

	if(0 == (kWriteType & WT_OP_No_Contents)
		&& ( WT_OP_MISSION & kWriteType ))
	{
		m_kMissionReport.WriteToPacket_MissionReport( rkPacket );
	}

	if( kWriteType & WT_OP_DBPlayerState )
	{
		rkPacket.Push( m_kInfo.byState );
	}

	rkPacket.Push( m_iUID );
	rkPacket.Push(m_kOwnerGuid);
	rkPacket.Push(m_kHomeAddr);
	rkPacket.Push(m_kIsInSide);
	rkPacket.Push(m_kOwnerName);
	rkPacket.Push(GetAbil(AT_MYHOME_VISITFLAG));
	rkPacket.Push(GetAbil(AT_MYHOME_STATE));
	rkPacket.Push(GetAbil(AT_HOME_SIDEJOB));
}

EWRITETYPE PgDoc_MyHome::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE const kWriteType = CUnit::ReadFromPacket(rkPacket);

	BM::GUID kMemberGuid = GetMemberGUID();
	SERVER_IDENTITY kSwitchServer;

	kSwitchServer.ReadFromPacket(rkPacket);
	rkPacket.Pop( m_kVolatileID );
	PgPlayerPlayTime::ReadFromPacket_PlayTime(rkPacket);
	
	switch( kWriteType & WTCHK_TYPE )
	{
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
		{
			//SPlayerInfo;
			BYTE byTeam = 0;
			POINT3 kPos;
			__int64 iVal64 = 0;	
			int iVal = 0;

			//SPlayerDBData
			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				rkPacket.Pop( kMemberGuid );
				rkPacket.Pop( m_kInfo );				//1
			}

			rkPacket.Pop( kPos.x );
			rkPacket.Pop( kPos.y );
			rkPacket.Pop( kPos.z ); SetPos(kPos);

			rkPacket.Pop( m_kBasic );//SPlayerBasicInfo	

			m_kInv.ReadFromPacket(rkPacket, kWriteType);

			rkPacket.Pop( m_kConnArea );				
			rkPacket.Pop( m_kMemberID );

			int iValue = 0;
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TODAYHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TOTALHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_VISITLOGCOUNT,iValue);

			Name(m_kInfo.szName);
			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				m_kInv.OwnerGuid(m_kInfo.guidCharacter);
				m_kInv.OwnerName(m_kInfo.szName);
				InvenRefreshAbil();
			}

			MacroResetInputTime();					// 매크로 입력 시간은 맵 이동시마다 다시 초기화

			SetAbil(AT_ENABLE_AUTOHEAL, AUTOHEAL_PLAYER);

			CopyAbilFromClassDef();
			CalculateInitAbil();

		}break;
	case WT_SIMPLE:
		{
			SetState(US_IDLE);

//			SPlayerDBSimpleData kSimpleData;
//			rkPacket.Pop( kSimpleData );

//			이전에 SPlayerDBSimpleData로 되어 있던것
			rkPacket.Pop( kMemberGuid );
			rkPacket.Pop( m_kInfo.guidCharacter );	SetID( m_kInfo.guidCharacter );
			rkPacket.Pop( m_kPartyGuid );
			rkPacket.Pop( m_kInfo.kGuildGuid );
			rkPacket.Pop( m_kInfo.kCoupleGuid );//5
			rkPacket.Pop( m_kName );
			rkPacket.Pop( m_kInfo.byGender );
			rkPacket.Pop( m_kInfo.iRace );
			rkPacket.Pop( m_kInfo.iClass );
			rkPacket.Pop( m_kInfo.wLv );//10
			rkPacket.Pop( m_kInfo.sTacticsLevel );

			POINT3 kPos;
			rkPacket.Pop( kPos );
			SetPos( kPos );

			rkPacket.Pop( m_kInfo.iHairColor );
			rkPacket.Pop( m_kInfo.iHairStyle );
			rkPacket.Pop( m_kInfo.iFace );//15
			rkPacket.Pop( m_kInfo.iJacket );
			rkPacket.Pop( m_kInfo.iPants );
			rkPacket.Pop( m_kInfo.iShoes );
			rkPacket.Pop( m_kInfo.iGloves );
			rkPacket.Pop( m_kInfo.byFiveElementBody );//20
			rkPacket.Pop( m_kInfo.kBirthDate );
			rkPacket.Pop( m_kInfo.kRecentInfo.iMapNo );
			rkPacket.Pop( m_byGMLevel );
			rkPacket.Pop( m_bIsPCCafe );
			rkPacket.Pop( m_byPCCafeGrade );
			rkPacket.Pop( m_kIsCreateCard);

			int iVal = 0;
			rkPacket.Pop( iVal );//24
			SetAbil( AT_HP, iVal );

			//SIMPLE_EXTRA_USERINFO
			rkPacket.Pop( iVal );	SetActionID(iVal);

			_tcscpy_s( m_kInfo.szName, MAX_CHARACTERNAME_LEN, Name().c_str() );

			BYTE byVal = 0;
			rkPacket.Pop( byVal ); SetAbil(AT_TEAM, static_cast<int>(byVal) );//팀
	
			//INV
			m_kInv.ReadFromPacket(IT_HOME, rkPacket);

			int iClientSaveOption = 0;
			rkPacket.Pop( iClientSaveOption );
			SetAbil(AT_CLIENT_OPTION_SAVE, iClientSaveOption);

			SPlayerBasicInfo kBasic;
			GET_DEF(PgClassDefMgr, kClassDefMgr);
			kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), kBasic);
			m_kBasic = kBasic;

			m_kCardAbilObj.ReadFromPacket(rkPacket);
			int iValue = 0;
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TODAYHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TOTALHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_VISITLOGCOUNT,iValue);

			rkPacket.Pop( m_kExpeditionGuid );
			rkPacket.Pop( m_kExpeditionPartyGuid);

			m_kQuest.Clear();
			CopyAbilFromClassDef();
			CalculateInitAbil();
		}break;
	}

	if (WT_DEFAULT_EXCLUDE & kWriteType)
	{
		m_kGroundKey.ReadFromPacket(rkPacket);
	}

	if(0 == (kWriteType & WT_OP_No_Contents) 
		&& ( WT_OP_MISSION & kWriteType ))
	{
		m_kMissionReport.ReadFromPacket_MissionReport(rkPacket);
	}

	if( kWriteType & WT_OP_DBPlayerState )
	{
		rkPacket.Pop( m_kInfo.byState );
	}

	rkPacket.Pop( m_iUID );

	rkPacket.Pop(m_kOwnerGuid);
	rkPacket.Pop(m_kHomeAddr);
	rkPacket.Pop(m_kIsInSide);
	rkPacket.Pop(m_kOwnerName);

	int iValue = 0;
	rkPacket.Pop(iValue); SetAbil(AT_MYHOME_VISITFLAG,iValue);
	rkPacket.Pop(iValue); SetAbil(AT_MYHOME_STATE,iValue);
	rkPacket.Pop(iValue); SetAbil(AT_HOME_SIDEJOB,iValue);

	Latency(0);
	m_kHackIndex.clear();

	//
	m_kNetModule.Set( kMemberGuid, kSwitchServer );

	return kWriteType;
}

void PgDoc_MyHome::CopyTo(PgDoc_Player & rkPlayer, EWRITETYPE const eType) const
{
	this->::PgDoc_Player::CopyTo(rkPlayer,eType);
	
	PgDoc_MyHome * pkHome = dynamic_cast<PgDoc_MyHome *>(&rkPlayer);
	if(NULL == pkHome)
	{
		return;
	}

	pkHome->m_kOwnerGuid = m_kOwnerGuid;
	pkHome->m_kIsInSide = m_kIsInSide;
	pkHome->m_kOwnerName = m_kOwnerName;
}
