#include "StdAfx.h"
#include <Mmsystem.h>
#include "Global.h"
#include "PgMyQuest.h"
#include "PgPlayer.h"
#include "TableDataManager.h"
#include "PgClassDefMgr.h"
#include "PgQuestInfo.h"
#include "PgMacroCheckTable.h"

PgPlayer::VECTOR_MAX_HACKINDEX PgPlayer::m_sMaxHackIndex;

PgPlayer::PgPlayer(void)
:	m_iUID(0)
,	m_byGMLevel(0)
,	m_bIsPCCafe(false)
,	m_byPCCafeGrade(0)
,	m_bDoSimulate(false)
,	m_kGmAbil(GMABIL_NONE)
,	m_kExtendCharacterNum(0)
,	m_iAccruedDamage(0)
,	m_iAccruedHeal(0)
,	m_RecentNormalMap(0, 0, POINT3(0, 0, 0))
,	m_kDefenceMssionExpBonusRate(0.0f)
,	m_kBearEffect(false)
,	m_bInitComplete(false)
{
//	m_kInfo;
//	m_kSwitchServer;

//	m_kActionInfo;	// Client와 Player의 위치 동기화를 위해서 필요하다.
	m_kOldActionInfo.Init();
//	MemberID.clear();
	
	NotifyTime(0);	// SpeedHack 을 막기위해 클라이언트가 자신의 GameTime 값을 계속 보낸다.
	Latency(0);
//	OldPos;
//	CLASS_DECLARATION_S(POINT3BY, MoveDirection);
//	CLASS_DECLARATION_S(BM::GUID, FollowingHeadGuid);// 따라가기 주체자(Head) GUID
//	CLASS_DECLARATION_S(SConnectionArea, ConnArea);
//	CLASS_DECLARATION_S(SGroundKey, GroundKey);
	Cash(0);
	
	m_ksJumpHeight = 0;	// AT_JUMP_HEIGHT
	m_ksCJumpHeight = 0;	// AT_C_JUMP_HEIGHT
	m_ksAddMoneyPer = 0;	// AT_ADD_MONEY_PER
	PosAtGround(POINT3::NullData());
	PosAtGroundTime(0);

//	PgPlayer_MissionReport	m_kMissionReport;
//	mutable PgQuickInventory m_kQInv;
//	PgMyQuest m_kQuest;
	
//	std::vector<BM::Stream> m_kPacketBuffer;
//	const static int m_iPacketBufferMaxSize = 80;
	MacroClear();

	m_kIsCreateCard = false;

	AchievementLastSaveTime(BM::GetTime32());

	OpenVendor(false);
	VendorTitle(_T(""));
	m_kCardAbilObj.Clear();
	m_kJobSkillExpertness.Clear();

	VisibleMyMinimapIcon(true);
}

PgPlayer::~PgPlayer(void)
{
}

HRESULT PgPlayer::Create( BM::GUID const &guidMember, SPlayerDBData const &kDBInfo, SPlayerBasicInfo const &kBasicInfo, SPlayerBinaryData const &kBinaryData )//Create 는 SPlayerInfo만을 사용해서 하라.
{
	PgPlayer::Init();

	SetState(US_OPENING);

	m_kNetModule.SetID( guidMember );
	SetID(kDBInfo.guidCharacter);

	m_kInfo = kDBInfo;
	m_kBasic = kBasicInfo;

	SetAbil64(AT_EXPERIENCE, kDBInfo.i64Exp);
	SetAbil64(AT_MONEY, kDBInfo.i64Money);
	SetAbil64(AT_FRAN, kDBInfo.i64Fran);

	SetAbil(AT_HP, kDBInfo.iHP);
	SetAbil(AT_MP, kDBInfo.iMP);	
	SetAbil(AT_CP, kDBInfo.iCP);

	m_kQInv.Init(kBinaryData.abyQuickSlot);// 플레이어의 퀵인벤정보

	m_kMySkill.Init();
	m_kMySkill.Add(MAX_DB_SKILL_SIZE, kBinaryData.abySkills);
	m_kMySkill.InitSkillExtend(MAX_DB_SKILL_EXTEND_SIZE, kBinaryData.abySkillExtends);

	m_kQuest.ReadFromBuff(PgQuestIngBufferR(kBinaryData.abyIngQuest), PgQuestEndBufferR(kBinaryData.abyEndQuest), PgQuestEndBufferR(kBinaryData.abyEndQuest2), PgQuestEndExtBufferR(kBinaryData.abyEndQuestExt), PgQuestEndExt2BufferR(kBinaryData.abyEndQuestExt2));

	SetPos( kDBInfo.pt3Pos );
	Name(kDBInfo.szName);

	DWORD const dwCurTime = BM::GetTime32();
	m_dwElapsedTimeCheckZone = dwCurTime;

	InvenRefreshAbil();
	m_kMySkill.Add(MAX_DB_GUILD_SKILL_SIZE, kDBInfo.cGuildSkill);
	SetTotalPlayTime_Character( kDBInfo.i64TotalPlayTimeSec_Character );
	m_kEffect.LoadFromDB(kBinaryData.abyEffect, MAX_DB_EFFECT_SIZE);

	m_kEmoticon.Set(kBinaryData.abyEmoticon);
	m_kEmotion.Set(kBinaryData.abyEmotion);
	m_kBalloon.Set(kBinaryData.abyBalloon);

	//	kBinaryData
	// 기본으로 주어지는 Abil
	SetAbil(AT_ENABLE_AUTOHEAL, AUTOHEAL_PLAYER);	// AutoHeal 능력은 기본으로 준다.
	CopyAbilFromClassDef();

	CalculateInitAbil();

	Latency(0);

	m_kInv.OwnerGuid(GetID());
	m_kInv.OwnerName(Name());
	m_kInv.MemberGuid(guidMember);

	for(int i=0; i<MAX_JOBSKILL_HISTORYITEM; ++i)
	{
		if(kBinaryData.dwJobSkillHistoryItem[i])
		{
			AddJobSkillHistoryItem(kBinaryData.dwJobSkillHistoryItem[i]);
		}
	}

	m_kDefenceMssionExpBonusRate = 0.0f;
	m_kBearEffect = false;

	return S_OK;
}

ERACE_TYPE PgPlayer::UnitRace(void) const
{
	SClassKey kClassKey = GetClassKey();
	if(kClassKey.iClass > 50)
	{
		return ERACE_DRAGON;
	}//현재 PC 종족은 인간/용족 뿐이므로 용족 외에는 모두 인간으로 취급한다.
	return ERACE_HUMAN;
}

void PgPlayer::SetCardAbil(VEC_CARD_ABIL const& rkContCardAbil)
{
	m_kCardAbilObj.Clear();
	for(VEC_CARD_ABIL::const_iterator iter = rkContCardAbil.begin();iter != rkContCardAbil.end();++iter)
	{
		m_kCardAbilObj.AddAbil((*iter).wAbilType,(*iter).iValue);
	}
	if( CEL::ST_MAP == g_kProcessCfg.ServerIdentity().nServerType ) // 맵서버 만 브로드 캐스팅
	{
		BM::Stream kPacket(PT_M_C_NFY_CARD_REFRESH_ABIL);
		kPacket.Push( GetID() );
		kPacket.Push( rkContCardAbil );

		BM::Stream kNfyPacket(PT_U_G_RUN_ACTION);
		kNfyPacket.Push( static_cast< short >(GAN_BroadCastSmallArea) );
		kNfyPacket.Push( kPacket.Data() );
		VNotify(&kNfyPacket);
	}
}
void PgPlayer::ResetCardAbil()
{
	m_kCardAbilObj.Clear();
}

void PgPlayer::CalculateInitAbil()
{
	CUnit::CalculateInitAbil();

	PgBase_Item kItem;
	if(S_OK == m_kInv.GetItem( SItemPos(IT_FIT, EQUIP_POS_WEAPON), kItem))
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if( pDef )
		{
			SetAbil( AT_EQUIP_WEAPON_PHY_ATTACK_MIN, pDef->ImproveAbil(AT_PHY_ATTACK_MIN, PgBase_Item::NullData()) );
			SetAbil( AT_EQUIP_WEAPON_PHY_ATTACK_MAX, pDef->ImproveAbil(AT_PHY_ATTACK_MAX, PgBase_Item::NullData()) );
			SetAbil( AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN, pDef->ImproveAbil(AT_MAGIC_ATTACK_MIN, PgBase_Item::NullData()) );
			SetAbil( AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX, pDef->ImproveAbil(AT_MAGIC_ATTACK_MAX, PgBase_Item::NullData()) );
		}
	}
	else
	{
		SetAbil( AT_EQUIP_WEAPON_PHY_ATTACK_MIN, 0 );
		SetAbil( AT_EQUIP_WEAPON_PHY_ATTACK_MAX, 0 );
		SetAbil( AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN, 0 );
		SetAbil( AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX, 0 );
	}
}

/*
// Called only in Client.......
bool PgPlayer::SetInfo(SPlayerDBSimpleData const &rkSimple, SPlayerBasicInfo* pkBasic)
{
	m_kInfo.guidMember = rkSimple.guidMember;
	m_kInfo.guidCharacter = rkSimple.guidCharacter;
	m_kPartyGuid = rkSimple.kPartyGuid;
	m_kInfo.kGuildGuid = rkSimple.kGuildGuid;
	m_kInfo.kCoupleGuid = rkSimple.kCoupleGuid;

	_tcscpy_s(m_kInfo.szName, MAX_CHARACTERNAME_LEN, rkSimple.szName);
	m_kInfo.byGender = rkSimple.byGender;
	m_kInfo.iRace = rkSimple.iRace;
	m_kInfo.iClass = rkSimple.iClass;
	m_kInfo.wLv = rkSimple.wLv;
//	m_kInfo.iHP = rkSimple.iHP;
	
	m_kInfo.sTacticsLevel = rkSimple.sTacticsLevel;

	SetPos(rkSimple.pt3RecentPos);

	m_kInfo.iHairColor = rkSimple.iHairColor;
	m_kInfo.iHairStyle = rkSimple.iHairStyle;
	m_kInfo.iFace = rkSimple.iFace;
	m_kInfo.iJacket = rkSimple.iJacket;
	m_kInfo.iPants = rkSimple.iPants;
	m_kInfo.iShoes = rkSimple.iShoes;
	m_kInfo.iGloves = rkSimple.iGloves;
	
	m_kInfo.byFiveElementBody = rkSimple.byFiveElementBody;//
	
	m_kInfo.kRecentInfo.iMapNo = rkSimple.iRecentMap;
	m_kInfo.kBirthDate = rkSimple.kBirthDate;

//////////////////////////////////////////////////////////////////////////////////////////
	SetID(m_kInfo.guidCharacter);

	Name(m_kInfo.szName);

//	SetAbil(AT_HP, m_kInfo.iHP);
	SetState(US_IDLE);
	
	if(pkBasic != NULL)
	{
		::memcpy_s(&m_kBasic, sizeof(m_kBasic), pkBasic, sizeof(*pkBasic));
	}
	
	m_kQuest.Clear();
	CopyAbilFromClassDef();
	CalculateInitAbil();
	return true;
}
*/

int PgPlayer::GetAbil(WORD const Type) const
{
	int iValue = 0;
	switch(Type)
	{
	case AT_SP:							{ iValue = m_kInfo.sSP; }break;
	case AT_JUMP_HEIGHT:				{ iValue = m_ksJumpHeight; }break;
	case AT_C_JUMP_HEIGHT:				{ iValue = m_ksCJumpHeight; }break;
	case AT_ADD_MONEY_PER:				{ iValue = m_ksAddMoneyPer; }break;
	case AT_FIVE_ELEMENT_TYPE_AT_BODY:	{ iValue = m_kInfo.byFiveElementBody; }break;
	case AT_LEVEL:						{ iValue = m_kInfo.wLv; }break;
	case AT_CLASS:						{ iValue = m_kInfo.iClass; }break;
	case AT_GENDER:						{ iValue = m_kInfo.byGender; }break;
	case AT_RACE:						{ iValue = m_kInfo.iRace; }break;
	case AT_TACTICS_LEVEL:				{ iValue = m_kInfo.sTacticsLevel; }break;
	case AT_LAST_VILLAGE:				{ iValue = m_kInfo.kLastVillage.iMapNo;		}break;
	case AT_RECENT:						{ iValue = m_kInfo.kRecentInfo.iMapNo;		}break;
	case AT_CP:							{ iValue = m_kInv.CP();	}break;
	case AT_CHARACTOR_SLOT:				{ iValue = static_cast<int>(m_kInfo.byCharactorSlot);	}break;
	case AT_STRATEGY_FATIGABILITY:		{ iValue = static_cast<int>(m_kInfo.Strategy_fatigability); }break;
	case AT_OWNER_TYPE:
		{
			return UT_PLAYER;
		}break;
	case AT_BASE_CLASS:
		{
			iValue = GetAbil( AT_CLASS );
			CONT_DEFUPGRADECLASS const* pkDefUpgradeClass = NULL;
			g_kTblDataMgr.GetContDef( pkDefUpgradeClass );
			if ( pkDefUpgradeClass )
			{
				CONT_DEFUPGRADECLASS::const_iterator itr = pkDefUpgradeClass->find( iValue );
				if ( itr != pkDefUpgradeClass->end() )
				{
					iValue = static_cast<int>(itr->second.byKind);
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found Class<" << iValue << L"> CharacterGUID<" << GetID() << L">");
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"CONT_DEFUPGRADECLASS is NULL");
			}
		}break;
	case AT_ACHIEVEMENT_TOTAL_POINT:
	case AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_1:
	case AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_2:
	case AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_3:
	case AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_4:
	case AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_5:
	case AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_6:
		{
			iValue = m_kAchievements.GetAchievementPoint( static_cast< EAchievementsCategory >(Type-AT_ACHIEVEMENT_TOTAL_POINT) );
		}break;
	case AT_STATUS_BONUS:		{ iValue = m_kInfo.sBonusStatus; }break;
	case AT_STR_ADD:	{ iValue = m_kInfo.sAddedStr; } break;
	case AT_INT_ADD:	{ iValue = m_kInfo.sAddedInt; } break;
	case AT_CON_ADD:	{ iValue = m_kInfo.sAddedCon; } break;
	case AT_DEX_ADD:	{ iValue = m_kInfo.sAddedDex; } break;
	case AT_LEVEL_RANK:	{ iValue = static_cast<int>(m_kLevelRank);}break;
// 아래는 64 비트 어빌
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Do not use this in GetAbil(use GetAbil64) Never Ever!") );
		}break;
	case AT_STRATEGYSKILL_OPEN:			{ iValue = m_kInfo.StrategySkillOpen; }break;
	case AT_STRATEGYSKILL_TABNO:		{ iValue = m_kInfo.StrategySkillTabNo; }break;
	default:
		{
			iValue = CUnit::GetAbil(Type);
			//if (iValue == 0)
			//{
			//	iValue = g_ClassDef.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), Type);
				// 함수 선언부에 있는 const 제한자 때문에..
				// 성능향상을 위해 Caching 한다.
				// 사실 아래 한줄을 주석 달아도 기능에는 문제가 없다...(더 좋은 방법을 찾아 보자)
				//((CUnit*)this)->SetAbil(Type, iValue);
			//}
		}break;
	}

	if( m_kCardAbilObj.HasAbil(Type) )
	{
		iValue += m_kCardAbilObj.GetAbil(Type);
	}

	//iValue += m_kInv.GetAbil(Type);//인벤에서 적용된 어빌

	return iValue;
}

// temporary for level/class to bypass DoLevelUp
bool PgPlayer::SetAbilDirect(WORD const Type, int const iValue)
{
	switch(Type)
	{
	case AT_LEVEL:
		{ 
			m_kInfo.wLv = static_cast<WORD>(iValue);
		}break;
	case AT_CLASS:
		{ 
			m_kInfo.iClass = iValue;
		}break;
	}
	return true;
}
	
