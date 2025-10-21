#include "StdAfx.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"

#include "BM/FileSupport.h"

#include "Lohengrin/LogGroup.h"
#include "BM/HSEL.h"

void CTableDataManager::PacketToData(BM::Stream &rkPacket, bool bReload)
{//占싱곤옙 3占쌨곤옙?
//	/*	
	//BM::CAutoMutex Lock(GetLock()->kObjectLock);
	BM::CAutoMutex lock(m_kMutex, true);
	Clear(bReload);

	PU::TLoadArray_A(rkPacket, m_kContDefCharacterCreateSet);
	PU::TLoadTable_MM(rkPacket, m_DefClass);
	PU::TLoadTable_MM(rkPacket, m_DefClassAbil);
	PU::TLoadTable_MM(rkPacket, m_DefClassPet);
	PU::TLoadTable_AM(rkPacket, m_DefClassPetLevel);
	PU::TLoadTableArray_AMM(rkPacket, m_DefClassPetSkill);
	PU::TLoadTable_AM(rkPacket, m_DefClassPetItemOption);
	PU::TLoadTable_MM(rkPacket, m_DefClassPetAbil);
	PU::TLoadTable_AM(rkPacket, m_DefItemBag);
	PU::TLoadTable_MM(rkPacket, m_DefAbilType);
	PU::TLoadTable_MM(rkPacket, m_DefBaseCharacter);
	PU::TLoadTable_MM(rkPacket, m_DefItem);
	PU::TLoadTable_MM(rkPacket, m_DefItemAbil);
	PU::TLoadTable_MM(rkPacket, m_DefItemRare);
	PU::TLoadTable_MM(rkPacket, m_DefItemRareGroup);
	PU::TLoadTable_MM(rkPacket, m_DefItemResConvert);
	PU::TLoadTable_MM(rkPacket, m_DefMap);
	PU::TLoadTable_MM(rkPacket, m_DefMapAbil);
	PU::TLoadTable_MM(rkPacket, m_DefMapItem);
	PU::TLoadTable_MM(rkPacket, m_DefMapRegenPoint);
	PU::TLoadTable_MM(rkPacket, m_DefMonster);
	PU::TLoadTable_MM(rkPacket, m_DefMonsterAbil);
	PU::TLoadTable_MM(rkPacket, m_DefMonsterTunningAbil);
	PU::TLoadTable_MM(rkPacket, m_DefNPC);
	PU::TLoadTable_MM(rkPacket, m_DefNPCAbil);
	PU::TLoadTable_MM(rkPacket, m_DefSkill);
	PU::TLoadTable_MM(rkPacket, m_DefSkillAbil);
	PU::TLoadTable_MM(rkPacket, m_DefSkillSet);
	PU::TLoadTable_MM(rkPacket, m_DefRes);
	PU::TLoadArray_A(rkPacket, m_DefChannelEffect);
	PU::TLoadTable_MM(rkPacket, m_DefStrings);
	PU::TLoadTable_MM(rkPacket, m_DefEffect);
	PU::TLoadTable_MM(rkPacket, m_DefEffectAbil);
	PU::TLoadTable_MM(rkPacket, m_DefUpgradeClass);
	PU::TLoadTable_MM(rkPacket, m_DefItemEnchant);
	PU::TLoadTable_MM(rkPacket, m_DefCharacterBaseWear);
	PU::TLoadTable_MM(rkPacket, m_DefItemPlusUpgrade);
	PU::TLoadTable_MM(rkPacket, m_DefItemRarityUpgrade);
	PU::TLoadTable_MM(rkPacket, m_DefItemRarityControl);
	PU::TLoadTable_MM(rkPacket, m_DefItemPlusUpControl);
	PU::TLoadTable_MM(rkPacket, m_DefItemEnchantShift);
	PU::TLoadTable_MM(rkPacket, m_DefItemDisassemble);
	PU::TLoadTable_MM(rkPacket, m_DefSuccessRateControl);
	PU::TLoadTable_MM(rkPacket, m_DefMonsterBag);
	PU::TLoadTable_MM(rkPacket, m_kDefGmCmd);
	PU::TLoadTable_MM(rkPacket, m_DefCountControl);
	PU::TLoadTable_MM(rkPacket, m_DefItemBagElements);
	PU::TLoadTable_MM(rkPacket, m_DefMonsterBagElements);
	PU::TLoadTable_MM(rkPacket, m_DefMonsterBagControl);
	PU::TLoadTable_MM(rkPacket, m_DefItemContainer);
	PU::TLoadTable_MM(rkPacket, m_DefObject);
	PU::TLoadTable_MM(rkPacket, m_DefObjectAbil);
	PU::TLoadTable_MM(rkPacket, m_DefObjcetBag);
	PU::TLoadTable_MM(rkPacket, m_DefObjcetBagElements);
	PU::TLoadTable_MM(rkPacket, m_DefItemBagGroup);
	PU::TLoadTable_MM(rkPacket, m_DefMapItemBag);
	PU::TLoadTable_MM(rkPacket, m_DefMissionResult);
	PU::TLoadTable_MM(rkPacket, m_DefMissionRoot);
	PU::TLoadTable_MM(rkPacket, m_DefMissionCandidate);
	PU::TLoadTable_MM(rkPacket, m_DefDefenceAddMonster);
    PU::TLoadTable_MM(rkPacket, m_DefShopInEmporia);
	PU::TLoadTable_MM(rkPacket, m_DefShopInGame);
	PU::TLoadTable_MM(rkPacket, m_DefShopInStock);
	PU::TLoadArray_M(rkPacket, m_DefItemOption);
	PU::TLoadTable_MM(rkPacket, m_DefItemOptionAbil);
	PU::TLoadTable_MM(rkPacket, m_DefDropMoneyControl);
	PU::TLoadTable_AM(rkPacket, m_DefPvPGroundGroup);
	PU::TLoadTable_MM(rkPacket, m_DefPvPGroundMode);
	PU::TLoadArray_M(rkPacket, m_kDefPvPReward);
	PU::TLoadTable_AM(rkPacket, m_kDefPvPRewardItem);
	PU::TLoadTable_AA(rkPacket, m_kDefPvPPlayTimeInfo);
	PU::TLoadTable_MM(rkPacket, m_DefItemMaking);
	PU::TLoadTable_MM(rkPacket, m_DefCooking);
	PU::TLoadTable_MM(rkPacket, m_DefResultControl);
	PU::TLoadTable_MM(rkPacket, m_DefQuestReward);
	PU::TLoadArray_M(rkPacket, m_DefQuestResetSchedule);
	PU::TLoadArray_M(rkPacket, m_DefQuestRandom);
	PU::TLoadTable_MM(rkPacket, m_DefQuestRandomExp);
	PU::TLoadTable_MM(rkPacket, m_DefQuestRandomTacticsExp);
	PU::TLoadTable_MM(rkPacket, m_DefQuestWanted);
	PU::TLoadTable_MM(rkPacket, m_DefDynamicAbilRate);
	PU::TLoadTable_MM(rkPacket, m_DefDynamicAbilRateBag);
	PU::TLoadTable_MM(rkPacket, m_DefItemSet);
	PU::TLoadTable_MM(rkPacket, m_DefSpendMoney);
	PU::TLoadTable_MM(rkPacket, m_DefGuildLevel);
	PU::TLoadTable_MM(rkPacket, m_DefGuildSkill);
	PU::TLoadArray_M(rkPacket, m_DefFilterUnicode);
	PU::TLoadTable_MM(rkPacket, m_kDefProperty);
	PU::TLoadArray_M(rkPacket, m_DefMapEffect);
	PU::TLoadArray_M(rkPacket, m_DefMapEntity);
	PU::TLoadTable_MM(rkPacket, m_DefFiveElementInfo);
	PU::TLoadTable_MM(rkPacket, m_DefMapStoneControl);
	PU::TLoadTable_MM(rkPacket, m_DefItemRarityUpgradeCostRate);
	PU::TLoadTable_MM(rkPacket, m_DefTacticsLevel);
	PU::TLoadTable_MM(rkPacket, m_DefTacticsQuestPseudo);
	PU::TLoadTable_MM(rkPacket, m_DefMonKillCountReward);
	PU::TLoadTable_AA(rkPacket, m_DefTransTower);
	PU::TLoadTable_AA(rkPacket, m_DefParty_Info);	
	PU::TLoadTable_MM(rkPacket, m_DefCashShop);
	PU::TLoadTable_AM(rkPacket, m_kDefEmporia);
	PU::TLoadTable_MM(rkPacket, m_DefCashShopArticle);
	PU::TLoadTable_MM(rkPacket, m_DefCashShopLimitSell);	
	PU::TLoadTable_MM(rkPacket, m_kDefContAchievements);
	PU::TLoadTable_MM(rkPacket, m_kDefAchievements);
	PU::TLoadTable_MM(rkPacket, m_kDefAchievements_SaveIdx);
	PU::TLoadTable_MM(rkPacket, m_kDefRecommendationItem);
	PU::TLoadTable_MM(rkPacket, m_DefItem2Achievement);
	PU::TLoadTable_MM(rkPacket, m_kDefGroundRareMonster);
	PU::TLoadTable_MM(rkPacket, m_kDefRareMonsterSpeech);
	PU::TLoadTable_MM(rkPacket, m_kCardLocal);
	PU::TLoadTable_MM(rkPacket, m_kDefCardKeyString);
	PU::TLoadTable_MM(rkPacket, m_kCardAbil);
	PU::TLoadTable_AM(rkPacket, m_kGemStore);
	PU::TLoadTable_AM(rkPacket, m_kDefGemStore);	
	PU::TLoadTable_AM(rkPacket, m_kDefAntique);
	PU::TLoadTable_MM(rkPacket, m_kOXQuizInfo);
	PU::TLoadTable_MM(rkPacket, m_kOXQuizReward);
	PU::TLoadTable_MM(rkPacket, m_kOXQuizEventInfo);
	PU::TLoadTable_AA(rkPacket, m_kDefMonsterCard);
	PU::TLoadTable_AA(rkPacket, m_kDefMarryText);
	PU::TLoadTable_AA(rkPacket, m_kDefHiddenRewordItem);	
	PU::TLoadTable_AA(rkPacket, m_kDefHiddenRewordBag);		
	PU::TLoadTable_AM(rkPacket, m_kDefMissionClassReward);
	PU::TLoadTable_AM(rkPacket, m_kDefMissionRankReward);
	PU::TLoadTable_AM(rkPacket, m_kMissionDefenceStage);	
	PU::TLoadTable_AM(rkPacket, m_kMissionDefenceWave);	

	PU::TLoadTable_AM(rkPacket, m_kMissionDefence7Mission);
	PU::TLoadTable_AM(rkPacket, m_kMissionDefence7Stage);
	PU::TLoadTable_AM(rkPacket, m_kMissionDefence7Wave);
	PU::TLoadTable_AM(rkPacket, m_kMissionDefence7guardian);

	PU::TLoadTable_MM(rkPacket, m_kMissionBonusMap);

	PU::TLoadTable_AA(rkPacket, m_kDefEmotion);
	PU::TLoadTable_AA(rkPacket, m_kDefEmotionGroup);
	PU::TLoadArray_A(rkPacket, m_kDefCashItemAbilFilter);
	PU::TLoadTable_AA(rkPacket, m_kDefGamble);
	PU::TLoadTable_AA(rkPacket, m_kContLimitedItem);
	PU::TLoadTable_AA(rkPacket, m_kContLimitedItemCtrl);
	PU::TLoadTable_AA(rkPacket, m_kPCRoom);
	PU::TLoadTable_AA(rkPacket, m_kPCCafe);
	PU::TLoadTable_AA(rkPacket, m_kPCCafeAbil);
	PU::TLoadTable_AA(rkPacket, m_kDefConvertItem);
	PU::TLoadTable_AA(rkPacket, m_kDefGambleShoutItem);
	PU::TLoadTable_AM(rkPacket, m_kDefPetHatch);
	PU::TLoadTable_AM(rkPacket, m_kDefPetBonusStatus);
	PU::TLoadTable_AM(rkPacket, m_kCCERewardTable);
	PU::TLoadTable_AM(rkPacket, m_kRealtyDealer);
	PU::TLoadTable_AA(rkPacket, m_kRareOptSkill);
	PU::TLoadTable_AA(rkPacket, m_kRareOptMagic);
	PU::TLoadTable_AA(rkPacket, m_kIdx2SkillNo);
	PU::TLoadTable_AA(rkPacket, m_kMixupItem);
	PU::TLoadTable_AA(rkPacket, m_kTown2Ground);
	PU::TLoadTable_AA(rkPacket, m_kMyHomeDefaultItem);
	PU::TLoadTable_AA(rkPacket, m_kMyhomeTex);
	PU::TLoadTable_AA(rkPacket, m_kHometownToMapCost);
	PU::TLoadTable_AA(rkPacket, m_DefShopNpcGuid);
	PU::TLoadTable_MM(rkPacket, m_kLuckyStarInfo);
	PU::TLoadTable_MM(rkPacket, m_kLuckyStarReward);
	PU::TLoadTable_MM(rkPacket, m_kLuckyStarRewardStep);
	PU::TLoadTable_MM(rkPacket, m_kLuckyStarEventJoinReward);
	PU::TLoadTable_AA(rkPacket, m_kDefItemEnchantAbilWeight);
	PU::TLoadTable_AM(rkPacket, m_kContEventItemReward);
	PU::TLoadTable_AM(rkPacket, m_kContTreasureChest);
	PU::TLoadTable_AM(rkPacket, m_DefItemByLevel);
	PU::TLoadTable_AM(rkPacket, m_kContDefCharCardEffect);
	PU::TLoadTable_AM(rkPacket, m_kContDefAlramMission);
	PU::TLoadTable_AA(rkPacket, m_kDefSideJobRate);
	PU::TLoadArray_M(rkPacket, m_kDefPartyEventItemSet);
	PU::TLoadTable_AA(rkPacket, m_kContDefRediceOptionCost);
	PU::TLoadTable_AA(rkPacket, m_kContDefMyHomeSidejobTime);
	PU::TLoadTable_MM(rkPacket, m_kContDefMonsterEnchantGrade);
	PU::TLoadTable_MM(rkPacket, m_kContDefMonsterGradeProbabilityGroup);
	PU::TLoadTable_MM(rkPacket, m_kContDefSuperGroundGroup);
	PU::TLoadTable_AA(rkPacket, m_kContDefMyhomeBuildings);
	PU::TLoadTable_AM(rkPacket, m_kContDefGroundBuildings);
	PU::TLoadTable_AA(rkPacket, m_kContDefBasicOptionAmp);
	PU::TLoadTable_AM(rkPacket, m_kContDefItemAmpSpecific);
	PU::TLoadTable_AA(rkPacket, m_kContDefDeathPenalty);
	PU::TLoadTable_AM(rkPacket, m_kContDefSkillExtendItem);
	PU::TLoadTable_AA(rkPacket, m_kContDefNpcTalkMapMove);
	PU::TLoadTable_AA(rkPacket, m_kContDefSpecificReward);
	PU::TLoadTable_AA(rkPacket, m_ContDefSpecificReward);
	PU::TLoadArray_M(rkPacket, m_kContDefPvPLeagueTime);
	PU::TLoadTable_AA(rkPacket, m_kContDefPvPLeagueSession);
	PU::TLoadTable_AA(rkPacket, m_kContDefPvPLeagueReward);
	PU::TLoadTable_AM(rkPacket, m_kContDefJobSkillProbabilityBag);
	PU::TLoadTable_AM(rkPacket, m_kContDefJobSkillItemUpgrade);
	PU::TLoadTable_AM(rkPacket, m_kContDefJobSkillSaveIdx);

	PU::TLoadTable_MM(rkPacket, m_kContDefJobSkillLocationItem);	
	PU::TLoadTable_MM(rkPacket, m_kContDefJobSkillSkill);	
	PU::TLoadArray_M(rkPacket, m_kContDefJobSkillExpertness);
	PU::TLoadTable_MM(rkPacket, m_kContDefJobSkillTool);
	PU::TLoadTable_AM(rkPacket, m_kContDefJobSkillShop);
	PU::TLoadTable_AM(rkPacket, m_kContDefSocketItem);	
	PU::TLoadTable_AM(rkPacket, m_kContDefJobSkillRecipe);	
	PU::TLoadTable_AM(rkPacket, m_kContDefJobSkillProbabilityBag);
	PU::TLoadArray_M(rkPacket, m_kContDefJobSkillSoulExtract);
	PU::TLoadArray_M(rkPacket, m_kContDefJobSkillSoulTrasition);
	PU::TLoadTable_MM(rkPacket, m_kContDefMissionMutator);
	PU::TLoadTable_MM(rkPacket, m_kContDefMissionMutatorAbil);
	PU::TLoadTable_AM(rkPacket, m_kContDefExpeditionNpc);

	PU::TLoadTable_AA(rkPacket, m_kContDefBSGenGround);

	PU::TLoadTable_AM(rkPacket, m_ContDefEventGroup);
	{
		int count = 0;
		rkPacket.Pop(count);
		CONT_DEF_EVENT_MONSTER_GROUP::key_type first;
		CONT_DEF_EVENT_MONSTER_GROUP::mapped_type second;
		for(int i = 0 ; i < count ; ++i)
		{
			rkPacket.Pop(first);
			PU::TLoadTable_AM(rkPacket, second);

			m_ContDefEventMonsterGroup.insert( std::make_pair( first, second ) );
			second.clear();
		}
	}
	PU::TLoadTable_AA(rkPacket, m_ContDefEventRewardItemGroup);


	PU::TLoadTable_AM(rkPacket, m_kContDefPremium);
	PU::TLoadTable_AA(rkPacket, m_kContDefPremiumType);
	PU::TLoadTable_AM(rkPacket, m_ContEventSchedule);
	PU::TLoadTable_AM(rkPacket, m_ContEvent_BossBattle);
	PU::TLoadTable_AM(rkPacket, m_ContEvent_Race);
	
	PU::TLoadTable_AM(rkPacket, m_kContDefQuestLevelReward);
	PU::TLoadArray_M(rkPacket, m_kContJobSkillEventLocation);
	PU::TLoadTable_MM(rkPacket, m_DefDailyReward);
	PU::TLoadTable_MM(rkPacket, m_DefCartMissionMonster);
	PU::TLoadTable_MM(rkPacket, m_kBattlePassQuest);
	IsInit(true);
//*/
}
