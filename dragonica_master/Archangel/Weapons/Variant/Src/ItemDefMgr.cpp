#include "StdAfx.h"
#include "Lohengrin/LockUtil.h"
#include "Constant.h"
#include "TableDataManager.h"
#include "ItemDefMgr.h"
#include "DefAbilType.h"
#include "PgQuestInfo.h"
#include "ItemBagMgr.h"
#include "monsterdefmgr.h"
#include "pgmonsterbag.h"
#include "PgObjcetUnitDefMgr.h"
#include "BM/LocalMgr.h"

////////////////////////////////////////////////////////////////////////////////////
//	CItemDef
////////////////////////////////////////////////////////////////////////////////////
namespace CItemDefUtil
{
	bool bServerMode = true;
	typedef std::list< BM::vstring > CONT_ERROR_MSG;
	CONT_ERROR_MSG kContErrorMsg;

	//
	void SetServerMode(bool const bNew)
	{
		bServerMode = bNew;
	}
	void AddError(BM::vstring const& rkErrorMsg)
	{
		if( bServerMode )
		{
			kContErrorMsg.push_back( rkErrorMsg );
		}
	}
	bool Display()
	{
		CONT_ERROR_MSG kContTemp;
		kContErrorMsg.swap(kContTemp);
		CONT_ERROR_MSG::const_iterator iter = kContTemp.begin();
		while( kContTemp.end() != iter )
		{
			CAUTION_LOG(BM::LOG_LV1, (*iter));
			++iter;
		}
		return kContTemp.empty();
	}

	//
	bool IsRightQuestID(CONT_DEF_QUEST_REWARD const* pkQuestReward, CItemDef const* pkDefItem, WORD const wAbilType)
	{
		int const iQuestID = pkDefItem->GetAbil(wAbilType);
		if( 0 == iQuestID )
		{
			return true;
		}

		CONT_DEF_QUEST_REWARD::const_iterator find_iter = pkQuestReward->find(iQuestID);
		if( pkQuestReward->end() == find_iter )
		{
			AddError( BM::vstring() << __FL__<<L"Is Quest ID["<<iQuestID<<L"] Can't find in [TB_DefQuestReward], in Quest Help Item["<<pkDefItem->No()<<L"] and AbilType["<<wAbilType<<L"] Value["<<iQuestID<<L"]" );
			return false;
		}

		CONT_DEF_QUEST_REWARD::mapped_type const& rkQuestReward = (*find_iter).second;
		if( !PgQuestInfoUtil::IsLoadXmlType(rkQuestReward.iDBQuestType) )
		{
			AddError( BM::vstring() << __FL__<<L"Is Quest ID["<<iQuestID<<L"] is Wrong, in Quest Help Item["<<pkDefItem->No()<<L"] and AbilType["<<wAbilType<<L"] Value["<<iQuestID<<L"]" );
			return false;
		}
		return true;
	}
};

LP_CALLBACK_RES_CHOOSER CItemDef::m_pResChooseFunc = NULL;
LP_CALLBACK_RES_CHOOSER CItemDef::m_pResChooseFuncNew = NULL;

void CItemDef::SetResChooserFunc(LP_CALLBACK_RES_CHOOSER pFunc)
{
	m_pResChooseFunc = pFunc;
}
void CItemDef::SetResChooserFuncNew(LP_CALLBACK_RES_CHOOSER pFunc)
{
	m_pResChooseFuncNew = pFunc;
}

bool CItemDef::SetAbil(WORD const Type, int const iValue)
{
	// Warning!!!
	// Data Race possible, do not use this function too much
	switch(Type)
	{
	case AT_COSTUME_GRADE:		{ m_iCostumeGrade = iValue; }break;
	case AT_ITEM_SORT_ORDER1:	{ m_iOrder1 = iValue; }break;
	case AT_ITEM_SORT_ORDER2:	{ m_iOrder2 = iValue; }break;
	case AT_ITEM_SORT_ORDER3:	{ m_iOrder3 = iValue; }break;
	case AT_TYPE: 				{ m_sType = iValue; }break;
	case AT_ATTRIBUTE:			{ m_iAttribute = iValue; }break;
	case AT_GENDERLIMIT:		{ m_byGender = iValue; }break;
	case AT_LEVELLIMIT:			{ m_sLevel = iValue; }break;
	case AT_CLASSLIMIT:
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"GetAbil64 로 뽑으시오!");
		}break;
	default:
		{
			return CAbilObject::SetAbil(Type, iValue);
		}break;
	}
	return true;
}

int CItemDef::GetAbil(WORD const wAbilType) const
{
	int iValue = 0;
	switch(wAbilType)
	{
	case AT_COSTUME_GRADE:
		{
			iValue = m_iCostumeGrade;
		}break;
	case AT_ITEM_SORT_ORDER1:
		{
			iValue = m_iOrder1;
		}break;
	case AT_ITEM_SORT_ORDER2:
		{
			iValue = m_iOrder2;
		}break;
	case AT_ITEM_SORT_ORDER3:
		{
			iValue = m_iOrder3;
		}break;
	case AT_TYPE:
		{
			iValue = m_sType;
		}break;
	case AT_ATTRIBUTE:
		{
			iValue = m_iAttribute;
		}break;
	case AT_GENDERLIMIT:
		{
			iValue = m_byGender;
		}break;
	case AT_LEVELLIMIT:
		{
			iValue = m_sLevel;
		}break;
	case AT_CLASSLIMIT:
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"GetAbil64 로 뽑으시오!");
		//	iValue = m_iClass;
		}break;
	case AT_DEFAULT_AMOUNT:
		{
			iValue = CAbilObject::GetAbil(wAbilType);
			if (iValue == 0)
			{
				// 입력되어 있지 않으면 개수 1개짜리 아이템으로 간주한다.
				iValue = USE_AMOUNT_BASE_VALUE + 1;
			}
		}break;
	case AT_EFFECT_DELETE_ALL_DEBUFF:
		{
			iValue = CAbilObject::GetAbil(wAbilType);
			if( iValue == 0 )
			{
				for( int AbilTypeNo = AT_EFFECTNUM1; AbilTypeNo <= AT_EFFECTNUM10; ++AbilTypeNo )
				{
					int const EffectAbil = CAbilObject::GetAbil(AbilTypeNo);
					if( EffectAbil )
					{
						CONT_DEFEFFECTABIL const * pConstContDef = NULL;
						g_kTblDataMgr.GetContDef(pConstContDef);
						if( pConstContDef )
						{
							CONT_DEFEFFECTABIL::const_iterator EffectAbil_Iter = pConstContDef->find( EffectAbil );
							if( pConstContDef->end() != EffectAbil_Iter )
							{
								for( int AbilTypeCnt = 0; AbilTypeCnt < MAX_EFFECT_ABIL_ARRAY; ++AbilTypeCnt )
								{
									if( AT_EFFECT_DELETE_ALL_DEBUFF == EffectAbil_Iter->second.iType[AbilTypeCnt] )
									{
										iValue = EffectAbil_Iter->second.iValue[AbilTypeCnt];
										return iValue;
									}
								}
							}
						}
					}
				}
			}
		}break;
	default:
		{
			iValue = CAbilObject::GetAbil(wAbilType);
		}break;
	}
	return iValue;
}

