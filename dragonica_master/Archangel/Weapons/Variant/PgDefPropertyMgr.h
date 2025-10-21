#ifndef WEAPON_VARIANT_ABIL_PGDEFPROPERTYMGR_H
#define WEAPON_VARIANT_ABIL_PGDEFPROPERTYMGR_H

#include "BM/ObjectPool.h"
#include "Lohengrin/ProcessConfig.h"
#include "TDefMgr.h"
#include "Lohengrin/dbtables.h"

class PgDefPropertyMgr
{
public:
	
	PgDefPropertyMgr(){}
	virtual ~PgDefPropertyMgr(){}

public:

	bool Build( CONT_DEF_PROPERTY const& kContDefProperty);
	virtual void Clear();
	void	swap(PgDefPropertyMgr &rRight);
	int		GetRate(int iOffense, int iOffenseLv, int iDefence, int iDefenceLv) const;

protected:	

	CONT_DEF_PROPERTY	m_kContProperty;
	//BM::TObjectPool< TBL_DEF_PROPERTY > m_kPropertyPool;
	mutable Loki::Mutex m_kMutex;
};

#endif // WEAPON_VARIANT_ABIL_PGDEFPROPERTYMGR_H