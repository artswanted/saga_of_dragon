#ifndef WEAPON_VARIANT_ABIL_PGDEFSPENDMONEY_H
#define WEAPON_VARIANT_ABIL_PGDEFSPENDMONEY_H

#include "Lohengrin/dbtables.h"

typedef enum eSpendMoneyType
{
	ESMT_NONE					= 0,
	ESMT_REVIVE_MISSION			= 1,//미션에서 부활할때 소비
	ESMT_REMOVE_MISSIONPENALTY	= 2,//미션패널티를 삭제할때 소비
	ESMT_REVIVE_BY_INSURANCE	= 3,//보험으로 부활 시도
	ESMT_REVIVE_BY_CROSS		= 4,//십자가로 부활 소비
	ESMT_REVIVE_BY_SUPER_GROUND_FEATHER = 5,//슈퍼던젼 부활 깃털 소비
}ESpendMoneyType;

typedef struct tagFindSpendMoney
{
	tagFindSpendMoney(const ESpendMoneyType _Type, int const _iUserLevel)
		:	eType(_Type)
		,	iUserLevel(_iUserLevel)
	{}

	bool operator<(const tagFindSpendMoney& rhs)const
	{
		if ( eType == rhs.eType )
		{
			return iUserLevel < rhs.iUserLevel;
		}
		return eType < rhs.eType;
	}

	ESpendMoneyType eType;
	int				iUserLevel;
}SFindSpendMoney;

class PgDefSpendMoneyMgr
{
	typedef std::map<SFindSpendMoney,int>	ContSpendMoney;

public:
	PgDefSpendMoneyMgr();
	~PgDefSpendMoneyMgr();

	bool Build(const CONT_DEF_SPEND_MONEY* pkDef);
	void Swap(PgDefSpendMoneyMgr& rkRight);
	void Clear();

	int GetSpendMoney(const ESpendMoneyType _Type, int const _iUserLevel) const;

private:
	mutable Loki::Mutex		m_kMutex;
	ContSpendMoney	m_kContSpendMoney;

};

//#define g_kDefSpendMoneyMgr SINGLETON_STATIC(PgDefSpendMoneyMgr)

#endif // WEAPON_VARIANT_ABIL_PGDEFSPENDMONEY_H