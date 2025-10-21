#ifndef WEAPON_VARIANT_ITEM_PGITEMOPTIONMGR_H
#define WEAPON_VARIANT_ITEM_PGITEMOPTIONMGR_H

#include "BM/stlsupport.h"
#include "Lohengrin/dbtables.h"
#include "item.h"
#include "itemDefMgr.h"

class PgItemOptionMgr
{
public:
	typedef std::map< int, int > CONT_TYPE_RATE;
	typedef std::map< TBL_PAIR_KEY_INT64, CONT_TYPE_RATE > CONT_OPTION_CANDI;//옵션 생성용 맵(pos, 그룹)에 따른. 옵션 대기 줄(key 장착위치 그룹, mapped :OptionType, OutRate) ex> 장갑, 1번그룹은 -> 무슨 속성이 무슨 확률로 정해짐
	typedef std::map< TBL_TRIPLE_KEY_INT, TBL_DEF_ITEM_OPTION > CONT_ITEM_OPTION_VALUE;//key = ( 타입, pos, 그룹 )
public:
	PgItemOptionMgr();
	virtual ~PgItemOptionMgr();

public:

	bool Build(CONT_TBL_DEF_ITEM_OPTION const &kItemOption);
	bool GetBasicAbil(SItemExtOptionKey const &kKey, CAbilObject *pkAbilObj)const;
	int	 GetBasicAbil(SItemExtOptionKey const &kKey) const;

	void Clear();
	void swap(PgItemOptionMgr& rkRight);


	bool CanAddOption(PgBase_Item const & rkItem)const;

	bool GenerateOption(PgBase_Item& rkItem, int const iRatiryControlNo) const;//최초 등급에 따라 옵션 생성.
	bool GenerateOption_Sub(PgBase_Item& rkItem, int const iCustomItemLimit = 0) const;//
	bool GenerateOption_Rare(PgBase_Item& rkItem) const;
	bool ReDiceOption(PgBase_Item& rkItem) const;//등급에 따라 옵션 변경
	bool ReplaceOption(int const iIdx,PgBase_Item& rkItem) const;
	bool DownGrade(int const iIdx, PgBase_Item& rkItem) const;
	
	
	int DiceOptionType(bool const bIsCashItem, int const iOptGroup, int const iEquipPos, std::list<int> const &kContIgnoreType )const;
	int DiceOptionLv(int const iItemLv)const;//해당 레벨에서 나올 수있는 옵션의 레벨

	static int DiceRarity(int const iControlType);//해당 레벨에서 나올 수있는 옵션의 레벨
	static int DicePlusUp( int const iControlType, int const iMaxPlusUpLv = IPULL_LIMIT_MAX );// PlusUp

	//아이템 옵션을 끝내고..
protected:
//	bool PrepareOptCandi(eEquipLimit const eBasePos,int const iOptionGroup,int const iAblePos, int const iOptType);//옵션 생성 관련 데이터 준비
	bool GetItemOptionCount(E_ITEM_GRADE const kItemGrade, int & iGenOptionCount) const;

protected:
	//옵션 후보군은. 가진 옵션에의해 변화하기 때문에 미리 계산이 불필요함.
	CONT_OPTION_CANDI			m_kContOptCandi;//옵션 생성 후보군(pos, 그룹)
	CONT_ITEM_OPTION_VALUE		m_kContOptValue;//타입, pos, 그룹
};

//#define g_kItemOptMgr SINGLETON_STATIC(PgItemOptionMgr)

#endif // WEAPON_VARIANT_ITEM_PGITEMOPTIONMGR_H