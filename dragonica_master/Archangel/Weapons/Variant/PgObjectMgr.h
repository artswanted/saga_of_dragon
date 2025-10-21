#ifndef WEAPON_VARIANT_UNIT_PGOBJECTMGR_H
#define WEAPON_VARIANT_UNIT_PGOBJECTMGR_H

#include <map>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "PgTotalObjectMgr.h"
#include "BM/Observer.h"

class CUnit;

typedef std::map< BM::GUID, CUnit*> CONT_OBJECT_MGR_UNIT;

class PgObjectMgr//맵이 가지는 오브젝트 목록
{
public:
	PgObjectMgr();
	virtual ~PgObjectMgr();
	
public:
	void InitObjectMgr();

	CUnit* GetUnit(BM::GUID const &rkGuid);//검색 속도 면에서 유닛 타입을 넣는 GetUnit 하는게 좋습니다.
	CUnit* GetUnit(EUnitType const eUnitType, BM::GUID const &rkGuid);

	CUnit* GetUnit(BM::GUID const &rkGuid) const;//검색 속도 면에서 유닛 타입을 넣는 GetUnit 하는게 좋습니다.
	CUnit* GetUnit(EUnitType const eUnitType, BM::GUID const &rkGuid) const;

	bool RegistUnit( EUnitType const eType, BM::GUID const &kAddID, CUnit* pkUnit );
	bool UnRegistUnit( EUnitType const eType, BM::GUID const &kGuid );

	bool RegistUnit(CUnit* pkUnit);//OK
	bool UnRegistUnit( CUnit* pkUnit );//Ok
	bool UnRegistAllUnit(EUnitType const eType);//OK
	size_t GetUnitCount(EUnitType const eType)const;//OK
	
	void GetFirstUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::iterator& rkItor);
	void GetFirstUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::const_iterator& rkItor) const;
	CUnit* GetNextUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::iterator& rkItor);
	CUnit* GetNextUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::const_iterator& rkItor) const;

	bool GetUnitContainer(EUnitType const eType, CONT_OBJECT_MGR_UNIT *&pkOutWorkingCont);
	bool GetUnitContainer(EUnitType const eType, CONT_OBJECT_MGR_UNIT const *&pkOutWorkingCont)const;

private:
	CONT_OBJECT_MGR_UNIT m_kPlayerCont;		// UT_PLAYER
	CONT_OBJECT_MGR_UNIT m_kMonsterCont;		// UT_MONSTER + UT_BOSSMONSTER
	CONT_OBJECT_MGR_UNIT m_kEntityCont;		// UT_ENTITY
//	CONT_OBJECT_MGR_UNIT m_kNpcCont;
	CONT_OBJECT_MGR_UNIT m_kPetCont;
	CONT_OBJECT_MGR_UNIT m_kGndItemBoxCont;
	CONT_OBJECT_MGR_UNIT m_kObjectUnitCont;
	CONT_OBJECT_MGR_UNIT m_kMyHomeCont;		// UT_PLAYER
	CONT_OBJECT_MGR_UNIT m_kSummonedCont;		// UT_SUMMONED
	CONT_OBJECT_MGR_UNIT m_kSubPlayerCont;	// UT_SUB_PLAYER
	CONT_OBJECT_MGR_UNIT m_kCustomUnitCont;

private:
	PgObjectMgr(const PgObjectMgr &);
};

#endif // WEAPON_VARIANT_UNIT_PGOBJECTMGR_H