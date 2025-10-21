
#ifndef WEAPON_VARIANT_PGSIMPLETIME_H
#define WEAPON_VARIANT_PGSIMPLETIME_H

#include <vector>
#include "winbase.h"
#include "BM/guid.h"

//다음의 타입들은 날짜를 제외한 요일, 시간 체크만을 위함이다.
typedef struct tagSSIMPLETIME
{
	BYTE byHour;
	BYTE byMin;

	tagSSIMPLETIME() : byHour(0), byMin(0) {}
	tagSSIMPLETIME(BYTE byInitHour, BYTE byInitMin, BYTE byInitSec) : byHour(byInitHour), byMin(byInitMin) {}
	tagSSIMPLETIME(SYSTEMTIME const& rkSysTime) :
		byHour(static_cast<BYTE>(rkSysTime.wHour)), byMin(static_cast<BYTE>(rkSysTime.wMinute)) {}
	tagSSIMPLETIME(BM::DBTIMESTAMP_EX const& rkDBTime) :
		byHour(static_cast<BYTE>(rkDBTime.hour)), byMin(static_cast<BYTE>(rkDBTime.minute)) {}

	void Set(BYTE byInitHour, BYTE byInitMin)
	{ byHour = byInitHour; byMin = byInitMin; }
}SSIMPLETIME;

typedef enum tagEDAYOFWEEK
{
	EDOW_NONE = 0,
	EDOW_MON = 1,
	EDOW_TUE = 2,
	EDOW_WED = 4,
	EDOW_THU = 8,
	EDOW_FRI = 16,
	EDOW_SAT = 32,
	EDOW_SUN = 64,
	EDOW_ALL = 255
}EDAYOFWEEK;

EDAYOFWEEK const eDayOfWeekIdx[8] = {EDOW_SUN, EDOW_MON, EDOW_TUE, EDOW_WED, EDOW_THU, EDOW_FRI, EDOW_SAT, EDOW_NONE};

typedef struct tagSDAYOFWEEK
{
protected:
	BYTE byDayOfWeek;

public:
	tagSDAYOFWEEK() : byDayOfWeek(EDOW_ALL) {}
	tagSDAYOFWEEK(EDAYOFWEEK eInitDay) : byDayOfWeek(eInitDay) {}
	void SetDay(EDAYOFWEEK eSetDay) { byDayOfWeek = eSetDay; }
	void AddDay(EDAYOFWEEK eAddDay) { byDayOfWeek |= eAddDay; }
	bool IsInDay(EDAYOFWEEK eDay) const { return (static_cast<WORD>(byDayOfWeek) & static_cast<WORD>(eDay)); }

	void SetDayofWeek(std::vector<std::wstring> const& DayofWeek);
protected:
	tagSDAYOFWEEK(BYTE) {}
	tagSDAYOFWEEK(int) {}
}SDAYOFWEEK;

typedef struct tagSSIMPLETIMELIMIT
{
	SSIMPLETIME kBegin, kEnd;
	SDAYOFWEEK kDayOfWeek;

	tagSSIMPLETIMELIMIT() :
		kBegin(), kEnd(), kDayOfWeek(EDOW_ALL) {}
	tagSSIMPLETIMELIMIT(SSIMPLETIME const& rkInitBegin, SSIMPLETIME const& rkInitEnd, SDAYOFWEEK const& rkInitDayOfWeek) :
		kBegin(rkInitBegin), kEnd(rkInitEnd), kDayOfWeek(rkInitDayOfWeek) {}

	void Clear(void)
	{
		kBegin.Set(0, 0);
		kEnd.Set(0, 0);
		kDayOfWeek.SetDay(EDOW_ALL);
	}

	bool CheckTimeIsInDuration(SYSTEMTIME const& rkNow) const;
}SSIMPLETIMELIMIT;

#endif
