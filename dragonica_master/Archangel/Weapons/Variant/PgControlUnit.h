#ifndef WEAPON_VARIANT_UNIT_PGCONTROLUNIT_H
#define WEAPON_VARIANT_UNIT_PGCONTROLUNIT_H

#include "unit.h"
#include "PgMySkill.h"

class PgControlUnit
	:	public CUnit
{
public:
	PgControlUnit(void);
	virtual ~PgControlUnit(void);

	virtual void Init();
	virtual HRESULT Create(void const* pkInfo);

	virtual bool IsCheckZoneTime(DWORD dwElapsed);
	virtual int CallAction(WORD wActCode, SActArg *pActArg) { return 0; }

	virtual bool SetAbil(WORD const Type, int const iValue, bool const bIsSend = false, bool const bBroadcast = false);

	SClassKey GetClassKey(void)const;

	PgMySkill* GetMySkill() { return &m_kMySkill; }
	PgMySkill const* GetMySkill() const { return &m_kMySkill; }

	virtual int GetBattleLevel(void)const;

protected:
	virtual bool DoLevelup( SClassKey const& rkNewLevelKey ) = 0;
	virtual bool DoBattleLevelUp( short const ) = 0;

protected:
	mutable PgMySkill m_kMySkill;
	BYTE		m_kLevelRank;
};

inline SClassKey PgControlUnit::GetClassKey(void)const
{
	return SClassKey( this->GetAbil(AT_CLASS), this->GetAbil(AT_LEVEL) );;
}

#endif // WEAPON_VARIANT_UNIT_PGCONTROLUNIT_H