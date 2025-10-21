#include "stdafx.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/dbtables2.h"
#include "Lohengrin/GameTime.h"
#include "Unit.h"
#include "PgBattleSquare.h"

//
SBSItemPoint::SBSItemPoint()
	: iItemNo(0), iEffect1(0), iEffect2(0), iEffect3(0), iMinimapIcon(0)
{
}
SBSItemPoint::SBSItemPoint(SBSItemPoint const& rhs)
	: iItemNo(rhs.iItemNo), iEffect1(rhs.iEffect1), iEffect2(rhs.iEffect2), iEffect3(rhs.iEffect3), iMinimapIcon(rhs.iMinimapIcon)
{
}
SBSItemPoint::~SBSItemPoint()
{
}
bool SBSItemPoint::operator < (SBSItemPoint const& rhs) const
{
	return iItemNo < rhs.iItemNo;
}

//
tagBSRewardItem::tagBSRewardItem()
	: iMinPoint(0), iItemNo1(0), iCount1(0), iItemNo2(0), iCount2(0)
{
}
tagBSRewardItem::tagBSRewardItem(tagBSRewardItem const& rhs)
	: iMinPoint(rhs.iMinPoint), iItemNo1(rhs.iItemNo1), iCount1(rhs.iCount1), iItemNo2(rhs.iItemNo2), iCount2(rhs.iCount2)
{
}
tagBSRewardItem::~tagBSRewardItem()
{
}
bool tagBSRewardItem::operator <(tagBSRewardItem const& rhs) const
{
	return iMinPoint < rhs.iMinPoint;
}
bool tagBSRewardItem::operator ==(tagBSRewardItem const& rhs) const
{
	return iMinPoint == rhs.iMinPoint;
}

//
tagBSGame::tagBSGame()
	: iGameIDX(0), bUse(false), iChannelNameTextID(0)
	, iLevelMin(0), iLevelMax(0), iMaxUser(0)
	, iWeekOfDay(0), kStartTime()
	, iGameSec(0), iGroundNo(0), iPreOpenSec(0), iMapBagItemGroundNo(0), iGenGroupGroundNo(0), iMonsterBagControlNo(0)
{
}
tagBSGame::tagBSGame(tagBSGame const& rhs)
	: iGameIDX(rhs.iGameIDX), bUse(rhs.bUse), iChannelNameTextID(rhs.iChannelNameTextID)
	, iLevelMin(rhs.iLevelMin), iLevelMax(rhs.iLevelMax), iMaxUser(rhs.iMaxUser)
	, iWeekOfDay(rhs.iWeekOfDay), kStartTime(rhs.kStartTime)
	, iGameSec(rhs.iGameSec), iGroundNo(rhs.iGroundNo), iPreOpenSec(rhs.iPreOpenSec)
	, iMapBagItemGroundNo(rhs.iMapBagItemGroundNo), iGenGroupGroundNo(rhs.iGenGroupGroundNo), iMonsterBagControlNo(rhs.iMonsterBagControlNo)
{
}
tagBSGame::~tagBSGame()
{
}
bool tagBSGame::operator <(tagBSGame const& rhs) const
{
	return iGameIDX < rhs.iGameIDX;
}
void tagBSGame::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( iGameIDX );
	rkPacket.Push( bUse );
	rkPacket.Push( iChannelNameTextID );
	rkPacket.Push( iLevelMin );
	rkPacket.Push( iLevelMax );
	rkPacket.Push( iMaxUser );
	rkPacket.Push( iWeekOfDay );
	rkPacket.Push( kStartTime );
	rkPacket.Push( iGameSec );
	rkPacket.Push( iGroundNo );
	rkPacket.Push( iPreOpenSec );
	rkPacket.Push( iMapBagItemGroundNo );
	rkPacket.Push( iGenGroupGroundNo );
	rkPacket.Push( iMonsterBagControlNo );
}
void tagBSGame::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( iGameIDX );
	rkPacket.Pop( bUse );
	rkPacket.Pop( iChannelNameTextID );
	rkPacket.Pop( iLevelMin );
	rkPacket.Pop( iLevelMax );
	rkPacket.Pop( iMaxUser );
	rkPacket.Pop( iWeekOfDay );
	rkPacket.Pop( kStartTime );
	rkPacket.Pop( iGameSec );
	rkPacket.Pop( iGroundNo );
	rkPacket.Pop( iPreOpenSec );
	rkPacket.Pop( iMapBagItemGroundNo );
	rkPacket.Pop( iGenGroupGroundNo );
	rkPacket.Pop( iMonsterBagControlNo );
}
size_t tagBSGame::min_size()const
{
	return sizeof(*this);
}

