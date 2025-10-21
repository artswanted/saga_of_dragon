#include "stdafx.h"
#include "PgHackDualKeyboard.h"

namespace HackDualKeyboard
{
	bool IsCanReset(DWORD const dwLimitTime, SMatchResult const& rkMatchResult)
	{
		return dwLimitTime <= rkMatchResult.dwEleapsedTime;
	}
	bool IsCanDetection(size_t const iMatchPercent, SMatchResult const& rkMatchResult)
	{
		if( 0 < rkMatchResult.iLhsTotalCount
		&&	0 < rkMatchResult.iRhsTotalCount )
		{
			size_t const iMatchBaseValue = 10000;
			size_t const iLhsMatchResult = rkMatchResult.iMatchCount * iMatchBaseValue / rkMatchResult.iLhsTotalCount;
			size_t const iRhsMatchResult = rkMatchResult.iMatchCount * iMatchBaseValue / rkMatchResult.iRhsTotalCount;
			return iMatchPercent <= iLhsMatchResult || iMatchPercent <= iRhsMatchResult;
		}
		return false;
	}

	//
	tagMatchResult::tagMatchResult()
		: kLhsKey(), iLhsTotalCount(0), kRhsKey(), iRhsTotalCount(0), iMatchCount(0), dwEleapsedTime(0), dwTotalEleapsedTime(0)//, iMatchPoint(0)
	{
	}
	tagMatchResult::tagMatchResult(tagMatchResult const& rhs)
		: kLhsKey(rhs.kLhsKey), iLhsTotalCount(rhs.iLhsTotalCount)
		, kRhsKey(rhs.kRhsKey), iRhsTotalCount(rhs.iRhsTotalCount)
		, iMatchCount(rhs.iMatchCount), dwEleapsedTime(rhs.dwEleapsedTime), dwTotalEleapsedTime(rhs.dwTotalEleapsedTime)//, iMatchPoint(rhs.iMatchPoint)
	{
	}
	//void tagMatchResult::AddPoint(size_t const iPoint)
	//{
	//	iMatchPoint += iPoint;
	//}
	void tagMatchResult::AddCount()
	{
		++iMatchCount;
	}
	void tagMatchResult::Reset()
	{
		iLhsTotalCount = 0;
		iRhsTotalCount = 0;
		iMatchCount = 0;
		dwEleapsedTime = 0;
		//dwTotalEleapsedTime = 0;
	}

	//
	tagActionItem::tagActionItem()
		: eType(HACT_NONE), dwTime(0)
	{
	}
	tagActionItem::tagActionItem(EHackActionCheckType const Type, DWORD const Time)
		: eType(Type), dwTime(Time)
	{
	}

	void tagActionItem::operator =(tagActionItem const& rhs)
	{
		eType = rhs.eType;
		dwTime = rhs.dwTime;
	}


	//
	PgActionHistory::PgActionHistory()
		: m_iPoint(0), m_kContItem()
	{
	}
	PgActionHistory::PgActionHistory(PgActionHistory const& rhs)
		: m_iPoint(0), m_kContItem(rhs.m_kContItem)
	{
	}
	PgActionHistory::~PgActionHistory()
	{
	}

