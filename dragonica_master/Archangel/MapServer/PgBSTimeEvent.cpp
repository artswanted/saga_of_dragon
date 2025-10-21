#include "stdafx.h"
#include "PgBSTimeEvent.h"

//
PgBSTimeEventMgr::SBSTimeEventTickFunc::SBSTimeEventTickFunc(ContGuidSet& rkContRevive, ContGuidSet& rkContTeamPoint)
	: m_rkContRevive(rkContRevive), m_rkContTeamPoint(rkContTeamPoint)
{
}
bool PgBSTimeEventMgr::SBSTimeEventTickFunc::operator ()(CONT_TIME_EVENT::value_type& rkTimeEvent)
{
	switch( rkTimeEvent.eType )
	{
	case BSTET_TEAM_POINT:
		{
			m_rkContTeamPoint.insert( rkTimeEvent.kCharGuid );
			return true;
		}break;
	case BSTET_REVIVE:
		{
			m_rkContRevive.insert( rkTimeEvent.kCharGuid );
			return true; // 삭제
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown EventType = ") << rkTimeEvent.eType);
			return true;
		}break;
	}
	return false; // 삭제 하지 않음
}

PgBSTimeEventMgr::PgBSTimeEventMgr()
	: _INHERIT_TYPE()
{
}
PgBSTimeEventMgr::~PgBSTimeEventMgr()
{
}