__int64 CItemDef::GetAbil64(WORD const wAbilType) const
{
	__int64 iValue = 0;
	switch(wAbilType)
	{
	case AT_CLASSLIMIT:	
		{			
			iValue = m_i64ClassLimit;
		}break;
	case AT_CLASSLIMIT_DISPLAY_FILTER:
		{
			iValue = m_i64ClassLimitDisplayFilter;
		}break;
	default:
		{
			iValue = CAbilObject::GetAbil64(wAbilType);
		}break;
	}
	return iValue;
}


CItemDef::CItemDef()
{
}

CItemDef:: ~CItemDef()
{
}

int CItemDef::ImproveAbil(EAbilType const eAbilType, PgBase_Item const &kItem)const//랭크같은거로 인해 어빌값이 바뀌는것.
{
	WORD kWord = eAbilType;
	return ImproveAbil(kWord, kItem);
}

int CItemDef::ImproveAbil(WORD &wAbilType, PgBase_Item const &kItem)const//랭크같은거로 인해 어빌값이 바뀌는것.
{
	int iAddRate = 0;
	switch(wAbilType)
	{
	case AT_PHY_ATTACK_MIN:
	case AT_MAGIC_ATTACK_MIN:
	case AT_PHY_ATTACK_MAX:
	case AT_MAGIC_ATTACK_MAX:
		{
			iAddRate = GetAbil(AT_ITEM_ATTACK_ADD_RATE);
		}break;
	case AT_PHY_DEFENCE:
	case AT_MAGIC_DEFENCE:
		{
			iAddRate = GetAbil(AT_ITEM_DEFENCE_ADD_RATE);
		}break;
	}

	switch(wAbilType)
	{
	case AT_PHY_ATTACK_MIN:
	case AT_MAGIC_ATTACK_MIN:
		{
			int iRet = GetAbil(wAbilType);
			iRet += iRet * (iAddRate/ABILITY_RATE_VALUE_FLOAT);
			return iRet;
		}break;
	case AT_PHY_ATTACK_MAX:
	case AT_MAGIC_ATTACK_MAX:
	case AT_PHY_DEFENCE:
	case AT_MAGIC_DEFENCE:
	case AT_PHY_ATTACK: //솔라이어링 소울크래프트 옵션 적용 되도록 수정 하였음
	case AT_MAGIC_ATTACK: //솔라이어링 소울크래프트 옵션 적용 되도록 수정 하였음
		{
			int iRet = GetAbil(wAbilType);
			iRet += iRet * (iAddRate/ABILITY_RATE_VALUE_FLOAT);

			if(kItem.IsEmpty())
			{
				return iRet;
			}

			if(false == kItem.EnchantInfo().IsSeal() && (0 < kItem.EnchantInfo().Rarity())
				&& 0 != iRet
				)
			{
				// Rare 등급에 따라 능력치를 증가 시켜 준다.
				// 무기 공격력or방어력 * (1+레어도/400))
				// 방어구 공격력or방어력 * (1+레어도/200))
				int iBaseRate = (EquipPos() == EQUIP_POS_WEAPON ? 400 : 200);
				iRet += (int)(1+((iRet*kItem.EnchantInfo().Rarity())/iBaseRate));
			}

			CONT_DEFITEMENCHANTABILWEIGHT const * pkTable = NULL;
			g_kTblDataMgr.GetContDef(pkTable);
			if(!pkTable)
			{
				return iRet;
			}

			CONT_DEFITEMENCHANTABILWEIGHT::const_iterator iter = pkTable->find(kItem.EnchantInfo().PlusLv());
			if(iter == pkTable->end())
			{
				return iRet;
			}

			return iRet + (iRet * (*iter).second)/ABILITY_RATE_VALUE_FLOAT;
		}break;
	case AT_I_PHY_DEFENCE_ADD_RATE:
		{
			// 해당 아이템의 물리방어력 값만 올리는 값
			// 이 값은, Rare등급에 따른 능력치가 적용된 값에서 올려 준다. (박일환씨 말, 2008.06.03)
			wAbilType = AT_PHY_DEFENCE;
			int iBasic = ImproveAbil(wAbilType, kItem);			
			return iBasic;
		}break;
	case AT_I_MAGIC_DEFENCE_ADD_RATE:
		{
			wAbilType = AT_MAGIC_DEFENCE;
			int iBasic = ImproveAbil(wAbilType, kItem);			
			return iBasic;
		}break;
	default:
		{
			return GetAbil(wAbilType);
		}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

bool CItemDef::IsAmountItem()const
{
	size_t const iValue = GetAbil(AT_DEFAULT_AMOUNT);//기본 수량치가 있어야하고
	if(iValue
	&& iValue >= USE_AMOUNT_BASE_VALUE)//얘는 겹쳐지는 아이템.
	{
		return true;
	}
	return false;
}

size_t CItemDef::MaxAmount()const 
{
	if(!IsAmountItem())
	{
		return GetAbil(AT_DEFAULT_AMOUNT);
	}

	return GetAbil(AT_MAX_LIMIT);
}

int CItemDef::PrimaryInvType()const
{
	return GetAbil(AT_PRIMARY_INV);
}

int CItemDef::PetInvType()const
{
	return GetAbil(AT_PET_INV);
}

bool CItemDef::CanConsume()const//사용가능?
{
	return IsType(ITEM_TYPE_CONSUME);
}

bool CItemDef::CanEquip()const//장착
{
	if( IsType(ITEM_TYPE_EQUIP) )
	{
		if(EQUIP_LIMIT_NONE != EquipPos())
		{
			return true;
		}
	}
	return false;
}

bool CItemDef::IsType(EItemType eTestType) const
{
	return 0 != (GetAbil(AT_TYPE) & eTestType);
}

bool CItemDef::IsPetItem()const
{
	return ( 0 != (GWL_PET_UNISEX & GetAbil(AT_GENDERLIMIT)) );
}

int CItemDef::EquipPos()const
{
	int const iEquipLimit = GetAbil(AT_EQUIP_LIMIT);
	if(EQUIP_LIMIT_NONE != iEquipLimit)
	{
		return EquipLimitToPos(iEquipLimit);
	}
	return EQUIP_LIMIT_NONE;
}

int CItemDef::OptionGroup()const
{
	return GetAbil(AT_OPTION_GROUP);
}

int CItemDef::EquipLimitToPos(int const iEquipLimit)
{
	switch(iEquipLimit)
	{
	case EQUIP_LIMIT_HAIR_COLOR	:{return EQUIP_POS_HAIR_COLOR;}
	case EQUIP_LIMIT_HAIR		:{return EQUIP_POS_HAIR;}
	case EQUIP_LIMIT_FACE		:{return EQUIP_POS_FACE;}
	case EQUIP_LIMIT_SHOULDER	:{return EQUIP_POS_SHOULDER;}
	case EQUIP_LIMIT_CLOAK		:{return EQUIP_POS_CLOAK;}
	case EQUIP_LIMIT_GLASS		:{return EQUIP_POS_GLASS;}
	case EQUIP_LIMIT_WEAPON		:{return EQUIP_POS_WEAPON;}
	case EQUIP_LIMIT_SHEILD		:{return EQUIP_POS_SHEILD;}
	case EQUIP_LIMIT_NECKLACE	:{return EQUIP_POS_NECKLACE;}
	case EQUIP_LIMIT_EARRING	:{return EQUIP_POS_EARRING;}
	case EQUIP_LIMIT_RING		:{return EQUIP_POS_RING;}
	case EQUIP_LIMIT_BELT		:{return EQUIP_POS_BELT;}
	case EQUIP_LIMIT_ATTSTONE	:{return EQUIP_POS_ATTSTONE;}
	case EQUIP_LIMIT_MEDAL		:{return EQUIP_POS_MEDAL;}
	case EQUIP_LIMIT_HELMET		:{return EQUIP_POS_HELMET;}
	case EQUIP_LIMIT_SHIRTS		:{return EQUIP_POS_SHIRTS;}
	case EQUIP_LIMIT_PANTS		:{return EQUIP_POS_PANTS;}
	case EQUIP_LIMIT_BOOTS		:{return EQUIP_POS_BOOTS;}
	case EQUIP_LIMIT_GLOVE		:{return EQUIP_POS_GLOVE;}
	case EQUIP_LIMIT_ARM		:{return EQUIP_POS_ARM;}
	case EQUIP_LIMIT_PET		:{return EQUIP_POS_PET;}
	case EQUIP_LIMIT_KICKBALL	:{return EQUIP_POS_KICKBALL;}
//	case (EQUIP_LIMIT_WEAPON | EQUIP_LIMIT_SHEILD) :{return EQUIP_POS_TWO_HAND;}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EQUIP_LIMIT_NONE"));
	return EQUIP_LIMIT_NONE;
}


void CItemDef::AddDropGround(int const iGroundNo)
{
	m_kContDropGround.insert(iGroundNo);
}

////////////////////////////////////////////////////////////////////////////////////
//	CItemDefMgr
////////////////////////////////////////////////////////////////////////////////////

#define checkabilbasezero(def,abil)	if(0 > def->GetAbil(abil)){VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ABIL MISSING !!!!![ITEMNO:"<<def->No()<<L" ABILNO:"<< abil << L"]");return false;}
#define checkabilbaseone(def,abil)	if(0 >= def->GetAbil(abil)){VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ABIL MISSING !!!!![ITEMNO:"<<def->No()<<L" ABILNO:"<< abil << L"]");return false;}
#define checkabilbaseone2(def,abil1,abil2)	if((0 >= def->GetAbil(abil1)) && (0 >= def->GetAbil(abil2))){VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ABIL MISSING !!!!![ITEMNO:"<<def->No()<<L" ABILNO:"<< abil1 << L" OR ABILNO:" << abil2 << L"]");return false;}

CItemDefMgr::CItemDefMgr()
	: m_pkContResConvert(NULL)
{
}
CItemDefMgr::~CItemDefMgr()
{
}
bool CItemDefMgr::CheckNeedAbil(DEF const * const pDef)
{
	int const iCustomType = pDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);

	switch(iCustomType)
	{
	case UICT_STYLEITEM:
		{
			checkabilbasezero(pDef,AT_USE_ITEM_CUSTOM_VALUE_1)
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_2)
		}break;
	case UICT_OPEN_CHEST:
	case UICT_LOCKED_CHEST:
		{
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_1)
		}break;
	case UICT_OPEN_PACK:
	case UICT_OPEN_PACK2:
		{
			checkabilbaseone2(pDef,AT_USE_ITEM_CUSTOM_VALUE_1,AT_USE_ITEM_CUSTOM_VALUE_2)
		}break;
	case UICT_INVEXTEND:
		{
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_1)
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_2)
		}break;
	case UICT_EMOTION:
		{
			checkabilbasezero(pDef,AT_USE_ITEM_CUSTOM_VALUE_1)
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_2)
		}break;
	case UICT_RENTALSAFE:
	case UICT_ADD_SP:
	case UICT_UNBIND:
	case UICT_SAFEEXTEND:
	case UICT_CASH_PACK:
	case UICT_SOCKET_SUCCESS:
	case UICT_CHANGE_CLASS:
	case UICT_GAMBLE:
		{
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_1)
		}break;
	case UICT_CREATE_CARD:
	case UICT_MODIFY_CARD:
		{
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_1)
		}break;
	case UICT_MONEY_PACK:
	case UICT_CHEST_KEY:
		{
			checkabilbaseone(pDef,AT_USE_ITEM_CUSTOM_VALUE_2)
		}break;
	case UICT_MONSTERCARD:
		{
			checkabilbaseone(pDef,AT_EQUIP_LIMIT)
			checkabilbaseone(pDef,AT_MONSTER_CARD_NO)
			checkabilbaseone(pDef,AT_MONSTER_CARD_ORDER)
		}break;
	}

	return true;
}

