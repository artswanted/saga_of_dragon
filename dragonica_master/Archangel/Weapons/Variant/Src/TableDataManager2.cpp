#include "StdAfx.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"

#include "BM/FileSupport.h"

#include "Lohengrin/LogGroup.h"
#include "BM/HSEL.h"

bool CTableDataManager::Dump(std::wstring const &wstrFolder)
{//占쏙옙占싱억옙占쏙옙.
//
	INFO_LOG(BM::LOG_LV7, __FL__<<L"Begin ---");
	//BM::CAutoMutex Lock(GetLock()->kObjectLock);
	BM::CAutoMutex lock(m_kMutex);

{
	BM::Stream kPacket;
	PU::TWriteArray_A(kPacket, m_kContDefCharacterCreateSet);
	PU::TWriteTable_MM(kPacket,m_DefClass);
	PU::TWriteTable_MM(kPacket,m_DefClassAbil);
	PU::TWriteTable_MM(kPacket,m_DefClassPet);
	PU::TWriteTable_AM(kPacket,m_DefClassPetLevel);
	PU::TWriteTableArray_AMM(kPacket,m_DefClassPetSkill);
	PU::TWriteTable_AM(kPacket,m_DefClassPetItemOption);
	PU::TWriteTable_MM(kPacket,m_DefClassPetAbil);
	PU::TWriteTable_AM(kPacket,m_DefItemBag);
	PU::TWriteTable_MM(kPacket,m_DefAbilType);
	PU::TWriteTable_MM(kPacket,m_DefItem);
	PU::TWriteTable_MM(kPacket,m_DefItemAbil);
	PU::TWriteTable_MM(kPacket,m_DefItemRare);
	PU::TWriteTable_MM(kPacket,m_DefItemRareGroup);
	PU::TWriteTable_MM(kPacket,m_DefItemResConvert);
	PU::TWriteTable_MM(kPacket,m_DefMap);
	PU::TWriteTable_MM(kPacket,m_DefMapAbil);
	PU::TWriteTable_MM(kPacket,m_DefMapItem);
	PU::TWriteTable_MM(kPacket,m_DefMonster);
	PU::TWriteTable_MM(kPacket,m_DefMonsterAbil);
	PU::TWriteTable_MM(kPacket,m_DefMonsterTunningAbil);
	PU::TWriteTable_MM(kPacket,m_DefNPC);
	PU::TWriteTable_MM(kPacket,m_DefNPCAbil);
	PU::TWriteTable_MM(kPacket,m_DefSkill);
	PU::TWriteTable_MM(kPacket,m_DefSkillAbil);
	PU::TWriteTable_MM(kPacket,m_DefSkillSet);
	PU::TWriteTable_MM(kPacket,m_DefRes);
	PU::TWriteTable_MM(kPacket,m_DefEffect);
	PU::TWriteTable_MM(kPacket,m_DefEffectAbil);
	PU::TWriteTable_MM(kPacket,m_DefUpgradeClass);
	PU::TWriteTable_MM(kPacket,m_DefItemEnchant);
	PU::TWriteTable_MM(kPacket,m_DefCharacterBaseWear);
	PU::TWriteTable_MM(kPacket,m_DefItemPlusUpgrade);
	PU::TWriteTable_MM(kPacket,m_DefItemRarityUpgrade);	
	PU::TWriteTable_MM(kPacket,m_DefItemEnchantShift);
	PU::TWriteTable_MM(kPacket,m_DefSuccessRateControl);
	PU::TWriteTable_MM(kPacket,m_DefMonsterBag);
	PU::TWriteArray_M(kPacket,m_DefItemOption);
	PU::TWriteTable_MM(kPacket,m_DefItemOptionAbil);
	PU::TWriteTable_MM(kPacket,m_DefMonsterBagControl);
	PU::TWriteTable_MM(kPacket,m_DefMonsterBagElements);
	PU::TWriteTable_MM(kPacket,m_DefDropMoneyControl);
	PU::TWriteTable_MM(kPacket,m_DefItemBagElements);
	PU::TWriteTable_MM(kPacket,m_DefCountControl);
	PU::TWriteTable_AM(kPacket,m_DefPvPGroundGroup);
	PU::TWriteTable_MM(kPacket,m_DefPvPGroundMode);
	PU::TWriteTable_AA(kPacket,m_kDefPvPPlayTimeInfo);
	PU::TWriteTable_MM(kPacket,m_DefItemMaking);
	PU::TWriteTable_MM(kPacket,m_DefCooking);
	PU::TWriteTable_MM(kPacket,m_DefResultControl);
	PU::TWriteTable_MM(kPacket,m_DefQuestReward);
	PU::TWriteTable_MM(kPacket,m_DefMissionRoot);	
	PU::TWriteTable_MM(kPacket,m_DefMissionCandidate);
	PU::TWriteTable_MM(kPacket, m_DefDefenceAddMonster);
	PU::TWriteTable_MM(kPacket,m_DefMissionResult);
	PU::TWriteTable_MM(kPacket,m_DefItemSet);		
	PU::TWriteTable_MM(kPacket,m_DefSpendMoney);
	PU::TWriteTable_MM(kPacket,m_DefGuildLevel);
	PU::TWriteTable_MM(kPacket,m_DefGuildSkill);
	PU::TWriteTable_MM(kPacket,m_DefObject);	
	PU::TWriteTable_MM(kPacket,m_DefObjectAbil);
	PU::TWriteTable_MM(kPacket,m_kDefProperty);
	PU::TWriteArray_M(kPacket,m_DefFilterUnicode);
	PU::TWriteArray_M(kPacket,m_DefQuestResetSchedule);
	PU::TWriteTable_MM(kPacket,m_DefQuestRandomExp);
	PU::TWriteTable_MM(kPacket,m_DefQuestRandomTacticsExp);
	PU::TWriteTable_MM(kPacket,m_DefQuestWanted);
	PU::TWriteTable_AM(kPacket,m_kDefGemStore);
	PU::TWriteTable_AM(kPacket,m_kDefAntique);
	

	PU::TWriteTable_MM(kPacket,m_DefFiveElementInfo);
	PU::TWriteTable_MM(kPacket,m_DefItemRarityUpgradeCostRate);
	PU::TWriteTable_MM(kPacket,m_DefTacticsLevel);
	PU::TWriteTable_MM(kPacket,m_DefMonKillCountReward);
	PU::TWriteTable_AA(kPacket, m_DefTransTower);
	PU::TWriteTable_AA(kPacket, m_DefParty_Info);	
	PU::TWriteTable_AM(kPacket,m_kDefEmporia);
	PU::TWriteTable_MM(kPacket,m_kDefContAchievements);
	PU::TWriteTable_MM(kPacket,m_kDefAchievements);
	PU::TWriteTable_MM(kPacket,m_DefItemBagGroup);
	PU::TWriteTable_MM(kPacket,m_DefMapItemBag);	
	PU::TWriteTable_MM(kPacket,m_kDefRecommendationItem);
	PU::TWriteTable_MM(kPacket, m_DefItemContainer);
	PU::TWriteTable_MM(kPacket, m_kDefGroundRareMonster);
	PU::TWriteTable_MM(kPacket, m_kDefRareMonsterSpeech);
	PU::TWriteTable_MM(kPacket, m_kCardLocal);
	PU::TWriteTable_MM(kPacket, m_kDefCardKeyString);
	PU::TWriteTable_MM(kPacket, m_kCardAbil);
	PU::TWriteTable_AA(kPacket, m_kDefMonsterCard);
	PU::TWriteTable_AA(kPacket, m_kDefMarryText);
	PU::TWriteTable_AA(kPacket, m_kDefHiddenRewordItem);	
	PU::TWriteTable_AA(kPacket, m_kDefHiddenRewordBag);	
	PU::TWriteTable_AM(kPacket, m_kDefMissionClassReward);	
	PU::TWriteTable_AM(kPacket, m_kDefMissionRankReward);	
	PU::TWriteTable_AM(kPacket, m_kMissionDefenceStage);		
	PU::TWriteTable_AM(kPacket, m_kMissionDefenceWave);		

	PU::TWriteTable_AM(kPacket, m_kMissionDefence7Mission);
	PU::TWriteTable_AM(kPacket, m_kMissionDefence7Stage);
	PU::TWriteTable_AM(kPacket, m_kMissionDefence7Wave);
	PU::TWriteTable_AM(kPacket, m_kMissionDefence7guardian);

	PU::TWriteTable_MM(kPacket, m_kMissionBonusMap);

	PU::TWriteTable_AA(kPacket, m_kDefEmotion);
	PU::TWriteTable_AA(kPacket, m_kDefEmotionGroup);
	PU::TWriteTable_AA(kPacket, m_kDefConvertItem);
	PU::TWriteTable_AM(kPacket, m_kDefPetHatch);
	PU::TWriteTable_AA(kPacket, m_kIdx2SkillNo);
	PU::TWriteTable_AA(kPacket, m_kTown2Ground);
	PU::TWriteTable_AA(kPacket, m_kMyhomeTex);
	PU::TWriteTable_AA(kPacket, m_kHometownToMapCost);
	PU::TWriteTable_AA(kPacket, m_kDefItemEnchantAbilWeight);
	PU::TWriteTable_MM(kPacket, m_DefObjcetBag);
	PU::TWriteTable_MM(kPacket, m_DefObjcetBagElements);
	PU::TWriteTable_AA(kPacket, m_kDefSideJobRate);
	PU::TWriteArray_M(kPacket, m_kDefPartyEventItemSet);
	PU::TWriteTable_AA(kPacket, m_kContDefRediceOptionCost);
	PU::TWriteTable_AA(kPacket, m_kContDefMyHomeSidejobTime);
	PU::TWriteTable_MM(kPacket, m_kContDefMonsterEnchantGrade);
	PU::TWriteTable_AA(kPacket, m_kContDefMyhomeBuildings);
	PU::TWriteTable_AM(kPacket, m_kContDefGroundBuildings);
	PU::TWriteTable_AA(kPacket, m_kContDefBasicOptionAmp);
	PU::TWriteTable_AM(kPacket, m_kContDefItemAmpSpecific);
	PU::TWriteTable_AM(kPacket, m_kContDefAlramMission);
	PU::TWriteTable_AA(kPacket, m_kContDefDeathPenalty);
	PU::TWriteTable_AM(kPacket, m_kContDefSkillExtendItem);
	PU::TWriteArray_M(kPacket, m_kContDefPvPLeagueTime);
	PU::TWriteTable_AA(kPacket, m_kContDefPvPLeagueSession);
	PU::TWriteTable_AA(kPacket, m_kContDefPvPLeagueReward);
	PU::TWriteTable_MM(kPacket, m_kContDefJobSkillSkill);
	PU::TWriteArray_M(kPacket, m_kContDefJobSkillExpertness);
	PU::TWriteTable_AM(kPacket, m_kContDefJobSkillSaveIdx);
	PU::TWriteTable_AM(kPacket, m_kContDefJobSkillItemUpgrade);
	PU::TWriteTable_MM(kPacket, m_kContDefJobSkillTool);
	PU::TWriteTable_AM(kPacket, m_kContDefSocketItem);	
	PU::TWriteTable_AM(kPacket, m_kContDefJobSkillRecipe);	
	PU::TWriteTable_AM(kPacket, m_kContDefJobSkillProbabilityBag);
	PU::TWriteArray_M(kPacket, m_kContDefJobSkillSoulExtract);
	PU::TWriteArray_M(kPacket, m_kContDefJobSkillSoulTrasition);
	PU::TWriteTable_MM(kPacket, m_kContDefMissionMutator);
	PU::TWriteTable_MM(kPacket, m_kContDefMissionMutatorAbil);
	PU::TWriteTable_AM(kPacket, m_kContDefExpeditionNpc);
	PU::TWriteTable_AM(kPacket, m_kContDefPremium);
	PU::TWriteTable_AA(kPacket, m_kContDefPremiumType);
	PU::TWriteTable_AM(kPacket, m_kContDefJumpingCharEvent);
	PU::TWriteTableArray_AMM(kPacket, m_kContDefJumpingCharEventReward);
	PU::TWriteTable_AM(kPacket, m_ContEventSchedule);
	PU::TWriteTable_AM(kPacket, m_ContEvent_BossBattle);
	PU::TWriteTable_AM(kPacket, m_ContEvent_Race);
	//PU::TWriteTable_AM(kPacket, m_kContDefJumpingCharEventItem);
	PU::TWriteTable_AM(kPacket, m_kContDefQuestLevelReward);
	PU::TWriteArray_M(kPacket, m_kContJobSkillEventLocation);
	PU::TWriteTable_MM(kPacket, m_DefDailyReward);
	PU::TWriteTable_MM(kPacket, m_DefCartMissionMonster);
	PU::TWriteTable_MM(kPacket, m_kBattlePassQuest);

	::_wmkdir( wstrFolder.c_str() );
	std::wstring const wstrFileName = wstrFolder + L"TB.BIN";
	if(!BM::EncSave( wstrFileName, kPacket.Data()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

{
	BM::Stream kPacket;
	PU::TWriteTable_MM(kPacket,m_DefMapRegenPoint);

	std::wstring const wstrFileName = wstrFolder + L"TB2.BIN";
	if(!BM::EncSave(wstrFileName, kPacket.Data()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

	INFO_LOG(BM::LOG_LV7, __FL__<<L"Ended ---");
	return true;
}

bool CTableDataManager::LoadDump(std::wstring const &strFolder)
{//4占쌨곤옙
	//BM::CAutoMutex Lock(GetLock()->kObjectLock);
	BM::CAutoMutex lock(m_kMutex, true);
{
	BM::vstring vstrFileName(strFolder + _T("TB"));//TB 占쏙옙 占쏙옙占싹몌옙
	vstrFileName += _T(".BIN");

	BM::Stream kPacket;
	if(!BM::DecLoad(vstrFileName, kPacket.Data()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	kPacket.PosAdjust();
	PU::TLoadArray_A(kPacket, m_kContDefCharacterCreateSet);
	PU::TLoadTable_MM(kPacket, m_DefClass);
	PU::TLoadTable_MM(kPacket, m_DefClassAbil);
	PU::TLoadTable_MM(kPacket, m_DefClassPet);
	PU::TLoadTable_AM(kPacket, m_DefClassPetLevel);
	PU::TLoadTableArray_AMM(kPacket, m_DefClassPetSkill);
	PU::TLoadTable_AM(kPacket, m_DefClassPetItemOption);
	PU::TLoadTable_MM(kPacket, m_DefClassPetAbil);
	PU::TLoadTable_AM(kPacket, m_DefItemBag);
	PU::TLoadTable_MM(kPacket, m_DefAbilType);
	PU::TLoadTable_MM(kPacket, m_DefItem);
	PU::TLoadTable_MM(kPacket, m_DefItemAbil);
	PU::TLoadTable_MM(kPacket, m_DefItemRare);
	PU::TLoadTable_MM(kPacket, m_DefItemRareGroup);
	PU::TLoadTable_MM(kPacket, m_DefItemResConvert);
	PU::TLoadTable_MM(kPacket, m_DefMap);
	PU::TLoadTable_MM(kPacket, m_DefMapAbil);
	PU::TLoadTable_MM(kPacket, m_DefMapItem);
	PU::TLoadTable_MM(kPacket, m_DefMonster);
	PU::TLoadTable_MM(kPacket, m_DefMonsterAbil);
	PU::TLoadTable_MM(kPacket, m_DefMonsterTunningAbil);
	PU::TLoadTable_MM(kPacket, m_DefNPC);
	PU::TLoadTable_MM(kPacket, m_DefNPCAbil);
	PU::TLoadTable_MM(kPacket, m_DefSkill);
	PU::TLoadTable_MM(kPacket, m_DefSkillAbil);	
	PU::TLoadTable_MM(kPacket, m_DefSkillSet);
	PU::TLoadTable_MM(kPacket, m_DefRes);
	PU::TLoadTable_MM(kPacket, m_DefEffect);
	PU::TLoadTable_MM(kPacket, m_DefEffectAbil);
	PU::TLoadTable_MM(kPacket, m_DefUpgradeClass);
	PU::TLoadTable_MM(kPacket, m_DefItemEnchant);
	PU::TLoadTable_MM(kPacket, m_DefCharacterBaseWear);
	PU::TLoadTable_MM(kPacket, m_DefItemPlusUpgrade);
	PU::TLoadTable_MM(kPacket, m_DefItemRarityUpgrade);
	PU::TLoadTable_MM(kPacket, m_DefItemEnchantShift);
	PU::TLoadTable_MM(kPacket, m_DefSuccessRateControl);
	PU::TLoadTable_MM(kPacket, m_DefMonsterBag);
	PU::TLoadArray_M(kPacket, m_DefItemOption);
	PU::TLoadTable_MM(kPacket, m_DefItemOptionAbil);
	PU::TLoadTable_MM(kPacket, m_DefMonsterBagControl);
	PU::TLoadTable_MM(kPacket, m_DefMonsterBagElements);
	PU::TLoadTable_MM(kPacket, m_DefDropMoneyControl);
	PU::TLoadTable_MM(kPacket, m_DefItemBagElements);
	PU::TLoadTable_MM(kPacket, m_DefCountControl);
	PU::TLoadTable_AM(kPacket, m_DefPvPGroundGroup);
	PU::TLoadTable_MM(kPacket, m_DefPvPGroundMode);
	PU::TLoadTable_AA(kPacket, m_kDefPvPPlayTimeInfo);
	PU::TLoadTable_MM(kPacket, m_DefItemMaking);
	PU::TLoadTable_MM(kPacket, m_DefCooking);
	PU::TLoadTable_MM(kPacket, m_DefResultControl);
	PU::TLoadTable_MM(kPacket, m_DefQuestReward);
	PU::TLoadTable_MM(kPacket, m_DefMissionRoot);
	PU::TLoadTable_MM(kPacket, m_DefMissionCandidate);
	PU::TLoadTable_MM(kPacket, m_DefDefenceAddMonster);
	PU::TLoadTable_MM(kPacket, m_DefMissionResult);
	PU::TLoadTable_MM(kPacket, m_DefItemSet);
	PU::TLoadTable_MM(kPacket, m_DefSpendMoney);
	PU::TLoadTable_MM(kPacket, m_DefGuildLevel);
	PU::TLoadTable_MM(kPacket, m_DefGuildSkill);
	PU::TLoadTable_MM(kPacket, m_DefObject);
	PU::TLoadTable_MM(kPacket, m_DefObjectAbil);	
	PU::TLoadTable_MM(kPacket, m_kDefProperty);
	PU::TLoadArray_M(kPacket, m_DefFilterUnicode);
	PU::TLoadArray_M(kPacket, m_DefQuestResetSchedule);
	PU::TLoadTable_MM(kPacket, m_DefQuestRandomExp);
	PU::TLoadTable_MM(kPacket, m_DefQuestRandomTacticsExp);
	PU::TLoadTable_MM(kPacket, m_DefQuestWanted);
	PU::TLoadTable_AM(kPacket, m_kDefGemStore);
	PU::TLoadTable_AM(kPacket, m_kDefAntique);

	PU::TLoadTable_MM(kPacket, m_DefFiveElementInfo);
	PU::TLoadTable_MM(kPacket, m_DefItemRarityUpgradeCostRate);
	PU::TLoadTable_MM(kPacket, m_DefTacticsLevel);
	PU::TLoadTable_MM(kPacket, m_DefMonKillCountReward);
	PU::TLoadTable_AA(kPacket, m_DefTransTower);
	PU::TLoadTable_AA(kPacket, m_DefParty_Info);	
	PU::TLoadTable_AM(kPacket, m_kDefEmporia);
	PU::TLoadTable_MM(kPacket, m_kDefContAchievements);
	PU::TLoadTable_MM(kPacket, m_kDefAchievements);
	PU::TLoadTable_MM(kPacket, m_DefItemBagGroup);
	PU::TLoadTable_MM(kPacket, m_DefMapItemBag);	
	PU::TLoadTable_MM(kPacket, m_kDefRecommendationItem);
	PU::TLoadTable_MM(kPacket, m_DefItemContainer);
	PU::TLoadTable_MM(kPacket, m_kDefGroundRareMonster);
	PU::TLoadTable_MM(kPacket, m_kDefRareMonsterSpeech);
	PU::TLoadTable_MM(kPacket, m_kCardLocal);
	PU::TLoadTable_MM(kPacket, m_kDefCardKeyString);
	PU::TLoadTable_MM(kPacket, m_kCardAbil);
	PU::TLoadTable_AA(kPacket, m_kDefMonsterCard);
	PU::TLoadTable_AA(kPacket, m_kDefMarryText);
	PU::TLoadTable_AA(kPacket, m_kDefHiddenRewordItem);	
	PU::TLoadTable_AA(kPacket, m_kDefHiddenRewordBag);
	PU::TLoadTable_AM(kPacket, m_kDefMissionClassReward);
	PU::TLoadTable_AM(kPacket, m_kDefMissionRankReward);
	PU::TLoadTable_AM(kPacket, m_kMissionDefenceStage);
	PU::TLoadTable_AM(kPacket, m_kMissionDefenceWave);	

	PU::TLoadTable_AM(kPacket, m_kMissionDefence7Mission);
	PU::TLoadTable_AM(kPacket, m_kMissionDefence7Stage);
	PU::TLoadTable_AM(kPacket, m_kMissionDefence7Wave);
	PU::TLoadTable_AM(kPacket, m_kMissionDefence7guardian);

	PU::TLoadTable_MM(kPacket, m_kMissionBonusMap);

	PU::TLoadTable_AA(kPacket, m_kDefEmotion);
	PU::TLoadTable_AA(kPacket, m_kDefEmotionGroup);
	PU::TLoadTable_AA(kPacket, m_kDefConvertItem);
	PU::TLoadTable_AM(kPacket, m_kDefPetHatch);
	PU::TLoadTable_AA(kPacket, m_kIdx2SkillNo);
	PU::TLoadTable_AA(kPacket, m_kTown2Ground);
	PU::TLoadTable_AA(kPacket, m_kMyhomeTex);
	PU::TLoadTable_AA(kPacket, m_kHometownToMapCost);
	PU::TLoadTable_AA(kPacket, m_kDefItemEnchantAbilWeight);
	PU::TLoadTable_MM(kPacket, m_DefObjcetBag);
	PU::TLoadTable_MM(kPacket, m_DefObjcetBagElements);
	PU::TLoadTable_AA(kPacket, m_kDefSideJobRate);
	PU::TLoadArray_M(kPacket, m_kDefPartyEventItemSet);
	PU::TLoadTable_AA(kPacket, m_kContDefRediceOptionCost);
	PU::TLoadTable_AA(kPacket, m_kContDefMyHomeSidejobTime);
	PU::TLoadTable_MM(kPacket, m_kContDefMonsterEnchantGrade);
	PU::TLoadTable_AA(kPacket, m_kContDefMyhomeBuildings);
	PU::TLoadTable_AM(kPacket, m_kContDefGroundBuildings);
	PU::TLoadTable_AA(kPacket, m_kContDefBasicOptionAmp);
	PU::TLoadTable_AM(kPacket, m_kContDefItemAmpSpecific);
	PU::TLoadTable_AM(kPacket, m_kContDefAlramMission);
	PU::TLoadTable_AA(kPacket, m_kContDefDeathPenalty);
	PU::TLoadTable_AM(kPacket, m_kContDefSkillExtendItem);
	PU::TLoadArray_M(kPacket, m_kContDefPvPLeagueTime);
	PU::TLoadTable_AA(kPacket, m_kContDefPvPLeagueSession);
	PU::TLoadTable_AA(kPacket, m_kContDefPvPLeagueReward);
	PU::TLoadTable_MM(kPacket, m_kContDefJobSkillSkill);
	PU::TLoadArray_M(kPacket, m_kContDefJobSkillExpertness);
	PU::TLoadTable_AM(kPacket, m_kContDefJobSkillSaveIdx);
	PU::TLoadTable_AM(kPacket, m_kContDefJobSkillItemUpgrade);
	PU::TLoadTable_MM(kPacket, m_kContDefJobSkillTool);
	PU::TLoadTable_AM(kPacket, m_kContDefSocketItem);	
	PU::TLoadTable_AM(kPacket, m_kContDefJobSkillRecipe);
	PU::TLoadTable_AM(kPacket, m_kContDefJobSkillProbabilityBag);
	PU::TLoadArray_M(kPacket, m_kContDefJobSkillSoulExtract);
	PU::TLoadArray_M(kPacket, m_kContDefJobSkillSoulTrasition);
	PU::TLoadTable_MM(kPacket, m_kContDefMissionMutator);
	PU::TLoadTable_MM(kPacket, m_kContDefMissionMutatorAbil);
	PU::TLoadTable_AM(kPacket, m_kContDefExpeditionNpc);
	PU::TLoadTable_AM(kPacket, m_kContDefPremium);
	PU::TLoadTable_AA(kPacket, m_kContDefPremiumType);
	PU::TLoadTable_AM(kPacket, m_kContDefJumpingCharEvent);
	PU::TLoadTableArray_AMM(kPacket, m_kContDefJumpingCharEventReward);
	PU::TLoadTable_AM(kPacket, m_ContEventSchedule);
	PU::TLoadTable_AM(kPacket, m_ContEvent_BossBattle);
	PU::TLoadTable_AM(kPacket, m_ContEvent_Race);
	//PU::TLoadTable_AM(kPacket, m_kContDefJumpingCharEventItem);
	PU::TLoadTable_AM(kPacket, m_kContDefQuestLevelReward);
	PU::TLoadArray_M(kPacket, m_kContJobSkillEventLocation);
	PU::TLoadTable_MM(kPacket, m_DefDailyReward);
	PU::TLoadTable_MM(kPacket, m_DefCartMissionMonster);
	PU::TLoadTable_MM(kPacket, m_kBattlePassQuest);
}

{
	BM::vstring vstrFileName(strFolder + _T("TB2"));//TB 占쏙옙 占쏙옙占싹몌옙
	vstrFileName += _T(".BIN");

	BM::Stream kPacket;
	if(!BM::DecLoad(vstrFileName, kPacket.Data())){return false;}
	kPacket.PosAdjust();

	PU::TLoadTable_MM(kPacket, m_DefMapRegenPoint);
}

{
	m_kDefAchievements_SaveIdx.clear();
	m_DefItem2Achievement.clear();
	CONT_DEF_ITEM2ACHIEVEMENT::mapped_type kItem;
	for(CONT_DEF_ACHIEVEMENTS::const_iterator c_iter=m_kDefAchievements.begin();c_iter!=m_kDefAchievements.end();++c_iter)
	{
		kItem.iItemNo = c_iter->second.iItemNo;
		kItem.iSaveIdx = c_iter->second.iSaveIdx;
		m_kDefAchievements_SaveIdx.insert(std::make_pair(c_iter->second.iSaveIdx,c_iter->second));
		m_DefItem2Achievement.insert(std::make_pair(kItem.iItemNo,kItem));
	}
}
	return true;
}
