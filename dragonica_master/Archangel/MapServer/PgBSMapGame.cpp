#include "stdafx.h"
#include "Variant/Global.h"
#include "Variant/item.h"
#include "Variant/PgStringUtil.h"
#include "Variant/PgBattleSquare.h"
#include "Global.h"
#include "PgVolatileInven.h"
#include "PgBSTeamMng.h"
#include "PgBSTimeEvent.h"
#include "PgBSMapGame.h"

//
PgBSMapGameImpl::PgBSMapGameImpl()
	: PgBSGame(), m_dwPlayerTick(0), m_kStartTime(0)
{
}
PgBSMapGameImpl::PgBSMapGameImpl(BM::Stream& rkPacket)
	: PgBSGame(rkPacket), m_dwPlayerTick(0)
{
	rkPacket.Pop( m_kStartTime );
	rkPacket.Pop( m_kPreOpenTime );
}
PgBSMapGameImpl::~PgBSMapGameImpl()
{
}
__int64 PgBSMapGameImpl::CalcPvpExp(int const iKillerLv, int const iDeadManLv)
{
	struct SLvExp
	{
		SLvExp(int const iMin, int const iMax, float const fScale)
			: iLvMin(iMin), iLvMax(iMax), fExpScale(fScale)
		{
		}
		bool IsInLevel(int const iLv) const
		{
			return iLvMin <= iLv && iLv <= iLvMax;
		}
		int iLvMin;
		int iLvMax;
		float fExpScale;
	};

	static SLvExp const abySetting[] = {	SLvExp(1, 9, 10.f),
											SLvExp(10, 19, 9.f),
											SLvExp(20, 29, 4.5f),
											SLvExp(30, 39, 3.f),
											SLvExp(40, 49, 2.3f),
											SLvExp(50, 59, 1.8f),
											SLvExp(60, 69, 1.5f), };
	float const fDefaultScale = 1.2f;

	int const iLVDiffLimit = 5;
	if( iLVDiffLimit <= iKillerLv - iDeadManLv )
	{
		return 0;
	}

	float fExpScale = fDefaultScale;
	SLvExp const* pkBegin = abySetting;
	while( PgArrayUtil::IsInArray(pkBegin, abySetting) )
	{
		if( pkBegin
		&&	pkBegin->IsInLevel(iKillerLv)  )
		{
			fExpScale = pkBegin->fExpScale;
			break;
		}
		++pkBegin;
	}
	
	__int64 const iRetExp = static_cast< __int64 >(iKillerLv * iDeadManLv * fExpScale);
	return iRetExp;
}
__int64 PgBSMapGameImpl::GetNowTime() const
{
	DWORD const dwMSecPerSec = 1000;
	SYSTEMTIME kNowDateTime;
	::GetLocalTime(&kNowDateTime);
	__int64 iNowTime = 0;
	CGameTime::SystemTime2SecTime(kNowDateTime, iNowTime);
	return iNowTime;
}
void PgBSMapGameImpl::Set(PgBSMapGameImpl const& rhs)
{
	PgBSGame::Set(rhs);
	m_kStartTime = rhs.m_kStartTime;
	m_kPreOpenTime = rhs.m_kPreOpenTime;
	// m_dwPlayerTick

	//
	DWORD const dwMSecPerSec = 1000;
	__int64 const iSec = GetNowTime() - m_kStartTime;
}
void PgBSMapGameImpl::Clear()
{
	m_kStartTime = 0;
	m_dwPlayerTick = 0;
	PgBSGame::Clear();
}
void PgBSMapGameImpl::FindReward(int const iValue, std::list< SBSRewardItem > const& rkRewardTable, CONT_BS_REWARD& rkOut)
{
	typedef std::list< SBSRewardItem > CONT_REWARD_TABLE;
	if( !rkRewardTable.empty() )
	{
		CONT_REWARD_TABLE::const_iterator find_iter = rkRewardTable.end();
		CONT_REWARD_TABLE::const_iterator iter = rkRewardTable.begin();
		while( rkRewardTable.end() != iter )
		{
			CONT_REWARD_TABLE::value_type const& rkRewardItem = (*iter);
			if( rkRewardItem.iMinPoint <= iValue )
			{
				find_iter = iter;
			}
			else
			{
				break;
			}
			++iter;
		}
		if( rkRewardTable.end() != find_iter )
		{
			CONT_REWARD_TABLE::value_type const& rkRewardItem = (*find_iter);
			if( 0 != rkRewardItem.iItemNo1
			&&	0 != rkRewardItem.iCount1 )
			{
				rkOut.push_back( CONT_BS_REWARD::value_type(rkRewardItem.iItemNo1, rkRewardItem.iCount1) );
			}
			if( 0 != rkRewardItem.iItemNo2
			&&	0 != rkRewardItem.iCount2)
			{
				rkOut.push_back( CONT_BS_REWARD::value_type(rkRewardItem.iItemNo2, rkRewardItem.iCount2) );
			}
		}
	}
}
bool PgBSMapGameImpl::CalcReward(bool const bWin, int const iPlayerLevel, int const iPoint, int const iTeamPoint, CONT_BS_REWARD& rkOut) const
{
	FindReward(iPoint, m_kContPrivateRewardItem, rkOut);
	FindReward(iPlayerLevel, m_kContPrivateLevelRewardItem, rkOut);
	if( bWin )
	{
		FindReward(iTeamPoint, m_kContWinTeamRewardItem, rkOut);
	}
	else
	{
		FindReward(iTeamPoint, m_kContLoseTeamRewardItem, rkOut);
	}
	return !rkOut.empty();
}
void PgBSMapGameImpl::MakeBonusReward(int const iWinTeamPlayerCont, CONT_BS_REWARD& rkContWinTeamBonusReward)
{
	FindReward(iWinTeamPlayerCont, m_kContWinBonusRewardItem, rkContWinTeamBonusReward);
}
bool PgBSMapGameImpl::IsBSItem(int const iItemNo) const
{
	return m_kContBSItem.end() != m_kContBSItem.find( iItemNo );
}
void PgBSMapGameImpl::WriteToMapUserPacket(BM::Stream& rkPacket) const
{
	SYSTEMTIME kNow;
	::GetLocalTime(&kNow);
	__int64 iNowTimeSec = 0;
	CGameTime::SystemTime2SecTime(kNow, iNowTimeSec);
	__int64 const iRemainTimeSec = m_kGameInfo.iGameSec - (iNowTimeSec - m_kStartTime);
	__int64 const iRemainStartTimeSec = m_kStartTime - iNowTimeSec;

	rkPacket.Push( iRemainStartTimeSec );
	rkPacket.Push( iRemainTimeSec );
	WriteToPacket( rkPacket );
}

