#include "stdafx.h"
#include "Inventory.h"
#include "InventoryUtil.h"
#include "PgControlDefMgr.h"
#include "ItemDefMgr.h"
#include "Global.h"
#include "tabledatamanager.h"
#include "PgItemRarityUpgradeFormula.h"
#include "Variant/PgSocketFormula.h"

namespace PgInventoryUtil
{
	// Util Class
	PgFitItem::PgFitItem(ItemContainer const& rkCont)
	{
		ItemContainer::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			ItemContainer::mapped_type const& rkItem = (*iter).second;
			E_ITEM_GRADE const eItemGrade = ::GetItemGrade(rkItem);
			if( IG_CURSE != eItemGrade )
			{
				m_kFitItem.insert( std::make_pair(rkItem.ItemNo(), rkItem) );
			}
			++iter;
		}
	}

	PgFitItem::~PgFitItem()
	{
	}

	// Util Function
	inline bool IsNormalSetAbil(WORD const &rkAbil)
	{
		switch( rkAbil )
		{
		case AT_CHAT_BALLOOON_TYPE:
			{
				return true;
			}break;
		default:
			{
				// none
			}break;
		}
		if(AT_DMG_REFLECT_RATE_FROM_ITEM01 <= rkAbil && AT_DMG_REFLECT_RATE_FROM_ITEM10 >= rkAbil	// 데미지 반사 확률
			|| AT_DMG_REFLECT_DMG_FROM_ITEM01 <= rkAbil && AT_DMG_REFLECT_DMG_FROM_ITEM10 >= rkAbil	// 절대값 데미지
			)
		{// 아이템의 데미지 반사 관련 어빌
			return true;
		}

		return false;
	}
	inline bool IsNormalAddAbil(WORD const &rkAbil)
	{
		switch( rkAbil )
		{
		case AT_JS_1ST_ADD_SKILL_TURNTIME:
		case AT_JS_1ST_ADD_USE_EXHAUSTION:
		case AT_JS_1ST_ADD_TOOL_USE_DURATION_RATE:
		case AT_JS_1ST_ADD_RESULT_ITEM_COUNT:
		case AT_JS_1ST_ADD_EXPERTNESS_RATE:
		case AT_JS_1ST_SUB_ADD_SKILL_TURNTIME:
		case AT_JS_1ST_SUB_ADD_USE_EXHAUSTION:
		case AT_JS_1ST_SUB_ADD_TOOL_USE_DURATION_RATE:
		case AT_JS_1ST_SUB_ADD_RESULT_ITEM_COUNT:
		case AT_JS_1ST_SUB_ADD_EXPERTNESS_RATE:
			{
				return true;
			}break;
		}
		return false;
	}
	inline bool FilterInventoryAbil(WORD const &rkAbil)
	{
		// 전체 Inventory의 어빌을 변경시키는 어빌들
		switch( rkAbil )
		{
		case AT_I_PHY_ATTACK_ADD_RATE:
		case AT_I_MAGIC_ATTACK_ADD_RATE:
		case AT_INVEN_PHY_DEFENCE_ADD_RATE:
		case AT_INVEN_MAGIC_DEFENCE_ADD_RATE:

		case AT_ADD_R_NEED_HP:
		case AT_ADD_R_NEED_MP:
		case AT_ADD_ENCHANT_RATE:
		case AT_ADD_SOULCRAFT_RATE:
		case AT_ADD_SOUL_RATE_REFUND:
		case AT_ADD_REPAIR_COST_RATE:

		case AT_HIT_DAMAGE_HP_RATE_ACTIVATE:
		case AT_HIT_DAMAGE_HP_RATE:	
		case AT_HIT_DECREASE_MP_RATE_ACTIVATE:
		case AT_HIT_DECREASE_MP_RATE:
		case AT_CRITICAL_DAMAGE_RATE_ACTIVATE:
		case AT_CRITICAL_DAMAGE_RATE:
		case AT_CRITICAL_DAMAGE_ACTIVATE:
		case AT_CRITICAL_DAMAGE:
		case AT_ATTACKED_DECREASE_DAMAGE_ACTIVATE_HP_RATE:
		case AT_ATTACKED_DECREASE_DAMAGE_RATE:

		case AT_SKILL_EFFICIENCY:
		case AT_INVEN_PET_ADD_TIME_EXP_RATE:
		case AT_INVEN_PET_ADD_HUNT_EXP_RATE:
		case AT_GIVE_TO_CALLER_ADD_EXP_RATE:
			{
				return true;
			}break;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	inline bool FilterPercentAbil(WORD const wAbilType)
	{
		switch( wAbilType )
		{
		case AT_R_PHY_DEFENCE: // % 의 어빌값만
		case AT_R_MAGIC_DEFENCE:
		case AT_R_PHY_ATTACK_MIN:
		case AT_R_PHY_ATTACK_MAX:
		case AT_R_MAGIC_ATTACK_MIN:
		case AT_R_MAGIC_ATTACK_MAX:
		case AT_I_PHY_DEFENCE_ADD_RATE:
		case AT_I_MAGIC_DEFENCE_ADD_RATE:
			{
				return true;
			}break;
		}
		return false;
	}

	inline void CalcedItemAddAbil(CAbilObject& rkAbilObject, PgBase_Item const& rkItem, CItemDef const* const pkItemDef, WORD const wAbilType, int const iAbilValue, CAbilObject kAddedOptAbil)
	{
		// 아이템의 %증가치 결과 값을 미리 결정 해놓는다.
		WORD const iBaseAbil = ::GetBasicAbil(wAbilType);
		int iValue = 0;

		if(true == FilterPercentAbil(wAbilType))
		{
			WORD wBaseAbil = iBaseAbil;
			int iOriginalValue = (int)(pkItemDef->ImproveAbil(wBaseAbil, rkItem));
			iValue = iOriginalValue * (iAbilValue / ABILITY_RATE_VALUE_FLOAT);
			rkAbilObject.AddAbil(wBaseAbil, iValue);

			switch( wAbilType )
			{
			case AT_I_PHY_DEFENCE_ADD_RATE: 
			case AT_I_MAGIC_DEFENCE_ADD_RATE:
				{
					int iPreAmplValue = iOriginalValue * (kAddedOptAbil.GetAbil(iBaseAbil) / ABILITY_RATE_VALUE_FLOAT);
					rkAbilObject.AddAbil(::GetAmpliAbil(iBaseAbil), iPreAmplValue);
				} break;
			default:
				{
					int iPreAmplValue = iOriginalValue * (kAddedOptAbil.GetAbil(wAbilType) / ABILITY_RATE_VALUE_FLOAT);
					rkAbilObject.AddAbil(::GetAmpliAbil(wAbilType), iPreAmplValue);
				} break;
			}			
		}
		else
		{
			rkAbilObject.AddAbil(wAbilType, iAbilValue);
		}
	}

	void OnEnumEffect(CItemDef const* const pkItemDef, int const iAbilMin,int const iRateMin,int const iTargetMin, PgInventory::CONT_ITEMACTIONEFFECT::mapped_type & kContItemActionEffect)
	{
		for(int i = 0;i < MAX_ITEM_EFFECT_ABIL_NUM;++i)
		{
			int const iEffectNo = pkItemDef->GetAbil(iAbilMin+i);
			int const iRate = pkItemDef->GetAbil(iRateMin+i);
			int const iTarget = pkItemDef->GetAbil(iTargetMin+i);

			if(0 == iEffectNo)
			{
				continue;
			}
			kContItemActionEffect.insert(PgInventory::CONT_ITEMACTIONEFFECT::mapped_type::value_type(iEffectNo,iRate,iTarget));
		}
	}

	inline void ApplyAbilDefault(CItemDef const* const pkItemDef, CAbilObject& rkCalculatedTarget, CAbilObject& rkFilteredTarget, PgInventory::CONT_ITEMACTIONEFFECT & kContItemActionEffect, PgBase_Item const& rkItem)
	{
		SAbilIterator kItor;
		pkItemDef->FirstAbil(&kItor);
		while(pkItemDef->NextAbil(&kItor))
		{
			if( ::IsCalculateAbilRange(kItor.wType))
			{
				CAbilObject decoy;
				int const iValue = pkItemDef->ImproveAbil(static_cast<EAbilType>(kItor.wType), rkItem);//아이템 등급에 따른 공격력, 방어력 향상.
				CalcedItemAddAbil(rkCalculatedTarget, rkItem, pkItemDef, kItor.wType, iValue, decoy);
			}
			else if ( FilterInventoryAbil(kItor.wType) )
			{
				rkFilteredTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else if(::IsMonsterCardAbil(kItor.wType))
			{
				rkCalculatedTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else if(::IsHitAbilFromEquipItem(kItor.wType))
			{// 장비류로 인해 타격시 발동 하는 이펙트 설정시
				rkCalculatedTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else if(IsItemActionAbil(kItor.wType))
			{
				OnEnumEffect(pkItemDef,AT_N_ATTACK_EFFECT_NO_MIN,AT_N_ATTACK_EFFECT_RATE_MIN,AT_N_ATTACK_EFFECT_TARGET_MIN,kContItemActionEffect[IAE_N_ATTACK]);
				OnEnumEffect(pkItemDef,AT_C_ATTACK_EFFECT_NO_MIN,AT_C_ATTACK_EFFECT_RATE_MIN,AT_C_ATTACK_EFFECT_TARGET_MIN,kContItemActionEffect[IAE_C_ATTACK]);
				OnEnumEffect(pkItemDef,AT_BEATTACKED_EFFECT_NO_MIN,AT_BEATTACKED_EFFECT_RATE_MIN,AT_BEATTACKED_EFFECT_TARGET_MIN,kContItemActionEffect[IAE_BEATTACKED]);
				OnEnumEffect(pkItemDef,AT_BLOCKED_EFFECT_NO_MIN,AT_BLOCKED_EFFECT_RATE_MIN,AT_BLOCKED_EFFECT_TARGET_MIN,kContItemActionEffect[IAE_BLOCKED]);
			}
			else if( IsNormalSetAbil(kItor.wType) )
			{
				rkCalculatedTarget.SetAbil(kItor.wType, kItor.iValue);
			}
			else if( IsNormalAddAbil(kItor.wType) )
			{
				rkCalculatedTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else if( ::IsElementAbil(kItor.wType) )
			{
				rkCalculatedTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else
			{
			}
		}
	}

	inline void ApplyAbilAdded(CAbilObject const& rkDefAbil, CAbilObject& rkCalculatedTarget, CAbilObject& rkFilteredTarget, PgBase_Item const& rkItem, CItemDef const* const pkItemDef, CAbilObject kAddedOptAbil)
	{
		SAbilIterator kItor;
		rkDefAbil.FirstAbil(&kItor);
		while( rkDefAbil.NextAbil(&kItor) )
		{
			int const iNewValue = kItor.iValue;
			if( IsCalculateAbilRange(kItor.wType) )
			{
				CalcedItemAddAbil(rkCalculatedTarget, rkItem, pkItemDef, kItor.wType, kItor.iValue, kAddedOptAbil);
			}
			else if( FilterInventoryAbil(kItor.wType) )
			{
				rkFilteredTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else if(::IsMonsterCardAbil(kItor.wType))
			{
				rkCalculatedTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else if( ::IsElementAbil(kItor.wType) )
			{
				rkCalculatedTarget.AddAbil(kItor.wType, kItor.iValue);
			}
			else
			{
				//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Abil[") << kItor.wType << _T("] is not calculated"));
			}
		}
	}

	void ApplyAbil_AddedAbil(CAbilObject &rkTarget, CAbilObject const &kAddedAbil)
	{
		SAbilIterator kItor;
		kAddedAbil.FirstAbil(&kItor);
		while( kAddedAbil.NextAbil(&kItor) )
		{
			switch (kItor.wType)
			{
			case AT_I_PHY_ATTACK_ADD_RATE:
				{
					//Inventory의 AT_PHY_ATTACK_MIN/AT_PHY_ATTACK_MAX 값을 Rate값으로 증폭
					int iAbil = rkTarget.GetAbil(AT_PHY_ATTACK_MIN);
					iAbil += iAbil * kItor.iValue / ABILITY_RATE_VALUE;
					rkTarget.SetAbil(AT_PHY_ATTACK_MIN, iAbil);
					iAbil = rkTarget.GetAbil(AT_PHY_ATTACK_MAX);
					iAbil += iAbil * kItor.iValue / ABILITY_RATE_VALUE;
					rkTarget.SetAbil(AT_PHY_ATTACK_MAX, iAbil);
				}break;
			case AT_I_MAGIC_ATTACK_ADD_RATE:
				{
					//Inventory의 AT_MAGIC_ATTACK_MIN/AT_MAGIC_ATTACK_MAX 값을 Rate값으로 증폭
					int iAbil = rkTarget.GetAbil(AT_MAGIC_ATTACK_MIN);
					iAbil += iAbil * kItor.iValue / ABILITY_RATE_VALUE;
					rkTarget.SetAbil(AT_MAGIC_ATTACK_MIN, iAbil);
					iAbil = rkTarget.GetAbil(AT_MAGIC_ATTACK_MAX);
					iAbil += iAbil * kItor.iValue / ABILITY_RATE_VALUE;
					rkTarget.SetAbil(AT_MAGIC_ATTACK_MAX, iAbil);
				}break;
			case AT_INVEN_PHY_DEFENCE_ADD_RATE:
				{
					// Item : Inventory의 AT_PHY_DEFENCE 값을 Rate값으로 증폭 (모든방어구의 방어력 적용)
					int iAbil = rkTarget.GetAbil(AT_PHY_DEFENCE);
					iAbil += iAbil * kItor.iValue / ABILITY_RATE_VALUE;
					rkTarget.SetAbil(AT_PHY_DEFENCE, iAbil);
				}break;
			case AT_INVEN_MAGIC_DEFENCE_ADD_RATE:
				{
					// Item : Inventory의 AT_MAGIC_DEFENCE 값을 Rate값으로 증폭 (모든방어구의 방어력 적용)
					int iAbil = rkTarget.GetAbil(AT_MAGIC_DEFENCE);
					iAbil += iAbil * kItor.iValue / ABILITY_RATE_VALUE;
					rkTarget.SetAbil(AT_MAGIC_DEFENCE, iAbil);
				}break;
			case AT_INVEN_PET_ADD_TIME_EXP_RATE:
			case AT_INVEN_PET_ADD_HUNT_EXP_RATE:
				{
					if ( ABILITY_RATE_VALUE < kItor.iValue )
					{// 100%이상 증가 안됨
						rkTarget.SetAbil( kItor.wType, ABILITY_RATE_VALUE );
					}
					else
					{
						rkTarget.SetAbil( kItor.wType, kItor.iValue );
					}
				}break;
			case AT_SKILL_EFFICIENCY:
			case AT_ADD_R_NEED_HP:
			case AT_ADD_R_NEED_MP:
			case AT_ADD_ENCHANT_RATE:
			case AT_ADD_SOULCRAFT_RATE:
			case AT_ADD_SOUL_RATE_REFUND:
			case AT_ADD_REPAIR_COST_RATE:

			case AT_HIT_DAMAGE_HP_RATE_ACTIVATE:
			case AT_HIT_DAMAGE_HP_RATE:	
			case AT_HIT_DECREASE_MP_RATE_ACTIVATE:
			case AT_HIT_DECREASE_MP_RATE:
			case AT_CRITICAL_DAMAGE_RATE_ACTIVATE:
			case AT_CRITICAL_DAMAGE_RATE:
			case AT_CRITICAL_DAMAGE_ACTIVATE:
			case AT_ATTACKED_DECREASE_DAMAGE_ACTIVATE_HP_RATE:
			case AT_ATTACKED_DECREASE_DAMAGE_RATE:

			case AT_GIVE_TO_CALLER_ADD_EXP_RATE:
				{
					if ( ABILITY_RATE_VALUE < kItor.iValue )
					{// 100%이상 증가 안됨
						rkTarget.SetAbil( kItor.wType, ABILITY_RATE_VALUE );
					}
					else
					{
						rkTarget.SetAbil( kItor.wType, kItor.iValue );
					}
				}break;
			
			case AT_CRITICAL_DAMAGE:
			
				{
					rkTarget.SetAbil( kItor.wType, kItor.iValue );
				}break;
			default:
				{
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Abil[") << kItor.wType << _T("] is not calculated"));
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}break;
			}
		}
	}

	void ApplyAbil_SetItem(CAbilObject &rkTarget, CAbilObject const &rkSetItemAbil)
	{
		// SetItem Abil 의 전체 Abil에 영향을 미친다.
		SAbilIterator kItor;
		rkSetItemAbil.FirstAbil(&kItor);

		while( rkSetItemAbil.NextAbil(&kItor) )
		{
			if( IsCalculateAbilRange(kItor.wType) || IsMonsterCardAbil(kItor.wType) || IsNormalAddAbil(kItor.wType)
				|| IsElementAbil(kItor.wType))
			{
				rkTarget.AddAbil(kItor.wType, kItor.iValue);
			}
		}
	}

	void MonsterCardAbil_Set(CAbilObject &rkTarget,int const iOrderindex, int const iCardNum)
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);

		CONT_MONSTERCARD const *kContMonsterCard = NULL;
		g_kTblDataMgr.GetContDef(kContMonsterCard);
		if( kContMonsterCard )
		{
			CONT_MONSTERCARD::key_type kKey(iOrderindex, iCardNum);

			CONT_MONSTERCARD::const_iterator iter = kContMonsterCard->find(kKey);
			if( kContMonsterCard->end() != iter )
			{
				CONT_MONSTERCARD::mapped_type const iMonsterCardItemNo = (*iter).second;

				if( iMonsterCardItemNo > 0 )
				{
					int iAbilNum = kItemDefMgr.GetAbil(iMonsterCardItemNo, AT_EFFECTNUM1);

					CONT_DEFEFFECTABIL const *pkDefEffectAbil = NULL;
					g_kTblDataMgr.GetContDef(pkDefEffectAbil);
					if( pkDefEffectAbil )
					{
						CONT_DEFEFFECTABIL::const_iterator iter = pkDefEffectAbil->find(iAbilNum);
						if( pkDefEffectAbil->end() != iter )
						{
							for(int i=0; i<MAX_EFFECT_ABIL_ARRAY; ++i)
							{
								if( iter->second.iType[i] > 0 )
								{
									rkTarget.AddAbil(iter->second.iType[i], iter->second.iValue[i]);
								}
							}
						}
					}
				}
			}
		}
	}

	bool CheckDisableItem(PgBase_Item const & kItem,CUnit const * pkUnit)
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);

		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if(NULL == pItemDef)
		{
			return true;
		}

		E_ITEM_GRADE const eItemGrade = ::GetItemGrade(kItem);
		if(IG_NORMAL > eItemGrade) // 저주나 봉인 상태
		{
			return true;
		}

		if(pkUnit)	// 유닛 정보가 있는 경우 아이템 착용 가능 레밸을 검사한다.
		{
			int const iLvLimit = std::max<int>(0,pItemDef->GetAbil(AT_LEVELLIMIT) - pkUnit->GetAbil(AT_EQUIP_LEVELLIMIT_MIN + pItemDef->EquipPos()));
			int const iLevel = pkUnit->GetAbil(AT_LEVEL);
			if(iLvLimit > iLevel)//레벨검사
			{
				return true;
			}

			int const iLvMaxLimit = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
			if (iLvMaxLimit != 0 &&  iLevel > iLvMaxLimit)
			{
				return true;
			}
		}

		if( (true == kItem.IsUseTimeOut())			|| 
			(true == kItem.EnchantInfo().IsSeal())	|| 
			(true == kItem.EnchantInfo().IsCurse()) ||
			(0 < kItem.EnchantInfo().IsNeedRepair()))
		{
			return true;
		}

		return false;
	}

	void GetOverSkill(CONT_SKILL_OPT & rkOverSkillOpt, int const iOverSkillNo, int const iAddLevel)
	{
		if( (0<iOverSkillNo) && (0!=iAddLevel) )
		{
			auto ret = rkOverSkillOpt.insert(std::make_pair(iOverSkillNo, iAddLevel)); //SkillNo, + Level
			// 이미 값이 중복이라면 누적
			if(!ret.second)
			{
				ret.first->second += iAddLevel;
			}

			// 0이되거나 작아지면 제거 한다.
			if(0 >= ret.first->second)
			{
				rkOverSkillOpt.erase(ret.first);
			}
		}
	}

	HRESULT ApplyAbil(CAbilObject* pkTarget, PgInventory::ItemContainer const* pCont, PgInventory::CONT_SETITEM_NO& rkFullSetCont, CONT_SKILL_OPT& rkOverSkillOpt, PgInventory::CONT_ITEMACTIONEFFECT & rkItemActionEffect, CUnit const * pkUnit, CAbilObject* pkSaveSubtractAbil)
	{
		if (pkTarget == NULL)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		if( !pCont )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		CAbilObject kAddedAbil;
		CAbilObject kSetItemAbil;		// Set item

		//PgInventory::ItemContainer *pCont = NULL;
		//if(S_OK == GetContainer(eInvType, pCont))
		{
			PgFitItem kFitItem(*pCont); // 전체 장착 목록

			PgInventory::ItemContainer::const_iterator item_itor;
			GET_DEF(CItemDefMgr, kItemDefMgr);
			GET_DEF(PgItemOptionMgr, kItemOptionMgr);
			GET_DEF(CItemSetDefMgr, kItemSetDefMgr);

			{
				PgInventory::CONT_SETITEM_NO kSetCont;
				// SetItem
				item_itor = (*pCont).begin();
				while(item_itor != (*pCont).end())
				{
					PgBase_Item const &kItem = (*item_itor).second;
					if(false == CheckDisableItem(kItem,pkUnit))
					{
						int const iSetNo = kItemSetDefMgr.GetItemSetNo(kItem.ItemNo());
						if( iSetNo)
						{
							kSetCont.insert(iSetNo);
						}
					}
					++item_itor;
				}

				PgInventory::CONT_SETITEM_NO::iterator set_itor = kSetCont.begin();
				while(set_itor != kSetCont.end())
				{
					int const iSetNo = (*set_itor);
				
					CItemSetDef const *pOrgSetDef = kItemSetDefMgr.GetDef(iSetNo);

					if(pOrgSetDef)
					{
						bool bCompleteSet = false;
						int const iEquipRate = pOrgSetDef->CheckNeedItem(kFitItem.FitItem(), pkUnit, bCompleteSet);
						if( iEquipRate )
						{
							CItemSetDef const *pEquipSetDef = kItemSetDefMgr.GetEquipAbilDef(iSetNo, iEquipRate);
							if( pEquipSetDef )
							{
								kSetItemAbil.operator +=(*pEquipSetDef);
								if( bCompleteSet )
								{//다 찬것만 풀셋으로 간주
									rkFullSetCont.insert(iSetNo);	
								}
							}
						}
					}
					++set_itor;
				}
			}

			CONT_DEFBASICOPTIONAMP const * pkDefBasicOptionAmp = NULL;
			g_kTblDataMgr.GetContDef(pkDefBasicOptionAmp);

			item_itor = (*pCont).begin();
			while(item_itor != (*pCont).end())
			{
				PgBase_Item const &kItem = (*item_itor).second;

				CItemDef const *pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
				if( pkItemDef )
				{
					E_ITEM_GRADE const eItemGrade = ::GetItemGrade(kItem);

					// 확장 어빌(인챈트 등)
					CAbilObject kOptionAbil;

					// 옵션 증폭 전.
					CAbilObject kAddedOptAbil;

					if(false == CheckDisableItem(kItem,pkUnit))
					{
						ApplyAbilDefault(pkItemDef, *pkTarget, kAddedAbil, rkItemActionEffect, kItem);

						SEnchantInfo const &kEnchantInfo = kItem.EnchantInfo();

	//							TBL_QUAD_KEY_INT
	//							타입, 레벨, pos, 그룹
						int const iEquipPos = pkItemDef->EquipPos();
						int const iOptionGroup = pkItemDef->OptionGroup();

						SItemExtOptionKey kKey1((int)kEnchantInfo.BasicType1(), iEquipPos, (int)kEnchantInfo.BasicLv1(), 0);//베이직은 ext 를 안씀.
						SItemExtOptionKey kKey2((int)kEnchantInfo.BasicType2(), iEquipPos, (int)kEnchantInfo.BasicLv2(), 0);
						SItemExtOptionKey kKey3((int)kEnchantInfo.BasicType3(), iEquipPos, (int)kEnchantInfo.BasicLv3(), 0);
						SItemExtOptionKey kKey4((int)kEnchantInfo.BasicType4(), iEquipPos, (int)kEnchantInfo.BasicLv4(), 0);

						SItemExtOptionKey kExtKey5((int)kEnchantInfo.RareOptType1(), iEquipPos, (int)kEnchantInfo.RareOptLv1(), iOptionGroup);
						SItemExtOptionKey kExtKey6((int)kEnchantInfo.RareOptType2(), iEquipPos, (int)kEnchantInfo.RareOptLv2(), iOptionGroup);
						SItemExtOptionKey kExtKey7((int)kEnchantInfo.RareOptType3(), iEquipPos, (int)kEnchantInfo.RareOptLv3(), iOptionGroup);
						SItemExtOptionKey kExtKey8((int)kEnchantInfo.RareOptType4(), iEquipPos, (int)kEnchantInfo.RareOptLv4(), iOptionGroup);

						CAbilObject kOptBasicAbil;
						kItemOptionMgr.GetBasicAbil(kKey1, &kOptBasicAbil);
						kItemOptionMgr.GetBasicAbil(kKey2, &kOptBasicAbil);
						kItemOptionMgr.GetBasicAbil(kKey3, &kOptBasicAbil);
						kItemOptionMgr.GetBasicAbil(kKey4, &kOptBasicAbil);
						
						CAbilObject kOrigOptionAbil;
						if(pkSaveSubtractAbil)
						{// PVP에서 빼주기 위해 더해진 어빌들을 계산해야 한다면
							kOrigOptionAbil = kOptBasicAbil;
						}
							
						PgItemRarityUpgradeFormula::ApplyBasicOptionAmp(GetEquipType(pkItemDef), pkItemDef->GetAbil(AT_LEVELLIMIT), kEnchantInfo.BasicAmpLv(), kOptBasicAbil, pkDefBasicOptionAmp);
						
						if(pkSaveSubtractAbil)
						{// 추가된 값을 저장하고
							kAddedOptAbil = kOptBasicAbil;	// 증가된 값을 저장하고
							kAddedOptAbil -= kOrigOptionAbil;			// 원래 값을 제거하면 이번에 추가된 값이 나오고
							(*pkSaveSubtractAbil) += kAddedOptAbil;		// 이걸 저장해주고
						}

						kOptionAbil += kOptBasicAbil;

						kItemOptionMgr.GetBasicAbil(kExtKey5, &kOptionAbil);
						kItemOptionMgr.GetBasicAbil(kExtKey6, &kOptionAbil);
						kItemOptionMgr.GetBasicAbil(kExtKey7, &kOptionAbil);
						kItemOptionMgr.GetBasicAbil(kExtKey8, &kOptionAbil);
					
						if( kEnchantInfo.IsPlus() )
						{
							int const iPlusType = static_cast<int>(kEnchantInfo.PlusType()) + ( pkItemDef->IsPetItem() ? SItemEnchantKey::ms_iPetBaseType : 0 );
							int const iPlusLv = static_cast<int>(kEnchantInfo.PlusLv());
							SItemEnchantKey const kKey( iPlusType, iPlusLv );
							
							GET_DEF(CItemEnchantDefMgr, kItemEnchantDefMgr);
							CItemEnchantDef const *pEnchantDef = kItemEnchantDefMgr.GetDef(kKey);

							if(pEnchantDef)
							{
								kOptionAbil.operator +=(*pEnchantDef);
							}
						}

						//몬스터카드가 장착되어있으면
						//몬스터카드에서 아이템 def 얻어서
						//어빌 세팅( IT_FIT || IT_FIT_CASH )
						{
							CONT_MONSTERCARD const *kContMonsterCard = NULL;
							g_kTblDataMgr.GetContDef(kContMonsterCard);

							for(int i=PgSocketFormula::SII_FIRST; i<=PgSocketFormula::SII_THIRD; ++i)
							{
								int iEnchanValue = 0;
								switch( i )
								{
								case PgSocketFormula::SII_FIRST:
									{
										iEnchanValue = kItem.EnchantInfo().MonsterCard();
									}break;
								case PgSocketFormula::SII_SECOND:
									{
										iEnchanValue = kItem.EnchantInfo().MonsterCard2();
									}break;
								case PgSocketFormula::SII_THIRD:
									{
										iEnchanValue = kItem.EnchantInfo().MonsterCard3();
									}break;
								default:
									{
									}break;
								}

								int const iMonsterCardIndex = static_cast<int>( iEnchanValue );
								if( iMonsterCardIndex > 0 )
								{
									bool bIsTimeOut = false;
									SMonsterCardTimeLimit kCard;
									if(true == kItem.Get(kCard))
									{
										bIsTimeOut = kCard.IsUseTimeOut();
									}

									if(false == bIsTimeOut)
									{
										MonsterCardAbil_Set(kOptionAbil, i, iMonsterCardIndex);
									}
								}

								// 몬스터 카드 스킬 옵션
								if( kContMonsterCard )
								{
									CONT_MONSTERCARD::key_type kKey(i, iMonsterCardIndex);

									CONT_MONSTERCARD::const_iterator iter = kContMonsterCard->find(kKey);
									if( kContMonsterCard->end() != iter )
									{
										CONT_MONSTERCARD::mapped_type const iMonsterCardItemNo = (*iter).second;

										if( iMonsterCardItemNo > 0 )
										{
											CItemDef const *pkCardItemDef = kItemDefMgr.GetDef(iMonsterCardItemNo);
											if( pkCardItemDef )
											{
												PgInventoryUtil::GetOverSkill(rkOverSkillOpt, pkCardItemDef->GetAbil(AT_EFFECTNUM9), pkCardItemDef->GetAbil(AT_EFFECTNUM10));
											}
										}
									}
								}
							}
						}

						// 레어 아이템 마법 옵션
						{
							int const iMagicOptIndex = static_cast<int>( kItem.EnchantInfo().MagicOpt() );
							if( iMagicOptIndex > 0 )
							{
								MonsterCardAbil_Set(kOptionAbil, static_cast<int>(PgSocketFormula::SII_FIRST), iMagicOptIndex);
							}
						}

						// 스킬 Level + 옵션
						if(0 != kItem.EnchantInfo().SkillOpt() && 0 != kItem.EnchantInfo().SkillOptLv())
						{
							// 스킬은 인덱스화 되어 있어서 다른 테이블에서 다시 스킬번호를 읽어 와야함
							int const iSkillIndex = static_cast<int>( kItem.EnchantInfo().SkillOpt() );

							CONT_IDX2SKILLNO const* pkDef = NULL;
							g_kTblDataMgr.GetContDef(pkDef);

							if(pkDef)
							{								
								CONT_IDX2SKILLNO::const_iterator itor = pkDef->find(iSkillIndex);
								if(pkDef->end() != itor)
								{
									PgInventoryUtil::GetOverSkill(rkOverSkillOpt, (*itor).second, static_cast<int>(kItem.EnchantInfo().SkillOptLv()));
								}
							}
						}
					}

					ApplyAbilAdded(kOptionAbil, *pkTarget, kAddedAbil, kItem, pkItemDef, kAddedOptAbil);
				}

				++item_itor;
			}
		}

		ApplyAbil_SetItem(*pkTarget, kSetItemAbil);
		ApplyAbil_AddedAbil(*pkTarget, kAddedAbil);
		return S_OK;
	}
};