//
PgBSGame::PgBSGame()
	: m_kGameInfo(), m_kContBSItem(), m_kContPrivateRewardItem(), m_kContPrivateLevelRewardItem(), m_kContWinTeamRewardItem(), m_kContWinBonusRewardItem(), m_kContLoseTeamRewardItem()
{
}
PgBSGame::PgBSGame(SBSGame const& rhs)
	: m_kGameInfo(rhs), m_kContBSItem(), m_kContPrivateRewardItem(), m_kContPrivateLevelRewardItem(), m_kContWinTeamRewardItem(), m_kContWinBonusRewardItem(), m_kContLoseTeamRewardItem()
{
}
PgBSGame::PgBSGame(BM::Stream& rkPacket)
{
	ReadFromPacket( rkPacket );
}
PgBSGame::PgBSGame(PgBSGame const& rhs)
{
	Set( rhs );
}
PgBSGame::~PgBSGame()
{
}
void PgBSGame::Set(PgBSGame const& rhs)
{
	m_kGameInfo = rhs.m_kGameInfo;
	m_kContBSItem = rhs.m_kContBSItem;
	m_kContPrivateRewardItem = rhs.m_kContPrivateRewardItem;
	m_kContPrivateLevelRewardItem = rhs.m_kContPrivateLevelRewardItem;
	m_kContWinTeamRewardItem = rhs.m_kContWinTeamRewardItem;
	m_kContLoseTeamRewardItem = rhs.m_kContLoseTeamRewardItem;
	m_kContWinBonusRewardItem = rhs.m_kContWinBonusRewardItem;
}
void PgBSGame::Clear()
{
	Set( PgBSGame() );
}
bool PgBSGame::AddWinTeamRewardItem(SBSRewardItem const& rkRewardItem)
{
	return AddRewardItem(m_kContWinTeamRewardItem, rkRewardItem);
}
bool PgBSGame::AddLoseTeamRewardItem(SBSRewardItem const& rkRewardItem)
{
	return AddRewardItem(m_kContLoseTeamRewardItem, rkRewardItem);
}
bool PgBSGame::AddPrivateRewardItem(SBSRewardItem const& rkRewardItem)
{
	return AddRewardItem(m_kContPrivateRewardItem, rkRewardItem);
}
bool PgBSGame::AddPrivateLevelRewardItem(SBSRewardItem const& rkRewardItem)
{
	return AddRewardItem(m_kContPrivateLevelRewardItem, rkRewardItem);
}
bool PgBSGame::AddWinBonusRewardItem(SBSRewardItem const& rkRewardItem)
{
	return AddRewardItem(m_kContWinBonusRewardItem, rkRewardItem);
}
bool PgBSGame::AddWinTeamRewardItem(CONT_BS_WIN_TEAM_REWARD_ITEM const& rkContRewardItem)				{ return AddRewardItem(m_kContWinTeamRewardItem,rkContRewardItem); }
bool PgBSGame::AddLoseTeamRewardItem(CONT_BS_WIN_TEAM_REWARD_ITEM const& rkContRewardItem)				{ return AddRewardItem(m_kContLoseTeamRewardItem,rkContRewardItem); }
bool PgBSGame::AddPrivateRewardItem(CONT_BS_PRIVATE_REWARD_ITEM const& rkContRewardItem)				{ return AddRewardItem(m_kContPrivateRewardItem,rkContRewardItem); }
bool PgBSGame::AddPrivateLevelRewardItem(CONT_BS_PRIVATE_LEVEL_REWARD_ITEM const& rkContRewardItem)		{ return AddRewardItem(m_kContPrivateLevelRewardItem,rkContRewardItem); }
bool PgBSGame::AddWinBonusRewardItem(CONT_BS_WIN_BONUS_REWARD_ITEM const& rkContRewardItem)				{ return AddRewardItem(m_kContWinBonusRewardItem,rkContRewardItem); }
bool PgBSGame::AddRewardItem(std::list< SBSRewardItem >& rkCont, std::list< SBSRewardItem > const& rkContSource)
{
	std::list< SBSRewardItem >::const_iterator iter = rkContSource.begin();
	while( rkContSource.end() != iter )
	{
		if( !AddRewardItem(rkCont, (*iter)) )
		{
			return false;
		}
		++iter;
	}
	return true;
}
bool PgBSGame::AddRewardItem(std::list< SBSRewardItem >& rkCont, SBSRewardItem const& rkRewardItem)
{
	if( rkCont.end() != std::find(rkCont.begin(), rkCont.end(), rkRewardItem) )
	{
		return false;
	}
	rkCont.push_back( rkRewardItem );
	rkCont.sort();
	return true;
}
void PgBSGame::SetBSItem(CONT_BS_ITEM const& rkContBSItem)
{
	m_kContBSItem = rkContBSItem;
}
void PgBSGame::WriteToPacket(BM::Stream &rkPacket) const
{
	m_kGameInfo.WriteToPacket( rkPacket );
	PU::TWriteTable_AM(rkPacket, m_kContBSItem);
	PU::TWriteArray_M(rkPacket, m_kContPrivateRewardItem);
	PU::TWriteArray_M(rkPacket, m_kContPrivateLevelRewardItem);
	PU::TWriteArray_M(rkPacket, m_kContWinTeamRewardItem);
	PU::TWriteArray_M(rkPacket, m_kContLoseTeamRewardItem);
	PU::TWriteArray_M(rkPacket, m_kContWinBonusRewardItem);
}
void PgBSGame::ReadFromPacket(BM::Stream &rkPacket)
{
	m_kGameInfo.ReadFromPacket( rkPacket );
	PU::TLoadTable_AM(rkPacket, m_kContBSItem);
	PU::TLoadArray_M(rkPacket, m_kContPrivateRewardItem);
	PU::TLoadArray_M(rkPacket, m_kContPrivateLevelRewardItem);
	PU::TLoadArray_M(rkPacket, m_kContWinTeamRewardItem);
	PU::TLoadArray_M(rkPacket, m_kContLoseTeamRewardItem);
	PU::TLoadArray_M(rkPacket, m_kContWinBonusRewardItem);
}
size_t PgBSGame::min_size() const
{
	return m_kGameInfo.min_size() + sizeof(SBSItemPoint) + sizeof(CONT_BS_WIN_TEAM_REWARD_ITEM) + sizeof(CONT_BS_PRIVATE_REWARD_ITEM)  + sizeof(CONT_BS_PRIVATE_LEVEL_REWARD_ITEM) + sizeof(CONT_BS_WIN_BONUS_REWARD_ITEM);
}


