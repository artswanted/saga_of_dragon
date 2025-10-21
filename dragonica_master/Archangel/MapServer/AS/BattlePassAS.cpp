#include "stdafx.h"
#include <ActorEventSystem.h>
#include "PgSendWrapper.h"
#include "PgStaticRaceGround.h"
#include "Variant/PgQuestInfo.h"
#include "PgQuest.h"
#include "Variant/PgBattlePassMgr.h"

static void PT_C_M_REQ_BATTLE_PASS_QUEST__AES(AES::EVENT_RECIVER pkUnit, AES::EVENT_BODY pkPacket, PgGround* pkGround)
{
	CONT_DEF_BATTLE_PASS_QUEST const* pBattlePassQuest = NULL;
	g_kTblDataMgr.GetContDef(pBattlePassQuest);
	if (!pBattlePassQuest)
	{
		INFO_LOG(BM::LOG_LV3, "CONT_DEF_BATTLE_PASS_QUEST is null!");
		return;
	}

	if (!g_kBattlePassMgr.Locked_IsActive())
	{
		pkUnit->SendWarnMessage(200174/*Battle pass locked*/);
		return;
	}

    if (pkUnit->GetAbil(AT_LEVEL) < 20)
	{
		pkUnit->SendWarnMessage(200171);
		return;
	}

	PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
	if( !pkPlayer )
	{
		return;
	}

	const int iCurrentSeason = g_kBattlePassMgr.Locked_SeasonNo();
	const int iPlayerSeason = pkPlayer->GetBattlePassInfo().SeasonNo();
	const int iPlayerGrade = pkPlayer->GetBattlePassInfo().Grade();

	int iGradeNo = 0;
    pkPacket->Pop(iGradeNo);

	if (INVALID_BATTLE_PASS_SEASON == iPlayerSeason || iCurrentSeason != iPlayerSeason || iPlayerGrade < iGradeNo)
	{
		pkUnit->SendWarnMessage(200173/*Battle pass not avalible for you*/);
		return;
	}

	PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
	if( !pkMyQuest )
	{
        INFO_LOG(BM::LOG_LV3, "Failed get player quests '" << pkPlayer->GetID() << "'");
		return;
	}

	if (MAX_ING_SCENARIO_QUEST == pkMyQuest->GetIngScenarioQuestNum())
	{
		pkPlayer->SendWarnMessage(700401);
		return;
	}

	int iBattlePassDay = 0;
    pkPacket->Pop(iBattlePassDay);

	CONT_DEF_BATTLE_PASS_QUEST::const_iterator kItor = pBattlePassQuest->find(TBL_TRIPLE_KEY_INT(iCurrentSeason, iGradeNo, iBattlePassDay));
	if ( kItor == pBattlePassQuest->end() )
	{
		pkPlayer->SendWarnMessage(200172);
		return;
	}

	PgQuestInfo const* pkQuestInfo = NULL;
	const int iQuestNo = kItor->second.iBattlePassQuest;
	if( !g_kQuestMan.GetQuest(iQuestNo, pkQuestInfo) )
	{
        INFO_LOG(BM::LOG_LV3, "Invalid quest no " << iQuestNo);
		return;
	}

    if (pkQuestInfo->Type() != QT_BattlePass)
    {
        INFO_LOG(BM::LOG_LV3, "Quest " << iQuestNo << " type is not a QT_BattlePass!");
        return;
    }

    if (pkMyQuest->IsIngQuest(iQuestNo) || pkMyQuest->IsEndedQuest(iQuestNo))
    {
        pkPlayer->SendWarnMessage(200170);
        return;
    }

    BM::Stream kAddonPacket;
	kAddonPacket.Push( iQuestNo );

	CONT_PLAYER_MODIFY_ORDER kOrder;
    SPMOD_AddIngQuest kAddQuestData(iQuestNo, QS_Begin);
	kOrder.push_back(SPMO(IMET_ADD_INGQUEST, pkUnit->GetID(), kAddQuestData));

	PgAction_ReqModifyItem kBeinQuestAction(IMEPT_QUEST_BATTLE_PASS, pkGround->GroundKey(), kOrder, kAddonPacket);
	kBeinQuestAction.DoAction(pkUnit, NULL);
}
AES_REGISTER_TYPED_SYSTEM(PT_C_M_REQ_BATTLE_PASS_QUEST);