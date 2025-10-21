#ifndef WEAPON_VARIANT_UNIT_PGMONSTERBAG_H
#define WEAPON_VARIANT_UNIT_PGMONSTERBAG_H

#include "Lohengrin/dbtables.h"

typedef std::map< int, SMonsterControl >	CONT_HASH_MON_MULTI_DATA;
typedef std::map< int, CONT_SET_DATA >		CONT_HASH_MON_SINGLE_DATA;

class PgMonsterBag
{
public:
	PgMonsterBag(void);
	virtual ~PgMonsterBag(void);

public:
	bool Build(	
		const CONT_DEF_MAP_REGEN_POINT *pkMapRegenPoint,
		const CONT_DEF_MONSTER_BAG_CONTROL *pkMonsterBagControl,
		const CONT_DEF_MONSTER_BAG *pkMonsterBag,
		const CONT_DEF_MONSTER_BAG_ELEMENTS *pkMonsterBagElements );

	//! 몬스터 번호를 가져올 수 있다.
	bool GetMonster(int iMapNo, SMonsterControl& rkMonsterData) const;
	bool PgMonsterBag::GetMonsterBag(int const iMonBagNo,int & iMonNo) const;

	CONT_HASH_MON_MULTI_DATA const & GetMapMonsterCont()const {return m_kRealMonData;}

protected:

	CONT_DEF_MAP_REGEN_POINT m_kMapRegenPoint;
	CONT_DEF_MONSTER_BAG_CONTROL m_kMonsterBagControl;
	CONT_DEF_MONSTER_BAG m_kMonsterBag;
	CONT_DEF_MONSTER_BAG_ELEMENTS m_kMonsterBagElements;

	// 데이터.
	CONT_HASH_MON_MULTI_DATA m_kRealMonData;	// (맵 넘버 / 몬스터 넘버)

};

//#define g_MonsterBag SINGLETON_STATIC(PgMonsterBag)

#endif // WEAPON_VARIANT_UNIT_PGMONSTERBAG_H