bool CItemDefMgr::BuildMissionItemDropPos(CONT_DEF_MISSION_ROOT const & kContMissionRoot, CONT_DEF_MISSION_RESULT const & kContMissionResult, CONT_DEF_MISSION_CANDIDATE const & kContMissionCandi, CItemBagMgr const & kItemBagMgr)
{
	bool bRet = true;
	for(CONT_DEF_MISSION_ROOT::const_iterator iter = kContMissionRoot.begin();iter != kContMissionRoot.end();++iter)
	{
		for(int i = 0;i < MAX_MISSION_LEVEL;++i)
		{
			int const iLevelNo = (*iter).second.aiLevel[i];
			int const iResultNo = (*iter).second.aiMissionResultNo[i];
			if((0 == iLevelNo) || (0 == iResultNo))
			{
				continue;
			}

			CONT_DEF_MISSION_CANDIDATE::const_iterator lv_iter = kContMissionCandi.find(iLevelNo);
			if(lv_iter == kContMissionCandi.end())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find LevelNo["<<iLevelNo<<L"] from MissionNo["<<(*iter).second.iMissionNo<<L"]" );
				bRet= false;
				continue;
			}

			CONT_DEF_MISSION_RESULT::const_iterator result_iter = kContMissionResult.find(iResultNo);
			if(result_iter == kContMissionResult.end())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find ResultNo["<<iResultNo<<L"] from MissionNo["<<(*iter).second.iMissionNo<<L"]" );
				bRet= false;
				continue;
			}

			CONT_SET_DATA kCont;

			for(int mr = 0;mr < MAX_MISSION_RANK_CLEAR_LV;++mr)
			{
				int const iItemContainer = (*result_iter).second.aiResultContainer[mr];
				if(0 == iItemContainer)
				{
					bRet= false;
					continue;
				}

				if(S_OK != kItemBagMgr.EnumerateItemContainer(iItemContainer, kCont))
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't EnumerateItemContainer["<<iItemContainer<<L"] from MissionNo["<<(*iter).second.iMissionNo<<"] Rank"<<mr<<L"]" );
					bRet= false;
					continue;
				}
			}

			int const iGroundNo = (*lv_iter).second.iCandidate_End; // 미션 보상은 제일 마지막 맵에서 받을 수 있다.

			if(false == BuildItemDropGroundInfo(kCont,iGroundNo))
			{
				bRet= false;
				continue;
			}
		}
	}
	return bRet;
}

