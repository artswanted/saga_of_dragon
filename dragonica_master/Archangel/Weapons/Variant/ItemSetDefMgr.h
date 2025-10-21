#ifndef WEAPON_VARIANT_ITEM_ITEMSETDEFMGR_H
#define WEAPON_VARIANT_ITEM_ITEMSETDEFMGR_H

#include "TDefMgr.h"
#include "DefAbilType.h"
#include "Lohengrin/dbtables.h"
#include "IDObject.h"
#include "ItemDefMgr.h"

//ItemDefMgr 보다 하위
class CItemSetDefMgr;
class CUnit;

class CItemSetDef
	:	public CAbilObject
{
	friend class CItemSetDefMgr;

public:
	CItemSetDef();
	virtual ~CItemSetDef();

	//NeedItemCheck;
	int CheckNeedItem(CONT_HAVE_ITEM_DATA const &kContItem,CUnit const * pkUnit, bool& rbCompleteSet)const;//내가 입고 있는 %단위로 리턴함.
	
	const CONT_HAVE_ITEM_NO& NeedItem()const{return m_kContElement;}
	
protected:
	CLASS_DECLARATION_S(int, SetNo);
	CLASS_DECLARATION_S(int, NameNo);
	
	CONT_HAVE_ITEM_NO m_kContElement;
};

class CItemSetDefMgr
	:	public TDefMgr< TBL_DEF_ITEM_SET, TBL_DEF_ITEM_OPTION_ABIL, CItemSetDef, TBL_KEY_INT, TBL_KEY_INT  >
{
	friend struct ::Loki::CreateStatic< CItemSetDefMgr >;
public:
	CItemSetDefMgr(){}
	virtual ~CItemSetDefMgr(){}
public:
//	bool GetAbil(int const iSetNo, CAbilObject *pkAbilObj)const;
	int GetItemSetNo(int const iItemNo) const;

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil){return true;}
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil, CItemDefMgr const &rkItemDefMgr);
	virtual bool DataCheck()const;

	virtual void Clear();
	virtual void swap(CItemSetDefMgr& rkRight);

	//셋트번호 - 어빌갯수 의 셋트 효과를 담아서 쓰자.

	CItemSetDef const * GetEquipAbilDef(int const iSetNo, int const iEquipPeice)const;

	typedef std::map< POINT2, CItemSetDef* > CONT_DEF_DIVIDE_ABILS;//셋트번호/장착개수, 어빌
	CONT_DEF_DIVIDE_ABILS m_kContDivSetAbil;


	typedef std::map<int, int> CONT_ITEM_TO_SET;
	
	CONT_ITEM_TO_SET m_kContItemToSet;
};

//#define g_kItemSetDefMgr SINGLETON_STATIC(CItemSetDefMgr)

#endif // WEAPON_VARIANT_ITEM_ITEMSETDEFMGR_H