//
PgBSContentsGame::PgBSContentsGame()
	: PgBSGame(), m_kStatus(BSGS_NONE), m_kStartTime(0), m_kEndTime(0), m_kPreOpenTime(0), m_kLastNoticeTime(0)
{
}
PgBSContentsGame::PgBSContentsGame(PgBSGame const& rhs)
	: PgBSGame(rhs), m_kStatus(BSGS_NONE), m_kStartTime(0), m_kEndTime(0), m_kPreOpenTime(0), m_kLastNoticeTime(0)
{
}
PgBSContentsGame::~PgBSContentsGame()
{
}
void PgBSContentsGame::Set(PgBSContentsGame const& rhs)
{
	m_kStatus = rhs.m_kStatus;
	m_kStartTime = rhs.m_kStartTime;
	m_kEndTime = rhs.m_kEndTime;
	m_kPreOpenTime = rhs.m_kPreOpenTime;
	PgBSGame::Set(rhs);
}
void PgBSContentsGame::ReadFromServerPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( m_kStatus );
	rkPacket.Pop( m_kStartTime );
	rkPacket.Pop( m_kEndTime );
	rkPacket.Pop( m_kPreOpenTime );
	PgBSGame::ReadFromPacket( rkPacket );
}
void PgBSContentsGame::WriteToServerPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_kStatus );
	rkPacket.Push( m_kStartTime );
	rkPacket.Push( m_kEndTime );
	rkPacket.Push( m_kPreOpenTime );
	PgBSGame::WriteToPacket( rkPacket );
}