bool CItemDefMgr::BuildMapItemBagDropPos(int const iGroundNo, CItemBagMgr const & kItemBagMgr, CONT_DEF_MAP_ITEM_BAG const & kDefMapBag, CONT_SET_DATA & kCont)
{
	CONT_DEF_MAP_ITEM_BAG::const_iterator iter = kDefMapBag.find(iGroundNo);
	if(iter == kDefMapBag.end())
	{
		return true;
	}

	bool bRet = true;

	for(int i = 0;i < MAP_ITEM_BAG_GRP;++i)
	{
		int const iBagGroupNo = (*iter).second.aiBagGrpNo[i];

		if(0 == iBagGroupNo)
		{
			continue;
		}

		if(S_OK != kItemBagMgr.EnumerateItemBagGroup(iBagGroupNo, kCont))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find BagGroup["<<iBagGroupNo<<" of GroundNo["<<iGroundNo<<L"]" );
			bRet = false;
		}
	}

	return bRet;
}

bool CItemDefMgr::BuildUnitDropItem(CAbilObject const * pkDef, CItemBagMgr const & kItemBagMgr, CONT_SET_DATA & kCont)
{
	int iAlwaysDropItem = 0;
	iAlwaysDropItem = pkDef->GetAbil(AT_ALWAYS_DROP_ITEM_1);if(iAlwaysDropItem){kCont.insert(iAlwaysDropItem);}
	iAlwaysDropItem = pkDef->GetAbil(AT_ALWAYS_DROP_ITEM_2);if(iAlwaysDropItem){kCont.insert(iAlwaysDropItem);}
	iAlwaysDropItem = pkDef->GetAbil(AT_ALWAYS_DROP_ITEM_3);if(iAlwaysDropItem){kCont.insert(iAlwaysDropItem);}
	iAlwaysDropItem = pkDef->GetAbil(AT_ALWAYS_DROP_ITEM_4);if(iAlwaysDropItem){kCont.insert(iAlwaysDropItem);}
	iAlwaysDropItem = pkDef->GetAbil(AT_ALWAYS_DROP_ITEM_5);if(iAlwaysDropItem){kCont.insert(iAlwaysDropItem);}

	int const iItemContainer = pkDef->GetAbil(AT_ITEM_DROP_CONTAINER);

	if(iItemContainer)
	{
		if(S_OK != kItemBagMgr.EnumerateItemContainer(iItemContainer, kCont))
		{
			return false;
		}
	}
	else
	{
		int const iPrimary = pkDef->GetAbil(AT_ITEM_BAG_PRIMARY);
		if(iPrimary)
		{
			if(S_OK != kItemBagMgr.EnumerateItemBag(iPrimary, kCont))
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Primary Bag["<<iPrimary<<L"]" );
				return false;
			}
		}

		int const iSecondary = pkDef->GetAbil(AT_ITEM_BAG_SECONDARY);
		if(iSecondary)
		{
			if(S_OK != kItemBagMgr.EnumerateItemBag(iSecondary, kCont))
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Secondary Bag["<<iSecondary<<L"]" );
				return false;
			}
		}
	}

	return true;
}