//
// 옵저버
// 아무도 없을 땐 가만히
// 누군가 팀 참가 인원이 있으면 해당 인원 보도록
// 아무도 업는 옵저버들은 참가 팀 인원이 생기면 그 사람을 보도록
// 게임에 참가하면 옵저버 모드 종료
PgBSObserverHelper::PgBSObserverHelper()
	: m_kContWaiter(), m_kContTarget()
{
}
PgBSObserverHelper::~PgBSObserverHelper()
{
}
void PgBSObserverHelper::Clear()
{
	m_kContWaiter.clear();
	m_kContTarget.clear();
}
void PgBSObserverHelper::AddTarget(BM::GUID const& rkGuid)
{
	if( m_kContTarget.end() == std::find(m_kContTarget.begin(), m_kContTarget.end(), rkGuid) )
	{
		m_kContTarget.push_back( rkGuid );
	}
}
bool PgBSObserverHelper::GetTarget(BM::GUID& rkOut) const
{
	int const iTargetCount = static_cast< int >( m_kContTarget.size() );
	if (iTargetCount <= 0)
	{
		return false;
	}
	int const iSelectCount = BM::Rand_Index(iTargetCount);
	if( 0 <= iSelectCount
	&&	iTargetCount > iSelectCount )
	{
		rkOut = m_kContTarget.at(iSelectCount);
		return true;
	}
	return false;
}
void PgBSObserverHelper::DelTarget(BM::GUID const& rkGuid)
{
	CONT_BS_OBSERVER_TARGET::iterator find_iter = std::find(m_kContTarget.begin(), m_kContTarget.end(), rkGuid);
	if( m_kContTarget.end() != find_iter )
	{
		m_kContTarget.erase(find_iter);
	}
}
void PgBSObserverHelper::AddWaiter(BM::GUID const& rkGuid)
{
	if( m_kContWaiter.end() == m_kContWaiter.find(rkGuid) )
	{
		m_kContWaiter.insert( rkGuid );
	}
}
void PgBSObserverHelper::DelWaiter(BM::GUID const& rkGuid)
{
	m_kContWaiter.erase( rkGuid );
}
void PgBSObserverHelper::DelWaiter(ContGuidSet const& rkContGuid)
{
	ContGuidSet::const_iterator iter = rkContGuid.begin();
	while( rkContGuid.end() != iter )
	{
		DelWaiter((*iter));
		++iter;
	}
}

