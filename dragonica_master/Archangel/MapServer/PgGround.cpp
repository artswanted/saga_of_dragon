#include "stdafx.h"
#include <NxBounds3.h>
#include "NiCollision.h"
#include "BM/PgTask.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/packetstruct.h"
#include "FCS/AntiHack.h"
#include "variant/Global.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/PgNpc.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgClassDefMgr.h"
#include "Variant/constant.h"
#include "Variant/MonsterDefMgr.h"
#include "variant/PgGroundItemBoxMgr.h"
#include "variant/ItemBagMgr.h"
#include "variant/PgItemOptionMgr.h"
#include "variant/PgActionResult.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "variant/PgEventview.h"
#include "Variant/PgWorldEvent.h"
#include "variant/PgMacroCheckTable.h"
#include "variant/PgQuestInfo.h"
#include "constant.h"
#include "global.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgTask_Mapserver.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgQuest.h"
#include "PgStoneMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "PgItemTradeMgr.h"
#include "SkillEffectAbilSystem/PgEffectAbilHandleManager.h"
#include "SkillEffectAbilSystem/PgSkillAbilHandleManager.h"
#include "SkillEffectAbilSystem/PgSkillOnFindTargetManager.h"
#include "SkillEffectAbilSystem/PgEffectFunction.h"
#include "PgGroundUnitMgr.h"
#include "PgActionAchievement.h"
#include "PgAction_Pet.h"
#include "publicmap/PgBSGround.h"
#include "variant/PgSocialutil.h"
#include "PgMissionGround.h"
#include "PublicMap/PgWarGround.h"
#include "Variant/PgJobSkillTool.h"
#include "PgActionJobSkill.h"
#include "AI\PgElitePattenManager.h"
#include "PgHackDualKeyboard.h"
#include "PgExpeditionLobby.h"
#include "PgStaticEventGround.h"
#include "PgPVPEffectSelector.h"
#include "PgStaticRaceGround.h"
#include "PgGroundMgr.h"
#include <drem/sys/assert.h>
#include "Effect/ChannelEffect.h"

typedef PgCheckAchievements< PgCheckComplete_Reverse, PgValueCtrl_Base, PgCheckComplete_DelegateMonster > CHECK_ACHIEVEMENTS_DELEGATEMONSTER;
typedef BM::TObjectPool< PgSmallArea > SmallAreaPool;
SmallAreaPool g_kSmallAreaPool(50);


namespace PgGroundUtil
{
	void SafeImportAlwaysDropItemCont(ContAbilValue& rkOut, int iItemNo, size_t iItemCount)
	{
		if(iItemNo == 0) { return; }
		if(iItemCount < 1) { iItemCount = 1; }

		ContAbilValue::iterator itFind = rkOut.find(iItemNo);
		if(itFind != rkOut.end())
		{
			ContAbilValue::value_type::second_type& iCount = itFind->second;
			++iCount;
		}
		else
		{
			ContAbilValue::value_type kNewElem(iItemNo, iItemCount);
			rkOut.insert(kNewElem);
		}
	}

	void GetAlwaysDropItem(CUnit& rkMonster, ContAbilValue &rkOut)
	{
		if(false == rkOut.empty())
		{
			rkOut.swap(ContAbilValue());
		}

		SafeImportAlwaysDropItemCont(rkOut, rkMonster.GetAbil(AT_ALWAYS_DROP_ITEM_1));
		SafeImportAlwaysDropItemCont(rkOut, rkMonster.GetAbil(AT_ALWAYS_DROP_ITEM_2));
		SafeImportAlwaysDropItemCont(rkOut, rkMonster.GetAbil(AT_ALWAYS_DROP_ITEM_3));
		SafeImportAlwaysDropItemCont(rkOut, rkMonster.GetAbil(AT_ALWAYS_DROP_ITEM_4));
		SafeImportAlwaysDropItemCont(rkOut, rkMonster.GetAbil(AT_ALWAYS_DROP_ITEM_5));

		int const iMaxCount = AT_ALWAYS_DROP_CUSTOM_ITEM_5 - AT_ALWAYS_DROP_CUSTOM_ITEM_1;
		for(int i = 0; i <= iMaxCount; i++)
		{
			SafeImportAlwaysDropItemCont(rkOut, rkMonster.GetAbil(AT_ALWAYS_DROP_CUSTOM_ITEM_1 + i), rkMonster.GetAbil(AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_1 + i));
		}
	}

	bool InsertAlwaysDropItem(CUnit * pkUnit, int const iItemNo, size_t iItemCount)
	{
		if(!pkUnit)
		{
			return false;
		}

		if(iItemCount < 1) { iItemCount = 1; }

		int const iMaxCount = AT_ALWAYS_DROP_CUSTOM_ITEM_5 - AT_ALWAYS_DROP_CUSTOM_ITEM_1;
		for(int i = 0; i <= iMaxCount; i++)
		{
			if(0 == pkUnit->GetAbil(AT_ALWAYS_DROP_CUSTOM_ITEM_1 + i))
			{
				pkUnit->SetAbil(AT_ALWAYS_DROP_CUSTOM_ITEM_1 + i, iItemNo);
				pkUnit->SetAbil(AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_1 + i, iItemCount);
				return true;
			}
		}
		return false;
	}

	void AddRegenTimeEffectReserve(CUnit* pkUnit, WORD const wEffectSlotAbilType)
	{
		if( pkUnit )
		{
			int const iRegenAddEffectNo = pkUnit->GetAbil(wEffectSlotAbilType);
			if( 0 < iRegenAddEffectNo )
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iRegenAddEffectNo);
				pkUnit->AddEffectQueue(kData);
			}
		}
	}

	bool InsertEffectAbil(CUnit * pkUnit, int const iEffectNo)
	{
		if(!pkUnit || 0==iEffectNo)
		{
			return false;
		}

		for(int i=0; i<=EFFECTNUM_MAX; ++i)
		{
			if(0==pkUnit->GetAbil(AT_EFFECTNUM1+i))
			{
				pkUnit->SetAbil(AT_EFFECTNUM1+i, iEffectNo);
				return true;
			}
		}
		return false;
	}

	void DeleteEffect(CUnit* pkUnit, WORD const wEffetHaveAbilType)
	{
		if( !pkUnit )
		{
			return;
		}

		PgUnitEffectMgr& rkEffectMgr = pkUnit->GetEffectMgr();
		ContEffectItor effect_iter;
		rkEffectMgr.GetFirstEffect(effect_iter);
		CEffect* pkEffect = NULL;
		while( (pkEffect = rkEffectMgr.GetNextEffect(effect_iter)) != NULL )
		{
			if( pkEffect )
			{
				if( 0 < pkEffect->GetAbil(wEffetHaveAbilType) )
				{
					pkUnit->DeleteEffect(pkEffect->GetEffectNo());
				}
			}
		}
	}

	void DeleteInvenItem(CUnit* pkUnit, EInvType const eFromInvType, WORD const wItemHaveAbilType, CItemDefMgr const& rkItemDefMgr, SGroundKey const& rkGndKey)
	{
		if( !pkUnit )
		{
			return;
		}

		CONT_PLAYER_MODIFY_ORDER kOrder;
		PgInventory* pkInven = pkUnit->GetInven();
		if( pkInven )
		{
			ContHaveItemNoCount kHaveItems;
			pkInven->GetItems(eFromInvType, kHaveItems);

			ContHaveItemNoCount::const_iterator item_iter = kHaveItems.begin();
			while( kHaveItems.end() != item_iter )
			{
				int const iHaveItemNo = (*item_iter).first;
				int const iHaveCount = static_cast< int >((*item_iter).second);
				CItemDef const* pItemDef = rkItemDefMgr.GetDef(iHaveItemNo);
				if( pItemDef )
				{
					if( 0 < pItemDef->GetAbil(wItemHaveAbilType) )
					{
						tagPlayerModifyOrderData_Add_Any kDelData(iHaveItemNo, -iHaveCount);

						SPMO kIMO(IMET_ADD_ANY, pkUnit->GetID(), kDelData);
						kOrder.push_back(kIMO);
					}
				}
				++item_iter;
			}
		}

		if( !kOrder.empty() )
		{
			PgAction_ReqModifyItem kItemModifyAction(IMPET_MapmoveDeleteItem, rkGndKey, kOrder);
			kItemModifyAction.DoAction(pkUnit, NULL);
		}
	}

	void DeleteInvenCoupleItem(CUnit* pkUnit, WORD const wItemHaveAbilType, CItemDefMgr const& rkItemDefMgr, SGroundKey const& rkGndKey)
	{
		if( !pkUnit )
		{
			return;
		}

		CONT_PLAYER_MODIFY_ORDER kOrder;
		PgInventory* pkInven = pkUnit->GetInven();
		if( pkInven )
		{
			ContHaveItemNoCount kHaveItems;
			pkInven->GetItems(IT_FIT, kHaveItems);
			pkInven->GetItems(IT_FIT_CASH, kHaveItems);
			pkInven->GetItems(IT_EQUIP, kHaveItems);
			pkInven->GetItems(IT_CASH, kHaveItems);

			pkInven->GetItems(IT_SAFE, kHaveItems);
			pkInven->GetItems(IT_CASH_SAFE, kHaveItems);
			pkInven->GetItems(IT_RENTAL_SAFE1, kHaveItems);
			pkInven->GetItems(IT_RENTAL_SAFE2, kHaveItems);
			pkInven->GetItems(IT_RENTAL_SAFE3, kHaveItems);
			pkInven->GetItems(IT_PET, kHaveItems);
			pkInven->GetItems(IT_SAFE_ADDITION, kHaveItems);

			ContHaveItemNoCount::const_iterator item_iter = kHaveItems.begin();
			while( kHaveItems.end() != item_iter )
			{
				int const iHaveItemNo = (*item_iter).first;
				int const iHaveCount = static_cast< int >((*item_iter).second);
				CItemDef const* pItemDef = rkItemDefMgr.GetDef(iHaveItemNo);
				if( pItemDef )
				{
					if( 0 < pItemDef->GetAbil(wItemHaveAbilType) )
					{
						tagPlayerModifyOrderData_Add_Any kDelData(iHaveItemNo, -iHaveCount);

						SPMO kIMO(IMET_ADD_ANY, pkUnit->GetID(), kDelData);
						kOrder.push_back(kIMO);
					}
				}
				++item_iter;
			}
		}

		if( !kOrder.empty() )
		{
			PgAction_ReqModifyItem kItemModifyAction(IMPET_MapmoveDeleteItem, rkGndKey, kOrder);
			kItemModifyAction.DoAction(pkUnit, NULL);
		}
	}


	//
	PgUnitSepcailStatusHandler::PgUnitSepcailStatusHandler()
	{
	}

	PgUnitSepcailStatusHandler::~PgUnitSepcailStatusHandler()
	{
	}

	ESpecialUnitStatusType PgUnitSepcailStatusHandler::EnterSpecStatus(BM::GUID const& rkGuid, ESpecialUnitStatusType const eEnterType, bool const bChange )
	{
		BM::CAutoMutex kLock(m_kUnitSpecialStatusMutex);
		auto kRet = m_kContSpecStatus.insert( std::make_pair(rkGuid, eEnterType) );
		if( !kRet.second )
		{
			ContUnitSpecialStatus::mapped_type& rkType = (*kRet.first).second;
			if( (true == bChange) || (SUST_None == rkType) )
			{
				rkType = eEnterType; //
			}
			else
			{
				return rkType; // �̹� �ٸ� ���� �ִ�.
			}
		}
		return eEnterType;
	}

	bool PgUnitSepcailStatusHandler::ChangeSpecStatus( BM::GUID const &rkGuid, ESpecialUnitStatusType const eCheckType, ESpecialUnitStatusType const eChangeType )
	{
		BM::CAutoMutex kLock(m_kUnitSpecialStatusMutex);
		ContUnitSpecialStatus::iterator itr = m_kContSpecStatus.find(rkGuid);
		if( itr != m_kContSpecStatus.end()  )
		{
			if ( eCheckType == itr->second )
			{
				itr->second = eChangeType;
				return true;
			}
		}
		return false;
	}

	bool PgUnitSepcailStatusHandler::LeaveSpecStatus(BM::GUID const& rkGuid, ESpecialUnitStatusType const eLeaveType)
	{
		BM::CAutoMutex kLock(m_kUnitSpecialStatusMutex);
		ContUnitSpecialStatus::iterator iter = m_kContSpecStatus.find(rkGuid);
		if( m_kContSpecStatus.end() != iter )
		{
			ESpecialUnitStatusType const eType = (*iter).second;
			if( eLeaveType == eType )
			{
				m_kContSpecStatus.erase(iter);
				return true;
			}
		}
		return false;
	}

	ESpecialUnitStatusType PgUnitSepcailStatusHandler::GetSpecStatus(BM::GUID const& rkGuid) const
	{
		BM::CAutoMutex kLock(m_kUnitSpecialStatusMutex);
		ContUnitSpecialStatus::const_iterator iter = m_kContSpecStatus.find(rkGuid);
		if( m_kContSpecStatus.end() != iter )
		{
			return (*iter).second;
		}
		return SUST_None;
	}

	void PgUnitSepcailStatusHandler::ClearSpecStatus(BM::GUID const& rkGuid)
	{
		BM::CAutoMutex kLock(m_kUnitSpecialStatusMutex);
		ContUnitSpecialStatus::iterator iter = m_kContSpecStatus.find(rkGuid);
		if( m_kContSpecStatus.end() != iter )
		{
			m_kContSpecStatus.erase(iter);
		}
	}

	SummonOptionType GetSummonOptionType(bool const bUniqueClass)
	{
		int iSummonType = ESO_NONE;
		if(bUniqueClass)
		{
			iSummonType |= ESO_UNIQUE_CLASS;
		}
		return static_cast<SummonOptionType>(iSummonType);
	}

	void SendAddSimpleUnit(PgNetModule<> const &kNetModule, UNIT_PTR_ARRAY const& kUnitArray, bool const bMustNowSend, bool const bMustSend)
	{
		UNIT_PTR_ARRAY::const_iterator add_itor = kUnitArray.begin();
		while(add_itor != kUnitArray.end())
		{
			if(add_itor->pkUnit)
			{
				static size_t const unit_count = 1;
				BM::Stream kAPacket(PT_M_C_ADD_UNIT);
				kAPacket.Push(unit_count);
				add_itor->pkUnit->WriteToPacket(kAPacket,WT_SIMPLE);
				kNetModule.Send( kAPacket, bMustNowSend, bMustSend );
			}
			++add_itor;
		}
	}

	void SendAddSimpleUnit(CUnit * pkUnit, UNIT_PTR_ARRAY const& kUnitArray, DWORD const dwSendFlag)
	{
		UNIT_PTR_ARRAY::const_iterator add_itor = kUnitArray.begin();
		while(add_itor != kUnitArray.end())
		{
			if(add_itor->pkUnit)
			{
				static size_t const unit_count = 1;
				BM::Stream kAPacket(PT_M_C_ADD_UNIT);
				kAPacket.Push(unit_count);
				add_itor->pkUnit->WriteToPacket(kAPacket,WT_SIMPLE);
				pkUnit->Send(kAPacket, dwSendFlag);
			}
			++add_itor;
		}
	}

	void SendPartyMgr_Refuse(BM::GUID const& rkCharGuid, EPartyRefuse const eType, bool const bRefuse)
	{
		BM::Stream kPacket(PT_M_N_REQ_JOIN_PARTY_REFUSE);
		kPacket.Push(rkCharGuid);
		kPacket.Push(bRefuse);
		kPacket.Push(static_cast<BYTE>(eType));
		SendToGlobalPartyMgr(kPacket);
	}

	void GetUnitInRange(CUnit * pkUnit, CUnit * pkTarget, POINT3 const& rkPos, UNIT_PTR_ARRAY &rkUnitArray, int const iRange, int const iZLimit = 0)
	{
		if(pkUnit && pkTarget && pkUnit != pkTarget)
		{
			if(0 < iRange)
			{
				if(ERange_OK==::IsInRange(rkPos, pkTarget->GetPos(), iRange, iZLimit))
				{
					rkUnitArray.AddToUniqe(pkTarget);
				}
			}
			else
			{
				rkUnitArray.AddToUniqe(pkTarget);
			}
		}
	}

	void SetActArgGround(SActArg & kArg, PgGround const* pkGround)
	{
		if(pkGround)
		{
			kArg.Set(ACTARG_GROUND, pkGround);
			kArg.Set(ACTARG_GROUND_NO, pkGround->GetGroundNo());
		}
	}
};

void OnDoEffectProc(EItemActionEvent const kEvent,CUnit * pkCaster,CUnit * pkTarget)
{
	PgInventory::CONT_ITEMACTIONEFFECT const & kCont = pkCaster->GetInven()->ItemActionEffect();

	PgInventory::CONT_ITEMACTIONEFFECT::const_iterator iter = kCont.find(kEvent);

	if(iter == kCont.end())
	{
		return;
	}

	PgInventory::CONT_ITEMACTIONEFFECT::mapped_type const & kContEffect = (*iter).second;

	for(PgInventory::CONT_ITEMACTIONEFFECT::mapped_type::const_iterator eiter = kContEffect.begin();eiter != kContEffect.end();++eiter)
	{
		int const & iEffectNo = (*eiter).iEffectNo;
		int const & iRate = (*eiter).iRate;
		int const & iTarget = (*eiter).iTarget;

		if(0 == iEffectNo)
		{
			continue;
		}

		if(iRate < BM::Rand_Index(ABILITY_RATE_VALUE))
		{
			continue;
		}

		SEffectCreateInfo kCreate;
		kCreate.eType = EFFECT_TYPE_NORMAL;
		kCreate.iEffectNum = iEffectNo;
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

		if(0 < iTarget)
		{
			pkCaster->AddEffect(kCreate);
		}
		else
		{
			pkTarget->AddEffect(kCreate);
		}
	}
}

void OnCriticalHit(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	OnDoEffectProc(IAE_C_ATTACK, pkCaster, pkTarget);
}

void OnNormalHit(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	OnDoEffectProc(IAE_N_ATTACK, pkCaster, pkTarget);
}

void OnOffenseUnitEffectProc(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	if(true == pkActionResult->GetCritical())
	{
		OnCriticalHit(pkCaster,pkTarget,pkActionResult);
		return;
	}

	OnNormalHit(pkCaster,pkTarget,pkActionResult);
}

void OnBlocked(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	OnDoEffectProc(IAE_BLOCKED, pkCaster, pkTarget);
}

void OnDefenseUnitEffectProc(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	if(0 < pkActionResult->GetBlocked())
	{
		OnBlocked(pkCaster,pkTarget,pkActionResult);
		return;
	}

	OnDoEffectProc(IAE_BEATTACKED, pkCaster, pkTarget);
}

void OnItemEffectProc(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	if(true == pkActionResult->GetInvalid() || true == pkActionResult->GetMissed())
	{
		return;
	}

	OnOffenseUnitEffectProc(pkCaster,pkTarget,pkActionResult);
	OnDefenseUnitEffectProc(pkCaster,pkTarget,pkActionResult);
}

void OnEffectAbilProc(CUnit * pkCaster,CUnit * pkTarget, WORD const wAbil, bool const bGetAbilerIsCaster)
{//bGetAbilerIsCaster ����� ĳ���Ϳ��� ��� ���°�?
	CUnit* pkUnit = bGetAbilerIsCaster ? (pkCaster) : (pkTarget);
	CUnit* pkUnit2 = bGetAbilerIsCaster ? (pkTarget) : (pkCaster);

	if(pkUnit)
	{
		// EffectNo
		// Rate
		// IsMe 3���� ��Ʈ

		for(int i = wAbil; i < wAbil + 10; ++i) //10������ ���� �ǵ��� ��.
		{
			int const iAddEffect = pkUnit->GetAbil(i);			
			if(0 < iAddEffect)
			{
				int const iRate = pkUnit->GetAbil(i + 10);			// AT_DMG_ADD_EFFECT_01_RATE
				if(iRate >= BM::Rand_Index(ABILITY_RATE_VALUE)) // 10000�з�
				{
					int const iIsTarget = pkUnit->GetAbil(i + 20); // AT_DMG_ADD_EFFECT_01_TARGET_TYPE_IS_ME
					CUnit* pkTargetUnit = (iIsTarget) ? (pkUnit) : (pkUnit2);
					if(pkTargetUnit)
					{
						SEffectCreateInfo kCreate;
						kCreate.eType = EFFECT_TYPE_NORMAL;
						kCreate.iEffectNum = iAddEffect;
						kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

						pkTargetUnit->AddEffect(kCreate);
					}
				}
			}
		}
	}
}

void OnEffectProc(CUnit * pkCaster,CUnit * pkTarget,PgActionResult* pkActionResult)
{
	if(true == pkActionResult->GetInvalid() || true == pkActionResult->GetMissed())
	{
		return;
	}

	if(pkActionResult)
	{
		if(pkActionResult->GetCritical())
		{
			OnEffectAbilProc(pkCaster, pkTarget, AT_CRITICAL_HIT_ADD_EFFECT_01, true);
		}
	}
	
	OnEffectAbilProc(pkCaster, pkTarget, AT_DMG_ADD_EFFECT_01, false);
}

extern void OnAttacked(CUnit* pkUnit, UNIT_PTR_ARRAY* pkUnitArray, int const iSkillNo, PgActionResultVector* pkResult, SActArg* pArg, DWORD const dwTimeStamp)
{
	//Caster�� ������ �ణ �̵�
	//Target�� �ڷ� �ణ �̵�
	if (!pkUnit || !pkUnitArray || !pkResult)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("OnAttacked Failed!"));
		return;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	int iCasterLen = pkSkill->GetAbil(AT_CASTER_MOVERANGE);
	int iTargetLen = pkSkill->GetAbil(AT_TARGET_MOVERANGE);

	int iIndex = 0;

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();

	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (pkTarget)
		{
			PgActionResult* pkActionResult = pkResult->GetResult(pkTarget->GetID(), true);
			if (pkActionResult)
			{
				//RemainHP �� ����
				pkActionResult->SetRemainHP(pkTarget->GetAbil(AT_HP));
// 				if (0 < pkTarget->GetAbil(AT_MAX_DP))
// 				{
// 					pkActionResult->SetAbil(AT_DP, 0);
// 				}
				//�ϴ��� Damage>0�̸�, Target �з�������
				if (!pkActionResult->GetInvalid() && 0 < pkActionResult->GetValue() && iTargetLen)
				{
					POINT3 kNow = pkUnit->GetPos();
					POINT3 const& rkTarget = pkTarget->GetPos();
					NxVec3 kDirection(rkTarget.x-kNow.x, rkTarget.y-kNow.y, rkTarget.z-kNow.z);
					kDirection.normalize();
					kDirection = kDirection * (float)iTargetLen * -1;
					kNow.x += kDirection.x; kNow.y += kDirection.y; kNow.z += kDirection.z;
					pkUnit->SetPos(kNow);
				}

				if (0==iIndex && iCasterLen)
				{
					POINT3 kNow = pkUnit->GetPos();
					POINT3 const& rkTarget = pkTarget->GetPos();
					NxVec3 kDirection(rkTarget.x-kNow.x, rkTarget.y-kNow.y, rkTarget.z-kNow.z);
					kDirection.normalize();
					kDirection = kDirection * (float)iCasterLen;
					kNow.x += kDirection.x; kNow.y += kDirection.y; kNow.z += kDirection.z;
					pkUnit->SetPos(kNow);
				}

				int const iReflect = pkActionResult->GetAbil(AT_REFLECTED_DAMAGE);
				if (0 < iReflect)
				{
					void* pkVoid = NULL;
					pArg->Get(ACTARG_GROUND, pkVoid);
					PgGround* pkGround = (PgGround*)pkVoid;
					if (pkGround)
					{
						CUnit* pkCaller = pkUnit;

						//Caller�� ��� ������ �Ե��� �����Ǿ�������
						if(0 < pkUnit->GetAbil(AT_DELIVER_DAMAGE_CALLER))
						{
							BM::GUID const& rkCaller = pkUnit->Caller();
							pkCaller = pkGround->GetUnit(rkCaller);
							if(NULL == pkCaller)
							{
								pkCaller = pkUnit;
							}
							else
							{
								//�ٲ� Ÿ���� ���� HP�� �����Ѵ�.
								pkActionResult->SetAbil(AT_REFLECT_DMG_HP, pkCaller->GetAbil(AT_HP) - iReflect);
							}
						}
	
						pkActionResult->SetAbil(AT_REFLECT_DMG_HP, ::OnDamaged(pkTarget, pkCaller, iSkillNo, iReflect, pkGround, dwTimeStamp, true) );
					}
				}
				else
				{
					pkActionResult->SetAbil(AT_REFLECTED_DAMAGE, 0);
					pkActionResult->SetAbil(AT_REFLECT_DMG_HP, 0);
				}

				OnItemEffectProc(pkUnit,pkTarget,pkActionResult);
				OnEffectProc(pkUnit,pkTarget,pkActionResult);
			}
		}
		++iIndex;
		++unit_itor;
	}
}

AntiHackCheckActionCount PgGround::ms_kAntiHackCheckActionCount;
AntiHackCheckVariable PgGround::ms_kAntiHackCheckVariable;

float PgGround::ms_fAutoHealMultiplier_NotInViliage = 0.0f;

int PgGround::ms_iEnableEntityCritialAttack = 0;
int PgGround::ms_iEnableEntityHitRate = 0;
int PgGround::ms_iEnableEntityAttackExtInfo = 0;

int PgGround::ms_iHiddenRewordItemNo = 0;
T_GNDATTR PgGround::ms_kOffPet_GroundAttr = GATTR_FLAG_PVP|GATTR_FLAG_EMPORIABATTLE|GATTR_FLAG_BATTLESQUARE;

float	PgGround::ms_KnightAwakeReviserByKill		= 0.0f;
float	PgGround::ms_WarriorAwakeReviserByKill		= 0.0f;
float	PgGround::ms_MageAwakeReviserByKill			= 0.0f;
float	PgGround::ms_BattleMageAwakeReviserByKill	= 0.0f;
float	PgGround::ms_HunterAwakeReviserByKill		= 0.0f;
float	PgGround::ms_RangerAwakeReviserByKill		= 0.0f;
float	PgGround::ms_ClownAwakeReviserByKill		= 0.0f;
float	PgGround::ms_AssassinAwakeReviserByKill		= 0.0f;
float	PgGround::ms_ShamanAwakeReviserByKill		= 0.0f;
float	PgGround::ms_DoubleFighterAwakeReviserByKill= 0.0f;

int		PgGround::ms_JobSkillBaseExpDiff[] = {0, };

PgGround::PgGround()	//�������� ��ģ��.
:	m_iMapLevel(0)
,	m_iGroundWeight(0)
,	m_iTunningLevel(0)
,	m_iGroundTunningNo(0)
,	m_iMonsterControlID(0)
,	m_kLocalPartyMgr()
,	m_pkBigArea(NULL)
,	m_dwOnTick1s_Check2s(BM::GetTime32())
,	m_kDynamicGndAttr(DGATTR_NONE)
,	m_kEventQuest()
,	m_kWorldEnvironmentStatus()
,   m_bRunAI(true)
,	m_i64EventViewOldTime(0i64)
,	m_bUpdateMonsterCount(false)
,	m_bIsBossRoom(false)
,	m_iMapMoveResultStartTimeSec(0)
,	m_kIndunPartyDie(false)
,	m_kPaused(false)
{
	__int64 const i64Now = BM::GetTime64();
	DWORD const dwNow = BM::GetTime32();
	LastGenCheckTime(dwNow);
	LastPlayTimeCalculateTime(i64Now);
	LastAddExpInVillageTickTime(dwNow);
}

PgGround::~PgGround()
{
	ReleaseZone();
	m_kAIMng.Release();
}

void PgGround::ProcessMonsterGenNfy(int const iGenGroupNo,E_MONSTER_GEN_MODE const kMode)
{
	switch(kMode)
	{
	case MGM_GEN:
		{
			ActivateMonsterGenGroup( iGenGroupNo, false, false, 0 );
			ActivateObjectUnitGenGroup(iGenGroupNo, false, false);
		}break;
	case MGM_REMOVE:
		{
			RemoveGenGroupMonster(iGenGroupNo,true);
			RemoveGenGroupObject(iGenGroupNo);
		}break;
	}
}

void PgGround::OnRecvMonsterGenNfy(int const iGenGroupNo,E_MONSTER_GEN_MODE const kMode)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	ProcessMonsterGenNfy(iGenGroupNo,kMode);
}

void PgGround::OnActivateEventMonsterGroup()
{
	CONT_NFY_MONSTERGEN kMonGenCont;
	g_kEventView.GetContMonsterGenNfy(kMonGenCont);

	CONT_NFY_MONSTERGEN::const_iterator monster_iter = kMonGenCont.begin();
	while(monster_iter != kMonGenCont.end())
	{
		PgGround::ProcessMonsterGenNfy((*monster_iter).iMonsterGonGroupNo,(*monster_iter).kMonsterGenMode);
		++monster_iter;
	}
}

EOpeningState PgGround::Init( int const iMonsterControlID, bool const bMonsterGen )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	m_iMonsterControlID = iMonsterControlID;
	LastGenCheckTime(0);

	m_kContLastTickTime.resize(ETICK_INTERVAL_MAX, BM::GetTime32());

	m_dwTimeStamp = 0;
	m_kContGenPoint_Monster.clear();
	m_kContGenPoint_Object.clear();
	m_kContGenPoint_SummonNPC.clear();
	m_ContDeleteReserveInsItem.clear();
	m_CurrentEventMapUserCountList.clear();
	m_EventMapEnterableList.clear();

	DefendModeTickTime(0);

    m_kContEventScriptPlayer.clear();
	m_i64EventViewOldTime = g_kEventView.GetLocalSecTime( CGameTime::MILLISECOND );
	m_bUpdateMonsterCount = false;

	if ( FAILED(BuildZone()) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"BuildZone Failed, Ground["<<GetGroundNo()<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_OPENING_ERROR"));
		return E_OPENING_ERROR;
	}

	InitRareMonsterGen();
	InitEventMonsterGen();
	DropAnyMonsterTimeCheck();

	ActivateInstanceItemGenGroup(EIGG_ANY, true, false, 0);

	HRESULT hRet = E_OPENING_NONE;
	if ( bMonsterGen )
	{
		hRet = ActivateMonsterGenGroup( 0, true, false, 0 );
		OnActivateEventMonsterGroup();
		RareMonsterGenerate();
	}

	{
		GET_DEF_CUSTOM(PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
		kGenPointMgr.GetGenPoint_Object( m_kContGenPoint_Object, GetGroundNo() );
	}

	{
		GET_DEF_CUSTOM(PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
		kGenPointMgr.GetGenPoint_SummonNPC( m_kContGenPoint_SummonNPC, GetGroundNo() );
	}
	
	CONT_MAP_ENTITY const *pkMapEntity = NULL;
	g_kTblDataMgr.GetContDef( pkMapEntity );
	if ( pkMapEntity )
	{// Map Entity ����

		CONT_MAP_ENTITY::const_iterator mapentity_itr = std::lower_bound( pkMapEntity->begin(), pkMapEntity->end(), CONT_MAP_ENTITY::value_type(GetGroundNo()) );
		while ( (mapentity_itr != pkMapEntity->end()) && (mapentity_itr->iGroundNo == GetGroundNo()) )
		{
			CONT_MAP_ENTITY::value_type const &kElement = *mapentity_itr;

			// Entity�� ������ �Ѵ�.
			SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey( mapentity_itr->iClass, mapentity_itr->nLevel ) );
			kCreateInfo.ptPos = mapentity_itr->pt3Pos;
			kCreateInfo.bEternalLife = true;

			if ( !IsCorrectPos( kCreateInfo.ptPos ) )
			{// ������ġ�� �ƴϸ� Spawn1�� �����ǵ��� �����.
				FindSpawnLoc(1, kCreateInfo.ptPos );
			}

			CUnit *pkCaller = NULL;
			if ( BM::GUID::IsNotNull( kElement.kOwnerPointGuid ) )
			{
				ContGenPoint_Monster::const_iterator gen_itr = m_kContGenPoint_Monster.find( kElement.kOwnerPointGuid );
				if ( gen_itr != m_kContGenPoint_Monster.end() )
				{
					ContGenPoint_Monster::mapped_type const &kGenPoint = gen_itr->second;

					OwnChildCont kContChildID;
					kGenPoint.GetChild( kContChildID );

					OwnChildCont::const_iterator child_itr = kContChildID.begin();
					for ( ; child_itr != kContChildID.end() ; ++child_itr )
					{
						pkCaller = PgObjectMgr2::GetUnit( UT_MONSTER, *child_itr );
						if ( pkCaller )
						{
							break;
						}
					}

					if ( !pkCaller )
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, L"Not Found MapEntity' Owner Monster OwnerPointGuid<" << kElement.kOwnerPointGuid << L"> GroundNo<" << GetGroundNo() << L">" );
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, L"Not Found MapEntity OwnerPointGuid<" << kElement.kOwnerPointGuid << L"> GroundNo<" << GetGroundNo() << L">" );
				}
			}

			CUnit *pkUnit = CreateEntity( pkCaller, &kCreateInfo, NULL, false );
			if ( pkUnit )
			{
				PgEntity *pkEntity = dynamic_cast<PgEntity*>(pkUnit);
				if ( pkEntity )
				{
					pkEntity->VisualFieldMin( mapentity_itr->pt3Min );
					pkEntity->VisualFieldMax( mapentity_itr->pt3Max );
					pkEntity->UseVisualField( true );
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Entity["<<pkUnit->GetID()<<L"] dynamic_cast Error!!" );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEntity is NULL"));
				}

				if ( mapentity_itr->iEffect > 0 )
				{
					SEffectCreateInfo kCreate;
					kCreate.eType = EFFECT_TYPE_BLESSED;
					kCreate.iEffectNum = mapentity_itr->iEffect;
					PgGroundUtil::SetActArgGround(kCreate.kActArg, this);
					kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					pkUnit->AddEffect( kCreate );
				}
			}
			++mapentity_itr;
		}
	}

	if (!m_kAIMng.Init(this))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_OPENING_ERROR"));
		return E_OPENING_ERROR;
	}
	if( !m_kEventItemSetMgr.Build() )
	{
		ASSERT_LOG(false, BM::LOG_LV2, __FL__ << _T("EventItemSetMgr Build Error"));
		return E_OPENING_ERROR;
	}

	POINT3 rkOut;
	PgSmallArea* pRetArea;
	if(S_OK == FindSpawnLoc(1, rkOut))
	{//���� �����̼��� �ִ��� Ȯ��.
		POINT3 ptTemp;
		if( S_OK != GetArea(rkOut, ptTemp, pRetArea, false))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__<<L"SpawnLoc(1) is Invalid Area Ground["<<GetGroundNo() <<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetArea Failed!"));
		}
	}
	else
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"SpawnLoc(1) is not exist Ground["<<GetGroundNo()<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("FindSpawnLoc Failed!"));
	}

	CONT_MAP_STONE_CONTROL const * pkContDefStoneCtrl = NULL;
	g_kTblDataMgr.GetContDef(pkContDefStoneCtrl);

	CONT_MAP_STONE_CONTROL::const_iterator stone_ctrl_itor =  pkContDefStoneCtrl->find(GetGroundNo());

	if(stone_ctrl_itor != pkContDefStoneCtrl->end())
	{
		m_kStoneCtrl.Init((*stone_ctrl_itor).second);
	}

	JobSkillLocationItemInit();

	return (EOpeningState)hRet;
}

HRESULT PgGround::GetArea(POINT3 const &rkPos, POINT3& rkOut, PgSmallArea* &pRetArea, bool const bIsMoveToSpawn, bool bCheckPos)
{
	if(bCheckPos && !IsCorrectPos(rkPos))//�������� �ƴ϶�� Spawn_1�� ������
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("IncorrectPos MapNo=") << GetGroundNo() << _T(", Pos[") << rkPos << _T("]"));
		goto __FAILED;
	}

	if( rkPos == POINT3::NullData() )//0,0,0 �̸� Spawn_1�� ������
	{
		INFO_LOG(BM::LOG_LV7, __FL__<<L"NullPoint ["<<GetGroundNo()<<L"]["<<rkPos<<L"]");
		goto __FAILED;
	}

	PgSmallArea* pkArea = GetArea( m_kSmallAreaInfo.GetAreaIndexFromPos( rkPos ) );//���� ��ġ�� AreaŰ�� �ٲ㺻��.
	//if(pkArea != NULL)
	{
		// pkArea == NULL �� ���� �ִ�.
		pRetArea = pkArea;
		return S_OK;
	}

	INFO_LOG(BM::LOG_LV5, __FL__ << _T("Incorrect MapNo=") << GetGroundNo() << _T(", Pos[") << rkPos << _T("]"));
	goto __FAILED;

__FAILED://������ ����Ʈ�� �ƴϾ���.
	{	//	rkOut = rkPos;
		if(bIsMoveToSpawn)
		{
			if(S_OK == FindSpawnLoc(1, rkOut))
			{
				//				INFO_LOG(BM::LOG_LV0, _T("[%s] Try Force Move To Spawn [%d][%0.f, %0.f, %0.f]"), __FUNCTIONW__, GroundKey().iGroundNo, rkOut.x, rkOut.y, rkOut.z);
				POINT3 ptTemp;
				if( S_OK == GetArea(rkOut, ptTemp, pRetArea, false, bCheckPos))
				{	
					return GA_FORCE_MOVE_TO_SPAWN;
				}
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Ground["<<GetGroundNo()<<L"] Can't Find SpawnPoint" );
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

#ifdef ZONE_OPTIMIZE_20090624

HRESULT PgGround::BuildZone()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	INFO_LOG(BM::LOG_LV6, __FL__ << _T("Begin Ground = ") << GroundKey().GroundNo());
	POINTER_VERIFY_INFO_LOG(m_spSceneRoot);
	if(!m_spSceneRoot)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	NiNode *pkNode = (NiNode *)m_spSceneRoot;//->GetObjectByName("physx");
	if(!pkNode)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot GetObject physx");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// ���� �ٿ���� ������ �޴´�.
	//const NiBound& kBound = m_spSceneRoot->GetWorldBound();
	//int iRadius = (int)kBound.GetRadius();
	//POINT3 const kCenter(kBound.GetCenter().x, kBound.GetCenter().y, kBound.GetCenter().z);
	const CONT_DEFMAP* pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	if ( !pkContDefMap )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// ������ �� ������ �����Ѵ�.
	ReleaseZone();

	// GSA�� ��� �Ʒ� FindMinMax()�� �ƴ϶� LoadGsa() �κп��� Min Max�� ã�´�.
	NxVec3 kMin = m_kMin;
	NxVec3 kMax = m_kMax;
	if( !m_bFoundMinMax )
	{
		// PhysX��带 �־ Bound�� �� Min,Max�� ã�´�.
		FindMinMax( pkNode, kMin, kMax );
	}

	// ĳ���� ������ ���������� ���� ��ġ �ϱ⶧���� ��� ���� ������ �� �ش�.
	kMin.z -= 200.0f;
	kMax.z += 200.0f;

	POINT3 const pt3Min( kMin.x, kMin.y, kMin.z );
	POINT3 const pt3Max( kMax.x, kMax.y, kMax.z );
	if ( FAILED( m_kSmallAreaInfo.Init( GetGroundNo(), *pkContDefMap, pt3Min, pt3Max ) ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("SmallAreaInfo Init Failed MapNo<") << GetGroundNo() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// Big Area�� �����.
	if ( !m_pkBigArea )
	{
		m_pkBigArea = g_kSmallAreaPool.New();
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("BigArea Point is Not NULL") );
	}

	if ( !m_pkBigArea )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("BigArea Create Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_pkBigArea->Init( pt3Min, pt3Max - pt3Min );
	m_pkBigArea->Index( PgSmallArea::BIG_AREA_INDEX );

	// �ٿ������ �°� ������ �����Ѵ�.
	size_t const iTotalSmallAreaSize = static_cast<size_t>(m_kSmallAreaInfo.GetTotalSize());
	m_kAreaCont.resize(iTotalSmallAreaSize, NULL);

	if (S_OK != MakeZone(pkNode))
	{
		return E_FAIL;
	}


	ContArea::iterator area_itr = m_kAreaCont.begin();
	int iIndex = 0;
	int iZoneCount = 0;
	for ( ; area_itr != m_kAreaCont.end(); ++area_itr )
	{
		PgSmallArea *pkCurrent = *area_itr;
		if (pkCurrent == NULL)
		{
			continue;
		}

		++iZoneCount;
		POINT3I pt3CurrentIndex3;
		if ( m_kSmallAreaInfo.GetAreaIndex3( pkCurrent->Index(), pt3CurrentIndex3 ) )
		{
			for (ESmallArea_Direction eDir = SaDir_Begin; eDir < SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
			{
				iIndex = m_kSmallAreaInfo.GetRelativeIndexFromIndex3( pt3CurrentIndex3, eDir );
				PgSmallArea *pkLinkArea = GetArea( iIndex );
				pkCurrent->SetAdjacentArea( eDir, pkLinkArea );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV3, __FL__ << _T("Logic Error!!") );
		}
	}

	INFO_LOG( BM::LOG_LV7, __FL__<<L"Ground["<<GetGroundNo()<<L"] Zone["<< iZoneCount << _T("/") << m_kAreaCont.size()<<L"] Created" );
	return S_OK;
}

HRESULT PgGround::MakeZone(NiNode *pkRootNode)
{
	if(!pkRootNode)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkRootNode is NULL"));
		return E_FAIL;
	}

	unsigned int uiArrayCount = pkRootNode->GetArrayCount();
	POINT3 const pt3AreaSize( static_cast<float>(m_kSmallAreaInfo.GetAreaSize().x), static_cast<float>(m_kSmallAreaInfo.GetAreaSize().y),  static_cast<float>(m_kSmallAreaInfo.GetAreaSize().z) );
	for(unsigned int uiIndex = 0; uiIndex < uiArrayCount; ++uiIndex)
	{
		NiAVObject *pkChild = pkRootNode->GetAt(uiIndex);
		if(!pkChild)
		{
			continue;
		}

		if(NiIsKindOf(NiGeometry, pkChild))
		{
			NiGeometry *pkGeometry = (NiGeometry *)pkChild;
			NiPoint3 *pkVertices = pkGeometry->GetVertices();
			unsigned short usVertexCnt = pkGeometry->GetVertexCount();
			for(unsigned short usIndex = 0; usIndex < usVertexCnt; ++usIndex)
			{
				NiPoint3 *pkVertex = &pkVertices[usIndex];
				NiPoint3 kWorldVertex = *pkVertex + pkChild->GetWorldTranslate();
				int const iAreaIndex = m_kSmallAreaInfo.GetAreaIndexFromPos( POINT3(kWorldVertex.x, kWorldVertex.y, kWorldVertex.z) );
				if (iAreaIndex < 0)
				{
					continue;
				}
				PgSmallArea* pkNew = GetArea(iAreaIndex);
				if (pkNew == NULL)
				{
					pkNew = g_kSmallAreaPool.New();
					if ( pkNew )
					{
						POINT3 ptMinPos;
						m_kSmallAreaInfo.GetAreaMinPos(iAreaIndex, ptMinPos);
						if( true == pkNew->Init( ptMinPos, pt3AreaSize ) )
						{
							pkNew->Index(iAreaIndex);
							m_kAreaCont[iAreaIndex] = pkNew;
							//INFO_LOG(BM::LOG_LV9, __FL__ << _T("Area[") << iAreaIndex << _T("]=") << pkNew);
						}
						else
						{
							g_kSmallAreaPool.Delete(pkNew);
							VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Zone Init failure <") << ptMinPos.x << _COMMA_ << ptMinPos.y << _COMMA_ << ptMinPos.z << _T(">") );
							return E_FAIL;
						}
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("SmallAreaPool Memory Alloc failed") );
						return E_FAIL;
					}
				}
				POINT3I pt3CurrentIndex3;
				if ( m_kSmallAreaInfo.GetAreaIndex3( pkNew->Index(), pt3CurrentIndex3 ) )
				{
					// �ڿ������� ����ȭ�� ���ؼ� AdjacentArea�� ���� ���� �ش�.
					for (ESmallArea_Direction eDir = SaDir_Begin; eDir < SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
					{
						int iAdIndex = m_kSmallAreaInfo.GetRelativeIndexFromIndex3( pt3CurrentIndex3, eDir );
						if (iAdIndex < 0)
						{
							continue;
						}
						PgSmallArea *pkLinkArea = GetArea( iAdIndex );
						if (pkLinkArea == NULL)
						{
							pkLinkArea = g_kSmallAreaPool.New();
							if ( pkLinkArea )
							{
								POINT3 ptMinPos;
								m_kSmallAreaInfo.GetAreaMinPos(iAdIndex, ptMinPos);
								if( true == pkLinkArea->Init( ptMinPos, pt3AreaSize ) )
								{
									pkLinkArea->Index(iAdIndex);
									m_kAreaCont[iAdIndex] = pkLinkArea;
									//INFO_LOG(BM::LOG_LV9, __FL__ << _T("Adjacent Area[") << iAdIndex << _T("]=") << pkLinkArea);
								}
								else
								{
									g_kSmallAreaPool.Delete(pkLinkArea);
									VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Zone Init failure <") << ptMinPos.x << _COMMA_ << ptMinPos.y << _COMMA_ << ptMinPos.z << _T(">") );
									return E_FAIL;
								}
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("SmallAreaPool Memory Alloc failed") );
								return E_FAIL;
							}
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot find SmallArea Index=") << pkNew->Index());
					return E_FAIL;
				}
			}
		}
		else if(NiIsKindOf(NiNode, pkChild))
		{
			MakeZone((NiNode *)pkChild);
		}
	}

	return S_OK;
}

#else

HRESULT PgGround::BuildZone()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	POINTER_VERIFY_INFO_LOG(m_spSceneRoot);
	if(!m_spSceneRoot)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	NiNode *pkNode = (NiNode *)m_spSceneRoot->GetObjectByName("physx");
	if(!pkNode)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot GetObject physx");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// ���� �ٿ���� ������ �޴´�.
	//const NiBound& kBound = m_spSceneRoot->GetWorldBound();
	//int iRadius = (int)kBound.GetRadius();
	//POINT3 const kCenter(kBound.GetCenter().x, kBound.GetCenter().y, kBound.GetCenter().z);
	const CONT_DEFMAP* pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	if ( !pkContDefMap )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// ������ �� ������ �����Ѵ�.
	ReleaseZone();

	// GSA�� ��� �Ʒ� FindMinMax()�� �ƴ϶� LoadGsa() �κп��� Min Max�� ã�´�.
	NxVec3 kMin = m_kMin;
	NxVec3 kMax = m_kMax;
	if( !m_bFoundMinMax )
	{
		// PhysX��带 �־ Bound�� �� Min,Max�� ã�´�.
		FindMinMax( pkNode, kMin, kMax );
	}

	// ĳ���� ������ ���������� ���� ��ġ �ϱ⶧���� ��� ���� ������ �� �ش�.
	kMin.z -= 200.0f;
	kMax.z += 200.0f;

// 	NxBounds3 kBound;
// 	kBound.set(kMin, kMax);
// 	m_kGroundPhyInfo.ptMin.Set(kBound.min.x, kBound.min.y, kBound.min.z);
// 	m_kGroundPhyInfo.ptMax.Set(kBound.max.x, kBound.max.y, kBound.max.z);
// 
// 	NxVec3 kOC;
// 	kBound.getCenter(kOC);
//	POINT3 const kCenter(kOC.x, kOC.y, kOC.z);

	POINT3 const pt3Min( kMin.x, kMin.y, kMin.z );
	POINT3 const pt3Max( kMax.x, kMax.y, kMax.z );
	if ( FAILED( m_kSmallAreaInfo.Init( GetGroundNo(), *pkContDefMap, pt3Min, pt3Max ) ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("SmallAreaInfo Init Failed MapNo<") << GetGroundNo() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// Big Area�� �����.
	if ( !m_pkBigArea )
	{
		m_pkBigArea = g_kSmallAreaPool.New();
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("BigArea Point is Not NULL") );
	}

	if ( !m_pkBigArea )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("BigArea Create Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_pkBigArea->Init( pt3Min, pt3Max - pt3Min );
	m_pkBigArea->Index( PgSmallArea::BIG_AREA_INDEX );

	// �ٿ������ �°� ������ �����Ѵ�.
	size_t const iTotalSmallAreaSize = static_cast<size_t>(m_kSmallAreaInfo.GetTotalSize());
	m_kAreaCont.reserve( iTotalSmallAreaSize );

	POINT3 const pt3AreaSize( static_cast<float>(m_kSmallAreaInfo.GetAreaSize().x), static_cast<float>(m_kSmallAreaInfo.GetAreaSize().y),  static_cast<float>(m_kSmallAreaInfo.GetAreaSize().z) );
	POINT3 pt3CurrentPos;
	int iIndex = 0;
	while ( true == m_kSmallAreaInfo.GetAreaMinPos( iIndex, pt3CurrentPos ) )
	{
		PgSmallArea* pkNew = g_kSmallAreaPool.New();
		if ( pkNew )
		{
			if( true == pkNew->Init( pt3CurrentPos, pt3AreaSize ) )
			{
				pkNew->Index(iIndex);
				m_kAreaCont.push_back( pkNew );
			}
			else
			{
				g_kSmallAreaPool.Delete(pkNew);
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Zone Init failure <") << pt3CurrentPos.x << _COMMA_ << pt3CurrentPos.y << _COMMA_ << pt3CurrentPos.z << _T(">") );
				break;
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("SmallAreaPool Error!!") );
			break;
		}

		++iIndex;
	}

	if ( iTotalSmallAreaSize != m_kAreaCont.size() )
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("SmallArea Build Count Faild : MapNo<") << GetGroundNo() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ContArea::iterator area_itr = m_kAreaCont.begin();
	for ( ; area_itr != m_kAreaCont.end(); ++area_itr )
	{
		PgSmallArea *pkCurrent = *area_itr;

		POINT3I pt3CurrentIndex3;
		if ( m_kSmallAreaInfo.GetAreaIndex3( pkCurrent->Index(), pt3CurrentIndex3 ) )
		{
			for (ESmallArea_Direction eDir = SaDir_Begin; eDir < SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
			{
				iIndex = m_kSmallAreaInfo.GetRelativeIndexFromIndex3( pt3CurrentIndex3, eDir );
				PgSmallArea *pkLinkArea = GetArea( iIndex );
				pkCurrent->SetAdjacentArea( eDir, pkLinkArea );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV3, __FL__ << _T("Logic Error!!") );
		}
	}

	INFO_LOG( BM::LOG_LV7, __FL__<<L"Ground["<<GetGroundNo()<<L"] Zone["<<m_kAreaCont.size()<<L"] Created" );
	return S_OK;
}
#endif

void PgGround::ReleaseZone()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	ContArea::iterator area_itor = m_kAreaCont.begin();

	while(area_itor != m_kAreaCont.end())
	{
#ifdef ZONE_OPTIMIZE_20090624
		PgSmallArea* pkArea = *area_itor;
		if (pkArea != NULL)
		{
			g_kSmallAreaPool.Delete(pkArea);
		}
#else
		g_kSmallAreaPool.Delete((*area_itor));
#endif
		++area_itor;
	}

	m_kAreaCont.clear();

	if ( m_pkBigArea )
	{
		g_kSmallAreaPool.Delete( m_pkBigArea );
		m_pkBigArea = NULL;
	}
}

HRESULT PgGround::FindTriggerLoc( char const* szName, POINT3 &rkTriggerLoc_out, bool const bFindSpawn )const
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( !m_spTriggerRoot )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Ground["<<GetGroundNo()<<L"] is No m_spTriggerRoot Loc" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	NiAVObject *pkSpawn = m_spTriggerRoot->GetObjectByName(szName);
	if(NULL==pkSpawn && bFindSpawn && m_spSpawnRoot)
	{
		pkSpawn = m_spSpawnRoot->GetObjectByName(szName);
	}
	if(!pkSpawn)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Ground["<<GetGroundNo()<<L"] Can't Find SpawnLoc Name:["<<UNI(szName)<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	NiPoint3 const pt = pkSpawn->GetWorldTranslate();

	rkTriggerLoc_out.Set(pt.x,pt.y,pt.z);

	return S_OK;
}

HRESULT PgGround::FindSpawnLoc( int iPotalNo, POINT3 &rkSpawnLoc_out, bool bRandomize, int iSpawnType )const
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( !m_spSpawnRoot )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Ground["<<GetGroundNo()<<L"] is No Spawn Loc PortalNo["<<iPotalNo<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( !iPotalNo && iSpawnType != E_SPAWN_EVENT_GROUND )
	{
		iPotalNo = GetRandomSpawn(iSpawnType);
	}

	char acSpawnName[64] = {0,};
	switch(iSpawnType)
	{
	case E_SPAWN_DEFAULT:
		{
			if ( 100 < iPotalNo )
			{
				::sprintf_s(acSpawnName, 64, "tower_spawn_%d", iPotalNo );
			}
			else
			{
				::sprintf_s(acSpawnName, 64, "char_spawn_%d", iPotalNo );
			}
		}break;
	case E_SPAWN_RED://RED
		{
			::sprintf_s(acSpawnName, 64, "red_spawn_%d", iPotalNo );
		}break;
	case E_SPAWN_BLUE://BLUE
		{
			::sprintf_s(acSpawnName, 64, "blue_spawn_%d", iPotalNo );
		}break;
	case E_SPAWN_WIN:
		{
			::sprintf_s(acSpawnName, 64, "win_spawn_%d", iPotalNo );
		}break;
	case E_SPAWN_LOSE:
		{
			::sprintf_s(acSpawnName, 64, "lose_spawn_%d", iPotalNo );
		}break;
	case E_SPAWN_BATTLEAREA_ATK:
		{
			::sprintf_s(acSpawnName, 64, "red_sel_spawn_%d", iPotalNo );
		}break;
	case E_SPAWN_BATTLEAREA_DEF:
		{
			::sprintf_s(acSpawnName, 64, "blue_sel_spawn_%d", iPotalNo );
		}break;
	case E_SPAWN_DEFENCE7_RED:
		{
			::sprintf_s(acSpawnName, 64, "B_PC_%d", std::max(iPotalNo-1,0) );
		}break;
	case E_SPAWN_DEFENCE7_BLUE:
		{
			::sprintf_s(acSpawnName, 64, "R_PC_%d", std::max(iPotalNo-1,0) );
		}break;
	case E_SPAWN_EVENT_GROUND:
		{
			::sprintf_s(acSpawnName, 64, "PC_Spawn_%d", iPotalNo );
		}break;
	}

	NiAVObject *pkSpawn = m_spSpawnRoot->GetObjectByName(acSpawnName);
	if(!pkSpawn)
	{
		pkSpawn = m_spSpawnRoot->GetObjectByName("char_spawn_1");
		if ( !pkSpawn )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Ground["<<GetGroundNo()<<L"] Can't Find SpawnLoc Name:["<<UNI(acSpawnName)<<L"]" );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}

	NiPoint3 const pt = pkSpawn->GetWorldTranslate();

	rkSpawnLoc_out.Set(pt.x,pt.y,pt.z);
	if (bRandomize)
	{
		rkSpawnLoc_out.x += (30 - BM::Rand_Index(60));
		rkSpawnLoc_out.y += (30 - BM::Rand_Index(60));
	}

	return S_OK;
}

bool PgGround::AddUnit( CUnit *pkUnit, bool const bIsSendAreaData )
{// �̰� �ϸ� SmallArea ���� �� Ŀ�� �Ѵ�.
	BM::CAutoMutex Lock(m_kRscMutex);
	if(!pkUnit)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"unit is null");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( false == PgObjectMgr::RegistUnit( pkUnit ) )
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>( pkUnit );
		if( pkPlayer )
		{
			BM::Stream kPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>( CDC_CharInfoErr ) );
			kPacket.Push( pkPlayer->GetMemberGUID() );
			SendToServer( pkPlayer->GetSwitchServer(), kPacket );
		}

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CObserver< BM::Stream* > *pkObserver = dynamic_cast< BM::CObserver< BM::Stream* >* >(this);
	pkUnit->VAttach(pkObserver);//�׶��忡 ����ϸ� �׶��� ���ָŴ����� ��Ż�� �÷��ְ� ¥��.

	switch(pkUnit->UnitType())
	{
	case UT_MYHOME:
		{
			PgMyHome* pkPlayer = dynamic_cast<PgMyHome*>(pkUnit);
			pkPlayer->SetAbil(AT_USENOT_SMALLAREA,1);// ����Ȩ ������ ������ ��ü ������ ���δ�.
			if(MYHOME_CLASS_OUTSIDE_NO == pkPlayer->GetAbil(AT_CLASS))
			{
				BM::vstring kNodeName(L"Home_");
				kNodeName += pkPlayer->HomeAddr().HouseNo();
				POINT3 kPos = GetNodePosition(MB(kNodeName));
				pkUnit->SetPos(kPos);
			}
		}break;
	}

	pkUnit->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
	if ( !AdjustArea( pkUnit, bIsSendAreaData, true ) )
	{
		PgObjectMgr2::UnRegistUnit( pkUnit );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// RandomSeed�� ���� ���� �Ѵ�.
	pkUnit->SetRandomSeed(g_kEventView.GetServerElapsedTime());

	switch(pkUnit->UnitType())
	{
	case UT_MYHOME:
		{
			PgMyHome* pkPlayer = dynamic_cast<PgMyHome*>(pkUnit);
			pkPlayer->ReadyNetwork(true);
//			++m_kUserCount;
		}break;
	case UT_PLAYER:
		{
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			g_kGroundUnitMgr.Add(pkUnit, GroundKey().GroundNo(), this->GetAttr());

			PgAction_MapLoaded kMapLoadedAction(this);
			kMapLoadedAction.DoAction(pkUnit, NULL);

			bool const bRet = AddComboCounter(pkUnit->GetID());
			m_kQuestTalkSession.AddQuestSession(pkUnit->GetID());

			//INFO_LOG(BM::LOG_LV9, _T("[%s] One user come in [%d]ground. Remain user count - [%d]"), __FUNCTIONW__, GroundKey().GroundNo(), (int)m_kUserCount);

			// ���� MapLoadComplete �Ǳ����� Client���� Action packet�� �����⵵ �Ѵ�.
			// �׷��� Player�� �������� ����, GameTime�� ����ȭ ���� �ش�.
			BM::Stream kSPacket(PT_M_C_SYNC_GAMETIME);
			g_kEventView.WriteToPacket(true, kSPacket);
			pkUnit->Send(kSPacket, E_SENDTYPE_SEND_BYFORCE|E_SENDTYPE_SELF);

			if (pkPlayer != NULL)
			{
				pkPlayer->GroundEnterTimeSec(BM::GetTime64());
				m_kPlayTime.iInCount++;

				pkPlayer->Gattr(this->GetAttr());		// ���� �÷��̾��� �׶��� �Ӽ�..(������ ����, ������ ������ ����).
				
				{// �ֵ��� ĳ����
					int const iClass = pkPlayer->GetAbil(AT_CLASS);
					if( IsClass_OwnSubPlayer(iClass) )
					{//�̶��
						if( 0 == pkPlayer->GetAbil(AT_DEL_SUB_PLAYER) )
						{
							BM::GUID kGuid = pkPlayer->SubPlayerID();
							if( kGuid.IsNull() )
							{
								kGuid.Generate();
							}
							CreateSubPlayer(pkPlayer, kGuid);
						}
					}
				}
			}

			m_kEventItemSetMgr.RegistUnit(pkPlayer);
			{
				SEventMessage kMsg(PMET_HACK_CHECK_DUALKEYBOARD, HackDualKeyboard::ACMM_ADD_USER);
				GroundKey().WriteToPacket( kMsg );
				kMsg.Push( pkPlayer->GetID() );
				g_kTask.PutMsg(kMsg);
			}
		}break;
	case UT_SUB_PLAYER:
		{
		}break;
	case UT_PET:
	case UT_SUMMONED:
		{

		}break;
	case UT_CUSTOM_UNIT:
		{
		}break;
	case UT_BOSSMONSTER:
	case UT_MONSTER:
		{
			bool const bRet = AddMeter(pkUnit->GetID(), pkUnit->GetAbil(AT_C_MAX_HP));

			PgAggroMeter *pkMeter = NULL;
			if( !GetMeter(pkUnit->GetID(), pkMeter) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Monster Aggro Meter[GUID: "<<pkUnit->GetID()<<L"]");
			}
			pkUnit->AggroMeter(pkMeter);

			// �α׸� ����
			PgLogCont kLogCont(ELogMain_Contents_Monster, ELogSub_Monster_Create );
			kLogCont.MemberKey( GroundKey().Guid() );	// GroundGuid
			kLogCont.CharacterKey( pkUnit->GetID() );
			kLogCont.GroundNo( GetGroundNo() );			// GroundNo
			kLogCont.Name( pkUnit->Name() );
			kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

			PgLog kLog( ELOrderMain_Monster, ELOrderSub_Create );
			kLog.Set( 1, static_cast<int>(this->GetAttr()) );	// iValue2 �׶��� Ÿ��
			
			kLogCont.Add( kLog );
			kLogCont.Commit();
		}break;
	case UT_OBJECT:
		{
			bool const bRet = AddMeter(pkUnit->GetID(), pkUnit->GetAbil(AT_C_MAX_HP));
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
		}break;
	}

	{//�켱 ������ ����Ʈ ���� �����ϰ� ����Ʈ�� ����!!
		// ���̵��� ���� �Ǿ�� �� �����۵��� ���⼭ ����� ��û����
		GET_DEF(CItemDefMgr, kItemDefMgr);
		PgGroundUtil::DeleteInvenItem(pkUnit, IT_CONSUME, AT_MAPMOVE_DELETE, kItemDefMgr, GroundKey());
		PgGroundUtil::DeleteEffect(pkUnit, AT_MAPMOVE_DELETE);

		if( pkUnit->IsUnitType(UT_PLAYER) )
		{
			// ��ȥ������ ������ ���� ��ü
			if( !(GetAttr() & GATTR_MARRY) )
			{
				int iEffectNo = 0;
				if( GWL_MALE == pkUnit->GetAbil(AT_GENDER) )
				{
					iEffectNo = m_kMarryMgr.GetEffectMALE();
				}
				else
				{
					iEffectNo = m_kMarryMgr.GetEffectFEMALE();
				}

				if(iEffectNo && pkUnit->GetEffect(iEffectNo, true))
				{
					pkUnit->DeleteEffect(iEffectNo);
				}
			}

			// Ŀ�� ����Ʈ�� PvP���� ���� �ȵ�.
			if(GetAttr() & GATTR_FLAG_PVP_ABLE)
			{
				if(pkUnit->GetEffect(COUPLE_SKILL_AREA, true))
				{
					pkUnit->DeleteEffect(COUPLE_SKILL_AREA);
				}
			}
		}

		// ������ ���� ����Ʈ(����)�� ������ �̸� ������
		{
			PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_DEFAULT_EFFECT);
			PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM1);
			PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM2);
			PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM3); // �ϴ� 3�� ������ Ǯ�����
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM4);
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM5);
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM6);
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM7);
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM8);
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM9);
			//PgGroundUtil::AddRegenTimeEffectReserve(pkUnit, AT_EFFECTNUM10); // �ʿ������� �׶� ���� �ϳ��� Ǯ���� ����
		}
	
		if( pkUnit->IsUnitType(UT_PLAYER) )
		{
			if(GetAttr() & GATTR_FLAG_PVP_ABLE)
			{
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, this);

				g_kPVPEffectSlector.AddEffect(pkUnit, &kArg);
			}

			if(PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit))
			{
				//�����̾� ����Ʈ ���̱�
				if( S_PST_ApplyEffect const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_ApplyEffect>() )
				{
					SActArg kArg;
					PgGroundUtil::SetActArgGround(kArg, this);
					for(VEC_INT::const_iterator c_it=pkPremium->kContEffect.begin(); c_it!=pkPremium->kContEffect.end(); ++c_it)
					{
						pkUnit->AddEffect(*c_it, 0, &kArg, pkUnit);
					}
				}
			}
		}

		if( pkUnit->IsInUnitType(static_cast<EUnitType>(UT_PLAYER|UT_MONSTER|UT_BOSSMONSTER)) )
		{
			// GroundEffect �ֱ�
			PgAction_GroundEffect kAction(this,EFFECT_TYPE_SEND_GROUND);
			kAction.DoAction(pkUnit, NULL);
		}
	}

	return true;
}

int PgGround::TickAI( CUnit* pkUnit, DWORD dwElapsed )
{
    if( !IsRunAI() )
    {
	    return 0;
    }

	//return 0;
	//	INFO_LOG(BM::LOG_LV9, _T("[%s]-[%d] Called[%d]"), __FUNCTIONW__,__LINE__, BM::GetTime32());
	//if(pkUnit->IsUnitType(UT_MONSTER))
	EUnitType eType = pkUnit->UnitType();
	if((eType & UT_UNIT_AI) != 0)
	{
		if(pkUnit->IsDead())//����� �ƹ��͵� ����.
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0 ;
		}

		DWORD dwElapsedCopy = dwElapsed; 

		//if ( pkMonster->CheckTickTime(300, dwElapsedCopy) )
		{
			//#ifdef AI_DEBUG
			//			DWORD dwTkdixTime = BM::GetTime32();
			//			INFO_LOG(BM::LOG_LV9, _T("[%s] Guid[%s], TickElapsed[%d]"), __FUNCTIONW__, pkMonster->GetID().str().c_str(), dwElapsedCopy);
			//#endif
			return MonsterTickAI(pkUnit,dwElapsedCopy) ;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
	return -1 ;
}

void PgGround::Clear()
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if ( 0 < PgObjectMgr::GetUnitCount(UT_PLAYER) )	
	{//���� �׶��忡 �������ִ� ������ ������ �ϴ� Save�ϰ� ���� �Ѵ�.

		CAUTION_LOG( BM::LOG_LV5, __FL__<<L"All player information save BEGIN Ground[" << GetGroundNo() << L"]" );

		// ���� �ִ� ���� ���� �����ϱ�
		CUnit* pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
		while ((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
		{
			SaveUnit( pkUnit );

			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkPlayer )
			{
				// ������ ������ ����� �Ѵ�.
				BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_MapServerClear) );
				kDPacket.Push( pkPlayer->GetMemberGUID() );
				::SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
			}
		}

		CAUTION_LOG( BM::LOG_LV5, __FL__<<L"All player information save END" );
		Sleep( std::min<DWORD>( static_cast<DWORD>(PgObjectMgr::GetUnitCount(UT_PLAYER)) * 100, 5000) );// ���ڱ� DB ���ֵǴ°� ���� ����.	
	}

	PgGroundResource::Clear(); 
	ReleaseZone();

	ReleaseAllUnit();
	m_kContGenPoint_Monster.clear();
	m_kContGenPoint_Object.clear();
	m_kContGenPoint_SummonNPC.clear();
	m_kLocalPartyMgr.Clear();
	m_kAIMng.Release();
	m_kGroundOwnerInfo = SGroundOwnerInfo();
	__int64 i64Now = BM::GetTime64();
	DWORD const dwNow = BM::GetTime32();
	LastPlayTimeCalculateTime(i64Now);
	LastAddExpInVillageTickTime(dwNow);
	m_kPlayTime.Clear();
	HiddenMapOut(true);
	m_kEventItemSetMgr.Clear();
	m_kDefencePartyMgr.Clear();
	JobSkillLocationMgr().Clear();
	m_kLocalVendorMgr.Clear();
	m_bUpdateMonsterCount = false;
}

bool PgGround::AddPartyMember( PgPlayer *pkMaster, PgReqMapMove &kMapMoveAction )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if ( !pkMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !m_kLocalPartyMgr.IsMaster( pkMaster->PartyGuid(), pkMaster->GetID() ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID kPartyList;
	if ( m_kLocalPartyMgr.GetPartyMemberGround( pkMaster->PartyGuid(), this->GroundKey(), kPartyList, pkMaster->GetID() ) )
	{
		VEC_GUID::const_iterator guid_itr = kPartyList.begin();
		for ( ; guid_itr!=kPartyList.end() ; ++guid_itr )
		{
			PgPlayer *pkPlayer = GetUser( *guid_itr );
			if ( pkPlayer )
			{
				if ( !kMapMoveAction.Add( pkPlayer ) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
	}
	return true;
}

bool PgGround::AddExpeditionMember( PgPlayer * pMaster, PgReqMapMove & MapMoveAction )
{
	BM::CAutoMutex Lock( m_kRscMutex );

	if( !pMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
/*
	if( !m_kLocalPartyMgr.IsExpeditionMaster(pMaster->ExpeditionGuid(), pMaster->GetID() ) )
	{
		pMaster->SendWarnMessage(720045); // �̵� ��û�� �������常 ����.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;	
	}
*/
	VEC_GUID ExpeditionList;
	if( m_kLocalPartyMgr.GetExpeditionMemberGround( pMaster->ExpeditionGuid(), this->GroundKey(), ExpeditionList, pMaster->GetID() ) )
	{
		VEC_GUID::const_iterator guid_itr = ExpeditionList.begin();
		for( ; guid_itr != ExpeditionList.end() ; ++guid_itr )
		{
			PgPlayer * pPlayer = GetUser(*guid_itr);
			if( pPlayer )
			{
				if( !MapMoveAction.Add(pPlayer) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
	}

	return true;
}

bool PgGround::CheckEffectFromExpeditionMember( BM::GUID const & ExpeditionGuid, int const EffectNo )
{
	BM::CAutoMutex Lock( m_kRscMutex );

	if ( BM::GUID::NullData() == ExpeditionGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID ExpeditionList;
	if( m_kLocalPartyMgr.GetExpeditionMemberGround( ExpeditionGuid, this->GroundKey(), ExpeditionList ) )
	{
		VEC_GUID::const_iterator guid_itr = ExpeditionList.begin();
		for( ; guid_itr != ExpeditionList.end() ; ++guid_itr )
		{
			PgPlayer * pMember = GetUser(*guid_itr);
			if( pMember )
			{
				CEffect const *pEffect = pMember->FindEffect( EffectNo );
				if( !pEffect )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
	}
	return true;
}

bool PgGround::CheckEffectFromPartyMember( BM::GUID const & PartyGuid, int const EffectNo )
{
	BM::CAutoMutex Lock( m_kRscMutex );

	if ( BM::GUID::NullData() == PartyGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID PartyList;
	if ( m_kLocalPartyMgr.GetPartyMemberGround( PartyGuid, this->GroundKey(), PartyList ) )
	{
		VEC_GUID::const_iterator guid_itr = PartyList.begin();
		for ( ; guid_itr!=PartyList.end() ; ++guid_itr )
		{
			PgPlayer *pMember = GetUser( *guid_itr );
			if ( pMember )
			{
				CEffect const *pEffect = pMember->FindEffect( EffectNo );
				if( !pEffect )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
	}
	return true;
}

void PgGround::GetIndunPartyGuid(CONT_INDUN_PARTY & rkCont)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_INDUN_PARTY::value_type kValue;
	BM::GUID kMasterGuid;
	VEC_GUID kContPartyGuid;
	m_kLocalPartyMgr.GetContPartyGuid(kContPartyGuid);
	VEC_GUID::const_iterator party_it = kContPartyGuid.begin();
	while(party_it != kContPartyGuid.end())
	{
		if( m_kLocalPartyMgr.GetPartyMasterGuid((*party_it), kMasterGuid))
		{
			if( PgPlayer * pkPlayer = GetUser( kMasterGuid ) )
			{
				SPartyOption kPartyOption;
				m_kLocalPartyMgr.GetPartyOption(*party_it, kPartyOption);

				kValue.kPartyGuid = (*party_it);
				kValue.kMasterGuid = kMasterGuid;
				kValue.kMasterName = pkPlayer->Name();
				kValue.iMasterClass = pkPlayer->GetAbil(AT_CLASS);
				kValue.iMasterLevel = pkPlayer->GetAbil(AT_LEVEL);
				kValue.iMasterMapNo = pkPlayer->GroundKey().GroundNo();
				kValue.kPartyTitle = m_kLocalPartyMgr.GetPartyName(*party_it);
				kValue.kPartySubName = m_kLocalPartyMgr.GetPartySubName(*party_it);
				kValue.iNowUser = m_kLocalPartyMgr.GetMemberCount(*party_it);
				kValue.iMaxUser = m_kLocalPartyMgr.GetMaxMemberCount(*party_it);
				kValue.iPartyOption = kPartyOption.iPartyOption;
				kValue.byPartyState = kPartyOption.GetOptionState();
				kValue.WorldGndNo = m_ConstellationKey.WorldGndNo;
				kValue.Position = m_ConstellationKey.Key.kPriKey;
				kValue.Difficulty = m_ConstellationKey.Key.kSecKey;

				rkCont.push_back(kValue);
			}
		}

		++party_it;
	}
}

bool PgGround::AddAnyPartyMember( PgPlayer *pkMember, PgReqMapMove &kMapMoveAction )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if ( !pkMember )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID kPartyList;
	if ( m_kLocalPartyMgr.GetPartyMemberGround( pkMember->PartyGuid(), this->GroundKey(), kPartyList, pkMember->GetID() ) )
	{
		VEC_GUID::const_iterator guid_itr = kPartyList.begin();
		for ( ; guid_itr!=kPartyList.end() ; ++guid_itr )
		{
			PgPlayer *pkPlayer = GetUser( *guid_itr );
			if ( pkPlayer )
			{
				if ( !kMapMoveAction.Add( pkPlayer ) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
	}
	return true;
}

bool PgGround::AddAnyExpeditionMember( PgPlayer *pMember, PgReqMapMove & MapMoveAction )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if( !pMember )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID ExpeditionList;
	if( m_kLocalPartyMgr.GetPartyMemberGround( pMember->ExpeditionGuid(), this->GroundKey(), ExpeditionList, pMember->GetID() ) )
	{
		VEC_GUID::const_iterator guid_itr = ExpeditionList.begin();
		for( ; guid_itr != ExpeditionList.end() ; ++guid_itr )
		{
			PgPlayer * pPlayer = GetUser(*guid_itr);
			if( pPlayer )
			{
				if( !MapMoveAction.Add(pPlayer) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
	}

	return true;
}

bool PgGround::AddPartyAllMember( PgPlayer *pkMaster, PgReqMapMove &kMapMoveAction )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if ( !pkMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !m_kLocalPartyMgr.IsMaster( pkMaster->PartyGuid(), pkMaster->GetID() ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID kPartyList;
	if ( m_kLocalPartyMgr.GetPartyMemberGround( pkMaster->PartyGuid(), this->GroundKey(), kPartyList, pkMaster->GetID() ) )
	{
		VEC_GUID::const_iterator guid_itr = kPartyList.begin();
		for ( ; guid_itr!=kPartyList.end() ; ++guid_itr )
		{
			PgPlayer *pkPlayer = GetUser( *guid_itr );
			if ( pkPlayer )
			{
				if ( !kMapMoveAction.Add( pkPlayer ) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if( m_kLocalPartyMgr.GetMemberCount(pkMaster->PartyGuid()) != (kPartyList.size()+1) )
	{
		// ���� �׶��忡 ��� �������� ���� ���
		return false;
	}
	return true;
}

void PgGround::ReqMapMoveCome( SReqMapMove_CM const &kRMMC, VEC_GUID const &kUserList )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	VEC_GUID kReqUserList;
	kReqUserList.reserve( kUserList.size() );

	VEC_GUID::const_iterator guid_itr = kUserList.begin();
	for ( ; guid_itr != kUserList.end() ; ++guid_itr )
	{
		CUnit *pkUnit = GetUnit( *guid_itr );
		if ( pkUnit )
		{// ���� �׶��忡 ������ POS�� �̵���Ű�� �ȴ�.
			SendToPosLoc( pkUnit, kRMMC.pt3TargetPos, kRMMC.cType );
		}
		else
		{
			kReqUserList.push_back( *guid_itr );
		}
	}

	if ( !kReqUserList.empty() )
	{
		BM::Stream kPacket( PT_M_T_REQ_MAP_MOVE_COME );
		kRMMC.WriteToPacket(kPacket);
		kPacket.Push( kUserList );
		SendToCenter( kPacket );//���ͷ� ����;
	}
}

bool PgGround::RecvMapMoveCome( SReqMapMove_CM const &kRMMC, BM::GUID const &kComeCharGuid )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgPlayer *pkPlayer = GetUser( kComeCharGuid );
	if ( pkPlayer )
	{
		if ( this->GroundKey() != kRMMC.kGndKey  )
		{
			SReqMapMove_MT kRMM(kRMMC);

			PgReqMapMove kMapMove( this, kRMM, NULL );
			if ( kMapMove.Add( pkPlayer ) )
			{
				return kMapMove.DoAction();
			}
		}
		else
		{
			// ������ ü������ �ؾ��Ѵ�.
			POINT3 pt3TargetPos = kRMMC.pt3TargetPos;
			if ( kRMMC.nPortalNo )
			{
				if ( FAILED(FindSpawnLoc( kRMMC.nPortalNo, pt3TargetPos ) ) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("FindSpawnLoc Failed!"));
					return false;
				}
			}

			SendToPosLoc( pkPlayer, pt3TargetPos, kRMMC.cType );
		}
	}
	else
	{
		BM::Stream kFailedPacket( PT_T_M_ANS_MAP_MOVE_COME_FAILED, kComeCharGuid );
		::SendToGround( kRMMC.kGndKey, kFailedPacket );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::RecvMapMoveTarget( BM::GUID const &kTargetCharGuid, BM::GUID const &kReqCharGuid, bool const bGMCommand )
{
	T_GNDATTR const kGndAttr = this->GetAttr();
	switch( kGndAttr )
	{
	case GATTR_PVP:
	case GATTR_EMPORIABATTLE:
		{
			BM::Stream kFailedPacket( PT_M_T_ANS_MAP_MOVE_TARGET_FAILED, kReqCharGuid );
			kFailedPacket.Push( kTargetCharGuid );
			kFailedPacket.Push( bGMCommand );
			kFailedPacket.Push( kGndAttr );//��û�Ҽ� ���� ��Ҿ�
			::SendToCenter( kFailedPacket );
		}break;
	default:
		{
			PgPlayer *pkTargetPlayer = GetUser( kTargetCharGuid );
			if ( pkTargetPlayer )
			{
				PgPlayer *pkReqPlayer = GetUser( kReqCharGuid );
				if ( pkReqPlayer )
				{
					SendToPosLoc( pkReqPlayer, pkTargetPlayer->GetPos(), MMET_Normal );
				}
				else
				{
					SReqMapMove_CM kRMMC( MMET_Normal );
					kRMMC.pt3TargetPos = pkTargetPlayer->GetPos();
					kRMMC.kGndKey = this->GroundKey();
					kRMMC.nPortalNo = 0;

					VEC_GUID kUnitGuidList;
					kUnitGuidList.push_back( kReqCharGuid );
					ReqMapMoveCome( kRMMC, kUnitGuidList );
				}
				return true;
			}
				
			BM::Stream kFailedPacket( PT_M_T_ANS_MAP_MOVE_TARGET_FAILED, kReqCharGuid );
			kFailedPacket.Push( kTargetCharGuid );
			kFailedPacket.Push( bGMCommand );
			kFailedPacket.Push( static_cast<T_GNDATTR>(GATTR_DEFAULT) );// ã�� �� ����
			::SendToCenter( kFailedPacket );
		}break;
	}
	
	return false;
}

void PgGround::RecvMapMoveComeFailed( BM::GUID const &kCharGuid )
{

}

bool PgGround::RecvMapMove( UNIT_PTR_ARRAY& rkUnitArray, SReqMapMove_MT &rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	SAnsMapMove_MT kAMM((EMapMoveEventType)rkRMM.cType);
	kAMM.kSI = g_kProcessCfg.ServerIdentity();
	kAMM.kGroundKey = rkRMM.kCasterKey;
	kAMM.kAttr = this->GetAttr();
	kAMM.iPortalID = rkRMM.nTargetPortal;
	m_bIsBossRoom = rkRMM.bIsBossRoom;

	if ( rkRMM.kCasterKey == rkRMM.kTargetKey )
	{
		kAMM.eRet = MMR_FAILED_AGAIN;
	}
	else
	{
		kAMM.eRet = MMR_FAILED;
	}

	if ( rkRMM.nTargetPortal )
	{
		if ( FAILED(FindSpawnLoc( rkRMM.nTargetPortal, rkRMM.pt3TargetPos ) ) )
		{
			// �ٵ�����������
			ProcessMapMoveResult( rkUnitArray, kAMM );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("FindSpawnLoc Failed!"));
			return false;
		}
	}
	else
	{
		if ( POINT3::NullData() != rkRMM.pt3TargetPos )
		{
			FindCorrectPos( rkRMM.pt3TargetPos, 50.0f );
		}
	}

	kAMM.pt3Pos = rkRMM.pt3TargetPos;

	int iRedIndex = 1;
	int iBlueIndex = 1;
	typedef std::map<int, BM::GUID> CONT_TEAM_PARTY;
	CONT_TEAM_PARTY m_kContTeamParty;
	m_kContTeamParty.clear();

	//
	NxRay kRay(NxVec3(rkRMM.pt3TargetPos.x, rkRMM.pt3TargetPos.y, rkRMM.pt3TargetPos.z+10), NxVec3(0.0f, 0.0f, -1.0f));
	NxRaycastHit kHit;
	NxShape* pkHitShape = GetPhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 170.0f);
	if(pkHitShape)
	{
		rkRMM.pt3TargetPos.z = kHit.worldImpact.z;
	}

	UNIT_PTR_ARRAY kFailedUnitArray;// ���⿡ ������ �ǵ��� ������ �Ѵ�.
	UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
	while( unit_itr != rkUnitArray.end() )
	{
		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if ( !pkUser )
		{
			unit_itr = rkUnitArray.erase(unit_itr);
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"pkUser Is Null");
			continue;
		}

		if(pkUser->OpenVendor() || pkUser->VendorGuid().IsNotNull() )
		{//���� �������� �ݵ���
			RecvVendorClose(unit_itr->pkUnit);
		}

		pkUser->SetPos(rkRMM.pt3TargetPos);

		if ( this->GetAttr() & GATTR_MISSION )
		{
			PgMissionGround *pkMissionGround = dynamic_cast<PgMissionGround*>(this);
			if( pkMissionGround )
			{
				if( pkMissionGround->IsDefenceMode7() || pkMissionGround->IsDefenceMode8() )
				{
					ESpawnType eSpawnType = E_SPAWN_DEFAULT;
					int iPortalNo = 4;
					POINT3 pt3Defence7TargetPos;

					if( pkMissionGround->IsDefenceMode7() )
					{
						eSpawnType = E_SPAWN_DEFENCE7_RED;
						iPortalNo = iRedIndex--;
					}
					else if( pkMissionGround->IsDefenceMode8() )
					{
						int iTeamKey = TEAM_RED;
						CONT_TEAM_PARTY::iterator iter = m_kContTeamParty.find(TEAM_RED);
						if( m_kContTeamParty.end() != iter )
						{
							iTeamKey = TEAM_BLUE;					
						}
						auto kPair = m_kContTeamParty.insert(std::make_pair(iTeamKey, pkUser->GetPartyGuid()));			

						CONT_TEAM_PARTY::iterator team_iter = m_kContTeamParty.find(TEAM_RED);
						if( m_kContTeamParty.end() != team_iter )
						{
							iTeamKey = ( pkUser->GetPartyGuid() == team_iter->second ) ? TEAM_RED : TEAM_BLUE;
						}
						else
						{
							iTeamKey = TEAM_RED;
						}
						pkUser->SetAbil(AT_TEAM, iTeamKey);

						if( pkUser->GetAbil(AT_TEAM) == TEAM_RED )
						{
							eSpawnType = E_SPAWN_DEFENCE7_RED;
							iPortalNo = iRedIndex--;
						}
						else
						{
							eSpawnType = E_SPAWN_DEFENCE7_BLUE;
							iPortalNo = iBlueIndex--;
						}				
					}

					if( E_SPAWN_DEFAULT != eSpawnType )
					{
						if( SUCCEEDED(FindSpawnLoc( iPortalNo, pt3Defence7TargetPos, false, eSpawnType)) )
						{
							pkUser->SetPos(pt3Defence7TargetPos);
						}
					}
				}
				else
				{
					//if( unit_itr != rkUnitArray.begin() )
					{
						PgCreateSpreadPos kAction(pkUser->GetPos());
						POINT3BY OrientedBy = pkUser->GetOrientedVector();
						POINT3 Oriented(OrientedBy.x, -1, OrientedBy.z);
						Oriented.Normalize();
						int const iSpreadRange = 200;
						kAction.AddDir(PhysXScene()->GetPhysXScene(), Oriented, iSpreadRange);

						POINT3 kRegenPos;
						kAction.PopPos( kRegenPos );
						pkUser->SetPos( kRegenPos );
					}
				}
			}
		}

		pkUser->LastAreaIndex(PgSmallArea::NONE_AREA_INDEX);
		pkUser->MapLoading();	// PT_C_M_NFY_MAPLOADED ������ ���� ��� Action�� ����~

		if ( rkRMM.iCustomValue )
		{
			pkUser->SetAbil( AT_TEAM, rkRMM.iCustomValue, false, false );
		}

		bool bRet = this->IsAccess( pkUser );
		bRet = bRet && AddUnit( unit_itr->pkUnit, false );

		if(	bRet )
		{
			// �α׸� ����
			PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Move );
			kLogCont.MemberKey( pkUser->GetMemberGUID() );
			kLogCont.CharacterKey( pkUser->GetID() );
			kLogCont.GroundNo( GetGroundNo() );
			kLogCont.ID( pkUser->MemberID() );
			kLogCont.UID(pkUser->UID());
			kLogCont.Name( pkUser->Name() );
			kLogCont.Class( pkUser->GetAbil(AT_CLASS) );
			kLogCont.Level( pkUser->GetAbil(AT_LEVEL) );
			kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

			PgLog kLog( ELOrderMain_Map, ELOrderSub_Move );
			kLog.Set( 0, GetGroundNo() );				// iValue1 Ÿ�ٸʹ�ȣ
			kLog.Set( 1, rkRMM.kCasterKey.GroundNo() );	// iValue2 �ɽ��ø� ��ȣ
			kLog.Set( 2, pkUser->GetAbil(AT_GENDER) );	// iValue3 ����
			kLog.Set( 3, static_cast< int >(rkRMM.cType) ); // iValue4 ���̵� ����
			kLog.Set( 2, GroundKey().Guid().str() );			// guidValue1 Ÿ�ٸ� GUID
			kLog.Set( 3, pkUser->PartyGuid().str() );			// guidValue2 ��Ƽ GUID

			kLogCont.Add( kLog );
			kLogCont.Commit();

			unit_itr->bAutoRemove = false;// �ڵ������� ���´�.

			BM::GUID const kSelectedPetID = pkUser->SelectedPetID();
			if ( kSelectedPetID.IsNotNull() )
			{
				CONT_PET_MAPMOVE_DATA::iterator pet_move_itr = kContPetMapMoveData.find( kSelectedPetID );
				if ( pet_move_itr != kContPetMapMoveData.end() )
				{
					this->CreatePet( pkUser, kSelectedPetID, pet_move_itr->second );
				}
// 				else
// 				{
// 					CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Not Found SelectedPet<" << kSelectedPetID << L">Info From User<" << pkUser->Name() << L"/" << pkUser->GetID() << L">" );
// 				}
			}

			//��ȯü ����
			CONT_UNIT_SUMMONED_MAPMOVE_DATA::iterator cont_summoned = kContUnitSummonedMapMoveData.find(pkUser->GetID());
			if(cont_summoned != kContUnitSummonedMapMoveData.end())
			{
				CONT_SUMMONED_MAPMOVE_DATA const& kSummonedMapMoveData = cont_summoned->second.kSummonedMapMoveData;
				CONT_SUMMONED_MAPMOVE_DATA::const_iterator c_it = kSummonedMapMoveData.begin();
				while(c_it != kSummonedMapMoveData.end())
				{
					SCreateSummoned kCreateInfo;
					kCreateInfo.kGuid = c_it->first;
					kCreateInfo.kClassKey.iClass = c_it->second.iClass;
					kCreateInfo.kClassKey.nLv = c_it->second.iLevel;
					kCreateInfo.bUniqueClass = c_it->second.bUniqueClass;
					kCreateInfo.sNeedSupply = c_it->second.sNeedSupply;
					kCreateInfo.iLifeTime = c_it->second.iLifeTime;

					CreateSummoned(pkUser, &kCreateInfo, L"Summoned", c_it->second);

					++c_it;
				}
			}

			++unit_itr;
		}
		else
		{
			kFailedUnitArray.swap( rkUnitArray, unit_itr );
		}	
	}
	
	if ( kFailedUnitArray.size() )
	{	
		ProcessMapMoveResult( kFailedUnitArray, kAMM );
	}

	if ( !rkUnitArray.empty() )
	{
		if ( kModifyOrder.size() )
		{
			BM::Stream kMapMoveItemPacket(PT_M_I_REQ_MODIFY_ITEM);
			kMapMoveItemPacket.Push( CIE_GateWayUnLock );
			kMapMoveItemPacket.Push( rkUnitArray.begin()->pkUnit->GetID() );
			kModifyOrder.WriteToPacket( kMapMoveItemPacket );
			kMapMoveItemPacket.Push( (bool)false );
			::SendToItem( GroundKey(), kMapMoveItemPacket );
		}
		
		kAMM.eRet = MMR_SUCCESS;
		kAMM.kGroundKey = GroundKey();
		ProcessMapMoveResult( rkUnitArray, kAMM );
		return true;
	}

	return false;
}

void PgGround::ProcessMapMoveResult( UNIT_PTR_ARRAY& rkUnitArray, SAnsMapMove_MT const &rkAMM )
{
	BM::Stream kAPacket( PT_M_T_ANS_MAP_MOVE_RESULT, rkAMM );
	rkUnitArray.WriteToPacket( kAPacket, WT_MAPMOVE_CLIENT );

	// �������� �ε��� �������� �뺸���־�� �Ѵ�.
	// �̼Ǹʿ����� �ʹ�ȣ�� �ٲ�ϱ�... �ٽ� �����־�� �Ѵ�.
	SGroundKey kUserGndKey = GroundKey();
	kUserGndKey.GroundNo( this->GetGroundNo() );// �̰ɷ� ����� ��..
	kUserGndKey.WriteToPacket( kAPacket );

	m_kSmallAreaInfo.WriteToPacket( kAPacket );// Zone ������ �ٿ��� ������ �Ѵ�.


	// ������ ���������� ������ �ٲ�� �Ѵ�.
	::SendToCenter( kAPacket );

	if(MMR_SUCCESS != rkAMM.eRet)
	{
		return; // �� �̵��� ���� �� ���� ó�� ���� �ʴ´�.
	}

	for(UNIT_PTR_ARRAY::iterator iter = rkUnitArray.begin();iter != rkUnitArray.end();++iter)
	{
		if(rkAMM.iPortalID)
		{
			PgAddAchievementValue kMA(AT_ACHIEVEMENT_USE_PORTAL,1,GroundKey());
			kMA.DoAction((*iter).pkUnit,NULL);
		}

		if( (*iter).pkUnit->GetPartyGuid().IsNotNull() )
		{
			PgGroundUtil::SendPartyMgr_Refuse((*iter).pkUnit->GetID(), EPR_MAP, false==IsPartyBreakIn());
			PgGroundUtil::SendPartyMgr_Refuse((*iter).pkUnit->GetID(), EPR_BOSSROOM, m_bIsBossRoom);
		}

		// ������ ���� ���
		if( false == ( GATTR_INSTANCE & GetAttr() ) )
		{
			(*iter).pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT, 0 );// �ǰ� ȸ�� �ʱ�ȭ
			(*iter).pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_TIMEATTACK, 0 );	// ���� ���� �ð� �ʱ�ȭ
			(*iter).pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_ANYONE_DIE, 0 );	// �ƹ��� �����ʰ� ����Ŭ���� �ʱ�ȭ
			(*iter).pkUnit->SetAbil( AT_INDUN_PARTICIPATION_RATE, 0 );	// �δ�������
		}
		else
		{
			if( 0 == (*iter).pkUnit->GetAbil( AT_ACHIEVEMENT_DUNGEON_TIMEATTACK ) )
			{
				if(0 == m_iMapMoveResultStartTimeSec)
				{
					 m_iMapMoveResultStartTimeSec = static_cast<int>(g_kEventView.GetLocalSecTime(CGameTime::SECOND));
				}
				(*iter).pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_TIMEATTACK, m_iMapMoveResultStartTimeSec );
			}
		}

		switch( rkAMM.cType )
		{
		case MMET_Login:
			{
				if(0 == (*iter).pkUnit->GetAbil(AT_ACHIEVEMENT_SELCHAR_COUNT))
				{
					BM::Stream kPacket(PT_M_N_NOTIFY_FIRST_LOGIN);
					kPacket.Push((*iter).pkUnit->GetID());
					SendToContents(kPacket);
				}

				CONT_PLAYER_MODIFY_ORDER kCont;

				if((*iter).pkUnit->GetAbil(AT_ACHIEVEMENT_SELCHAR_COUNT) < std::numeric_limits<int>::max())
				{
					(*iter).pkUnit->AddAbil(AT_ACHIEVEMENT_SELCHAR_COUNT,1);
					
					kCont.push_back(SPMO(IMET_SET_ABIL,(*iter).pkUnit->GetID(),SPMOD_AddAbil(AT_ACHIEVEMENT_SELCHAR_COUNT,(*iter).pkUnit->GetAbil(AT_ACHIEVEMENT_SELCHAR_COUNT))));

					PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, GroundKey(), kCont, BM::Stream(), true);
					kItemModifyAction.DoAction((*iter).pkUnit,NULL);
				}

				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>((*iter).pkUnit);
				ApplyChannelEffect(pkPlayer, this);
				{
					if( pkPlayer && pkPlayer->IsPCCafe() )
					{
						const CONT_PCCAFE_ABIL* pkContPCCafeAbil = NULL;
						g_kTblDataMgr.GetContDef( pkContPCCafeAbil );
						if( pkContPCCafeAbil )
						{
							CONT_PCCAFE_ABIL::const_iterator c_iter = pkContPCCafeAbil->find( pkPlayer->PCCafeGrade() );
							if( c_iter != pkContPCCafeAbil->end() )
							{
								SActArg kArg;
								kArg.Set(ACTARG_GROUND, this);

								for( int i = 0; i < MAX_PCCAFEABIL_COUNT; ++i )
								{
									int iAbilNo = c_iter->second.arAbilNo[i];
									if( iAbilNo )
									{
										pkPlayer->AddEffect( iAbilNo, 0, &kArg, pkPlayer);
									}
								}
							}
						}
					}
				}

				{// "�������̳�" ���� üũ
					PgCheckAchievements<PgCheckComplete_Base,PgValueCtrl_LongTimeNoSee> kCheckAchievements(AT_ACHIEVEMENT_LONGTIMENOSEE, GroundKey() );
					kCheckAchievements.DoAction((*iter).pkUnit,NULL);
				}

				{// "��ħ���ΰ�, �û������ΰ�" ���� üũ
					BM::DBTIMESTAMP_EX kCurTime;
					g_kEventView.GetLocalTime(kCurTime);
					int const iType = AT_ACHIEVEMENT_LOGIN_0 + kCurTime.hour;
					if(AT_ACHIEVEMENT_LOGIN_23 >= iType)
					{
						PgAddAchievementValue kMA(iType,1,GroundKey());
						kMA.DoAction((*iter).pkUnit,NULL);
					}
				}
			}break;
		default:
			{
			}break;
		}
	}
}

void PgGround::PreReleaseUnit( CUnit const *pkUnit )
{
	if (!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	if (UT_PLAYER == pkUnit->UnitType())
	{
		const BM::GUID kGuid = pkUnit->GetID();
		STradeInfo kInfo;
		if (g_kItemTradeMgr.FindTradeByChar(kGuid, kInfo))
		{
			BM::Stream const kPacket(PT_M_C_NFY_EXCHANGE_ITEM_RESULT, (int)TR_FAIL);
			if (kInfo.kCasterGuid)
			{
				CUnit* pkCaster = PgObjectMgr::GetUnit(kInfo.kCasterGuid);
				if (pkCaster)
				{
					pkCaster->Send(kPacket);
				}
			}

			if (kInfo.kTargetGuid)
			{
				CUnit* pkTarget = PgObjectMgr::GetUnit(kInfo.kTargetGuid);
				if (pkTarget)
				{
					pkTarget->Send(kPacket);
				}
			}
			g_kItemTradeMgr.DelItemTradeByChar(kGuid);
		}
	}
}

// bRecursiveCall :
//	���̳� ��ȯ�� �� Player�� ������ �ٸ� ������ ���� ���
//	Player ���� �Ǹ鼭 �Բ� Release�� ȣ���ϵ��� �����Ѵ�.
bool PgGround::ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall, bool const bSendArea )
{
	if(!pkUnit)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"unit is null");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	BM::CAutoMutex Lock(m_kRscMutex);

//	INFO_LOG( BM::LOG_LV9, _T("[%s] ReleaseUnit Start UnitType[%hd]"), __FUNCTIONW__, pkUnit->UnitType());
	CUnit* pkCaller = NULL;
	{
		BM::GUID const &kCallerID = pkUnit->Caller();
		if ( kCallerID != pkUnit->GetID() )
		{
			pkCaller = GetUnit(kCallerID);
			if ( pkCaller )
			{
				pkCaller->DeleteSummonUnit(pkUnit->GetID());
				if( pkUnit->IsUnitType(UT_SUMMONED)
					&& pkCaller->IsUnitType(UT_PLAYER) 
					)
				{// ��ȯü�� ��������Ƿ� ��ȯü ������ ������� ���� �ɸ��� ����Ʈ�� �����ϰ�
					RefreshSummonEffect( dynamic_cast<PgPlayer*>(pkCaller) );
				}
			}
		}
	}

	if( pkUnit->GetAbil(AT_RELEASE_DEL_EFFECT) )
	{
		INFO_LOG( BM::LOG_LV8, __FL__ << _T("AT_RELEASE_DEL_EFFECT Start, Class<") << pkUnit->GetAbil(AT_CLASS) << _T("> Guid<") << pkUnit->GetID() << _T(">") );
		PgUnitEffectMgr const& rkEffectMgr = pkUnit->GetEffectMgr();
		ContEffectItor effect_iter;
		rkEffectMgr.GetFirstEffect(effect_iter);
		CEffect* pkEffect = NULL;
		while(pkEffect = rkEffectMgr.GetNextEffect(effect_iter))
		{
			if(EDelEffectType const eType = static_cast<EDelEffectType>(pkEffect->GetAbil(AT_RELEASE_DEL_EFFECT_TYPE)) )
			{
				CUnit * pkTarget = NULL;
				VEC_GUID const& rkTargtList = pkEffect->GetTargetList();
				VEC_GUID::const_iterator target_guid_itor = rkTargtList.begin();
				while(target_guid_itor != rkTargtList.end())
				{
					if(pkTarget = GetUnit(*target_guid_itor))
					{
						if(EDET_EFFECT_NO & eType)
						{
							EffectQueueData kData(EQT_DELETE_EFFECT, pkEffect->GetEffectNo());
							pkTarget->AddEffectQueue(kData);
						}
						if(EDET_ABIL_01 & eType)
						{
							EffectQueueData kData(EQT_DELETE_EFFECT, pkEffect->GetAbil(AT_EFFECTNUM1));
							pkTarget->AddEffectQueue(kData);
						}
					}
					++target_guid_itor;
				}
			}
		}
		INFO_LOG( BM::LOG_LV8, __FL__ << _T("AT_RELEASE_DEL_EFFECT End, Guid<") << pkUnit->GetID() << _T(">") );
	}

	PreReleaseUnit(pkUnit);
	
	//if (!bRecursiveCall) ��ȯü ����� ������ ���� �ʱ⶧���� ��ȣ��Ǿ ����
	EUnitDieType const eDieType = static_cast<EUnitDieType>(pkUnit->GetAbil(AT_DIE_TYPE));
	if(false==(eDieType&EUDT_NOT_CHILD_DIE)
		&& pkUnit->GetSummonUnitCount()>0 )
	{
		// ����!! ������ ��ȯ����Ʈ�� ��� �Ǹ� RecursiveCall�϶��� ����ȣ�� ���°� ��.

		BM::GUID kOwnerGuid;
		CUnit* pkOwner = NULL;
		if( CheckUnitOwner(pkUnit, kOwnerGuid) )
		{
			pkOwner = GetUnit( kOwnerGuid );
		}

		// �α׸� ����
		PgLogCont kLogCont(ELogMain_Contents_Monster, ELogSub_Monster_Death );
		kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
		kLogCont.CharacterKey( pkUnit->GetID() );
		kLogCont.GroundNo( GetGroundNo() );			// GroundNo
		kLogCont.Name( pkUnit->Name() );
		kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );
		kLogCont.UnitGender(static_cast<EUnitGender>(pkUnit->GetAbil(AT_GENDER)));

		CUnit * pkEntity = NULL;
		VEC_SUMMONUNIT const kContSummonUnit = pkUnit->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			bool bRelease = true;

			pkEntity = GetUnit(c_it->kGuid);
			switch(pkEntity->UnitType())
			{
			case UT_SUMMONED:
				{
					if(PgSummoned * pkSummonUnit = dynamic_cast<PgSummoned*>(pkEntity))
					{
						bRelease = (false == pkSummonUnit->IsNPC());
					}
				}break;
			case UT_ENTITY:
				{
					if(pkOwner && pkOwner->IsUnitType(UT_PLAYER))
					{
						PgLog kLog( ELOrderMain_Entity, ELOrderSub_Death );
						kLog.Set( 0, pkEntity->GetAbil(AT_CLASS) );
						kLog.Set( 1, pkEntity->GetAbil(AT_LEVEL) );
						kLogCont.Add( kLog );
						
						if(0==pkUnit->GetAbil(AT_VOLUNTARILY_DIE))
						{
							SetUnitDropItem( pkOwner, pkEntity, kLogCont );
						}
					}
				}break;
			}

			if(bRelease)
			{
				pkEntity->SetAbil(AT_HP,0);
				pkEntity->ClearAllEffect(true);
				ReleaseUnit(pkEntity, true);
			}
		}

		kLogCont.Commit();
	}

	//�ڽ��� �ڽĺ��� ���� �״� ���, ������ �θ𿡰� �ѱ�
	if(pkCaller && pkUnit->GetAbil(AT_MON_CHILD_DIE) && pkCaller->GetAbil(AT_MON_CHILD_DIE))
	{
		CUnit* pkSummonUnit = NULL;
		VEC_SUMMONUNIT const& kContSummonUnit = pkUnit->GetSummonUnit();
		bool const bEmpty = kContSummonUnit.empty();
		if(false==bEmpty)
		{
			INFO_LOG( BM::LOG_LV8, __FL__ << _T("AT_MON_CHILD_DIE Start, Guid<") << pkUnit->GetID() << _T(">") );
		}

		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			if(pkSummonUnit = GetUnit((*c_it).kGuid))
			{
				pkCaller->AddSummonUnit((*c_it).kGuid, (*c_it).iClassNo, ESO_IGNORE_MAXCOUNT);
				pkSummonUnit->Caller(pkCaller->GetID());
			}
		}

		if(false==bEmpty)
		{
			INFO_LOG( BM::LOG_LV8, __FL__ << _T("AT_MON_CHILD_DIE End, Guid<") << pkUnit->GetID() << _T(">") );
		}
	}

	if(pkUnit->GetAbil(AT_MUTATOR_HYDRA_SELF) == EHU_YES)
	{
		MakeHydraUnit(pkUnit);
	}

	EUnitType const eUnitType = pkUnit->UnitType();
	PgSmallArea *pkArea = GetArea( pkUnit->LastAreaIndex() );
//	POINT3 ptTemp;
//	if( E_FAIL != GetArea(pkUnit->LastAreaPos(), ptTemp, pkArea, false, false) )
	if ( pkArea )
	{
		///////////////////////////////////
		// 2007.11.29 ���� ���������� 
		// ���� Release�� Ŭ�󿡼� �����Ѵ�.
		// HP_VALUE_KILL_NOTADDED_UNIT ���� ������ ������ ���γ��̴�....(Ŭ��� ReleaseUnit��Ŷ ������� �Ѵ�.)
		//if (pkUnit->IsUnitType(UT_MONSTER) == false || pkUnit->GetAbil(AT_HP) == HP_VALUE_KILL_NOTADDED_UNIT)
		////////////////////////////////////
		// 2007.11.29 ���� ���ʹ�
		// Monster Die�� ������ ������ Ŭ���̾�Ʈ�� �����ʴ� Dummy Monster�� �����ȴ�.
		//	��, Client������ �ٸ��÷��̾ ���͸� ������ ���϶� / ���͸� ���̰�, �÷��̾�� �Ⱥ��δٰ� �Ѵٸ�
		//	�÷��̾��� ����Action Packet�� ���� �� ���� ������ ������ �˼� ���� �ȴ�.
		//	�׷��� ���Ͱ� �������� AT_HP�� ������ Ŭ���̾�Ʈ���� �����׼� ������ �� �� �ֵ��� �Ѵ�.
		
		if ( bSendArea )
		{
			switch( eUnitType )
			{
			case UT_OBJECT:
				{
					if ( pkUnit->IsAlive() )
					{
						SendAreaData( pkUnit, NULL, pkArea );
					}
				}break;
			case UT_SUMMONED:
				{
					pkUnit->SendAbil( AT_HP, E_SENDTYPE_BROADALL );
					if(0 != pkUnit->GetAbil(AT_HP)
					|| HP_VALUE_KILL_NOTADDED_UNIT == pkUnit->GetAbil(AT_HP) )
					{
						SendAreaData( pkUnit, NULL, pkArea );
					}					
				}break;
			case UT_MONSTER:
			case UT_BOSSMONSTER:
				{
					if ( HP_VALUE_KILL_NOTADDED_UNIT != pkUnit->GetAbil(AT_HP) )
					{
						pkUnit->SendAbil( AT_HP, E_SENDTYPE_BROADALL );
						break;
					}

#ifdef AI_DEBUG2
					INFO_LOG( BM::LOG_LV9, __FL__ << _T("UT_MONSTER Released Class<") << pkUnit->GetAbil(AT_CLASS) << _T("> Guid<") << pkUnit->GetID() << _T(">") );
#endif
				}// break ������� ����
			default:
				{
					SendAreaData( pkUnit, NULL, pkArea );
				}break;
			}
		}
		
		pkArea->RemoveUnit( pkUnit->GetID() );
	}
	else
	{
		//VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Can't Release Unit Cause: None Area"), __FUNCTIONW__, __LINE__);
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't Release Unit Cause: None Area");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkArea is NULL"));
	}

	m_kStoneCtrl.RemoveTicket(pkUnit);

	switch( eUnitType )
	{
	case UT_MYHOME:
		{
			// Ȩ�� �̵� ����!!
		}break;
	case UT_PLAYER:
		{
			if( PgObjectMgr2::UnRegistUnit( pkUnit ) )
			{
				BM::GUID const kCharGuid = pkUnit->GetID();

				g_kGroundUnitMgr.Del(pkUnit);

				bool const bDelComboRet = DelComboCounter(kCharGuid);
				m_kQuestTalkSession.RemoveQuestSession(kCharGuid);
				ClearSpecStatus(kCharGuid);

				PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if ( pkPlayer )
				{
					pkPlayer->MapLoading();
					m_kPlayTime.iOutCount++;
					m_kPlayTime.iPlayTime += BM::GetTime64() - std::max<__int64>(pkPlayer->GroundEnterTimeSec(), LastPlayTimeCalculateTime());

					PgPet *pkPet = GetPet( pkPlayer );
					if ( pkPet )
					{
						ReleaseUnit( pkPet );
					}
					PgSubPlayer* pkSubPlayer = GetSubPlayer( pkPlayer );
					if( pkSubPlayer )
					{
						pkSubPlayer->SetCallerUnit(NULL);
						ReleaseUnit( pkSubPlayer );
					}
				}
				
				if( GroundMute() == kCharGuid )
				{
					m_kGroundMute.Clear();
				}

				m_kEventItemSetMgr.UnregistUnit(pkPlayer);
                m_kContEventScriptPlayer.erase(kCharGuid);
				{
					SEventMessage kMsg(PMET_HACK_CHECK_DUALKEYBOARD, HackDualKeyboard::ACMM_DEL_USER);
					GroundKey().WriteToPacket( kMsg );
					kMsg.Push( kCharGuid );
					g_kTask.PutMsg(kMsg);
				}
//				INFO_LOG(BM::LOG_LV9, _T("[%s] One user out in [%d]ground. Remain user count - [%d]"), __FUNCTIONW__, GroundKey().GroundNo(), (int)m_kUserCount);
			}
		}break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		{
			if ( PgObjectMgr2::UnRegistUnit( pkUnit ) )
			{
				pkUnit->AggroMeter(NULL);
				DelMeter(pkUnit->GetID());
			}
			m_bUpdateMonsterCount = true;
		}break;
	case UT_ENTITY:
	case UT_PET:
	case UT_SUB_PLAYER:
	default:
		{
			PgObjectMgr2::UnRegistUnit( pkUnit );
		}break;
	}

	g_kTotalObjMgr.ReleaseUnit(pkUnit);
	return true;
}

void PgGround::LogOut( SERVER_IDENTITY const &kSI )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_OBSERVER::const_iterator ob_itr = m_kContObserver.begin();
	for ( ; ob_itr != m_kContObserver.end() ; ++ob_itr )
	{
		if ( S_OK == PgObjectMgr2::UnRegistObserver( ob_itr->second ) )
		{
// 			int iAreaIndex = PgSmallArea::NONE_AREA_INDEX;
// 			CUnit *pkOwnerUnit = PgObjectMgr::GetUnit( ob_itr->second.GetOwnerID() );
// 			if ( pkOwnerUnit )
// 			{
// 				iAreaIndex = pkOwnerUnit->LastAreaIndex();
// 			}

			PgPlayer *pkMyPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetUnit( UT_PLAYER, ob_itr->second.GetCharID() ));
			if ( pkMyPlayer )
			{
				BM::Stream kEndPacket( PT_M_C_NFY_OBMODE_END );
				pkMyPlayer->Send( kEndPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);

				// ���� Area������ ���� ���� �־�� �Ѵ�~
				pkMyPlayer->SetSyncType( SYNC_TYPE_DEFAULT, false );

				if(pkMyPlayer->OpenVendor() || pkMyPlayer->VendorGuid().IsNotNull() )
				{
					RecvVendorClose( PgObjectMgr::GetUnit( UT_PLAYER, ob_itr->second.GetCharID() ) );
				}

// 				if ( pkMyPlayer->LastAreaIndex() != iAreaIndex )
// 				{
// 					PgSmallArea *pkCasterArea = GetArea( pkMyPlayer->LastAreaIndex() );
// 					PgSmallArea *pkTargetArea = GetArea( iAreaIndex );
// 					SendAreaData( pkMyPlayer, pkCasterArea, pkTargetArea, SYNC_TYPE_RECV_ADD );// AddUnit �ޱ⸸ �ؾ� �Ѵ�.
// 				}
			}
		}
	}
	m_kContObserver.clear();

	PgPlayer *pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		switch ( kSI.nServerType )
		{
		case CEL::ST_CENTER:
			{
				if( pkPlayer->GetChannel() == kSI.nChannel )
				{
					// ���Ͱ� �������Ŵ�. �׳� ������ Release�� �ؾ� �Ѵ�!
					CAUTION_LOG( BM::LOG_LV0, L"Center["<<kSI.nChannel<<L"] Error!! No Save ReleaseUnit["<<pkPlayer->Name()<<L"-"<<pkPlayer->GetID()<<L"]" );
					ReleaseUnit(pkPlayer);
				}
			}break;
		case CEL::ST_SWITCH:
			{
				if( pkPlayer->GetSwitchServer() == kSI )
				{
					this->LogOut( pkPlayer->GetID() );
				}
			}break;
		}
	}
}

bool PgGround::LogOut(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgObjectMgr2::UnRegistObserver( rkCharGuid, NULL );

	if( GetAttr() & GKIND_EXPEDITION_LOBBY )	// �κ���� ��� ������ ���Դ�������� Ȯ��(ä���̵�����).
	{
		PgExpeditionLobby * pGround = dynamic_cast<PgExpeditionLobby*>(this);
		if( pGround )
		{
			pGround->DelJoinExpeditionWaitList(rkCharGuid);
		}
	}

	CUnit* pkUser = PgObjectMgr::GetUnit( UT_PLAYER, rkCharGuid );
	if(pkUser)
	{
		PgPlayer const* pkPlayer = dynamic_cast<PgPlayer*>(pkUser);
		if(pkPlayer)
		{
			if(true == pkPlayer->CashShopGuid().IsNotNull())
			{
				BM::Stream kPacket(PT_M_I_CS_REQ_EXIT_CASHSHOP);
				kPacket.Push(pkPlayer->GetID());
				kPacket.Push(pkPlayer->CashShopGuid());
				SendToItem(pkPlayer->GroundKey(),kPacket);
			}
			RecvReqVendorDelete(pkUser, pkUser->GetID());
		}

		this->SaveUnit( pkUser );
        SetTeam(pkUser->GetID(), pkUser->GetAbil(AT_TEAM));
		ReleaseUnit(pkUser);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

// bRecursiveCall :
//	���̳� ��ȯ�� �� Player�� ������ �ٸ� ������ ���� ���
//	Player ���� �Ǹ鼭 �Բ� Release�� ȣ���ϵ��� �����Ѵ�.
void PgGround::ReleaseUnit(UNIT_PTR_ARRAY& rkUnitArray, bool bRecursiveCall)
{
	UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
	while ( unit_itr!=rkUnitArray.end() )
	{
		ReleaseUnit(unit_itr->pkUnit,bRecursiveCall);
		unit_itr = rkUnitArray.OnlyErase(unit_itr);
	}
}

#define MAX_HYDRA_SPAWN_RANGE_X 30
#define MAX_HYDRA_SPAWN_RANGE_Y 30
#define MIN_HYDRA_SPAWN_RANGE_X 10
#define MIN_HYDRA_SPAWN_RANGE_Y 10

void PgGround::MakeHydraUnit(CUnit *pkUnit)
{
	for(int i = 0; i < pkUnit->GetAbil(AT_MUTATOR_HYDRA_COUNT); i++)
	{
		BM::GUID kNewMonGuid;
		kNewMonGuid.Generate();
		TBL_DEF_MAP_REGEN_POINT gkGenInfo;
		gkGenInfo.cBagControlType = ERegenBag_Type_Monster;
		gkGenInfo.dwPeriod = 0;
		gkGenInfo.iBagControlNo = 0;
		gkGenInfo.iMapNo = this->GetGroundNo();
		gkGenInfo.iMoveRange = pkUnit->GetAbil(AT_MOVE_RANGE);
		gkGenInfo.iPointGroup = 1000;
		gkGenInfo.pt3Pos = pkUnit->GetPos();
		{
			int iTempPostion = BM::Rand_Range(MAX_HYDRA_SPAWN_RANGE_X, MIN_HYDRA_SPAWN_RANGE_X);
			gkGenInfo.pt3Pos.x += (gkGenInfo.pt3Pos.x >= 0 ? iTempPostion : -iTempPostion);
			iTempPostion = BM::Rand_Range(MAX_HYDRA_SPAWN_RANGE_Y, MIN_HYDRA_SPAWN_RANGE_Y);
			gkGenInfo.pt3Pos.y +=( gkGenInfo.pt3Pos.y >= 0 ? iTempPostion : -iTempPostion);
			gkGenInfo.pt3Pos.z += 15;
		}

		if( InsertMonster( gkGenInfo, pkUnit->GetAbil(AT_CLASS),kNewMonGuid, pkUnit ) != S_OK )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__ << L"Failed Make Hydra Monster Parent Mon GUID[" << pkUnit->GetID() << "]");
		}
	}
}

#undef MAX_HYDRA_SPAWN_RANGE_X
#undef MAX_HYDRA_SPAWN_RANGE_Y
#undef MIN_HYDRA_SPAWN_RANGE_X
#undef MIN_HYDRA_SPAWN_RANGE_Y

bool PgGround::SaveUnit( CUnit *pkUnit, SReqSwitchReserveMember const *pRSRM, BM::Stream * const pPacket )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( pkUnit->UnitType() == UT_PLAYER )
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if(pkPlayer)
		{
			PgQuickInventory* pkQInv = pkPlayer->GetQInven();
			if ( !pkQInv )
			{
				CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("QuickInven Is NULL Possible??? Name=") << pkPlayer->Name() << _T(" CharacterGuid=") << pkPlayer->GetID() );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			SGroundKey SaveGroundKey = GroundKey();
			
			bool bSelectChannel = false;
			if( pPacket )
			{
				pPacket->Pop(bSelectChannel);

				if( pPacket->RemainSize() )
				{
					short sRealmNo, sChannelNo;
					int iGroundNo;
					pPacket->Pop(sRealmNo);
					pPacket->Pop(sChannelNo);
					pPacket->Pop(iGroundNo);
					SaveGroundKey.GroundNo(iGroundNo);
				}
			}

			// ������ �κ� ���� �� ������ ������ ���ϰ�� ���� ������ �ǵ��� ��� ��.
			// ä���� ������ ������ �� ��� ���� �� �� ����.
			SRecentPlayerNormalMapData MapData;
			pkPlayer->GetRecentNormalMap(MapData);
			if( (GetAttr() & GKIND_EXPEDITION)
				|| (GetAttr() & GKIND_EXPEDITION_LOBBY)
				|| (GetAttr() & GKIND_EVENT) )
			{				
				SaveGroundKey.GroundNo(MapData.GroundNo);
				pkPlayer->SetPos(MapData.RecentPos);
			}
			else if( 0 == (GetAttr() & GATTR_FLAG_NOSAVE_RECENT) )
			{
				MapData.ChannelNo = g_kProcessCfg.ChannelNo();
				MapData.GroundNo = GroundKey().GroundNo();
				MapData.RecentPos = pkPlayer->GetPos();
				pkPlayer->SetRecentNormalMap(MapData);
			}

			bool bSaveHPMP = PgGroundUtil::IsSaveHPMP(static_cast<EGndAttr>(GetAttr()));
//			pkPlayer->UpdateRecent( this->GroundKey(), this->GetAttr() ); ���ʿ� ���� ���� �������� �ٷ� ���� �ٲ���.
			BM::Stream kSPacket( PT_M_T_REQ_SAVE_CHARACTER, pkPlayer->GetID() );
			kSPacket.Push( bSelectChannel );	// ä�� ��������, ĳ���� ��������.
			kSPacket.Push( SaveGroundKey );
			kSPacket.Push( MapData );	// ������ ä�η� �̵��� ���. ä��/��/��ġ����.
			kSPacket.Push( pkPlayer->GetPos() );		// 1 Recent Position
			kSPacket.Push( (bSaveHPMP) ? pkPlayer->GetAbil(AT_HP) : -1 );	// 2 HP
			kSPacket.Push( (bSaveHPMP) ? pkPlayer->GetAbil(AT_MP) : -1 );	// 3 MP
			pkQInv->WriteToPacket( kSPacket );			// 4 Quick Inventory
			kSPacket.Push( static_cast<short>(pkPlayer->GetAbil(AT_STRATEGY_FATIGABILITY)) );	// 5. Fatigue Degree.

			SPlayerBinaryData kBinary;
			pkPlayer->GetEffectMgr().SaveToDB(kBinary.abyEffect, sizeof(kBinary.abyEffect));
			kSPacket.Push(kBinary.abyEffect, sizeof(kBinary.abyEffect));
			{
				__int64 const i64AddExp = pkPlayer->GetAbil64(AT_REST_EXP_ADD_MAX);
				kSPacket.Push(i64AddExp);
			}
			{
				BYTE abyItemGroupCoolTime[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE] = {0,};
				pkUnit->GetInven()->SaveGroupCoolTime(g_kEventView.GetServerElapsedTime(), abyItemGroupCoolTime);
				kSPacket.Push(abyItemGroupCoolTime, MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE);
			}
			{//���� ������ �ܿ� ��Ÿ�� ����
				CSkill::MAP_COOLTIME kContCoolTime;
				pkUnit->GetSkill()->SaveToDBCoolTimeMap(kContCoolTime);
				kSPacket.Push(kContCoolTime);
			}
			
			PgPet *pkPet = GetPet( pkPlayer );
			if ( pkPet && BM::GUID::IsNotNull(pkPet->GetID()) )
			{
				kSPacket.Push( pkPet->GetID() );
				kSPacket.Push( pkPet->GetAbil(AT_MP) );

/*
				bool bUpdateCoolTime = false;
				BYTE const byMax = static_cast<BYTE>(MAX_PET_SKILLCOUNT-1);
				__int64 i64CoolTime[byMax] ={0i64,};

				// ��Ÿ�� �����ؾ� �Ѵ�.
				CSkill *pkPetSkill = pkPet->GetSkill();
				PgMySkill *pkPetMySkill = pkPet->GetMySkill();
				DWORD const dwCurrentTime = g_kEventView.GetServerElapsedTime() + 3000;//3���� ����
				__int64 const i64NowMSTime = g_kEventView.GetLocalSecTime( CGameTime::MILLISECOND );

				CSkill::CONST_COOLTIME_INDEX kItor;
				pkPetSkill->GetFirstCoolTime(kItor);

				CSkill::SCoolTimeInfo kInfo;
				while( true == pkPetSkill->GetNextCoolTime(kItor, kInfo) )
				{
					if ( SKILL_NO_GLOBAL_COOLTIME != kInfo.iSkillNo )
					{
						if ( dwCurrentTime < kInfo.dwCoolTime )
						{
							BYTE byIndex = 0;
							if ( true == pkPetMySkill->GetSkillIndex( GET_BASICSKILL_NUM(kInfo.iSkillNo), byIndex ) )
							{
								if ( (0 < byIndex) && (byMax > byIndex) )
								{
									bUpdateCoolTime = true;
									i64CoolTime[--byIndex] = i64NowMSTime + static_cast<__int64>(kInfo.dwCoolTime - dwCurrentTime);
								}
							}
						}
					}
				}

				kSPacket.Push( bUpdateCoolTime );
				if ( true == bUpdateCoolTime )
				{
					kSPacket.Push( PET_COOLTIME_SAVEVER );
					kSPacket.Push( i64CoolTime, sizeof(i64CoolTime) );
				}
*/
			}
			else
			{
				kSPacket.Push( BM::GUID::NullData() );
			}

			if ( pRSRM )
			{
				kSPacket.Push( true );
				pRSRM->WriteToPacket( kSPacket );
			}
			else
			{
				kSPacket.Push( false );
			}

			::SendToCenter( kSPacket );


			{
				SocialDB::SSocialCharacterKey const kKey(g_kProcessCfg.RealmNo(), pkPlayer->GetID());
				SocialDB::SSocialCharacterInfo const kCharInfo(*pkPlayer);
				BM::Stream kPacket(PT_A_N_NFY_CHARACTER_INFO);
				kKey.WriteToPacket(kPacket);
				kCharInfo.WriteToPacket(kPacket);

				PgInventory* pkInven = pkPlayer->GetInven();
				if( pkInven )
				{
					GET_DEF(CItemDefMgr, kItemDefMgr);
					SocialDB::CONT_SOCIAL_INV_ITEM_ALL kContItemInfo;
					SocialDB::BuildFitItem(kItemDefMgr, *pkInven, kContItemInfo);
					SocialDB::BuildFitCastItem(kItemDefMgr, *pkInven, kContItemInfo);
					SocialDB::BuildFitCostumeItem(kItemDefMgr, *pkInven, kContItemInfo);
					SocialDB::BuildInvenItem(kItemDefMgr, IT_EQUIP, *pkInven, kContItemInfo);
					SocialDB::BuildInvenItem(kItemDefMgr, IT_CONSUME, *pkInven, kContItemInfo);
					SocialDB::BuildInvenItem(kItemDefMgr, IT_ETC, *pkInven, kContItemInfo);
					SocialDB::BuildInvenItem(kItemDefMgr, IT_CASH, *pkInven, kContItemInfo);
					kPacket.Push( kContItemInfo.size() );
					SocialDB::CONT_SOCIAL_INV_ITEM_ALL::const_iterator iter = kContItemInfo.begin();
					while( kContItemInfo.end() != iter )
					{
						kPacket.Push((*iter).first);
						PU::TWriteArray_M(kPacket, (*iter).second);
						++iter;
					}
				}
				{// �޺� ����
					PgComboCounter* pkCounter = NULL;
					if( GetComboCounter(pkPlayer->GetID(), pkCounter) )
					{
						COMBO_TYPE const iLatestCombo = pkCounter->GetComboCount();

						// NC��
						BM::Stream kNPacket(PT_A_N_NFY_CHARACTER_COMBO_INFO);
						kNPacket.Push( static_cast< int >(g_kProcessCfg.RealmNo()) );
						kNPacket.Push( pkPlayer->GetID() );
						kNPacket.Push( iLatestCombo );
						kNPacket.Push( BM::DBTIMESTAMP_EX(BM::PgPackedTime::LocalTime()) );
						::SendToRealmContents(PMET_WEB_HELPER, kNPacket);

						// ���� TB_UserCharacter_Point
						// ProcessModifyPlayer_LogOut() ������ ���� ó�� �� �ش�.( iLatestCombo �� �״�� UPDATE ) 
						if( iLatestCombo > pkPlayer->GetAbil( AT_MAX_COMBO ) )
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							SPMOD_AddRankPoint kSetRank( E_RANKPOINT_MAXCOMBO, iLatestCombo );
							kOrder.push_back( SPMO( IMET_ADD_RANK_POINT, pkPlayer->GetID(), kSetRank ) );

							PgAction_ReqModifyItem kItemModifyAction(CIE_Rank_Point, GroundKey(), kOrder, BM::Stream(), true);//��ŷ�� ��� ���� ����.
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}
				}

				::SendToRealmContents(PMET_WEB_HELPER, kPacket);
			}

			return true;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("This is Not Player") << pkUnit->GetID() );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgGround::RecvReqAction2(CUnit* pkUnit, SActionInfo& rkAction, BM::Stream * const pkPacket)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	/*
	if(pkUnit->GetID() != rkAction.guidPilot)
	{
		if(pkUnit->Caller() != rkAction.guidPilot)
		{
			INFO_LOG(BM::LOG_LV5, _T("[%s] Unit Guid[%s] is Not Matach ActionInfo Guid[%s]"), __FUNCTIONW__, pkUnit->GetID().str().c_str(), rkAction.guidPilot.str().c_str());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched ActionInfo!"));
			return;
		}
	}
	*/
	//INFO_LOG(BM::LOG_LV6, _T("ReqAction2....Action[%d]"), rkAction.iActionID);

#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("Player Pos [%04d, %04d, %04d]"), (int)rkAction.ptPos.x, (int)rkAction.ptPos.y, (int)rkAction.ptPos.z);
#endif
	if(false == VCheckUsableSkill(pkUnit, rkAction)) { return; }

	bool bHacking = CheckHackingAction(pkUnit, rkAction);
	
	pkUnit->SetPos(rkAction.ptPos);

	// �ϴ� ������ �־�� �Ѵ�.
	if(0 != (rkAction.byDirection & 0x0F))
	{
		pkUnit->FrontDirection(static_cast<Direction>(rkAction.byDirection & 0x0F));
	}
	if(pkUnit->IsUnitType(UT_PLAYER))
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if ( PgGround::ms_kAntiHackCheckActionCount.m_bUseAntiHackCheckActionCount )
		{
			HRESULT hResult = pkPlayer->CheckActionPacketCount(rkAction);
			if(S_OK != hResult)
			{
				switch(hResult)
				{
				case E_ACTION_COUNT_SLOW_PACKET:
					{
						PLAYER_ACTION_TIME_COUNT const& rkInfo = pkPlayer->GetActionPacketCountInfo();
						 
						BM::vstring kLogMsg;
						kLogMsg << __FL__ << "[HACKING][ActionCount] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() 
							<< "]  MemberGUID[" << pkPlayer->GetMemberGUID() << _T("] GroundNo[") << GroundKey().GroundNo() << _T("] HackingType[FastAction] LastClientElapsedTime[")
							<< rkInfo.m_kLastActionInfo.m_dwActionTime << _T("] NewClientElapsedTime[") << rkAction.dwTimeStamp << _T("]");
						//VERIFY_INFO_LOG(false, BM::LOG_LV1, kLogMsg) ;
						HACKING_LOG(BM::LOG_LV0, kLogMsg);

						//Player���� ����� ������ ���(Debug)
						if((g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug) && (PgGround::ms_kAntiHackCheckActionCount.m_bResultToPlayer))
						{
							pkPlayer->SendWarnMessageStr(kLogMsg);
						}

						//��ŷ ����Ʈ�� ��� �� ���
						if(PgGround::ms_kAntiHackCheckActionCount.m_bHackingIndex)
						{
							if(pkPlayer->SuspectHacking(EAHP_ActionCount, PgGround::ms_kAntiHackCheckActionCount.m_iAddHackingPoint))
							{
								// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
								BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_ActionCount_Hack_B) );
								kDPacket.Push( pkPlayer->GetMemberGUID() );
								SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
							}
						}
					}break;
				}

			}
			
			PLAYER_ACTION_TIME_COUNT_RESULT kResult;
			if(pkPlayer->ResultActionPacketCount(false, &kResult, PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime, PgGround::ms_kAntiHackCheckActionCount.m_iCheckCountSum, PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime_Mutiflier))
			{
				if (PgGround::ms_kAntiHackCheckActionCount.m_bForcedResultView || kResult.m_bHackingWarning )
				{
					BM::vstring kLogMsg;
					kLogMsg << __FL__ << "[HACKING][ActionCount] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() 
						<< "]  MemberGUID[" << pkPlayer->GetMemberGUID() << _T("] GroundNo[") << GroundKey().GroundNo() << _T("] HackingType[FastAction] RecvPacketCount[") 
						<< kResult.m_iRecvPacketCount << _T("] TotalAniTime[") << kResult.m_iTotalActionAniTime << _T("] ClientElapsedTime[") << kResult.m_iClientElapsedTime << _T("] ServerElapsedTime[") << kResult.m_iServerElapsedTime << _T("]");
					//VERIFY_INFO_LOG(false, BM::LOG_LV1, kLogMsg) ;
					HACKING_LOG(BM::LOG_LV0, kLogMsg);

					//Player���� ����� ������ ���(Debug)
					if((g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug) && (PgGround::ms_kAntiHackCheckActionCount.m_bResultToPlayer))
					{
						BM::vstring kLogMsg; kLogMsg << _T("HackingType[FastAction] RecvPacketCount[") << kResult.m_iRecvPacketCount << _T("] TotalAniTime[") << kResult.m_iTotalActionAniTime << _T("] ClientElapsedTime[") << kResult.m_iClientElapsedTime << _T("] ServerElapsedTime[") << kResult.m_iServerElapsedTime << _T("]");
						pkPlayer->SendWarnMessageStr(kLogMsg);
					}

					//��ŷ ����Ʈ�� ��� �� ���
					if(kResult.m_bHackingWarning && PgGround::ms_kAntiHackCheckActionCount.m_bHackingIndex)
					{
						if(pkPlayer->SuspectHacking(EAHP_ActionCount, PgGround::ms_kAntiHackCheckActionCount.m_iAddHackingPoint))
						{
							// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
							BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_ActionCount_Hack_C) );
							kDPacket.Push( pkPlayer->GetMemberGUID() );
							SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
						}
					}
				}
			}
		}

		//��Ŷ�� ���� �ð�/��ġ/�׼� �� �����Ѵ�.
		PLAYER_ACTION_INFO* pkOldActionInfo = pkPlayer->GetOldPlayerActionInfo();
		if(pkOldActionInfo)
		{
			pkOldActionInfo->SetPos(rkAction.dwTimeStamp, rkAction.ptPos);					
			pkOldActionInfo->iOldActionID = pkOldActionInfo->iActionID;
			pkOldActionInfo->iActionID = rkAction.iActionID;
			pkOldActionInfo->iGroundNo = GroundKey().GroundNo();
			pkOldActionInfo->iMoveSpeed = pkPlayer->GetPlayerActionInfo()->iMoveSpeed;
			//INFO_LOG(BM::LOG_LV6, __FL__ << _T("OldAction=") << pkOldActionInfo->iOldActionID << _COMMA_ << pkOldActionInfo->iActionID);
		}

		pkPlayer->MoveDirection(rkAction.ptDirection);
		POINT3BY ptPathNormal(rkAction.ptPathNormal.x, rkAction.ptPathNormal.y, 0);
		pkPlayer->PathNormal(ptPathNormal);
	}

	// ��ŷ�̶� ó�� �ϰ� CheckHackingAction �ȿ��� ��ŷ ī��Ʈ�� ���� �ǹǷ� ������ ���̸� ���´�.
	// ��ŷ�̸� �׼� ó�� ���� �ʴ´�. Ŭ��� � �����͵� ������ ����
	//if(bHacking)
	//{
	//	return ;
	//}
	
	//if (pkUnit->IsState(US_BIT_CANT_ATTACK))
	//{
	//	return;
	//}
	//INFO_LOG(BM::LOG_LV6, _T("ReqAction2....Action[%d], ActionParam[%d]"), rkAction.iActionID, (int)rkAction.byActionParam);

	AdjustArea( pkUnit, true, pkUnit->IsCheckZoneTime(0) );
	
	PgActionResultVector kActionResultVec;
	UNIT_PTR_ARRAY kTargetArray;
	BM::GUID kGuid[MAX_SKILL_TARGETNUM];
	//BYTE byCollision[MAX_SKILL_TARGETNUM];

	EActionResultCode eResult = CheckAction2(pkUnit, rkAction);
	DWORD dwSyncTime;
	
	pkPacket->Pop(dwSyncTime);
	// BM::GetTime32() - dwSyncTime = ��Ŷ�� ���µ� �ɸ� �ð�.
	DWORD dwSeedCallCounter = 0;
	pkPacket->Pop(dwSeedCallCounter);

	// RandomSeedCallCounter üĿ�� ����� ��
	if(PgGround::ms_kAntiHackCheckVariable.bUse_RandomSeedCallCounter)
	{
		if(dwSeedCallCounter != pkUnit->RandomSeedCallCounter())
		{
			// Player�� ���̵��� �� ��� �÷��̾� ������ ���� ������ ���� ��Ŷ�� ��Ȥ �ִ�. �̶� ������ ���� 0�̴�.
			// ������ 0�� ��� ���� ������ ��Ŷ�� ���� ��찡 �ִ�.
			if(0 != pkUnit->RandomSeedCallCounter())
			{
				PgPlayer* pkPlayer = NULL;
				if(pkUnit->IsUnitType(UT_PLAYER))
				{
					pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				}
				//��ƼƼ�� ���� ���߿� ó��
				//else if(pkUnit->IsUnitType(UT_ENTITY))
				//{
				//	CUnit* pkCaller = GetUnit(pkUnit->Caller());
				//	if( pkCaller )//�����ڰ� Entity�� ��ȯ�� ���
				//	{
				//		if(pkCaller->IsUnitType(UT_PLAYER))
				//		{
				//			pkPlayer = dynamic_cast<PgPlayer*>(pkCaller);
				//		}
				//	}				
				//}

				if(pkPlayer)
				{
					bool bShowHackingLog = false;
					bool bHackingUser = false;
					bool bResAction = false;

					// ī���� ���̰� ���ϰ� ���� ��� ��ŷ���� �����Ѵ�.
					if(static_cast<int>(dwSeedCallCounter - pkPlayer->RandomSeedCallCounter()) >= PgGround::ms_kAntiHackCheckVariable.iRandomSeedCallCounter_CheckValue)
					{
						bShowHackingLog = true;
						bHackingUser = true;
						bResAction = true;
					}
					else
					{
						// ī���� ������ ������ ���־�� �Ѵ�.
						// pkUnit�� Player / Entity�� �� �ֱ� ������
						if(dwSeedCallCounter > pkUnit->RandomSeedCallCounter())
						{
							while(dwSeedCallCounter != pkUnit->RandomSeedCallCounter())
							{
								pkUnit->GetRandom();
							}
						}
						else	
						{
							GET_DEF(CSkillDefMgr, kSkillDefMgr);
							CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(rkAction.iActionID);

							CSkillDef const* pkOldSkillDef = kSkillDefMgr.GetDef(pkPlayer->GetOldPlayerActionInfo()->iOldActionID);
							if(pkOldSkillDef)
							{
								if(EST_ACTIVE == pkOldSkillDef->GetType() || EST_TOGGLE == pkOldSkillDef->GetType())
								{
									bShowHackingLog = true;
								}
							}

							// Fire�� �ߵ� ���Ѿ� Fire ���� Ÿ���� ���� �� RandomSeedCallCounter ���� �Ѵ�.
							// �� TargetList�� ���� �Ǿ��� ��츸 ĵ�� ��Ŵ.
							if(ESS_TARGETLISTMODIFY == rkAction.byActionParam)
							{
								bShowHackingLog = true;
								bResAction = true;
							}

						}
					}

					if(bShowHackingLog)
					{
						//Ŭ���̾�Ʈ���� ������ Seed�� ��
						//dwSeedCallCounter Ŭ��� ���� �޾ƿ� pkUnit�� seedī���� �̴�.
						//�α׸� ����°��� player�̾�� �Ѵ�.
						//pkUnit �� pkPlayer�� �򰥸��� �ȵȴ�.
						PLAYER_ACTION_INFO* pkOldActionInfo = pkPlayer->GetOldPlayerActionInfo();
						//GetOldPlayerActionInfo ��  PgPlayer�� Member ������ �̹Ƿ� * �˻� ���� �ʾƵ� ��.

						BM::vstring kLogMsg; kLogMsg << __FL__ << _T("[HACKING][RandomSeedCallCounter] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() << "]  MemberGUID[" 
							<< pkPlayer->GetMemberGUID() <<"] GroundNo[" << GroundKey().GroundNo() << "] Client[") << dwSeedCallCounter << _T("] Server[") << pkUnit->RandomSeedCallCounter() << _T("] OldSkill[") << pkOldActionInfo->iOldActionID << _T("] NewSkill[") << pkOldActionInfo->iActionID << _T("]");

						//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
						HACKING_LOG(BM::LOG_LV0, kLogMsg);

						if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
						{
							pkPlayer->SendWarnMessageStr(kLogMsg);
						}
					}

					if(bHackingUser)
					{
						if(pkPlayer->SuspectHacking(EAHP_RandomSeedCallCounter, PgGround::ms_kAntiHackCheckVariable.sRandomSeedCallCounter_AddHackIndex))
						{
							// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
							BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_RandomSeedCallCounter) );
							kDPacket.Push( pkPlayer->GetMemberGUID() );
							SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
						}
					}

					if(bResAction)
					{
						short sErr = EActionR_Err_Unknown;
						// Send Error code
						BM::Stream kRPacket(PT_M_C_RES_ACTION2);
						kRPacket.Push(rkAction.iActionID);
						kRPacket.Push(rkAction.iActionInstanceID);
						kRPacket.Push(sErr);
						pkUnit->Send(kRPacket);
						return;
					}

				}// pkPlayer
			}//0 != pkUnit->RandomSeedCallCounter()
		}// dwSeedCallCounter != pkUnit->RandomSeedCallCounter()		
	}// Hacking Check

	BYTE byTargetNum = 0;
	pkPacket->Pop(byTargetNum);	

	if(EActionR_Success_Max >= eResult)
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(rkAction.iActionID);
		if(pkSkillDef)
		{
			switch( pkSkillDef->GetAbil(AT_ACTION_AFTER_CHANGE_BLOCK_ABLE)  )
			{// �׼� �Ŀ� ������
			case ECBA_MAKE_CAN_NOT_BLOCK:
				{// �Ұ��� �ϰ� �ϴ� �׼�����
					if(0 == pkUnit->GetAbil(AT_CAN_NOT_BLOCK_BY_ACTION) )
					{
						pkUnit->SetAbil(AT_CAN_NOT_BLOCK_BY_ACTION, 1, true, true);
					}
				}break;
			case ECBA_MAKE_CAN_BLOCK:
				{// ���� �ϰ� �ϴ� �׼�����
					if(0 < pkUnit->GetAbil(AT_CAN_NOT_BLOCK_BY_ACTION) )
					{
						pkUnit->SetAbil(AT_CAN_NOT_BLOCK_BY_ACTION, 0, true, true);
					}
				}break;
			}// ���� �ϰ�
			
			if(ESS_FIRE == rkAction.byActionParam)
			{
				if( EST_TOGGLE  != pkSkillDef->GetType() ) // ����� PgDefaultSkillFunction::SkillToggle���� ���� ó��
				{/// �׼� ������ �ɾ��� ����Ʈ�� ������ �ɾ��ش�
					int const iPrevActionID = pkUnit->GetAbil(AT_PREV_ACTION_ID);
					if(0 < iPrevActionID)
					{// ���� �׼��� ���� �Ǿ����Ƿ�
						CSkillDef const* pkPrevSkillDef = kSkillDefMgr.GetDef(iPrevActionID);
						if(pkPrevSkillDef)
						{// ���� �׼��� ������ 
							int const iAddEffectID =  pkPrevSkillDef->GetAbil(AT_ADD_EFFECT_WHEN_SKILL_END);
							if(0 < iAddEffectID)
							{// �ɾ��־���� ����Ʈ�� �����Ѵٸ� �ɾ��ְ�
								SEffectCreateInfo kCreate;
								kCreate.eType = EFFECT_TYPE_NORMAL;
								kCreate.iEffectNum = iAddEffectID;
								PgGroundUtil::SetActArgGround(kCreate.kActArg, this);
								kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
								pkUnit->AddEffect(kCreate);
							}
							{// ����ȿ���� ����ϴ� ��ų�̶��, �ߺ������� ���� ����� ���� �ʱ�ȭ�ϰ�
								CUnit* pkTarget = NULL;
								switch( pkUnit->UnitType() )
								{
								case UT_SUB_PLAYER:
									{
										pkTarget = GetUnit( pkUnit->Caller() );
									}break;
								default:
									{
										pkTarget = pkUnit;
									}break;
								}
								SkillFuncUtil::SetSkillLinkage_StampMark(pkTarget, pkPrevSkillDef, false);
							}
						}
					}
					pkUnit->SetAbil(AT_PREV_ACTION_ID, rkAction.iActionID); // ���� �׼��� �����صд�
				}
			}

			{// ������� 
				int const iValue = pkSkillDef->GetAbil(AT_AWAKE_SKILL_FIRE_ADD_EFFECT);
				if(iValue)
				{//��ų ���� ������� ���� �Ƹ� ����Ʈ�� ������ �ɾ��ش�.
					if(!pkUnit->GetEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO))
					{
						SEffectCreateInfo kCreate;
						kCreate.eType = EFFECT_TYPE_NORMAL;
						kCreate.iEffectNum = EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO;
						PgGroundUtil::SetActArgGround(kCreate.kActArg, this);
						kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

						pkUnit->AddEffect( kCreate );
					}
				}
				else
				{//������ �����־�� �Ѵ�.
					if(pkUnit->GetEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO))
					{
						pkUnit->DeleteEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO);
					}
				}
			}

			{// �ñر��
				int const iValue = pkSkillDef->GetAbil(AT_MASTER_SKILL_FIRE_ADD_EFFECT);
				if(iValue)
				{//��ų ���� �ñر�� ���� ����Ʈ�� ������ �ɾ��ش�.
					if(!pkUnit->GetEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO))
					{
						SEffectCreateInfo kCreate;
						kCreate.eType = EFFECT_TYPE_NORMAL;
						kCreate.iEffectNum = EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO;
						PgGroundUtil::SetActArgGround(kCreate.kActArg, this);
						kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

						pkUnit->AddEffect( kCreate );
					}
				}
				else
				{//������ �����־�� �Ѵ�.
					if(pkUnit->GetEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO))
					{
						pkUnit->DeleteEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO);
					}
				}
			}
		}
		else
		{//SkillDef�� ��ã���� ���δ� �����־�� �Ѵ�.
			if(pkUnit->GetEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO))
			{
				pkUnit->DeleteEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO);
			}

			if(pkUnit->GetEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO))
			{
				pkUnit->DeleteEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO);
			}		
		}
	}
	else
	{// ��ų ���� �ÿ��� �� �����־�� �Ѵ�.
		if(pkUnit->GetEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO))
		{
			pkUnit->DeleteEffect(EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO);
		}

		if(pkUnit->GetEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO))
		{
			pkUnit->DeleteEffect(EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO);
		}
	}
	
	// ESS_FIRE or ESS_TARGETLISTMODIFY ������� Target ������ ������ FireSkill ȣ��
	// ���� Target�� ���� ���(��ġ�� ��ų��)���� ESS_FIRE �� ��Ŷ���� �´�.
	switch( eResult )
	{
	case EActionR_Success_Fire:	// FireSkill
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(rkAction.iActionID);

			if (PgGround::ms_kAntiHackCheckVariable.bUse_MaxTarget)
			{
				if(byTargetNum > pkSkillDef->GetAbil(AT_MAX_TARGETNUM))
				{
					//Ÿ������ ��ų�� �ִ� Ÿ������ ���� �� ����.
					//��ŷ�� �ǽ��� ���� �Ѵ�.
					//��ų ��ü�� ĵ�� ���ѹ�����.
					eResult = EActionR_Err_LimitMaxTarget;
					//���� ���ϰ��� ���� �ε� Ŭ���̾�Ʈ�� ������ �˷����� �ʴ´�.
					PgPlayer* pkPlayer = NULL;
					if(pkUnit->IsUnitType(UT_PLAYER))
					{
						pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					}
					else if(pkUnit->IsUnitType(UT_ENTITY))
					{
						CUnit* pkCaller = GetUnit(pkUnit->Caller());
						if( pkCaller )//�����ڰ� Entity�� ��ȯ�� ���
						{
							if(pkCaller->IsUnitType(UT_PLAYER))
							{
								pkPlayer = dynamic_cast<PgPlayer*>(pkCaller);
							}
						}				
					}

					if(pkPlayer)
					{
						BM::vstring kLogMsg;
						kLogMsg << __FL__ << "[HACKING][MaxTarget] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() << "]  MemberGUID[" 
							<< pkPlayer->GetMemberGUID() <<"] GroundNo[" << GroundKey().GroundNo() << "] HackingType[SkillTargetNum] SkillNo[" << rkAction.iActionID 
							<< "] MaxTarget[" << pkSkillDef->GetAbil(AT_MAX_TARGETNUM) << "] TargetNum[" << byTargetNum << "]";
						//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
						HACKING_LOG(BM::LOG_LV0, kLogMsg);

						if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
						{
							pkPlayer->SendWarnMessageStr(kLogMsg);
						}

						if(pkPlayer->SuspectHacking(EAHP_MaxTarget, PgGround::ms_kAntiHackCheckVariable.sMaxTarget_AddIndex))
						{
							// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
							BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_MaxTarget_Hack) );
							kDPacket.Push( pkPlayer->GetMemberGUID() );
							SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
						}

						short sErr = EActionR_Err_Unknown;
						// Send Error code
						BM::Stream kRPacket(PT_M_C_RES_ACTION2);
						kRPacket.Push(rkAction.iActionID);
						kRPacket.Push(rkAction.iActionInstanceID);
						kRPacket.Push(sErr);
						pkUnit->Send(kRPacket);

						return;
					}
				}
			}

			//else if(byTargetNum == 0 && (pkSkillDef->GetTargetType() & ESTARGET_LOCATION) == 0)
			//else if(byTargetNum == 0 && rkAction.byActionParam != ESS_TARGETLISTMODIFY)
			//{
				// Target ������ ������ Fire ȣ���ؼ��� �ȵȴ�.
				// ESS_FIRE or ESS_TARGETLISTMODIFY ������ �������� �ι� ����.
				// ��ġ���� ��ų���� , TargetNum == 0 ������ FireSkill ȣ���ؾ� ��.
			//	break;
			//}

			if (true==PgGround::ms_kAntiHackCheckVariable.bUse_SkillRange)
			{
				int const iZAdded = ((pkSkillDef->GetAbil(AT_CASTER_STATE) & ECaster_State_Jumping) && (pkSkillDef->GetAbil(AT_ATTACK_UNIT_POS) & EAttacked_Pos_Blowup)) ? 95 : 0;

				// Skill : Target �� �����Ű��
				if ( CheckPathNormal(pkUnit, pkSkillDef, rkAction) )
				{
					if ( rkAction.byActionParam == ESS_FIRE || rkAction.byActionParam == ESS_TARGETLISTMODIFY)
					{
						g_kSkillOnFindTargetMgr.OnFindTarget(pkPacket, pkSkillDef, pkUnit, byTargetNum, this, rkAction, kTargetArray, kActionResultVec);//�ϳ��� �����ߴٰ� �� �����ϸ� �ȵ�
						FireSkill(pkUnit, kTargetArray, rkAction, &kActionResultVec);
					}
				}
			}
			else
			{	
				BM::GUID kGuid[MAX_SKILL_TARGETNUM];
				BYTE byCollision[MAX_SKILL_TARGETNUM];

				for (BYTE byI=0; byI<byTargetNum; ++byI)
				{
					pkPacket->Pop(kGuid[byI]);
					pkPacket->Pop(byCollision[byI]);
					CUnit* pkTarget = PgObjectMgr::GetUnit(kGuid[byI]);
					if (pkTarget != NULL)
					{
						kTargetArray.Add(pkTarget);
						kActionResultVec.GetResult(kGuid[byI], true)->SetCollision(byCollision[byI]);
					}

				}

				if ( rkAction.byActionParam == ESS_FIRE || rkAction.byActionParam == ESS_TARGETLISTMODIFY)
				{
					FireSkill(pkUnit, kTargetArray, rkAction, &kActionResultVec);	
				}
			}

			pkUnit->StandUpTime(0);		//Ŭ�� �׼���(���� ��) �������� �̹� �Ͼ ��Ȳ
			pkUnit->SetAbil(AT_POSTURE_STATE,0);
			if (pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE))
			{
				SClientProjectile kInfo;
				int iSkillRange = PgAdjustSkillFunc::GetAttackRange(pkUnit, pkSkillDef);
				if (iSkillRange < 100.0f)
				{
					iSkillRange += 50;	// SkillRange�� �ʹ� ª���� X �� ���൵ ���� ȿ�� ����.
				}
				kInfo.fDmgRangeQ = pow(iSkillRange * 2.0f , 2.0f);
				GetClinetProjectileInfo(pkUnit, rkAction, pkSkillDef, kInfo);
				// AT_PROJECTILE_NUM = 3126
				if (false == pkUnit->AddClientProjectile(rkAction.iActionInstanceID, kInfo))
				{
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if (pkPlayer)
					{
						// �̰� ��Ŀ��
						BM::vstring kLogMsg;
						kLogMsg << __FL__ << "[HACKING][Projectile] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() << "] MemberGUID[" << pkPlayer->GetMemberGUID() 
							<< "] GroundNo[" << GroundKey().GroundNo() << "] Projectile Add failed...Action InstanceID : " << rkAction.iActionInstanceID;
						//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
						HACKING_LOG(BM::LOG_LV0, kLogMsg);
						if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
						{
							pkPlayer->SendWarnMessageStr(kLogMsg);
						}

						if (pkPlayer->SuspectHacking(EAHP_Projectile, PgGround::ms_kAntiHackCheckVariable.sProjectile_AddIndex))
						{
							// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
							BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_Projectile_Hack) );
							kDPacket.Push( pkPlayer->GetMemberGUID() );
							SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
						}
					}
				}
			}
		}break;
	case EActionR_Success_Toggle:	// ToggleSkill
		{			
			FireToggleSkill(pkUnit, kTargetArray, rkAction, &kActionResultVec);
		}break;
	}

	if (eResult < EActionR_Success_Max)	// Success --> Broadcasting
	{
		// Send Broadcasting
		bool bExtraData = false;
		pkPacket->Pop(bExtraData);
		BYTE byResultNum = kActionResultVec.GetValidCount();
		POINT3 pt3GoalPos(0,0,0);
		rkAction.dwTimeStamp = GetTimeStamp();

		BM::Stream kMPacket(PT_M_C_NFY_ACTION2);
		rkAction.SendToClient(kMPacket);
		kMPacket.Push(dwSyncTime);
		kMPacket.Push(pt3GoalPos);
		kActionResultVec.WriteToPacket(kMPacket);
#ifdef AI_DEBUG
		//BM::GUID kSTarget;
		//PgActionResult* pkTResult = kActionResultVec.GetResult(0, kSTarget);
		//if (pkTResult != NULL)
		//{
		//	INFO_LOG(BM::LOG_LV9, _T("[%s] ActionResult GetRemainHP[%d]"), __FUNCTIONW__, pkTResult->GetRemainHP());
		//}
#endif

		if(byTargetNum != 0)
		{
			// ���� �ɸ��� �ȵ�.. Ȥ�� ���� ���� ó��.
			kMPacket.Push((bool)0);		// Use Extra Data
		}
		else
		{
			kMPacket.Push(bExtraData);
			if(bExtraData)
			{
				kMPacket.Push(*pkPacket);
			}
		}
		if( pkUnit->IsUnitType(UT_PLAYER) )
		{
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			pkPlayer->SetActionID(rkAction.iActionID, (Direction)rkAction.byDirection);
			pkPlayer->SetActionInstanceID(rkAction.iActionInstanceID);

			if(ACTIONID_A_DASH == rkAction.iActionID)
			{
				PgAddAchievementValue kMA(AT_ACHIEVEMENT_SPRINT, 1, GroundKey());
				kMA.DoAction(pkPlayer,NULL);
			}
			//INFO_LOG(BM::LOG_LV9, _T("[%s] ACtionID[%d], Direction[%d]"), __FUNCTIONW__, rkAction.iActionID, (int)rkAction.byDirection);
		}
#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("[%s] Broadcast....ActionID[%d], ActionParam[%d]"), __FUNCTIONW__, rkAction.iActionID, (int)rkAction.byActionParam);
#endif
		SendToArea( &kMPacket, pkUnit->LastAreaIndex(), BM::GUID::NullData(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );

	}
	else	// FAILED
	{
		INFO_LOG(BM::LOG_LV0, __FL__  << _T("User[") << pkUnit->Name() << _T("] Action Error : ReqAction2 Return [") << static_cast<int>(eResult) << _T("], ActionID[") << rkAction.iActionID << _T("]"));

		// Send Error code
		BM::Stream kRPacket(PT_M_C_RES_ACTION2);
		kRPacket.Push(rkAction.iActionID);
		kRPacket.Push(rkAction.iActionInstanceID);
		kRPacket.Push((short int)eResult);
		pkUnit->Send(kRPacket);
	}
}

bool PgGround::CheckAction2_SubFunc_CheckPlayerAction(PgPlayer* pkPlayer, CSkillDef* pkSkillDef, SActionInfo const& rkAction, EActionResultCode& rkOut)
{
	if(	pkPlayer 
		&& pkSkillDef
		)
	{
		if(pkSkillDef->IsSkillAtt(SAT_DEFAULT) == false && pkPlayer->GetMySkill()->IsExist(rkAction.iActionID, true)==false)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NotLearned"));
			rkOut = EActionR_Err_NotLearned;
			return true;
		}

		// Class Limit
		__int64 const i64Value = pkSkillDef->GetAbil64(AT_CLASSLIMIT);
		if (i64Value != 0 && !IS_CLASS_LIMIT(i64Value, pkPlayer->GetAbil(AT_CLASS)))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_ClassLimit"));
			rkOut = EActionR_Err_ClassLimit;
			return true;
		}

		// Weapon limit
		int iValue = pkSkillDef->GetAbil(AT_WEAPON_LIMIT);
		if ( iValue != 0 )
		{
			PgBase_Item kItem;
			if (S_OK != pkPlayer->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kItem))
			{
				INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot use skill [WEAPON_LIMIT]...Player has no weapon..ActionID["<<rkAction.iActionID<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_WeaponLimit"));
				rkOut = EActionR_Err_WeaponLimit;
				return true;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef* pkDef = (CItemDef*) kItemDefMgr.GetDef(kItem.ItemNo());
			if (pkDef == NULL)
			{
				INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot find ItemDef....ItemNo["<<kItem.ItemNo()<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_WeaponLimit"));
				rkOut = EActionR_Err_WeaponLimit;
				return true;
			}
			if ((pkDef->GetAbil(AT_WEAPON_TYPE) & iValue) == 0)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_WeaponLimit"));
				rkOut = EActionR_Err_WeaponLimit;
				return true;
			}
		}
	}
	return false;
}

// Skill �� �˻��Ѵ�.
// [RETURN]
// EActionResultCode ����
EActionResultCode PgGround::CheckAction2(CUnit* pkUnit, SActionInfo const &rkAction)
{
	EActionResultCode eReturn = EActionR_Success;

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef* pkSkillDef = (CSkillDef*) kSkillDefMgr.GetDef(rkAction.iActionID);
	if (pkSkillDef == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot find SkillDef....User["<<pkUnit->GetID()<<L"], ActionID["<<rkAction.iActionID<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NoSkillDef"));
		return EActionR_Err_NoSkillDef;
	}
	
	if(GetAttr() & pkSkillDef->GetAbil(AT_CANT_USE_THIS_GATTR_FLAG))
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot use this Ground ....User["<<pkUnit->GetID()<<L"], Ground["<<GetAttr() <<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_CantUseThisGround"));
		return EActionR_Err_CantUseThisGround;
	}

	ESkillType eType = (ESkillType) pkSkillDef->GetType();
	if (eType == EST_ACTIVE || eType == EST_TOGGLE)
	{
		if (rkAction.byActionParam == ESS_TOGGLE_ON || rkAction.byActionParam == ESS_FIRE)
		{
			if (!pkUnit->CheckSkillFilter(rkAction.iActionID))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_StateLimit"));
				return EActionR_Err_StateLimit;
			}
		}
		if ((eType == EST_TOGGLE && rkAction.byActionParam == ESS_FIRE)
			|| (eType != EST_TOGGLE && (rkAction.byActionParam == ESS_TOGGLE_ON || rkAction.byActionParam == ESS_TOGGLE_OFF)))
		{
			INFO_LOG(BM::LOG_LV3, __FL__<<L"SkillID["<<rkAction.iActionID<<L"], SkillType("<<eType<<L") doesn't match with ActinParam["<<rkAction.byActionParam<<L"] UnitGuid["<<pkUnit->GetID()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_ToggleOnOff"));
			return EActionR_Err_ToggleOnOff;
		}

		if (eType == EST_TOGGLE && rkAction.byActionParam == ESS_FIRE)
		{
			INFO_LOG(BM::LOG_LV3, __FL__<<L"Toggle skill cannot be ESS_FIRE UnitGuid["<<pkUnit->GetID()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_ToggleOnOff"));
			return EActionR_Err_ToggleOnOff;
		}
		// Checking Skill limit

		switch ( pkUnit->UnitType() )
		{
		case UT_PET:
			{
				PgPet *pkPet = dynamic_cast<PgPet*>(pkUnit);
				if ( pkPet )
				{
					if( pkSkillDef->IsSkillAtt(SAT_DEFAULT) == false && pkPet->GetMySkill()->IsExist(rkAction.iActionID, true)==false )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NotLearned"));
						return EActionR_Err_NotLearned;
					}
				}
			}break;
		case UT_PLAYER:
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				EActionResultCode eResult = EActionR_Success;
				if( CheckAction2_SubFunc_CheckPlayerAction(pkPlayer, pkSkillDef, rkAction, eResult) )
				{
					return eResult;
				}
			}break;
		case UT_SUB_PLAYER:
			{
				CUnit* pkOwner = GetUnit( pkUnit->Caller() );
				if(pkOwner)
				{
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkOwner);
					EActionResultCode eResult = EActionR_Success;
					if( CheckAction2_SubFunc_CheckPlayerAction(pkPlayer, pkSkillDef, rkAction, eResult) )
					{
						return eResult;
					}
				}
			}break;
		default:
			{

			}break;
		}

		/////////////////////
		// TODO : State Limit checking....(NoLimit=0, Normal=1, Battle=2, Dead=3)
		/////////////////////
		/*
		if (rkAction.byActionParam == ESS_TARGETLISTMODIFY)
		{
			// SAT_MAINTENANCE_CAST �Ӽ��� �����Ǿ� �ִ� ��ų�� ESS_TARGETLISTMODIFY�� ����� �� �ִ�.
			if (!pkSkillDef->IsSkillAtt(SAT_MAINTENANCE_CAST))
			{
				return EActionR_Err_MCastAttr;
			}
			if (pkUnit->GetSkill()->GetSkillNo() != rkAction.iActionID)
			{
				return EActionR_Err_ModifyTarget;	// ESS_FIRE �� ���Ŀ� ESS_TARGETLISTMODIFY �;߸� �Ѵ�.
			}
		}
		*/
		if ( !pkSkillDef->GetAbil(AT_CAST_TIME) || rkAction.byActionParam == ESS_CASTTIME )
		{
			if (pkUnit->GetAbil(AT_CANNOT_CASTSKILL) > 0)
			{
				// Casting Type�� ��ų ��� ����~
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_StateLimit"));
				eReturn = EActionR_Err_StateLimit;
			}
			else
			{
				SActArg kArg;
				g_kSkillAbilHandleMgr.SkillBegin(pkUnit, rkAction.iActionID, &kArg, (unsigned long)rkAction.byActionParam);
			}
		}
		if (rkAction.byActionParam == ESS_TOGGLE_ON || rkAction.byActionParam == ESS_TOGGLE_OFF)
		{
			if (pkSkillDef->GetType() != EST_TOGGLE)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NotToggle"));
				return EActionR_Err_NotToggle;
			}
			if (pkUnit->IsUnitType(UT_PLAYER))
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if (!pkPlayer->GetMySkill()->SetToggle((rkAction.byActionParam == ESS_TOGGLE_ON) ? true : false, rkAction.iActionID))
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_ToggleOnOff"));
					return EActionR_Err_ToggleOnOff;
				}
			}
		}

		eReturn = pkUnit->GetSkill()->CheckSkillAction(rkAction.iActionID, (ESkillStatus)rkAction.byActionParam, rkAction.dwTimeStamp,pkUnit->GetAbil(AT_R_COOLTIME_RATE_SKILL), pkUnit->GetAbil(AT_CAST_TIME_RATE));
		if (eReturn > EActionR_Success_Max)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << static_cast<short int>(eReturn));
			return eReturn;
		}
	}
	else if (eType == EST_GENERAL)
	{
		//char chFunction[100];
		int iBasicSkill = pkSkillDef->GetAbil(AT_TEMPLATE_SKILLNO);
		if (iBasicSkill > 0)
		{
			//sprintf_s(chFunction, 100, "Action_Fire%d", iBasicSkill);
			SActArg kArg;
			PgGroundUtil::SetActArgGround(kArg, this);
			/*eReturn = (EActionResultCode)*/
			//INFO_LOG(BM::LOG_LV9, _T("[%s] LuaCall [%s]"), __FUNCTIONW__, UNI(chFunction));
			g_kSkillAbilHandleMgr.SkillFire(pkUnit, rkAction.iActionID, (unsigned long)rkAction.byActionParam, &kArg);
		}
	}

	if ( rkAction.byActionParam == ESS_FIRE || rkAction.byActionParam == ESS_TOGGLE_ON )
	{
		int const iNeedSupply = pkSkillDef->GetAbil(AT_CREATE_SUMMONED_SUPPLY);
		if(iNeedSupply)
		{
			int const iCount = GetTotalSummonedSupply(pkUnit);
			int const iMaxSupply = pkUnit->GetAbil(AT_C_SUMMONED_MAX_SUPPLY);
			if(iMaxSupply < iCount+iNeedSupply)
			{
				return EActionR_Err_NeedSummonSupply;
			}
			
			bool const bUniqueClass = (pkSkillDef->GetAbil(AT_CREATE_UNIQUE_SUMMONED)>0);
			if(bUniqueClass)
			{
				if( pkUnit->IsSummonUnitClass( pkSkillDef->GetAbil(AT_CLASS) ) ) 
				{
					pkUnit->SendWarnMessage(792102);
					return EActionR_Err_Already_UniqueSummon;
				}
			}
		}

		int iNeedHP = pkSkillDef->GetAbil(AT_NEED_HP);
		int iNeedMP = pkSkillDef->GetAbil(AT_NEED_MP);
		int const iNeedHPPer = pkUnit->GetAbil(AT_ADD_R_NEED_HP); // �Ҹ�HP�� %
		int const iNeedMPPer = pkUnit->GetAbil(AT_ADD_R_NEED_MP); // �Ҹ�MP�� %
		int const iNeedHPPer2 = pkUnit->GetAbil(AT_ADD_R_NEED_HP_2); // �Ҹ�HP�� ����%
		int const iNeedMPPer2 = pkUnit->GetAbil(AT_ADD_R_NEED_MP_2); // �Ҹ�MP�� ����%

		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
		int const iNeedMaxHPPer = pkSkillDef->GetAbil(AT_NEED_MAX_R_HP);
		int const iNeedMaxMPPer = pkSkillDef->GetAbil(AT_NEED_MAX_R_MP);

		//��ų ���� MaxHP�� %�� �Ҹ� ��Ű�� ��� iNeedHp�� ���� ���� �ȴ�.
		if(0 < iNeedMaxHPPer)
		{
			iNeedHP = static_cast<int>(iMaxHP * (static_cast<float>(iNeedMaxHPPer) / ABILITY_RATE_VALUE_FLOAT));			
		}
		//��ų ���� MaxMP�� %�� �Ҹ� ��Ű�� ��� iNeedMp�� ���� ���� �ȴ�.
		if(0 < iNeedMaxMPPer)
		{
			iNeedMP = static_cast<int>(iMaxMP * (static_cast<float>(iNeedMaxMPPer) / ABILITY_RATE_VALUE_FLOAT));			
		}

		if(0 < iNeedHPPer)
		{
			iNeedHP -= static_cast<int>(iNeedHP * (static_cast<float>((ABILITY_RATE_VALUE - iNeedHPPer) + iNeedHPPer2) / ABILITY_RATE_VALUE_FLOAT));
		}
		if(0 < iNeedMPPer)
		{
			iNeedMP -= static_cast<int>(iNeedMP * (static_cast<float>((ABILITY_RATE_VALUE - iNeedMPPer) + iNeedMPPer2) / ABILITY_RATE_VALUE_FLOAT));
		}

		if (0 < iNeedHP)
		{
			// ��ų ���� �ʿ��� �ּ� ���� ���� ��� üũ�Ѵ�
			int const iNeedMinValue = pkSkillDef->GetAbil(AT_NEED_HP_MIN_VALUE);
			int const iCurrent = pkUnit->GetAbil(AT_HP);

			if(0 < iNeedMinValue)
			{
				if(iCurrent < iNeedMinValue)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NeedHP"));
					return EActionR_Err_NeedHP;
				}
			}

			if (iCurrent < iNeedHP)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NeedHP"));
				return EActionR_Err_NeedHP;
			}
			pkUnit->SetAbil(AT_HP, iCurrent-iNeedHP, true );
		}
		if (0 < iNeedMP)
		{
			// ��ų ���� �ʿ��� �ּ� ���� ���� ��� üũ�Ѵ�
			int const iNeedMinValue = pkSkillDef->GetAbil(AT_NEED_MP_MIN_VALUE);
			int const iCurrent = pkUnit->GetAbil(AT_MP);

			if(0 < iNeedMinValue)
			{
				if(iCurrent < iNeedMinValue)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NeedHP"));
					return EActionR_Err_NeedHP;
				}
			}
			
			if (iCurrent < iNeedMP)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NeedMP"));
				return EActionR_Err_NeedMP;
			}
			pkUnit->SetAbil(AT_MP, iCurrent-iNeedMP, true);
		}

		// ������ ������ ��� �ϴ� ��

		// ������ ������ ���밪 ����
		int iNeedAwake = pkSkillDef->GetAbil(AT_NEED_AWAKE); 

		// ������ �������� ��ü�� %�� �Ҹ�
		int const iNeedAwakePer = pkSkillDef->GetAbil(AT_NEED_MAX_R_AWAKE);

		if(0 < iNeedAwakePer)
		{
			iNeedAwake = static_cast<int>(AWAKE_VALUE_MAX * (static_cast<float>(iNeedAwakePer) / ABILITY_RATE_VALUE_FLOAT));			
		}

		// ������ ������ ���� ���
		int const iAddNeedAwakePer = pkUnit->GetAbil(AT_ADD_NEED_R_AWAKE);
		if(iAddNeedAwakePer)
		{
			iNeedAwake -= static_cast<int>(iNeedAwake * (static_cast<float>(iAddNeedAwakePer) / ABILITY_RATE_VALUE_FLOAT));
		}

		// �����ִ� ��� ������ ������ �Ҹ��ϴ� ���
		int const iAllNeedAwake = pkSkillDef->GetAbil(AT_ALL_NEED_AWAKE); 
		if(0 < iAllNeedAwake)
		{
			//���� �ִ� ��� ������ �������� �Ҹ� �Ѵ�.
			iNeedAwake = pkUnit->GetAbil(AT_AWAKE_VALUE);
		}

		if (0 < iNeedAwake)
		{
			int iCurrent = pkUnit->GetAbil(AT_AWAKE_VALUE);
			if (iCurrent < iNeedAwake)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NeedAwake"));
				return EActionR_Err_NeedAwake;
			}

			pkUnit->SetAbil(AT_AWAKE_VALUE, iCurrent-iNeedAwake, true );
			pkUnit->DeleteEffect(EFFECTNO_AWAKE_NORMAL_STATE);
			pkUnit->DeleteEffect(EFFECTNO_AWAKE_MAX_STATE);
		}
		else
		{
			// ������ �������� 0�� ��쿡 AllNeedAwake�� �ִ� ��ų ����
			if(iAllNeedAwake)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_NeedAwake"));
				return EActionR_Err_NeedAwake;
			}
		}
	}
	return eReturn;
}

void PgGround::Broadcast( BM::Stream const& rkPacket,BM::GUID const &kCallerCharGuid, DWORD const dwSendFlag )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	CUnit *pkUnit = GetUnit( kCallerCharGuid );
	Broadcast( rkPacket, pkUnit, dwSendFlag );
}

void PgGround::Broadcast( BM::Stream const& rkPacket, CUnit const *pkCaller, DWORD const dwSendFlag )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	BroadcastUtil::CONT_BROAD_LIST kContTarget;

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator unit_itr;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
	{
		if ( pkUnit )
		{
			if ( pkUnit != pkCaller )
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if(	pkPlayer )
				{
					BroadcastUtil::AddSwitchInfo(kContTarget, pkPlayer->GetSwitchServer(), pkPlayer->GetMemberGUID()); // ��ε�ĳ��Ʈ ��Ͽ� ����
				}
			}

			CONT_OBSERVER_SWITH_MEMBER_LIST kContList;
			pkUnit->GetObserverMemberList( kContList );
			BroadcastUtil::AddSwitchInfo(kContTarget, kContList);
		}
	}

	BroadcastUtil::Broadcast(kContTarget, rkPacket);
}

void PgGround::Broadcast( BM::Stream const& rkPacket, VEC_GUID& rkVecUnit, DWORD const dwSendFlag)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	VEC_GUID::iterator guid_itr;
	for(guid_itr=rkVecUnit.begin();guid_itr!=rkVecUnit.end();++guid_itr)
	{
		CUnit *pkUser = GetUnit(*guid_itr);
		if ( pkUser )
		{
			pkUser->Send( rkPacket, dwSendFlag|E_SENDTYPE_SELF);
		}
	}
}

void PgGround::SendToParty(BM::GUID const& rkPartyGuid, BM::Stream & Packet)
{
	VEC_GUID MemberList;
	if( m_kLocalPartyMgr.GetPartyMember(rkPartyGuid, MemberList) )
	{
		SendToUser_ByGuidVec(MemberList, Packet);
	}
}

// �α� Area���� Broadcasting  �Ѵ�
void PgGround::SendToArea( BM::Stream *pkPacket, int const iAreaIndex, BM::GUID const &rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag )
{
	PgSmallArea *pkArea = GetArea( iAreaIndex );
	if ( pkArea )
	{
		SendToArea( pkPacket, *pkArea, rkIgnoreGuid, bySyncTypeCheck, dwSendFlag );
	}
}

void PgGround::SendToArea( BM::Stream *pkPacket, PgSmallArea const & rkSmallArea, BM::GUID const &rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag )
{
	if ( rkSmallArea.IsBigArea() )
	{
		Broadcast( *pkPacket, rkIgnoreGuid, dwSendFlag );
	}
	else
	{
		rkSmallArea.Broadcast_Adjacent( pkPacket, rkIgnoreGuid, bySyncTypeCheck, dwSendFlag );
	}
}

bool PgGround::InitRareMonsterGen()
{
	m_kContRareMonsterGen.clear();
	m_kContGeneratedMonster.clear();

	const CONT_DEF_GROUND_RARE_MONSTER * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		return false;
	}

	CONT_DEF_GROUND_RARE_MONSTER::const_iterator iter = pkDef->find(GetGroundNo());
	if(iter == pkDef->end())
	{
		return false;
	}

	CGameTime kGameTime;
	__int64 const i64CurTime = kGameTime.GetLocalSecTime();

	CONT_DEF_GROUND_RARE_MONSTER::mapped_type const & kMonTable = (*iter).second;

	for(CONT_DEF_GROUND_RARE_MONSTER::mapped_type::const_iterator geniter = kMonTable.begin();geniter != kMonTable.end();++geniter)
	{
		CONT_DEF_GROUND_RARE_MONSTER::mapped_type::mapped_type const & kMon = (*geniter).second;
		m_kContRareMonsterGen.insert(std::make_pair(kMon.iMonNo,SMonsterGen(kMon.iMonNo,i64CurTime + kMon.iDelayGenTime)));
	}

	return true;
}

bool PgGround::GetRareMonster(int const iMonBagNo,int & iMonNo) const
{
	CONT_DEF_MONSTER_BAG const *pkContMonsterBag;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pkContMonsterBagElement;
	
	g_kTblDataMgr.GetContDef(pkContMonsterBag);
	g_kTblDataMgr.GetContDef(pkContMonsterBagElement);

	if(NULL == pkContMonsterBag || NULL == pkContMonsterBagElement)
	{
		return false;
	}

	CONT_DEF_MONSTER_BAG::const_iterator iter = pkContMonsterBag->find(iMonBagNo);
	if(iter == pkContMonsterBag->end())
	{
		return false;
	}

	CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator moniter = pkContMonsterBagElement->find((*iter).second.iElementNo);
	if(moniter == pkContMonsterBagElement->end())
	{
		return false;
	}

	size_t kIdx = 0;
	if(false == RouletteRate((*iter).second.iRateNo,kIdx,MAX_SUCCESS_RATE_ARRAY))
	{
		return false;
	}

	iMonNo = (*moniter).second.aElement[kIdx];

	return true;
}

bool PgGround::InitEventMonsterGen()
{
	CONT_DEF_EVENT_GROUP const * pEventGroup = NULL;
	g_kTblDataMgr.GetContDef(pEventGroup);
	if( NULL == pEventGroup )
	{
		return false;
	}

	CONT_DEF_EVENT_MONSTER_GROUP const * pEventMonsterGroup = NULL;
	g_kTblDataMgr.GetContDef(pEventMonsterGroup);
	if( NULL == pEventMonsterGroup )
	{
		return false;
	}

	m_ContEventMonsterGen.clear();
	m_ContGeneratedEventMonster.clear();
	m_ContEventDropAnyMonster.clear();

	BM::PgPackedTime NowTime;
	NowTime.SetLocalTime();

	CGameTime GameTime;
	__int64 const CurTime = GameTime.GetLocalSecTime();

	CONT_DEF_EVENT_GROUP::const_iterator iter = pEventGroup->begin();
	for( ; iter != pEventGroup->end() ; ++iter )		// ���� ���� �̺�Ʈ�� ��� Ȯ��.
	{
		CONT_DEF_EVENT_GROUP::mapped_type const & Element = (*iter).second;

		if( !(NowTime < Element.EndTime) )
		{
			continue;
		}

		CONT_DEF_EVENT_MONSTER_GROUP::const_iterator mon_iter = pEventMonsterGroup->find(Element.MonsterGroupNo);	// �����ؾߵ� ���� �׷��� ã�´�.
		if( mon_iter != pEventMonsterGroup->end() )
		{
			CONT_DEF_EVENT_MONSTER_GROUP::mapped_type const & MonsterElement = (*mon_iter).second;

			CONT_EVENTMONSTERGROUP::const_iterator group_iter = MonsterElement.find(GroundKey().GroundNo());	// ���� �׶��忡�� ���� �� ���Ͱ� �ִ��� ã�´�.
			if( group_iter != MonsterElement.end() )
			{
				CONT_EVENTMONSTERGROUP::mapped_type const & GruopElement = (*group_iter).second;
				InitEventMonsterGen_sub(Element, GruopElement.MonsterInfo, true, NowTime, CurTime);
			}

			CONT_EVENTMONSTERGROUP::const_iterator event_iter = MonsterElement.find(0);	// Ű���� 0�� ���� �׷��� ���� �׷��� �ƴϰ� ���� �ʿ� ��� ���Ͱ� ���� �������� ����ϵ��� ��.
			if( event_iter != MonsterElement.end() )
			{
				CONT_EVENTMONSTERGROUP::mapped_type const & GruopElement = (*event_iter).second;
				InitEventMonsterGen_sub(Element, GruopElement.MonsterInfo, false, NowTime, CurTime);
			}
		}
	}

	return true;
}

void PgGround::InitEventMonsterGen_sub(SEventGroup const& rkElement, VEC_EVENTMONSTERINFO const& rkMonsterInfo, bool const bInsertMonster, BM::PgPackedTime const& rkNowTime, __int64 const& rkCurTime)
{
	for(VEC_EVENTMONSTERINFO::const_iterator info_iter = rkMonsterInfo.begin(); info_iter != rkMonsterInfo.end() ; ++info_iter)
	{
		VEC_EVENTMONSTERINFO::value_type const & InfoElement = (*info_iter);
		if(InfoElement.MonsterNo == 0)
		{
			SEventDropAnyMonster DropAnyMonster;
			DropAnyMonster.bIsEventTime = false;
			DropAnyMonster.StartTime = rkElement.StartTime;
			DropAnyMonster.EndTime = rkElement.EndTime;
			DropAnyMonster.RewardItemGroupNo = InfoElement.RewardItemGroup;
			DropAnyMonster.RewardCount = InfoElement.RewardItemCount;

			m_ContEventDropAnyMonster.push_back(DropAnyMonster);
		}
		else
		{
			SEventMonsterGen EventMonsterGen;
			EventMonsterGen.MonNo = InfoElement.MonsterNo;
			EventMonsterGen.GenPos = InfoElement.GenPos;
			EventMonsterGen.RewardCount = InfoElement.RewardItemCount;
			EventMonsterGen.RewardItemGroupNo = InfoElement.RewardItemGroup;
			EventMonsterGen.DelayTime = rkElement.DelayGenTime;
			EventMonsterGen.RegenPeriod = rkElement.RegenPeriod;
			EventMonsterGen.GenerateMessage = rkElement.Message;
			EventMonsterGen.IsInsertMonster = bInsertMonster;

			__int64 Now = 0;
			BM::DBTIMESTAMP_EX DBNowTime(rkNowTime);		// ���� �ð�. CurTime�� ����Ÿ���̶� ����� �� ����.
			CGameTime::DBTimeEx2SecTime(DBNowTime, Now);

			__int64 EndTime = 0;
			BM::DBTIMESTAMP_EX DBEndTime(rkElement.EndTime);
			CGameTime::DBTimeEx2SecTime(DBEndTime, EndTime);
			EventMonsterGen.DelTime = rkCurTime + ( EndTime - Now );	// �ش� ���� �̺�Ʈ �ð��� ������ �����ؾߵ� �ð�.

			if( rkNowTime < rkElement.StartTime )		// �̺�Ʈ ���� �ð� ���̶��.
			{
				__int64 StartTime = 0;
				BM::DBTIMESTAMP_EX DBStartTime(rkElement.StartTime);		// �̺�Ʈ ���۽ð�.
				CGameTime::DBTimeEx2SecTime(DBStartTime, StartTime);

				EventMonsterGen.GenTime = rkCurTime + ( StartTime - Now );
			}
			else if( (rkElement.StartTime < rkNowTime) && (rkNowTime < rkElement.EndTime) )
			{
				EventMonsterGen.GenTime = rkCurTime + EventMonsterGen.DelayTime;		// �̺�Ʈ ���̶�� ������ �ð� �Ŀ� ����.
			}
			else
			{
				continue;		// �׿ܿ� ����Ű�� �ʴ´�.
			}

			m_ContEventMonsterGen.insert( std::make_pair( EventMonsterGen.MonNo, EventMonsterGen) );
		}
	}
}

bool PgGround::EventMonsterGenerate()
{
	CGameTime GameTime;
	__int64 const CurTime = GameTime.GetLocalSecTime();

	CONT_EVENT_MONSTER_GEN::iterator iter = m_ContEventMonsterGen.begin();
	for( ; iter != m_ContEventMonsterGen.end() ; ++iter )
	{
		CONT_EVENT_MONSTER_GEN::mapped_type & Element = (*iter).second;

		if(false==Element.IsInsertMonster)
		{
			continue;
		}

		CONT_GENERATED_MONSTER::const_iterator gen_iter = m_ContGeneratedEventMonster.find(Element.MonNo);
		if( gen_iter != m_ContGeneratedEventMonster.end() )
		{
			continue;
		}

		if( Element.GenTime > CurTime )
		{
			continue;
		}

		BM::GUID TempGuid;
		if( Element.GenPos == POINT3::NullData() )
		{
			PgGenPoint GenPoint;
			GetRandomMonsterGenPoint(12, GenPoint);
			InsertMonster(GenPoint.Info(), Element.MonNo, TempGuid);
		}
		else
		{
			TBL_DEF_MAP_REGEN_POINT GenPoint;
			GenPoint.iMapNo = GetGroundNo();
			GenPoint.pt3Pos = Element.GenPos;
			InsertMonster(GenPoint, Element.MonNo, TempGuid);
		}		

		m_ContGeneratedEventMonster.insert( Element.MonNo );

		if( !Element.GenerateMessage.empty() )
		{
			BM::Stream kBroadPacket( PT_N_C_NFY_NOTICE_PACKET, static_cast<size_t>(1) );
			kBroadPacket.Push(NOTICE_REGEN_EVENT_MONSTER);
			kBroadPacket.Push( GetGroundNo() );
			kBroadPacket.Push( Element.GenerateMessage );
			SendToCenter(kBroadPacket);
		}
	}

	return true;
}

bool PgGround::DropAnyMonsterTimeCheck()
{
	BM::DBTIMESTAMP_EX NowTime;
	NowTime.SetLocalTime();

	VEC_EVENTDROPANYMONSTER::iterator iter = m_ContEventDropAnyMonster.begin();
	while( iter != m_ContEventDropAnyMonster.end() )
	{
		VEC_EVENTDROPANYMONSTER::value_type & Element = *iter;
		if( Element.EndTime < NowTime )
		{	// �ð� ����. �����̳ʿ��� ����.
			iter = m_ContEventDropAnyMonster.erase(iter);
			continue;
		}

		if( (Element.StartTime <= NowTime) && (NowTime < Element.EndTime) )
		{
			Element.bIsEventTime = true;
		}

		++iter;
	}

	return true;
}

void PgGround::DropAnyMonsterCheck(CONT_MONSTER_DROP_ITEM & DropItems) const
{
	CONT_DEF_EVENT_REWARD_ITEM_GROUP const * pRewardItemGroup = NULL;
	g_kTblDataMgr.GetContDef(pRewardItemGroup);

	BM::CAutoMutex Lock(m_kRscMutex);

	if( pRewardItemGroup )
	{
		VEC_EVENTDROPANYMONSTER::const_iterator iter = m_ContEventDropAnyMonster.begin();
		for( ; iter != m_ContEventDropAnyMonster.end() ; ++iter)
		{
			VEC_EVENTDROPANYMONSTER::value_type const & Element = *iter;
			if( Element.bIsEventTime )
			{
				CONT_DEF_EVENT_REWARD_ITEM_GROUP::const_iterator item_iter = pRewardItemGroup->find(Element.RewardItemGroupNo);
				if( item_iter != pRewardItemGroup->end() )
				{
					int TotalRate = 0;
					bool bAlldrop = false;
					bool ExceptionCase = false;
					VEC_EVENTREWARDITEMGROUP::const_iterator drop_iter = (*item_iter).second.begin();
					for( ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
					{
						TotalRate += (*drop_iter).Rate;
					}

					int AllDropRate = (*item_iter).second.size() * 100;
					if( AllDropRate == TotalRate )
					{	// �����̳ʿ� ����ִ� ������ ��� Ȯ���� ��� 100�̶��..
						bAlldrop = true;
					}

					if( TotalRate < 100 )
					{	// ����Ȯ���� 100�� �ȵǸ�
						ExceptionCase = true;
					}

					if( bAlldrop )
					{
						for( drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter )
						{
							for( int i = 0 ; i < (*drop_iter).Count ; ++i )
							{	// ������ ���� ��ŭ ������ ���ڷ� ���.
								PgBase_Item Item;
								if(S_OK == CreateSItem((*drop_iter).ItemNo, 1, GetGroundItemRarityNo(), Item))
								{
									CheckEnchantControl(Item);
									DropItems.push_back( Item );
								}
							}
						}
					}
					else if( ExceptionCase )
					{	// ���� ���̽���, �� �������� ��з��� ���.
						for(int i = 0 ; i < Element.RewardCount ; ++i)
						{
							int const Rate = BM::Rand_Range(100, 1);

							for(drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
							{
								if( Rate <= (*drop_iter).Rate )
								{
									for( int i = 0 ; i < (*drop_iter).Count ; ++i )
									{	// ������ ���� ��ŭ ������ ���ڷ� ���.
										PgBase_Item Item;
										if(S_OK == CreateSItem((*drop_iter).ItemNo, 1, GetGroundItemRarityNo(), Item))
										{
											CheckEnchantControl(Item);
											DropItems.push_back( Item );
										}
									}
								}
							}
						}
					}
					else
					{
						for(int i = 0 ; i < Element.RewardCount ; ++i)
						{
							int Rate = BM::Rand_Index(TotalRate);
							int CurRate = 0;

							for(drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
							{								
								CurRate += (*drop_iter).Rate;
								if( Rate < CurRate )
								{
									for( int i = 0 ; i < (*drop_iter).Count ; ++i )
									{	// ������ ���� ��ŭ ������ ���ڷ� ���.
										PgBase_Item Item;
										if(S_OK == CreateSItem((*drop_iter).ItemNo, 1, GetGroundItemRarityNo(), Item))
										{
											CheckEnchantControl(Item);
											DropItems.push_back( Item );
										}
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

void PgGround::RemoveEventMonster(int MonsterNo)
{
	CONT_GENERATED_MONSTER::iterator iter = m_ContGeneratedEventMonster.find(MonsterNo);
	if( iter == m_ContGeneratedEventMonster.end() )
	{
		return;
	}

	m_ContGeneratedEventMonster.erase(iter);

	CONT_EVENT_MONSTER_GEN::iterator gen_iter = m_ContEventMonsterGen.find(MonsterNo);
	if( gen_iter == m_ContEventMonsterGen.end() )
	{
		return;
	}

	CGameTime GameTime;
	(*gen_iter).second.GenTime = GameTime.GetLocalSecTime() + (*gen_iter).second.RegenPeriod;
}

void PgGround::GetEventMonsterGenInfo(int MonsterNo, SEventMonsterGen & OutInfo) const
{
	CONT_EVENT_MONSTER_GEN::const_iterator gen_iter = m_ContEventMonsterGen.find(MonsterNo);
	if( gen_iter == m_ContEventMonsterGen.end() )
	{
		return;
	}

	OutInfo = (*gen_iter).second;
}

bool PgGround::RareMonsterGenerate()
{
	if( PgGroundUtil::IsBSGround(GetAttr())
	&&	false == PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
	{
		return false; // ��Ʋ ������ ���������� �ʴٸ� ���� ���� ����
	}

	CGameTime kGameTime;
	__int64 const i64CurTime = kGameTime.GetLocalSecTime();

	const CONT_DEF_GROUND_RARE_MONSTER * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		return false;
	}

	CONT_DEF_GROUND_RARE_MONSTER::const_iterator iter = pkDef->find(GetGroundNo());
	if(iter == pkDef->end())
	{
		return false;
	}

	CONT_DEF_GROUND_RARE_MONSTER::mapped_type const & kMonTable = (*iter).second;

	GET_DEF(CMonsterDefMgr, kMonsterDefMgr);

	for(CONT_MONSTER_GEN::iterator iter = m_kContRareMonsterGen.begin();iter != m_kContRareMonsterGen.end();++iter)
	{
		SMonsterGen & kMonGen = (*iter).second;

		CONT_GENERATED_MONSTER::const_iterator geniter = m_kContGeneratedMonster.find(kMonGen.iMonNo);
		if(geniter != m_kContGeneratedMonster.end())
		{
			continue;
		}

		CONT_DEF_GROUND_RARE_MONSTER::mapped_type::const_iterator moniter = kMonTable.find(kMonGen.iMonNo);
		if(moniter == kMonTable.end())
		{
			continue;
		}

		if(kMonGen.i64GenTime > i64CurTime)
		{
			continue;
		}

		int iMonsterNo = 0;
		if(false == GetRareMonster(kMonGen.iMonNo,iMonsterNo))
		{
			continue;
		}

		PgGenPoint kGenPoint;
		if(false == GetRandomMonsterGenPoint((*moniter).second.iGenGroupNo,kGenPoint))
		{
			continue;
		}

		BM::GUID kGuid;
		HRESULT hRet = InsertMonster(kGenPoint.Info(),iMonsterNo,kGuid,NULL);
		if(S_OK != hRet)
		{
			continue;
		}

		CUnit * pkMon = GetUnit(kGuid);
		if(pkMon)
		{
			pkMon->SetAbil(AT_MONBAGNO,kMonGen.iMonNo);
		}

		m_kContGeneratedMonster.insert(kMonGen.iMonNo);
/*
		BM::Stream kPacket(PT_M_C_NFY_CHAT);
		kPacket.Push(static_cast<BYTE>(CT_RAREMONSTERGEN));
		kPacket.Push(kGuid);
		kPacket.Push(iMonsterNo);
		Broadcast(kPacket,NULL,E_SENDTYPE_SELF);
*/
	}

	return true;
}

bool PgGround::RemoveRareMonster(int const iMonNo,PgMonster * pkMonster)
{
	int const iMonBagNo = pkMonster->GetAbil(AT_MONBAGNO);

	CONT_GENERATED_MONSTER::iterator geniter = m_kContGeneratedMonster.find(iMonBagNo);
	if(geniter == m_kContGeneratedMonster.end())
	{
		return false;
	}

	const CONT_DEF_GROUND_RARE_MONSTER * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		return false;
	}

	CONT_DEF_GROUND_RARE_MONSTER::const_iterator defiter = pkDef->find(GetGroundNo());
	if(defiter == pkDef->end())
	{
		return false;
	}

	CONT_DEF_GROUND_RARE_MONSTER::mapped_type::const_iterator moniter = (*defiter).second.find((*geniter));
	if(moniter == (*defiter).second.end())
	{
		return false;
	}

	m_kContGeneratedMonster.erase(geniter);

	CONT_MONSTER_GEN::iterator rareiter = m_kContRareMonsterGen.find(iMonBagNo);
	if(rareiter == m_kContRareMonsterGen.end())
	{
		return false;
	}

	CGameTime kGameTime;
	(*rareiter).second.i64GenTime = kGameTime.GetLocalSecTime() + (*moniter).second.iRegenTime;
/*
	if(pkMonster)
	{
		BM::GUID kOwnerGuid;
		CUnit* pkOwner = NULL;
		if(true == CheckUnitOwner(pkMonster, kOwnerGuid) )//������ ���������� ���� ������ �������� Ȯ���Ѵ�
		{
			pkOwner = GetUnit( kOwnerGuid );
			if(pkOwner)
			{
				BM::Stream kPacket(PT_M_C_NFY_CHAT);
				kPacket.Push(static_cast<BYTE>(CT_RAREMONSTERDEAD));
				kPacket.Push(pkMonster->GetID());
				kPacket.Push(iMonNo);
				kPacket.Push(pkOwner->Name());
				Broadcast(kPacket,NULL,E_SENDTYPE_SELF);
			}
		}
	}
*/
	return true;
}

bool PgGround::GetRandomMonsterGenPoint(int const iGenGroup,PgGenPoint & kGenPoint)
{
	SGenGroupKey kGenGroupKey;
	GetGenGroupKey(kGenGroupKey);
	GET_DEF_CUSTOM( PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
	ContGenPoint_Monster kContGenPoint;
	if( SUCCEEDED(kGenPointMgr.GetGenPoint_Monster( kContGenPoint, kGenGroupKey, true, iGenGroup )) )
	{
		size_t const kGenCount = kContGenPoint.size();

		if(kGenCount <= 0)
		{
			return false;
		}

		ContGenPoint_Monster::const_iterator iter = kContGenPoint.begin();
		size_t kCount = BM::Rand_Index(kGenCount);

		while(kCount > 0)
		{
			--kCount;
			++iter;
		}

		kGenPoint = (*iter).second;
		return true;
	}
	return false;
}

HRESULT PgGround::InsertMonster(TBL_DEF_MAP_REGEN_POINT const &rkGenInfo, int const iMonNo, BM::GUID &rkOutGuid, CUnit* pkCaller, bool bDropAllItem, int iEnchantGradeNo, ECREATE_HP_TYPE const eCreate_HP_Type)
{
	// Monster Unit Creating
	BM::CAutoMutex Lock(m_kRscMutex);

	GET_DEF(CMonsterDefMgr, kMonsterDefMgr);
	const CMonsterDef *pkMonDef = kMonsterDefMgr.GetDef( iMonNo );
	if (pkMonDef == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"MonsterDef is NULL MonsterNo["<<iMonNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if(int const iMaxMonsterCount = MaxMonsterCount())
	{
		if(0 == pkMonDef->GetAbil(AT_IGNORE_MONSTER_REGEN_COUNT))
		{
			size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
			if(iMaxMonsterCount<=iLiveMonsterCount)
			{
				INFO_LOG(BM::LOG_LV0, __FL__<<L"Over Monster Count, MonsterNo["<<iMonNo<<L"]"<<", Max["<<iMaxMonsterCount<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
	}

	if(pkCaller)
	{
		if(int const iMaxSummonCount = pkCaller->GetAbil(AT_MAX_SUMMON_COUNT))
		{
			if(iMaxSummonCount <= pkCaller->GetSummonUnitCount())
			{
				//INFO_LOG(BM::LOG_LV0, __FL__<<L"Over Summon Monster Count, MonsterNo["<<iMonNo<<L"]"<<", Max["<<iMaxSummonCount<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
	}

	const wchar_t *pText = NULL;
	if(!GetDefString(pkMonDef->NameNo(), pText))
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't Get MonsterName MonsterNo["<<iMonNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if( 0 == iEnchantGradeNo )
	{
		CONT_DEFMAP_ABIL const* pkDefMapAbil = NULL;
		g_kTblDataMgr.GetContDef(pkDefMapAbil);
		CONT_DEFMAP_ABIL::const_iterator iter = pkDefMapAbil->find(GetGroundNo());
		if( pkDefMapAbil->end() != iter )
		{
			CONT_DEFMAP_ABIL::mapped_type const kDefMapAbil = (*iter).second;
			for( int iCur = 0; MAX_MAP_ABIL > iCur; ++iCur )
			{
				if( AT_MON_ENCHANT_PROB_NO == kDefMapAbil.iType[iCur] )
				{
					iEnchantGradeNo = PgAction_MonsterEnchantProbCalc(kDefMapAbil.iValue[iCur]).Get();
					break;
				}
			}
		}
		if( 0 == iEnchantGradeNo
		&&	pkMonDef->GetAbil(AT_MON_ENCHANT_PROB_NO) )
		{
			iEnchantGradeNo = PgAction_MonsterEnchantProbCalc(pkMonDef->GetAbil(AT_MON_ENCHANT_PROB_NO)).Get();
		}
	}

	SMonsterCreate kCreateInfo;

	kCreateInfo.kTable = rkGenInfo;
	if(0==kCreateInfo.kTable.iTunningNo)
	{
		kCreateInfo.kTable.iTunningNo = GroundTunningNo();
	}
	kCreateInfo.kGuid.Generate();
	kCreateInfo.iMonNo = iMonNo;
	kCreateInfo.wstrName = pText;
	kCreateInfo.iControlID = m_iMonsterControlID;
	kCreateInfo.iWeight = m_iGroundWeight;
	kCreateInfo.iTunningLevel = m_iTunningLevel;
	if(0==pkMonDef->GetAbil(AT_NO_ENCHNAT_MONSTER))
	{
		kCreateInfo.iEnchantGradeNo = (0 == pkMonDef->GetAbil(AT_MON_ENCHANT_GRADE_NO))? iEnchantGradeNo: pkMonDef->GetAbil(AT_MON_ENCHANT_GRADE_NO); // DefMonAbil�� �켱
	}

	EUnitGrade eMonGrade = (EUnitGrade)pkMonDef->GetAbil(AT_GRADE);
	
	PgMonster* pkMonster = dynamic_cast<PgMonster*>(g_kTotalObjMgr.CreateUnit((eMonGrade == EMGRADE_BOSS) ? UT_BOSSMONSTER : UT_MONSTER, kCreateInfo.kGuid));//��� �����̹Ƿ� ��� ����.
	if(!pkMonster)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"CreateUnit failed...UnitType["<<((eMonGrade == EMGRADE_BOSS) ? UT_BOSSMONSTER : UT_MONSTER)<<L"], Guid["<<kCreateInfo.kGuid<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	//xxx ������ ��~~;.
	POINT3 pt3Pos = kCreateInfo.kTable.pt3Pos;
	pt3Pos.z += 35;

	HRESULT const eRet = pkMonster->Create((LPBYTE)&kCreateInfo);//���� ����.

	if ( true == bDropAllItem )
	{
		pkMonster->SetAbil( AT_MON_DROP_ALLITEM, 1 );
	}

#ifdef AI_DEBUG
	// AI Test�� ���� AI ������ ���� �����Ѵ�.
	//SUnit_AI_Info* pkInfo = pkMonster->GetAI();
	//pkInfo->AddActionType(EAI_ACTION_CHASEENEMY, 1);
#endif

	// �ٴ����� �÷� ���´�.
	NxRay kRay(NxVec3(pt3Pos.x, pt3Pos.y, pt3Pos.z), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = RayCast(kRay, kHit, 200.0f);
	if(pkHitShape)
	{
		pt3Pos.z = kHit.worldImpact.z;
	}
	//INFO_LOG(BM::LOG_LV9, _T("[%s] InitialPos[%6.2f, %6.2f, %6.2f]"), __FUNCTIONW__, pt3Pos.x, pt3Pos.y, pt3Pos.z);
	pkMonster->SetPos(pt3Pos);
	pkMonster->LastAreaIndex(PgSmallArea::NONE_AREA_INDEX);

	// ���� ����
	if ( pkMonster->GetAbil( AT_MANUAL_OPENING ) > 0 )
	{
		pkMonster->SetAbil( AT_USENOT_SMALLAREA, 1 );
	}
	pkMonster->SetAbil(AT_MON_DEFAULT_ANGLE, kCreateInfo.kTable.cDirection);//���� ��������
	pkMonster->FrontDirection(static_cast<Direction>(pkMonDef->GetAbil(AT_MON_ANGLE_FIX)));//�ٶ󺸴� ��������

	if ( pkCaller )
	{
		pkMonster->Caller(pkCaller->GetID());

		if(pkCaller->GetAbil(AT_MON_CHILD_DIE))
		{
			pkCaller->AddSummonUnit(pkMonster->GetID(), 0, ESO_IGNORE_MAXCOUNT);
			pkMonster->SetAbil(AT_MON_CHILD_DIE, pkCaller->GetAbil(AT_MON_CHILD_DIE));
		}
	}

	int iCreateHP = 0;
	switch(eCreate_HP_Type)
	{
	case ECHT_ELGA_BONE_MON:
		{
			if(pkCaller)
			{
				if(int const iHPRate = pkCaller->GetAbil(AT_ELGA_BONE_MON_HP))
				{
					iCreateHP = pkMonster->GetAbil(AT_C_MAX_HP) * (iHPRate / ABILITY_RATE_VALUE_FLOAT);
					pkCaller->SetAbil(AT_ELGA_BONE_MON_HP, 0);
				}
			}
		}break;
	case ECHT_ELGA_EYE_MON:
		{
			if(pkCaller && pkCaller->GetAbil(AT_ELGA_EYE_MON_HP_TIME))
			{
				int const iMaxHp = pkMonster->GetAbil(AT_C_MAX_HP);
				int const iPassTime = BM::GetTime32() - pkCaller->GetAbil(AT_ELGA_EYE_MON_HP_TIME);
				int const iAutoHP = pkCaller->GetAbil(AT_ELGA_EYE_MON_AUTO_HP) * (iPassTime / 1000.f);
				iCreateHP = std::min<int>(iMaxHp, pkCaller->GetAbil(AT_ELGA_EYE_MON_HP)+iAutoHP);
				pkCaller->SetAbil(AT_ELGA_EYE_MON_HP_TIME, 0);
			}
		}break;
	}

	bool bNeedReSetAbilHp = false;
	if(iCreateHP)
	{
		iCreateHP = std::min<int>(iCreateHP, pkMonster->GetAbil(AT_C_MAX_HP));
		iCreateHP = std::max<int>(0, iCreateHP);
		if(0==iCreateHP)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"InsertMonster CreateHP is zero MonNo["<<pkMonster->GetAbil(AT_CLASS)<<L"]");
		}
		else
		{
			if(pkMonster->GetAbil(AT_C_MAX_HP) != iCreateHP)	//�￴�� ��쿡�� Now HP�ִ� ������Ʈ ������ �ȵǵ���
			{
				bNeedReSetAbilHp = true;
			}
		}
		pkMonster->SetAbil(AT_HP, iCreateHP);
	}

	if( GetAttr() == GATTR_MISSION && m_kGroundMutator.size() > 0 ) // if mission ground we can use mutator
	{
		CONT_DEF_MISSION_MUTATOR const * pkMutators = NULL;
		g_kTblDataMgr.GetContDef(pkMutators);
		if( pkMutators )
		{
			CONT_DEF_MISSION_MUTATOR_ABIL const * pkMutatorAbil = NULL;
			g_kTblDataMgr.GetContDef(pkMutatorAbil);
			if( pkMutatorAbil )
			{
				for(MUTATOR_SET::iterator it = m_kGroundMutator.begin(); it != m_kGroundMutator.end(); it++) // get all mutator from set
				{
					CONT_DEF_MISSION_MUTATOR::const_iterator kMutatorIter = pkMutators->find(*it);
					if(kMutatorIter == pkMutators->end())
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Not found MutatorNo[" << (*it) << "]");
						continue;
					}

					if(kMutatorIter->second.iMutatorUnitType != MISSION_MUTATOR_UNIT_MONSTER)
					{
						continue;
					}

					for( int i = 0; i < MAX_MUTATORABIL; i++ ) // setting up abil to monster
					{
						if( kMutatorIter->second.iAbil[i] == 0 )
							continue;
						CONT_DEF_MISSION_MUTATOR_ABIL::const_iterator kMutatorAbilIter = pkMutatorAbil->find(kMutatorIter->second.iAbil[i]);
						if( kMutatorAbilIter == pkMutatorAbil->end() )
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Not found MutatorAbilNo[" << kMutatorIter->second.iAbil[i] << "]");
							continue;
						}

						pkMonster->SetMutatorAbil(kMutatorAbilIter->second); // setting up abil to monster
					}
				}

				pkMonster->SetAbil(AT_LEVEL, m_iTunningLevel); // Mutation Monster Have same lv whit party lv
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"TB_DefMissionAbil_Mutator is empty! Can't add mutation to monster.");
			}
		}
	}

	bool bIsHydra = false;
	if(pkCaller)
	{
		bIsHydra = pkCaller->GetAbil(AT_MUTATOR_HYDRA_SELF) == EHU_YES;
		pkMonster->SetAbil(AT_MUTATOR_HYDRA_MONSTER, EHU_NO);
	}

	if(bIsHydra)
	{ // Setting Hydra Monster Abil
		pkMonster->SetAbil(AT_MUTATOR_HYDRA_MONSTER, EHU_YES);
		pkMonster->SetAbil(AT_MUTATOR_HYDRA_SELF, false); // delete hydra flag
		pkMonster->SetAbil(AT_HP, pkMonster->GetAbil(AT_HP) / 2); // main monster hp / 2
		pkMonster->SetAbil(AT_C_MAX_HP, pkMonster->GetAbil(AT_C_MAX_HP) / 2); // main monster hp / 2
#ifdef MUTATOR_HYDRA_DEBUG
		INFO_LOG(BM::LOG_LV0, __FL__ << L"Maked new hydra monster MonsterNo[" << iMonNo << L"]");
#endif
	}

	if( AddUnit(pkMonster) )
	{
		if(true==bNeedReSetAbilHp)
		{
			pkMonster->SetAbil(AT_HP, iCreateHP);
		}
		rkOutGuid = kCreateInfo.kGuid;

		if(pkMonster->GetAbil(AT_TALK_IDLE))// ��� ������ ��� �� ����� ���� �Ҵ�Ǿ� �ִ�.
		{
			BM::Stream kPacket(PT_M_C_NFY_CHAT);
			kPacket.Push(static_cast<BYTE>(CT_RAREMONSTERGEN));
			kPacket.Push(pkMonster->GetID());
			kPacket.Push(pkMonster->GetAbil(AT_CLASS));
			Broadcast(kPacket,NULL,E_SENDTYPE_SELF);
		}
		m_bUpdateMonsterCount = true;
		return eRet;
	}
	// ���������Ƿ� Unit �����־�� �Ѵ�.
	if (pkMonster != NULL)
	{
		g_kTotalObjMgr.ReleaseUnit(pkMonster);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgGround::InsertObjectUnit( TBL_DEF_MAP_REGEN_POINT const &rkGenInfo, SObjUnitBaseInfo const &kInfo, BM::GUID &rkOutGuid, CUnit* pkCaller )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	GET_DEF(PgObjectUnitDefMgr, kObjectUnitDef);
	PgObjectUnitDef const *pkObjectDef = kObjectUnitDef.GetDef( kInfo.iID );
	if (pkObjectDef == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"ObjectUnitDef is NULL ObejctNo["<<kInfo.iID<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	SObjUnitCreateInfo kCreateInfo(kInfo);
	kCreateInfo.dwAttribute = pkObjectDef->GetAbil(AT_ATTRIBUTE);
// 	if(!GetDefString( pkObjectDef->NameNo(), kCreateInfo.wstrName) )
// 	{
// 		INFO_LOG(BM::LOG_LV5, _T("[%s] Can't Get ObjectUnitName ObejctNo[%d]"), __FUNCTIONW__, kInfo.iID );
// 		return E_FAIL;
// 	}

	PgObjectUnit* pkObjectUnit = dynamic_cast<PgObjectUnit*>(g_kTotalObjMgr.CreateUnit(UT_OBJECT, kCreateInfo.kGuid));//��� �����̹Ƿ� ��� ����.
	if(!pkObjectUnit)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"CreateUnit failed...UnitType["<<UT_OBJECT<<L"], Guid["<<kCreateInfo.kGuid<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	POINT3 pt3Pos = kCreateInfo.pt3Pos;
	pt3Pos.z += 25;

	HRESULT eRet = pkObjectUnit->Create((LPBYTE)&kCreateInfo);//���� ����.

	// �ٴ����� �÷� ���´�.
	NxRay kRay(NxVec3(pt3Pos.x, pt3Pos.y, pt3Pos.z), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 90.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		pt3Pos.z = kHit.worldImpact.z;
	}
	pkObjectUnit->SetPos(pt3Pos);
	pkObjectUnit->LastAreaIndex(PgSmallArea::NONE_AREA_INDEX);
	pkObjectUnit->SetAbil(AT_MON_DEFAULT_ANGLE, rkGenInfo.cDirection);//���� ��������

	if ( pkCaller )
	{
		pkObjectUnit->Caller(pkCaller->GetID());
	}

	if(GKIND_WAR==GetKind())
	{
		PgWarGround *pkWarGnd = dynamic_cast<PgWarGround*>(this);
		if(pkWarGnd)
		{
			if(pkWarGnd->IsModeType( WAR_TYPE_DESTROYCORE ))
			{
				pkObjectUnit->SetAbil(AT_CANNOT_DAMAGE, 1);	//�ھ������� ������Ʈ�� ������ �� ������ ����, PgWarType_DestroyCore::InitUnitInfo���� ������
			}
		}
	}

	if( AddUnit(pkObjectUnit) )
	{
		rkOutGuid = kCreateInfo.kGuid;
		return eRet;
	}

	// ���������Ƿ� Unit �����־�� �Ѵ�.
	if ( pkObjectUnit != NULL )
	{
		g_kTotalObjMgr.ReleaseUnit(pkObjectUnit);
	}
	return E_FAIL;
}

#ifdef _DEBUG
//DWORD g_dwSendAreaDataTime = 0;
#endif

void PgGround::SendAddUnitAreaData( PgNetModule<> const &kNetModule, PgSmallArea const* pkTo, PgSmallArea const* pkFrom, BM::GUID const &kIgnoreCharGuid )
{
	// �ڽſ��� SmallArea�� ������ ������.
	UNIT_PTR_ARRAY kAddUnitArray;
	UNIT_PTR_ARRAY kDelUnitArray;

	PgSmallArea* pkArea = NULL;
	if ( pkFrom )
	{
		for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
		{
			pkArea = pkFrom->GetAdjacentArea(eDir);
			if ( pkArea )
			{
				if ( (pkTo == NULL) || !pkTo->IsAdjacentArea(pkArea))
				{
					pkArea->GetUnitList( kDelUnitArray, kIgnoreCharGuid, UT_NONETYPE, SYNC_TYPE_SEND_ADD );
				}
			}
		}
	}

	if ( pkTo )
	{
		for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
		{
			pkArea = pkTo->GetAdjacentArea(eDir);
			if ( pkArea )
			{
				if ( (pkFrom == NULL) || !pkFrom->IsAdjacentArea(pkArea) )
				{
					pkArea->GetUnitList(kAddUnitArray, kIgnoreCharGuid, UT_NONETYPE, SYNC_TYPE_SEND_ADD );
				}
			}
		}
	}

	PgGroundUtil::SendAddSimpleUnit(kNetModule, kAddUnitArray, false, true);

	if (kDelUnitArray.size() > 0)
	{
		BM::Stream kDPacket(PT_M_C_NFY_REMOVE_CHARACTER, (size_t)kDelUnitArray.size());
		UNIT_PTR_ARRAY::const_iterator itor = kDelUnitArray.begin();
		while ( itor != kDelUnitArray.end() )
		{
			kDPacket.Push(itor->pkUnit->GetID());
			++itor;
		}
		kNetModule.Send( kDPacket, false, true );
	}
}

void PgGround::SendAreaData( CUnit *pkUnit, PgSmallArea const* pkTo, PgSmallArea const* pkFrom, BYTE const bySyncType )
{
	assert(pkTo != pkFrom);
#ifdef AI_DEBUG
//	if (pkTo != NULL && pkFrom != NULL)
//	{
//		INFO_LOG(BM::LOG_LV9, _T("[%s] Unit[%s] From[%d]-->To[%d]"), __FUNCTIONW__, pkUnit->GetID().str().c_str(), pkFrom->Index(), pkTo->Index());
//	}
//	else if (pkTo == NULL)
//	{
//		INFO_LOG(BM::LOG_LV9, _T("[%s] Unit[%s] From[%d]-->To[NULL]"), __FUNCTIONW__, pkUnit->GetID().str().c_str(), pkFrom->Index());
//	}
//	else if (pkFrom == NULL)
//	{
//		INFO_LOG(BM::LOG_LV9, _T("[%s] Unit[%s] From[NULL]-->To[%d]"), __FUNCTIONW__, pkUnit->GetID().str().c_str(), pkTo->Index());
//	}
#endif
	POINT3 const ptUnitPos = pkUnit->GetPos();
	
	bool const bSendAddUnit = ( SYNC_TYPE_SEND_ADD & pkUnit->GetSyncType() & bySyncType );
	bool const bRecvAddUnit = ( SYNC_TYPE_RECV_ADD & pkUnit->GetSyncType() & bySyncType );

	EUnitType const eIgnoreType = static_cast<EUnitType>(UT_PET | UT_SUMMONED);
	BM::GUID const& kAddIgnoreGuid = ( pkUnit->IsInUnitType(eIgnoreType) ? pkUnit->Caller() : pkUnit->GetID() );
	BM::GUID const& kDelIgnoreGuid = ( pkUnit->IsInUnitType(UT_SUMMONED) ? pkUnit->Caller() : pkUnit->GetID() );
	if ( pkUnit->IsInUnitType(UT_PLAYER) )
	{
		// �ڽſ��� SmallArea�� ������ ������.
		UNIT_PTR_ARRAY kAddUnitArray;
		UNIT_PTR_ARRAY kDelUnitArray;
		
		// �ٸ� Player���� ���ο� Unit�� ����/������ �˸���.

		// Add Packet
		UNIT_PTR_ARRAY kAddUnitArray2;
		kAddUnitArray2.Add(pkUnit);
		
		BM::Stream kAPacket2(PT_M_C_ADD_UNIT);
		kAddUnitArray2.WriteToPacket( kAPacket2, WT_SIMPLE );

		// Del Packet
		BM::Stream kDPacket2(PT_M_C_NFY_REMOVE_CHARACTER, (size_t)1);
		kDPacket2.Push( pkUnit->GetID() );

		if (pkFrom != NULL)
		{
			for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
			{
				PgSmallArea* pkArea = pkFrom->GetAdjacentArea(eDir);
				if (pkArea != NULL)
				{
					if (pkTo == NULL || !pkTo->IsAdjacentArea(pkArea))
					{
						// Sending Delete_Unit_Packet
						if ( true == bRecvAddUnit )
						{
							pkArea->GetUnitList( kDelUnitArray, pkUnit, UT_NONETYPE, SYNC_TYPE_SEND_ADD );
						}
						
						if ( true == bSendAddUnit )
						{
							pkArea->Broadcast( &kDPacket2, pkUnit->GetID(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
						}
					}
				}
			}
		}

		if (pkTo != NULL)
		{
			for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
			{
				PgSmallArea* pkArea = pkTo->GetAdjacentArea(eDir);
				if (pkArea != NULL)
				{
					if (pkFrom == NULL || !pkFrom->IsAdjacentArea(pkArea))
					{
						// Sending Add_Unit_Packet
						if ( true == bRecvAddUnit )
						{
							pkArea->GetUnitList(kAddUnitArray, pkUnit, UT_NONETYPE, SYNC_TYPE_SEND_ADD );
						}

						if ( true == bSendAddUnit )
						{
							pkArea->Broadcast( &kAPacket2, kAddIgnoreGuid, SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
						}
					}
				}
			}
		}		

			/*
			while(area_itor != m_kAreaCont.end())
			{
				PgSmallArea* pkArea = (*area_itor).second;
				if((pkTo != NULL && pkTo->IsAdjacentArea(pkArea)) && (pkFrom == NULL || !pkFrom->IsAdjacentArea(pkArea)))
				{
					pkArea->GetUnitList(kAddUnitArray, pkUnit);
					pkArea->Broadcast(&kAPacket2, pkUnit->GetID());
				}
				else if((pkFrom != NULL && pkFrom->IsAdjacentArea(pkArea)) && (pkTo == NULL || !pkTo->IsAdjacentArea(pkArea)))
				{
					pkArea->GetUnitList(kDelUnitArray, pkUnit);
					pkArea->Broadcast(&kDPacket2, pkUnit->GetID());
				}
				++area_itor;
			}
			*/

		PgGroundUtil::SendAddSimpleUnit(pkUnit, kAddUnitArray, E_SENDTYPE_SELF);

		if (kDelUnitArray.size() > 0)
		{
			BM::Stream kDPacket(PT_M_C_NFY_REMOVE_CHARACTER, (size_t)kDelUnitArray.size());
			UNIT_PTR_ARRAY::const_iterator itor = kDelUnitArray.begin();
			while (itor != kDelUnitArray.end())
			{
				kDPacket.Push(itor->pkUnit->GetID());
				++itor;
			}
			pkUnit->Send(kDPacket,E_SENDTYPE_SELF);
		}
	}
	else
	// -----------------------------------------------------------------------------------------
	// �Ʒ� ��Ŷ ������ ����~~~~~~~~~~~~~~~~~~~~~~~~~~
	// -----------------------------------------------------------------------------------------
	{

#ifdef AI_DEBUG2
		INFO_LOG(BM::LOG_LV9, _T("SENDING REMOVE MONSTER ----"));
#endif

#ifdef _DEBUG
		//DWORD dwBegin = 0;
		//if (GroundKey().GroundNo() == 9013400)
		//{
		//	dwBegin = BM::GetTime32();
		//}
#endif
		if (pkFrom != NULL)
		{
			BM::Stream kDPacket(PT_M_C_NFY_REMOVE_CHARACTER, (size_t)1);
			kDPacket.Push(pkUnit->GetID());
			if ( pkFrom->IsBigArea() )
			{
				Broadcast( kDPacket, kDelIgnoreGuid );
			}
			else
			{
				for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
				{
					PgSmallArea* pkArea = pkFrom->GetAdjacentArea(eDir);
					if (pkArea != NULL)
					{
						if (pkTo == NULL || !pkTo->IsAdjacentArea(pkArea))
						{
							// Sending Delete_Unit_Packet
							pkArea->Broadcast( &kDPacket, kDelIgnoreGuid, SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
						}
					}
				}
			}	
		}

		// �ٸ� Player���� ���ο� Unit�� ����/������ �˸���.
		if (pkTo != NULL)
		{
			// SyncUnit �� �ƴ϶�� Add Packet�� �Ⱥ����� �Ѵ�.
			if ( true == bSendAddUnit )
			{
				UNIT_PTR_ARRAY kAddUnitArray;
				BM::Stream kAPacket(PT_M_C_ADD_UNIT);
				//PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				//if(pkPlayer)
				//{// �ֵ��� ĳ����
				//	int const iClass = pkPlayer->GetAbil(AT_CLASS);
				//	if( IsClass_OwnSubPlayer(iClass) )
				//	{//�̶��
				//		if( 0 == pkPlayer->GetAbil(AT_DEL_SUB_PLAYER) )
				//		{
				//			BM::GUID kSubPlayerGuid = pkPlayer->SubPlayerID();
				//			CUnit* pkSubPlayerUnit = GetUnit( kSubPlayerGuid );
				//			if(pkSubPlayerUnit)
				//			{
				//				kAddUnitArray.Add( pkSubPlayerUnit );
				//			}
				//		}
				//	}
				//}
				kAddUnitArray.Add(pkUnit);
				kAddUnitArray.WriteToPacket(kAPacket,WT_SIMPLE);

				if ( pkTo->IsBigArea() )
				{
					Broadcast( kAPacket, kAddIgnoreGuid );
				}
				else
				{
					for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
					{
						PgSmallArea* pkArea = pkTo->GetAdjacentArea(eDir);
						if (pkArea != NULL)
						{
							if (pkFrom == NULL || !pkFrom->IsAdjacentArea(pkArea))
							{
								// Sending Add_Unit_Packet
								pkArea->Broadcast( &kAPacket, kAddIgnoreGuid, SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
							}
						}
					}
				}
			}
		}

		/*
		ContArea::iterator area_itor = m_kAreaCont.begin();
		while(area_itor != m_kAreaCont.end())
		{
			PgSmallArea* pkArea = (*area_itor).second;
			if(bSyncUnit && (pkTo != NULL && pkTo->IsAdjacentArea(pkArea)) && (pkFrom == NULL || !pkFrom->IsAdjacentArea(pkArea)))
			{
#ifdef AI_DEBUG2
				//INFO_LOG(BM::LOG_LV9, _T("Sending PT_M_C_ADD_UNIT"));
#endif
				pkArea->Broadcast(&kAPacket, pkUnit->GetID());
			}
			else if((pkFrom != NULL && pkFrom->IsAdjacentArea(pkArea)) && (pkTo == NULL || !pkTo->IsAdjacentArea(pkArea)))
			{
				pkArea->Broadcast(&kDPacket, pkUnit->GetID());
#ifdef AI_DEBUG2
				INFO_LOG(BM::LOG_LV9, _T("[%s] SmallArea Index[%d]"), __FUNCTIONW__, pkArea->Index());
#endif
			}
			++area_itor;
		}
		*/
#ifdef _DEBUG
		//if (GroundKey().GroundNo() == 9013400)
		//{
		//	g_dwSendAreaDataTime += (BM::GetTime32() - dwBegin);
		//}
#endif
	}
}

//void PgGround::AddNPC(char const* pcName, char const* pcActor, char const* pcScript, char const* pcLocation, BM::GUID const &rkGuid, int iID)
//{
//	BM::CAutoMutex Lock(m_kRscMutex);
//
//	if(rkGuid == BM::GUID::NullData())
//	{
//		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] NPC IS NULL GUID MapNo[%d] Name = %s %s"), __FUNCTIONW__, GroundKey().GroundNo(), UNI(pcName), GroundKey().Guid().str().c_str());
//		return;
//	}
//
//	BM::vstring vStrNameNo(UNI(pcName));
//	int const iNameNo = (int)vStrNameNo;
//
//	const wchar_t *pText = NULL;
//	if(!GetDefString(iNameNo, pText))
//	{
//		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] Can't Get NpcName NameNo[%d]"), __FUNCTIONW__, iNameNo);
//		return;
//	}
//
//	POINT3 ptLoc;
//	::sscanf_s(pcLocation, "%f,%f,%f", &ptLoc.x, &ptLoc.y, &ptLoc.z);
//
//	INFO_LOG(BM::LOG_LV7, _T("[%s] MapNo = [%d] Name = [%s] GUID[%s] xyz(%.2f,%.2f,%.2f)"), __FUNCTIONW__, GroundKey().GroundNo(), UNI(pcName), GroundKey().Guid().str().c_str(), ptLoc.x, ptLoc.y, ptLoc.z);
//
//	//const BM::GUID kGuid = BM::GUID::Create();
//	PgNpc *pkNpc = dynamic_cast<PgNpc*>(g_kTotalObjMgr.CreateUnit(UT_NPC, rkGuid));
//
//	ptLoc.z += 50;	// �ٴڿ� ������ �ʵ��� ���� �÷�����
//	pkNpc->LastAreaPos(ptLoc);	// �� �κ��� ���� ��� AdjustArea(..)���� ���� �޽��� ��µȴ�.....(�ʿ�~~~!!!!!!!!!)
//
//	if(NULL != pkNpc)
//	{
//		pkNpc->Create(rkGuid, pText, UNI(pcActor), UNI(pcScript), ptLoc, iID);
//
//		//
//		g_kQuestMan.BuildNpc(pkNpc);
//
//		//
//		m_kNpcCont.insert(std::make_pair(rkGuid, pkNpc));
//	}
//	else
//	{
//		VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Critical Error: NPC is NULL(can't create npc)"), __FUNCTIONW__, __LINE__);
//	}
//}


// Player�� ��ų�� ����Ͽ��� : Attack Skill �ƴϾ �Լ��� ȣ��ȴ�. (��罺ų)
// [PARAMETER]
//	pkCaster : Skill caster
//	pkTarget : Skill Target unit
//	rkAction : Action information
//	piEffect : [OUT] result Effect number
// [RETURN]
//	success ??
bool PgGround::FireSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, const SActionInfo &rkAction, PgActionResultVector* pkActionResultVec)
{
	BM::CAutoMutex Lock(m_kRscMutex);

//	INFO_LOG(BM::LOG_LV8, _T("ReqDamage....ActionID[%d], InstanceID[%d]"), rkAction.iActionID, rkAction.iActionInstanceID);
	if(pkCaster == NULL)
	{
		FireSkillFailed(pkCaster, rkTargetArray, rkAction, pkActionResultVec);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( 0 != pkCaster->GetAbil(AT_CANNOT_ATTACK) )
	{
		//INFO_LOG(BM::LOG_LV5, _T("WARNING...[%s] Caster->GetAbil(AT_CANNOT_ATTACK)=[%d], Cannot Attack!!!!"), __FUNCTIONW__, pkCaster->GetAbil(AT_CANNOT_ATTACK));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iThisMapNo = GetGroundNo();
	bool bSuccess = false;

	// �����ִٰ� �°ų�/������ Hidden �Ӽ��� �����
	// �� AT_LOCK_HIDDEN_ATTACK �� ������ Ǯ���� �ʴ´�.
	int iHiddenAbil = pkCaster->GetAbil(AT_UNIT_HIDDEN);
	if (iHiddenAbil > 0 && pkCaster->GetAbil(AT_LOCK_HIDDEN_ATTACK) <= 0)
	{
		pkCaster->SetAbil(AT_UNIT_HIDDEN, iHiddenAbil-1);
	}

	bool bClientCtrl = true;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(rkAction.iActionID);
	if (rkAction.byActionParam == ESS_TARGETLISTMODIFY || !pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE))
	{
		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, this);
		kArg.Set(ACTARG_ACTIONINSTANCEID, rkAction.iActionInstanceID);	// Client���� AddEffect ��Ŷ ������ �ʿ��ϴ�.
		kArg.Set(ACTARG_CUSTOM_UNIT1, pkCaster->GetID());
		kArg.Set(ACTARG_CUSTOM_UNIT1_TYPE, pkCaster->UnitType());
		kArg.Set(ACTARG_ACTION_TIMESTAMP, rkAction.dwTimeStamp); // �޺� ī���� ����ϴµ� �ʿ��ϴ�

		int iUseChoasDebuff = pkCaster->GetAbil(AT_PHY_DMG_PER) + pkCaster->GetAbil(AT_MAGIC_DMG_PER);
		kArg.Set(ACTARG_EFFECT_CASTER_USE_CHAOS_DEBUFF, iUseChoasDebuff); // ī���� �ʿ� �ִ��� ���θ� ��ų ���ڷ� �����Ͽ� ��ų �� ����Ʈ���� ������ �Ѵ�.
		kArg.Set(ACTARG_EFFECT_CASTER_UNIT_TYPE, pkCaster->UnitType());	  // ĳ���� Ÿ���� �����Ѵ�
		{
			CheckTargetList(rkTargetArray);

			int iLuaRet = g_kSkillAbilHandleMgr.SkillFire(pkCaster, rkAction.iActionID, &kArg, &rkTargetArray, pkActionResultVec);
			if (iLuaRet < 0)
			{
				int iBasicSkill = kSkillDefMgr.GetCallSkillNum(rkAction.iActionID);
				INFO_LOG(BM::LOG_LV0, __FL__<<L"Skill_Fire["<<iBasicSkill<<L"] returnd ["<<iLuaRet<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			OnAttacked(pkCaster, &rkTargetArray, rkAction.iActionID, pkActionResultVec, &kArg, rkAction.dwTimeStamp);
		}
		
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		UNIT_PTR_ARRAY::const_iterator itor = rkTargetArray.begin();
		while(itor != rkTargetArray.end())
		{
			if(CUnit* pkTarget = itor->pkUnit)
			{
				::CheckSkillFilter_Delete_Effect(pkCaster, pkTarget, rkAction.iActionID);

				if(PgActionResult* pkResult = pkActionResultVec->GetResult(pkTarget->GetID()))
				{
					if( this->IsDecEquipDuration())
					{//������ ���.
						if(0 < pkResult->GetValue())
						{// Ÿ���ÿ� �������� �������� �������� �پ���� �Ѵ�.
							{
								PgAction_DecEquipDuration kDecDurAction(GroundKey(), true, pkResult->GetValue());//ī��Ʈ�� �߿��Ѱ̴ϴ�.
								kDecDurAction.DoAction(pkCaster, pkTarget);
							}

							{
								PgAction_DecEquipDuration kDecDurAction(GroundKey(), false, pkResult->GetValue());
								kDecDurAction.DoAction(pkTarget, pkCaster);
							}
						}
					}

					int const iDmgEffect = pkResult->GetEffect(0);
					if(pkTarget->IsAlive() && 0<iDmgEffect && pkTarget->IsDamageAction())
					{
						CEffectDef* pkDmgEffect = (CEffectDef*) kEffectDefMgr.GetDef(iDmgEffect);
						if(pkDmgEffect && 0 < pkDmgEffect->GetAbil(AT_BLOW_VALUE) )	// AT_BLOW_VALUE���� ����̸� DamageDelay�� �ִ� �����̴�.
						{
							pkTarget->SetBlowAttacker(pkCaster->GetID());
							pkTarget->SetAbil(AT_DAMAGE_EFFECT_DURATION, pkDmgEffect->GetDurationTime());//������ �׼� �ð� ����
							pkTarget->SetDelay(__max(pkDmgEffect->GetDurationTime(), 3000));
						}
#ifdef _DEBUG
						else
						{
							INFO_LOG(BM::LOG_LV8, __FL__<<L"ActionID ["<<rkAction.iActionID<<L"], DmgEffect["<<iDmgEffect<<L"]");
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetDef Failed!"));
						}
#endif
					}
				}
			}

			++itor;
		}
	}

	if(rkAction.byActionParam == ESS_FIRE && NULL != pkSkillDef)
	{
		// ����� ������ ������ ä��� ��
		::CalcAwakeValue(pkCaster, pkSkillDef);
	}
	return true;
}

void PgGround::FireSkillFailed(CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, const SActionInfo &rkAction, PgActionResultVector* pkActionResultVec)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] ActionID[%d]"), __FUNCTIONW__, rkAction.iActionID);

	SActArg kArg;
	g_kSkillAbilHandleMgr.SkillFail(pkCaster, rkAction.iActionID, &kArg, &rkTargetArray, pkActionResultVec);
}

void PgGround::SendAllQuestInfo(PgPlayer* pkPlayer)//�׶��� ��ü NPC�� Quest ������ ������.
{
	BM::CAutoMutex Lock(m_kRscMutex);

	int iCount = 0;
	//CONT_OBJECT_MGR_UNIT::iterator kItor;
	CONT_OBJECT_MGR_UNIT::iterator npc_iter = m_kNpcCont.begin();
	BM::Stream kTempPacket;
	while(m_kNpcCont.end() != npc_iter)
	{
		const CONT_OBJECT_MGR_UNIT::mapped_type pkElement = (*npc_iter).second;

		ContSimpleQuest kStateVec;
		PgCheckQuest kCheck(kStateVec);
		if( kCheck.DoAction(pkPlayer, pkElement) )
		{
			kTempPacket.Push(pkElement->GetID());
			kTempPacket.Push(kStateVec);

			++iCount;
		}

		++npc_iter;
	}

	BM::Stream kQuestInfo(PT_M_C_ANS_NPC_QUEST, iCount);
	if( 0 != iCount )
	{
		kQuestInfo.Push(kTempPacket);
	}
	pkPlayer->Send(kQuestInfo);
}

void PgGround::RecvReqTrigger(CUnit* pkUnit, int const iType, BM::Stream *pkNfy)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	BM::GUID kObjectGuid;
	int iActionType = 0;
	switch(iType)
	{
	case QOT_NPC:
		{
			pkNfy->Pop(kObjectGuid);
			pkNfy->Pop(iActionType);

			if( m_kQuestTalkSession.IsEmptyTalk(pkUnit->GetID()) )
			{
				ReqNpcTrigger(pkUnit, kObjectGuid, iActionType);
			}
		}break;
	case QOT_Monster:
		{
			pkNfy->Pop(kObjectGuid);
			pkNfy->Pop(iActionType);

			ReqMonsterTrigger(pkUnit, kObjectGuid, iActionType);
		}break;
	case QOT_ShineStone:
		{
			pkNfy->Pop(kObjectGuid);
			m_kStoneCtrl.PlayerStoneUpdate(pkUnit,GroundKey(), kObjectGuid);
		}break;
	case QOT_Trigger://��� �̺�Ʈ Ʈ����
		{
			//
		}break;
	case QOT_Location://����Ʈ ��� Ʈ���Ÿ� �Ѵ�.
		{
			int iTriggerNo = 0;
			pkNfy->Pop(iTriggerNo);

			SQuestTriggerInfo kTriggerInfo(GetGroundNo(), iTriggerNo);
			int iQuestID = 0;
			if( g_kQuestMan.GetTriggerToQuest(kTriggerInfo, iQuestID) )
			{
				PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUnit);
				if( !pkPC )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
					break;
				}

				PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
				if( !pkMyQuest )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkMyQuest is NULL"));
					break;
				}

				bool const bIngRet = pkMyQuest->IsIngQuest(iQuestID);
				if( !bIngRet )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bIngRet is NULL"));
					break;
				}

				PgQuestInfo const* pkQuestInfo = NULL;
				if( !g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetQuest Failed!"));
					break;
				}

				const ContQuestLocation &rkDependLocation = pkQuestInfo->m_kDepend_Location;
				ContQuestLocation::const_iterator location_iter = rkDependLocation.begin();
				while(rkDependLocation.end() != location_iter)
				{
					if( (*location_iter).iGroundNo == kTriggerInfo.iGroundNo
					&&	(*location_iter).iLocationNo == kTriggerInfo.iTriggerNo )
					{
						PgAction_IncQuestParam kActionQuest(GroundKey(), iQuestID, (*location_iter).iObjectNo, 1);
						kActionQuest.DoAction(pkUnit, NULL);
					}
					++location_iter;
				}
			}
			
		}break;
	case QOT_Dialog:
		{
			int iQuestID = 0;
			int iDialogID = 0;
			POINT3 kSyncPos;
			pkNfy->Pop(kObjectGuid);
			pkNfy->Pop(iQuestID);
			pkNfy->Pop(iDialogID);
			pkNfy->Pop(kSyncPos);

			PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkUnit);
			if( !pkPC )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
				break;
			}

			PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
			if( !pkMyQuest )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkMyQuest is NULL"));
				break;
			}

			SActionInfo kAction;
			kAction.iActionID = pkPC->GetPlayerActionInfo()->iActionID;
			kAction.ptPos = kSyncPos;
			kAction.dwTimeStamp = g_kEventView.GetServerElapsedTime();
			CheckHackingAction(pkPC, kAction);

			CUnit* pkNPC = GetNPC(kObjectGuid);
			if( !pkNPC )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkNPC is NULL"));
				break;
			}
			
			if( !PgGroundUtil::IsCanTalkableRange(pkPC, kSyncPos, kObjectGuid, pkNPC, GetGroundNo(), NMT_Quest, __FUNCTIONW__, __LINE__) )
			{
				break;
			}

			EQuestState eState = QS_None;
			SUserQuestState const *pkUserState = pkMyQuest->Get(iQuestID);
			if( pkUserState )
			{
				eState = (EQuestState)pkUserState->byQuestState;
			}
			else
			{
				eState = QS_Begin;
			}

			int const iEventNo = g_kQuestMan.GetNPCEventNo(kObjectGuid, iQuestID, eState);
			if( !iEventNo )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("iEventNo is 0"));
				break;
			}

			//
			CONT_QUEST_EFFECT kContAddEffect;
			CONT_PLAYER_MODIFY_ORDER kOrder;
			
			PgAction_QuestDialogEvent kActionQuest(GroundKey(), kObjectGuid, iQuestID, iEventNo, iDialogID, kOrder, kContAddEffect);
			if( !kActionQuest.DoAction(pkUnit, pkNPC) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ActionQuest DoAction Failed!"));
				break;
			}

			int const iFailedDialogID = kActionQuest.ItemResultFailDialogID();

			if( kOrder.empty() )
			{
				PgAction_QuestDialogEvent::NfyAddEffectToMap(pkUnit, kContAddEffect);
				SendShowQuestDialog(pkUnit, kObjectGuid, QSDT_NormalDialog, iQuestID, iDialogID);
			}
			else
			{
				BM::Stream kAddonPacket(PT_M_I_REQ_QUEST_EVENT);
				kAddonPacket.Push( kObjectGuid );
				kAddonPacket.Push( iQuestID );
				kAddonPacket.Push( iDialogID );
				kAddonPacket.Push( iFailedDialogID );
				kAddonPacket.Push( kContAddEffect );

				PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST_DIALOG, GroundKey(), kOrder);
				kItemModifyAction.DoAction(pkUnit, NULL);
			}
		}break;
	case QOT_Trap:
		{
			int iTrapSkillNo = 0;
			pkNfy->Pop(kObjectGuid);
			pkNfy->Pop(iTrapSkillNo);

			// Trap�� �¾Ƽ� Damage�� �־�� �Ѵ�.
			PgAction_ReqTrapDamage kAction(GroundKey(), this, iTrapSkillNo);
			kAction.DoAction(NULL, pkUnit);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown Trigger Type["<<iType<<L"], ObjectGuid["<<kObjectGuid<<L"], ActionType["<<iActionType<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}

	

}

void PgGround::ReqIntroQuest(CUnit *pkUnit, int const iQuestID, BM::GUID const& rkObjectGuid)
{
	if( !pkUnit )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"CUnit *pkUnit is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkUnit);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"'t Player Unit, GUID: "<<pkUnit->GetID()<<L", Name: "<<pkUnit->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
		return;
	}

	PgQuestInfo const *pkQuestInfo = NULL;
	g_kQuestMan.GetQuest(iQuestID, pkQuestInfo);
	if( !pkQuestInfo )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID["<<iQuestID<<L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkQuestInfo is NULL"));
		return;
	}

	PgNpc *pkNpc = NULL;
	if( !pkQuestInfo->IsCanRemoteComplete() )
	{
		pkNpc = dynamic_cast<PgNpc*>(GetUnit(rkObjectGuid));
		if( !pkNpc )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't NPC Unit, GUID: "<<rkObjectGuid<<L"");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkNpc is NULL"));
			return;
		}
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkUnit->GetID()<<L", Name: "<<pkUnit->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkMyQuest is NULL"));
		return;
	}

	SUserQuestState const *pkState = pkMyQuest->Get(iQuestID);
	EQuestState eState = QS_None;
	if( pkState )//�� ����Ʈ�� ������ ������, �Ϸ� ���߿� �ϳ���
	{
		eState = static_cast< EQuestState >(pkState->byQuestState);
	}
	else//�����鼭 ���� �� �� ������ ���۽�Ų��.
	{
		eState = QS_Begin;

		bool bCantBegin = false;

		switch( pkQuestInfo->Type() )
		{
		case QT_Scenario:
		case QT_Soul:
			{
				bCantBegin = MAX_ING_SCENARIO_QUEST == pkMyQuest->GetIngScenarioQuestNum();
			}break;
		default:
			{
				bCantBegin = MAX_ING_QUESTNUM == (pkMyQuest->GetIngQuestNum() - pkMyQuest->GetIngScenarioQuestNum());
			}break;
		}

		if( g_kLocal.ServiceRegion() == LOCAL_MGR::NC_USA
		&&	bCantBegin )
		{
			SendShowQuestDialog(pkUnit, rkObjectGuid, QSDT_ErrorDialog, iQuestID, QRDID_MaxQuestSlot);
			return;
		}
	}

	int const iNpcEventNo = (pkQuestInfo->IsCanRemoteComplete())? pkQuestInfo->FindNpcEventNo(rkObjectGuid, eState): g_kQuestMan.GetNPCEventNo(rkObjectGuid, iQuestID, eState);
	if( !iNpcEventNo )
	{
		std::wstring const kNpcName( ((pkNpc)? pkNpc->Name(): std::wstring()) );
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"NPC[Guid: "<<rkObjectGuid<<L", Name: "<<kNpcName<<L"] is not dependent quest[ID: "<<iQuestID<<L", State: "<<eState<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("iNpcEventNo is 0"));
		return;
	}

	switch( pkQuestInfo->Type() )
	{
	case QT_SweetHeart:
	case QT_Couple:
		{
			if( QS_Begin == eState )
			{
				if( PgGroundQuestUtil::IsHaveCoupleQuest(pkMyQuest) )
				{
					SendShowQuestDialog(pkUnit, rkObjectGuid, QSDT_ErrorDialog, iQuestID, QRDID_OnlyOneCoupleQuest);
					return;
				}
			}
		}break;
	}

	int const iDialogID = pkQuestInfo->GetNpcDialog(iNpcEventNo, eState);

	CONT_QUEST_EFFECT kContAddEffect;
	CONT_PLAYER_MODIFY_ORDER kOrder;
	PgAction_QuestDialogEvent kActionQuest(GroundKey(), rkObjectGuid, iQuestID, iNpcEventNo, iDialogID, kOrder, kContAddEffect,this);
	if( !kActionQuest.DoAction(pkUnit, pkNpc) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ActionQuest DoAction Failed!"));
		return;
	}

	int const iFailedDialogID = kActionQuest.ItemResultFailDialogID();

	if( kOrder.empty() )
	{
		PgAction_QuestDialogEvent::NfyAddEffectToMap(pkUnit, kContAddEffect);
		SendShowQuestDialog(pkUnit, rkObjectGuid, QSDT_NormalDialog, iQuestID, iDialogID);
	}
	else
	{
		BM::Stream kAddonPacket(PT_M_I_REQ_QUEST_EVENT);
		kAddonPacket.Push( rkObjectGuid );
		kAddonPacket.Push( iQuestID );
		kAddonPacket.Push( iDialogID );
		kAddonPacket.Push( iFailedDialogID );

		PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST_DIALOG, GroundKey(), kOrder, kAddonPacket);
		kItemModifyAction.DoAction(pkUnit, NULL);
	}
}

void PgGround::ReqRemoteCompleteQuest(CUnit *pkUnit, int const iQuestID)
{
	PgQuestInfo const* pkQuestInfo = NULL;
	if( !g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
	{
		return;
	}

	PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
	if( !pkPlayer )
	{
		return;
	}

	PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
	if( !pkMyQuest )
	{
		return;
	}

	int const iOnlyHaveQuest = 401433;
	int const iNotStateQuest = 401434;
	int const iNotInMapCouple = 401435;
	int const iNoHaveQuestCouple = 401436;
	int const iNoStateCouple = 401437;
	int const iCantCompleteInGround = 19936;

	SUserQuestState const* pkState = pkMyQuest->Get(iQuestID);
	if( !pkState )
	{
		pkUnit->SendWarnMessage(iOnlyHaveQuest); // ������ ���� ���� ����Ʈ �Դϴ�.
		return;
	}

	if( QS_End != pkState->byQuestState
	||	!pkQuestInfo->IsCanRemoteComplete() )
	{
		pkUnit->SendWarnMessage(iNotStateQuest); // �Ϸ� �� �� ���� ������ ����Ʈ �Դϴ�.
		return;
	}

	//���� ����Ʈ ���� ������� ���� ���� ���� : �ʼӼ��� ������� ���� ���� ����(2012.06.27 ������)
	//// �׶��� �˻�
	//switch( pkQuestInfo->Type() )
	//{
	//case QT_Couple:
	//case QT_SweetHeart:
	//case QT_RandomTactics:
	//case QT_Random:
	//case QT_Wanted:
	//	{
	//		if( 0 != (GetAttr() & GATTR_FLAG_MISSION) ) // �̼ǿ��� �Ϸ� �ȵ�
	//		{
	//			return;
	//		}
	//	}break;
	//default:
	//	{
	//		// �׿� ����Ʈ�� ����/���ʵ�/����Ȩ������ ����
	//		switch( GetAttr() )
	//		{
	//		case GATTR_DEFAULT:
	//		case GATTR_VILLAGE:
	//		case GATTR_MYHOME:
	//		case GATTR_HIDDEN_F:
	//		case GATTR_CHAOS_F:
	//		case GATTR_HOMETOWN:
	//		case GATTR_EXPEDITION_LOBBY:
	//			{
	//				// ���⼱ ����
	//			}break;
	//		default:
	//			{
	//				pkUnit->SendWarnMessage(iCantCompleteInGround);
	//				return;
	//			}break;
	//		}
	//	}break;
	//}


	// ���� �Ϸ� �õ�
	switch( pkQuestInfo->Type() )
	{
	case QT_Couple:
	case QT_SweetHeart:
		{
			CONT_PLAYER_MODIFY_ORDER kMyOrder, kCoupleOrder;

			int iTTW = iNotStateQuest; // �Ϸ� �� �� ���� ������ ����Ʈ �Դϴ�.
			bool bCanComplete = PgGroundQuestUtil::MakeQuestReward(kMyOrder, pkQuestInfo, pkPlayer);
			if( bCanComplete )
			{
				bCanComplete = false;
				iTTW = iNotInMapCouple; // Ŀ��/�κ� ����Ʈ�� �Ϸ� �Ϸ��� ������� ���� �׶��峻�� �־�� �մϴ�.
				PgPlayer* pkCouple = dynamic_cast< PgPlayer* >(GetUnit(pkPlayer->CoupleGuid()));
				if( pkCouple )
				{
					PgMyQuest const* pkCoupleQuest = pkCouple->GetMyQuest();
					if( pkCoupleQuest )
					{
						iTTW = iNoHaveQuestCouple; // Ŀ�� �Ǵ� �κΰ� �ش� ����Ʈ�� ������ ���� �ʽ��ϴ�.
						SUserQuestState const* pkCoupleState = pkCoupleQuest->Get(iQuestID);
						if( NULL != pkCoupleState )
						{
							iTTW = iNoStateCouple; // Ŀ�� �Ǵ� �κΰ� �ش� ����Ʈ�� �Ϸ��� ������ ���� �ʽ��ϴ�.
							if( QS_End == pkCoupleState->byQuestState )
							{
								bCanComplete = PgGroundQuestUtil::MakeQuestReward(kCoupleOrder, pkQuestInfo, pkCouple);
							}
						}

						if( !bCanComplete ) // �� Ŀ���� �̹� ����Ʈ�� �Ϸ� ������ ���� �Ϸ� ���� �ϴ�
						{
							iTTW = iNoStateCouple;
							bCanComplete = pkCoupleQuest->IsEndedQuest(iQuestID);
						}
					}
				}
			}
			if( bCanComplete )
			{
				iTTW = iNoStateCouple;// Ŀ�� �Ǵ� �κΰ� �ش� ����Ʈ�� �Ϸ��� ������ ���� �ʽ��ϴ�.
				CUnit* pkCouple = GetUnit(pkPlayer->CoupleGuid());
				if( pkCouple
				&&	!kMyOrder.empty() )
				{
					{
						PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST_REMOTE_COMPLETE, GroundKey(), kMyOrder);
						kItemModifyAction.DoAction(pkUnit, NULL);
					}
					if( !kCoupleOrder.empty() )
					{
						PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST_REMOTE_COMPLETE, GroundKey(), kCoupleOrder);
						kItemModifyAction.DoAction(pkCouple, NULL);
					}
				}
				else
				{
					bCanComplete = false;
				}
			}

			// Last
			if( !bCanComplete )
			{
				if( iTTW )
				{
					pkUnit->SendWarnMessage( iTTW );
				}
			}
		}break;
	case QT_Random:
	case QT_RandomTactics:
	case QT_Day:
	case QT_Wanted:
	case QT_BattlePass:
		{
			BM::GUID kObjectGuid;
			if( pkQuestInfo->GetFirstPayerNpc(kObjectGuid) )
			{
				PgAction_AnsQuestDialog kActionQuest(GroundKey(), kObjectGuid, iQuestID, QRDID_CanComplete, QRDID_COMPLETE_Start, 0, 0);
				kActionQuest.DoAction(pkUnit, NULL);
			}
		}break;
	default:
		{
			BM::GUID kObjectGuid;
			if( pkQuestInfo->GetFirstPayerNpc(kObjectGuid) )
			{
				ReqIntroQuest(pkUnit, iQuestID, kObjectGuid);
			}
		}break;
	}
}

bool PgGround::FindEnemy( CUnit* pkFrom, UNIT_PTR_ARRAY& rkUnitArray, int const iMaxTarget, int const iUnitType, bool const bFindFromGround, bool const bCallTimeCheck )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( (0 != pkFrom->GetAbil(AT_CANNOT_ATTACK)) || !pkFrom->GetSkill()->CheckCoolTime(SKILL_NO_GLOBAL_COOLTIME, 0))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	DWORD kDelta = BM::GetTime32() - pkFrom->GetAI()->GetFunctionCallTime(EAI_Func_FindEnemy);
	if (true==bCallTimeCheck && kDelta < 500)
	{
		//INFO_LOG(BM::LOG_LV6, _T("[%s] Function Cache"), __FUNCTIONW__);
		BM::Stream& rkResult = pkFrom->GetAI()->GetFunctionCallResult(EAI_Func_FindEnemy);
		bool bResult = false;
		rkResult.Pop(bResult);
		return bResult;
	}

	POINT3 const FromPos = GetUnitPos(pkFrom);
	int const iRange = pkFrom->GetDetectRange();
	EUnitType const eUnitType = pkFrom->UnitType();
	EUnitType eCheckUnitType = UT_NONETYPE;
	switch(eUnitType)
	{
	case UT_ENTITY:
		{
			if ( iRange < 0 )
			{
				PgEntity *pkEntity = dynamic_cast<PgEntity*>(pkFrom); 
				if ( pkEntity )
				{
					if( UT_PLAYER & iUnitType )
					{
						GetUnitInCube( pkEntity->VisualFieldMin(), pkEntity->VisualFieldMax(), GetUnitPos(pkFrom), UT_PLAYER, rkUnitArray );
					}

					if( UT_MONSTER & iUnitType )
					{
						GetUnitInCube( pkEntity->VisualFieldMin(), pkEntity->VisualFieldMax(), GetUnitPos(pkFrom), UT_MONSTER, rkUnitArray );
					}
				}
				break;
			}
		}//break; No break;
	case UT_PLAYER:
		{
			GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_PLAYER, rkUnitArray, static_cast<int>(AI_Z_LIMIT));
			GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_MONSTER, rkUnitArray, static_cast<int>(AI_Z_LIMIT));
		}break;
	case UT_SUMMONED:
		{
			if( false==IsInBattleZone(pkFrom) )
			{
				break;
			}
		}//No break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		{
			int iZLimit = pkFrom->GetAbil(AT_DETECT_RANGE_Z);//�÷��̾�� ��ƼƼ�� ����� ���� ����
			if(0==iZLimit)
			{
				iZLimit = static_cast<int>(AI_Z_LIMIT);
			}
			
			EUnitType eFirstUnitType = static_cast<EUnitType>(pkFrom->GetAbil(AT_MONSTER_FIRST_TARGET));
			if( (UT_MYHOME < eFirstUnitType) || (UT_NONETYPE == eFirstUnitType) )
			{
				eFirstUnitType = UT_PLAYER;
			}

			CUnit* InvokedUnit = IsUnitInRangeImp(GetReservedUnitByEffect(pkFrom), FromPos, iRange, eFirstUnitType, iZLimit, bFindFromGround);
			if(NULL!=InvokedUnit)
			{
				rkUnitArray.Add(InvokedUnit);
			}
			else
			{
				GetUnitInRange(eCheckUnitType, FromPos, iRange, eFirstUnitType, rkUnitArray, iZLimit, bFindFromGround);
				if(UT_SUMMONED==eUnitType)
				{
					if(GATTR_FLAG_PVP_ABLE & GetAttr())
					{
						GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_SUMMONED, rkUnitArray, iZLimit, bFindFromGround);
						GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_PLAYER, rkUnitArray, iZLimit, bFindFromGround);
					}
					
					GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_MONSTER, rkUnitArray, iZLimit, bFindFromGround);
				}
				else
				{
					GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_SUMMONED, rkUnitArray, iZLimit, bFindFromGround);
					GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_PLAYER, rkUnitArray, iZLimit, bFindFromGround);
				}
			}
			
			if(!rkUnitArray.empty())
			{
				UNIT_PTR_ARRAY::iterator it = rkUnitArray.begin();

				while(it!=rkUnitArray.end())
				{
					CUnit const* pkTarget = (*it).pkUnit;
					if(pkTarget)
					{
						if(0>=pkTarget->GetAbil(AT_UNIT_HIDDEN))
						{
							PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkFrom);

							if(pkMonster && pkMonster->CheckIgnoreTarget(pkTarget, BM::GetTime32()))
							{
								break;	//�ѳ��̶� ����� �� ���� �ִٸ�
							}
						}
						else
						{
							it = rkUnitArray.erase(it);
							continue;
						}
					}
					++it;


					if(rkUnitArray.empty())
					{
						if(UT_SUMMONED!=eUnitType)
						{
							pkFrom->SetState(US_RETREAT);
							if(true==g_kPatternMng.IsPatternExist(pkFrom->GetAbil(AT_AI_TYPE), EAI_ACTION_PATROL))
							{
								pkFrom->GetAI()->SetEvent(pkFrom->GetTarget(), EAI_EVENT_RETURN_WAYPOINT);
							}
						}
						else
						{//������ ����
						}
					}
				}
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"NOT implemented UnitType["<<eUnitType<<L"]");
		}break;
	}

	if( UT_SUMMONED & iUnitType )
	{
		GetUnitInRange(eCheckUnitType, FromPos, iRange, UT_SUMMONED, rkUnitArray, static_cast<int>(AI_Z_LIMIT));
	}

	if ( rkUnitArray.empty() )
	{
		BM::Stream kResultPacket;
		kResultPacket.Push((bool)false);
		pkFrom->GetAI()->SetFunctionCallInfo(EAI_Func_FindEnemy, kResultPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	UNIT_PTR_ARRAY::const_iterator unit_itr;
	int iCount = 0;
	for ( unit_itr=rkUnitArray.begin(); unit_itr!=rkUnitArray.end(); ++unit_itr )
	{
		CUnit* pkTarget = unit_itr->pkUnit;
		if (pkFrom->IsTarget(pkTarget, DynamicGndAttr()) && ( pkTarget->GetAbil(AT_CANNOT_DAMAGE) == 0) )
		{
			pkFrom->AddTarget(pkTarget->GetID());
			//pkFrom->GoalPos(GetUnitPos(pkTarget, true));
			if (iCount == 0)
			{
				// AI ��ü���� �ٸ������ PathNormal ������ Copy�ؼ� ����..
				pkFrom->PathNormal(pkTarget->PathNormal());
			}
			++iCount;
			if (iCount >= iMaxTarget)
			{
				break;
			}
		}
	}

	bool bReturn = (iCount > 0);
	BM::Stream kResultPacket;
	kResultPacket.Push((bool)bReturn);
	pkFrom->GetAI()->SetFunctionCallInfo(EAI_Func_FindEnemy, kResultPacket);
	return bReturn;
}

CUnit* PgGround::GetReservedUnitByEffect(CUnit *pUnit)
{
	if (NULL==pUnit)
	{
		return NULL;;
	}

	int const effectId = pUnit->GetAbil(AT_PROVOKE_EFFECT_NO);
	if (0>=effectId)
	{
		return NULL;
	}

	PgUnitEffectMgr const& EffectMgr = pUnit->GetEffectMgr();
	CEffect const* pEffect = EffectMgr.FindEffect(effectId);
	if(NULL==pEffect)
	{
		return NULL;
	}

	BM::GUID const& CasterGuid = pEffect->GetCaster();
	CUnit* TargetUnit = GetUnit(CasterGuid);
	return TargetUnit;
}

// �־��� ��ǥ���� �������� Unit�� ã�´�.
// [PARAMETER]
//	eUT : ã���� �ϴ� ����Ÿ��
//	pt3Pos : ã���� �ϴ� �߽���
//	fRange : ã�� ������
//	iFindType : ã�� ����
//		0 : �������� ��� ��ü
//		1 : X���� ������ ��ü�� ã��
//		2 : X���� ���� ��ü�� ã��
//	bOnlyAlive : ��� �ִ� ��ü�� ã�� ���ΰ�?
//	pfDist : [OUT] ã�� ��ü���� �Ÿ�
//	pkFindUnit : [OUT] ã�� Unit ����
// [RETURN]
//	ã������ true
bool PgGround::FindNearestUnt(const EUnitType eUT, POINT3 const& rkPos, float const fRange, int const iFindType, bool const bOnlyAlive, float* pfDist,
							  BM::GUID& pkFindUnit, NxVec3& kNormalVector)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	float fNearest = 99999.9f;

	CUnit* pkUnit = NULL;
	CUnit* pkFind = NULL ;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUT, kItor);
	while ((pkUnit = PgObjectMgr::GetNextUnit(eUT, kItor)) != NULL)
	{
		if( bOnlyAlive 
		&&	pkUnit->IsDead() )
		{
			continue;
		}
		/*
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if (pkPlayer->GmAbilCheck(US_BIT_INVISIBLE))
		{
			continue;
		}
		*/
		if (pkUnit->GetAbil(AT_UNIT_HIDDEN) > 0 || (0 != pkUnit->GetAbil(AT_CANNOT_DAMAGE)) )	// ���� �ִ� ĳ���̴�.
		{
			// Hidden �����̰ų� AT_CANNOT_DAMAGE �����̸� ���� �� ����.
			continue;
		}

		POINT3 const& kPos = GetUnitPos(pkUnit);

		float fDist = 9999.9f;
		NxVec3 kDistance = NxVec3(kPos.x, kPos.y, kPos.z) - NxVec3(rkPos.x, rkPos.y, rkPos.z);
#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("[%s] Unit[%05d,%05d,%05d], Target[%05d,%05d,%05d], FindType[%d]"), __FUNCTIONW__,
		//	(int)rkPos.x, (int)rkPos.y, (int)rkPos.z, (int)kPos.x, (int)kPos.y, (int)kPos.z, iFindType);
#endif
		if (iFindType != 0)
		{
			NxVec3 kDir = kNormalVector.cross(kDistance);
			bool bIsLeft = (kDir.z > 0);
			if ((bIsLeft && (iFindType != 2)) || (!bIsLeft && (iFindType != 1)))
			{
				continue;
			}
		}
		if (abs(rkPos.z - kPos.z) < 20)	// ���̰� ���̳��� ������ ��~~�� �Ÿ�
		{
			fDist = kDistance.magnitude();
		}

		if(fDist <= fRange && fDist < fNearest)	// New Unit found
		{
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV8, __FL__<<L"FOUND : fDist["<<fDist<<L"], fRange["<<fRange<<L"]");
#endif
			fNearest = fDist;
			pkFind = pkUnit ;
		}
	}
	
	if (pfDist != NULL)
	{
		*pfDist = fNearest;
	}
	if (pkFind != NULL)
	{
		//pkFindUnit = pkFind;
		pkFindUnit = pkFind->GetID();
	}
	return (pkFind != NULL);
}

CUnit* PgGround::FindRandomUnit(POINT3 const &rkPos, float const fRange, EUnitType const eUnitType)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	std::vector<CUnit*> kFindVector;
	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUnitType, kItor);
	while ( (pkUnit = PgObjectMgr::GetNextUnit(eUnitType, kItor)) != NULL)
	{
		float fDistance = ::GetDistance(rkPos,pkUnit->GetPos());
		if ( fRange == -1 || fDistance < fRange )
		{
			kFindVector.push_back(pkUnit);
		}
	}

	int const iSize = kFindVector.size();
	if ( iSize > 0 )
	{
		int const iChoose = BM::Rand_Index(iSize);
		return kFindVector.at(iChoose);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL ;
}

void PgGround::RecvReqRegQuickSlot(CUnit* pkUnit, size_t const slot_idx, SQuickInvInfo const& kQuickInvInfo)
{
	SQuickInvInfo kTempQuickInvInfo = kQuickInvInfo;

	PgRegQuickSlot kReg(slot_idx, kTempQuickInvInfo);
	if(kReg.DoAction(pkUnit))
	{
		BM::Stream kPacket(PT_M_C_NFY_QUICKSLOTCHANGE);
		kPacket.Push(slot_idx);
		kPacket.Push(kTempQuickInvInfo);
		pkUnit->Send(kPacket);
	}
}

void PgGround::RecvReqRegQuickSlotViewPage(CUnit* pkUnit, char const cViewPage)
{
	PgRegQuickSlotViewPage kReg(cViewPage);
	if(kReg.DoAction(pkUnit))
	{//�������� �÷��� �ǰ�. ���̵� �ϸ� ���� �Ǵϱ�. �׳� ������. (�������� ������ ����)
//		BM::Stream kPacket(PT_C_M_REQ_REGQUICKSLOT_VIEWPAGE, cViewPage);
//		SendToContents(kPacket);
//		BM::Stream kPacket(PT_M_C_NFY_QUICKSLOTCHANGE);
//		kPacket.Push(slot_idx);
//		kPacket.Push(kTempQuickInvInfo);
//		pkUnit->Send(kPacket);
	}
}

PgMonster* PgGround::GetFirstMonster(int const iMonID)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgMonster* pkMonster = NULL ;
	
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
	while ((pkMonster = dynamic_cast<PgMonster*>(PgObjectMgr::GetNextUnit(UT_MONSTER, kItor))) != NULL)
	{
		if ( pkMonster->GetAbil(AT_CLASS) == iMonID )
		{
			return pkMonster;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

PgBoss* PgGround::GetBossMonster(int const iMonID)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgBoss* pkBoss = NULL ;
	
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_BOSSMONSTER, kItor);
	while ((pkBoss = dynamic_cast<PgBoss*> (PgObjectMgr::GetNextUnit(UT_BOSSMONSTER, kItor))) != NULL)
	{
		if ( pkBoss->GetAbil(AT_CLASS) == iMonID )
		{
			return pkBoss ;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL ;
}

void PgGround::OnTick100ms()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	DWORD const dwNow = BM::GetTime32();
	DWORD dwkElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_100MS, dwNow, dwkElapsed, true);
#ifdef DEF_ESTIMATE_TICK_DELAY
	Tick100msDelayAverage((Tick100msDelayAverage() + dwkElapsed) / 2);
#endif
//#ifdef AI_DEBUG
//	if (GroundKey().GroundNo() == 9010300)
//	{
//		INFO_LOG(BM::LOG_LV7, _T("OnTick100ms ElapsedTime[%d], Current[%d]"), dwkElapsed, BM::GetTime32());
//	}
//#endif
	//EventView
	__int64 const iEventViewNowTime = g_kEventView.GetLocalSecTime( CGameTime::MILLISECOND );
	DWORD const iEventViewElapsedTime = iEventViewNowTime - m_i64EventViewOldTime;
	m_i64EventViewOldTime = iEventViewNowTime;

	this->PlayerTick(dwkElapsed);
	this->PetTick(dwkElapsed);

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	VEC_UNIT kDeleteUnitVec;
	switch( MonsterTick(dwkElapsed,kDeleteUnitVec) )
	{
	case E_FAIL:
		{
			// �Ʒ��� �ٸ� ƽ�� �� �־ ���ƾ� �Ѵ� --
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("MonsterTick Ret E_FAIL"));
			//return;
		}break;
	case S_FALSE:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("MonsterTick Ret S_FALSE"));
			RemoveAllMonster();
			return;
		}break;
	}

	// xxxxxxxxxxxxxxxxxxxxxxxxx
	//static bool bPrint = false;
	//if(bPrint)
	//{
	//	CPacketLogger &rkPL = GetPacketLogger();
	//	DWORD dwEarlyTime = BM::GetTime32();
	//	DWORD dwLastTime = 0;
	//	int iTotalCount = 0;
	//	int iTotalSentBytes = 0;
	//	int iTotalObjectCnt = 0;

	//	for(CPacketLogger::iterator itr = rkPL.begin();
	//		itr != rkPL.end();
	//		++itr)
	//	{
	//		iTotalObjectCnt++;
	//		float fEachElapsedTime = (itr->second.dwEndTime - itr->second.dwStartTime) / 1000.0f;
	//		INFO_LOG(BM::LOG_LV1, _T("ID : %s, Sent Count : %d, Sent : %d (Bytes), Sent/Sec : %.1f (Bytes/Sec), Count/Sec : %.1f (Count/Sec)"), 
	//			itr->first.str().c_str(),
	//			itr->second.iPacketSendCount,
	//			itr->second.dwSendBytes,
	//			(float)itr->second.dwSendBytes / fEachElapsedTime,
	//			(float)itr->second.iPacketSendCount / fEachElapsedTime);
	//		iTotalCount += itr->second.iPacketSendCount;
	//		iTotalSentBytes += itr->second.dwSendBytes;
	//		dwEarlyTime = std::min(itr->second.dwStartTime, dwEarlyTime);
	//		dwLastTime = std::max(itr->second.dwEndTime, dwLastTime);
	//	}

	//	float fElapsedTime = (dwLastTime - dwEarlyTime) / 1000;
	//	INFO_LOG(BM::LOG_LV1, _T("Total Sent Count : %d, Total Sent : %d (Bytes), Sent/Sec : %.1f (Bytes/Sec), Count/Sec : %.1f (Count/Sec)"),
	//		iTotalCount,
	//		iTotalSentBytes,
	//		iTotalSentBytes / fElapsedTime,
	//		iTotalCount / fElapsedTime);
	//	INFO_LOG(BM::LOG_LV0, _T("Total ElapsedTime : %.1f, Average Sent/Sec : %.1f (Bytes/Object Count), Average Count/Sec %.1f (Sent Count/Object Count)"), fElapsedTime, (float)iTotalSentBytes / iTotalObjectCnt, 
	//		(float)iTotalCount / iTotalObjectCnt);

	//	bPrint = false;
	//}

	float const fAutoHealMultiplier = GetAutoHealMultiplier();
	// Entity�� Tick
	PgObjectMgr::GetFirstUnit(UT_ENTITY,kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_ENTITY,kItor)) != NULL)
	{
		EntityTick(pkUnit, dwkElapsed, fAutoHealMultiplier);
		
		// �׾��°� �˻�
		if( (pkUnit->GetAbil(AT_HP) <= 0) || pkUnit->IsState(US_DEAD) )
		{
			kDeleteUnitVec.push_back(pkUnit);
		}
	}

	bool const MonsterTickOK = this->IsMonsterTickOK();
	
	PgPlayer * pkPlayer = NULL;
	PgObjectMgr::GetFirstUnit(UT_SUMMONED,kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_SUMMONED,kItor)) != NULL)
	{
		if( MonsterTickOK )
		{// ���� AI ���� ��
			if(EAI_ACTION_OPENING==pkUnit->GetAI()->eCurrentAction)
			{
				pkPlayer = dynamic_cast<PgPlayer*>( GetUnit(pkUnit->Caller()) );
				if(pkPlayer && (pkPlayer->IsMapLoading() || false==pkPlayer->IsReadyNetwork()))
				{
					continue;
				}
			}

			pkUnit->UpdateStandUpTime(dwkElapsed);
			TickAI(pkUnit, dwkElapsed);
			AutoHealAndCheckDieSkill(pkUnit, iEventViewElapsedTime, fAutoHealMultiplier);
			EffectTick(pkUnit, dwkElapsed);

			if (pkUnit->GetAbil(AT_HP) <= 0)
			{
				kDeleteUnitVec.push_back(pkUnit);
			}
		}
		else
		{// �δ����� PLAY ���°� ���� �� AI ������ ������ ������ ó���� �ʿ��� ��
			EIndunState const IndunState = this->GetState();
			if( INDUN_STATE_RESULT_WAIT == IndunState
				|| INDUN_STATE_RESULT == IndunState
				|| INDUN_STATE_FAIL == IndunState
				|| INDUN_STATE_CLOSE == IndunState )
			{
				pkUnit->UpdateStandUpTime(dwkElapsed);
				AutoHealAndCheckDieSkill(pkUnit, iEventViewElapsedTime, fAutoHealMultiplier);
				EffectTick(pkUnit, dwkElapsed);

				if (pkUnit->GetAbil(AT_HP) <= 0)
				{
					kDeleteUnitVec.push_back(pkUnit);
				}
			}
		}
	}

	// ���� �̺�Ʈ ƽ
	PgWorldEventMgr::TickWorldEvent(GetGroundNo(), dwkElapsed);

	//
	for(VEC_UNIT::iterator unit_itr=kDeleteUnitVec.begin();unit_itr!=kDeleteUnitVec.end();++unit_itr)
	{
		if (*unit_itr)
		{
			ReleaseUnit(*unit_itr);
		}
	}

	if(m_bUpdateMonsterCount && (GATTR_INSTANCE&GetAttr()) )
	{
		m_bUpdateMonsterCount = false;
		SendMonsterCount();
	}
}

void PgGround::EntityTick(CUnit * pkUnit, DWORD const dwkElapsed, float const fAutoHealMultiplier)
{
	if( true==IsMonsterTickOK() )
	{
		TickAI(pkUnit, dwkElapsed);
		AutoHealAndCheckDieSkill(pkUnit, dwkElapsed, fAutoHealMultiplier);
		EffectTick(pkUnit, dwkElapsed);
	}
}


void PgGround::SendMonsterCount()
{
	size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
	Broadcast( BM::Stream(PT_M_C_NFY_REST_MONSTER_NUM, iLiveMonsterCount) );
}

void PgGround::CheckPetAliveAchievement(PgPlayer * pkPlayer)
{
	/*
	PgBase_Item kPetItem;
	if( S_OK != pkPlayer->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_PET), kPetItem) )
	{
		return;
	}

	PgItem_PetInfo* pkPetInfo = NULL;
	if( false == kPetItem.GetExtInfo(pkPetInfo) )
	{
		return;
	}

	if(true == pkPetInfo->IsDead())
	{
		return;
	}

	SClassKey const kLevelKey = pkPetInfo->ClassKey();

	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);

	PgClassPetDef kPetDef_New;
	if( (false == kClassPetDefMgr.GetDef( kLevelKey, &kPetDef_New )) || 
		(true == pkPetInfo->IsDead()) ||
		(EPET_TYPE_2 != kPetDef_New.GetPetType()))
	{
		return;
	}

	__int64 i64GenTime = 0;
	CGameTime::DBTimeEx2SecTime(static_cast<BM::DBTIMESTAMP_EX>(kPetItem.CreateDate()),i64GenTime);
	__int64 const i64AliveTime = std::max<__int64>(0,g_kEventView.GetLocalSecTime() - i64GenTime);
	int const iDays = static_cast<int>(i64AliveTime/(24 * 60 * 60));
	pkPlayer->SetAbil(AT_ACHIEVEMENT_PET_ALIVETIME,iDays);

	PgCheckAchievements<> kCheckAchievements(AT_ACHIEVEMENT_PET_ALIVETIME, GroundKey());
	kCheckAchievements.DoAction(pkPlayer,NULL);
	*/
}

void SaveAchievement(PgPlayer * pkPlayer)
{
	DWORD dwLastTime = pkPlayer->AchievementLastSaveTime();
	if(false == BM::TimeCheck(dwLastTime,300000))
	{
		return;
	}

	pkPlayer->AchievementLastSaveTime(dwLastTime);

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_SAVE_ACHIEVEMENTS, pkPlayer->GetID()));

	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, pkPlayer->GroundKey(), kOrder);
	kItemModifyAction.DoAction(pkPlayer, NULL);
}

void ProcessAchievementTimeOut(PgPlayer * pkPlayer)
{
	CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
	CONT_ACHIEVEMENT_TIMEOUTED kTimeOuted;

	pkPlayer->GetAchievements()->ProcessAchievementTimeOut(pkPlayer->GetID(), kContModifyOrder, kTimeOuted);

	if(true == kContModifyOrder.empty())
	{
		return;
	}

	PgInventory * pkInv = pkPlayer->GetInven();

	PgBase_Item kItem;
	SItemPos const kItemPos = SItemPos(IT_FIT,EQUIP_POS_MEDAL);

	if(S_OK == pkInv->GetItem(kItemPos, kItem))
	{
		CONT_DEF_ITEM2ACHIEVEMENT const *pkContDef = NULL;
		g_kTblDataMgr.GetContDef(pkContDef);
		if(pkContDef)
		{
			CONT_DEF_ITEM2ACHIEVEMENT::const_iterator iter = pkContDef->find(kItem.ItemNo());
			if(iter != pkContDef->end())
			{
				int const iSaveIdx = (*iter).second.iSaveIdx;
				if(kTimeOuted.find(iSaveIdx) != kTimeOuted.end())
				{
					kContModifyOrder.push_back(SPMO(IMET_MODIFY_COUNT, pkPlayer->GetID(), SPMOD_Modify_Count(kItem,kItemPos,0,true))); // �κ����� ������ ����
				}
			}
		}
	}

	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, pkPlayer->GroundKey(), kContModifyOrder);
	kItemModifyAction.DoAction(pkPlayer, NULL);
}

void PgGround::OnTick30s()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	DWORD const dwNow = BM::GetTime32();
	DWORD dwkElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_30S, dwNow, dwkElapsed, true);	

	{
		BM::PgPackedTime kFailPrevTime, kNextTime;
		PgQuestInfoUtil::MakeDayLoopQuestTime(kFailPrevTime, kNextTime);

		BM::PgPackedTime WeekFailPrevTime, WeekNextTime; // �ְ�����Ʈ�� ���� �ð� ����
		PgQuestInfoUtil::MakeWeekLoopQuestTime(WeekFailPrevTime, WeekNextTime);

		PgPlayer* pkPlayer = NULL;
		CONT_OBJECT_MGR_UNIT::iterator itr;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, itr);
		while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, itr))) != NULL)
		{
			CheckDayLoopQuestTime(GroundKey(), pkPlayer, kFailPrevTime, kNextTime); // ��������Ʈ 24�ð��� ���
			CheckWeekLoopQuestTime(GroundKey(), pkPlayer, WeekFailPrevTime, WeekNextTime); // �ְ�����Ʈ ���
			CheckPetAliveAchievement(pkPlayer);
			SaveAchievement(pkPlayer);
			ProcessAchievementTimeOut(pkPlayer);
			if( JobSkillExpertnessUtil::IsCanResetExhaustion(pkPlayer->JobSkillExpertness()) )
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;
				SPMO kIMO(IMET_JOBSKILL_RESET_EXHAUSTION, pkPlayer->GetID(), SPMOD_JobSkillExhaustion(0)); // �Ƿε� ����
				kOrder.push_back(kIMO);

				PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL, GroundKey(), kOrder);
				kItemModifyAction.DoAction(pkPlayer, NULL);
			}

			// ����� 2000�� �̻� �÷��� ����
			if( this->GetAttr() & GATTR_HIDDEN_F )
			{
				PgAddAchievementValue kMA( AT_ACHIEVEMENT_HIDDEN_PLAY_TIME, 30, GroundKey() );
				kMA.DoAction( pkPlayer, NULL );
			}

			// �̺�Ʈ �Ⱓ ���� �÷��� �ð� ���� ���� Ȯ��( ��� �޼����� ����Ǵ°��� �ƴϴ�. Ư�� �̺�Ʈ �Ⱓ ���ȿ��� �޼� �Ǿ�� �� �α� �ƿ� �ϸ� �ʱ�ȭ �Ǿ�� �� )
			PgAddAchievementValue kAAV(AT_ACHIEVEMENT_CHEKCPLAYTIME, dwkElapsed,GroundKey(),false);
			kAAV.DoAction(pkPlayer, NULL);

			
		}
	}
	
	m_kStoneCtrl.OnTick();

	{
		UNIT_PTR_ARRAY kDeletePetArray;
		PgActPet_TickState kActPetTick( kDeletePetArray, this );

		PgPet * pkPet = NULL;
		CONT_OBJECT_MGR_UNIT::iterator itr;
		PgObjectMgr::GetFirstUnit( UT_PET, itr);
		while ((pkPet = dynamic_cast<PgPet*> (PgObjectMgr::GetNextUnit(UT_PET, itr))) != NULL)
		{
			kActPetTick.DoAction( pkPet );
		}

		UNIT_PTR_ARRAY::iterator unit_itr = kDeletePetArray.begin();
		for ( ;unit_itr != kDeletePetArray.end() ; ++unit_itr )
		{
			BM::GUID const kCallerID = unit_itr->pkUnit->Caller();
			BM::GUID const kPetID = unit_itr->pkUnit->GetID();

			bool bIsType2 = false;
			if(pkPet = dynamic_cast<PgPet*>(unit_itr->pkUnit))
			{
				bIsType2 = (EPET_TYPE_2 == pkPet->GetPetType() || EPET_TYPE_3 == pkPet->GetPetType());
			}
			
			if ( true == ReleaseUnit( unit_itr->pkUnit ) )
			{
				PgPlayer * pkCaller = GetUser( kCallerID );
				if ( pkCaller )
				{
					if ( kPetID == pkCaller->CreatePetID() )
					{
						if(true == bIsType2)
						{
							PgBase_Item kPetItem;
							SItemPos kItemPos;
							if(S_OK == pkCaller->GetInven()->GetItem(kPetID,kPetItem,kItemPos))// �� �����ۿ� �׾��� ������ �����Ѵ�.
							{
								SEnchantInfo kEnchantInfo = kPetItem.EnchantInfo();
								if(false == kEnchantInfo.HasPetDead())
								{
									kEnchantInfo.HasPetDead(true);
									CONT_PLAYER_MODIFY_ORDER kOrder;
									kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaller->GetID(), SPMOD_Enchant(kPetItem, kItemPos, kEnchantInfo)));
									PgAction_ReqModifyItem kItemModifyAction(MIE_Modify, GroundKey(), kOrder);
									kItemModifyAction.DoAction(pkCaller, NULL);
								}
							}

							PgAddAchievementValue kMA(AT_ACHIEVEMENT_PET_DEAD,1,GroundKey());
							kMA.DoAction(pkCaller,NULL);
						}

						PgActPet_RefreshPassiveSkill::DoAction_ReleaseTarget( pkPet, pkCaller, this );
						pkCaller->CreatePetID( BM::GUID::NullData() );
						pkCaller->NftChangedAbil( AT_REFRESH_ABIL_INV, E_SENDTYPE_SELF );
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Different Pet, Caller's PetID : ReleasePetID<" << kPetID << L"> CallerID<" << kCallerID << L"> Caller's PetID<" << pkCaller->CreatePetID() << L">" );
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Not Found Caller<" << pkCaller->GetID() << L"> ReleasePetID<" << kPetID << L">" );
				}
			}
		}
	}

	CheckTickHidden();
}

void PgGround::UpdatePartyUnitAbil(PgPlayer const *pkPC, BYTE const cAbil)
{
	assert(pkPC);
	BM::GUID const & rkPartyGuid = pkPC->PartyGuid();
	BM::GUID const & rkCharGuid = pkPC->GetID();
	if( BM::GUID::NullData() == rkPartyGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("rkPartyGuid is NullData"));
		return;
	}

	
	int iCur = 0, iMax = 0;
	unsigned short sUpdatedHP = 0, sUpdatedMP = 0, sUpdatedClass = 0, sUpdatedLevel = 0;
	POINT3 ptPos;
	bool const bUpdateClass = 0 != (cAbil & PMCAT_Class);
	bool const bUpdatedLevel = 0 != (cAbil & PMCAT_Level);
	bool const bUpdatedHP = 0 != (cAbil & PMCAT_HP);
	bool const bUpdatedMP = 0 != (cAbil & PMCAT_MP);
	bool const bUpdatePos = 0 != (cAbil & PMCAT_POS);

	if( bUpdateClass )
	{
		sUpdatedClass = (unsigned short)pkPC->GetAbil(AT_CLASS);
	}
	if( bUpdatedLevel )
	{
		sUpdatedLevel = (unsigned short)pkPC->GetAbil(AT_LEVEL);
	}
	if( bUpdatedHP )
	{
		iCur = pkPC->GetAbil(AT_HP);//���� HP
		iMax = pkPC->GetAbil(AT_C_MAX_HP);//���յ� �ִ� HP
		sUpdatedHP = (unsigned short)((iCur/(float)iMax)*PMCA_MAX_PERCENT); //100.00 ��Ȯ��
	}
	if( bUpdatedMP )
	{
		iCur = pkPC->GetAbil(AT_MP);
		iMax = pkPC->GetAbil(AT_C_MAX_MP);
		sUpdatedMP = (unsigned short)((iCur/(float)iMax)*PMCA_MAX_PERCENT); //100.00 ��Ȯ��
	}
	if( bUpdatePos )
	{
		ptPos.Clear();
		ptPos = pkPC->GetPos();		
	}

	if( 0 != cAbil )
	{
		BM::Stream kPacket; // ������Ʈ �޽����� PgLocalPartyMgr�� ������
		kPacket.Push( rkPartyGuid );
		kPacket.Push( rkCharGuid );
		kPacket.Push( cAbil );
		if( bUpdateClass )	{ kPacket.Push( sUpdatedClass ); }
		if( bUpdatedLevel )	{ kPacket.Push( sUpdatedLevel ); }
		if( bUpdatedHP )	{ kPacket.Push( sUpdatedHP ); }
		if( bUpdatedMP )	{ kPacket.Push( sUpdatedMP ); }
		if( bUpdatePos )	{ kPacket.Push( ptPos ); }
		m_kLocalPartyMgr.ProcessMsg(PT_A_T_UPDATE_PARTY_PROPERTY, GroundKey(), &kPacket);
	}
}

void PgGround::UpdateAbilGuild(PgPlayer const *pkPC, WORD const eAbil)
{
	assert(pkPC);
	BM::GUID const & rkCharGuid = pkPC->GetID();
	if( BM::GUID::NullData() == rkCharGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("rkCharGuid is NulllData"));
		return;
	}

	int iUpdatedVal = 0;
	switch(eAbil)
	{
	case AT_LEVEL:
	case AT_CLASS:
		{
			iUpdatedVal = pkPC->GetAbil(eAbil);
		}break;
	default:
		{
			iUpdatedVal = 0;
		}break;
	}

	if( iUpdatedVal )
	{
		BM::Stream kNPacket(PT_M_N_NFY_CHANGEABIL, rkCharGuid);
		kNPacket.Push(eAbil);
		kNPacket.Push(iUpdatedVal);
		SendToGuildMgr(kNPacket);
	}
}

void PgGround::ProcessTimeOutedItem(PgPlayer * const pkPlayer)
{
	if((NULL == pkPlayer) || pkPlayer->IsMapLoading())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ProcessTimeOutedItem Failed!"));
		return;
	}

	CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
	pkPlayer->GetInven()->ProcessTimeOutedItem( kContModifyOrder, pkPlayer );

	if(true == kContModifyOrder.empty())
	{
		return;
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_TimeOut, GroundKey(), kContModifyOrder);
	kItemModifyAction.DoAction(pkPlayer, NULL);
}

void PgGround::ProcessTimeOutedMonsterCard(PgPlayer * const pkPlayer)
{
	if((NULL == pkPlayer) || pkPlayer->IsMapLoading())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ProcessTimeOutedItem Failed!"));
		return;
	}

	CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
	pkPlayer->GetInven()->ProcessTimeOutedMonsterCard(kContModifyOrder, pkPlayer);

	if(true == kContModifyOrder.empty())
	{
		return;
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_TimeOut, GroundKey(), kContModifyOrder);
	kItemModifyAction.DoAction(pkPlayer, NULL);
}

void PgGround::ProcessUnbindItem(PgPlayer * const pkPlayer)
{
	if((NULL == pkPlayer) || pkPlayer->IsMapLoading())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ProcessTimeOutedItem Failed!"));
		return;
	}

	CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
	pkPlayer->GetInven()->ProcessUnbindTimeItem(kContModifyOrder);

	if(true == kContModifyOrder.empty())
	{
		return;
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_Item_Unbind, GroundKey(), kContModifyOrder);
	kItemModifyAction.DoAction(pkPlayer, NULL);
}

bool PgGround::IsMacroCheckGround() const
{
	static const int MACRO_GROUND_FLAG = GATTR_FLAG_CONSTELLATION | GATTR_EVENT_GROUND |
		GATTR_FLAG_EXPEDITION | GATTR_FLAG_HARDCORE_DUNGEON | GATTR_FLAG_CART_MISSION | GATTR_FLAG_SUPER | GATTR_FLAG_MISSION;

	return (MACRO_GROUND_FLAG & GetAttr()) ? true : false;
}

void PgGround::ProcessCheckMacroUse(PgPlayer * const pkPlayer)
{
	if ( false == PgGround::ms_kAntiHackCheckVariable.bUse_AntiMacro )
	{
		return;
	}

	if(NULL == pkPlayer) 
	{
		return;
	}

	if( pkPlayer->IsDead() )
	{
		pkPlayer->MacroClear();
		return;
	}

	if( GATTR_FLAG_VILLAGE != (GATTR_FLAG_VILLAGE & GetAttr()) ) // ������ ������ ��� �ʵ忡�� ����â �ð� ������
	{
		if( pkPlayer->ProcessMacroInputTimeOut() )
		{
			pkPlayer->MacroClear();// ��ũ�θ� Ŭ�����ϰ� ���̵��� �ؾ� �Ѵ�.
			
			SReqMapMove_MT kRMM(MMET_None);
			kRMM.kTargetKey.GroundNo(pkPlayer->GetAbil(AT_LAST_VILLAGE));
			kRMM.nTargetPortal = 1;
			PgReqMapMove kMapMove( this, kRMM, NULL );
			HACKING_LOG(BM::LOG_LV0, "[HACKING] Player suspect macro usage [" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() << "]");
			if ( kMapMove.Add( pkPlayer ) )
			{
				kMapMove.DoAction();
			}
			return;
		}
	}

	if( !IsMacroCheckGround() )
	{
		return;
	}

	if(pkPlayer->MacroUseDetected())
	{
		SMACRO_CHECK_TABLE kTable = g_kMacroCheckTable.GetMacroCheckTable();
		wchar_t kCharKey = 0;
		int iCount = 0;
		pkPlayer->MakeMacroCheckPassword(kCharKey,iCount);
		BM::Stream kPacket(PT_M_C_NOTI_MACRO_INPUT_PASSWORD);
		kPacket.Push(kCharKey);
		kPacket.Push(iCount);
		kPacket.Push(pkPlayer->MacroInputFailCount());
		kPacket.Push(kTable.iMacroInputTime);
		pkPlayer->Send(kPacket,E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);
	}
}

int const MAX_REFRESH_RECOMMEND_POINT = 5;
int const RECOMMEND_POINT_REFRESH_TIME = 6;

void PgGround::ProcessRecommendPointRefresh(PgPlayer * const pkPlayer)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	if(ENABLE_RECOMMEND_LEVEL > pkPlayer->GetAbil(AT_LEVEL))
	{
		return;
	}

	BM::DBTIMESTAMP_EX kCurTime;
	g_kEventView.GetLocalTime(kCurTime);

	kCurTime.hour = RECOMMEND_POINT_REFRESH_TIME;
	kCurTime.minute = 0;
	kCurTime.second = 0;
	kCurTime.fraction = 0;

	BM::DBTIMESTAMP_EX kRefreshTime = static_cast<BM::DBTIMESTAMP_EX>(pkPlayer->RefreshDate());
	kRefreshTime.hour = RECOMMEND_POINT_REFRESH_TIME;
	kRefreshTime.minute = 0;
	kRefreshTime.second = 0;
	kRefreshTime.fraction = 0;

	if(kRefreshTime >= kCurTime)
	{
		return;
	}

	BM::PgPackedTime kNewTime = static_cast<BM::PgPackedTime>(kCurTime);
	pkPlayer->RefreshDate(kNewTime);

	CONT_PLAYER_MODIFY_ORDER kCont;
	kCont.push_back(SPMO(IMET_MODIFY_REFRESHDATE, pkPlayer->GetID(),SMOD_RefreshDate(kNewTime)));
	kCont.push_back(SPMO(IMET_SET_RECOMMENDPOINT, pkPlayer->GetID(),SMOD_SetRecommendPoint(MAX_REFRESH_RECOMMEND_POINT)));

	if(true == pkPlayer->IsCreateCard())
	{
		kCont.push_back(SPMO(IMET_SET_TODAYPOPULARPOINT, pkPlayer->GetID(),SMOD_AddPopularPoint(0))); // �ڽ��� 
	}

	PgAction_ReqModifyItem kAction(CIE_Modify_RecommendPoint,GroundKey(),kCont);
	kAction.DoAction(pkPlayer,NULL);
}

void PgGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	DWORD const dwNow = BM::GetTime32();

	DWORD dwkElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwkElapsed, true);
	// Player AutoHealing.......
	UNIT_PTR_ARRAY kDeleteUnit;
	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);

	BM::PgPackedTime const kCurTime = BM::PgPackedTime::LocalTime();

	float const fAutoHealMultiplier = GetAutoHealMultiplier();

	// �޽İ���ġ

	int const iRestExpLevelLimit = GetMapAbil(AT_BONUS_EXP_LEVELLIMIT);

	DWORD dwAddExpInVillageInterval = 0;
	float fAddExpInVillageRate = 0.0f;
	int iMaxExperienceRate = 0;
	dwAddExpInVillageInterval = GetMapAbil(AT_BONUS_EXP_INTERVAL_INVILLAGE) * 1000;
	bool bAddExpInVillage = (dwAddExpInVillageInterval == 0) ? false : BM::TimeCheck(m_kLastAddExpInVillageTickTime, dwAddExpInVillageInterval);
	PgClassDefMgr const * pkClassDefMgr = NULL;
	if (bAddExpInVillage)
	{
		GET_DEF(PgClassDefMgr, kClassDef);
		pkClassDefMgr = &kClassDef;
		iMaxExperienceRate = g_kEventView.VariableCont().iExpAdd_MaxExperienceRate;
		fAddExpInVillageRate = GetMapAbil(AT_BONUS_EXP_RATE_INVILLAGE) / 1000000.0f;
	}

	DWORD dwkElapsed2s = 0;
	bool const bSpeedHackCountClear = CheckTickAvailable(ETICK_INTERVAL_2S, dwNow, dwkElapsed2s, true);

	static DWORD s_dwOnTick1s_Check5s = BM::GetTime32();
	bool const bDoClientProjectileTick = BM::TimeCheck(s_dwOnTick1s_Check5s, 5000);	// 5s ���� �ѹ���

	bool const bDefendModeTick = BM::TimeCheck( m_kDefendModeTickTime, 30000 );

	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		ProcessUnbindItem(pkPlayer);
		ProcessTimeOutedItem(pkPlayer);
		ProcessTimeOutedMonsterCard(pkPlayer);
		ProcessCheckMacroUse(pkPlayer);
		ProcessRecommendPointRefresh(pkPlayer);

		if (bDoClientProjectileTick)
		{
			pkPlayer->ClientProjectileTimeout(dwNow);
		}
		if (bSpeedHackCountClear)
		{
			pkPlayer->SetAbil(AT_SPEED_HACK_COUNT, 0);
		}

		bool const bUpdated = pkPlayer->AutoHeal( dwkElapsed, fAutoHealMultiplier );
		if ( PgGround::ms_kAntiHackCheckActionCount.m_bUseAntiHackCheckActionCount )
		{
			PLAYER_ACTION_TIME_COUNT_RESULT kResult;
			if(pkPlayer->ResultActionPacketCount(false, &kResult, PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime, PgGround::ms_kAntiHackCheckActionCount.m_iCheckCountSum, PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime_Mutiflier))
			{
				if (PgGround::ms_kAntiHackCheckActionCount.m_bForcedResultView || kResult.m_bHackingWarning )
				{
					BM::vstring kLogMsg;
					kLogMsg << __FL__ << "[HACKING][ActionCount] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() << "]  MemberGUID[" << pkPlayer->GetMemberGUID() 
						<< _T("] GroundNo[") << GroundKey().GroundNo() << _T("] HackingType[FastAction] RecvPacketCount[") << kResult.m_iRecvPacketCount << _T("] TotalAniTime[") 
						<< kResult.m_iTotalActionAniTime << _T("] ClientElapsedTime[") << kResult.m_iClientElapsedTime << _T("] ServerElapsedTime[") << kResult.m_iServerElapsedTime << _T("]");
					//VERIFY_INFO_LOG(false, BM::LOG_LV1, kLogMsg) ;
					HACKING_LOG(BM::LOG_LV0, kLogMsg);
					
					//Player���� ����� ������ ���(Debug)
					if((g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug) && (PgGround::ms_kAntiHackCheckActionCount.m_bResultToPlayer))
					{
						BM::vstring kLogMsg; kLogMsg << _T("HackingType[FastAction] RecvPacketCount[") << kResult.m_iRecvPacketCount << _T("] TotalAniTime[") << kResult.m_iTotalActionAniTime << _T("] ClientElapsedTime[") << kResult.m_iClientElapsedTime << _T("] ServerElapsedTime[") << kResult.m_iServerElapsedTime << _T("]");
						pkPlayer->SendWarnMessageStr(kLogMsg);
					}

					//��ŷ ����Ʈ�� ��� �� ���
					if(kResult.m_bHackingWarning && PgGround::ms_kAntiHackCheckActionCount.m_bHackingIndex)
					{
						if (pkPlayer->SuspectHacking(EAHP_ActionCount, static_cast<short>(PgGround::ms_kAntiHackCheckActionCount.m_iAddHackingPoint)))
						{
							// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
							BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_ActionCount_Hack_C) );
							kDPacket.Push( pkPlayer->GetMemberGUID() );
							SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
						}
					}
				}
			}
		}

		UpdatePartyUnitAbil(pkPlayer, PMCAT_HP|PMCAT_MP|PMCAT_POS);//1�� ���� ��Ƽ HP/MP ����

		if ( bDefendModeTick )
		{
			if( pkPlayer->IsUse() )
			{
				EWallowDefendMode	m_eDefendMode = pkPlayer->GetDefendMode();
				int  const iExp = pkPlayer->GetExpRate();
				int  const iMoney = pkPlayer->GetMoneyRate();
				int  const iDrop = pkPlayer->GetDropRate();
				EPPTCondition const kEtcBoolean = pkPlayer->GetEtcBoolean();
				pkPlayer->Update_PlayTime(dwNow);
				if( (m_eDefendMode != pkPlayer->GetDefendMode())
				|| (iExp != pkPlayer->GetExpRate()) 
				|| (iMoney != pkPlayer->GetMoneyRate())
				|| (iDrop != pkPlayer->GetDropRate())
				|| (kEtcBoolean != pkPlayer->GetEtcBoolean()) )
				{
					BM::Stream kPacket(PT_M_O_REQ_PLAYERTIME_DEFEND);
					kPacket.Push(pkPlayer->GetID());
					pkPlayer->WriteToPacket_PlayTimeSimple(kPacket);
					SendToContents(kPacket);

					BM::Stream kRPacket(PT_M_C_REQ_PLAYERTIME_DEFEND);
					pkPlayer->WriteToPacket_PlayTimeSimple(kRPacket);
					pkPlayer->Send(kRPacket);
				}

				if( pkPlayer->IsTimeOverKick() )
				{
					BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_PlayerPlayTimeOver) );
					kDPacket.Push( pkPlayer->GetMemberGUID() );
					::SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
				}
			}
		}

		if (bAddExpInVillage && fAddExpInVillageRate > 0 && (0 < iRestExpLevelLimit && pkPlayer->GetAbil(AT_LEVEL) <= iRestExpLevelLimit))
		{
			int const iClass = pkPlayer->GetAbil(AT_CLASS);
			__int64 const i64LvExp = pkClassDefMgr->GetExperience4Levelup(SClassKey(iClass, pkPlayer->GetAbil(AT_LEVEL)));
			__int64 const i64CurExp = pkPlayer->GetAbil64(AT_EXPERIENCE);
			__int64 const i64MaxExpAdd = std::min<__int64>( pkClassDefMgr->GetMaxExperience(iClass), i64CurExp + static_cast<__int64>(iMaxExperienceRate / 100.0f * i64LvExp));
			
			__int64 i64NewExpAdd = std::max((pkPlayer->GetAbil64(AT_REST_EXP_ADD_MAX) - i64CurExp), 0i64);
			i64NewExpAdd += i64CurExp;
			i64NewExpAdd += std::max(1i64, static_cast<__int64>(i64LvExp * fAddExpInVillageRate));

			i64NewExpAdd = std::min(i64MaxExpAdd, i64NewExpAdd);	//�ִ밪 �˻�
			pkPlayer->SetAbil64(AT_REST_EXP_ADD_MAX, i64NewExpAdd, true);
		}

		m_kEventItemSetMgr.Tick(pkPlayer);
	}// Player Tick End

	// Entity Tick
	PgEntity *pkEntity;
	CONT_OBJECT_MGR_UNIT::iterator kItor_Entity;
	PgObjectMgr::GetFirstUnit(UT_ENTITY, kItor_Entity);
	while ((pkEntity = dynamic_cast<PgEntity*> (PgObjectMgr::GetNextUnit(UT_ENTITY, kItor_Entity))) != NULL)
	{
		if (bDoClientProjectileTick)
		{
			pkEntity->ClientProjectileTimeout(dwNow);
		}
	}

	// home Tick
	PgMyHome * pkHome = NULL;
	CONT_OBJECT_MGR_UNIT::iterator itr_home;
	PgObjectMgr::GetFirstUnit(UT_MYHOME, itr_home);
	while ((pkHome = dynamic_cast<PgMyHome *> (PgObjectMgr::GetNextUnit(UT_MYHOME, itr_home))) != NULL)
	{
		ProcessTimeOutedItem(pkHome);
	}

	// Pet Tick
	PgPet *pkPet = NULL;
	CONT_OBJECT_MGR_UNIT::iterator itr_pet;
	PgObjectMgr::GetFirstUnit(UT_PET, itr_pet);
	while ((pkPet = dynamic_cast<PgPet*> (PgObjectMgr::GetNextUnit(UT_PET, itr_pet))) != NULL)
	{
		pkPet->AutoHeal( dwkElapsed );
	}

	// ItemBox Deleting......
	PgGroundItemBox* pkBox = NULL;
	PgObjectMgr::GetFirstUnit(UT_GROUNDBOX, kItor);
	while( (pkBox = dynamic_cast<PgGroundItemBox*>(PgObjectMgr::GetNextUnit(UT_GROUNDBOX, kItor))) != NULL)
	{
		if( !pkBox->IsInstanceItem() )
		{
			if (dwNow - pkBox->CreateDate() > PgGroundItemBox::ms_GROUNDITEMBOX_DURATION_TIME)
			{
				kDeleteUnit.Add(pkBox);				
			}
		}
	}

	ObjectUnitTick( dwkElapsed, kDeleteUnit );

	UNIT_PTR_ARRAY::const_iterator itor = kDeleteUnit.begin();
	while (itor != kDeleteUnit.end())
	{
		ReleaseUnit(itor->pkUnit);

		++itor;
	}

	//Trade Update
	g_kItemTradeMgr.Tick(dwNow);

	// World Environment
	if( m_kWorldEnvironmentStatus.Tick() )
	{
		BM::Stream kPacket(PT_M_C_NFY_ALL_GROUND_WORLD_ENVIRONMENT_STATUS);
		kPacket.Push( m_kWorldEnvironmentStatus.GetFlag() );
		Broadcast(kPacket);
	}

	// CoupleSkill
	CheckTickCouple();

	// JobSkillLocationItem
	CheckTickJobSkillLocationItem();

	// Duel System Update
	m_kDuelMgr.Update();
}

void PgGround::OnTick5s()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	DWORD const dwNow = BM::GetTime32();
	DWORD dwkElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_5S, dwNow, dwkElapsed, true);
	
	if( BM::TimeCheck(m_kLastGenCheckTime, 6000) )//5�� ���� �����ϱ� 6�ʵ���. �׷� ���ݾ� ��߳��� �Ҳ���
	{
		RareMonsterGenerate();
		MonsterGenerate(m_kContGenPoint_Monster);
		ObjectUnitGenerate(m_kContGenPoint_Object);
		InstanceItemGenerate(m_ContGenPoint_InstanceItem_AnyTeam);
		InstanceItemGenerate(m_ContGenPoint_InstanceItem_RedTeam);
		InstanceItemGenerate(m_ContGenPoint_InstanceItem_BlueTeam);
	}

	DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime();

	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		pkPlayer->Update(dwNow);

		if ( !pkPlayer->IsMapLoading() )
		{
			OnTick_AlramMission( pkPlayer, dwServerElapsedTime );
		}
	}

	CheckTickMarry();	
}

void PgGround::OnTick1m()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	EventMonsterGenerate();
	DropAnyMonsterTimeCheck();
}

void PgGround::AutoHealAndCheckDieSkill(CUnit * pkUnit, DWORD const dwkElapsed, float const fAutoHealMultiplier)
{
	if(!pkUnit)
	{
		return;
	}

	EUnitState const eOldState = pkUnit->GetState();
	pkUnit->AutoHeal( dwkElapsed, fAutoHealMultiplier );
	
	if( US_DEAD != eOldState
	 && pkUnit->GetState() == US_DEAD
	 && pkUnit->GetAbil(AT_SKILL_ON_DIE) )
	{
		if( 0==pkUnit->GetAbil(AT_MON_SKILL_AFTER_DIE) )
		{
			pkUnit->SetState(eOldState);
			pkUnit->SetAbil(AT_MON_SKILL_AFTER_DIE, 1);		// ������ �ѹ��� ��ų�� �ߵ��ǵ��� üũ�ϱ� ���� �뵵
			PgSkillHelpFunc::SkillOnDie(pkUnit, pkUnit->GetAbil(AT_SKILL_ON_DIE), false);
		}
		else if( ON_DIE_SKILL_HP==pkUnit->GetAbil(AT_HP) )
		{
			pkUnit->SetState(eOldState);
		}
	}
}

void PgGround::RecvReqCastBegin(CUnit* pkUnit, int const iSkillNo, DWORD const dwCurrentTime)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] SkillNo[%d]"), __FUNCTIONW__, iSkillNo);
	short int sErrorCode = pkUnit->GetSkill()->CheckSkillAction(iSkillNo, ESS_REQ_CAST, dwCurrentTime,pkUnit->GetAbil(AT_R_COOLTIME_RATE_SKILL), pkUnit->GetAbil(AT_CAST_TIME_RATE));
	BM::Stream kDPacket(PT_M_C_RES_BEGINCAST, iSkillNo);
	kDPacket.Push(sErrorCode);
	pkUnit->Send(kDPacket);
}

void PgGround::InsertItemBox(POINT3 const &kptPos, VEC_GUID const &kOwners, CUnit* pkCaller, PgBase_Item const &kItem, __int64 const i64Money, PgLogCont &kLogCont, ETeam Team , BM::GUID & OutGuid, WORD wCollectRemainTime)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgGroundItemBox *pkItemBox = NULL;

	bool const bIsEmptyItem = PgBase_Item::IsEmpty(&kItem);
	if(!bIsEmptyItem
	|| i64Money)
	{
		pkItemBox = dynamic_cast<PgGroundItemBox*>(g_kTotalObjMgr.CreateUnit(UT_GROUNDBOX, BM::GUID::Create()));

		POINT3 ptRandom1( (float)( 25 - (BM::Rand_Index(50)) ), (float)( 25 - (BM::Rand_Index(50)) ), 25.0f );//�ణ ����.
		// Player�� ���� ���� ������ ������ �������� ���� �����ؾ� �Ѵ�.
		NxVec3 kDirNormal(ptRandom1.x, ptRandom1.y, 0);
		kDirNormal.normalize();
		NxRay kRay(NxVec3(kptPos.x, kptPos.y, kptPos.z+20), kDirNormal);
		NxRaycastHit kHit;
		RayCast(kRay, kHit, NxVec3(ptRandom1.x, ptRandom1.y, ptRandom1.z).magnitude());
		NxShape *pkHitShape = RayCast(kRay, kHit, NxVec3(ptRandom1.x, ptRandom1.y, ptRandom1.z).magnitude());
		if (pkHitShape != NULL)
		{
			ptRandom1.Clear();
			//INFO_LOG(BM::LOG_LV9, _T("[%s] ItemBox pos resetting.."), __FUNCTIONW__);
		}
		pkItemBox->Owner(kOwners);
		pkItemBox->SetPos(kptPos+ptRandom1);
		if (pkCaller != NULL)
		{
			pkItemBox->Caller(pkCaller->GetID());
			pkItemBox->ActionInstanceID(pkCaller->GetActionInstanceID());
		}
		else
		{
			pkItemBox->Caller(BM::GUID::NullData());
			pkItemBox->ActionInstanceID(0);
		}

		if(!bIsEmptyItem)
		{
			pkItemBox->AddItem(kItem);
		}
	
		if(i64Money)
		{
			pkItemBox->AddMoney(static_cast<int>(i64Money));
		}

		GET_DEF(CItemDefMgr, ItemDefMgr);
		CItemDef const * pItemDef = ItemDefMgr.GetDef( kItem.ItemNo() );
		if( pItemDef )
		{
			if( pItemDef->GetAbil(AT_INSTANCE_ITEM) )
			{	// �ν��Ͻ� ������!
				pkItemBox->IsInstanceItem(true);
				
				// GenGroup ��ȣ�� �� ��ȣ�� ����.
				pkItemBox->Team(Team);

				if( wCollectRemainTime > 0 )
				{	// ȸ�� �ð��� ���� ��� ������ �ڽ��� ����.
					pkItemBox->CollectRemainTime(wCollectRemainTime);
				}
			}
		}

		pkItemBox->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
		if ( AddUnit(pkItemBox) )
		{
			OutGuid = pkItemBox->GetID();
			// �α׸� ����
			PgLog kLog( ELOrderMain_Item, ELOrderSub_Drop );
			kLog.Set( 0, pkItemBox->Name() );					// wstrValue1 �����۸�
			if(pkCaller)
			{
				kLog.Set( 1, pkCaller->Name() );				// wstrValue2 ������͸�
			}
			else
			{
				kLog.Set( 1, _T("Not Monster") );				// wstrValue2 ������͸�
			}

			kLog.Set( 0, static_cast<int>(kItem.ItemNo()) );	// iValue1 ������ ��ȣ
			kLog.Set( 2, static_cast<int>(kItem.Count()) );		// iValue3 ������ ����
			kLog.Set( 0, i64Money );							// i64Value1 Money
			kLog.Set( 1, kItem.EnchantInfo().Field_1() );		// i64Value2 ��þƮ���� 1
			kLog.Set( 2, kItem.EnchantInfo().Field_2() );		// i64Value3 ��þƮ���� 2
			kLog.Set( 3, kItem.EnchantInfo().Field_1() );		// i64Value4 ��þƮ���� 3
			kLog.Set( 4, kItem.EnchantInfo().Field_2() );		// i64Value5 ��þƮ���� 4
			kLog.Set( 2, pkItemBox->GetID().str() );			// guidValue1 ������ GUID

			if(pkCaller)
			{
				kLog.Set( 3, pkCaller->GetID().str() );			// guidValue2 ������� GUID
			}
			else
			{
				kLog.Set( 3, BM::GUID::NullData().str() );		// guidValue2 ������� GUID
			}
			kLogCont.Add( kLog );
		}
	}
}

void PgGround::InsertItemBox(TBL_DEF_MAP_REGEN_POINT const & GenInfo, VEC_GUID const & Owners, CUnit* pCaller, PgBase_Item const & Item, __int64 const i64Money, PgLogCont & LogCont, BM::GUID & OutGuid, WORD wCollectRemainTime )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgGroundItemBox * pItemBox = NULL;

	bool const bIsEmptyItem = PgBase_Item::IsEmpty(&Item);

	if(!bIsEmptyItem
	|| i64Money)
	{
		pItemBox = dynamic_cast<PgGroundItemBox*>(g_kTotalObjMgr.CreateUnit(UT_GROUNDBOX, BM::GUID::Create()));

		POINT3 ptRandom1( (float)( 25 - (BM::Rand_Index(50)) ), (float)( 25 - (BM::Rand_Index(50)) ), 25.0f );//�ణ ����.
		// Player�� ���� ���� ������ ������ �������� ���� �����ؾ� �Ѵ�.
		NxVec3 DirNormal(ptRandom1.x, ptRandom1.y, 0);
		DirNormal.normalize();
		NxRay Ray(NxVec3(GenInfo.pt3Pos.x, GenInfo.pt3Pos.y, GenInfo.pt3Pos.z+20), DirNormal);
		NxRaycastHit Hit;
		RayCast(Ray, Hit, NxVec3(ptRandom1.x, ptRandom1.y, ptRandom1.z).magnitude());
		NxShape * pHitShape = RayCast(Ray, Hit, NxVec3(ptRandom1.x, ptRandom1.y, ptRandom1.z).magnitude());
		if( pHitShape != NULL )
		{
			ptRandom1.Clear();
			//INFO_LOG(BM::LOG_LV9, _T("[%s] ItemBox pos resetting.."), __FUNCTIONW__);
		}
		pItemBox->Owner(Owners);
		pItemBox->SetPos(GenInfo.pt3Pos + ptRandom1);
		if( pCaller != NULL )
		{
			pItemBox->Caller(pCaller->GetID());
			pItemBox->ActionInstanceID(pCaller->GetActionInstanceID());
		}
		else
		{
			pItemBox->Caller(BM::GUID::NullData());
			pItemBox->ActionInstanceID(0);
		}

		if( !bIsEmptyItem )
		{
			pItemBox->AddItem(Item);
		}
	
		if( i64Money )
		{
			pItemBox->AddMoney(static_cast<int>(i64Money));
		}

		GET_DEF(CItemDefMgr, ItemDefMgr);
		CItemDef const * pItemDef = ItemDefMgr.GetDef( Item.ItemNo() );
		if( pItemDef )
		{
			if( pItemDef->GetAbil(AT_INSTANCE_ITEM) )
			{	// �ν��Ͻ� ������!
				pItemBox->IsInstanceItem(true);
				pItemBox->GenInfo(GenInfo);
				
				// GenGroup ��ȣ�� �� ��ȣ�� ����.
				int Team = GenInfo.iPointGroup % 10;
				pItemBox->Team(Team);

				if( wCollectRemainTime > 0 )
				{	// ȸ�� �ð��� ���� ��� ������ �ڽ��� ����.
					pItemBox->CollectRemainTime(wCollectRemainTime);
				}
			}
		}

		pItemBox->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
		if ( AddUnit(pItemBox) )
		{
			OutGuid = pItemBox->GetID();

			// �α׸� ����
			PgLog Log( ELOrderMain_Item, ELOrderSub_Drop );
			Log.Set( 0, pItemBox->Name() );					// wstrValue1 �����۸�
			if( pCaller )
			{
				Log.Set( 1, pCaller->Name() );				// wstrValue2 ������͸�
			}
			else
			{
				Log.Set( 1, _T("Not Monster") );				// wstrValue2 ������͸�
			}

			Log.Set( 0, static_cast<int>(Item.ItemNo()) );	// iValue1 ������ ��ȣ
			Log.Set( 2, static_cast<int>(Item.Count()) );		// iValue3 ������ ����
			Log.Set( 0, i64Money );							// i64Value1 Money
			Log.Set( 1, Item.EnchantInfo().Field_1() );		// i64Value2 ��þƮ���� 1
			Log.Set( 2, Item.EnchantInfo().Field_2() );		// i64Value3 ��þƮ���� 2
			Log.Set( 3, Item.EnchantInfo().Field_1() );		// i64Value4 ��þƮ���� 3
			Log.Set( 4, Item.EnchantInfo().Field_2() );		// i64Value5 ��þƮ���� 4
			Log.Set( 2, pItemBox->GetID().str() );			// guidValue1 ������ GUID

			if( pCaller )
			{
				Log.Set( 3, pCaller->GetID().str() );			// guidValue2 ������� GUID
			}
			else
			{
				Log.Set( 3, BM::GUID::NullData().str() );		// guidValue2 ������� GUID
			}
			LogCont.Add( Log );
		}
	}
}

HRESULT PgGround::PlayerTick(DWORD const dwElapsed)
{
	CUnit *pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
	{
		// Tick���� DelayTime �� �о �����̻��� Ǯ�� �־�� �Ѵ�.
		int iDelay = __max(pkUnit->GetDelay() - dwElapsed, 0);
		if (iDelay <= 0)
		{
			if(pkUnit->IsAlive())
			{
				pkUnit->SetState(US_IDLE);
			}
		}
		EffectTick(pkUnit,dwElapsed);
		pkUnit->UpdateStandUpTime(dwElapsed);

		//
		int const iActivateEventScriptID = pkUnit->GetAbil(AT_EVENT_SCRIPT);
		if( 0 != iActivateEventScriptID )
		{
			int const iEventScriptElapsedTime = pkUnit->GetAbil(AT_EVENT_SCRIPT_TIME);
			int const iAlertTime = 1000 * 60 * 15; //15�� ���� ���
			if( iAlertTime < iEventScriptElapsedTime )
			{
				CAUTION_LOG(BM::LOG_LV5, __FL__<<L"player[Ground: "<<GetGroundNo()<<L", Name: "<<pkUnit->Name()<<L", Guid: "<<pkUnit->GetID()<<L"] is long time(over "<<iAlertTime<<L" msec, now: "<<iEventScriptElapsedTime<<L") play Event[ID: "<<iActivateEventScriptID<<L"]");
				pkUnit->SetAbil(AT_EVENT_SCRIPT_TIME, dwElapsed); // ��� �ð� �ʱ�ȭ
			}
			else
			{
				pkUnit->SetAbil(AT_EVENT_SCRIPT_TIME, iEventScriptElapsedTime + dwElapsed); // ��� �ð� ����
			}
		}
		//PlayerQuestTick(pkUnit);
	}
	return S_OK;
}

HRESULT PgGround::PetTick(DWORD const dwElapsed)
{
	CUnit *pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PET, kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PET, kItor)) != NULL)
	{
		EffectTick(pkUnit,dwElapsed);
	}

	return S_OK;
}

//void PgGround::PlayerQuestTick(CUnit* pkUnit)//�ð� ���� ����Ʈ ���۽� ���� �ּ�ó��
//{
//	if( !pkUnit->IsUnitType(UT_PLAYER) ) {return;}
//	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUnit);
//	if( !pkPC ) {return;}
//	PgMyQuest* pkMyQuest = pkPC->GetMyQuest();
//	if( !pkMyQuest ) {return;}
//
//	ContUserQuestState kQuestVec;
//	bool const bRet = pkMyQuest->GetQuestList(kQuestVec);
//	if( !bRet ) {return;}
//
//	//ContUserQuestState kUpdatedVec;
//
//	ContUserQuestState::iterator state_iter = kQuestVec.begin();
//	while(kQuestVec.end() != state_iter)
//	{
//		const ContUserQuestState::value_type& rkUserState = (*state_iter);
//		SUserQuestState* pkUserState = pkMyQuest->Get(rkUserState.iQuestID);
//		if( pkUserState )
//		{
//			PgQuestInfo const* pkQuestInfo = NULL;
//			bool const bFindQuest = g_kQuestMan.GetQuest(rkUserState.iQuestID, pkQuestInfo);
//			if( bFindQuest )
//			{
//				bool const bCanQuest = pkQuestInfo->m_kLimit.CheckTime();
//				if( !bCanQuest )
//				{
//					pkUserState->byQuestState = QS_Failed;
//					//kUpdatedVec.push_back(*pkUserState);
//
//					pkQuestInfoPgAction_NfyChangeState kNfyQuestAction(pkUserState);
//					kNfyQuestAction.DoAction(pkPC, NULL);
//				}
//			}
//		}
//		++state_iter;
//	}
//}

//#ifdef DEF_RETURN_TICK_IF_NOPLAYER
//#undef DEF_RETURN_TICK_IF_NOPLAYER
//#endif

//#define RAY_CHECK_TEST
HRESULT PgGround::ObjectUnitTick( DWORD const dwElapsed, UNIT_PTR_ARRAY& rkDelUnitArray )
{
#ifdef DEF_RETURN_TICK_IF_NOPLAYER
	if ( false == this->IsMonsterTickOK() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
#endif

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_OBJECT, kItor)) != NULL)
	{
		pkUnit->AutoHeal( dwElapsed );

		switch( pkUnit->GetState() )
		{
		case US_DEAD:
			{
				PgObjectUnit* pkObjUnit = dynamic_cast<PgObjectUnit*>(pkUnit);
				if ( pkObjUnit )
				{
					switch( CheckObjectUnitDie(pkObjUnit) )
					{
					case S_OK:
						{
							rkDelUnitArray.Add(pkUnit);
						}break;
					case E_FAIL:
						{
							// �̷� ��Ȳ�� ������ �̻��Ѱǵ�.
							VERIFY_INFO_LOG(false, BM::LOG_LV1,__FL__<<L"ObjectUnit["<<pkObjUnit->Name()<<L"] State:DEAD But HP["<<pkObjUnit->GetAbil(AT_HP)<<L"], GUID["<<pkObjUnit->GetID()<<L"]" );
						}break;
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("PgObjectUnit Point ERROR ") << pkUnit->GetID() );
					rkDelUnitArray.Add(pkUnit);
				}
				
			}break;
		default:
			{
				EffectTick( pkUnit, dwElapsed );
			}break;
		}
	}
	return S_OK;
}

HRESULT PgGround::MonsterTick( DWORD const dwElapsed, VEC_UNIT& rkDelUnitVec )
{
#ifdef DEF_RETURN_TICK_IF_NOPLAYER
	if ( false == this->IsMonsterTickOK() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
#endif

#ifdef RAY_CHECK_TEST
	LARGE_INTEGER kFreq;
	if(!QueryPerformanceFrequency(&kFreq))
	{
		__asm int 3;
	}
	//CONT_OBJECT_MGR_UNIT::iterator kTmpItor;

	NxRay kRay;
	NxRaycastHit kHit;

	NxShape *pkHitShape;
	NxScene *pkScene = GetPhysXScene()->GetPhysXScene();
	int iNbMonster = 0;

	static NxVec3 kRayDir[360] =
	{NxVec3(0, 0, 1),

	static int iPerfCount = 10000;
	static bool bUseCache = false;
	static LARGE_INTEGER kPerfCountStart;
	static LARGE_INTEGER kPerfCountEnd;
	static bool bOutputDebug = false;
	long double fTotalElapsedTime = 0.0f;
#endif

#ifdef _DEBUG
	//DWORD dw00BeginTime = BM::GetTime32();
#endif
	CUnit* pkUnit = NULL;
	PgMonster* pkMonster = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_MONSTER, kItor)) != NULL)
	{
		pkMonster = dynamic_cast<PgMonster*>(pkUnit);

#ifdef RAY_CHECK_TEST
// Ray Check Test		
		iNbMonster++;
		
		bool bIncreaseCount = false;
		
		if(bIncreaseCount == true)
		{
			iPerfCount *= 2;	
		}

		kRay.orig = NxVec3(pkMonster->GetPos().x, pkMonster->GetPos().y, pkMonster->GetPos().z);
		kRay.dir = NxVec3(0,0,1.0f);

		if(pkScene && pkUnit)
		{
			int i = 0;
			::QueryPerformanceCounter(&kPerfCountStart);
			while(i++ < iPerfCount)
			{
				pkScene->raycastAnyShape(kRay, NX_STATIC_SHAPES, -1, 90.0f);
				//pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 90.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
				kRay.dir.x += i;
				kRay.dir.y -= iNbMonster;
				kRay.dir.z += 1.0f;
				kRay.dir.normalize();
			}
			::QueryPerformanceCounter(&kPerfCountEnd);

			__int64 i64Elapsed = kPerfCountEnd.QuadPart - kPerfCountStart.QuadPart;
			long double fElapsed = (long double)(i64Elapsed * 1000000 / kFreq.QuadPart);
			fTotalElapsedTime += fElapsed;

			if(bOutputDebug)
			{
				char szMsg[1024] = {0};
				sprintf_s(szMsg, 1024, "[%3dth Monster] Ray Perfom [%d] : %lf microsec.\n", iNbMonster, iPerfCount, fElapsed);
				OutputDebugStringA(szMsg);
			}
		}
#endif

		switch(pkMonster->GetState())
		{
		case US_DEAD:
			{
				// ���� �׾��� ��~~~
				switch( CheckMonsterDie(pkMonster) )
				{
				case S_OK:
					{
						rkDelUnitVec.push_back(pkMonster);
					}break;
				case S_FALSE:// ��� ���͸� �����ؾ� �ϴ� ��Ȳ ���̻� Tick�� �� �ʿ䰡 ����.
					{
						rkDelUnitVec.clear();
						return S_FALSE;
					}
				case E_FAIL:
					{
						// �̷� ��Ȳ�� ������ �̻��Ѱǵ�.
						VERIFY_INFO_LOG(false, BM::LOG_LV1,__FL__<<L"Monster["<<pkMonster->Name()<<L"] State:DEAD But HP["<<pkMonster->GetAbil(AT_HP)<<L"], GUID["<<pkMonster->GetID()<<L"]");
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CheckMonsterDie Ret E_Fail!"));
					}break;
				}
			}break;
		default:
			{
				MonsterTickAI(pkMonster,dwElapsed);
				g_kElitePatternMng.Run(this,pkMonster,dwElapsed);
				pkUnit->UpdateStandUpTime(dwElapsed);
			}break;
		}

		EffectTick(pkUnit, dwElapsed);
	}

#ifdef _DEBUG
	//if (GetGroundNo() == 9013400)
	//{
	//	DWORD dw00EndTime = BM::GetTime32();
	//	std::cout << __FUNCTION__ << ", ElapsedTime [" << dw00EndTime - dw00BeginTime << "]" << std::endl;
	//	std::cout << "\t\tSendAreaData Time [" << g_dwSendAreaDataTime << "]" << std::endl;
	//	g_dwSendAreaDataTime = 0;
	//}
#endif
	return S_OK;
}

int PgGround::MonsterTickAI(CUnit* pkUnit, DWORD dwElapsed)
{	
    if( !IsRunAI() )
    {
	    return 0;
    }

	//PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkUnit);
	if(pkUnit->GetAbil(AT_FROZEN) > 0)
	{
		if(EAI_ACTION_ROAD==pkUnit->GetAI()->eCurrentAction)
		{ //�������°� Ǯ���� �ٽ� walk action ������ �����ؾ� �ϱ� ������
			pkUnit->SetSync(true);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}
	m_kAIMng.RunAI(pkUnit, dwElapsed);

	if ( pkUnit->GetSync() == true )
	{
		BM::GUID const &rkGuid = pkUnit->GetTarget();
		if(rkGuid != BM::GUID::NullData())
		{
			PgActionResultVector kResultVec;
			PgActionResult* pkResult = kResultVec.GetResult(rkGuid, true);
			SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec);
		}
		else
		{
			SendNfyAIAction(pkUnit, pkUnit->GetState(), 0);
		}		
	}
	else
	{
		if( pkUnit->GetAbil(AT_ELITEPATTEN_STATE) )
		{
			EUnitState const eState = pkUnit->GetState();
			if( pkUnit->GetAbil(AT_SEND_UNIT_STATE) != eState )
			{
				pkUnit->SetAbil(AT_SEND_UNIT_STATE, eState, true, true);
			}
		}
	}

	if ( pkUnit->IsCheckZoneTime(dwElapsed) )
	{
		AdjustArea( pkUnit, true, true );
	}

	return 0;
}

/*
bool PgGround::StageActionFire(BM::GUID &rkGuid, int iSkillNo, SActArg *pkActArg)
{
	CUnit *pkUnit = PgObjectMgr::GetUnit(rkGuid);
	if(!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	iSkillNo = pkUnit->GetSkill()->GetSkillNo();
	if (iSkillNo <= 0)
	{
		INFO_LOG(BM::LOG_LV0, _T("[%s] Unit cannot FireSkill because SkillNo is %d"), __FUNCTIONW__, iSkillNo);
		pkUnit->SetDelay(0);	// ERROR �߻��Ͽ� �ٸ� Action���� �����ؾ� ��.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	UNIT_PTR_ARRAY kUnitArray;
	PgActionResultVector kResult;
	g_kSkillAbilHandleMgr.SkillFire(pkUnit, iSkillNo, pkActArg, &kUnitArray, &kResult);

	OnAttacked(pkUnit, &kUnitArray, iSkillNo, &kResult, pkActArg);

	pkUnit->SetState(US_ATTACK);

	// Sending Result
	SendNfyAIAction(pkUnit, US_ATTACK, &kResult);

	return true;
}
*/

/*
bool PgGround::ApplyAction(BM::GUID &rkGuid, int iActionID, SActArg *pkActArg, char const *pcExtraScript)
{
	CUnit *pkUnit = PgObjectMgr::GetUnit(rkGuid);
	if(!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkUnit->GetSkill()->Reserve(iActionID);
	EUnitState eState = pkUnit->GetState();
	BYTE byActionParam = 0;

	if(eState == US_ATTACK)
	{
		byActionParam = ESS_FIRE;
	}
	else if(eState == US_SKILL_CAST)
	{
		byActionParam = ESS_CASTTIME;
	}
	
	SActionInfo kAction;
	kAction.guidPilot = rkGuid;
	kAction.bIsDown = true;
	kAction.byDirection = 0;
	kAction.ptPos = pkUnit->GetPos();
	kAction.dwActionTerm = 0;
	kAction.iActionID = iActionID;
	kAction.iActionInstanceID = pkUnit->GetActionInstanceID();
	kAction.byActionParam = byActionParam;
	kAction.dwTimeStamp = GetTimeStamp();

	BM::Stream kPacket(PT_M_C_NFY_ACTION2);
	kAction.SendToClient(kPacket);
	kPacket.Push(BM::GetTime32());
	kPacket.Push(pkUnit->LastRandomSeed());
	kPacket.Push(pkUnit->RandomSeedCallCounter());
	kPacket.Push(pkUnit->GoalPos());
	kPacket.Push((BYTE)0);

	bool bExtraData = false;
	if(pcExtraScript)
	{
		bExtraData = true;
		kPacket.Push(bExtraData);
		lua_tinker::call<bool, lwGround, lwPacket, lwUnit, lwActArg>(pcExtraScript, lwGround(this), lwPacket(&kPacket), lwUnit(pkUnit), lwActArg(pkActArg));
	}
	else
	{
		kPacket.Push(bExtraData);
	}

	SendToArea( &kPacket, pkUnit->LastAreaIndex() );

	return true;
}
*/
void PgGround::SendNfyAIAction(CUnit* pkUnit, EUnitState eState, PgActionResultVector* pkActionResultVec, BM::Stream* pkTailPacket, int const iSkillNo)
{
	if (pkUnit == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Unit is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	pkUnit->SetSync(false);
	pkUnit->SetAbil(AT_AI_SYNC, 0);

	int iActionID = 0;
	BYTE byActionParam = 0;
	//EUnitState eState = pkUnit->GetState();
	WORD wGoalDistance = 0;
#ifdef AI_DEBUG
	static DWORD dw153Time = 0;
	if (eState == US_SKILL_CAST)
	{
		DWORD dwNowTime = BM::GetTime32();
		if (dwNowTime - dw153Time < 150)
		{
			INFO_LOG(BM::LOG_LV9, _T("double"));
		}
		dw153Time = dwNowTime;
	}
	POINT3 ptPos = pkUnit->GetPos();
	POINT3 ptGoal = pkUnit->GoalPos();
	INFO_LOG(BM::LOG_LV9, __FL__<<L"State["<<eState<<L"], Pos["<<ptPos<<L"]->GoalPos["<<ptGoal<<L"]");
#endif
	switch(eState)
	{
	case US_NONE:
	case US_IDLE:
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_idle"));
		}break;
	case US_FIRE_WAITING:	//������ ��ٸ��� �����Ҷ� Ŭ��� ���ݸ���� �ؾ� �Ѵ�
	case US_ATTACK:
		{
			iActionID = (0<iSkillNo) ? iSkillNo : pkUnit->GetSkill()->GetSkillNo();
			byActionParam = ESS_FIRE;
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"ActionID["<<iActionID<<L"]");
#endif
		}break;
	case US_SKILL_FIRE:
		{
			if (pkActionResultVec == NULL || pkActionResultVec->GetValidCount() <= 0)
			{
				// Ÿ���� ���ٸ� ������ ���� �ʿ� ����.
				return;
			}
#ifdef AI_DEBUG
			static DWORD dwdkdTime = BM::GetTime32();
			DWORD dwNowtt = BM::GetTime32();
			if (dwNowtt - dwdkdTime < 150)
			{
				INFO_LOG(BM::LOG_LV9, __FL__<<L"double called");
			}
			dwdkdTime = dwNowtt;
#endif
			iActionID = pkUnit->GetSkill()->GetSkillNo();
			byActionParam = ESS_TARGETLISTMODIFY;
		}break;
	case US_MOVE:
	case US_MOVEBYFORCE:
	case US_AROUND_ENEMY:
	case US_CHASE_ENEMY:
		{
			if (pkUnit->GetAbil(AT_MONSTER_TYPE) == EMONTYPE_FLYING)
			{
				GET_DEF(CSkillDefMgr, kSkillDefMgr);
				iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_fly"));
			}
			else
			{
				GET_DEF(CSkillDefMgr, kSkillDefMgr);
				std::wstring const kActionName( pkUnit->GetAbil(AT_AI_RUN_ACTION_MOVESPEED) ? L"a_run" : L"a_walk_left" );
				iActionID = kSkillDefMgr.GetSkillNoFromActionName(kActionName);
			}
			wGoalDistance = pkUnit->GetSkill()->GetAbil(AT_ATTACK_RANGE);
#ifdef AI_DEBUG
			//INFO_LOG(BM::LOG_LV9, _T("[%s] wGoalDistance[%d]"), __FUNCTIONW__, wGoalDistance);
#endif
		}break;
	case US_OPENING:
		{
			iActionID = pkUnit->GetAbil(AT_MANUAL_OPENING);
			if ( iActionID > 0 )
			{
				GET_DEF(CSkillDefMgr, kSkillDefMgr);
				iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_idle"));
			}
			else
			{
				GET_DEF(CSkillDefMgr, kSkillDefMgr);
				iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_opening"));
			}
		}break;
	case US_SKILL_CAST:
		{
			iActionID = pkUnit->GetSkill()->GetSkillNo();
			byActionParam = ESS_CASTTIME;
		}break;
	case US_HIDE_IN:
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_hide"));
		}break;
	case US_HIDE_OUT:
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_unhide"));
		}break;
	case US_RESURRECTION:
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_Resurrection_01"));
		}break;
	default:
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"UNKNOWN Monster State ["<<eState<<L"] "<<pkUnit->GetID()<<" SkillNo["<<iSkillNo<<"]");
			iActionID = kSkillDefMgr.GetSkillNoFromActionName(_T("a_idle"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
		}
	}
	//ASSERT_INFO_LOG(iActionID > 0, BM::LOG_LV5, _T("[%s] ActionID is ZERO, UnitState[%d]"), __FUNCTIONW__, eState);
	if (iActionID <= 0)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"ActionID is ZERO, UnitState["<<eState<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Wrong iActionID!"));
		return;
	}

	SActionInfo kAction;
	kAction.guidPilot = pkUnit->GetID();
	kAction.bIsDown = true;	// �̰��� false�� �ϸ� Monster�� ���������� �ʴ���..
	kAction.byDirection = 0;	// TODO : Direction�� 0���� �ص� �����������??
	kAction.ptPos = pkUnit->GetPos();

	int iElapsedTime = 0;
	PgMonster *pkMonster = dynamic_cast<PgMonster *>(pkUnit);
	if(pkMonster)
	{
		DWORD dwNow = BM::GetTime32();
		DWORD dwLastActionTime = pkMonster->GetLastActionTime();
		if(dwLastActionTime != 0)
		{
			iElapsedTime = dwNow - dwLastActionTime;
		}
		pkMonster->SetLastActionTime(dwNow);
	}
	
	kAction.dwActionTerm = iElapsedTime;
	kAction.iActionID = iActionID;
	kAction.iActionInstanceID = pkUnit->GetActionInstanceID();
	kAction.byActionParam = byActionParam;
	kAction.dwTimeStamp = GetTimeStamp();
	kAction.iUnitState = eState;

	BM::Stream kMPacket(PT_M_C_NFY_ACTION2);
	kAction.SendToClient(kMPacket);
	kMPacket.Push(BM::GetTime32());
	kMPacket.Push(pkUnit->GoalPos());
	if (pkActionResultVec == NULL)
	{
		kMPacket.Push((BYTE)0);
	}
	else
	{
		pkActionResultVec->WriteToPacket(kMPacket);
	}

	// Extra Data
	if (pkTailPacket == NULL)
	{
		kMPacket.Push((bool)false);
	}
	else
	{
		kMPacket.Push((bool)true);
		kMPacket.Push(*pkTailPacket);
		//INFO_LOG(BM::LOG_LV9, _T("[%s] Additional Packed sended PacketSize[%d]"), __FUNCTIONW__, (int)pkTailPacket->Size());
	}

	//INFO_LOG(BM::LOG_LV9, _T("[%s] State[%d], ActionIns No : %d, ElapsedTime : %d"), __FUNCTIONW__, 
	//	(int)pkMonster->GetState(), kAction.iActionInstanceID, iElapsedTime);

	SendToArea( &kMPacket, pkUnit->LastAreaIndex(), BM::GUID::NullData(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );

	// �� ���ʹ� 1�ʴ�, ��Ŷ�� �󸶳� �����°�?
	//{
	//	INFO_LOG(BM::LOG_LV9, _T("[%s] Sent Packet Size : %d\n"), __FUNCTIONW__, kMPacket.Size());
	//	CPacketLogger &rkPacketLogger = GetPacketLogger();
	//	CPacketLogger::iterator itr = rkPacketLogger.find(pkUnit->GetID());
	//	if(itr == rkPacketLogger.end())
	//	{
	//		stPacketData kPacketData;
	//		kPacketData.dwStartTime = BM::GetTime32();
	//		kPacketData.dwEndTime = kPacketData.dwStartTime;
	//		kPacketData.dwSendBytes = kMPacket.Size();
	//		kPacketData.iPacketSendCount = 0;
	//		rkPacketLogger.insert(std::make_pair(pkUnit->GetID(), kPacketData));
	//	}
	//	else
	//	{
	//		itr->second.dwSendBytes += kMPacket.Size();
	//		itr->second.dwEndTime = BM::GetTime32();
	//		++(itr->second.iPacketSendCount);
	//	}
	//}

	pkUnit->ActionID(iActionID);
}

void PgGround::SendNftChaseEnemy(CUnit* pkUnit)
{
	BM::Stream kCPacket(PT_M_C_NFY_MONSTERGOAL, pkUnit->GetID());
//	kCPacket.Push(iElapsedFromLastAction);
//	kCPacket.Push(pkUnit->GetPos());
	kCPacket.Push(pkUnit->GoalPos());
	//kCPacket.Push(pkUnit->GetTarget());
	SendToArea( &kCPacket, pkUnit->LastAreaIndex(), BM::GUID::NullData(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
}

void PgGround::EffectTick(CUnit* pkUnit, DWORD dwElapsed)
{
	// �̹� Unit�� ���� Lock�� ���� �����Ƿ� Effect Lock�� ������ ���� ���Ѵ�.
	if (pkUnit->GetEffectMgr().MustAllClear())
	{
		// �������� �Ѵٰ� ǥ�õǾ� ������...
		pkUnit->ClearAllEffect(true);
		return;
	}
	
	if( pkUnit->GetEffectMgr().MustCurseClear())
	{
		pkUnit->ClearCurseEffect(true);
		return;
	}

	std::vector<int> kDelete;
	PgUnitEffectMgr& rkEffectMgr= pkUnit->GetEffectMgr();
	bool const bDamaged = rkEffectMgr.Damaged();
	if ( bDamaged )
	{
		rkEffectMgr.Damaged(false);
	}

	bool bIndunApply = true;
	if( GetAttr() & GATTR_INSTANCE )
	{
		bIndunApply = false;
		if( GetState() & INDUN_STATE_PLAY )
		{
			bIndunApply = true;
		}
	}

	CEffect* pkEffect = NULL;
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	ContEffectItor kItor;
	rkEffectMgr.GetFirstEffect(kItor);
	bool bKickWhenEffectEnd = false;
	while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
	{
		ECT_CheckTick kCheck = pkEffect->CheckTick( dwElapsed, bDamaged );
		int iLuaCheck = 0;
		bool const bApply = bIndunApply ? true : (pkEffect->GetType()!=EFFECT_TYPE_CURSED && pkEffect->GetType()!=EFFECT_TYPE_SYSTEM && pkEffect->GetType()!=EFFECT_TYPE_SEND_GROUND);
		if ( ECT_DOTICK & kCheck && bApply )	// Do Effect
		{
			// �� �̵����� ���� pkEffect�� SActArg�ȿ� ����� PgGround�� ���ư��� ��� �ٽ� ���� �� �ش�.
			SActArg* pkArg = pkEffect->GetActArg();
			if(pkArg)
			{
				PgGround* pkGround = NULL;
				if(pkArg)
				{
					pkArg->Get(ACTARG_GROUND, pkGround);
				}
				if(NULL == pkGround)
				{
					PgGroundUtil::SetActArgGround(*pkArg, this);
				}
			}

			iLuaCheck = g_kEffectAbilHandleMgr.EffectTick(pkUnit, pkEffect, pkEffect->GetActArg(), dwElapsed);
		}
		else if ( ECT_RESET & kCheck )
		{
			iLuaCheck = g_kEffectAbilHandleMgr.EffectReset(pkUnit, pkEffect, pkEffect->GetActArg(), dwElapsed);
		}

		if ((kCheck & ECT_MUSTDELETE) || (iLuaCheck & ECT_MUSTDELETE))	// Delete
		{
			//INFO_LOG(BM::LOG_LV8, _T("[%s] Effect deleted....ID[%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
			int const iEndActionNo = pkEffect->GetAbil(AT_END_ACTION_WHEN_EFFECT_DELETED);
			if(iEndActionNo)
			{// ����Ʈ�� ���� �ɶ� ��������� �׼��� �ִٸ�
				BM::Stream kPacket(PT_M_C_NFY_END_ACTION);
				kPacket.Push(pkUnit->GetID());
				kPacket.Push(iEndActionNo);
				pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
			}
			kDelete.push_back(pkEffect->GetKey());
			bKickWhenEffectEnd |= (bool)pkEffect->GetAbil(AT_KICK_WHEN_EFFECT_DELETED);
		}
		if (iLuaCheck & ECT_TICKBREAK)
		{
			rkEffectMgr.GetFirstEffect(kItor);
			continue;
		}
	}

	pkUnit->EffectQueueUpdate();

	int iSize = kDelete.size();
	for (int i=0; i<iSize; ++i)
	{
		int iEffect = kDelete.at(i);
		pkUnit->DeleteEffect(iEffect);
		//SEND_DELETE_EFFECT(pkUnit, iEffect, this);
	}

	PgPlayer* pkPlayer = pkUnit->IsUnitType(UT_PLAYER) ? dynamic_cast<PgPlayer*>(pkUnit) : NULL;
	if (bKickWhenEffectEnd && pkPlayer)
	{
		BM::Stream kNotify;
		kNotify.Push(pkPlayer->Name());
		GMCommandToContents(pkPlayer, GMCMD_KICKUSER, &kNotify);
	}
}

bool PgGround::UnitEventNfy(BM::GUID const &rkGuid, BM::Stream* pkPacket)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CUnit *pkUnit = PgObjectMgr::GetUnit(rkGuid);
	if(pkUnit)
	{
		//pkUnit->VNotify(pkPacket);
		BM::CSubject< BM::Stream* >* pkSubject = dynamic_cast<BM::CSubject< BM::Stream* >*>(pkUnit);
		if (pkSubject)
		{
			VUpdate(pkSubject, pkPacket);
		}
		return true;
	}

	// Observer�� ���� �־�
	PgNetModule<> kNetModule;
	if ( SUCCEEDED( PgObjectMgr2::GetObserver( rkGuid, kNetModule ) ) )
	{
		BM::Stream::DEF_STREAM_TYPE kType = 0;
		pkPacket->Pop( kType );

		ProcessObserverPacket( rkGuid, kNetModule, NULL, kType, *pkPacket );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

const int FEARLESS_LEVEL_GAP = 10; // ���� �� ���� ��ɲ� ���� ����

HRESULT PgGround::CheckMonsterDie(PgMonster *pkMonster)
{
	int const iHp = pkMonster->GetAbil(AT_HP);
	
	if( 0 >= iHp )
	{
		BM::GUID kOwnerGuid;
		CUnit* pkOwner = NULL;
		if( CheckUnitOwner(pkMonster, kOwnerGuid) )//������ ���������� ���� ������ �������� Ȯ���Ѵ�
		{
			pkOwner = GetUnit( kOwnerGuid );
		}

		RemoveRareMonster(pkMonster->GetAbil(AT_CLASS),pkMonster);
		RemoveEventMonster(pkMonster->GetAbil(AT_CLASS));

		BM::GUID const& rkPosGuid = pkMonster->GenInfo().kPosGuid;
		int const iGenPointGroup = pkMonster->GenInfo().iPointGroup;

		ContGenPoint_Monster::iterator gen_point_itor = m_kContGenPoint_Monster.find( rkPosGuid );
		if(gen_point_itor != m_kContGenPoint_Monster.end())
		{
			PgGenPoint &kGenPoint = (*gen_point_itor).second;
			kGenPoint.RemoveChild( pkMonster->GetID() );//�긦 ����Ʈ���� ����.
		}

		PgWorldEventMgr::DecMonsterGenPointCount(rkPosGuid);
		PgWorldEventMgr::DecMonsterGenPointGroupCount(iGenPointGroup);
		
		if( pkOwner )
		{
			CUnit * pActor = NULL; // �̺�Ʈ�� �߻� ��ų ĳ���Ͱ� ��������..(��Ƽ��/�������常 ���� or ��Ƽ/������� �ƹ���)
			if( GetAttr() & GKIND_EXPEDITION )		// ������ ������ ���..
			{
				pActor = pkOwner;

				/*PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if( pPlayer )
				{
				if( BM::GUID::NullData() != pPlayer->ExpeditionGuid() )
				{
				BM::GUID MasterGuid;
				m_kLocalPartyMgr.GetExpeditionMasterGuid(pPlayer->ExpeditionGuid(), MasterGuid);
				if( MasterGuid != pPlayer->GetID() )
				{
				pActor = GetUnit(MasterGuid);
				}
				}
				}*/
			}
			else
			{
				pActor = pkOwner;
				if( BM::GUID::NullData() != pkOwner->GetPartyGuid() )
				{
					BM::GUID kMasterGuid;
					if ( GetPartyMasterGuid( pkOwner->GetPartyGuid(), kMasterGuid ) )
					{
						if ( kMasterGuid != pkOwner->GetID() )
						{
							pActor = GetUnit(kMasterGuid);
							if( NULL == pActor )
							{//��Ƽ���� �ʼ����� ���� ���, �����ڸ� ���ͷ� �����Ѵ�.
								pActor = pkOwner;
							}
						}
					}
				}
			}

			if( NULL != pActor )
			{
				PgWorldEventMgr::DoMonsterRegenPoint(GetGroundNo(), rkPosGuid, iGenPointGroup, pActor);
			}
		}
		else
		{
			PgWorldEventMgr::DoMonsterRegenPoint(GetGroundNo(), rkPosGuid, iGenPointGroup);
		}

		// �α׸� ����
		PgLogCont kLogCont(ELogMain_Contents_Monster, ELogSub_Monster_Death );
		kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
		kLogCont.CharacterKey( pkMonster->GetID() );
		kLogCont.GroundNo( GetGroundNo() );			// GroundNo
		kLogCont.Name( pkMonster->Name() );
		kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

		if( pkOwner )
		{
			kLogCont.UnitGender(static_cast<EUnitGender>(pkOwner->GetAbil(AT_GENDER)));
		}

		PgLog kLog( ELOrderMain_Monster, ELOrderSub_Death );
		kLog.Set( 1, static_cast<int>(this->GetAttr()) );	// iValue2 �׶��� Ÿ��

		kLogCont.Add( kLog );
		

		if( pkOwner )
		{
			if(pkMonster->GetAbil(AT_TALK_IDLE) && 0==pkMonster->GetAbil(AT_VOLUNTARILY_DIE))//��� ���͸� �ش� ����� ����Ѵ�.
			{
				BM::Stream kPacket(PT_M_C_NFY_CHAT);
				kPacket.Push(static_cast<BYTE>(CT_RAREMONSTERDEAD));
				kPacket.Push(pkMonster->GetID());
				kPacket.Push(pkMonster->GetAbil(AT_CLASS));
				kPacket.Push(pkOwner->Name());
				Broadcast(kPacket,NULL,E_SENDTYPE_SELF);
			}

			{// ����
				VEC_GUID kPartyMemberVec;
				GetPartyMemberGround(dynamic_cast<PgPlayer const*>(pkOwner), GroundKey(), kPartyMemberVec);

				if( kPartyMemberVec.empty() )
				{
					std::back_inserter(kPartyMemberVec) = pkOwner->GetID();
				}

				VEC_GUID::iterator partyMember_iter = kPartyMemberVec.begin();
				while( partyMember_iter != kPartyMemberVec.end() )
				{
					CUnit* pkUnit = GetUnit( *partyMember_iter );
					if( pkUnit )
					{
						// ������ ����
						if( EMGRADE_BOSS == pkMonster->GetAbil( AT_GRADE ) )
						{
							// ���� �ǰ� ȸ�� ���Ϸ� ���� Ŭ����
							PgComboCounter* pkCounter = NULL;
							if( GetComboCounter( pkUnit->GetID(), pkCounter ) )
							{
								COMBO_TYPE const iAttackedCount = pkCounter->GetComboCount( ECOUNT_TOTAL_DAMAGE ) + pkUnit->GetAbil( AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT );
								pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT, iAttackedCount );
								
								CHECK_ACHIEVEMENTS_DELEGATEMONSTER kCA( AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT, pkMonster->GetAbil(AT_CLASS), GroundKey() );
								kCA.DoAction( pkUnit, NULL );
							}

							{// ���� Ÿ�� ���� ����
								int const iNowTimeSec = g_kEventView.GetLocalSecTime( CGameTime::SECOND );
								int iDungeonStartTimeSec = pkUnit->GetAbil( AT_ACHIEVEMENT_DUNGEON_TIMEATTACK );
								pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_TIMEATTACK, iNowTimeSec - iDungeonStartTimeSec );

								CHECK_ACHIEVEMENTS_DELEGATEMONSTER kCA( AT_ACHIEVEMENT_DUNGEON_TIMEATTACK, pkMonster->GetAbil(AT_CLASS), GroundKey() );
								kCA.DoAction( pkUnit, NULL );
							}
						}

						int const iMonsterIdx = pkMonster->GetAbil(AT_ACHIEVEMENT_MONSTER_TYPE);
						if(iMonsterIdx && (AT_ACHIEVEMENT_KILL_MONSTER_MAX >= iMonsterIdx))
						{
							// �̼� Ŭ���� ȸ�� ����
							PgAddAchievementValue kMA(iMonsterIdx,1,GroundKey());
							kMA.DoAction(pkUnit,NULL);
						}

						int const iLvGap = pkMonster->GetAbil(AT_LEVEL) - pkUnit->GetAbil(AT_LEVEL);
						if(iLvGap >= FEARLESS_LEVEL_GAP)
						{
							PgAddAchievementValue kMA(AT_ACHIEVEMENT_FEARLESS,1,GroundKey());
							kMA.DoAction(pkUnit,NULL);
						}

						const CONT_DEFMAP* pkContDefMap = NULL;
						g_kTblDataMgr.GetContDef(pkContDefMap);
						if(pkContDefMap)
						{
							CONT_DEFMAP::const_iterator itor = pkContDefMap->find(GetGroundNo());
							if (itor != pkContDefMap->end())
							{
								int const iMapKey = (*itor).second.iKey;
								int const iAchievementType = AT_ACHIEVEMENT_MAP_MONSTER_KILL_MIN+iMapKey;
								if(0 < iMapKey && (AT_ACHIEVEMENT_MAP_MONSTER_KILL_MAX >= iAchievementType))
								{
									PgAddAchievementValue kMA(iAchievementType,1,GroundKey());
									kMA.DoAction(pkUnit,NULL);
								}
							}
						}

						//������
						pkUnit->SetAbil( AT_INDUN_PARTICIPATION_RATE, pkUnit->GetAbil(AT_INDUN_PARTICIPATION_RATE)+1 );
					}

					++partyMember_iter;
				}

				if( (false == IndunPartyDie()) && (EMGRADE_BOSS == pkMonster->GetAbil( AT_GRADE )) )
				{// ��Ƽ���� �ƹ��� ���� �ʰ� ���� Ŭ���� ���� ����
					VEC_GUID::iterator partyMember_iter = kPartyMemberVec.begin();
					while( partyMember_iter != kPartyMemberVec.end() )
					{
						CUnit* pkUnit = GetUnit( *partyMember_iter );
						if( pkUnit )
						{
							pkUnit->SetAbil( AT_ACHIEVEMENT_DUNGEON_ANYONE_DIE, 1 );

							CHECK_ACHIEVEMENTS_DELEGATEMONSTER kCA( AT_ACHIEVEMENT_DUNGEON_ANYONE_DIE, pkMonster->GetAbil(AT_CLASS), GroundKey() );
							kCA.DoAction( pkUnit, NULL );
						}

						++partyMember_iter;
					}
				}
			}

			//Exp
			GiveExp(pkMonster,pkOwner);

			// AwakeVale(�÷��̾� ����ġ)
			GiveAwakeValue(pkMonster, pkOwner);

			//Quest
			CheckMonsterDieQuest( pkOwner, pkMonster );
		}

		// PkOwner�� ���� NULL üũ�� SetUnitDropItem���� �Ѵ�.
		HRESULT const hRet = this->SetUnitDropItem( pkOwner, pkMonster, kLogCont );
		kLogCont.Commit();
		return hRet;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgGround::CheckObjectUnitDie(PgObjectUnit* pkUnit)
{
	if( 0 >= pkUnit->GetAbil(AT_HP) )
	{
		BM::GUID const& rkGroupGuid = pkUnit->GetGroupIndex().kGroupID;

		ContGenPoint_Object::iterator gen_point_itor = m_kContGenPoint_Object.find( rkGroupGuid );
		int gpg = 0;
		if(gen_point_itor != m_kContGenPoint_Object.end())
		{
			PgGenPoint_Object& rkGenPoint = (*gen_point_itor).second;
			rkGenPoint.RemoveChild( pkUnit->GetID() );//�긦 ����Ʈ���� ����.
			gpg = rkGenPoint.Info().iPointGroup;
		}

		PgWorldEventMgr::DecObjectGenPointCount(rkGroupGuid);
		PgWorldEventMgr::DecObjectGenPointGroupCount(gpg);
		PgWorldEventMgr::DoObjectRegenPoint(GetGroundNo(), rkGroupGuid, gpg);

		// �α׸� ����
		PgLogCont kLogCont(ELogMain_Contents_Monster, ELogSub_Monster_Death );
		kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
		kLogCont.CharacterKey( pkUnit->GetID() );
		kLogCont.GroundNo( GetGroundNo() );			// GroundNo
		kLogCont.Name( pkUnit->Name() );
		kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

		PgLog kLog( ELOrderMain_Monster, ELOrderSub_Death );
		kLog.Set( 1, static_cast<int>(this->GetAttr()) );	// iValue2 �׶��� Ÿ��

		kLogCont.Add( kLog );

		CUnit* pkOwner = NULL;
		BM::GUID kOwnerGuid;
		if( CheckUnitOwner(pkUnit, kOwnerGuid) )//������ ���������� ���� ������ �������� Ȯ���Ѵ�
		{
			pkOwner = GetUnit(kOwnerGuid);

			if ( pkOwner )
			{
				CheckMonsterDieQuest(pkOwner, pkUnit);

				PgAddAchievementValue kMA(AT_ACHIEVEMENT_DESTROY_OBJECT,1,GroundKey());
				kMA.DoAction(pkOwner,NULL);
			}
		}

		// PkOwner�� ���� NULL üũ�� SetUnitDropItem���� �Ѵ�.
		HRESULT const hRet = this->SetUnitDropItem( pkOwner, pkUnit, kLogCont );
		kLogCont.Commit();
		return hRet;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

size_t PgGround::GetAddDropItemCount(CUnit* pkOwner)
{
	if(NULL == pkOwner) { return 0; }

	int const MAX_RATE = 10000;
	int iAddOnceDropRate = pkOwner->GetAbil(AT_ADD_ONCE_ITEMDROP_RATE);
	int iRandMax = 0;
	size_t iAddOnceDropCount = 0;

	if(iAddOnceDropRate > MAX_RATE)
	{
		iAddOnceDropCount = iAddOnceDropRate / MAX_RATE;
		iRandMax = iAddOnceDropRate % MAX_RATE;
	}
	else
	{
		if(iAddOnceDropRate > 0)
		{
			iRandMax = iAddOnceDropRate;
		}
	}

	if(iRandMax > 0)
	{
		int iRandValue = BM::Rand_Range(MAX_RATE, 1);
		if(iRandValue <= iRandMax)
		{
			iAddOnceDropCount++;
		}
	}
	return iAddOnceDropCount;
}

void PgGround::GetItemOwner(CUnit const* pkUnit, VEC_GUID & rkOwnerVec)const
{
	if(pkUnit)
	{
		if( BM::GUID::IsNotNull(pkUnit->GetExpeditionGuid()) )	// �������� ���.
		{
			m_kLocalPartyMgr.GetExpeditionMemberGround(pkUnit->GetExpeditionGuid(), GroundKey(), rkOwnerVec);
		}
		else if( BM::GUID::IsNotNull(pkUnit->GetPartyGuid()) ) // �Ϲ� ��Ƽ�� ���.
		{
			m_kLocalPartyMgr.GetPartyMemberGround(pkUnit->GetPartyGuid(), GroundKey(), rkOwnerVec);
		}
		else
		{
			std::back_inserter(rkOwnerVec) = pkUnit->GetID();
		}
	}
}

HRESULT PgGround::SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont )
{
	//Item & Gold
	if( pkOwner && pkDroper && 0==pkDroper->GetAbil(AT_VOLUNTARILY_DIE) && pkOwner->GetAbil(AT_DUEL) <= 0)
	{
		// init drop pos
		POINT3 kDropPos = pkDroper->GetPos();
		PgCreateSpreadPos kAction(kDropPos);
		POINT3BY OrientedBy = pkDroper->GetOrientedVector();
		POINT3 Oriented(OrientedBy.x, OrientedBy.y, OrientedBy.z);
		Oriented.Normalize();
		
		int iSpreadRange = 10;
		int const iForceRange = pkDroper->GetAbil(AT_SPREAD_RANGE);
		if( iForceRange )
		{
			iSpreadRange = iForceRange;
		}

		if( EMGRADE_ELITE == pkDroper->GetAbil(AT_GRADE) )
		{
			iSpreadRange = 200;
		}
		kAction.AddDir(PhysXScene()->GetPhysXScene(), Oriented, iSpreadRange);

		PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkOwner);
		const int iDropRate = pkPC ? pkPC->GetDropRate() : 100;
		if(iDropRate < 1)
		{
			return S_OK;
		}

		size_t const iAddDropCount = GetAddDropItemCount(pkPC);

		const int iItemDropRate = iDropRate*2;//level�� Bag drop �α��뼭 Ȯ���� �����ؼ�
		const float fMoneyDropRate = static_cast<float>(pkPC ? pkPC->GetMoneyRate() : 100);
		VEC_GUID kOwnerVec;
		GetItemOwner(pkOwner, kOwnerVec);
		// ignore level item drop
		{
			//F6 ��� ������ ������ ���
			PgGroundUtil::ContAbilValue kAbilCont;
			PgGroundUtil::GetAlwaysDropItem(*pkDroper, kAbilCont);
			PgGroundUtil::ContAbilValue::const_iterator iterItem = kAbilCont.begin();
			while( kAbilCont.end() != iterItem )
			{
				int const iItemNo = iterItem->first;
				int iItemCount = iterItem->second + iAddDropCount;
				bool bDrop = (BM::Rand_Range(100, 1) <= iItemDropRate);

				if( (0 != iItemNo) && bDrop )
				{
					while(iItemCount--)
					{
						kAction.PopPos(kDropPos);
						PgBase_Item kDropItem;
						if(S_OK == CreateSItem(iItemNo, 1, GIOT_FIELD, kDropItem))
						{
							InsertItemBox(kDropPos, kOwnerVec, pkDroper, kDropItem, 0i64, kLogCont );
						}
					}

				}

				++iterItem;
			}
		}

		if(true == g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) || pkDroper->GetAbil(AT_MON_DROPITEM_ENFORCE))//�ӽ�
		{
			// �߱��� ���� ���̿� ���� ������ ��� Ȯ�� �پ��°� ���� ������ �������� �ش�.
		}
		else
		{
			// level diff
			static int const iMinPercent = 0;
			static int const iMaxPercent = 100;
			static int const iLevelWeight = 5;
			int const iPlayerLevel = pkOwner->GetAbil(AT_LEVEL);
			int const iMonsterLevel = pkDroper->GetAbil(AT_LEVEL);
			int const iDiceValue = BM::Rand_Range(iMaxPercent, iMinPercent);
			int const iDiffPercent = iMaxPercent - ((iPlayerLevel - iMonsterLevel) * iLevelWeight);
			if( iDiceValue > iDiffPercent )
			{
				if( !pkDroper->GetAbil(AT_EVENT_MONSTER_DROP) )
				{
					// �������̰� ���� �ƹ��͵� ����
					return S_OK;
				}
			}
		}

		CONT_DEF_MAP_ITEM_BAG const *pkContMapItemBag = NULL;
		g_kTblDataMgr.GetContDef(pkContMapItemBag);

		CONT_DEF_MAP_ITEM_BAG::const_iterator map_item_bag_Itr = pkContMapItemBag->find( GetMapItemBagGroundNo() );
		if( map_item_bag_Itr == pkContMapItemBag->end() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
			return S_OK;
		}

		SetUnitDropItemImp(pkOwner, pkDroper, kDropPos, kAction, iItemDropRate, fMoneyDropRate, kOwnerVec, map_item_bag_Itr->second, kLogCont);
		int iRemainDropCount = iAddDropCount;
		while(iRemainDropCount-- > 0)
		{
			SetUnitDropItemImp(pkOwner, pkDroper, kDropPos, kAction, iItemDropRate, fMoneyDropRate, kOwnerVec, map_item_bag_Itr->second, kLogCont);
		}
	}	
	return S_OK;
}

HRESULT PgGround::SetUnitDropItemImp(CUnit *pkOwner, CUnit *pkDroper, POINT3 const& kDropPos, PgCreateSpreadPos& kCreatePosAction, int iItemDropRate, float fMoneyDropRate, VEC_GUID const& kOwnerVec, TBL_DQT_DEF_MAP_ITEM_BAG const& kDefMapItemBag, PgLogCont &kLogCont)
{
	if(pkOwner == NULL || pkDroper == NULL) { return S_FALSE; }

	// in Bag drop
	PgAction_DropItemBox kDropItem( this, kDefMapItemBag );
	bool const bDropItemRet = kDropItem.DoAction(pkDroper, pkOwner);
	if( false == bDropItemRet ) { return S_FALSE; }

	{
		PgAction_DropLimitedItem kLimitAction(GroundKey(), pkOwner->GetID(), kDropPos, UT_MONSTER, pkDroper->GetAbil(AT_CLASS));
		kLimitAction.DoAction(pkOwner, pkDroper);
	}

	{
		PgAction_DropLimitedItem kLimitAction(GroundKey(), pkOwner->GetID(), kDropPos, 0, GroundKey().GroundNo());
		kLimitAction.DoAction(pkOwner, pkDroper);
	}

	CONT_MONSTER_DROP_ITEM const& rkContDropItem = kDropItem.m_kContDropItem;

	int iAddDropMoneyValueRate = 0;
	int const iAddDropMoneyRate = pkOwner->GetAbil(AT_HUNTED_MON_ADD_DROPMONEY_RATE);
	if (	0 < iAddDropMoneyRate 
		&&	true == lwIsRandSuccess(iAddDropMoneyRate)
		)
	{
		iAddDropMoneyValueRate = pkOwner->GetAbil(AT_HUNTED_MON_ADD_DROPMONEY_VALUE_RATE);
	}

	POINT3 kNewPos;
	CONT_MONSTER_DROP_ITEM::const_iterator bag_itor = rkContDropItem.begin();
	while(rkContDropItem.end() != bag_itor)
	{
		CONT_MONSTER_DROP_ITEM::value_type const& rkElement = (*bag_itor);

		kCreatePosAction.PopPos(kNewPos);

		if(!rkElement.ItemNo())
		{//�� ���.
			int iDropMoney = static_cast<int>(rkElement.EnchantInfo().Field_1());
			if ( iAddDropMoneyValueRate )
			{// �������̴�.
				iDropMoney += ( iDropMoney * iAddDropMoneyValueRate / ABILITY_RATE_VALUE ); 
			}

			iDropMoney *= fMoneyDropRate / 100; //�Ƿε� ����
			if(iDropMoney > 0)
			{
				// ��� �Ӵϴ� �Ӵ� ȹ�淮 ���� ȿ���� ������ ���� �ʰ� ���� ������ ȹ���ϴ� �Ӵϸ� ������ �޵��� �Ѵ�.
				InsertItemBox(kNewPos, kOwnerVec, pkDroper, PgBase_Item::NullData(), static_cast<__int64>(iDropMoney), kLogCont );
			}
		}
		else
		{
			bool bDrop = (BM::Rand_Range(100, 1) <= iItemDropRate); //�Ƿε�

			if( bDrop )
			{
				InsertItemBox(kNewPos, kOwnerVec, pkDroper, rkElement, 0i64, kLogCont );
			}
		}
		++bag_itor;
	}

	return S_OK;
}

void PgGround::CheckMonsterDieQuest(CUnit* pkCaster, CUnit* pkMonster)
{
	if( pkCaster
	&&	pkMonster )
	{
		COMBO_TYPE const iCurCombo = GetCurrentCombo(pkCaster->GetID());

		PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
		if( pkPC )
		{
			VEC_GUID kGuidVec;
			GetPartyMemberGround(pkPC, GroundKey(), kGuidVec, pkPC->GetID());

			//��Ƽ�� ������
			VEC_GUID::iterator guid_iter = kGuidVec.begin();
			while( kGuidVec.end() != guid_iter )
			{
				BM::GUID const & rkGuid = (*guid_iter);
				CUnit* pkPartyMember = GetUnit(rkGuid);
				if( pkPartyMember )
				{
					PgAction_MonsterQuestCheck kMonsterQuestAction(GetGroundNo(),GroundKey(), iCurCombo, 0, 0, 0, this, pkCaster);
					kMonsterQuestAction.DoAction(pkPartyMember, pkMonster);

					PgAction_GlobalQuestCheck kGlobalQuestAction(GetGroundNo(),GroundKey(), iCurCombo);
					kGlobalQuestAction.DoAction(pkPartyMember, pkMonster);

					PgAction_GroundQuestCheck kGroundQuestAction(GetGroundNo(), GroundKey(), iCurCombo);
					kGroundQuestAction.DoAction(pkPartyMember, pkMonster);
				}

				++guid_iter;
			}

			{
				PgAction_MonsterQuestCheck kMonsterQuestAction(GetGroundNo(),GroundKey(), iCurCombo, 0, 0, 0, this, pkCaster);
				kMonsterQuestAction.DoAction(pkCaster, pkMonster);

				PgAction_GlobalQuestCheck kGlobalQuestAction(GetGroundNo(),GroundKey(), iCurCombo);
				kGlobalQuestAction.DoAction(pkCaster, pkMonster);

				PgAction_GroundQuestCheck kGroundQuestAction(GetGroundNo(), GroundKey(), iCurCombo);
				kGroundQuestAction.DoAction(pkCaster, pkMonster);
			}
		}
	}
}

void PgGround::PartyBuffAction(int& iMasterGroundNo, BM::GUID& rkPartyGuid, BM::GUID& kCharGuid, CUnit* pkUnit, bool bBuff, BM::Stream* pkPacket)
{
	BM::GUID kMyCharGuid;
	VEC_UserInfo kVec_UserInfo;
	int iGroundMember = 0;

	pkPacket->Pop(kMyCharGuid);
	PU::TLoadArray_M(*pkPacket, kVec_UserInfo);//pkPacket->Pop(kVec_UserInfo);


	ContPartyMember ContMember;
	ContClassLevel ContClassLevelMember;

	VEC_UserInfo::iterator iterUserInfo = kVec_UserInfo.begin();
	while( kVec_UserInfo.end() != iterUserInfo )
	{
		SPartyUserInfo &kMember = (*iterUserInfo);
		auto ret = ContMember.insert(std::make_pair(kMember.kCharGuid, kMember));
		if( ret.second )
		{
			if( kMember.GroundNo() == iMasterGroundNo )
			{
				bool bBuffRet = false;
				if( !pkUnit )
				{
					bBuffRet = true;
				}
				else if( !(pkUnit->GetID() == kMember.kCharGuid && !bBuff) )
				{
					bBuffRet = true;
				}
				if( bBuffRet )
				{
					auto bret = ContClassLevelMember.insert(std::make_pair(kMember.iClass, 1));
					if( !bret.second )
					{
						ContClassLevel::iterator iter = ContClassLevelMember.find(kMember.iClass);
						if( ContClassLevelMember.end() != iter )
						{
							(*iter).second++;
						}
					}
				}
				++iGroundMember;

				if( !bBuff )
				{
					if( kCharGuid == kMember.kCharGuid )
					{
						--iGroundMember;
					}
				}
			}
		}
		++iterUserInfo;
	}

	SActArg kArg;
	PgGroundUtil::SetActArgGround(kArg, this);

	GET_DEF(PgClassDefMgr, kClassDefMgr);

	int iClassMember = 0;
	int iLevelMember = 0;
	int iPerMember = 0;

	if( ContMember.size() )
	{
		ContPartyMember::iterator iter = ContMember.find(kMyCharGuid);
		if( ContMember.end() != iter )
		{
			CUnit* pkCaster = GetUnit((*iter).first);
			if( pkCaster )
			{
				ContPartyMember::iterator itor = ContMember.begin();
				while( ContMember.end() != itor )
				{
					PgPlayer *pkMemberPlayer = GetUser( (*itor).second.kCharGuid );
					if( pkMemberPlayer )
					{
						iClassMember = pkMemberPlayer->GetAbil(AT_CLASS);
					}					
					else
					{
						iClassMember = (*itor).second.iClass;
					}
					iLevelMember = (*itor).second.sLevel;
					iPerMember = kClassDefMgr.GetAbil(SClassKey(iClassMember, iLevelMember), AT_CLASS_PARTY_BUFF_01);

					int iBuffLevel = 0;

					for(int i=0; i<(int)ContMember.size(); ++i)
					{
						iBuffLevel = i + 1;
						pkCaster->DeleteEffect(iPerMember+iBuffLevel-1);
						//lwUnit(pkCaster).DeleteEffect(iPerMember+iBuffLevel-1);
					}

					CUnit* pkMemberUnit = GetUnit((*itor).first);
					if( pkMemberUnit )
					{
						if( iGroundMember > 1 ) //((bBuff) ? 1 : 2)
						{
							bool bRet = false;
							if( !pkUnit )
							{
								bRet = true;
							}
							else if( (pkUnit->GetID() == pkCaster->GetID()) && !bBuff )
							{
								bRet = false;
							}
							else if( !(pkUnit->GetID() == pkMemberUnit->GetID() && !bBuff) )
							{
								bRet = true;
							}
							if( bRet )
							{
								/*iClassMember = (*itor).second.iClass;
								iLevelMember = (*itor).second.sLevel;
								iPerMember = kClassDefMgr.GetAbil(SClassKey(iClassMember, iLevelMember), AT_CLASS_PARTY_BUFF_01);*/
								ContClassLevel::iterator iterClass = ContClassLevelMember.find(iClassMember);
								if( ContClassLevelMember.end() != iterClass )
								{
									iBuffLevel = (*iterClass).second;
								}
								else
								{
									iBuffLevel = 1;
								}

								int iLevelParty = iBuffLevel-1;

								if( (*itor).second.GroundNo() == iMasterGroundNo )
								{
									GET_DEF(CSkillDefMgr, kSkillDefMgr);
									CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iPerMember+iLevelParty);

									if( pkSkillDef )
									{
										pkCaster->AddEffect(pkSkillDef->GetEffectNo(), 0, &kArg , pkMemberUnit);
									}
								}
							}
						}
					}
					++itor;
				}
			}
		}
	}
}

bool PgGround::CheckUnitOwner(CUnit* pkUnit, BM::GUID& rkOut, BM::GUID * pkOutApplyOwner)
{
	if( !pkUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kOwnerGuid = pkUnit->GetTarget();
	BM::GUID kTopGuid;
	if (pkUnit->IsUnitType(UT_MONSTER))
	{
		PgAggroMeter* pkMeter = NULL;
		bool const bFindMeter = GetMeter(pkUnit->GetID(), pkMeter);
		DREM_ASSERT_FMT(bFindMeter, "Can't Find aggro meter, Ground[%i] Monster[C:%i], Name:%s", GetGroundNo(), pkUnit->GetAbil(AT_CLASS), MB(pkUnit->Name()));
		if (bFindMeter)
		{
			VEC_GUID kScoreGuidVec;
			size_t const iCount = pkMeter->GetScoreTop(kScoreGuidVec, 1);
			if( iCount && GetUnit(*kScoreGuidVec.begin()) )
			{
				kTopGuid = (*kScoreGuidVec.begin());
			}
		}
	}

	CUnit* pkOwner = GetUnit(kTopGuid.IsNotNull() ? kTopGuid : kOwnerGuid);
	if( pkOwner )
	{
        //
        //���� ������
        //
		if( pkOwner->IsUnitType(UT_PET) )
		{
			BM::GUID const & rkCaller = pkOwner->Caller();
			CUnit* pkCaller = GetUnit(rkCaller);
			if( pkCaller )//�����ڰ� Entity�� ��ȯ�� ���
			{
				kOwnerGuid = rkCaller;
				if(pkOwner->IsInUnitType(UT_PET))
				{
					pkCaller->AddAbil(AT_IGNORE_MACRO_COUNT, 1);	//���� ���ΰŴ� ��ũ�� üũ ȸ�� ī��Ʈ ����
				}
			}
		}
        else if( pkOwner->IsUnitType(UT_ENTITY) )
        {
            CUnit* pkCaller = GetUnit(pkOwner->Caller());
            while( pkCaller && pkCaller->IsUnitType(UT_ENTITY) )
            {
                if( !pkCaller->IsHaveCaller() )
                {
                    break;
                }
                pkCaller = GetUnit(pkCaller->Caller());
            }

            if( pkCaller )
            {
                kOwnerGuid = pkCaller->Caller();
            }
        }
		else if( pkOwner->IsUnitType(UT_SUMMONED) )
		{
			kOwnerGuid = pkOwner->Caller();
		}
		else if( pkOwner->IsUnitType(UT_PLAYER) )//������ ���γ�
		{
			kOwnerGuid = pkOwner->GetID();
		}

        //
        //�� Ground���� ������ ������� ������
        //
        if( pkOutApplyOwner )
        {
            if( false==CheckApplyUnitOwner(pkOwner, *pkOutApplyOwner) )
            {
                *pkOutApplyOwner = kOwnerGuid;
            }
        }
	}

	rkOut = kOwnerGuid;
	return true;
}

bool PgGround::CheckApplyUnitOwner(CUnit* pkOwner, BM::GUID& rkOutApplyOwner)
{
    return false;
}

void PgGround::GiveExp(CUnit* pkMonster, CUnit* pkOwner)
{
	if( pkMonster && 0==pkMonster->GetAbil(AT_VOLUNTARILY_DIE)
	&&	pkOwner && pkOwner->GetAbil(AT_DUEL) <= 0)
	{
		int const iPlayerLv = GetGiveLevel(pkMonster, pkOwner);

		if( !iPlayerLv )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("iPlayerLv is 0"));
			return;
		}

		COMBO_TYPE const iCurComboCount = GetCurrentCombo(pkOwner->GetID());
		int const iBonusRate = CalcComboBonusRate(iCurComboCount);
		__int64 const iMonsterExp = (pkMonster->GetAbil64(AT_DIE_EXP) * (0 != pkMonster->GetAbil(AT_ADD_EXP_PER)? pkMonster->GetAbil(AT_ADD_EXP_PER)/100.f: 1.f)); // ���� ���̽� ����ġ ����
		int const iMonsterLv = pkMonster->GetAbil(AT_LEVEL);

		// ���Ͱ���ġ * (1-(�÷��̾�� - ���ͷ���) / 20)) * �޺� ���ʽ�
		float fDiff = 1.f;
		if( iPlayerLv < iMonsterLv )
		{
			fDiff = 1 - ((iPlayerLv-iMonsterLv) / 60.0f);
		}
		else if( iPlayerLv > iMonsterLv )
		{
			fDiff = 1 - ((iPlayerLv-iMonsterLv) / 20.0f);
		}
		// iPlayerLv == iMonsterLv
		__int64 const iAddExp = static_cast<__int64>(iMonsterExp * fDiff);

		//INFO_LOG(BM::LOG_LV9, _T("[%s] kAction_AddExp::DoAction(%I64d)"), __FUNCTIONW__, iAddExp);
		if (iAddExp > 0)	// PlayerLv - MonsterLv �� ���� �����ϼ��� �ִ�.
		{	
			PgAction_AddExp kAction_AddExp(GroundKey(), iAddExp, AEC_KillMonster, this, iMonsterLv, iBonusRate);
			kAction_AddExp.DoAction(pkOwner, pkMonster);
		}

		// �߰����� Copper�� ��� �� ���� �ִ� (��:������ �Ҹ�ġ�� ��ų)
		// ������ CheckMonsterDie ���� �Ϸ��� �ߴµ� �װ����� GetUnit(..)�ѹ� �� �ؾ� �ϱ淡 �̰����� �Ű���.
		if( (BM::Rand_Index(ABILITY_RATE_VALUE)) < pkOwner->GetAbil(AT_ADD_MONEY_RATE) )
		{
			int const iLevel = pkMonster->GetAbil(AT_LEVEL);
			int const iAdd = pkOwner->GetAbil(AT_ADD_MONEY_PER_LV) * iLevel / ABILITY_RATE_VALUE;
			if (iAdd > 0)
			{
				PgAction_ShareGold kMoneyAction(this, iAdd, MCE_Skill, this->m_kLocalPartyMgr);
				kMoneyAction.DoAction(pkOwner, NULL);
				//INFO_LOG(BM::LOG_LV7, _T("[%s] Extra DropMoney Given to [%s]->Copper[%d]"), __FUNCTIONW__, pkOwner->GetID().str().c_str(), iAdd);
			}
		}
	}
}

int PgGround::GetGiveLevel(CUnit * pkMonster, CUnit * pkOwner)
{
    int iPlayerLv = 0;
	switch(pkOwner->UnitType())
	{
	case UT_PET:
		{
			CUnit * pkOwnerPlayer = PgObjectMgr::GetUnit( UT_PLAYER, pkOwner->Caller() );
			if ( !pkOwnerPlayer )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found Pet<" << pkOwner->GetID() << ">'s OwnerPlayer<" << pkOwner->Caller() << L">" );
				return 0;
			}

			pkOwner = pkOwnerPlayer;// pkOwner�� ����Ǿ���
		}// break ��� ����
	case UT_PLAYER:
		{
			iPlayerLv = pkOwner->GetAbil(AT_LEVEL);
		}break;
	case UT_ENTITY:
		{
			PgPlayer *pkOwnerPC = dynamic_cast<PgPlayer*>( GetUnit(pkOwner->Caller()) );
			if( pkOwnerPC )
			{
				iPlayerLv = pkOwnerPC->GetAbil(AT_LEVEL);
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
			assert(false);
			iPlayerLv = 0;
		}break;
	}
    return iPlayerLv;
}

void PgGround::GiveAwakeValue(CUnit* pkMurderee, CUnit* pkMurderer)
{
	if(!pkMurderee || 0!=pkMurderee->GetAbil(AT_VOLUNTARILY_DIE))
	{//������ ���
		return;
	}

	if(!pkMurderer || pkMurderer->GetAbil(AT_DUEL) > 0) //�������϶�
	{
		return;
	}

	// ������ : (((����/20)*����)+(�޺�/5))*(Ŭ���� ���)
	// ��  ������ 1~10, �޺��� 1~100 ������ ������

	// ���Ϳ��� ������ -�̸� ���Ͱ� �� ����
	int iLvGap = pkMurderee->GetAbil(AT_LEVEL) - pkMurderer->GetAbil(AT_LEVEL);
	iLvGap  = std::max< int >( iLvGap, 1 );
	iLvGap  = std::min< int >( iLvGap, 10 );

	COMBO_TYPE uiCurCombo = GetCurrentCombo(pkMurderer->GetID()); // �޺� ��
	uiCurCombo  = std::max< COMBO_TYPE >( uiCurCombo, 1 );
	uiCurCombo  = std::min< COMBO_TYPE >( uiCurCombo, 100 );

	int const iClassNo = pkMurderer->GetAbil(AT_CLASS);		// ����
	int const iMurdererLv = pkMurderer->GetAbil(AT_LEVEL);		// �÷��̾� ����

	// �ش� �������� üũ
	float fClassReviser =0;
	if(IS_CLASS_LIMIT(UCLIMIT_COMMON_KNIGHT,iClassNo))
	{// ��� �迭
		fClassReviser = PgGround::ms_KnightAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_WARRIOR,iClassNo))
	{// ���� �迭
		fClassReviser = PgGround::ms_WarriorAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_MAGE,iClassNo))
	{// ������ �迭
		fClassReviser = PgGround::ms_MageAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_BATTLEMAGE,iClassNo))
	{// ��Ʋ ������ �迭
		fClassReviser = PgGround::ms_BattleMageAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_HUNTER,iClassNo))
	{// ��ɲ� �迭
		fClassReviser = PgGround::ms_HunterAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_RANGER,iClassNo))
	{// ������ �迭
		fClassReviser = PgGround::ms_RangerAwakeReviserByKill;
	}		
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_CLOWN,iClassNo))
	{// ���� �迭
		fClassReviser = PgGround::ms_ClownAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_COMMON_ASSASSIN,iClassNo))
	{// ��ؽ� �迭
		fClassReviser = PgGround::ms_AssassinAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_SHAMAN,iClassNo))
	{// ��ȯ�� �迭
		fClassReviser = PgGround::ms_AssassinAwakeReviserByKill;
	}
	else if(IS_CLASS_LIMIT(UCLIMIT_DOUBLE_FIGHTER,iClassNo))
	{// �ֵ��� ������ �迭
		fClassReviser = PgGround::ms_AssassinAwakeReviserByKill;
	}
	if(0.0f == fClassReviser)
	{// ���� ���ٸ� Default ������ ����
		fClassReviser = DEFAULT_CLASS_AWAKE_VALUE_REVISER;
	}

	int iAddValue = 0;
	{// ��� ���� ���� (((iMurdererLv/20)*iLvGap)+((uiCurCombo/5))) * (fClassReviser)
		float  fCalc1 = (static_cast<float>(iMurdererLv)/20.0f)*static_cast<float>(iLvGap);
		float  fCalc2 = static_cast<float>(uiCurCombo)/5.0f;
		float  fCalc3 = fCalc1+fCalc2;
		float  fCalc4 = fCalc3 * fClassReviser;
		iAddValue = static_cast<int>(fCalc4);
	}
	iAddValue = std::max< int >( iAddValue, 1 );

	{// ��Ƽ���� ����ġ ���� �ϱ�����
		VEC_GUID kPartyMemberArray;
		PgPlayer* pkPlayer = NULL;
		switch(pkMurderer->UnitType())
		{// ���� ������ � ������ Ȯ���Ͽ�, �÷��̾ ã��
		case UT_PLAYER:
			{
				pkPlayer = dynamic_cast<PgPlayer*>(pkMurderer);
			}break;
		case UT_ENTITY:
		case UT_PET:
			{
				BM::GUID const & rkCaller = pkMurderer->Caller();
				CUnit* pkCaller = GetUnit(rkCaller);
				if( pkCaller )//�����ڰ� Entity�� ��ȯ�� ���
				{
					if(pkCaller->IsInUnitType(UT_PLAYER))
					{
						pkPlayer = dynamic_cast<PgPlayer*>(pkCaller);
					}
				}
			}break;
		default:
			{
			}break;
		}
		if(pkPlayer)
		{// �÷��̾ ã������
			VEC_GUID kPartyMemberArray;
			GetPartyMemberGround(pkPlayer, GroundKey(), kPartyMemberArray, pkPlayer->GetID());
			if(kPartyMemberArray.size())
			{// �÷��̾���
				VEC_GUID::iterator kItor = kPartyMemberArray.begin();
				while(kPartyMemberArray.end() != kItor)
				{
					CUnit* pkPartyUnit = GetUnit(*kItor);
					if(pkPartyUnit
						&& pkPartyUnit->GetAbil(AT_ENABLE_AWAKE_SKILL)  // ����ġ�� ���ϼ� �ִ� ��Ƽ���鿡��
						)
					{// ���� ���� ����ġ�� �����ټ� �ְ� �ϰ�
						int const iNowAwakeValue = pkPartyUnit->GetAbil(AT_AWAKE_VALUE);
						int const iRetValue = std::min< int >( iNowAwakeValue+iAddValue , AWAKE_VALUE_MAX );
						if( iRetValue != iNowAwakeValue )
						{
							pkPartyUnit->SetAbil(AT_AWAKE_VALUE, iRetValue, true);
						}
					}
					++kItor;
				}
			}
		}
	}

	if ( pkMurderer->GetAbil(AT_ENABLE_AWAKE_SKILL) )
	{// ��Ÿ�� ���� �÷��̾��� ���� ����ġ�� �����Ѵ�
		int const iNowAwakeValue = pkMurderer->GetAbil(AT_AWAKE_VALUE);
		int const iRetValue = std::min< int >( iNowAwakeValue+iAddValue , AWAKE_VALUE_MAX );
		if ( iRetValue != iNowAwakeValue )
		{
			pkMurderer->SetAbil( AT_AWAKE_VALUE, iRetValue, true );
		}
	}
}
//bool PgGround::SendToUser_ByGuidVec(VEC_GUID const &rkVec, BM::Stream const &rkPacket, BM::GUID const &rkPartyGuid, DWORD const dwSendType)
//{
//	if(0 == rkVec.size())
//	{
//		return false;
//	}
//
//	BM::CAutoMutex kLock(m_kRscMutex);
//	for(unsigned int i=0; rkVec.size() > i; ++i)
//	{
//		CUnit *pkUnit = GetUnit(rkVec[i]);
//		if(!pkUnit) continue;
//
//		PgPlayer *pkPC = dynamic_cast<PgPlayer *>(pkUnit);
//		if(!pkPC) continue;
//
//		if(pkPC->PartyGuid() != rkPartyGuid) continue;
//
//		pkPC->Send(rkPacket, dwSendType);
//	}
//	return true;
//}

void PgGround::SetTeam(BM::GUID const& kGuid, int const iTeam)
{
}

int PgGround::GetTeam(BM::GUID const& kGuid)const
{
    return 0;
}

bool PgGround::SendToUser_ByGuidVec(VEC_GUID const &rkVec, BM::Stream const &rkPacket, DWORD const dwSendType)
{
	if(0 == rkVec.size())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CAutoMutex kLock(m_kRscMutex);

	for(unsigned int i=0; rkVec.size() > i; ++i)
	{
		CUnit *pkUnit = GetUnit(rkVec[i]);
		if(!pkUnit) continue;

		PgPlayer *pkPC = dynamic_cast<PgPlayer *>(pkUnit);
		if(!pkPC) continue;

		pkPC->Send(rkPacket, dwSendType);
	}
	return true;
}

/*
bool PgGround::SendToUser_ByGuid(BM::GUID& rkGuid, BM::Stream &rkPacket, DWORD const dwSendType)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CUnit *pkUnit = GetUnit(rkGuid);
	if ( !pkUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkUnit->Send(rkPacket, dwSendType);
	return true;
}
*/

//bool PgGround::SetUser_PartyGuid(VEC_GUID const& rkVec, BM::GUID const &rkPartyGuid)
//{
//	if( rkVec.empty() )
//	{
//		return false;
//	}
//
//	BM::CAutoMutex kLock(m_kRscMutex);
//	PgAction_PartyGuid kAction( PARTY_SYS_DEFAULT, this->GetAttr(), rkPartyGuid );
//
//	VEC_GUID::const_iterator guid_iter = rkVec.begin();
//	for(; rkVec.end() != guid_iter; ++guid_iter)
//	{
//		kAction.DoAction( GetUnit(*guid_iter), NULL );
//	}
//	return true;
//}

void PgGround::SetUnitAbil( BM::GUID const & kCharGuid, WORD const Type, int const iValue, bool const blsSend, bool const bBroadcast )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CUnit *pkUnit = GetUnit( kCharGuid );
	if ( !pkUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	pkUnit->SetAbil( Type, iValue, blsSend, bBroadcast );
}

bool PgGround::RemoveAllMonster( bool const bKill , CUnit *pUnit, int iVoluntarilyDie)	//! ��� ���͸� �����Ѵ�. 
{
	BM::CAutoMutex kLock(m_kRscMutex);

	typedef std::set< std::pair< BM::GUID, int > > CONT_MON_GENPOINT_INFO;

	CONT_OBJECT_MGR_UNIT *kUnitCont;
	if ( PgObjectMgr::GetUnitContainer(UT_MONSTER, kUnitCont) )
	{
		CUnit* pkUnit = NULL;
		PgSmallArea *pkArea = NULL;
		ContGuidSet kContObjectGenPointGuid;
		CONT_MON_GENPOINT_INFO kContMonGenPointInfo;
		CONT_MON_GENPOINT_INFO kContObjectGenPointInfo;
		for ( CONT_OBJECT_MGR_UNIT::iterator it = kUnitCont->begin(); it!=kUnitCont->end(); ++it )
		{
			pkUnit = it->second;
			if ( pkUnit )
			{
				// �һ� ���� �ʵ��� ��� ���
				pkUnit->SetAbil(AT_REVIVE_COUNT, 0);

				EUnitType const eType = pkUnit->UnitType();
				switch( eType )
				{
				case UT_MONSTER:
				case UT_BOSSMONSTER:
					{
						PgMonster* pkMonster = dynamic_cast< PgMonster* >(pkUnit);
						if( pkMonster )
						{
							BM::GUID const& rkPosGuid = pkMonster->GenInfo().kPosGuid;
							int const iGenPointGroup = pkMonster->GenInfo().iPointGroup;
							PgWorldEventMgr::DecMonsterGenPointCount(rkPosGuid);
							PgWorldEventMgr::DecMonsterGenPointGroupCount(iGenPointGroup);
							kContMonGenPointInfo.insert( std::make_pair(rkPosGuid, iGenPointGroup) );
						}
					}break;
				case UT_OBJECT:
					{
						PgObjectUnit* pkObjectUnit = dynamic_cast< PgObjectUnit* >(pkUnit);
						if( pkObjectUnit )
						{
							BM::GUID const& rkGroupGuid = pkObjectUnit->GetGroupIndex().kGroupID;
							PgWorldEventMgr::DecObjectGenPointCount(rkGroupGuid);
							PgWorldEventMgr::DecObjectGenPointGroupCount(pkObjectUnit->GetGroupIndex().iIndex);
							kContObjectGenPointGuid.insert(rkGroupGuid);

							kContObjectGenPointInfo.insert( std::make_pair(rkGroupGuid, pkObjectUnit->GetGroupIndex().iIndex) );
						}
					}break;
				default:
					{
					}break;
				}

				if ( bKill )
				{
					if( iVoluntarilyDie > 0 )
					{
						pkUnit->SetAbil(AT_VOLUNTARILY_DIE, iVoluntarilyDie);
					}
					::OnDamaged(NULL, pkUnit, 0, pkUnit->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());
				}
				else
				{
					pkArea = GetArea( pkUnit->LastAreaIndex() );
					if( pkArea )
					{
						pkArea->RemoveUnit(pkUnit->GetID());
					}
					DelMeter(pkUnit->GetID());
					g_kTotalObjMgr.ReleaseUnit(pkUnit);
					pkUnit = NULL;
				}
			}
		}

		{ // World Event Process
			CUnit * pActor = NULL; // �̺�Ʈ�� �߻� ��ų ĳ���Ͱ� ��������..(��Ƽ��/�������常 ���� or ��Ƽ/������� �ƹ���)
			if( GetAttr() & GKIND_EXPEDITION )		// ������ ������ ���..
			{
				pActor = pUnit;

				/*PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if( pPlayer )
				{
				if( BM::GUID::NullData() != pPlayer->ExpeditionGuid() )
				{
				BM::GUID MasterGuid;
				m_kLocalPartyMgr.GetExpeditionMasterGuid(pPlayer->ExpeditionGuid(), MasterGuid);
				if( MasterGuid != pPlayer->GetID() )
				{
				pActor = GetUnit(MasterGuid);
				}
				}
				}*/
			}
			else
			{
				pActor = pUnit;
				if(pUnit)
				{
					if( BM::GUID::NullData() != pUnit->GetPartyGuid() )
					{
						BM::GUID kMasterGuid;
						if ( GetPartyMasterGuid( pUnit->GetPartyGuid(), kMasterGuid ) )
						{
							if ( kMasterGuid != pUnit->GetID() )
							{
								pActor = GetUnit(kMasterGuid);
							}
						}
					}
				}
			}

			if( NULL != pActor )
			{
				CONT_MON_GENPOINT_INFO::const_iterator mon_iter = kContMonGenPointInfo.begin();
				while( kContMonGenPointInfo.end() != mon_iter )
				{
					PgWorldEventMgr::DoMonsterRegenPoint(GetGroundNo(), (*mon_iter).first, (*mon_iter).second, pActor);
					++mon_iter;
				}

				CONT_MON_GENPOINT_INFO::const_iterator object_iter = kContObjectGenPointInfo.begin();
				while( kContObjectGenPointInfo.end() != object_iter )
				{
					PgWorldEventMgr::DoObjectRegenPoint(GetGroundNo(), (*object_iter).first, (*object_iter).second, pActor);
					++object_iter;
				}
			}
		}

		if ( !bKill )
		{
			PgObjectMgr2::UnRegistAllUnit(UT_MONSTER);
			Broadcast( BM::Stream(PT_M_C_NFY_REMOVE_ALL_MONSTER) );
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::RemoveGenGroupMonster(int const iGenGroupNo, bool const bKill )	//! ��� ���͸� �����Ѵ�. 
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CONT_OBJECT_MGR_UNIT *kUnitCont;
	if (! PgObjectMgr::GetUnitContainer(UT_MONSTER, kUnitCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CUnit* pkUnit = NULL;
	PgSmallArea *pkArea = NULL;

	SGenGroupKey kGenGroupKey;
	GetGenGroupKey(kGenGroupKey);

	ContGenPoint_Monster kContGenPoint;
	GET_DEF_CUSTOM( PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
	if(!SUCCEEDED(kGenPointMgr.GetGenPoint_Monster( kContGenPoint, kGenGroupKey, false, iGenGroupNo )) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	for(ContGenPoint_Monster::iterator iter = kContGenPoint.begin();iter != kContGenPoint.end();++iter)
	{
		ContGenPoint_Monster::iterator gen_iter = m_kContGenPoint_Monster.find((*iter).first);
		if(gen_iter == m_kContGenPoint_Monster.end())
		{
			continue;
		}

		PgGenPoint & kGenPoint = (*gen_iter).second;

		OwnChildCont kContChild;
		kGenPoint.GetChild(kContChild);

		for(OwnChildCont::const_iterator child_iter = kContChild.begin();child_iter != kContChild.end();++child_iter)
		{
			BM::GUID const & kChildGuid = (*child_iter);

			CONT_OBJECT_MGR_UNIT::iterator find_iter = kUnitCont->find(kChildGuid);
			if(find_iter == kUnitCont->end())
			{
				continue;
			}

			pkUnit = (*find_iter).second;

			if(NULL == pkUnit)
			{
				continue;
			}

			pkUnit->ClearTargetList();

			::OnDamaged(NULL, pkUnit, 0, pkUnit->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());
		}

		m_kContGenPoint_Monster.erase(gen_iter);
	}

	return true;
}

bool PgGround::RemoveGenGroupInsItem(int const iGenGroupNo)	//! ��� �ν��Ͻ������� ����.
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_OBJECT_MGR_UNIT * UnitCont;
	if( !PgObjectMgr::GetUnitContainer(UT_GROUNDBOX, UnitCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CUnit * pUnit = NULL;
	PgSmallArea * pArea = NULL;

	SGenGroupKey GenGroupKey;
	GetGenGroupKey(GenGroupKey);

	ContGenPoint_InstanceItem ContGenPoint;
	GET_DEF_CUSTOM(PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, GenPointMgr);
	if( !SUCCEEDED( GenPointMgr.GetGenPoint_Monster( ContGenPoint, GenGroupKey, false, iGenGroupNo )) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int TeamNo = iGenGroupNo % 10;
	ContGenPoint_InstanceItem * pContItem = NULL;
	switch( TeamNo )
	{
	case TEAM_NONE:
		{
			pContItem = &m_ContGenPoint_InstanceItem_AnyTeam;
		}break;
	case TEAM_RED:
		{
			pContItem = &m_ContGenPoint_InstanceItem_RedTeam;
		}break;
	case TEAM_BLUE:
		{
			pContItem = &m_ContGenPoint_InstanceItem_BlueTeam;
		}break;
	}

	if( !pContItem )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	for( ContGenPoint_InstanceItem::iterator iter = ContGenPoint.begin() ; iter != ContGenPoint.end() ; ++iter )
	{
		ContGenPoint_InstanceItem::iterator gen_iter = pContItem->find((*iter).first);
		if( gen_iter == pContItem->end() )
		{
			continue;
		}

		PgGenPoint & GenPoint = (*gen_iter).second;
		OwnChildCont ContChild;
		GenPoint.GetChild(ContChild);

		for( OwnChildCont::const_iterator child_iter = ContChild.begin() ; child_iter != ContChild.end() ; ++child_iter )
		{
			BM::GUID const & ChildGuid = (*child_iter);
			CONT_OBJECT_MGR_UNIT::iterator find_iter = UnitCont->find(ChildGuid);
			if( find_iter == UnitCont->end() )
			{
				continue;
			}

			pUnit = (*find_iter).second;
			if( NULL == pUnit )
			{
				continue;
			}

			this->ReleaseUnit(pUnit);
		}

		pContItem->erase(gen_iter);
	}

	return true;
}


bool PgGround::RemoveAllObject()
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT *pkUnitCont;
	if ( PgObjectMgr::GetUnitContainer(UT_OBJECT, pkUnitCont) )
	{
		CUnit* pkUnit = NULL;
		PgSmallArea *pkArea = NULL;
		for ( CONT_OBJECT_MGR_UNIT::iterator it = pkUnitCont->begin(); it!=pkUnitCont->end(); ++it )
		{
			pkUnit = it->second;
			if ( pkUnit )
			{
				pkArea = GetArea( pkUnit->LastAreaIndex() );
				if( pkArea )
				{
					pkArea->RemoveUnit(pkUnit->GetID());
				}

				g_kTotalObjMgr.ReleaseUnit(pkUnit);
				pkUnit = NULL;
			}
		}

		for( ContGenPoint_Object::iterator gen_itr=m_kContGenPoint_Object.begin(); gen_itr!=m_kContGenPoint_Object.end(); ++gen_itr )
		{
			gen_itr->second.RemoveAll();
		}

		PgObjectMgr2::UnRegistAllUnit(UT_OBJECT);
		Broadcast( BM::Stream(PT_M_C_NFY_REMOVE_ALL_OBJECT) );
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::RemoveGenGroupObject(int const iGenGroupNo)	//! �� �׷��� ������Ʈ�� �����Ѵ�. 
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CONT_OBJECT_MGR_UNIT * pkUnitCont;
	if (! PgObjectMgr::GetUnitContainer(UT_OBJECT, pkUnitCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CUnit* pkUnit = NULL;
	PgSmallArea *pkArea = NULL;
	POINT3 ptTemp;

	ContGenPoint_Object kContGenPoint;
	GET_DEF_CUSTOM( PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
	if(!SUCCEEDED(kGenPointMgr.GetGenPoint_Object( kContGenPoint, GetGroundNo(), false, iGenGroupNo )) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	for(ContGenPoint_Object::iterator iter = kContGenPoint.begin();iter != kContGenPoint.end();++iter)
	{
		ContGenPoint_Object::iterator gen_iter = m_kContGenPoint_Object.find((*iter).first);
		if(gen_iter == m_kContGenPoint_Object.end())
		{
			continue;
		}

		PgGenPoint_Object & kGenPoint = (*gen_iter).second;

		OwnChildCont kContChild;
		kGenPoint.GetChild(kContChild);

		for(OwnChildCont::const_iterator child_iter = kContChild.begin();child_iter != kContChild.end();++child_iter)
		{
			BM::GUID const & kChildGuid = (*child_iter);

			CONT_OBJECT_MGR_UNIT::iterator find_iter = pkUnitCont->find(kChildGuid);
			if(find_iter == pkUnitCont->end())
			{
				continue;
			}

			pkUnit = (*find_iter).second;
			if ( !pkUnit )
			{
				continue;
			}

			pkUnit->ClearTargetList();

			::OnDamaged(NULL, pkUnit, 0, pkUnit->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());
		}

		m_kContGenPoint_Object.erase(gen_iter);
	}

	return true;
}

void PgGround::DisplayAreaState()
{
	BM::CAutoMutex kLock( m_kRscMutex );
#ifdef DEF_ESTIMATE_TICK_DELAY
	INFO_LOG(BM::LOG_LV7, __FL__<<L"TickDelayTime ["<<Tick100msDelayAverage()<<L"]");
#endif
	ContArea::iterator area_itor = m_kAreaCont.begin();

	while(m_kAreaCont.end() != area_itor)
	{
		ContArea::value_type pkArea = (*area_itor);
		if( pkArea )
		{
			pkArea->DisplayState();
		}
		++area_itor;
	}
}

bool PgGround::Clone( PgGround* pkGround )
{
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CloneResource( dynamic_cast<PgGroundResource*>(pkGround) );
	m_kGroundOwnerInfo = pkGround->m_kGroundOwnerInfo;

	if ( FAILED(BuildZone()) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"BuildZone Failed, Ground["<<GetGroundNo()<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgGround::SendToPosLoc( CUnit *pkUnit, POINT3 const &pt3Pos, BYTE const byType )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	pkUnit->SetPos( pt3Pos );

	PgSmallArea *pkTgtArea = NULL;
	POINT3 ptTemp;

	if ( SUCCEEDED( GetArea( pt3Pos, ptTemp, pkTgtArea ) ) )
	{
		PgSmallArea *pkLastArea = GetArea( pkUnit->LastAreaIndex() );
		if ( pkLastArea )
		{
			if ( pkLastArea != pkTgtArea )
			{
				//INFO_LOG( BM::LOG_LV8, _T("[%s]-[%d] UnitType[%d], Guid[%s]"), __FUNCTIONW__, __LINE__, pkUnit->UnitType(), pkUnit->GetID().str().c_str() );
				ChangeArea( pkUnit, pkTgtArea, pkLastArea, true );
			}

			SendUnitPosChange( pkUnit, byType, E_SENDTYPE_BROADALL );
			if(pkUnit->UnitType()==UT_PLAYER)
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if(pkPlayer)
				{
					pkPlayer->GetOldPlayerActionInfo()->ptPos = pt3Pos;	//�������� �����̵� ���� �ִ°���
				}
			}
			return true;
		}

		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Get Area");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;	
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Get Target Area");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;

}

bool PgGround::SendToSpawnLoc( CUnit *pkUnit, int iPotalNo, bool bRandomize, int iSpawnType )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	POINT3 pt3TempSpawnLoc;
	if ( SUCCEEDED(FindSpawnLoc( iPotalNo, pt3TempSpawnLoc, bRandomize, iSpawnType )) )
	{
		return SendToPosLoc( pkUnit, pt3TempSpawnLoc );
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Ground["<<GetGroundNo()<<L"] Can't Find Spawn location" );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::SendToSpawnLoc( BM::GUID const &rkCharGuid, int iPotalNo, bool bRandomize, int iSpawnType )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	CUnit *pkUnit = GetUnit( rkCharGuid );
	if ( !pkUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return SendToSpawnLoc( pkUnit, iPotalNo, bRandomize, iSpawnType );
}

void PgGround::SendMapLoadComplete( PgPlayer *pkUser )
{
//	���ֿ� ���� ������ �����ߵǰ�.
//	���� ���� ���� ���Դٰ� ������ϰ�.
//	����Ʈ ���� ����ϰ�.
//	������ ���� �Ϸ� ������.
	m_kStoneCtrl.TryCatchStoneTicket(pkUser);

	pkUser->ReadyNetwork(true);
	SendNfyMapLoaded(pkUser);
	RefreshGroundQuestInfo(pkUser);//���� ����Ʈ ����
	CheckEffectUser(pkUser);
	HiddenMapTimeLimit(pkUser);
	HiddenMapRewordItem(pkUser);
	//HiddenMapCheck(pkUser);
	{
		BM::PgPackedTime kFailPrevTime, kNextTime;
		PgQuestInfoUtil::MakeDayLoopQuestTime(kFailPrevTime, kNextTime);
		CheckDayLoopQuestTime(GroundKey(), pkUser, kFailPrevTime, kNextTime); // ��������Ʈ 24�ð��� ���

		PgQuestInfoUtil::MakeWeekLoopQuestTime(kFailPrevTime, kNextTime);
		CheckWeekLoopQuestTime(GroundKey(), pkUser, kFailPrevTime, kNextTime); // �ְ�����Ʈ ���
	}

	CheckJoinPartyMemberList(dynamic_cast<CUnit *>(pkUser));

	if( 0 == (GetAttr() & GATTR_EVENT_GROUND) )
	{
		DelJoinPartyMemberList(BM::GUID(pkUser->GetID()));
	}

	if( GetAttr() & GATTR_FLAG_CONSTELLATION )
	{
		Constellation_NotifyResult(pkUser, 0, m_ConstellationKey.WorldGndNo, m_ConstellationKey.Key);
	}
}

void PgGround::PartyBreakIn_CousumeKeyItem(PgPlayer * pPlayer)
{
	if( NULL == pPlayer )
	{
		return;
	}

	CONT_DEFMAP const* pDefMap = NULL;
	g_kTblDataMgr.GetContDef(pDefMap);
	if( NULL == pDefMap )
	{
		return;
	}

	CONT_DEFMAP::const_iterator iter = pDefMap->find(GroundKey().GroundNo());
	if( pDefMap->end() == iter )
	{
		return;
	}

	PgInventory * pInv = pPlayer->GetInven();
	if( NULL == pInv )
	{
		return;
	}

	bool IsHaveItem = false;
	int NeedItemNo = 0;

	CONT_NEED_KEY_ITEM::const_iterator item_iter = iter->second.ContItemNo.begin();
	while( iter->second.ContItemNo.end() != item_iter )
	{
		if( pInv->GetInvTotalCount(*item_iter) > 0 )
		{
			IsHaveItem = true;
			NeedItemNo = *item_iter;
			break;
		}
		++item_iter;
	}

	if( false == IsHaveItem )
	{
		item_iter = iter->second.ContCashItemNo.begin();
		while( iter->second.ContCashItemNo.end() != item_iter )
		{
			if( pInv->GetInvTotalCount(*item_iter) > 0 )
			{
				IsHaveItem = true;
				NeedItemNo = *item_iter;
				break;
			}
			++item_iter;
		}
	}

	if( IsHaveItem )
	{
		CONT_PLAYER_MODIFY_ORDER Order;
		SPMO IMO(IMET_ADD_ANY, pPlayer->GetID(), SPMOD_Add_Any(NeedItemNo, -1));
		Order.push_back(IMO);

		CUnit* pUnit = GetUnit(pPlayer->GetID());
		if( pUnit )
		{
			PgAction_ReqModifyItem ItemModifyAction(CIE_Constellation, GroundKey(), Order);
			ItemModifyAction.DoAction(pUnit, NULL);
		}
	}
}

bool PgGround::GetAroundVector(CUnit *pkUnit, BM::GUID const &rkTargetGuid, float const fRange, bool bNear, POINT3 &rkOut)
{
	CUnit *pkTarget = GetUnit(rkTargetGuid);
	if(!pkUnit || !pkTarget)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	POINT3 ptUnitPos = pkUnit->GetPos();
	POINT3 ptTarget = pkTarget->GetPos();
	
	NxVec3 kUnitPos(ptUnitPos.x, ptUnitPos.y, ptUnitPos.z);
	NxVec3 kTargetPos(ptTarget.x, ptTarget.y, ptTarget.z);
	
	POINT3BY kPathNormalBy = GetPathNormal(rkTargetGuid);
	NxVec3 kPathNormal(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);	// front direction
	kPathNormal.normalize();

	NxVec3 kRightVec = kPathNormal.cross(NxVec3(0,0,1));

	NxVec3 kFirst = kTargetPos + kRightVec * fRange;
	NxVec3 kSecond = kTargetPos - kRightVec * fRange;

	NxVec3 kNearTarget;
	NxVec3 kFarTarget;

	if((kFirst - kUnitPos).magnitudeSquared() > (kSecond - kUnitPos).magnitudeSquared())
	{
		kFarTarget = kFirst;
		kNearTarget = kSecond;
	}
	else
	{
		kFarTarget = kSecond;
		kNearTarget = kFirst;
	}

	if(bNear)
	{
		rkOut.Set(kNearTarget.x, kNearTarget.y, kNearTarget.z);
	}
	else
	{
		rkOut.Set(kFarTarget.x, kFarTarget.y, kFarTarget.z);
	}
	return true;
}

inline POINT3 floor(POINT3 pt)
{
	pt.x = floor(pt.x);
	pt.y = floor(pt.y);
	pt.z = floor(pt.z);
	return pt;
}

bool PgGround::GetVisionDirection(CUnit* pkUnit, NxVec3& rkDirectionOut, bool bUseTargetPos)
{
	if(!pkUnit)
	{
		return false;
	}

	if(false==bUseTargetPos)
	{
		if(pkUnit->IsUnitType(UT_PLAYER))
		{
			POINT3 kOrientedVec = pkUnit->GetDirectionVector( pkUnit->FrontDirection() );
			rkDirectionOut.set(kOrientedVec.x, kOrientedVec.y, kOrientedVec.z);
			return true;
		}
		else if(pkUnit->GetAbil(AT_MON_ANGLE_FIX))
		{
			POINT3 kOrientedVec = pkUnit->GetDirectionVector( pkUnit->FrontDirection() );
			rkDirectionOut.set(kOrientedVec.x, kOrientedVec.y, kOrientedVec.z);
			return true;
		}
	}

	POINT3 const ptCurPos = floor(pkUnit->GetPos()*100)/100;
	POINT3 ptMovingVec(0.0f ,0.0f ,0.0f);

	BM::GUID const& kTargetGuid = pkUnit->GetTarget();
	if(bUseTargetPos && false==kTargetGuid.IsNull())
	{
		POINT3 const kTargetPos = floor(GetUnitPos(kTargetGuid, true, 30.0f)*100)/100.f;
		ptMovingVec = kTargetPos - ptCurPos;
	}
	else
	{
		ptMovingVec = ptCurPos - floor(pkUnit->LastPos()*100)/100;
	}

	if(pkUnit->GetID() != kTargetGuid && ptMovingVec.x == 0.0f && ptMovingVec.y == 0.0f)	//Ÿ���� ���̸� ƨ��� �ȵ�
	{
		bool bOk = false;
		if(bUseTargetPos)
		{
			ptMovingVec = ptCurPos - pkUnit->LastPos();
			bOk = !(ptMovingVec.x == 0.0f && ptMovingVec.y == 0.0f);//��¥ �ƴϸ�?
		}

		if(!bOk)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	ptMovingVec.z = 0;
	ptMovingVec.Normalize();

	POINT3BY kPathNormalBy = pkUnit->PathNormal();
	POINT3 kPathNormal(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	kPathNormal.Normalize();

	// It must be normalized!!
	float fDotProduct = kPathNormal.x * ptMovingVec.x + kPathNormal.y * ptMovingVec.y + kPathNormal.z * ptMovingVec.z;
	if(fDotProduct < -0.9999f || fDotProduct > 0.9999f)
	{
		// Moving Vector�� Path Normal�� �����ؼ� Vision Vector�� ���� �� ����.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	POINT3 kOrientedVec(0,0,0);
	if(kPathNormal == POINT3(0, 1, 0) || kPathNormal == POINT3(0, -1, 0))
	{
		// PathNormal�� �ַ� Y, -Y�ε�, �׷��� ����� ������.
		int const iGrade = pkUnit->GetAbil(AT_GRADE);
		if(EMGRADE_UPGRADED <= iGrade && iGrade < EMGRADE_MONSTERMAX)
		{
			kOrientedVec.Set(ptMovingVec.x, ptMovingVec.y, 0.0f);
		}
		else
		{
			kOrientedVec.Set(ptMovingVec.x, 0.0f, 0.0f);
		}
	}
	else
	{
		// OrientedVector ���.
		kOrientedVec = kPathNormal.Cross(POINT3(0.0f ,0.0f ,1.0f));	// Right Vector
		POINT3 kCrossed = kPathNormal.Cross(ptMovingVec);
		kCrossed.Normalize();

		if(kCrossed == POINT3(0.0f, 0.0f, 1.0f))
		{
			kOrientedVec *= -1.0f;
		}
	}

	kOrientedVec.Normalize();
	rkDirectionOut.set(kOrientedVec.x, kOrientedVec.y, kOrientedVec.z);

	return true;
}

float PgGround::GetAdditionalAttackProb(CUnit *pkUnit)
{
	if(!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0.0f"));
		return 0.0f;
	}

	float fProbOfAttackAlpha = 0.0f;
	PgPlayer *pkTarget = dynamic_cast<PgPlayer *>(GetUnit(pkUnit->GetTarget()));
	if(pkTarget)
	{
		/// 1. �ǰ� �������� ���� �� Ȯ�� ���� (�ִ� 30% ����)
		float fDamaged = 1.0f - (float)pkTarget->GetAbil(AT_HP) / (float)pkTarget->GetAbil(AT_C_MAX_HP);
		fProbOfAttackAlpha += fDamaged * 3000.0f;

		/// 2. Ÿ���� �ڸ� ��� ���� �� �����Ϸ� �� Ȯ�� ���� (�ִ� 30% ����)
		POINT3BY kMoveDir = pkTarget->MoveDirection();
		NxVec3 kMovingDir(kMoveDir.x, kMoveDir.y, kMoveDir.z);

		NxVec3 kVisionDir;
		GetVisionDirection(pkUnit, kVisionDir);

		kMovingDir.normalize();
		float fDot = 1.0f - NiAbs(NiACos(kVisionDir.dot(kMovingDir)));
		if(fDot >= 0.0f)
		{
			// ������� ����, ���� �ְ� ���� ����.
			fProbOfAttackAlpha += fDot * 3000.0f;
		}

		/// 3. Lv�� �� ���� �� Ȯ�� ���� (�ִ� 20% ����) [ Unit : Monster, Target : Player ]
		int iLvDiff = pkUnit->GetAbil(AT_LEVEL) - pkTarget->GetAbil(AT_LEVEL);
		if(iLvDiff > 0)
		{
			fProbOfAttackAlpha += NiMin(5, iLvDiff) * 400.0f;
		}

		// Debug Log
 		//INFO_LOG(BM::LOG_LV9, _T("Hp's Prob : %.4f, Direction Prob : %.4f, Lv Prob : %.4f"), fDamaged * 3000, fDot * 3000, NiMin(5, iLvDiff) * 400.0f);
	}

	return fProbOfAttackAlpha;
}

void PgGround::SendUnitPosChange(CUnit *pkUnit, BYTE const byType, DWORD const dwSendFlag )
{
	if ( pkUnit )
	{
		BM::Stream kCPacket( PT_M_C_UNIT_POS_CHANGE, byType );
		kCPacket.Push(pkUnit->GetID());
		kCPacket.Push(pkUnit->GetPos());
		pkUnit->Send( kCPacket,E_SENDTYPE_SELF|dwSendFlag);
	}
}

void PgGround::CheckTargetList(UNIT_PTR_ARRAY& rkTargetArray)
{
	for(UNIT_PTR_ARRAY::iterator itor = rkTargetArray.begin(); itor != rkTargetArray.end(); ++itor)
	{
		if(!(*itor).bReference)
		{
			if((*itor).pkUnit)
			{
				BM::GUID const &rkGuid = (*itor).pkUnit->GetDeliverDamageUnit();
				if(!BM::GUID::IsNull(rkGuid))
				{
					//�ڱ� �ڽ��̸� �߰����� �ʴ´�.
					if((*itor).pkUnit->GetID() == rkGuid)
					{
						continue;
					}
	
					CUnit* pkUnit = PgObjectMgr::GetUnit(rkGuid);
					if(pkUnit)
					{
						rkTargetArray.Add(pkUnit, false, false, true); //���������� Ÿ�� ����Ʈ�� �־��ش�. ��¥ Ÿ���� �ƴϴ�.
					}
					else
					{
						(*itor).pkUnit->SetDeliverDamageUnit(BM::GUID::NullData());
					}
				}
			}
		}
	}
}

bool PgGround::CheckHackingAction(CUnit* pkUnit, SActionInfo& rkAction)
{
	if ( false == PgGround::ms_kAntiHackCheckVariable.bUse_MoveSpeed )
	{
		return false;
	}

	if(pkUnit->IsUnitType(UT_PLAYER))
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		
		//���������� �̵� �Ÿ� üũ
		PLAYER_ACTION_INFO const * pkOldActionInfo = pkPlayer->GetOldPlayerActionInfo();
		POINT3 kOldPos = pkOldActionInfo->ptPos;
		POINT3 kNewPos = rkAction.ptPos;

		if(pkOldActionInfo->iGroundNo != GroundKey().GroundNo()) // ���̵��̹Ƿ� üũ ���� ����
		{
			return false;
		}

		//ó������ OldPos�� 0,0,0�� �ȴ�.
		if(POINT3(0.0f, 0.0f, 0.0f) == kOldPos)
		{
			return false;
		}

		//float fZAdjust = 0.0f;
		//fZAdjust = kOldPos.z - kNewPos.z;

		//Z�� ���� ��Ų��.
		kOldPos.z = kNewPos.z;		

		//������ �׼��� ���� �ð��� ���� �ð����� ���Ѵ�
		//�ð��� �ӵ��� ���ؼ� �ִ� �̵��Ÿ��� �����Ѵ�.
		DWORD dwOldTime = pkOldActionInfo->dwLastTime;
		DWORD dwNowTime = rkAction.dwTimeStamp;

		DWORD dwElapsedTime = dwNowTime - dwOldTime;
		POINT3 kDirection = kNewPos - kOldPos;
		if(POINT3(0.0f, 0.0f, 0.0f) == kDirection)
		{
			return false;
		}

		bool bHacked = false;
		// ElapsedTime �� �ſ� ���� ��� ������ ���� ũ�� �߻��Ѵ�.
		float fBoundRate = PgGround::ms_kAntiHackCheckVariable.fMoveSpeed_BoundRate;
		// ���� ���� ���� �ӵ��� ������ ��Ŷ�� �ִ�..(����� 2~3�� ���� ��)
		// Ŭ���̾�Ʈ ������ �����Ͽ� �ذ� �ؾ� �Ѵ�.
		if (dwElapsedTime < 50)	
		{
			fBoundRate *= 2.2f;
		}
		else if (dwElapsedTime < 100)
		{
			fBoundRate *= 1.5f;
		}
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkOldSkillDef = kSkillDefMgr.GetDef(pkOldActionInfo->iActionID);
		CSkillDef const* pkOldOldSkillDef = kSkillDefMgr.GetDef(pkOldActionInfo->iOldActionID);
		CSkillDef const* pkNewSkillDef = kSkillDefMgr.GetDef(rkAction.iActionID);
		int iMoveSpeed = pkOldSkillDef ? pkOldSkillDef->GetAbil(AT_SKILL_MAXIMUM_SPEED) : 0;
		if(10100==rkAction.iActionID)	//��Ȱ ó��
		{
			iMoveSpeed = 9999999;
		}

		
		float fMoveSpeed = GetMaxActionMoveSpeed(pkPlayer, pkOldActionInfo->iActionID, iMoveSpeed); 
		float fSavedfMoveSpeed = fMoveSpeed;
		
		if(pkPlayer->GetAbil(AT_C_MOVESPEED) < fMoveSpeed)
		{
			pkUnit->SetAbil(AT_MOVESPEED_SAVED,0);
		}

		// ������ ��� ���� �̵��ӵ��� �߰� �ȴ�.
		if(GetAttr() & GATTR_VILLAGE)
		{
			fMoveSpeed += static_cast<float>(pkPlayer->GetAbil(AT_C_VILLAGE_MOVESPEED));
		}

		if(GetAttr() & GATTR_VILLAGE)
		{
			if(0 < pkPlayer->GetAbil(AT_VILLAGE_MOVESPEED_SAVED))
			{
				fMoveSpeed = fSavedfMoveSpeed + std::max(fMoveSpeed, static_cast<float>(pkPlayer->GetAbil(AT_VILLAGE_MOVESPEED_SAVED)));
			}
		}

		if(GetAttr() & GATTR_VILLAGE)
		{
			if(pkPlayer->GetAbil(AT_C_VILLAGE_MOVESPEED) < (fMoveSpeed - fSavedfMoveSpeed))
			{
				pkUnit->SetAbil(AT_VILLAGE_MOVESPEED_SAVED, 0);
			}
		}

		float const fMoveDistanceQ = kDirection.x * kDirection.x + kDirection.y * kDirection.y + kDirection.z * kDirection.z;
		float const fMoveDistanceSimulateQ = pow(dwElapsedTime / 1000.0f * fMoveSpeed * fBoundRate, 2);

		//INFO_LOG(BM::LOG_LV6, __FL__ <<" OldAction[" << pkOldActionInfo->iOldActionID<<"/"<<pkOldOldSkillDef->GetActionName()<< _COMMA_ << pkOldActionInfo->iActionID << "/" << pkOldSkillDef->GetActionName() << "] New Action[" << rkAction.iActionID << "/" << pkNewSkillDef->GetActionName() << "] moving DistanceQ[" << fMoveDistanceQ << "], ElpasedTime=" << dwElapsedTime	<< ", ReqSpeed=" << fMoveSpeed << ", UnitSpeed=" << sqrt(fMoveDistanceQ) / dwElapsedTime * 1000.0f);
			//INFO_LOG(BM::LOG_LV6, __FL__ << _T("NewPos=") << kNewPos.x << _COMMA_ << kNewPos.y << _COMMA_ << kNewPos.z);

		float fMinDistance = 100.0f;

		// �������� �߰� �̵��ӵ� ���� ��� ���� ���� ��� �ּ� ���� �� �� �÷��� üũ �ϵ��� �Ѵ�.
		if(0 < pkPlayer->GetAbil(AT_C_VILLAGE_MOVESPEED))
		{
			fMinDistance = fSavedfMoveSpeed + pkPlayer->GetAbil(AT_C_VILLAGE_MOVESPEED);
		}

		bool bElapsed1 = (1 == dwElapsedTime && fMinDistance > fMoveDistanceQ);
		if(fMoveDistanceSimulateQ < fMoveDistanceQ && !bElapsed1)
		{
			pkPlayer->SetAbil(AT_SPEED_HACK_COUNT, pkPlayer->GetAbil(AT_SPEED_HACK_COUNT)+1);	
			NiPoint3 kDeltaPosUnit(rkAction.ptPos.x-pkOldActionInfo->ptPos.x, rkAction.ptPos.y-pkOldActionInfo->ptPos.y, rkAction.ptPos.z-pkOldActionInfo->ptPos.z);
			kDeltaPosUnit.Unitize();
			NiPoint3 kAxis(kDeltaPosUnit.x,kDeltaPosUnit.y,0);
			kAxis.Unitize();
			float const fAngle = fabs(kAxis.Dot(kDeltaPosUnit));
			if(0.766f < fAngle)//�� 40������ ��簡 ���� ���
			{
				pkPlayer->SetAbil(AT_SPEED_HACK_COUNT, pkPlayer->GetAbil(AT_SPEED_HACK_COUNT)+1);	//��ŷ �ε��� �ѹ� �� ����
			}

			if(pkOldSkillDef && pkNewSkillDef)
			{
				if(PgGround::ms_kAntiHackCheckVariable.sMoveSpeed_HackingCount<=pkPlayer->GetAbil(AT_SPEED_HACK_COUNT))//��ȸ �̻�
				{
					BM::vstring kLogMsg;
					kLogMsg << __FL__ << "[HACKING][MoveSpeed] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID()
						<< "] MemberGUID[" << pkPlayer->GetMemberGUID() <<"] GroundNo[" << GroundKey().GroundNo() << "] HackingType[ActionHacking]  OldAction[" 
						<< pkOldActionInfo->iOldActionID <<"/"<<pkOldOldSkillDef->GetActionName()  << _COMMA_ << pkOldActionInfo->iActionID << "/" << pkOldSkillDef->GetActionName() << "] New Action[" 
						<< rkAction.iActionID << "/" << pkNewSkillDef->GetActionName() << "] moving DistanceQ[" << fMoveDistanceQ << "], ElpasedTime=" << dwElapsedTime
						<< ", ReqSpeed=" << fMoveSpeed << ", UnitSpeed=" << sqrt(fMoveDistanceQ) / dwElapsedTime * 1000.0f<<"\n OldPos X: "<<pkOldActionInfo->ptPos.x<<" Y: "<<pkOldActionInfo->ptPos.y<<" Z: "<<pkOldActionInfo->ptPos.z<<" NewPos X:"<<rkAction.ptPos.x<<" Y: "<<rkAction.ptPos.y<<" Z: "<<rkAction.ptPos.z<<" HackCount : "<<pkPlayer->GetAbil(AT_SPEED_HACK_COUNT)<<" Angle: "<<fAngle;
					//��ŷ �ǽ�
					//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
					HACKING_LOG(BM::LOG_LV0, kLogMsg);

					if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
					{
						pkPlayer->SendWarnMessageStr(kLogMsg);
					}
					if (pkPlayer->SuspectHacking(EAHP_MoveSpeed, PgGround::ms_kAntiHackCheckVariable.sMoveSpeed_AddIndex))
					{
						// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
						BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_MoveSpeed_Hack) );
						kDPacket.Push( pkPlayer->GetMemberGUID() );
						SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
					}
				}
			}
			else
			{
				BM::vstring kLogMsg;
				kLogMsg << __FL__ << "[HACKING][MoveSpeed] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() 
					<< "] MemberGUID[" << pkPlayer->GetMemberGUID() <<"] GroundNo[" << GroundKey().GroundNo() << "] HackingType[ActionHacking], unknown ActionID, OldAction=" 
					<< pkOldActionInfo->iOldActionID  << _COMMA_ << pkOldActionInfo->iActionID << " ActionID=" << rkAction.iActionID
					<< ", UnitSpeed=" << sqrt(fMoveDistanceQ) / dwElapsedTime * 1000.0f<<" HackCount : "<<pkPlayer->GetAbil(AT_SPEED_HACK_COUNT);
				//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg );
				HACKING_LOG(BM::LOG_LV0, kLogMsg );

				if(PgGround::ms_kAntiHackCheckVariable.sMoveSpeed_HackingCount<=pkPlayer->GetAbil(AT_SPEED_HACK_COUNT))
				{
					if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
					{
						pkPlayer->SendWarnMessageStr(kLogMsg);
					}
					if(pkPlayer->SuspectHacking(EAHP_MoveSpeed, PgGround::ms_kAntiHackCheckVariable.sMoveSpeed_AddIndex))
					{
						// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
						BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_MoveSpeed_Hack) );
						kDPacket.Push( pkPlayer->GetMemberGUID() );
						SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool PgGround::GetHaveAbilUnitArray( UNIT_PTR_ARRAY &rkUnitArray, WORD const kAbilType, EUnitType const eUnitType )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUnitType, kItor);
	while ((pkUnit = PgObjectMgr::GetNextUnit(eUnitType, kItor)) != NULL)
	{
		if ( pkUnit->GetAbil(kAbilType) > 0 )
		{
			rkUnitArray.Add(pkUnit);
		}
	}
	return rkUnitArray.size() > 0;
}

void PgGround::AddUnitArray( UNIT_PTR_ARRAY &kUnitArray, EUnitType const kUnitType )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	CUnit *pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(kUnitType, kItor);
	while ((pkUnit = PgObjectMgr::GetNextUnit(kUnitType, kItor)) != NULL)
	{
		kUnitArray.Add(pkUnit,false,false,false);
	}
}

bool PgGround::RecvRecentMapMove( PgPlayer *pkUser )
{
	// �δ����� ȣ�� �� ��� : �ֱ� ���ʵ�� �̵�
	// ���ʵ忡�� ȣ�� �� ��� : �ֱ� ������ �̵�
	// 
	if ( pkUser )
	{
		BM::CAutoMutex Lock(m_kRscMutex);
	
		if ( this->GetAttr() & GATTR_INSTANCE )
		{
			if( this->GetAttr() & GATTR_FLAG_EXPEDITION )
			{// ������ �� �ȿ��� ������ ���̶�� ������ Ż�� ��Ų��.
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUser);
				if( pPlayer )
				{
					if( BM::GUID::NullData() != pPlayer->ExpeditionGuid() )
					{
						BM::Stream LeavePacket(PT_C_N_REQ_LEAVE_EXPEDITION);
						LeavePacket.Push(pPlayer->ExpeditionGuid());
						LeavePacket.Push(pPlayer->GetID());
						SendToGlobalPartyMgr(LeavePacket);
						return false;
					}
				}
			}
			else
			{
				switch( GetState() )
				{
				case INDUN_STATE_READY:
				case INDUN_STATE_PLAY:
					{
						VEC_GUID kContGuid;
						if( GetPartyMemberGround(pkUser, GroundKey(), kContGuid, pkUser->GetID()) )
						{
							//���������� ���� �ٸ� �׷������ �޽�������
							BM::Stream kMsgData;
							kMsgData.Push(pkUser->Name());

							BM::Stream kMsgStream(PT_M_C_NFY_WARN_MESSAGE3, 8020);
							kMsgStream.Push(kMsgData.Data());
							kMsgStream.Push(EL_Normal);
							Broadcast(kMsgStream, kContGuid);
						}
					}break;
				}
			}

			SGroundKey kKey;
			pkUser->GetRecentGround( kKey, GATTR_DEFAULT );

			SReqMapMove_MT kRMM( MMET_Normal, pkUser->GetRecentPos(GATTR_DEFAULT), kKey );

			PgReqMapMove kMapMove( this, kRMM, NULL );
			if ( kMapMove.Add( pkUser ) )
			{
				return kMapMove.DoAction();
			}
		}
		else
		{
			switch( this->GetAttr() )
			{
			case GATTR_DEFAULT:
				{
					SGroundKey kKey;
					pkUser->GetRecentGround( kKey, GATTR_VILLAGE );
					SReqMapMove_MT kRMM( MMET_Normal, pkUser->GetRecentPos(GATTR_VILLAGE), kKey );

					PgReqMapMove kMapMove( this, kRMM, NULL );
					if ( kMapMove.Add( pkUser ) )
					{
						return kMapMove.DoAction();
					}
				}break;
			case GATTR_BATTLESQUARE: // (BattleSquare-�����ߴ� ������ �̵�) �ϵ��ھ���� �ڵ尡 �־, Public-Channel���� ���� �׶���� ���ư� �� TargetKey�� �ʱ�ȭ �Ǿ ������ ����� �Ѵ�
			case GATTR_HIDDEN_F:
			case GATTR_FLAG_STATIC_DUNGEON:
				{
					SGroundKey kKey;
					pkUser->GetRecentGround( kKey, GATTR_DEFAULT );

					SReqMapMove_MT kRMM( MMET_Normal, pkUser->GetRecentPos(GATTR_DEFAULT), kKey );

					PgReqMapMove kMapMove( this, kRMM, NULL );
					if ( kMapMove.Add( pkUser ) )
					{
						return kMapMove.DoAction();
					}
				}break;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::IsInReturnZone(CUnit * pkUnit)const
{
	if(!pkUnit)
	{
		return false;
	}
	
	int iZoneRange = pkUnit->GetAbil(AT_RETURNZONE_RANGE);
	CUnit* pkCaller = GetUnit(pkUnit->Caller());
	if(pkCaller)
	{
		if(0==iZoneRange)
		{
			iZoneRange = AI_MONSTER_MIN_DISTANCE_CHASE_Q;
		}

		if(POINT3::Distance(pkCaller->GetPos(), pkUnit->GetPos()) <= iZoneRange)
		{
			return true;
		}
	}

	return false;
}

bool PgGround::IsInWalkingZone(CUnit * pkUnit)const
{
	if(!pkUnit)
	{
		return false;
	}
	
	int iZoneRange = pkUnit->GetAbil(AT_WALKINGZONE_RANGE);
	CUnit* pkCaller = GetUnit(pkUnit->Caller());
	if(pkCaller)
	{
		if(0==iZoneRange)
		{
			iZoneRange = 100;
		}

		if(POINT3::Distance(pkCaller->GetPos(), pkUnit->GetPos()) <= iZoneRange)
		{
			return true;
		}
	}

	return false;
}

bool PgGround::IsInRunningZone(CUnit * pkUnit)const
{
	if(!pkUnit)
	{
		return false;
	}
	
	int iZoneRange = pkUnit->GetAbil(AT_RUNNINGZONE_RANGE);
	CUnit* pkCaller = GetUnit(pkUnit->Caller());
	if(pkCaller)
	{
		if(0==iZoneRange)
		{
			//iZoneRange = pkCaller->GetDetectRange();
			iZoneRange = 300;
		}

		if(POINT3::Distance(pkCaller->GetPos(), pkUnit->GetPos()) <= iZoneRange)
		{
			return true;
		}
	}

	return false;
}

bool PgGround::IsInBattleZone(CUnit * pkUnit)const
{
	return IsInRunningZone(pkUnit);
}

bool PgGround::IsInTriggerUnit( GTRIGGER_ID const& rkTriggerID, CUnit const* pkUnit )
{
//	BM::CAutoMutex Lock(m_kRscMutex);
	
	if( m_kContTrigger.empty() )
	{
		return false;
	}

	if( !pkUnit )
	{
		return false;
	}

	CONT_GTRIGGER::const_iterator iter = m_kContTrigger.find( rkTriggerID );
	if( m_kContTrigger.end() == iter )
	{
		return false;
	}

	CONT_GTRIGGER::mapped_type pkTrigger = (*iter).second;

	if( !pkTrigger )
	{
		return false;
	}

	if( !pkTrigger->Enable() )
	{
		return false;
	}

	if( !pkTrigger->IsInPos( pkUnit->GetPos() ) )
	{
		return false;
	}
	return true;
}

void PgGround::GetTriggerInRange( GTRIGGER_ID const& rkTriggerID, EUnitType const eUnitType, UNIT_PTR_ARRAY & rkUnitArray)const
{
	if( m_kContTrigger.empty() )
	{
		return;
	}

	CONT_GTRIGGER::const_iterator iter = m_kContTrigger.find( rkTriggerID );
	if( m_kContTrigger.end() == iter )
	{
		return;
	}

	CONT_GTRIGGER::mapped_type pkTrigger = (*iter).second;
	if( !pkTrigger )
	{
		return;
	}

	CUnit * pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUnitType, kItor);
	while ( (pkUnit = PgObjectMgr::GetNextUnit(eUnitType, kItor) ) != NULL )
	{
		if( pkTrigger->IsInPos( pkUnit->GetPos() ) )
		{
			rkUnitArray.Add(pkUnit);
		}
		++kItor;
	}
}

bool PgGround::SetTriggerEnable(GTRIGGER_ID const& rkTriggerID, bool const bNewEnable)
{
//	BM::CAutoMutex Lock(m_kRscMutex);
	if( m_kContTrigger.empty() )
	{
		return false;
	}

	CONT_GTRIGGER::const_iterator iter = m_kContTrigger.find( rkTriggerID );
	if( m_kContTrigger.end() == iter )
	{
		return false;
	}

	CONT_GTRIGGER::mapped_type pkElement = (*iter).second;
	if( pkElement
	&&	pkElement->Enable() != bNewEnable )
	{
		pkElement->Enable(bNewEnable);

		BM::Stream kNfyPacket(PT_M_C_NFY_TRIGGER_ENABLE);
		pkElement->WriteToPacket( kNfyPacket );
		Broadcast(kNfyPacket);
	}
	return true;
}

bool PgGround::RecvTriggerActionScript(std::string const& rkID)
{
	BM::Stream kNfyPacket(PT_M_C_NFY_TRIGGER_SCRIPT);
	kNfyPacket.Push( static_cast<std::string>(rkID) );	
	Broadcast(kNfyPacket);

	return true;
}


bool PgGround::RecvTriggerAction( CUnit *pkUnit, BM::Stream * const pkPacket )
{
//	BM::CAutoMutex Lock(m_kRscMutex);

	if ( m_kContTrigger.empty() )
	{
		// �߸��� ��Ż�Դϴ�.
		pkUnit->SendWarnMessage( 18994 );

		INFO_LOG( BM::LOG_LV0, __FL__<<L"ContTrigger is NULL GroundNo["<<GetGroundNo()<<L"] / User["<<pkUnit->Name()<<L"-"<<pkUnit->GetID()<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GTRIGGER_ID kTriggerID;
	pkPacket->Pop( kTriggerID );

	CONT_GTRIGGER::const_iterator itr = m_kContTrigger.find( kTriggerID );
	if ( itr == m_kContTrigger.end() )
	{
		// �߸��� ��Ż�Դϴ�.
		pkUnit->SendWarnMessage( 18994 );

		INFO_LOG( BM::LOG_LV5, __FL__<<L"NotFound Trigger["<<UNI(kTriggerID)<<L"] GroundNo["<<GetGroundNo()<<L"] / User["<<pkUnit->Name()<<L"-"<<pkUnit->GetID()<<L"]" <<"X["<<pkUnit->GetPos().x<<"] Y["<<pkUnit->GetPos().y<<"] Z["<<pkUnit->GetPos().z<<"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return itr->second->Event( pkUnit, this, pkPacket );
}

bool PgGround::GetPartyMember(BM::GUID const & rkPartyGuid, VEC_GUID& rkOut)const
{
	return m_kLocalPartyMgr.GetPartyMember(rkPartyGuid, rkOut);
}

bool PgGround::GetPartyMaster(CONT_PARTY_MASTER & rkOut)const
{
	VEC_GUID kContPartyGuid;
	m_kLocalPartyMgr.GetContPartyGuid(kContPartyGuid);

	BM::GUID kMasterGuid;
	for(VEC_GUID::const_iterator c_it = kContPartyGuid.begin(); c_it != kContPartyGuid.end(); ++c_it)
	{
		if( GetPartyMasterGuid(*c_it, kMasterGuid) )
		{
			rkOut.insert(std::make_pair(*c_it, kMasterGuid));
		}
	}

	return false == rkOut.empty();
}

bool PgGround::GetPartyMasterGuid(BM::GUID const & rkPartyGuid, BM::GUID& rkOutGuid)const
{
	return m_kLocalPartyMgr.GetPartyMasterGuid(rkPartyGuid, rkOutGuid);
}

bool PgGround::GetPartyMaxLv(BM::GUID const& rkPartyGuid, int& rkOutLevel) const
{
	return m_kLocalPartyMgr.GetPartyMaxLevel(rkPartyGuid, rkOutLevel);
}

PgPlayer const* PgGround::GetPartyMaster(PgPlayer const* pkPlayer)const
{
	if(!pkPlayer)
	{
		return NULL;
	}

	bool bResult = false;

	BM::GUID kMasterGuid;
	if( BM::GUID::IsNotNull( pkPlayer->ExpeditionGuid() ) )
	{
		bResult = m_kLocalPartyMgr.GetExpeditionMasterGuid(pkPlayer->ExpeditionGuid(), kMasterGuid);
	}
	else if( BM::GUID::IsNotNull( pkPlayer->GetPartyGuid() ) )
	{
		bResult = m_kLocalPartyMgr.GetPartyMasterGuid(pkPlayer->GetPartyGuid(), kMasterGuid);
	}

	if(bResult && pkPlayer->GetID() != kMasterGuid)
	{
		pkPlayer = dynamic_cast<PgPlayer*>(GetUnit(kMasterGuid));
	}

	return pkPlayer;
}

bool PgGround::GetPartyMemberGround(BM::GUID const & rkPartyGuid, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const & rkIgnore)const
{
	return m_kLocalPartyMgr.GetPartyMemberGround(rkPartyGuid, rkGndKey, rkOutVec, rkIgnore);
}

bool PgGround::GetPartyMemberGround(PgPlayer const* pkPlayer, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const & rkIgnore)const
{
	if(!pkPlayer)
	{
		return false;
	}

	bool bResult = false;
	
	if( BM::GUID::IsNotNull( pkPlayer->ExpeditionGuid() ) )
	{
		bResult = m_kLocalPartyMgr.GetExpeditionMemberGround(pkPlayer->ExpeditionGuid(), GroundKey(), rkOutVec, rkIgnore);
	}
	else if( BM::GUID::IsNotNull( pkPlayer->GetPartyGuid() ) )
	{
		bResult = m_kLocalPartyMgr.GetPartyMemberGround(pkPlayer->GetPartyGuid(), GroundKey(), rkOutVec, rkIgnore);
	}
	return bResult;
}

size_t PgGround::GetPartyMemberCount(BM::GUID const & rkPartyGuid)const
{
	return m_kLocalPartyMgr.GetMemberCount(rkPartyGuid);
}

//void PgGround::SendToGroundPartyPacket(SGroundKey const& rkGndKey, BM::GUID const & rkPartyGuid, VEC_GUID const& rkRecvGuidVec, BM::Stream const& rkPacket, DWORD const dwSendType) const
//{
//	return m_kLocalPartyMgr.SendToGroundPartyPacket(rkGndKey, rkPartyGuid, rkRecvGuidVec, rkPacket, dwSendType);
//}

bool PgGround::GetPartyOption(BM::GUID const & rkPartyGuid, SPartyOption& rkOut)
{
	return m_kLocalPartyMgr.GetPartyOption(rkPartyGuid, rkOut);
}

bool PgGround::GetPartyShareItem_NextOwner(BM::GUID const & rkPartyGuid, SGroundKey const& rkGndKey, BM::GUID& rkNextOwner)
{
	return m_kLocalPartyMgr.GetPartyShareItem_NextOwner(rkPartyGuid, rkGndKey, rkNextOwner);
}

int PgGround::GetPartyMemberFriend(BM::GUID const & rkPartyGuid, BM::GUID const & rkCharGuid)
{
	return m_kLocalPartyMgr.GetPartyMemberFriend(rkPartyGuid, rkCharGuid);
}

bool PgGround::SetPartyGuid( CUnit* pkUnit, BM::GUID const& rkNewGuid, EPartySystemType const kCause )
{
	PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkUnit);
	if ( pkUser )
	{
		if ( pkUser->PartyGuid() == rkNewGuid )
		{
			return false;
		}

		// ��ó��
		BM::GUID const kOldPartyGuid = pkUnit->GetPartyGuid();
		if( BM::GUID::IsNotNull(kOldPartyGuid)
		&&	BM::GUID::IsNull(rkNewGuid) )
		{
			m_kEventItemSetMgr.LeavePartyUnit(dynamic_cast< PgPlayer* >(pkUnit)); // Null Guid �Ǳ� ��
		}
		if( BM::GUID::IsNull(kOldPartyGuid)
		&&	BM::GUID::IsNotNull(rkNewGuid) )
		{
			m_kEventItemSetMgr.JoinPartyUnit(dynamic_cast< PgPlayer* >(pkUnit), rkNewGuid);
		}

#ifdef _DEBUG
		if( BM::GUID::IsNotNull( pkUser->PartyGuid() ) )
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << L"Player[" << pkUser->Name() << L"][" << pkUser->GetID() << L"] PartyGUID is Not NULL");
		}
#endif

		switch ( kCause )
		{
		case PARTY_SYS_DEFAULT:
			{
				if ( GATTR_FLAG_NOPARTY & GetAttr() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"NoParty Ground Attr[" << GetAttr() << L"] [" << pkUser->Name() << L"][" << rkNewGuid << L"]" );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				BM::Stream kNfyPacket(PT_M_C_NFY_PARTY_JOIN);
				kNfyPacket.Push(pkUser->GetID());
				kNfyPacket.Push(rkNewGuid);
				pkUser->Send( kNfyPacket, E_SENDTYPE_BROADCAST );//��Ƽ ������ BroadAll

				if ( GATTR_FLAG_HAVEPARTY & GetAttr()
					&& (0 == GATTR_FLAG_EXPEDITION & GetAttr()) )
				{
					BM::GUID kPartyMaster;
					if ( true == m_kLocalPartyMgr.GetPartyMasterGuid( pkUser->PartyGuid(), kPartyMaster ) )
					{
						if ( pkUser->GetID() == kPartyMaster )
						{
							if ( 1 < m_kLocalPartyMgr.GetMemberCount(pkUser->PartyGuid()) )
							{
								// Recent MapMove�� ��Ű������ ��ƼGUID�� ������� �Ѵ�!!!!
								pkUser->PartyGuid(rkNewGuid);	
								RecvRecentMapMove( pkUser );
								return true;// return!!!
							}
						}
						else if ( PgObjectMgr2::GetUnit( UT_PLAYER, kPartyMaster ) )
						{
							// Recent MapMove�� ��Ű������ ��ƼGUID�� ������� �Ѵ�!!!!
							pkUser->PartyGuid(rkNewGuid);					

							RecvRecentMapMove( pkUser );
							return true;// return!!!
						}
					}
				}
			}break;
		case PARTY_SYS_DELETE:
			{
				if ( GATTR_FLAG_NOPARTY & GetAttr() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"NoParty Ground Attr[" << GetAttr() << L"] [" << pkUser->Name() << L"][" << rkNewGuid << L"]" );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				if ( BM::GUID::IsNotNull(rkNewGuid) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Error Call" );
					return false;
				}

				BM::Stream kNfyPacket(PT_M_C_NFY_PARTY_JOIN);
				kNfyPacket.Push(pkUser->GetID());
				kNfyPacket.Push(rkNewGuid);
				pkUser->Send( kNfyPacket, E_SENDTYPE_BROADCAST );//��Ƽ ������ BroadAll
			}break;
		case PARTY_SYS_PVP:
			{
				if ( !(GATTR_FLAG_PVP & GetAttr() ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"No PVPParty Ground Attr[" << GetAttr() << L"] [" << pkUser->Name() << L"][" << rkNewGuid << L"]" );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Cause[" << kCause << L"] Error" );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}break;
		}

		pkUser->PartyGuid(rkNewGuid);
		return true;
	}
	return false;
}

void PgGround::SetPartyGuid(BM::GUID const& rkCharGuid, BM::GUID const& rkNewGuid, EPartySystemType const kCause)
{
	SetPartyGuid(GetUnit(rkCharGuid), rkNewGuid, kCause);
}

void PgGround::SetPartyGuid(VEC_GUID const& rkVecGuid, BM::GUID const& rkNewGuid, EPartySystemType const kCause)
{
	VEC_GUID::const_iterator guid_iter = rkVecGuid.begin();
	while( rkVecGuid.end() != guid_iter )
	{
		SetPartyGuid(GetUnit((*guid_iter)), rkNewGuid, kCause);
		++guid_iter;
	}
}

bool PgGround::GetExpeditionMember(BM::GUID const & ExpeditionGuid, VEC_GUID & Out)
{
	return m_kLocalPartyMgr.GetExpeditionMember(ExpeditionGuid, Out); 
}

bool PgGround::GetExpeditionMasterGuid(BM::GUID const & rkPartyGuid, BM::GUID& rkOutGuid)const
{
	return m_kLocalPartyMgr.GetExpeditionMasterGuid(rkPartyGuid, rkOutGuid);
}

bool PgGround::GetExpeditionMemberGround(BM::GUID const & ExpeditionGuid, SGroundKey const& GndKey, VEC_GUID & OutVec, BM::GUID const & Ignore) const
{
	return m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GndKey, OutVec, Ignore);
}

bool PgGround::AccumulatePlayerHeal(CUnit * pHealCaster, int HealValue)
{// ���� ���� ������ ��� �ϱ� ����
	if( !(GetAttr() & GATTR_EXPEDITION_GROUND) )
	{// ������ ��������
		return false;
	}
	if( NULL == pHealCaster )
	{
		return false;
	}

	BM::CAutoMutex kLock(m_kRscMutex);
	
	BM::GUID HealerGuid;
	switch( pHealCaster->UnitType() )
	{// ������ ������ ����, ���� �÷��̾� GUID�� ����
	case UT_ENTITY:		// ��ƼƼ
	case UT_SUB_PLAYER:	// ����ĳ����(�ֵ��� ����)
	case UT_SUMMONED:	// AI��ȯü(��ȯ����)
		{
			HealerGuid = pHealCaster->Caller();
		}break;
	case UT_PLAYER:		// �÷��̾�
		{
			HealerGuid = pHealCaster->GetID();
		}break;
	default:
		{
		}break;
	}
		
	CUnit * pHealer = PgObjectMgr2::GetUnit(HealerGuid);
	if( NULL == pHealer )
	{// GUID�� ������ ã��
		return false;
	}
	if( UT_PLAYER != pHealer->UnitType() )
	{// ������ �÷��̾� �̸鼭
		return false;
	}
	PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pHealer);
	if( NULL == pPlayer )
	{
		return false;
	}
	if( pPlayer->HaveExpedition() )
	{// �����뿡 ���� �ִٸ�
		pPlayer->AccumulateHeal(HealValue);	// �� ������ �����Ѵ�
		return true;
	}
	return false;
}

bool PgGround::SetExpeditionGuid( CUnit* pUnit, BM::GUID const & NewGuid, EPartySystemType const Cause )
{
	PgPlayer * pUser = dynamic_cast<PgPlayer*>(pUnit);
	if( pUser )
	{
		if ( pUser->ExpeditionGuid() == NewGuid )
		{
			return false;
		}

		switch( Cause )
		{
		case PARTY_SYS_EXPEDITION:
			{
				BM::Stream NfyPacket(PT_M_C_NFY_EXPEDITION_JOIN);
				NfyPacket.Push(pUser->GetID());
				NfyPacket.Push(NewGuid);
				pUser->Send(NfyPacket, E_SENDTYPE_BROADCAST);

				if ( GATTR_FLAG_EXPEDITION & GetAttr() )
				{
					BM::GUID ExpeditionMaster;
					if ( true == m_kLocalPartyMgr.GetExpeditionMasterGuid( pUser->ExpeditionGuid(), ExpeditionMaster ) )
					{
						if ( pUser->GetID() == ExpeditionMaster )
						{
							if ( 0 < m_kLocalPartyMgr.GetExpeditionMemberCount(pUser->ExpeditionGuid()) )
							{
								// Recent MapMove�� ��Ű������ ��ƼGUID�� ������� �Ѵ�!!!!
								pUser->ExpeditionGuid( NewGuid );
								RecvRecentMapMove( pUser );
								return true;// return!!!
							}
						}
						else if ( PgObjectMgr2::GetUnit( UT_PLAYER, ExpeditionMaster ) )
						{
							// Recent MapMove�� ��Ű������ ��ƼGUID�� ������� �Ѵ�!!!!
							pUser->ExpeditionGuid(NewGuid);
							RecvRecentMapMove( pUser );
							return true;// return!!!
						}
					}
				}
			}break;
		case PARTY_SYS_EXPEDITION_DELTE:
			{
				if ( BM::GUID::IsNotNull(NewGuid) )
				{	// �߸� ȣ����. �����밡 �����Ƿ��� GUID�� ����ߵ�.
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Error Call" );
					return false;
				}
				
				BM::Stream NfyPacket(PT_M_C_NFY_EXPEDITION_JOIN);
				NfyPacket.Push(pUser->GetID());
				NfyPacket.Push(NewGuid);
				pUser->Send(NfyPacket, E_SENDTYPE_BROADCAST);

				if( GATTR_FLAG_EXPEDITION & GetAttr() )
				{
					pUser->ExpeditionGuid( NewGuid );
					RecvRecentMapMove( pUser );
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Cause[" << Cause << L"] Error" );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			}break;
		}

		pUser->ExpeditionGuid(NewGuid);
		return true;
	}

	return false;
}

void PgGround::SetExpeditionGuid(BM::GUID const & CharGuid, BM::GUID const & NewGuid, EPartySystemType const Cause)
{
	SetExpeditionGuid(GetUnit(CharGuid), NewGuid, Cause);
}

void PgGround::SetExpeditionGuid(VEC_GUID const& VecGuid, BM::GUID const& NewGuid, EPartySystemType const Cause)
{
	VEC_GUID::const_iterator guid_iter = VecGuid.begin();
	while( VecGuid.end() != guid_iter )
	{
		SetExpeditionGuid(GetUnit((*guid_iter)), NewGuid, Cause);
		++guid_iter;
	}
}

void PgGround::GetUnitTargetList(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit)
{
	if(pkUnit)
	{
		if( ESTARGET_SELF & iTargetType )
		{
			rkUnitArray.AddToUniqe(pkUnit);
		}

		if((ESTARGET_CASTER & iTargetType) && pkUnit->IsHaveCaller())
		{
			BM::CAutoMutex kLock( m_kRscMutex );
			PgGroundUtil::GetUnitInRange(pkUnit, GetUnit(pkUnit->Caller()), pkUnit->GetPos(), rkUnitArray, iRange, iZLimit);
		}

		int const iTeam = pkUnit->GetAbil(AT_TEAM);
		switch(pkUnit->UnitType())
		{
		case UT_PET:
		case UT_PLAYER:
		case UT_MONSTER:
		case UT_ENTITY:
		case UT_SUMMONED:
			{
				if( iTeam )//���������� ���Ͱ� ���� ���� ��쿡 ���� ó���� �ʿ�. �÷��̾�� ���� �� �ٸ� üũ�ؾ� ��
				{
					FindUnit_WhenJoinedTeam( pkUnit, rkUnitArray, iTargetType, iRange, iZLimit, iTeam , (UT_PLAYER == pkUnit->UnitType()) );
				}
				else
				{
					FindUnit_WhenNoneJoinedTeam( pkUnit, rkUnitArray, iTargetType, iRange, iZLimit );
				}
				FindUnit_DuelTarget(pkUnit, rkUnitArray, iTargetType, iRange, iZLimit);
			}break;
		}
	}
	
	if(ESTARGET_HIDDEN & iTargetType)
	{// ��ư�� Ÿ���̸�, ������ �ƴ� ���ֵ��� ����
		RemoveNoneHiddenUnit(pkUnit, rkUnitArray, iRange, iZLimit);
	}
	if(ESTARGET_DEAD & iTargetType)
	{// ���� ĳ���� Ÿ���̸�, ����ִ� ���ֵ��� ����
		RemoveAliveUnit(pkUnit, rkUnitArray, iRange, iZLimit);
	}
}

void PgGround::FindUnit_WhenJoinedTeam(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit, int const iTeam, bool const bIfNoneRangeThenAddUnit )
{// ���� �����Ұ��( ���� ���Ͱ� ���� ��������� ó���� �����Ǿ����� ����)
	if( !pkUnit )
	{
		return;
	}	
	if( !iTeam )//���������� ���Ͱ� ���� ���� ��쿡 ���� ó���� �ʿ�. �÷��̾�� ���� �Ѵ� üũ�ؾ� ��(���� ���Ͱ� iTeam�� ������ ���� ����)
	{
		return;
	}
	EUnitType const eUnitType = pkUnit->UnitType() == UT_ENTITY ? UT_PLAYER : pkUnit->UnitType();

	BM::CAutoMutex kLock( m_kRscMutex );
	if ( ESTARGET_ENEMY & iTargetType )
	{
		CUnit* pkOtherUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
		while ( (pkOtherUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor) ) != NULL )
		{// �÷��̾�� �߰��ϰ�
			if ( pkOtherUnit->GetAbil(AT_TEAM) != iTeam )
			{
				if(0 < iRange)
				{
					POINT3 ptTargetPos = GetUnitPos(pkOtherUnit);
					if(ERange_OK==::IsInRange( pkUnit->GetPos(), ptTargetPos, iRange, iZLimit) )
					{
						rkUnitArray.AddToUniqe(pkOtherUnit);
					}
				}
				else if( bIfNoneRangeThenAddUnit )
				{
					rkUnitArray.AddToUniqe(pkOtherUnit);
				}
			}
		}
		if(UT_MONSTER != eUnitType)
		{// �����ڰ� ���Ͱ� �ƴ϶��, �ǰ� ����� ���͵� ���Եȴ�
			GetUnitInRange(pkUnit->GetPos(), iRange, UT_MONSTER, rkUnitArray, iZLimit);
		}
	}
	else if ( (ESTARGET_PARTY|ESTARGET_ALLIES) & iTargetType )
	{
		CUnit* pkOtherUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(eUnitType, kItor);
		while ( (pkOtherUnit = PgObjectMgr::GetNextUnit(eUnitType, kItor) ) != NULL )
		{
			if ( pkOtherUnit->GetAbil(AT_TEAM) == iTeam )
			{
				if(0 < iRange)
				{
					POINT3 ptTargetPos = GetUnitPos(pkOtherUnit);
					if(ERange_OK==::IsInRange( pkUnit->GetPos(), ptTargetPos, iRange, iZLimit) )
					{
						rkUnitArray.AddToUniqe(pkOtherUnit);
					}
				}
				else if( bIfNoneRangeThenAddUnit )
				{
					rkUnitArray.AddToUniqe(pkOtherUnit);
				}
			}
		}
	}
	/*
	if ( ESTARGET_GUILD &usTargetType )
	{
	}
	if(ESTARGET_FRIEND & usTargetType)
	{
	}
	if(ESTARGET_COUPLE & usTargetType)
	{
	}
	*/
}

void PgGround::FindUnit_WhenNoneJoinedTeam(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit)
{
	if( !pkUnit )
	{
		return;
	}
	CUnit* pkOwnerUnit = NULL;

	BM::GUID kGuildGuid;
	BM::GUID kPartyGuid;
	BM::GUID kCoupleGuid;
	if( pkUnit->IsInUnitType(static_cast<EUnitType>(UT_ENTITY|UT_SUMMONED))
		&& BM::GUID::IsNotNull(pkUnit->Caller())
		)
	{// ��ȯü�ϰ��
		if(pkOwnerUnit = GetUnit(pkUnit->Caller()))
		{
			kGuildGuid = pkOwnerUnit->GetCoupleGuid();
			kPartyGuid = pkOwnerUnit->GetPartyGuid();
			kCoupleGuid = pkOwnerUnit->GetCoupleGuid();
		}
	}
	else
	{// ��ȯü�� �ƴҰ��
		pkOwnerUnit = pkUnit;
		kGuildGuid = pkUnit->GetCoupleGuid();
		kPartyGuid = pkUnit->GetPartyGuid();
		kCoupleGuid = pkUnit->GetCoupleGuid();
	}

	if( (ESTARGET_ENEMY | ESTARGET_ALLIES) & iTargetType )
	{
		EUnitType eType = (pkUnit->UnitType() == UT_MONSTER) ? UT_PLAYER : UT_MONSTER;
		if(ESTARGET_ALLIES & iTargetType)
		{
			eType = (pkUnit->UnitType() == UT_MONSTER) ? UT_MONSTER : UT_PLAYER;
		}

		GetUnitInRange(pkUnit->GetPos(), iRange, eType, rkUnitArray, iZLimit);
	}

	BM::CAutoMutex kLock( m_kRscMutex );
	if(ESTARGET_PARTY & iTargetType)
	{
		if(BM::GUID::IsNotNull(kPartyGuid))
		{
			VEC_GUID kPartyList;
			GetPartyMember(kPartyGuid, kPartyList);

			VEC_GUID::iterator itor = kPartyList.begin();
			while(kPartyList.end() != itor)
			{
				//��Ƽ�� ���� ���Ե��� �ʴ´�.  Self�� ���� ����.
				PgGroundUtil::GetUnitInRange(pkOwnerUnit, GetUnit(*itor), pkUnit->GetPos(), rkUnitArray, iRange, iZLimit);
				++itor;
			}
		}
	}
	//if(ESTARGET_LOCATION & usTargetType)
	//{
	//	//���� ������ ����
	//}
	//if(ESTARGET_FRIEND & usTargetType)
	//{
	//	//ģ���� ���� MapServer���� Ÿ���� ���� �� ����.
	//}
	if(ESTARGET_COUPLE & iTargetType)
	{
		if(BM::GUID::IsNotNull(kCoupleGuid))
		{
			//Ŀ�ÿ� ���� ���Ե��� �ʴ´�.  Self�� ���� ����.
			PgGroundUtil::GetUnitInRange(pkOwnerUnit, GetUnit(kCoupleGuid), pkUnit->GetPos(), rkUnitArray, iRange, iZLimit);
		}
	}
	if(ESTARGET_GUILD & iTargetType)
	{	
		UNIT_PTR_ARRAY kTempList;

		GetUnitInRange(pkUnit->GetPos(), iRange, UT_PLAYER, kTempList, iZLimit);

		UNIT_PTR_ARRAY::iterator itor = kTempList.begin();
		while(kTempList.end() != itor)
		{
			CUnit* pkTarget = (*itor).pkUnit;
			if(pkTarget)
			{
				if(pkTarget->GetGuildGuid() == kGuildGuid)
				{
					//��忡 ���� ���Ե��� �ʴ´�. Self�� ���� ����.
					rkUnitArray.AddToUniqe(pkTarget);
				}
			}
			++itor;
		}
	}
}

void PgGround::FindUnit_DuelTarget(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit)
{
	// Ground ��ü���� ã�´�.
	BM::CAutoMutex kLock(m_kRscMutex);
	int const iZ = (iZLimit == 0) ? iRange : iZLimit;

	FindUnit_DuelTarget(UT_PLAYER, pkUnit, rkUnitArray, iTargetType, iRange, iZ);
	FindUnit_DuelTarget(UT_SUMMONED, pkUnit, rkUnitArray, iTargetType, iRange, iZ);
	FindUnit_DuelTarget(UT_SUB_PLAYER, pkUnit, rkUnitArray, iTargetType, iRange, iZ);
}

void PgGround::FindUnit_DuelTarget(EUnitType const eUnitType, CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit)
{
	if(NULL == pkUnit)
	{
		return;
	}

	CUnit* pkTempUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUnitType, kItor);
	while ((pkTempUnit = PgObjectMgr::GetNextUnit(eUnitType, kItor)) != NULL)
	{
		CUnit* pRetUnit = IsUnitInRangeImp(pkTempUnit, pkUnit->GetPos(), iRange, UT_PLAYER, iZLimit, true);
		if(NULL == pRetUnit) { continue; }
		if( false == (iTargetType & ESTARGET_SELF) && pRetUnit == pkUnit)
		{
			continue;
		}

		if(pkUnit->GetAbil(AT_DUEL) > 0 && pkUnit->GetAbil(AT_DUEL) == pRetUnit->GetAbil(AT_DUEL) )
		{
			rkUnitArray.AddToUniqe(pRetUnit);
		}
	}
}


void PgGround::RemoveNoneHiddenUnit(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iRange, int const iZLimit)
{// ������ �ƴ� ���ֵ��� ����
	UNIT_PTR_ARRAY::iterator itor = rkUnitArray.begin();
	while(rkUnitArray.end() != itor)
	{
		if((*itor).pkUnit)
		{
			// ������ �ƴ� ��� ����
			if(!((0 < (*itor).pkUnit->GetAbil(AT_HIDE)) || (0 < (*itor).pkUnit->GetAbil(AT_UNIT_HIDDEN))))
			{
				itor = rkUnitArray.erase(itor);
			}
			else
			{
				++itor;
			}
		}
		else
		{
			++itor;
		}
	}
}

void PgGround::RemoveAliveUnit(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iRange, int const iZLimit)
{// ����ִ� ���ֵ��� ����
	UNIT_PTR_ARRAY::iterator itor = rkUnitArray.begin();
	while(rkUnitArray.end() != itor)
	{
		if((*itor).pkUnit)
		{//��� �ִٸ� ����
			if((*itor).pkUnit->IsAlive())
			{
				itor = rkUnitArray.erase(itor);
			}
			else
			{
				++itor;
			}
		}
		else
		{
			++itor;
		}
	}
}

void PgGround::GetSummonUnitArray( CUnit *pkUnit, UNIT_PTR_ARRAY& rkUnitArray )
{
	int iSummonIndex = 0;
	BM::GUID kSummonGuid;
	while ( (kSummonGuid = pkUnit->GetSummonUnit(iSummonIndex)) != BM::GUID::NullData() )
	{
		CUnit* pkSummonUnit = GetUnit(kSummonGuid);
		if ( pkSummonUnit )
		{
			rkUnitArray.Add( pkSummonUnit );
			++iSummonIndex;
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Critical Error!! CallerID<") << pkUnit->GetID() << _T("> SummonGuid<") << kSummonGuid << _T(">") );
			pkUnit->DeleteSummonUnit( kSummonGuid );
			// �ε����� �������� �ʴ´�.
		}
	}
}

void PgGround::DoWorldEventCondition(int const iGroundNo, SWorldEventCondition const& rkCondition, PgWorldEvent const& rkWorldEvent, CUnit const* pkCaster)
{
	using namespace WorldEventTarget;
	using namespace WorldEventCondition;

	bool bProcessed = true;
	switch( rkCondition.TargetType() )
	{
	case WETT_UnitGoalTrigger:
		{
			switch( rkCondition.ConditionType() )
			{
			case CT_Enter:
				{
					if( !pkCaster )
					{
						bProcessed = false;
						break;
					}

					POINT3 kTargetPos;
					if( S_OK != PgGround::FindTriggerLoc(MB(rkCondition.TargetID()), kTargetPos, true) )
					{
						INFO_LOG(BM::LOG_LV1, __FL__ << "[UNIT_GOAL_TARGET] Can't find object in gas [" << rkCondition.TargetID() << "]");
						bProcessed = false;
						break;
					}

					const POINT3 kPos = pkCaster->GetPos();
					const bool bStatus = POINT3::SqrDistance(kTargetPos, kPos) <= 0.447/*0.2 ^ 2*/;
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_AliveUnit:
		{
			bool bStatus = true;
			if( rkCondition.TargetID() == BM::vstring(_T("ALL_PLAYER")) )
			{
				if( 1 == rkCondition.ConditionValueAsInt() )
				{
					CUnit * pPlayer = NULL;

					CONT_OBJECT_MGR_UNIT::iterator kItor;
					PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
					while ((pPlayer = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
					{
						if ( pPlayer->IsAlive() == false )
						{
							bStatus = false;
						}
					}	
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}
				else
				{
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}
			}
			else
			{
				bProcessed = false;
			}
		}break;
	case WETT_UnitLevel:
		{
			CUnit * FirstUnit = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			FirstUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor);

			PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(FirstUnit);
			int const LevelLimit = rkCondition.ConditionValueAsInt();

			switch( rkCondition.ConditionType() )
			{
			case CT_Interval:
				{
					bool bStatus = false;
					if( pPlayer )
					{
						VEC_WSTRING ValueVector;
						BM::vstring value = rkCondition.ConditionValue();
						PgStringUtil::BreakSep(std::wstring(value), ValueVector, _T("/"));

						if( ValueVector.size() == 2 )
						{
							int const MinLevel = PgStringUtil::SafeAtoi(ValueVector[0]);
							int const MaxLevel = PgStringUtil::SafeAtoi(ValueVector[1]);

							int MasterLevel = 0;

							if( pPlayer->HaveExpedition() )
							{
								BM::GUID MasterGuid;
								m_kLocalPartyMgr.GetExpeditionMasterGuid(pPlayer->ExpeditionGuid(), MasterGuid);
								PgPlayer * pMaster = GetUser(MasterGuid);
								if( pMaster )
								{
									MasterLevel = pMaster->GetAbil(AT_LEVEL);									
								}
								else
								{
									MasterLevel = pPlayer->GetAbil(AT_LEVEL);	
								}
							}
							else if( pPlayer->HaveParty() )
							{
								BM::GUID MasterGuid;
								m_kLocalPartyMgr.GetPartyMasterGuid(pPlayer->PartyGuid(), MasterGuid);
								PgPlayer * pMaster = GetUser(MasterGuid);
								if( pMaster )
								{
									MasterLevel = pMaster->GetAbil(AT_LEVEL);
								}
								else
								{
									MasterLevel = pPlayer->GetAbil(AT_LEVEL);	
								}
							}							
							else
							{
								MasterLevel = pPlayer->GetAbil(AT_LEVEL);								
							}

							if( (MinLevel <= MasterLevel) &&  (MasterLevel <= MaxLevel) )
							{
								bStatus = true;
							}
						}
					}			
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}break;
			case CT_Over:
				{
					bool bStatus = true;
					if( pPlayer )
					{
						if( pPlayer->HaveExpedition() )
						{
							BM::GUID MasterGuid;
							m_kLocalPartyMgr.GetExpeditionMasterGuid(pPlayer->ExpeditionGuid(), MasterGuid);
							PgPlayer * pMaster = GetUser(MasterGuid);
							if( pMaster )
							{
								if( pMaster->GetAbil(AT_LEVEL) < LevelLimit )
								{
									bStatus = false;
								}
							}
						}
						else if( pPlayer->HaveParty() )
						{
							BM::GUID MasterGuid;
							m_kLocalPartyMgr.GetPartyMasterGuid(pPlayer->PartyGuid(), MasterGuid);
							PgPlayer * pMaster = GetUser(MasterGuid);
							if( pMaster )
							{
								if( pMaster->GetAbil(AT_LEVEL) < LevelLimit )
								{
									bStatus = false;
								}
							}
						}							
						else
						{
							if( pPlayer->GetAbil(AT_LEVEL) < LevelLimit )
							{
								bStatus = false;
							}
						}
					}
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}break;
			}
		}break;
	case WETT_Trigger:
		{
			switch( rkCondition.ConditionType() )
			{
			case CT_Enter:
			case CT_Action:
				{
					if( pkCaster )
					{
						VEC_GUID kContGuid;
						if( GetPartyMember(pkCaster->GetPartyGuid(), kContGuid) )
						{
							VEC_GUID::const_iterator iter_guid = kContGuid.begin();
							bool bStatus = false;
							while( kContGuid.end() != iter_guid )
							{
								CUnit* pPlayer = GetUnit(*iter_guid);
								if( pPlayer )
								{
									bStatus = IsInTriggerUnit(MB(rkCondition.TargetID()), pPlayer);
									if( bStatus )
									{
										SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
										break;
									}
								}
								++iter_guid;
							}
						}
						else
						{
							bool const bStatus = IsInTriggerUnit(MB(rkCondition.TargetID()), pkCaster);
							SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
						}
					}
				}break;
			case CT_Leave:
				{
					if( pkCaster )
					{
						VEC_GUID kContGuid;
						if( GetPartyMember(pkCaster->GetPartyGuid(), kContGuid) )
						{
							VEC_GUID::const_iterator iter_guid = kContGuid.begin();
							bool bStatus = false;
							while( kContGuid.end() != iter_guid )
							{
								CUnit* pPlayer = GetUnit(*iter_guid);
								if( pPlayer )
								{
									bStatus = IsInTriggerUnit(MB(rkCondition.TargetID()), pPlayer);
									if( !bStatus )
									{//�������������ϱ�....
										SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
										break;
									}
								}
								++iter_guid;
							}
						}
						else
						{
							bool const bStatus = IsInTriggerUnit(MB(rkCondition.TargetID()), pkCaster);
							SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
						}
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_WEClientObject:
		{
			WORD const kWEClientObjectID = static_cast< WORD >(rkCondition.TargetIDAsDWORD());
			PgWEClientObjectState const* pkWEClientObjectState = NULL;
			bProcessed = PgWEClientObjectServerMgr::GetWEClientObjectState(kWEClientObjectID, pkWEClientObjectState);

			if( !bProcessed )
			{
				break;
			};

			switch( rkCondition.ConditionType() )
			{
			case CT_Status:
				{
					bool const bStatus = rkCondition.ConditionValueAsInt() == pkWEClientObjectState->State();
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_Item:
		{
			if( pkCaster )
			{
				size_t iTotalItemCount = 0;

				PgInventory const* pkInven = pkCaster->GetInven();
				if( pkInven )
				{
					iTotalItemCount = pkInven->GetTotalCount(rkCondition.TargetIDAsInt());
				}
				
				bool bStatus = false;
				switch( rkCondition.ConditionType() )
				{
				case CT_Equal:
					{
						bStatus = rkCondition.ConditionValueAsDWORD() == iTotalItemCount;
					}break;
				case CT_Have:
					{
						bStatus = rkCondition.ConditionValueAsDWORD() <= iTotalItemCount;
					}break;
				case CT_Over:
					{
						bStatus = rkCondition.ConditionValueAsDWORD() < iTotalItemCount;
					}break;
				case CT_Less:
					{
						bStatus = rkCondition.ConditionValueAsDWORD() > iTotalItemCount;
					}break;
				default:
					{
						bProcessed = false;
					}break;
				}

				SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, (bProcessed && bStatus));
			}
			else
			{
				bProcessed = true; // Unit�� ���� ��쵵 �ִ�.
			}
		}break;
	case WETT_Quest:
		{
			if( pkCaster && pkCaster->IsUnitType(UT_PLAYER) )
			{
				PgPlayer const* pkPlayer = dynamic_cast< PgPlayer const* >(pkCaster);
				if(rkCondition.TargetID() == "PARTY_MASTER")
				{
					if(PgPlayer const* pkMaster = GetPartyMaster(pkPlayer))
					{
						pkPlayer = pkMaster;
					}
				}

				if( pkPlayer )
				{
					PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
					bool bStatus = false;
					switch( rkCondition.ConditionType() )
					{
					case CT_Ing:
						{
							bStatus = pkMyQuest->IsIngQuest( rkCondition.ConditionValueAsInt() );
						}break;
					case CT_End:
						{
							bStatus = pkMyQuest->IsEndedQuest( rkCondition.ConditionValueAsInt() );
						}break;
					default:
						{
							bProcessed = false;
						}break;
					}
					SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
				}
				else
				{
					CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Ground: " << iGroundNo << L" this Unit[G:" << pkCaster->GetID() << L" N:" << pkCaster->Name() << L"] not a Player");
				}
			}
			else
			{
				 bProcessed = true; // Unit�� ���� ��쵵 �ִ�.
			}
		}break;
	case WETT_MonEffect:
		{
			BM::CAutoMutex kLock(m_kRscMutex);

			int const iMonNo = rkCondition.TargetIDAsInt();
			int const iEffectNo = rkCondition.ConditionValueAsInt();

			if( pkCaster && pkCaster->GetAbil(AT_CLASS)==iMonNo )
			{
				switch( rkCondition.ConditionType() )
				{
				case CT_Equal:
					{
						CEffect const* pkEffect = pkCaster->FindEffect(iEffectNo);
						bool const bStatus = pkEffect ? (pkEffect && false==pkEffect->IsDelete()) : false;
						SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
						bProcessed = true;
					}break;
				}
			}
		}break;
	case WETT_MonRegenPoint:
	case WETT_ObjectRegenPoint:
	case WETT_MonRegenPointGroup:
	case WETT_ObjectRegenPointGroup:
		{
			BM::CAutoMutex kLock(m_kRscMutex);
			size_t iRemainCount = 0;
			BM::GUID const targetGuid = rkCondition.TargetIDAsGuid();
			int const targetID = rkCondition.TargetIDAsInt();
			switch( rkCondition.TargetType() )
			{
			case WETT_MonRegenPoint:
				{
					bProcessed = PgWorldEventMgr::IsMonsterGenPointCount( targetGuid );
					if( bProcessed )
					{
						iRemainCount = PgWorldEventMgr::GetMonsterGenPointCount( targetGuid );
					}
				}break;
			case WETT_MonRegenPointGroup:
				{
					bProcessed = PgWorldEventMgr::IsMonsterGenPointGroupCount( targetID );
					if( bProcessed )
					{
						iRemainCount = PgWorldEventMgr::GetMonsterGenPointGroupCount( targetID );
					}
				}break;
			case WETT_ObjectRegenPoint:
				{
					bProcessed = PgWorldEventMgr::IsObjectGenPointCount( targetGuid );
					if( bProcessed )
					{
						iRemainCount = PgWorldEventMgr::GetObjectGenPointCount( targetGuid );
					}
				}break;
			case WETT_ObjectRegenPointGroup:
				{
					bProcessed = PgWorldEventMgr::IsObjectGenPointGroupCount( targetID );
					if( bProcessed )
					{
						iRemainCount = PgWorldEventMgr::GetObjectGenPointGroupCount( targetID );
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}

			bool bStatus = false;
			switch( rkCondition.ConditionType() )
			{
			case CT_Equal:
				{
					bStatus = rkCondition.ConditionValueAsDWORD() == iRemainCount;
				}break;
			case CT_Have:
				{
					bStatus = rkCondition.ConditionValueAsDWORD() <= iRemainCount;
				}break;
			case CT_Over:
				{
					bStatus = rkCondition.ConditionValueAsDWORD() < iRemainCount;
				}break;
			case CT_Less:
				{
					bStatus = rkCondition.ConditionValueAsDWORD() > iRemainCount;
				}break;
			//case CT_ActAfterTime:
			//	{
			//	}break;
			default:
				{
					bProcessed = false;
				}break;
			}

			SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, (bProcessed && bStatus));
		}break;
	case WETT_IndunState:
		{
			bProcessed = true;

			if( this->GetAttr() & GATTR_INSTANCE )
			if( PgIndun * pkIndun = dynamic_cast<PgIndun*>(this) )
			{
				switch( rkCondition.ConditionType() )
				{
				case CT_Equal:
					{
						bool const bStatus = pkIndun->GetState()==rkCondition.ConditionValueAsInt();
						SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
					}break;
				}
			}
		}break;
	case WETT_EventGroundState:
		{
			bProcessed = true;

			if( this->GetAttr() == GATTR_RACE_GROUND )
			{
				PgStaticRaceGround * pStaticRaceGround = dynamic_cast<PgStaticRaceGround *>(this);
				if( pStaticRaceGround )
				{
					switch( rkCondition.ConditionType() )
					{
					case CT_Equal:
						{
							bool const bStatus = pStaticRaceGround->GetEventGroundState() == rkCondition.ConditionValueAsInt();
							SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
						}break;
					}
				}
			}
			else if( this->GetAttr() == GATTR_EVENT_GROUND )
			{
				PgStaticEventGround * pStaticEventGround = dynamic_cast<PgStaticEventGround *>(this);
				if( pStaticEventGround )
				{
					switch( rkCondition.ConditionType() )
					{
					case CT_Equal:
						{
							bool const bStatus = pStaticEventGround->GetEventGroundState() == rkCondition.ConditionValueAsInt();
							SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
						}break;
					}
				}
			}
		}break;
	default:
		{
			bProcessed = false;
		}break;
	}

	if( !bProcessed )
	{
		PgWorldEventMgr::DoWorldEventCondition(iGroundNo, rkCondition, rkWorldEvent, pkCaster);
	}
}

void PgGround::DoWorldEventAction(int const iGroundNo, SWorldEventAction const& rkAction, PgWorldEvent const& rkWorldEvent)
{
	using namespace WorldEventTarget;
	using namespace WorldEventAction;

	bool bProcessed = true;
	switch( rkAction.TargetType() )
	{
	case WETT_Trigger:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Enable:
				{
					bProcessed = SetTriggerEnable(MB(rkAction.TargetID()), rkAction.ActionValueAsBool());
				}break;
			//case AT_Status:
			//case AT_Show:
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_WEClientObject:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Status:
				{
					bProcessed = SetWEClientObjectState( static_cast< WORD >(rkAction.TargetIDAsDWORD()), rkAction.ActionValueAsInt() );
				}break;
			case AT_Show:
				{
					bProcessed = SetWEClientObjectShow( static_cast< WORD >(rkAction.TargetIDAsDWORD()), rkAction.ActionValueAsBool() );
				}break;
			case AT_Script:
				{
					bProcessed = RecvTriggerActionScript(MB(rkAction.TargetID()));
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_MonRegenPoint:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Active:
				{
					BM::CAutoMutex kLock(m_kRscMutex);

					int iMonNo = 0;
					BM::GUID kMonGuid;

					ContGenPoint_Monster::iterator find_iter = m_kContGenPoint_Monster.find( rkAction.TargetIDAsGuid() );
					bProcessed = m_kContGenPoint_Monster.end() != find_iter;
					if( bProcessed )
					{
						ContGenPoint_Monster::mapped_type& rkGenPoint = (*find_iter).second;
						int iTargetCount = rkAction.ActionValueAsInt();
						while( 0 < iTargetCount )
						{
							MonsterGenerate(rkGenPoint, iMonNo, kMonGuid);
							--iTargetCount;
						}
					}
				}break;
			case AT_Remove:
				{
					if( rkAction.TargetIDAsGuid().IsNull() )
					{
						RemoveAllMonster(true);
					}
					else
					{
						BM::CAutoMutex kLock(m_kRscMutex);
						ContGenPoint_Monster::iterator find_iter = m_kContGenPoint_Monster.find( rkAction.TargetIDAsGuid() );
						bProcessed = m_kContGenPoint_Monster.end() != find_iter;
						if( bProcessed )
						{
							ContGenPoint_Monster::mapped_type& rkGenPoint = (*find_iter).second;
							OwnChildCont kContChild;
							rkGenPoint.GetChild(kContChild);
							OwnChildCont::const_iterator mon_iter = kContChild.begin();
							while( kContChild.end() != mon_iter )
							{
								CUnit* pkMonster = GetUnit((*mon_iter));
								if( pkMonster )
								{
									PgAggroMeter* pkMeter = NULL;
									if( GetMeter(pkMonster->GetID(), pkMeter) )
									{
										pkMeter->Clear(true); // ����ġ ����
									}
									pkMonster->ClearTargetList();
									::OnDamaged(NULL, pkMonster, 0, pkMonster->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime()); // �׾��
								}
								++mon_iter;
							}
						}
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}

			if( bProcessed )
			{
			}
		}break;
	case WETT_MonRegenPointGroup:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Active:
				{
					BM::CAutoMutex kLock(m_kRscMutex);

					int iMonNo = 0;
					BM::GUID kMonGuid;

					bProcessed = true;
					int iTargetCount = rkAction.ActionValueAsInt();
					ActivateMonsterGenGroup( rkAction.TargetIDAsInt(), false, true, iTargetCount-1 );

					int const iCount = GetMonRegenPointGroupTotalCount();
					Broadcast( BM::Stream(PT_M_C_ANS_GENMONSTER_GROUP_NUM, iCount) );
				}break;
			case AT_Remove:
				{
					BM::CAutoMutex kLock(m_kRscMutex);

					bProcessed = true;
					if( bProcessed )
					{
						ContGenPoint_Monster::iterator find_iter = m_kContGenPoint_Monster.begin();
						while( m_kContGenPoint_Monster.end() != find_iter )
						{
							ContGenPoint_Monster::mapped_type& rkGenPoint = (*find_iter).second;
							if( rkAction.TargetIDAsInt() == rkGenPoint.Info().iPointGroup )
							{
								OwnChildCont kContChild;
								rkGenPoint.GetChild(kContChild);
								OwnChildCont::const_iterator mon_iter = kContChild.begin();
								while( kContChild.end() != mon_iter )
								{
									CUnit* pkMonster = GetUnit((*mon_iter));
									if( pkMonster )
									{
										PgAggroMeter* pkMeter = NULL;
										if( GetMeter(pkMonster->GetID(), pkMeter) )
										{
											pkMeter->Clear(true); // ����ġ ����
										}
										pkMonster->ClearTargetList();
										::OnDamaged(NULL, pkMonster, 0, pkMonster->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime()); // �׾��
									}
									++mon_iter;
								}
							}
							++find_iter;
						}
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}

			if( bProcessed )
			{
			}
		}break;
	case WETT_ObjectRegenPoint:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Active:
				{
					BM::CAutoMutex kLock(m_kRscMutex);

					ContGenPoint_Object::iterator find_iter = m_kContGenPoint_Object.find( rkAction.TargetIDAsGuid() );
					bProcessed = m_kContGenPoint_Object.end() != find_iter;
					if( bProcessed )
					{
						ContGenPoint_Object::mapped_type& rkGenPoint = (*find_iter).second;
						int iTargetCount = rkAction.ActionValueAsInt();
						while( 0 < iTargetCount )
						{
							ObjectUnitGenerate(rkGenPoint);
							--iTargetCount;
						}
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_ObjectRegenPointGroup:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Active:
				{
					BM::CAutoMutex kLock(m_kRscMutex);
					int iTargetCount = rkAction.ActionValueAsInt();
					while( 0 < iTargetCount )
					{
						ActivateObjectUnitGenGroup( rkAction.TargetIDAsInt(), false, false );
						--iTargetCount;
					}
				}break;
			case AT_Remove:
				{
					BM::CAutoMutex kLock(m_kRscMutex);
					ContGenPoint_Object::iterator find_iter = m_kContGenPoint_Object.begin();
					while(m_kContGenPoint_Object.end() != find_iter)
					{
						ContGenPoint_Object::mapped_type& rkGenPoint = (*find_iter).second;
						if( rkAction.TargetIDAsInt() == rkGenPoint.Info().iPointGroup )
						{
							OwnChildCont kContChild;
							rkGenPoint.GetChild(kContChild);
							OwnChildCont::const_iterator mon_iter = kContChild.begin();
							while( kContChild.end() != mon_iter )
							{
								CUnit* pkMonster = GetUnit((*mon_iter));
								if( pkMonster )
								{
									PgAggroMeter* pkMeter = NULL;
									if( GetMeter(pkMonster->GetID(), pkMeter) )
									{
										pkMeter->Clear(true); // ����ġ ����
									}
									pkMonster->ClearTargetList();
									::OnDamaged(NULL, pkMonster, 0, pkMonster->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime()); // �׾��
								}
								++mon_iter;
							}
						}
						++find_iter;
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_SummonNPC_RegenPoint:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Active:
				{
					PgIndun * pkIndun = dynamic_cast<PgIndun*>(this);
					if(NULL == pkIndun)
					{
						break;
					}

					BM::CAutoMutex kLock(m_kRscMutex);
					ContGenPoint_Object::iterator find_iter = m_kContGenPoint_SummonNPC.find( rkAction.TargetIDAsGuid() );
					bProcessed = m_kContGenPoint_SummonNPC.end() != find_iter;
					if( bProcessed )
					{
						ContGenPoint_Object::mapped_type& rkGenPoint = (*find_iter).second;
						pkIndun->SummonNPC_Generate(rkGenPoint, rkAction.ActionValueAsInt());
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	case WETT_MoveUnit:
		{
			switch( rkAction.ActionType() )
			{
			case AT_Status:
				{
					POINT3 kTargetPos;
					if( S_OK == PgGround::FindTriggerLoc(MB(rkAction.ActionValue()), kTargetPos, true) )
					{
						NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+5.0f), NxVec3(0, 0, -1.0f));
						NxRaycastHit kHit;
						NxShape *pkHitShape = RayCast(kRay, kHit, 150.0f);
						if(pkHitShape)
						{
							kTargetPos.z = kHit.worldImpact.z;
						}

						EUnitType eUnitType = UT_NONETYPE;
						BM::CAutoMutex kLock(m_kRscMutex);
						if(rkAction.TargetID()==BM::vstring(L"UT_PLAYER"))
						{
							eUnitType = UT_PLAYER;
						}
						else if (rkAction.TargetID()==BM::vstring(L"UT_OBJECT"))
						{
							eUnitType = UT_OBJECT;
						}

						if(UT_NONETYPE!=eUnitType)
						{
							CUnit* pkTarget = NULL;
							CONT_OBJECT_MGR_UNIT::iterator kItor;
							PgObjectMgr::GetFirstUnit(eUnitType, kItor);
							while((pkTarget = PgObjectMgr::GetNextUnit(eUnitType,kItor)) != NULL)
							{
								if(pkTarget->IsUnitType(UT_PLAYER))
								{
									SendToPosLoc(pkTarget, kTargetPos, MMET_Normal);
								}
								else
								{
									pkTarget->SetPos(kTargetPos);
								}
							}
						}
					}
				}break;
			}
		}break;
	default:
		{
			bProcessed = false;
		}break;
	}

	if( !bProcessed )
	{
		PgWorldEventMgr::DoWorldEventAction(iGroundNo, rkAction, rkWorldEvent);
	}
}

void PgGround::NfyWEClientOjbectToGroundUser(PgWEClientObjectState const& rkState)
{
	BM::Stream kNfyPacket( PT_M_C_NFY_CLIENTOBJECT_CHANGED );
	rkState.WriteToPacket( kNfyPacket );
	Broadcast(kNfyPacket);
}

void PgGround::NfyWorldEventToGroundUser(PgWorldEventState const& rkState)
{
	BM::Stream kNfyPacket( PT_M_C_NFY_WORLDEVENT_CHANGED );
	rkState.WriteToPacket( kNfyPacket );
	Broadcast(kNfyPacket);
}

void PgGround::SendShowQuestDialog(CUnit* pkUnit, BM::GUID const &rkNpcGUID, EQuestShowDialogType const eQuestShowDialogType, int const iQuestID, int const iDialogID)
{
	if( !pkUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	//����Ʈ ��ȭ�� ����
	bool const bFindDialog = PgGroundQuestUtil::IsCanShowQusetDialog(eQuestShowDialogType, iQuestID, iDialogID);
	int const iPreQuestId = pkUnit->GetAbil(AT_QUEST_TALK);
	int const iTalkState = (bFindDialog)? iQuestID: QTS_None;

	BM::GUID kTalkSessionGuid;
	if( bFindDialog )
	{
		m_kQuestTalkSession.NewTalk(pkUnit->GetID(), rkNpcGUID, iQuestID, iDialogID, kTalkSessionGuid);
	}
	else
	{
		kTalkSessionGuid.Clear();
		m_kQuestTalkSession.ClearTalk(pkUnit->GetID());
	}

	switch( eQuestShowDialogType )
	{
	case QSDT_BalloonTip:
		{
			// do nothing
		}break;
	case QSDT_NormalDialog:
	case QSDT_BeginDialog:
	case QSDT_CompleteDialog:
	case QSDT_FailedDialog:
	case QSDT_ErrorDialog:
		{
			pkUnit->SetAbil(AT_QUEST_TALK, iTalkState, false, true); // ����Ʈ ��ȭ�� ����
			//��ȹ��û���� �����z �޴� ���� ������ �Ե��� ����(13.03.12)
			//if( bFindDialog )
			//{
			//	if( QTS_None == iPreQuestId ) // ���� ��ȭ��
			//	{
			//		pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_QUEST, true, E_SENDTYPE_BROADALL_EFFECTABIL );// Ÿ���� �ȵǵ��� ����
			//		pkUnit->AddCountAbil( AT_CANNOT_ATTACK, AT_CF_QUEST, true, E_SENDTYPE_BROADALL_EFFECTABIL );// ���ݸ��ϵ��� ����
			//	}
			//}
			//else
			//{
			//	pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_QUEST, false, E_SENDTYPE_BROADALL_EFFECTABIL );// Ÿ���� �ȵǵ��� ���� ����
			//	pkUnit->AddCountAbil( AT_CANNOT_ATTACK, AT_CF_QUEST, false, E_SENDTYPE_BROADALL_EFFECTABIL );// ���ݸ��ϵ��� ���� ����
			//}
		}break;
	case QSDT_SelectDialog:
	default:
		{
			pkUnit->SetAbil(AT_QUEST_TALK, QTS_None, false, true); // ����Ʈ ��ȭ�� ����
		}break;
	}

	BM::Stream kPacket(PT_M_C_SHOWDIALOG);
	kPacket.Push( kTalkSessionGuid );
	kPacket.Push( rkNpcGUID );
	kPacket.Push( static_cast<BYTE>(eQuestShowDialogType) );//Dialog Type
	kPacket.Push( iQuestID ); // Quest ID
	kPacket.Push( iDialogID ); // Dialog ID
	pkUnit->Send(kPacket);
}

void PgGround::SyncPlayerPlayTime(BM::GUID const kCharGuid, int const iAccConSec, int const iAccDisSec, __int64 const i64SelectSec)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(GetUnit(kCharGuid));
	if ( pkPlayer )
	{
		pkPlayer->SetPlayTime(iAccConSec,iAccDisSec);
		pkPlayer->SetSelectCharacterSec(i64SelectSec);
		pkPlayer->SetLasSpecificRewardSec(i64SelectSec);

		BM::Stream kPacket(PT_M_C_UPDATE_PLAYERPLAYTIME);
		kPacket.Push(iAccConSec);
		kPacket.Push(iAccDisSec);
		kPacket.Push(i64SelectSec);
		pkPlayer->Send(kPacket);
	}
}

bool PgGround::CheckTickAvailable(ETickInterval const eInterval, DWORD const dwNow, DWORD& rdwElapsedTime, bool bUpdateLastTime)
{
	static DWORD const asdwTickDueTime[ETICK_INTERVAL_MAX] = {MONSTER_AI_TICK_INTER-MONSTER_AI_TICK_INTER_ERROR, 30000-1000, 1000-200, 5000-1000 };	//������ CPU������ ������� 100ms�� 300ms�� ������ ������

	LASTTICKTIME_VEC::value_type &kLastTickTime = m_kContLastTickTime.at(eInterval);

	LASTTICKTIME_VEC::value_type kTempTickTime = kLastTickTime;

	rdwElapsedTime = dwNow - kLastTickTime;//���� �ð��� ������ ���
	if (m_kPaused)
	{
		rdwElapsedTime = 0;
	}

	bool const bRet = BM::TimeCheck(kTempTickTime, asdwTickDueTime[eInterval]);

	if(bUpdateLastTime)
	{
		kLastTickTime = kTempTickTime;
	}
	return bRet;
}
/*
inline bool PgGround::CheckTickAvailable(ETickInterval eInterval, DWORD const dwNow, DWORD& rdwElapsedTime, bool bUpdateLastTime)
{
	static DWORD const adwTickDueTime[ETICK_INTERVAL_MAX] = {MONSTER_AI_TICK_INTER-MONSTER_AI_TICK_INTER_ERROR, 30000-1000, 1000-200, 5000-1000 };	//������ CPU������ ������� 100ms�� 300ms�� ������ ������

	DWORD const dwLastTime = (m_kLastTickTimeVec[eInterval] == 0) ? dwNow : m_kLastTickTimeVec[eInterval];
	rdwElapsedTime = dwNow - dwLastTime;
	if (rdwElapsedTime < adwTickDueTime[eInterval])
	{
		if (m_kLastTickTimeVec[eInterval] == 0)
		{
			m_kLastTickTimeVec[eInterval] = dwNow;
		}
		LIVE_CHECK_LOG_NEW(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (bUpdateLastTime)
	{
		m_kLastTickTimeVec[eInterval] = dwNow;
	}
	return true;
}
*/

void PgGround::DynamicGndAttr(EDynamicGroundAttr const eNewDGAttr)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	//
	m_kDynamicGndAttr = eNewDGAttr;

	//
	BM::Stream kPacket(PT_M_C_NFY_GROUND_DYNAMIC_ATTR);
	kPacket.Push( static_cast< int >(m_kDynamicGndAttr) );
	Broadcast(kPacket, NULL, E_SENDTYPE_MUSTSEND);
}

void PgGround::CheckTickMarry()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( this->GetAttr() & GATTR_MARRY )
	{
		if( m_kMarryMgr.IsMarryUse() )
		{
			BM::GUID kCharGuid;
			BM::GUID kCoupleGuid;

			m_kMarryMgr.GetMarryCoupleGuid(kCharGuid, kCoupleGuid);

			CUnit *pkCharUnit = PgObjectMgr::GetUnit(kCharGuid);
			CUnit *pkCoupleUnit = PgObjectMgr::GetUnit(kCoupleGuid);

			if( !pkCharUnit && !pkCoupleUnit )
			{
				m_kMarryMgr.SetActionState(EM_MARRY_ACTION);			
			}
			else
			{
				BYTE eType = 0;
				if( true == m_kMarryMgr.GetActionState(eType) )
				{					
					if( static_cast<BYTE>(EM_ACTION_END) == eType )
					{
						// ���͸� �Ѹ� ���¶��... ���������� üũ�غ��� ���� �ܰ�� �Ѿ� ������.
						CUnit* pkUnit = NULL;
						PgMonster* pkMonster = NULL;
						CONT_OBJECT_MGR_UNIT::iterator kItor;
						PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
						pkUnit = PgObjectMgr::GetNextUnit(UT_MONSTER, kItor);
						if( !pkUnit )
						{
							// ���Ͱ� �� �װ� ����
							m_kMarryMgr.SetActionState(EM_MARRY_ACTION);
						}
					}
				}
			}
		}
		m_kMarryMgr.Tick();
	}
}

void PgGround::CheckTickHidden()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	BM::DBTIMESTAMP_EX kNow;
	g_kEventView.GetLocalTime(kNow);

	int const iCheckMinTime = 5;

	if( iCheckMinTime < kNow.minute )
	{
		HiddenMapOut(false); // �ðܳ��� �ȴ� ��������..
	}

	bool bResult = false;

	if( false == HiddenMapOut() ) // �ðܳ� ���� ���ٸ�...
	{
		// 00���� �Ǹ�...
		if( iCheckMinTime >= kNow.minute )
		{
			if( this->GetAttr() & GATTR_HIDDEN_F )
			{
				CUnit* pkUnit = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER,kItor);
				while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER,kItor)) != NULL)
				{		
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
					if( pkPlayer )
					{			// ����� ������ �ðܳ���.
						if( LeaveSpecStatus(pkPlayer->GetID(), PgGroundUtil::SUST_CashShop) )
						{
							BM::Stream kClientPacket(PT_M_C_CS_ANS_EXIT_CASHSHOP_UNLOCK);
							pkPlayer->Send(kClientPacket, E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);

							FakeAddUnit(pkUnit);

							BM::Stream kPacket( PT_M_I_CS_REQ_EXIT_CASHSHOP, pkPlayer->GetID() );
							kPacket.Push(pkPlayer->CashShopGuid());

							SendToItem(GroundKey(),kPacket);
						}

						this->RecvRecentMapMove(pkPlayer);

						bResult = true;
					}
				}

				size_t iUnitCount = PgObjectMgr::GetUnitCount(UT_PLAYER);
				if( 0 >= iUnitCount )
				{
					bResult = true;
				}

				if( true == bResult )
				{
					HiddenMapOut(true);
				}
			}
		}
	}

	HiddenMapTimeLimit(NULL);
}

void PgGround::HiddenMapRewordItem(CUnit* pkUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( (this->GetAttr() & GATTR_HIDDEN_F) || (this->GetAttr() & GATTR_CHAOS_F) )
	{
		if( pkUnit )
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if( pkPlayer )
			{
				int const iLevel = pkPlayer->GetAbil(AT_LEVEL);

				if( 0 < ms_iHiddenRewordItemNo )
				{
					size_t const kStoneCount = pkUnit->GetInven()->GetTotalCount(ms_iHiddenRewordItemNo);

					if( 0 <= kStoneCount )
					{
						int const iItemCount = m_kHiddenRewordItemMgr.GetMyItem_ViewCount(iLevel, kStoneCount);
						int const iItemNo = m_kHiddenRewordItemMgr.GetMyItem_ViewItemNo(iLevel, kStoneCount);

						BM::Stream kCPacket(PT_M_C_NFY_HIDDEN_REWORDITEM);
						kCPacket.Push(static_cast<int>(ms_iHiddenRewordItemNo));
						kCPacket.Push(static_cast<int>(kStoneCount));
						kCPacket.Push(static_cast<int>(iItemCount));
						kCPacket.Push(static_cast<int>(iItemNo));
						pkPlayer->Send(kCPacket);
					}
				}
			}
		}
	}
}

void PgGround::HiddenMapTimeLimit(CUnit* pkUnit)
{
	if( this->GetAttr() & GATTR_HIDDEN_F )
	{
		// �ð� ����ȭ
		DWORD m_dwTotalTimeLimit = 0;

		GetHiddenMapTime(m_dwTotalTimeLimit);

		BM::Stream kCPacket(PT_M_C_NFY_HIDDEN_TIME_LIMIT);
		kCPacket.Push(static_cast<int>(m_dwTotalTimeLimit));

		if( pkUnit )
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if( pkPlayer )
			{
				pkPlayer->Send(kCPacket);
			}
		}
		else
		{
			Broadcast(kCPacket);
		}
	}
}

void PgGround::GetHiddenMapTime(DWORD &m_dwTotalTimeLimit)
{	
	BM::DBTIMESTAMP_EX kNow;
	g_kEventView.GetLocalTime(kNow);

	int const iTimeMin = static_cast<int>(kNow.minute);
	int const iTimeSec = static_cast<int>(kNow.second);

	DWORD dwTotalTimeLimit = (60*59 - (iTimeMin*60)) + (59 - (iTimeSec));
	if( 0 > dwTotalTimeLimit )
	{
		dwTotalTimeLimit = 0;
	}
	else
	{
		dwTotalTimeLimit *= 1000;
	}

	m_dwTotalTimeLimit = dwTotalTimeLimit;
}

void PgGround::CheckTickCouple()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER,kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER,kItor)) != NULL)
	{		
		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
		if( pkPlayer )
		{
			BM::GUID kCoupleGuid = pkPlayer->GetCoupleGuid();

			int const iEffectNo = COUPLE_SKILL_AREA;
			int const iCoupleRingEffectNo = COUPLE_SKILL_RING;
			bool bRingEffect = false;

			if( BM::GUID::IsNotNull(kCoupleGuid) )
			{
				CUnit *pkCoupleUnit = PgObjectMgr::GetUnit(kCoupleGuid);

				if( pkCoupleUnit )
				{
					if( kCoupleGuid == pkCoupleUnit->GetID() )
					{
						// Ŀ�ø�
						int iRingItem[] = {60002620, 60002630, 0};
						bool bRingEquip[] = {false, false};

						PgBase_Item kMyRingItem;
						if( SUCCEEDED(pkUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_L), kMyRingItem)) )
						{
						}
						PgBase_Item kMyRingItem2;
						if( SUCCEEDED(pkUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_R), kMyRingItem2)) )
						{
						}
						PgBase_Item kCoupleRingItem;
						if( SUCCEEDED(pkCoupleUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_L), kCoupleRingItem)) )
						{
						}
						PgBase_Item kCoupleRingItem2;
						if( SUCCEEDED(pkCoupleUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_R), kCoupleRingItem2)) )
						{
						}

						int i = 0;
						while( iRingItem[i] )
						{
							if( (iRingItem[i] == kMyRingItem.ItemNo()) || (iRingItem[i] == kMyRingItem2.ItemNo()) )
							{
								bRingEquip[0] = true;
							}
							++i;
						}

						i = 0;
						while( iRingItem[i] )
						{
							if( (iRingItem[i] == kCoupleRingItem.ItemNo()) || (iRingItem[i] == kCoupleRingItem2.ItemNo()) )
							{
								bRingEquip[1] = true;
							}
							++i;
						}

						if( (true == bRingEquip[0]) && (true == bRingEquip[1])  )
						{							
							CEffect const* pkEffect = pkPlayer->GetEffect(iCoupleRingEffectNo, true);
							if( !pkEffect )
							{
								SEffectCreateInfo kCreate;
								kCreate.eType = EFFECT_TYPE_NORMAL;
								kCreate.iEffectNum = iCoupleRingEffectNo;
								kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
								pkPlayer->AddEffect(kCreate);								
							}
							bRingEffect = true;
						}
						else
						{
							CEffect const* pkEffect = pkPlayer->GetEffect(iCoupleRingEffectNo, true);
							if(NULL != pkEffect)
							{
								if( 0 < iCoupleRingEffectNo )
								{
									pkPlayer->DeleteEffect(iCoupleRingEffectNo);
								}
							}
						}
						//
						POINT3 const &rkCurPos = pkPlayer->GetPos();
						POINT3 const &rkCouplePos = pkCoupleUnit->GetPos();
						float const fDistance = POINT3::Distance(rkCurPos, rkCouplePos);
						float const fCanAddExpDistance = COUPLE_PASSIVE_HPMP_INCREASE_EFFECT_ABLE_DIST;
						if( fCanAddExpDistance >= fDistance )
						{
							CEffect const* pkEffect = pkPlayer->GetEffect(iEffectNo, true);
							if(NULL != pkEffect)
							{
								continue;
							}

							SEffectCreateInfo kCreate;
							kCreate.eType = EFFECT_TYPE_NORMAL;
							kCreate.iEffectNum = iEffectNo;
							kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
							pkPlayer->AddEffect(kCreate);
							
							continue;
						}
					}
				}
			}
			CEffect const* pkEffect = pkPlayer->GetEffect(iEffectNo, true);
			if(NULL != pkEffect)
			{
				if( 0 < iEffectNo )
				{
					pkPlayer->DeleteEffect(iEffectNo);
				}
			}
			if( false == bRingEffect )
			{
				CEffect const* pkEffect = pkPlayer->GetEffect(iCoupleRingEffectNo, true);
				if(NULL != pkEffect)
				{
					if( 0 < iCoupleRingEffectNo )
					{
						pkPlayer->DeleteEffect(iCoupleRingEffectNo);
					}
				}
			}
		}
	}
}

void PgGround::UpdateWorldEnvironmentStatus(SWorldEnvironmentStatus const& rkNewEnvStatus)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( m_kWorldEnvironmentStatus.Update(rkNewEnvStatus) )
	{
		BM::Stream kPacket(PT_M_C_NFY_ALL_GROUND_WORLD_ENVIRONMENT_STATUS);
		kPacket.Push( m_kWorldEnvironmentStatus.GetFlag() );
		Broadcast(kPacket);
	}
}
void PgGround::SetRealmQuestStatus(SRealmQuestInfo const& rkRealmQuestInfo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_REALM_QUEST_INFO::iterator iter = m_kRealmQuestInfo.find(rkRealmQuestInfo.kRealmQuestID);
	if( m_kRealmQuestInfo.end() == iter )
	{
		auto kRet = m_kRealmQuestInfo.insert( std::make_pair(rkRealmQuestInfo.kRealmQuestID, rkRealmQuestInfo) );
		iter = kRet.first;
	}
	else
	{
		(*iter).second = rkRealmQuestInfo;
	}
}

size_t PgGround::GetUnitCount( const eUnitType eType )const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	return PgObjectMgr::GetUnitCount(eType);
}

int PgGround::GetMaxSummonUnitCount(CUnit * pkUnit)const
{
    return pkUnit->GetMaxSummonUnitCount();
}

bool PgGround::GetDefenceModeMatchParty(BM::GUID const& kMyPartyGuid, BM::GUID& rkPartyGuid)
{
	VEC_GUID kVecGuid;

	m_kDefencePartyMgr.FindWaitList(kVecGuid);	// ������� ��Ƽ��...
	if( kVecGuid.empty() )
	{
		return false;
	}

	std::random_shuffle(kVecGuid.begin(), kVecGuid.end(), BM::Rand_Index);

	int iMyMemberCount = GetPartyMemberCount(kMyPartyGuid);

	VEC_GUID::iterator iter = kVecGuid.begin();
	while( kVecGuid.end() != iter )
	{
		VEC_GUID::value_type kGuid = (*iter);

		if( kGuid != kMyPartyGuid )
		{
			int iMemberCount = GetPartyMemberCount(kGuid);

			if( iMyMemberCount == iMemberCount )
			{
				rkPartyGuid = kGuid;
				
				return true;
			}
		}

		++iter;
	}
	return false;
}

int PgGround::DefenceIsJoinParty(BM::GUID const& kPartyGuid)
{
	return m_kDefencePartyMgr.IsJoinParty(kPartyGuid);
}

bool PgGround::DefenceAddWaitParty(BM::GUID const& kPartyGuid, int eType)
{
	return m_kDefencePartyMgr.AddWaitParty(kPartyGuid, eType);
}

bool PgGround::DefenceModifyWaitParty(BM::GUID const& kPartyGuid, int eType)
{
	return m_kDefencePartyMgr.ModifyWaitParty(kPartyGuid, eType);
}

bool PgGround::DefenceDelWaitParty(BM::GUID const& kPartyGuid)
{
	return m_kDefencePartyMgr.DelWaitParty(kPartyGuid);
}

void PgGround::Defence7ItemUse(CUnit* pkUnit, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( !pkUnit || !pkUnit->GetInven() )
	{
		return;
	}
	PgInventory* pkInv = pkUnit->GetInven();

	int iItemNo = 0;
	SItemPos kItemPos;

	pkPacket->Pop( iItemNo );
	pkPacket->Pop( kItemPos );

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pItemDef = kItemDefMgr.GetDef(iItemNo);
	if( pItemDef )
	{
		BM::GUID const rkGuid = pkUnit->GetID();

		T_GNDATTR const iNotAttr = (T_GNDATTR)(pItemDef->GetAbil(AT_NOTAPPLY_MAPATTR));
		T_GNDATTR const iCanAttr = (T_GNDATTR)(pItemDef->GetAbil(AT_CAN_GROUND_ATTR));
		bool bCantUseGround = (0 != (iNotAttr & GetAttr()));
		bCantUseGround = ((0 != iCanAttr)? 0 == (GetAttr() & iCanAttr): false) || bCantUseGround;
		if( bCantUseGround )
		{
			// ����� �� ���� ����Դϴ�.
			pkUnit->SendWarnMessage(20027);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return;
		}

		int const iCustomKind = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
		int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

		if( UICT_MISSION_DEFENCE7_ITEM != iCustomKind )
		{
			// �ű� ���ҽ� �������� �ƴϴ�.
			return;
		}

		switch( iCustomType )
		{
		case DI_ADD_GUARDIAN:
			{
				int const iGuardianNo = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
				int const iLifeTime = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);
				if( 0 >= iGuardianNo )
				{
					return;
				}

				const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
				g_kTblDataMgr.GetContDef(pkGuardian);

				if( !pkGuardian )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
					return;
				}

				CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardianNo);

				CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
				if( pkGuardian->end() == iter )
				{
					return;
				}
				SMISSION_DEFENCE7_GUARDIAN const& kValue = iter->second.kCont.at(0);

				BM::vstring kNodeName("Entity");
				POINT3 kNodePos = pkUnit->GetPos();

				SCreateEntity kCreateInfo;
				kCreateInfo.kClassKey.iClass = kValue.iMonsterNo;
				kCreateInfo.kClassKey.nLv = kValue.iUpgrade_Step;
				kCreateInfo.bUniqueClass = false;
				//kCreateInfo.bEternalLife = true;	// �̶� �����Ǵ� ������� ������ Ÿ���� ������.
				kCreateInfo.iTunningNo = GetGuardianTunningNo(iGuardianNo);
				kCreateInfo.iTunningLevel = pkUnit->GetAbil(AT_LEVEL);
				kCreateInfo.iLifeTime = iLifeTime;
				kCreateInfo.kGuid.Generate();

				//��ȯ ��ġ : PC�� ����
				{
					float fDistance = pItemDef->GetAbil(AT_CREATE_ENTITY_RANGE);
					if( 0.f == fDistance )
					{
						fDistance = 200.f;
					}
					Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());
					//::GetDistanceToPosition( GetPathRoot(), pkUnit->GetPos(), eFrontDir, fDistance, kNodePos );

					POINT3 const kTempFrontVec = pkUnit->GetDirectionVector(eFrontDir);;
					NxVec3 const kDirVec(kTempFrontVec.x,kTempFrontVec.y,kTempFrontVec.z);
					NxRay kRay(NxVec3(kNodePos.x, kNodePos.y, kNodePos.z), kDirVec);
					NxRaycastHit kHit;
					NxShape *pkHitShape = RayCast(kRay, kHit, fDistance);
					if(pkHitShape)
					{
						kNodePos.x = kHit.worldImpact.x;
						kNodePos.y = kHit.worldImpact.y;
					}
					else
					{
						::GetDistanceToPosition( GetPathRoot(), pkUnit->GetPos(), eFrontDir, fDistance, kNodePos );
					}
				}

				// �ٴڿ� ���� ��Ų��
				NxRay kRay(NxVec3(kNodePos.x, kNodePos.y, kNodePos.z+20), NxVec3(0, 0, -1.0f));
				NxRaycastHit kHit;
				NxShape *pkHitShape = RayCast(kRay, kHit);
				if(pkHitShape)
				{
					kNodePos.z = kHit.worldImpact.z;
				}
				kCreateInfo.ptPos = kNodePos;

				CUnit* pkEntityUnit = CreateGuardianEntity(pkUnit, &kCreateInfo, kNodeName);
				if( !pkEntityUnit )
				{
					return;
				}
				SetGuardianAbil(pkEntityUnit, kValue);
			}break;
		default:
			{
				return;
			}break;
		}

		PgBase_Item kItem;
		if( S_OK == pkInv->GetItem(kItemPos, kItem) )
		{
			CONT_PLAYER_MODIFY_ORDER kOrder;
			SPMOD_Modify_Count kDelData(kItem, kItemPos, -1);
			SPMO kIMO(IMET_MODIFY_COUNT, pkUnit->GetID(), kDelData);
			kOrder.push_back(kIMO);

			PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_UseItem, GroundKey(), kOrder);
			kItemModifyAction.DoAction(pkUnit, NULL);
		}
	}
	else
	{
		pkUnit->SendWarnMessage(20029);
	}
}

int PgGround::GetGuardianTunningNo(int const iGuardian)const
{
    if( 0==iGuardian )
    {
        return 0;
    }
    const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);

	if( !pkGuardian )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_GUARDIAN_BAG is NULL") );
		return 0;
	}

	CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey(iGuardian);
	CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator iter = pkGuardian->find(kKey);
	if( pkGuardian->end() != iter )
	{
		SMISSION_DEFENCE7_GUARDIAN const & kValue = iter->second.kCont.at(0);
		return kValue.iGuardian_TunningNo;
	}

	return 0;
}

void PgGround::SetGuardianAbil(CUnit* pkUnit, SMISSION_DEFENCE7_GUARDIAN const& kValue)const
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( !pkUnit )
	{
		return;
	}

	CONT_DEFMONSTER const* pkDefMonster;
	g_kTblDataMgr.GetContDef(pkDefMonster);

	if( !pkDefMonster )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMONSTER is NULL") );
		return;
	}

	CONT_DEFMONSTER::const_iterator iter = pkDefMonster->find(kValue.iMonsterNo);
	if( pkDefMonster->end() != iter )
	{
		for(int i=0; i<MAX_MONSTER_ABIL_LIST; ++i)
		{
			SetGuardianAbilUpdate(pkUnit, iter->second.aAbil[i]);
		}
	}
	SetGuardianAbilUpdate(pkUnit, kValue.iGuardian_Abil01);
	SetGuardianAbilUpdate(pkUnit, kValue.iGuardian_Abil02);
    PgEntity *pkEntity = dynamic_cast<PgEntity*>(pkUnit);
    pkEntity ? pkEntity->TunningAbil(kValue.iGuardian_TunningNo, pkUnit->GetAbil(AT_GRADE), m_iTunningLevel, true, true) : 0;
}

void PgGround::SetGuardianAbilUpdate(CUnit* pkUnit, int const iMonAbilNo)const
{
	if( !pkUnit )
	{
		return;
	}
	if( !pkUnit->IsUnitType(UT_ENTITY) )
	{
		return;
	}

	CONT_DEFMONSTERABIL const* pkDefMonAbil = NULL;
	g_kTblDataMgr.GetContDef(pkDefMonAbil);

	if( !pkDefMonAbil )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMONSTERABIL is NULL") );
		return;
	}

	CONT_DEFMONSTERABIL::const_iterator abil_iter = pkDefMonAbil->find(iMonAbilNo);
	if( pkDefMonAbil->end() != abil_iter )
	{
		CONT_DEFMONSTERABIL::mapped_type const& rkMonAbil = (*abil_iter).second;
		for( int iAbilCur = 0; MAX_MONSTER_ABIL_ARRAY > iAbilCur; ++iAbilCur )
		{
			WORD const wAbilType = static_cast< WORD >(rkMonAbil.aType[iAbilCur]);
			int const iAbilValue = rkMonAbil.aValue[iAbilCur];
			if( (0 != wAbilType) && (0 != iAbilValue) )
			{
				pkUnit->SetAbil(wAbilType, iAbilValue, true, true);
			}
		}
	}
}

void PgGround::SendDefencePartyAllList(CUnit* pkUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_DEFENCE_PARTY_LIST rkWaitCont, rkPlayCont;
	rkWaitCont.clear();
	rkPlayCont.clear();

	VEC_GUID kWaitVecGuid, kPlayVecGuid;
	m_kDefencePartyMgr.GetDefencePartyTypeList(kWaitVecGuid, static_cast<int>(PI_WAIT));
	m_kDefencePartyMgr.GetDefencePartyTypeList(kPlayVecGuid, static_cast<int>(PI_PLAY));

	VEC_GUID::iterator iter = kWaitVecGuid.begin();
	while( kWaitVecGuid.end() != iter )
	{
		VEC_GUID::value_type kGuid = (*iter);

		int iMemberCount = GetPartyMemberCount(kGuid);

		auto bRet = rkWaitCont.insert(std::make_pair(iMemberCount, 1));
		if( !bRet.second )
		{
			++(*bRet.first).second;
		}

		++iter;
	}

	iter = kPlayVecGuid.begin();
	while( kPlayVecGuid.end() != iter )
	{
		VEC_GUID::value_type kGuid = (*iter);

		int iMemberCount = GetPartyMemberCount(kGuid);

		auto bRet = rkPlayCont.insert(std::make_pair(iMemberCount, 1));
		if( !bRet.second )
		{
			++(*bRet.first).second;
		}

		++iter;
	}

	BM::Stream kSPacket(PT_M_C_ANS_DEFENCE_PARTYLIST);
	kSPacket.Push( rkWaitCont );
	kSPacket.Push( rkPlayCont );
	pkUnit->Send(kSPacket);	
}

bool PgGround::RecvReqVendorCreate(CUnit* pkUnit, BM::GUID const& rkVendorGuid, std::wstring const& VendorTitle)
{
	return m_kLocalVendorMgr.Create(rkVendorGuid, VendorTitle);
}
bool PgGround::RecvReqVendorRename(CUnit* pkUnit, BM::GUID const& rkVendorGuid, std::wstring const& VendorTitle)
{
	return m_kLocalVendorMgr.Rename(rkVendorGuid, VendorTitle);
}
bool PgGround::RecvReqVendorDelete(CUnit* pkUnit, BM::GUID const& rkVendorGuid)
{
	return m_kLocalVendorMgr.Delete(rkVendorGuid);
}
bool PgGround::RecvReqReadToPacketVendorName(CUnit* pkUnit, BM::GUID const& rkVendorGuid, BM::Stream &rkPacket)
{
	return m_kLocalVendorMgr.ReadToPacketVendorName(rkVendorGuid, rkPacket);
}
void PgGround::RecvVendorClose(CUnit* pkUnit)
{
	if(!pkUnit)
	{
		return;
	}
	if(true == RecvReqVendorDelete(pkUnit, pkUnit->GetID() ) )
	{//���� ������ ���
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if(pkPlayer)
		{
			pkPlayer->OpenVendor(false);
			std::wstring kTemp;
			pkPlayer->VendorTitle(kTemp);
			pkPlayer->VendorGuid(BM::GUID::NullData());

			BM::Stream kPacket(PT_M_C_NFY_VENDOR_STATE);
			kPacket.Push(true);
			kPacket.Push(pkPlayer->GetID());
			kPacket.Push(pkPlayer->OpenVendor());
			kPacket.Push(pkPlayer->VendorTitle());
			kPacket.Push(pkPlayer->VendorGuid());
			Broadcast(kPacket);

			pkUnit->SetAbil(AT_CALL_MARKET,0,true);
			LeaveSpecStatus(pkUnit->GetID(), PgGroundUtil::SUST_Vendor);
		}
	}
	else
	{//�մ��� ���
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if(pkPlayer)
		{
			pkPlayer->OpenVendor(false);
			std::wstring kTemp;
			pkPlayer->VendorTitle(kTemp);
			pkPlayer->VendorGuid(BM::GUID::NullData());

			BM::Stream kPacket(PT_M_C_UM_ANS_VENDOR_EXIT);
			kPacket.Push(pkPlayer->GetID());
			pkUnit->Send(kPacket);
		}
	}
}

void PgGround::JobSkillLocationItemInit()
{
	const CONT_DEF_JOBSKILL_LOCATIONITEM * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_JOBSKILL_LOCATIONITEM is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return;
	}

	CONT_GTRIGGER::const_iterator triger_iter = m_kContTrigger.begin();
	while( m_kContTrigger.end() != triger_iter )
	{
		CONT_GTRIGGER::mapped_type const& pkElement = (*triger_iter).second;
		if( pkElement )		
		{
			if( GTRIGGER_TYPE_JOB_SKILL == pkElement->GetType() )
			{
				PgGTrigger_JobSkill *pkGTrigger = dynamic_cast<PgGTrigger_JobSkill*>(pkElement);
				if( pkGTrigger )
				{
					int iJobGrade = pkGTrigger->GetJobGrade();

					SJobSkill_LocationItem kInfo;					

					CONT_DEF_JOBSKILL_LOCATIONITEM::const_iterator defiter = pkDef->find(iJobGrade);
					if(defiter != pkDef->end())
					{
						CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type const &kElement = defiter->second;

						CONT_GTRIGGER::key_type kTriggerID = triger_iter->first;
						kInfo.iJobGrade = iJobGrade;
						kInfo.iGatherType = kElement.iGatherType;
						kInfo.bSpot = (kElement.iSpot_Probability || kElement.iSpot_TotalProbability) ? true : false;
						bool bIsRet = JobSkillLocationMgr().AddElement(kTriggerID, kInfo);
						if( !bIsRet )
						{
							CAUTION_LOG( BM::LOG_LV1, __FL__<<C2L(GroundKey())<<L"Can't copy trigger, Duplicate ID["<<(*triger_iter).first );
						}
						if( kInfo.bSpot )
						{
							bIsRet = JobSkillLocationMgr().SetSpotProbability(kTriggerID);
						}
					}
				}
			}
		}
		++triger_iter;
	}
}

void PgGround::CheckTickJobSkillLocationItem()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( JobSkillLocationMgr().IsEmpty() )
	{
		return;
	}
	CONT_DEF_JOBSKILL_SAVEIDX const* pkDefJobSkillSaveIdx = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillSaveIdx);
	if( !pkDefJobSkillSaveIdx )
	{
		return;
	}

	CONT_JOBSKILL_LOCATIONINFO kInfo;
	JobSkillLocationMgr().LocationDrationTime(kInfo);

	for(CONT_JOBSKILL_LOCATIONINFO::const_iterator iter = kInfo.begin(); kInfo.end() != iter; ++iter)
	{
		CONT_JOBSKILL_LOCATIONINFO::key_type const& kTriggerID = iter->first;
		CONT_JOBSKILL_LOCATIONINFO::mapped_type const& kElement = iter->second;

		BM::Stream kPacket;
		JobSkillLocationMgr().SendLocationInfo(kPacket, kTriggerID, kElement);
		Broadcast(kPacket, NULL, E_SENDTYPE_BROADCAST);
	}

	// ���� ��ų �ð�üũ
	CONT_JOBSKILL_LOCATIONITEM_RESULT kResult;
	JobSkillLocationMgr().UserDrationSkillTime(kResult);

	for(CONT_JOBSKILL_LOCATIONITEM_RESULT::const_iterator item_iter = kResult.begin(); kResult.end() != item_iter; ++item_iter)
	{
		CONT_JOBSKILL_LOCATIONITEM_RESULT::key_type const& kGuid = item_iter->first;
		CONT_JOBSKILL_LOCATIONITEM_RESULT::mapped_type const& kTriggerID = item_iter->second;

		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(GetUnit( kGuid ));
		if( pkPlayer )
		{
			bool bEquippedToolItemDestroy = false;
			bool bJobSkillSuccess = false;
			SJobSkillUserInfo kUserInfo;
			if( JobSkillLocationMgr().GetUser(kTriggerID, kGuid, kUserInfo) )
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;
				
				int iJobSkill_AddToolUseDurationRate = AT_JS_1ST_ADD_TOOL_USE_DURATION_RATE;
				int iJobSkill_AddResultItemCount	 = AT_JS_1ST_ADD_RESULT_ITEM_COUNT;
				int iJobSkill_AddUseExhaustion		 = AT_JS_1ST_ADD_USE_EXHAUSTION;
				int iJobSkill_AddExpertnessRate		 = AT_JS_1ST_ADD_EXPERTNESS_RATE;
				if( JST_1ST_SUB == JobSkill_Util::GetJobSkillType(kUserInfo.iUseSkillNo) )
				{
					iJobSkill_AddToolUseDurationRate = AT_JS_1ST_SUB_ADD_TOOL_USE_DURATION_RATE;
					iJobSkill_AddResultItemCount	 = AT_JS_1ST_SUB_ADD_RESULT_ITEM_COUNT;
					iJobSkill_AddUseExhaustion		 = AT_JS_1ST_SUB_ADD_USE_EXHAUSTION;
					iJobSkill_AddExpertnessRate		 = AT_JS_1ST_SUB_ADD_EXPERTNESS_RATE;
				}
				int const iUseDuration = JobSkillExpertnessUtil::GetUseDuration(kUserInfo.iUseSkillNo, kUserInfo.iExpertness, pkPlayer->GetAbil(iJobSkill_AddToolUseDurationRate));

				if( 0 < iUseDuration ) 
				{//�Ҹ��ų �������� ������..
					PgInventory * pkInv = pkPlayer->GetInven();
					if( pkInv )
					{//�κ��� �����ͼ�
						SItemPos kItemPos;
						PgBase_Item kItem;
						
						bool bEquippedToolItem = (S_OK == pkInv->GetFirstItem(IT_FIT, kUserInfo.iUseToolItemNo, kItemPos)) ? true:
												 (S_OK == pkInv->GetFirstItem(IT_FIT_CASH, kUserInfo.iUseToolItemNo, kItemPos) ? true: false
												 /*(S_OK == pkInv->GetFirstItem(IT_FIT_COSTUME, kUserInfo.iUseToolItemNo, kItemPos))*/); // Can't use job tool in costume

						if( bEquippedToolItem
							&& S_OK == pkInv->GetItem(kItemPos, kItem) )
						{//�����۰� ��ġ�� �����ͼ�
							if( IT_FIT == kItemPos.x || IT_FIT_CASH == kItemPos.x )
							{//�������̸� ���� �Ҹ� ���� ����
								SPMOD_Modify_Count kDelData(kItem, kItemPos, -iUseDuration);
								SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), kDelData);
								kOrder.push_back(kIMO);

								if( 1 >= kItem.Count() )
								{
									bEquippedToolItemDestroy = true;
									pkPlayer->SendWarnMessage(25012);
								}
							}
						}
					}
				}

				{
					int iJobGrade = 0;
					CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type kElement;
					if( JobSkillLocationMgr().GetJobGrade(kTriggerID, iJobGrade) )
					{
						if( JobSkillLocationMgr().GetDefLocationItem(iJobGrade, kElement) )
						{
							int iAddExpertnessRate = pkPlayer->GetAbil(iJobSkill_AddExpertnessRate);
							if( S_PST_JS1_AddExpertness const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_JS1_AddExpertness>() )
							{
								iAddExpertnessRate += pkPremium->iRate;
							}

							PgAction_JobSkill_AddExpertness kAddExpertness(CIE_JOBSKILL, kUserInfo.iUseSkillNo, kElement.iBase_Expertness, GroundKey());
							kAddExpertness.AddExpertnessRate(iAddExpertnessRate);
							kAddExpertness.DoAction(pkPlayer, NULL);
						}
					}
				}
				size_t const iOldOrderCount = kOrder.size();
				int const iResultCount = JobSkillToolUtil::GetResultCount(kUserInfo.iUseToolItemNo);
				for( int iIndex=0; iIndex<iResultCount; ++iIndex)
				{//������ ResultCount��ŭ Ȯ�� ��� �Ͽ� �������� ���� �ϵ��� ��.
					PgBase_Item kResultItem;
					ProbabilityUtil::SGetOneArguments kArg(pkPlayer->GetID(), *pkDefJobSkillSaveIdx);
					if( JobSkillLocationMgr().GetCreateItem(kTriggerID, kArg, kUserInfo.iUseToolItemNo, kResultItem) )
					{// ������ ����

						kResultItem.Count( kResultItem.Count() + pkPlayer->GetAbil(iJobSkill_AddResultItemCount) );

						tagPlayerModifyOrderData_Insert_Fixed kAddItem(kResultItem, SItemPos(), true);
						SPMO kIMO(IMET_INSERT_FIXED, pkPlayer->GetID(), kAddItem);
						kOrder.push_back(kIMO);

						//������ ȹ�� ���� �޼��� ���
						BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_SUCCEED_GET_ITEM);
						kPacket.Push( kResultItem.ItemNo() );
						pkPlayer->Send(kPacket);

						bJobSkillSuccess = true;
					}
					else
					{
						if( 0 != kArg.iErrorCode ) {
							BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_GUIDE_FAIL_LOCATION);
							kPacket.Push( kArg.iErrorCode );
							pkPlayer->Send( kPacket );
						}
					}
					//�̺�Ʈ ä�� ���
					CONT_DEF_JOBSKILL_EVENT_LOCATION const *pkContLocation;
					g_kTblDataMgr.GetContDef(pkContLocation);
					CONT_DEF_EVENT_REWARD_ITEM_GROUP const *pkEventRewardItemGroup;
					g_kTblDataMgr.GetContDef(pkEventRewardItemGroup);
					if( pkContLocation && pkEventRewardItemGroup )
					{
						BM::DBTIMESTAMP_EX kSysTime;
						g_kEventView.GetLocalTime(kSysTime);
						BM::PgPackedTime kCurTime(kSysTime);;
						int iJobGrade = 0;
						if( JobSkillLocationMgr().GetJobGrade(kTriggerID, iJobGrade) )
						{
							CONT_HAVE_ITEM_DATA_ALL kContItem;
							if( JobSkillLocationMgr().GetEventLocationItem(iJobGrade, kContItem, pkContLocation, pkEventRewardItemGroup, kCurTime) )
							{
								CONT_HAVE_ITEM_DATA_ALL::const_iterator itor_event = kContItem.begin();
								while( kContItem.end() != itor_event )
								{
									PgBase_Item kEventItem = (*itor_event);
									tagPlayerModifyOrderData_Insert_Fixed kAddItem(kEventItem, SItemPos(), true);
									SPMO kIMO(IMET_INSERT_FIXED, pkPlayer->GetID(), kAddItem);
									kOrder.push_back(kIMO);

									//������ ȹ�� ���� �޼��� ���
									BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_SUCCEED_GET_ITEM);
									kPacket.Push( kEventItem.ItemNo() );
									pkPlayer->Send(kPacket);

									++itor_event;
								}
							}
						}
					}
				}

				if( iOldOrderCount == kOrder.size() ) // ��������� ������ ������ �ƹ� �����۵� ���������̴�
				{
					int const iFailedJobSkillResult = 25019;
					pkPlayer->SendWarnMessage( iFailedJobSkillResult );
				}
				
				{//�Ƿε� �Ҹ�� ������ ȹ�� ������ ���ԵǸ� �ȵȴ�. �ӽ��κ��� �� ��� ������ ��ҵǱ� ����.
					CONT_PLAYER_MODIFY_ORDER kOrderExhaution;
					int const iMinimumUseExhaustion = 1;
					int const iResultUseExhaustion = std::max(kUserInfo.iUseExhaustion + pkPlayer->GetAbil(iJobSkill_AddUseExhaustion), iMinimumUseExhaustion);
					SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pkPlayer->GetID(), SPMOD_JobSkillExhaustion(iResultUseExhaustion)); // �Ƿε� �Ҹ�
					kOrderExhaution.push_back(kIMO);
					PgAction_ReqModifyItem kExhautionAction(CIE_JOBSKILL, GroundKey(), kOrderExhaution);
					kExhautionAction.DoAction(pkPlayer, NULL);
				}

				PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL, GroundKey(), kOrder);
				kItemModifyAction.DoAction(pkPlayer, NULL);
			}

			PgLogCont kLogCont(ELogMain_JobSkill, ELogSub_Collect, pkPlayer->GetMemberGUID(), pkPlayer->GetID(),
				pkPlayer->MemberID(), pkPlayer->Name(), pkPlayer->GetAbil(AT_CLASS), pkPlayer->GetAbil(AT_LEVEL),
				pkPlayer->GroundKey().GroundNo(), static_cast<EUnitGender>(pkPlayer->GetAbil(AT_GENDER)));
			kLogCont.UID( pkPlayer->UID() );
			PgLog kLog( ELOrderMain_Item, (bJobSkillSuccess ? ELOrderSub_Success : ELOrderSub_Fail));
			kLog.Set(0, UNI(kTriggerID));
			kLog.Set(0, static_cast<int>(kUserInfo.iUseSkillNo));
			kLogCont.Add( kLog );
			kLogCont.Commit();
			
			//ä�� �Ϸ�, �׼� �����϶�� ��Ŷ ����
			BM::Stream kPacket(PT_M_C_NFY_COMPLETE_JOBSKILL);// �÷��̾ Ʈ���� �����Ҷ����� ������ �ϴ� ����
			kPacket.Push(bEquippedToolItemDestroy);
			kPacket.Push(bJobSkillSuccess);
			pkPlayer->Send(kPacket);
		}

		// ���� �����(������ ���� �ѹ� �õ��� �� ������ �ȴ�.)
		CUnit* pkUnit = GetUnit( kGuid );
		JobSkillLocationMgr().DelUser(pkUnit, kTriggerID, kGuid);
	}
}

void PgGround::GetGenGroupKey(SGenGroupKey& rkGenGrpKey)const
{
	rkGenGrpKey.iMapNo = GetGroundNo();
	rkGenGrpKey.iBalance = GetMapLevel();//�̰� �ٲٸ� �ٸ� �� ���̺���
}

SRecentInfo const& PgGround::GetRecentInfo()const
{
	static SRecentInfo const kNullData;
	return kNullData;
}

void PgGround::Recv_PT_T_M_NFY_RECOVERY_STRATEGY_FATIGUABILITY(int RecoveryValue)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CUnit* pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator iter;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, iter);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, iter)) != NULL )
	{
		PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if( pPlayer )
		{
			pPlayer->SetAbil(AT_STRATEGY_FATIGABILITY, RecoveryValue, true);
		}
	}
}

void PgGround::Recv_PT_T_M_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE(int EventNo, bool EventMapState)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_EVENT_MAP_ENTERABLE_ENTER_LIST::iterator map_iter = m_EventMapEnterableList.find(EventNo);
	if( map_iter != m_EventMapEnterableList.end() )
	{
		(*map_iter).second = EventMapState;
	}
	else if( map_iter == m_EventMapEnterableList.end() )
	{
		m_EventMapEnterableList.insert( std::make_pair( EventNo, EventMapState) );
	}
}

void PgGround::Recv_PT_T_M_NFY_EVENT_GROUND_USER_COUNT_MODIFY(int EventNo, int UserCount)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_CURRENT_EVENT_MAP_USER_COUNT_LIST::iterator usercount_iter = m_CurrentEventMapUserCountList.find(EventNo);
	if( usercount_iter == m_CurrentEventMapUserCountList.end() )
	{
		m_CurrentEventMapUserCountList.insert( std::make_pair(EventNo, UserCount) );
	}
	else
	{
		usercount_iter->second = UserCount;
	}
}

EErrorEventMapMove PgGround::Recv_PT_C_M_REQ_MOVE_EVENT_GROUND(CUnit * pUnit, BM::Stream & Packet)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	int EventNo = 0;
	Packet.Pop(EventNo);

	CONT_DEF_EVENT_SCHEDULE const * pEventSchedule = NULL;
	g_kTblDataMgr.GetContDef(pEventSchedule);

	if( NULL == pEventSchedule )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("EventSchedule Table NULL!!"));
		return EEMM_LIMIT_NONE;
	}

	CONT_DEF_EVENT_SCHEDULE::const_iterator schedule_iter = pEventSchedule->find(EventNo);
	if( schedule_iter == pEventSchedule->end() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("There is no EventNo[") << EventNo << _T("] in EventSchedule Table!!"));
		return EEMM_LIMIT_NONE;
	}

	SEventScheduleData const & ScheduleData = schedule_iter->second;

	PgPlayer * pPlayer = dynamic_cast<PgPlayer *>(pUnit);
	if( NULL == pPlayer )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("This Unit is Not Player"));
		return EEMM_LIMIT_NONE;
	}
	
	VEC_GUID PartyMember;
	bool const bParty = GetPartyMemberGround(pPlayer, GroundKey(), PartyMember);

	if( bParty )
	{
		if( 0 < ScheduleData.PartyMemeberMin )
		{	// �ּ� �ο� ������ �ִٸ� �˻�.
			size_t MemberCount = m_kLocalPartyMgr.GetMemberCount(pPlayer->PartyGuid());

			if( MemberCount < ScheduleData.PartyMemeberMin )
			{	// �ּ� �ο� ���� ä���� ����. ���� �Ұ�.
				return EEMM_LIMIT_MINIMUM_PARTYMEMBER;
			}

			if( MemberCount != PartyMember.size() )
			{	// ��� ��Ƽ���� ���� �ʿ� ���� ����. ���� �Ұ�.
				return EEMM_NO_PARTY_MEMBER_SAME_GROUND;
			}
		}
	}

	if( 0 < ScheduleData.LevelMin )
	{	// �ּ� ���� ������ �ִٸ� �˻�.
		if( bParty )
		{
			VEC_GUID::iterator vec_iter = PartyMember.begin();
			for( ; vec_iter != PartyMember.end() ; ++vec_iter )
			{	// ��� ��Ƽ���� �˻��ؾ��Ѵ�.
				PgPlayer * pPartyMember = GetUser((*vec_iter));
				if( pPartyMember )
				{
					int Level = pPartyMember->GetAbil(AT_LEVEL);
					if( Level < ScheduleData.LevelMin )
					{	// �̺�Ʈ ���� �ּ� ���� ���� ĳ���� ������ �� ����.
						return EEMM_LIMIT_LEVEL_MIN;
					}
				}
			}
		}
		else
		{
			int Level = pPlayer->GetAbil(AT_LEVEL);
			if( Level < ScheduleData.LevelMin )
			{	// �̺�Ʈ ���� �ּ� ���� ���� ĳ���� ������ �� ����.
				return EEMM_LIMIT_LEVEL_MIN;
			}
		}
	}

	if( 0 < ScheduleData.LevelMax )
	{	// �ִ� ���� ������ �ִٸ� �˻�.
		if( bParty )
		{
			VEC_GUID::iterator vec_iter = PartyMember.begin();
			for( ; vec_iter != PartyMember.end() ; ++vec_iter )
			{	// ��� ��Ƽ���� �˻��ؾ��Ѵ�.
				PgPlayer * pPartyMember = GetUser((*vec_iter));
				if( pPartyMember )
				{
					int Level = pPartyMember->GetAbil(AT_LEVEL);
					if( Level > ScheduleData.LevelMax )
					{	// �̺�Ʈ ���� �ּ� ���� ���� ĳ���� ������ �� ����.
						return EEMM_LIMIT_LEVEL_MIN;
					}
				}
			}
		}
		else
		{
			int Level = pPlayer->GetAbil(AT_LEVEL);
			if( Level > ScheduleData.LevelMax )
			{	// �̺�Ʈ ���� �ִ� ���� ���� ĳ���� ������ �� ����.
				return EEMM_LIMIT_LEVEL_MAX;
			}
		}
	}

	//ClassLimit �˻�
	__int64 iUnionClassLimit = (ScheduleData.ClassLimitDragon << DRAGONIAN_LSHIFT_VAL);
	iUnionClassLimit |= ScheduleData.ClassLimitHuman;
	if(bParty)
	{ //��Ƽ ���� ��
		VEC_GUID::iterator vec_iter = PartyMember.begin();
		for( ; vec_iter != PartyMember.end() ; ++vec_iter )
		{	// ��� ��Ƽ���� �˻��ؾ��Ѵ�.
			PgPlayer * pPartyMember = GetUser((*vec_iter));
			if( pPartyMember )
			{
				ERACE_TYPE PlayerRace = pPartyMember->UnitRace();
				if( ERACE_DRAGON == PlayerRace)
				{
					if( ScheduleData.ClassLimitDragon == 0 || !IS_CLASS_LIMIT(iUnionClassLimit, pPartyMember->GetAbil(AT_CLASS)) )
					{	// ClassLimit�� 0�̰ų� Ŭ���� ���ѿ� �ɸ��� ���� �Ұ�.
						return EEMM_LIMIT_CLASS;
					}
				}
				else if( ERACE_HUMAN == PlayerRace)
				{
					if( ScheduleData.ClassLimitHuman == 0 || !IS_CLASS_LIMIT(iUnionClassLimit, pPartyMember->GetAbil(AT_CLASS)) )
					{	// ClassLimit�� 0�̰ų� Ŭ���� ���ѿ� �ɸ��� ���� �Ұ�.
						return EEMM_LIMIT_CLASS;
					}
				}
			}
		}
	}
	else
	{ //���� ������ ��
		if(pPlayer->UnitRace() == ERACE_DRAGON)
		{
			if( ScheduleData.ClassLimitDragon == 0 || !IS_CLASS_LIMIT(iUnionClassLimit, pPlayer->GetAbil(AT_CLASS)) )
			{	// ClassLimit�� 0�̰ų� Ŭ���� ���ѿ� �ɸ��� ���� �Ұ�.
				return EEMM_LIMIT_CLASS;
			}
		}
		else if(pPlayer->UnitRace() == ERACE_HUMAN)
		{
			if( ScheduleData.ClassLimitHuman == 0 || !IS_CLASS_LIMIT(iUnionClassLimit, pPlayer->GetAbil(AT_CLASS)) )
			{	// ClassLimit�� 0�̰ų� Ŭ���� ���ѿ� �ɸ��� ���� �Ұ�.
				return EEMM_LIMIT_CLASS;
			}
		}
	}

	if( 0 < ScheduleData.NeedQuest && 0 < ScheduleData.NeedQuestState )
	{	// �䱸 ����Ʈ�� �ִٸ� ������ �ִ��� �˻�.
		if( bParty )
		{
			VEC_GUID::iterator vec_iter = PartyMember.begin();
			for( ; vec_iter != PartyMember.end() ; ++vec_iter )
			{	// ��� ��Ƽ���� �˻��ؾ��Ѵ�.
				PgPlayer * pPartyMember = GetUser((*vec_iter));
				if( pPartyMember )
				{
					PgMyQuest const * pQuest = pPartyMember->GetMyQuest();
					if( pQuest )
					{
						if( 1 == ScheduleData.NeedQuestState )
						{
							if( !pQuest->IsIngQuest(ScheduleData.NeedQuest) )
							{	// �Ѹ��̶� ������ ���� �Ұ�.
								return EEMM_LIMIT_QUEST;
							}
						}
						else if( 2 == ScheduleData.NeedQuestState )
						{
							if( !pQuest->IsEndedQuest(ScheduleData.NeedQuest) )
							{
								return EEMM_LIMIT_QUEST;
							}
						}
					}
				}
			}
		}
		else
		{
			PgMyQuest const * pQuest = pPlayer->GetMyQuest();
			if( pQuest )
			{
				if( 1 == ScheduleData.NeedQuestState )
				{
					if( !pQuest->IsIngQuest(ScheduleData.NeedQuest) )
					{	// �Ѹ��̶� ������ ���� �Ұ�.
						return EEMM_LIMIT_QUEST;
					}
				}
				else if( 2 == ScheduleData.NeedQuestState )
				{
					if( !pQuest->IsEndedQuest(ScheduleData.NeedQuest) )
					{
						return EEMM_LIMIT_QUEST;
					}
				}
			}
		}
	}

	CONT_PLAYER_MODIFY_ORDER Order;
	if( 0 < ScheduleData.NeedItemNo )
	{	// �䱸 �������� �ִٸ� ������ �ִ��� �˻�.
		if( bParty )
		{
			VEC_GUID::iterator vec_iter = PartyMember.begin();
			for( ; vec_iter != PartyMember.end() ; ++vec_iter )
			{
				PgPlayer * pPartyMember = GetUser((*vec_iter));
				if( pPartyMember )
				{
					PgInventory * pInven = pPartyMember->GetInven();
					if( pInven )
					{
						if( ScheduleData.NeedItemCount > pInven->GetTotalCount(ScheduleData.NeedItemNo) )
						{	// �䱸 �������� ����. �Ѹ��̶� ������ ���� �Ұ�.
							return EEMM_LIMIT_ITEM;
						}
						else
						{
							if( ScheduleData.NeedItemCount > 0 )
							{
								PgBase_Item KeyItem;
								SItemPos KeyItemPos;
								if( SUCCEEDED(pInven->GetFirstItem(ScheduleData.NeedItemNo, KeyItemPos)) )
								{
									if( SUCCEEDED(pInven->GetItem(KeyItemPos, KeyItem)) )
									{
										SPMO IMO(IMET_MODIFY_COUNT | IMC_DEC_DUR_BY_USE, pPartyMember->GetID(), SPMOD_Modify_Count(KeyItem, KeyItemPos, -ScheduleData.NeedItemCount));
										Order.push_back(IMO);
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			PgInventory * pInven = pPlayer->GetInven();
			if( pInven )
			{
				if( ScheduleData.NeedItemCount > pInven->GetTotalCount(ScheduleData.NeedItemNo) )
				{
					return EEMM_LIMIT_ITEM;
				}
				else
				{
					if( ScheduleData.NeedItemCount > 0 )
					{
						PgBase_Item KeyItem;
						SItemPos KeyItemPos;
						if( SUCCEEDED(pInven->GetFirstItem(ScheduleData.NeedItemNo, KeyItemPos)) )
						{
							if( SUCCEEDED(pInven->GetItem(KeyItemPos, KeyItem)) )
							{
								SPMO IMO(IMET_MODIFY_COUNT | IMC_DEC_DUR_BY_USE, pPlayer->GetID(), SPMOD_Modify_Count(KeyItem, KeyItemPos, -ScheduleData.NeedItemCount));
								Order.push_back(IMO);
							}
						}
					}
				}
			}
		}
	}

	CONT_EVENT_MAP_ENTERABLE_ENTER_LIST::iterator map_iter = m_EventMapEnterableList.find(EventNo);
	if( map_iter != m_EventMapEnterableList.end() )
	{
		if( !(*map_iter).second )
		{
			return EEMM_LIMIT_EVENT_TIME;
		}
	}
	else
	{
		return EEMM_LIMIT_EVENT_TIME;
	}

	if( ScheduleData.UserCountMax > 0 )
	{
		CONT_CURRENT_EVENT_MAP_USER_COUNT_LIST::iterator usercount_iter = m_CurrentEventMapUserCountList.find(EventNo);
		if( usercount_iter != m_CurrentEventMapUserCountList.end() )
		{
			int RequestMoveUserCount = 0;
			if( bParty )
			{
				RequestMoveUserCount = m_kLocalPartyMgr.GetMemberCount(pPlayer->PartyGuid());
			}
			else
			{
				RequestMoveUserCount = 1;
			}
			if( (*usercount_iter).second + RequestMoveUserCount > ScheduleData.UserCountMax )
			{
				return EEMM_LIMIT_MAP_FULL;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// �˻� ������ ��� ��������� ä�� ���̵��� �����־�� �Ѵ�(100��ä��-������ ä��).//
	//////////////////////////////////////////////////////////////////////////////////////

	if( BM::GUID::IsNotNull(pPlayer->PartyGuid()) )
	{	// ��Ƽ�� ������ �ִ�.
		BM::Stream ToCenter(PT_M_N_PRE_MAKE_PARTY_EVENT_GROUND);
		ToCenter.Push(EXPEDITION_CHANNEL_NO);
		ToCenter.Push(pPlayer->PartyGuid());
		SendToContents(ToCenter);

		BM::GUID MasterGuid;
		m_kLocalPartyMgr.GetPartyMasterGuid(pPlayer->PartyGuid(), MasterGuid);

		// ��Ƽ �����͸� ���� �̵���Ŵ.
		MoveToEventGround(ScheduleData.EventNo, pPlayer, ScheduleData.EventGround, true, true);
		
		VEC_GUID::iterator vec_iter = PartyMember.begin();
		for( ; vec_iter != PartyMember.end() ; ++vec_iter )
		{
			PgPlayer * pPartyMember = GetUser((*vec_iter));
			if( pPartyMember 
				&& (pPartyMember->GetID() != MasterGuid) )
			{
				MoveToEventGround(ScheduleData.EventNo, pPartyMember, ScheduleData.EventGround, false, true);
			}
		}
	
		if( !Order.empty() )
		{
			PgAction_ReqModifyItem ItemModifyAction(MIE_UseItem, GroundKey(), Order);
			ItemModifyAction.DoAction(pPlayer, NULL);
		}
	}
	else
	{	// ��Ƽ�� ����.
		MoveToEventGround(ScheduleData.EventNo, pPlayer, ScheduleData.EventGround);

		if( !Order.empty() )
		{
			PgAction_ReqModifyItem ItemModifyAction(MIE_UseItem, GroundKey(), Order);
			ItemModifyAction.DoAction(pPlayer, NULL);
		}
	}

	return EEMM_SUCCESS;
}

void PgGround::MoveToEventGround(int EventNo, PgPlayer * pPlayer, int GroundNo, bool bMaster, bool bSendCenter)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	SChannelMapMove MoveInfo;
	MoveInfo.iChannelNo = EXPEDITION_CHANNEL_NO;
	MoveInfo.iGroundNo = GroundNo;

	if( bSendCenter )
	{
		BM::Stream ToEventMap(PT_M_N_REQ_JOIN_ANOTHER_CHANNEL_PARTY);
		ToEventMap.Push(EXPEDITION_CHANNEL_NO);
		ToEventMap.Push(GroundNo);
		ToEventMap.Push(pPlayer->GetID());
		ToEventMap.Push(pPlayer->PartyGuid());
		ToEventMap.Push(bMaster);

		SendToContents(ToEventMap);
	}

	SRecentPlayerNormalMapData MapData(this->GetGroundNo(), g_kProcessCfg.ChannelNo(), pPlayer->GetPos());
	pPlayer->SetRecentNormalMap(MapData);

	// ����ġ�� ����.
	BM::Stream Packet(PT_M_L_TRY_LOGIN_CHANNELMAPMOVE);
	Packet.Push(pPlayer->GetID());
	Packet.Push(MoveInfo);
	SendToServer(pPlayer->GetSwitchServer(), Packet);

	// Ŭ��� ����.
	BM::Stream UserPacket(PT_M_C_TRY_LOGIN_CHANNELMAPMOVE);
	UserPacket.Push(MoveInfo);
	pPlayer->Send(UserPacket);

	CONT_CURRENT_EVENT_MAP_USER_COUNT_LIST::iterator find_iter = m_CurrentEventMapUserCountList.find(EventNo);
	if( find_iter == m_CurrentEventMapUserCountList.end() )
	{
		m_CurrentEventMapUserCountList.insert( std::make_pair( EventNo, 1) );
	}
	else
	{
		++(*find_iter).second;
	}
}

void PgGround::AddJoinPartyMemberList(BM::Stream & Packet)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	BM::GUID CharGuid;
	BM::GUID PartyGuid;
	bool bMaster;

	Packet.Pop(CharGuid);
	Packet.Pop(PartyGuid);
	Packet.Pop(bMaster);

	SJoinPartyMember JoinPartyMember;
	JoinPartyMember.CharGuid = CharGuid;
	JoinPartyMember.PartyGuid = PartyGuid;
	JoinPartyMember.bMaster = bMaster;

	auto Ret = m_JoinPartyMemberList.insert( std::make_pair(CharGuid, JoinPartyMember) );
	if( !Ret.second )
	{
		// �ߺ�?
	}
}

void PgGround::DelJoinPartyMemberList(BM::GUID & CharGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	m_JoinPartyMemberList.erase(CharGuid);
}

void PgGround::CheckJoinPartyMemberList(CUnit * pUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_JOIN_PARTY_MEMBER_LIST::iterator iter = m_JoinPartyMemberList.find(pUnit->GetID());
	if( iter != m_JoinPartyMemberList.end() )
	{
		BM::Stream ToCenter(PT_M_T_REQ_JOIN_EVENT_GROUND_PARTY);
		ToCenter.Push(iter->second.CharGuid);
		ToCenter.Push(iter->second.PartyGuid);
		ToCenter.Push(iter->second.bMaster);

		SendToGlobalPartyMgr(ToCenter);
	}
}


DWORD const PgGround::Constellation_CheckCondition(PgPlayer *pPlayer, Constellation::ConstellationKey const& Key, bool const UseCashItem, int & NeedItemNo, int & NeedItemCount)
{
	DWORD dwRet = Constellation::ECE_OK;

	if( NULL == pPlayer )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	int const GroundNo = GetGroundNo();

	Constellation::CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	Constellation::CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(GroundNo);
	if( ContConstellation.end() == iter )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM; 
	}

	Constellation::CONT_CONSTELLATION_ENTER::const_iterator sub_iter = iter->second.find(Key);
	if( iter->second.end() == sub_iter )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM; 
	}

	if( pPlayer->HaveParty() )
	{// ��Ƽ�常 üũ�ϸ� �ȴ�.
		BM::GUID MasterGuid;
		if( false == m_kLocalPartyMgr.GetPartyMasterGuid( pPlayer->PartyGuid(), MasterGuid ) )
		{
			return dwRet |= Constellation::ECE_ERROR_SYSTEM;
		}
		
		if( pPlayer->GetID() != MasterGuid )
		{
			return dwRet |= Constellation::ECE_ERROR_PARTYMASTER;
		}
	}
	else
	{// ��Ƽ�� ������ ������ �� ����.
		return dwRet |= Constellation::ECE_ERROR_NO_PARTY;
	}

	dwRet |= Constellation_CheckCondition_Level(pPlayer, sub_iter->second);
	dwRet |= Constellation_CheckCondition_Quest(pPlayer, sub_iter->second);
	if (dwRet != Constellation::ECE_OK)
		return dwRet;

	if( UseCashItem )
	{
		if( !Constellation_CheckCondition_CashItem(pPlayer, NeedItemNo, NeedItemCount ) )
		{// ĳ�������� ����üũ
			return Constellation::ECE_ERROR_CASH_ITEM;
		}
		return Constellation::ECE_OK;
	}

	dwRet |= Constellation_CheckCondition_DayofWeek(sub_iter->second);		// ���� üũ
	dwRet |= Constellation_CheckCondition_Item(pPlayer, sub_iter->second, NeedItemNo, NeedItemCount);	// ������ üũ

	return dwRet;
}

bool PgGround::Constellation_CheckCondition_CashItem(PgPlayer *pPlayer, int & NeedItemNo, int & NeedItemCount)
{
	if( NULL == pPlayer )
	{
		return false;
	}

	Constellation::CONT_CONSTELLATION_ENTRANCE_CASH_ITEM const& ContCashItem = g_kGndMgr.m_kGndRscMgr.GetConstellationCashItem();
	Constellation::CONT_CONSTELLATION_ENTRANCE_CASH_ITEM::const_iterator iter = ContCashItem.find(GetGroundNo());
	if( ContCashItem.end() == iter )
	{
		return false;
	}

	PgInventory * pInv = pPlayer->GetInven();
	if( NULL == pInv )
	{
		return false;
	}

	NeedItemCount = 1;
	if( pPlayer->HaveParty() )
	{
		if( 3 <= m_kLocalPartyMgr.GetMemberCount(pPlayer->PartyGuid()) )
		{
			NeedItemCount = 2;
		}
	}

	NeedItemNo = 0;
	int ContSize = iter->second.ItemNo.size() - 1;
	while( ContSize >= 0 )
	{
		int CashItemCount = pInv->GetTotalCount(iter->second.ItemNo.at(ContSize));
		if( NeedItemCount <= CashItemCount )
		{
			NeedItemNo = iter->second.ItemNo.at(ContSize);
			break;
		}
		--ContSize;
	}
	
	return (NeedItemNo > 0) ? true : false;
}

DWORD PgGround::Constellation_CheckCondition_DayofWeek(Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo)
{
	DWORD dwRet = Constellation::ECE_OK;

	SYSTEMTIME sysTime;
	g_kEventView.GetLocalTime(&sysTime);

	SDAYOFWEEK DayofWeek(EDOW_NONE);
	DayofWeek.SetDayofWeek(MissionInfo.Condition.DayofWeek);

	if( false == DayofWeek.IsInDay(static_cast<EDAYOFWEEK>(eDayOfWeekIdx[sysTime.wDayOfWeek])) )
	{
		dwRet |= Constellation::ECE_ERROR_DAY;
	}
	
	return dwRet;
}

DWORD PgGround::Constellation_CheckCondition_Level(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo)
{
	DWORD dwRet = Constellation::ECE_OK;

	if( NULL == pPlayer )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	int const LimitMaxLevel = MissionInfo.Condition.MaxLevel;
	int const LimitMinLevel = MissionInfo.Condition.MinLevel;

	if( 0 == LimitMaxLevel && 0 == LimitMinLevel )
	{// ����üũ�� �� �ʿ䰡 ����.
		return Constellation::ECE_OK;
	}

	int const MyLevel = pPlayer->GetAbil(AT_LEVEL);

	if( LimitMaxLevel && LimitMaxLevel < MyLevel )
	{// �ִ뷹��  ���ѿ� �ɸ���.
		dwRet |= Constellation::ECE_ERROR_LV_MAX;
	}
	if( LimitMinLevel && LimitMinLevel > MyLevel )
	{// �ּҷ��� ���ѿ� �ɸ���.
		dwRet |= Constellation::ECE_ERROR_LV_MIN;
	}

	return dwRet;
}

DWORD PgGround::Constellation_CheckCondition_Item(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo, int & NeedItemNo, int & NeedItemCount)
{
	DWORD dwRet = Constellation::ECE_OK;

	if( NULL == pPlayer )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	NeedItemNo = MissionInfo.Condition.ItemNo;
	NeedItemCount = MissionInfo.Condition.ItemCount;

	if( 0 == NeedItemNo )
	{// üũ�� �������� ����.
		return Constellation::ECE_OK;
	}

	if( pPlayer->HaveParty() )
	{
		if( 3 <= m_kLocalPartyMgr.GetMemberCount(pPlayer->PartyGuid()) )
		{
			NeedItemCount = 2;
		}
	}

	CONT_DEFITEM const* pDefItem = NULL;
	g_kTblDataMgr.GetContDef(pDefItem);
	if( !pDefItem )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	CONT_DEFITEM::const_iterator def_item = pDefItem->find( NeedItemNo );
	if( pDefItem->end() == def_item )
	{// �������� �ʴ� �������̴�.
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	PgInventory * pInven = pPlayer->GetInven();
	if( NULL == pInven )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	int ItemCount = (int)pInven->GetTotalCount( NeedItemNo );
	if( NeedItemCount > ItemCount )
	{
		dwRet |= Constellation::ECE_ERROR_ITEM;
	}

	return dwRet;
}

DWORD PgGround::Constellation_CheckCondition_Quest(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo)
{
	DWORD dwRet = Constellation::ECE_OK;
	
	if( NULL == pPlayer )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	int const NeedQuestNo = MissionInfo.Condition.QuestNo;

	if( 0 == NeedQuestNo )
	{// üũ�� ����Ʈ�� ����.
		return Constellation::ECE_OK;
	}

	PgMyQuest const* pMyQuest = pPlayer->GetMyQuest();
	if( NULL == pMyQuest )
	{
		return dwRet |= Constellation::ECE_ERROR_SYSTEM;
	}

	if( (false == pMyQuest->IsEndedQuest(NeedQuestNo)) && (false == pMyQuest->IsIngQuest(NeedQuestNo)) )
	{
		return dwRet |= Constellation::ECE_ERROR_QUEST;
	}

	return dwRet;
}

bool PgGround::Constellation_Consume_Item(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo
										  , PgReqMapMove & MapMove, int const& NeedItemNo, int const& NeedItemCount)
{
	if( NULL == pPlayer )
	{
		return false;
	}

	if( 0 == NeedItemNo )
	{// üũ�� �������� ����.
		return true;
	}

	PgInventory * pInven = pPlayer->GetInven();
	if( NULL == pInven )
	{
		return false;
	}

	int const MyItemCount = (int)pInven->GetTotalCount(NeedItemNo);	
	if( (MyItemCount > 0) && (0 == NeedItemCount) )
	{// �Һ����� �ʴ´�.
		return true;
	}

	if( NeedItemCount > MyItemCount )
	{
		return false;
	}

	SPMO IMO( IMET_ADD_ANY|IMC_DEC_DUR_BY_USE, pPlayer->GetID(), SPMOD_Add_Any(NeedItemNo, -NeedItemCount) );
	MapMove.AddModifyOrder(IMO);
	return true;
}

Constellation::ECE_RESULT PgGround::Constellation_MoveDungeon(PgPlayer *pPlayer, Constellation::ConstellationKey const& Key, int & NeedItemNo, int & NeedItemCount)
{
	if( NULL == pPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM;
	}

	int const GroundNo = GetGroundNo();

	Constellation::CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	Constellation::CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(GroundNo);
	if( ContConstellation.end() == iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM; 
	}

	Constellation::CONT_CONSTELLATION_ENTER::const_iterator sub_iter = iter->second.find(Key);
	if( iter->second.end() == sub_iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM; 
	}

	Constellation::CONT_MAP::value_type BossMap;
	Constellation::CONT_MAP ContMap = sub_iter->second.MapGroup.ContMap;
	Constellation::CONT_MAP::iterator map_iter = ContMap.begin();
	while( ContMap.end() != map_iter )
	{// ������ Ÿ���� ����
		if( Constellation::EMAP_BOSS == map_iter->MapType )
		{
			BossMap = *map_iter;
			map_iter = ContMap.erase(map_iter);
			continue;
		}
		++map_iter;
	}

	if( ContMap.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM;
	}

	std::random_shuffle( ContMap.begin(), ContMap.end(), BM::Rand_Index );

	SReqMapMove_MT RMM(MMET_None);

	RMM.kTargetKey.GroundNo(ContMap.begin()->ID);
	RMM.nTargetPortal = 1;

	PgReqMapMove MapMove( this, RMM, NULL );		// �� �̵��� �ؾ� ��.

	VEC_GUID Guid_List;
	if( pPlayer->HaveParty() )
	{// ��Ƽ�� ��� �̵����Ѿ� �Ѵ�.
		m_kLocalPartyMgr.GetPartyMemberGround(pPlayer->PartyGuid(), GroundKey(), Guid_List);
	}
	else
	{// ��Ƽ�� ������ �ȵȴ�. �����̶� ��Ƽ���·� �����ؾ���.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_NO_PARTY;
	}

	if( Guid_List.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM;
	}

	for( VEC_GUID::const_iterator user_iter = Guid_List.begin(); user_iter != Guid_List.end(); ++user_iter )
	{
		PgPlayer * pUser = GetUser(*user_iter);
		if( pUser )
		{
			bool const IsAdd = MapMove.Add(pUser); // �÷��̾� �߰�
			if( false == IsAdd )
			{// �Ѹ��̶� �����ϸ� �ȵȴ�.
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_MAPMOVE"));
				return Constellation::ECE_ERROR_MAPMOVE;
			}
		}
	}

	if( false == Constellation_Consume_Item(pPlayer, sub_iter->second, MapMove, NeedItemNo, NeedItemCount) )
	{// ���� ������ ���
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_MAPMOVE"));
		return Constellation::ECE_ERROR_MAPMOVE;
	}
	Constellation::SConstellationKey constellationKey(pPlayer->PartyGuid(), Key, GroundNo, ContMap.begin()->ID, BossMap.ID);
	MapMove.SetConstellationKey(constellationKey);
	bool bRet = MapMove.DoAction();
	if( false == bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_MAPMOVE"));
		return Constellation::ECE_ERROR_MAPMOVE;
	}

	return Constellation::ECE_OK;
}

Constellation::ECE_RESULT PgGround::Constellation_MoveDungeon_Boss(PgPlayer *pPlayer, Constellation::SConstellationKey const& constellationKey)
{
	if( NULL == pPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM;
	}

	Constellation::CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	Constellation::CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(constellationKey.WorldGndNo);
	if( ContConstellation.end() == iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM; 
	}

	Constellation::CONT_CONSTELLATION_ENTER::const_iterator sub_iter = iter->second.find(constellationKey.Key);
	if( iter->second.end() == sub_iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM; 
	}

	Constellation::CONT_MAP::value_type BossMap;
	Constellation::CONT_MAP ContMap = sub_iter->second.MapGroup.ContMap;
	Constellation::CONT_MAP::iterator map_iter = ContMap.begin();
	while( ContMap.end() != map_iter )
	{// ������ Ÿ���� ����
		if( Constellation::EMAP_BOSS == map_iter->MapType )
		{
			BossMap = *map_iter;
			map_iter = ContMap.erase(map_iter);
			continue;
		}
		++map_iter;
	}

	if( ContMap.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM;
	}

	SReqMapMove_MT RMM(MMET_None);

	RMM.kTargetKey.GroundNo(BossMap.ID);
	RMM.nTargetPortal = 1;

	PgReqMapMove MapMove( this, RMM, NULL );		// �� �̵��� �ؾ� ��.

	VEC_GUID Guid_List;
	if( pPlayer->HaveParty() )
	{// ��Ƽ�� ��� �̵����Ѿ� �Ѵ�.
		m_kLocalPartyMgr.GetPartyMemberGround(pPlayer->PartyGuid(), GroundKey(), Guid_List);
	}
	else
	{// ��Ƽ�� ������ �ȵȴ�. �����̶� ��Ƽ���·� �����ؾ���.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_NO_PARTY;
	}

	if( Guid_List.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_SYSTEM"));
		return Constellation::ECE_ERROR_SYSTEM;
	}

	for( VEC_GUID::const_iterator user_iter = Guid_List.begin(); user_iter != Guid_List.end(); ++user_iter )
	{
		PgPlayer * pUser = GetUser(*user_iter);
		if( pUser )
		{
			bool const IsAdd = MapMove.Add(pUser); // �÷��̾� �߰�
			if( false == IsAdd )
			{// �Ѹ��̶� �����ϸ� �ȵȴ�.
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_MAPMOVE"));
				return Constellation::ECE_ERROR_MAPMOVE;
			}
		}
	}

	MapMove.SetConstellationKey(constellationKey);
	bool bRet = MapMove.DoAction();
	if( false == bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return : ECE_ERROR_MAPMOVE"));
		return Constellation::ECE_ERROR_MAPMOVE;
	}

	return Constellation::ECE_OK;
}

void PgGround::Constellation_NotifyResult(PgPlayer *pPlayer, DWORD const dwRet, int const worldNo, Constellation::ConstellationKey const& Key)
{
	if( NULL == pPlayer )
	{
		return;
	}

	BM::Stream Packet(PT_M_C_NFY_ENTER_CONSTELLATION);
	Packet.Push(dwRet);
	Packet.Push(worldNo);
	Packet.Push(Key.kPriKey); // Position
	Packet.Push(Key.kSecKey); // Difficulty

	VEC_GUID Guid_List;
	if( pPlayer->HaveParty() )
	{// ��Ƽ�� ������ ��Ƽ�� ��ο��� ����
		m_kLocalPartyMgr.GetPartyMemberGround(pPlayer->PartyGuid(), GroundKey(), Guid_List);
	}
	else
	{
		Guid_List.push_back(pPlayer->GetID());
	}

	if( false == Guid_List.empty() )
	{
		SendToUser_ByGuidVec(Guid_List, Packet);
	}
}

void PgGround::Constellation_GetMapNo(VEC_INT & ContMapNo)
{
	int const GroundNo = GetGroundNo();

	Constellation::CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	Constellation::CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(GroundNo);
	if( ContConstellation.end() == iter )
	{
		return;
	}

	Constellation::CONT_CONSTELLATION_ENTER::const_iterator sub_iter = iter->second.begin();
	while( iter->second.end() != sub_iter )
	{
		Constellation::CONT_MAP::const_iterator map_iter = sub_iter->second.MapGroup.ContMap.begin();
		while( sub_iter->second.MapGroup.ContMap.end() != map_iter )
		{
			if( Constellation::EMAP_BOSS == map_iter->MapType )
			{
				++map_iter;
				continue;
			}
			VEC_INT::iterator cont_iter = std::find( ContMapNo.begin(), ContMapNo.end(), map_iter->ID );
			if( cont_iter == ContMapNo.end() )
			{
				ContMapNo.push_back(map_iter->ID);
			}
			++map_iter;
		}
		++sub_iter;
	}
}

void PgGround::ProcessGateWayUnLock(HRESULT const hRet, CUnit * pkCaster, BM::Stream & rkAddonPacket)
{
	int iErrorMessageNo = 0;
	rkAddonPacket.Pop( iErrorMessageNo );

	if(NULL == pkCaster)
	{
		return;
	}

	if( S_OK == hRet )
	{
		pkCaster->VNotify( &rkAddonPacket );
	}
	else
	{
		if( 0 != iErrorMessageNo )
		{
			pkCaster->SendWarnMessage( iErrorMessageNo );
		}
		else
		{
			pkCaster->SendWarnMessage( 18998 );
		}
	}
}