bool PgPlayer::SetAbil(WORD const Type, int const iInValue, bool const bIsSend, bool const bBroadcast)
{
	int iValue = iInValue;
	SetUsedAchievementAbilList(Type);
	CheckSetAbil(Type, iValue);

	bool bIntercept = true;
	switch(Type)
	{
	case AT_LEVEL:
	case AT_CLASS:
	case AT_BATTLE_LEVEL:
	case AT_LEVEL_RANK:
		{ 
			return PgControlUnit::SetAbil( Type, iValue, bIsSend, bBroadcast );
		}break;
	case AT_GENDER:						{ m_kInfo.byGender = iValue; }break;
	case AT_RACE:						{ m_kInfo.iRace = iValue; }break;
	case AT_SP:							{ m_kInfo.sSP = iValue; }break;
	case AT_JUMP_HEIGHT:				{ m_ksJumpHeight = iValue; }break;
	case AT_C_JUMP_HEIGHT:				{ m_ksCJumpHeight = iValue; }break;
	case AT_ADD_MONEY_PER:				{ m_ksAddMoneyPer = iValue; }break;
	case AT_FIVE_ELEMENT_TYPE_AT_BODY:	{ m_kInfo.byFiveElementBody = iValue; }break;
	case AT_TACTICS_LEVEL:				{ m_kInfo.sTacticsLevel = iValue; }break;
	case AT_LAST_VILLAGE:				{ m_kInfo.kLastVillage.iMapNo = iValue; }break;
	case AT_CP:							{ m_kInv.CP(iValue);	}break;
	case AT_STR_ADD:					{ m_kInfo.sAddedStr = iValue; } break;
	case AT_INT_ADD:					{ m_kInfo.sAddedInt = iValue; } break;
	case AT_CON_ADD:					{ m_kInfo.sAddedCon = iValue; } break;
	case AT_DEX_ADD:					{ m_kInfo.sAddedDex = iValue; } break;
	case AT_STATUS_BONUS:				{ m_kInfo.sBonusStatus = iValue; }break;
	case AT_STRATEGYSKILL_OPEN:			{ m_kInfo.StrategySkillOpen = iValue; }break;
	case AT_STRATEGYSKILL_TABNO:		{ m_kInfo.StrategySkillTabNo = iValue; }break;
	case AT_CHARACTOR_SLOT:				{ m_kInfo.byCharactorSlot = static_cast<BYTE>(iValue); }break;
	case AT_STRATEGY_FATIGABILITY:		{ m_kInfo.Strategy_fatigability = static_cast<short>(iValue); }break;
	default:
		{
			bIntercept = false;
			bool bNeedCalc = false;
			if(Type==AT_TEAM || Type==AT_DUEL)
			{
				int const pastvalue = GetAbil(Type);
				if(pastvalue != iInValue)
				{
					if(0==pastvalue || 0==iInValue)	//팀 속성이 있다가 없거나 없다가 있을 경우
					{
						bNeedCalc = true;	
					}
				}
			}
			CUnit::SetAbil(Type, iValue, bIsSend, bBroadcast);
			if(true == bNeedCalc)
			{
				CalculateInitAbil();
			}
			
		}break;
	}

	if(bIntercept)
	{
		DWORD dwSendFlag = 0;
		if(bIsSend){dwSendFlag |= E_SENDTYPE_SELF;}
		if(bBroadcast){dwSendFlag |= E_SENDTYPE_BROADCAST;}

		if(dwSendFlag & E_SENDTYPE_AREATYPE_BITMASK)
		{
			dwSendFlag |= E_SENDTYPE_MUSTSEND;
			SendAbil((EAbilType)Type, dwSendFlag);
		}

		 OnSetAbil(Type, iValue);
	}
	return true;
}

void PgPlayer::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
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
			rkPacket.Push( GetPos().x );
			rkPacket.Push( GetPos().y );
			rkPacket.Push( GetPos().z );

			if (0 == (WT_OP_No_Contents & kWriteType))
			{
				rkPacket.Push( GetMemberGUID() );
				rkPacket.Push( m_kInfo );
				rkPacket.Push( m_byGMLevel );
				rkPacket.Push( m_bIsPCCafe );
				rkPacket.Push( m_byPCCafeGrade );

				rkPacket.Push(m_kTransTowerSaveRct);
				rkPacket.Push(m_kLevelRank);
			}
			else
			{
				rkPacket.Push( m_kInfo.kCoupleStatus );
			}
			m_kInv.WriteToPacket(rkPacket, kWriteType);// Inventory는 Contents에서 관리하자나..			

			rkPacket.Push( PartyGuid() );
			rkPacket.Push( m_kBasic );//SPlayerBasicInfo

			rkPacket.Push( m_kGmAbil);
			rkPacket.Push( (BYTE)GetAbil(AT_TEAM) );
			rkPacket.Push( GetAbil(AT_MONSTER_KILL_COUNT) );		//40

			rkPacket.Push( GetAbil(AT_MAX_COMBO));
			rkPacket.Push( GetAbil(AT_MISSION_THROWUP_PENALTY) );
			rkPacket.Push( GetAbil(AT_MISSION_EVENT) );			
			rkPacket.Push( GetAbil(AT_MISSION_NO) );
			rkPacket.Push( GetAbil(AT_MISSION_COUNT) );
			rkPacket.Push( GetAbil(AT_AWAKE_STATE) );
			rkPacket.Push( GetAbil(AT_AWAKE_VALUE) );			
			rkPacket.Push( GetAbil(AT_GMCMD_LIMIT_HP) );
			rkPacket.Push( GetAbil(AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT) );
			rkPacket.Push( GetAbil(AT_ACHIEVEMENT_DUNGEON_TIMEATTACK) );
			rkPacket.Push( GetAbil(AT_INDUN_PARTICIPATION_RATE) );
			
			m_kQInv.WriteToPacket(rkPacket);
			m_Skill.WriteToPacket(rkPacket, kWriteType);
			m_kPremium.WriteToPacket(rkPacket);
			m_kDailyReward.WriteToPacket(rkPacket);

			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				m_kAchievements.WriteToPacket(rkPacket);//45

				rkPacket.Push(m_kContAchievementAbil.size());
				for(CONT_ACHIEVEMENT_ABIL::const_iterator iter = m_kContAchievementAbil.begin();iter != m_kContAchievementAbil.end(); ++iter)
				{
					rkPacket.Push((*iter));
					rkPacket.Push(GetAbil((*iter)));
				}

				// Contents Server 가 관리하는 Data
				// Map->Contents로 Player정보 왔는때, Contents에서 관리하는 Data는 Overwrite 되지 않도록 하기 위한 옵션
				rkPacket.Push( m_kGroundKey );
				m_kQuest.WriteToPacket(rkPacket);
				m_kWorldMapFlag.WriteToPacket( rkPacket );
				rkPacket.Push( GetAbil64(AT_FRAN) );

				PU::TWriteTable_AM(rkPacket, m_kContPortal);
				rkPacket.Push( m_kIsCreateCard);
				m_kCardAbilObj.WriteToPacket(rkPacket);

				m_kHiddenOpen.WriteToPacket(rkPacket);
			}

			m_kMySkill.WriteToPacket(kWriteType, rkPacket);
			rkPacket.Push( ConnArea() );
			rkPacket.Push( m_kMemberID );
			SPvPReport::WriteToPacket(rkPacket);
			rkPacket.Push( m_bySyncType );
			rkPacket.Push( GetAbil(AT_CLIENT_OPTION_SAVE) );
			rkPacket.Push( GetAbil(AT_SKILLSET_USE_COUNT) );
			rkPacket.Push( GetAbil(AT_STRATEGYSKILL_OPEN) );
			rkPacket.Push( GetAbil(AT_STRATEGYSKILL_TABNO) );

			rkPacket.Push( m_kMacroWaitAns);			// 메크로 응답 대기 여부
			rkPacket.Push( m_kMacroMode);			// 메크로 모드 false : 정상 true : 메크로 사용
			rkPacket.Push( m_kMacroRecvExp);			// 메크로 경험치 획득 여부
			rkPacket.Push( m_kMacroModeStartTime);	// 메크로 모드 타임 아웃
			rkPacket.Push( m_kMacroLastRecvExpTime);	// 메크로 마지막 경험치 획득 시간
			rkPacket.Push( m_kMacroKeyCount);				// 메크로 키 개수
			rkPacket.Push( m_kMacroInputFailCount);	// 메크로 응답 실패 카운트
			rkPacket.Push( m_kMacroCheckCount);		// 메크로 사용 누적 카운트
			rkPacket.Push( m_kMacroPopupCount);		// 메크로 팝업 카운트

			m_kEmoticon.WriteToPacket(rkPacket);
			m_kEmotion.WriteToPacket(rkPacket);
			m_kBalloon.WriteToPacket(rkPacket);

			m_kaddrRemote.WriteToPacket(rkPacket);

			rkPacket.Push(m_kExtendCharacterNum);	// 캐릭터 슬롯 확장 개수
			m_kAlramMission.WriteToPacket(rkPacket);

			rkPacket.Push( m_kBasic.iMaxHP );//SPlayerBasicInfo

			rkPacket.Push(OpenVendor());
			rkPacket.Push(VendorTitle());
			rkPacket.Push(VendorGuid());

			m_kJobSkillExpertness.WriteToPacket(rkPacket);	// 직업스킬 숙련도
			rkPacket.Push(m_kMoveDir);
			rkPacket.Push( ExpeditionGuid() );
			rkPacket.Push( ExpeditionPartyGuid() );

			rkPacket.Push(static_cast<short>(GetAbil(AT_STRATEGY_FATIGABILITY)));
			rkPacket.Push(m_iAccruedDamage);
			rkPacket.Push(m_iAccruedHeal);

			rkPacket.Push( static_cast<BYTE>(GetAbil(AT_DUEL)) );
			rkPacket.Push( static_cast<WORD>(GetAbil(AT_DUEL_WINS)) );
			rkPacket.Push( static_cast<WORD>(GetAbil(AT_PARTY_MASTER_PROGRESS_POS)) );

			m_kBattlePassInfo.WriteToPacket(rkPacket);

			m_RecentNormalMap.WriteToPacket(rkPacket);
			PU::TWriteTable_AA(rkPacket, m_ChatBlockList);

			PU::TWriteTable_AA(rkPacket, m_GuardianDiscount);

			if ( !(WT_OP_NOHPMP & kWriteType) )
			{
				rkPacket.Push( GetAbil(AT_HP) );
				rkPacket.Push( GetAbil(AT_MP) );
			}

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
			rkPacket.Push( m_kInfo.kCoupleStatus );
			rkPacket.Push( Name() );
			rkPacket.Push( m_kInfo.byGender );
			rkPacket.Push( m_kInfo.iRace );
			rkPacket.Push( m_kInfo.iClass );//10
			rkPacket.Push( m_kInfo.wLv );
			rkPacket.Push( m_kInfo.sTacticsLevel );
			rkPacket.Push( GetPos() );
			rkPacket.Push( m_kInfo.iHairColor );
			rkPacket.Push( m_kInfo.iHairStyle );//15
			rkPacket.Push( m_kInfo.iFace );
			rkPacket.Push( m_kInfo.iJacket );
			rkPacket.Push( m_kInfo.iPants );
			rkPacket.Push( m_kInfo.iShoes );
			rkPacket.Push( m_kInfo.iGloves );//20
			rkPacket.Push( m_kInfo.byFiveElementBody );
			rkPacket.Push( m_kInfo.kBirthDate );
			rkPacket.Push( GetAbil(AT_RECENT) );
			rkPacket.Push( m_byGMLevel );
			rkPacket.Push( m_bIsPCCafe );// 25
			rkPacket.Push( m_byPCCafeGrade );
			rkPacket.Push( m_kIsCreateCard );
			rkPacket.Push( GetAbil(AT_HP) );
			
			//SIMPLE_EXTRA_USERINFO
			rkPacket.Push( GetActionID() );

			rkPacket.Push( (BYTE)GetAbil(AT_TEAM) );//팀

			m_kInv.WriteToPacket(IT_FIT, rkPacket);
			m_kInv.WriteToPacket(IT_FIT_CASH, rkPacket);
			m_kInv.WriteToPacket(IT_FIT_COSTUME, rkPacket);
			rkPacket.Push( GetAbil(AT_CLIENT_OPTION_SAVE) );
			rkPacket.Push( GetAbil(AT_SKILLSET_USE_COUNT) );
			rkPacket.Push( GetAbil(AT_STRATEGYSKILL_OPEN) );
			rkPacket.Push( GetAbil(AT_STRATEGYSKILL_TABNO) );

			m_kCardAbilObj.WriteToPacket( rkPacket );
			rkPacket.Push(m_kInfo.sAddedStr);
			rkPacket.Push(m_kInfo.sAddedInt);
			rkPacket.Push(m_kInfo.sAddedDex);
			rkPacket.Push(m_kInfo.sAddedCon);
			
			rkPacket.Push(OpenVendor());
			rkPacket.Push(VendorTitle());
			rkPacket.Push(VendorGuid());

			rkPacket.Push(m_kInfo.byCharactorSlot);

			rkPacket.Push(m_kMoveDir);
			rkPacket.Push( ExpeditionGuid() );
			rkPacket.Push( ExpeditionPartyGuid() );

			rkPacket.Push(static_cast<short>(GetAbil(AT_STRATEGY_FATIGABILITY)));
			rkPacket.Push(m_iAccruedDamage);
			rkPacket.Push(m_iAccruedHeal);

			rkPacket.Push( static_cast<BYTE>(GetAbil(AT_DUEL)) );
			rkPacket.Push( static_cast<WORD>(GetAbil(AT_DUEL_WINS)) );
			rkPacket.Push( static_cast<WORD>(GetAbil(AT_PARTY_MASTER_PROGRESS_POS)) );

			m_RecentNormalMap.WriteToPacket(rkPacket);

			PU::TWriteTable_AA(rkPacket, m_GuardianDiscount);	// 김종수. 에러나면 삭제
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
		rkPacket.Push( GetAbil(AT_AWAKE_STATE) );
		rkPacket.Push( GetAbil(AT_AWAKE_VALUE) );
	}

	if( kWriteType & WT_OP_DBPlayerState )
	{
		rkPacket.Push( m_kInfo.byState );
	}

	rkPacket.Push( m_iUID );
	rkPacket.Push( m_kHomeAddr);
	rkPacket.Push( CoupleColorGuid() );
	rkPacket.Push(GetAbil64(AT_REST_EXP_ADD_MAX));
	rkPacket.Push(SubPlayerID());

}