bool CItemDefMgr::BuildMonsterBagDropPos(SMonsterControl const & kMonCtrl, CMonsterDefMgr const & kMonsterDefMgr, CItemBagMgr const & kItemBagMgr, CONT_SET_DATA & kCont)
{
	bool bRet = true;
	for(int i = 0;i < MAX_MONSTERBAG_ELEMENT;++i)
	{
		for(CONT_SET_DATA::const_iterator iter = kMonCtrl.kSetMonster[i].begin();iter != kMonCtrl.kSetMonster[i].end();++iter)
		{
			CMonsterDef const * pkDef = kMonsterDefMgr.GetDef((*iter));
			if(!pkDef)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Monster["<<(*iter)<<L"]" );
				bRet = false;
				continue;
			}

			if(false == BuildUnitDropItem(pkDef,kItemBagMgr,kCont))
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find UnitDropItem of MonsterNo["<<pkDef->No()<<L"]" );
				bRet = false;
				continue;
			}
		}
	}
	return bRet;
}

bool CItemDefMgr::BuildItemDropGroundInfo(CONT_SET_DATA const & kCont,int const iGroundNo)
{
	for(CONT_SET_DATA::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
	{
		CONT_DEF::iterator item_iter = m_contDef.find((*iter));
		if(item_iter == m_contDef.end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find ItemBag=") << (*iter) << _T(", Ground =") << iGroundNo);
			return false;
		}

		if(true == (*item_iter).second->CanEquip())
		{
			(*item_iter).second->AddDropGround(iGroundNo);
		}
	}
	return true;
}

HRESULT CItemDefMgr::BuildDropPos(CMonsterDefMgr const & kMonsterDefMgr, CItemBagMgr const & kItemBagMgr, PgMonsterBag const & kMonsterBagMgr, CONT_DEF_MAP_ITEM_BAG const & kDefMapBag)
{
	HRESULT bRet = S_OK;
	CONT_HASH_MON_MULTI_DATA const & kContMonster = kMonsterBagMgr.GetMapMonsterCont();

	for(CONT_HASH_MON_MULTI_DATA::const_iterator iter = kContMonster.begin();iter != kContMonster.end();++iter)
	{
		int const iGroundNo = (*iter).first;

		CONT_SET_DATA kCont;

		bool bResult = BuildMapItemBagDropPos(iGroundNo,kItemBagMgr,kDefMapBag,kCont);
		if(false == bResult)
		{
			bRet = E_BUILD_MAPITEMBAG_FAIL;
			continue;
		}

		bResult = BuildMonsterBagDropPos((*iter).second,kMonsterDefMgr,kItemBagMgr,kCont);
		if(false == bResult)
		{
			bRet = E_BUILD_MONSTERITEMBAG_FAIL;
			continue;
		}

		bResult = BuildItemDropGroundInfo(kCont,iGroundNo);
		if(false == bResult)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't BuildItemDropGroundInfo of GroundNo["<<iGroundNo<<L"]" );
			bRet = E_BUILD_ITEMDROPMAPINFO_FAIL;
		}
	}
	
	return bRet;
}

bool CItemDefMgr::BuildObjectItemDropPos(CONT_DEF_MAP_REGEN_POINT const & kRegenPoint, CONT_DEF_OBJECT_BAG const & kObjectBag, CONT_DEF_OBJECT_BAG_ELEMENTS const & kObjectBagElement, CItemBagMgr const & kItemBagMgr, PgObjectUnitDefMgr const & kObjectDefMgr)
{
	for(CONT_DEF_MAP_REGEN_POINT::const_iterator map_iter = kRegenPoint.begin();map_iter != kRegenPoint.end();++map_iter)
	{
		if(ERegenBag_Type_OjbUnit != (*map_iter).second.cBagControlType)
		{
			continue;
		}

		int const iMapNo = (*map_iter).second.iMapNo;

		CONT_DEF_OBJECT_BAG::const_iterator bag_iter = kObjectBag.find((*map_iter).second.iBagControlNo);
		if(bag_iter == kObjectBag.end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find BagControlNo=") << (*map_iter).second.iBagControlNo << _T(", Ground =") << iMapNo);
			return false;
		}
		
		CONT_SET_DATA kCont;

		for(int i = 0;i < MAX_OBJECT_ELEMENT_NO;++i)
		{
			int const & iElementNo = (*bag_iter).second.iElementNo[i];
			if(0 == iElementNo)
			{
				continue;
			}

			CONT_DEF_OBJECT_BAG_ELEMENTS::const_iterator element_iter = kObjectBagElement.find(iElementNo);
			if(element_iter == kObjectBagElement.end())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find BagElementNo=") << iElementNo << _T(", Ground =") << iMapNo);
				return false;
			}

			PgObjectUnitDef const * pkDef = kObjectDefMgr.GetDef((*element_iter).second.iObjectNo);
			if(NULL == pkDef)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find ObjectUnitNo=") << (*element_iter).second.iObjectNo << _T(", Ground =") << iMapNo);
				return false;
			}

			if(false == BuildUnitDropItem(pkDef,kItemBagMgr,kCont))
			{
				return false;
			}
		}

		if(false == BuildItemDropGroundInfo(kCont,(*map_iter).second.iMapNo))
		{
			return false;
		}
	}
	return true;
}