	void PgActionHistory::Add(EHackActionCheckType const eType, DWORD const dwTime)
	{
		// 마지막 시간을 기준으로 dwTime이 들어 온다
		m_kContItem.push_back( CONT_ACTION_ITEM::value_type(eType, dwTime) );
	}
	void PgActionHistory::Check(PgActionHistory const& rhs, SMatchResult& rkOutMatchResult) const
	{
		CONT_ACTION_ITEM::const_iterator iter = m_kContItem.begin();
		CONT_ACTION_ITEM::const_iterator right_iter = rhs.m_kContItem.begin();
		while( m_kContItem.end() != iter
			&& rhs.m_kContItem.end() != right_iter)
		{
			CONT_ACTION_ITEM::value_type const& rkBase = (*iter);
			while( rhs.m_kContItem.end() != right_iter )
			{
				CONT_ACTION_ITEM::value_type const& rkRhs = (*right_iter);
				DWORD dwDiff = 0;
				if( CheckTime(dwDiff, rkBase, rkRhs, rkOutMatchResult) )
				{
					++right_iter; // iter 증가 시키고
					break; // 1회 찾기 중단
				}
				else
				{
					// 기준시간 이외면? 비슷한 시간대 나올 때 까지 반복
					if( 1000 < dwDiff )
					{
						// 더 늦은 시간을 찾자
						if( rkBase.dwTime > rkRhs.dwTime ) // 같을 순 없다
						{
							++right_iter; // 우측이 적으니 우측의 시간 진행
							continue;
						}
					}
					break;
				}
				++right_iter;
			}
			++iter;
		}
	}
	void PgActionHistory::Flush()
	{
		m_kContItem.clear();
	}
	size_t PgActionHistory::GetTotalCount() const
	{
		return m_kContItem.size();
	}
	bool PgActionHistory::CheckTime(DWORD& dwDiff, CONT_ACTION_ITEM::value_type const& lhs, CONT_ACTION_ITEM::value_type const& rhs, SMatchResult& rkOutMatchResult) const
	{
		int const dwGapLevel1 = 150;	// 가장 큰 값
		int const dwGapLevel2 = 100;	// 중간
		int const dwGapLevel3 = 50;		// 
		int const dwGapLevel4 = 10;		// 가장 작은 값
		dwDiff = std::max(lhs.dwTime, rhs.dwTime) - std::min(lhs.dwTime, rhs.dwTime);
		if( dwGapLevel1 >= dwDiff ) // +- 150 Mili Second 초 안에서
		{
			if( lhs.eType == rhs.eType ) // 행동까지 같다고!?
			{
				//if( dwGapLevel4 >= dwDiff ) // 논리적 순서 주의!!! 가장 작은것 부터
				//{
				//	rkOutMatchResult.AddPoint( 5 ); // 경고 5점
				//}
				//else if( dwGapLevel3 >= dwDiff )
				//{
				//	rkOutMatchResult.AddPoint( 3 ); // 경고 3점
				//}
				//else if( dwGapLevel2 >= dwDiff ) // 중간것
				//{
				//	rkOutMatchResult.AddPoint( 2 ); // 경고 2점
				//}
				//else // 가장 큰건 이미 체크 했으니
				//{
				//	rkOutMatchResult.AddPoint( 1 ); // 경고 1점
				//}
				rkOutMatchResult.AddCount();
			}
			return true;
		}
		return false;
	}

	//
	PgActionCheckMgr_Impl::PgActionCheckMgr_Impl()
		: m_kContHistory(), m_dwTimeLine(), m_kMutex(), m_kContResult()
	{
	}
	PgActionCheckMgr_Impl::PgActionCheckMgr_Impl(PgActionCheckMgr_Impl const& rhs)
		: m_kContHistory(rhs.m_kContHistory), m_dwTimeLine(rhs.m_dwTimeLine), m_kMutex(), m_kContResult(rhs.m_kContResult)
	{
	}
	PgActionCheckMgr_Impl::~PgActionCheckMgr_Impl()
	{
	}