//
tagBSTeamMember::tagBSTeamMember()
	: kCharGuid(), kCharName(), usLevel(0), usClass(0), iPoint(0), usKill(0), usDead(0), usIconCount(0)
{
}
tagBSTeamMember::tagBSTeamMember(CUnit* pkUnit)
	: kCharGuid(), kCharName(), usLevel(0), usClass(0), iPoint(0), usKill(0), usDead(0), usIconCount(0)
{
	if( pkUnit )
	{
		kCharGuid = pkUnit->GetID();
		kCharName = pkUnit->Name();
		usLevel = static_cast<unsigned short>(pkUnit->GetAbil(AT_LEVEL));
		usClass = static_cast<unsigned short>(pkUnit->GetAbil(AT_CLASS));
	}
}
tagBSTeamMember::tagBSTeamMember(tagBSTeamMember const& rhs)
: kCharGuid(rhs.kCharGuid), kCharName(rhs.kCharName), usLevel(rhs.usLevel), usClass(rhs.usClass), iPoint(rhs.iPoint), usKill(rhs.usKill), usDead(rhs.usDead), usIconCount(rhs.usIconCount)
{
}

bool tagBSTeamMember::operator <(tagBSTeamMember const& rhs) const
{
	if( iPoint > rhs.iPoint )
	{
		return true;
	}
	else if( iPoint == rhs.iPoint )
	{
		if( usIconCount > rhs.usIconCount )
		{
			return true;
		}
	}
	return false;
}
bool tagBSTeamMember::operator ==(tagBSTeamMember const& rhs) const
{
	return (iPoint == rhs.iPoint) && (usIconCount == rhs.usIconCount);
}
bool tagBSTeamMember::operator ==(BM::GUID const& rhs) const
{
	return kCharGuid == rhs;
}
void tagBSTeamMember::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( kCharGuid );
	rkPacket.Push( kCharName );
	rkPacket.Push( usLevel );
	rkPacket.Push( usClass );
	rkPacket.Push( iPoint );
	rkPacket.Push( usKill );
	rkPacket.Push( usDead );
	rkPacket.Push( usIconCount );
}
void tagBSTeamMember::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( kCharGuid );
	rkPacket.Pop( kCharName );
	rkPacket.Pop( usLevel );
	rkPacket.Pop( usClass );
	rkPacket.Pop( iPoint );
	rkPacket.Pop( usKill );
	rkPacket.Pop( usDead );
	rkPacket.Pop( usIconCount );
}
size_t tagBSTeamMember::min_size() const
{
	return sizeof(BM::GUID) + sizeof(size_t) + (sizeof(unsigned short)*6);
}



//
PgBSTeam::PgBSTeam(EBattleSquareTeam const eTeam)
	: m_eTeam(eTeam), m_iTeamPoint(0), m_kContMember(), m_kContWaiter(), m_kIconCount(0)
{
}
PgBSTeam::PgBSTeam(PgBSTeam const& rhs)
: m_eTeam(rhs.m_eTeam), m_iTeamPoint(rhs.m_iTeamPoint), m_kContMember(rhs.m_kContMember), m_kContWaiter(rhs.m_kContWaiter), m_kIconCount(rhs.m_kIconCount)
{
}
PgBSTeam::~PgBSTeam()
{
}