bool CItemDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	bool bReturn = true;
	Clear();

	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->No((*base_itor).second.ItemNo);
		pDef->NameNo((*base_itor).second.NameNo);
		pDef->ResNo((*base_itor).second.ResNo);
		pDef->m_sType = base_itor->second.sType;
		pDef->SellPrice(base_itor->second.iSellPrice);
		pDef->BuyPrice(base_itor->second.iPrice);
		pDef->m_iAttribute = base_itor->second.iAttribute;
		pDef->m_byGender = base_itor->second.byGender;
		pDef->m_sLevel = base_itor->second.sLevel;
		pDef->m_i64ClassLimit = base_itor->second.i64ClassLimit | (base_itor->second.i64DraClassLimit<<DRAGONIAN_LSHIFT_VAL);
		pDef->m_i64ClassLimitDisplayFilter = base_itor->second.i64ClassLimitDisplayFilter | (base_itor->second.i64DraClassLimitDisplayFilter<<DRAGONIAN_LSHIFT_VAL);
		pDef->m_iOrder1 = base_itor->second.iOrder1;
		pDef->m_iOrder2 = base_itor->second.iOrder2;
		pDef->m_iOrder3 = base_itor->second.iOrder3;
		pDef->m_iCostumeGrade = base_itor->second.iCostumeGrade;

		for(int iIndex = 0; MAX_ITEM_ABIL_LIST > iIndex ; ++iIndex)
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aAbil[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

				if( abil_itor != tblAbil.end() )
				{
					for( int j = 0; MAX_ITEM_ABIL_ARRAY > j ; ++j)
					{
						if( 0 != (*abil_itor).second.aType[j]  )
						{
							switch((*abil_itor).second.aType[j])
							{
							case AT_DIE_EXP:
							case AT_CLASSLIMIT:	
								{
									// (*abil_itor).second.aValue[j] 값이 int 라서 int64로 건네줄때 값이 짤릴텐데
									// classlimit는 m_iClassLimit 에 직접 때려박는다지만 다른건 어떻게 해야하나
									// CONT_ABIL 를 int64용으로 또 만들어야 하나?
									VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Error!!");

									pDef->SetAbil64((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); 
								}break;
							case AT_EXPERIENCE:	// 경험치 주는 Scroll 생겼음~
							case AT_MONEY:		// Money주는 Scroll 생길 수도 있으니 미리 빼놓음.
								{
									pDef->SetAbil64((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]);
								}break;
							case AT_ATTRIBUTE:
								{
									pDef->m_iAttribute = (*abil_itor).second.aValue[j];
								}break;
							// fix: that cause of bug, when we set type abil from abils, not from item type
							// todo: cleanup item abil
							case AT_COSTUME_GRADE:
							case AT_ITEM_SORT_ORDER1:
							case AT_ITEM_SORT_ORDER2:
							case AT_ITEM_SORT_ORDER3:
							case AT_TYPE:
							case AT_GENDERLIMIT:
							case AT_LEVELLIMIT:
							case AT_FRAN:{}break;
							default:
								{ 
									pDef->SetAbil((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); 
								}break;
							}
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find ItemAbil["<<iAbilNo<<L"] From ItemNo["<<pDef->No()<<L"]" );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
					bReturn = false;
				}
			}
		}

		bReturn = CheckNeedAbil(pDef) && bReturn;

		auto kRet = m_contDef.insert( std::make_pair(pDef->No(), pDef) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicat ItemDef No ["<<pDef->No()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}
		++base_itor;
	}

	if( m_pkContResConvert )
	{
		CONT_DEF_ITEM_RES_CONVERT::const_iterator res_conv_itor = m_pkContResConvert->begin();
		while(m_pkContResConvert->end() != res_conv_itor)
		{
			CONT_DEF::iterator item_itor = m_contDef.find(res_conv_itor->second.ItemNo);
			if(item_itor != m_contDef.end())
			{
				if(g_kLocal.IsServiceRegion(res_conv_itor->second.NationCode))//국가코드 일치시
				{
					item_itor->second->ResNo(res_conv_itor->second.ResNo);
				}
			}
			++res_conv_itor;
		}
	}

	return bReturn && DataCheck();
}

bool CItemDefMgr::DataCheck()const
{
	if(!m_contDef.size())
	{
		assert(NULL);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFRES const* pkDefRes = NULL;
	CONT_DEFSTRINGS const* pkDefStrings = NULL;
	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkDefRes);
	if( !pkDefRes )
	{
		CAUTION_LOG(BM::LOG_LV1,  __FL__ << _T("can't go build Item, can't get [DefRes]"));
		return false;
	}
	g_kTblDataMgr.GetContDef(pkDefStrings);
	if( !pkDefStrings )
	{
		CAUTION_LOG(BM::LOG_LV1,  __FL__ << _T("can't go build Item, can't get [DefStrings]"));
		return false;
	}

	g_kTblDataMgr.GetContDef(pkQuestReward);
	if( !pkQuestReward )
	{
		CAUTION_LOG(BM::LOG_LV1,  __FL__ << _T("can't go build Item, can't get [DefQuestReward]"));
		return false;
	}

	CONT_DEF::const_iterator def_itor = m_contDef.begin();
	while( m_contDef.end() != def_itor )
	{
		DEF const* const pDef = (*def_itor).second;

		if( pkDefRes->end() == pkDefRes->find(pDef->ResNo()) )
		{
			CItemDefUtil::AddError( BM::vstring() << __FL__ << _T("ItemNo[") << pDef->No() << _T("] is ResNo[") << pDef->ResNo() << _T("] is can't find from the [TB_DefRes]") );
		}

		if( pkDefStrings->end() == pkDefStrings->find(pDef->NameNo()) )
		{
			CItemDefUtil::AddError( BM::vstring() << __FL__ << _T("ItemNo[") << pDef->No() << _T("] is NameNo[") << pDef->NameNo() << _T("] is can't find from the [TB_DefStrings]") );
		}

		if(0 == pDef->GetAbil(AT_VIRTUAL_ITEM))
		{
			if(!pDef->GetAbil(AT_DEFAULT_AMOUNT))	
			{
				CItemDefUtil::AddError( BM::vstring() << __FL__<<L"ItemDef NoAmount Item["<<(*def_itor).first<<L"]" );
			}//
			if(!pDef->GetAbil(AT_PRIMARY_INV))		
			{
				CItemDefUtil::AddError( BM::vstring() << __FL__<<L"ItemDef NoPriInv Item["<<(*def_itor).first<<L"]" );
			}//
		}

		switch( pDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) )
		{
		case UICT_LEARNSKILL:
			{
				int const iSkillNo = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				if( 0 == iSkillNo )
				{
					CItemDefUtil::AddError( BM::vstring() << __FL__<<L"CustomType[LearnSkill] but, SkillNo is 0" );
				}
				if( iSkillNo != GET_BASICSKILL_NUM(iSkillNo) )
				{
					CItemDefUtil::AddError( BM::vstring() << __FL__<<L"CustomType[LearnSkill] but, SkillNo["<<iSkillNo<<L"] is Only BasicSkill" );
				}
			}break;
		case UICT_HELP_END_QUEST:
			{
				int const iQuestID1 = pDef->GetAbil(AT_HELP_END_QUEST1);
				int const iQuestID2 = pDef->GetAbil(AT_HELP_END_QUEST2);
				int const iQuestID3 = pDef->GetAbil(AT_HELP_END_QUEST3);
				int const iQuestID4 = pDef->GetAbil(AT_HELP_END_QUEST4);
				if( 0 == iQuestID1
				&&	0 == iQuestID2
				&&	0 == iQuestID3
				&&	0 == iQuestID4 )
				{
					CItemDefUtil::AddError( BM::vstring() << __FL__<<L"Is Quest Help Item["<<(*def_itor).first<<L"] But Have a QuestID is all 0" );
				}

				CItemDefUtil::IsRightQuestID(pkQuestReward, pDef, AT_HELP_END_QUEST1);
				CItemDefUtil::IsRightQuestID(pkQuestReward, pDef, AT_HELP_END_QUEST2);
				CItemDefUtil::IsRightQuestID(pkQuestReward, pDef, AT_HELP_END_QUEST3);
				CItemDefUtil::IsRightQuestID(pkQuestReward, pDef, AT_HELP_END_QUEST4);
			}break;
		case UICT_REBUILD_RAND_QUEST_CONSUME:
		case UICT_REBUILD_RAND_QUEST:
			{
				int const iCustomValue1 = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				switch( iCustomValue1 )
				{
				case QT_Random:
					{
						if( RandomQuest::iRandomQuestMinLimitLevel > pDef->GetAbil(AT_LEVELLIMIT) )
						{
							CItemDefUtil::AddError( BM::vstring() << __FL__<<L"wrong item["<<(*def_itor).first<<L"] Limit level["<<pDef->GetAbil(AT_LEVELLIMIT)<<L"], right level is ["<<RandomQuest::iRandomQuestMinLimitLevel<<L"]" );
						}
					}break;
				case QT_RandomTactics:
					{
						if( RandomQuest::iRandomTacticsQuestMinLimitLevel > pDef->GetAbil(AT_LEVELLIMIT) )
						{
							CItemDefUtil::AddError( BM::vstring() << __FL__<<L"wrong item["<<(*def_itor).first<<L"] Limit level["<<pDef->GetAbil(AT_LEVELLIMIT)<<L"], right level is ["<<RandomQuest::iRandomTacticsQuestMinLimitLevel<<L"]" );
						}
					}break;
				default:
					{
						// 불허용
						CItemDefUtil::AddError( BM::vstring() << __FL__<<L"wrong item["<<(*def_itor).first<<L"] custome value[abil:"<<AT_USE_ITEM_CUSTOM_VALUE_1<<L" Value:"<<iCustomValue1<<L"], only ["<<QT_Random<<L", "<<QT_RandomTactics<<L"]" );
					}break;
				}
			}break;
		case UICT_SUPER_GROUND_MOVE_FLOOR:
			{
				int const iCustomValue1 = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				if( 0 == iCustomValue1 )
				{
					CItemDefUtil::AddError( BM::vstring() << __FL__<<L"wrong item["<<(*def_itor).first<<L"] custom value1["<<iCustomValue1<<L"], this is can't use 0" );
				}
			}break;
		case UICT_SUPER_GROUND_ENTER_FLOOR:
			{
				int const iSuperGroundNo = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				int const iModeNo = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
				int const iFloorNo = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);
				int const iSpawnNo = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_4);
				CONT_DEF_SUPER_GROUND_GROUP const* pkDefSuperGround = NULL;
				g_kTblDataMgr.GetContDef(pkDefSuperGround);
				if( pkDefSuperGround )
				{
					CONT_DEF_SUPER_GROUND_GROUP::const_iterator iter = pkDefSuperGround->find(iSuperGroundNo);
					if( pkDefSuperGround->end() == iter )
					{
						CItemDefUtil::AddError( BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] wrong custom value1["<<iSuperGroundNo<<L"], can't find [SuperGroundNo]" );
					}
					if( pkDefSuperGround->end() != iter )
					{
						size_t const iMaxFloor = (*iter).second.kContSuperGround.size();
						int const iMinFloor = 1;
						if( iMinFloor > iFloorNo
						||	iMaxFloor < iFloorNo )
						{
							CItemDefUtil::AddError( BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] custom value1["<<iSuperGroundNo<<L"] wrong custom value2["<<iFloorNo<<L"], max floor is " << iMaxFloor );
						}
					}
					switch( iModeNo )
					{
					case SGM_NORMAL:
					case SGM_ENCHANT:
					case SGM_RARE:
						{
						}break;
					default:
						{
							CItemDefUtil::AddError( BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] custom value1["<<iSuperGroundNo<<L"] custom value2["<<iFloorNo<<L"], wrong custom value3["<<iModeNo<<L"], is can only use [0, 1, 2]" );
						}break;
					}
				}
			}break;
		case UICT_GAMBLE:
			{
				int const iCustomValue1 = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				CONT_GAMBLE const * pkContDefGamble = NULL;
				g_kTblDataMgr.GetContDef(pkContDefGamble);
				if( pkContDefGamble )
				{
					CONT_GAMBLE::const_iterator gamble_itr = pkContDefGamble->find(iCustomValue1);
					if( pkContDefGamble->end() == gamble_itr )
					{
						CItemDefUtil::AddError( BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] custom value1["<<iCustomValue1<<L"], can not found GambleID in [TB_DefGamble]" );
					}
				}
			}break;
		case UICT_JOBSKILL_SAVEIDX:
			{
				int const iOneBytePerBit = 8;
				int const iLimitSaveIdx = MAX_DB_JOBKSILL_SAVEIDX_SIZE * iOneBytePerBit;
				int const iCustomValue1 = pDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				if( iLimitSaveIdx <= iCustomValue1 )
				{
					CItemDefUtil::AddError( BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] [JobSkill_SaveIdx] custom value1["<<iCustomValue1<<L"], is can't over then ["<< iLimitSaveIdx <<"]" );
				}
				CONT_DEF_JOBSKILL_SAVEIDX const* pkDefJSSaveIdx = NULL;
				g_kTblDataMgr.GetContDef(pkDefJSSaveIdx);
				if( pkDefJSSaveIdx )
				{
					CONT_DEF_JOBSKILL_SAVEIDX::const_iterator find_iter = pkDefJSSaveIdx->find(iCustomValue1);
					if( pkDefJSSaveIdx->end() == find_iter )
					{
						BM::vstring kLogMsg(BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] [JobSkill_SaveIdx] custom value1["<<iCustomValue1<<L"], can not found SaveIdx in [TB_DefJobSkill_SaveIdx]");
						if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_DEVELOP) )
						{
							CAUTION_LOG(BM::LOG_LV1, kLogMsg);
						}
						else
						{
							CItemDefUtil::AddError( kLogMsg );
						}
					}
					else if( (*find_iter).second.iBookItemNo != pDef->No() )
					{
						BM::vstring kLogMsg(BM::vstring() << __FL__<<L"item["<<(*def_itor).first<<L"] [JobSkill_SaveIdx] custom value1["<<iCustomValue1<<L"], can not same ItemNo, From [TB_DefJobSkill_SaveIdx].[f_BookItemNo]"<<(*find_iter).second.iBookItemNo);
						if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_DEVELOP) )
						{
							CAUTION_LOG(BM::LOG_LV1, kLogMsg);
						}
						else
						{
							CItemDefUtil::AddError( kLogMsg );
						}
					}
				}
			}break;
		}

		++def_itor;
	}
	return CItemDefUtil::Display();
}

