#include "stdafx.h"
#include "PgMission_Result.h"

PgMission_Result::PgMission_Result()
{
	Clear();
}

PgMission_Result::~PgMission_Result()
{

}

void PgMission_Result::Clear()
{
	m_kConRankData.clear();
}

void PgMission_Result::WriteToPacket(BM::Stream& rkPacket)const
{
	PU::TWriteArray_M(rkPacket,m_kConRankData);
}

void PgMission_Result::ReadFromPacket(BM::Stream& rkPacket)
{
	m_kConRankData.clear();
	PU::TLoadArray_M(rkPacket,m_kConRankData);
}

PgMission_ResultC::PgMission_ResultC()
{
	Clear();
}

PgMission_ResultC::~PgMission_ResultC()
{

}

void PgMission_ResultC::Clear()
{
	PgMission_Result::Clear();
	m_iMissionNo = 0;
	m_kMissionKey.Clear();
	m_iClearTime = 0;
	m_iClearTimePoint = 0;
	m_iClearTime_Top = 0;
	m_iClearTime_Avg = 0;
	m_kConUserInfo.clear();
	::memset(m_kPoint,0,sizeof(m_kPoint));
	::memset(m_kBonusPoint,0,sizeof(m_kBonusPoint));
	m_kView = VIEWTYPE_NONE;
	m_kMemoGuid.Clear();
	m_iStanding = 0;
	IsOwner(false);

	kTotalScore = 0;
	m_kSense.Clear();
	m_kAbility.Clear();
	m_kPenalty.Clear();
	kGadaCoinCount = 0;
	kGadaCoinNeedCount = 0;
	iRetContNo = 0;
	iType = 0;
	iPoint = 0;
	iWinTeam = 0;
	iWinItemNo = 0;
	iAddItemResult_No = 0;
	m_bLastStage = false;
	m_i64BonusExp = 0;
	m_bResultStage = false;
	m_bConstellationMission = false;
}

