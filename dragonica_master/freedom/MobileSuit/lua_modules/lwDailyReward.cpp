#include "stdafx.h"
#include "lwUI.h"
#include <PgScripting.h>
#include <lwWString.h>
#include "PgPilotMan.h"

static inline void lwDailyRewardTaked()
{
    PgPlayer* pkPlayer = g_kPilotMan.GetPlayerUnit();
    if (!pkPlayer)
    {
        return;
    }

	XUI::CXUI_Wnd* pWnd = XUIMgr.Get(_T("SFRM_DAILY_REWARD"));
	if( !pWnd )
	{
		return;
	}

    PgDailyReward & kDailyReward = pkPlayer->GetDailyReward();
	BM::vstring	kStr(_T("FRM_ITEM_VAL"));
	kStr += kDailyReward.DayNo();
	XUI::CXUI_Wnd* pSlot = pWnd->GetControl(kStr);
    if (!pSlot)
    {
        return;
    }

    XUI::CXUI_Wnd* pBtn = pSlot->GetControl(_T("CBTN_SELECT_ITEM"));
    if (!pBtn)
    {
        return;
    }

    lwUIWnd(pBtn).CheckState(true);

    pBtn = pWnd->GetControl(_T("BTN_OK"));
    if (!pBtn)
    {
        return;
    }

    lwUIWnd(pBtn).Disable(true);
}

static inline void lwDailyRewardUI(lwUIWnd kWnd)
{
    PgPlayer* pkPlayer = g_kPilotMan.GetPlayerUnit();
    if (!pkPlayer)
    {
        return;
    }

	CONT_DEF_DAILY_REWARD const* pkDefDailyReward = NULL;
	g_kTblDataMgr.GetContDef(pkDefDailyReward);
    if ( !pkDefDailyReward )
    {
        return;
    }

	XUI::CXUI_Wnd* pWnd = kWnd.GetSelf();
	if( !pWnd )
	{
		return;
	}

    XUI::CXUI_Builder* pBuilder = dynamic_cast<XUI::CXUI_Builder *>(pWnd->GetControl(_T("BLD_ITEM_VAL")));
    if (!pBuilder)
    {
        return;
    }

    PgDailyReward & kDailyReward = pkPlayer->GetDailyReward();
    int const MAX_SLOT = pBuilder->CountX() * pBuilder->CountY();
	for( int i = 0; i < MAX_SLOT; ++i )
	{
		BM::vstring	kStr(_T("FRM_ITEM_VAL"));
		kStr += i;
		XUI::CXUI_Wnd* pSlot = pWnd->GetControl(kStr);
        if (!pSlot)
        {
            continue;
        }

		XUI::CXUI_Wnd* pText = pSlot->GetControl(_T("SFRM_TEXT"));
		XUI::CXUI_Wnd* pIcon = pSlot->GetControl(_T("ICN_ITEM_ICON"));
        XUI::CXUI_Wnd* pBtn = pSlot->GetControl(_T("CBTN_SELECT_ITEM"));
		if( !pText || !pIcon || !pBtn )
		{
			continue;
		}

        CONT_DEF_DAILY_REWARD::const_iterator kFind = pkDefDailyReward->find(i);
        if (kFind == pkDefDailyReward->end())
        {
            pSlot->Visible(false);
            continue;
        }

        int iItemNo = kFind->second.iItems[0];
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pDef = kItemDefMgr.GetDef(iItemNo);
		if( !pDef )
		{
			continue;
		}

        if (kDailyReward.DayNo() == i)
        {
            kStr = TTW(405005);
        }
        else
        {
            kStr = TTW(405027);
            kStr += _T("\n");
            kStr += i;
        }
        pText->Text((std::wstring const&)kStr);

        PgBase_Item kItem;
        kItem.ItemNo(iItemNo);
        kItem.Count(pDef->GetAbil(AT_DEFAULT_AMOUNT));
        lwUIWnd(pIcon).SetCustomDataAsSItem(kItem);
        lwUIWnd(pBtn).CheckState(kDailyReward.DayNo() > i);
    }
}

static inline void lwActivateDailyReward()
{
    PgPlayer* pkPlayer = g_kPilotMan.GetPlayerUnit();
    if (!pkPlayer || pkPlayer->GetAbil(AT_LEVEL) < 20 || !pkPlayer->GetDailyReward().CanGetReward())
    {
        return;
    }
    lwActivateUI("SFRM_DAILY_REWARD");
}

void DailyReward_RegisterWrapper(lua_State *pkState)
{
    lua_tinker::def(pkState, "lwActivateDailyReward", &lwActivateDailyReward);
    lua_tinker::def(pkState, "lwDailyRewardUI", &lwDailyRewardUI);
    lua_tinker::def(pkState, "lwDailyRewardTaked", &lwDailyRewardTaked);
}