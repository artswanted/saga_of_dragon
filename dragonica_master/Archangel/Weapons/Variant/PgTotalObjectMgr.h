#ifndef WEAPON_VARIANT_UNIT_PGTOTALOBJECT_H
#define WEAPON_VARIANT_UNIT_PGTOTALOBJECT_H

#include <map>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "Unit.h"
#include "PgEntity.h"
#include "PgPlayer.h"
#include "PgMonster.h"
#include "PgBoss.h"
#include "PgNpc.h"
#include "PgGroundItemBox.h"
#include "PgObjectUnit.h"
#include "PgPet.h"
#include "PgMyHome.h"
#include "PgSummoned.h"
#include "PgSubPlayer.h"
#include "PgCustomUnit.h"

class PgTotalObjectMgr //오브젝트 풀.
{
	typedef std::map< BM::GUID, CUnit* > Cont_Unit;
	typedef std::map< BM::GUID, PgGroundItemBox* > ContGndItemBox;

	typedef BM::TObjectPool< PgPlayer >			PlayerPool;
	typedef BM::TObjectPool< PgMonster >		MonsterPool;
	typedef BM::TObjectPool< PgBoss >			BossPool;
	typedef BM::TObjectPool< PgNpc >			NpcPool;
	typedef BM::TObjectPool< PgGroundItemBox >	GndItemBoxPool;
	typedef BM::TObjectPool< PgEntity >			EntityPool;
	typedef BM::TObjectPool< PgObjectUnit >		ObjectUnitPool;
	typedef BM::TObjectPool< PgPet >			PetPool;
	typedef BM::TObjectPool< PgMyHome >			MyHomePool;
	typedef BM::TObjectPool< PgSummoned >		SummonedPool;
	typedef BM::TObjectPool< PgSubPlayer >		SubPlayerPool;
	typedef BM::TObjectPool< PgCustomUnit >		CustomUnitPool;

public:
	PgTotalObjectMgr();
	virtual ~PgTotalObjectMgr(){};

public:
	CUnit* CreateUnit(const EUnitType eType, BM::GUID const &rkGuid, bool* pbFind=NULL);
	bool ReleaseUnit(CUnit* pkUnit);
	bool ReleaseFlush();

	bool RegistUnit(CUnit* pkUnit);
	bool UnRegistUnit(CUnit* pkUnit);//지우는건 여기서 안한다

protected:
	Cont_Unit m_kTotalUnit;
	//ContGndItemBox m_kGndItemBox;

	PlayerPool		m_kPlayerPool;
	MonsterPool		m_kMonsterPool;
	NpcPool			m_kNpcPool;
	BossPool		m_kBossPool;
	GndItemBoxPool	m_kGndItemBoxPool;
	EntityPool		m_kEntityPool;
	ObjectUnitPool	m_kObjectUnitPool;
	PetPool			m_kPetPool;
	MyHomePool		m_kMyHomePool;
	SummonedPool	m_kSummonedPool;
	SubPlayerPool	m_kSubPlayerPool;
	CustomUnitPool	m_kCustomUnitPool;

	typedef std::set< CUnit* > RELEASE_WAIT_CONT;
	RELEASE_WAIT_CONT m_kRelWait;
	Loki::Mutex m_kMutex;
	Loki::Mutex m_kReleaseMutex;
};

typedef struct tagUNIT_PTR_HELPER
{
	tagUNIT_PTR_HELPER()
		:	pkUnit(NULL)
		,	bAutoRemove(false)
		,	bReference(false)
		,	bRestore(false)
	{}

	explicit tagUNIT_PTR_HELPER( CUnit *_pkUnit, bool bAuto=false, bool bRef = false)
		:	pkUnit(_pkUnit)
		,	bAutoRemove(bAuto)
		,	bReference(bRef)
		,	bRestore(false)
	{}

	tagUNIT_PTR_HELPER const& operator=( tagUNIT_PTR_HELPER &rhs )
	{
		bAutoRemove = rhs.bAutoRemove;
		rhs.bAutoRemove = false;//
		pkUnit = rhs.pkUnit;
		bReference = rhs.bReference;
		bRestore = rhs.bRestore;
		return *this;
	}

	bool operator==( CUnit *_pkUnit )const
	{
		return pkUnit == _pkUnit;
	}

	bool operator == (const tagUNIT_PTR_HELPER &rhs)const
	{
		return pkUnit->GetID() == rhs.pkUnit->GetID();
	}

	bool operator == (const BM::GUID &rhs)const
	{
		return pkUnit->GetID() == rhs;
	}

	bool operator < (const tagUNIT_PTR_HELPER &rhs)const
	{
		if( pkUnit->GetID() < rhs.pkUnit->GetID())	{return true;}
		if( pkUnit->GetID() > rhs.pkUnit->GetID())	{return false;}

		return false;
	}

	bool bAutoRemove;
	CUnit* pkUnit;
	bool bReference; // 참고용 유닛인가?
	bool bRestore; // 서버에서 실패 처리된 유닛으로, 서버의 상태로 복구 해줘야하는 유닛이다.
} UNIT_PTR_HELPER;

__int64 const MAP_CHANGE_COUNT_UP = 999;

class UNIT_PTR_ARRAY
	:	public std::list<UNIT_PTR_HELPER>
{
public:
	UNIT_PTR_ARRAY();
	UNIT_PTR_ARRAY( CUnit *pkUnit, bool bAutoRemove=false, bool bFront=false, bool bRef = false );
	virtual ~UNIT_PTR_ARRAY();

public:
	void clear();//오버로딩
	void swap( UNIT_PTR_ARRAY& rhs );
	bool swap( UNIT_PTR_ARRAY &rhs, UNIT_PTR_ARRAY::iterator &unit_itr );

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const;
//	virtual void WriteToPacket(BM::Stream &rkPacket, bool const bIsSimple, bool const bIsForSave);
	virtual EWRITETYPE ReadFromPacket( BM::Stream &rkPacket, bool bTotalObjFind=false );

	void Add(CUnit* pkUnit, bool const bAutoRemove=false, bool const bFront=false, bool const bRef = false);
	void AddToRestore(CUnit* pkUnit, bool const bRestore=true, bool const bAutoRemove=false, bool const bFront=false, bool const bRef = false);
	void AddToUniqe(CUnit* pkUnit, bool const bAutoRemove=false, bool const bFront=false, bool const bRef = false);

	UNIT_PTR_ARRAY::iterator Remove(CUnit *pkUnit);
	UNIT_PTR_ARRAY::iterator erase(UNIT_PTR_ARRAY::iterator unit_itr);
	UNIT_PTR_ARRAY::iterator OnlyErase(UNIT_PTR_ARRAY::iterator unit_itr);

	CLASS_DECLARATION_NO_SET(EWRITETYPE, m_kWriteType, GetWriteType);
	
	void SetWriteType(EWRITETYPE const kWriteType)//add를 외부에서 강제로 할경우에만 셋팅 하세요.
	{
		m_kWriteType = kWriteType;
	}
};

#define g_kTotalObjMgr SINGLETON_STATIC(PgTotalObjectMgr)

#endif // WEAPON_VARIANT_UNIT_PGTOTALOBJECT_H