//
PgBSMapGame::PgBSMapGame()
	: m_kBSGame(), m_kVIMng(), m_kTeamMng(), m_kTimeEvent()
{
}
PgBSMapGame::PgBSMapGame(PgBSMapGame const& rhs)
{
}
PgBSMapGame::~PgBSMapGame()
{
}
void PgBSMapGame::operator =(PgBSMapGame const& rhs)
{
}
__int64 PgBSMapGame::CalcPvpExp(int const iKillerLv, int const iDeadManLv)
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSGame.CalcPvpExp(iKillerLv, iDeadManLv);
}
void PgBSMapGame::Set(PgBSMapGameImpl const& rhs)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kBSGame.Set(rhs);
}
void PgBSMapGame::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kBSGame.Clear();
	m_kVIMng.ClearInven();
	m_kTeamMng.Clear();
	m_kTimeEvent.Clear();
}
void PgBSMapGame::WriteToMapUserPacket(BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kBSGame.WriteToMapUserPacket(rkPacket);
	m_kTeamMng.WriteToPacket(rkPacket);
}
bool PgBSMapGame::CalcReward(BM::GUID const& rkCharGuid, CONT_BS_REWARD& rkOut) const
{
	BM::CAutoMutex kLock(m_kMutex);
	int const iMyTeam = m_kTeamMng.GetTeam(rkCharGuid);
	int const iWinTeamNo = m_kTeamMng.GetWinTeam();
	bool const bMyWin = (BST_NONE != iWinTeamNo)? (iMyTeam == iWinTeamNo): false;
	int const iTeamPoint = m_kTeamMng.GetTeamPoint(rkCharGuid);
	SBSTeamMember kBSTeamMember;
	m_kTeamMng.GetMember(rkCharGuid, kBSTeamMember);
	return m_kBSGame.CalcReward(bMyWin, kBSTeamMember.usLevel, kBSTeamMember.iPoint, iTeamPoint, rkOut);
}
void PgBSMapGame::MakeWinTeamBonusReward(CONT_BS_USER_REWARD& rkContWinTeamBonusReward)
{
	BM::CAutoMutex kLock(m_kMutex);
	int const iWinTeam = m_kTeamMng.GetWinTeam();
	int const iWinTeamMemberCount = m_kTeamMng.GetTeamMemberCount(iWinTeam);

	CONT_BS_REWARD kContBonusReward;
	m_kBSGame.MakeBonusReward(iWinTeamMemberCount, kContBonusReward);
	if( kContBonusReward.empty() )
	{
		return;
	}

	VEC_GUID kContGuid;
	m_kTeamMng.GetTeamMemberGuid(iWinTeam, kContGuid);
	if( kContGuid.empty() )
	{
		return;
	}

	//int const iMaxCount = static_cast< int >(kContGuid.size());
	while( !kContBonusReward.empty() )
	{
		CONT_BS_REWARD::value_type& rkBonusReward = kContBonusReward.front();
		std::random_shuffle(kContGuid.begin(), kContGuid.end());
		VEC_GUID::iterator iter = kContGuid.begin();
		while( kContGuid.end() != iter )
		{
			BM::GUID const& rkGuid = (*iter);
			CONT_BS_USER_REWARD::iterator find_iter = rkContWinTeamBonusReward.find( rkGuid );
			if( rkContWinTeamBonusReward.end() == find_iter )
			{
				auto kRet = rkContWinTeamBonusReward.insert( std::make_pair(rkGuid, CONT_BS_USER_REWARD::mapped_type()) );
				if( kRet.second )
				{
					find_iter = kRet.first;
				}
			}
			if( rkContWinTeamBonusReward.end() != find_iter )
			{
				int const iBaseCount = 1;
				(*find_iter).second.push_back( CONT_BS_REWARD::value_type(rkBonusReward.iItemNo, iBaseCount) );
				rkBonusReward.iCount -= iBaseCount;
			}
			if( 0 >= rkBonusReward.iCount )
			{
				break;
			}
			++iter;
		}
		kContBonusReward.pop_front();
	}
}
bool PgBSMapGame::IsBSItem(int const iItemNo) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSGame.IsBSItem(iItemNo);
}
int PgBSMapGame::GameIDX() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSGame.GameInfo().iGameIDX;
}
//
size_t PgBSMapGame::PushItem(BM::GUID const& rkOwnerGuid, PgBase_Item const& rkNewItem, CONT_BS_BUFF& kOutAddBuff, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	DWORD const dwItemHaveTickSec = 10000; // 10초
	size_t const iRet = m_kVIMng.PushItem(m_kBSGame.ContBSItem(), rkOwnerGuid, rkNewItem, kOutAddBuff);
	if( 0 != iRet )
	{
		m_kTimeEvent.Add(rkOwnerGuid, BSTET_TEAM_POINT, dwItemHaveTickSec);

		m_kTeamMng.IncreaseIcon(rkOwnerGuid);
		//UpdateTeamIconCount();
		ContGuidSet kSyncList;
		kSyncList.insert(rkOwnerGuid);
		m_kTeamMng.WriteToScorePacket(kSyncList, rkPacket);
	}
	return iRet;
}
bool PgBSMapGame::SwapInven(BM::GUID const& rkOwnerGuid, CONT_VOLATILE_INVEN_ITEM& rkOut, CONT_BS_BUFF& rkDelBuff, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bRet = m_kVIMng.SwapInven(rkOwnerGuid, rkOut, rkDelBuff);
	if( bRet )
	{
		m_kTimeEvent.DelType(rkOwnerGuid, BSTET_TEAM_POINT);
		m_kTeamMng.DropAllIcon(rkOwnerGuid);

		ContGuidSet kSyncList;
		kSyncList.insert(rkOwnerGuid);
		m_kTeamMng.WriteToScorePacket(kSyncList, rkPacket);
	}
	return bRet;
}
//
EBS_JOIN_RETURN PgBSMapGame::JoinTeam(CUnit* pkUnit, EBattleSquareTeam const eTeam)
{
	BM::CAutoMutex kLock(m_kMutex);
	EBS_JOIN_RETURN const eRet = m_kTeamMng.Join(pkUnit, m_kBSGame.GameInfo().iMaxUser, eTeam);
	if( BSJR_SUCCESS == eRet )
	{
		m_kVIMng.AddInven(pkUnit->GetID());
		m_kBSObserverMode.DelWaiter(pkUnit->GetID()); // 대기자에서 빼고
		m_kBSObserverMode.AddTarget(pkUnit->GetID()); // 대상자에 넣는다
	}
	if( BSJR_BALANCE == eRet )
	{
		m_kBSObserverMode.AddWaiter(pkUnit->GetID()); // 대기자에 넣기
	}
	return eRet;
}
int PgBSMapGame::GetTeam(BM::GUID const& rkGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kTeamMng.GetTeam(rkGuid);
}
int PgBSMapGame::GetWinTeam() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kTeamMng.GetWinTeam();
}
void PgBSMapGame::Leave(CUnit* pkUnit)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !pkUnit )
	{
		return;
	}
	m_kTeamMng.Leave(pkUnit);
	m_kVIMng.DelInven(pkUnit->GetID());
	m_kTimeEvent.DelAll(pkUnit->GetID());
	m_kBSObserverMode.DelWaiter(pkUnit->GetID());
	m_kBSObserverMode.DelTarget(pkUnit->GetID());
}
void PgBSMapGame::NfyPlayerKill(CUnit* pkKiller, CUnit* pkDeadMan, BM::Stream& rkScorePacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !pkKiller
	||	!pkDeadMan )
	{
		return;
	}
	int const iKillerTeamPoint = m_kTeamMng.GetTeamPoint(pkKiller->GetID());
	int const iDeadManTeamPoint = m_kTeamMng.GetTeamPoint(pkDeadMan->GetID());
	int const iDiffTeamPoint = iKillerTeamPoint - iDeadManTeamPoint;
	int iPoint = 0;
	if( 0 < iDiffTeamPoint )
	{
		iPoint = 300; // 이기고 있음
	}
	else if( 0 > iDiffTeamPoint )
	{
		iPoint = 350; // 지고 있음
	}
	else
	{
		iPoint = 300; // 동점
	}

	int const iNone = 0;
	int const iOneKill = 1;
	int const iOneDead = 1;
	m_kTeamMng.AddScore(pkKiller->GetID(), iPoint, iOneKill, iNone);
	m_kTeamMng.AddTeamPoint(pkKiller->GetID(), iPoint);
	m_kTeamMng.AddScore(pkDeadMan->GetID(), iNone, iNone, iOneDead);
	ContGuidSet kContGuid;
	kContGuid.insert( pkKiller->GetID() );
	kContGuid.insert( pkDeadMan->GetID() );
	m_kTeamMng.WriteToScorePacket(kContGuid, rkScorePacket);
}
void PgBSMapGame::AddDeadMan(BM::GUID const& rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	DWORD const dwReviveSec = 10000;
	m_kTimeEvent.Add(rkCharGuid, BSTET_REVIVE, dwReviveSec);
}
void PgBSMapGame::SyncPlayerList(BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kTeamMng.WriteToPacket(rkPacket);
}
bool PgBSMapGame::ProcessWaiter(ContGuidSet& rkOut)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kTeamMng.ProcessWaiter(m_kBSGame.GameInfo().iMaxUser, rkOut);
	ContGuidSet::const_iterator iter = rkOut.begin();
	while( rkOut.end() != iter )
	{
		m_kVIMng.AddInven( (*iter) );
		m_kBSObserverMode.DelWaiter( (*iter) );
		m_kBSObserverMode.AddTarget( (*iter) );
		++iter;
	}
	return !rkOut.empty();
}
bool PgBSMapGame::Tick1s(DWORD const dwElapsedTime, CONT_BS_HAVE_MINIMAP_ICON& rkOut, ContGuidSet& rkContRevive/*, CONT_BS_MEMBER_POINT& rkMemberPoint*/)
{
	BM::CAutoMutex kLock(m_kMutex);
	ContGuidSet kPointChanged;
	m_kTimeEvent.Tick(dwElapsedTime, PgBSTimeEventMgr::SBSTimeEventTickFunc(rkContRevive, kPointChanged));

	m_kVIMng.GetMinimapIconUser(rkOut);
	{
		VEC_GUID kVecGuid;
		m_kTeamMng.GetTeamMemberGuid(BST_RED, kVecGuid);
		VEC_GUID::const_iterator itor_guid = kVecGuid.begin();
		while (itor_guid != kVecGuid.end())
		{
			AddTeamInfo(*itor_guid, BST_RED, rkOut);
			++itor_guid;
		}
	}
	{
		VEC_GUID kVecGuid;
		m_kTeamMng.GetTeamMemberGuid(BST_BLUE, kVecGuid);
		VEC_GUID::const_iterator itor_guid = kVecGuid.begin();
		while (itor_guid != kVecGuid.end())
		{
			AddTeamInfo(*itor_guid, BST_BLUE, rkOut);
			++itor_guid;
		}
	}
	return true;
}
void PgBSMapGame::SetTeam(CUnit* pkUnit, bool const bIsGame) const
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kTeamMng.SetTeam(pkUnit, bIsGame);
}
bool PgBSMapGame::GetObserverTarget(BM::GUID& rkOut) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSObserverMode.GetTarget(rkOut);
}
void PgBSMapGame::AddObserverWaiter(BM::GUID const& rkOut)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kBSObserverMode.AddWaiter(rkOut);
}
bool PgBSMapGame::ProcessObserverWaiter(PgBSObserverHelper::CONT_BS_OBSERVER_RESULT& rkContOut)
{
	BM::CAutoMutex kLock(m_kMutex);
	typedef PgBSObserverHelper::CONT_BS_OBSERVER_WAITER CONT_BS_OBSERVER_WAITER;
	typedef PgBSObserverHelper::CONT_BS_OBSERVER_RESULT CONT_BS_OBSERVER_RESULT;
	ContGuidSet kContDelWaiter;
	CONT_BS_OBSERVER_WAITER::const_iterator iter = m_kBSObserverMode.ContWaiter().begin();
	while( m_kBSObserverMode.ContWaiter().end() != iter )
	{
		CONT_BS_OBSERVER_WAITER::value_type const& rkGuid = (*iter);
		BM::GUID kTargetGuid;
		if( m_kBSObserverMode.GetTarget(kTargetGuid) )
		{
			CONT_BS_OBSERVER_RESULT::iterator find_iter = rkContOut.find(rkGuid);
			if( rkContOut.end() != find_iter )
			{
				(*find_iter).second = kTargetGuid;
			}
			else
			{
				auto kRet = rkContOut.insert( std::make_pair(rkGuid, kTargetGuid) );
			}
			kContDelWaiter.insert( rkGuid );
		}
		++iter;
	}
	m_kBSObserverMode.DelWaiter( kContDelWaiter );
	return !rkContOut.empty();
}
void PgBSMapGame::AddScore(BM::GUID const& rkCharGuid, int const iPoint, int const iTeamPoint, BM::Stream& rkScorePacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( 0 != iPoint )
	{
		m_kTeamMng.AddScore(rkCharGuid, iPoint, 0, 0); // 개인 포인트 증가
	}
	if( 0 != iTeamPoint )
	{
		m_kTeamMng.AddTeamPoint(rkCharGuid, iTeamPoint); // 팀 포인트 증가
	}
	ContGuidSet kSyncList;
	kSyncList.insert( rkCharGuid );
	m_kTeamMng.WriteToScorePacket(kSyncList, rkScorePacket);
}

