#ifndef WEAPON_VARIANT_PET_PGPETHATCHMGR_H
#define WEAPON_VARIANT_PET_PGPETHATCHMGR_H

#include "PgClassPetDefMgr.h"

#pragma pack(1)

struct SPetBonusStatus
{
	SPetBonusStatus( int const _iValue, int const _iRate )
		:	iValue(_iValue)
		,	iRate(_iRate)
	{}

	int		iValue;
	int		iRate;
};
typedef std::vector< SPetBonusStatus >		LIST_PET_BONUS_STATUS;

struct SPetBonusStatusSelector
{
	SPetBonusStatusSelector()
		:	iSelectRate(0)
	{}

	bool PopBonusStatus( PgBase_Item &rkPetItem )const;

	int						iSelectRate;
	LIST_PET_BONUS_STATUS	kTypeList;
	LIST_PET_BONUS_STATUS	kLevelList;
};
typedef std::list< SPetBonusStatusSelector >	LIST_BONUS_STATUS_SELECTOR;

struct SPetClassRate
	:	public SClassKey
{
	SPetClassRate( SClassKey const &kClassKey, int const _iRate )
		:	SClassKey( kClassKey )
		,	iRate(_iRate)
	{
	}

	int iRate;
	LIST_BONUS_STATUS_SELECTOR	kBonusStatusSelectorList;
};
typedef std::vector< SPetClassRate >		LIST_PET_CLASS_RATE;

struct SPetPeriodRate
{
	SPetPeriodRate( short _sPeriod, int const _iRate )
		:	sPeriod(_sPeriod)
		,	iRate(_iRate)
	{}

	short sPeriod;
	int iRate;
};
typedef std::vector< SPetPeriodRate >		LIST_PET_PERIOD_RATE;

#pragma pack()


class PgPetHatchAction
{
public:
	PgPetHatchAction(void);
	~PgPetHatchAction(void);

	bool Init( PgClassPetDefMgr const &rkClassPetDefMgr, CONT_DEF_PET_HATCH::mapped_type const &kPetHatch, CONT_DEF_PET_BONUSSTATUS const &kContPetBonusStatus );
	bool PopPet( PgBase_Item &rkOutPetItem )const;

	LIST_PET_CLASS_RATE const & PetClassRate() const {return m_kListPetClassRate;}

private:
	LIST_PET_CLASS_RATE		m_kListPetClassRate;
	LIST_PET_PERIOD_RATE	m_kListPetPeriodRate;
};

class PgPetHatchMgr
{
	typedef std::map< int, PgPetHatchAction >		CONT_PET_HATCH_ACTION;
	typedef std::map<SClassKey, SPetClassRate>		CONT_PET_CLASS_RATE;

public:
	PgPetHatchMgr(void);
	~PgPetHatchMgr(void);

	bool Build( PgClassPetDefMgr const &rkClassPetDefMgr, CONT_DEF_PET_HATCH const &rkDefPetHatch, CONT_DEF_PET_BONUSSTATUS const &rkDefPetBonusStatus );
	bool PopPet( int const iPetHatchNo, PgBase_Item &rkOutPetItem )const;
	bool RedicePetOption(PgBase_Item const & kItem,SEnchantInfo & kNewEnchantInfo) const;
	
private:
	CONT_PET_HATCH_ACTION		m_kContPetHatchAction;
	CONT_PET_CLASS_RATE			m_kContPetClassRate;
};

#endif // WEAPON_VARIANT_PET_PGPETHATCHMGR_H