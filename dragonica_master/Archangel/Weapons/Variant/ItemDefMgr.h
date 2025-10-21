#ifndef WEAPON_VARIANT_ITEM_ITEMDEFMGR_H
#define WEAPON_VARIANT_ITEM_ITEMDEFMGR_H

#include "Lohengrin/dbtables.h"
#include "TDefMgr.h"
#include "DefAbilType.h"
#include "IDObject.h"
#include "Item.h"

namespace CItemDefUtil
{
	void SetServerMode(bool const bNew);
}

class CMonsterDefMgr;
class CItemBagMgr;
class PgMonsterBag;
class PgObjectUnitDefMgr;

typedef int (CALLBACK *LP_CALLBACK_RES_CHOOSER)(int const iGenderLimit);

class CItemDef
	:	public CAbilObject
{
public:
	CItemDef();
	virtual ~CItemDef();

public:
	virtual bool SetAbil(WORD const Type, int const iValue);
	virtual int GetAbil(WORD const wAbilType) const;
	virtual __int64 GetAbil64(WORD const wAbilType) const;

	CLASS_DECLARATION_S(int, SellPrice);//유저가 파는 가격
	CLASS_DECLARATION_S(int, BuyPrice);//유저가 사는 가격

	int ImproveAbil(WORD &wAbilType, PgBase_Item const &kItem)const;//랭크같은거로 인해 어빌값이 바뀌는것.
	int ImproveAbil(EAbilType const eAbilType, PgBase_Item const &kItem)const;//랭크같은거로 인해 어빌값이 바뀌는것.

	bool IsAmountItem()const;
	size_t MaxAmount()const;
	int PrimaryInvType()const;
	int PetInvType()const;
	bool CanConsume()const;//사용가능?
	bool CanEquip()const;//장착
	bool IsType(EItemType eTestType) const;
	bool IsPetItem()const;
	
	int EquipPos()const;
	int OptionGroup()const;

	static int EquipLimitToPos(int const iEquipLimit);

	void ResNo(int const iResNo){m_ResNo = iResNo;}
	virtual int ResNo()const;

	static void SetResChooserFunc(LP_CALLBACK_RES_CHOOSER pFunc);
	static void SetResChooserFuncNew(LP_CALLBACK_RES_CHOOSER pFunc);

	void AddDropGround(int const iGroundNo);
	
	CONT_SET_DATA const & GetDropGround()const{return m_kContDropGround;}

protected:
	CLASS_DECLARATION_V(int, m_No, No);
	CLASS_DECLARATION_V(int, m_NameNo, NameNo);
	short int m_sType;
	int m_iAttribute;//거래 속성등.
	BYTE m_byGender;
	short int m_sLevel;
	__int64 m_i64ClassLimit;
	__int64 m_i64ClassLimitDisplayFilter;
	int m_ResNo;
	int m_iOrder1,
		m_iOrder2,
		m_iOrder3,
		m_iCostumeGrade;
	static LP_CALLBACK_RES_CHOOSER m_pResChooseFunc;
	static LP_CALLBACK_RES_CHOOSER m_pResChooseFuncNew;
	CONT_SET_DATA m_kContDropGround;
};


