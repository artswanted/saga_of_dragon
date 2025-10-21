#include "StdAfx.h"
#include "constant.h"
#include "BM/Guid.h"
#include "TableDataManager.h"

#include "PgAchievements.h"

PgAchievements& PgAchievements::operator=( PgAchievements const &rhs )
{
	::memcpy( m_byteAchievements, rhs.m_byteAchievements, MAX_ACHIEVEMENTS_BYTES );
	m_kContTimeLimit = rhs.m_kContTimeLimit;
	return *this;
}

void PgAchievements::Init()
{
	memset( m_byteAchievements, 0, sizeof(m_byteAchievements) );
	CONT_ACHIEVEMENT_TIMELIMIT().swap(m_kContTimeLimit);
}

bool const PgAchievements::IsComplete(int const iIdx) const
{
	int iByteOffset = 0;
	BYTE bValue = 0;

	if(CalcIDToOffset(iIdx,false,iByteOffset,bValue))
	{
		return ((m_byteAchievements[iByteOffset] & bValue) == bValue);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgAchievements::SetAchievementTimeLimit(int const iSaveIdx,BM::PgPackedTime const & kTimeLimit)
{
	m_kContTimeLimit[iSaveIdx] = kTimeLimit;
}

bool PgAchievements::ResetAchievementTimeLimit(int const iSaveIdx)
{
	CONT_ACHIEVEMENT_TIMELIMIT::iterator iter = m_kContTimeLimit.find(iSaveIdx);
	if(iter == m_kContTimeLimit.end())
	{
		return false;
	}
	m_kContTimeLimit.erase(iter);
	return true;
}

bool PgAchievements::GetAchievementTimeLimit(int const iSaveIdx,BM::PgPackedTime & kTimeLimit) const
{
	CONT_ACHIEVEMENT_TIMELIMIT::const_iterator iter = m_kContTimeLimit.find(iSaveIdx);
	if(iter == m_kContTimeLimit.end())
	{
		return false;
	}

	kTimeLimit = (*iter).second;
	return true;
}

void PgAchievements::ProcessAchievementTimeOut(BM::GUID const & kOwnerGuid, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder, CONT_ACHIEVEMENT_TIMEOUTED & kTimeOuted) const
{
	BM::DBTIMESTAMP_EX kCurTime;
	g_kEventView.GetLocalTime(kCurTime);

	for(CONT_ACHIEVEMENT_TIMELIMIT::const_iterator iter = m_kContTimeLimit.begin();iter != m_kContTimeLimit.end();++iter)
	{
		if((*iter).second < static_cast<BM::PgPackedTime>(kCurTime))
		{
			rkContModifyOrder.push_back(SPMO(IMET_SET_ACHIEVEMENT,kOwnerGuid, SPlayerModifyOrderData_ModifyAchievement((*iter).first,0)));
			kTimeOuted.insert((*iter).first);
		}
	}
}

bool const PgAchievements::CheckHasItem(int const iIdx) const
{
	int iByteOffset = 0;
	BYTE bValue = 0;

	if(CalcIDToOffset(iIdx,true,iByteOffset,bValue))
	{
		return ((m_byteAchievements[iByteOffset] & bValue) == bValue);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAchievements::ResetItem(int const iIdx,bool const IsPopItem)
{
	int iByteOffset = 0;
	BYTE bValue = 0;

	if(CalcIDToOffset(iIdx,true,iByteOffset,bValue))
	{
		if(IsPopItem)
		{
			m_byteAchievements[iByteOffset] &= ~bValue;
		}
		else
		{
			m_byteAchievements[iByteOffset] |= bValue;
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAchievements::Complete(int const iIdx)
{
	int iByteOffset1 = 0,iByteOffset2 = 0;
	BYTE bValue1 = 0,bValue2 = 0;

	if(CalcIDToOffset(iIdx,false,iByteOffset1,bValue1) && CalcIDToOffset(iIdx,true,iByteOffset2,bValue2))
	{
		m_byteAchievements[iByteOffset1] |= bValue1;
		m_byteAchievements[iByteOffset2] |= bValue2;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAchievements::Reset(int const iIdx)
{
	int iByteOffset1 = 0,iByteOffset2 = 0;
	BYTE bValue1 = 0,bValue2 = 0;

	if(CalcIDToOffset(iIdx,false,iByteOffset1,bValue1) && CalcIDToOffset(iIdx,true,iByteOffset2,bValue2))
	{
		m_byteAchievements[iByteOffset1] &= ~bValue1;
		m_byteAchievements[iByteOffset2] &= ~bValue2;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgAchievements::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push(m_byteAchievements,MAX_ACHIEVEMENTS_BYTES);
	PU::TWriteTable_AA(kPacket, m_kContTimeLimit);
	PU::TWriteTable_AA(kPacket, m_kContPoint);
}

void PgAchievements::ReadFromPacket(BM::Stream & kPacket)
{
	kPacket.PopMemory(m_byteAchievements,MAX_ACHIEVEMENTS_BYTES);
	PU::TLoadTable_AA(kPacket, m_kContTimeLimit);
	PU::TLoadTable_AA(kPacket, m_kContPoint);
}

bool PgAchievements::CalcIDToOffset(int const iIdx,bool const bCheckHasItem,int & iByteOffset,BYTE & bValue) const
{
	if(iIdx < MAX_ACHIEVEMENTS_NUM)
	{
		int iBitPos = iIdx * ACHIEVEMENT_BIT_NUM + (bCheckHasItem ? 1 : 0);
		iByteOffset = (iBitPos / 8);
		bValue = (0x01 << (iBitPos % 8));
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgAchievements::LoadDBAchievement(CEL::DB_DATA_ARRAY::const_iterator & itor)
{
	(*itor).PopMemory(m_byteAchievements,MAX_ACHIEVEMENTS_BYTES); ++itor;
}

void PgAchievements::LoadDBAchievementTimeLimit(CEL::DB_DATA_ARRAY::const_iterator & itor, int const iAchievementsTimeLimitCount)
{
	for(int i = 0;i < iAchievementsTimeLimitCount;++i)
	{
		int iIdx = 0;
		int iTime = 0;
		(*itor).Pop(iIdx);	++itor;
		(*itor).Pop(iTime); ++itor;
		BM::PgPackedTime kPackedTime;
		kPackedTime.SetTime(iTime);
		SetAchievementTimeLimit(iIdx,kPackedTime);
	}
}
void PgAchievements::LoadDBAchievementPoint(CEL::DB_DATA_ARRAY::const_iterator & itor, int const iPointCount)
{
	for( int iCur = 0; iPointCount > iCur; ++iCur )
	{
		int iCategory = 0, iPoint = 0;
		(*itor).Pop( iCategory );	++itor;
		(*itor).Pop( iPoint );		++itor;
		SetAchievementPoint(static_cast< EAchievementsCategory >(iCategory), iPoint);
	}
	CalcTotalAchievementPoint();
}
void PgAchievements::AddAchievementPoint(EAchievementsCategory const eType, int const iAddPoint)
{
	switch( eType )
	{
	case AC_CATEGORY_01:
	case AC_CATEGORY_02:
	case AC_CATEGORY_03:
	case AC_CATEGORY_04:
	case AC_CATEGORY_05:
	case AC_CATEGORY_06:
		{
		}break;
	case AC_TOTAL:
	default:
		{
			return;
		}break;
	}

	CONT_AVCHIEVEMENT_POINT::iterator find_iter = m_kContPoint.find( eType );
	if( m_kContPoint.end() == find_iter )
	{
		SetAchievementPoint(eType, iAddPoint);
	}
	else
	{
		(*find_iter).second += iAddPoint;
	}
	CalcTotalAchievementPoint();
}
void PgAchievements::SetAchievementPoint(EAchievementsCategory const eType, int const iPoint)
{
	switch( eType )
	{
	case AC_TOTAL:
	case AC_CATEGORY_01:
	case AC_CATEGORY_02:
	case AC_CATEGORY_03:
	case AC_CATEGORY_04:
	case AC_CATEGORY_05:
	case AC_CATEGORY_06:
		{
		}break;
	default:
		{
			return;
		}break;
	}

	CONT_AVCHIEVEMENT_POINT::iterator find_iter = m_kContPoint.find( eType );
	if( m_kContPoint.end() == find_iter )
	{
		m_kContPoint.insert( std::make_pair(eType, iPoint) );
	}
	else
	{
		(*find_iter).second = iPoint;
	}
}
int PgAchievements::GetAchievementPoint(EAchievementsCategory const eType) const
{
	CONT_AVCHIEVEMENT_POINT::const_iterator find_iter = m_kContPoint.find( eType );
	if( m_kContPoint.end() == find_iter )
	{
		return 0;
	}
	return (*find_iter).second;
}
void PgAchievements::CalcTotalAchievementPoint()
{
	int iSumPoint = 0;
	iSumPoint += GetAchievementPoint(AC_CATEGORY_01);
	iSumPoint += GetAchievementPoint(AC_CATEGORY_02);
	iSumPoint += GetAchievementPoint(AC_CATEGORY_03);
	iSumPoint += GetAchievementPoint(AC_CATEGORY_04);
	iSumPoint += GetAchievementPoint(AC_CATEGORY_05);
	iSumPoint += GetAchievementPoint(AC_CATEGORY_06);
	SetAchievementPoint(AC_TOTAL, iSumPoint);
}