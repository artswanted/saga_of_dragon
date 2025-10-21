#include "stdafx.h"
#include "Lohengrin/PacketType5.h"
#include "Lohengrin/dbtables.h"
#include "Variant/tabledatamanager.h"
#include "ActorEventSystem.h"
#include "PgGround.h"

static void PT_C_M_REQ_GET_DAILY__AES(PgPlayer* pkUnit, AES::EVENT_BODY pkBody, void *)
{
    if (pkUnit->GetAbil(AT_LEVEL) < 20)
    {
        pkUnit->SendWarnMessage(405029);
        return;
    }

    PgDailyReward & kDailyReward = pkUnit->GetDailyReward();
    if (!kDailyReward.CanGetReward())
    {
        pkUnit->SendWarnMessage(405024);
        return;
    }

    if (kDailyReward.IsExpired())
    {
        kDailyReward.DayNo(0);
    }

    kDailyReward.IncreaseRewardDate();

    CONT_DEF_DAILY_REWARD const * pDailyReward = NULL;
    g_kTblDataMgr.GetContDef(pDailyReward);
    CONT_DEF_DAILY_REWARD::const_iterator kFindIt = pDailyReward->find(kDailyReward.DayNo());
    if (kFindIt == pDailyReward->end())
    {
        pkUnit->SendWarnMessage(405025);
        kDailyReward.DayNo(0); // reset a day to 1
        return;
    }

    kDailyReward.DayNo((kDailyReward.DayNo() + 1) % pDailyReward->size());
    CONT_PLAYER_MODIFY_ORDER kOrder;
    for(int i = 0; i < MAX_DAILY_REWARD_ITEM; i++)
    {
        int iItemNo = kFindIt->second.iItems[i];
        if (iItemNo == 0)
        {
            continue;
        }

        kOrder.push_back(
            SPMO(IMET_ADD_ANY, pkUnit->GetID(), SPMOD_Add_Any(iItemNo, 1))
        );
    }

    kOrder.push_back(SPMO(IMET_DAILY_REWARD_UPDATE, pkUnit->GetID(), SPMOD_DailyReward(kDailyReward)));
    PgAction_ReqModifyItem kItemModifyAction_Add(CIE_DailyReward, pkUnit->GroundKey(), kOrder);
    kItemModifyAction_Add.DoAction(pkUnit, NULL);
    pkUnit->Send(BM::Stream(PT_M_C_NFY_GET_DAILY), E_SENDTYPE_SELF);
}
AES_REGISTER_TYPED_SYSTEM(PT_C_M_REQ_GET_DAILY);