void PgBSMapGame::AddTeamInfo(BM::GUID const& kGuid, EBattleSquareTeam const eTeam, CONT_BS_HAVE_MINIMAP_ICON& rkOut)
{
	CONT_BS_HAVE_MINIMAP_ICON::iterator itor_minimap = rkOut.find(kGuid);
	if (itor_minimap == rkOut.end())
	{
		auto ibRet = rkOut.insert(std::make_pair(kGuid, BSIconOwnInfo(kGuid, eTeam, 0, 0)));
	}
	else
	{
		(*itor_minimap).second.iTeam = static_cast<int>(eTeam);
	}
}

void PgBSMapGame::GetTeamPoint(int const iTeam, int& rPoint, int& rIconCount) const
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kTeamMng.GetTeamPoint(iTeam, rPoint, rIconCount);
}

void PgBSMapGame::UpdateBSPoint(ContGuidSet& rkOutGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	PgBSTimeEventMgr::CONT_EVENTCOUNT kEventCount;
	m_kTimeEvent.CountEvent(BSTET_TEAM_POINT, kEventCount);

	{
		VEC_GUID kBlueTeam;
		m_kTeamMng.GetTeamMemberGuid(BST_BLUE, kBlueTeam);
		VEC_GUID::const_iterator itor_guid = kBlueTeam.begin();
		while (itor_guid != kBlueTeam.end())
		{
			int const iIconCount = m_kVIMng.GetIconCount(*itor_guid);
			PgBSTimeEventMgr::CONT_EVENTCOUNT::const_iterator itor_event = kEventCount.find(*itor_guid);
			int const iWaitingIcon = (itor_event == kEventCount.end()) ? 0 : (*itor_event).second;

			int const iPoint = std::max(iIconCount-iWaitingIcon, 0) * POINT_PER_ICON;
			m_kTeamMng.AddScore(*itor_guid, iPoint, 0, 0);
			m_kTeamMng.AddTeamPoint(*itor_guid, iPoint);
			if (iPoint >0)
			{
				rkOutGuid.insert(*itor_guid);
			}
			++itor_guid;
		}
	}
	{
		VEC_GUID kRedTeam;
		m_kTeamMng.GetTeamMemberGuid(BST_RED, kRedTeam);
		VEC_GUID::const_iterator itor_guid = kRedTeam.begin();
		while (itor_guid != kRedTeam.end())
		{
			int const iIconCount = m_kVIMng.GetIconCount(*itor_guid);
			PgBSTimeEventMgr::CONT_EVENTCOUNT::const_iterator itor_event = kEventCount.find(*itor_guid);
			int const iWaitingIcon = (itor_event == kEventCount.end()) ? 0 : (*itor_event).second;

			int const iPoint = std::max(iIconCount-iWaitingIcon, 0) * POINT_PER_ICON;
			m_kTeamMng.AddScore(*itor_guid, iPoint, 0, 0);
			m_kTeamMng.AddTeamPoint(*itor_guid, iPoint);
			if (iPoint >0)
			{
				rkOutGuid.insert(*itor_guid);
			}
			++itor_guid;
		}
	}
}

