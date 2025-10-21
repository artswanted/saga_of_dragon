#include "StdAfx.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"

#include "BM/FileSupport.h"

#include "Lohengrin/LogGroup.h"
#include "BM/HSEL.h"

CTableDataManager::CTableDataManager(void) : m_bUTF8(false)
{
	IsInit(false);
}

CTableDataManager::~CTableDataManager(void)
{
}

void CTableDataManager::DataToPacket(BM::Stream &rkPacket)
{//�̰� 2�ް�.
//	/*
	rkPacket.Data().reserve(1024*1024*15);//10�ް��� ������

	DWORD const dwBeginTime = BM::GetTime32();
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Begin ----");

	//BM::CAutoMutex Lock(GetLock()->kObjectLock);//�� ���̺��� �ܺο��� ���� ����� �ɷ����� ������ ������.
	BM::CAutoMutex lock(m_kMutex);

	PU::TWriteArray_A(rkPacket, m_kContDefCharacterCreateSet);
	PU::TWriteTable_MM(rkPacket, m_DefClass);
	PU::TWriteTable_MM(rkPacket, m_DefClassAbil);
	PU::TWriteTable_MM(rkPacket, m_DefClassPet);
	PU::TWriteTable_AM(rkPacket, m_DefClassPetLevel);
	PU::TWriteTableArray_AMM(rkPacket, m_DefClassPetSkill);
	PU::TWriteTable_AM(rkPacket, m_DefClassPetItemOption);
	PU::TWriteTable_MM(rkPacket, m_DefClassPetAbil);
	PU::TWriteTable_AM(rkPacket, m_DefItemBag);
	PU::TWriteTable_MM(rkPacket, m_DefAbilType);
	PU::TWriteTable_MM(rkPacket, m_DefBaseCharacter);
	PU::TWriteTable_MM(rkPacket, m_DefItem);
	PU::TWriteTable_MM(rkPacket, m_DefItemAbil);
	PU::TWriteTable_MM(rkPacket, m_DefItemRare);
	PU::TWriteTable_MM(rkPacket, m_DefItemRareGroup);
	PU::TWriteTable_MM(rkPacket, m_DefItemResConvert);
	PU::TWriteTable_MM(rkPacket, m_DefMap);
	PU::TWriteTable_MM(rkPacket, m_DefMapAbil);
	PU::TWriteTable_MM(rkPacket, m_DefMapItem);
	PU::TWriteTable_MM(rkPacket, m_DefMapRegenPoint);
	PU::TWriteTable_MM(rkPacket, m_DefMonster);
	PU::TWriteTable_MM(rkPacket, m_DefMonsterAbil);
	PU::TWriteTable_MM(rkPacket, m_DefMonsterTunningAbil);
	PU::TWriteTable_MM(rkPacket, m_DefNPC);
	PU::TWriteTable_MM(rkPacket, m_DefNPCAbil);
	PU::TWriteTable_MM(rkPacket, m_DefSkill);
	PU::TWriteTable_MM(rkPacket, m_DefSkillAbil);
	PU::TWriteTable_MM(rkPacket, m_DefSkillSet);
	PU::TWriteTable_MM(rkPacket, m_DefRes);
	PU::TWriteArray_A(rkPacket, m_DefChannelEffect);
	PU::TWriteTable_MM(rkPacket, m_DefStrings);
	PU::TWriteTable_MM(rkPacket, m_DefEffect);
	PU::TWriteTable_MM(rkPacket, m_DefEffectAbil);
	PU::TWriteTable_MM(rkPacket, m_DefUpgradeClass);
	PU::TWriteTable_MM(rkPacket, m_DefItemEnchant);
	PU::TWriteTable_MM(rkPacket, m_DefCharacterBaseWear);
	PU::TWriteTable_MM(rkPacket, m_DefItemPlusUpgrade);
	PU::TWriteTable_MM(rkPacket, m_DefItemRarityUpgrade);
	PU::TWriteTable_MM(rkPacket, m_DefItemRarityControl);
	PU::TWriteTable_MM(rkPacket, m_DefItemPlusUpControl);
	PU::TWriteTable_MM(rkPacket, m_DefItemEnchantShift);
	PU::TWriteTable_MM(rkPacket, m_DefItemDisassemble);
	PU::TWriteTable_MM(rkPacket, m_DefSuccessRateControl);
	PU::TWriteTable_MM(rkPacket, m_DefMonsterBag);
	PU::TWriteTable_MM(rkPacket, m_kDefGmCmd);
	PU::TWriteTable_MM(rkPacket, m_DefCountControl);
	PU::TWriteTable_MM(rkPacket, m_DefItemBagElements);
	PU::TWriteTable_MM(rkPacket, m_DefMonsterBagElements);
	PU::TWriteTable_MM(rkPacket, m_DefMonsterBagControl);
	PU::TWriteTable_MM(rkPacket, m_DefItemContainer);
	PU::TWriteTable_MM(rkPacket, m_DefObject);
	PU::TWriteTable_MM(rkPacket, m_DefObjectAbil);
	PU::TWriteTable_MM(rkPacket, m_DefObjcetBag);
	PU::TWriteTable_MM(rkPacket, m_DefObjcetBagElements);
	PU::TWriteTable_MM(rkPacket, m_DefItemBagGroup);
	PU::TWriteTable_MM(rkPacket, m_DefMapItemBag);
	PU::TWriteTable_MM(rkPacket, m_DefMissionResult);
	PU::TWriteTable_MM(rkPacket, m_DefMissionRoot);
	PU::TWriteTable_MM(rkPacket, m_DefMissionCandidate);
	PU::TWriteTable_MM(rkPacket, m_DefDefenceAddMonster);
    PU::TWriteTable_MM(rkPacket, m_DefShopInEmporia);
	PU::TWriteTable_MM(rkPacket, m_DefShopInGame);
	PU::TWriteTable_MM(rkPacket, m_DefShopInStock);
	PU::TWriteArray_M(rkPacket, m_DefItemOption);
	PU::TWriteTable_MM(rkPacket, m_DefItemOptionAbil);
	PU::TWriteTable_MM(rkPacket, m_DefDropMoneyControl);
	PU::TWriteTable_AM(rkPacket, m_DefPvPGroundGroup);
	PU::TWriteTable_MM(rkPacket, m_DefPvPGroundMode);
	PU::TWriteArray_M(rkPacket, m_kDefPvPReward);
	PU::TWriteTable_AM(rkPacket, m_kDefPvPRewardItem);
	PU::TWriteTable_AA(rkPacket, m_kDefPvPPlayTimeInfo);
	PU::TWriteTable_MM(rkPacket, m_DefItemMaking);
	PU::TWriteTable_MM(rkPacket, m_DefCooking);
	PU::TWriteTable_MM(rkPacket, m_DefResultControl);
	PU::TWriteTable_MM(rkPacket, m_DefQuestReward);
	PU::TWriteArray_M(rkPacket, m_DefQuestResetSchedule);
	PU::TWriteArray_M(rkPacket, m_DefQuestRandom);
	PU::TWriteTable_MM(rkPacket, m_DefQuestRandomExp);
	PU::TWriteTable_MM(rkPacket, m_DefQuestRandomTacticsExp);
	PU::TWriteTable_MM(rkPacket, m_DefQuestWanted);
	PU::TWriteTable_MM(rkPacket, m_DefDynamicAbilRate);
	PU::TWriteTable_MM(rkPacket, m_DefDynamicAbilRateBag);
	PU::TWriteTable_MM(rkPacket, m_DefItemSet);
	PU::TWriteTable_MM(rkPacket, m_DefSpendMoney);
	PU::TWriteTable_MM(rkPacket, m_DefGuildLevel);
	PU::TWriteTable_MM(rkPacket, m_DefGuildSkill);
	PU::TWriteArray_M(rkPacket, m_DefFilterUnicode);
	PU::TWriteTable_MM(rkPacket, m_kDefProperty);
	PU::TWriteArray_M(rkPacket, m_DefMapEffect);
	PU::TWriteArray_M(rkPacket, m_DefMapEntity);
	PU::TWriteTable_MM(rkPacket, m_DefFiveElementInfo);
	PU::TWriteTable_MM(rkPacket, m_DefMapStoneControl);
	PU::TWriteTable_MM(rkPacket, m_DefItemRarityUpgradeCostRate);
	PU::TWriteTable_MM(rkPacket, m_DefTacticsLevel);
	PU::TWriteTable_MM(rkPacket, m_DefTacticsQuestPseudo);
	PU::TWriteTable_MM(rkPacket, m_DefMonKillCountReward);
	PU::TWriteTable_AA(rkPacket, m_DefTransTower);
	PU::TWriteTable_AA(rkPacket, m_DefParty_Info);	
	PU::TWriteTable_MM(rkPacket, m_DefCashShop);
	PU::TWriteTable_AM(rkPacket, m_kDefEmporia);
	PU::TWriteTable_MM(rkPacket, m_DefCashShopArticle);
	PU::TWriteTable_MM(rkPacket, m_DefCashShopLimitSell);
	PU::TWriteTable_MM(rkPacket, m_kDefContAchievements);
	PU::TWriteTable_MM(rkPacket, m_kDefAchievements);
	PU::TWriteTable_MM(rkPacket, m_kDefAchievements_SaveIdx);
	PU::TWriteTable_MM(rkPacket, m_kDefRecommendationItem);
	PU::TWriteTable_MM(rkPacket, m_DefItem2Achievement);
	PU::TWriteTable_MM(rkPacket, m_kDefGroundRareMonster);
	PU::TWriteTable_MM(rkPacket, m_kDefRareMonsterSpeech);
	PU::TWriteTable_MM(rkPacket, m_kCardLocal);
	PU::TWriteTable_MM(rkPacket, m_kDefCardKeyString);
	PU::TWriteTable_MM(rkPacket, m_kCardAbil);
	PU::TWriteTable_AM(rkPacket, m_kGemStore);
	PU::TWriteTable_AM(rkPacket, m_kDefGemStore);	
	PU::TWriteTable_AM(rkPacket, m_kDefAntique);
	PU::TWriteTable_MM(rkPacket, m_kOXQuizInfo);
	PU::TWriteTable_MM(rkPacket, m_kOXQuizReward);
	PU::TWriteTable_MM(rkPacket, m_kOXQuizEventInfo);
	PU::TWriteTable_AA(rkPacket, m_kDefMonsterCard);
	PU::TWriteTable_AA(rkPacket, m_kDefMarryText);
	PU::TWriteTable_AA(rkPacket, m_kDefHiddenRewordItem);	
	PU::TWriteTable_AA(rkPacket, m_kDefHiddenRewordBag);
	PU::TWriteTable_AM(rkPacket, m_kDefMissionClassReward);
	PU::TWriteTable_AM(rkPacket, m_kDefMissionRankReward);	
	PU::TWriteTable_AM(rkPacket, m_kMissionDefenceStage);		
	PU::TWriteTable_AM(rkPacket, m_kMissionDefenceWave);		

	PU::TWriteTable_AM(rkPacket, m_kMissionDefence7Mission);
	PU::TWriteTable_AM(rkPacket, m_kMissionDefence7Stage);
	PU::TWriteTable_AM(rkPacket, m_kMissionDefence7Wave);
	PU::TWriteTable_AM(rkPacket, m_kMissionDefence7guardian);

	PU::TWriteTable_MM(rkPacket, m_kMissionBonusMap);	

	PU::TWriteTable_AA(rkPacket, m_kDefEmotion);
	PU::TWriteTable_AA(rkPacket, m_kDefEmotionGroup);
	PU::TWriteArray_A(rkPacket, m_kDefCashItemAbilFilter);
	PU::TWriteTable_AA(rkPacket, m_kDefGamble);
	PU::TWriteTable_AA(rkPacket, m_kContLimitedItem);
	PU::TWriteTable_AA(rkPacket, m_kContLimitedItemCtrl);
	PU::TWriteTable_AA(rkPacket, m_kPCRoom);
	PU::TWriteTable_AA(rkPacket, m_kPCCafe);
	PU::TWriteTable_AA(rkPacket, m_kPCCafeAbil);
	PU::TWriteTable_AA(rkPacket, m_kDefConvertItem);
	PU::TWriteTable_AA(rkPacket, m_kDefGambleShoutItem);
	PU::TWriteTable_AM(rkPacket, m_kDefPetHatch);
	PU::TWriteTable_AM(rkPacket, m_kDefPetBonusStatus);
	PU::TWriteTable_AM(rkPacket, m_kCCERewardTable);
	PU::TWriteTable_AM(rkPacket, m_kRealtyDealer);
	PU::TWriteTable_AA(rkPacket, m_kRareOptSkill);
	PU::TWriteTable_AA(rkPacket, m_kRareOptMagic);
	PU::TWriteTable_AA(rkPacket, m_kIdx2SkillNo);
	PU::TWriteTable_AA(rkPacket, m_kMixupItem);
	PU::TWriteTable_AA(rkPacket, m_kTown2Ground);
	PU::TWriteTable_AA(rkPacket, m_kMyHomeDefaultItem);
	PU::TWriteTable_AA(rkPacket, m_kMyhomeTex);
	PU::TWriteTable_AA(rkPacket, m_kHometownToMapCost);
	PU::TWriteTable_AA(rkPacket, m_DefShopNpcGuid);
	PU::TWriteTable_MM(rkPacket, m_kLuckyStarInfo);
	PU::TWriteTable_MM(rkPacket, m_kLuckyStarReward);
	PU::TWriteTable_MM(rkPacket, m_kLuckyStarRewardStep);	
	PU::TWriteTable_MM(rkPacket, m_kLuckyStarEventJoinReward);	
	PU::TWriteTable_AA(rkPacket, m_kDefItemEnchantAbilWeight);
	PU::TWriteTable_AM(rkPacket, m_kContEventItemReward);
	PU::TWriteTable_AM(rkPacket, m_kContTreasureChest);
	PU::TWriteTable_AM(rkPacket, m_DefItemByLevel);
	PU::TWriteTable_AM(rkPacket, m_kContDefCharCardEffect);
	PU::TWriteTable_AM(rkPacket, m_kContDefAlramMission);
	PU::TWriteTable_AA(rkPacket, m_kDefSideJobRate);
	PU::TWriteArray_M(rkPacket, m_kDefPartyEventItemSet);
	PU::TWriteTable_AA(rkPacket, m_kContDefRediceOptionCost);
	PU::TWriteTable_AA(rkPacket, m_kContDefMyHomeSidejobTime);
	PU::TWriteTable_MM(rkPacket, m_kContDefMonsterEnchantGrade);
	PU::TWriteTable_MM(rkPacket, m_kContDefMonsterGradeProbabilityGroup);
	PU::TWriteTable_MM(rkPacket, m_kContDefSuperGroundGroup);
	PU::TWriteTable_AA(rkPacket, m_kContDefMyhomeBuildings);
	PU::TWriteTable_AM(rkPacket, m_kContDefGroundBuildings);
	PU::TWriteTable_AA(rkPacket, m_kContDefBasicOptionAmp);
	PU::TWriteTable_AM(rkPacket, m_kContDefItemAmpSpecific);
	PU::TWriteTable_AA(rkPacket, m_kContDefDeathPenalty);
	PU::TWriteTable_AM(rkPacket, m_kContDefSkillExtendItem);
	PU::TWriteTable_AA(rkPacket, m_kContDefNpcTalkMapMove);
	PU::TWriteTable_AA(rkPacket, m_kContDefSpecificReward);
	PU::TWriteTable_AA(rkPacket, m_ContDefSpecificReward);
	PU::TWriteArray_M(rkPacket, m_kContDefPvPLeagueTime);
	PU::TWriteTable_AA(rkPacket, m_kContDefPvPLeagueSession);
	PU::TWriteTable_AA(rkPacket, m_kContDefPvPLeagueReward);
	PU::TWriteTable_AM(rkPacket, m_kContDefJobSkillProbabilityBag);
	PU::TWriteTable_AM(rkPacket, m_kContDefJobSkillItemUpgrade);
	PU::TWriteTable_AM(rkPacket, m_kContDefJobSkillSaveIdx);

	PU::TWriteTable_MM(rkPacket, m_kContDefJobSkillLocationItem);
	PU::TWriteTable_MM(rkPacket, m_kContDefJobSkillSkill);
	PU::TWriteArray_M(rkPacket, m_kContDefJobSkillExpertness);
	PU::TWriteTable_MM(rkPacket, m_kContDefJobSkillTool);
	PU::TWriteTable_AM(rkPacket, m_kContDefJobSkillShop);
	PU::TWriteTable_AM(rkPacket, m_kContDefSocketItem);	
	PU::TWriteTable_AM(rkPacket, m_kContDefJobSkillRecipe);	
	PU::TWriteTable_AM(rkPacket, m_kContDefJobSkillProbabilityBag);
	PU::TWriteArray_M(rkPacket, m_kContDefJobSkillSoulExtract);
	PU::TWriteArray_M(rkPacket, m_kContDefJobSkillSoulTrasition);
	PU::TWriteTable_MM(rkPacket, m_kContDefMissionMutator);
	PU::TWriteTable_MM(rkPacket, m_kContDefMissionMutatorAbil);
	PU::TWriteTable_AM(rkPacket, m_kContDefExpeditionNpc);

	PU::TWriteTable_AA(rkPacket, m_kContDefBSGenGround);

	PU::TWriteTable_AM(rkPacket, m_ContDefEventGroup);
	{
		rkPacket.Push(m_ContDefEventMonsterGroup.size());
		CONT_DEF_EVENT_MONSTER_GROUP::iterator iter = m_ContDefEventMonsterGroup.begin();
		for( ; iter != m_ContDefEventMonsterGroup.end() ; ++iter )
		{
			rkPacket.Push((*iter).first);

			PU::TWriteTable_AM(rkPacket, (*iter).second);
		}
	}
	PU::TWriteTable_AA(rkPacket, m_ContDefEventRewardItemGroup);
	
	PU::TWriteTable_AM(rkPacket, m_kContDefPremium);
	PU::TWriteTable_AA(rkPacket, m_kContDefPremiumType);


	PU::TWriteTable_AM(rkPacket, m_ContEventSchedule);
	PU::TWriteTable_AM(rkPacket, m_ContEvent_BossBattle);
	PU::TWriteTable_AM(rkPacket, m_ContEvent_Race);

	PU::TWriteTable_AM(rkPacket, m_kContDefQuestLevelReward);
	PU::TWriteArray_M(rkPacket, m_kContJobSkillEventLocation);
	PU::TWriteTable_MM(rkPacket, m_DefDailyReward);
	PU::TWriteTable_MM(rkPacket, m_DefCartMissionMonster);
	PU::TWriteTable_MM(rkPacket, m_kBattlePassQuest);

	DWORD const dwEndTime = BM::GetTime32();
	INFO_LOG(BM::LOG_LV9, __FL__<<L"End Elapsed["<<dwEndTime - dwBeginTime<<L"]");
//*/
}