void PgBSTeam::Clear()
{
	m_iTeamPoint = 0;
	m_kContMember.clear();
	m_kContWaiter.clear();
	IconCount(0);
}
int const PgBSTeam::GetMemberCount() const
{
	return static_cast< int >(m_kContMember.size());
}
int const PgBSTeam::GetWaiterCount() const
{
	return static_cast< int >(m_kContWaiter.size());
}
bool const PgBSTeam::AddMember(CUnit* pkUnit)
{
	if( !pkUnit )
	{
		return false;
	}
	return AddMember(SBSTeamMember(pkUnit));
}
bool const PgBSTeam::AddMember(SBSTeamMember const& rkMember)
{
	if( m_kContMember.end() == m_kContMember.find(rkMember.kCharGuid) )
	{
		m_kContMember.insert( std::make_pair(rkMember.kCharGuid, rkMember) );
	}
	return true;
}
void PgBSTeam::DelMember(BM::GUID const& rkCharGuid)
{
	m_kContMember.erase( rkCharGuid );
}
bool const PgBSTeam::AddWaiter(CUnit* pkUnit)
{
	if( !pkUnit )
	{
		return false;
	}
	if( m_kContWaiter.end() == std::find(m_kContWaiter.begin(), m_kContWaiter.end(), pkUnit->GetID()) )
	{
		m_kContWaiter.push_back( CONT_BS_TEAM_MEMBER::value_type(pkUnit) );
	}
	return true;
}
void PgBSTeam::DelWaiter(BM::GUID const& rkCharGuid)
{
	CONT_BS_TEAM_MEMBER::iterator find_iter =std::find(m_kContWaiter.begin(), m_kContWaiter.end(), rkCharGuid);
	if( m_kContWaiter.end() != find_iter )
	{
		m_kContWaiter.erase(find_iter);
	}
}
void PgBSTeam::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_eTeam );						// 팀
	rkPacket.Push( m_iTeamPoint );					// 팀점수
	rkPacket.Push( IconCount() );
	PU::TWriteTable_AM(rkPacket, m_kContMember);	// 팀 인원
	PU::TWriteArray_M(rkPacket, m_kContWaiter);		// 팀 대기 인원
}
void PgBSTeam::ReadFromPacket(BM::Stream& rkPacket)
{
	CONT_BS_TEAM_GUID_MEMBER kContMember;
	CONT_BS_TEAM_MEMBER kContWaiter;
	rkPacket.Pop( m_eTeam );						// 팀
	rkPacket.Pop( m_iTeamPoint );					// 팀점수
	rkPacket.Pop( m_kIconCount );
	PU::TLoadTable_AM(rkPacket, kContMember);		// 팀 인원
	PU::TLoadArray_M(rkPacket, kContWaiter);		// 팀 대기 인원
	m_kContMember.swap(kContMember);
	m_kContWaiter.swap(kContWaiter);
}
void PgBSTeam::WriteToScorePacket(ContGuidSet const& rkContGuid, BM::Stream& rkPacket, bool const bSyncAll) const
{
	size_t iCount = 0;
	rkPacket.Push( m_iTeamPoint );
	rkPacket.Push( IconCount() );
	size_t const iWrPos = rkPacket.WrPos();
	rkPacket.Push( iCount );
	CONT_BS_TEAM_GUID_MEMBER::const_iterator iter = m_kContMember.begin();
	while( m_kContMember.end() != iter )
	{
		if( bSyncAll
		||	(rkContGuid.end() != rkContGuid.find((*iter).first)) )
		{
			rkPacket.Push( (*iter).first );
			rkPacket.Push( (*iter).second.iPoint );
			rkPacket.Push( (*iter).second.usKill );
			rkPacket.Push( (*iter).second.usDead );
			rkPacket.Push( (*iter).second.usIconCount );
			++iCount;
		}
		++iter;
	}
	if( 0 != iCount )
	{
		rkPacket.ModifyData(iWrPos, &iCount, sizeof(size_t));
	}
}
void PgBSTeam::ReadFromScorePacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( m_iTeamPoint );
	rkPacket.Pop( m_kIconCount );
	size_t iCount = 0;
	rkPacket.Pop( iCount );
	while( 0 < iCount )
	{
		BM::GUID kGuid;
		unsigned short usKill = 0, usDead = 0, usIconCount = 0;
		int iPoint = 0;
		rkPacket.Pop( kGuid );
		rkPacket.Pop( iPoint );
		rkPacket.Pop( usKill );
		rkPacket.Pop( usDead );
		rkPacket.Pop( usIconCount );
		CONT_BS_TEAM_GUID_MEMBER::iterator find_iter = m_kContMember.find(kGuid);
		if( m_kContMember.end() != find_iter )
		{
			(*find_iter).second.iPoint = iPoint;
			(*find_iter).second.usKill = usKill;
			(*find_iter).second.usDead = usDead;
			(*find_iter).second.usIconCount = usIconCount;
		}
		--iCount;
	}
}
void PgBSTeam::AddScore(BM::GUID const& rkGuid, int const iPoint, int const iKill, int const iDead)
{
	CONT_BS_TEAM_GUID_MEMBER::iterator find_iter = m_kContMember.find(rkGuid);
	if( m_kContMember.end() == find_iter )
	{
		return;
	}
	CONT_BS_TEAM_GUID_MEMBER::mapped_type& rkTeamMember = (*find_iter).second;
	rkTeamMember.iPoint += iPoint;
	rkTeamMember.usKill += iKill;
	rkTeamMember.usDead += iDead;
}
bool PgBSTeam::IsTeamMember(BM::GUID const& rkGuid) const
{
	return m_kContMember.end() != m_kContMember.find(rkGuid);
}
bool PgBSTeam::IsTeamWaiter(BM::GUID const& rkGuid) const
{
	return m_kContWaiter.end() != std::find(m_kContWaiter.begin(), m_kContWaiter.end(), rkGuid);
}
bool PgBSTeam::GetMember(BM::GUID const& rkGuid, SBSTeamMember& rkOut) const
{
	CONT_BS_TEAM_GUID_MEMBER::const_iterator find_iter = m_kContMember.find( rkGuid );
	if( m_kContMember.end() != find_iter )
	{
		rkOut = (*find_iter).second;
		return true;
	}
	return false;
}

