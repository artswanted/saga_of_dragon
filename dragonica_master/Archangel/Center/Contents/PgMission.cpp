#include "stdafx.h"
#include "Variant/Global.h"
#include "PgMission.h"

//////////////////////////////////////////////////////////////////////////
//	PgMissionContents
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

PgMissionContents::PgMissionContents()
{
	Clear();
}

PgMissionContents::~PgMissionContents()
{

}

void PgMissionContents::Clear()
{
	PgMission::Clear();
}

SMissionStageKey PgMissionContents::GetBonusMapSelect(int const iSelectMapCount)
{
	SMissionStageKey kKey;

	kKey.iGroundNo = 0;
	kKey.kBitFalg = 0x01 << 7;

	if( 1 >= iSelectMapCount )
	{
		return kKey;
	}	

	CONT_MISSION_BONUSMAP const *pkBonusMap;
	g_kTblDataMgr.GetContDef(pkBonusMap);
	if( !pkBonusMap )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_BONUSMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkBonusMap is NULL"));
		return kKey;
	}

	CONT_MISSION_BONUSMAP::const_iterator iter = pkBonusMap->find(m_iCandidateNo);
	if( pkBonusMap->end() != iter )
	{
		// 설정된 보너스 맵이 존재 하는 경우
		CONT_MISSION_BONUSMAP::mapped_type const &kElement = (*iter).second;
		if( 0 < kElement.iUse )
		{
			// 보너스 맵이 걸릴 확률
			if( kElement.iChanceRate > BM::Rand_Index(ABILITY_RATE_VALUE) ) // 10000분률
			{
				// 보너스 맵 1번이 걸릴 확률				
				if( kElement.iBonus1stMapRate > BM::Rand_Index(ABILITY_RATE_VALUE) ) // 10000분률
				{
					kKey.iGroundNo = kElement.iBonusMap1;					
				}
				else
				{
					kKey.iGroundNo = kElement.iBonusMap2;
				}
			}			
		}
	}		

	return kKey;
}

