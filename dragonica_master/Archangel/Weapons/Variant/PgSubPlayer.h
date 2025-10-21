#ifndef WEAPON_VARIANT_UNIT_PGSUBPLAYER_H
#define WEAPON_VARIANT_UNIT_PGSUBPLAYER_H

#include "PgControlUnit.h"

class PgSubPlayer
	:	public PgControlUnit
{
public:
	PgSubPlayer();
	virtual ~PgSubPlayer();

	virtual void Init();
	HRESULT Create( CUnit* pkOwner );
	virtual EUnitType UnitType()const{return UT_SUB_PLAYER;}

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);

	virtual int GetAbil(WORD const Type) const;
	virtual bool SetAbil(WORD const Type, int const iInValue, bool const bIsSend = false, bool const bBroadcast = false);
	virtual bool AddAbil(WORD const Type, int const iValue);

	void SetCallerUnit(CUnit* pkCaller);
	CUnit* GetCallerUnit();

	PgMySkill const* GetCallerMySkill();

protected:
	bool SetBasicAbil();
	void SetBasicAbil(PgClassPetDef const &kPetDef);

	virtual bool DoLevelup( SClassKey const& rkNewLevelKey );
	virtual bool DoBattleLevelUp( short const nBattleLv );

	virtual void VOnRefreshAbil();

protected:
	PgPlayer* m_pkCaller;
};

#endif //WEAPON_VARIANT_UNIT_PGSUBPLAYER_H