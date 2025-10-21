#ifndef WEAPON_VARIANT_UNIT_PGENTITY_H
#define WEAPON_VARIANT_UNIT_PGENTITY_H

#include "unit.h"

// 이 클래스는 범위마법(예: 블리자드 같은 것을 사용했을때 나오는 안보이는 더미이다.)을 사용하기위한 더미이다.(안보이는)
// 나중에 몬스터도 사용할 수도 있기때문에 CUnit을 바로 상속받는다.
class PgEntity
	: public CUnit
{
public:
	PgEntity();
	virtual ~PgEntity();

	virtual void Init();
	virtual HRESULT Create(const void* pkInfo);
	virtual int AutoHeal( unsigned long ulElapsedTime, float const fMultiplier = 1.0f);
	virtual EUnitType UnitType()const{ return UT_ENTITY; }

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);
	void Invalidate();

	virtual int CallAction(WORD wActCode, SActArg *pActArg)	{	return 0;	}
	virtual bool IsCheckZoneTime( DWORD dwElapsed );
	virtual HRESULT AddExp(__int64 const& iAddValue);

	virtual int GetAbil(WORD const Type) const;
	virtual int GetMyClass()const;

    void TunningAbil(int const iNo, int const iGrade, int const iLv, bool const bSend=false, bool const bBroadcast=false);
    bool IsEternalLife()const { return m_bEternalLife; }

protected:
	bool SetInfo( SEntityInfo const *pkInfo );
	void CopyAbilFromClassDef( CLASS_DEF_BUILT const *pkDef);
    bool IsSkipTunningAbil(int const iAbilType)const;

private:

	CLASS_DECLARATION_S(unsigned long, LifeTime);	// 유지시간
	CLASS_DECLARATION_S(BM::GUID, Party);			// 파티(파티스킬시 알기위해)

	CLASS_DECLARATION_S(POINT3, VisualFieldMin);
	CLASS_DECLARATION_S(POINT3, VisualFieldMax);
	CLASS_DECLARATION(bool, m_bUseVisualField, UseVisualField );

private:
	bool	m_bEternalLife;	
	SClassKey		m_kCallerClassKey;

};

#endif // WEAPON_VARIANT_UNIT_PGENTITY_H