void PgMissionContents::SetStageMap(BM::Stream* const pkPacket)
{
	int iMapType = 0;
	int iSelectMapCustom1 = 0;
	int iSelectMapCustom2 = 0;
	int iSelectMapCustom3 = 0;

	ConStage RecommendMissionMap;
	PU::TLoadArray_A(*pkPacket, RecommendMissionMap);

	pkPacket->Pop(iMapType);

	switch( static_cast<EMissionMapSelectType>(iMapType) )
	{
	case MMST_AUTO:
		{
			pkPacket->Pop(iSelectMapCustom1);			

			if ( iSelectMapCustom1 < m_kStage.size() )
			{
				ConStage::value_type kLastStage = m_kStage.back();
				m_kStage.pop_back();

				if( RecommendMissionMap.size() )
				{// 추천맵이 있다면 원래 컨테이너에서 추천맵과 중복되는 맵은 제거하고 두 컨테이너를 교체해 준다.
					ConStage::iterator Rec_iter, Ori_iter;
					for( Rec_iter = RecommendMissionMap.begin(); Rec_iter != RecommendMissionMap.end(); ++Rec_iter )
					{
						for( Ori_iter = m_kStage.begin(); Ori_iter != m_kStage.end(); ++Ori_iter )
						{
							if( Ori_iter->iGroundNo == Rec_iter->iGroundNo )
							{
								Ori_iter = m_kStage.erase(Ori_iter);
								break;
							}
						}
					}
					// m_kStage가 최종 사용될 원래 컨테이너. 중복체크를 했으므로 swap을 하게되면
					// m_kStage에는 추천맵만 들어있고 RecommendMissionMap에는 그외 맵만 들어있다.
					m_kStage.swap(RecommendMissionMap);
					if( m_kStage.size() > iSelectMapCustom1 )
					{
						while( m_kStage.size() > iSelectMapCustom1 )
						{// 갯수를 맞춘다. 선택된 맵 갯수보다 컨테이너가 더 크면 맞을때까지 하나씩 뺀다.
							m_kStage.pop_back();
						}
					}
					else if( m_kStage.size() < iSelectMapCustom1 )
					{
						while( m_kStage.size() < iSelectMapCustom1 )
						{// 마찬가지로 맵 갯수를 맞춘다. 컨테이너 크기가 더 작으면 하나씩 더해준다.
							m_kStage.push_back(*(RecommendMissionMap.rbegin()));
							RecommendMissionMap.pop_back();
						}
					}
					std::random_shuffle( m_kStage.begin(), m_kStage.end(), BM::Rand_Index );
				}
				else
				{// 추천맵이 비어있으면 전체 맵에서 랜덤으로 뽑는다.
					std::random_shuffle( m_kStage.begin(), m_kStage.end(), BM::Rand_Index );
					while( m_kStage.size() > iSelectMapCustom1 )
					{
						m_kStage.pop_back();
					}
				}

				m_iStageCountExceptBonus = m_kStage.size();

				SMissionStageKey kBonusKey = GetBonusMapSelect(iSelectMapCustom1);		// 2장 이상일 경우만 처리 된다
				if( 0 < kBonusKey.iGroundNo )
				{
					m_kStage.pop_back();
					m_kStage.push_back(kBonusKey);

					if( (1 < iSelectMapCustom1) && (1 < m_kStage.size()) )
					{
						std::random_shuffle( m_kStage.begin() + 1, m_kStage.end(), BM::Rand_Index );
					}
				}

				m_kStage.push_back(kLastStage);
				m_iStageCount = GetTotalStageCount();
			}			
		}break;
	case MMST_SELECT:
		{
			pkPacket->Pop(iSelectMapCustom1);
			pkPacket->Pop(iSelectMapCustom2);
			pkPacket->Pop(iSelectMapCustom3);

			VEC_INT kVec;
			if( 0 < iSelectMapCustom1 )
			{
				kVec.push_back(iSelectMapCustom1);
			}
			if( 0 < iSelectMapCustom2 )
			{
				kVec.push_back(iSelectMapCustom2);
			}
			if( 0 < iSelectMapCustom3 )
			{
				kVec.push_back(iSelectMapCustom3);
			}

			m_iStageCountExceptBonus = kVec.size();	// Quest Tag = ClearN 체크용.
			
			ConStage::value_type kLastStage = m_kStage.back();
			m_kStage.pop_back();

			ConStage kTempMap;
			VEC_INT::iterator iter = kVec.begin();
			while( kVec.end() != iter )
			{
				int const iSelectMap = (*iter);
				if( 0 < iSelectMap )
				{
					int const iIndex = iSelectMap - 1;
					if( 0 < GetStageGroundNo(iIndex) )
					{
						ConStage::value_type kElement = GetStage(iIndex);
						kTempMap.push_back(kElement);
					}
				}
				++iter;
			}

			m_kStage.swap(kTempMap);
			m_kStage.push_back(kLastStage);
			m_iStageCount = GetTotalStageCount();
		}break;
	default:
		{
		}break;
	}
}

bool PgMissionContents::SetMutator(BM::Stream * const pkPacket)
{
	int iMutatorCount = 0;
	std::set<int> kMutatorsSet;

	pkPacket->Pop(iMutatorCount);
	if(iMutatorCount > 0)
	{
		for( int i = 0; i < iMutatorCount; i++ )
		{
			int iTemp;
			pkPacket->Pop(iTemp);
			this->m_kMutators.insert(iTemp);// insert data to mutator list
		}
	}

	return true;
}

bool PgMissionContents::CreateRandomStage()
{
	if ( m_iStageCount == m_kStage.size() )
	{// StageCount랑 Stage갯수가 같으면 랜덤으로 만들지 않는다는 의미
		return true;
	}

	ConStage::value_type kLastStage = m_kStage.back();
	m_kStage.pop_back();
	std::random_shuffle( m_kStage.begin(), m_kStage.end(), BM::Rand_Index );
	while(m_kStage.size() >= m_iStageCount )
	{
		m_kStage.pop_back();
	}
	m_kStage.push_back(kLastStage);
	return true;
}

