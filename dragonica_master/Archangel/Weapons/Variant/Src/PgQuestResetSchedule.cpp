#include "stdafx.h"
#include "Lohengrin/dbtables2.h"
#include "Lohengrin/GameTime.h"
#include "PgQuestResetSchedule.h"

PgQuestResetSchedule::PgQuestResetSchedule(SQuestResetSchedule const& rkDBResetSchedule)
	: m_kDBResetSchedule(rkDBResetSchedule), m_iDayTime(0)
{
	CGameTime::SystemTime2SecTime(m_kDBResetSchedule.kResetBaseDate.operator SYSTEMTIME(), m_iDayTime, CGameTime::OneDay);
}

PgQuestResetSchedule::PgQuestResetSchedule(PgQuestResetSchedule const& rhs)
	: m_kDBResetSchedule(rhs.m_kDBResetSchedule), m_iDayTime(rhs.m_iDayTime)
{
}

PgQuestResetSchedule::~PgQuestResetSchedule()
{
}

bool PgQuestResetSchedule::operator <(PgQuestResetSchedule const& rhs)const
{
	return m_iDayTime < rhs.m_iDayTime;
}

namespace PgQuestResetScheduleUtil
{
	bool CheckResetSchedule(__int64 const iDffUserToNow, __int64 const iUserDayTime, __int64 const iNowDayTime, PgQuestResetSchedule const& rkResetSchedule)
	{
		if( 0 == iDffUserToNow )
		{
			return false;
		}

		__int64 const iDiffDay = rkResetSchedule.DateToDay() - iNowDayTime;
		__int64 const iModDay = (iDiffDay % rkResetSchedule.ResetLoopDay());
		if( 0 == iModDay
		||	(0 > iModDay && iDffUserToNow < iModDay) )
		{
			return true;
		}
		return false;
	}

	bool CheckResetSchedule(__int64 const iUserDayTime, __int64 const iNowDayTime, PgQuestResetSchedule const& rkResetSchedule)
	{
		return CheckResetSchedule(iUserDayTime - iNowDayTime, iUserDayTime, iNowDayTime, rkResetSchedule);
	}

	void CheckResetSchedule(BM::PgPackedTime const& rkUserDate, BM::PgPackedTime const& rkNowDate, ContQuestResetSchedule const& rkScheduleList, ContQuestID& rkOut)
	{
		__int64 iUserDayTime = 0;
		__int64 iNowDayTime = 0;

		CGameTime::SystemTime2SecTime(rkUserDate.operator SYSTEMTIME(), iUserDayTime, CGameTime::OneDay);
		CGameTime::SystemTime2SecTime(rkNowDate.operator SYSTEMTIME(), iNowDayTime, CGameTime::OneDay);

		ContQuestResetSchedule::const_iterator loop_iter = rkScheduleList.begin();
		while( rkScheduleList.end() != loop_iter )
		{
			ContQuestResetSchedule::value_type const& rkResetSchedule = (*loop_iter);
			if( CheckResetSchedule(iUserDayTime, iNowDayTime, rkResetSchedule) )
			{
				std::back_inserter(rkOut) = rkResetSchedule.QuestID();
			}
			++loop_iter;
		}
	}
};