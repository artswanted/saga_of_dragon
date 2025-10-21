#include "stdafx.h"
#include "Lohengrin/GameTime.h"
#include "PgEventQuestBase.h"

namespace PgEventQuestBaseUtil
{
	// <=
	bool CompareTimeLessEqualThan(BM::PgPackedTime const& rkCurTime, BM::PgPackedTime const& rkStartTime, __int64 const iPlayTime)
	{
		SYSTEMTIME const kCurTime = rkCurTime;
		SYSTEMTIME const kStartTime = rkStartTime;
		__int64 iCurTime = 0, iStartTime = 0;
		CGameTime::SystemTime2SecTime(kCurTime, iCurTime);
		CGameTime::SystemTime2SecTime(kStartTime, iStartTime);
		__int64 const iEndTime = iStartTime + iPlayTime;
		return (iStartTime <= iCurTime) && (iCurTime <= iEndTime);
	}
};


//
SEventQuestItem::SEventQuestItem()
	: iItemNo(0), iCount(0)
{
}
SEventQuestItem::~SEventQuestItem()
{
}

bool SEventQuestItem::IsEmpty() const
{
	return 0 == iItemNo
		&&	0 == iCount;
}
size_t SEventQuestItem::min_size() const
{
	return sizeof(iItemNo) + sizeof(iCount);
}
void SEventQuestItem::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( iItemNo );
	rkPacket.Push( iCount );
}
void SEventQuestItem::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( iItemNo );
	rkPacket.Pop( iCount );
}
bool SEventQuestItem::operator ==(int const rhs) const
{
	return iItemNo == rhs;
}

//
SEventQuestNotice::SEventQuestNotice()
	: iTime(0), kMessage()
{
}
SEventQuestNotice::~SEventQuestNotice()
{
}
size_t SEventQuestNotice::min_size() const
{
	return sizeof(iTime) + (sizeof(std::wstring::value_type) * kMessage.size());
}
void SEventQuestNotice::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( iTime );
	rkPacket.Push( kMessage );
}
void SEventQuestNotice::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( iTime );
	rkPacket.Pop( kMessage );
}
bool SEventQuestNotice::operator < (SEventQuestNotice const& rhs) const
{
	return iTime < rhs.iTime;
}

//
PgEventQuest::PgEventQuest()
{
	Clear();
}

PgEventQuest::~PgEventQuest()
{
}

void PgEventQuest::Clear()
{
	m_kStartTime.Clear();
	m_kEndTime = 0;
	m_kTitle.clear();
	m_kPrologue.clear();
	m_kInfo.clear();
	m_kNpcTalkPrologue.clear();
	m_kNpcTalkNotEnd.clear();
	m_kNpcTalkEnd.clear();
	m_kRewardMailTitle.clear();
	m_kRewardMailContents.clear();
	m_kTargetItem.clear();
	m_kRewardItem.clear();
	m_kRewardGold = 0;
}

void PgEventQuest::Set(PgEventQuest const& rhs)
{
	m_kStartTime = rhs.m_kStartTime;
	m_kEndTime = rhs.m_kEndTime;
	m_kTitle = rhs.m_kTitle;
	m_kPrologue = rhs.m_kPrologue;
	m_kInfo = rhs.m_kInfo;
	m_kNpcTalkPrologue = rhs.m_kNpcTalkPrologue;
	m_kNpcTalkNotEnd = rhs.m_kNpcTalkNotEnd;
	m_kNpcTalkEnd = rhs.m_kNpcTalkEnd;
	m_kRewardMailTitle = rhs.m_kRewardMailTitle;
	m_kRewardMailContents = rhs.m_kRewardMailContents;
	m_kTargetItem = rhs.m_kTargetItem;
	m_kRewardItem = rhs.m_kRewardItem;
	m_kRewardGold = rhs.m_kRewardGold;
}

void PgEventQuest::operator =(PgEventQuest const& rhs)
{
	Set(rhs);
}

bool PgEventQuest::IsEmpty() const
{
	return	BM::PgPackedTime() == m_kStartTime
		&&	0 == m_kEndTime;
}

bool PgEventQuest::IsCanRun(BM::PgPackedTime const& rkCurTime) const
{
	return PgEventQuestBaseUtil::CompareTimeLessEqualThan(rkCurTime, m_kStartTime, m_kEndTime);
}