	void PgActionCheckMgr_Impl::Add(BM::GUID const& rkKeyGuid)
	{
		BM::CAutoMutex kLock(m_kMutex);
		if( m_kContHistory.end() == m_kContHistory.find(rkKeyGuid) )
		{
			m_kContHistory.insert( std::make_pair(rkKeyGuid, CONT_ACTION_HISTORY::mapped_type()) );
		}
	}
	void PgActionCheckMgr_Impl::Del(BM::GUID const& rkKeyGuid, CONT_MATCH_RESULT_LIST& rkContOut)
	{
		BM::CAutoMutex kLock(m_kMutex);
		m_kContHistory.erase( rkKeyGuid );
		{
			CONT_MATCH_RESULT::iterator iter = m_kContResult.begin();
			while( m_kContResult.end() != iter )
			{
				if( (*iter).first.first == rkKeyGuid
					||	(*iter).first.second == rkKeyGuid )
				{
					rkContOut.push_back( (*iter).second );
					iter = m_kContResult.erase( iter );
				}
				else
				{
					++iter;
				}
			}
		}
	}
	void PgActionCheckMgr_Impl::Action(BM::GUID const& rkKeyGuid, EHackActionCheckType const eType, DWORD const dwTime)
	{
		BM::CAutoMutex kLock(m_kMutex);
		if( m_dwTimeLine <= dwTime ) // Tick 지난건 통과
		{
			switch( eType )
			{
			case HACT_MOVE:
			case HACT_MOVE_BOTTOM:
			case HACT_MOVE_LEFT:
			case HACT_MOVE_RIGHT:
			case HACT_MOVE_TOP:
			default:
				{
					CONT_ACTION_HISTORY::iterator find_iter = m_kContHistory.find(rkKeyGuid);
					if( m_kContHistory.end() != find_iter )
					{
						CONT_ACTION_HISTORY::mapped_type& rkActionHistory = (*find_iter).second;
						rkActionHistory.Add( eType, dwTime - m_dwTimeLine); // 마지막 틱시간을 기준으로 흐른 시간을 기록
					}
				}break;
			}
		}
	}
	void PgActionCheckMgr_Impl::Tick(DWORD const dwTimeLine)
	{
		BM::CAutoMutex kLock(m_kMutex);
		DWORD const dwEleapsedTime = dwTimeLine - m_dwTimeLine;
		m_dwTimeLine = dwTimeLine;
		if( m_kContHistory.empty() )
		{
			return;
		}
		CONT_ACTION_HISTORY::iterator base_iter = m_kContHistory.begin();
		while( m_kContHistory.end() != base_iter )
		{
			CONT_ACTION_HISTORY::key_type const& lhsKey = (*base_iter).first;
			CONT_ACTION_HISTORY::mapped_type& lhsValue = (*base_iter).second;
			CONT_ACTION_HISTORY::iterator right_iter = base_iter;
			++right_iter; // 다음부터
			while( m_kContHistory.end() != right_iter )
			{
				CONT_ACTION_HISTORY::key_type const& rhsKey = (*right_iter).first;
				CONT_ACTION_HISTORY::mapped_type const& rhsValue = (*right_iter).second;

				CONT_MATCH_RESULT::key_type const kResultKey( std::make_pair(lhsKey, rhsKey) );
				CONT_MATCH_RESULT::iterator result_iter = m_kContResult.find( kResultKey );
				if( m_kContResult.end() == result_iter )
				{
					auto kRet = m_kContResult.insert( std::make_pair(kResultKey, MakeNewResult(base_iter, right_iter)) );
					if( kRet.second )
					{
						result_iter = kRet.first;
					}
				}
				else
				{
					(*result_iter).second.iLhsTotalCount += (*base_iter).second.GetTotalCount();
					(*result_iter).second.iRhsTotalCount += (*right_iter).second.GetTotalCount();
				}

				if( m_kContResult.end() != result_iter )
				{
					(*result_iter).second.dwEleapsedTime += dwEleapsedTime;
					(*result_iter).second.dwTotalEleapsedTime += dwEleapsedTime;
					lhsValue.Check( rhsValue, (*result_iter).second );
				}
				++right_iter;
			}
			lhsValue.Flush();
			++base_iter;
		}

	}
	void PgActionCheckMgr_Impl::GetResult(CONT_MATCH_RESULT_LIST& rkContOut, size_t const iMatchPercent, DWORD const dwLimitTime) // 10000분 률
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_MATCH_RESULT::iterator iter = m_kContResult.begin();
		while( m_kContResult.end() != iter )
		{
			CONT_MATCH_RESULT::mapped_type const& rkMatchResult = (*iter).second;
			if( IsCanReset(dwLimitTime, rkMatchResult) )
			{
				if( IsCanDetection(iMatchPercent, rkMatchResult) )
				{
					rkContOut.push_back( rkMatchResult );
				}
				(*iter).second.Reset();
			}
			++iter;
		}
	}
	SMatchResult PgActionCheckMgr_Impl::MakeNewResult(CONT_ACTION_HISTORY::const_iterator lhs, CONT_ACTION_HISTORY::const_iterator rhs) const
	{
		SMatchResult kNewResult;
		kNewResult.kLhsKey = (*lhs).first;
		kNewResult.iLhsTotalCount = (*lhs).second.GetTotalCount();
		kNewResult.kRhsKey = (*rhs).first;
		kNewResult.iRhsTotalCount = (*rhs).second.GetTotalCount();
		//kNewResult.iMatchCount = kNewResult.iMatchPoint = 0; // 생성자에서 초기화 함으로써 생략~
		return kNewResult;
	}

	//
	PgActionCheckMgr::PgActionCheckMgr()
		: m_kCont(), m_kThreadMutex()
	{
	}
	PgActionCheckMgr::~PgActionCheckMgr()
	{
	}
	void PgActionCheckMgr::Reg(SGroundKey const& rkGndKey, DWORD const dwTime)
	{
		BM::CAutoMutex kLock(m_kThreadMutex, true);
		m_kCont[rkGndKey].Tick(dwTime);
	}
	void PgActionCheckMgr::Unreg(SGroundKey const& rkGndKey)
	{
		BM::CAutoMutex kLock(m_kThreadMutex, true);
		m_kCont.erase(rkGndKey);
	}
	void PgActionCheckMgr::Add(SGroundKey const& rkGndKey, BM::GUID const& rkCharGuid)
	{
		BM::CAutoMutex kLock(m_kThreadMutex);
		CONT_HACK_ACTION_CHECKER::iterator find_iter = m_kCont.find(rkGndKey);
		if( m_kCont.end() != find_iter )
		{
			(*find_iter).second.Add(rkCharGuid);
		}
	}
	void PgActionCheckMgr::Del(SGroundKey const& rkGndKey, BM::GUID const& rkCharGuid, CONT_MATCH_RESULT_LIST& rkContOut)
	{
		BM::CAutoMutex kLock(m_kThreadMutex);
		CONT_HACK_ACTION_CHECKER::iterator find_iter = m_kCont.find(rkGndKey);
		if( m_kCont.end() != find_iter )
		{
			(*find_iter).second.Del(rkCharGuid, rkContOut);
		}
	}
	void PgActionCheckMgr::Action(SGroundKey const& rkGndKey, BM::GUID const& rkKeyGuid, EHackActionCheckType const eType, DWORD const dwTime)
	{
		BM::CAutoMutex kLock(m_kThreadMutex);
		CONT_HACK_ACTION_CHECKER::iterator find_iter = m_kCont.find(rkGndKey);
		if( m_kCont.end() != find_iter )
		{
			(*find_iter).second.Action(rkKeyGuid, eType, dwTime);
		}
	}
	void PgActionCheckMgr::Tick(DWORD const dwTimeLine, CONT_MATCH_MAP_RESULT& rkContOut, size_t const iLimitMatchRate, DWORD const dwLimitTime)
	{
		BM::CAutoMutex kLock(m_kThreadMutex);

		CONT_HACK_ACTION_CHECKER::iterator iter = m_kCont.begin();
		while( m_kCont.end() != iter )
		{
			(*iter).second.Tick(dwTimeLine);
			(*iter).second.GetResult(rkContOut[(*iter).first], iLimitMatchRate, dwLimitTime);
			++iter;
		}
	}

}