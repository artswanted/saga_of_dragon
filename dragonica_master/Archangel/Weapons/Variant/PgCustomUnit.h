#ifndef WEAPON_VARIANT_UNIT_CUSTOMUNIT_H
#define WEAPON_VARIANT_UNIT_CUSTOMUNIT_H

#include "unit.h"

// 이 클래스는 범위마법(예: 블리자드 같은 것을 사용했을때 나오는 안보이는 더미이다.)을 사용하기위한 더미이다.(안보이는)
// 나중에 몬스터도 사용할 수도 있기때문에 CUnit을 바로 상속받는다.
class PgCustomUnit
	: public CUnit
{
public:
	PgCustomUnit();
	virtual ~PgCustomUnit();

	virtual HRESULT Create(const void* pkInfo);
	virtual EUnitType UnitType()const{ return UT_CUSTOM_UNIT; }
	virtual bool IsCheckZoneTime( DWORD dwElapsed );
	virtual int CallAction(WORD wActCode, SActArg *pActArg)	{	return 0;	}
	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);

protected:
	bool SetInfo( SCustomUnitInfo const *pkInfo );

};

#endif // WEAPON_VARIANT_UNIT_CUSTOMUNIT_H