//
class CItemDefMgr
	:	public TDefMgr< TBL_DEF_ITEM, TBL_DEF_ITEMABIL, CItemDef, TBL_KEY_INT, TBL_KEY_INT >
{
	friend struct ::Loki::CreateStatic< CItemDefMgr >;
public:
	CItemDefMgr();
	virtual ~CItemDefMgr();

	int GetAbil(int iItemNo, int iAbil) const;

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
	virtual bool DataCheck()const;

	HRESULT BuildDropPos(CMonsterDefMgr const & kMonsterDefMgr, CItemBagMgr const & kItemBagMgr, PgMonsterBag const & kMonsterBagMgr, CONT_DEF_MAP_ITEM_BAG const & kDefMapBag);
	bool	BuildMissionItemDropPos(CONT_DEF_MISSION_ROOT const & kContMissionRoot, CONT_DEF_MISSION_RESULT const & kContMissionResult, CONT_DEF_MISSION_CANDIDATE const & kContMissionCandi, CItemBagMgr const & kItemBagMgr);
	bool	BuildObjectItemDropPos(CONT_DEF_MAP_REGEN_POINT const & kRegenPoint, CONT_DEF_OBJECT_BAG const & kObjectBag, CONT_DEF_OBJECT_BAG_ELEMENTS const & kObjectBagElement, CItemBagMgr const & kItemBagMgr, PgObjectUnitDefMgr const & kObjectDefMgr);

private:

	bool BuildUnitDropItem(CAbilObject const * pkDef, CItemBagMgr const & kItemBagMgr, CONT_SET_DATA & kCont);
	bool BuildItemDropGroundInfo(CONT_SET_DATA const & kCont,int const iGroundNo);
	bool BuildMapItemBagDropPos(int const iGroundNo, CItemBagMgr const & kItemBagMgr, CONT_DEF_MAP_ITEM_BAG const & kDefMapBag, CONT_SET_DATA & kCont);
	bool BuildMonsterBagDropPos(SMonsterControl const & kMonCtrl, CMonsterDefMgr const & kMonsterDefMgr, CItemBagMgr const & kItemBagMgr, CONT_SET_DATA & kCont);
	bool CheckNeedAbil(DEF const * const pDef);

	CLASS_DECLARATION(CONT_DEF_ITEM_RES_CONVERT const*, m_pkContResConvert, ContResConvert);
};

//#define g_kItemDefMgr SINGLETON_STATIC(CItemDefMgr)

class CItemEnchantDef
	:	public CAbilObject
{
public:
	CItemEnchantDef(){}
	virtual ~CItemEnchantDef(){}

	SItemEnchantKey Key()const
	{
		return SItemEnchantKey(Type(), Lv());
	}
protected:
	CLASS_DECLARATION_V(int, m_Type, Type);
	CLASS_DECLARATION_V(int, m_Lv, Lv);
	CLASS_DECLARATION_V(int, m_NameNo, NameNo);
};

class CItemEnchantDefMgr
	:	public TDefMgr< TBL_DEF_ITEM_ENCHANT, TBL_DEF_ITEMABIL, CItemEnchantDef, SItemEnchantKey, TBL_KEY_INT >
{
	friend struct ::Loki::CreateStatic< CItemEnchantDefMgr >;
public:
	CItemEnchantDefMgr(){}
	virtual ~CItemEnchantDefMgr(){}

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
};

//#define g_ItemEnchantDefMgr SINGLETON_STATIC(CItemEnchantDefMgr)

class CRareDef
	:	public CAbilObject
{
public:
	CRareDef(){}
	virtual ~CRareDef(){}

public:
	virtual int GetAbil(WORD const wAbilType)
	{
		return CAbilObject::GetAbil(wAbilType);
	}
	virtual __int64 GetAbil64(WORD const wAbilType) const
	{
		return CAbilObject::GetAbil64(wAbilType);
	}

protected:
	CLASS_DECLARATION_V(int, m_No, No);
	CLASS_DECLARATION_V(int, m_NameNo, NameNo);
};

class CRareDefMgr
	:	public TDefMgr< TBL_DEF_ITEMRARE, TBL_DEF_ITEMABIL, CRareDef, TBL_KEY_INT, TBL_KEY_INT  >
{
	friend struct ::Loki::CreateStatic< CRareDefMgr >;
public:
	CRareDefMgr(){}
	virtual ~CRareDefMgr(){}

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
};

//#define g_RareDefMgr SINGLETON_STATIC(CRareDefMgr)

#endif // WEAPON_VARIANT_ITEM_ITEMDEFMGR_H