EWRITETYPE PgPlayer::ReadFromPacket(BM::Stream &rkPacket)
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
			POINT3 kPos;
			rkPacket.Pop( kPos.x );
			rkPacket.Pop( kPos.y );
			rkPacket.Pop( kPos.z ); SetPos(kPos);

			BYTE byTeam = 0;
			__int64 iVal64 = 0;	
			int iVal = 0;

			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				rkPacket.Pop( kMemberGuid );
				rkPacket.Pop( m_kInfo );Name(m_kInfo.szName);			//1
				rkPacket.Pop( m_byGMLevel );
				rkPacket.Pop( m_bIsPCCafe );
				rkPacket.Pop( m_byPCCafeGrade );

				rkPacket.Pop(m_kTransTowerSaveRct);
				rkPacket.Pop(m_kLevelRank);
			}
			else
			{
				rkPacket.Pop( m_kInfo.kCoupleStatus );
			}
			m_kInv.ReadFromPacket(rkPacket, kWriteType);
			m_kInv.OwnerGuid(GetID());
			m_kInv.OwnerName(Name());
			m_kInv.MemberGuid(kMemberGuid);

			rkPacket.Pop( m_kPartyGuid );
			
			rkPacket.Pop( m_kBasic );//SPlayerBasicInfo	
			rkPacket.Pop( m_kGmAbil );
			rkPacket.Pop( byTeam ); SetAbil(AT_TEAM, byTeam);
			rkPacket.Pop( iVal );	SetAbil(AT_MONSTER_KILL_COUNT, iVal);	//40

			rkPacket.Pop( iVal );	SetAbil(AT_MAX_COMBO, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_MISSION_THROWUP_PENALTY, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_MISSION_EVENT, iVal);			
			rkPacket.Pop( iVal );	SetAbil(AT_MISSION_NO, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_MISSION_COUNT, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_AWAKE_STATE, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_AWAKE_VALUE, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_GMCMD_LIMIT_HP, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_ACHIEVEMENT_DUNGEON_TIMEATTACK, iVal);
			rkPacket.Pop( iVal );	SetAbil(AT_INDUN_PARTICIPATION_RATE, iVal);
			
			m_kQInv.ReadFromPacket(rkPacket);
			m_Skill.ReadFromPacket(rkPacket, kWriteType);
			m_kPremium.ReadFromPacket(rkPacket);
			m_kDailyReward.ReadFromPacket(rkPacket);

			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				m_kAchievements.ReadFromPacket(rkPacket);//45

				CONT_ACHIEVEMENT_ABIL::size_type kAchievementCount = 0,kCounter = 0;
				CONT_ACHIEVEMENT_ABIL::value_type kAchievementType = 0;

				rkPacket.Pop(kAchievementCount);
				for( kCounter = 0; kCounter < kAchievementCount; ++kCounter)
				{
					iVal = 0;
					rkPacket.Pop(kAchievementType);
					rkPacket.Pop(iVal);
					SetAbil( kAchievementType, iVal );
				}

				// Contents Server 가 관리하는 Data
				// Map->Contents로 Player정보 왔는때, Contents에서 관리하는 Data는 Overwrite 되지 않도록 하기 위한 옵션
				rkPacket.Pop( m_kGroundKey );
				m_kQuest.ReadFromPacket(rkPacket);
				m_kWorldMapFlag.ReadFromPacket( rkPacket );
				rkPacket.Pop( iVal64 );	SetAbil64(AT_FRAN, iVal64);

				InvenRefreshAbil();

				m_kContPortal.clear();
				PU::TLoadTable_AM( rkPacket, m_kContPortal, MAX_USER_PORTAL_NUM );
				rkPacket.Pop( m_kIsCreateCard);
				m_kCardAbilObj.ReadFromPacket( rkPacket );

				m_kHiddenOpen.ReadFromPacket(rkPacket);
			}

			m_kMySkill.ReadFromPacket(rkPacket);
			rkPacket.Pop( m_kConnArea );				
			rkPacket.Pop( m_kMemberID );
			SPvPReport::ReadFromPacket(rkPacket);
			rkPacket.Pop( m_bySyncType );

			iVal = 0;rkPacket.Pop( iVal );SetAbil(AT_CLIENT_OPTION_SAVE, iVal );
			iVal = 0;rkPacket.Pop( iVal );SetAbil(AT_SKILLSET_USE_COUNT, iVal );
			iVal = 0;rkPacket.Pop( iVal );SetAbil(AT_STRATEGYSKILL_OPEN, iVal );
			iVal = 0;rkPacket.Pop( iVal );SetAbil(AT_STRATEGYSKILL_TABNO, iVal );

			rkPacket.Pop( m_kMacroWaitAns);			// 메크로 응답 대기 여부
			rkPacket.Pop( m_kMacroMode);			// 메크로 모드 false : 정상 true : 메크로 사용
			rkPacket.Pop( m_kMacroRecvExp);			// 메크로 경험치 획득 여부
			rkPacket.Pop( m_kMacroModeStartTime);	// 메크로 모드 타임 아웃
			rkPacket.Pop( m_kMacroLastRecvExpTime);	// 메크로 마지막 경험치 획득 시간
			rkPacket.Pop( m_kMacroKeyCount);		// 메크로 키 개수
			rkPacket.Pop( m_kMacroInputFailCount);	// 메크로 응답 실패 카운트
			rkPacket.Pop( m_kMacroCheckCount);		// 메크로 사용 누적 카운트
			rkPacket.Pop( m_kMacroPopupCount);		// 메크로 팝업 카운트

			m_kEmoticon.ReadFromPacket(rkPacket);
			m_kEmotion.ReadFromPacket(rkPacket);
			m_kBalloon.ReadFromPacket(rkPacket);

			m_kaddrRemote.ReadFromPacket(rkPacket);

			rkPacket.Pop(m_kExtendCharacterNum);	// 캐릭터 슬롯 확장 개수
			m_kAlramMission.ReadFromPacket(rkPacket);
			//////////////////
			MacroResetInputTime();					// 매크로 입력 시간은 맵 이동시마다 다시 초기화

			SetAbil(AT_ENABLE_AUTOHEAL, AUTOHEAL_PLAYER);

			CopyAbilFromClassDef();
			CalculateInitAbil();

			int hp = 0;
			rkPacket.Pop( hp );

			SetAbil(AT_MAX_HP, hp);

			rkPacket.Pop(m_kOpenVendor);
			rkPacket.Pop(m_kVendorTitle);
			rkPacket.Pop(m_kVendorGuid);

			m_kJobSkillExpertness.ReadFromPacket(rkPacket);	// 직업스킬 숙련도
			rkPacket.Pop(m_kMoveDir);
			rkPacket.Pop( m_kExpeditionGuid );
			rkPacket.Pop( m_kExpeditionPartyGuid );

			short Fatigability = 0;
			rkPacket.Pop(Fatigability);	SetAbil(AT_STRATEGY_FATIGABILITY, static_cast<int>(Fatigability));
			rkPacket.Pop(m_iAccruedDamage);
			rkPacket.Pop(m_iAccruedHeal);

			BYTE byDuel = 0;
			rkPacket.Pop(byDuel); SetAbil(AT_DUEL, byDuel);
			WORD wWins = 0;
			rkPacket.Pop(wWins); SetAbil(AT_DUEL_WINS, wWins);
			WORD wProgressPos = 0;
			rkPacket.Pop(wProgressPos);
			SetAbil(AT_PARTY_MASTER_PROGRESS_POS, wProgressPos);

			m_kBattlePassInfo.ReadFromPacket(rkPacket);

			m_RecentNormalMap.ReadFromPacket(rkPacket);
			PU::TLoadTable_AA(rkPacket, m_ChatBlockList);

			PU::TLoadTable_AA(rkPacket, m_GuardianDiscount);

			if ( !(WT_OP_NOHPMP & kWriteType) )
			{
				rkPacket.Pop( iVal );	SetAbil(AT_HP, iVal); SetAbil(AT_MEMORIZED_HP, iVal);
				rkPacket.Pop( iVal );	SetAbil(AT_MP, iVal); SetAbil(AT_MEMORIZED_MP, iVal);
			}

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
			rkPacket.Pop( m_kInfo.kCoupleStatus );
			rkPacket.Pop( m_kName );
			rkPacket.Pop( m_kInfo.byGender );
			rkPacket.Pop( m_kInfo.iRace );
			rkPacket.Pop( m_kInfo.iClass );//10
			rkPacket.Pop( m_kInfo.wLv );
			rkPacket.Pop( m_kInfo.sTacticsLevel );

			POINT3 kPos;
			rkPacket.Pop( kPos );
			SetPos( kPos );

			rkPacket.Pop( m_kInfo.iHairColor );
			rkPacket.Pop( m_kInfo.iHairStyle );//15
			rkPacket.Pop( m_kInfo.iFace );
			rkPacket.Pop( m_kInfo.iJacket );
			rkPacket.Pop( m_kInfo.iPants );
			rkPacket.Pop( m_kInfo.iShoes );
			rkPacket.Pop( m_kInfo.iGloves );//20
			rkPacket.Pop( m_kInfo.byFiveElementBody );
			rkPacket.Pop( m_kInfo.kBirthDate );
			rkPacket.Pop( m_kInfo.kRecentInfo.iMapNo );
			rkPacket.Pop( m_byGMLevel );
			rkPacket.Pop( m_bIsPCCafe );
			rkPacket.Pop( m_byPCCafeGrade );
			rkPacket.Pop( m_kIsCreateCard);

			int iVal = 0;
			rkPacket.Pop( iVal );//25
			SetAbil( AT_HP, iVal );

			//SIMPLE_EXTRA_USERINFO
			rkPacket.Pop( iVal );	SetActionID(iVal);

			_tcscpy_s( m_kInfo.szName, MAX_CHARACTERNAME_LEN, Name().c_str() );

			BYTE byVal = 0;
			rkPacket.Pop( byVal ); SetAbil(AT_TEAM, static_cast<int>(byVal) );//팀
	
			//INV
			m_kInv.ReadFromPacket(IT_FIT, rkPacket);
			m_kInv.ReadFromPacket(IT_FIT_CASH, rkPacket);
			m_kInv.ReadFromPacket(IT_FIT_COSTUME, rkPacket);
			InvenRefreshAbil();

			int iClientSaveOption = 0;
			rkPacket.Pop( iClientSaveOption );
			SetAbil(AT_CLIENT_OPTION_SAVE, iClientSaveOption);

			int iSkillSetItemCount = 0;
			rkPacket.Pop( iSkillSetItemCount );
			SetAbil(AT_SKILLSET_USE_COUNT, iSkillSetItemCount);

			iVal=0;rkPacket.Pop( iVal );SetAbil(AT_STRATEGYSKILL_OPEN, iVal);
			iVal=0;rkPacket.Pop( iVal );SetAbil(AT_STRATEGYSKILL_TABNO, iVal);

			SPlayerBasicInfo kBasic;
			GET_DEF(PgClassDefMgr, kClassDefMgr);
			kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), kBasic);
			m_kBasic = kBasic;

			m_kCardAbilObj.ReadFromPacket( rkPacket );
			rkPacket.Pop(m_kInfo.sAddedStr);
			rkPacket.Pop(m_kInfo.sAddedInt);
			rkPacket.Pop(m_kInfo.sAddedDex);
			rkPacket.Pop(m_kInfo.sAddedCon);

			m_kQuest.Clear();
			CopyAbilFromClassDef();
			CalculateInitAbil();
			
			rkPacket.Pop(m_kOpenVendor);
			rkPacket.Pop(m_kVendorTitle);
			rkPacket.Pop(m_kVendorGuid);

			rkPacket.Pop(m_kInfo.byCharactorSlot);

			rkPacket.Pop(m_kMoveDir);
			rkPacket.Pop( m_kExpeditionGuid );
			rkPacket.Pop( m_kExpeditionPartyGuid );

			short Fatigability = 0;
			rkPacket.Pop(Fatigability);	SetAbil(AT_STRATEGY_FATIGABILITY, static_cast<int>(Fatigability));
			rkPacket.Pop(m_iAccruedDamage);
			rkPacket.Pop(m_iAccruedHeal);

			BYTE byDuel = 0;
			rkPacket.Pop(byDuel); SetAbil(AT_DUEL, byDuel);
			WORD wWins = 0;
			rkPacket.Pop(wWins); SetAbil(AT_DUEL_WINS, wWins);
			WORD wProgressPos = 0;
			rkPacket.Pop(wProgressPos);
			SetAbil(AT_PARTY_MASTER_PROGRESS_POS, wProgressPos);

			m_RecentNormalMap.ReadFromPacket(rkPacket);

			PU::TLoadTable_AA(rkPacket, m_GuardianDiscount);
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
		int iVal = 0;
		rkPacket.Pop( iVal );	SetAbil(AT_AWAKE_STATE, iVal);
		rkPacket.Pop( iVal );	SetAbil(AT_AWAKE_VALUE, iVal);
	}

	if( kWriteType & WT_OP_DBPlayerState )
	{
		rkPacket.Pop( m_kInfo.byState );
	}

	rkPacket.Pop( m_iUID );

	rkPacket.Pop(m_kHomeAddr);
	rkPacket.Pop( m_kInfo.kCoupleColorGuid );
	__int64 i64AddedExpLogoff;
	rkPacket.Pop(i64AddedExpLogoff);	SetAbil64(AT_REST_EXP_ADD_MAX, i64AddedExpLogoff);
	BM::GUID kSubPlayerGUID;
	rkPacket.Pop(kSubPlayerGUID);	SubPlayerID(kSubPlayerGUID);

	Latency(0);
	m_kHackIndex.clear();

	//
	m_kNetModule.Set( kMemberGuid, kSwitchServer );

	return kWriteType;
}

bool PgPlayer::SetState(EUnitState const eState)
{
	EUnitState eCurr = GetState();
	switch(eCurr)
	{
	case US_OPENING:
	case US_ACTION_LOCK:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	case US_DEAD:
		{
//			if ( eState != US_DEADREADY )
//			{
//				return false;
//			}
		}break;
//	case US_DEADREADY:
//		{
//			if ( eState != US_RESURRECTION )
//			{
//				return false;
//			}
//		}break;
// 	case US_PORTAL:
// 		{//포탈 탈때 죽으면 포탈 못탐??
// 			if( IsDead() )
// 			{
// 				return false;
// 			}
// 		}break;
	}
	return CUnit::SetState(eState);
}

