#include "stdafx.h"
#include "actions/element/PgActionExtractElement.h"

static const EAbilType EXTRACT_TYPES[] =
{
    AT_EXTRACT_ELEMENT_FIRE,
    AT_EXTRACT_ELEMENT_ICE,
    AT_EXTRACT_ELEMENT_NATURE,
    AT_EXTRACT_ELEMENT_CURSE,
    AT_EXTRACT_ELEMENT_DESTROY,
    AT_EXTRACT_ELEMENT_RANDOM,
    AT_EXTRACT_ELEMENT_RANDOM
};

static HRESULT Process(PgAction_Extract_Element* pkSelf, CUnit* pUser, CUnit* pTarget)
{
    PgInventory* pInv = pUser->GetInven();
    if (!pInv)
        return E_FAIL;

    PgBase_Item kItem;
	SItemPos kItemPos;
	pkSelf->m_kPacket.Pop(kItemPos);
    if ( pInv->GetItem(kItemPos, kItem) == E_FAIL )
        return E_FAIL;

	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef* pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pItemDef)
        return E_FAIL;

    int iItemBagNo = -1;
    for (int i = 0; i < _countof(EXTRACT_TYPES); ++i)
    {
        int iTemp = pItemDef->GetAbil(EXTRACT_TYPES[i]);
        if (iTemp != 0)
        {
            iItemBagNo = iTemp;
            break;
        }
    }

	if (iItemBagNo < 0)
        return E_FAIL;

	int const iLevel = pUser->GetAbil(AT_LEVEL);
    int iResultItemNo = 0;
	int iResultItemCount = 0;
	PgItemBag kItemBag;
	GET_DEF(CItemBagMgr, kItemBagMgr);
	kItemBagMgr.GetItemBagByGrp(iItemBagNo, iLevel, kItemBag);
	kItemBag.PopItem(iLevel, iResultItemNo, iResultItemCount);
	if (0 == iResultItemNo || 0 == iResultItemCount)
        return E_FAIL;

	int iAchievementType = 0;
	switch(iResultItemNo)
    {
	    case 99800110: { iAchievementType = AT_ACHIEVEMENT_ATTR_LOW_FIRE; }break;
	    case 99800120: { iAchievementType = AT_ACHIEVEMENT_ATTR_HIGH_FIRE; }break;
        case 99800210: { iAchievementType = AT_ACHIEVEMENT_ATTR_LOW_ICE; }break;
        case 99800220: { iAchievementType = AT_ACHIEVEMENT_ATTR_HIGH_ICE; }break;
	    case 99800310: { iAchievementType = AT_ACHIEVEMENT_ATTR_LOW_NATURE; }break;
	    case 99800320: { iAchievementType = AT_ACHIEVEMENT_ATTR_HIGH_NATURE; }break;
        case 99800410: { iAchievementType = AT_ACHIEVEMENT_ATTR_LOW_CURSE; }break;
        case 99800420: { iAchievementType = AT_ACHIEVEMENT_ATTR_HIGH_CURSE; }break;
        case 99800510: { iAchievementType = AT_ACHIEVEMENT_ATTR_LOW_DESTROY; }break;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder_Del;
    SPMOD_Modify_Count kDelData(kItem, kItemPos, -1);
	SPMO MODS_Del(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), kDelData);
	kOrder_Del.push_back(MODS_Del);

    PgAction_ReqModifyItem kItemModifyAction_Del(CIE_ExtractElement, pkSelf->m_kGndKey, kOrder_Del);
    kItemModifyAction_Del.DoAction(pUser, NULL);

    CONT_PLAYER_MODIFY_ORDER kOrder_Add;
    SPMOD_Add_Any AddData(iResultItemNo,iResultItemCount);
    SPMO MODS_Add(IMET_ADD_ANY,pUser->GetID(),AddData);
    kOrder_Add.push_back(MODS_Add);

    BM::Stream kAddPacket;
    kAddPacket.Push(iAchievementType);
    kAddPacket.Push(iResultItemCount);
    PgAction_ReqModifyItem kItemModifyAction_Add(CIE_ExtractElement, pkSelf->m_kGndKey, kOrder_Add, kAddPacket);
    kItemModifyAction_Add.DoAction(pUser, NULL);

	BM::Stream kPacket(PT_M_C_ANS_ELEMENT_EXTRACT);
	kPacket.Push(HRESULT(S_OK));
	kPacket.Push(iResultItemNo);
	kPacket.Push(iResultItemCount);
	pUser->Send(kPacket);


	return S_OK;
}

bool PgAction_Extract_Element::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	if(Process(this, pUser, pTarget) == S_OK)
	{
		return true;
	}
	return false;
}
