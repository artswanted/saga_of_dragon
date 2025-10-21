#pragma once

#include "Lohengrin/DBTables.h"
#include "PgPremium.h"

typedef struct tagPremiumData
{
	tagPremiumData():iServiceNo(0), byGrade(0), wUseDate(0) {}

	int iServiceNo;
	BYTE byGrade;
	WORD wUseDate;
	CONT_DEF_PREMIUM_TYPE kContType;
	std::wstring kTitle;
} SPremiumData;

class PgDefPremiumMgr
{
public:
	typedef std::map<int, SPremiumData> CONT_DEF_PREMIUM;
	PgDefPremiumMgr(void);
	~PgDefPremiumMgr(void);

	SPremiumData const* GetDef(int const iServiceNo)const;

	bool Build(CONT_DEF_PREMIUM_SERVICE const& kContPremium, CONT_DEF_PREMIUM_ARTICLE const& kContServiceType);
	bool CreateCopy(int const iServiceNo, BM::DBTIMESTAMP_EX & kStartDate, BM::DBTIMESTAMP_EX & kEndDate, CONT_DEF_PREMIUM_TYPE & kContDef, CONT_PREMIUM_TYPE & kContCustom)const;

private:
	CONT_DEF_PREMIUM m_kContPremium;
};