bool PgPlayer::IsMapLoading()
{
	switch( GetState() )
	{
	case US_NONE:
	case US_OPENING:
	case US_GHOST:
		{
			return true;
		}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgPlayer::Alive( EALIVETYPE const kAliveType, DWORD const dwFlag, int iNewHP, int iNewMP)
{
	SetAbil( AT_HP, (iNewHP == -1) ? GetAbil(AT_C_MAX_HP) : iNewHP );
	SetAbil( AT_MP, (iNewMP == -1) ? GetAbil(AT_C_MAX_MP) : iNewMP );
	
	RemoveCountAbil( AT_CANNOT_ATTACK, AT_CF_ALL, E_SENDTYPE_NONE );
	RemoveCountAbil( AT_CANNOT_DAMAGE, AT_CF_ALL, E_SENDTYPE_NONE );

	if ( kAliveType == EALIVE_NONE )
	{
		WORD wType[2] = { AT_HP, AT_MP };
		SendAbiles(wType,2,dwFlag);
	}
	else
	{	
		m_Skill.Init();
		ClearAllEffect();

		// 살린다는 패킷을 보내준다.
		BM::Stream kRPacket(PT_M_C_NFY_YOU_ARE_REVIVE,GetID());
		kRPacket.Push(GetAbil(AT_HP));
		kRPacket.Push(GetAbil(AT_MP));
		kRPacket.Push((bool)(kAliveType<=EALIVE_MONEY));//bRevive
		Send( kRPacket, dwFlag );
	}
}

void PgPlayer::SendAllAbil()
{
	BM::Stream kPacket(PT_M_C_NFY_ABIL_SYNC);

	kPacket.Push(m_mapAbils);
	kPacket.Push(m_mapAbils64);
	
	Send(kPacket);
}

void PgPlayer::RecvAllAbil(BM::Stream &rkPacket)
{
	m_mapAbils.clear();
	m_mapAbils64.clear();

	rkPacket.Pop(m_mapAbils);
	rkPacket.Pop(m_mapAbils64);
}

void PgPlayer::Send(BM::Stream const &Packet, DWORD const dwSendFlag)
{	// 아래와 같이 하지 말자.....이것땜시 고생많이했음.
	// if(dwSendFlag & E_SENDTYPE_BROADCAST_GROUND)
	if((dwSendFlag & E_SENDTYPE_BROADCAST_GROUND) == E_SENDTYPE_BROADCAST_GROUND)
	{
		DWORD const dwNewSendFlag = (E_SENDTYPE_OPTION & dwSendFlag);

		BM::Stream kBPacket( PT_U_G_BROADCAST_GROUND, dwNewSendFlag );
		kBPacket.Push(Packet);
		VNotify(&kBPacket);
		return;// Notify에서 자기 자신에게도 패킷을 보낸다.
	}

	if((dwSendFlag & E_SENDTYPE_BROADCAST) == E_SENDTYPE_BROADCAST)
	{
		DWORD const dwNewSendFlag = (E_SENDTYPE_OPTION & dwSendFlag);
		BM::Stream kBPacket( PT_U_G_BROADCAST_AREA, dwNewSendFlag );
		kBPacket.Push(GetID());
		kBPacket.Push(Packet);
		VNotify(&kBPacket);
	}

	if( E_SENDTYPE_SELF & dwSendFlag )
	{
		// ReadyNetwork 상태가 아니면 아무런 패킷도 보내지 않는다.
		// 그러나 E_SENDTYPE_SEND_BYFORCE 일때는 강제로 보낸다.
		if ( true == m_kNetModule.Send( Packet, (E_SENDTYPE_SEND_BYFORCE & dwSendFlag), (E_SENDTYPE_MUSTSEND & dwSendFlag) ) )
		{
			SendObserver( Packet );
		}
	}	
}

void PgPlayer::Invalidate()
{
	ReadyNetwork(false);
	m_kQuest.Clear();
	// PgLocalPartyMgr::Leave()에서도 파티포인터 RefCount를 감소시키지만
	// 파티처리 보다 유저나가기 처리가 빠르면 PgLocalPartyMgr::Leave()에
	// 서 처리를 못할수 있으므로 이곳에서도 처리한다
	//	Party(NULL, NULL, BM::GUID::NullData());

	CUnit::Invalidate();
	return;
}

void PgPlayer::SetActionID(unsigned int const iActionID, BYTE const btDirection)
{
	m_kActionInfo.iActionID = iActionID;
	m_kActionInfo.iMoveSpeed = GetAbil(AT_C_MOVESPEED);

	BYTE btTemp = ( DIR_ALL & btDirection );
	if( DIR_NONE != btTemp )
	{		
		FrontDirection(static_cast<Direction>(btTemp));		
	}

	btTemp = ( DIR_ALL & ( btDirection >> 4 ) );
	if( DIR_NONE != btTemp )
	{
		MoveDir(static_cast<Direction>(btTemp));
	}
}

int PgPlayer::GetActionID()const
{
	return m_kActionInfo.iActionID;
}

void PgPlayer::Init()
{
	PgControlUnit::Init();

	m_kNetModule = PgNetModule<>(false);//ReadyNetwork(false);
	
	m_kInfo.Init();
//	m_dwLastSaveTime = 0;
	m_kQInv.Clear();
	m_kQuest.Clear();
	m_kPartyGuid = m_kPartyGuid.NullData();
	m_kCashShopGuid.Clear();
	//m_iActionID = 0;
	//m_ulJumpingTime = 0;
	//m_iJumpingNum = 0;
	m_kGmAbil = GMABIL_NONE;
	m_byGMLevel = 0;
	m_bIsPCCafe = false;
	m_byPCCafeGrade = 0;

	m_bDoSimulate = true;
	m_kMissionReport.Clear();
	SPvPReport::Clear();
	m_kConnArea.Clear();

	PgPlayerPlayTime::Clear();
	NotifyTime(0);
	m_dwElapsedTimeCheckZone = BM::GetTime32();

	m_kWorldMapFlag.Clear();
	m_kGroundKey.Clear();
	m_kOldActionInfo.Init();
	m_kActionInfo.Init();

	Cash(0);

	m_iUID = 0;
	m_kHyperMoveInfo.Init();

	MacroClear();
	m_kIsCreateCard = false;
}

GMABIL_TYPE PgPlayer::GmAbilAdd( GMABIL_TYPE const kGmAbil, bool const bAdd )
{
	if (bAdd)
	{
		m_kGmAbil |= kGmAbil;
	}
	else
	{
		m_kGmAbil ^= kGmAbil;
	}
	return m_kGmAbil;
}

bool PgPlayer::GmAbilCheck(GMABIL_TYPE const kCheck)const
{
	return ( (m_kGmAbil & kCheck) == kCheck ) ? true : false;
}

void PgPlayer::VOnDie()
{
	// 죽으면, 스킬 CoolTime, 몬스터 킬 카운트 모두 초기화 시킨다.
	ClearAllEffect();

	GetSkill()->ClearCoolTime();
	BM::Stream kCPacket(PT_M_C_NFY_COOLTIME_INIT);
	kCPacket.Push(GetID());
	Send(kCPacket);

	// 초기화 되었다고 패킷
	SetAbil(AT_AWAKE_VALUE, 0, true);

	CUnit::VOnDie();
}

void PgPlayer::SetPos(POINT3 const &ptNew)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] OldPos[%05d,%05d,%05d], CurrPos[%05d,%05d,%05d], NewPos[%05d,%05d,%05d]"), __FUNCTIONW__,
	//	(int)OldPos().x, (int)OldPos().y, (int)OldPos().z, (int)GetPos().x, (int)GetPos().y, (int)GetPos().z, (int)ptNew.x, (int)ptNew.y, (int)ptNew.z);
	m_kActionInfo.SetPos(BM::GetTime32(), ptNew);
	
	OldPos(GetPos());	
	CUnit::SetPos(ptNew);
}

HRESULT PgPlayer::AddTacticsExp(__int64 const iAddVal)
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
		return SetTacticsExp(GetAbil64(AT_TACTICS_EXP) + iCalcValue);
	}
	return S_OK;
}

HRESULT PgPlayer::SetTacticsExp(__int64 const iVal)
{
	CONT_DEF_TACTICS_LEVEL const *pkDefTacticsLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefTacticsLevel);
	
	if( pkDefTacticsLevel )
	{
		CONT_DEF_TACTICS_LEVEL::const_iterator find_iter = pkDefTacticsLevel->find( GetAbil(AT_TACTICS_LEVEL)+1 ); // 다음 레벨
		if( pkDefTacticsLevel->end() != find_iter )
		{
			CONT_DEF_TACTICS_LEVEL::mapped_type const &rkNextTacticsLevel = (*find_iter).second;
			SetAbil64(AT_TACTICS_EXP, iVal);
			SendAbil64(AT_TACTICS_EXP);

			__int64 const iCurExp = GetAbil64(AT_TACTICS_EXP);
			if( rkNextTacticsLevel.iTacticsExp <= iCurExp )
			{
				//Do Levelup
				SetAbil(AT_TACTICS_LEVEL, rkNextTacticsLevel.sTacticsLevel);
				SendAbil(AT_TACTICS_LEVEL);

				VOnChangeTactics();
			}
		}
	}
	
	return S_OK;
}

void PgPlayer::SendWarnMessage( int const iMessageNo, BYTE const byLevel )
{
	BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, iMessageNo );
	kPacket.Push(byLevel);
	Send( kPacket, E_SENDTYPE_SELF );
}

void PgPlayer::SendWarnMessage2( int const iMessageNo, int const iValue, BYTE const byLevel )
{	
	BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE2, iMessageNo );
	kPacket.Push(iValue);
	kPacket.Push(byLevel);
	Send( kPacket, E_SENDTYPE_SELF );
}

void PgPlayer::SendWarnMessage3( int const iMessageNo, BM::Stream const& kValue, BYTE const byLevel )
{
	BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE3, iMessageNo );
	kPacket.Push(kValue.Data());
	kPacket.Push(byLevel);
	Send( kPacket, E_SENDTYPE_SELF );
}

void PgPlayer::SendWarnMessageStr(BM::vstring const& rkStr, BYTE const byLevel, bool bForce)
{
	if( 0 < rkStr.size()
	&&	((g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug) ||	bForce) )
	{
		std::wstring const kStr = (std::wstring const)rkStr;
		BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE_STR );
		kPacket.Push(kStr);
		kPacket.Push(byLevel);
		Send( kPacket, E_SENDTYPE_SELF );
	}
}

void PgPlayer::SendWarnMessageStrDebug(BM::vstring const& rkStr, BYTE const byLevel)
{
	SendWarnMessageStr(rkStr, byLevel, true);
}

void PgPlayer::RecvLatency(DWORD const kLatencyTime, POINT3 const & kPos)
{
	SetPos(kPos);
	Latency(kLatencyTime);
}

/*
void PgPlayer::CalculateBattleAbil()
{
	int const iStr = GetAbil(AT_C_STR);
	int const iDex = GetAbil(AT_C_DEX);
	int const iCon = GetAbil(AT_C_CON);
	int const iInt = GetAbil(AT_C_INT);

	//전투 공식 관련.
	// 물리 : 캐릭터 공격력
	int iValue = iStr + (iStr / 10.0) * (iStr / 10.0);
	SetAbil(AT_PHY_ATTACK_MAX, iValue);
	SetAbil(AT_PHY_ATTACK_MIN, iValue);
	// 물리 : 무기 공격력 : Inven에서 읽어야 한다.
	// 마법 : 캐릭터 공격력
	iValue = iInt + (iInt / 10.0) * (iInt / 10.0);
	SetAbil(AT_MAGIC_ATTACK_MIN, iValue);
	SetAbil(AT_MAGIC_ATTACK_MAX, iValue);
	// 마법 : 무기공격력 : Inven에서 읽어야 한다.
	// 물리 : 캐릭터 방어력 (후방어력)
	SetAbil(AT_PHY_DEFENCE, iDex/2 + iCon/5 + iStr/5);
	// 물리 : Armor 방어력 (전방어력) : Inven에서 직접 읽어 오게 된다.
	// 마법 : 캐릭터 방어력 (후방어력)
	SetAbil(AT_MAGIC_DEFENCE, iDex/5 + iCon/5 + iInt/2);
	// 마법 : Armor 방어력 (전방어력) : Inven에서 직접 읽어 오게 된다.
	// Critical : 이미 Abil에 입력되어 있다.
	// 회피율 : 이미 Abil에 입력되어 있다.
	//WORD wType[6] = {AT_PHY_ATTACK_MAX, AT_PHY_ATTACK_MIN, AT_MAGIC_ATTACK_MIN, AT_MAGIC_ATTACK_MAX, AT_PHY_DEFENCE, AT_MAGIC_DMG_DEC};
	//SendAbiles(wType, 6, E_SENDTYPE_SELF);
}
*/

bool PgPlayer::OnSetAbil(WORD const Type, int const iValue)
{
	//////////////////////////////////////////////
	// 전투공식 입니다.
	//////////////////////////////////////////////
	switch(Type)
	{
	case AT_C_STR:
		{
			if (iValue == 0)
			{
				break;
			}
			int const iCon = GetAbil(AT_C_CON);
			int const iDex = GetAbil(AT_C_DEX);
			int iCal = 0;
			{// 물리 공격력
				int iMin = 0, iMax = 0;
				PgSkillHelpFunc::CalcStatToPhyAttack(iValue, iCon, iMin, iMax);
				SetAbil(AT_PHY_ATTACK_MAX, iMin);		NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE);
				SetAbil(AT_PHY_ATTACK_MIN, iMax);		NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE);
			}
			{// 물리 후 공격력 = (STR*2) + LEVEL * 6
				int const iLevel = GetBattleLevel();
				PgSkillHelpFunc::CalcStatToAddPhyAttack(iLevel, iValue, iCal);
				SetAbil(AT_ABS_ADDED_DMG_PHY, iCal);	NftChangedAbil(AT_ABS_ADDED_DMG_PHY, E_SENDTYPE_NONE);
			}
			{// 물리 방어력
				PgSkillHelpFunc::CalcStatToPhyDefence(iValue, iCon, iDex, iCal);
				SetAbil(AT_PHY_DEFENCE, iCal);			NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE);
			}
		}break;
	case AT_C_INT:
		{
			if (iValue == 0)
			{
				if( 0 > iValue )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" INT cannot be NEGATIVE [") << iValue << _T("]") );
				}
				break;
			}
			{//마법 공격력
				int iMin = 0, iMax = 0;
				PgSkillHelpFunc::CalcStatToMagicAttack(iValue, iMin, iMax);
				SetAbil(AT_MAGIC_ATTACK_MAX, iMax);		NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE);
				SetAbil(AT_MAGIC_ATTACK_MIN, iMin);		NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE);
			}
			int const iLevel = GetBattleLevel();
			int iCal = 0;
			{// 마법방어력
				PgSkillHelpFunc::CalcStatToMagicDefence(iLevel, iValue, iCal);
				SetAbil(AT_MAGIC_DEFENCE, iCal);		NftChangedAbil(AT_MAGIC_DEFENCE, E_SENDTYPE_NONE);
			}
			{// Max MP
				PgSkillHelpFunc::CalcStatToMaxMP(iLevel, iValue, iCal);
				SetAbil( AT_MAX_MP, iCal );				NftChangedAbil( AT_MAX_MP, E_SENDTYPE_NONE );
			}
			// MP 회복량 =  DB에 있는 값 사용
		}break;
	case AT_C_DEX:
		{
			if (iValue == 0)
			{
				break;
			}
			
			int const iStr = GetAbil(AT_C_STR);
			int const iCon = GetAbil(AT_C_CON);

			int iCal = 0;
			{// 물리 방어력 = (STR*0.8)+(CON*0.4)+(DEX*0.2)
				PgSkillHelpFunc::CalcStatToPhyDefence(iStr, iCon, iValue, iCal);
				SetAbil(AT_PHY_DEFENCE, iCal);				NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE);
			}
			{// 크리티컬 성공 수치
				PgSkillHelpFunc::CalcStatToCriticalSuccess(this,iValue, iCal);
				SetAbil(AT_CRITICAL_SUCCESS_VALUE, iCal);	NftChangedAbil(AT_CRITICAL_SUCCESS_VALUE, E_SENDTYPE_NONE);
			}
			{// 명중수치
				PgSkillHelpFunc::CalcStatToHitSuccess(this,iValue,iCal);
				SetAbil(AT_HIT_SUCCESS_VALUE, iCal);		NftChangedAbil(AT_HIT_SUCCESS_VALUE, E_SENDTYPE_NONE);
			}
			{// 회피수치
				PgSkillHelpFunc::CalcStatToDodgeSuccess(this,iValue,iCal);
				SetAbil(AT_DODGE_SUCCESS_VALUE, iCal);		NftChangedAbil(AT_DODGE_SUCCESS_VALUE, E_SENDTYPE_NONE);
			}
		}break;
	case AT_C_CON:
		{
			if (iValue == 0)
			{
				break;
			}
			int const iLevel = GetBattleLevel();
			int const iStr = GetAbil(AT_C_STR);
			int const iDex = GetAbil(AT_C_DEX);
			int iCal = 0;
			{// 물리공격력
				int iMin = 0, iMax = 0;
				PgSkillHelpFunc::CalcStatToPhyAttack(iStr, iValue, iMin, iMax);
				SetAbil(AT_PHY_ATTACK_MAX, iMax);		NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE);
				SetAbil(AT_PHY_ATTACK_MIN, iMin);		NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE);
			}
			{// 물리 방어력
				PgSkillHelpFunc::CalcStatToPhyDefence(iStr, iValue, iDex, iCal);
				SetAbil(AT_PHY_DEFENCE, iCal);			NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE);
			}
			{// Max HP
				PgSkillHelpFunc::CalcStatToMaxHP(iLevel, iValue, iCal);
				SetAbil( AT_MAX_HP, iCal );				NftChangedAbil( AT_MAX_HP, E_SENDTYPE_NONE );
			}
			// HP 회복량 = DB 있는값 사용
		}break;
	}

	// 2007-12-05 이전 전투 공식 //////////////
	/*
	switch(Type)
	{
	case AT_C_STR:
		{
			if (iValue == 0)
			{
				break;
			}
			int const iCal = pow(iValue - GetAbil(AT_LEVEL), 1.8) / 7.0;
			SetAbil(AT_PHY_ATTACK_MAX, iCal);
			NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE);
			SetAbil(AT_PHY_ATTACK_MIN, iCal);
			NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE);
		}break;
	case AT_C_INT:
		{
			if (iValue == 0)
			{
				break;
			}
			int const iLevel = GetAbil(AT_LEVEL);
			int const iCal = pow(iValue - GetAbil(AT_LEVEL), 1.8) / 7.0;
			SetAbil(AT_MAGIC_ATTACK_MAX, iCal);
			NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE);
			SetAbil(AT_MAGIC_ATTACK_MIN, iCal);
			NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE);
			int const iCal2 = pow(GetAbil(AT_C_DEX) + 2 * iValue - iLevel, 1.1) / 10.0;
			SetAbil(AT_MAGIC_DEFENCE, iCal2);
			NftChangedAbil(AT_MAGIC_DEFENCE, E_SENDTYPE_NONE);
			int const iCal3 = (150 + (iLevel + iValue) * 5) * GetAbil(AT_MAX_MP_EXPAND_RATE) / 100;
			SetAbil(AT_MAX_MP, iCal3);
			NftChangedAbil(AT_MAX_MP, E_SENDTYPE_NONE);
			int const iCal4 = 10 + pow(iValue/60.0, 2.2);
			SetAbil(AT_MP_RECOVERY, iCal4);
			NftChangedAbil(AT_MP_RECOVERY, E_SENDTYPE_NONE);
		}break;
	case AT_C_DEX:
		{
			if (iValue == 0)
			{
				break;
			}
			int const iLevel = GetAbil(AT_LEVEL);
			int const iCal = pow(3 * iValue + GetAbil(AT_C_CON) - iLevel, 1.1) / 5.0;
			SetAbil(AT_PHY_DEFENCE, iCal);
			NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE);
			int const iCal2 = pow(iValue + 2 * GetAbil(AT_C_INT) - iLevel, 1.1) / 10.0;
			SetAbil(AT_MAGIC_DEFENCE, iCal2);
			NftChangedAbil(AT_MAGIC_DEFENCE, E_SENDTYPE_NONE);
			int const iCal3 = int(pow(iValue * 1.1, 2));	//만분율값
			SetAbil(AT_CRITICAL_SUCCESS_VALUE, iCal3);
			NftChangedAbil(AT_CRITICAL_SUCCESS_VALUE, E_SENDTYPE_NONE);
		}break;
	case AT_C_CON:
		{
			if (iValue == 0)
			{
				break;
			}
			int const iLevel = GetAbil(AT_LEVEL);
			int const iCal = pow(3 * GetAbil(AT_C_DEX) + iValue - iLevel, 1.1) / 5.0;
			SetAbil(AT_PHY_DEFENCE, iCal);
			NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE);
			int const iCal2 = (100 + (iLevel + iValue) * 5) * GetAbil(AT_MAX_HP_EXPAND_RATE) /100;
			SetAbil(AT_MAX_HP, iCal2);
			NftChangedAbil(AT_MAX_HP, E_SENDTYPE_NONE);
			int const iCal3 = 10 + pow(iValue/60.0, 2.2);
			SetAbil(AT_HP_RECOVERY, iCal3);
			NftChangedAbil(AT_HP_RECOVERY, E_SENDTYPE_NONE);
		}break;
	default:
		{}break;
	}
	*/

	bool const bRet = CUnit::OnSetAbil(Type, iValue);
	if( 0 != iValue
	&&	CEL::ST_MAP == g_kProcessCfg.ServerIdentity().nServerType
	&&	PgQuestInfoUtil::IsNfyQuestDependAbilType(Type) )
	{
		BM::Stream kNfyPacket(PT_U_G_NFY_ABIL_CHANGE, Type);
		VNotify(&kNfyPacket);
	}
	return bRet;
}