int CItemDefMgr::GetAbil(int iItemNo, int iAbil) const
{
	CItemDef const* pkDef = GetDef(iItemNo);
	if( !pkDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot Get ItemDef Item["<<iItemNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return pkDef->GetAbil(iAbil);
}

int CItemDef::ResNo()const
{
	if(	m_pResChooseFunc 
	&&	GetAbil(AT_MULTI_RES))
	{
		return m_ResNo + m_pResChooseFunc(GetAbil(AT_GENDERLIMIT));
	}
	else if(	m_pResChooseFuncNew 
	&&	GetAbil(AT_NEW_MULTI_TRS))
	{
		return m_ResNo + m_pResChooseFuncNew(GetAbil(AT_GENDERLIMIT));
	}
	return m_ResNo;
}

bool CItemEnchantDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	bool bReturn = true;
	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->Type((*base_itor).second.Type);
		pDef->Lv((*base_itor).second.Lv);
		pDef->NameNo((*base_itor).second.NameNo);
		
		for(int iIndex = 0; MAX_ITEM_ENCHANT_ABIL_LIST > iIndex ; ++iIndex)
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aAbil[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

				if( abil_itor != tblAbil.end() )
				{
					for( int j = 0; MAX_ITEM_ABIL_ARRAY > j ; ++j)
					{
						if(0 != (*abil_itor).second.aType[j])
						{
							switch((*abil_itor).second.aType[j])
							{
							case AT_EXPERIENCE:
							case AT_MONEY:
							case AT_DIE_EXP:
							case AT_CLASSLIMIT:	{ pDef->SetAbil((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); }break;
							default:			{ pDef->SetAbil( (*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); }break;
							}
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Abil List");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
					bReturn = false;
				}
			}
		}

		auto kRet = m_contDef.insert( std::make_pair(pDef->Key(), pDef) );
		if( !kRet.second )
		{
			SItemEnchantKey const& rkKey = pDef->Key();
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate Enchant EnchantKey[Type:"<<rkKey.Type<<L",Lv:"<<static_cast<int>(rkKey.Lv)<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}
		++base_itor;
	}
	return bReturn;
}

bool CRareDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	bool bReturn = true;
	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->No((*base_itor).second.RareNo);
		pDef->NameNo((*base_itor).second.NameNo);
		
/*		int const iAbilNo = (*base_itor).second.aAbilNo[iIndex];
		if( iAbilNo )
		{
			CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

			if( abil_itor != tblAbil.end() )
			{
				for( int j = 0; MAX_ITEM_ABIL_ARRAY > j ; ++j)
				{
					if( 0 != (*abil_itor).second.aType[j]  )
					{
						pDef->SetAbil( (*abil_itor).second.aType[j], (*abil_itor).second.aValue[j] );
					}
				}
			}
			else
			{
				assert( NULL && "Can't Find Abil List" );
			}
		}
*/
		auto kRet = m_contDef.insert( std::make_pair(pDef->No(), pDef) );;
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate RareDef No ["<<pDef->No()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}
		++base_itor;
	}
	return bReturn;
}