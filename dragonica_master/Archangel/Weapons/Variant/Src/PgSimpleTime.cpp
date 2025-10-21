
#include "stdafx.h"
#include "PgSimpleTime.h"

void SDAYOFWEEK::SetDayofWeek(std::vector<std::wstring> const& DayofWeek)
{
	std::vector<std::wstring>::const_iterator day_iter;
	for( day_iter = DayofWeek.begin(); day_iter != DayofWeek.end(); ++day_iter)
	{
		if((*day_iter) == L"MON")
		{
			AddDay(EDOW_MON);
		}
		else if((*day_iter) == L"TUE")
		{
			AddDay(EDOW_TUE);
		}
		else if((*day_iter) == L"WED")
		{
			AddDay(EDOW_WED);
		}
		else if((*day_iter) == L"THU")
		{
			AddDay(EDOW_THU);
		}
		else if((*day_iter) == L"FRI")
		{
			AddDay(EDOW_FRI);
		}
		else if((*day_iter) == L"SAT")
		{
			AddDay(EDOW_SAT);
		}
		else if((*day_iter) == L"SUN")
		{
			AddDay(EDOW_SUN);
		}
	}
}

bool SSIMPLETIMELIMIT::CheckTimeIsInDuration(SYSTEMTIME const& rkNow) const
{
	bool bBeginTimeCorrect = false;
	bool bEndTimeCorrect = false;
	if(kBegin.byHour == 0 && kBegin.byMin == 0 && kEnd.byHour == 0 && kEnd.byMin == 0)
	{ //제한 시간값이 모두 0이면 시간체크를 하지 않는다
		bBeginTimeCorrect = true;
		bEndTimeCorrect = true;
	}
	else
	{
		if(kBegin.byHour < rkNow.wHour)
		{
			bBeginTimeCorrect = true;
		}
		else if(kBegin.byHour == rkNow.wHour)
		{
			if(kBegin.byMin <= rkNow.wMinute)
			{
				bBeginTimeCorrect = true;
			}
		}

		if(kEnd.byHour > rkNow.wHour)
		{
			bEndTimeCorrect = true;
		}
		else if(kEnd.byHour == rkNow.wHour)
		{
			if(kEnd.byMin >= rkNow.wMinute)
			{
				bEndTimeCorrect = true;
			}
		}
	}
	return (bBeginTimeCorrect && bEndTimeCorrect && kDayOfWeek.IsInDay(eDayOfWeekIdx[rkNow.wDayOfWeek]));
}