bool PgPlayer::OnDamage( int& iPower, int& iHP )
{
	if ( IsMapLoading() || (0 != GetAbil(AT_CANNOT_DAMAGE)) )
	{
		// MapLoading중이면 데미지를 받지 않는다.
		iHP = GetAbil(AT_HP);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return CUnit::OnDamage(iPower,iHP);
}

void PgPlayer::CopyAbilFromClassDef()
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
	//SetAbil(AT_HP_RECOVERY_ADD, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_HP_RECOVERY_ADD));
	//SetAbil(AT_MP_RECOVERY_ADD, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_MP_RECOVERY_ADD));
	SetAbil(AT_CRITICAL_SUCCESS_VALUE, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_CRITICAL_SUCCESS_VALUE));
	SetAbil(AT_CRITICAL_POWER, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_CRITICAL_POWER));
	SetAbil(AT_SUMMONED_MAX_SUPPLY, kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), AT_SUMMONED_MAX_SUPPLY));
	if( 0 >= GetAbil(AT_C_ATTACK_SPEED) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Player AttackSpeed must be more than ZERO Value[") << GetAbil(AT_C_ATTACK_SPEED) << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Player AttackSpeed must be more than ZERO"));
	}
	//CLASSDEF_ABIL_CONT::const_iterator itor = pkDef->kAbil.begin();
	//while (itor != pkDef->kAbil.end())
	//{
	//	SetAbil(itor->first, itor->second);

	//	++itor;
	//}
}

void PgPlayer::GetRecentGround( SGroundKey& rOutkKey, T_GNDATTR const kGndAttr )const
{	
	int iAbilType = AT_RECENT;
	if ( kGndAttr & GATTR_FLAG_VILLAGE )
	{
		iAbilType = AT_LAST_VILLAGE;
	}

	rOutkKey.Set(GetAbil(iAbilType));
}

int	PgPlayer::GetRecentMapNo(T_GNDATTR const kGndAttr)const
{
	int iAbilType = AT_RECENT;
	if ( kGndAttr & GATTR_FLAG_VILLAGE )
	{
		iAbilType = AT_LAST_VILLAGE;
	}

	return GetAbil(iAbilType);
}

POINT3 const &PgPlayer::GetRecentPos(T_GNDATTR const kGndAttr)const
{	
	if ( kGndAttr & GATTR_FLAG_VILLAGE )
	{
		return m_kInfo.kLastVillage.ptPos3;
	}
	return m_kInfo.kRecentInfo.ptPos3;
}