bool PgMissionContents::Start( const PgMission_Base& rhs, BM::GUID const &kMissionID, bool const bCopyStage, BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	Clear();
	if ( !PgMission_Base::Clone(rhs) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SetStageMap(pkPacket);
	SetMutator(pkPacket);

	m_kGuidID = kMissionID;

	if ( bCopyStage )
	{
		m_iStageCount = m_kStage.size();
		m_iRegistGndNo = GetStageGroundNo(0);
		return true;
	}
	
	
	if( CreateRandomStage() )
	{
		m_iRegistGndNo = GetStageGroundNo(0);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionContents::Restart(const PgMission_Base& rhs, BM::Stream* const pkPacket)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	PgMission_Base::Clear();
//	m_kOwnerGuid.Clear();
	m_kConUser.clear();
//	m_kGuidID.Clear();
	m_dwPlayTime = 0;
	m_iPlayTimePoint = 0;
//	m_iRegistGndNo = 0;
	if ( !PgMission_Base::Clone(rhs) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SetStageMap(pkPacket);

	return CreateRandomStage();
}

/*
HRESULT PgMissionContents::DoAction_Result()
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	PgMission_Rank* pkRank = g_kRankMgr.GetRank(m_kKey);
	
	int const iPlayTime = (int)GetPlayTime();
	int const iPlayTimePoint = GetPlayTimePoint();

	//Client로 보내줄 유저정보
	ConPlayRankInfo kCUserList;
	PgMission::ConUser::iterator user_itr;
	for(user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr)
	{
		ConPlayRankInfo::key_type kCUserData(user_itr->second,iPlayTimePoint,iPlayTime);
		kCUserList.insert(kCUserData);
	}

	PgMission_Result kTopRankData;
	int iClearTime_Top = 0;
	int iClearTime_Avg = 0;
	if ( pkRank )
	{
		pkRank->DoAction(kCUserList,iClearTime_Top,iClearTime_Avg);

		// TopRank를 뽑자
		pkRank->Get(kTopRankData.GetRankData(),1);
	}

	BYTE kWriteType = PgMission_Result::VIEWTYPE_RESULT;
	BM::Stream kCPacket(PT_N_C_NFY_MISSION_RESULT,kWriteType);
	kCPacket.Push(GetMissionNo());
	kCPacket.Push(m_kKey);
	kCPacket.Push(iPlayTime);
	kCPacket.Push(iPlayTimePoint);
	kCPacket.Push(iClearTime_Top);
	kCPacket.Push(iClearTime_Avg);
	kCPacket.Push(kCUserList.size());
	PgMission_Rank::ConPlayRankInfo::iterator cuser_itr;
	for(cuser_itr=kCUserList.begin();cuser_itr!=kCUserList.end();++cuser_itr)
	{
		//cuser_itr->WriteToPacket(kCPacket,(__int64)(cuser_itr->kRankKey.iPoint));
		// Client로 보내기 위한 Structure 타입으로 변환해서 보낼것.
		((SMissionPlayerInfo_Client const)(*cuser_itr)).WriteToPacket(kCPacket,(__int64)(cuser_itr->kRankKey.iPoint));
	}
	size_t const start_pos = kCPacket.WrPos();

	// 랭크를 계산하여서...

	HRESULT kResult = S_OK;
	for(cuser_itr=kCUserList.begin(); cuser_itr!=kCUserList.end(); ++cuser_itr)
	{
		kCPacket.WrPos(start_pos);
		kCPacket.Push(cuser_itr->kPoint, sizeof(cuser_itr->kPoint));
		kCPacket.Push(cuser_itr->kBonusPoint, sizeof(cuser_itr->kBonusPoint));
		BM::GUID const &rkMemGuid = cuser_itr->kMemoGuid;
		kCPacket.Push(rkMemGuid);
		kCPacket.Push(cuser_itr->iStanding);
		kCPacket.Push((bool)(cuser_itr->kCharGuid==GetOwner()));
		if ( rkMemGuid != BM::GUID::NullData() )
		{
			g_kRankMgr.AddRegister(pkRank,*cuser_itr);
			kResult = S_FALSE;
		}

		kTopRankData.WriteToPacket(kCPacket);
		g_kServerSetMgr.Locked_SendToUser(cuser_itr->kCharGuid,kCPacket,false);
	}

	return kResult;
}
*/

HRESULT PgMissionContents::DoAction_Result_Req(SGroundKey &kGndKey)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	int const iPlayTime = (int)GetPlayTime();
	int const iPlayTimePoint = GetPlayTimePoint();

	ConPlayRankInfo kCUserList;
	PgMission::ConUser::iterator user_itr;
	for(user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr)
	{
		ConPlayRankInfo::key_type kCUserData(user_itr->second,iPlayTimePoint,iPlayTime);
		kCUserData.iResultBagNo = (*user_itr).second.iResultBagNo;
		kCUserData.iScore = (*user_itr).second.iScore;
		
		if( kCUserData.kCharGuid != BM::GUID::NullData() )
		{
			SContentsUser kUser;
			if( S_OK == ::GetPlayerByGuid(kCUserData.kCharGuid , false, kUser) )
			{
				kCUserData.iLevel = kUser.sLevel;
			}
		}
		kCUserList.insert(kCUserData);
	}
	BM::Stream kReqPacket(PT_T_N_REQ_MISSION_RANKING, g_kProcessCfg.ServerIdentity());
	PU::TWriteKey_M(kReqPacket, kCUserList);
	kReqPacket.Push(GetID());
	m_kKey.WriteToPacket(kReqPacket);
	kReqPacket.Push(kGndKey);
	kReqPacket.Push(iPlayTime);
	kReqPacket.Push(iPlayTimePoint);
	SendToRankMgr(kReqPacket);
	return S_OK;
}

HRESULT PgMissionContents::DoAction_Result_Res(BM::Stream* const pkPacket)
{
	SMissionKey kMissionKey;
	PgMission_Result kTopRankData;
	int iClearTime_Top = 0;
	int iClearTime_Avg = 0;
	ConPlayRankInfo kCUserList;
	SGroundKey kGndKey;

	// Packet Reading ....
	kMissionKey.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kGndKey);
	PU::TLoadKey_M(*pkPacket, kCUserList);
	kTopRankData.ReadFromPacket(*pkPacket);
	pkPacket->Pop(iClearTime_Top);
	pkPacket->Pop(iClearTime_Avg);

	int const iPlayTime = (int)GetPlayTime();
	int const iPlayTimePoint = GetPlayTimePoint();

	/*
	PgMission::ConUser::iterator user_itr;
	for(user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr)
	{
		ConPlayRankInfo::key_type kCUserData(user_itr->second,iPlayTimePoint,iPlayTime);
		kCUserList.insert(kCUserData);
	}
	*/

	BYTE kWriteType = PgMission_Result::VIEWTYPE_RESULT;
	BM::Stream kCPacket(PT_N_C_NFY_MISSION_RESULT,kWriteType);
	kCPacket.Push(GetMissionNo());
	kCPacket.Push(m_kKey);
	kCPacket.Push(iPlayTime);
	kCPacket.Push(iPlayTimePoint);
	kCPacket.Push(iClearTime_Top);
	kCPacket.Push(iClearTime_Avg);
	kCPacket.Push(kCUserList.size());
	ConPlayRankInfo::iterator cuser_itr;
	for(cuser_itr=kCUserList.begin();cuser_itr!=kCUserList.end();++cuser_itr)
	{
		// Client로 보내기 위한 Structure 타입으로 변환해서 보낼것.		
		SMissionPlayerInfo_Client const &rkInfo = (*cuser_itr);
		rkInfo.WriteToPacket(kCPacket,(__int64)(cuser_itr->kRankKey.iPoint));
	}
	size_t const start_pos = kCPacket.WrPos();

	// 랭크를 계산하여서...
	HRESULT kResult = S_OK;
	for(cuser_itr=kCUserList.begin(); cuser_itr!=kCUserList.end(); ++cuser_itr)
	{
		kCPacket.WrPos(start_pos);
		kCPacket.Push(cuser_itr->kPoint, sizeof(cuser_itr->kPoint));
		kCPacket.Push(cuser_itr->kBonusPoint, sizeof(cuser_itr->kBonusPoint));
		BM::GUID const &rkMemGuid = cuser_itr->kMemoGuid;
		kCPacket.Push(rkMemGuid);
		kCPacket.Push(cuser_itr->iStanding);
		kCPacket.Push((bool)(cuser_itr->kCharGuid==GetOwner()));

		kCPacket.Push(cuser_itr->kTotalScore);
		kCPacket.Push(cuser_itr->m_kSense);
		kCPacket.Push(cuser_itr->m_kAbility);
		kCPacket.Push(cuser_itr->m_kPenalty);
		kCPacket.Push(cuser_itr->kGadaCoinCount);
		kCPacket.Push(cuser_itr->kGadaCoinNeedCount);
		kCPacket.Push(cuser_itr->iRetContNo);
		kCPacket.Push(cuser_itr->iType);
		kCPacket.Push(cuser_itr->kRankKey.iPoint);

		if ( rkMemGuid != BM::GUID::NullData() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
			kResult = S_FALSE;
		}
		kTopRankData.WriteToPacket(kCPacket);
		g_kServerSetMgr.Locked_SendToUser(cuser_itr->kCharGuid,kCPacket,false);

		//
		if( 0 < cuser_itr->iNewRank )
		{
			BM::Stream kItemPacket(PT_T_M_MISSION_RANK_RESULT_ITEM);
			kItemPacket.Push(cuser_itr->kCharGuid);
			kItemPacket.Push(cuser_itr->iNewRank);
			g_kServerSetMgr.Locked_SendToGround(kGndKey, kItemPacket, true);
		}
	}

	return kResult;
}