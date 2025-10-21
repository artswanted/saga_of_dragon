#include "stdafx.h"
#include "PgRequest.h"
#include "Variant/PgMission.h"
#include "Variant/PgMissionInfo.h"
#include "Variant/PgQuestInfo.h"
#include "PgQuest.h"
#include "PgMissionMan.h"
#include "PgGround.h"
#include "PgConstellationMgr.h"

bool PgRequest::Send()const
{
	return SendToMissionMgr( m_kPacket );
}

PgRequest_MissionInfo::PgRequest_MissionInfo(int const iMissionKey, SGroundKey const& _rkGndkey, int rkType)
:	PgRequest( PT_M_N_REQ_MISSION_INFO)
,	m_iMissionKey(iMissionKey)
,	m_iType(rkType)
{
	m_kPacket.Push(_rkGndkey);
	m_kPacket.Push(m_iMissionKey);
	m_kPacket.Push(m_iType);
}

bool PgRequest_MissionInfo::DoAction(PgPlayer *pkPlayer)
{
	PgPlayer_MissionData const *pkMissionData = pkPlayer->GetMissionData( (unsigned int)m_iMissionKey );
	if ( pkMissionData )
	{
		pkMissionData->WriteToPacket( m_kPacket );
		m_kPacket.Push(pkPlayer->GetID());
//		m_kPacket.Push(pkPlayer->GetMemberGUID());
		m_kPacket.Push(pkPlayer->GetAbil(AT_LEVEL));
		return Send();
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

PgRequest_MissionJoin::PgRequest_MissionJoin(SMissionKey const& _rkMissionKey, PgGround const* pkGround, int const rkType, BM::Stream * const pkPacket)
:	PgRequest(PT_M_N_REQ_ENTER_MISSION)	
,	m_MissionKey(_rkMissionKey)
,	m_iType(rkType)
,   m_pkAddonPacket(*pkPacket)
{
	m_kPacket.Push(_rkMissionKey);
	m_kPacket.Push(pkGround->GroundKey());
	m_kPacket.Push(m_iType);

	GetConstellationMission( pkGround->GetConstellationKey(), m_kConstellationMission );
}

bool PgRequest_MissionJoin::DoAction(PgPlayer *pkPlayer)
{
	ConStage RecommendMissionMap;
	PgRequestUtil::GetRecommendMissionMap(pkPlayer, m_MissionKey, RecommendMissionMap);

	m_kPacket.Push(pkPlayer->GetID());	// Owner
	m_kPacket.Push(pkPlayer->GetAbil(AT_LEVEL));	// Contents Server가 레벨을 검사하여야 한다.
	m_kConstellationMission.WriteToPacket(m_kPacket);
	PU::TWriteArray_A(m_kPacket, RecommendMissionMap);
	m_kPacket.Push(m_pkAddonPacket);
	return Send();
}

PgRequest_MissionReStart::PgRequest_MissionReStart(BM::GUID const & _kMissionID, SMissionKey const& _kReMissionKey, BM::Stream * const pkPacket)
:	PgRequest(PT_C_M_REQ_MISSION_RESTART)
,	m_MissionKey(_kReMissionKey)
,   m_pkAddonPacket(*pkPacket)
{
	m_kPacket.Push(_kMissionID);
	m_kPacket.Push(_kReMissionKey);
}

bool PgRequest_MissionReStart::DoAction(PgPlayer *pkPlayer)
{
	ConStage RecommendMissionMap;
	PgRequestUtil::GetRecommendMissionMap(pkPlayer, m_MissionKey, RecommendMissionMap);

	m_kPacket.Push(pkPlayer->GetAbil(AT_LEVEL));
	PU::TWriteArray_A(m_kPacket, RecommendMissionMap);
	m_kPacket.Push(m_pkAddonPacket);
	return Send();
}

PgRequest_Notice::PgRequest_Notice( E_NOTICE_TYPE kType )
:	PgRequest(PT_M_N_NFY_NOTICE_PACKET)
,	m_kType(kType)
{
}

bool PgRequest_Notice::DoAction( BM::Stream &kPacket )
{
	if ( m_kGuidList.empty() )
	{
		if ( m_kType == NOTICE_ALL )
		{
			m_kGuidList.push_back( BM::GUID::NullData() );
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return Send( kPacket );
}

void PgRequest_Notice::Add( BM::GUID const &kGuid )
{
	m_kGuidList.push_back( kGuid );
}

bool PgRequest_Notice::Send( BM::Stream &kPacket )
{
	m_kPacket.Push( m_kType );
	m_kPacket.Push( m_kGuidList );
	m_kPacket.Push( kPacket );
	return SendToContents( m_kPacket );
}

PgRequest_CheckPenalty::PgRequest_CheckPenalty( SGroundKey const &kGndKey, WORD const wType, BM::Stream * const pkPacket )
:	PgRequest(PT_M_N_REQ_CHECK_PENALTY)
,	m_kType(wType)
,	m_pkPacket(pkPacket)
{
	m_kPacket.Push( g_kProcessCfg.ChannelNo() );
	m_kPacket.Push( kGndKey );
}

bool PgRequest_CheckPenalty::DoAction( PgPlayer *pkPlayer )
{
	m_kPacket.Push( pkPlayer->GetID() );
	m_kPacket.Push( m_kType );

	if ( m_pkPacket )
	{
		size_t const iRDPos = m_pkPacket->RdPos();
		m_pkPacket->PosAdjust();
		m_kPacket.Push( *m_pkPacket );
		m_pkPacket->RdPos( iRDPos );
	}

	return Send();
}

bool PgRequest_CheckPenalty::Send()const
{
	return SendToContents( m_kPacket );
}


namespace PgRequestUtil
{
	bool GetRecommendMissionMap(PgPlayer * pPlayer, SMissionKey const & MissionKey, ConStage & RecommendMissionMap)
	{// 맵 추천 //
		if( NULL == pPlayer )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		
		const CONT_DEF_MISSION_ROOT* pContDefMission = NULL;	// 미션 루트 테이블 가져오기
		g_kTblDataMgr.GetContDef(pContDefMission);
		if( NULL == pContDefMission )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		PgMissionInfo const *pMissionInfo = NULL;
		if( false == g_kMissionMan.GetMissionKey(MissionKey.iKey, pMissionInfo) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		int RootLevelValue = 0;
		CONT_DEF_MISSION_ROOT::const_iterator mission_iter = pContDefMission->find( pMissionInfo->m_kBasic.iMissionID );
		if( pContDefMission->end() != mission_iter )
		{
			RootLevelValue = mission_iter->second.aiLevel[MissionKey.iLevel];
		}

		if( 0 == RootLevelValue )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		const CONT_DEF_MISSION_CANDIDATE* pContDefMissionCandi = NULL;	// 미션 Candidate 테이블 가져오기
		g_kTblDataMgr.GetContDef(pContDefMissionCandi);
		if( NULL == pContDefMissionCandi )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		CONT_DEF_MISSION_CANDIDATE::const_iterator candi_itr = pContDefMissionCandi->find(RootLevelValue);
		if( pContDefMissionCandi->end() == candi_itr )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		
		ConStage TempRecommend;
		SMissionStageKey StageKey;
		for( int i = 0; i < MAX_MISSION_CANDIDATE; ++i)
		{
			if( 0 != candi_itr->second.aiCandidate[i] )
			{
				StageKey.iGroundNo = candi_itr->second.aiCandidate[i];
				StageKey.kBitFalg = 0x01 << i;
				TempRecommend.push_back(StageKey);
			}
		}

		// 퀘스트 추천 ////////////////////////////////////////////////////////////////////////////////
		PgMyQuest const * pMyQuest = pPlayer->GetMyQuest();
		if( pMyQuest )
		{
			ContUserQuestState QuestCont;
			pMyQuest->GetQuestList(QuestCont);	// 유저가 진행중인 퀘스트 리스트를 가져온다.

			ContUserQuestState::const_iterator state_iter = QuestCont.begin();
			for( ; QuestCont.end() != state_iter; ++state_iter )
			{// 수행중인 퀘스트 수만큼 검사한다.
				ContUserQuestState::value_type const& UserQuestState = (*state_iter);
				if( QS_Ing == UserQuestState.byQuestState )
				{// 아직 완료되지 않고 진행중인 퀘스트만 해당된다.
					PgQuestInfo const* pQuestInfo;
					if( g_kQuestMan.GetQuest(UserQuestState.iQuestID, pQuestInfo) )
					{// 퀘스트 정보
						ContQuestMonster::const_iterator mon_iter = pQuestInfo->m_kDepend_Monster.begin();
						ContQuestGround::const_iterator gnd_iter = pQuestInfo->m_kDepend_Ground.begin();
						ContQuestLocation::const_iterator loc_iter = pQuestInfo->m_kDepend_Location.begin();

						for( ; pQuestInfo->m_kDepend_Location.end() != loc_iter; ++loc_iter )
						{
							ContQuestLocation::value_type const& QuestLocation = (*loc_iter);

							bool bQuestLocationClear = false;
							if( pQuestInfo->m_kObject.GetEndCount(QuestLocation.iObjectNo) > 0 )
							{// 퀘스트 목표 갯수가 0개 이상일 때
								if( UserQuestState.byParam[QuestLocation.iObjectNo] >= pQuestInfo->m_kObject.GetEndCount(QuestLocation.iObjectNo) )
								{// 해당 조건을 완료했다.
									bQuestLocationClear = true;
								}
							}

							if( QuestLocation.iGroundNo && (false == bQuestLocationClear) )
							{
								ConStage::const_iterator temp_iter = TempRecommend.begin();
								for( ; temp_iter != TempRecommend.end(); ++temp_iter )
								{// 모든 미션 맵들에 대해 검사한다.
									if( temp_iter->iGroundNo == QuestLocation.iGroundNo )
									{// 퀘스트에 해당하는 미션 맵이면
										bool bHit = false;
										ConStage::const_iterator rec_iter = RecommendMissionMap.begin();
										for( ; rec_iter != RecommendMissionMap.end(); ++rec_iter )
										{// 추가하기전에 이미 있는 그라운드인지 확인
											if( rec_iter->iGroundNo == temp_iter->iGroundNo )
											{
												bHit = true;
												break;
											}
										}
										if( false == bHit )
										{
											RecommendMissionMap.push_back( *temp_iter );
										}
									}
								}
							}
						}

						for( ; pQuestInfo->m_kDepend_Ground.end() != gnd_iter; ++gnd_iter )
						{// 컨테이너를 순회하며 퀘스트에 필요한 모든 그라운드를 비교
							ContQuestGround::mapped_type const& QuestGround = (*gnd_iter).second;
							ConStage::const_iterator temp_iter = TempRecommend.begin();
							for( ; temp_iter != TempRecommend.end(); ++temp_iter )
							{// 모든 미션 맵들에 대해 검사한다.
								if( temp_iter->iGroundNo == QuestGround.iGroundNo )
								{// 캔디데이트 맵에 해당 그라운드가 있는 맵이 있으면 컨테이너에 추가한다.
									bool bHit = false;
									ConStage::const_iterator rec_iter = RecommendMissionMap.begin();
									for( ; rec_iter != RecommendMissionMap.end(); ++rec_iter )
									{// 추가하기전에 이미 있는 그라운드인지 확인
										if( rec_iter->iGroundNo == temp_iter->iGroundNo )
										{
											bHit = true;
											break;
										}
									}
									if( false == bHit )
									{
										RecommendMissionMap.push_back( *temp_iter );
									}
								}
							}
						}

						for( ; pQuestInfo->m_kDepend_Monster.end() != mon_iter; ++mon_iter )
						{// 컨테이너를 순회하며 퀘스트에 필요한 모든 몬스터를 비교
							ContQuestMonster::mapped_type const& QuestMonster = (*mon_iter).second;

							bool bQuestMonsterClear = false;
							if( pQuestInfo->m_kObject.GetEndCount(QuestMonster.iObjectNo) > 0 )
							{// 퀘스트 목표 갯수가 0개 이상일 때
								if( UserQuestState.byParam[QuestMonster.iObjectNo] >= pQuestInfo->m_kObject.GetEndCount(QuestMonster.iObjectNo) )
								{// 해당 조건을 완료했다.
									bQuestMonsterClear = true;
								}
							}

							if( false == bQuestMonsterClear )
							{
								ConStage::const_iterator temp_iter = TempRecommend.begin();
								for( ; temp_iter != TempRecommend.end(); ++temp_iter )
								{// 모든 미션 맵들에 대해 검사한다.
									if( temp_iter->iGroundNo == QuestMonster.iTargetGroundNo )
									{// 캔디데이트 맵에 해당 몬스터가 있는 맵이 있으면 컨테이너에 추가한다.
										bool bHit = false;
										ConStage::const_iterator rec_iter = RecommendMissionMap.begin();
										for( ; rec_iter != RecommendMissionMap.end(); ++rec_iter )
										{// 추가하기전에 이미 있는 그라운드인지 확인
											if( rec_iter->iGroundNo == temp_iter->iGroundNo )
											{
												bHit = true;
												break;
											}
										}
										if( false == bHit )
										{
											RecommendMissionMap.push_back( *temp_iter );
										}
									}
								}
							}
						}
					}
				}
			}
		}
		// 퀘스트 추천 끝 /////////////////////////////////////////////////////////////////////////////

		// 달성률 추천 ////////////////////////////////////////////////////////////////////////////////
		PgPlayer_MissionData const *pMissionData = pPlayer->GetMissionData( (unsigned int)MissionKey.iKey );
		if ( pMissionData )
		{
			int StageNo = 0;
			ConStage::const_iterator temp_iter = TempRecommend.begin();
			for( ; temp_iter != TempRecommend.end(); ++temp_iter )
			{
				if( false == pMissionData->IsClearStage(MissionKey.iLevel, StageNo) )
				{
					bool bHit = false;
					ConStage::const_iterator rec_iter = RecommendMissionMap.begin();
					for( ; rec_iter != RecommendMissionMap.end(); ++rec_iter )
					{// 추가하기전에 이미 있는 그라운드인지 확인
						if( rec_iter->iGroundNo == temp_iter->iGroundNo )
						{
							bHit = true;
							break;
						}
					}
					if( false == bHit )
					{
						RecommendMissionMap.push_back( *temp_iter );
					}
				}
				++StageNo;
			}
		}
		// 달성률 추천 끝 /////////////////////////////////////////////////////////////////////////////
		return true;
	}
}