bool PgPlayer::UpdateRecent( SGroundKey const &kGndKey, T_GNDATTR const kGndAttr )
{
	if ( kGndKey.IsEmpty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( (GATTR_FLAG_NOSAVE_RECENT) & kGndAttr )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( GATTR_FLAG_VILLAGE & kGndAttr )
	{
		m_kInfo.kLastVillage.iMapNo = kGndKey.GroundNo();
		m_kInfo.kLastVillage.ptPos3 = GetPos();
		m_kInfo.kLastVillage.ptPos3.z += 3.0f;
	}

	m_kInfo.kRecentInfo.iMapNo = kGndKey.GroundNo();
	m_kInfo.kRecentInfo.ptPos3 = GetPos();
	m_kInfo.kRecentInfo.ptPos3.z += 3.0f;
	return true;
}

void PgPlayer::CopyTo(SContentsUser &rkOut)const
{
	rkOut.kMemGuid = GetMemberGUID();
	rkOut.iUID = UID();
	rkOut.kCharGuid = GetID();
	rkOut.kName = Name();
	rkOut.sLevel = GetAbil(AT_LEVEL);
	rkOut.iClass = GetAbil(AT_CLASS);
	rkOut.cGender = GetAbil(AT_GENDER);
	rkOut.iBaseClass = GetAbil(AT_BASE_CLASS);
	rkOut.kGndKey = m_kGroundKey;
	rkOut.kGuildGuid = GuildGuid();
	rkOut.kCoupleGuid = CoupleGuid();
	rkOut.sChannel = GetChannel();
	rkOut.kAccountID = MemberID();
	rkOut.kGuildLeaveDate = m_kInfo.kDateContents.kGuildLeaveDate;
	rkOut.kHomeAddr = HomeAddr();
}

HRESULT PgPlayer::IsOpenWorldMap( int const iGroundNo )const
{
	if(iGroundNo==9018200){return S_OK;}
	CONT_DEFMAP const *pkDefMap;
	g_kTblDataMgr.GetContDef(pkDefMap);
	if ( pkDefMap )
	{
		CONT_DEFMAP::const_iterator itr = pkDefMap->find(iGroundNo);
		if ( itr != pkDefMap->end())
		{
			if ( m_kWorldMapFlag.IsOn(itr->second.iKey) )
			{
				return S_OK;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
			return E_ACCESSDENIED;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgPlayer::UpdateWorldMap( int const iGroundNo )
{
	CONT_DEFMAP const *pkDefMap;
	g_kTblDataMgr.GetContDef(pkDefMap);
	if ( pkDefMap )
	{
		CONT_DEFMAP::const_iterator itr = pkDefMap->find(iGroundNo);
		if (itr != pkDefMap->end())
		{
			return m_kWorldMapFlag.On( itr->second.iKey );
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgPlayer::CopyTo(PgPlayer& rkPlayer, EWRITETYPE eType) const
{
	BM::Stream kCopyPacket;
	WriteToPacket(kCopyPacket, eType);
	//rkPlayer.Init();
	rkPlayer.ReadFromPacket(kCopyPacket);
}

void PgPlayer::GetPlayerAbil(PLAYER_ABIL &rkOut) const
{
	rkOut.sSP = m_kInfo.sSP;	// Skill Point
	rkOut.sJumpHeight = m_ksJumpHeight;	// AT_JUMP_HEIGHT
	rkOut.sCJumpHeight = m_ksCJumpHeight;	// AT_C_JUMP_HEIGHT
	rkOut.sAddMoneyPer = m_ksAddMoneyPer;	// AT_ADD_MONEY_PER

	rkOut.iHairColor = m_kInfo.iHairColor;
	rkOut.iHairStyle = m_kInfo.iHairStyle;
	rkOut.iFace	= m_kInfo.iFace;
	rkOut.iJacket = m_kInfo.iJacket;
	rkOut.iPants = m_kInfo.iPants;
	rkOut.iShoes = m_kInfo.iShoes;
	rkOut.iGloves = m_kInfo.iGloves;
	rkOut.byFiveElementBody = m_kInfo.byFiveElementBody;
}

bool PgPlayer::SetDefaultItem(int const iEquipPos,int const iItemNo)
{
	switch(iEquipPos)
	{
	case EQUIP_POS_HAIR_COLOR:
		{
			m_kInfo.iHairColor = iItemNo;
		}break;
	case EQUIP_POS_HAIR:
		{
			m_kInfo.iHairStyle = iItemNo;
		}break;
	case EQUIP_POS_FACE:
		{
			m_kInfo.iFace	= iItemNo;
		}break;
	case EQUIP_POS_SHIRTS:
		{
			m_kInfo.iJacket = iItemNo;
		}break;
	case EQUIP_POS_PANTS:
		{
			m_kInfo.iPants = iItemNo;
		}break;
	case EQUIP_POS_BOOTS:
		{
			m_kInfo.iShoes = iItemNo;
		}break;
	case EQUIP_POS_GLOVE:
		{
			m_kInfo.iGloves = iItemNo;
		}break;
	default:
		{
			// 여기 파츠는 기본 아이템이 없다.
			return false;
		}break;
	}
	return true;
}

/*
void PgPlayer::GetInfo(SPlayerDBSimpleData &kSimpleData) const
{
	kSimpleData.guidMember = m_kInfo.guidMember;
	kSimpleData.guidCharacter = GetID();
	kSimpleData.kPartyGuid = m_kPartyGuid;
	kSimpleData.kGuildGuid = m_kInfo.kGuildGuid;
	kSimpleData.kCoupleGuid = m_kInfo.kCoupleGuid;
	_tcscpy_s(kSimpleData.szName, MAX_CHARACTERNAME_LEN, m_kInfo.szName);
	kSimpleData.byGender = m_kInfo.byGender;
	kSimpleData.iRace = m_kInfo.iRace;
	kSimpleData.iClass = m_kInfo.iClass;
	kSimpleData.wLv = m_kInfo.wLv;
//	kSimpleData.iHP = m_kInfo.iHP;
	
	kSimpleData.sTacticsLevel = m_kInfo.sTacticsLevel;

	kSimpleData.pt3RecentPos = GetPos();

	kSimpleData.iHairColor = m_kInfo.iHairColor;
	kSimpleData.iHairStyle = m_kInfo.iHairStyle;
	kSimpleData.iFace = m_kInfo.iFace;
	kSimpleData.iJacket = m_kInfo.iJacket;
	kSimpleData.iPants = m_kInfo.iPants;
	kSimpleData.iShoes = m_kInfo.iShoes;
	kSimpleData.iGloves = m_kInfo.iGloves;

	kSimpleData.byFiveElementBody = m_kInfo.byFiveElementBody;//
	kSimpleData.iRecentMap = m_kInfo.kRecentInfo.iMapNo;
	kSimpleData.kBirthDate = m_kInfo.kBirthDate;
}
*/

__int64 PgPlayer::GetAbil64(WORD const eAT)const
{
	__int64 iValue = 0;
	switch(eAT)
	{
	case AT_FRAN:			{ iValue = m_kInfo.i64Fran; }break;
	case AT_EXPERIENCE:		{ iValue = m_kInfo.i64Exp; }break;
	case AT_MONEY:			{ iValue = m_kInv.Money(); }break;
	case AT_TACTICS_EXP:	{ iValue = m_kInfo.i64TacticsExp; }break;
	default:
		{
			iValue = CUnit::GetAbil64(eAT);
		}break;
	}
	return iValue;
}

bool PgPlayer::SetAbil64(WORD const Type, __int64 const iInValue, bool const bIsSend , bool const bBroadcast)
{	// 이 함수까지 내려와야 CheckSetAbil(..) OnSetAbil(..) 등의 함수가 호출된다.
	// 윗단에서도 호출 될 수 있는 일관성 있는 호출체계가 필요
	__int64 iValue = iInValue;
	if ( !CheckSetAbil(Type, (int&)iValue) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch(Type)
	{
	case AT_FRAN:			{ m_kInfo.i64Fran = iValue; }break;
	case AT_EXPERIENCE:		{ m_kInfo.i64Exp = iValue; }break;
	case AT_MONEY:			{ m_kInv.Money(iValue); }break;
	case AT_TACTICS_EXP:	{ m_kInfo.i64TacticsExp = iValue; }break;
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
	case AT_CASH:
	case AT_BONUS_CASH:
	case AT_MILEAGE_TOTAL_CASH:
	case AT_MILEAGE_100:
	case AT_MILEAGE_1000:
	case AT_TIME:
	case AT_REST_EXP_ADD_MAX:
	case AT_SKILL_LINKAGE_FLAG:
		{
			if( !CUnit::SetAbil64(Type, iValue))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Invalid Abil...AbilType[") << Type << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	DWORD dwSendFlag = 0;
	
	if(bIsSend){dwSendFlag |= E_SENDTYPE_SELF;}
	if(bBroadcast){dwSendFlag |= E_SENDTYPE_BROADCAST;}

	if(dwSendFlag)
	{
		SendAbil64((EAbilType)Type, dwSendFlag);
	}
	return true;
}

bool PgPlayer::DoLevelup( SClassKey const& rkNewLevelKey )
{
	SClassKey const kOldClassKey = GetClassKey();

	if ( kOldClassKey != rkNewLevelKey )
	{
		bool bIsSuccess = false;
		int const iBattleLv = CAbilObject::GetAbil( AT_BATTLE_LEVEL );
		if ( 0 < iBattleLv )
		{
			GET_DEF(PgClassDefMgr, kClassDefMgr);
			if ( kClassDefMgr.GetDef( rkNewLevelKey ) )
			{
				m_kInfo.wLv = rkNewLevelKey.nLv;
				m_kInfo.iClass = rkNewLevelKey.iClass;
				bIsSuccess = true;
			}	
		}
		else
		{
			GET_DEF(PgClassDefMgr, kClassDefMgr);
			if ( S_OK == kClassDefMgr.GetAbil( rkNewLevelKey, m_kBasic ) )
			{
				m_kInfo.wLv = rkNewLevelKey.nLv;
				m_kInfo.iClass = rkNewLevelKey.iClass;

				NftChangedAbil(AT_EVENT_LEVELUP, E_SENDTYPE_NONE);
				CalculateInitAbil();
				SetAbil( AT_HP, GetAbil(AT_C_MAX_HP), false, false );
				SetAbil( AT_MP, GetAbil(AT_C_MAX_MP), false, false );

				VOnRefreshAbil();//캐릭터 어빌을 Refresh 한다.
				bIsSuccess = true;
			}
		}

		if ( true == bIsSuccess )
		{
			if( kOldClassKey.nLv != rkNewLevelKey.nLv )//변하면
			{
				VOnLvUp();
			}

			if( kOldClassKey.iClass != rkNewLevelKey.iClass )//변하면
			{
				VOnChangeClass();
			}
		}
		return bIsSuccess;
	}
	return false;
}

bool PgPlayer::DoBattleLevelUp( short const nBattleLv )
{
	HRESULT hRet = E_FAIL;
	if ( 0 < nBattleLv )
	{
		SClassKey kClassKey = GetClassKey();
		kClassKey.nLv = nBattleLv;

		GET_DEF(PgClassDefMgr, kClassDefMgr);
		hRet = kClassDefMgr.GetAbil( kClassKey, m_kBasic );
	}
	
	bool bIsSuccess = false;
	if( S_OK == hRet )
	{
		CAbilObject::SetAbil( AT_BATTLE_LEVEL, static_cast<int>(nBattleLv) );
		bIsSuccess = true;
	}
	else
	{
		short const nOldBattleLv = GetAbil( AT_BATTLE_LEVEL );
		if ( nOldBattleLv )
		{
			CAbilObject::EraseAbil( AT_BATTLE_LEVEL );

			SClassKey const kOrgClassKey = GetClassKey();

			GET_DEF(PgClassDefMgr, kClassDefMgr);
			bIsSuccess = ( SUCCEEDED(kClassDefMgr.GetAbil( kOrgClassKey, m_kBasic ) ) );
		}
	}

	if ( true == bIsSuccess )
	{
		NftChangedAbil(AT_EVENT_LEVELUP, E_SENDTYPE_NONE);

		CalculateInitAbil();
		SetAbil( AT_HP, GetAbil(AT_C_MAX_HP), false, false );
		SetAbil( AT_MP, GetAbil(AT_C_MAX_MP), false, false );

		VOnRefreshAbil();//캐릭터 어빌을 Refresh 한다.
	}
	return bIsSuccess;
}

void PgPlayer::SetUsedAchievementAbilList(WORD const wAbilType)
{
	if((AT_ACHIEVEMENT_DATA_MIN <= wAbilType) && (wAbilType <= AT_ACHIEVEMENT_DATA_MAX))
	{
		m_kContAchievementAbil.insert(wAbilType);
	}
	else
	{//날쌘돌이 업적용 예외처리
		if(AT_ACHIEVEMENT_MISSION_HIT==wAbilType)
		{
			m_kContAchievementAbil.insert(wAbilType);
		}
	}
}

HRESULT PgPlayer::CheckActionPacketCount(SActionInfo const& rkActionInfo)
{	
	// 이전에 받은 마지막 패킷 도착 시간보다 클라이언트가 보낸 시간이 작을 경우 해킹이다.
	// 지나간 시간은 줄지 않는다.

	if(m_kPacketCounter.m_kLastActionInfo.m_dwActionTime > rkActionInfo.dwTimeStamp)
	{
		return E_ACTION_COUNT_SLOW_PACKET;
	}

	if(0 == m_kPacketCounter.m_dwCheckFirstCleintTime) // 처음 시작
	{		
		m_kPacketCounter.m_dwCheckFirstCleintTime = rkActionInfo.dwTimeStamp;	
		m_kPacketCounter.m_dwCheckFirstServerTime = g_kEventView.GetServerElapsedTime();

		// Server가 패킷을 Client 시간보다 늦게 받을 경우
		if(m_kPacketCounter.m_dwCheckFirstServerTime > m_kPacketCounter.m_dwCheckFirstCleintTime)
		{
			//Client 시간으로 맞추어 준다.
			m_kPacketCounter.m_dwCheckFirstServerTime = m_kPacketCounter.m_dwCheckFirstCleintTime;
		}
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = (CSkillDef*) kSkillDefMgr.GetDef(rkActionInfo.iActionID);

	if(NULL == pkSkillDef)
	{
		m_kPacketCounter.m_kLastActionInfo.Init();
		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[") << __FL__ << _T("] Cannot get SkillDef SkillNo[") << rkActionInfo.iActionID << _T("]"));
		//BM::vstring kLogMsg; kLogMsg << _T("[") << __FL__ << _T("] Cannot get SkillDef SkillNo[") << rkActionInfo.iActionID << _T("]");
		//SendWarnMessageStr(kLogMsg);
		return false;
	}
	
	if(0 != m_kPacketCounter.m_kLastActionInfo.m_dwActionTime)
	{
		// 이전 액션이 캐스팅일 떄
		if(ESS_CASTTIME == m_kPacketCounter.m_kLastActionInfo.m_byActionParam)
		{
			int ElapsedTime = static_cast<int>(m_kPacketCounter.m_kLastActionInfo.m_dwActionTime + m_kPacketCounter.m_kLastActionInfo.m_iAnimationTime - rkActionInfo.dwTimeStamp);
			//다음 액션이 캐스팅 타임이 끝나지 않은채 온 액션이라면
			if(0 < ElapsedTime)
			{
				//다음 액션이 같은 액션의 Fire, Modify, Toggle 라면 해킹
				if( rkActionInfo.iActionID == m_kPacketCounter.m_kLastActionInfo.m_iActionID &&
					(ESS_FIRE == rkActionInfo.byActionParam || 
					 ESS_TARGETLISTMODIFY == rkActionInfo.byActionParam || 
					 ESS_TOGGLE_ON == rkActionInfo.byActionParam || 
					 ESS_TOGGLE_OFF == rkActionInfo.byActionParam) )
				{
					//해킹
					return E_ACTION_COUNT_CASTING_CANCEL;
				}
				//다른 스킬이라면 남은 캐스팅 시간을 깍아 준다.
				else
				{
					m_kPacketCounter.m_iTotalActionAniTime -= ElapsedTime;
				}
			}
		}
		else
		{
			if( ESS_FIRE == rkActionInfo.byActionParam
			 || ESS_TOGGLE_ON == rkActionInfo.byActionParam
			 || ESS_TOGGLE_OFF == rkActionInfo.byActionParam )
			{
				if(rkActionInfo.dwTimeStamp < m_kPacketCounter.m_kLastActionInfo.m_dwActionTime + m_kPacketCounter.m_kLastActionInfo.m_iAnimationTime)
				{
					//현재 액션을 발생한 시간이 마지막 액션을 한시간 + 애니 시간보다 작을 경우 해킹 일 수 있다.
					// 예외 상황
					// 맞았을 경우 데미지 액션(서버로 오지않는 액션) 후에 a_idle 이 온다.
					// 또는 리액션 액션이 오는 경우는 처리되어야한다.
					//
					//return true;
				}
			}
		}		
	}

	m_kPacketCounter.m_kLastActionInfo.m_iAnimationTime = 0;
	m_kPacketCounter.m_kLastActionInfo.m_dwActionTime = rkActionInfo.dwTimeStamp;
	m_kPacketCounter.m_kLastActionInfo.m_byActionParam = rkActionInfo.byActionParam;
	m_kPacketCounter.m_kLastActionInfo.m_iActionID = rkActionInfo.iActionID;
	m_kPacketCounter.m_kLastActionInfo.m_dwServerTime = g_kEventView.GetServerElapsedTime();

	//캐스팅이 시작 되면 캐스팅 타임을 더 해준다.
	if(ESS_CASTTIME == rkActionInfo.byActionParam)
	{
		int iCastTime = pkSkillDef->GetAbil(AT_CAST_TIME);
		if(0 < iCastTime)
		{
			iCastTime += GetSkill()->GetSkillCastingTime();
			iCastTime = __max(0, iCastTime);

			m_kPacketCounter.m_iTotalActionAniTime += iCastTime;

			//마지막 액션의 액션 정보를 기록
			m_kPacketCounter.m_kLastActionInfo.m_iAnimationTime = iCastTime;
		}		
	}
	// 스킬이 Fire 될때 애니가 시작 된다.
	// Modify 일 땐 스킬이 이미 사용 중인 것 이다.
	// Fire / Toggle 일때가 액션을 발동한 시기이므로 연출시간을 더해준다.
	else
	{		
		if(ESS_FIRE == rkActionInfo.byActionParam 
		|| ESS_TOGGLE_ON == rkActionInfo.byActionParam
		|| ESS_TOGGLE_OFF == rkActionInfo.byActionParam)
		{
			int iAnimationTime = pkSkillDef->GetAbil(AT_ANIMATION_TIME);
			// 공격속도에 영향을 주는 스킬이 있을 경우
			if(0 < pkSkillDef->GetAbil(AT_APPLY_ATTACK_SPEED))
			{
				int iRate = ABILITY_RATE_VALUE - GetAbil(AT_C_ATTACK_SPEED);

				// +-가 될 수 있기 때문에 0이 아닐때만 추가로 값을 구한다.
				if(0 != iRate) 
				{
					iAnimationTime += static_cast<int>(static_cast<float>(iAnimationTime) * (static_cast<float>(iRate) / ABILITY_RATE_VALUE_FLOAT));
				}

				iAnimationTime = __max(0, iAnimationTime);
			}
			m_kPacketCounter.m_iTotalActionAniTime += iAnimationTime;

			//마지막 액션의 액션 정보를 기록
			m_kPacketCounter.m_kLastActionInfo.m_iAnimationTime = iAnimationTime;			
		}
	}

	++m_kPacketCounter.m_iRecvPacketCount;
	
	return S_OK;
}

bool PgPlayer::ResultActionPacketCount(bool bForceResult, PLAYER_ACTION_TIME_COUNT_RESULT* pkResult, DWORD  dwInitTime, int iActionCountSum, int iCheckTimeMutiflier)
{
	if(0 == m_kPacketCounter.m_dwCheckFirstCleintTime || 0 == m_kPacketCounter.m_iRecvPacketCount)
	{
		return false;
	}

	bool bViewResult = false;

	// 마지막 액션을 받은 시간을 기준으로 계산
	if((dwInitTime + m_kPacketCounter.m_dwCheckFirstCleintTime) < m_kPacketCounter.m_kLastActionInfo.m_dwActionTime)
	{
		bViewResult = true;
	}

	if(iActionCountSum < m_kPacketCounter.m_iRecvPacketCount)
	{
		bViewResult = true;
	}

	if(bForceResult)
	{
		bViewResult = bForceResult;
	}

	if(bViewResult)
	{
		int const iClientElapsedTime = static_cast<int>(m_kPacketCounter.m_kLastActionInfo.m_dwActionTime - m_kPacketCounter.m_dwCheckFirstCleintTime);
		int const iServerElapsedTime = static_cast<int>(m_kPacketCounter.m_kLastActionInfo.m_dwServerTime - m_kPacketCounter.m_dwCheckFirstServerTime);

		if(pkResult)
		{
			pkResult->m_iClientElapsedTime = iClientElapsedTime;
			pkResult->m_iServerElapsedTime = iServerElapsedTime;
			pkResult->m_iRecvPacketCount = m_kPacketCounter.m_iRecvPacketCount;
			pkResult->m_iTotalActionAniTime = m_kPacketCounter.m_iTotalActionAniTime;

			if(iClientElapsedTime > ((iServerElapsedTime + iServerElapsedTime * 0.1f) + 500)) // 500ms 보정치 +1로 인해서
			{
				pkResult->m_bHackingWarning = true;
			}

			if((iServerElapsedTime + iCheckTimeMutiflier + 3000) < m_kPacketCounter.m_iTotalActionAniTime)
			{
				pkResult->m_bHackingWarning = true;
				//VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << " User[" << Name() << "] GUID[" << GetID() << "]  MemberGUID[" << GetMemberGUID() << _T("] HackingType[FastAction] RecvPacketCount[") << m_kPacketCounter.m_iRecvPacketCount << _T("] TotalAniTime[") << m_kPacketCounter.m_iTotalActionAniTime << _T("] ElapsedTime[") << iClientElapsedTime << _T("]"));
				//HACKING_LOG(BM::LOG_LV0, __FL__ << " User[" << Name() << "] GUID[" << GetID() << "]  MemberGUID[" << GetMemberGUID() << _T("] HackingType[FastAction] RecvPacketCount[") << m_kPacketCounter.m_iRecvPacketCount << _T("] TotalAniTime[") << m_kPacketCounter.m_iTotalActionAniTime << _T("] ElapsedTime[") << iClientElapsedTime << _T("]"));
			}
		}

		m_kPacketCounter.m_dwCheckFirstCleintTime = 0;
		m_kPacketCounter.m_dwCheckFirstServerTime = 0;
		m_kPacketCounter.m_iRecvPacketCount = 0;
		m_kPacketCounter.m_iTotalActionAniTime = 0;

		return true;
	}

	return false;
}

bool PgPlayer::AddAbil(WORD const Type, int const iValue)
{
	SetUsedAchievementAbilList(Type);

	switch(Type)
	{//플레이어는 SetAbil 쓸 것.
	case AT_SP:
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
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Do not use this in AddAbil(use SetAbil)") );
		}break;
	case AT_HP:
		{
			// Max 값을 체크 해야 한다.
			int const iRet = std::min( iValue + GetAbil( AT_HP ), GetAbil(AT_C_MAX_HP) );
			CUnit::SetAbil( AT_HP, iRet );
		}break;
	case AT_MP:
		{
			int const iRet = std::min( iValue + GetAbil( AT_MP ), GetAbil(AT_C_MAX_MP) );
			CUnit::SetAbil( AT_MP, iRet );
		}break;
	default:
		{
			return CUnit::AddAbil(Type, iValue);
		}break;
	}
	return true;
}

bool PgPlayer::AddAbil64(WORD const Type, __int64 const iValue)
{	// 이 함수까지 내려와야 CheckSetAbil(..) OnSetAbil(..) 등의 함수가 호출된다.
	// 윗단에서도 호출 될 수 있는 일관성 있는 호출체계가 필요
	switch(Type)
	{//플레이어는 SetAbil 쓸 것.
	case AT_FRAN:	
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_TACTICS_EXP:	
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Invalid Abil...AbilType[") << Type << _T("]") );
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Invalid Abil...AbilType[") << Type << _T("]") );
		}break;
	}

	return true;
}

void PgPlayer::UpdateQuestResult(DB_ITEM_STATE_CHANGE const &rkResult)
{
	BM::Stream kPacket = rkResult.kAddonData;

	switch( rkResult.State() )
	{
	case DISCT_MODIFY_QUEST_ING:
	case DISCT_MODIFY_QUEST_END:
	case DISCT_MODIFY_QUEST_PARAM:
		{
			int iQuestID = 0,
				iQuestValue = 0;
			//
			kPacket.Pop(iQuestID);
			kPacket.Pop(iQuestValue);

			CONT_DEF_QUEST_REWARD const *pkQuestDefReward = NULL;
			g_kTblDataMgr.GetContDef(pkQuestDefReward);

			CONT_DEF_QUEST_REWARD::const_iterator def_iter = pkQuestDefReward->find( iQuestID );
			if( pkQuestDefReward->end() == def_iter )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
				return;
			}
			CONT_DEF_QUEST_REWARD::mapped_type const &rkQuestRewardDef = (*def_iter).second;

			switch( rkResult.State() )
			{
			case DISCT_MODIFY_QUEST_ING: //Begin /Drop(none) /Complete(finished) /Fail 만
				{
					switch( iQuestValue ) //퀘스트 상태
					{
					case QS_Begin: //시작
						{
							SUserQuestState kNewState(iQuestID, QS_Ing);
							m_kQuest.AddIngQuest(&kNewState, rkQuestRewardDef.iDBQuestType);
						}break;
					case QS_Finished: //완료
					case QS_None: //포기
						{
							m_kQuest.DropQuest(iQuestID);
						}break;
					case QS_Ing:
					case QS_End:
					case QS_Failed:
						{
							SUserQuestState const *pkState = m_kQuest.Get( iQuestID );
							if( !pkState )
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get UserQuestState Failed!"));
								return;
							}

							SUserQuestState kNewState = *pkState;
							kNewState.byQuestState = (BYTE)iQuestValue;

							m_kQuest.UpdateQuest(kNewState);
						}break;
					default:
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't handling Quest State: ") << iQuestValue );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
						}break;
					}
				}break;
			case DISCT_MODIFY_QUEST_END:
				{
					if( 0 != iQuestValue ) //0 (안했다), 1 (했다)
					{
						m_kQuest.AddEndQuest( iQuestID, __FUNCTIONW__ );
					}
					else
					{
						m_kQuest.DeEndQuest( iQuestID, __FUNCTIONW__ );
					}
				}break;
			case DISCT_MODIFY_QUEST_PARAM:
				{
					//rkResult.ValueType(); //퀘스트 번호

					int iParamValue = 0;
					kPacket.Pop(iParamValue);

					SUserQuestState const *pkState = m_kQuest.Get( iQuestID );
					if( !pkState )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get UserQuestState Failed!"));
						return;
					}

					SUserQuestState kNewState = *pkState;
					kNewState.byParam[iQuestValue] = (BYTE)iParamValue;

					m_kQuest.UpdateQuest(kNewState);
				}break;
			default:
				{
				}break;
			}
			//
		}break;
	case DISCT_MODIFY_QUEST_EXT:
		{
			BM::PgPackedTime kDayLoopQuestTime;
			ContQuestID kRandomQuest, kTacticsRandomQuest, kWantedQuest;
			bool bBuildRandomQuest = false, bBuildTacticsRandomQuest = false, bBuildWantedQuest = false;
			int iBuildRandomPlayerLevel = 0, iBuildTacticsRandomPlayerLevel = 0, iBuildWantedPlayerLevel = 0;
			ContQuestClearCount kWantedQuestClearCount;
			bool bExistCanBuildWantedQuest = false;

			kPacket.Pop( kDayLoopQuestTime );
			kPacket.Pop( kRandomQuest );
			kPacket.Pop( bBuildRandomQuest );
			kPacket.Pop( iBuildRandomPlayerLevel );
			kPacket.Pop( kTacticsRandomQuest );
			kPacket.Pop( bBuildTacticsRandomQuest );
			kPacket.Pop( iBuildTacticsRandomPlayerLevel );
			kPacket.Pop( kWantedQuest );
			kPacket.Pop( bBuildWantedQuest );
			kPacket.Pop( iBuildWantedPlayerLevel );
			kPacket.Pop( kWantedQuestClearCount );
			kPacket.Pop( bExistCanBuildWantedQuest );

			m_kQuest.DayLoopQuestTime(kDayLoopQuestTime);
			m_kQuest.ContRandomQuest(kRandomQuest);
			m_kQuest.BuildedRandomQuest(bBuildRandomQuest);
			m_kQuest.BuildedRandomQuestPlayerLevel(iBuildRandomPlayerLevel);
			m_kQuest.ContTacticsQuest(kTacticsRandomQuest);
			m_kQuest.BuildedTacticsQuest(bBuildTacticsRandomQuest);
			m_kQuest.BuildedTacticsQuestPlayerLevel(iBuildTacticsRandomPlayerLevel);
			m_kQuest.ContWantedQuest( kWantedQuest );
			m_kQuest.BuildedWantedQuest( bBuildWantedQuest );
			m_kQuest.BuildedWantedQuestPlayerLevel( iBuildWantedPlayerLevel );
			m_kQuest.ContWantedQuestClearCount( kWantedQuestClearCount );
			m_kQuest.ExistCanBuildWantedQuest( bExistCanBuildWantedQuest );
		}break;
	case DISCT_UPDATE_QUEST_CLEAR_COUNT:
		{
			int iQuestID = 0;
			kPacket.Pop(iQuestID);
			AddWantedQuestClearCount(iQuestID);
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't handling Type: ") << rkResult.State() );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

void PgPlayer::InitWantedQuestClearCount()
{
	m_kQuest.InitWantedQuestClearCount();
}

void PgPlayer::SetWantedQuestClearCount(ContQuestClearCount const &rkContQuestClearCount)
{
	m_kQuest.ContWantedQuestClearCount( rkContQuestClearCount );
}

void PgPlayer::AddWantedQuestClearCount(int const iQuestID)
{
	m_kQuest.AddWantedQuestClearCount(iQuestID);
}

int PgPlayer::GetWantedQuestClearCount(int const iQuestID) const
{
	return m_kQuest.GetWantedQuestClearCount(iQuestID);
}

void PgPlayer::SetExistCanBuildWantedQuest( const bool bCanBuild )
{
	m_kQuest.ExistCanBuildWantedQuest( bCanBuild );
}

void PgPlayer::UpdateQuestResult(SUserQuestState* pkInfo, int iQuestValue, int const iQuestType)
{
	switch( iQuestValue )
	{
	case QS_Begin:
		{
			m_kQuest.AddIngQuest(pkInfo, iQuestType);
		}break;
	default:
		{
		}break;
	}		
}
int PgPlayer::GetRandomQuestCompleteCount() const
{
	return m_kQuest.RandomQuestCompleteCount();
}
int PgPlayer::GetRandomQuestPlayerLevel() const
{
	return m_kQuest.BuildedRandomQuestPlayerLevel();
}
int PgPlayer::GetRandomTacticsQuestCompleteCount() const
{
	return m_kQuest.TacticsQuestCompleteCount();
}
int PgPlayer::GetRandomTacticsQuestPlayerLevel() const
{
	return m_kQuest.BuildedTacticsQuestPlayerLevel();
}
void PgPlayer::EndQuestMigration(bool const bIsNewbiePlayer)
{
	CONT_DEF_QUEST_REWARD const* pkDefQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkDefQuestReward);
	m_kQuest.CheckMigration(pkDefQuestReward, bIsNewbiePlayer);
}

void PgPlayer::ClearInstantAbils(T_GNDATTR const attr)	// 여기 있는 어빌은 맵 이동시 마다 초기화 되어야 함
{
	SetAbil(AT_ACHIEVEMENT_PVP_KILL,0);
	SetAbil(AT_ACHIEVEMENT_PVP_DEAD,0);

	if (0==(attr & GATTR_FLAG_MISSION))	//미션맵끼리 이동할땐 초기화 하면 안됨
	{//미션맵에서 나가거나 일반 냅일때 초기화 된다는 전제가 필요함
		SetAbil(AT_ACHIEVEMENT_MISSION_KILL,0);
		SetAbil(AT_ACHIEVEMENT_MISSION_DEAD,0);
		SetAbil(AT_ACHIEVEMENT_MISSION_USEITEM,0);
		SetAbil(AT_ACHIEVEMENT_MISSION_HIT,0);
		SetAbil(AT_ACHIEVEMENT_MISSION_REVIVE,0);
	}

	//필드 결투 관련 어빌들
	if(GetAbil(AT_DUEL))
	{
		SetAbil(AT_DUEL, 0);
	}
	if(GetAbil(AT_DUEL_WINS))
	{
		SetAbil(AT_DUEL_WINS, 0);
	}
	if(GetAbil(AT_PARTY_MASTER_PROGRESS_POS))
	{
		SetAbil(AT_PARTY_MASTER_PROGRESS_POS, 0);
	}

}

void PgPlayer::ClearIngQuest()
{
	m_kQuest.ClearIngQuest();
}

void PgPlayer::SendHyperMoveInfoToClient()	//클라에게 인증정보를 주자
{
	BM::Stream kPacket(PT_M_C_ANS_START_HYPER_MOVE);

	if(!m_kHyperMoveInfo.IsValidType())	//타입이 안정해졌다면 초기화가 안된것
	{
		m_kHyperMoveInfo.Init();	//초기화
		INFO_LOG(BM::LOG_LV9, "Invalid HyperMove Attempt. PlayerGuid : "<<GetID().str().c_str());
		return;
	}

	kPacket.Push(m_kHyperMoveInfo.kAuthGuid);
	Send(kPacket);

	AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_HYPERMOVE, true, E_SENDTYPE_NONE );// 타겟팅 안되도록 설정
	AddCountAbil( AT_CANNOT_ATTACK, AT_CF_HYPERMOVE, true, E_SENDTYPE_NONE );// 공격못하도록 설정
//	++m_kHyperMoveInfo.iAddAbilCount;
}

bool PgPlayer::SetHyperMoveInfo(HYPERMOVE_TYPE const kHyperType, DWORD const dwCurrentTime, POINT3 const& rkPos)
{
	if(!m_kHyperMoveInfo.kAuthGuid.Generate())	//guid생성실패. 그럴일은 없겠지만
	{
		m_kHyperMoveInfo.Init();	//초기화
		INFO_LOG(BM::LOG_LV6, "HyperMove Guid Generate Fail. PlayerGuid : "<<GetID().str().c_str());
		return false;
	}
	m_kHyperMoveInfo.kType = kHyperType;
	switch( kHyperType )
	{
	case TELE_JUMP:
		{
			m_kHyperMoveInfo.dwArriveTime = dwCurrentTime + 10000;
		}break;
	case TELE_MOVE:
	case TELE_PARTY_MOVE:
	case TELE_PARTY_MOVE_OTHER:
	default:
		{
			m_kHyperMoveInfo.dwArriveTime = dwCurrentTime + 3000;
		}break;
	}
	m_kHyperMoveInfo.kArrivePos = rkPos;
	
	return true;
}

HRESULT PgPlayer::EndHyperMove(POINT3 const& rkEndPos, BM::GUID const& rkGuidFromClient, bool const bRealyJump, float& fErrorDist)	//실패했을 때만 거리를 돌려주자
{
	HRESULT hReturn = S_OK;
	//if(pkPacket)
	{
		/*POINT3 kEndPos;
		pkPacket->Pop(kEndPos);
		BM::GUID kGuidFromClient;
		pkPacket->Pop(kGuidFromClient);
		bool bRealyJump = false;
		pkPacket->Pop(bRealyJump);*/

		bool bRealyJumpOnServer = false;	//서버가 한번 더 검사한 결과
		if(true==bRealyJump)	//클라가 제대로 뛰었다고 해도 그래도 검사 해 봐야지
		{
			if(true==m_kHyperMoveInfo.IsValidType())
			{
				if( m_kHyperMoveInfo.kAuthGuid == rkGuidFromClient)
				{
					bRealyJumpOnServer = m_kHyperMoveInfo.IsInPos(rkEndPos);
				}
				else//발급한 GUID가 맞지 않는 경우
				{
					//SuspectHackingIndex(SuspectHackingIndex()+MAX_SUSPECT_HACKING_INDEX*0.3f);//전체의 1/3에 해당하는 양
					INFO_LOG(BM::LOG_LV5, __FUNCTION__<<" AuthGuid Is Not Same. PlayerGuid : "<<GetID());
					hReturn = E_ANTIHACK_WRONG_GUID;
				}
			}
		}

		if(true==bRealyJumpOnServer)
		{
			SetPos(rkEndPos);
		}
		else if(true!=bRealyJump)	//아예 클라에서 잘못뛰었다고 왔을 경우
		{
		}
		else
		{
			float const fDist = POINT3::Distance(m_kHyperMoveInfo.kArrivePos, rkEndPos);
			INFO_LOG(BM::LOG_LV5, __FUNCTION__<<" IsInPos Fail. PlayerGuid : "<<GetID().str().c_str()<<" fDist : "<<fDist);
			hReturn = E_ANTIHACK_DISTANCE;
			fErrorDist = fDist;
		}

		RemoveCountAbil( AT_CANNOT_ATTACK, AT_CF_HYPERMOVE, E_SENDTYPE_NONE );
		RemoveCountAbil( AT_CANNOT_DAMAGE, AT_CF_HYPERMOVE, E_SENDTYPE_NONE );

		m_kHyperMoveInfo.Init();

		if(S_OK==hReturn)
		{
			m_kOldActionInfo.ptPos = rkEndPos;	//포탈이나 대점프를 정상적으로 잘 탓음
		}
	}
	return hReturn;
}

void PgPlayer::Update(unsigned long const ulElapsedTime)
{//대점프 도착 시간을 초과했는지 검사
	if(0 < m_kHyperMoveInfo.dwArriveTime && ulElapsedTime > m_kHyperMoveInfo.dwArriveTime)
	{
		CAUTION_LOG( BM::LOG_LV6, _T("HyperJump Limit Time Over! PlayerGuid : ") << GetID() << _T(" AddAbilCount : ") << GetCountAbil( AT_CANNOT_DAMAGE, AT_CF_HYPERMOVE) );
		m_kHyperMoveInfo.Init();
		RemoveCountAbil( AT_CANNOT_ATTACK, AT_CF_HYPERMOVE, E_SENDTYPE_NONE );
		RemoveCountAbil( AT_CANNOT_DAMAGE, AT_CF_HYPERMOVE, E_SENDTYPE_NONE );
// 		if( 0 < m_kHyperMoveInfo.iAddAbilCount )
// 		{
// 			AddAbil(AT_CANNOT_DAMAGE, -m_kHyperMoveInfo.iAddAbilCount);
// 			AddAbil(AT_CANNOT_ATTACK, -m_kHyperMoveInfo.iAddAbilCount);
// 			m_kHyperMoveInfo.iAddAbilCount = 0;
// 		}
	}
}

void PgPlayer::MacroClear()
{
	CGameTime kTime;
	MacroWaitAns(false);
	MacroMode(false);
	MacroRecvExp(false);
	
	MacroModeStartTime(kTime.GetLocalSecTime());
	MacroInputTimeOut(kTime.GetLocalSecTime());
	MacroLastRecvExpTime(kTime.GetLocalSecTime());

	MacroKeyCount(0);
	MacroInputFailCount(0);
	MacroCheckCount(0);
	MacroPopupCount(0);
}

bool PgPlayer::MacroUseDetected()
{
	if(MacroWaitAns())
	{
		return false;
	}

	SMACRO_CHECK_TABLE kTable = g_kMacroCheckTable.GetMacroCheckTable();

	if(false == kTable.IsEnable())	// 매크로 검사 테이블중에 하나라도 0 값이 있으면 동작 하지 않는다.
	{
		return false;
	}

	CGameTime kTime;
	
	__int64 const i64CurTime = kTime.GetLocalSecTime();

	__int64 const i64CheckedTime = i64CurTime - MacroModeStartTime();

	if(i64CheckedTime < kTable.iMacroCheckTime)
	{
		return false;
	}

	MacroModeStartTime(i64CurTime);

	__int64 const i64CheckRecvExpTime = i64CurTime - MacroLastRecvExpTime();

	if(true == MacroMode())
	{
		if(kTable.iMacroReleaseTime < i64CheckRecvExpTime)	// 경험치 획득 시간에 매크로 릴리즈 시간보다 크면 매크로 비감지 모드로 전환
		{
			MacroMode(false);
			return false;
		}

		MacroCheckCount(MacroCheckCount()+1); // 매크로 감지 횟수를 1 증가

		if(MacroCheckCount() < kTable.iMacroCheckCount)
		{
			return false;
		}

		if(0 == MacroPopupCount())		// 매크로 팝업창 띄울 카운트 수가 아직 지정되지 않았으면 여기서 지정
		{
			MacroPopupCount(BM::Rand_Range(kTable.iMacroPopupMax,kTable.iMacroCheckCount));
		}

		if(MacroPopupCount() > MacroCheckCount())
		{
			return false;
		}
		return true;					// 매크로 검사창 띄워라
	}

	if(i64CheckRecvExpTime < kTable.iMacroCheckTime)
	{
		MacroMode(true);
		return false;
	}

	if(0 >= MacroCheckCount())
	{
		return false;
	}

	MacroCheckCount(MacroCheckCount()-1);

	return false;
}

void PgPlayer::MakeMacroCheckPassword(wchar_t & kCharKey,int & iCount)
{
	int const kRandTable[] = {1,3,5,7};
	int const iRandomMax = sizeof(kRandTable)/sizeof(kRandTable[0]);
	int const iIdx = BM::Rand_Index(iRandomMax);
	iCount = kRandTable[iIdx];

	if(BM::Rand_Index(2))
	{
		kCharKey = static_cast<wchar_t>(BM::Rand_Range(static_cast<wchar_t>('9'),static_cast<wchar_t>('2')));
	}
	else
	{
		kCharKey = static_cast<wchar_t>(BM::Rand_Range(static_cast<wchar_t>('Z'),static_cast<wchar_t>('A')));
	}

	MacroWaitAns(true);
	MacroKeyCount(iCount);
	MacroResetInputTime();
}

void PgPlayer::MacroResetInputTime()
{
	CGameTime kTime;
	__int64 const i64CurTime = kTime.GetLocalSecTime();
	SMACRO_CHECK_TABLE kTable = g_kMacroCheckTable.GetMacroCheckTable();
	MacroInputTimeOut(i64CurTime + kTable.iMacroInputTime);
}

bool PgPlayer::ProcessMacroInputTimeOut()
{
	if(false == MacroWaitAns())
	{
		return false;
	}

	CGameTime kTime;
	if(MacroInputTimeOut() > kTime.GetLocalSecTime())
	{
		return false;
	}

	return true;
}

bool PgPlayer::ProcessMacroCheckPassword(int const iCount)
{
	if(iCount == MacroKeyCount())
	{
		MacroWaitAns(false);
		return true;
	}

	MacroInputFailCount(MacroInputFailCount()+1);

	return false;
}

bool PgPlayer::IsOpenStrategySkill(ESkillTabType const eTabType) const
{
	switch(eTabType)
	{
	case ESTT_BASIC:
		{
			return true;
		}break;
	case ESTT_SECOND:
		{
			ESkillTabOpenType const eType = static_cast<ESkillTabOpenType>(GetAbil(AT_STRATEGYSKILL_OPEN));
			return eType & ESTOT_SECOND;
		}break;
	}
	return false;
}

bool PgPlayer::GetSwapStrategySkillTabNo(ESkillTabType & eTabType)const
{
	ESkillTabType const eTmpType = (ESTT_BASIC==GetAbil(AT_STRATEGYSKILL_TABNO) ? ESTT_SECOND : ESTT_BASIC);

	if( !IsOpenStrategySkill(eTmpType) )
	{
		return false;
	}

	eTabType = eTmpType;
	return true;
}

namespace PgPlayerUtil
{
	inline bool IsNewbiePlayer(int const iRecentMap, int const iRecentVillage)
	{
		return (0 == iRecentMap) && (0 == iRecentVillage);
	}

	bool IsNewbiePlayer(SPlayerDBData const& rkDBData)
	{
		return IsNewbiePlayer(rkDBData.kRecentInfo.iMapNo, rkDBData.kLastVillage.iMapNo);
	}

	bool IsNewbiePlayer(PgPlayer const* pkPlayer)
	{
		if( !pkPlayer )
		{
			return false;
		}

		int const iRecentMap = pkPlayer->GetRecentMapNo(GATTR_DEFAULT);
		int const iRecentVillage = pkPlayer->GetRecentMapNo(GATTR_FLAG_VILLAGE);
		return IsNewbiePlayer(iRecentMap, iRecentVillage);
	}
}

bool PgPlayer::InsertPortal(CONT_USER_PORTAL::key_type const & kKey,CONT_USER_PORTAL::mapped_type const & kPortal)
{
	if( MAX_USER_PORTAL_NUM > m_kContPortal.size() )
	{
		return m_kContPortal.insert(std::make_pair(kKey,kPortal)).second;
	}
	return false;
}

bool PgPlayer::RemovePortal(CONT_USER_PORTAL::key_type const & kKey)
{
	CONT_USER_PORTAL::iterator iter = m_kContPortal.find(kKey);
	if(iter == m_kContPortal.end())
	{
		return false;
	}

	m_kContPortal.erase(iter);
	return true;
}

bool PgPlayer::GetPortal(CONT_USER_PORTAL::key_type const & kKey,CONT_USER_PORTAL::mapped_type & kPortal)
{
	CONT_USER_PORTAL::iterator iter = m_kContPortal.find(kKey);
	if(iter == m_kContPortal.end())
	{
		return false;
	}

	kPortal = (*iter).second;

	return true;
}

bool PgPlayer::SuspectHacking(short const sHackType, short const sAddIndex)
{
	if (0 > sHackType || sHackType >= static_cast<short>(m_sMaxHackIndex.size()))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("HackType is invalid HackType=") << sHackType);
		return false;
	}
	
	auto ret = m_kHackIndex.insert(std::make_pair(sHackType, 0));
	CONT_HACKINDEX::iterator itor_hack = ret.first;

	(*itor_hack).second += sAddIndex;

	bool bReturn = m_sMaxHackIndex[sHackType] < (*itor_hack).second;

	//해킹 체크가 되는 경우 값을 초기화 시켜 준다.
	if(true == bReturn)
	{
		(*itor_hack).second = 0;
	}

	return bReturn;
}

bool PgPlayer::GetContEmotion(BYTE const _type,PgBitArray<MAX_DB_EMOTICON_SIZE> & kCont)
{
	switch(_type)
	{
	case ET_EMOTICON:
		{
			kCont = Emoticon();
			return true;
		}break;
	case ET_EMOTION:
		{
			kCont = Emotion();
			return true;
		}break;
	case ET_BALLOON:
		{
			kCont = Balloon();
			return true;
		}break;
	}
	return false;
}

bool PgPlayer::SetContEmotion(BYTE const _type,PgBitArray<MAX_DB_EMOTICON_SIZE> const & kCont)
{
	switch(_type)
	{
	case ET_EMOTICON:
		{
			Emoticon(kCont);
			return true;
		}break;
	case ET_EMOTION:
		{
			Emotion(kCont);
			return true;
		}break;
	case ET_BALLOON:
		{
			Balloon(kCont);
			return true;
		}break;
	}
	return false;
}

void PgPlayer::InvenRefreshAbil()
{
	CUnit::InvenRefreshAbil();

	CONT_SKILL_OPT kSkillOpt;
	GetInven()->GetOverSkillOption(kSkillOpt);

	GetMySkill()->ClearOverSkillLevel();
	GetMySkill()->SwapOverSkillLevel(PgMySkill::SOLT_ONLY_ONE_SKILL, kSkillOpt);	

	{ // AT_DEFENCE_ENCHANT_LEVEL
		int iValue = m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_SHOULDER));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_CLOAK));
		//iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_GLASS));
		//iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_SHEILD));
		//iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_NECKLACE));
		//iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_EARRING));
		//iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_RING_L));
		//iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_RING_R));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_BELT));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_HELMET));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_SHIRTS));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_PANTS));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_BOOTS));
		iValue += m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_GLOVE));
		int const iEnchantPartsCount = 8;
		iValue = iValue / iEnchantPartsCount;
		SetAbil(AT_DEFENCE_ENCHANT_LEVEL, iValue);
	}
	{ // AT_OFFENCE_ENCHANT_LEVEL
		SetAbil(AT_OFFENCE_ENCHANT_LEVEL, m_kInv.GetItemEnchantLevel(SItemPos(IT_FIT, EQUIP_POS_WEAPON)));
	}
}