void PgBSTeam::IncreaseIcon(BM::GUID const& rkGuid)
{
	CONT_BS_TEAM_GUID_MEMBER::iterator find_iter = m_kContMember.find(rkGuid);
	if( m_kContMember.end() == find_iter )
	{
		return;
	}
	CONT_BS_TEAM_GUID_MEMBER::mapped_type& rkTeamMember = (*find_iter).second;
	++rkTeamMember.usIconCount;

	UpdateIconCount();
}

void PgBSTeam::DropAllIcon(BM::GUID const& rkGuid)
{
	CONT_BS_TEAM_GUID_MEMBER::iterator find_iter = m_kContMember.find(rkGuid);
	if( m_kContMember.end() == find_iter )
	{
		return;
	}
	CONT_BS_TEAM_GUID_MEMBER::mapped_type& rkTeamMember = (*find_iter).second;
	rkTeamMember.usIconCount = 0;

	UpdateIconCount();
}

int PgBSTeam::UpdateIconCount()
{
	CONT_BS_TEAM_GUID_MEMBER::const_iterator itor_mem = m_kContMember.begin();
	int iCount = 0;
	while (m_kContMember.end() != itor_mem)
	{
		iCount += (*itor_mem).second.usIconCount;
		++itor_mem;
	}
	IconCount(iCount);
	return iCount;
}