void PgEventQuest::WriteToClientPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_kTitle );
	rkPacket.Push( m_kPrologue );
	rkPacket.Push( m_kInfo );
	rkPacket.Push( m_kNpcTalkPrologue );
	rkPacket.Push( m_kNpcTalkNotEnd );
	rkPacket.Push( m_kNpcTalkEnd );
	PU::TWriteArray_M(rkPacket, m_kTargetItem);
	PU::TWriteArray_M(rkPacket, m_kRewardItem);
	rkPacket.Push( m_kRewardGold );
}
void PgEventQuest::WriteToMapPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_kStartTime );
	rkPacket.Push( m_kEndTime );
	WriteToClientPacket(rkPacket);
}
void PgEventQuest::WriteToPacket(BM::Stream& rkPacket) const
{
	WriteToMapPacket(rkPacket);
	rkPacket.Push( m_kRewardMailTitle );
	rkPacket.Push( m_kRewardMailContents );
}

void PgEventQuest::ReadFromClientPacket(BM::Stream& rkPacket)
{
	m_kTargetItem.clear();
	m_kRewardItem.clear();
	rkPacket.Pop( m_kTitle );
	rkPacket.Pop( m_kPrologue );
	rkPacket.Pop( m_kInfo );
	rkPacket.Pop( m_kNpcTalkPrologue );
	rkPacket.Pop( m_kNpcTalkNotEnd );
	rkPacket.Pop( m_kNpcTalkEnd );
	PU::TLoadArray_M(rkPacket, m_kTargetItem);
	PU::TLoadArray_M(rkPacket, m_kRewardItem);
	rkPacket.Pop( m_kRewardGold );
}
void PgEventQuest::ReadFromMapPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( m_kStartTime );
	rkPacket.Pop( m_kEndTime );
	ReadFromClientPacket(rkPacket);
}
void PgEventQuest::ReadFromPacket(BM::Stream& rkPacket)
{
	ReadFromMapPacket(rkPacket);
	rkPacket.Pop( m_kRewardMailTitle );
	rkPacket.Pop( m_kRewardMailContents );
}
void PgEventQuest::ReadFromDBResult(CEL::DB_RESULT_COUNT::const_iterator& count_iter, CEL::DB_DATA_ARRAY::const_iterator& result_iter)
{
	Clear();

	int const iRewardCount = (*count_iter); ++count_iter;
	for( int iCur = 0; iRewardCount > iCur; ++iCur )
	{
		ContEventQuestReward::value_type kElement;
		(*result_iter).Pop( kElement.iItemNo );		++result_iter;
		(*result_iter).Pop( kElement.iCount );		++result_iter;

		if( EQE_MAX_REWARD_ITEM_COUNT > m_kRewardItem.size() ) // 최대 n개 (UI 제한폭이 n개)
		{
			m_kRewardItem.push_back( kElement );
		}
	}

	int const iTargetCount = (*count_iter); ++count_iter;
	for( int iCur = 0; iTargetCount > iCur; ++iCur )
	{
		ContEventQuestTarget::value_type kElement;
		(*result_iter).Pop( kElement.iItemNo );		++result_iter;
		(*result_iter).Pop( kElement.iCount );		++result_iter;

		if( m_kTargetItem.end() == std::find(m_kTargetItem.begin(), m_kTargetItem.end(), kElement.iItemNo) )
		{
			m_kTargetItem.push_back( kElement );
		}
	}

	int const iEventCount = (*count_iter); ++count_iter;
	for( int iCur = 0; iEventCount > iCur; ++iCur )
	{
		BM::DBTIMESTAMP_EX kStartTime;
		(*result_iter).Pop( kStartTime );				++result_iter;		m_kStartTime = kStartTime;
		(*result_iter).Pop( m_kEndTime );				++result_iter;
		(*result_iter).Pop( m_kTitle );					++result_iter;
		(*result_iter).Pop( m_kPrologue );				++result_iter;
		(*result_iter).Pop( m_kInfo );					++result_iter;
		(*result_iter).Pop( m_kNpcTalkPrologue );		++result_iter;
		(*result_iter).Pop( m_kNpcTalkNotEnd );			++result_iter;
		(*result_iter).Pop( m_kNpcTalkEnd );			++result_iter;
		(*result_iter).Pop( m_kRewardMailTitle );		++result_iter;
		(*result_iter).Pop( m_kRewardMailContents );	++result_iter;
		(*result_iter).Pop( m_kRewardGold );			++result_iter;
	}
}