void PgMission_ResultC::ReadFromPacket(BM::Stream& rkPacket)
{
	// 절대로 Clear()함수를 호출하지 말것!!!

	BYTE kView;
	rkPacket.Pop(kView);

	switch( kView )
	{
	case VIEWTYPE_RESULT:
		{
			Clear();
			rkPacket.Pop(m_iMissionNo);
			rkPacket.Pop(m_kMissionKey);
			rkPacket.Pop(m_iClearTime);
			rkPacket.Pop(m_iClearTimePoint);
			rkPacket.Pop(m_iClearTime_Top);
			rkPacket.Pop(m_iClearTime_Avg);
			PU::TLoadArray_M(rkPacket,m_kConUserInfo);
			rkPacket.PopMemory(m_kPoint,sizeof(m_kPoint));
			rkPacket.PopMemory(m_kBonusPoint,sizeof(m_kBonusPoint));
			rkPacket.Pop(m_kMemoGuid);
			rkPacket.Pop(m_iStanding);
			rkPacket.Pop(m_bOwner);

			rkPacket.Pop(kTotalScore);
			rkPacket.Pop(m_kSense);
			rkPacket.Pop(m_kAbility);
			rkPacket.Pop(m_kPenalty);
			rkPacket.Pop(kGadaCoinCount);			
			rkPacket.Pop(kGadaCoinNeedCount);
			rkPacket.Pop(iRetContNo);
			rkPacket.Pop(iType);
			rkPacket.Pop(iPoint);
		}// Break을 걸지 않는다.
	case VIEWTYPE_RANK:
		{
			PgMission_Result::ReadFromPacket(rkPacket);
		}break;
	case VIEWTYPE_DEFENCE:
		{
			Clear();

			rkPacket.Pop(m_bConstellationMission);
			rkPacket.Pop(m_bLastStage);
			rkPacket.Pop(m_i64BonusExp);
			rkPacket.Pop(m_iMissionNo);
			rkPacket.Pop(m_kMissionKey);
			rkPacket.Pop(m_iSuccessCount);
			rkPacket.Pop(m_bPrevStage_UseSelectItem);
			rkPacket.Pop(m_iMonsterNo);
			rkPacket.Pop(m_iItemNo);
			rkPacket.Pop(m_iDropRate);
			rkPacket.Pop(m_bResultStage);
			if( m_bResultStage )
			{
				PU::TLoadArray_M(rkPacket,m_kConUserInfo);
				rkPacket.Pop(m_bOwner);
				rkPacket.Pop(kGadaCoinCount);			
				rkPacket.Pop(kGadaCoinNeedCount);
				rkPacket.Pop(iRetContNo);
				rkPacket.Pop(iType);
			}
		}break;
	case VIEWTYPE_DEFENCE7:
		{
			Clear();

			rkPacket.Pop(m_bConstellationMission);
			rkPacket.Pop(m_bLastStage);
			rkPacket.Pop(m_iMissionNo);
			rkPacket.Pop(m_iStageCount);
			rkPacket.Pop(m_kMissionKey);
			rkPacket.Pop(m_bResultStage);
			if( m_bResultStage )
			{
				PU::TLoadArray_M(rkPacket,m_kConUserInfo);
				rkPacket.Pop(m_bOwner);
				rkPacket.Pop(kGadaCoinCount);			
				rkPacket.Pop(kGadaCoinNeedCount);
				rkPacket.Pop(iRetContNo);
				rkPacket.Pop(iType);
				rkPacket.Pop(iAddItemResult_No);
			}
		}break;
	case VIEWTYPE_DEFENCE8:
		{
			Clear();

			rkPacket.Pop(m_bConstellationMission);
			rkPacket.Pop(m_bLastStage);
			rkPacket.Pop(m_iMissionNo);
			rkPacket.Pop(m_iStageCount);
			rkPacket.Pop(m_kMissionKey);
			rkPacket.Pop(m_bResultStage);
			if( m_bResultStage )
			{
				PU::TLoadArray_M(rkPacket,m_kConUserInfo);
				rkPacket.Pop(m_bOwner);
				rkPacket.Pop(kGadaCoinCount);			
				rkPacket.Pop(kGadaCoinNeedCount);
				rkPacket.Pop(iRetContNo);
				rkPacket.Pop(iType);
				rkPacket.Pop(iAddItemResult_No);
			}
			if( m_bLastStage )
			{			
				// 승리 팀, 승리하여 얻는 아이템(실패한 유저도 패킷은 동일하게 받지만, 실제 지급은 안된다)
				rkPacket.Pop(iWinTeam);
				rkPacket.Pop(iWinItemNo);
			}
		}break;
	}

	m_kView |= kView;
}

bool const PgMission_ResultC::GetInfoFromUserInfoAt(size_t const iAt, SMissionPlayerInfo_Client& rkInfo)const
{
	if (m_kConUserInfo.size()  <= iAt || 0 > iAt)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	rkInfo = m_kConUserInfo.at(iAt);
	return true;
}

int const PgMission_ResultC::GetPoint(int const iAt) const
{
	if (iAt >= MPOINT_MAX || 0 > iAt)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	return m_kPoint[iAt];
}

int	const PgMission_ResultC::GetBonusPoint(int const iAt) const
{
	if( iAt >= MBONUSPOINT_MAX || 0 > iAt)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	return m_kBonusPoint[iAt];
}

int const PgMission_ResultC::GetSense()const
{
	return m_kSense.GetSensePoint();
}

int const PgMission_ResultC::GetAbility()const
{
	return m_kAbility.GetAbilityPoint();
}

int const PgMission_ResultC::GetPenalty()const
{
	return m_kPenalty.GetPenaltyPoint();
}

int const PgMission_ResultC::GetGadaCoin()const
{
	return static_cast<int>(kGadaCoinCount);
}

int const PgMission_ResultC::GetGadaCoinNeedCount()const
{
	return static_cast<int>(kGadaCoinNeedCount);
}

int const PgMission_ResultC::GetMissionType()const
{
	return static_cast<int>(iType);
}

int const PgMission_ResultC::GetMissionPoint()const
{
	return iPoint;
}

int const PgMission_ResultC::GetRetContNo()const
{
	return iRetContNo;
}

int const PgMission_ResultC::GetTotalScore()const
{
	return kTotalScore;
}