void PgPlayer::CoupleGuid(BM::GUID const & kCoupleGuid)
{
	m_kInfo.kCoupleGuid = kCoupleGuid;

	/*if(kCoupleGuid.IsNotNull())
	{//GUID 를  순방향, 역방향을 XOR로 만듬(시퀀셜 GUID 때문에. 순방, 역방을 섞어야함)
		BM::GUID const kOwnerGuid = GetID();
		
		BYTE const *pkOwnerMem = reinterpret_cast<BYTE const*>(&kOwnerGuid);
		BYTE const *pkCoupleMem = reinterpret_cast<BYTE const*>(&kCoupleGuid)+sizeof(BM::GUID)-1;
		BYTE *pkRetMem = reinterpret_cast<BYTE *>(&m_kCoupleColorGuid);

		size_t i = 0;
		while(sizeof(BM::GUID) > i)
		{
			*(pkRetMem+i) = static_cast<float>(*++pkOwnerMem ^ *--pkCoupleMem); 
			++i;
		}
	}*/
}

void PgPlayer::ForceSetCoupleColorGuid(BM::GUID const & kCoupleColorGuid)
{//원래 자동 셋팅인건데. 클라이언트용으로 강제 셋팅
	m_kInfo.kCoupleColorGuid = kCoupleColorGuid;
}

