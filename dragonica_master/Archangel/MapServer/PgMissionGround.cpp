#include "stdafx.h"
#include "constant.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/Global.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/PgMission_Result.h"
#include "variant/PgEventview.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "Global.h"
#include "BM/PgTask.h"
#include "PgTask_MapServer.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgMissionGround.h"
#include "PgStoneMgr.h"
#include "Variant/PgMissionInfo.h"
#include "PgMissionMan.h"
#include "PgActionAchievement.h"
#include "Collins/Log.h"
#include "Variant/pgitemrarityupgradeformula.h"
#include "Lohengrin/packetstruct.h"
#include "lohengrin/DBTables.h"
#include "lohengrin/ErrorCode.h"
#include "AI/PgAISkillChooser.h"

int const DEFAULT_NEXT_WAVE_DELAY_TIME = 100;

bool IsDefenceMissionLevelCheck(PgPlayer *pkPlayer, PgGround const *pkGround, int const iMissionNo, int const iMissionLevel)
{
	if( !pkPlayer || !pkGround )
	{
		return false;
	}

	if(GATTR_FLAG_CONSTELLATION & pkGround->GetAttr())
	{//���ڸ� ������ ������ �Һ� ����
		return true;
	}

	if( DEFENCE7_MISSION_LEVEL <= (iMissionLevel+1) )
	{
		int const iMode = ( DEFENCE7_MISSION_LEVEL == (iMissionLevel+1) ) ? MO_DEFENCE7 : MO_DEFENCE8;						
		CONT_MISSION_DEFENCE7_MISSION_BAG const * pkMission;
		g_kTblDataMgr.GetContDef(pkMission);
		if( !pkMission )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_MISSION_BAG is NULL") );
			return false;
		}
		CONT_MISSION_DEFENCE7_MISSION_BAG::key_type const kKey(iMissionNo, iMode);
		CONT_MISSION_DEFENCE7_MISSION_BAG::const_iterator iter = pkMission->find(kKey);
		if( iter == pkMission->end() )
		{
			return false;
		}

		SMISSION_DEFENCE7_MISSION const& kValue = iter->second.kCont.at(0);						
		int const iLevelLimit_Min = kValue.iMin_Level;
		int const iLevelLimit_Max = kValue.iMax_Level;

		if( BM::GUID::IsNotNull(pkPlayer->PartyGuid()) )
		{
			VEC_GUID kPartyList;
			if( pkGround->GetPartyMemberGround( pkPlayer->PartyGuid(), pkGround->GroundKey(), kPartyList ) )
			{
				VEC_GUID::const_iterator guid_itr = kPartyList.begin();
				for( ; guid_itr != kPartyList.end() ; ++guid_itr )
				{
					PgPlayer *pkMemberPlayer = pkGround->GetUser( *guid_itr );
					if( pkMemberPlayer )
					{
						int const iLevel = pkMemberPlayer->GetAbil(AT_LEVEL);
						if ( iLevelLimit_Min > iLevel )
						{
							pkPlayer->SendWarnMessage2( 800, iLevelLimit_Min );
							return false;
						}
						else if( iLevelLimit_Max < iLevel )
						{
							pkPlayer->SendWarnMessage2( 801, iLevelLimit_Max );
							return false;
						}
					}
				}
			}
		}
		else
		{
			int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
			if ( iLevelLimit_Min > iLevel )
			{
				pkPlayer->SendWarnMessage2( 800, iLevelLimit_Min );
				return false;
			}
			else if( iLevelLimit_Max < iLevel )
			{
				pkPlayer->SendWarnMessage2( 801, iLevelLimit_Max );
				return false;
			}
		}
	}
	return true;
}

bool SortyByHP(PgMonster const * rhs, PgMonster const * lhs)
{
	return rhs->GetAbil(AT_HP) > lhs->GetAbil(AT_HP);
}

PgMissionGround::PgMissionGround()
{
}

PgMissionGround::~PgMissionGround()
{
}

EOpeningState PgMissionGround::Init( int const iMonsterControlID, bool const bMonsterGen )
{
	ClearStage();
	m_iNowStage = 0;
	m_iModeType = MO_ITEM;
	m_iEleite = 0;
	m_bPortal = false;
	m_bTimeAttack = false;
	m_dwTotalTime = 0;
	m_dwTotalTimeLimit = 0;
	m_dwStartTimeLimit = 0;
	m_dwTotalItemTime = 0;
	GMState(false);
	GMScore(0);
	m_kContMonsterNum.clear();
	MonsterTotalCount(0);
	MonsterTotalKillCount(0);
	m_dwTimeScoreStart = 0;
	m_iTimeScore = 0;
	iTotalScoreUpdate = 0;
	iTotalScoreChange = 0;
	iTotalOverScore = 0;
	m_kContMonsterNum.clear();
	m_kContObjectNum.clear();
	StageClear(false);
	ClassDemage(false);
	m_iNowWave = 0;
    m_dwCheckReReqWavePacket = 0;
	m_dwNextWave_Delay = 0;
	m_dwNextWave_StartDelay = 0;
	m_dwStage_StartTime = 0;
	m_dwStage_Time = 0;
	//m_dwStage_StartDelay = 0;
	//m_dwStage_Delay = 0;
	eMonsterGen = D_RANDOM;
	m_bDefenceEndStage = false;
	m_iSelect_SuccessCount = 0;
	m_iCountSelectItem = 0;
	m_bPrevStage_UseSelectItem = false;
	m_iPrevStage_CountSelectItem = 0;
	m_kContUserDirection.clear();
	m_bSelectSuccess = false;
	ms_iDefenceChangeTargetRate = 0;
	ms_iDefenceObjectAttackMonCount = 0;
	ms_iDefence7ChangeTargetRate = 0;
	ms_iDefence7ObjectAttackMonCount = 0;
	m_dwStageExp = 0;
	m_dwTowerHpMessageDelay = 0;
	m_dwTowerHpMessageDelay1 = 0;
	m_kObjectTeam.clear();
	m_kObjectHp.clear();	
	m_iStageTimePlus = 0;	
	m_kContStrategicPoint.clear();
	m_kContDefenceItemList.clear();
	m_kContDefenceItem.clear();
	m_kGuardianInstall.clear();
	m_bDefenceStageStart = true;
    m_bGuardianTunningLevelChange = false;
	m_kDefenceMissionSkill.Clear();
	m_kTeamPoint.clear();
	m_iTeamPointCheck = 0;
	m_bDefence_SetWaveMonster = false;
	m_bDefence_NextStage = false;
	m_bFailMission = false;
	m_dwFailMissionWaitTime = 0;
	m_kPointAccumCount = 0;
	m_kAccumPoint = 0;
	m_kPointCopying = false;
	m_kAccumMonsterKillableCount = 0;
	m_bGameOwnerStart = false;
	m_bGameOwnerStartWave = false;

	UpdateScore();

	return PgIndun::Init( iMonsterControlID, bMonsterGen );
}

DWORD PgMissionGround::GetResultWaitTime()
{
	if(GetConstellationKey().PartyGuid.IsNotNull() && false == m_bFailMission)
	{
		return 4000;
	}
	return 15000;
}

void PgMissionGround::UpdateScore()
{
	int iMissionPartyMemberScore[EVar_Mission_Max] = {0,};

	for(int i=1; i<EVar_Mission_Max; ++i)
	{
		if( S_OK != g_kVariableContainer.Get(EVar_Kind_Mission, i, iMissionPartyMemberScore[i]) )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Mission 'PartyMember' Value, Type[") << i << _T("]") );
		}
	}
	// AttackScore	
	int k = 0;
	int count = 0;
	for(k = EVar_Mission_PartyMember1_AttackScore; k <= EVar_Mission_PartyMember4_AttackScore; ++k)
	{
		ms_iAttackScore[count++] = iMissionPartyMemberScore[k];
	}
	// ComboScore
	count = 0;
	for(k = EVar_Mission_PartyMember1_ComboScore; k <= EVar_Mission_PartyMember4_ComboScore; ++k)
	{
		ms_iComboScore[count++] = iMissionPartyMemberScore[k];
	}
	// DemageScore
	count = 0;
	for(k = EVar_Mission_PartyMember1_DemageScore; k <= EVar_Mission_PartyMember4_DemageScore; ++k)
	{
		ms_iDemageScore[count++] = iMissionPartyMemberScore[k];
	}
	// TimeSec
	count = 0;
	for(k = EVar_Mission_PartyMember1_TimeSec; k <= EVar_Mission_PartyMember4_TimeSec; ++k)
	{
		ms_iTimeSec[count++] = iMissionPartyMemberScore[k];
	}
	// TimeScore
	count = 0;
	for(k = EVar_Mission_PartyMember1_TimeScore; k <= EVar_Mission_PartyMember4_TimeScore; ++k)
	{
		ms_iTimeScore[count++] = iMissionPartyMemberScore[k];
	}
	// DieScore
	count = 0;
	for(k = EVar_Mission_PartyMember1_DieScore; k <= EVar_Mission_PartyMember4_DieScore; ++k)
	{
		ms_iDieScore[count++] = iMissionPartyMemberScore[k];
	}
	// OverHitScore
	ms_iOverHitScore = iMissionPartyMemberScore[EVar_Mission_OverHitScore];
	// BackAttack
	ms_iBackAttack = iMissionPartyMemberScore[EVar_Mission_BackAttack];
	// CounterScore
	ms_iCounterScore = iMissionPartyMemberScore[EVar_Mission_Counter];
	// StyleScore
	ms_iStyleScore = iMissionPartyMemberScore[EVar_Mission_Style];

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Mission, EVar_Mission_DefenceObjectAttackMonCount, ms_iDefenceObjectAttackMonCount) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Mission 'DefenceObjectAttackMonCount' Value") );
	}

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Mission, EVar_Mission_DefenceChangeTargetRate, ms_iDefenceChangeTargetRate) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Mission 'DefenceObjectAttackMonCount' Value") );
	}	
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Mission, EVar_Mission_Defence7ObjectAttackMonCount, ms_iDefence7ObjectAttackMonCount) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Mission 'Defence7ObjectAttackMonCount' Value") );
	}

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Mission, EVar_Mission_Defence7ChangeTargetRate, ms_iDefence7ChangeTargetRate) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Mission 'Defence7ObjectAttackMonCount' Value") );
	}	
}

void PgMissionGround::Clear()
{
	PgIndun::Clear();
	PgMission::Clear();

	ClearStage();
	m_iNowStage = 0;
	m_iModeType = MO_ITEM;
	m_iEleite = 0;
	m_dwTotalTime = 0;
	m_dwTotalTimeLimit = 0;
	m_dwStartTimeLimit = 0;
	m_dwTotalItemTime = 0;
	GMState(false);
	GMScore(0);
	m_kContMonsterNum.clear();
	MonsterTotalCount(0);
	MonsterTotalKillCount(0);
	m_dwTimeScoreStart = 0;
	m_iTimeScore = 0;
	iTotalScoreUpdate = 0;
	iTotalScoreChange = 0;
	iTotalOverScore = 0;
	m_iNowWave = 0;
    m_dwCheckReReqWavePacket = 0;
	m_dwNextWave_Delay = 0;
	m_dwNextWave_StartDelay = 0;
	m_dwStage_StartTime = 0;
	m_dwStage_Time = 0;
	//m_dwStage_StartDelay = 0;
	//m_dwStage_Delay = 0;
	eMonsterGen = D_RANDOM;
	m_bDefenceEndStage = false;
	m_iSelect_SuccessCount = 0;
	m_iCountSelectItem = 0;
	m_bPrevStage_UseSelectItem = false;
	m_iPrevStage_CountSelectItem = 0;
	m_kContUserDirection.clear();
	m_bSelectSuccess = false;
	m_dwStageExp = 0;
	m_dwTowerHpMessageDelay = 0;
	m_dwTowerHpMessageDelay1 = 0;
	m_kObjectTeam.clear();
	m_kObjectHp.clear();
	m_iStageTimePlus = 0;
	m_kContStrategicPoint.clear();
	m_kContDefenceItemList.clear();
	m_kContDefenceItem.clear();	
	m_kGuardianInstall.clear();
	m_bDefenceStageStart = true;
	m_kDefenceMissionSkill.Clear();
	m_kTeamPoint.clear();
	m_iTeamPointCheck = 0;
	m_bDefence_SetWaveMonster = false;
	m_bDefence_NextStage = false;
}

bool PgMissionGround::Clone( PgGround* pkGround )
{
	PgMission::Clear();
	return this->Clone( (dynamic_cast<PgMissionGround*>(pkGround)) );
}

bool PgMissionGround::Clone( PgMissionGround* pkIndun )
{
	PgMission *pkMission = dynamic_cast<PgMissionGround*>(pkIndun);
	if ( pkMission )
	{
		PgMission::CloneMission(dynamic_cast<PgMissionGround*>(pkIndun));
	}
	else
	{
		PgMission::Clear();
	}
//	PgMission::Clear();
	return PgIndun::Clone( dynamic_cast<PgIndun*>(pkIndun) );
}

T_GNDATTR PgMissionGround::GetAttr()const
{
	if( m_kGndResounrce.size() <= m_iNowStage )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GATTR_MISSION"));
		return GATTR_MISSION;
	}

	PgGroundResource const *pkGndRsc = m_kGndResounrce.at(m_iNowStage);
	if( !pkGndRsc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GATTR_MISSION"));
		return GATTR_MISSION;
	}
	return pkGndRsc->GetAttr();
}

int PgMissionGround::GetGroundNo() const
{
	if( m_kGndResounrce.size() > m_iNowStage )
	{
		PgGroundResource const *pkGndRsc = m_kGndResounrce.at(m_iNowStage);
		if( pkGndRsc )
		{
			return pkGndRsc->GroundKey().GroundNo();
		}	
	}
	return PgGround::GetGroundNo();
}; //���� �׶��� ��ȣ

void PgMissionGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	DWORD const dwNow = BM::GetTime32();
	DWORD dwElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwElapsed, true);
	switch( m_eState )
	{
	case INDUN_STATE_PLAY:
		{
			if( m_iModeType == MO_SCENARIO && m_bTimeAttack )
			{
				if( m_iEleite == 0 )	// ������ ���׾�����... ���� ó�� ����
				{
					if( m_dwTotalTime <= (GetPlayTime()+DifftimeGetTime(m_dwStartTime,BM::GetTime32())) )
					{
						SetState(INDUN_STATE_RESULT_WAIT);
					}
				}
			}			
			else if( (true == IsLastStage()) && (m_iModeType == MO_ITEM && false == m_bTimeAttack) && (6 > (GetLevel()+1)) )
			{
				// �ó����� Ÿ�Ӿ����� �ƴϸ鼭 5���� ������ ����
				if( m_iEleite == 0 )	// ������ ���׾�����... ���� ó�� ����
				{
					if( m_dwTotalTimeLimit <= (DifftimeGetTime(m_dwStartTimeLimit,BM::GetTime32())) )
					{
						SetState(INDUN_STATE_RESULT_WAIT);
					}
				}
			}
			else if( (false == IsLastStage()) && (m_iModeType == MO_ITEM && false == m_bTimeAttack) && (MISSION_ITEM_TIME > (GetLevel()+1) && IsBonusStage()) )
			{
				// ���ʽ� ��
				if( (0 !=m_dwTotalTimeLimit) && (0 != m_dwStartTimeLimit) )
				{
					if( m_dwTotalTimeLimit <= (DifftimeGetTime(m_dwStartTimeLimit,BM::GetTime32())) )
					{
						m_dwTotalTimeLimit = m_dwStartTimeLimit = 0;

						BM::Stream kCPacket(PT_M_C_NFY_MISSION_BONUS_TIME_CLOSE);
						Broadcast(kCPacket);
					}
				}
			}
			else if( true == IsDefenceMode() )	// BM::TimeCheck(m_kMarryCheckTime, 5000)
			{
				// Defence Mode
				int const iMonsterCount = PgObjectMgr::GetUnitCount(UT_MONSTER);				

				if( (( (0 != m_dwStage_StartTime) && (true == BM::TimeCheck(m_dwStage_StartTime, m_dwStage_Time)) ) && (0 != iMonsterCount)) || (false == m_kContObjectNum.empty()) )
				{
					m_bFailMission = true;
					// �������� ���ѽð� �����粲.
					SetState(INDUN_STATE_RESULT_WAIT);					

					BM::Stream kPacket(PT_M_C_NFY_DEFENCE_FAILSTAGE);
					Broadcast(kPacket);
				}
				/*else if( (0 != m_dwStage_StartDelay) && (true == BM::TimeCheck(m_dwStage_StartDelay, m_dwStage_Delay)) )
				{
					// ������������ ����
					NextStageSend();
				}*/
				else if( 0 != m_dwNextWave_StartDelay && 0 != m_dwNextWave_Delay ) 
				{
					if( (0 != m_iNowWave) && ((0 == iMonsterCount) || ( true == BM::TimeCheck(m_dwNextWave_StartDelay, m_dwNextWave_Delay) )) )
					{
						bool bLastWave = IsDefenceLastWave();
						int const iTotalWaveMonsterCount = GetStageTotalMonsterCount() + iMonsterCount;

						if( (false == bLastWave) || (0 == iMonsterCount) )
						{
							if( (true == bLastWave) && (0 != iTotalWaveMonsterCount) )
							{
								// ������ ���̺��ε� ���ʹ� �� ���� �ʰ� �ð��� �Ǹ� �н�
							}
							else
							{
								// Wave ����� ���ؼ� ó���� ���Ѵ�.
								m_dwNextWave_StartDelay = 0;
								m_dwNextWave_Delay = 0;

								if( D_RANDOM == eMonsterGen )
								{
									EDefenceMonsterGen kMonsterGen = (BM::Rand_Index(2) == 0 ? D_LEFT : D_RIGHT);
									eMonsterGen = kMonsterGen;
								}

								BM::Stream kDefencePacket(PT_M_C_NFY_DEFENCE_WAVE);
								kDefencePacket.Push( bLastWave );
								kDefencePacket.Push( static_cast<BYTE>(eMonsterGen) );
								Broadcast(kDefencePacket);
							}
						}
					}
				}
			}
			else if( true == IsDefenceMode7() )
			{
				int const iMonsterCount = PgObjectMgr::GetUnitCount(UT_MONSTER);

                bool bFailMission = m_bFailMission;
				if( ((0 != m_dwStage_StartTime) /*&& (0 != iMonsterCount)*/) && (false == m_kContObjectNum.empty()) )
				{
					m_kContObjectNum.clear();
					StopAI();
                    m_bFailMission = true;

					BM::Stream kPacket(PT_M_C_NFY_DEFENCE_FAILSTAGE);
					Broadcast(kPacket);
				}

				if(true==m_bFailMission && m_dwFailMissionWaitTime>0)
				{
					if(m_dwFailMissionWaitTime <= dwNow)
					{
						m_bFailMission = false;
						m_dwFailMissionWaitTime = 0;

						GM_DefenceNowStage(std::max<int>(0,m_iNowStage));
						SendMissionState();
					}
				}

				if( !bFailMission && ((0 != m_dwNextWave_StartDelay && 0 != m_dwNextWave_Delay) || (m_dwCheckReReqWavePacket && BM::TimeCheck(m_dwCheckReReqWavePacket, 60000))) ) 
				{
					if( (0 != m_iNowWave) && ((0 == iMonsterCount) || ( true == BM::TimeCheck(m_dwNextWave_StartDelay, m_dwNextWave_Delay) )) )
					{
						bool bLastWave = IsDefenceLastWave();
						int const iTotalWaveMonsterCount = GetStageTotalMonsterCount() + iMonsterCount;

						if( (false == bLastWave) || (0 == iMonsterCount) )
						{
							if( (true == bLastWave) && (0 != iTotalWaveMonsterCount) )
							{
								// ������ ���̺��ε� ���ʹ� �� ���� �ʰ� �ð��� �Ǹ� �н�
							}
							else
							{
								// Wave ����� ���ؼ� ó���� ���Ѵ�.
                                if(m_dwCheckReReqWavePacket)
                                {
                                    VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("DefenceMode7 NextStage: NowStage[") << m_iNowStage << _T("] NowWave[") << m_iNowWave << _T("] MonsterCount[") << iMonsterCount << _T("] TotalWaveMonsterCnt[") << iTotalWaveMonsterCount << _T("] LastWave[") << bLastWave << _T("]"));
                                }
                                m_dwCheckReReqWavePacket = bLastWave ? BM::GetTime32() : 0;
								m_dwNextWave_StartDelay = 0;
								m_dwNextWave_Delay = 0;
								eMonsterGen = D_RIGHT;

								BM::Stream kDefencePacket(PT_M_C_NFY_DEFENCE_WAVE);
								kDefencePacket.Push( bLastWave );
								kDefencePacket.Push( static_cast<BYTE>(eMonsterGen) );
								Broadcast(kDefencePacket);
							}
						}
					}
				}
				CheckMonsterDistanceStrategicPoint();
			}
			else if( true == IsDefenceMode8() )
			{
				int const iMonsterCount = PgObjectMgr::GetUnitCount(UT_MONSTER);

				if( ((0 != m_dwStage_StartTime) /*&& (0 != iMonsterCount)*/) && (false == m_kContObjectNum.empty()) )
				{
					SetState(INDUN_STATE_RESULT_WAIT);
					SendToFailStageCheck();
				}

				if( 0 != m_dwNextWave_StartDelay && 0 != m_dwNextWave_Delay ) 
				{
					if( (0 != m_iNowWave) && ((0 == iMonsterCount) || ( true == BM::TimeCheck(m_dwNextWave_StartDelay, m_dwNextWave_Delay) )) )
					{
						bool bLastWave = IsDefenceLastWave();
						int const iTotalWaveMonsterCount = GetStageTotalMonsterCount() + iMonsterCount;

						if( (false == bLastWave) || (0 == iMonsterCount) )
						{
							if( (true == bLastWave) && (0 != iTotalWaveMonsterCount) )
							{
								// ������ ���̺��ε� ���ʹ� �� ���� �ʰ� �ð��� �Ǹ� �н�
							}
							else
							{
								// Wave ����� ���ؼ� ó���� ���Ѵ�.
								m_dwNextWave_StartDelay = 0;
								m_dwNextWave_Delay = 0;
								
								SendTeamMonsterGen(bLastWave);
							}
						}
					}
				}
				CheckMonsterDistanceStrategicPoint();
			}

			if( IsDefenceMode() || IsDefenceMode7() || IsDefenceMode8() )
			{
				if( 0 != m_dwTowerHpMessageDelay )
				{
					BM::GUID kObjectGuid;
					CONT_OBJECT_TEAM::iterator iter = m_kObjectTeam.find(TEAM_RED);
					if( m_kObjectTeam.end() != iter )
					{
						kObjectGuid = iter->second;
					}

					if( kObjectGuid.IsNotNull() )
					{
						CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
						if( pkUnit )
						{
							CONT_OBJECT_HP::iterator itor = m_kObjectHp.find(TEAM_RED);
							if( m_kObjectHp.end() != itor )
							{
								CONT_OBJECT_HP::mapped_type& kObjectHp = itor->second;

								int const iHp = pkUnit->GetAbil(AT_HP);
								if( kObjectHp > iHp )
								{
									if( BM::TimeCheck(m_dwTowerHpMessageDelay, TOWER_NOTI_DELAY) )
									{
										SendTeamTowerAttackMsg(TEAM_RED);
									}
								}
								kObjectHp = iHp;
							}						

						}
					}
				}

				//
				if( (true == m_bGameOwnerStart) && (true == m_kContEventScriptPlayer.empty()) )
				{// �̺�Ʈ��ũ��Ʈ �������� ������ ������ �������� ����
					m_bDefence_SetWaveMonster = true;
					SetWaveMonster();
					m_bGameOwnerStart = false;
				}

				if( (true == m_bGameOwnerStartWave) && (true == m_kContEventScriptPlayer.empty()) )
				{// �̺�Ʈ��ũ��Ʈ �������� ������ ������ �������� ����
					m_bDefence_NextStage = true;
					DefenceNextStage();
					m_bGameOwnerStartWave = false;
				}
			}
			if( IsDefenceMode() || IsDefenceMode7() )
			{
				if(GetConstellationKey().PartyGuid.IsNotNull() && false == m_bFailMission)
				{
					bool const IsLastStage = (m_iNowStage == GetDefenceStageCount());
					m_kConstellationResult.OnTick1s(this, IsLastStage);
				}
			}
			if( IsDefenceMode8() )
			{
				if( 0 != m_dwTowerHpMessageDelay1 )
				{
					BM::GUID kObjectGuid;
					CONT_OBJECT_TEAM::iterator iter = m_kObjectTeam.find(TEAM_BLUE);
					if( m_kObjectTeam.end() != iter )
					{
						kObjectGuid = iter->second;
					}

					if( kObjectGuid.IsNotNull() )
					{
						CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
						if( pkUnit )
						{
							CONT_OBJECT_HP::iterator itor = m_kObjectHp.find(TEAM_BLUE);
							if( m_kObjectHp.end() != itor )
							{
								CONT_OBJECT_HP::mapped_type& kObjectHp = itor->second;

								int const iHp = pkUnit->GetAbil(AT_HP);
								if( kObjectHp > iHp )
								{
									if( BM::TimeCheck(m_dwTowerHpMessageDelay1, TOWER_NOTI_DELAY) )
									{
										SendTeamTowerAttackMsg(TEAM_BLUE);
									}
								}
								kObjectHp = iHp;
							}						

						}
					}
				}
			}
		}break;
	case INDUN_STATE_RESULT:
		{
			if(GetConstellationKey().PartyGuid.IsNotNull() && false == m_bFailMission)
			{
				bool const IsLastStage = (m_iNowStage == GetDefenceStageCount());
				m_kConstellationResult.OnTick1s(this, IsLastStage);
			}
			else
			{
				SetState(INDUN_STATE_CLOSE);
			}
		}break;
	case INDUN_STATE_CLOSE:
		{// Owner�� ������ ���⼭ �i�Ƴ���(�����ϴϱ� �Ѹ��� �i�Ƴ���)
			if ( PgMission::GetOwner() == BM::GUID::NullData() )
			{
				UNIT_PTR_ARRAY kDeleteUnit;
				PgPlayer* pkPlayer = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
				{
					if ( SUCCEEDED(PgMission::IsAbleOutUser(pkPlayer->GetID()) ) )
					{
						this->RecvRecentMapMove(pkPlayer);
						break;
					}
				}
			}
		}break;
	case INDUN_STATE_FAIL:
		{
			DWORD const dwNow = BM::GetTime32();
			if( dwNow - m_FailTime > 15000)
			{
				SetState(INDUN_STATE_CLOSE);
			}
		}break;
	default:
		{
			PgIndun::OnTick1s();
//			return; �ؾ��ϳ� ���ƾ� �ϳ�
		}break;
	}
	
	float const fAutoHealMultiplier = GetAutoHealMultiplier();
	bool const bSpeedHackCountClear = BM::TimeCheck(m_dwOnTick1s_Check2s, 2000);	// 2s ���� �ѹ���
	bool const bDefendModeTick = BM::TimeCheck( m_kDefendModeTickTime, 30000 );

	// Player AutoHealing.......
	UNIT_PTR_ARRAY kDeleteUnit;
	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	UNIT_PTR_ARRAY kMoveUnitArray;
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		if( MO_ITEM == m_iModeType
		&&	INDUN_STATE_PLAY == m_eState )
		{
			ProcessCheckMacroUse(pkPlayer);
		}
		if( INDUN_STATE_CLOSE == m_eState
		&&	pkPlayer->MacroWaitAns() )
		{
			pkPlayer->MacroClear();
		}

		pkPlayer->AutoHeal(dwElapsed, fAutoHealMultiplier);//HP/MP�� ����Ǹ�
		UpdatePartyUnitAbil(pkPlayer, PMCAT_HP|PMCAT_MP);//1�� ���� ��Ƽ HP/MP ����
		if(bSpeedHackCountClear)
		{
			pkPlayer->SetAbil(AT_SPEED_HACK_COUNT, 0);
		}

		if ( bDefendModeTick ) 
		{
			if( pkPlayer->IsUse() )
			{
				EWallowDefendMode	m_eDefendMode = pkPlayer->GetDefendMode();
				int  const iExp = pkPlayer->GetExpRate();
				int  const iMoney = pkPlayer->GetMoneyRate();
				int  const iDrop = pkPlayer->GetDropRate();
				EPPTCondition const kEtcBoolean = pkPlayer->GetEtcBoolean();
				pkPlayer->Update_PlayTime(dwNow);
				if( (m_eDefendMode != pkPlayer->GetDefendMode())
				|| (iExp != pkPlayer->GetExpRate()) 
				|| (iMoney != pkPlayer->GetMoneyRate())
				|| (iDrop != pkPlayer->GetDropRate())
				|| (kEtcBoolean != pkPlayer->GetEtcBoolean()) )
				{
					BM::Stream kPacket(PT_M_O_REQ_PLAYERTIME_DEFEND);
					kPacket.Push(pkPlayer->GetID());
					pkPlayer->WriteToPacket_PlayTimeSimple(kPacket);
					SendToContents(kPacket);

					BM::Stream kRPacket(PT_M_C_REQ_PLAYERTIME_DEFEND);
					pkPlayer->WriteToPacket_PlayTimeSimple(kRPacket);
					pkPlayer->Send(kRPacket);
				}

				if( pkPlayer->IsTimeOverKick() )
				{
					BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_PlayerPlayTimeOver) );
					kDPacket.Push( pkPlayer->GetMemberGUID() );
					::SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
				}
			}
		}

		ConUser::iterator user_itr = m_kConUser.find(pkPlayer->GetID());
		if(pkPlayer->IsDead())
		{			
			DWORD dwDeathDelayTime = 0;
			if( GATTR_CHAOS_MISSION == this->GetAttr() )
			{
				dwDeathDelayTime = GetChaosDeathDelayTime();
			}
			else
			{
				dwDeathDelayTime = this->GetDeathDelayTime();
			}
			if ( DifftimeGetTime( pkPlayer->DeathTime(), dwNow ) >= dwDeathDelayTime )
			{
				kMoveUnitArray.Add(pkPlayer);
				//BM::Stream kNfyPacket(PT_C_M_NFY_RECENT_MAP_MOVE);
				//pkPlayer->VNotify(&kNfyPacket);
			}
		}

		// �ݺ� Ƚ�� ����
		if( GetMissionNo() != pkPlayer->GetAbil(AT_MISSION_NO) )
		{
			pkPlayer->SetAbil(AT_MISSION_NO, GetMissionNo());
			pkPlayer->SetAbil(AT_MISSION_COUNT, 0);
		}
		m_kEventItemSetMgr.Tick(pkPlayer);
	}
	for (UNIT_PTR_ARRAY::const_iterator itor_move = kMoveUnitArray.begin(); itor_move != kMoveUnitArray.end(); ++itor_move)
	{
		BM::Stream kNfyPacket(PT_C_M_NFY_RECENT_MAP_MOVE);
		(*itor_move).pkUnit->VNotify(&kNfyPacket);
	}


	if( m_iModeType == MO_ITEM )
	{
		if( m_eState == INDUN_STATE_PLAY )
		{
			iTotalScoreUpdate = 0;
			// �׵��� ���� ����(�Ű��� ����)�� ���Ѵ�.		
			Sense_Ability_Sum(iTotalScoreUpdate);
			// �׵��� ����� ������ ����ش�.
			Sense_Ability_Sub(iTotalScoreUpdate);
			// �ð�
			if( (iTotalScoreUpdate - m_iTimeScore) >= 0 )
			{
				iTotalScoreUpdate -= m_iTimeScore;
			}
			// �Ѵ� ������ ��� �׸�ŭ�� �����ش�.
			if( iTotalScoreUpdate > (MISSION_TOTALSCORE_MAX*MISSION_SCORE_CALC) )
			{
				iTotalOverScore += ( iTotalScoreUpdate - (MISSION_TOTALSCORE_MAX*MISSION_SCORE_CALC) );
				iTotalScoreUpdate = (MISSION_TOTALSCORE_MAX*MISSION_SCORE_CALC);
			}

			// ��ȭ�Ǵ� ���� �ݿ��� �ش�.
			CONT_OBJECT_MGR_UNIT::iterator kChangeItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kChangeItor);
			while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kChangeItor))) != NULL)
			{
				ConUser::iterator user_itr = m_kConUser.find(pkPlayer->GetID());
				if( user_itr != m_kConUser.end() )
				{
					PgPlayer* pkUser = GetUser(user_itr->first);
					if( pkUser )
					{
						bool bChangeDemage = false;
						ConUser::mapped_type& rkMissionUser = (*user_itr).second;
						if(pkPlayer->IsDead())
						{			
							if( !rkMissionUser.kDie )
							{
								rkMissionUser.kPoint[MPOINT_DIE]++;
								rkMissionUser.kDie = true;

								int iDie = GetTypeScore(EMission_DIE);
								if( (iTotalScoreUpdate - iDie) >= 0 )
								{
									rkMissionUser.m_kPenalty.kDieScore += iDie;
								}
								else
								{
									iDie = abs( (iTotalScoreUpdate - GetTypeScore(EMission_DIE)) );
									rkMissionUser.m_kPenalty.kDieScore += iDie;
								}
								if( (iTotalScoreUpdate - iDie) >= 0 )
								{
									iTotalScoreUpdate -= iDie;
								}
							}
						}
						else
						{
							if( rkMissionUser.kDie  )
							{
								rkMissionUser.kDie = false;
							}

							bChangeDemage = Sense_ValueUpdate(pkPlayer->GetID());
						}
						if( bChangeDemage )
						{
							int const iDemageScore = GetTypeScore(EMission_DEMAGE);
							if( (iTotalScoreUpdate - iDemageScore) >= 0 )
							{
								rkMissionUser.m_kPenalty.kDemageScore += iDemageScore;
								iTotalScoreUpdate -= iDemageScore;
							}
							else if( 0 != iTotalScoreUpdate )
							{
								rkMissionUser.m_kPenalty.kDemageScore += iTotalScoreUpdate;
								iTotalScoreUpdate = 0;
							}								
						}		
					}
				}
			}

			// Time���� ������� ����
			DWORD const iDiffTime = DifftimeGetTime(m_dwTimeScoreStart,BM::GetTime32());
			if( iDiffTime >= static_cast<DWORD>(GetTypeScore(EMission_TIMESEC)) )
			{
				bool bStartTime = false;
				int iNowTimeScore = GetTypeScore(EMission_TIME);
				if( (iTotalScoreUpdate - iNowTimeScore) >= 0 )
				{
					m_iTimeScore += iNowTimeScore;
					iTotalScoreUpdate -= iNowTimeScore;
					bStartTime = true;
				}
				else if( 0 != iTotalScoreUpdate )
				{
					m_iTimeScore += iTotalScoreUpdate;
					iTotalScoreUpdate = 0;
					bStartTime = true;
				}
				if( bStartTime )
				{
					// Time������ ���� �ð�üũ ����
					m_dwTimeScoreStart = BM::GetTime32();
				}
			}

			// Client ���� ������ ����(Update)
			if( iTotalScoreUpdate <= 0 )
			{
				iTotalScoreUpdate = 0;
			}
			if( iTotalScoreUpdate >= 0 )
			{
				iTotalScoreUpdate = ( iTotalScoreUpdate > (MISSION_TOTALSCORE_MAX*MISSION_SCORE_CALC) ) ? (MISSION_TOTALSCORE_MAX*MISSION_SCORE_CALC) : iTotalScoreUpdate;

				if( iTotalScoreChange != iTotalScoreUpdate )
				{
					DisplayUpdateUI(EMission_Update_Total, iTotalScoreUpdate);

					if( iTotalScoreChange < iTotalScoreUpdate )
					{
						DisplayUpdateUI(EMission_Update_Ani, 0);
					}
				}

				iTotalScoreChange = iTotalScoreUpdate;

				if( iTotalScoreUpdate <= 0 )
				{
					// Time������ ���� �ð�üũ ����
					m_dwTimeScoreStart = BM::GetTime32();
				}
			}
		}
	}
	else if( IsDefenceGameMode() )
	{
	}
	else
	{
		if( (INDUN_STATE_PLAY == m_eState) && (false == StageClear()) )
		{
			if( true == MissionStageClearCheck() )
			{
				StageClear(true);
				Broadcast(BM::Stream(PT_M_C_NFY_STAGECLEAR_GOGO));
			}
		}
	}

	/*PgEntity *pkEntity;
	CONT_OBJECT_MGR_UNIT::iterator kItor_Entity;
	PgObjectMgr::GetFirstUnit(UT_ENTITY, kItor_Entity);
	while ((pkEntity = dynamic_cast<PgEntity*> (PgObjectMgr::GetNextUnit(UT_ENTITY, kItor_Entity))) != NULL)
	{
		pkEntity->AutoHeal( dwElapsed, fAutoHealMultiplier );

		if ( pkEntity->GetState() == US_DEAD )
		{
			kDeleteUnit.Add( pkEntity );
		}
	}*/

	// Pet Tick
	PgPet *pkPet = NULL;
	CONT_OBJECT_MGR_UNIT::iterator itr_pet;
	PgObjectMgr::GetFirstUnit(UT_PET, itr_pet);
	while ((pkPet = dynamic_cast<PgPet*> (PgObjectMgr::GetNextUnit(UT_PET, itr_pet))) != NULL)
	{
		pkPet->AutoHeal( dwElapsed );
	}

	// ItemBox Deleting......
	PgGroundItemBox* pkBox = NULL;
	PgObjectMgr::GetFirstUnit(UT_GROUNDBOX, kItor);
	while( (pkBox = dynamic_cast<PgGroundItemBox*>(PgObjectMgr::GetNextUnit(UT_GROUNDBOX, kItor))) != NULL)
	{
		if( !pkBox->IsInstanceItem() )
		{
			if (dwNow - pkBox->CreateDate() > PgGroundItemBox::ms_GROUNDITEMBOX_DURATION_TIME)
			{
				kDeleteUnit.Add(pkBox);				
			}
		}
	}

	ObjectUnitTick( dwElapsed, kDeleteUnit );

	UNIT_PTR_ARRAY::const_iterator itor = kDeleteUnit.begin();
	while (itor != kDeleteUnit.end())
	{
		ReleaseUnit(itor->pkUnit);

		++itor;
	}
	UpdateAutoNextState(dwNow, dwElapsed);
}

EChapterType PgMissionGround::ChapterCheck(ContMissionOptionChapter const *pkOptionChapter)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	EChapterType eRet = MC_NONE;
	int iResultValue = 0;
	ContMissionOptionChapter::const_iterator iter = pkOptionChapter->begin();
	while( pkOptionChapter->end() != iter )
	{
		ContMissionOptionChapter::value_type const &kElement = (*iter);
		switch( kElement.iType )
		{
		case MC_MON_ALL_KILL:
			{
				if( 0 == (PgObjectMgr::GetUnitCount(UT_MONSTER)) )
				{
					iResultValue++;
				}
				else
				{
					return MC_MON_ALL_KILL;
				}
			}break;
		case MC_MON_TARGET_KILL:
			{
				PgMissionInfo const * pkMissionInfo = NULL;
				bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
				if( bRet )
				{
					VEC_INT	VecKindValue;
					VecKindValue.clear();
					kElement.GetKindNo(VecKindValue);
					if( false == GetMonsterCheck(VecKindValue, kElement.iCount) )
					{
						return MC_MON_TARGET_KILL;
					}
					else
					{
						iResultValue++;
					}
				}
			}break;
		case MC_EFFECT:
			{
				PgMissionInfo const * pkMissionInfo = NULL;
				bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
				if( bRet )
				{
					VEC_INT	VecEffectValue;
					VecEffectValue.clear();
					kElement.GetEffectNo(VecEffectValue);
					if( false == GetEffeckCheck(VecEffectValue) )
					{
						return MC_EFFECT;
					}
					else
					{
						iResultValue++;
					}
				}
			}break;
		case MC_OBJECT_TARGET_KILL:
			{
				PgMissionInfo const * pkMissionInfo = NULL;
				bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
				if( bRet )
				{
					VEC_INT	VecKindValue;
					VecKindValue.clear();
					kElement.GetKindNo(VecKindValue);
					if( false == GetObjectCheck(VecKindValue, kElement.iCount) )
					{
						return MC_OBJECT_TARGET_KILL;
					}
					else
					{
						iResultValue++;
					}
				}
			}break;
		default:
			iResultValue++;
			break;
		}
		++iter;
	}

	if( iResultValue == (int)(pkOptionChapter->size()) )
	{
		eRet = MC_NONE;
	}
	return eRet;
}

bool PgMissionGround::GetEffeckCheck(VEC_INT &rkEffect)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	if( 0 == rkEffect.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("MC_EFFECT Option is Empty! MissionNo[") << GetMissionNo() << _T("] GroundNo[") << GetGroundNo() << _T("]") );
		return false;
	}

	ConUser::iterator user_itr;
	for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
	{
		PgPlayer* pkUser = GetUser(user_itr->first);
		if( pkUser )
		{
			bool bCheck = false;
			VEC_INT::const_iterator iter = rkEffect.begin();
			while( rkEffect.end() != iter )
			{
				CEffect *pkEffect = pkUser->GetEffect((*iter));
				if( pkEffect )
				{
					bCheck = true;
					break;
				}
				
				++iter;
			}

			if( bCheck )
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool PgMissionGround::GetMonsterCheck(VEC_INT &rkValue, int const iTotalCount)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	if( 0 == rkValue.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("MC_MON_TARGET_KILL Option is Empty! MissionNo[") << GetMissionNo() << _T("] GroundNo[") << GetGroundNo() << _T("]") );
		return false;
	}	

	int iCount = 0;

	VEC_INT::const_iterator iter = rkValue.begin();
	while( rkValue.end() != iter )
	{
		CONT_MON_NUM::const_iterator itor = m_kContMonsterNum.find((*iter));
		if( m_kContMonsterNum.end() != itor )
		{
			iCount += ((*itor).second);
		}
		++iter;
	}

	if( iCount >= iTotalCount )
	{
		return true;
	}

	return false;
}

bool PgMissionGround::GetObjectCheck(VEC_INT &rkValue, int const iTotalCount)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	if( 0 == rkValue.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("MC_OBJECT_TARGET_KILL Option is Empty! MissionNo[") << GetMissionNo() << _T("] GroundNo[") << GetGroundNo() << _T("]") );
		return false;
	}	

	int iCount = 0;

	VEC_INT::const_iterator iter = rkValue.begin();
	while( rkValue.end() != iter )
	{
		CONT_OBJECT_NUM::const_iterator itor = m_kContObjectNum.find((*iter));
		if( m_kContObjectNum.end() != itor )
		{
			iCount += ((*itor).second);
		}
		++iter;
	}

	if( iCount >= iTotalCount )
	{
		return true;
	}

	return false;
}

bool PgMissionGround::GetPortalMissionState()
{
	return m_bPortal;
}

EMissionType PgMissionGround::MissionCheck(ContMissionOptionMissionList const *pkOptionMission)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	EMissionType eRet = MM_NONE;
	int iResultValue = 0;

	ContMissionOptionMissionList::const_iterator iter = pkOptionMission->begin();
	while( pkOptionMission->end() != iter )
	{
		ContMissionOptionMissionList::value_type const &kElement = (*iter);
		switch( (*iter).iType )
		{
		case MM_MON_BOSS_KILL:
			{
				if ( (*iter).iCount ==static_cast<int>(m_iEleite) )
				{
					iResultValue++;
				}
				else
				{
					return MM_MON_BOSS_KILL;
				}
			}break;
		case MM_OBJECT_KICK:
			{
				PgMissionInfo const * pkMissionInfo = NULL;
				bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
				if( bRet )
				{
					VEC_INT VecKindValue;
					VecKindValue.clear();
					kElement.GetKindNo(VecKindValue);
					if( false == GetObjectCheck(VecKindValue, kElement.iCount) )
					{
						return MM_OBJECT_KICK;
					}
					else
					{
						iResultValue++;
					}
				}
			}break;
		default:
			iResultValue++;
			break;
		}
		++iter;
	}
	if( iResultValue == static_cast<int>(pkOptionMission->size()) )
	{
		eRet = MM_NONE;
	}
	return eRet;
}

bool PgMissionGround::LogOut(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CUnit* pkUnit = PgObjectMgr::GetUnit(rkCharGuid);//�� �׶��忡 �ִ°�.
	if(pkUnit)
	{
		if(m_eState == INDUN_STATE_PLAY)
		{
			PgThrowUpPenalty kAction(GroundKey(), false);
			kAction.DoAction(pkUnit,-5000);
		}
	}
	
	return PgGround::LogOut(rkCharGuid);
}

bool  PgMissionGround::ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall, bool const bSendArea )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	bool bSendMonsterNum = false;
	switch( pkUnit->UnitType() )
	{
	case UT_PLAYER:
		{
			PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkUnit);
			if ( !pkUser )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("PgPlayer is NULL") );
				break;
			}

			pkUser->DefenceMssionExpBonusRate(0.0f); // Ȥ�ø𸣴� �ʱ�ȭ.

			bool bChangeOwner = false;
			bool bLog = false;
			switch( m_eState )
			{
			case INDUN_STATE_WAIT:
				{
					ReleaseWaitUser( pkUser );
					bLog = true;
				}break;
			case INDUN_STATE_READY:
			case INDUN_STATE_PLAY:
				{
                    int const iTeam = GetTeam(pkUnit->GetID());
					
					bLog = true;
// 					if ( pkUnit->GetID() == PgMission::GetOwner() )
// 					{// ���ʸ� ��ü�ؾ� �Ѵ�.
// 						PgMission::ChangeOwner();
// 
// 						BM::Stream kNPacket(PT_C_N_REQ_PARTY_CHANGE_MASTER,pkUnit->GetID());
// 						kNPacket.Push(PgMission::GetOwner());
// 						SendToContents(kNPacket);
// 					}
					
					if ( IsDefenceMode7() )
					{
						bChangeOwner = PgMission::ChangeOwner(pkUnit->GetID());
					}
					// ����� Caller ��ü
					GuardianOwnerChange(pkUnit, iTeam);
                    m_bGuardianTunningLevelChange = true;
				}break;
			case INDUN_STATE_RESULT_WAIT:
				{
					bLog = true;
				}//break ������� ����
			case INDUN_STATE_RESULT:
				{
					BM::CAutoMutex kLock(m_kMissionMutex);
					if ( pkUnit->GetID() == PgMission::GetOwner() )
					{
						PgMission::SetOwner(BM::GUID::NullData());
					}
				}break;
			}

			{
				BM::CAutoMutex kLock(m_kMissionMutex);
				PgMission::m_kConUser.erase(pkUnit->GetID());
				PgMission::m_kReqRestartUser.erase(pkUnit->GetID());
				PgMission::m_kReqDefenceNextStageUser.erase(pkUnit->GetID());
				PgMission::m_kReqGadaCoinUse.erase(pkUnit->GetID());
			}

			if( IsDefenceGameMode() )
			{
				pkUser->SetAbil(AT_IGNORE_PVP_MODE_ABIL, 0, true);
			}

			PgGround *pkGround = dynamic_cast<PgGround*>(this);
			if( IsDefenceMode8() && pkGround )
			{				
				BM::GUID const kPartyGuid( pkUser->PartyGuid() );
				BM::GUID const kCharGuid( pkUser->GetID() );
				if( BM::GUID::IsNotNull(kPartyGuid) )
				{
					size_t const iMinSize = 1;
					VEC_GUID kContGuid;

					m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kContGuid, kCharGuid);
					if( m_kLocalPartyMgr.IsMaster(kPartyGuid, kCharGuid)
					&&	iMinSize <= kContGuid.size() )
					{
						// ���� ��Ƽ���� ���� ������ �������.
					}
					else
					{
						/*if( static_cast<int>(PI_NONE) != pkGround->DefenceIsJoinParty(kPartyGuid) )
						{
							if( pkGround->DefenceDelWaitParty(kPartyGuid) )
							{
								// ��� ��Ƽ ����
							}
						}*/

						if( IsDefenceMode8() )
						{
							// ��������� Owner�� ��ü
							TeamChangeOwner(pkUnit);
						}

						SGroundKey kGndKey;
						pkUser->GetRecentGround( kGndKey, GATTR_DEFAULT );

						BM::Stream kNPacket(PT_M_T_NFY_DEFENCE_DELETE_PARTYINFO);
						kNPacket.Push(kGndKey);
						kNPacket.Push( kPartyGuid );
						SendToMissionMgr(kNPacket);
					}
				}
			}

			if(IsDefenceGameMode() && bChangeOwner)
			{
				SetWaveMonster();
				DefenceNextStage();
			}

			if ( true == bLog )
			{
				// Log
				ConUser::iterator user_itr = PgMission::m_kConUser.find(pkUnit->GetID());
				if ( user_itr != m_kConUser.end() )
				{
					ConUser::mapped_type& rkMissionUser = (*user_itr).second;

					PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_End);				
					kLogCont.MemberKey( pkUser->GetMemberGUID() );
					kLogCont.CharacterKey( pkUser->GetID() );
					kLogCont.ID( pkUser->MemberID() );
					kLogCont.UID( pkUser->UID() );
					kLogCont.Name( pkUser->Name() );
					kLogCont.ChannelNo( pkUser->GetChannel() );
					kLogCont.Class( static_cast<short>(pkUser->GetAbil(AT_CLASS)) );
					kLogCont.Level( static_cast<short>(pkUser->GetAbil(AT_LEVEL)) );
					kLogCont.GroundNo( GetGroundNo() );

					PgLog kLog(ELOrderMain_Mission, ELOrderSub_End);
					kLog.Set( 0, static_cast<int>(2) );// ������ ����
					kLog.Set( 1, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(pkUser->PartyGuid())) );
					kLog.Set( 2, static_cast<int>(rkMissionUser.kPoint[MPOINT_BONUSEXP]) );
					kLog.Set( 3, static_cast<int>(GetLevel()+1) );
					kLog.Set( 0, pkUser->GetAbil64(AT_EXPERIENCE) );// i64Value 1
					kLog.Set( 2, pkUser->PartyGuid().str() );	//guidValue 1
					kLog.Set( 3, this->GroundKey().Guid().str() );// guidValue 2
					kLog.Set( 0, PgMission_Base::GetMissionRankName(rkMissionUser.kRank) );
					kLogCont.Add( kLog );
					kLogCont.Commit();
				}
				// Log End
			}
		}break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		{
			bSendMonsterNum = true;

			PgMonster* pkMonster;
			pkMonster = dynamic_cast<PgMonster*>(pkUnit);
			if( pkMonster )
			{
				int const iMonNo = pkMonster->GetAbil(AT_CLASS);
				auto bRet = m_kContMonsterNum.insert(std::make_pair(iMonNo, 1));
				if( !bRet.second )
				{
					++(*bRet.first).second;
				}

				if( IsStrategicMonsterDie(pkUnit) )
				{
					// ���� ����Ʈ
					int const iTeam = std::max((pkMonster->GetAbil(AT_TEAM) - 2), 1);					
					int iStrategicPoint = std::max(pkMonster->GetAbil(AT_STRATEGIC_POINT), 0);
					int iStrategicMul = pkMonster->GetAbil(AT_STRATEGIC_MUL_POINT);
					iStrategicPoint = ( 0 < iStrategicMul ) ? iStrategicPoint * iStrategicMul : iStrategicPoint;

					AddTeamStrategicPoint(iTeam, iStrategicPoint);
					int OriValue = std::max(pkMonster->GetAbil(AT_STRATEGIC_POINT), 0);
					int MulValue = std::max(iStrategicPoint - OriValue, 0);
					SendStrategicPoint(iTeam, OriValue, MulValue);
				}
			}
			m_kMonsterTotalKillCount++;
		}break;
	case UT_OBJECT:
		{
			if(IsDefenceMode7())
			{
				int const iTeam = pkUnit->GetAbil(AT_TEAM);
				CONT_OBJECT_TEAM::iterator iter_team = m_kObjectTeam.find(iTeam);
				if( m_kObjectTeam.end() != iter_team )
				{
					if(pkUnit->GetID()==(*iter_team).second)
					{
						m_kObjectTeam.erase(iter_team);

						CONT_OBJECT_HP::iterator itor_hp = m_kObjectHp.find(iTeam);
						if( m_kObjectHp.end() != itor_hp )
						{
							m_kObjectHp.erase(itor_hp);
						}
					}
				}
			}

			int const iObjectNo = pkUnit->GetAbil(AT_CLASS);
			auto bRet = m_kContObjectNum.insert(std::make_pair(iObjectNo, 1));
			if( !bRet.second )
			{
				++(*bRet.first).second;
			}
			m_bPortal = true;
		}break;
	}
	
	bool bResult = PgGround::ReleaseUnit( pkUnit, bRecursiveCall, bSendArea );

	if ( bSendMonsterNum )
	{
		size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
		if ( IsLastStage() && (0==iLiveMonsterCount) && (false == IsDefenceGameMode()) )
		{
			// ������ ���������̰� ���Ͱ� �ϳ��� ������ �̼� ����
			AddPlayTime(DifftimeGetTime(m_dwStartTime,BM::GetTime32()));
			SetState(INDUN_STATE_RESULT_WAIT);
		}
	}

	if( !bResult )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return bResult;
}


bool PgMissionGround::MissionCompleteCheck()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	int const iGroundNo = GetStageGroundNo(m_iNowStage);

	ContMissionOptionMissionList const *rkMissionOptionMission = NULL;
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission(GetMissionNo(), pkMissionInfo);
	if( bRet )
	{		
		if( false == pkMissionInfo->GetMissionOptionMission(GetLevel(), rkMissionOptionMission) )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionOptionComplete is Fail : MissionNo[") << GetMissionNo() << _T("], GroundNo[") << iGroundNo << _T("], Level[") << GetLevel()+1 << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMissionOptionComplete is Failed!"));
			return false;
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionInfo is Fail : MissionNo[") << GetMissionNo() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMission is Failed!"));
		return false;
	}

	if( NULL == rkMissionOptionMission )
	{
		return false;
	}

	EMissionType eRet = MissionCheck(rkMissionOptionMission);
	if( MM_NONE == eRet )
	{
		return true;
	}
	return false;
}

void PgMissionGround::SetTeam(BM::GUID const& kGuid, int const iTeam)
{
    ConUser::iterator user_itr = m_kConUser.find(kGuid);
    if(user_itr != m_kConUser.end() )
    {
        (*user_itr).second.iTeam = iTeam;
    }
}

int PgMissionGround::GetTeam(BM::GUID const& kGuid)const
{
    ConUser::const_iterator user_itr = m_kConUser.find(kGuid);
    if(user_itr != m_kConUser.end() )
    {
        return (*user_itr).second.iTeam;
    }
    return 0;
}

bool PgMissionGround::MissionStageClearCheck()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgMissionInfo const * pkMissionInfo = NULL;
	ContMissionOptionChapter kContMissionOptionChapter;

	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{		
		int const iGroundNo = GetStageGroundNo(m_iNowStage);

		bool const bRet2 = pkMissionInfo->GetMissionOptionChapter( iGroundNo, GetLevel(), kContMissionOptionChapter );
		if( !bRet2 )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionOptionChapter is Fail : MissionNo[") << GetMissionNo() << _T("], GroundNo[") << iGroundNo << _T("], Level[") << GetLevel()+1 << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMissionOptionChapter is Failed!"));
			return false;
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionInfo is Fail : MissionNo[") << GetMissionNo() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMission is Failed!"));
		return false;
	}

	EChapterType eRet = ChapterCheck( &kContMissionOptionChapter );
	if( MC_NONE == eRet )
	{
		return true;
	}
	return false;
}

bool PgMissionGround::MissionOptionChapterCheck(eMissionOptionKind eType)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgMissionInfo const * pkMissionInfo = NULL;
	ContMissionOptionChapter kContMissionOptionChapter;

	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{		
		int const iGroundNo = GetStageGroundNo(m_iNowStage);

		bool const bRet2 = pkMissionInfo->GetMissionOptionChapter( iGroundNo, GetLevel(), kContMissionOptionChapter );
		if( !bRet2 )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionOptionChapter is Fail : MissionNo[") << GetMissionNo() << _T("], GroundNo[") << iGroundNo << _T("], Level[") << GetLevel()+1 << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMissionOptionChapter is Failed!"));
			return false;
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionInfo is Fail : MissionNo[") << GetMissionNo() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMission is Failed!"));
		return false;
	}

	switch( eType )
	{
	case MO_CHAPTER:
		{
			EChapterType eRet = ChapterCheck( &kContMissionOptionChapter );
			switch( eRet )
			{
			case MC_NONE:
				{
				}break;
			case MC_MON_ALL_KILL:
				{
					Broadcast(BM::Stream(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE,2));//���� �� ��ƾ� ���� �̵� �� �� �ִ�!!!
					return false;
				}break;
			case MC_MON_TARGET_KILL:
				{
					Broadcast(BM::Stream(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE,2));
					return false;
				}break;
			case MC_EFFECT:
				{
					Broadcast(BM::Stream(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE, 413));
					return false;
				}break;
			case MC_OBJECT_TARGET_KILL:
				{
					Broadcast(BM::Stream(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE, 414));
					return false;
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
					return false;
				}break;				
			}
			return true;
		}break;
	case MO_SCENARIO_TIMEATTACK:
		{
			ContMissionOptionChapter::const_iterator iter = kContMissionOptionChapter.begin();
			while( kContMissionOptionChapter.end() != iter )
			{		
				if( (*iter).iType == MC_TIME_ATTACK )
				{
					return true;
				}
				++iter;
			}
			return false;
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Get MissionInfo is Fail : Mission Type[") << eType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
			return false;
		}break;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionGround::CheckPlayerDie(CUnit* pkUnit)
{
	if (!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_OBJECT_MGR_UNIT* pkUnitCont = NULL;
	if (PgObjectMgr::GetUnitContainer(UT_PLAYER, pkUnitCont))
	{
		if (1 >= pkUnitCont->size())	//ȥ�� ������
		{
			return true;
		}

		CONT_OBJECT_MGR_UNIT::const_iterator it = pkUnitCont->begin();
		BM::Stream kClose(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE);
		kClose.Push(417);

		std::list<CUnit*> kCloseList;
		bool bDieUser = false;

		while (it != pkUnitCont->end())
		{
			CUnit* pkOtherUnit = it->second;
			if (pkOtherUnit)
			{
				if( true == pkOtherUnit->IsDead() )
				{
					bDieUser = true;
				}
				else
				{
					// �뺸�� ����(������ ����)
					kCloseList.push_back(pkOtherUnit);
				}
			}
			++it;
		}
		
		if( true == bDieUser )
		{
			std::list<CUnit*>::iterator itClose = kCloseList.begin();
			while (kCloseList.end() != itClose)
			{
				(*itClose)->Send(kClose);
				++itClose;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgMissionGround::CheckPlayerInArea(CUnit* pkUnit, float fRange)
{
	if (!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	POINT3 const kCenter = pkUnit->GetPos();

	CONT_OBJECT_MGR_UNIT* pkUnitCont = NULL;
	if (PgObjectMgr::GetUnitContainer(UT_PLAYER, pkUnitCont))
	{
		if (1 >= pkUnitCont->size())	//ȥ�� ������
		{
			return true;
		}
		CONT_OBJECT_MGR_UNIT::const_iterator it = pkUnitCont->begin();
		BM::Stream kClose(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE);
		BM::Stream kFar(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE);

		kClose.Push(0);
		kFar.Push(1);

		std::list<CUnit*> kFarList;
		std::list<CUnit*> kCloseList;
		
		while (it != pkUnitCont->end())
		{
			CUnit* pkOtherUnit = it->second;
			if (pkOtherUnit)
			{
				float const fDist = pkOtherUnit->GetPos().Distance(kCenter, pkOtherUnit->GetPos());
				if (fDist > fRange)	// �ָ� �ִ� ��
				{	//��Ŷ ������
					kFarList.push_back(pkOtherUnit);
				}
				else	// ������ �ִ� ��
				{
					//��Ŷ ������
					kCloseList.push_back(pkOtherUnit);
				}
			}
			++it;
		}
		if (!kFarList.empty())
		{
			std::list<CUnit*>::iterator itFar = kFarList.begin();
			while (kFarList.end() != itFar)
			{
				(*itFar)->Send(kFar);
				++itFar;
			}

			std::list<CUnit*>::iterator itClose = kCloseList.begin();
			while (kCloseList.end() != itClose)
			{
				(*itClose)->Send(kClose);
				++itClose;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	return true;
}

bool PgMissionGround::CheckMonsterInMap()
{
	size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
	if ( 0 == iLiveMonsterCount )
	{
		return true;
	}

	Broadcast(BM::Stream(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE,2));//���� �� ��ƾ� ���� �̵� �� �� �ִ�!!!
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionGround::RecvRecentMapMove( PgPlayer *pkUser )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if ( !pkUser )
	{
		CUnit *pkUnit = dynamic_cast<CUnit*>(pkUser);
		if ( pkUnit )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unit[") << pkUnit->Name().c_str() << _T("] Type[") << pkUnit->UnitType() << _T("] Is Not Player") );
		}
		else
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unit Is Null") );
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	UpdateIndunRecentMap(pkUser);

	switch( m_eState )
	{
	case INDUN_STATE_READY:
	case INDUN_STATE_PLAY:
		{
			PgThrowUpPenalty kAction( GroundKey(), true );
			kAction.DoAction( pkUser, -5000 );

			//��Ƽ�� ��ü
			BM::GUID const& kPartyGuid = pkUser->GetPartyGuid();
			BM::GUID const& kCharGuid = pkUser->GetID();
			if( BM::GUID::IsNotNull(kPartyGuid) )
			{
				size_t const iMinSize = 1;
				VEC_GUID kContGuid;
				m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kContGuid, kCharGuid);
				if( m_kLocalPartyMgr.IsMaster(kPartyGuid, kCharGuid)
				&&	iMinSize <= kContGuid.size() )
				{
					// ���ʸ� ��ü�ؾ� �Ѵ�.
					BM::Stream kNPacket(PT_C_N_REQ_PARTY_CHANGE_MASTER, kCharGuid);
					kNPacket.Push( (*kContGuid.begin()) );
					::SendToGlobalPartyMgr(kNPacket);
				}
			}

 			//if ( PgMission::ChangeOwner(pkUser->GetID()) )
			if( IsDefenceMode8() )
			{
				// ��������� Owner�� ��ü
				CUnit* pkUnit = PgObjectMgr::GetUnit(pkUser->GetID());
				if( pkUnit )
				{
					TeamChangeOwner(pkUnit);
				}
			}
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{
		}break;
	case INDUN_STATE_RESULT:
		{
			// �׾��µ� ���â ���� ���߿� �ñ�� �Ǹ� ó��
			CUnit *pkUnit = dynamic_cast<CUnit*>(pkUser);
			if( pkUnit )
			{
				if( (true == pkUnit->IsDead()) && (!IsDefenceGameMode()) )
				{
					BM::Stream kCPacket( PT_M_C_NFY_MISSION_UNLOCKINPUT );					
					pkUnit->Send(kCPacket);
				}
			}
		}break;
	}

	//�̼� ����Ʈ ����
	//PgAction_RemoveMissionQuest kRemoveMissionQuest;
	//kRemoveMissionQuest.DoAction(pkUser, NULL);

	unsigned int const iMissionKey = (unsigned int)GetKey();
	PgPlayer_MissionData const *pkMissionData = pkUser->GetMissionData( iMissionKey );
	if ( pkMissionData )
	{
		BM::Stream kCPacket( PT_M_C_NFY_MISSION_DATA, iMissionKey );
		pkMissionData->WriteToPacket( kCPacket );
		pkUser->Send(kCPacket);
	}

	return PgGround::RecvRecentMapMove(pkUser);
}

void PgMissionGround::SendMapLoadComplete( PgPlayer *pkUser )
{
	BM::CAutoMutex Lock( m_kRscMutex );

	bool bAddMissionUser = false;
	switch( m_eState )
	{
	case INDUN_STATE_WAIT:
		{
			if ( SUCCEEDED(ReleaseWaitUser( pkUser )) && (0 == m_iNowStage) )
			{
				bAddMissionUser = true;
				if( pkUser->PartyGuid().IsNull() || m_kLocalPartyMgr.IsMaster(pkUser->PartyGuid(), pkUser->GetID()) )
				{
					m_kRecentInfo = pkUser->GetRecentMapInfo();
				}
			}
		}break;
	case INDUN_STATE_READY:
	case INDUN_STATE_PLAY:
		{
			PgGround::SendMapLoadComplete( pkUser );
		}break;
	default:
		{
			// xxxxxx �ٽ� ���� ������ �Ѵ�.
		}break;
	}

	if( bAddMissionUser || (NULL == PgMission::GetMissionUser(pkUser->GetID())) )
	{
		SMissionPlayerInfo kInfo;
		kInfo.kCharGuid = pkUser->GetID();
		kInfo.iLevel = pkUser->GetAbil(AT_LEVEL);
		kInfo.kClass = (BYTE)pkUser->GetAbil(AT_CLASS);
		kInfo.wstrName = pkUser->Name();
		kInfo.iAccExp = -pkUser->GetAbil64(AT_EXPERIENCE);	// ó���� ����ġ�� minus�� �����صд�
		PgMission::AddMissionUser(kInfo.kCharGuid, kInfo);

		if( false == bAddMissionUser )
		{
			SendMissionState(pkUser);
		}

		if( pkUser && IsDefenceMode8() )
		{
			BM::GUID const kPartyGuid( pkUser->PartyGuid() );
			SGroundKey kGndKey;
			pkUser->GetRecentGround( kGndKey, GATTR_DEFAULT );

			BM::Stream kNPacket(PT_M_T_NFY_DEFENCE_PLAY_PARTYINFO);
			kNPacket.Push( kGndKey );
			kNPacket.Push( kPartyGuid );
			SendToMissionMgr(kNPacket);
		}
	}
}

bool PgMissionGround::SwapStage( BM::Stream * const pkPacket )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if ( m_eState != INDUN_STATE_PLAY )
	{
		// �÷��� ���°� �ƴϸ� SwapStage�Ұ�
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("SwapStage Failed State[") << m_eState << _T("] Ground[") << GroundKey().GroundNo() << _T("-") << GroundKey().Guid().str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// ResPacket processing
	size_t iVecSize = 0;
	bool bDoUpdate = false;
	pkPacket->Pop(bDoUpdate);
	pkPacket->Pop(iVecSize);

	BM::GUID MasterGuid, PartyGuid;
	for (size_t i=0; i<iVecSize; i++)
	{
		BM::GUID kCharGuid;
		pkPacket->Pop(kCharGuid);

		PgPlayer* pkPlayer = GetUser(kCharGuid);
		if (!bDoUpdate || pkPlayer == NULL)
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find User[") << kCharGuid.str().c_str() << _T("]") );
			PgPlayer kPlayer;
			kPlayer.ReadFromPacket(*pkPacket);
		}
		else
		{
			pkPlayer->ReadFromPacket(*pkPacket);

			PartyGuid = pkPlayer->GetPartyGuid();
		}
	}

	if( true == m_kLocalPartyMgr.GetPartyMasterGuid(PartyGuid, MasterGuid) )
	{
		BM::Stream Packet(PT_M_T_REQ_CLEAR_PARTY_WAITER);
		Packet.Push(MasterGuid);
		Packet.Push(PartyGuid);
		SendToGlobalPartyMgr(Packet);
	}

	size_t iStage = 0;
	pkPacket->Pop(iStage);
	pkPacket->Pop(iStage);	// AddonPacket size
	pkPacket->Pop(iStage);	// Stage

	if ( m_iNowStage == iStage )	
	{// Stage ������ �Ұ���.
		return false;
	}

	if ( iStage >= m_kGndResounrce.size() )	
	{// �߸��� Stage
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	PgWorldEventMgr::ClearGenPointCount();

	// Portal
	m_bPortal = false;

	BM::Stream kTempMovePacket;

	{// 1. Player�� Packet���� �����
		UNIT_PTR_ARRAY kUnitArray;

		CUnit *pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr2::GetFirstUnit(UT_PLAYER, kItor);
		while ( (pkUnit = PgObjectMgr2::GetNextUnit(UT_PLAYER, kItor)) != NULL )
		{
			kUnitArray.Add( pkUnit, false, false, false );
		}

		if ( kUnitArray.empty() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		kUnitArray.WriteToPacket( kTempMovePacket, WT_MAPMOVE_SERVER );
	}

	{// 2. Pet�� Packet���� �����
		size_t iPetSize = 0;
		size_t const iWRPos = kTempMovePacket.WrPos();
		kTempMovePacket.Push( iPetSize );

		CUnit *pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr2::GetFirstUnit(UT_PET, kItor);
		while ( (pkUnit = PgObjectMgr2::GetNextUnit(UT_PET, kItor)) != NULL )
		{
			PgPet * pkPet = dynamic_cast<PgPet*>(pkUnit);
			if ( pkPet )
			{
				pkPet->WriteToPacket_SPetMapMoveData(kTempMovePacket);
				++iPetSize;
			}
		}

		kTempMovePacket.ModifyData( iWRPos, &iPetSize, sizeof(iPetSize) );
	}

	CONT_UNIT_SUMMONED_MAPMOVE_DATA kContUnitSummonedData;
	{// 3. ��ȯü �����̳ʸ� ����
		//CONT_UNIT_SUMMONED_MAPMOVE_DATA	//BM::GUID(Player), ��ȯü��
		//CONT_SUMMONED_MAPMOVE_DATA		//BM::GUID(Summoned), ��ȯü		
		CUnit *pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr2::GetFirstUnit(UT_SUMMONED, kItor);
		while ( (pkUnit = PgObjectMgr2::GetNextUnit(UT_SUMMONED, kItor)) != NULL )
		{
			PgSummoned * pkSummoned = dynamic_cast<PgSummoned*>(pkUnit);
			if ( pkSummoned && pkSummoned->IsAlive() )
			{
				auto kPair = kContUnitSummonedData.insert(std::make_pair(pkSummoned->Caller(), SUserSummonedMapMoveData()));
				CONT_SUMMONED_MAPMOVE_DATA & kContSummoned = kPair.first->second.kSummonedMapMoveData;

				BM::Stream kSummonedPacket;
				pkSummoned->WriteToPacket_MapMoveData(kSummonedPacket);

				BM::GUID kGuid;
				kSummonedPacket.Pop(kGuid);
				if(kGuid==pkSummoned->GetID())
				{
					SSummonedMapMoveData kValue;
					kValue.ReadFromPacket(kSummonedPacket);
					
					kContSummoned.insert(std::make_pair(pkSummoned->GetID(), kValue));
				}
			}
		}
	}


	// 4. Release / Init
	m_iNowStage = iStage;
	PgGroundResource const *pkGndRsc = m_kGndResounrce.at(iStage);

	ReleaseZone();
	ReleaseAllUnit();
	m_kContGenPoint_Monster.clear();
	m_kContGenPoint_Object.clear();
	m_kContGenPoint_SummonNPC.clear();
	m_kAIMng.Release();
	CloneResource( pkGndRsc );
	PgIndun::Init( m_iMonsterControlID, m_iNowStage!=0 );
	m_kContMonsterNum.clear();
	m_kContObjectNum.clear();
	StageClear(false);

	POINT3 pt3TargetPos;
	if ( FAILED(FindSpawnLoc( 1, pt3TargetPos)) )
	{// ����� �ɸ��� �ȵǴµ�... 18
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 5. ��Ŷ���� ���� Player�� �̾ƿ���
	UNIT_PTR_ARRAY kTempUnitArray;
	CONT_PET_MAPMOVE_DATA kContPetData;
	kTempUnitArray.ReadFromPacket( kTempMovePacket );
	PU::TLoadTable_AM( kTempMovePacket, kContPetData );

	// 6. ��Ŷ���� �̾ƿ� Player��� Pet, Summoned �������ѳ���
	VEC_GUID kWaitList;
	UNIT_PTR_ARRAY::iterator unit_itr = kTempUnitArray.begin();
	for ( ; unit_itr!= kTempUnitArray.end(); ++unit_itr )
	{
		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if ( pkUser )
		{
			if ( 0 == iStage )
			{
				// �޺� ī���� �ʱ�ȭ
				PgComboCounter* pkCounter = NULL;
				if( GetComboCounter(pkUser->GetID(), pkCounter) )
				{
					pkCounter->Clear(false);
				}
				GMState(false);
				GMScore(0);
			}

			PgAction_QuestMissionPercent kQuestAction(GroundKey(), GetMissionKey().iKey);//�̼� �޼�������Ʈ
			kQuestAction.DoAction(pkUser, NULL);

			pkUser->SetPos(pt3TargetPos);
			pkUser->LastAreaIndex(PgSmallArea::NONE_AREA_INDEX);//���� �̰ɷ� ����
			pkUser->MapLoading();	// PT_C_M_NFY_MAPLOADED ������ ���� ��� Action�� ����~
			AddUnit( unit_itr->pkUnit, false );
			unit_itr->bAutoRemove = false;	// �ڵ������� ���ƾ���...
			
			if ( PgMission::GetMissionUser( pkUser->GetID() ) )
			{
				kWaitList.push_back( pkUser->GetID() );
			}

			BM::GUID const kSelectedPetID = pkUser->SelectedPetID();
			if ( kSelectedPetID.IsNotNull() )
			{
				CONT_PET_MAPMOVE_DATA::iterator pet_move_itr = kContPetData.find( kSelectedPetID );
				if ( pet_move_itr != kContPetData.end() )
				{
					this->CreatePet( pkUser, kSelectedPetID, pet_move_itr->second );
				}
// 				else
// 				{
// 					CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Not Found SelectedPet<" << kSelectedPetID << L">Info From User<" << pkUser->Name() << L"/" << pkUser->GetID() << L">" );
// 				}
			}

			//��ȯü ����
			CONT_UNIT_SUMMONED_MAPMOVE_DATA::iterator cont_summoned = kContUnitSummonedData.find(pkUser->GetID());
			if(cont_summoned != kContUnitSummonedData.end())
			{
				CONT_SUMMONED_MAPMOVE_DATA const& kSummonedMapMoveData = cont_summoned->second.kSummonedMapMoveData;
				CONT_SUMMONED_MAPMOVE_DATA::const_iterator c_it = kSummonedMapMoveData.begin();
				while(c_it != kSummonedMapMoveData.end())
				{
					SCreateSummoned kCreateInfo;
					kCreateInfo.kGuid = c_it->first;
					kCreateInfo.kClassKey.iClass = c_it->second.iClass;
					kCreateInfo.kClassKey.nLv = c_it->second.iLevel;
					kCreateInfo.bUniqueClass = c_it->second.bUniqueClass;
					kCreateInfo.sNeedSupply = c_it->second.sNeedSupply;
					kCreateInfo.iLifeTime = c_it->second.iLifeTime;

					CreateSummoned(pkUser, &kCreateInfo, L"Summoned", c_it->second);

					++c_it;
				}
			}
		}
	}

	// 7. WaitList�� �ʱ�ȭ �ϰ�
	if ( !kWaitList.empty() )
	{
		InitWaitUser( kWaitList );
	}

	// 8. Player���� ������ ��Ŷ(�Ϲ� ���̵��� ����)������
	for ( unit_itr=kTempUnitArray.begin(); unit_itr!=kTempUnitArray.end(); ++unit_itr )
	{
		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if ( pkUser )
		{
			// SwitchServer �� User Ground ������ �˷�����.
			BM::Stream kDPacket( PT_M_S_NFY_CHARACTER_MAP_MOVE);
			kDPacket.Push(pkUser->GetMemberGUID());
			kDPacket.Push(pkUser->GetID());
			g_kProcessCfg.ServerIdentity().WriteToPacket(kDPacket);
			GroundKey().WriteToPacket(kDPacket);
			SendToServer( pkUser->GetSwitchServer(), kDPacket );

			// client�� �ʷε� ��û�ϱ�
			BM::Stream kCPacket( PT_T_C_NFY_CHARACTER_MAP_MOVE );
			kCPacket.Push((BYTE)MMET_Normal);
			kCPacket.Push(GroundKey());
			kCPacket.Push(this->GetAttr());
			kCPacket.Push(pkUser->GetID());
			kCPacket.Push(static_cast<size_t>(1));
			pkUser->WriteToPacket( kCPacket, WT_MAPMOVE_CLIENT );//���� �� ���µ� �ڿ� ����

			m_kSmallAreaInfo.WriteToPacket( kCPacket );// Zone ������ �ٿ��� ������ �Ѵ�.
			pkUser->Send( kCPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );

			// HP,MP�� Client�� �߸� ����Ͽ� �ùٸ��� �Ⱥ��̴� ���װ� �־�, ������� �ѹ� �� ���� �ش�.
			WORD const wAbil[2] = { AT_HP, AT_MP };
			pkUser->SendAbiles( wAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);
		}
	}

	// �ٽ� ������ �׶��� ��ȣ�� ������ �־�� �Ѵ�.
	m_kGroundKey.GroundNo( m_iRegistGndNo );

	PgPlayer* pkUser = GetUser(PgMission::GetOwner());
	if(pkUser)
	{
		if( pkUser->GetPartyGuid().IsNotNull() )
		{
			PgGroundUtil::SendPartyMgr_Refuse(pkUser->GetID(), EPR_MAP, false==IsPartyBreakIn());
			PgGroundUtil::SendPartyMgr_Refuse(pkUser->GetID(), EPR_BOSSROOM, IsBossStage());
		}
	}
	return true;
}

bool PgMissionGround::IsStrategicMonsterDie(CUnit * pkUnit)const
{
    if( IsDefenceMode7() || IsDefenceMode8() )
    {
        BM::GUID const & kGuid = pkUnit->GetTarget();
        CUnit * pkTargetUnit = GetUnit(kGuid);
        EUnitType const kCheckUnitType = static_cast<EUnitType>(UT_PLAYER | UT_ENTITY| UT_SUMMONED | UT_SUB_PLAYER);
        if( pkTargetUnit && pkTargetUnit->IsInUnitType(kCheckUnitType) )
        {
            return true;
        }
        return false;
    }
    return true;
}

bool PgMissionGround::CheckApplyUnitOwner(CUnit* pkOwner, BM::GUID& rkOutApplyOwner)
{
    if( !pkOwner )
    {
        return false;
    }

    if( IsDefenceMode7() || IsDefenceMode8() )
    {
        CUnit* pkCaller = pkOwner;
        while( pkCaller && pkCaller->IsUnitType(UT_ENTITY) )
        {
            if( ENTITY_GUARDIAN==pkCaller->GetAbil(AT_ENTITY_TYPE) )
            {
                rkOutApplyOwner = pkCaller->GetID();
                return true;
            }

            if( !pkCaller->IsHaveCaller() )
            {
                break;
            }

            pkCaller = GetUnit(pkCaller->Caller());
        }
    }
    return PgGround::CheckApplyUnitOwner(pkOwner, rkOutApplyOwner);
}

int PgMissionGround::GetGiveLevel(CUnit * pkMonster, CUnit * pkOwner)
{
    if( IsDefenceMode7() || IsDefenceMode8() )
    {
        //
        //���� ������ ������ ã��
        //
        BM::GUID kOwnerGuid;
        BM::GUID kApplyOwnerGuid;
        CUnit* pkApplyOwner = pkOwner;

        if( CheckUnitOwner(pkMonster, kOwnerGuid, &kApplyOwnerGuid) )
        {
            if( pkOwner && pkOwner->GetID()!=kApplyOwnerGuid )
            {
                pkApplyOwner = GetUnit( kApplyOwnerGuid );
            }
        }

        //
        //
        //
        if( pkApplyOwner )
        {
            if(pkApplyOwner->IsUnitType(UT_ENTITY) && ENTITY_GUARDIAN==pkApplyOwner->GetAbil(AT_ENTITY_TYPE))
            {
                return pkApplyOwner->GetAbil(AT_LEVEL);
            }
        }
    }
    return PgGround::GetGiveLevel(pkMonster, pkOwner);
}

void PgMissionGround::GiveExp(CUnit* pkMonster, CUnit* pkOwner)
{
    if( IsStrategicMonsterDie(pkMonster) )
    {
        PgGround::GiveExp(pkMonster, pkOwner);
    }
}

HRESULT PgMissionGround::SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont )
{
	int const iGrade = pkDroper->GetAbil(AT_GRADE);
	/*if( pkOwner )
	{
		SMissionPlayerInfo* pkMissionInfo = GetMissionUser(pkOwner->GetID());
		if ( pkMissionInfo )
		{
			WORD iPoint = ms_kGradeValue[iGrade];
			pkMissionInfo->kPoint[MPOINT_MONSTER] += (int)iPoint;
		}
	}*/
	
    if( !IsStrategicMonsterDie(pkDroper) )
    {
        return S_OK;
    }

	switch( iGrade )
	{
	case EMGRADE_ELITE:
		{
			HRESULT hRet = PgGround::SetUnitDropItem(pkOwner, pkDroper, kLogCont );

			if( true == IsDefenceGameMode() )
			{
				return hRet;
			}
			// ������ ������ �̼� ����
			//////////////////////////////////////////
			// �̼� �Ϸ� ���� ���� �ɼ� �߰� //
			m_iEleite++;

			AddPlayTime(DifftimeGetTime(m_dwStartTime,BM::GetTime32()));
			SetState(INDUN_STATE_RESULT_WAIT);
			return S_FALSE;
		}break;
	case EMGRADE_MISSION_ELITE_OBJECT:
		{
			PgGround::SetUnitDropItem(pkOwner, pkDroper, kLogCont );

			//if( true == MissionCompleteCheck() )
			{
				AddPlayTime(DifftimeGetTime(m_dwStartTime,BM::GetTime32()));
				SetState(INDUN_STATE_RESULT_WAIT);
				return S_FALSE;
			}
		}break;
	default:
		{
			return PgGround::SetUnitDropItem(pkOwner, pkDroper, kLogCont );
		}break;
	}
	return S_OK;
}

bool PgMissionGround::SetMission(PgMission* pkMission)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	return CloneMission(pkMission);
}

void PgMissionGround::ReStartMission( PgMission* pkMission, ConStageResource& rkContResource )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	PgMission::ConUser::const_iterator user_itr = PgMission::m_kConUser.begin();
	for( ; user_itr != PgMission::m_kConUser.end() ; ++user_itr )
	{
		pkMission->AddMissionUser( user_itr->first );
	}

	PgMission::Clear();
	SetMission(pkMission);
	m_kGndResounrce.swap(rkContResource);

	// MapLevel�ٽü���
	SetMapLevel( pkMission->GetLevel()+1 );

	// ���� ��Ʈ�� �� ���̵� �ٽ� ����
	m_iMonsterControlID = pkMission->GetAbilRateBagNo();

	DWORD const dwCurTime = BM::GetTime32();
	
	m_dwStartTime = dwCurTime;
	m_dwAutoStateRemainTime = 0;
	m_dwTimeScoreStart = 0;
	m_iTimeScore = 0;
	iTotalScoreUpdate = 0;
	iTotalOverScore = 0;
	IndunPartyDie(false);

	bool bDefenceMode = false;
	if( MAX_MISSION_LEVEL <= (pkMission->GetLevel()+1) )
	{
		bDefenceMode = true;
	}

	PgPlayer* pkUser = GetUser(PgMission::GetOwner());

	CONT_PLAYER_MODIFY_ORDER kOrder;
	if( true == bDefenceMode )
	{
		if( PgMission::GetOwner() != BM::GUID::NullData() )
		{			
			if( pkUser )
			{				
				if( MAX_MISSION_LEVEL == (pkMission->GetLevel()+1) )
				{
					if( true == PgGround::MissionItemOrderCheck(pkUser, kOrder, GetMissionNo(), 1,MAX_MISSION_LEVEL) )
					{
					}
					else
					{
						pkUser->SendWarnMessage2(460034, 1);
						return;
					}
				}
				else if( DEFENCE7_MISSION_LEVEL == (pkMission->GetLevel()+1) )
				{
					int iSubCount = 1;
					size_t iCount = m_kLocalPartyMgr.GetMemberCount(pkUser->PartyGuid());
					if( 2 < iCount )
					{
						iSubCount = 2;											
					}
					if( true == PgGround::MissionItemOrderCheck(pkUser, kOrder, GetMissionNo(), iSubCount) )
					{
					}
					else
					{
						pkUser->SendWarnMessage2(400976, iSubCount);
						return;
					}
				}
				else
				{
					// ��������
					return;
				}
			}
		}
	}
	else if( MILT_LEVLE2 == (pkMission->GetLevel()+1) )
	{					
		if( (MT_EVENT_MISSION != pkMission->GetTriggerType()) && (MT_EVENT_HIDDEN != pkMission->GetTriggerType()) )
		{
			if( pkUser )
			{
				if( false == MissionChaosItemOrderCheck(pkUser, kOrder, GetMissionNo(), 1) )
				{
					pkUser->SendWarnMessage(401194);
					pkUser->Send(BM::Stream(PT_M_C_ANS_RESTART_MISSION_INFO_UI));
					return;
				}
			}
		}
	}

	m_eState = INDUN_STATE_PLAY;

	if( true == SwapStage_Before( 0, NULL ) )
	{
		if(false == kOrder.empty())
		//if( true == bDefenceMode ) //���� ������ ���潺��常�̶�� ������ �޾Ƴ��Ҵ��� �𸣰�����, 
		{							 //��·�� ����� ī������ ��ũ�� ������ ������ ���ҵǾ�� �ϹǷ� ���� Ǯ����Ұ� ������ �۵��Ǵµ� �ϴ�.
			PgAction_ReqModifyItem kItemModifyAction(CIE_GateWayUnLock, GroundKey(), kOrder, BM::Stream(), true);
			kItemModifyAction.DoAction(NULL, NULL);
		}
	}	
}

void PgMissionGround::AddStage( ConStageResource::value_type pkGndResource )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	m_kGndResounrce.push_back(pkGndResource);
}

void PgMissionGround::EndDefenceMission()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( false == IsDefenceGameMode() )
	{
		return;
	}

	m_bDefenceStageStart = true;
	if( false == m_bDefenceEndStage )
	{
		m_bDefenceEndStage = true;
	}
	else
	{
		return;
	}

	bool bResultItem = false;
	int iRetContNo = 0;
	DWORD dwStageExp = 0;
	DWORD kNow = BM::GetTime32();
	eMonsterGen = D_RANDOM;
	
	if( m_dwStage_Time >= DifftimeGetTime(m_dwStage_StartTime, kNow) )
	{
		dwStageExp = (m_dwStage_Time - DifftimeGetTime(m_dwStage_StartTime, kNow)) / 1000;
	}
	if( 0 < dwStageExp )
	{
		CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
		if( true == GetDefenceStage(m_iNowStage+1, kStageData) )
		{
			CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);
			m_dwStageExp = m_dwStageExp + (dwStageExp * kValue.iTimeToExp_Rate);

			bResultItem = ((0 < kValue.iResultNo) ? true : false);
		}
	}

	SMissionKey const & kMissionKey = GetMissionKey();
	BYTE kWriteType = PgMission_Result::VIEWTYPE_DEFENCE;

	BM::Stream kPacket;
	kPacket.Push(static_cast<BYTE>(kWriteType));
	kPacket.Push(GetConstellationKey().PartyGuid.IsNotNull());
	kPacket.Push(static_cast<bool>(IsDefenceLastStage()));
	kPacket.Push(static_cast<__int64>(m_dwStageExp));			// �߰� ����ġ �ֱ�	
	kPacket.Push(GetMissionNo());				
	kMissionKey.WriteToPacket(kPacket);
	kPacket.Push(m_iSelect_SuccessCount);
	kPacket.Push(m_bPrevStage_UseSelectItem);

	CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type kElement;
	if( false == GetDefenceAddMonsterStage(m_iNowStage+1, m_iSelect_SuccessCount+1, kElement) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("DefenceAddMonsterInfo empty Error! Stage[") << m_iNowStage+1 << _T("]") );
	}

	kPacket.Push(kElement.iMonsterNo);
	kPacket.Push(kElement.iItemNo);
	kPacket.Push(kElement.iDropRate);

	kPacket.Push(bResultItem);
	if( true == bResultItem )
	{
		kPacket.Push(m_kConUser.size());
		ConUser::iterator user_itr;
		for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
		{			
			PgPlayer* pkUser = GetUser(user_itr->first);
			if ( pkUser )
			{
				SMissionPlayerInfo_Client kInfo;

				kInfo.kCharGuid = pkUser->GetID();
				kInfo.iLevel = pkUser->GetAbil(AT_LEVEL);
				kInfo.kClass = pkUser->GetAbil(AT_CLASS);
				kInfo.wstrName = pkUser->Name();


				RESULT_SITEM_BAG_LIST_CONT	kConRetItem;				
				RESULT_SITEM_BAG_LIST_CONT	kContItem;
				int iResultRewardItemCount = 0;

				CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
				if( true == GetDefenceStage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
				{
					CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);

					iResultRewardItemCount = kValue.iResultCount;
					iRetContNo = kValue.iResultNo;
				}

				if( ms_iRewardItemMax < iResultRewardItemCount )
				{
					iResultRewardItemCount = ms_iRewardItemMax;

					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Mission ResultItem Reward Count Error! MissionNo[") << GetMissionNo() << _T("] GroundNo[") << GetGroundNo() << _T("]") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
				}

				PgAction_PopItemContainer kTempPopItem( CIE_Mission, GroundKey(), iRetContNo, GetEventAbil() );
				kTempPopItem.DoAction(pkUser, iResultRewardItemCount, false);						

				if( ms_iRewardItemMax < kTempPopItem.ConRetItemNo().size() )
				{
					// ���� ������ 3~4��, �Ӵ��� 1�� �ִ� 4�� �̻� �� ���� ����.
					iResultRewardItemCount = ms_iRewardItemMax - 1;
				}

				PgAction_PopItemContainer kPopItem( CIE_Mission, GroundKey(), iRetContNo, GetEventAbil() );
				kPopItem.DoAction(pkUser, iResultRewardItemCount, true);

				kPopItem.SwapRetItem(kContItem);


				if( 0 == kContItem.size() )
				{
					// ��� �������� ���ٸ�... "��"���� ó���� ������
					for(int i=0; i<iResultRewardItemCount; ++i)
					{
						PgBase_Item kItem;
						if(S_OK == CreateSItem(iResultItem, 1, PgItemRarityUpgradeFormula::GetItemRarityContorolType(CIE_Mission), kItem))
						{
							kConRetItem.push_back(kItem);
						}
					}
					kConRetItem.swap(kContItem);
					kConRetItem.clear();
				}
				kContItem.swap(kInfo.kContItem);
				kInfo.iResultBagNo = m_kMissionResult.iRouletteBagGrpNo;
				kInfo.iScore = 0;
				kInfo.WriteToPacket(kPacket, static_cast<__int64>(0));

				PgGround *pkGround = dynamic_cast<PgGround*>(this);
				if( pkGround )
				{
					// ���ʽ� ����ġ�� �ش�.
					PgAction_AddExp kAction_AddExp(GroundKey(), static_cast<__int64>(m_dwStageExp), AEC_MissionBonus, pkGround);
					kAction_AddExp.DoAction(pkUser, NULL);

					m_dwStageExp = 0;
				}
			}
		}

		ConstellationResultInit();
		m_kConstellationResult.SetDelayTime(2);
	}

	ConUser::iterator user_itr;
	for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
	{			
		PgPlayer* pkUser = GetUser(user_itr->first);
		if ( pkUser )
		{
			PgAction_QuestMissionUtil::ProcessQuestMission(GetModeType(), GroundKey(), GetMissionKey(), pkUser, MRANK_NONE, GetStageCountExceptBonus(), m_iNowStage, true);

			size_t const kCoinCount = pkUser->GetInven()->GetTotalCount(GADA_COIN_NO);
			int iNeedCount = 1;			
			bool bGadaRet = GetGadaCoinCount(iNeedCount);

			//BM::Stream kCPacket(PT_M_C_NFY_DEFENCE_ENDSTAGE);

			BM::Stream kCPacket(PT_N_C_NFY_MISSION_RESULT);
			kCPacket.Push(kPacket);
			kCPacket.Push(pkUser->GetID() == PgMission::GetOwner());
			kCPacket.Push(static_cast<int>(kCoinCount));
			kCPacket.Push(static_cast<int>(iNeedCount));
			kCPacket.Push(iRetContNo);
			kCPacket.Push(PgMission::GetTriggerType());
			pkUser->Send(kCPacket);
		}
	}
}

void PgMissionGround::EndMission(BM::Stream * const pkPacket)
{
	if( m_iModeType != MO_ITEM )
	{
		return;
	}

	GadaCoinUseClear();

	// ResPacket processing
	size_t iVecSize;
	bool bDoUpdate;
	BM::GUID kCharGuid;
	pkPacket->Pop(bDoUpdate);
	pkPacket->Pop(iVecSize);
	for (size_t i=0; i<iVecSize; ++i)
	{
		pkPacket->Pop(kCharGuid);

		PgPlayer* pkPlayer = GetUser(kCharGuid);
		if (!bDoUpdate || pkPlayer == NULL)
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find User[") << kCharGuid.str().c_str() << _T("]") );
			PgPlayer kPlayer;
			kPlayer.ReadFromPacket(*pkPacket);
		}
		else
		{
			pkPlayer->ReadFromPacket(*pkPacket);
		}
	}

	//INFO_LOG(BM::LOG_LV0,_T("[%s] EndMission %d-%s"),__FUNCTIONW__,GroundKey().GroundNo(),GroundKey().Guid().str().c_str());
	BM::Stream kNPacket(PT_M_N_NFY_MISSION_RESULT,PgMission::GetID());
	bool bErrorRank = false;
	CONT_DEF_MISSION_CANDIDATE::const_iterator candi_itr;
	const CONT_DEF_MISSION_CANDIDATE *pkCandi = NULL;
	g_kTblDataMgr.GetContDef(pkCandi);
	if ( !pkCandi )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_MISSION_CANDIDATE is NULL") );
		bErrorRank = true;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCandi is NULL"));
	}
	else
	{
		candi_itr = pkCandi->find(PgMission::m_iCandidateNo);
		if( candi_itr == pkCandi->end() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Not Found Mission Candidate[") << PgMission::m_iCandidateNo << _T("]") );
			bErrorRank = true;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
		}
	}	

	ConUser::iterator user_itr;
	for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
	{
		PgPlayer* pkUser = GetUser(user_itr->first);
		if ( pkUser )
		{
			SMissionKey const & kMissionKey = GetMissionKey();

			PgPlayer_MissionData const * kMissionData = pkUser->GetMissionData(kMissionKey.iKey);
			if(kMissionData)
			{
				int const iAchievementType = AT_ACHIEVEMENT_MISSION_KEY(kMissionKey.iKey,kMissionKey.iLevel);
				if(AT_ACHIEVEMENT_MISSION_MAX >= iAchievementType)
				{
					PgSyncClinetAchievementHandler<> kSA(iAchievementType, kMissionData->GetClearLevelCount(kMissionKey.iLevel), GroundKey());
					kSA.DoAction(pkUser,NULL);
				}
			}

			if(0 == pkUser->GetAbil(AT_ACHIEVEMENT_MISSION_HIT))
			{
				const CONT_DEF_MISSION_ROOT* pContDefMap = NULL;
				g_kTblDataMgr.GetContDef(pContDefMap);
				if (pContDefMap)
				{
					CONT_DEF_MISSION_ROOT::const_iterator root_itr = pContDefMap->find(GetMissionNo());
					if (pContDefMap->end() != root_itr)
					{
						const TBL_DEF_MISSION_ROOT& element = (*root_itr).second;
						int const iMin = element.aiLevel_Min[__max(GetLevel(),0)];
						int const iMax = element.aiLevel_Max[__min(GetLevel(),MAX_MISSION_LEVEL-1)];
						if(iMin <= pkUser->GetAbil(AT_LEVEL) && pkUser->GetAbil(AT_LEVEL) <= iMax)
						{
							PgAddAchievementValue kSA(AT_ACHIEVEMENT_MISSION_NO_HIT, 1, GroundKey());
							kSA.DoAction(pkUser,NULL);
						}
					}
				}
			}

			{// ��ż��� ����: �ڽź��� 5~10���� ���� ������ �̼� �Ϸ� ���� ���( ������ 40���� ���϶�� 3��, �� �ܴ� 1�� )
				const int iLevel = pkUser->GetAbil(AT_LEVEL);
				const int iConditionMinLevel = iLevel - 10; // ���� �޼��� �� �ִ� ������ �ּ� ����
				const int iConditionMaxLevel = iLevel - 5;	// ���� �޼� �� �� �ִ� ������ �ִ� ����
				const int iAdvantageLevel = 40;				// 40���� ���ϴ� �߰� ����
				const int iAdvantageScore = 3;				// �߰� ������ 3��
				const int iMaxScore = 5;					// �ִ� 5���� �ѱ��� ���Ѵ�.

				int iScore = 0;
				
				ConUser::const_iterator c_iter = m_kConUser.begin();
				while( m_kConUser.end() != c_iter )
				{
					const ConUser::mapped_type kMissionPlayerInfo = c_iter->second;

					if( pkUser->GetID() == kMissionPlayerInfo.kCharGuid )
					{
						++c_iter; continue;
					}

					if( iConditionMinLevel <= kMissionPlayerInfo.iLevel 
					&&	iConditionMaxLevel >= kMissionPlayerInfo.iLevel )
					{
						if( kMissionPlayerInfo.iLevel <= iAdvantageLevel )
						{
							iScore += iAdvantageScore;
						}
						else
						{
							++iScore;
						}
					}

					++c_iter;
				}

				if( iMaxScore <= iScore )
				{
					iScore = iMaxScore;
				}

				if( 0 < iScore )
				{
					PgAddAchievementValue kSA(AT_ACHIEVEMENT_MISSION_HELPER, iScore, GroundKey());
					kSA.DoAction(pkUser,NULL);
				}
			}

			pkUser->SetAbil(AT_ACHIEVEMENT_MISSION_HIT,0);// ���� �̼ǿ��� ����ī��Ʈ ����

			ConUser::mapped_type& rkMissionUser = (*user_itr).second;
			// ����ġ~
			rkMissionUser.iAccExp += pkUser->GetAbil64(AT_EXPERIENCE);
			if( rkMissionUser.iAccExp < 0 )
			{
				rkMissionUser.iAccExp = 0;
			}
			
			rkMissionUser.kBonusPoint[MBONUSPOINT_REMAINDER] = 0;
			if( CONN_AREA_PCROOM == pkUser->ConnArea().nArea )
			{
				rkMissionUser.kBonusPoint[MBONUSPOINT_PCROOM] = static_cast<int>(rkMissionUser.iAccExp * 0.1f);
			}
			// 0 / 2.0 / 4.0 / 8.0
			if( m_kConUser.size() > 1 )
			{
				rkMissionUser.kBonusPoint[MBONUSPOINT_PARTY] = static_cast<int>( (rkMissionUser.iAccExp*0.1f) * ((1<<(m_kConUser.size()-1))/100.0f) );
			}
			rkMissionUser.kBonusPoint[MBONUSPOINT_EVENT] = 0;
			rkMissionUser.kBonusPoint[MBONUSPOINT_EVENT] = 0;
			rkMissionUser.kBonusPoint[MBONUSPOINT_AVATA] = 0;
			rkMissionUser.kBonusPoint[MBONUSPOINT_MEMBER] = 0;			

			// ��ũ�� ����Ѵ�.
			rkMissionUser.kRank = static_cast<BYTE>(GetMissionRank(bErrorRank));

			// ���� ����
			rkMissionUser.kPoint[MPOINT_TOTALSCORE] = static_cast<int>(GetTotalScoreUpdateResult());
			// �����⿩�� ����
			rkMissionUser.iScore = rkMissionUser.kTotalScore;

			rkMissionUser.kBonusPoint[MBONUSPOINT_RANK] = static_cast<int>((rkMissionUser.iAccExp*0.2f) * ((22-(rkMissionUser.kRank*2))/100.0f));

			__int64 iBonusPoint = 0;

			for(int i=0; i<MBONUSPOINT_MAX;++i)
			{
				iBonusPoint += rkMissionUser.kBonusPoint[i];
			}

			// �ݺ� Ƚ�� ����
			pkUser->SetAbil(AT_MISSION_NO, GetMissionNo());
			int const iMissionCount = pkUser->GetAbil(AT_MISSION_COUNT);
			pkUser->SetAbil(AT_MISSION_COUNT, (iMissionCount+1));

			MissionCountAddExp(pkUser->GetAbil(AT_MISSION_COUNT), iBonusPoint);

			if( 1 < pkUser->GetAbil(AT_MISSION_COUNT) )
			{
				rkMissionUser.kBonusPoint[MBONUSPOINT_COUNT] = pkUser->GetAbil(AT_MISSION_COUNT);
			}
			else
			{
				rkMissionUser.kBonusPoint[MBONUSPOINT_COUNT] = 0;
			}

			if ( iBonusPoint > 0 )
			{
				// �� ��ų�� ���� 10% ���ʽ� ����ġ
				CEffect *pkEffect = pkUser->GetEffect(PET_EXP_EFFECT_NO);
				if( pkEffect )
				{
					rkMissionUser.kBonusPoint[MBONUSPOINT_PET] = static_cast<int>(iBonusPoint*0.1f);
				}
				else
				{
					rkMissionUser.kBonusPoint[MBONUSPOINT_PET] = 0;
				}

				iBonusPoint += std::max(rkMissionUser.kBonusPoint[MBONUSPOINT_PET],0);

				PgGround *pkGround = dynamic_cast<PgGround*>(this);
				if( pkGround )
				{
					// ���ʽ� ����ġ�� �ش�.
					PgAction_AddExp kAction_AddExp(GroundKey(), static_cast<__int64>(iBonusPoint), AEC_MissionBonus, pkGround);
					kAction_AddExp.DoAction(pkUser, NULL);
					rkMissionUser.iAccExp += static_cast<__int64>(iBonusPoint);
				}
			}
			else
			{
				iBonusPoint = 0;
			}						
			rkMissionUser.kPoint[MPOINT_BONUSEXP] = static_cast<int>(iBonusPoint);
			if( 0 > rkMissionUser.kPoint[MPOINT_BONUSEXP] )
			{
				rkMissionUser.kPoint[MPOINT_BONUSEXP] = 0;
			}
			
			// �̼� Ÿ��
			rkMissionUser.iType = PgMission::GetTriggerType();

			// �������� ����
			size_t const kCoinCount = pkUser->GetInven()->GetTotalCount(GADA_COIN_NO);
			rkMissionUser.kGadaCoinCount = static_cast<int>(kCoinCount);

			int iNeedCount = 1;
			bool bGadaRet = GetGadaCoinCount(iNeedCount);
			rkMissionUser.kGadaCoinNeedCount = iNeedCount;

			//pkUser->UpdateMission( GetMissionKey(), 0x00 );

			// ��� �������� �����Ѵ�.
			if( rkMissionUser.kRank <= MAX_MISSION_RANK_CLEAR_LV
			&&	rkMissionUser.kRank > 0)
			{
				int const iRetRank = rkMissionUser.kRank-1;

				int const iRetContNo = m_kMissionResult.aiResultContainer[iRetRank];
				int const iRetCount = BM::Rand_Range(m_kMissionResult.aiResultCountMax[iRetRank], m_kMissionResult.aiResultCountMin[iRetRank]);

				if(iRetCount || (rkMissionUser.kRank >= MRANK_F) )//���� ������.
				{
					EItemModifyParentEventType CIE_MissionType = CIE_Mission;
					switch( GetLevel() + 1 )
					{
					case 1:
						{
							CIE_MissionType = CIE_Mission1;
						}break;
					case 2:
						{
							CIE_MissionType = CIE_Mission2;
						}break;
					case 3:
						{
							CIE_MissionType = CIE_Mission3;
						}break;
					case 4:
						{
							CIE_MissionType = CIE_Mission4;
						}break;
					default:
						{
							CIE_MissionType = CIE_Mission;
						}break;
					}
					
					RESULT_SITEM_BAG_LIST_CONT	kConRetItem;
					if( rkMissionUser.kRank >= MRANK_F ) 
					{
						for(int i=0; i<ms_iRewardItemMax; ++i)
						{
							PgBase_Item kItem;
							if(S_OK == CreateSItem(iResultItem, 1, PgItemRarityUpgradeFormula::GetItemRarityContorolType(CIE_MissionType), kItem))
							{
								kConRetItem.push_back(kItem);
							}
						}
						kConRetItem.swap(rkMissionUser.kContItem);
						kConRetItem.clear();
					}
					else
					{
						int iResultRewardItemCount = iRetCount + GetAddDropItemCount(pkUser);

						if( ms_iRewardItemMax < iResultRewardItemCount )
						{
							iResultRewardItemCount = ms_iRewardItemMax;

							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Mission ResultItem Reward Count Error! MissionNo[") << GetMissionNo() << _T("] GroundNo[") << GetGroundNo() << _T("]") );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
						}

						PgAction_PopItemContainer kTempPopItem( CIE_MissionType, GroundKey(), iRetContNo, GetEventAbil() );
						kTempPopItem.DoAction(pkUser, iResultRewardItemCount, false);						

						if( ms_iRewardItemMax < kTempPopItem.ConRetItemNo().size() )
						{
							// ���� ������ 3~4��, �Ӵ��� 1�� �ִ� 4�� �̻� �� ���� ����.
							iResultRewardItemCount = ms_iRewardItemMax - 1;
						}

						PgAction_PopItemContainer kPopItem( CIE_MissionType, GroundKey(), iRetContNo, GetEventAbil() );
						kPopItem.DoAction(pkUser, iResultRewardItemCount, true);

						kPopItem.SwapRetItem(rkMissionUser.kContItem);


						if( 0 == rkMissionUser.kContItem.size() )
						{
							// ��� �������� ���ٸ�... "��"���� ó���� ������
							for(int i=0; i<iResultRewardItemCount; ++i)
							{
								PgBase_Item kItem;
								if(S_OK == CreateSItem(iResultItem, 1, PgItemRarityUpgradeFormula::GetItemRarityContorolType(CIE_MissionType), kItem))
								{
									kConRetItem.push_back(kItem);
								}
							}
							kConRetItem.swap(rkMissionUser.kContItem);
							kConRetItem.clear();
						}
					}

					rkMissionUser.iRetContNo = iRetContNo;

					if( 0 != m_kMissionResult.iRouletteBagGrpNo ) // �����ֱ� ������ S Rank ������ ���� ��´�.
					{
						rkMissionUser.iResultBagNo = m_kMissionResult.iRouletteBagGrpNo;
					}
					else
					{
						CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Mission ResultNo[") << m_kMissionResult.iResultNo << _T("] is RouletteBagGroupNo is 0"));

						CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;
						//GET_DEF(CItemBagMgr, kItemBagMgr);
						g_kTblDataMgr.GetContDef(pContContainer);
						int const iSRankMissionResultID = m_kMissionResult.aiResultContainer[MRANK_S-1];
						CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find( iSRankMissionResultID );
						if( pContContainer->end() != item_cont_itor )
						{
							size_t iRetIDX = 0;
							if(::RouletteRate((*item_cont_itor).second.iSuccessRateControlNo, iRetIDX, MAX_ITEM_CONTAINER_LIST))
							{
								if( 0 == iRetIDX )
								{
									// 0�� �ε����� ��ĭ¥�� ������ ���̴� (���� �ٲ�� ���� �� �� ����)
									rkMissionUser.iResultBagNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX+1];
								}

								if( 0 == rkMissionUser.iResultBagNo )
								{
									// ������ ���õ� 1�� IDX�� ���ȣ�� ������ ���� ������ ���ȣ�� �����´�.
									rkMissionUser.iResultBagNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
								}
							}
						}
					}
				}

				PgAction_QuestMissionUtil::ProcessQuestMission(GetModeType(), GroundKey(), GetMissionKey(), pkUser, rkMissionUser.kRank, GetStageCountExceptBonus(), m_iNowStage);

				// Log
				PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_End);				
				kLogCont.MemberKey(pkUser->GetMemberGUID());
				kLogCont.CharacterKey(pkUser->GetID());
				kLogCont.ID(pkUser->MemberID());
				kLogCont.UID(pkUser->UID());
				kLogCont.Name(pkUser->Name());
				kLogCont.ChannelNo( pkUser->GetChannel() );
				kLogCont.Class(static_cast<short>(pkUser->GetAbil(AT_CLASS)));
				kLogCont.Level(static_cast<short>(pkUser->GetAbil(AT_LEVEL)));
				kLogCont.GroundNo( GetGroundNo() );

				PgLog kLog(ELOrderMain_Mission, ELOrderSub_End);
				kLog.Set( 0, static_cast<int>(1));// ������� ����
				kLog.Set( 1, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(pkUser->PartyGuid())) );
				kLog.Set( 2, static_cast<int>(rkMissionUser.kPoint[MPOINT_BONUSEXP]));
				kLog.Set( 3, static_cast<int>(GetLevel()+1) );
				kLog.Set( 0, pkUser->GetAbil64(AT_EXPERIENCE) );// i64Value1
				kLog.Set( 2, pkUser->PartyGuid().str() );	// guidValue 1
				kLog.Set( 3, this->GroundKey().Guid().str() );// guidValue 2
				kLog.Set( 0, PgMission_Base::GetMissionRankName(rkMissionUser.kRank) );
				kLogCont.Add(kLog);
				kLogCont.Commit();
				// Log End
			}
		}
	}
	SGroundKey kGndKey;
	GetGroundKey(kGndKey);
	kNPacket.Push(kGndKey);
	WriteToPacket_UserList(kNPacket,true);

	SendToMissionMgr(kNPacket);
}


void PgMissionGround::SetState( EIndunState const eState, bool bAutoChange, bool bChangeOnlyState )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	if ( eState == m_eState)
	{
		return;
	}

	m_dwAutoStateRemainTime = 0;
	m_eState = eState;

	switch( m_eState )
	{
	case INDUN_STATE_OPEN:
		{
			SetAutoNextState(md_dwMaxOpenWaitingTime);//�߿�
		}break;
	case INDUN_STATE_WAIT:
		{
			SetAutoNextState(PgIndun::ms_dwMaxWaitUserWaitngTime);
		}break;
	case INDUN_STATE_READY:
		{
            if(IsDefenceGameMode())
            {
                StopAI();
            }

			TunningLevel( GetTunningLevel() );
			m_kConstellationResult.State(PgConstellationResult::ER_STAND_BY, this);
			
			//�̼� TunningNo ���
			const CONT_DEF_MISSION_ROOT* pContDefMission = NULL;
			g_kTblDataMgr.GetContDef(pContDefMission);

			const CONT_DEF_MISSION_CANDIDATE* pContDefMissionCandi = NULL;
			g_kTblDataMgr.GetContDef(pContDefMissionCandi);
			if(pContDefMission && pContDefMissionCandi)
			{
				CONT_DEF_MISSION_ROOT::const_iterator root_itr = pContDefMission->find(GetMissionNo());
				if (pContDefMission->end() != root_itr)
				{
					const TBL_DEF_MISSION_ROOT& element = (*root_itr).second;
					int iLevel = 0;
					if( GetLevel() < MAX_MISSION_LEVEL )
					{
						iLevel = element.aiLevel[std::max(GetLevel(),0)];
					}
					else if(GetLevel()+1 == DEFENCE_MISSION_LEVEL)
					{
						iLevel = element.iDefence;
					}

					if(iLevel)
					{
						CONT_DEF_MISSION_CANDIDATE::const_iterator candi_itr = pContDefMissionCandi->find(iLevel);
						if ( candi_itr!=pContDefMissionCandi->end() )
						{
							m_iGroundTunningNo = (*candi_itr).second.iMissionTunningNo;
						}
					}
				}
			}

			// ������ ���ڿ� ���� ���� ���̵��� ����
			if ( 0 == m_iNowStage )
			{
				// �߾ȿ� ���� ������ �ݿ��ҷ��� ������ '0 == m_iNowStage'�� ���ָ� �ȴ�!!
				GroundWeight( PgMission::m_kConUser.size() - 1 );
				ActivateMonsterGenGroup( 0, true, true, 0 );
			}

			bool const bNotRegen = !IsLastStage();
			ActivateMonsterGenGroup( -1, false, bNotRegen, m_iGroundWeight );
			ActivateMonsterGenGroup( m_iGroundWeight+1, false, bNotRegen, 0 );

			PgGround::OnActivateEventMonsterGroup();

			MapLoadComplete();

			PgMissionInfo const * pMissionInfo = NULL;
			bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pMissionInfo );
			if( bRet )
			{
				if( true == IsDefenceMode7()  && pMissionInfo->GetHaveArcadeMode() )
				{	// ���� ���潺��� �Ƿε��� ���� �߰� ����ġ ������ ����ؾ� ��.
					CalcBonusExpDefence7();
				}
			}

			if( true == IsDefenceGameMode() )
			{
				// Defence Mode
				/*if( IsDefenceMode7() || IsDefenceMode8() )
				{
					SetDefenceItemList();
				}*/
				ObjectUnitGenerate(m_kContGenPoint_Object);
				NextStageSend();
				ObjectCreate();

				DefenceNextStage(0, 0);

				ConUser::iterator user_itor;
				for( user_itor = m_kConUser.begin() ; user_itor != m_kConUser.end() ; ++user_itor )
				{
					PgPlayer * pPlayer = GetUser(user_itor->first);
					if( pPlayer )
					{
						// defence ����� ��� �� ����� �������־�� �ɼ� �������� �����ϴ� ��ƾ�� Ÿ�� ����.
						pPlayer->SetAbil(AT_IGNORE_PVP_MODE_ABIL, 1, true);
					}
				}
			}
			else
			{
				SetState(INDUN_STATE_PLAY);
			}
		}break;
	case INDUN_STATE_PLAY:
		{
			if( 0 == m_iNowStage )
			{
				//CUnit *pkUnit = NULL;// ù��° �ʿ����� �̼� ����Ʈ ���� ����
				//CONT_OBJECT_MGR_UNIT::iterator unit_iter;
				//PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
				//while( NULL != (pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)))
				//{
				//	PgAction_RemoveMissionQuest kRemoveMissionQuest;//����, ���� �̼� ����Ʈ���� ����
				//	kRemoveMissionQuest.DoAction(pkUnit, NULL);

				//	CheckMissionQuestCard( dynamic_cast<PgPlayer*>(pkUnit) );
				//}
				//////////////////////////////////////////////////
				// Log
				ConUser::iterator user_itr;
				for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
				{
					PgPlayer* pkUser = GetUser(user_itr->first);
					if ( pkUser )
					{						
						PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_Start);				
						kLogCont.MemberKey(pkUser->GetMemberGUID());
						kLogCont.CharacterKey(pkUser->GetID());
						kLogCont.UID(pkUser->UID());
						kLogCont.ID(pkUser->MemberID());
						kLogCont.Name(pkUser->Name());
						kLogCont.ChannelNo( pkUser->GetChannel() );
						kLogCont.Class(static_cast<short>(pkUser->GetAbil(AT_CLASS)));
						kLogCont.Level(static_cast<short>(pkUser->GetAbil(AT_LEVEL)));
						kLogCont.GroundNo( GetGroundNo() );

						PgLog kLog(ELOrderMain_Mission, ELOrderSub_Start);
						kLog.Set(0, static_cast<int>(this->GetAttr()) );
						kLog.Set(1, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(pkUser->PartyGuid())) );
						kLog.Set(2, static_cast<int>(GetLevel()+1) );
						kLog.Set(0, static_cast<__int64>(pkUser->GetAbil(AT_STRATEGY_FATIGABILITY)) );
						kLog.Set(2, pkUser->PartyGuid().str() );
						kLog.Set(3, this->GroundKey().Guid().str() );

						kLogCont.Add(kLog);
						kLogCont.Commit();
					}
				}
				// Log End
				// Chapter Option Check //
				int m_iMin = 0;
				int m_iSec = 0;

				if( m_iModeType == MO_SCENARIO )
				{
					m_bTimeAttack = MissionOptionChapterCheck(MO_SCENARIO_TIMEATTACK);
					if( m_bTimeAttack )
					{
						bool bRet = GetOptionTime(m_iMin, m_iSec);
						if( !bRet )
						{
							INFO_LOG( BM::LOG_LV0, __FL__ << _T("Get MissionTime is Fail : MissionNo[") << GetMissionNo() << _T("]") );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetOptionTime Failed!"));
						}
						m_dwTotalTime = GetPlayTime() + (m_iSec*1000) + (m_iMin*60*1000);
					}
				}
				else if( true == IsDefenceGameMode() )
				{
					// Defence Mode
				}
				else
				{
					// ���ѽð� üũ(������ ���)
					bool bRet = GetOptionItemTime(GetLevel(), m_iMin, m_iSec);
					if( !bRet )
					{
						INFO_LOG( BM::LOG_LV0, __FL__ << _T("Get MissionTime is Fail") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetOptionItemTime Failed!"));
					}
					m_dwTotalTime = GetPlayTime() + (m_iSec*1000) + (m_iMin*60*1000);
				}
				//////////////////////////////////////////////////
			}
			else
			{
				AddPlayTime(DifftimeGetTime(m_dwStartTime,BM::GetTime32()));

				// �ó����� Ÿ�Ӿ����� �ƴϸ鼭 5���� ������ ����
				if( (true == IsLastStage()) && (false == m_bTimeAttack) && (MISSION_LEVEL_MAX > (GetLevel()+1)) )
				{
					m_dwStartTimeLimit = BM::GetTime32();

					m_dwTotalTimeLimit = (GetPartyTimeLimit()*60*1000);;
					BM::Stream kCPacket(PT_M_C_NFY_MISSION_TIME_LIMIT);
					kCPacket.Push(static_cast<int>(m_dwTotalTimeLimit));
					Broadcast(kCPacket);
				}
				else if( (false == IsLastStage()) && (false == m_bTimeAttack) && (MISSION_ITEM_TIME > (GetLevel()+1) && IsBonusStage()) )
				{
					m_dwStartTimeLimit = BM::GetTime32();

					m_dwTotalTimeLimit = static_cast<DWORD>(GetBonusStageTime());
					BM::Stream kCPacket(PT_M_C_NFY_MISSION_BONUS_TIME_LIMIT);
					kCPacket.Push(static_cast<int>(m_dwTotalTimeLimit));
					Broadcast(kCPacket);
				}
			}

			m_dwStartTime = BM::GetTime32();

			size_t iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );

			// ��ü ���� ���� ����
			m_kMonsterTotalCount += iLiveMonsterCount;

			//�̼� �ñ׳� ����
			SendMissionState();
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{
			ConstellationResultInit();
			//m_kConstellationResult.SetDelayTime(2);

			//////////////////////////////////////////
			if( m_iModeType == MO_SCENARIO && m_bTimeAttack )
			{
				int iScriptNo = 0;
				if( m_iEleite == 0 )	// ������ ���׾�����... ���� ó�� ����
				{
					if( m_dwTotalTime <= (GetPlayTime()+DifftimeGetTime(m_dwStartTime,BM::GetTime32())) )
					{
						bool bRet = GetOptionScript(iScriptNo);
						if( !bRet )
						{
							INFO_LOG( BM::LOG_LV0, __FL__ << _T("Get MissionScript No is Fail : MissionNo[") << GetMissionNo() << _T("]") );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetOptionScript Failed!"));
						}
					}
				}

				BM::Stream kCPacket(PT_N_C_NFY_MISSION_CLOSETIMER);
				kCPacket.Push(iScriptNo);
				Broadcast(kCPacket);
			}
			//else if( m_iModeType == MO_ITEM )
			{
				BM::Stream kCPacket(PT_N_C_NFY_MISSION_CLOSESCORE);
				kCPacket.Push(true);
				Broadcast(kCPacket);
			}

			// �ó�����, �����̵� ��� ����
			if( (true == IsLastStage()) && (false == m_bTimeAttack) && (6 > (GetLevel()+1)) )
			{
				if( m_dwTotalTimeLimit <= (DifftimeGetTime(m_dwStartTimeLimit,BM::GetTime32())) )
				{
					// TimeLimit Fail Event Number!
					int const iScriptNo = 27;					

					BM::Stream kCPacket(PT_N_C_NFY_MISSION_CLOSETIMER);
					kCPacket.Push(iScriptNo);
					Broadcast(kCPacket);
				}
			}

			m_dwStartTimeLimit = BM::GetTime32();
			m_dwStartTime = BM::GetTime32();
			SetAutoNextState(GetResultWaitTime());
		}break;
	case INDUN_STATE_RESULT:
		{
			EndMission_Before();
		//	EndMission();
		}break;
	case INDUN_STATE_CLOSE:
		{
			if(GetConstellationKey().PartyGuid.IsNotNull())
			{
				PgPlayer* pkPlayer = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
				{
					this->RecvRecentMapMove(pkPlayer);
				}
			}
		}break;
	case INDUN_STATE_FAIL:
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	}
}


//bool PgMissionGround::CheckMissionQuestCard(PgPlayer *pkPlayer)
//{
//	if( !pkPlayer )
//	{
//		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
//		return false;
//	}
//
//	PgInventory *pkInven = pkPlayer->GetInven();
//	if( !pkInven )
//	{
//		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
//		return false;
//	}
//
//	ContHaveItemNoCount kItemMap;
//	if( S_OK != pkInven->GetItems(IT_CONSUME, kItemMap) )
//	{
//		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
//		return false;
//	}
//
//	ContHaveItemNoCount::iterator item_iter = kItemMap.begin();
//	while(kItemMap.end() != item_iter)
//	{
//		const ContHaveItemNoCount::key_type &rkItemNo = (*item_iter).first;
//		//const ContHaveItemNoCount::mapped_type &rkItemCount = (*item_iter).second;
//		bool const bIsMissionQuestItem = g_kMissionQuestMng.IsCanUseMissionCard( SMissionQuestKey(rkItemNo, GetMissionKey().iKey) );
//		if( !bIsMissionQuestItem )
//		{
//			item_iter = kItemMap.erase(item_iter);
//		}
//		else
//		{
//			++item_iter;
//		}
//	}
//
//	//////////////////////////////////////////////////
//	// �̼� ������ ��� �˸� �޽��� ��� �κ�
//	/*SMissionObject m_kObjectData;
//	m_kObjectData.Clear();
//	bool bRet = GetOptionText(GetLevel(), m_kObjectData);
//	if( !bRet )
//	{
//		INFO_LOG(BM::LOG_LV1,_T("[%s] Get MissionInfo is Fail : MissionNo[%d]"),__FUNCTIONW__, GetMissionNo());
//	}*/
//	//////////////////////////////////////////////////
//
//	BM::Stream kNfyPacket(PT_M_C_ANS_MISSION_QUEST);//�̼� ����Ʈ ���� ������ ����
//	kNfyPacket.Push((BYTE)MQC_Nfy_CardItem);
//	kNfyPacket.Push(kItemMap);
//	kNfyPacket.Push(GetModeType());
//	//kNfyPacket.Push(m_kObjectData);
//	pkPlayer->Send(kNfyPacket);
//	return true;
//}

bool PgMissionGround::GetOptionTime(int& iMin, int& iSec)
{
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{
		iMin = pkMissionInfo->GetTimeMin();
		iSec = pkMissionInfo->GetTimeSec();
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgMissionGround::GetOptionItemTime(int iLevel, int& iMin, int& iSec)
{
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{
		iMin = pkMissionInfo->GetItemTimeMin(iLevel);
		iSec = pkMissionInfo->GetItemTimeSec(iLevel);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgMissionGround::GetOptionScript(int& iScript)
{
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{
		iScript = pkMissionInfo->GetScript();
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgMissionGround::GetOptionErrorText(int& iErrorTTW)
{
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{
		iErrorTTW = pkMissionInfo->GetErrorText();
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgMissionGround::GetGadaCoinCount(int& iGadaCoinCount)
{
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( bRet )
	{
		iGadaCoinCount = pkMissionInfo->GetGadaCoinCount();
	}
	else
	{
		// �⺻���� 1 ����
		iGadaCoinCount = 1;

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

/*
bool PgMissionGround::GetOptionText(int iLevel, SMissionObject& pkOut)
{
	int iCount = 0;

	PgMissionInfo* pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission(GetMissionNo(), pkMissionInfo);
	if( bRet )
	{
		const ContMissionOptionChapter* rkMissionOptionChapter;
		SGroundKey kGndKey;
		GetGroundKey(kGndKey);
		int const iGroundNo = kGndKey.GroundNo();

		bool bRet = pkMissionInfo->SetMissionOptionChapter(iGroundNo);
		if( bRet )
		{
			if( pkMissionInfo->GetMissionOptionChapter(iLevel, rkMissionOptionChapter) )
			{
				if( !(iCount > MISSION_PARAMNUM) ) 
				{
					ContMissionOptionChapter::const_iterator iter = rkMissionOptionChapter->begin();
					while( rkMissionOptionChapter->end() != iter )
					{
						pkOut.SetObject(iCount++, (*iter).iObjectTextNo, (*iter).iCount);
						++iter;
					}
				}
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}
*/

//bool PgMissionGround::ProcessMissionQuestCmd(EMissionQuestCommand eCmd, BM::Stream &rkPacket, CUnit *pkUnit)
//{
//	if( !pkUnit )
//	{
//		return false;
//	}
//
//	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkUnit);
//	if( !pkPC )
//	{
//		return false;
//	}
//
//	PgInventory *pkInven = pkPC->GetInven();
//	if( !pkInven )
//	{
//		return false;
//	}
//
//	switch(eCmd)
//	{
//	case MQC_Req_SelectCard:
//		{
//			if( 0 != m_iNowStage )
//			{
//				break;//ù��° �� �ƴϸ� �ȵ�
//			}
//
//			int iItemNo = 0;
//			
//			rkPacket.Pop(iItemNo);
//			if( iItemNo )//���� �ߴ�
//			{
//				//���⼭ ����Ʈ �ϳ� ����
//				ContMissionQuestVec kOutVec;
//				SMissionQuestKey kMissionQuestKey(iItemNo, GetMissionKey().iKey);
//				bool const bRet = g_kMissionQuestMng.GetMissionQuest(kMissionQuestKey, kOutVec);
//				if( !bRet )
//				{
//					//������ ���� ������ ����Ʈ�� �����ϴ�.
//					BM::Stream kAnsPacket(PT_M_C_ANS_MISSION_QUEST, (BYTE)MQC_Ans_SelectCard);
//					kAnsPacket.Push((int)MQCR_Failed);
//					pkUnit->Send(kAnsPacket);
//				}
//				else
//				{
//					ContMissionQuestVec kNewVec;
//					kNewVec.reserve(kOutVec.size());
//
//					ContMissionQuestVec::iterator quest_iter = kOutVec.begin();
//					while(kOutVec.end() != quest_iter)
//					{
//						const ContMissionQuestVec::value_type &rkElement = (*quest_iter);
//						PgCheckQuestBegin kCheckQuest(rkElement.iQuestID);//���� �������� üũ
//						if( kCheckQuest.DoAction(pkUnit, NULL) )//���� �����ϸ�
//						{
//							kNewVec.push_back(rkElement);
//						}
//						++quest_iter;
//					}
//
//					if( kNewVec.empty() )
//					{
//						BM::Stream kAnsPacket(PT_M_C_ANS_MISSION_QUEST, (BYTE)MQC_Ans_SelectCard);
//						kAnsPacket.Push((int)MQCR_NoQuest);
//						pkUnit->Send(kAnsPacket);
//						break;
//					}
//
//					size_t const iCur = BM::Rand_Index(kNewVec.size());
//					const ContMissionQuestVec::value_type &rkCurMissionQuest = kNewVec[iCur];//����!
//
//					BM::Stream kAddonPacket(PT_M_I_REQ_START_MISSION_QUEST);
//					rkCurMissionQuest.WriteToPacket(kAddonPacket);//�� ����Ʈ�� ���� ���Ѷ�
//
//					CONT_PLAYER_MODIFY_ORDER kOrderList;
//					tagPlayerModifyOrderData_Add_Any kDelData(iItemNo, -1);//������ ������ �Ҹ�
//					SPMO kIMO(IMET_ADD_ANY, pkPC->GetID(), kDelData);
//					kOrderList.push_back(kIMO);
//
//					PgAction_ReqModifyItem kAction(CIE_MissionQuestCard, GroundKey(), kOrderList, kAddonPacket);
//					kAction.DoAction(pkPC, NULL);
//				}
//			}
//		}break;
//	case MQC_Nfy_CardItem://�̷��͵� ���� �ȵ�
//	case MQC_Ans_StartQuest:
//	case MQC_Ans_QuestResult:
//	default:
//		{
//			assert(false);
//			return false;
//		}break;
//	}
//
//	return true;
//}

void PgMissionGround::EndMission_Before()
{
	VEC_GUID kGuidVec;
	ConUser::const_iterator itor_user = m_kConUser.begin();
	while (itor_user != m_kConUser.end())
	{
		kGuidVec.push_back(itor_user->first);
		++itor_user;
	}

	bool bEventMap = false;
	int const iTriggerType = static_cast<int>(PgMission::GetTriggerType());
	if( (MT_EVENT_MISSION == static_cast<EMissionTypeKind>(iTriggerType)) || (MT_EVENT_HIDDEN == static_cast<EMissionTypeKind>(iTriggerType)) )
	{
		bEventMap = true;
	}

	int iValue = 0;

	if( (true == bEventMap) && (MT_EVENT_HIDDEN == static_cast<EMissionTypeKind>(iTriggerType)) &&  (m_iModeType == MO_SCENARIO) )
	{
		// �̺�Ʈ �ʿ����� ���� ���� Ŭ���� �� ��� "SSS"��� ���� �ʱ�ȭ
		iValue = 0;

		VEC_GUID::const_iterator itor_guid = kGuidVec.begin();
		while (itor_guid != kGuidVec.end())
		{
			CONT_PLAYER_MODIFY_ORDER kEventOrder;
			SPMOD_AddMissionEvent kMissionEventUpdate(E_MISSION_EVENT_RESET, iValue);
			SPMO kIMOEVENT(IMET_END_MISSION_EVENT, (*itor_guid), kMissionEventUpdate);
			kEventOrder.push_back(kIMOEVENT);

			BM::Stream kPacket;
			kPacket.Push(static_cast<int>(E_MISSION_EVENT_RESET));
			kPacket.Push(iValue);

			PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_Event, GroundKey(), kEventOrder, kPacket, true);
			kItemModifyAction.DoAction(NULL, NULL);

			++itor_guid;
		}
		return;
	}

	if( m_iModeType != MO_ITEM )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	if( true == bEventMap )
	{
		// ���� ��ŷ���� �� ���� ��Ƽ������ "SSS" ��� �Ϸ� ������ �޾Ƶ� �ȴ�.		
		int const iMissionLevel = (GetLevel());
		if( MRANK_SSS == GetMissionRank(false) )
		{				
			if( MAX_MISSION_LEVEL > iMissionLevel )
			{
				iValue = (0x0001 << iMissionLevel);
				if( 0x000F > iValue )
				{
					VEC_GUID::const_iterator itor_guid = kGuidVec.begin();
					while (itor_guid != kGuidVec.end())
					{
						CONT_PLAYER_MODIFY_ORDER kEventOrder;
						SPMOD_AddMissionEvent kMissionEventUpdate(E_MISSION_EVENT_SET, iValue);
						SPMO kIMOEVENT(IMET_END_MISSION_EVENT, (*itor_guid), kMissionEventUpdate);
						kEventOrder.push_back(kIMOEVENT);

						BM::Stream kPacket;
						kPacket.Push(static_cast<int>(E_MISSION_EVENT_SET));
						kPacket.Push(iValue);

						PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_Event, GroundKey(), kEventOrder, kPacket, true);
						kItemModifyAction.DoAction(NULL, NULL);

						++itor_guid;
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	CONT_PLAYER_MODIFY_ORDER kOrder;
	MissionReport_Update kMissionUpdate(true, kGuidVec, GetMissionKey(), 0x00);
	SPMO kIMO(IMET_END_MISSION, BM::GUID::NullData(), kMissionUpdate);
	kOrder.push_back(kIMO);

	PgAction_ReqModifyItem kItemModifyAction(CIE_MissionUpdate, GroundKey(), kOrder, BM::Stream(), true);//�̼� ������ �׾� �ִ���. ���� �ǵ���.
	kItemModifyAction.DoAction(NULL, NULL);
}

bool PgMissionGround::SwapStage_Before( size_t const iStage, CUnit *pkUnit )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	if ( m_eState != INDUN_STATE_PLAY )
	{
		// �÷��� ���°� �ƴϸ� SwapStage�Ұ�
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("SwapStage Failed State[") << m_eState << _T("] Ground[") << GroundKey().GroundNo() << _T("-") << GroundKey().Guid().str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( m_iNowStage == iStage )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if ( iStage >= m_kGndResounrce.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( pkUnit )
	{
		if ( NULL == PgMission::GetMissionUser( pkUnit->GetID() ) )
		{
			// �̼� ������ �ƴϸ� �̵���û �Ұ�
			return false;
		}
		/*if ( !CheckMonsterInMap() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}*/

		///////////////////////////////////////////////
		// ���� CheckMonsterInMap() �ּ� ó�� MissionOptionChapterCheck() ���� �ߺ� ó���Ѵ�.
		//if( GetPortalMissionState() != PORTAL_NEXT_STAGE )
		//{
		//	return false;
		//}
		if( !MissionOptionChapterCheck(MO_CHAPTER) ) 
		{			
			return false;
		}			
		//if( !GetPortalMissionState() )
		//{				
		//	Broadcast(BM::Stream(PT_M_C_NFY_REJECT_STAGE_MAP_MOVE,193));
		//	return false;
		//}		
		///////////////////////////////////////////////

		// ��Ƽ���� ������ �̵��� �� �� ����.
		if( !CheckPlayerDie( pkUnit ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		// �̼ǿ��� ��Ƽ���� ��ġ üũ ����
		/*
		if ( !CheckPlayerInArea( pkUnit ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		*/
	}

	VEC_GUID kGuidVec;
	ConUser::const_iterator itor_user = m_kConUser.begin();
	while (itor_user != m_kConUser.end())
	{
		BM::GUID const kCharGuid = itor_user->first;
		CUnit* pkAbilUnit = PgObjectMgr::GetUnit(kCharGuid);//�� �׶��忡 �ִ°�.
		if(pkAbilUnit)
		{
			{// ���� ���¿�
				CONT_PLAYER_MODIFY_ORDER kEventOrder;
				int const iValue = pkAbilUnit->GetAbil(AT_AWAKE_STATE);
				SPMOD_AddMissionEvent kMissionEventUpdate(E_MISSION_ABIL_AWAKE_STATE, iValue);
				SPMO kIMOEVENT(IMET_END_MISSION_EVENT, (kCharGuid), kMissionEventUpdate);
				kEventOrder.push_back(kIMOEVENT);

				BM::Stream kPacket;
				kPacket.Push(static_cast<int>(E_MISSION_ABIL_AWAKE_STATE));
				kPacket.Push(iValue);

				PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_Event, GroundKey(), kEventOrder, kPacket, true);
				kItemModifyAction.DoAction(NULL, NULL);
			}

			{// ������ ��ġ ���� ������
				CONT_PLAYER_MODIFY_ORDER kEventOrder;
				int const iValue = pkAbilUnit->GetAbil(AT_AWAKE_VALUE);
				SPMOD_AddMissionEvent kMissionEventUpdate(E_MISSION_ABIL_AWAKE_VALUE, iValue);
				SPMO kIMOEVENT(IMET_END_MISSION_EVENT, (kCharGuid), kMissionEventUpdate);
				kEventOrder.push_back(kIMOEVENT);

				BM::Stream kPacket;
				kPacket.Push(static_cast<int>(E_MISSION_ABIL_AWAKE_VALUE));
				kPacket.Push(iValue);

				PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_Event, GroundKey(), kEventOrder, kPacket, true);
				kItemModifyAction.DoAction(NULL, NULL);
			}
		}

		kGuidVec.push_back(itor_user->first);
		++itor_user;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	MissionReport_Update kMissionUpdate(iStage != 0, kGuidVec, GetMissionKey(), GetStageBitFlag(m_iNowStage));	// iStage==0�϶��� Update�ϸ� �ȵȴ�.
	SPMO kIMO(IMET_SWAP_MISSION, BM::GUID::NullData(), kMissionUpdate);
	kOrder.push_back(kIMO);
	BM::Stream kAddon;
	kAddon.Push(iStage);
	PgAction_ReqModifyItem kItemModifyAction(CIE_MissionUpdate, GroundKey(), kOrder, kAddon, true);//�̼� �������� �Ѿ�� ����� ���� ���� ������Ʈ.
	kItemModifyAction.DoAction(NULL, NULL);
	return true;
}

void PgMissionGround::SetGMMissionScore(int iScore)
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if( iScore < 0 ) 
	{
		iScore = 0;
	}
	else if( iScore > MISSION_TOTALSCORE_MAX )
	{
		iScore = MISSION_TOTALSCORE_MAX;
	}

	GMState(true);
	GMScore(iScore);
}

int const PgMissionGround::CalcClass(int iClass)
{
	int iClassValue = 1;

	if( UCLASS_THIEF >= iClass )
	{
		// 1�� Ŭ����
		return iClass;
	}
	else if( UCLASS_ASSASSIN >= iClass )
	{
		// 1/2�� Ŭ����
		iClassValue = (BM::Rand_Index(2) == 0 ? iClass : 0);
	}
	else if( UCLASS_SHADOW >= iClass )
	{
		// 3�� Ŭ����
		iClassValue = (BM::Rand_Index(3) == 0 ? iClass : 0);

		if( 0 == iClassValue )
		{
			// 1/2�� Ŭ����
			iClassValue = (BM::Rand_Index(2) == 0 ? (iClass-8) : 0);
		}
	}
	else if( UCLASS_VOCAL >= iClass )
	{
		// 4�� Ŭ����
		iClassValue = (BM::Rand_Index(4) == 0 ? iClass : 0);

		if( 0 == iClassValue )
		{
			// 1/2/3�� Ŭ����
			iClassValue = (BM::Rand_Index(3) == 0 ? (iClass-8) : 0);
			
			if( 0 == iClassValue )
			{
				// 1/2�� Ŭ����
				iClassValue = (BM::Rand_Index(2) == 0 ? (iClass-16) : 0);
			}
		}
	}
	else if( UCLASS_DOUBLE_FIGHTER >= iClass )
	{
		// ������ 1�� Ŭ����.
		return iClass;
	}
	else if( UCLASS_TWINS >= iClass )
	{
		// ������ 1/2�� Ŭ����.
		iClassValue = (BM::Rand_Index(2) == 0 ? iClass : 0);
	}
	else if( UCLASS_MIRAGE >= iClass )
	{
		// ������ 3�� Ŭ����.
		iClassValue = (BM::Rand_Index(3) == 0 ? iClass : 0);
		if( 0 == iClassValue )
		{
			// ������ 1/2�� Ŭ����.
			iClassValue = (BM::Rand_Index(2) == 0 ? (iClass - 2) : 0);
		}
	}
	else if( UCLASS_DRAGON_FIGHTER >= iClass )
	{
		// ������ 4�� Ŭ����.
		iClassValue = (BM::Rand_Index(4) == 0 ? iClass : 0);
		if( 0 == iClassValue )
		{
			// ������ 1/2/3�� Ŭ����.
			iClassValue = (BM::Rand_Index(3) == 0 ? (iClass - 2) : 0);
			if( 0 == iClassValue )
			{
				// ������ 1/2�� Ŭ����.
				iClassValue = (BM::Rand_Index(2) == 0 ? (iClass - 4) : 0);
			}
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Class Data is not Find") );
	}

	if( 0 > iClassValue )
	{
		iClassValue = 1;
	}

	if( 0 == iClassValue )
	{
		// 1�� Ŭ����
		int iClassLimit = (INT64_1 << iClass);

		if(iClassLimit & UCLIMIT_MARKET_FIGHTER)
		{
			return UCLASS_FIGHTER;
		}
		else if(iClassLimit & UCLIMIT_MARKET_MAGICIAN)
		{
			return UCLASS_MAGICIAN;
		}
		else if(iClassLimit & UCLIMIT_MARKET_ARCHER)
		{
			return UCLASS_ARCHER;
		}
		else if(iClassLimit & UCLIMIT_MARKET_THIEF)
		{
			return UCLASS_THIEF;
		}
		else if( iClassLimit & UCLIMIT_MARKET_SHAMAN)
		{
			return UCLASS_SHAMAN;
		}
		else if( iClassLimit & UCLIMIT_MARKET_DOUBLE_FIGHTER )
		{
			return UCLASS_DOUBLE_FIGHTER;
		}
	}

	return iClassValue;
}

int PgMissionGround::GetTypeScore(EMissionScoreType iType) const
{
	size_t iPartyCount = GetUserCount();

	if( (iPartyCount > 0) && (iPartyCount <= MISSION_SCORE) )
	{
		// �Ҽ��� ȯ������
		switch( iType )
		{
		case EMission_ATTACK:
			{
				// (0.5/0.6/0.8/1)
				return ms_iAttackScore[iPartyCount-1];
			}break;
		case EMission_COMBO:
			{
				return ms_iComboScore[iPartyCount-1];
			}break;
		case EMission_DEMAGE:
			{
				return ms_iDemageScore[iPartyCount-1];
			}break;
		case EMission_TIMESEC:
			{
				return ms_iTimeSec[iPartyCount-1];
			}break;
		case EMission_TIME:
			{
				return ms_iTimeScore[iPartyCount-1];
			}break;
		case EMission_DIE:
			{
				return ms_iDieScore[iPartyCount-1];
			}break;
		case EMission_OVERHIT:
			{
				return ms_iOverHitScore;
			}break;
		case EMission_BACKATTACK:
			{
				return ms_iBackAttack;
			}break;
		case EMission_STYLE:
			{
				return ms_iStyleScore;
			}break;
		case EMission_COUNTER:
			{
				return ms_iCounterScore;
			}
			break;
		default:
			{
			}break;
		}
	}
	return 0;
}

int PgMissionGround::GetScoreCalc(int const iAdd)
{	
	int iValue = (iAdd % MISSION_SCORE_PERSENT);

	iValue = (iValue <= 0) ? 0 : iValue;
	iValue = (iValue >= MISSION_SCORE_PERSENT) ? (iValue - MISSION_SCORE_PERSENT) : iValue;

	if( 0 != iValue )
	{
		iValue = (iValue / MISSION_SCORE_CALC);
	}

	return iValue;
}

void PgMissionGround::AddMissionScore(BM::Stream * const pkPacket)									  
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	EMissionScoreType iType;
	BM::GUID rkCharGuid;

	pkPacket->Pop(iType);
	pkPacket->Pop(rkCharGuid);

	int iAbility = 0;
	ConUser::iterator user_itr = m_kConUser.find(rkCharGuid);
	if( user_itr != m_kConUser.end() )
	{
		PgPlayer* pkUser = GetUser(user_itr->first);
		if( pkUser )
		{
			ConUser::mapped_type& rkMissionUser = (*user_itr).second;

			switch( iType )
			{
			case EMission_OVERHIT:
				{
					rkMissionUser.m_kAbility.kOverHitScore += GetTypeScore(iType);

					BM::GUID rkTargetGuid;
					pkPacket->Pop(rkTargetGuid);

					BM::Stream kPacket(PT_M_C_MISSION_ABILITY_DEMAGE);
					kPacket.Push(iType);
					kPacket.Push(rkTargetGuid);
					Broadcast(kPacket);
				}break;
			case EMission_BACKATTACK:
				{
					rkMissionUser.m_kAbility.kBackAttackScore += GetTypeScore(iType);					

					BM::GUID rkTargetGuid;
					pkPacket->Pop(rkTargetGuid);

					BM::Stream kPacket(PT_M_C_MISSION_ABILITY_DEMAGE);
					kPacket.Push(iType);
					kPacket.Push(rkTargetGuid);
					Broadcast(kPacket);
				}break;
			case EMission_STYLE:
				{
					rkMissionUser.m_kAbility.kStyleScore += GetTypeScore(iType);

					BM::GUID rkTargetGuid;
					pkPacket->Pop(rkTargetGuid);

					BM::Stream kPacket(PT_M_C_MISSION_ABILITY_DEMAGE);
					kPacket.Push(iType);
					kPacket.Push(rkTargetGuid);
					Broadcast(kPacket);
				}break;
			case EMission_COUNTER:
				{
					rkMissionUser.m_kAbility.kCounterScore += GetTypeScore(iType);					
				}break;
			default:
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled Cause[") << iType << _T("]") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
					return;
				}break;
			}
			iAbility = (rkMissionUser.m_kAbility.GetAbilityPoint());
		}
	}
	
	DisplayUpdateUI(EMission_Update_Ability, iAbility, rkCharGuid);
}

void PgMissionGround::DisplayUpdateUI(EMissionUpdateType iType, int iTotal, BM::GUID const& rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	int iUpdateTotal = 0;

	BM::Stream kPacket(PT_M_C_NFY_CHANGE_MISSIONSCORE_COUNT);
	kPacket.Push(iType);

	switch( iType )
	{
	case EMission_Update_Sense:
	case EMission_Update_Ability:
		{
			iUpdateTotal = GetScoreCalc(iTotal);
		}break;
	case EMission_Update_Total:
		{
			iUpdateTotal = (iTotal / MISSION_SCORE_CALC);
			iUpdateTotal = ( iUpdateTotal > MISSION_TOTALSCORE_MAX ) ? MISSION_TOTALSCORE_MAX : iUpdateTotal;
		}break;
	case EMission_Update_Ani:
		{
			// Sense/Ability Score Add
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled Cause[") << iType );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
			return;
		}break;
	}
	
	kPacket.Push(static_cast<int>(iUpdateTotal));
	if( rkCharGuid == BM::GUID::NullData() )
	{
		Broadcast(kPacket);
	}
	else
	{
		PgPlayer* pkPlayer = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
		while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
		{
			if( pkPlayer->GetID() == rkCharGuid )
			{
				pkPlayer->Send(kPacket);
			}
		}
	}
}

void PgMissionGround::Sense_Ability_Sum(int &iTotalScoreUpdate)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	// �׵��� ���� ����(�Ű��� ����)�� ���Ѵ�.
	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		ConUser::iterator user_itr = m_kConUser.find(pkPlayer->GetID());
		if( user_itr != m_kConUser.end() )
		{
			PgPlayer* pkUser = GetUser(user_itr->first);
			if( pkUser )
			{
				ConUser::mapped_type& rkMissionUser = (*user_itr).second;
				// Sense ������ 100% �ѱ�� ���������� �����ϱ�
				int kTotalScore = 0;
				int iSenseAdd = rkMissionUser.m_kSense.GetSensePoint();
				if( (iSenseAdd) >= MISSION_SCORE_PERSENT )
				{
					int iTotalAdd = (iSenseAdd / MISSION_SCORE_PERSENT);

					if( iTotalAdd >= 0 )
					{
						kTotalScore = (iTotalAdd * (ms_TotalScore_TotalUpPersent*MISSION_SCORE_CALC));
					}
				}
				// Ability ������ 100% �ѱ�� ���������� �����ϱ�
				int iAbility = rkMissionUser.m_kAbility.GetAbilityPoint();
				if( (iAbility) >= MISSION_SCORE_PERSENT )
				{
					int iTotalAdd = (iAbility / MISSION_SCORE_PERSENT);

					if( iTotalAdd >= 0 )
					{
						kTotalScore += (iTotalAdd * (ms_TotalScore_TotalUpPersent*MISSION_SCORE_CALC));
					}
				}
				if( 0 >= kTotalScore )
				{
					kTotalScore = 0;
				}
				rkMissionUser.kTotalScore = kTotalScore;
				iTotalScoreUpdate += rkMissionUser.kTotalScore;
			}
		}
	}
}

void PgMissionGround::Sense_Ability_Sub(int &iTotalScoreUpdate)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	// �Ѵ� ������ ��� �׸�ŭ�� �����ش�.
	if( (iTotalScoreUpdate - iTotalOverScore) >= 0 )
	{
		iTotalScoreUpdate -= iTotalOverScore;
	}

	// �׵��� ����� ������ ����ش�.
	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		ConUser::iterator user_itr = m_kConUser.find(pkPlayer->GetID());
		if( user_itr != m_kConUser.end() )
		{
			PgPlayer* pkUser = GetUser(user_itr->first);
			if( pkUser )
			{
				ConUser::mapped_type& rkMissionUser = (*user_itr).second;
				// �ǰ� 
				int iDemageSub = rkMissionUser.m_kPenalty.kDemageScore;
				if( (iTotalScoreUpdate - iDemageSub) >= 0 )
				{
					rkMissionUser.m_kPenalty.kDemageScore = iDemageSub;
					iTotalScoreUpdate -= iDemageSub;
				}
				// ���
				int iDieSub = rkMissionUser.m_kPenalty.kDieScore;
				if( (iTotalScoreUpdate - iDieSub) >= 0 )
				{
					rkMissionUser.m_kPenalty.kDieScore = iDieSub;
					iTotalScoreUpdate -= iDieSub;
				}				
			}
		}
	}
}

bool PgMissionGround::Sense_ValueUpdate(BM::GUID const& rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	bool bChangeAttack = false;	
	bool bChangeCombo = false;
	bool bChangeDemage = false;
	int iComboCountValue = 0;
	PgComboCounter* pkCounter = NULL;

	ConUser::iterator user_itr = m_kConUser.find(rkCharGuid);
	if( user_itr != m_kConUser.end() )
	{
		PgPlayer* pkUser = GetUser(user_itr->first);
		if( pkUser )
		{
			ConUser::mapped_type& rkMissionUser = (*user_itr).second;

			if( GetComboCounter(rkCharGuid, pkCounter) )
			{
				int const iNowAttack = pkCounter->GetComboCount(ECOUNT_TOTAL_HIT);
				if( rkMissionUser.kPoint[MPOINT_ATTACK] < iNowAttack )
				{
					rkMissionUser.kPoint[MPOINT_ATTACK] = iNowAttack;
					rkMissionUser.m_kSense.kAttackScore += GetTypeScore(EMission_ATTACK);
					bChangeAttack = true;
				}

				iComboCountValue = pkCounter->GetComboCount(ECOMBO_TOTAL);

				if( MISSION_COMBO <= iComboCountValue )
				{
					if( rkMissionUser.kPoint[MPOINT_COMBO] != iComboCountValue )
					{
						rkMissionUser.kPoint[MPOINT_COMBO] = iComboCountValue;
						rkMissionUser.m_kSense.kComboScore += GetTypeScore(EMission_COMBO);
						bChangeCombo = true;
					}
				}

				int const iNowDemage = pkCounter->GetComboCount(ECOUNT_TOTAL_DAMAGE);
				if( rkMissionUser.kPoint[MPOINT_DEMAGE] < iNowDemage )
				{
					rkMissionUser.kPoint[MPOINT_DEMAGE] = iNowDemage;
					bChangeDemage = true;

					int const iClass = pkUser->GetAbil(AT_CLASS);
					if( 0 < iClass )
					{
						int iClassLimit = (INT64_1 << iClass);

						if(iClassLimit & UCLIMIT_MARKET_FIGHTER)
						{
							if( true == ClassDemage() )
							{
								// ���� �ݿ��� 2�� ���� ��츸 �ݿ��� �Ѵ�.(��� Ŭ������...)
								ClassDemage(false);
							}
							else
							{
								ClassDemage(true);
								bChangeDemage = false;
							}
						}
					}
				}
			}
			else
			{
				iComboCountValue = 0;
			}

			if( MISSION_COMBO > iComboCountValue )
			{
				rkMissionUser.ResetComboCount();
			}

			if( rkMissionUser.kComboCount <= iComboCountValue )
			{
				rkMissionUser.m_kSense.kComboScore += (MISSION_BONUSSCORE);
				rkMissionUser.kComboCount += (MISSION_COMBO_BONUS);
				bChangeCombo = true;
			}

			// Sense Update
			if( bChangeAttack || bChangeCombo )
			{
				int iSense = (rkMissionUser.m_kSense.GetSensePoint());
				DisplayUpdateUI(EMission_Update_Sense, iSense, rkCharGuid);
			}
		}
	}

	return bChangeDemage;
}

int PgMissionGround::GadaCoin_SendItemCheck(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	ConUser::iterator user_itr = PgMission::m_kConUser.find(rkCharGuid);
	if( user_itr != m_kConUser.end() )
	{
		ConUser::mapped_type& rkMissionUser = (*user_itr).second;

		return rkMissionUser.iRetContNo;
	}
	return 0;
}

void PgMissionGround::GadaCoin_UserCheck(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	ConUser::iterator user_itr = PgMission::m_kConUser.find(rkCharGuid);
	if( user_itr != m_kConUser.end() )
	{
		ConUser::mapped_type& rkMissionUser = (*user_itr).second;

		rkMissionUser.iRetContNo = 0;
	}
}

bool PgMissionGround::GetGadaCoinItemRankCheck(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	ConUser::iterator user_itr = PgMission::m_kConUser.find(rkCharGuid);
	if( user_itr != m_kConUser.end() )
	{
		ConUser::mapped_type& rkMissionUser = (*user_itr).second;
		
		return (MRANK_E > rkMissionUser.kRank);
	}
	return false;
}

void PgMissionGround::GadaCoin_SendItem(CUnit* pkUnit, int iRetContNo)
{
	BM::CAutoMutex kLock(m_kRscMutex);		

	if( !pkUnit )
	{
		return;
	}

	//if( 0 != iRetContNo )
	{
		EItemModifyParentEventType CIE_MissionType = CIE_Mission;
		switch( GetLevel() + 1 )
		{
		case 1:
			{
				CIE_MissionType = CIE_Mission1;
			}break;
		case 2:
			{
				CIE_MissionType = CIE_Mission2;
			}break;
		case 3:
			{
				CIE_MissionType = CIE_Mission3;
			}break;
		case 4:
			{
				CIE_MissionType = CIE_Mission4;
			}break;
		default:
			{
				CIE_MissionType = CIE_Mission;
			}break;
		}

		int iCount = 1;	// 1���� �Һ�

		bool bGadaRet = GetGadaCoinCount(iCount);

		int const iLevel = (GetLevel() + 1);
		
//		int const iClass = CalcClass(pkUnit->GetAbil(AT_CLASS));
		int const iClass = pkUnit->GetAbil(AT_CLASS);

		VEC_INT Vec_Class;
		Vec_Class.push_back(pkUnit->GetAbil(AT_BASE_CLASS));		// �⺻ Ŭ������ ���� ����.

		GetLowClassType(iClass, Vec_Class);						// ��� Ŭ���� ��ȣ�� ����.

		PgBase_Item kItem;
		bool bRet = GetGadaCoinItemResult(iLevel, Vec_Class, 1, kItem); // �������� 1���� �����.
		if( false == bRet )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("GadaCointItem Give Fail") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkItem is NULL"));
			return;
		}

		RESULT_SITEM_BAG_LIST_CONT kContItem;
		kContItem.clear();

		kContItem.push_back(kItem);
		
		/*PgAction_PopItemContainer kPopItem( CIE_MissionType, GroundKey(), iRetContNo, GetEventAbil() );
		kPopItem.DoAction(pkUnit, iCount, false);
		
		RESULT_SITEM_BAG_LIST_CONT kContItem;
		kContItem.clear();

		kPopItem.SwapRetItem(kContItem);*/

		// �������� ����
		size_t const kCoinCount = pkUnit->GetInven()->GetTotalCount(GADA_COIN_NO);
		
		if( iCount <=  kCoinCount )
		{
			CONT_PLAYER_MODIFY_ORDER kOrder;

 			SItemPos kItemPos;
 			if(S_OK == pkUnit->GetInven()->GetFirstItem(IT_CONSUME, GADA_COIN_NO, kItemPos))
			{
 				PgBase_Item kItem;
 				if( S_OK == pkUnit->GetInven()->GetItem(kItemPos, kItem) )
				{
					if( S_OK != PgMission::IsGadaCoinUse(pkUnit->GetID()) )
					{
						return;		// ������ �޾Ҵ�.
					}

					{//�������� ��� ������ ���� ����
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_GADACOIN, iCount, GroundKey() );
						kMA.DoAction( pkUnit, NULL );
					}

					SPMOD_Add_Any kAddItem(GADA_COIN_NO, -iCount);
					SPMO kIMO(IMET_ADD_ANY, pkUnit->GetID(), kAddItem);
					kOrder.push_back(kIMO);

					if( kContItem.size() )
					{
						GadaCoin_UserCheck(pkUnit->GetID());

						SItemPos kItemPos;

						BM::Stream kPacket;
						kContItem.at(0).WriteToPacket( kPacket );
						kPacket.Push(kItemPos);

						PgAction_ReqModifyItem kItemModifyAction(CIE_MissionType, GroundKey(), kOrder, kPacket);
						kItemModifyAction.DoAction(pkUnit, NULL);
					}
				}
			}
		}
		else
		{
			pkUnit->SendWarnMessage(550005);
		}
	}
}

// �ش� Ŭ������ ��� ���� Ŭ���� ��ȣ�� ���Ѵ�.
void PgMissionGround::GetLowClassType(int iClass, VEC_INT & Vec_Class)
{
	if( UCLASS_THIEF >= iClass )
	{
		// 1�� Ŭ������ �ۿ��� ����.
		return;
	}

	if( UCLASS_ASSASSIN >= iClass )
	{
		// 2�� Ŭ����.
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_NINJA >= iClass )
	{
		// 3�� Ŭ����.
		Vec_Class.push_back(iClass - 8);
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_SHADOW >= iClass )
	{
		// 4�� Ŭ����.
		Vec_Class.push_back(iClass - 16);
		Vec_Class.push_back(iClass - 8);
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_DUELIST >= iClass )
	{
		// 5th job
		Vec_Class.push_back(iClass - 24);
		Vec_Class.push_back(iClass - 16);
		Vec_Class.push_back(iClass - 8);
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_DOUBLE_FIGHTER >= iClass)
	{
		// 1�� Ŭ������ �ۿ��� ����.
		return;
	}

	if( UCLASS_TWINS >= iClass )
	{
		// ������ 2�� Ŭ����.
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_MIRAGE >= iClass )
	{
		// ������ 3�� Ŭ����.
		Vec_Class.push_back(iClass - 2);
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_DRAGON_FIGHTER >= iClass )
	{
		// ������ 4�� Ŭ����.
		Vec_Class.push_back(iClass - 4);
		Vec_Class.push_back(iClass - 2);
		Vec_Class.push_back(iClass);
		return;
	}

	if( UCLASS_ECHIDNA >= iClass )
	{
		// Drakan 5th job
		Vec_Class.push_back(iClass - 6);
		Vec_Class.push_back(iClass - 4);
		Vec_Class.push_back(iClass - 2);
		Vec_Class.push_back(iClass);
		return;
	}
}

bool PgMissionGround::GetGadaCoinItemResult(int const iLevel, VEC_INT & Vec_Class, int const iCount, PgBase_Item & kItem)
{
	CONT_MISSION_CLASS_REWARD_BAG TempRewardBag;
	CONT_MISSION_CLASS_REWARD_BAG::const_iterator item_itr;
	const CONT_MISSION_CLASS_REWARD_BAG *pkItem = NULL;
	g_kTblDataMgr.GetContDef(pkItem);
	if( !pkItem )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_CLASS_REWARD_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkItem is NULL"));
	}
	else
	{
		VEC_INT::iterator class_iter = Vec_Class.begin();		// ���� Ŭ������ ���� Ŭ���� �ش��ϴ� ������ �� �׷��� ���Ѵ�.
		while( class_iter != Vec_Class.end() )
		{
			CONT_MISSION_CLASS_REWARD_BAG::key_type kKey(GetMissionNo(), iLevel, *class_iter);

			item_itr = pkItem->find(kKey);

			if( item_itr != pkItem->end() )
			{
				TempRewardBag.insert(std::make_pair(kKey, item_itr->second));
			}
			++class_iter;
		}

		if( TempRewardBag.empty() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Not Found Mission ClassReward[") << GetMissionNo() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
		}
		else
		{
			CONT_MISSION_CLASS_REWARD_ITEM Vec_TotalBag;
			CONT_MISSION_CLASS_REWARD_BAG::iterator bag_iter = TempRewardBag.begin();
			while( bag_iter != TempRewardBag.end() )
			{
				CONT_MISSION_CLASS_REWARD_BAG::mapped_type const &kElement = (*bag_iter).second;

				for(CONT_MISSION_CLASS_REWARD_ITEM::const_iterator iter = kElement.kCont.begin() ; iter != kElement.kCont.end() ; ++iter)
				{
					Vec_TotalBag.push_back(*iter);
				}
				
				++bag_iter;
			}

			GET_DEF(CItemBagMgr, kItemBagMgr);

			int iItemBagTotalRate = 0;

			for(CONT_MISSION_CLASS_REWARD_ITEM::const_iterator iter = Vec_TotalBag.begin() ; iter != Vec_TotalBag.end() ; ++iter)
			{
				iItemBagTotalRate += (*iter).iRate;
			}

			int const iRet = BM::Rand_Range((iItemBagTotalRate - 1), 0);
			int iTotalRate = 0;

			for(CONT_MISSION_CLASS_REWARD_ITEM::const_iterator result_iter = Vec_TotalBag.begin() ; result_iter != Vec_TotalBag.end() ; ++result_iter)
			{
				iTotalRate += (*result_iter).iRate;
				if( iRet < iTotalRate )
				{
					CONT_DEF_ITEM_BAG_GROUP const *pContContainer = NULL;
					g_kTblDataMgr.GetContDef(pContContainer);

					CONT_DEF_ITEM_BAG_GROUP::const_iterator item_cont_itor = pContContainer->find((*result_iter).iItemBagNo);
					if(item_cont_itor == pContContainer->end())
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}

					PgItemBag kItemBag;
					int const iResultBagGroupNo = (*item_cont_itor).second.iBagGroupNo;
					if( S_OK == kItemBagMgr.GetItemBagByGrp(iResultBagGroupNo, static_cast<short>(iLevel), kItemBag))
					{
						int iResultItem = 0;
						if(S_OK == kItemBag.PopItem( iLevel, iResultItem ) )
						{
							if(S_OK == CreateSItem(iResultItem, 1, GIOT_MISSION_GADACOIN, kItem))
							{
								return true;
							}
						}
					}
				}
			}

		}
	}

	return false;
}

bool PgMissionGround::GetRankItemResult(int const iLevel, int const iRank, PgBase_Item & kItem)
{
	CONT_MISSION_RANK_REWARD_BAG::const_iterator item_itr;
	const CONT_MISSION_RANK_REWARD_BAG *pkItem = NULL;
	g_kTblDataMgr.GetContDef(pkItem);
	if( !pkItem )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_RANK_REWARD_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkItem is NULL"));
	}
	else
	{
		CONT_MISSION_RANK_REWARD_BAG::key_type kKey(GetMissionNo(), iLevel, iRank);

		item_itr = pkItem->find(kKey);
		if( item_itr == pkItem->end() )
		{
			//VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Not Found Mission RankReward[") << GetMissionNo() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
		}
		else
		{
			CONT_MISSION_RANK_REWARD_BAG::mapped_type const &kElement = (*item_itr).second;

			int iItemTotalRate = 0;

			for(CONT_MISSION_RANK_REWARD_ITEM::const_iterator iter = kElement.kCont.begin();iter != kElement.kCont.end();++iter)
			{				
				iItemTotalRate += (*iter).iRate;
			}
	
			int const iRet = BM::Rand_Range((iItemTotalRate - 1), 0);
			int iTotalRate = 0;

			for(CONT_MISSION_RANK_REWARD_ITEM::const_iterator result_iter = kElement.kCont.begin();result_iter != kElement.kCont.end();++result_iter)
			{
				iTotalRate += (*result_iter).iRate;
				if( iRet < iTotalRate )
				{
					if(S_OK == CreateSItem((*result_iter).iItemNo, (*result_iter).sCount, 0, kItem))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool PgMissionGround::IsMacroCheckGround() const
{
	return IsLastStage();
}

int PgMissionGround::GetTotalScoreUpdateResult()
{
	return (iTotalScoreUpdate / MISSION_SCORE_CALC);
}

EMissionRank PgMissionGround::GetMissionRank(bool bErrorRank)
{
	int iTotalResultValue = GetTotalScoreUpdateResult();
	
	// GM Score
	if( GMState() )
	{
		iTotalResultValue = GMScore();
	}

	if( iTotalResultValue > MISSION_TOTALSCORE_MAX ) iTotalResultValue = MISSION_TOTALSCORE_MAX;
	if( iTotalResultValue < 0 ) iTotalResultValue = 0;

	// ��ũ�� ����ϰ�
	//BYTE kRank = MRANK_F;
	if ( !bErrorRank )
	{
		for(int i=0; i<MISSION_RANK;)
		{
			if( iTotalResultValue >= ms_aiResultRank[i++] )
			{
				return static_cast<EMissionRank>(i);
			}
		}
	}
	return MRANK_F;
}

void PgMissionGround::MissionCountAddExp(int iMissionCount, __int64 &iBonusExp)
{
	if( 0 >= iBonusExp )
	{
		return;
	}

	if( 1 >= iMissionCount )
	{
		return;
	}

	if( 15 <= iMissionCount )
	{
		iBonusExp = static_cast<__int64>(iBonusExp + ((iBonusExp * 150) / 100));
	}
	else
	{
		iBonusExp = static_cast<__int64>(iBonusExp + ((iBonusExp * (iMissionCount * 10)) / 100));
	}
}

DWORD PgMissionGround::GetPartyTimeLimit()
{
	int const iPartyNumber = PgMission::m_kConUser.size();

	int iMissionVarValue = 0;
	EVar_Mission eMissionVar;

	switch( iPartyNumber )
	{
	case 1:
		{
			eMissionVar = EVar_Mission_TimeLimitParty1;
		}break;
	case 2:
		{
			eMissionVar = EVar_Mission_TimeLimitParty2;
		}break;
	case 3:
		{
			eMissionVar = EVar_Mission_TimeLimitParty3;
		}break;
	case 4:
		{
			eMissionVar = EVar_Mission_TimeLimitParty4;
		}break;
	default:
		{
			eMissionVar = EVar_Mission_TimeLimitParty1;
		}break;
	}

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Mission, eMissionVar, iMissionVarValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Mission 'TimeLimitParty' Value, Type[") << eMissionVar << _T("]") );
	}

	if( SHRT_MAX < iMissionVarValue )
	{
		iMissionVarValue = SHRT_MAX;
	}

	return static_cast<DWORD>(iMissionVarValue);
}

int PgMissionGround::GetOwnerLevel()
{
	int iLevel = 0;
	for(ConUser::iterator user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr)
	{
		PgPlayer* pkUser = GetUser(user_itr->first);
		if( !pkUser )
		{
			continue;
		}

		iLevel = pkUser->GetAbil(AT_LEVEL);
		if(m_kLocalPartyMgr.IsMaster(pkUser->GetPartyGuid(), pkUser->GetID()))
		{
			return iLevel;
		}
	}
	return iLevel;	
}

int PgMissionGround::GetUserAverageLevel()
{
    int iLevel = 0;
	for(ConUser::iterator user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr)
	{
		PgPlayer* pkUser = GetUser(user_itr->first);
		if( !pkUser )
		{
			continue;
		}

		iLevel += pkUser->GetAbil(AT_LEVEL);
	}

    if(iLevel)
    {
        return iLevel / m_kConUser.size();
    }

	return iLevel;
}

int PgMissionGround::GetTunningLevel()
{
	const CONT_DEF_MISSION_ROOT* pContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pContDefMap);
	if(!pContDefMap)
	{
		return 0;
	}

	int iTunningLevel = 0;
	if(IsDefenceMode())
	{
		iTunningLevel = GetUserAverageLevel();
		CONT_DEF_MISSION_ROOT::const_iterator root_itr = pContDefMap->find(GetMissionNo());
		if (pContDefMap->end() != root_itr)
		{
			const TBL_DEF_MISSION_ROOT& element = (*root_itr).second;
			int const iMin = element.aiLevel_AvgMin[0];
			int const iMax = element.aiLevel_AvgMax[0];
			if(0!=iMin && 0!=iMax && iMin<=iMax)
			{
				iTunningLevel = std::max(iMin,iTunningLevel);
				iTunningLevel = std::min(iMax,iTunningLevel);
			}
		}
	}
    else if( IsDefenceMode7() || IsDefenceMode8() )
    {
        iTunningLevel = GetUserAverageLevel();
    }
	else
	{
		iTunningLevel = GetOwnerLevel();
		CONT_DEF_MISSION_ROOT::const_iterator root_itr = pContDefMap->find(GetMissionNo());
		if (pContDefMap->end() != root_itr)
		{
			const TBL_DEF_MISSION_ROOT& element = (*root_itr).second;
			int const iMin = element.aiLevel_AvgMin[__max(GetLevel(),0)];
			int const iMax = element.aiLevel_AvgMax[__min(GetLevel(),MAX_MISSION_LEVEL-1)];
			if(0!=iMin && 0!=iMax && iMin<=iMax)
			{
				iTunningLevel = std::max(iMin,iTunningLevel);
				iTunningLevel = std::min(iMax,iTunningLevel);
			}
		}
	}

	if(0==iTunningLevel)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"OwnerLevel is Zero");
	}
    return iTunningLevel;
}

void PgMissionGround::AddMonsterEnchant()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
	if( false == GetDefenceStage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
	{
		return;
	}

	CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);

	CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type kElement;
	GetDefenceAddMonster(kValue.iAddMonster_GroupNo, m_iSelect_SuccessCount, kElement);
	int const iMonsterNo = kElement.iMonsterNo;					//kValue.iAddMonster_GroupNo -> ���̺� �����ؼ� ���� ��ȣ ������
	int const iMonsterEnchantProbCalc = kElement.iEnchant_Probability;

	int iTunningNo = 0;
	int const iNowWave = m_iNowWave + 1;
	CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type kWaveData;
	if( true == GetDefenceWave(kValue.iWave_GroupNo, iNowWave, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
	{
		CONT_MISSION_DEFENCE_WAVE::value_type const& kWaveValue = kWaveData.kCont.at(0);

		iTunningNo = kWaveValue.iTunningNo;
	}

	bool bSelectMonsterGen = false;

	switch( eMonsterGen )
	{
	case D_LEFT:
		{
			bSelectMonsterGen = true;
		}break;
	case D_RIGHT:
		{
			bSelectMonsterGen = false;
		}break;
	case D_RANDOM:
	default:
		{
			bSelectMonsterGen = (BM::Rand_Index(2) == 0 ? true : false);
		}break;
	}

	POINT3 kRegenPos(0, 0, 0);	

	char acSpawnName[64] = {0,};		
	if( true == bSelectMonsterGen )
	{
		::sprintf_s(acSpawnName, 64, "mon_Left_5");
	}
	else
	{
		::sprintf_s(acSpawnName, 64, "mon_Right_5");
	}

	if( S_OK == PgGround::FindTriggerLoc(acSpawnName, kRegenPos) )
	{
		TBL_DEF_MAP_REGEN_POINT kRegenInfo;
		kRegenInfo.iMapNo = GetGroundNo();
		kRegenInfo.pt3Pos = kRegenPos;
		kRegenInfo.iTunningNo = iTunningNo;
		BM::GUID kMonsterGuid;	

		if( 0 < iMonsterNo )
		{
			if (E_FAIL == InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid, NULL, false, PgAction_MonsterEnchantProbCalc(iMonsterEnchantProbCalc).Get() ))			
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			}
			else
			{
				// ��æƮ ���� �� ���� �ϸ� �������� �ð� ����				
				m_iStageTimePlus = kElement.iAdd_StageTime;
				CUnit * pkMonster = GetUnit(kMonsterGuid);
				if(pkMonster)
				{
					if(kElement.iItemNo && (BM::Rand_Range(100, 1) <= kElement.iDropRate))
					{
						size_t iDropCount = std::max(kElement.iItemCount, kElement.iItemCount * m_iPrevStage_CountSelectItem);
						if(false==PgGroundUtil::InsertAlwaysDropItem(pkMonster, kElement.iItemNo, iDropCount)) //���̺��� ������ �⺻ ��� ����*��߹�� ������ ��� ����
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << L"Full AlwaysDropItem GroupNo["<<kValue.iAddMonster_GroupNo<< L" SuccessCount["<<m_iSelect_SuccessCount<< L"] ItemNo["<<kElement.iItemNo<< L"]" );
						}
					}
				}
			}
		}
	}
}

void PgMissionGround::DefenceWaveMonsterGenerate(CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type& kElement)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_MISSION_DEFENCE_WAVE::value_type &kWave = kElement.kCont.at(0);

	BYTE bySelectMonsterGen = 0;

	switch( eMonsterGen )
	{
	case D_LEFT:
		{
			bySelectMonsterGen = 0x02;
		}break;
	case D_RIGHT:
		{
			bySelectMonsterGen = 0x01;
		}break;
	case D_RANDOM:
	default:
		{
			bySelectMonsterGen = (BM::Rand_Index(2) == 0 ? 0x02 : 0x01);
		}break;
	}

	eMonsterGen = D_RANDOM;

	if( (INDUN_STATE_PLAY == m_eState) &&(0 == m_iNowStage) && (0 == m_iNowWave) )
	{
		// ���� ���� ���� ���� �������Ƿ� �� �ʿ� ����
		//++m_iNowWave;

		return;
	}

	int iObjectAttackMonCount = 0;
	int iObjectAttackMonMaxCount = 0;

	for(int i=0; i<MAX_WAVE_MONSTER_NUM; ++i)
	{
		if( 0 < kWave.iMonster[i] )
		{
			++iObjectAttackMonMaxCount;
		}
	}

	if( 0 < ms_iDefenceObjectAttackMonCount )
	{
		iObjectAttackMonMaxCount = (iObjectAttackMonMaxCount * ms_iDefenceObjectAttackMonCount / 100);
	}
	if( 0 > iObjectAttackMonMaxCount )
	{
		iObjectAttackMonMaxCount = 0;
	}

	

	BM::GUID kMonsterGuid;
	bool bMonsterZen = false;
	for(int i=0; i<MAX_WAVE_MONSTER_NUM; ++i)
	{
		POINT3 kRegenPos(0, 0, 0);	

		char acSpawnName[64] = {0,};		
		if( 0x02 & bySelectMonsterGen )
		{
			::sprintf_s(acSpawnName, 64, "mon_Left_%d", (i+1) );
		}
		else
		{
			::sprintf_s(acSpawnName, 64, "mon_Right_%d", (i+1) );
		}

		if( S_OK == PgGround::FindTriggerLoc(acSpawnName, kRegenPos) )
		{
			bMonsterZen = true;

			NxRay kRay(NxVec3(kRegenPos.x, kRegenPos.y, kRegenPos.z), NxVec3(0, 0, -1.0f));
			NxRaycastHit kHit;
			NxShape *pkHitShape = RayCast(kRay, kHit, 50.0f);
			if(pkHitShape)
			{
				kRegenPos.z = kHit.worldImpact.z;
			}

			TBL_DEF_MAP_REGEN_POINT kRegenInfo;
			kRegenInfo.iMapNo = GetGroundNo();
			kRegenInfo.pt3Pos = kRegenPos;
			kRegenInfo.iTunningNo = kWave.iTunningNo;

			int const iMonsterNo = kWave.iMonster[i];

			if( 0 < iMonsterNo )
			{
				if( S_OK == InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid ) )
				{
					CUnit* pkUnit = PgObjectMgr::GetUnit(kMonsterGuid);
					if( pkUnit )
					{
						// Speed
						int const iMoveSpeed = pkUnit->GetAbil(AT_MOVESPEED);
						int iSpeed = iMoveSpeed + (iMoveSpeed * kWave.iAddMoveSpeedPercent / 100.0f);
						
						if( 0 < iSpeed )
						{
							pkUnit->SetAbil(AT_MOVESPEED, iSpeed);
							pkUnit->NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL);
						}

						// Exp
						int const iAddExp = kWave.iAddExpPercent;
						if( 0 < iAddExp )
						{
							int const iAddExpPer = std::max(iAddExp, 0);
							pkUnit->SetAbil(AT_ADD_EXP_PER, iAddExpPer);
						}

						// Hp
						int const iHP = pkUnit->GetAbil(AT_HP);
						int const iRateHP = kWave.iAddHPPercent;
						if(	0 < iRateHP )
						{
							int const iMaxHP = pkUnit->GetAbil(AT_MAX_HP); 

							int const iAddHP = iHP + (iHP * pkUnit->GetAbil(AT_C_HP_POTION_ADD_RATE) + iRateHP * iMaxHP) / ABILITY_RATE_VALUE;

							int const iRetHP = std::max(iMaxHP, iAddHP);
							int const iRetMaxHP = std::max(iMaxHP, iAddHP);

							pkUnit->SetAbil(AT_MAX_HP, iRetMaxHP, true, true);
							pkUnit->SetAbil(AT_C_MAX_HP, 0);
                            pkUnit->SetAbil(AT_HP, pkUnit->GetAbil(AT_C_MAX_HP), true, true);
						}

						// Damage
						int const iAddDamage = kWave.iAddDamagePercent;
						pkUnit->AddAbil(AT_R_PHY_ATTACK_MAX, iAddDamage);
						pkUnit->AddAbil(AT_R_PHY_ATTACK_MIN, iAddDamage);
						pkUnit->AddAbil(AT_R_MAGIC_ATTACK_MAX, iAddDamage);
						pkUnit->AddAbil(AT_R_MAGIC_ATTACK_MIN, iAddDamage);

						// ���� ���� �Ӽ� ����
						SetMonsterAttack(pkUnit, iObjectAttackMonCount, iObjectAttackMonMaxCount);								
					}
				}
			}
		}
	}

	if( true == bMonsterZen )
	{
		BM::Stream kPacket(PT_NFY_M_C_DEFENCE_ZEN_EFFECT);
		kPacket.Push(bySelectMonsterGen);
		Broadcast(kPacket);
	}

	//SetDefenceGroundInfo();
}

void PgMissionGround::SetMonsterAttack(CUnit *pkMonster, int &iObjectAttackMonCount, int &iObjectAttackMonMaxCount)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( !pkMonster )
	{
		return;
	}

	if( false == pkMonster->IsUnitType(UT_MONSTER) )
	{
		return;
	}

	if( EMGRADE_ELITE == pkMonster->GetAbil(AT_GRADE) )
	{
		return;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);

	bool bSetAbilMonster = false;

	for (int i=0; i<MAX_AI_SKILL_NUM; i++)
	{
		int const iSkill = pkMonster->GetAbil(AT_MON_SKILL_01 + i);

		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkill);
		if( !pkSkillDef )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Cannot get SkillDef ["<<iSkill<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return;
		}
		
		if( true == pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE) )
		{
			bSetAbilMonster = true;
			break;
		}
	}

	if( false == bSetAbilMonster )
	{
		if( iObjectAttackMonCount < iObjectAttackMonMaxCount )
		{
			// Ÿ���� ���� ��Ÿ��
			if( IsDefenceMode() )
			{
				if( 0 < ms_iDefenceChangeTargetRate )
				{
					pkMonster->SetAbil(AT_AI_TARGETTING_RATE, ms_iDefenceChangeTargetRate);	// XXX %Ȯ���� Ÿ�� ����
				}
			}
			else if( IsDefenceMode7() || IsDefenceMode8() )
			{
				if( 0 < ms_iDefence7ChangeTargetRate )
				{
					pkMonster->SetAbil(AT_AI_TARGETTING_RATE, ms_iDefence7ChangeTargetRate);	// XXX %Ȯ���� Ÿ�� ����
				}
			}
			pkMonster->SetAbil(AT_MONSTER_FIRST_TARGET, static_cast<int>(UT_OBJECT));

			++iObjectAttackMonCount;
			return;
		}
	}

	// PC�� ������ ����
	CUnit* pkObjectUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
	if((pkObjectUnit = PgObjectMgr::GetNextUnit(UT_OBJECT, kItor)) != NULL)
	{
		PgMonster* pkMonsterUnit = dynamic_cast<PgMonster*>(pkMonster);
		if(pkMonsterUnit)
		{
			pkMonsterUnit->AddIgnoreTarget(pkObjectUnit, BM::GetTime32());
		}
	}

	pkMonster->SetAbil(AT_MONSTER_FIRST_TARGET, static_cast<int>(UT_PLAYER));
	pkMonster->SetAbil(AT_AI_TARGETTING_RATE, 1);
	
}

void PgMissionGround::SetDefenceGroundInfo()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if(IsDefenceGameMode())
	{
		if( 0==m_iNowStage && 1==m_iNowWave )
		{
			StartAI();
		}
	}

	CONT_OBJECT_MGR_UNIT::iterator kItor;

	PgMonster* pkMonster = NULL;	
	PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
	while ((pkMonster = dynamic_cast<PgMonster*> (PgObjectMgr::GetNextUnit(UT_MONSTER, kItor))) != NULL)
	{
		if( IsDefenceMode() || IsDefenceMode7() )
		{
			pkMonster->SetAbil(AT_TEAM, TEAM_BLUE, true, true);
		}

        if( IsDefenceMode7() || IsDefenceMode8() )
        {
            if( 0==m_iNowStage && 1==m_iNowWave )
            {//���ӽ��������� ���ݹ��� �ʵ��� ������ Ǯ��
                pkMonster->SetAbil(AT_CANNOT_DAMAGE, 0, true, true);
            }
        }

		pkMonster->SetAbil(AT_USENOT_SMALLAREA, 1);
		pkMonster->SetAbil(AT_DETECT_RANGE, 3000);
	}

	if( IsDefenceMode() || IsDefenceMode7() )
	{
		if( 0==m_iNowStage && 1==m_iNowWave )
		{
			PgPlayer* pkPlayer = NULL;
			CUnit* pkSummoned = NULL;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
			{
				pkPlayer->SetAbil(AT_TEAM, TEAM_RED, true, true);

				VEC_SUMMONUNIT const& kContSummonUnit = pkPlayer->GetSummonUnit();
				for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
				{
					pkSummoned = GetUnit((*c_it).kGuid);
					if(pkSummoned)
					{
						pkSummoned->SetAbil(AT_TEAM, TEAM_RED, true, true);
					}
				}
			}

			if( IsDefenceMode() )	// 7,8���� ��ȣ�� ����� ����(DB)
			{
				// Ÿ���� �� ���� 1�� �ؾ� �ȴ�.
				CUnit* pkUnit = NULL;
				PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
				while((pkUnit = PgObjectMgr::GetNextUnit(UT_OBJECT, kItor)) != NULL)
				{
					pkUnit->SetAbil(AT_TEAM, TEAM_RED, true, true);
				}
			}
		}
	}
}

bool PgMissionGround::IsDefenceLastStage()
{
	switch( m_iModeType )
	{
	case MO_DEFENCE:
		{
			const CONT_MISSION_DEFENCE_STAGE_BAG *pkStage;
			g_kTblDataMgr.GetContDef(pkStage);
			if( !pkStage )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE_STAGE_BAG is NULL") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
			}
			else
			{
				int const iPartyNumber = PgMission::m_kConUser.size();

				int const iNowStage = m_iNowStage + 1;
				int const iNextStage = iNowStage + 1;

				CONT_MISSION_DEFENCE_STAGE_BAG::key_type kKey(GetMissionNo(), iPartyNumber, iNextStage);
				CONT_MISSION_DEFENCE_STAGE_BAG::const_iterator stage_itr = pkStage->find(kKey);
				if( stage_itr == pkStage->end() )
				{
					// ������ ��������
					return true;
				}
			}
		}break;
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			const CONT_MISSION_DEFENCE7_STAGE_BAG *pkStage;
			g_kTblDataMgr.GetContDef(pkStage);
			if( !pkStage )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_STAGE_BAG is NULL") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
			}
			else
			{
				int const iPartyNumber = PgMission::m_kConUser.size();

				int const iNowStage = m_iNowStage + 1;
				int const iNextStage = iNowStage + 1;

				CONT_MISSION_DEFENCE7_STAGE_BAG::key_type kKey(GetMissionNo(), m_iModeType, iPartyNumber, iNextStage);
				CONT_MISSION_DEFENCE7_STAGE_BAG::const_iterator stage_itr = pkStage->find(kKey);
				if( stage_itr == pkStage->end() )
				{
					// ������ ��������
					return true;
				}
			}
		}break;
	default:
		{
		}break;
	}


	return false;
}

bool PgMissionGround::IsDefenceLastWave()
{
	int const iNowStage = m_iNowStage + 1;
	int const iNextWave = (m_iNowWave + 1);

	switch( m_iModeType )
	{
	case MO_DEFENCE:
		{
			CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
			if( true == GetDefenceStage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);					

				CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type kWaveData;
				if( true == GetDefenceWave(kValue.iWave_GroupNo, iNextWave, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
				{
					// Wave �ִ�.
					return false;
				}
				else
				{
					return true;
				}
			}
		}break;
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kStageData;
			if( true == GetDefence7Stage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				CONT_MISSION_DEFENCE7_STAGE::value_type &kValue = kStageData.kCont.at(0);					

				CONT_MISSION_DEFENCE7_WAVE_BAG::key_type	kKeyData;
				CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type kWaveData;
				if( true == GetDefence7Wave(iNowStage, iNextWave, kKeyData, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
				{
					// Wave �ִ�.
					return false;
				}
				else
				{
					return true;
				}
			}
		}break;
	default:
		{
		}break;
	}

	return true;
}

int PgMissionGround::GetDefenceStageCount()
{
	switch( m_iModeType )
	{
	case MO_DEFENCE:
		{
			const CONT_MISSION_DEFENCE_STAGE_BAG *pkStage;
			g_kTblDataMgr.GetContDef(pkStage);
			if( !pkStage )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE_STAGE_BAG is NULL") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
			}
			else
			{
				int const iPartyNumber = PgMission::m_kConUser.size();
				int iStageNo = 1;
				int iStageCount = 0;

				CONT_MISSION_DEFENCE_STAGE_BAG::const_iterator stage_itr = pkStage->end();
				do
				{
					CONT_MISSION_DEFENCE_STAGE_BAG::key_type kKey(GetMissionNo(), iPartyNumber, ++iStageNo);
					stage_itr = pkStage->find(kKey);
					if( pkStage->end() != stage_itr )
					{
						++iStageCount;
					}
				}while(pkStage->end() != stage_itr);

				return iStageCount;
			}
		}break;
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			const CONT_MISSION_DEFENCE7_STAGE_BAG *pkStage;
			g_kTblDataMgr.GetContDef(pkStage);
			if( !pkStage )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_STAGE_BAG is NULL") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
			}
			else
			{
				int const iPartyNumber = PgMission::m_kConUser.size();
				int iStageNo = 1;
				int iStageCount = 0;

				CONT_MISSION_DEFENCE7_STAGE_BAG::const_iterator stage_itr = pkStage->end();
				do
				{
					CONT_MISSION_DEFENCE7_STAGE_BAG::key_type kKey(GetMissionNo(), m_iModeType, iPartyNumber, ++iStageNo);
					stage_itr = pkStage->find(kKey);
					if( pkStage->end() != stage_itr )
					{
						++iStageCount;
					}
				}while(pkStage->end() != stage_itr);

				return iStageCount;
			}
		}break;
	default:
		{
		}break;
	}

	return 0;
}

bool PgMissionGround::GetDefenceStage(int const iStage, CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type& kElement)const
{
	int const iPartyNumber = PgMission::m_kConUser.size();

	const CONT_MISSION_DEFENCE_STAGE_BAG *pkStage;
	g_kTblDataMgr.GetContDef(pkStage);
	if( !pkStage )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE_STAGE_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
	}
	else
	{
		CONT_MISSION_DEFENCE_STAGE_BAG::key_type kKey(GetMissionNo(), iPartyNumber, iStage);

		CONT_MISSION_DEFENCE_STAGE_BAG::const_iterator stage_itr = pkStage->find(kKey);		
		if( stage_itr == pkStage->end() )
		{
			// ���̻� ������ �������� ����
			return false;
		}
		else
		{
			kElement = (*stage_itr).second;

			return true;
		}
	}

	return false;
}
/*template<typename T_Stage, typename T_Key, typename T_Element>
bool PgMissionGround::GetDefenceStage(T_Stage const iStage, T_Key kKeyType, T_Element& kElement)
{
	typedef std::map< T_Key, T_Element > _CONT;
	const _CONT *pkStage;
	g_kTblDataMgr.GetContDef(pkStage);
	if( !pkStage )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE_STAGE_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
	}
	else
	{
		int const iPartyNumber = PgMission::m_kConUser.size();

		_CONT::key_type kKey;
		kKey.Set(GetMissionNo(), iPartyNumber, iStage);

		_CONT::const_iterator stage_itr = pkStage->find(kKey);		
		if( stage_itr == pkStage->end() )
		{
			// ���̻� ������ �������� ����
			return false;
		}
		else
		{
			kElement = (*stage_itr).second;

			return true;
		}
	}

	return false;
}*/

bool PgMissionGround::GetDefenceWave(int const iWaveGroupNo, int const iWaveNo, CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type& kElement)const
{	
	const CONT_MISSION_DEFENCE_WAVE_BAG *pkWave;
	g_kTblDataMgr.GetContDef(pkWave);
	if( !pkWave )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE_WAVE_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkWave is NULL"));
	}
	else
	{
		CONT_MISSION_DEFENCE_WAVE_BAG::key_type kKey(iWaveGroupNo, iWaveNo);

		CONT_MISSION_DEFENCE_WAVE_BAG::const_iterator wave_iter = pkWave->find(kKey);
		if( wave_iter == pkWave->end() )
		{
			// ���̻� ������ ���̺갡 ����.
			return false;
		}
		else
		{
			kElement = (*wave_iter).second;

			return true;
		}		
	}

	return false;
}

bool PgMissionGround::IsDefenceNextStage()
{
	//�ּҽð� üũ..
	if(IsDefenceMode7())
	{
		int const iMonsterCount = PgObjectMgr::GetUnitCount(UT_MONSTER);
		if(0==iMonsterCount)
		{
			return true;
		}
		
		if( iMonsterCount && IsDefenceLastWave() )
		{
			return false;
		}

		if( (0==m_dwNextWave_StartDelay)
		|| (m_dwNextWave_StartDelay+m_dwNextWave_Delay < BM::GetTime32()) )
		{
			return true;
		}

		return false;
	}
	return true;
}

void PgMissionGround::DefenceNextStage()
{
	if(true==m_bFailMission)
	{
		return;
	}
	if(false==m_bDefence_NextStage)
	{
		return;
	}
	m_bDefence_SetWaveMonster = false;
	m_bDefence_NextStage = false;

	DefenceNextStage(m_iNowStage, m_iNowWave);
	if((GetState() & (INDUN_STATE_RESULT_WAIT | INDUN_STATE_RESULT)) == 0)
	{
		SetState(INDUN_STATE_PLAY);
	}
}

void PgMissionGround::DefenceNextStage(int const iNowStage, int const iNowWave)
{
	if(m_bDefenceEndStage)
	{
		return;
	}

	EDefenceMsg eDefenceMsg;

	int const iRegenStage = iNowStage + 1;
	
	eDefenceMsg = DefenceNextWaveMonsterGen(iRegenStage, iNowWave+1);	// 0���� �����ϹǷ�...
    m_dwCheckReReqWavePacket = 0;

	switch( eDefenceMsg )
	{
	case D_OK:
		{		
			if( 0 == m_iNowWave )
			{
				//SetDropItemView();
			}

			if( true == ((INDUN_STATE_READY == m_eState) &&(0 == m_iNowStage) && (0 == m_iNowWave)) )
			{
				break;
			}

			++m_iNowWave;	// Wave ����

			SetDefenceGroundInfo();
		}break;
	case D_WAVE_END:
		{
			// �ϳ��� �������� ��
			m_iNowWave = 0;
			//m_dwStage_StartDelay = BM::GetTime32();
			//
			GadaCoinUseClear();

			switch( m_iModeType )
			{
			case MO_DEFENCE:
				{
					EndDefenceMission();
				}break;
			case MO_DEFENCE7:
				{
					EndDefence7Mission();
				}break;
			case MO_DEFENCE8:
				{
					EndDefence7Mission();
				}break;
			default:
				{
				}break;
			}

			if(IsDefenceLastStage())
			{
				SetState(INDUN_STATE_RESULT_WAIT);
			}
		}break;
	default:
		{
		}break;
	}

	if( D_STAGE_END == eDefenceMsg )
	{
		// ��ü �������� ������???
	}
}

/*EDefenceMsg PgMissionGround::DefenceNextWaveMonsterGen(int const iNowStage, int const iNowWave)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
	if( true == GetDefenceStage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
	{
		CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);					

		CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type kWaveData;
		if( true == GetDefenceWave(kValue.iWave_GroupNo, iNowWave, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
		{
			CONT_MISSION_DEFENCE_WAVE::value_type &kWaveValue = kWaveData.kCont.at(0);						
			m_dwNextWave_StartDelay = BM::GetTime32();
			m_dwNextWave_Delay = static_cast<DWORD>(kWaveValue.iWave_Delay);	// ���� ����� Wave Time		

			DefenceWaveMonsterGenerate(kWaveData);

			return D_OK;
		}
		// �ش� Stage�� Wave�� ���̻� ���ٴ� �ǹ�
		return D_WAVE_END;
	}
	// �ش� Stage�� ���̻� ���ٴ� �ǹ�
	return D_STAGE_END;
}*/

EDefenceMsg PgMissionGround::DefenceNextWaveMonsterGen(int const iNowStage, int const iNowWave)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	switch( m_iModeType )
	{
	case MO_DEFENCE:
		{
			CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
			if( true == GetDefenceStage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				CONT_MISSION_DEFENCE_STAGE::value_type const& kValue = kStageData.kCont.at(0);					

				CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type kWaveData;
				if( true == GetDefenceWave(kValue.iWave_GroupNo, iNowWave, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
				{
					CONT_MISSION_DEFENCE_WAVE::value_type const& kWaveValue = kWaveData.kCont.at(0);						
					m_dwNextWave_StartDelay = BM::GetTime32();
					m_dwNextWave_Delay = static_cast<DWORD>(kWaveValue.iWave_Delay);	// ���� ����� Wave Time		
					if( 0 >= m_dwNextWave_Delay )
					{
						m_dwNextWave_Delay = DEFAULT_NEXT_WAVE_DELAY_TIME;
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("WaveDelay Data Error! MissionNo[") << GetMissionNo() << _T("] Stage[") << iNowStage << _T("]") << _T("] Wave[") << iNowWave << _T("]") );
					}

					DefenceWaveMonsterGenerate(kWaveData);

					return D_OK;
				}

				int const iMonsterCount = PgObjectMgr::GetUnitCount(UT_MONSTER);
				if(iMonsterCount > 0)
				{
					m_dwNextWave_StartDelay = BM::GetTime32();
					m_dwNextWave_Delay = DEFAULT_NEXT_WAVE_DELAY_TIME;
					return D_FAIL;
				}

				// �ش� Stage�� Wave�� ���̻� ���ٴ� �ǹ�
				return D_WAVE_END;
			}
		}break;
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kStageData;
			if( true == GetDefence7Stage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				CONT_MISSION_DEFENCE7_STAGE::value_type const& kValue = kStageData.kCont.at(0);					

				CONT_MISSION_DEFENCE7_WAVE_BAG::key_type	kKeyData;
				CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type kWaveData;
				if( true == GetDefence7Wave(iNowStage, iNowWave, kKeyData, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
				{
					CONT_MISSION_DEFENCE7_WAVE::value_type const& kWaveValue = kWaveData.kCont.at(0);						
					m_dwNextWave_StartDelay = BM::GetTime32();
					m_dwNextWave_Delay = static_cast<DWORD>(kWaveValue.iWave_Delay);	// ���� ����� Wave Time		
					if( 0 >= m_dwNextWave_Delay )
					{
						m_dwNextWave_Delay = DEFAULT_NEXT_WAVE_DELAY_TIME;
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("WaveDelay Data Error! MissionNo[") << GetMissionNo() << _T("] Stage[") << iNowStage << _T("]") << _T("] Wave[") << iNowWave << _T("]") );
					}

					Defence7WaveMonsterGenerate(kWaveData);

					return D_OK;
				}
				// �ش� Stage�� Wave�� ���̻� ���ٴ� �ǹ�
				return D_WAVE_END;
			}
		}break;
	default:
		{
		}break;
	}


	// �ش� Stage�� ���̻� ���ٴ� �ǹ�
	return D_STAGE_END;
}

/*void PgMissionGround::SetWaveMonster(BM::Stream * const pkPacket)
{
	// Wave ���� ���� �� ���� ����
	CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
	if( true == GetDefenceStage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
	{
		CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);

		if( true == m_bSelectSuccess )
		{
			AddMonsterEnchant();
			m_bSelectSuccess = false;
		}
		
		// �������� �ð� ����
		//eMonsterGen = D_RANDOM;
		m_dwStage_Time = kValue.iStage_Time;
		m_dwStage_StartTime = BM::GetTime32();
		//m_dwStage_Delay = kValue.iStage_Delay;

		// �ð� �ֱ�
		int iStageStartTime = 0;
		if( 0 < m_iStageTimePlus )
		{
			m_dwStage_Time += m_iStageTimePlus;			
			m_iStageTimePlus = 0;
		}
		iStageStartTime = m_dwStage_Time;

		BM::Stream kCPacket(PT_M_C_NFY_DEFENCE_STAGETIME);
		kCPacket.Push(static_cast<int>(iStageStartTime));
		Broadcast(kCPacket);
	}	

	// Wave ���� �ض�	
	if( D_RANDOM == eMonsterGen )
	{
		EDefenceMonsterGen kMonsterGen = (BM::Rand_Index(2) == 0 ? D_LEFT : D_RIGHT);
		eMonsterGen = kMonsterGen;
	}

	if( 0 != m_iNowWave )
	{
		BM::Stream kDefencePacket(PT_M_C_NFY_DEFENCE_WAVE);
		kDefencePacket.Push( false );
		kDefencePacket.Push( static_cast<BYTE>(eMonsterGen) );
		Broadcast(kDefencePacket);
	}
	else
	{
		// �������� ���� �� ���� ������
		size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
		int const iTotalWaveMonsterCount = GetStageTotalMonsterCount();
		Broadcast( BM::Stream(PT_M_C_NFY_REST_MONSTER_NUM, iLiveMonsterCount+iTotalWaveMonsterCount) );

		SetDefenceGroundInfo();
	}
}*/

void PgMissionGround::NextStageSend()
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if(true==m_bFailMission && m_dwFailMissionWaitTime>0)
	{
		m_dwFailMissionWaitTime = BM::GetTime32();
		return;
	}
	if( false == m_bDefenceStageStart )
	{
		return;
	}
	m_bDefenceStageStart = false;
	m_bDefence_SetWaveMonster = false;	
	m_bDefence_NextStage = false;

	if( INDUN_STATE_PLAY == m_eState )
	{
		++m_iNowStage;
	}

	m_iNowWave = 0;
	//m_dwStage_StartDelay = 0;
	m_bDefenceEndStage = false;


	m_bSelectSuccess = true;
	
	EDefenceMonsterGen kMonsterGen = (BM::Rand_Index(2) == 0 ? D_LEFT : D_RIGHT);
	if( ( 0 == m_iCountSelectItem && GetSelectedDirection() != kMonsterGen )
		|| 0 == m_iNowStage )
	{//������ �Ƚ��, ���⵵ �������� / ù ���������� �ȳ���.
		m_bSelectSuccess = false;

		// ���н� �����߼� �ʱ�ȭ
		m_iSelect_SuccessCount = 0;
		// ���н� ���е� ������ ���� ���;� �ȴ�
		eMonsterGen = kMonsterGen;
	}
	else
	{//���߱� �����ؼ� ī��Ʈ ����
		if( 0 == m_iCountSelectItem )
		{
			eMonsterGen = kMonsterGen;
		}
		++m_iSelect_SuccessCount;
	}

	if( INDUN_STATE_READY == m_eState )
	{
		eMonsterGen = kMonsterGen;
	}

	// �̹� ���������� ������ ��� ���θ� ����
	m_bPrevStage_UseSelectItem = (m_iCountSelectItem > 0);
	m_iPrevStage_CountSelectItem = m_iCountSelectItem;
	m_kContUserDirection.clear();
	m_iCountSelectItem = 0;

	// �������� ���� ����.
	int const iStageCount = GetDefenceStageCount();

	PgMissionInfo const * pMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pMissionInfo );
	if( bRet )
	{		
		if( IsDefenceMode7() && pMissionInfo->GetHaveArcadeMode() )
		{	// �������潺 ��忡�� �������� ������ �� ���� �Ƿε��� ����.
			DecreaseStrategyFatigueDegree();
		}
	}

	BM::Stream kCPacket(PT_M_C_NFY_DEFENCE_STAGE);
	kCPacket.Push(GetMissionNo());
	kCPacket.Push(GetLevel()+1);
	kCPacket.Push(static_cast<int>(m_iNowStage));
	kCPacket.Push(iStageCount);
	kCPacket.Push(static_cast<EDefenceMonsterGen>(eMonsterGen));
	kCPacket.Push(m_bSelectSuccess);
	kCPacket.Push(m_iSelect_SuccessCount);
	Broadcast(kCPacket);

	//�� ���� �ʱ�ȭ
	//eMonsterGen = D_RANDOM;
}

void PgMissionGround::GM_DefenceNowStage( int const iNowStage )
{
	if( !IsDefenceGameMode() )
	{
		return;
	}
	m_iNowStage = std::min(iNowStage,GetDefenceStageCount());
	m_bDefenceStageStart = true;
	NextStageSend();
}

bool PgMissionGround::GetDefenceAddMonster(int const iMonsterGroupNo, int const iSelect_SuccessCount, CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type& kElement)
{
	CONT_DEF_DEFENCE_ADD_MONSTER const *pkAddMonsterBag;
	g_kTblDataMgr.GetContDef(pkAddMonsterBag);
	if( !pkAddMonsterBag )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE_ADD_MONSTER_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkAddMonsterBag is NULL"));
	}

	
	CONT_DEF_DEFENCE_ADD_MONSTER::key_type kKey(iMonsterGroupNo, iSelect_SuccessCount);
	CONT_DEF_DEFENCE_ADD_MONSTER::const_iterator AddMonster_itor = pkAddMonsterBag->find(kKey);
	if( pkAddMonsterBag->end() == AddMonster_itor )
	{
		return false;
	}

	kElement = (*AddMonster_itor).second;
	return true;
}

bool PgMissionGround::GetDefenceAddMonsterStage(int const iStageNo, int const iSelect_SuccessCount, CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type& kElement)
{
	CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
	if( GetDefenceStage(iStageNo, kStageData) )	
	{
		CONT_MISSION_DEFENCE_STAGE::value_type const& kValue = kStageData.kCont.at(0);
		return GetDefenceAddMonster(kValue.iAddMonster_GroupNo, iSelect_SuccessCount, kElement);
	}
	return false;
}

size_t PgMissionGround::GetNeedSelectItemCount()
{
	if( m_bPrevStage_UseSelectItem )
	{
		return 3;
	}
	return 1;
}

void PgMissionGround::ReqUseDefenceHpPlus(CUnit *pkOwner, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);		

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}

	PgInventory* pkInv = pkUser->GetInven();
	if( !pkInv )
	{
		return;
	}

	SItemPos kItemPos;
	if( !pkNfy->Pop(kItemPos) )
	{
		return;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	BM::Stream kPacket(PT_M_C_ANS_DEFENCE_POTION);
	HRESULT hRet(E_FAIL);

	if( false == IsDefenceMode() )
	{
		kPacket.Push(hRet);
		pkUser->Send(kPacket);

		return;
	}

	BM::GUID kObjectGuid;

	CONT_OBJECT_TEAM::iterator iter = m_kObjectTeam.find(TEAM_RED);
	if( m_kObjectTeam.end() != iter )
	{
		kObjectGuid = iter->second;
	}

	if( kObjectGuid.IsNotNull() )
	{
		CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
		if( pkUnit )
		{
			int const iHP = pkUnit->GetAbil(AT_HP);
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

			if( 0 < iHP )
			{
				if( iHP == iMaxHP )
				{
					hRet = E_NOT_ENOUGH;

					kPacket.Push(hRet);
					pkUser->Send(kPacket);
					return;
				}
			}
			else
			{
				return;
			}
		}
	}

	PgBase_Item kItem;
	if( S_OK == pkInv->GetItem(kItemPos, kItem) )
	{		
		CItemDef const * pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if( pItemDef )
		{
			int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
			int const iCustomValue = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

			if( UICT_MISSION_DEFENCE_POTION == iCustomType)
			{
				size_t const iHaveItemCount = pkInv->GetTotalCount(kItem.ItemNo());
				size_t const iNeedItemCount = 1;
				if( iNeedItemCount <= iHaveItemCount )
				{
					CONT_PLAYER_MODIFY_ORDER kOrder;
					SPMOD_Modify_Count kDelData(kItem, kItemPos, -iNeedItemCount);
					SPMO kIMO(IMET_MODIFY_COUNT, pkOwner->GetID(), kDelData);
					kOrder.push_back(kIMO);

					BM::Stream kPacket;
					kPacket.Push(static_cast<int>(iCustomValue));

					PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_DefencePotion, GroundKey(), kOrder, kPacket);
					kItemModifyAction.DoAction(pkOwner, NULL);
					return;
				}
				else
				{// �������� �����ϴ�...
					hRet = E_NOT_ENOUGH_ITEM;
				}
			}
			else
			{
				// �߸��� ������ ����
				hRet = E_INCORRECT_ITEM;
			}
		}
		else
		{
			hRet = E_NOT_FOUND_ITEM;
		}
	}
	else
	{// �������� ã�� �� ����.
		hRet = E_NOT_FOUND_ITEM;
	}

	kPacket.Push(hRet);
	pkUser->Send(kPacket);
}

void PgMissionGround::SetDefenceHpPlus(CUnit *pkOwner, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}

	int iHpPlus = 0;

	if( !pkNfy->Pop(iHpPlus) )
	{
		return;
	}

	// Hp ����
	if( 0 < iHpPlus )
	{
		BM::GUID kObjectGuid;

		CONT_OBJECT_TEAM::iterator iter = m_kObjectTeam.find(TEAM_RED);
		if( m_kObjectTeam.end() != iter )
		{
			kObjectGuid = iter->second;
		}

		if( kObjectGuid.IsNotNull() )
		{
			CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
			if( pkUnit )
			{
				int const iNowHP = pkUnit->GetAbil(AT_HP);
				int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
				int const iAddHP = iMaxHP * iHpPlus / 100.0f;
				int const iRetHP = std::min(iMaxHP, iAddHP+iNowHP);

				pkUnit->SetAbil(AT_HP, iRetHP, true);

				HRESULT hRet(S_OK);

				BM::Stream kPacket(PT_M_C_ANS_DEFENCE_POTION);
				kPacket.Push(hRet);
				kPacket.Push(static_cast<int>(iAddHP));
				Broadcast(kPacket);
			}
		}
	}
}

bool PgMissionGround::GetDefenceObjectHpCheck()
{
	BM::CAutoMutex kLock(m_kRscMutex);

	BM::GUID kObjectGuid;
	CONT_OBJECT_TEAM::iterator iter = m_kObjectTeam.find(TEAM_RED);
	if( m_kObjectTeam.end() != iter )
	{
		kObjectGuid = iter->second;
	}

	if( kObjectGuid.IsNotNull() )
	{
		CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
		if( pkUnit )
		{
			int const iNowHP = pkUnit->GetAbil(AT_HP);
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
			int const iCheckHP = iMaxHP * 50 / 100.0f;

			if( iNowHP <= iCheckHP )	// 50% ������ ���
			{
				return true;
			}
		}
	}
	return false;
}

void PgMissionGround::ReqUseDefenceTimePlus(CUnit *pkOwner, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);		

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}

	PgInventory* pkInv = pkUser->GetInven();
	if( !pkInv )
	{
		return;
	}

	SItemPos kItemPos;
	if( !pkNfy->Pop(kItemPos) )
	{
		return;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	BM::Stream kPacket(PT_M_C_ANS_DEFENCE_TIMEPLUS);
	HRESULT hRet(E_FAIL);

	if( false == IsDefenceMode() )
	{
		kPacket.Push(hRet);
		pkUser->Send(kPacket);

		return;
	}

	DWORD kNow = BM::GetTime32();
	if( m_dwStage_Time >= DifftimeGetTime(m_dwStage_StartTime, kNow) )
	{
		DWORD dwEndPlayTime = (m_dwStage_Time - DifftimeGetTime(m_dwStage_StartTime, kNow));

		if( DEFENCE_ITEM_USE_TIME >= dwEndPlayTime )
		{
			PgBase_Item kItem;
			if( S_OK == pkInv->GetItem(kItemPos, kItem) )
			{		
				CItemDef const * pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
				if( pItemDef )
				{
					int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
					int const iCustomValue = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

					if( UICT_MISSION_DEFENCE_TIMEPLUS == iCustomType)
					{
						size_t const iHaveItemCount = pkInv->GetTotalCount(kItem.ItemNo());
						size_t const iNeedItemCount = 1;
						if( iNeedItemCount <= iHaveItemCount )
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							SPMOD_Modify_Count kDelData(kItem, kItemPos, -iNeedItemCount);
							SPMO kIMO(IMET_MODIFY_COUNT, pkOwner->GetID(), kDelData);
							kOrder.push_back(kIMO);

							BM::Stream kPacket;
							kPacket.Push(static_cast<int>(iCustomValue));

							PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_DefenceTimePlus, GroundKey(), kOrder, kPacket);
							kItemModifyAction.DoAction(pkOwner, NULL);
							return;
						}
						else
						{// �������� �����ϴ�...
							hRet = E_NOT_ENOUGH_ITEM;
						}
					}
					else
					{
						// �߸��� ������ ����
						hRet = E_INCORRECT_ITEM;
					}
				}
				else
				{
					hRet = E_NOT_FOUND_ITEM;
				}
			}
			else
			{// �������� ã�� �� ����.
				hRet = E_NOT_FOUND_ITEM;
			}
		}
		else
		{
			// 10�ʰ� �� ���Ҵ�.
			hRet = E_COMMON_TIMEOUT;
		}
	}
	else
	{
		// ���� �����ε� ������?
		hRet = E_FAIL;
	}

	kPacket.Push(hRet);
	pkUser->Send(kPacket);
}

void PgMissionGround::SetDefenceTimePlus(CUnit *pkOwner, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}

	int iTimePlus = 0;

	if( !pkNfy->Pop(iTimePlus) )
	{
		return;
	}

	// �ð� ����
	SetStageTimePlus(iTimePlus);
}

void PgMissionGround::ReqUseInfallibleSelection( CUnit *pkOwner, BM::Stream* pkNfy )
{
	BM::CAutoMutex kLock(m_kRscMutex);		

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}
	PgInventory* pkInv = pkUser->GetInven();
	if( !pkInv )
	{
		return;
	}

	EDefenceMonsterGen eDirection; 
	if( !pkNfy->Pop(eDirection) )
	{
		return;
	}

	SItemPos kItemPos;
	if( !pkNfy->Pop(kItemPos) )
	{
		return;
	}

	if( !m_bDefenceStageStart )
	{
		return;
	}

	if(eMonsterGen != D_RANDOM && eMonsterGen != eDirection)
	{ //�̹� ������ ���������ٸ� �� �������θ� ������Ѵ�.
		return;
	}

	if(m_iCountSelectItem > 0)
	{ //��߹��� �������� ������ ����ؾ� �Ѵٸ�
		PgMissionInfo const * pkMissionInfo = NULL;
		bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
		if( true == bRet && NULL != pkMissionInfo)
		{
			if(0 == pkMissionInfo->m_kBasic.iEventMission)
			{ //�̺�Ʈ �̼��� �ƴ� ��� ���� ������ ����� �Ұ�
				return;
			}
		}
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	BM::Stream kPacket(PT_M_C_NFY_DEFENCE_INFALLIBLE_SELECTION);
	HRESULT hRet(E_FAIL);

	PgBase_Item kItem;
	if( S_OK == pkInv->GetItem(kItemPos, kItem) )
	{
		CItemDef const * pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if( pItemDef )
		{
			CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
			if( false == GetDefenceStage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				return;
			}

			CONT_MISSION_DEFENCE_STAGE::value_type &kValue = kStageData.kCont.at(0);
			if(kItem.ItemNo() != kValue.iDirection_Item)
			{
				return;
			}

			size_t const iHaveItemCount = pkInv->GetTotalCount(kItem.ItemNo());
			size_t const iNeedItemCount = GetNeedSelectItemCount();
			if( iNeedItemCount <= iHaveItemCount )
			{
				if( m_kConUser.size() > m_iCountSelectItem )
				{
					// �α׸� ����
					PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_Game);				
					kLogCont.MemberKey(pkUser->GetMemberGUID());
					kLogCont.CharacterKey(pkUser->GetID());
					kLogCont.ID(pkUser->MemberID());
					kLogCont.UID(pkUser->UID());
					kLogCont.Name(pkUser->Name());
					kLogCont.ChannelNo( pkUser->GetChannel() );
					kLogCont.Class(static_cast<short>(pkUser->GetAbil(AT_CLASS)));
					kLogCont.Level(static_cast<short>(pkUser->GetAbil(AT_LEVEL)));
					kLogCont.GroundNo( GetGroundNo() );
					//kLogCont.ChannelType( pkUser->ChannelType() );

					PgLog kLog(ELOrderMain_Item, ELOrderSub_Modify);
					std::wstring kItemName;
					if( ::GetItemName(kItem.ItemNo(),kItemName) )
					{// wstrValue1 �����۸�
						kLog.Set(0,kItemName);
					}
					kLog.Set( 0, static_cast<int>(kItem.ItemNo()) );	// iValue1 ������ ��ȣ
					kLog.Set( 1, static_cast<int>(iNeedItemCount) );	// iValue2 ����� ����
					kLog.Set( 2, static_cast<int>(GetMissionNo()) );	// iValue3 �̼ǹ�ȣ
					kLog.Set( 3, static_cast<int>(GetLevel()) );	// iValue4 �̼� ���̵�
					kLog.Set( 4, static_cast<int>(m_iNowStage) );	// iValue5 ��������
					kLog.Set( 1, kItem.EnchantInfo().Field_1() );	// i64Value2
					kLog.Set( 2, kItem.EnchantInfo().Field_2() );	// i64Value3
					kLog.Set( 3, kItem.EnchantInfo().Field_3() );	// i64Value4
					kLog.Set( 4, kItem.EnchantInfo().Field_4() );	// i64Value5
					kLogCont.Add(kLog);
					kLogCont.Commit();

					CONT_PLAYER_MODIFY_ORDER kOrder;
					SPMOD_Modify_Count kDelData(kItem, kItemPos, -iNeedItemCount);
					SPMO kIMO(IMET_MODIFY_COUNT, pkOwner->GetID(), kDelData);
					kOrder.push_back(kIMO);

					BM::Stream kPacket;
					kPacket.Push(static_cast<BYTE>(eDirection));

					PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_InfallibleSelection, GroundKey(), kOrder, kPacket);
					kItemModifyAction.DoAction(pkOwner, NULL);
					return;
				}
				else
				{//�� �̻� �������� ����� �� ����
					hRet =E_INFALLIBLE_SELECTION_CANT_USE;
				}
			}
			else
			{// �������� �����ϴ�...
				hRet =E_INFALLIBLE_SELECTION_NOT_ENOUGH_ITEM;
			}
		}
		else
		{
			hRet = E_NOT_FOUND_ITEM;
		}
	}
	else
	{// �������� ã�� �� ����.
		hRet = E_INFALLIBLE_SELECTION_NOT_FOUND_ITEM;
	}

	kPacket.Push(hRet);
	pkUser->Send(kPacket);
}

void PgMissionGround::SetDirection(CUnit *pkOwner, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}

	BYTE kDirection; 
	if( !pkNfy->Pop(kDirection) )
	{
		return;
	}

	//! ���� ����
	eMonsterGen = static_cast<BYTE>(kDirection);

	//! ����Ѱ����� ����
	m_iCountSelectItem++;
	HRESULT hRet(E_INFALLIBLE_SELECTION_SUCCESS);
	bool bStart = true;
	PgMissionInfo const * pkMissionInfo = NULL;
	bool bRet = g_kMissionMan.GetMission( GetMissionNo(), pkMissionInfo );
	if( true == bRet && NULL != pkMissionInfo)
	{
		if(0 != pkMissionInfo->m_kBasic.iEventMission)
		{ //�̺�Ʈ �̼��� ���
			bStart = (m_iCountSelectItem >= m_kConUser.size()); //��߹��� �������� ���� ���� �ʾҴٸ� �ٷ� �������� �ʰ� ���
		}
	}

	//! ��� �������� �˷��ش�.
	BM::Stream kPacket(PT_M_C_NFY_DEFENCE_INFALLIBLE_SELECTION);
	kPacket.Push(hRet);
	kPacket.Push(pkUser->GetID());
	kPacket.Push(bStart);
	kPacket.Push(eMonsterGen);
	kPacket.Push(m_iCountSelectItem);
	Broadcast(kPacket);
}

void PgMissionGround::SelectDirection(CUnit *pkOwner, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkOwner);
	if( !pkUser )
	{
		return;
	}

	BYTE kDirection; 
	if( !pkNfy->Pop(kDirection) )
	{
		return;
	}

	CONT_USER_DIRECTION::iterator direction_itor = m_kContUserDirection.find(pkOwner->GetID());
	if( m_kContUserDirection.end() != direction_itor )
	{//������ �����ϴ� ����� ����
		direction_itor->second = kDirection;
	}
	else
	{//ó�� �����ϴ°���� ����
		m_kContUserDirection.insert( std::make_pair( pkOwner->GetID(), kDirection ) );
	}

	bool bUsedInfallibleItem = (m_iCountSelectItem > 0);
	//! ��� �������� �˷��ش�.
	BM::Stream kPacket(PT_M_C_NFY_DEFENCE_DIRECTION);
	PU::TWriteTable_AA(kPacket, m_kContUserDirection);
	kPacket.Push(static_cast<bool>( CloseSelectDirection() ));
	kPacket.Push(bUsedInfallibleItem);
	Broadcast(kPacket);
}

bool PgMissionGround::CloseSelectDirection()
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CONT_USER_DIRECTION::const_iterator direction_itr;
	for(ConUser::const_iterator user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr)
	{
		direction_itr = m_kContUserDirection.find(user_itr->first);
		if(direction_itr == m_kContUserDirection.end())
		{
			return false;
		}
	}

	// ��� ������ ���� �ߴ���
	return true;
}

EDefenceMonsterGen PgMissionGround::GetSelectedDirection()
{
	int iLeft = 0, iRight = 0;
	CONT_USER_DIRECTION::const_iterator direction_itor = m_kContUserDirection.begin();
	while( m_kContUserDirection.end() != direction_itor )
	{
		EDefenceMonsterGen eDirection = static_cast<EDefenceMonsterGen>(direction_itor->second);
		switch(eDirection)
		{
		case D_LEFT: { ++iLeft; }break;
		case D_RIGHT: { ++iRight; }break;
		case D_RANDOM: {} break;
		}
		++direction_itor;
	}

	EDefenceMonsterGen eResultDirection = D_RANDOM;
	if( iLeft > iRight )
	{
		eResultDirection = D_LEFT;
	}
	else if( iLeft < iRight )
	{
		eResultDirection = D_RIGHT;
	}
	else
	{
		eResultDirection = (BM::Rand_Index(2) == 0 ? D_LEFT : D_RIGHT);
	}

	return eResultDirection;
}

void PgMissionGround::ObjectCreate()
{
	//if( (0 == m_iNowStage) && (0 == m_iNowWave) )
	{
		BM::CAutoMutex kLock(m_kRscMutex);

		/*POINT3 pt3TargetPos;
		if ( FAILED(FindSpawnLoc( 1, pt3TargetPos)) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return;
		}		

		int const iObjectNo = 5006901;

		TBL_DEF_MAP_REGEN_POINT kRegenInfo;
		kRegenInfo.iMapNo = GetGroundNo();
		BM::GUID kObjectGuid;
		SObjUnitBaseInfo kTemp(SObjUnitGroupIndex(), iObjectNo);
		kTemp.pt3Pos = pt3TargetPos;
		if( E_FAIL == InsertObjectUnit( kRegenInfo, kTemp, kObjectGuid ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		}
		else
		{
			CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
			if( pkUnit )
			{				
				m_kObjectGuid = kObjectGuid;
				m_iObjectHp = pkUnit->GetAbil(AT_HP);
				m_dwTowerHpMessageDelay = (BM::GetTime32() + TOWER_NOTI_DELAY);
			}
		}*/

		CUnit* pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
		while((pkUnit = PgObjectMgr::GetNextUnit(UT_OBJECT, kItor)) != NULL)
		{
			int iTeam = pkUnit->GetAbil(AT_TEAM);
			if( 0 < iTeam )
			{
				BM::GUID kObjectGuid = pkUnit->GetID();
				auto kPair = m_kObjectTeam.insert(std::make_pair(iTeam, kObjectGuid));
				if( kPair.second )
				{
					int iObjectHp = pkUnit->GetAbil(AT_HP);
					auto kPair2 = m_kObjectHp.insert(std::make_pair(iTeam, iObjectHp));
					
					m_dwTowerHpMessageDelay = (BM::GetTime32() + TOWER_NOTI_DELAY);
					m_dwTowerHpMessageDelay1 = (BM::GetTime32() + TOWER_NOTI_DELAY);
				}
			}
			if( true == IsDefenceGameMode() )
			{
				pkUnit->SetAbil(AT_DAMAGE_IS_ONE, 1, true, true);
			}
		}
	}
}

int PgMissionGround::GetStageTotalMonsterCount()const
{
	BM::CAutoMutex kLock(m_kRscMutex);

	int iNowStage = m_iNowStage + 1;
	int iNextWave = (m_iNowStage == 0) && (m_iNowWave == 0) ? 2 : m_iNowWave + 1; // ����� ���� ���̺�. ���潺�� ù�������� ù���̺�� 2��° ���̺���� ����ؾ� �ȴ�.
	int iTotalMonsterCount = 0;

	switch( m_iModeType )
	{
	case MO_DEFENCE:
		{
			CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
			if( true == GetDefenceStage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				CONT_MISSION_DEFENCE_STAGE::value_type const& kValue = kStageData.kCont.at(0);
				
				for(;0 != kValue.iWave_GroupNo;++iNextWave)
				{
					CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type kWaveData;
					if( true == GetDefenceWave(kValue.iWave_GroupNo, iNextWave, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
					{
						CONT_MISSION_DEFENCE_WAVE::value_type const& kWaveValue = kWaveData.kCont.at(0);

						for(int iMon=0; iMon<MAX_WAVE_MONSTER_NUM; ++iMon)
						{
							if( 0 < kWaveValue.iMonster[iMon] )
							{
								++iTotalMonsterCount;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}break;
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			CONT_MISSION_DEFENCE7_WAVE_BAG::key_type	kKeyData;
			do
			{				
				++iNextWave;
				CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type kWaveData;
				if( true == GetDefence7Wave(iNowStage, iNextWave, kKeyData, kWaveData) )	// Wave�� ������ 0���� �����ϹǷ�...
				{					
					CONT_MISSION_DEFENCE7_WAVE::value_type const& kWaveValue = kWaveData.kCont.at(0);

					for(int iMon=0; iMon<MAX_DEFENCE7_WAVE_MONSTER; ++iMon)
					{
						if( 0 < kWaveValue.iMonster[iMon] )
						{
							++iTotalMonsterCount;
						}
					}
				}
				else
				{
					break;
				}
			}while( 0 < kKeyData.iWave_No );
		}break;	
	default:
		{
		}break;
	}

	return iTotalMonsterCount;
}

void PgMissionGround::SetStageTimePlus(int const iTimePlus)
{
	// �ð� ����
	if( 0 < iTimePlus )
	{
		m_dwStage_Time += iTimePlus;
	}

	HRESULT hRet(S_OK);

	BM::Stream kPacket(PT_M_C_ANS_DEFENCE_TIMEPLUS);
	kPacket.Push(hRet);
	kPacket.Push(static_cast<int>(iTimePlus));
	Broadcast(kPacket);
}

//=============================================================================================
//===== Defence7 ==============================================================================
//=============================================================================================

bool PgMissionGround::GetDefence7Stage(int const iStage, CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type& kElement)const
{
	int const iPartyNumber = PgMission::m_kConUser.size();

	switch( m_iModeType )
	{
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			const CONT_MISSION_DEFENCE7_STAGE_BAG *pkStage;
			g_kTblDataMgr.GetContDef(pkStage);
			if( !pkStage )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_STAGE_BAG is NULL") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
			}
			else
			{
				CONT_MISSION_DEFENCE7_STAGE_BAG::key_type kKey(GetMissionNo(), m_iModeType, iPartyNumber, iStage);

				CONT_MISSION_DEFENCE7_STAGE_BAG::const_iterator stage_itr = pkStage->find(kKey);		
				if( stage_itr == pkStage->end() )
				{
					// ���̻� ������ �������� ����
					return false;
				}
				else
				{
					kElement = (*stage_itr).second;

					return true;
				}
			}
		}break;
	default:
		{
		}break;
	}

	return false;
}


bool PgMissionGround::GetDefence7Wave(int const iStage, int const iWaveNo, CONT_MISSION_DEFENCE7_WAVE_BAG::key_type& kKey, CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type& kElement)const
{	
	const CONT_MISSION_DEFENCE7_WAVE_BAG *pkWave;
	g_kTblDataMgr.GetContDef(pkWave);
	if( !pkWave )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_WAVE_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkWave is NULL"));
	}
	else
	{
		int const iPartyNumber = PgMission::m_kConUser.size();

		CONT_MISSION_DEFENCE7_WAVE_BAG::key_type kKey(GetMissionNo(), m_iModeType, iPartyNumber, iStage, iWaveNo);

		CONT_MISSION_DEFENCE7_WAVE_BAG::const_iterator wave_iter = pkWave->find(kKey);
		if( wave_iter == pkWave->end() )
		{
			// ���̻� ������ ���̺갡 ����.
			return false;
		}
		else
		{
			kKey = (*wave_iter).first;
			kElement = (*wave_iter).second;

			return true;
		}		
	}

	return false;
}

void PgMissionGround::Defence7WaveMonsterGenerate(CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type& kElement)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_MISSION_DEFENCE7_WAVE::value_type &kWave = kElement.kCont.at(0);

	if( (INDUN_STATE_PLAY == m_eState) &&(0 == m_iNowStage) && (0 == m_iNowWave) )
	{
		// ���� ���� ���� ���� �������Ƿ� �� �ʿ� ����
		//++m_iNowWave;

		return;
	}

	int iObjectAttackMonCount = 0;
	int iObjectAttackMonMaxCount = 0;

	for(int i=0; i<MAX_DEFENCE7_WAVE_MONSTER; ++i)
	{
		if( 0 < kWave.iMonster[i] )
		{
			++iObjectAttackMonMaxCount;
		}
	}

	if( 0 < ms_iDefence7ObjectAttackMonCount )
	{
		iObjectAttackMonMaxCount = (iObjectAttackMonMaxCount * ms_iDefence7ObjectAttackMonCount / 100);
	}
	if( 0 > iObjectAttackMonMaxCount )
	{
		iObjectAttackMonMaxCount = 0;
	}
	
	int iNextGive_StrategicPoint = kWave.iGive_StrategicPoint;
	int DamageDecRate = kWave.DamageDecRate;

	int iDefenceMode = TEAM_RED;
	/*if( IsDefenceMode8() )
	{
		iDefenceMode = TEAM_BLUE;
	}*/

	BM::GUID kMonsterGuid;
	for(int iCount=0; iCount<iDefenceMode; ++iCount)
	{
		bool bMonsterZen = false;
        BYTE bySelectMonsterGen = 0;
		for(int i=0; i<MAX_DEFENCE7_WAVE_MONSTER; ++i)
		{
			POINT3 kRegenPos(0, 0, 0);	

			char acSpawnName[64] = {0,};		
			//if( 0 == iCount )
			{
				::sprintf_s(acSpawnName, 64, "B_Mon_%d", i );
			}
			/*else
			{
				::sprintf_s(acSpawnName, 64, "R_Mon_%d", i );
			}*/

			if( S_OK == PgGround::FindTriggerLoc(acSpawnName, kRegenPos) )
			{
				bMonsterZen = true;

				NxRay kRay(NxVec3(kRegenPos.x, kRegenPos.y, kRegenPos.z), NxVec3(0, 0, -1.0f));
				NxRaycastHit kHit;
				NxShape *pkHitShape = RayCast(kRay, kHit, 50.0f);
				if(pkHitShape)
				{
					kRegenPos.z = kHit.worldImpact.z;
				}

				TBL_DEF_MAP_REGEN_POINT kRegenInfo;
				kRegenInfo.iMapNo = GetGroundNo();
				kRegenInfo.pt3Pos = kRegenPos;
				kRegenInfo.iTunningNo = kWave.iTunningNo;

				int const iMonsterNo = kWave.iMonster[i];

				if( 0 < iMonsterNo )
				{
					if( S_OK == InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid ) )
					{
                        if(i < 5)
                        {
                            bySelectMonsterGen |= 0x01;
                        }
                        else
                        {
                            bySelectMonsterGen |= 0x02;
                        }

						CUnit* pkUnit = PgObjectMgr::GetUnit(kMonsterGuid);
						if( pkUnit )
						{
							// Speed
							int const iMoveSpeed = pkUnit->GetAbil(AT_MOVESPEED);
							int iSpeed = iMoveSpeed + (iMoveSpeed * kWave.iAddMoveSpeedPercent / 100.0f);

							if( 0 < iSpeed )
							{
								pkUnit->SetAbil(AT_MOVESPEED, iSpeed);
								pkUnit->NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL);
							}

							// Exp
							int const iAddExp = kWave.iAddExpPercent;
							if( 0 < iAddExp )
							{
								int const iAddExpPer = std::max(iAddExp, 0);
								pkUnit->SetAbil(AT_ADD_EXP_PER, iAddExpPer);
							}

							// Hp
							int const iHP = pkUnit->GetAbil(AT_HP);
							int const iRateHP = kWave.iAddHPPercent;
							if(	0 < iRateHP )
							{
								int const iMaxHP = pkUnit->GetAbil(AT_MAX_HP); 

								//int const iAddHP = iHP + (iHP * pkUnit->GetAbil(AT_C_HP_POTION_ADD_RATE) + iRateHP * iMaxHP) / ABILITY_RATE_VALUE;

								__int64 i64HP = static_cast<__int64>(iHP);
								__int64 i64RateHP = static_cast<__int64>(iRateHP);
								__int64 i64MaxHP = static_cast<__int64>(iMaxHP);
								int const iAddHP = static_cast<int>(i64HP + (i64HP * pkUnit->GetAbil(AT_C_HP_POTION_ADD_RATE) + i64RateHP * i64MaxHP) / ABILITY_RATE_VALUE);

								int const iRetHP = std::max(iMaxHP, iAddHP);
								int const iRetMaxHP = std::max(iMaxHP, iAddHP);

								pkUnit->SetAbil(AT_MAX_HP, iRetMaxHP, true, true);
								pkUnit->SetAbil(AT_C_MAX_HP, 0);
                                pkUnit->SetAbil(AT_HP, pkUnit->GetAbil(AT_C_MAX_HP), true, true);
							}

							// Damage
							/*int const iAddDamage = kWave.iAddDamagePercent;
							pkUnit->AddAbil(AT_R_PHY_ATTACK_MAX, iAddDamage);
							pkUnit->AddAbil(AT_R_PHY_ATTACK_MIN, iAddDamage);
							pkUnit->AddAbil(AT_R_MAGIC_ATTACK_MAX, iAddDamage);
							pkUnit->AddAbil(AT_R_MAGIC_ATTACK_MIN, iAddDamage);*/

							// ��������Ʈ ����
							pkUnit->SetAbil(AT_STRATEGIC_POINT, iNextGive_StrategicPoint, true, true);

							// ���� ���� �Ӽ� ����
							SetMonsterAttack(pkUnit, iObjectAttackMonCount, iObjectAttackMonMaxCount);

							// ������ ���� ���� ����.
							pkUnit->SetAbil(AT_R_ABS_DEC_DMG_PHY, DamageDecRate, true, true);
							pkUnit->SetAbil(AT_R_ABS_DEC_DMG_MAGIC, DamageDecRate, true, true);

							// ���� �� ����
							if( IsDefenceMode8() )
							{
								int iTeam = ( TEAM_RED == iDefenceMode ) ? TEAM_BLUE : TEAM_RED;
								pkUnit->SetAbil(AT_TEAM, iTeam, true, true);
							}

                            if( 0==m_iNowStage && 0==m_iNowWave )
                            {
                                pkUnit->SetAbil(AT_CANNOT_DAMAGE, 1, true, true);
                            }
						}
					}
				}
			}
		}
		if( true == bMonsterZen )
		{
			BM::Stream kPacket(PT_NFY_M_C_DEFENCE_ZEN_EFFECT);
			kPacket.Push( bySelectMonsterGen );
			Broadcast(kPacket);
		}
	}
}

void PgMissionGround::SetWaveMonster()
{
	if(true==m_bFailMission)
	{
		return;
	}
	if(false==m_bDefence_SetWaveMonster)
	{
		return;
	}
	m_bDefence_SetWaveMonster = false;
	m_bDefence_NextStage = false;

	bool bDefenceStage = false;
    StartAI();

	// Wave ���� ���� �� ���� ����
	switch( m_iModeType )
	{
	case MO_DEFENCE:
		{
			CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kStageData;
			if( true == GetDefenceStage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
			{
				CONT_MISSION_DEFENCE_STAGE::value_type const& kValue = kStageData.kCont.at(0);

				if( true == m_bSelectSuccess )
				{
					AddMonsterEnchant();
					m_bSelectSuccess = false;
				}
				
				// �������� �ð� ����
				//eMonsterGen = D_RANDOM;
				m_dwStage_Time = kValue.iStage_Time;
				m_dwStage_StartTime = BM::GetTime32();
				//m_dwStage_Delay = kValue.iStage_Delay;

				// �ð� �ֱ�
				int iStageStartTime = 0;
				if( 0 < m_iStageTimePlus )
				{
					m_dwStage_Time += m_iStageTimePlus;			
					m_iStageTimePlus = 0;
				}
				iStageStartTime = m_dwStage_Time;

				BM::Stream kCPacket(PT_M_C_NFY_DEFENCE_STAGETIME);
				kCPacket.Push(static_cast<int>(iStageStartTime));
				Broadcast(kCPacket);
			}
		}break;
	case MO_DEFENCE7:
	case MO_DEFENCE8:
		{
			m_dwStage_StartTime = BM::GetTime32();
		}break;
	default:
		{
		}break;
	}

	// Wave ���� �ض�	
	if( D_RANDOM == eMonsterGen )
	{
		EDefenceMonsterGen kMonsterGen = (BM::Rand_Index(2) == 0 ? D_LEFT : D_RIGHT);
		eMonsterGen = kMonsterGen;
	}

	if( 0 != m_iNowWave )
	{
		BM::Stream kDefencePacket(PT_M_C_NFY_DEFENCE_WAVE);
		kDefencePacket.Push( false );
		kDefencePacket.Push( static_cast<BYTE>(eMonsterGen) );
		Broadcast(kDefencePacket);
	}
	else
	{
		SetDefenceGroundInfo();
	}
}

void PgMissionGround::EndDefence7Mission()
{
	BM::CAutoMutex Lock(m_kRscMutex);

    if( m_bGuardianTunningLevelChange )
    {
        m_bGuardianTunningLevelChange = false;
        TunningLevel( GetTunningLevel() );
        SendWarnMessageBroadcast(401173);
        ModifyInstallGuardian();
    }

	if( false == IsDefenceGameMode() )
	{
		return;
	}

	m_bDefenceStageStart = true;
	if( false == m_bDefenceEndStage )
	{
		m_bDefenceEndStage = true;
	}
	else
	{
		return;
	}

	AddStageTeamStrategicPoint();

	int iAddItemResult_No = 0;
	int iAddItemResult_Count = 0;
	bool bResultItem = false;
	int iRetContNo = 0;
	DWORD dwStageExp = 0;
	DWORD const kNow = BM::GetTime32();
	
	CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kStageData;
	if( true == GetDefence7Stage(m_iNowStage+1, kStageData) )
	{
		CONT_MISSION_DEFENCE7_STAGE::value_type const& kValue = kStageData.kCont.at(0);		

		bResultItem = ((0 < kValue.iResult_ItemNo || 0 < kValue.iResult_No) ? true : false);
	}

	bool bLastStage = IsDefenceLastStage();
	SMissionKey const & kMissionKey = GetMissionKey();
	BYTE kWriteType = ( IsDefenceMode7() ) ? PgMission_Result::VIEWTYPE_DEFENCE7 : PgMission_Result::VIEWTYPE_DEFENCE8;

	BM::Stream kPacket;
	kPacket.Push(static_cast<BYTE>(kWriteType));
	kPacket.Push(GetConstellationKey().PartyGuid.IsNotNull());
	kPacket.Push(static_cast<bool>(bLastStage));
	kPacket.Push(GetMissionNo());				
	kPacket.Push(GetDefenceStageCount());
	kMissionKey.WriteToPacket(kPacket);
	kPacket.Push(bResultItem);

	if( true == bResultItem )
	{
		kPacket.Push(m_kConUser.size());
		ConUser::iterator user_itr;
		for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
		{			
			PgPlayer* pkUser = GetUser(user_itr->first);
			if ( pkUser )
			{
				SMissionPlayerInfo_Client kInfo;

				kInfo.kCharGuid = pkUser->GetID();
				kInfo.iLevel = pkUser->GetAbil(AT_LEVEL);
				kInfo.kClass = pkUser->GetAbil(AT_CLASS);
				kInfo.wstrName = pkUser->Name();

				if(GetConstellationKey().PartyGuid.IsNull())
				{
					RESULT_SITEM_BAG_LIST_CONT	kConRetItem;
					RESULT_SITEM_BAG_LIST_CONT	kContItem;
					int iResultRewardItemCount = 0;
	
					CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kStageData;
					if( true == GetDefence7Stage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
					{
						CONT_MISSION_DEFENCE7_STAGE::value_type const& kValue = kStageData.kCont.at(0);	
						iResultRewardItemCount = kValue.iResult_Count;
						iRetContNo = kValue.iResult_No;	
						iAddItemResult_No = kValue.iResult_ItemNo;
						iAddItemResult_Count = kValue.iResult_ItemCount;
					}
	
					if( ms_iRewardItemMax < iResultRewardItemCount )
					{
						iResultRewardItemCount = ms_iRewardItemMax;
	
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Mission ResultItem Reward Count Error! MissionNo[") << GetMissionNo() << _T("] GroundNo[") << GetGroundNo() << _T("]") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
					}
	
					PgAction_PopItemContainer kTempPopItem( CIE_Mission, GroundKey(), iRetContNo, GetEventAbil() );
					kTempPopItem.DoAction(pkUser, iResultRewardItemCount, false);
	
	
					if( ms_iRewardItemMax < kTempPopItem.ConRetItemNo().size() )
					{
						// ���� ������ 3~4��, �Ӵ��� 1�� �ִ� 4�� �̻� �� ���� ����.
						iResultRewardItemCount = ms_iRewardItemMax - 1;
					}
	
					PgAction_PopItemContainer kPopItem( CIE_Mission, GroundKey(), iRetContNo, GetEventAbil() );
					kPopItem.DoAction(pkUser, iResultRewardItemCount, true);
					kPopItem.SwapRetItem(kContItem);
	
					if( 0 == kContItem.size() )
					{
						// ��� �������� ���ٸ�... "��"���� ó���� ������
						for(int i=0; i<iResultRewardItemCount; ++i)
						{
							PgBase_Item kItem;
							if(S_OK == CreateSItem(iResultItem, 1, PgItemRarityUpgradeFormula::GetItemRarityContorolType(CIE_Mission), kItem))
							{
								kConRetItem.push_back(kItem);
							}
						}
						kConRetItem.swap(kContItem);
						kConRetItem.clear();
					}
					kContItem.swap(kInfo.kContItem);
					kInfo.iResultBagNo = m_kMissionResult.iRouletteBagGrpNo;
					kInfo.iScore = 0;
				}
				kInfo.WriteToPacket(kPacket, static_cast<__int64>(0));
			}
		}

		ConstellationResultInit();
		if(IsDefenceMode7())
		{
			m_kConstellationResult.SetDelayTime(2);
		}
	}

	PgBase_Item kWinItem;
	int iWinTeam = TEAM_NONE;
	/*if( IsDefenceMode8() && bLastStage )
	{
		CONT_TEAM_POINT::mapped_type kRedPoint = 0;
		CONT_TEAM_POINT::mapped_type kBluePoint = 0;

		CONT_TEAM_POINT::const_iterator iter = m_kTeamPoint.find(TEAM_RED);
		if( m_kTeamPoint.end() != iter )
		{
			kRedPoint = (*iter).second;
		}
		iter = m_kTeamPoint.find(TEAM_BLUE);
		if( m_kTeamPoint.end() != iter )
		{
			kBluePoint = (*iter).second;
		}

		if( kRedPoint > kBluePoint )
		{
			iWinTeam = TEAM_RED;
		}
		else if( kRedPoint < kBluePoint )
		{
			iWinTeam = TEAM_BLUE;
		}

		int iWinItemNo = 0;
		int iWinItemCount = 0;
		const CONT_MISSION_DEFENCE7_MISSION_BAG *pkMission;
		g_kTblDataMgr.GetContDef(pkMission);
		if( !pkMission )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_MISSION_BAG is NULL") );
			return;
		}	
		else
		{		
			CONT_MISSION_DEFENCE7_MISSION_BAG::key_type		kKey(GetMissionNo(), m_iModeType);
			CONT_MISSION_DEFENCE7_MISSION_BAG::const_iterator iter = pkMission->find(kKey);
			if( iter != pkMission->end() )
			{
				SMISSION_DEFENCE7_MISSION kValue = iter->second.kCont.at(0);
				iWinItemNo = kValue.iWin_ItemNo;
				iWinItemCount = kValue.iWin_ItemCount;
			}
		}

		if( iWinTeam != TEAM_NONE )
		{
			// Win
			if(S_OK == CreateSItem(iWinItemNo, iWinItemCount, GIOT_NONE, kWinItem))
			{
			}
		}
		//else
		//{
		//	// Draw
		//	if(S_OK == CreateSItem(iItemNo, iCount, GIOT_NONE, kWinItem))
		//	{
		//	}
		//}
	}*/

	ConUser::iterator user_itr;
	for( user_itr=m_kConUser.begin(); user_itr != m_kConUser.end(); ++user_itr )
	{			
		PgPlayer* pkUser = GetUser(user_itr->first);
		if ( pkUser )
		{
			PgAction_QuestMissionUtil::ProcessQuestMission(GetModeType(), GroundKey(), GetMissionKey(), pkUser, MRANK_NONE, GetStageCountExceptBonus(), m_iNowStage, true);

			size_t const kCoinCount = pkUser->GetInven()->GetTotalCount(GADA_COIN_NO);
			int iNeedCount = 1;			
			bool bGadaRet = GetGadaCoinCount(iNeedCount);

			//BM::Stream kCPacket(PT_M_C_NFY_DEFENCE_ENDSTAGE);

			BM::Stream kCPacket(PT_N_C_NFY_MISSION_RESULT);
			kCPacket.Push(kPacket);
			
			bool IsMaster = m_kLocalPartyMgr.IsMaster(pkUser->GetPartyGuid(), pkUser->GetID());

			kCPacket.Push(IsMaster);
			kCPacket.Push(static_cast<int>(kCoinCount));
			kCPacket.Push(static_cast<int>(iNeedCount));
			kCPacket.Push(iRetContNo);
			kCPacket.Push(PgMission::GetTriggerType());
			kCPacket.Push(iAddItemResult_No);			

			CUnit* pkUnit = GetUnit(pkUser->GetID());
			if( pkUnit )
			{
				if( IsDefenceMode7() || IsDefenceMode8() )
				{
					if( (0 < iAddItemResult_No) && (0 < iAddItemResult_Count) )					
					{
						PgBase_Item kResultItem;
						if(S_OK == CreateSItem(iAddItemResult_No, iAddItemResult_Count, GIOT_NONE, kResultItem))
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkUnit->GetID(), SPMOD_Insert_Fixed(kResultItem, SItemPos(), true)));
							if( !kOrder.empty() )
							{
								PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_DefenceWin, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkUnit, NULL);
							}
						}
					}
				}

				if( IsDefenceMode8() && bLastStage )
				{
					/*if( pkUser->GetAbil(AT_TEAM) == iWinTeam )
					{
						if( !kWinItem.IsEmpty() )
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkUnit->GetID(), SPMOD_Insert_Fixed(kWinItem, SItemPos(), true)));
							if( !kOrder.empty() )
							{
								PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_DefenceWin, GroundKey(), kOrder);
								kItemModifyAction.DoAction(pkUnit, NULL);
							}
						}
					}*/
					kCPacket.Push(iWinTeam);
					kCPacket.Push(kWinItem.ItemNo());
				}
			}

			pkUser->Send(kCPacket);
		}
	}
}

int PgMissionGround::GetDefence7WaveStrategicPoint(int const iStage, int const iWaveNo)
{	
	const CONT_MISSION_DEFENCE7_WAVE_BAG *pkWave;
	g_kTblDataMgr.GetContDef(pkWave);
	if( !pkWave )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_WAVE_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkWave is NULL"));
	}
	else
	{
		int const iPartyNumber = PgMission::m_kConUser.size();

		CONT_MISSION_DEFENCE7_WAVE_BAG::key_type kKey(GetMissionNo(), m_iModeType, iPartyNumber, iStage, iWaveNo);

		CONT_MISSION_DEFENCE7_WAVE_BAG::const_iterator wave_iter = pkWave->find(kKey);
		if( wave_iter == pkWave->end() )
		{
			return 0;
		}
		else
		{
			CONT_MISSION_DEFENCE7_WAVE::value_type const& kWaveValue = wave_iter->second.kCont.at(0);		

			return kWaveValue.iGive_StrategicPoint;
		}		
	}

	return 0;
}

void PgMissionGround::SetDefenceItemList()
{
/*
	BM::CAutoMutex Lock(m_kRscMutex);

	m_kContDefenceItemList.clear();

	const CONT_MISSION_DEFENCE7_MISSION_BAG *pkMission;
	g_kTblDataMgr.GetContDef(pkMission);
	if( !pkMission )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_MISSION_BAG is NULL") );
		return;
	}	
	else
	{		
		CONT_MISSION_DEFENCE7_MISSION_BAG::key_type		kKey(GetMissionNo(), m_iModeType);

		CONT_MISSION_DEFENCE7_MISSION_BAG::const_iterator iter = pkMission->find(kKey);
		if( iter != pkMission->end() )
		{
			SMISSION_DEFENCE7_MISSION kValue = iter->second.kCont.at(0);

			for(int iItemCount = 0; iItemCount<MAX_DEFENCE7_MISSION_ITEM; ++iItemCount)
			{
				int const iValue = kValue.iItem_F7[iItemCount];
				if( 0 < iValue )
				{
					m_kContDefenceItemList.insert(iValue);
				}
			}
		}
	}
*/
}

bool PgMissionGround::IsDefenceItemList(int const iItemNo)const
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( IsDefenceMode7() || IsDefenceMode8() )
	{
		CONT_SET_DATA::const_iterator iter = m_kContDefenceItemList.find(iItemNo);
		return ( m_kContDefenceItemList.end() != iter );
	}
	return false;
}

bool PgMissionGround::PushMissionDefenceInvenItem(CUnit* pkUnit, PgBase_Item const& rkItem)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( pkUnit )
	{
		int const iItemNo = rkItem.ItemNo();
		if( IsDefenceItemList(iItemNo) )
		{
			BM::GUID const rkGuid = pkUnit->GetID();

			if( rkGuid.IsNotNull() )
			{
				CONT_DEFENCE_ITEM::iterator iter = m_kContDefenceItem.find(rkGuid);
				if( m_kContDefenceItem.end() == iter )
				{
					auto kPair = m_kContDefenceItem.insert(std::make_pair(rkGuid, CONT_DEFENCE_ITEM::mapped_type()));
					iter = kPair.first;
				}
				else
				{
					CONT_DEFENCE_ITEM::mapped_type& kValue = iter->second;
					CONT_DEFENCE_ITEM::mapped_type::iterator find_iter = std::find(kValue.begin(), kValue.end(), iItemNo);
					if( kValue.end() == find_iter )
					{
						if( DEFENCE_ITEM_INV_COUNT <= kValue.size() )
						{					
							kValue.erase(kValue.begin());
						}
					}
					else
					{
						// �����Ѱ� ������ �н�
						return false;
					}
				}

				(*iter).second.push_back(iItemNo);

				CONT_DEFENCE_ITEM::mapped_type kSendValue = iter->second;
				SendToUserItemList(pkUnit, kSendValue);

				return true;
			}
		}
	}
	return false;
}

void PgMissionGround::Defence7ItemUse(CUnit* pkUnit, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	int iItemNo = 0;
	SItemPos kItemPos;
	int iOption = 0;

	pkPacket->Pop( iItemNo );
	pkPacket->Pop( kItemPos );
	pkPacket->Pop( iOption );

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pItemDef = kItemDefMgr.GetDef(iItemNo);
	if( pkUnit && pItemDef )
	{
		BM::GUID const rkGuid = pkUnit->GetID();

		T_GNDATTR const iNotAttr = (T_GNDATTR)(pItemDef->GetAbil(AT_NOTAPPLY_MAPATTR));
		T_GNDATTR const iCanAttr = (T_GNDATTR)(pItemDef->GetAbil(AT_CAN_GROUND_ATTR));
		bool bCantUseGround = (0 != (iNotAttr & GetAttr()));
		bCantUseGround = ((0 != iCanAttr)? 0 == (GetAttr() & iCanAttr): false) || bCantUseGround;
		if( bCantUseGround )
		{
			// ����� �� ���� ����Դϴ�.
			pkUnit->SendWarnMessage(20027);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return;
		}

		bool bItemList = IsDefenceItemList(iItemNo);
		bool bLevelCheck = IsMissionItemLevel(pItemDef->GetAbil(AT_MISSIONITEM_LEVELTYPE));

		if( bLevelCheck )
		{
			int const iCustomKind = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
			int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

			if( UICT_MISSION_DEFENCE7_ITEM != iCustomKind )
			{
				// �ű� ���ҽ� �������� �ƴϴ�.
				return;
			}

			switch( iCustomType )
			{
			case DI_ADD_POINT:
				{
					int const iCustomValue2 = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);

					int const iTeam = std::max(pkUnit->GetAbil(AT_TEAM), 1);
					int iStrategicPoint = std::max(iCustomValue2, 0);

					AddTeamStrategicPoint(iTeam, iStrategicPoint);

					int OriValue = std::max(iCustomValue2, 0);
					int MulValue = std::max(iStrategicPoint - OriValue, 0);
					SendStrategicPoint(iTeam, OriValue, MulValue);
				}break;						
			case DI_EFFECT_DAMAGE:
			case DI_EFFECT_MOVESPEED:
			case DI_EFFECT_MOVESTOP:
				{
					int const iNewEffect1 = pItemDef->GetAbil(AT_EFFECTNUM1);							

					VEC_GUID kMemberVec;
					int iTeam = (pkUnit->GetAbil(AT_TEAM) == TEAM_RED) ? TEAM_BLUE : TEAM_RED;
					GetTeamPartyMember(iTeam, kMemberVec);

					for(VEC_GUID::const_iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
					{
						VEC_GUID::value_type const& kValue = (*iter);
						CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
						if( pkTargetUnit )
						{
							Defence7AddEffect(pkUnit, pkTargetUnit, iItemNo, iNewEffect1);
						}
					}
				}break;
			case DI_ADD_MONSTER:
				{
					int const iMonsterNo = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
					if( 0 >= iMonsterNo )
					{
						return;
					}

					POINT3 kRegenPos(0, 0, 0);	
					char acSpawnName[64] = {0,};

					int iTeam = (pkUnit->GetAbil(AT_TEAM) == TEAM_RED) ? TEAM_BLUE : TEAM_RED;

					//if( TEAM_RED == iTeam )
					{
						::sprintf_s(acSpawnName, 64, "B_Mon_5");
					}
					/*else
					{
						::sprintf_s(acSpawnName, 64, "R_Mon_5");
					}*/

					if( S_OK != PgGround::FindTriggerLoc(acSpawnName, kRegenPos) )
					{
						return;
					}

					TBL_DEF_MAP_REGEN_POINT kRegenInfo;
					kRegenInfo.iMapNo = GetGroundNo();
					kRegenInfo.pt3Pos = kRegenPos;
					//kRegenInfo.iTunningNo = kWave.iTunningNo;
					BM::GUID kMonsterGuid;

					if( S_OK != InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid ) )
					{
						return;
					}
				}break;
			case DI_ADD_GUARDIAN:
				{
					int const iGuardianNo = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
					int const iLifeTime = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);
					if( 0 >= iGuardianNo )
					{
						return;
					}

					const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
					g_kTblDataMgr.GetContDef(pkGuardian);

					if( !pkGuardian )
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
						return;
					}

					CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardianNo);

					CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
					if( pkGuardian->end() == iter )
					{
						return;
					}
					SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);
					
					PgGround *pkGround = dynamic_cast<PgGround*>(this);
					if( !pkGround )
					{
						return;
					}

					BM::vstring kNodeName("Entity");
					POINT3 kNodePos = pkUnit->GetPos();

					SCreateEntity kCreateInfo;
					kCreateInfo.kClassKey.iClass = kValue.iMonsterNo;
					kCreateInfo.kClassKey.nLv = kValue.iUpgrade_Step;
					kCreateInfo.bUniqueClass = false;							
					//kCreateInfo.bEternalLife = true;	// �̶� �����Ǵ� ������� ������ Ÿ���� ������.
                    kCreateInfo.iTunningNo = GetGuardianTunningNo(iGuardianNo);
                    kCreateInfo.iTunningLevel = m_iTunningLevel;
                    kCreateInfo.iLifeTime = iLifeTime;
					kCreateInfo.kGuid.Generate();

					// �ٴڿ� ���� ��Ų��
					NxRay kRay(NxVec3(kNodePos.x, kNodePos.y, kNodePos.z+20), NxVec3(0, 0, -1.0f));
					NxRaycastHit kHit;
					NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
					if(pkHitShape)
					{
						kNodePos.z = kHit.worldImpact.z;
					}
					kCreateInfo.ptPos = kNodePos;

					CUnit* pkEntityUnit = pkGround->CreateGuardianEntity(pkUnit, &kCreateInfo, kNodeName);
					if( !pkEntityUnit )
					{
						return;
					}
					SetGuardianAbil(pkEntityUnit, kValue);
				}break;
			case DI_EFFECT_BOMB:
			case DI_ATTACK:
			case DI_GUARDIAN:
				{
				}break;
			case DI_D7_RELAY_STAGE:
				{
					if(UICT_DEFENCE7_RELAY != pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2))
					{
						return;
					}

					if( IsDefenceMode7() )
					{
						if(false==m_bFailMission)
						{
							return;
						}

						CONT_PLAYER_MODIFY_ORDER kOrder;
						SPMO kIMO(IMET_ADD_ANY, pkUnit->GetID(), SPMOD_Add_Any(iItemNo,-1));
						kOrder.push_back(kIMO);

						BM::Stream kPacket;
						kPacket.Push(iOption);
						PgAction_ReqModifyItem kItemModifyAction(CIE_Defence7_Relay_Stage, GroundKey(), kOrder, kPacket);
						kItemModifyAction.DoAction(pkUnit, NULL);
						return;
					}
				}break;
			case DI_D7_POINT_COPY:
				{
					if( UICT_DEFENCE7_POINT_COPY != pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2) )
					{
						return;
					}

					if( PointCopying() )
					{
						pkUnit->SendWarnMessage(799398); // �̹� ����Ʈ �����Ⱑ ��� ���Դϴ�.
						return;
					}

					if( m_iNowStage < 10 )
					{	// 11������������ ��밡��..
						pkUnit->SendWarnMessage(799396); // 11Stage���� ��� �����մϴ�.
						return;
					}

					if( IsDefenceMode7() )
					{
						if( m_bFailMission )
						{
							return;
						}

						CONT_PLAYER_MODIFY_ORDER Order;
						SPMO IMO(IMET_ADD_ANY, pkUnit->GetID(), SPMOD_Add_Any(iItemNo, -1));
						Order.push_back(IMO);

						PgAction_ReqModifyItem ItemModifyAction(CIE_Defence7_Point_Copy, GroundKey(), Order);
						ItemModifyAction.DoAction(pkUnit, NULL);
						return;
					}
					else
					{
						// ����� �� ���� ����Դϴ�.
						pkUnit->SendWarnMessage(20027);
					}
				}break;
			default:
				{
					return;
				}break;
			}

			if( !bItemList )
			{
				PgInventory* pkInv = pkUnit->GetInven();
				if( !pkInv )
				{
					return;
				}

				PgBase_Item kItem;
				if( S_OK == pkInv->GetItem(kItemPos, kItem) )
				{
					CONT_PLAYER_MODIFY_ORDER kOrder;
					SPMOD_Modify_Count kDelData(kItem, kItemPos, -1);
					SPMO kIMO(IMET_MODIFY_COUNT, pkUnit->GetID(), kDelData);
					kOrder.push_back(kIMO);

					PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_UseItem, GroundKey(), kOrder);
					kItemModifyAction.DoAction(pkUnit, NULL);
				}
			}
		}
        else
        {
            pkUnit->SendWarnMessage(20029);
        }

		if( bItemList )
		{
			CONT_DEFENCE_ITEM::iterator iter = m_kContDefenceItem.find(rkGuid);
			if( m_kContDefenceItem.end() != iter )
			{
				CONT_DEFENCE_ITEM::mapped_type& kValue = iter->second;
				CONT_DEFENCE_ITEM::mapped_type::iterator find_iter = std::find(kValue.begin(), kValue.end(), iItemNo);
				if( kValue.end() != find_iter )
				{
					// ������ ��� ����
					kValue.erase(find_iter);
					SendToUserItemList(pkUnit, kValue);
				}
			}
		}
		else if( bLevelCheck )
		{
		}
	}
}

void PgMissionGround::GetTeamPartyMember(int const iTeam, VEC_GUID& kGuidVec)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	kGuidVec.clear();
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgPlayer* pkPlayer = NULL;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		if( iTeam ==  pkPlayer->GetAbil(AT_TEAM) )
		{
			kGuidVec.push_back(pkPlayer->GetID());
		}
	}
}

void PgMissionGround::Defence7AddEffect(CUnit* pkCaster, CUnit* pkTarget, int const iItemNo, int const iNewEffect)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	PgGround *m_pkGround = dynamic_cast<PgGround*>(this);
	if( !m_pkGround )
	{
		return;
	}

	SActArg kActArg;
	PgGroundUtil::SetActArgGround(kActArg, m_pkGround);
	kActArg.Set(ACTARG_ITEMNO, iItemNo);

	if( 0 >= iItemNo )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"ItemNo : " << iItemNo);
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pItemDef = kItemDefMgr.GetDef(iItemNo);

	if( (0 < iNewEffect) && pItemDef )
	{
		SEffectCreateInfo kCreate;
		kCreate.eType = EFFECT_TYPE_ITEM;
		kCreate.iEffectNum = iNewEffect;
		kCreate.kActArg = kActArg;
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

		int const iEffectTimeType = pItemDef->GetAbil( AT_DURATION_TIME_TYPE );
		if ( E_TIME_ELAPSED_TYPE_WORLDTIME == iEffectTimeType )
		{
			int const iDurTime = pItemDef->GetAbil( AT_DURATION_TIME );

			SYSTEMTIME kLocalTime;
			g_kEventView.GetLocalTime(&kLocalTime);
			CGameTime::AddTime(kLocalTime, iDurTime * CGameTime::MILLISECOND );
			kCreate.kWorldExpireTime = BM::DBTIMESTAMP_EX(kLocalTime);
		}

		if( pkTarget )
		{
			pkTarget->AddEffect(kCreate);
		}
		if( pkCaster && pkTarget )
		{
			::CheckSkillFilter_Delete_Effect(pkCaster, pkTarget, iNewEffect);
		}
	} 
}

void PgMissionGround::SubNeedGuardianStrategicPoint(int const iTeam, int const iGuardian, CUnit * pCaster)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return;
	}

	if( 0 < iGuardian )
	{
		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardian);

		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
		if( pkGuardian->end() != iter )
		{
			SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);

			CONT_STRATEGIC_POINT::iterator iter = m_kContStrategicPoint.find(iTeam);
			if( m_kContStrategicPoint.end() != iter )
			{
				if( 0 < kValue.iNeed_StrategicPoint )
				{
					int Need_StrategicPoint = kValue.iNeed_StrategicPoint;

					int DisCountRate  = 0;
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pCaster);
					if( pPlayer )
					{
						int SingleDiscount = pPlayer->GetGuardianDiscountRate(kValue.GuardianType);	// ��ġ�Ϸ��� Ÿ���� ����� ���η�.
						int AllDiscount = pPlayer->GetGuardianDiscountRate(GT_ALLTYPE);		// ��� Ÿ�� ���η�.

						int rate = std::max(SingleDiscount, AllDiscount);	// ���߿� ū �͸� ����.

						int DiscountCost = (Need_StrategicPoint * rate) / 10000;	// �������� ���з�.
						Need_StrategicPoint -= DiscountCost;
					}

					CONT_STRATEGIC_POINT::mapped_type kPoint = iter->second;
					kPoint = std::max(kPoint - Need_StrategicPoint, 0);

					iter->second = kPoint;
				}
			}
		}
	}
}

void PgMissionGround::SubNeedGuardianSkillPoint(int const iTeam, int const iGuardianNo, int const iSkillNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

    int const iNeedPoint = PgDefenceMissionSkill::GetGuardianSkillPoint(iGuardianNo, iSkillNo);
    if( 0==iNeedPoint )
    {
        VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("GuardianSkillPoint 0") );
        return;
    }

	CONT_STRATEGIC_POINT::iterator iter = m_kContStrategicPoint.find(iTeam);
	if( m_kContStrategicPoint.end() != iter )
	{
		if( 0 < iNeedPoint )
		{
			CONT_STRATEGIC_POINT::mapped_type kPoint = iter->second;
			kPoint = std::max(kPoint - iNeedPoint, 0);
			
			iter->second = kPoint;
		}
	}
}

bool PgMissionGround::IsNeedGuardianStrategicPoint(int const iTeam, int const iGuardian)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return false;
	}

	if( 0 < iGuardian )
	{
		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardian);

		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
		if( pkGuardian->end() != iter )
		{
			SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);

			CONT_STRATEGIC_POINT::iterator iter = m_kContStrategicPoint.find(iTeam);
			if( m_kContStrategicPoint.end() != iter )
			{
				if( 0 < kValue.iNeed_StrategicPoint )
				{
					return (kValue.iNeed_StrategicPoint <= iter->second);
				}
			}
		}
	}

	return false;
}

bool PgMissionGround::IsNeedGuardianSkillPoint(int const iTeam, int const iGuardianNo, int const iSkillNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

    int const iNeedPoint = PgDefenceMissionSkill::GetGuardianSkillPoint(iGuardianNo, iSkillNo);
    if( 0==iNeedPoint )
    {
        VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("GuardianSkillPoint 0") );
        return false;
    }

	
	CONT_STRATEGIC_POINT::iterator iter = m_kContStrategicPoint.find(iTeam);
	if( m_kContStrategicPoint.end() != iter )
	{
		if( 0 < iNeedPoint )
		{
			return (iNeedPoint <= iter->second);
		}
	}

	return false;
}

int PgMissionGround::GetGuardianStrategicPoint(int const iSlot)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	m_kContDefenceItemList.clear();

	const CONT_MISSION_DEFENCE7_MISSION_BAG *pkMission;
	g_kTblDataMgr.GetContDef(pkMission);
	if( !pkMission )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_MISSION_BAG is NULL") );
		return 0;
	}	

	CONT_MISSION_DEFENCE7_MISSION_BAG::key_type		kKey(GetMissionNo(), m_iModeType);

	CONT_MISSION_DEFENCE7_MISSION_BAG::const_iterator iter = pkMission->find(kKey);
	if( iter == pkMission->end() )
	{
		return 0;
	}

	SMISSION_DEFENCE7_MISSION kValue = iter->second.kCont.at(0);

	int iGuardianValue = 0;

	switch( iSlot )
	{
	case MDEFENCE7_MISSION_SLOT1:
		{
			iGuardianValue = kValue.iSlot_F1;
		}break;
	case MDEFENCE7_MISSION_SLOT2:
		{
			iGuardianValue = kValue.iSlot_F2;
		}break;
	case MDEFENCE7_MISSION_SLOT3:
		{
			iGuardianValue = kValue.iSlot_F3;
		}break;
	case MDEFENCE7_MISSION_SLOT4:
		{
			iGuardianValue = kValue.iSlot_F4;
		}break;
	case MDEFENCE7_MISSION_SLOT5:
		{
			iGuardianValue = kValue.iSlot_F5;
		}break;
	case MDEFENCE7_MISSION_SLOT6:
		{
			iGuardianValue = kValue.iSlot_F6;
		}break;
	case MDEFENCE7_MISSION_SLOT7:
		{
			iGuardianValue = kValue.iSlot_F7;
		}break;
	case MDEFENCE7_MISSION_SLOT8:
		{
			iGuardianValue = kValue.iSlot_F8;
		}break;
	default:
		{
			iGuardianValue = 0;
		}break;
	}

	return iGuardianValue;
}

void PgMissionGround::SetMissionInsertDropItem(POINT3 const& rkPos, int const iItemNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	VEC_GUID kOwnerVec;
	POINT3 kDropPos;
	PgBase_Item kDropItem;

	kOwnerVec.clear();
	PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Mission_Defence7 );

	PgCreateSpreadPos kAction(rkPos);
	POINT3BY const kOriented = POINT3BY();
	POINT3 Oriented(kOriented.x, kOriented.y, kOriented.z);
	Oriented.Normalize();
	int const iSpreadRange = 30;
	kAction.AddDir(PhysXScene()->GetPhysXScene(), Oriented, iSpreadRange);
	
	if(S_OK == CreateSItem(iItemNo, 1, GIOT_NONE, kDropItem))
	{
		kAction.PopPos( kDropPos );
		InsertItemBox(kDropPos, kOwnerVec, NULL, kDropItem, 0i64, kLogCont );
	}
}

int PgMissionGround::GetDefence7DropItemNo()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	int const iSize = m_kContDefenceItemList.size();

	if( 0 < iSize )
	{
		VEC_INT kVec;

		kVec.resize( iSize );
		std::copy( m_kContDefenceItemList.begin(), m_kContDefenceItemList.end(), kVec.begin() );

		std::random_shuffle(kVec.begin(), kVec.end(), BM::Rand_Index);
		VEC_INT::const_iterator iter_value = kVec.begin();
		if( kVec.end() != iter_value )
		{
			VEC_INT::value_type kValue = (*iter_value);

			if( 0 < kValue )
			{
				return kValue;
			}
		}
	}

	return 0;
}

bool PgMissionGround::GetDefence7ItemPos(int const iTeam, int const iIndex, POINT3& rkOutPos)const
{
	BM::CAutoMutex Lock(m_kRscMutex);

	POINT3 kRegenPos(0, 0, 0);	
	char acSpawnName[64] = {0,};

	//if( TEAM_RED == iTeam )
	{
		::sprintf_s(acSpawnName, 64, "B_Item_%d", iIndex);
	}
	/*else
	{
		::sprintf_s(acSpawnName, 64, "R_Item_%d", iIndex);
	}*/

	if( S_OK == PgGround::FindTriggerLoc(acSpawnName, kRegenPos) )
	{
		rkOutPos.Set(kRegenPos.x, kRegenPos.y, kRegenPos.z);
        return true;
	}

    return false;
}

void PgMissionGround::SetDropItemView()
{
	POINT3 kRegenPos(0, 0, 0);
	int const iRange = 8;

	if( IsDefenceMode7() || IsDefenceMode8() )
	{
		if( GetDefence7ItemPos(TEAM_RED, BM::Rand_Index(iRange), kRegenPos) )
        {
		    SetMissionInsertDropItem(kRegenPos, GetDefence7DropItemNo());
        }
	}

	if( IsDefenceMode8() )
	{
		if( GetDefence7ItemPos(TEAM_BLUE, BM::Rand_Index(iRange), kRegenPos) )
        {
		    SetMissionInsertDropItem(kRegenPos, GetDefence7DropItemNo());
        }
	}
}

bool PgMissionGround::GetGuardianGuid(std::wstring kName, BM::GUID& rkGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_GUARDIAN_INSTALL::iterator iter = m_kGuardianInstall.find(kName);
	if( m_kGuardianInstall.end() != iter )
	{
		rkGuid = iter->second.kGuid;
		return true;
	}
	return false;
}

bool PgMissionGround::GetGuardianNo(std::wstring kName, int& iGuardianNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_GUARDIAN_INSTALL::iterator iter = m_kGuardianInstall.find(kName);
	if( m_kGuardianInstall.end() != iter )
	{
		iGuardianNo = iter->second.iGuardianNo;
		return true;
	}
	return false;
}

bool PgMissionGround::IsGuardianPos(std::wstring kName)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_GUARDIAN_INSTALL::iterator iter = m_kGuardianInstall.find(kName);
	return (m_kGuardianInstall.end() != iter);
}

bool PgMissionGround::SetGuardian(std::wstring kName, int const iGuardianNo, BM::GUID const kGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( (0 < iGuardianNo) && (kGuid.IsNotNull()) )
	{
		SGuardian_Install kInfo;
		kInfo.iGuardianNo = iGuardianNo;
		kInfo.kGuid = kGuid;

		CONT_GUARDIAN_INSTALL::iterator iter = m_kGuardianInstall.find(kName);
		if( m_kGuardianInstall.end() == iter )
		{		
			auto kPair = m_kGuardianInstall.insert(std::make_pair(kName, CONT_GUARDIAN_INSTALL::mapped_type()));
			iter = kPair.first;
		}
		(*iter).second = kInfo;

		return true;
	}
	return false;	
}

bool PgMissionGround::DelGuardian(std::wstring kName)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_GUARDIAN_INSTALL::iterator iter = m_kGuardianInstall.find(kName);
	if( m_kGuardianInstall.end() != iter )
	{		
		m_kGuardianInstall.erase(iter);
		return true;
	}
	return false;
}

bool PgMissionGround::IsNeedSkillStrategicPoint(int const iTeam, int const iSkillNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return false;
	}

	int const iGuardian = GetSkillStrategicPoint(iSkillNo);
	if( 0 < iGuardian )
	{
		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardian);

		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
		if( pkGuardian->end() != iter )
		{
			SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);

			CONT_STRATEGIC_POINT::iterator iter = m_kContStrategicPoint.find(iTeam);
			if( m_kContStrategicPoint.end() != iter )
			{
				if( 0 < kValue.iNeed_StrategicPoint )
				{
					return (kValue.iNeed_StrategicPoint <= iter->second);
				}
			}
		}
	}

	return false;
}

int PgMissionGround::GetSkillStrategicPoint(int const iSkillNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	m_kContDefenceItemList.clear();

	const CONT_MISSION_DEFENCE7_MISSION_BAG *pkMission;
	g_kTblDataMgr.GetContDef(pkMission);
	if( !pkMission )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_MISSION_BAG is NULL") );
		return 0;
	}	

	CONT_MISSION_DEFENCE7_MISSION_BAG::key_type		kKey(GetMissionNo(), m_iModeType);

	CONT_MISSION_DEFENCE7_MISSION_BAG::const_iterator iter = pkMission->find(kKey);
	if( iter == pkMission->end() )
	{
		return 0;
	}

	SMISSION_DEFENCE7_MISSION kValue = iter->second.kCont.at(0);

	for(int i=0; i<MAX_DEFENCE7_MISSION_SKILL; ++i)
	{
		if( iSkillNo == kValue.iSkill[i] )
		{
			return iSkillNo;
		}
	}

	return 0;
}

void PgMissionGround::GetDefence7GuardianPos(int const iTeam, std::wstring const & kName, POINT3& rkOutPos)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	POINT3 kRegenPos(0, 0, 0);	
	char acSpawnName[64] = {0,};

	::sprintf_s(acSpawnName, 64, "%s", MB(kName.c_str()));


	if( S_OK == PgGround::FindTriggerLoc(acSpawnName, kRegenPos) )
	{
		rkOutPos.Set(kRegenPos.x, kRegenPos.y, kRegenPos.z);
	}
}

bool PgMissionGround::GuardianUpgradeCheck(CUnit* pkUnit, int iGuardianNo, std::wstring kName)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return false;
	}

	int iOldGuardianNo = 0;

	if( GetGuardianNo(kName, iOldGuardianNo) )
	{
		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iOldGuardianNo);

		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
		if( pkGuardian->end() != iter )
		{
			SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);
			if( GT_CHANGE == kValue.iUpgrade_Type )
			{
				for(int i=0; i<MAX_DEFENCE7_GUARDIAN_UPGRADE; ++i)
				{
					if( iGuardianNo == kValue.iUpgrade[i] )
					{
						return true;
					}
				}
			}
			else
			{
				if( iGuardianNo == kValue.iUpgrade[0] )
				{
					return true;
				}
			}

			// ���� �Ǵ� ���׷��̵带 �Ҽ� ���� �߸��� �ൿ
			if( pkUnit )
			{
				pkUnit->SendWarnMessage(401133);
			}
		}
	}
	else
	{
		// ��ġ�� ������� ����.
		if( pkUnit )
		{
			pkUnit->SendWarnMessage(401135);
		}
	}

	return false;
}

void PgMissionGround::RemoveGuardian(CUnit* pkUnit, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	int iGuardianNo = 0;
	std::wstring kTriggerName;
	POINT3 rkPos;

	pkNfy->Pop( kTriggerName );

	if( !pkUnit )
	{
		return;
	}

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return;
	}

	if( true == IsGuardianPos(kTriggerName) )
	{
		BM::GUID kEntityGuid;
		if( GetGuardianGuid(kTriggerName, kEntityGuid) )
		{
			CUnit* pkDelete = GetUnit(kEntityGuid);
			if (pkDelete != NULL && pkDelete->IsUnitType(UT_ENTITY))
			{
				PgEntity* pkDeleteEntity = dynamic_cast<PgEntity*>(pkDelete);
				if (pkDeleteEntity != NULL )
				{
					int iGuardianNo = 0;
					if( GetGuardianNo(kTriggerName, iGuardianNo) )
					{	
						CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardianNo);
						CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
						if( pkGuardian->end() != iter )
						{
							SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);
							
							bool bRet = ReleaseUnit(pkDeleteEntity);
							bRet = bRet && DelGuardian(kTriggerName);
							if( bRet )
							{
								BM::Stream kPacket(PT_M_C_ANS_GUARDIAN_SET);
								kPacket.Push(pkUnit->GetID());
								PU::TWriteTable_AM(kPacket, m_kGuardianInstall);
								Broadcast(kPacket);

								int iStrategicPoint = kValue.Sell_StrategicPoint;
								int const iTeam = pkUnit->GetAbil(AT_TEAM);								
								int iPoint = static_cast<int>(iStrategicPoint);
								AddTeamStrategicPoint(iTeam, iStrategicPoint, true);

								int OriValue = std::max(iPoint, 0);
								int MulValue = std::max(iStrategicPoint - OriValue, 0);
								SendStrategicPoint(iTeam, OriValue, MulValue);
							}
						}
					}
				}
			}
		}
	}
}


void PgMissionGround::ModifyInstallGuardian()
{
    CONT_GUARDIAN_INSTALL kContTmp;
    {
        BM::CAutoMutex Lock(m_kRscMutex);
        kContTmp = m_kGuardianInstall;
    }

    CONT_GUARDIAN_INSTALL::const_iterator iter = kContTmp.begin();
	while( kContTmp.end() != iter )
    {
        std::wstring const & rkTriggerName = iter->first;
        int const iGuardianNo = iter->second.iGuardianNo;

        RealUpgradeGuardian(iGuardianNo, rkTriggerName, NULL);
        ++iter;
    }

    BM::Stream kPacket(PT_M_C_ANS_GUARDIAN_SET);
	kPacket.Push(BM::GUID::NullData());
    PU::TWriteTable_AM(kPacket, m_kGuardianInstall);
    Broadcast(kPacket);
}

HRESULT PgMissionGround::RealUpgradeGuardian(int const iGuardianNo, std::wstring const & rkTriggerName, CUnit * pkUnit)
{
    if( 0==iGuardianNo || rkTriggerName.empty() )
    {
        return E_FAIL;
    }

    const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);
	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return E_FAIL;
	}

    PgGround *pkGround = dynamic_cast<PgGround*>(this);
    if( !pkGround )
    {
        return E_FAIL;
    }

    CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardianNo);
    CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
    if( pkGuardian->end() == iter )
    {
        return E_FAIL;
    }

    SMISSION_DEFENCE7_GUARDIAN const & kValue = iter->second.kCont.at(0);

    BM::GUID kEntityGuid;
    if( !GetGuardianGuid(rkTriggerName, kEntityGuid) )
    {
        // ������ ������� ã�� ���ߴ�.
        return E_COMMON_CANNOT_FIND;
    }

    CUnit* pkDelete = GetUnit(kEntityGuid);
    if (pkDelete != NULL && pkDelete->IsUnitType(UT_ENTITY))
    {
        if( !pkUnit )
        {
            pkUnit = GetUnit(pkDelete->Caller());
        }

        PgEntity* pkDeleteEntity = dynamic_cast<PgEntity*>(pkDelete);
        if (pkUnit && pkDeleteEntity != NULL /*&& pkDeleteEntity->Caller() == pkUnit->GetID()*/)
        {
            ContEffectItor kItor;
            CEffect* pkEffect = NULL;
            PgUnitEffectMgr const& rkEffectMgr = pkDeleteEntity->GetEffectMgr();
            rkEffectMgr.GetFirstEffect(kItor);
            GET_DEF(CEffectDefMgr, kEffectDefMgr);
			typedef std::vector<std::pair<int,unsigned long> > CONT_ADD_EFFECT;	//EffectID, GetTime
            CONT_ADD_EFFECT kAddEffect;
            while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
            {
				kAddEffect.push_back(std::make_pair(pkEffect->GetKey(),pkEffect->GetTime()));
            }

            POINT3 kNodePos;
            if(pkUnit)
            {
                GetDefence7GuardianPos(pkUnit->GetAbil(AT_TEAM), rkTriggerName, kNodePos);
            }
            else
            {
                kNodePos = pkDeleteEntity->GetPos();
            }

            bool bRet = ReleaseUnit(pkDeleteEntity);
            if( bRet )
            {
                BM::vstring kNodeName("Entity");
                //POINT3 kNodePos = pkDeleteEntity->GetPos();

                SCreateEntity kCreateInfo;
                kCreateInfo.kClassKey.iClass = kValue.iMonsterNo;
                kCreateInfo.kClassKey.nLv = kValue.iUpgrade_Step;
                kCreateInfo.bUniqueClass = false;
                kCreateInfo.bEternalLife = true;
                kCreateInfo.iTunningNo = GetGuardianTunningNo(iGuardianNo);
                kCreateInfo.iTunningLevel = m_iTunningLevel;
                kCreateInfo.kGuid.Generate();

                // �ٴڿ� ���� ��Ų��
                NxRay kRay(NxVec3(kNodePos.x, kNodePos.y, kNodePos.z+20), NxVec3(0, 0, -1.0f));
                NxRaycastHit kHit;
                NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
                if(pkHitShape)
                {
                    kNodePos.z = kHit.worldImpact.z;
                }
                kCreateInfo.ptPos = kNodePos;

                CUnit* pkEntity = pkGround->CreateGuardianEntity(pkUnit, &kCreateInfo, kNodeName);
                if( pkEntity )
                {
                    SubNeedGuardianStrategicPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo, pkUnit);
                    SendStrategicPoint(pkUnit->GetAbil(AT_TEAM), 0, 0);
                    SetGuardianAbil(pkEntity, kValue);
                    SetGuardian(rkTriggerName, iGuardianNo, pkEntity->GetID());

                    // ������ �ִ� Effect����
                    CONT_ADD_EFFECT::iterator iter = kAddEffect.begin();
                    while( kAddEffect.end() != iter )
                    {
						SActArg kActArg;
						PgGroundUtil::SetActArgGround(kActArg, this);

                        SEffectCreateInfo kCreate;
                        kCreate.eType = EFFECT_TYPE_NORMAL;
                        kCreate.iEffectNum = (*iter).first;
						kCreate.kActArg = kActArg;
                        kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
						kCreate.dwElapsedTime = (*iter).second;
                        pkEntity->AddEffect(kCreate);

                        ++iter;
                    }

                    return S_OK;
                }
            }
        }
    } 

    return E_FAIL;
}

bool PgMissionGround::UpGradeGuardian(CUnit* pkUnit, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	int iGuardianNo = 0;
	std::wstring kTriggerName;

	pkNfy->Pop( iGuardianNo );
	pkNfy->Pop( kTriggerName );

	if( !pkUnit )
	{
		return false;
	}

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return false;
	}	

	if( (0 < iGuardianNo) && (true == IsGuardianPos(kTriggerName)) )
	{
		if( true == IsNeedGuardianStrategicPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo) )
		{
			if( true == GuardianUpgradeCheck(pkUnit, iGuardianNo, kTriggerName) )
			{
                HRESULT hRt = RealUpgradeGuardian(iGuardianNo, kTriggerName, pkUnit);
                if(S_OK == hRt)
                {
                    BM::Stream kPacket(PT_M_C_ANS_GUARDIAN_SET);
					kPacket.Push(pkUnit->GetID());
                    PU::TWriteTable_AM(kPacket, m_kGuardianInstall);
                    Broadcast(kPacket);
                }
                else if(E_COMMON_CANNOT_FIND==hRt)
                {
                    // ������ ������� ã�� ���ߴ�.
                    pkUnit->SendWarnMessage(401134);
                }
                else
                {
                    // ���׷��̵� ����
				    pkUnit->SendWarnMessage(401133);
                }
			}
		}
		else
		{
			pkUnit->SendWarnMessage(401139);
		}
	}
	else
	{
		// ����� ��ȣ �߸��� ������ �Ǵ� �̹� ������� ��ġ�Ǿ� ���� �ʴ� ���
		pkUnit->SendWarnMessage(401135);
	}
	return false;
}

bool PgMissionGround::InsertGuardian(CUnit* pkUnit, BM::Stream* pkNfy)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	int iGuardianNo = 0;
	std::wstring kTriggerName;
	POINT3 rkPos;

	pkNfy->Pop( iGuardianNo );
	pkNfy->Pop( kTriggerName );

	if( !pkUnit )
	{
		return false;
	}

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return false;
	}	

	if( (0 < iGuardianNo) && (false == IsGuardianPos(kTriggerName)) )
	{
		GetDefence7GuardianPos(pkUnit->GetAbil(AT_TEAM), kTriggerName, rkPos);

		if( true == IsNeedGuardianStrategicPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo) )
		{
			CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardianNo);

			CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
			if( pkGuardian->end() != iter )
			{
				SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);			

				PgGround *pkGround = dynamic_cast<PgGround*>(this);
				if( pkGround )
				{
					// �ٸ� ������� ������ ����Ʈ ����(���� ����𸶴� �ڽ��� ����Ʈ�� ����)
					/*VEC_INT kAddEffect;
					BM::GUID rkGuid;
					CONT_GUARDIAN_INSTALL::iterator install_iter = m_kGuardianInstall.begin();
					while( m_kGuardianInstall.end() != install_iter )
					{
						rkGuid = install_iter->second.kGuid;
						CUnit* pkTeam = GetUnit(rkGuid);
						if( pkTeam != NULL && pkTeam->IsUnitType(UT_ENTITY) )
						{
							if( pkUnit->GetAbil(AT_TEAM) == pkTeam->GetAbil(AT_TEAM) )
							{
								PgEntity* pkTeamEntity = dynamic_cast<PgEntity*>(pkTeam);
								if( pkTeamEntity )
								{
									ContEffectItor kItor;
									CEffect* pkEffect = NULL;
									PgUnitEffectMgr& rkEffectMgr = pkTeamEntity->GetEffectMgr();
									rkEffectMgr.GetFirstEffect(kItor);
									GET_DEF(CEffectDefMgr, kEffectDefMgr);
									while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
									{
										kAddEffect.push_back(pkEffect->GetKey());
									}
									break;
								}
							}
						}
						++install_iter;
					}*/					

					BM::vstring kNodeName("Entity");
					POINT3 kNodePos = rkPos;

					SCreateEntity kCreateInfo;
					kCreateInfo.kClassKey.iClass = kValue.iMonsterNo;
					kCreateInfo.kClassKey.nLv = kValue.iUpgrade_Step;
					kCreateInfo.bUniqueClass = false;
					kCreateInfo.bEternalLife = true;
                    kCreateInfo.iTunningNo = GetGuardianTunningNo(iGuardianNo);
                    kCreateInfo.iTunningLevel = m_iTunningLevel;
					kCreateInfo.kGuid.Generate();

					// �ٴڿ� ���� ��Ų��
					NxRay kRay(NxVec3(kNodePos.x, kNodePos.y, kNodePos.z+20), NxVec3(0, 0, -1.0f));
					NxRaycastHit kHit;
					NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
					if(pkHitShape)
					{
						kNodePos.z = kHit.worldImpact.z;
					}
					kCreateInfo.ptPos = kNodePos;

					CUnit* pkEntity = pkGround->CreateGuardianEntity(pkUnit, &kCreateInfo, kNodeName);
					if( pkEntity )
					{
						SubNeedGuardianStrategicPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo, pkUnit);
						SendStrategicPoint(pkUnit->GetAbil(AT_TEAM), 0, 0);
						SetGuardianAbil(pkEntity, kValue);
						SetGuardian(kTriggerName, iGuardianNo, pkEntity->GetID());

						/*VEC_INT::iterator iter = kAddEffect.begin();
						while( kAddEffect.end() != iter )
						{
							SEffectCreateInfo kCreate;
							kCreate.eType = EFFECT_TYPE_NORMAL;
							kCreate.iEffectNum = (*iter);
							kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
							pkEntity->AddEffect(kCreate);

							++iter;
						}*/

						BM::Stream kPacket(PT_M_C_ANS_GUARDIAN_SET);
						kPacket.Push(pkUnit->GetID());
						PU::TWriteTable_AM(kPacket, m_kGuardianInstall);
						Broadcast(kPacket);

						return true;
					}
				}
			}
		}
		else
		{
			pkUnit->SendWarnMessage(401139);
		}
	}
	else
	{
		// ����� ������ ��ȣ�� �߸��Ǿ��ų�, �̹� ��ġ�� �Ǿ� �ִ� ���
		pkUnit->SendWarnMessage(401136);
	}
	return false;
}

void PgMissionGround::SendStrategicPoint(int const iTeam, int const OriValue, int const MulValue, EDefencePointType const eType)
{
	BM::Stream kPacket(PT_M_C_NFY_DEFENCE_STRATEGIC_POINT);
	kPacket.Push( iTeam );
	kPacket.Push( OriValue );
	kPacket.Push( MulValue );
    kPacket.Push( eType );
	kPacket.Push( m_kContStrategicPoint );
	Broadcast(kPacket);
}

void PgMissionGround::GMCommand_RecvGamePoint( PgPlayer *pkPlayer, int const iPoint )
{
	if( !pkPlayer ){ return; }
	BM::CAutoMutex kLock(m_kRscMutex);

	if ( INDUN_STATE_PLAY == GetState() )
	{
		int const iTeam = std::max((pkPlayer->GetAbil(AT_TEAM) - 2), 1);
		int iStrategicPoint = iPoint;

		AddTeamStrategicPoint(iTeam, iStrategicPoint);

		int OriValue = std::max(iPoint, 0);
		int MulValue = std::max(iStrategicPoint - OriValue, 0);
		SendStrategicPoint(iTeam, OriValue, MulValue);
	}
}

void PgMissionGround::AddTeamStrategicPoint(int const iTeam, int& iStrategicPoint, bool const bNotProbability)
{
	BM::CAutoMutex kLock(m_kRscMutex);

    if( !bNotProbability )
    {//Ȯ���� ������ ��츸
	    VEC_GUID kMemberVec;					
	    GetTeamPartyMember(iTeam, kMemberVec);

	    for(VEC_GUID::iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
	    {
		    VEC_GUID::value_type kValue = (*iter);
		    CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
		    if( pkTargetUnit )
		    {
			    int iAddStrategicPointPercent = std::max(pkTargetUnit->GetAbil(AT_STRATEGIC_POINT), 0);
			    if( 0 < iAddStrategicPointPercent )
			    {
				    iStrategicPoint += (iStrategicPoint * iAddStrategicPointPercent / 100);
				    break;
			    }
		    }
	    }
    }

	SetStrategicPoint(iTeam, iStrategicPoint);
}

void PgMissionGround::SetStrategicPoint(int const iTeam, int const iStrategicPoint)
{
	auto bRetValue = m_kContStrategicPoint.insert(std::make_pair(iTeam, iStrategicPoint));
	if( !bRetValue.second )
	{
		(*bRetValue.first).second += iStrategicPoint;
	}
}

int PgMissionGround::GetStrategicPoint(int const iTeam)const
{
	int iTotalStrategicPoint = 0;
	CONT_STRATEGIC_POINT::const_iterator iter = m_kContStrategicPoint.find(iTeam);
	if( iter != m_kContStrategicPoint.end() )
	{
		return (*iter).second;
	}
	return 0;
}

void PgMissionGround::AddStageTeamStrategicPoint()
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( IsDefenceMode() )
	{
		return;
	}

	const CONT_MISSION_DEFENCE7_STAGE_BAG *pkStage;
	g_kTblDataMgr.GetContDef(pkStage);
	if( !pkStage )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_STAGE_BAG is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkStage is NULL"));
	}
	else
	{
		int iRedMul = 0;
		int iBlueMul = 0;

		VEC_GUID kMemberVec;		
		GetTeamPartyMember(TEAM_RED, kMemberVec);
		for(VEC_GUID::iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
		{
			VEC_GUID::value_type kValue = (*iter);
			CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
			if( pkTargetUnit )
			{
				iRedMul = pkTargetUnit->GetAbil(AT_STRATEGIC_MUL_POINT);
				break;
			}
		}

		kMemberVec.clear();
		GetTeamPartyMember(TEAM_BLUE, kMemberVec);
		for(VEC_GUID::iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
		{
			VEC_GUID::value_type kValue = (*iter);
			CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
			if( pkTargetUnit )
			{
				iBlueMul = pkTargetUnit->GetAbil(AT_STRATEGIC_MUL_POINT);
				break;
			}
		}

		int const iNowStage = m_iNowStage + 1;

		CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kStageData;
		if( true == GetDefence7Stage(iNowStage, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
		{			
			CONT_MISSION_DEFENCE7_STAGE::value_type &kValue = kStageData.kCont.at(0);

			int OriValue = kValue.iClear_StategicPoint;
			if( IsDefenceMode7() )
			{				
				int iStrategicPoint = std::max(OriValue, 0);
				int iStrategicMul = iRedMul;
				iStrategicPoint = ( 0 < iStrategicMul ) ? iStrategicPoint * iStrategicMul : iStrategicPoint;
				if( 0 < iStrategicPoint )
				{
					AddTeamStrategicPoint(TEAM_RED, iStrategicPoint);
					int MulValue = std::max(iStrategicPoint - OriValue, 0);
					SendStrategicPoint(TEAM_RED, OriValue, MulValue, DP_STAGEEND);
				}
			}

			if( IsDefenceMode8() )
			{				
				int iStrategicPoint = std::max(OriValue, 0);
				int iStrategicMul = iBlueMul;
				iStrategicPoint = ( 0 < iStrategicMul ) ? iStrategicPoint * iStrategicMul : iStrategicPoint;
				if( 0 < iStrategicPoint )
				{
					AddTeamStrategicPoint(TEAM_BLUE, iStrategicPoint);
					int MulValue = std::max(iStrategicPoint - OriValue, 0);
					SendStrategicPoint(TEAM_BLUE, OriValue, MulValue, DP_STAGEEND);
				}
			}
		}
	}
}

void PgMissionGround::SendToUserItemList(CUnit* pkUnit, VEC_INT& kSendValue)
{
	BM::Stream kPacket(PT_M_C_NFY_DEFENCE_USEITEM_LIST);
	kPacket.Push( kSendValue );
	pkUnit->Send( kPacket );
}

void PgMissionGround::Defence7SkillLearn(CUnit* pkUnit, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( !pkUnit )
	{
		return;
	}

	BM::GUID rkGuardianGuid;
    int iGuardianNo = 0;
	int iSkillNo = 0;
	int iEffectNo = 0;
    pkPacket->Pop( iGuardianNo );
	pkPacket->Pop( iSkillNo );
	pkPacket->Pop( rkGuardianGuid );

	//IGuardianNo�� Ŭ�󿡼� �Ѿ�� ������ ������ ����
	CUnit * pkGuardianUnit = NULL;
	if(rkGuardianGuid.IsNotNull())
	{
		CONT_GUARDIAN_INSTALL::const_iterator c_iter = m_kGuardianInstall.begin();
		while( m_kGuardianInstall.end() != c_iter )
		{
			CONT_GUARDIAN_INSTALL::mapped_type const& kValue = c_iter->second;
			if( rkGuardianGuid==kValue.kGuid)
			{
				pkGuardianUnit = GetUnit(rkGuardianGuid);
				if(pkGuardianUnit)
				{
					iGuardianNo = kValue.iGuardianNo;
					break;
				}
			}
			++c_iter;
		}
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if( !pkSkill )
	{
		return;		
	}
	iEffectNo = pkSkill->GetEffectNo();


	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iEffectNo);
	if( !pkEffectDef )
	{
		return;
	}
	
	int const iCustomType = pkEffectDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	if( 0 >= iCustomType )
	{
		return;
	}

	BM::GUID const rkGuid = pkUnit->GetID();
	int const iTeam = std::max(pkUnit->GetAbil(AT_TEAM), 1);
    bool const bGuardianSkill = PgDefenceMissionSkill::IsGuardianSkill(iSkillNo);
	
    if( false==bGuardianSkill )
    {
        if( 0 < m_kDefenceMissionSkill.GetNextSkillNo(iTeam, iSkillNo) )
	    {
	    }
	    else
	    {
		    pkUnit->SendWarnMessage(401137);
		    return;
	    }

	    if( true == m_kDefenceMissionSkill.IsLearn(iTeam, iSkillNo) )
	    {
		    pkUnit->SendWarnMessage(401137);
		    return;
	    }
    }

	switch( iCustomType )
	{
	case DS_ADD_POINT:
		{
			if( true == IsNeedGuardianSkillPoint(iTeam, iGuardianNo, iEffectNo) )
			{
				SubNeedGuardianSkillPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo, iEffectNo);
				int const iAddStrategicPointPercent = std::max(pkEffectDef->GetAbil(AT_STRATEGIC_POINT), 0);	// ȹ�淮 %					
				
				VEC_GUID kMemberVec;					
				GetTeamPartyMember(iTeam, kMemberVec);

				for(VEC_GUID::iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
				{
					VEC_GUID::value_type kValue = (*iter);
					CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
					if( pkTargetUnit )
					{
						pkTargetUnit->SetAbil(AT_STRATEGIC_POINT, iAddStrategicPointPercent, true, true);							

						SEffectCreateInfo kCreate;
						kCreate.eType = EFFECT_TYPE_NORMAL;
						kCreate.iEffectNum = iEffectNo;
						kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

						pkTargetUnit->AddEffect(kCreate);
					}
				}
				SendStrategicPoint(iTeam, 0, 0);
			}
			else
			{
				// ����Ʈ ���߶�.
				pkUnit->SendWarnMessage(401139);
				return;
			}
		}break;						
	case DS_ADD_MOVE:
	case DS_ADD_PC_ATTACK:
		{
			if( true == IsNeedGuardianSkillPoint(iTeam, iGuardianNo, iEffectNo) )
			{
				SubNeedGuardianSkillPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo, iEffectNo);

				VEC_GUID kMemberVec;					
				GetTeamPartyMember(iTeam, kMemberVec);

				for(VEC_GUID::iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
				{
					VEC_GUID::value_type kValue = (*iter);
					CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
					if( pkTargetUnit )
					{
						SEffectCreateInfo kCreate;
						kCreate.eType = EFFECT_TYPE_NORMAL;
						kCreate.iEffectNum = iEffectNo;
						kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

						pkTargetUnit->AddEffect(kCreate);
					}
				}

				//����� ���ݷ�
				if( int const iGuardianEffect = pkSkill->GetAbil(AT_EFFECTNUM1) )
				{
					CUnit * pkTargetUnit = NULL;
					CONT_GUARDIAN_INSTALL::const_iterator c_iter = m_kGuardianInstall.begin();
					while( m_kGuardianInstall.end() != c_iter )
					{
						if(CUnit * pkTargetUnit = GetUnit((*c_iter).second.kGuid))
						{
							SEffectCreateInfo kCreate;
							kCreate.eType = EFFECT_TYPE_NORMAL;
							kCreate.iEffectNum = iGuardianEffect;
							kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
							pkTargetUnit->AddEffect(kCreate);
						}
						++c_iter;
					}
				}
				SendStrategicPoint(iTeam, 0, 0);
			}
			else
			{
				// ����Ʈ ���߶�.
				pkUnit->SendWarnMessage(401139);
				return;
			}
		}break;	
	case DS_ADD_ENTITY_SKILL_01:
	case DS_ADD_ENTITY_SKILL_02:
		{
			if( !pkGuardianUnit || !pkGuardianUnit->IsUnitType(UT_ENTITY) )
			{
				return;
			}

			if(iTeam != pkGuardianUnit->GetAbil(AT_TEAM))
			{
				return;
			}

			if(pkGuardianUnit->GetEffect(iEffectNo))
			{
				pkUnit->SendWarnMessage(235);
				return;
			}

			if(!IsNeedGuardianSkillPoint(iTeam, iGuardianNo, iEffectNo))
			{
				// ����Ʈ ���߶�.
				pkUnit->SendWarnMessage(401139);
				return;
			}

			SEffectCreateInfo kCreate;
			kCreate.eType = EFFECT_TYPE_NORMAL;
			kCreate.iEffectNum = iEffectNo;
			kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
			pkGuardianUnit->AddEffect(kCreate);

			SubNeedGuardianSkillPoint(pkUnit->GetAbil(AT_TEAM), iGuardianNo, iEffectNo);
			SendStrategicPoint(pkUnit->GetAbil(AT_TEAM), 0, 0);
		}break;
	default:
		{
		}break;
	}

    if( false==bGuardianSkill )
    {
	    if( false == m_kDefenceMissionSkill.SetSkillInsert(iTeam, iSkillNo) )
	    {
		    // �̹� ������ ��ų
		    pkUnit->SendWarnMessage(401137);
		    return;
	    }

	    CONT_DEFENCE_SKILL kCont;
	    GetContDefenceMissionSkill(kCont);

	    BM::Stream kPacket(PT_M_C_ANS_DEFENCE_SKILL_USE);
	    PU::TWriteTable_AA(kPacket, kCont);
	    Broadcast(kPacket);
    }
}

void PgMissionGround::GetContDefenceMissionSkill(CONT_DEFENCE_SKILL & kOut)const
{
	m_kDefenceMissionSkill.Get(kOut);
}

void PgMissionGround::SendToFailStageCheck()
{
	BM::CAutoMutex kLock(m_kRscMutex);

	for(int i=TEAM_RED; i<=TEAM_BLUE; ++i)
	{
		BM::GUID kObjectGuid;
		CONT_OBJECT_TEAM::iterator iter = m_kObjectTeam.find(i);
		if( m_kObjectTeam.end() != iter )
		{
			kObjectGuid = iter->second;
		}
		if( kObjectGuid.IsNotNull() )
		{
			CUnit* pkUnit = PgObjectMgr::GetUnit(kObjectGuid);
			if( pkUnit )
			{
				int const iHp = pkUnit->GetAbil(AT_HP);
				if( 0 >= iHp )
				{
					BM::Stream kPacket(PT_M_C_NFY_DEFENCE_FAILSTAGE);

					PgPlayer* pkPlayer = NULL;
					CONT_OBJECT_MGR_UNIT::iterator kItor;
					PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
					while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
					{
						if( pkPlayer->GetAbil(AT_TEAM) == i )
						{
							pkPlayer->Send(kPacket);
						}
					}
				}
			}
		}
	}
}

void PgMissionGround::SendStageTeamPoint()
{
	BM::Stream kPacket(PT_M_C_ANS_TEAM_POINT);
	kPacket.Push( m_kTeamPoint );
	Broadcast(kPacket);
}

void PgMissionGround::TeamChangeOwner(CUnit* pkUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	VEC_GUID kMemberVec;
	int iTeam = (pkUnit->GetAbil(AT_TEAM) == TEAM_RED) ? TEAM_BLUE : TEAM_RED;
	GetTeamPartyMember(iTeam, kMemberVec);

	for(VEC_GUID::iterator iter = kMemberVec.begin();iter != kMemberVec.end();++iter)
	{
		VEC_GUID::value_type kValue = (*iter);
		CUnit* pkTargetUnit = PgObjectMgr::GetUnit(kValue);
		if( pkTargetUnit )
		{
			if( m_kLocalPartyMgr.IsMaster(pkTargetUnit->GetPartyGuid(), pkTargetUnit->GetID()) )
			{
				BM::CAutoMutex kLock(m_kMissionMutex);
				PgMission::SetOwner(pkTargetUnit->GetID());

				break;
			}
		}
	}
}

void PgMissionGround::GuardianOwnerChange(CUnit* pkUnit, int const iTeam)
{
	BM::CAutoMutex Lock(m_kRscMutex);

    PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkUnit);
    if( !pkUnit || !pkUser || m_kConUser.empty() )
	{
		return;
	}
    
    if( IsDefenceMode7() || IsDefenceMode8() )
    {
        CUnit* pkTargetUnit = NULL;
        PgPlayer *pkPlayer= NULL;
        CONT_OBJECT_MGR_UNIT::iterator kItor;
        PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	    while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	    {
		    if( pkPlayer->GetAbil(AT_TEAM)==iTeam && pkPlayer->GetID()!=pkUnit->GetID())
            {
                pkTargetUnit = pkPlayer;
                break;
            }
	    }

        if( pkTargetUnit )
        {
            CUnit* pkEntity = NULL;
		    CONT_GUARDIAN_INSTALL::iterator iter = m_kGuardianInstall.begin();
		    while( m_kGuardianInstall.end() != iter )
		    {
			    pkEntity = GetUnit(iter->second.kGuid);
			    if( pkEntity != NULL && pkEntity->IsUnitType(UT_ENTITY) )
			    {
				    if( pkUnit->GetID() == pkEntity->Caller() )
				    {
                        pkEntity->Caller(pkTargetUnit->GetID());

						//Logout���� ȣ���ϴ� ReleaseUnit���� ��ȯü�� ����⶧���� ����
						//pkTargetUnit�� AddSummonUnit�� ����� �� ������ ã�� ���� ó������ ����
						pkUnit->DeleteSummonUnit(pkEntity->GetID());
				    }
			    }

			    ++iter;
		    }
        }
	}
}

void PgMissionGround::SendTeamMonsterGen(bool bLastWave)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		eMonsterGen = ( pkPlayer->GetAbil(AT_TEAM) == TEAM_RED ) ? D_RIGHT : D_LEFT;

		BM::Stream kDefencePacket(PT_M_C_NFY_DEFENCE_WAVE);
		kDefencePacket.Push( bLastWave );
		kDefencePacket.Push( static_cast<BYTE>(eMonsterGen) );
		pkPlayer->Send(kDefencePacket);
	}
}

void PgMissionGround::SendTeamTowerAttackMsg(ETeam const eTeam)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		if( pkPlayer->GetAbil(AT_TEAM) == eTeam )
		{
			BM::Stream kDefencePacket(PT_M_C_NFY_TOWER_ATTACK);
			pkPlayer->Send(kDefencePacket);
		}
	}
}

void PgMissionGround::CheckMonsterDistanceStrategicPoint()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( !(IsDefenceMode7() || IsDefenceMode8()) )
	{
		return;
	}

	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgMonster* pkMonster = NULL;
	PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
	while ((pkMonster = dynamic_cast<PgMonster*> (PgObjectMgr::GetNextUnit(UT_MONSTER, kItor))) != NULL)
	{
		char kTriggerID[64] = {0,};
		bool bIsIn = false;

		for(int i=0; i<10; ++i)
		{
			::sprintf_s(kTriggerID, 64, "DOUBLE_UP_%d", (i+1) );			
			bIsIn = PgGround::IsInTriggerUnit(std::string(kTriggerID), pkMonster);
			if( bIsIn )
			{
				break;
			}
		}

		int const iOldValue = pkMonster->GetAbil(AT_STRATEGIC_MUL_POINT);
		if( bIsIn )
		{
			pkMonster->SetAbil(AT_STRATEGIC_MUL_POINT, 2);
		}
		else
		{
			pkMonster->SetAbil(AT_STRATEGIC_MUL_POINT, 0);				
		}
		
		if( iOldValue != pkMonster->GetAbil(AT_STRATEGIC_MUL_POINT) )
		{
			pkMonster->SendAbil(AT_STRATEGIC_MUL_POINT, E_SENDTYPE_BROADCAST | E_SENDTYPE_SELF);
		}
	}
}

bool PgMissionGround::IsMissionItemLevel(int const iUseLevel)
{
	int const iMissionValue = (1 << GetLevel());

	if( iMissionValue > MILT_MAX )
	{
		return false;
	}

	if( iMissionValue & iUseLevel )
	{
		return true;
	}
	return false;
}

int PgMissionGround::GetMaxSummonUnitCount(CUnit * pkUnit)const
{
    if( pkUnit->IsUnitType(UT_PLAYER) && (IsDefenceMode7()||IsDefenceMode8()) )
    {
        return INT_MAX;
    }
    return pkUnit->GetMaxSummonUnitCount();
}

void PgMissionGround::SendMonsterCount()
{
	size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
	if( true == IsDefenceMode() )
	{
		int const iTotalWaveMonsterCount = GetStageTotalMonsterCount();
		Broadcast( BM::Stream(PT_M_C_NFY_REST_MONSTER_NUM,iLiveMonsterCount+iTotalWaveMonsterCount) );
	}
	else if( IsDefenceMode7() || IsDefenceMode8() )
	{
	}
	else
	{
		Broadcast( BM::Stream(PT_M_C_NFY_REST_MONSTER_NUM,iLiveMonsterCount) );
	}
}

void PgMissionGround::SendWarnMessageBroadcast(int const iTTNo)
{
    ConUser::const_iterator user_itr = m_kConUser.begin();
    while( user_itr != m_kConUser.end() )
    {
        PgPlayer* pkUser = GetUser(user_itr->first);
        if( pkUser )
        {
            pkUser->SendWarnMessage(iTTNo);
        }
        ++user_itr;
    }
}

bool PgMissionGround::IsBonusStage()
{	
	CONT_MISSION_BONUSMAP const *pkBonusMap;
	g_kTblDataMgr.GetContDef(pkBonusMap);
	if( !pkBonusMap )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_BONUSMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkBonusMap is NULL"));
	}
	else
	{
		CONT_MISSION_BONUSMAP::const_iterator iter = pkBonusMap->find(PgMission::m_iCandidateNo);
		if( pkBonusMap->end() != iter )
		{
			CONT_MISSION_BONUSMAP::mapped_type const &kElement = (*iter).second;
			if( 0 < kElement.iUse )
			{
				int iGroundNo = GetGroundNo();
				if( (iGroundNo == kElement.iBonusMap1) || (iGroundNo == kElement.iBonusMap2) )
				{
					return true;
				}
			}
		}
	}
	return false;
}

int PgMissionGround::GetBonusStageTime()
{
	CONT_MISSION_BONUSMAP const *pkBonusMap;
	g_kTblDataMgr.GetContDef(pkBonusMap);
	if( !pkBonusMap )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_BONUSMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkBonusMap is NULL"));
	}
	else
	{
		CONT_MISSION_BONUSMAP::const_iterator iter = pkBonusMap->find(PgMission::m_iCandidateNo);
		if( pkBonusMap->end() != iter )
		{
			CONT_MISSION_BONUSMAP::mapped_type const &kElement = (*iter).second;
			if( 0 < kElement.iUse )
			{
				return kElement.iTimeLimit;
			}
		}
	}
	return 0;
}

void PgMissionGround::CheckMonsterDieQuest(CUnit* pkCaster, CUnit* pkMonster)
{
	if( pkCaster
	&&	pkMonster )
	{
		COMBO_TYPE const iCurCombo = GetCurrentCombo(pkCaster->GetID());

		PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
		if( pkPC )
		{
			if( BM::GUID::NullData() != pkPC->PartyGuid() )//��Ƽ�� ������
			{
				VEC_GUID kGuidVec;
				bool const bFindMember = m_kLocalPartyMgr.GetPartyMemberGround(pkPC->PartyGuid(), GroundKey(), kGuidVec, pkCaster->GetID());				
				if( bFindMember )
				{
					VEC_GUID::iterator guid_iter = kGuidVec.begin();
					while( kGuidVec.end() != guid_iter )
					{
						BM::GUID const & rkGuid = (*guid_iter);
						CUnit* pkPartyMember = GetUnit(rkGuid);
						if( pkPartyMember )
						{
							PgAction_MonsterQuestCheck kMonsterQuestAction(GetGroundNo(),GroundKey(), iCurCombo, GetKey(), GetLevel(), GetStageCountExceptBonus(), this, pkCaster);
							kMonsterQuestAction.DoAction(pkPartyMember, pkMonster);

							PgAction_GlobalQuestCheck kGlobalQuestAction(GetGroundNo(),GroundKey(), iCurCombo);
							kGlobalQuestAction.DoAction(pkPartyMember, pkMonster);

							PgAction_GroundQuestCheck kGroundQuestAction(GetGroundNo(), GroundKey(), iCurCombo);
							kGroundQuestAction.DoAction(pkPartyMember, pkMonster);
						}

						++guid_iter;
					}
				}
			}

			{
				PgAction_MonsterQuestCheck kMonsterQuestAction(GetGroundNo(),GroundKey(), iCurCombo, GetKey(), GetLevel(), GetStageCountExceptBonus(), this, pkCaster);
				kMonsterQuestAction.DoAction(pkCaster, pkMonster);

				PgAction_GlobalQuestCheck kGlobalQuestAction(GetGroundNo(),GroundKey(), iCurCombo);
				kGlobalQuestAction.DoAction(pkCaster, pkMonster);

				PgAction_GroundQuestCheck kGroundQuestAction(GetGroundNo(), GroundKey(), iCurCombo);
				kGroundQuestAction.DoAction(pkCaster, pkMonster);
			}
		}
	}
}

void PgMissionGround::Defence7RelayStage(CUnit * pkUnit, bool const bRemoveGuardian)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( IsDefenceMode7() )
	{
		int const RELAY_DELAY_TIME = 60000;//60���Ŀ� ����
		if(pkUnit)
		{
			BM::Stream kPacket(PT_M_C_ANS_DEFENCE7_RELAY_STAGE);
			kPacket.Push( pkUnit->Name() );
			kPacket.Push( m_iNowStage+1 );
			kPacket.Push( RELAY_DELAY_TIME );
			Broadcast(kPacket);
		}

		m_dwFailMissionWaitTime = BM::GetTime32() + RELAY_DELAY_TIME;

		int const iTeam = std::max(pkUnit->GetAbil(AT_TEAM), 1);

		//���� ����
		RemoveAllMonster(true);

		ObjectUnitGenerate(m_kContGenPoint_Object, true);
		ObjectCreate();

		//�÷��̾� HP ȸ��
		CUnit * pkPlayer = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
		while( (pkPlayer = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
		{
			PgDeathPenalty kAction(GroundKey(), LURT_MissionGround);
			kAction.DoAction(pkPlayer, NULL);
			pkPlayer->SetAbil(AT_HP,pkPlayer->GetAbil(AT_C_MAX_HP),true,true);
			pkPlayer->SetAbil(AT_MP,pkPlayer->GetAbil(AT_C_MAX_MP),true,true);
		}

		//����Ʈ
		int iStrategicPoint = 0;
		CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kStageData;
		if( true == GetDefence7Stage(m_iNowStage+1, kStageData) )	// ���������� 0���� ���� �ϹǷ�...
		{
			CONT_MISSION_DEFENCE7_STAGE::value_type const& kValue = kStageData.kCont.at(0);
			if(kValue.iUseCoin_StrategicPoint)
			{
				iStrategicPoint = std::max(kValue.iUseCoin_StrategicPoint, 0);
				AddTeamStrategicPoint(iTeam, iStrategicPoint);

				int OriValue = std::max(kValue.iUseCoin_StrategicPoint, 0);
				int MulValue = std::max(iStrategicPoint - OriValue, 0);
				SendStrategicPoint(iTeam, OriValue, MulValue);
				
				iStrategicPoint = OriValue;
			}
		}

		//����� ��ü
		const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
		g_kTblDataMgr.GetContDef(pkGuardian);
		if(bRemoveGuardian && pkGuardian)
		{
			int iGuardianPoint = 0;
			CONT_GUARDIAN_INSTALL::const_iterator c_iter = m_kGuardianInstall.begin();
			while( m_kGuardianInstall.end() != c_iter )
			{
				CONT_GUARDIAN_INSTALL::mapped_type const& kGuardianValue = c_iter->second;
				CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type const kKey(kGuardianValue.iGuardianNo);
				CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
				if( pkGuardian->end() != iter )
				{
					CUnit* pkDelete = GetUnit(kGuardianValue.kGuid);
					if (pkDelete != NULL && pkDelete->IsUnitType(UT_ENTITY))
					{
						//
						ReleaseUnit(pkDelete);

						//
						iGuardianPoint += iter->second.kCont.at(0).iAutoSell_StrategicPoint;
					}
				}
				++c_iter;
			}
			m_kGuardianInstall.clear();

			BM::Stream kPacket(PT_M_C_ANS_GUARDIAN_SET);
			kPacket.Push(BM::GUID::NullData());
			PU::TWriteTable_AM(kPacket, m_kGuardianInstall);
			Broadcast(kPacket);

			int iStrategicPoint = std::max(iGuardianPoint, 0);
			AddTeamStrategicPoint(iTeam, iStrategicPoint);

			int OriValue = std::max(iGuardianPoint, 0);
			int MulValue = std::max(iStrategicPoint - OriValue, 0);
			SendStrategicPoint(iTeam, OriValue, MulValue);
		}

		//Log
		PgPlayer * pkOwner = dynamic_cast<PgPlayer*>(pkUnit);
		if(pkOwner)
		{
			PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_Game);
			kLogCont.MemberKey( pkOwner->GetMemberGUID() );
			kLogCont.CharacterKey( pkOwner->GetID() );
			kLogCont.ID( pkOwner->MemberID() );
			kLogCont.UID( pkOwner->UID() );
			kLogCont.Name( pkOwner->Name() );
			kLogCont.ChannelNo( pkOwner->GetChannel() );
			kLogCont.Class( static_cast<short>(pkOwner->GetAbil(AT_CLASS)) );
			kLogCont.Level( static_cast<short>(pkOwner->GetAbil(AT_LEVEL)) );
			kLogCont.GroundNo( GetGroundNo() );

			PgLog kLog(ELOrderMain_Item, ELOrderSub_RelayPlay);
			kLog.Set(0, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(pkOwner->PartyGuid())) );
			kLog.Set(1, static_cast<int>(GetLevel()+1) );
			kLog.Set(2, static_cast<int>(m_iNowStage) );
			kLog.Set(3, m_iNowWave );
			kLog.Set(0, static_cast<__int64>(iStrategicPoint) );
			kLog.Set(1, static_cast<__int64>(GetStrategicPoint(iTeam)) );
			kLogCont.Add( kLog );
			kLogCont.Commit();
		}

		--m_iNowStage;
	}
}

int PgMissionGround::CaclAccumPoint()
{
	if( MAX_CHARGE_COPY_MACHINE > m_kPointAccumCount )
	{
		return 15 * pow(2.0f, static_cast<int>(m_kPointAccumCount));
	}

	return 0;
}

void PgMissionGround::Defence7PointCopy(PgPlayer * pPlayer)
{
	if( m_iNowStage < 10 )
	{
		return; // 11�������� ������ ����Ҽ� ����(Stage�� 0���� ����).
	}

	if( !PointCopying() )		// ������ �����⸦ ó�� ���.
	{
		PointCopying(true);	// ������ ������ �����·� �ٲ�.

		m_kPointAccumCount = 0;	// ó���̴ϱ� ����Ƚ�� 1��.
		AccumPoint(CaclAccumPoint());	// ���� ���� ����Ʈ ���(Stage N = Power(2,N-1)).
		++m_kPointAccumCount; // ����ȸ�� ����.
		int const NextAccumPoint = CaclAccumPoint(); // ���� ���������� ���� ����Ʈ.

		++m_kAccumMonsterKillableCount;	// ���� ������ ���� �� ����.

		BM::Stream Packet(PT_M_C_NFY_USE_POINT_COPY_ITEM);
		Packet.Push(pPlayer->Name());	// ����� ĳ���� �̸�.
		Packet.Push(PointAccumCount());	// ���� Ƚ��.
		Packet.Push(AccumPoint()); // ���� ����.
		Packet.Push(NextAccumPoint);
		Packet.Push(m_kAccumMonsterKillableCount);

		Broadcast(Packet);
	}
}

void PgMissionGround::AccumPointToStrategyPoint()
{
	if( PointCopying() )
	{
		PointAccumCount(0);	// ����ȸ�� �ʱ�ȭ.

		BM::GUID Owner = GetOwner();
		PgPlayer * pPlayer = GetUser(Owner);
		if( pPlayer )
		{
			int const iAccumPoint = AccumPoint();
			int iTeam = pPlayer->GetAbil(AT_TEAM);
			SetStrategicPoint(iTeam, iAccumPoint);	// ���� ����Ʈ�� ���� ����Ʈ�� ��ȯ.

			BM::Stream Packet(PT_M_C_NFY_ACCUMPOINT_TO_STRATEGICPOINT);
			Packet.Push(AccumPoint());
			Broadcast(Packet);

			AccumPoint(0);	// ���� ����Ʈ �ʱ�ȭ.
			PointCopying(false);	// ������ ������ ��� ����.
			AccumMonsterKillableCount(0);	// ���� ������ ���� �� �ʱ�ȭ.

			SendStrategicPoint(iTeam, 0, 0);

			//Log
			PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_Game);
			kLogCont.MemberKey( pPlayer->GetMemberGUID() );
			kLogCont.CharacterKey( pPlayer->GetID() );
			kLogCont.ID( pPlayer->MemberID() );
			kLogCont.UID( pPlayer->UID() );
			kLogCont.Name( pPlayer->Name() );
			kLogCont.ChannelNo( pPlayer->GetChannel() );
			kLogCont.Class( static_cast<short>(pPlayer->GetAbil(AT_CLASS)) );
			kLogCont.Level( static_cast<short>(pPlayer->GetAbil(AT_LEVEL)) );
			kLogCont.GroundNo( GetGroundNo() );

			PgLog kLog(ELOrderMain_Item, ELOrderSub_AccumPointToStrategyPoint);
			kLog.Set(0, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(pPlayer->PartyGuid())) );
			kLog.Set(1, static_cast<int>(GetLevel()+1) );
			kLog.Set(2, static_cast<int>(m_iNowStage) );
			kLog.Set(3, m_iNowWave );
			kLog.Set(0, static_cast<__int64>(iAccumPoint) );
			kLog.Set(1, static_cast<__int64>(GetStrategicPoint(iTeam)) );
			kLogCont.Add( kLog );
			kLogCont.Commit();
		}
	}
}

void PgMissionGround::KillMonsterByAccumPoint()
{
	if( PointCopying() )
	{
		KillableMonsterList		ContKillableMonster;

		PointAccumCount(0);	// ����ȸ�� �ʱ�ȭ.

		BM::GUID Owner = GetOwner();
		PgPlayer * pPlayer = GetUser(Owner);
		if( pPlayer )
		{
			CONT_OBJECT_MGR_UNIT::iterator Itor; // ���� �ʿ� �ִ� ��� ���͵��� ��ǥ ���� ����.
			PgMonster* pMonster = NULL;
			PgObjectMgr::GetFirstUnit(UT_MONSTER, Itor);
			while( (pMonster = dynamic_cast<PgMonster*> (PgObjectMgr::GetNextUnit(UT_MONSTER, Itor))) != NULL )
			{
				int GoalSection = pMonster->GetAbil(AT_ROADPOINT_INDEX);	// ������ ��ǥ ������ ��� �ͼ�.
				KillableMonsterList::iterator iter = ContKillableMonster.find(GoalSection);
				if( iter != ContKillableMonster.end() )
				{	// �ش� ������ ���� ����Ʈ�� �߰�.
					(*iter).second.push_back(pMonster);
				}
				else
				{	// ���ο� ��ǥ �����̶�� �����̳ʿ� ����.
					LIST_MONSTER MonsterList;
					MonsterList.push_back(pMonster);

					// ������ ���� �����̳ʴ� key �������� ���� ��.
					ContKillableMonster.insert( std::make_pair( GoalSection, MonsterList ) );
				}
			}

			CUnit * pPlayerUnit = dynamic_cast<CUnit*>(pPlayer);
			int MonsterCount = 0;
			KillableMonsterList::iterator iter = ContKillableMonster.begin();	// ������ ���� ����Ʈ.
			for( ; iter != ContKillableMonster.end() ; ++iter )
			{
				KillableMonsterList::mapped_type & Element = (*iter).second;
				if( MonsterCount < Element.size() )	// ���� ������ �ִ� ���Ͱ� �����ؾ��� ���� ������ ���ٸ�.
				{	// HP ������ �����ؼ� HP�� ���� ������ ����.
					Element.sort(::SortyByHP);
				}

				LIST_MONSTER::iterator monster_iter = Element.begin();
				for( ; monster_iter != Element.end() ; ++monster_iter )
				{
					if( MonsterCount == m_kAccumMonsterKillableCount )
					{	// ������ ������ ���ڸ�ŭ �����ߴٸ� ����.
						break;
					}

					CUnit * pMonsterUnit = dynamic_cast<CUnit*>( (*monster_iter) );
					::OnDamaged(pPlayerUnit, pMonsterUnit, 0, pMonsterUnit->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());	// ���� ����.
					++MonsterCount;
				}
			}

			int const iAccumPoint = AccumPoint();
			AccumPoint(0);	// ���� ����Ʈ �ʱ�ȭ.
			PointCopying(false);	// ������ ������ ��� ����.
			AccumMonsterKillableCount(0);	// ���� ������ ���� �� �ʱ�ȭ.

			SendStrategicPoint(pPlayer->GetAbil(AT_TEAM), 0, 0);

			BM::Stream Packet(PT_M_C_NFY_KILL_MONSTER_BY_ACCUMPOINT);
			Broadcast(Packet);

			ContKillableMonster.clear();

			//Log
			int const iTeam = std::max(pPlayer->GetAbil(AT_TEAM), 1);

			PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_Game);
			kLogCont.MemberKey( pPlayer->GetMemberGUID() );
			kLogCont.CharacterKey( pPlayer->GetID() );
			kLogCont.ID( pPlayer->MemberID() );
			kLogCont.UID( pPlayer->UID() );
			kLogCont.Name( pPlayer->Name() );
			kLogCont.ChannelNo( pPlayer->GetChannel() );
			kLogCont.Class( static_cast<short>(pPlayer->GetAbil(AT_CLASS)) );
			kLogCont.Level( static_cast<short>(pPlayer->GetAbil(AT_LEVEL)) );
			kLogCont.GroundNo( GetGroundNo() );

			PgLog kLog(ELOrderMain_Item, ELOrderSub_KillMonsterByAccumPoint);
			kLog.Set(0, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(pPlayer->PartyGuid())) );
			kLog.Set(1, static_cast<int>(GetLevel()+1) );
			kLog.Set(2, static_cast<int>(m_iNowStage) );
			kLog.Set(3, m_iNowWave );
			kLog.Set(0, static_cast<__int64>(iAccumPoint) );
			kLog.Set(1, static_cast<__int64>(GetStrategicPoint(iTeam)) );
			kLog.Set(2, static_cast<__int64>(MonsterCount) );
			kLogCont.Add( kLog );
			kLogCont.Commit();
		}
	}
}

void PgMissionGround::OccupiedPointCopy()
{
	if( PointCopying() )
	{
		if( MAX_CHARGE_COPY_MACHINE > m_kPointAccumCount )
		{			
			AccumPoint(CaclAccumPoint());	// ���� ���� ����Ʈ ���(Stage N = Power(2,N-1)).
			++m_kPointAccumCount; // ����ȸ�� ����.

			int const NextAccumPoint = CaclAccumPoint(); // ���� ���������� ���� ����Ʈ.
			
			++m_kAccumMonsterKillableCount;	// ���� ������ ���� �� ����.

			BM::Stream Packet(PT_M_C_NFY_CURRENT_ACCUMPOINT);
			Packet.Push(PointAccumCount());	// ���� Ƚ��.
			Packet.Push(AccumPoint());
			Packet.Push(NextAccumPoint);
			Packet.Push(m_kAccumMonsterKillableCount);

			Broadcast(Packet);
		}
	}
}

void PgMissionGround::SendMissionState(CUnit * pkUnit)
{
	size_t iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
	if( true == IsDefenceGameMode() )
	{
		int const iTotalWaveMonsterCount = GetStageTotalMonsterCount();
		iLiveMonsterCount += iTotalWaveMonsterCount;
	}

	BM::Stream kCPacket(PT_M_C_NFY_MISSION_STATE, GetMissionNo());
	kCPacket.Push(GetLevel()+1);//Client�� 1���� ������...
	kCPacket.Push(PgMission::m_kConUser.size());
	kCPacket.Push((int)GetPlayTime());
	kCPacket.Push(m_bTimeAttack);
	kCPacket.Push((int)m_dwTotalTime);
	kCPacket.Push((BYTE)GetStageCount());
	kCPacket.Push((BYTE)m_iNowStage);
	kCPacket.Push(GetConstellationKey().PartyGuid.IsNotNull());

	BM::GUID kObjectGuid;
	if( true == IsDefenceGameMode() )
	{
		// Red
		kObjectGuid = BM::GUID::NullData();
		CONT_OBJECT_TEAM::const_iterator iter = m_kObjectTeam.find(TEAM_RED);
		if( m_kObjectTeam.end() != iter )
		{
			kObjectGuid = iter->second;
		}
		kCPacket.Push(kObjectGuid);
	}
	if( true == IsDefenceMode8() )
	{
		// Blue
		kObjectGuid = BM::GUID::NullData();
		CONT_OBJECT_TEAM::const_iterator iter = m_kObjectTeam.find(TEAM_BLUE);
		if( m_kObjectTeam.end() != iter )
		{
			kObjectGuid = iter->second;
		}
		kCPacket.Push(kObjectGuid);
	}
	kCPacket.Push(iLiveMonsterCount);

	if(pkUnit)
	{
		pkUnit->Send(kCPacket);
		if( (false == IsLastStage()) && (false == m_bTimeAttack) && (MISSION_ITEM_TIME > (GetLevel()+1) && IsBonusStage()) )
		{
			BM::Stream Packet(PT_M_C_NFY_MISSION_BONUS_TIME_LIMIT);

			DWORD RemainTime = m_dwTotalTimeLimit - DifftimeGetTime(m_dwStartTimeLimit,BM::GetTime32());
			if( 0 > RemainTime )
			{
				RemainTime = 0;
			}
			Packet.Push(static_cast<int>(RemainTime));
			pkUnit->Send(Packet);
		}
	}
	else
	{
		Broadcast(kCPacket);
	}
}

// ���� ���潺 ��忡�� ���������� ������ ��, �������� �÷������� �����Ƿε��� ���ҽ�Ų��.
void PgMissionGround::DecreaseStrategyFatigueDegree()
{
	if( !IsDefenceMode7() )
	{	// F7��尡 �ƴϸ� �ϸ� �ȵ�.
		return;
	}

	int DecPerStageFatigability = 0;		// ���������� �Ƿε� ����ġ.
	SSyncVariable SyncVariable = g_kEventView.VariableCont();

	BM::GUID Owner = GetOwner();
	PgPlayer * pPlayer = GetUser(Owner);
	if( pPlayer )
	{
		if( pPlayer->HaveParty() )
		{
			int MemberCount = m_kLocalPartyMgr.GetMemberCount(pPlayer->GetPartyGuid());
			switch( MemberCount )
			{
			case 1:	// ��Ƽ�� 1��
				{
					DecPerStageFatigability = SyncVariable.iDecPerStageFatigability_1;
				}break;
			case 2:	// ��Ƽ�� 2��
				{
					DecPerStageFatigability = SyncVariable.iDecPerStageFatigability_2;
				}break;
			case 3:	// ��Ƽ�� 3��
				{
					DecPerStageFatigability = SyncVariable.iDecPerStageFatigability_3;
				}break;
			case 4:	// ��Ƽ�� 4��
				{
					DecPerStageFatigability = SyncVariable.iDecPerStageFatigability_4;
				}break;
			default:
				{
					DecPerStageFatigability = SyncVariable.iDecPerStageFatigability_1;
				}break;
			}
		}
		else
		{
			DecPerStageFatigability = SyncVariable.iDecPerStageFatigability_1;
		}
	}

	// ���� ���潺����� �÷��̾ �������..
	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator unit_iter;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL )
	{
		PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if( pPlayer )
		{
			int const Abil = pPlayer->GetAbil(AT_RECOVERY_STRATEGY_FATIGABILITY);	// �Ƿε� ȸ�� ���� ĳ�� �������� ����ߴٸ� �Ƿε� ���ҵ��� ����.
			if( 0 == Abil )
			{
				// ������ �Ƿε��� �����´�.
				short const PrevFatigability = static_cast<short>(pPlayer->GetAbil(AT_STRATEGY_FATIGABILITY));

				// �Ƿε��� �ٿ���.
				if( PrevFatigability > 0 )
				{
					short NewFatigability = PrevFatigability - DecPerStageFatigability;
					if( NewFatigability < 0 )
					{	// 0���� �۾����� 0���� �ǵ���.
						NewFatigability = 0;
					}

					// ���ο� �Ƿε��� Set.
					pPlayer->SetAbil(AT_STRATEGY_FATIGABILITY, static_cast<short>(NewFatigability), true);	// �Ƿε��� �����ؼ� UI ǥ�ÿ� ���.
				}
			}
		}
	}
}

// ���� ���潺 ��忡�� �Ƿε��� ���� �߰� ����ġ ���� ���.
void PgMissionGround::CalcBonusExpDefence7()
{
	if( !IsDefenceMode7() )
	{	// F7��尡 �ƴϸ� �ϸ� �ȵ�.
		return;
	}

	// ���� ���潺����� �÷��̾ �������..
	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator unit_iter;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL )
	{
		PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if( pPlayer )
		{
			// ������ �Ƿε��� �����´�.
			float Fatigability = static_cast<float>(pPlayer->GetAbil(AT_STRATEGY_FATIGABILITY));
			
			SSyncVariable SyncVariable = g_kEventView.VariableCont();

			float BonusExpRate = SyncVariable.fBonusExpRate;		// �ּ� �߰� ����.
			float FatigabilityDivValue = static_cast<float>(SyncVariable.iFatigabilityDivValue);	// �Ƿε� ���.

			float FinalRate = (BonusExpRate + (Fatigability / FatigabilityDivValue));

			int TemResult = static_cast<int>((FinalRate * 10));	// �Ҽ��� 1�ڸ� ���� ����.
			FinalRate = static_cast<float>(TemResult) / 10;

			pPlayer->DefenceMssionExpBonusRate(FinalRate);
		}
	}
}

void PgMissionGround::ConstellationResultInit()
{
	if(GetConstellationKey().PartyGuid.IsNotNull())
	{
		m_kConstellationResult.OnInit(this);
		PgPlayer* pkPlayer = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
		while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
		{
			m_kConstellationResult.OnSendMapLoadComplete(pkPlayer);
		}
	}
}