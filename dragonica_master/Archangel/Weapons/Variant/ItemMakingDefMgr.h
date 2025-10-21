#ifndef WEAPON_VARIANT_ITEM_ITEMMAKINGDEFMGR_H
#define WEAPON_VARIANT_ITEM_ITEMMAKINGDEFMGR_H

#include "defAbilType.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/LockUtil.h"
#include "Item.h"

typedef struct tagDefItemMakingData
	: public tagTblDefItemMaking
{
	tagDefItemMakingData()
	{
	}

	TBL_DEF_ITEM_BAG_ELEMENTS kNeedElements;
	TBL_DEF_COUNT_CONTROL kNeedCount;
	TBL_DEF_ITEM_BAG_ELEMENTS akResultItemElements[MAX_ITEMMAKING_ARRAY];//결과물.
	TBL_DEF_COUNT_CONTROL akResultItemCount[MAX_ITEMMAKING_ARRAY];
} SDefItemMakingData;

typedef std::map< int, SDefItemMakingData> ItemMakingDef_Cont;

class CItemMakingDefMgr
{
public:
	CItemMakingDefMgr(void);
	virtual ~CItemMakingDefMgr(void);

public:
	bool Build(
		const CONT_DEFITEMMAKING *pkDefItemMaking,
		const CONT_DEFRESULT_CONTROL *pkDefResultControl
		);
	// 컨테이너 전부 리턴.
	bool GetDefItemMaking(CONT_DEFITEMMAKING& rkDefItemMaking)const;
	bool GetDefResultControl(CONT_DEFRESULT_CONTROL& rkDefResultControl)const;

	// 각 항목들 리턴.
	bool GetMakingInfo(TBL_DEF_ITEMMAKING& rkOutInfo, int const iNo)const;
	bool GetResultInfo(TBL_DEF_RESULT_CONTROL& rkOutInfo, int iNo)const;
	bool GetDefItemMakingInfo(TBL_DEF_ITEMMAKING& rkDefItemMakingInfo, int const iMakingNo)const;	// Tbl 데이터
	bool GetDefItemMakingInfo(SDefItemMakingData& rkOutData, int const iMakingNo)const;				// 확장된 데이터
	bool GetContainer(const EMakingType eType, ItemMakingDef_Cont &rkCont) const;

protected:
	CONT_DEFITEMMAKING		m_kDefItemMaking;
	CONT_DEFRESULT_CONTROL	m_kDefResultControl;

	mutable Loki::Mutex m_kMutex;
};

//#define g_ItemMakingDefMgr SINGLETON_STATIC(CItemMakingDefMgr)

#endif // WEAPON_VARIANT_ITEM_ITEMMAKINGDEFMGR_H