void PgPlayer::SetCoupleStatus(BYTE const & kStatus)
{
	m_kInfo.kCoupleStatus = kStatus;
}
void PgPlayer::SetDateContents(EDateContentsType const eType, BM::DBTIMESTAMP_EX const& rkDateTime)
{
	switch( eType )
	{
	case DCT_GUILD_LEAVE_DATE:	{ m_kInfo.kDateContents.kGuildLeaveDate = rkDateTime; }break;
	default:
		{
		}break;
	}
}
bool PgPlayer::GetDateContents(EDateContentsType const eType, BM::DBTIMESTAMP_EX& rkOutDateTime) const
{
	switch( eType )
	{
	case DCT_GUILD_LEAVE_DATE:	{ rkOutDateTime = m_kInfo.kDateContents.kGuildLeaveDate; }break;
	default:
		{
			return false;
		}break;
	}
	return true;
}

bool PgPlayer::AddJobSkillHistoryItem(DWORD const iSaveIdx)
{
	int const iIdx = m_kContJobSkillHistoryItem.size();
	if(iIdx > 0)
	{
		if(m_kContJobSkillHistoryItem.at(0) == iSaveIdx)
		{
			return false;
		}
	}

	if(iIdx >= MAX_JOBSKILL_HISTORYITEM)
	{
		m_kContJobSkillHistoryItem.pop_back();
	}

	m_kContJobSkillHistoryItem.push_front(iSaveIdx);
	return true;
}

bool PgPlayer::EffectEscapeKeyDown(int const iEffectNo)
{
	int const MIN_KEY_VALUE = 5;

	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkDefEffect = kEffectDefMgr.GetDef(iEffectNo);
	CEffect * pkEffect = GetEffect(iEffectNo);
	
	if(!pkDefEffect || !pkEffect)
	{
		return false;
	}

	int iMin = pkDefEffect->GetAbil(AT_EFFECT_ESCAPE_MIN);
	if(0==iMin)
	{
		iMin = MIN_KEY_VALUE;
	}
	int iMax = pkDefEffect->GetAbil(AT_EFFECT_ESCAPE_MAX);
	if(0==iMax)
	{
		iMax = iMin;
	}

	int iNow = 0;
	pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1, iNow);
	++iNow;
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1, iNow);

	int const iValue = BM::Rand_Range(iMax,iMin);
	if(iNow >= iValue)
	{
		DeleteEffect(iEffectNo);
		return true;
	}

	return false;
}

// 원정대 던전에서 데미지 누적.
void PgPlayer::AccumulateDamage(int iDamage)
{
	m_iAccruedDamage += iDamage;
}

// 원정대 던전에서 힐량 누적.
void PgPlayer::AccumulateHeal(int iHeal)
{
	m_iAccruedHeal += iHeal;
}

// 원정대 던전 누적량 초기화.
void PgPlayer::ClearAccmulateValue()
{
	m_iAccruedDamage = 0;
	m_iAccruedHeal = 0;
}

void PgPlayer::SetRecentNormalMap(SRecentPlayerNormalMapData const & MapData)
{
	m_RecentNormalMap = MapData;
}

void PgPlayer::GetRecentNormalMap(SRecentPlayerNormalMapData & OutMapData)
{
	 OutMapData = m_RecentNormalMap;
}

void PgPlayer::SetRecentMapInfo(int const GroundNo, POINT3 const & Pos)
{
	// 유저가 선택할 수 없는 채널에서 비정상 종료 되었을 경우 위치를 정상적으로 되돌려 주어야 함.
	m_kInfo.kRecentInfo.iMapNo = GroundNo;
	m_kInfo.kRecentInfo.ptPos3 = Pos;
}

void PgPlayer::SetRecentMapInfo(SRecentInfo const& kRecentInfo)
{
	m_kInfo.kRecentInfo = kRecentInfo;
}

SRecentInfo const& PgPlayer::GetRecentMapInfo()const
{
	return m_kInfo.kRecentInfo;
}

int PgPlayer::GetGuardianDiscountRate(int GuardianType)
{
	CONT_GUARDIAN_INSTALL_DISCOUNT::iterator iter = m_GuardianDiscount.find(GuardianType);
	if( iter != m_GuardianDiscount.end() )
	{
		return iter->second;
	}

	return 0;
}

void PgPlayer::SetGuardianDiscountRate(int GuardianType, int RateValue)
{
	auto Ret = m_GuardianDiscount.insert( std::make_pair(GuardianType, RateValue) );
	if( !Ret.second )
	{
		if( RateValue > Ret.first->second )	// 새로운 할인률이 기존보다 클때만 적용.
		{
			Ret.first->second = RateValue;

			BM::Stream Packet(PT_M_C_NFY_CHANGE_GUARDIAN_INSTALL_DICOUNT);
			PU::TWriteTable_AA(Packet, m_GuardianDiscount);
			Send(Packet);
		}
	}
}

void PgPlayer::SetGuardianDicountContainer(CONT_GUARDIAN_INSTALL_DISCOUNT & ContDiscount)
{
	m_GuardianDiscount.swap(ContDiscount);
}

void PgPlayer::GetChatBlockList(CONT_BLOCKCHAT_LIST & ChatBlockList)
{
	ChatBlockList.insert(m_ChatBlockList.begin(), m_ChatBlockList.end());
}

bool PgPlayer::AddChatBlockList(std::wstring & CharName, BYTE BlockType)
{
	auto ret = m_ChatBlockList.insert( std::make_pair(CharName, BlockType) );
	
	return ret.second;
}

void PgPlayer::RemoveChatBlockList(std::wstring & CharName)
{
	m_ChatBlockList.erase(CharName);
}

bool PgPlayer::CheckChatBlockList(std::wstring & CharName)
{	// 차단 리스트에 등록 되어 있는지 확인.
	CONT_BLOCKCHAT_LIST::iterator find_iter = m_ChatBlockList.find( CharName );
	if( find_iter != m_ChatBlockList.end() )
	{	// 찾았으면 true.
		return true;
	}
	// 없으면 false.
	return false;
}

void PgPlayer::ModifyChatBlockCharacter(std::wstring & CharName, BYTE BlockType)
{
	CONT_BLOCKCHAT_LIST::iterator find_iter = m_ChatBlockList.find( CharName );
	if( find_iter != m_ChatBlockList.end() )
	{	// 찾았으면 차단 타입 변경.
		find_iter->second = BlockType;
	}
}