void PgBSMapGame::UpdateTeamIconCount()
{
	BM::CAutoMutex kLock(m_kMutex);
	{
		VEC_GUID kBlueTeam;
		m_kTeamMng.GetTeamMemberGuid(BST_BLUE, kBlueTeam);
		int const iCount = m_kVIMng.GetMinimapIconCount(kBlueTeam);
		m_kTeamMng.SetTeamIconCount(BST_BLUE, iCount);
	}
	{
		VEC_GUID kRedTeam;
		m_kTeamMng.GetTeamMemberGuid(BST_RED, kRedTeam);
		int const iCount = m_kVIMng.GetMinimapIconCount(kRedTeam);
		m_kTeamMng.SetTeamIconCount(BST_RED, iCount);
	}

}

void PgBSMapGame::WriteScore(BM::GUID const& kCharGuid, BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	ContGuidSet kContGuidSet;
	kContGuidSet.insert(kCharGuid);
	m_kTeamMng.WriteToScorePacket(kContGuidSet, rkPacket);
}

void PgBSMapGame::WriteScore(ContGuidSet const& rkContGuid, BM::Stream& rkPacket, bool const bSyncAll) const
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kTeamMng.WriteToScorePacket(rkContGuid, rkPacket, bSyncAll);
}


int PgBSMapGame::GetMapItemBagNo() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSGame.GameInfo().iMapBagItemGroundNo;
}
int PgBSMapGame::GetGenGroupGroundNo() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSGame.GameInfo().iGenGroupGroundNo;
}
int PgBSMapGame::GetBagControlNo() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kBSGame.GameInfo().iMonsterBagControlNo;
}
