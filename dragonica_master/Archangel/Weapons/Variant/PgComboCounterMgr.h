#ifndef WEAPON_VARIANT_ACTION_BATTLE_PGCOMBOCOUNTERMGR_H
#define WEAPON_VARIANT_ACTION_BATTLE_PGCOMBOCOUNTERMGR_H

class PgComboCounterMgr
{
	typedef std::map< BM::GUID, PgComboCounter* > ContCombo;

public:
	PgComboCounterMgr();
	virtual ~PgComboCounterMgr();

	void Clear();

public:

	COMBO_TYPE GetCurrentCombo(BM::GUID const &rkGuid, const ECOMBO_STYLE kStyle=ECOMBO_TOTAL) const;

protected:

	bool AddComboCounter(BM::GUID const &rkGuid);
	bool DelComboCounter(BM::GUID const &rkGuid);
	bool GetComboCounter(BM::GUID const &rkGuid, PgComboCounter*& pkOut) const;

	mutable Loki::Mutex m_kCounterMutex;
	ContCombo m_kMap;
};

#endif // WEAPON_VARIANT_ACTION_BATTLE_PGCOMBOCOUNTERMGR_H