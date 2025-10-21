#include "stdafx.h"
#include "BM/PgTask.h"
#include "BM/pgfilterstring.h"
#include "lohengrin/DBTables.h"
#include "Lohengrin/VariableContainer.h"
#include "lohengrin/errorcode.h"
#include "Collins/Log.h"
#include "Variant/PgLogUtil.h"
#include "Variant/DefAbilType.h"
#include "variant/constant.h"
#include "Variant/PgMission.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "variant/item.h"
#include "variant/MonsterDefMgr.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/GroundEffect.h"
#include "variant/TableDataManager.h"
#include "variant/PgClassDefMgr.h"
#include "variant/PgStoreMgr.h"
#include "variant/ItemBagMgr.h"
#include "variant/PgItemOptionMgr.h"
#include "variant/PgEventView.h"
#include "Variant/PgActionResult.h"
#include "Variant/pgitemrarityupgradeformula.h"
#include "variant/PgGambleMachine.h"
#include "Variant/InventoryUtil.h"
#include "Variant/PetSkillDataConverter.h"
#include "Variant/PgJobSkillTool.h"
#include "Variant/PgJobSkill.h"
#include "Variant/PgSoulTransfer.h"
#include "Variant/Emoticon.h"
#include "constant.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgAction_Pet.h"
#include "PgQuest.h"
#include "PgStoneMgr.h"
#include "pgtask_MapServer.h"
#include "PgPartyItemRule.h"
#include "PgExpeditionItemRule.h"
#include "SkillEffectAbilSystem/PgSkillAbilHandleManager.h"
#include "PgActionUserMarket.h"
#include "PgActionAchievement.h"
#include "PgGroundMgr.h"
#include "PgEffectAbilTable.h"
#include "PgRequest.h"
#include "PgActionJobSkill.h"
#include "XUI/XUI_Font.h"
#include "PgStaticEventGround.h"
#include <Variant/math/re_math.h>

extern void ProcessReqMapMove(CUnit* pkPlayer,SReqMapMove_CM const &kRMM);
extern void ProcessReqMapMove(CUnit* pkCaster,SReqMapMove_MT const& kRMM);

static int const g_aiDefenceCandidate[] = {//���� ���̴� ���.
		EQUIP_POS_HELMET,							EQUIP_POS_EARRING,
		EQUIP_POS_SHOULDER,							//������ �ȱ�.	EQUIP_POS_MEDAL
		
		EQUIP_POS_SHIRTS,		EQUIP_POS_FACE,		EQUIP_POS_NECKLACE,
		EQUIP_POS_GLOVE,		EQUIP_POS_CLOAK,	EQUIP_POS_BELT,

		EQUIP_POS_PANTS,		EQUIP_POS_SHEILD,	EQUIP_POS_RING_L,
		EQUIP_POS_BOOTS,	/*EQUIP_POS_WEAPON,*/	EQUIP_POS_RING_R,	
		EQUIP_POS_KICKBALL, };

static int const g_aiDeathPenaltyCandidate[] = {//����� ���̴� ���.
		EQUIP_POS_HELMET,							EQUIP_POS_EARRING,
		EQUIP_POS_SHOULDER,							//������ �ȱ�.	EQUIP_POS_MEDAL
		
		EQUIP_POS_SHIRTS,		EQUIP_POS_FACE,		EQUIP_POS_NECKLACE,
		EQUIP_POS_GLOVE,		EQUIP_POS_CLOAK,	EQUIP_POS_BELT,

		EQUIP_POS_PANTS,		EQUIP_POS_SHEILD,	EQUIP_POS_RING_L,
		EQUIP_POS_BOOTS,		EQUIP_POS_WEAPON,	EQUIP_POS_RING_R,	
		EQUIP_POS_KICKBALL, };

inline void DoAction_SetAchievementValue(PgPlayer * pkPlayer, EAbilType const eType, int const iValue)
{
	if(pkPlayer)
	{
		PgSetAchievementValue kSA(eType, iValue, pkPlayer->GroundKey());
		kSA.DoAction( pkPlayer, NULL );
	}
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_StoreItemBuyResult
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqAddMoney::PgAction_ReqAddMoney(EItemModifyParentEventType const kCause, __int64 const iAddMoney, SGroundKey const &kGroundKey, BM::Stream const& rkPacket)
	:	m_kCause(kCause), m_iAddMoney(iAddMoney), m_kGndKey(kGroundKey), m_kPacket(rkPacket)
{
}

bool PgAction_ReqAddMoney::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( 0 == m_iAddMoney )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( m_iAddMoney > 0 )
	{// �׾����� �Һ�� �Ǿ�� �ϱ⶧����....
		if(pkCaster->IsDead())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false; // �׾������� �� �Դ´�
		}
	}
	
	CONT_PLAYER_MODIFY_ORDER kOrder;
	SPMOD_Add_Money kDelMoneyData(m_iAddMoney);//�ʿ�Ӵ� ����.
	SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
	kOrder.push_back(kIMO);

	PgAction_ReqModifyItem kItemModifyAction(m_kCause, m_kGndKey, kOrder, m_kPacket, true);//������ ������� �˻� �ϹǷ� ���� ���¸� ���� �ض�.
	return kItemModifyAction.DoAction(pkCaster, pkTarget);
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_StoreItemBuyResult
//////////////////////////////////////////////////////////////////////////////
PgAction_StoreItemBuyResult::PgAction_StoreItemBuyResult(HRESULT const hRet, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, __int64 const iMoney)
	:	m_hRet(hRet), m_kChangeArray(kChangeArray), m_iMoney(iMoney)
{
}

bool PgAction_StoreItemBuyResult::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	switch(m_hRet)
	{
	case E_NOT_ENOUGH_MONEY:
		{
			pkCaster->SendWarnMessage(700036);
		}break;
	case E_NOT_ENOUGH_INV_SPACE:
		{
			pkCaster->SendWarnMessage(700037);
		}break;
	}

//	PgAction_ModifyItem kAction(m_kChangeArray, MIE_AddItem);
//	kAction.DoAction(pkCaster,NULL);

//	PgAction_ModifyMoney kMoneyAction(MCE_BuyItem, m_iMoney);
//	kMoneyAction.DoAction(pkCaster, NULL);

	return false;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_StoreItemSellResult
//////////////////////////////////////////////////////////////////////////////
PgAction_StoreItemSellResult::PgAction_StoreItemSellResult(HRESULT const hRet, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, __int64 const iMoney)
	:	m_hRet(hRet), m_kChangeArray(kChangeArray), m_iMoney(iMoney)
{
}

bool PgAction_StoreItemSellResult::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	switch(m_hRet)
	{
	case S_OK:
		{//����
			
		}break;
	}

//	PgAction_ModifyItem kAction(m_kChangeArray, MIE_Move);
//	kAction.DoAction(pkCaster,NULL);

//	PgAction_ModifyMoney kMoneyAction(MCE_SellItem, m_iMoney);
//	kMoneyAction.DoAction(pkCaster, NULL);

	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqStoreItemBuy
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqStoreItemBuy::PgAction_ReqStoreItemBuy(BM::GUID const &kReqShopGuid, BYTE const byType, BYTE const bySecondType, int const iItemNo, int const iCount, SGroundKey const &kGroundKey, PgEventAbil const * const pkEventAbil )
	:m_kReqShopGuid(kReqShopGuid), m_byType(byType), m_bySecondType(bySecondType), m_iItemNo(iItemNo), m_iCount(iCount), m_kGndKey(kGroundKey)
	,	m_pkEventAbil(pkEventAbil)
{
}

int const GADA_COIN_NO = 20200188;

bool PgAction_ReqStoreItemBuy::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( 0 >= m_iCount  )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgStore kOutStore;
	GET_DEF(PgStoreMgr, kStoreMgr);
	if(S_OK == kStoreMgr.FindStore(m_kReqShopGuid, m_byType, kOutStore))
	{//������. ���� ��.
		PgStore::CONT_GODDS::value_type kGoods;
		if(S_OK == kOutStore.GetGoods(m_iItemNo, kGoods))
		{
			if( kGoods.iState)
			{//�Ǹ� ����.
				return false;
			}

			int iCount = m_iCount;

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(m_iItemNo);
			if(pItemDef)
			{
				if( !pItemDef->IsAmountItem() )//������ �������� 1���� ����
				{
					iCount = 1;
				}

				CONT_PLAYER_MODIFY_ORDER kOrder;
				if( 0 >= (kGoods.iPrice + kGoods.iCP + kGoods.iCoin) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				if( kGoods.iPrice )
				{
					__int64 const i64TotalPrice = kGoods.iPrice * iCount;
					if( i64TotalPrice <= pkCaster->GetAbil64(AT_MONEY) )
					{
						SPMOD_Add_Money kDelMoneyData(-i64TotalPrice);//�ʿ�Ӵ� ����.
						SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
						kOrder.push_back(kIMO);
					}
					else
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}

				if( m_byType != STORE_TYPE_JOBKSILL
				&&	kGoods.iCP )
				{
					int const iTotalCP = kGoods.iCP * iCount;
					if( iTotalCP <= pkCaster->GetAbil(AT_CP) )
					{
						SPMOD_Add_CP kDelCPData(-iTotalCP);//�ʿ� CP ����.
						SPMO kIMO(IMET_ADD_CP, pkCaster->GetID(), kDelCPData);
						kOrder.push_back(kIMO);
					}
					else
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}

				if( kGoods.iCoin )
				{
					int const iTotalCoin = kGoods.iCoin * iCount;
					size_t const kCoinCount = pkCaster->GetInven()->GetTotalCount(GADA_COIN_NO);

					if( iTotalCoin <= static_cast<int>(kCoinCount) )
					{						
						{//�������� ��� ������ ���� ����
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_GADACOIN, iTotalCoin, m_kGndKey );
							kMA.DoAction( pkCaster, NULL );
						}

						SPMOD_Add_Any kDelData(GADA_COIN_NO, -iTotalCoin);
						SPMO kIMO(IMET_ADD_ANY, pkCaster->GetID(), kDelData);
						kOrder.push_back(kIMO);
					}
					else
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}

				{
					int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
					while( iCount > 0 )
					{
						PgBase_Item kItem;
						if(		UICT_GAMBLE == iCustomType
							&&	pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_2 ) )
						{
							// �׺� �������̴�~
							if ( S_OK != PgAction_ReqOpenGamble::PopItem( pkCaster, pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1), kItem, m_pkEventAbil ) )
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}

							if ( 1 < iCount )
							{// Ȥ�� �� ���� ó��
								kItem.Count(iCount);
								iCount = 1;
							}
						}
						else
						{
							if( E_FAIL == CreateSItem( m_iItemNo, iCount, GIOT_NONE, kItem ) )
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}

							if( 0 < kGoods.iUseTime )
							{
								if( !kItem.SetUseTime( kGoods.bTimeType, kGoods.iUseTime ) )
								{
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}
						}

						tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, SItemPos(), true);
						SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddItem);
						kOrder.push_back(kIMO);
						
						if( pItemDef->IsAmountItem() )
						{
							iCount -= kItem.Count();
						}
						else
						{
							--iCount;
						}
					}
				}

				EItemModifyParentEventType kCause = MCE_BuyItem;
				if(ESST_COIN_CHANGE==m_bySecondType && kGoods.iCoin)
				{
					kCause = CIE_CoinChange_Buy;
				}
				PgAction_ReqModifyItem kItemModifyAction(kCause, m_kGndKey, kOrder);
				kItemModifyAction.DoAction(pkCaster, pkTarget);
				return true;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqStoreItemSell
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqStoreItemSell::PgAction_ReqStoreItemSell(SItemPos const &kItemPos, int const iItemNo, int const iCount, SGroundKey const &kGroundKey, BM::GUID const &kShopGuid, BYTE const byType, bool const bIsStockShop)
	:m_kItemPos(kItemPos), m_iItemNo(iItemNo), m_iCount(iCount), m_kGndKey(kGroundKey), m_kShopGuid(kShopGuid), m_byType(byType), m_bIsStockShop(bIsStockShop)
{
}

bool PgAction_ReqStoreItemSell::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iErrMsgNo = 700115;
	PgInventory* pkInv = pkCaster->GetInven();
	//�Ⱦ����� ����. ��. ��¥���� �����ۼ����� �Ǵ��ض�.
	if(pkInv)
	{
		PgBase_Item kItem;
		if(	SUCCEEDED(pkInv->GetItem(m_kItemPos, kItem))
		&&	kItem.ItemNo()
		&&	kItem.Count() )
		{
			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
			if(!pItemDef)
			{
				goto __FAILED;
			}

			if(0 >= m_iCount)
			{
				goto __FAILED;
			}

			PgStore kOutStore;
			GET_DEF(PgStoreMgr, kStoreMgr);
			if(S_OK != kStoreMgr.FindStore(m_kShopGuid, m_byType, kOutStore))
			{
				goto __FAILED;
			}
			
			int iStockSellPrice = 0;

			if(	(ICMET_Cant_ShopSell & pItemDef->GetAbil(AT_ATTRIBUTE)))
			{
				if( STORE_TYPE_STOCK == kOutStore.GetType())
				{//�ֽ� ���̳�.
					PgStore::CONT_GODDS::value_type kGoods;
					if(S_OK == kOutStore.GetGoods(kItem.ItemNo(), kGoods))
					{//�ֽļ����� �־�ߵ�.
						iStockSellPrice = kGoods.iSellPrice;
					}
					else
					{
						goto __FAILED;
					}
				}
				else
				{
					goto __FAILED;
				}
			}
			
			if( kItem.EnchantInfo().IsBinding())
			{//���ε� �Ǹ� ���Ǵ�.
				goto __FAILED;
			}

			int iSellPrice = pItemDef->SellPrice();
			if(!iSellPrice 
			&& !iStockSellPrice)//�Ѵ� ���� ������ �ǸŰ� �ȵ�
			{
				goto __FAILED;
			}

			if(iStockSellPrice)
			{
				iSellPrice = iStockSellPrice;//�ֽ� ������ ����.
			}

			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
			if( !pkPlayer )
			{
				goto __FAILED;
			}

			__int64 iEarnMoney = 0;

			if(S_OK == CalcSellMoney(kItem, iEarnMoney, iSellPrice, m_iCount, pkPlayer->GetPremium()))
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;
				{
					SPMOD_Add_Money kDelMoneyData(iEarnMoney);//�� ����.
					SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
					kOrder.push_back(kIMO);
				}

				if( 0 < iEarnMoney )
				{// �Ǹ��� ������ �ݾ� �˻�
					PgAddAchievementValue kMA(AT_ACHIEVEMENT_BESTSELLER,iEarnMoney,m_kGndKey);
					kMA.DoAction(pkCaster,NULL);
				}

				{
					bool const bIsAmountItem = pItemDef->IsAmountItem();
					bool bIsDel = false;
					if(bIsAmountItem)
					{
						if(kItem.Count() == m_iCount)
						{
							bIsDel = true;
						}
					}
					else
					{//������ �������� ������ ����.
						bIsDel = true;
					}

					SPMOD_Modify_Count kAddItem(kItem, m_kItemPos, -m_iCount, bIsDel);
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_SHOP_SELL, pkCaster->GetID(), kAddItem);
					kOrder.push_back(kIMO);
				}
				
				PgAction_ReqModifyItem kItemModifyAction(MCE_SellItem, m_kGndKey, kOrder);
				kItemModifyAction.DoAction(pkCaster, pkTarget);
			}
		}
		return true;
	}

__FAILED:
	{
		pkCaster->SendWarnMessage(700115);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqStoreItemList
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqStoreItemList::PgAction_ReqStoreItemList(BM::GUID const &kReqShopGuid, BYTE const byType)
	:m_kReqShopGuid(kReqShopGuid), m_byType(byType)
{
}

bool PgAction_ReqStoreItemList::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	PgStore kOutStore;
	GET_DEF(PgStoreMgr, kStoreMgr);
	if(S_OK == kStoreMgr.FindStore(m_kReqShopGuid, m_byType, kOutStore))
	{
		BM::Stream kPacket(PT_M_C_ANS_STORE_ITEM_LIST);
		kOutStore.WriteToPacket(kPacket);
		pkCaster->Send(kPacket);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_SendItemChange
//////////////////////////////////////////////////////////////////////////////
PgAction_SendItemChange::PgAction_SendItemChange(EItemModifyParentEventType const kCause, SGroundKey const& kGndKey, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, PgLocalPartyMgr &kLocalPartyMgr)
	: m_kCause(kCause), m_kGndKey(kGndKey), m_kChangeArray(kChangeArray), m_kLocalPartyMgr(kLocalPartyMgr)
{
}

bool PgAction_SendItemChange::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{	
	VEC_GUID kPartyMemberArray;

	DB_ITEM_STATE_CHANGE_ARRAY kPartyBBSArray;
	DB_ITEM_STATE_CHANGE_ARRAY kBBSArray;

	DB_ITEM_STATE_CHANGE_ARRAY::const_iterator change_itor = m_kChangeArray.begin();
	while(m_kChangeArray.end() != change_itor)
	{
		DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkItemChange = (*change_itor);

		BYTE const kType = rkItemChange.ItemWrapper().Pos().x;
		
		switch ( pkCaster->UnitType() )
		{
		case UT_MYHOME:
			{
				if(DISCT_MYHOME_MODIFY != (*change_itor).State())
				{
					kBBSArray.push_back(rkItemChange);
				}
			}break;
		case UT_PET:
			{
				if(IT_FIT == kType || IT_FIT_CASH == kType || DISCT_SET_DEFAULT_ITEM == (*change_itor).State())
				{
					kBBSArray.push_back(rkItemChange);
				}
			}break;
		case UT_PLAYER:
			{
				if(IT_FIT == kType || IT_FIT_CASH == kType || IT_FIT_COSTUME == kType || DISCT_SET_DEFAULT_ITEM == (*change_itor).State())
				{
					kBBSArray.push_back(rkItemChange);
				}
				else
				{
					//FIT �� ������ ���ϱ� �ߺ��ؼ� �Ⱥ����� ����.
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if ( pkPlayer )
					{
						//g_kLocalPartyMgr.GetPartyMemberGround(pkPlayer->PartyGuid(), GndKey(), kPartyMemberArray, pkPlayer->GetID());
						if(kPartyMemberArray.empty())
						{
							m_kLocalPartyMgr.GetPartyMemberGround(pkPlayer->PartyGuid(), GndKey(), kPartyMemberArray, pkPlayer->GetID());	
						}
						if(kPartyMemberArray.size())
						{//Create �� Modify �� ���� �̺�Ʈ�ε� ������ �����ϳ�?
							kPartyBBSArray.push_back(rkItemChange);
						}
					}	
				}
			}break;
		}

		++change_itor;
	}

	{// �ڽſ��� ����.
		BM::Stream kPacket(PT_M_C_NFY_ITEM_CHANGE);

		kPacket.Push(m_kCause);
		kPacket.Push(pkCaster->GetID());//--Push
		PU::TWriteArray_M(kPacket,m_kChangeArray);//--Push
///		kPacket.Push((BYTE)m_iEventType);//� ����
		pkCaster->Send(kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
	}

	if(kPartyBBSArray.size())
	{//��Ƽ���� ����.
		BM::Stream kTempPacket(PT_M_C_NFY_ITEM_CHANGE);
		kTempPacket.Push(m_kCause);
		kTempPacket.Push(pkCaster->GetID());//����
		PU::TWriteArray_M(kTempPacket, kPartyBBSArray);//��

		BM::Stream kNfyPacket(PT_U_G_SEND_TO_USERS);
		kNfyPacket.Push(kPartyMemberArray);
		kNfyPacket.Push(kTempPacket.Data());

		pkCaster->VNotify(&kNfyPacket);
	}

	//�Ϲ��ο��Ը� �˸�.
	if(kBBSArray.size())//�̰Ÿ� BBS
	{
		BM::Stream kBBSPacket(PT_M_C_NFY_ITEM_CHANGE);
		kBBSPacket.Push(m_kCause);
		kBBSPacket.Push(pkCaster->GetID());
		PU::TWriteArray_M(kBBSPacket,kBBSArray);//������ ������ �� �� �ʿ�� ���°� �����ѵ�.
		pkCaster->Send(kBBSPacket, E_SENDTYPE_BROADCAST);	
	}

	return true;
}

PgAction_ModifyItem_AddOnPacketProcess::PgAction_ModifyItem_AddOnPacketProcess( HRESULT const _hRet, PgGround * const pkGround, BM::Stream const &kAddonPacket)
	:m_kRet(_hRet), m_pkGround(pkGround), m_kAddonPacket(kAddonPacket)
{
}

bool PgAction_ModifyItem_AddOnPacketProcess::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(!m_kAddonPacket.IsEmpty())
	{
		WORD wType;
		m_kAddonPacket.Pop(wType);

		switch(wType)
		{
		case PT_C_M_ANS_ITEM_PLUS_UPGRADE:
			{
				BM::Stream kResult(wType);

				bool bNoti = false;
				m_kAddonPacket.Pop(bNoti);
				if(true == bNoti)
				{
					BM::Stream kPacket;
					m_kAddonPacket.Pop(kPacket.Data());
					kPacket.PosAdjust();
					SendToCenter( kPacket );
				}

				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_ENCHANT_SHIFT:
		case PT_M_C_ANS_MYHOME_SELL:
		case PT_M_C_ANS_RESET_ATTATCHED:
		case PT_M_C_ANS_SKILL_EXTEND:
		case PT_M_C_ANS_EXCHANGE_LOGCOUNTTOEXP:
		case PT_M_C_ANS_USE_UPGRADE_OPTION_ITEM:
		case PT_M_C_ANS_USE_REDICE_OPTION_ITEM:
		case PT_M_C_ANS_USE_REDICE_OPTION_PET:
		case PT_M_C_ANS_EVENT_ITEM_REWARD:
		case PT_M_C_ANS_TREASURE_CHEST:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_ENTER_SIDE_JOB:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);

				if(S_OK == Ret())
				{
					SReqMapMove_MT kRMM;

					m_kAddonPacket.Pop(kRMM);

					if ( !g_kProcessCfg.IsPublicChannel() )
					{
						kRMM.cType = MMET_GoToPublicGround;
					}

					PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if(pkPlayer)
					{
						PgReqMapMove kMapMove( m_pkGround, kRMM, NULL );
						if ( kMapMove.Add( pkPlayer ) )
						{
							kMapMove.DoAction();
						}
					}
				}
			}break;
		case PT_M_C_ANS_START_SIDE_JOB:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());

				PgMyHome * pkMyHome = dynamic_cast<PgMyHome *>(pkCaster);
				if(pkMyHome)
				{
					CUnit * pkUnit = m_pkGround->GetUnit(pkMyHome->OwnerGuid());
					if(pkUnit)
					{
						pkUnit->Send(kResult);
					}
				}
			}break;
		case PT_M_C_ANS_CANCEL_SIDE_JOB:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());

				PgMyHome * pkMyHome = dynamic_cast<PgMyHome *>(pkCaster);
				if(pkMyHome)
				{
					CUnit * pkUnit = m_pkGround->GetUnit(pkMyHome->OwnerGuid());
					if(pkUnit)
					{
						pkUnit->Send(kResult);
					}
				}
			}break;
		case PT_M_C_ANS_RELOAD_ROULETTE:
		case PT_M_C_ANS_ROULETTE_RESULT:
			{
				if(S_OK != Ret())
				{
					BM::Stream kResult(wType);
					kResult.Push(Ret());
					pkCaster->Send(kResult);
				}
			}break;
		case PT_M_C_ANS_USE_GAMBLEMACHINE_READY:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_COLLECT_ANTIQUE:
			{
				int iMenu = 0;
				int iItemNo = 0;
				int iIndex = 0;
				int iResultItemNo = 0;
				m_kAddonPacket.Pop(iMenu);
				m_kAddonPacket.Pop(iItemNo);
				m_kAddonPacket.Pop(iIndex);
				m_kAddonPacket.Pop(iResultItemNo);

				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(iResultItemNo);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_EXCHANGE_GEMSTORE:
			{
				int iResultItemNo = 0;
				m_kAddonPacket.Pop(iResultItemNo);

				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(iResultItemNo);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_USE_GAMBLEMACHINE:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());

				if(S_OK == Ret())
				{
					CONT_GAMBLEMACHINERESULT kContResult;
					PU::TLoadArray_M(m_kAddonPacket, kContResult);
					bool bBroadcast = false;
					m_kAddonPacket.Pop(bBroadcast);

					if(true == bBroadcast)
					{
						BM::Stream kNotiPacket(PT_M_C_NFY_GAMBLEMACHINE_RESULT);
						kNotiPacket.Push(pkCaster->Name());
						PU::TWriteArray_M(kNotiPacket, kContResult);
						SendToItem(m_pkGround->GroundKey(),kNotiPacket);
					}

					PU::TWriteArray_M(kResult, kContResult);

					CONT_PLAYER_MODIFY_ORDER kOrder;
					for(CONT_GAMBLEMACHINERESULT::const_iterator iter = kContResult.begin();iter != kContResult.end();++iter)
					{
						PgBase_Item const & kResultItem = (*iter);
						kOrder.push_back(SPMO(IMET_INSERT_FIXED,pkCaster->GetID(),SPMOD_Insert_Fixed(kResultItem,SItemPos(), true)));
					}

					PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine, m_pkGround->GroundKey(), kOrder);
					kItemModifyAction.DoAction(pkCaster, NULL);
				}

				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_HOME_ITEM_MODIFY:
		case PT_M_C_ANS_MYHOME_AUCTION_REG:
		case PT_M_C_ANS_MYHOME_AUCTION_UNREG:
		case PT_M_C_ANS_HOME_VISITFLAG_MODIFY:
		case PT_M_C_ANS_HOME_EQUIP:
		case PT_M_C_ANS_HOME_UNEQUIP:
		case PT_M_C_ANS_MYHOME_PAY_TEX:
		case PT_M_C_ANS_HOME_USE_ITEM_EFFECT:
		case PT_M_C_ANS_MYHOME_BIDDING:
		case PT_M_C_ANS_MIXUPITEM:
		case PT_M_C_ANS_MYHOME_BUY:
		case PT_M_C_ANS_EXPCARD_USE:
		case PT_M_C_ANS_CONVERTITEM:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_OPEN_GAMBLE:
			{
				if(S_OK == Ret())
				{
					// ���� �� �������̴�. �κ��� ������ ��츦 ����ؼ� ������ ���� ���� ������ ������ ���� ������ ������ �ٽ� ������ ������.
					// �ڽ����� ������ ������ �̷��� �ؾ߸� �ذᰡ��(�ް� ���� �������� ��� �޴� ���� ������...)
					CONT_PLAYER_MODIFY_ORDER kOrder;
					kOrder.ReadFromPacket(m_kAddonPacket);
					PgAction_ReqModifyItem kAction( CIE_Open_Gamble, m_pkGround->GroundKey(), kOrder);
					kAction.DoAction(pkCaster, NULL);
				}

				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);

				bool bBroadCast = false;
				m_kAddonPacket.Pop(bBroadCast);

				if(true == bBroadCast)
				{
					BM::Stream kNoti(PT_M_C_NOTI_OPEN_GAMBLE);
					kNoti.Push(m_kAddonPacket);
					SendToCenter(kNoti);
				}
			}break;
		case PT_M_C_ANS_HIDDEN_ITEM_PACK:
			{
				if(S_OK == Ret())
				{
					// ���� �� �������̴�. �κ��� ������ ��츦 ����ؼ� ������ ���� ���� ������ ������ ���� ������ ������ �ٽ� ������ ������.
					// �ڽ����� ������ ������ �̷��� �ؾ߸� �ذᰡ��(�ް� ���� �������� ��� �޴� ���� ������...)
					CONT_PLAYER_MODIFY_ORDER kOrder;
					kOrder.ReadFromPacket(m_kAddonPacket);
					PgAction_ReqModifyItem kAction( CIE_HiddenPack, m_pkGround->GroundKey(), kOrder);
					kAction.DoAction(pkCaster, NULL);
				}

				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_OPEN_LOCKED_CHEST:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);

				int iRewardType = 0,
					iItemNo = 0;

				m_kAddonPacket.Pop(iRewardType);
				m_kAddonPacket.Pop(iItemNo);

				if(S_OK == Ret())
				{
					switch(iRewardType)
					{
					case UICT_BLANK:
					case UICT_ONECEMORE:
						{
						}break;
					default:
						{// ���� �� �������̴�. �κ��� ������ ��츦 ����ؼ� ������ ���� ���� ������ ������ ���� ������ ������ �ٽ� ������ ������.
							// �ڽ����� ������ ������ �̷��� �ؾ߸� �ذᰡ��(�ް� ���� �������� ��� �޴� ���� ������...)
							CONT_PLAYER_MODIFY_ORDER kOrder;
							kOrder.ReadFromPacket(m_kAddonPacket);
							PgAction_ReqModifyItem kAction( CIE_Locked_Chest, m_pkGround->GroundKey(), kOrder);
							kAction.DoAction(pkCaster, NULL);
						}break;
					}
				}
			}break;
		case PT_M_C_ANS_ITEM_RARITY_UPGRADE:
			{
				BM::Stream kResult(wType);
				EItemRarityUpgradeResult kError = IRUR_NONE;

				m_kAddonPacket.Pop(kError);

				if(S_OK != Ret())
				{
					kError = IRUR_DB_ERROR;
				}

				kResult.Push(kError);
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_BASIC_OPTION_AMP:
			{
				BM::Stream kResult(wType);
				HRESULT kErr;
				m_kAddonPacket.Pop(kErr);

				if(S_OK != Ret())
				{
					kErr = E_BASICOPTIONAMP_DBERROR;
				}

				kResult.Push(kErr);
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_ROLLBACK_ENCHANT:
		case PT_M_C_ANS_REMOVE_MONSTERCARD:
		case PT_M_C_ANS_RESET_MONSTERCARD:
		case PT_M_C_ANS_SEAL_ITEM:
		case PT_M_C_ANS_GEMSTORE_BUY:
		case PT_M_C_ANS_REG_PORTAL:
		case PT_M_C_ANS_MODIFY_CHARACTER_CARD:
		case PT_M_C_ANS_MODIFY_CHARACTER_CARD_COMMENT:
		case PT_M_C_ANS_MODIFY_CHARACTER_CARD_STATE:
		case PT_M_C_ANS_INVENTORY_EXTEND:
		case PT_M_C_ANS_ITEM_ACTION_BIND:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_GEN_SOCKET:
		case PT_M_C_ANS_SET_MONSTERCARD:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_CREATE_CHARACTER_CARD:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);
				if(S_OK == Ret())	// �ֺ� ����鿡�� ī�� ������ �뺸�Ѵ�.
				{
					PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if(pkPlayer)
					{
						pkPlayer->IsCreateCard(true);
						BM::Stream kPacket(PT_M_C_NOTI_CREATE_CARD);
						kPacket.Push(pkPlayer->GetID());
						pkPlayer->Send(kPacket,E_SENDTYPE_BROADCAST);
					}
				}
			}break;
		case PT_M_C_ANS_RECOMMEND_CHARACTER:
			{
				BM::GUID kOwner;
				BYTE kType = 0;
				m_kAddonPacket.Pop(kOwner);
				m_kAddonPacket.Pop(kType);
				if(pkCaster->GetID() == kOwner)
				{
					BM::Stream kResult(wType);
					kResult.Push(kType);
					kResult.Push(Ret());					
					kResult.Push(m_kAddonPacket);
					pkCaster->Send(kResult);
				}
			}break;
		case PT_M_C_ANS_ITEM_ACTION_UNBIND:
			{
				DWORD dwItemNo;
				m_kAddonPacket.Pop(dwItemNo);

				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(dwItemNo);//���� ��ȣ
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_UM_ANS_MARKET_CLOSE:
		case PT_M_C_UM_ANS_MARKET_OPEN:
			{
				BM::Stream kResult(wType);
				kResult.Push(static_cast<EUserMarketResult>(Ret()));
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_UM_ANS_ARTICLE_REG:
		case PT_M_C_UM_ANS_ARTICLE_DEREG:
		case PT_M_C_UM_ANS_ARTICLE_BUY:
		case PT_M_C_UM_ANS_DEALINGS_READ:
		case PT_M_C_UM_ANS_MARKET_MODIFY_STATE:
		case PT_M_C_UM_ANS_USE_MARKET_MODIFY_ITEM:
			{
				BM::Stream kResult(wType);
				kResult.Push(static_cast<EUserMarketResult>(Ret()));
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_SYSTEM_INVENTORY_REMOVE:
		case PT_M_C_ANS_SYSTEM_INVENTORY_RECV:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_RENTALSAFE_EXTEND:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_SUMMONPARTYMEMBER:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);

				BM::GUID kCharGuid;
				POINT3 kTargetPos;
				SGroundKey kGndKey;

				m_kAddonPacket.Pop(kCharGuid);
				m_kAddonPacket.Pop(kGndKey);			

				BM::Stream kAnsPacket(PT_M_T_ANS_MOVETOSUMMONER);
				kAnsPacket.Push(kCharGuid);
				kAnsPacket.Push(kGndKey);
				kAnsPacket.Push(Ret());
				SendToCenter(kAnsPacket);

				if(S_OK == Ret())
				{
					VEC_GUID kMembGuids;
					kMembGuids.push_back(kCharGuid);
					SReqMapMove_CM kRMMC( MMET_PartyWarp );
					kRMMC.pt3TargetPos = pkCaster->GetPos();
					kRMMC.pt3TargetPos.z += 20;
					kRMMC.kGndKey = m_pkGround->GroundKey();
					kRMMC.nPortalNo = 0;

					SEventMessage kEventMsg;
					kEventMsg.PriType(PMET_GROUND_MGR);
					kEventMsg.SecType(PT_M_C_ANS_SUMMONPARTYMEMBER);
					kRMMC.WriteToPacket(kEventMsg);
					kEventMsg.Push(kMembGuids);
					g_kTask.PutMsg(kEventMsg);
				}
			}break;
		case PT_M_C_ANS_MOVETOPARTYMEMBER:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);

				if(S_OK == Ret())
				{
					POINT3 kTargetPos;
					SGroundKey kGndKey;
					m_kAddonPacket.Pop(kGndKey);			
					m_kAddonPacket.Pop(kTargetPos);

					SReqMapMove_MT kRMM;
					kRMM.kCasterKey = m_pkGround->GroundKey();
					kRMM.kTargetKey = kGndKey;
					kRMM.kCasterSI = g_kProcessCfg.ServerIdentity();
					kRMM.pt3TargetPos = kTargetPos;
					kRMM.pt3TargetPos.z += 20;
					kRMM.nTargetPortal = 0;
					ProcessReqMapMove(pkCaster,kRMM);
				}
			}break;
		case PT_M_C_ANS_USER_MAP_MOVE:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				pkCaster->Send(kResult);

				if(S_OK == Ret())
				{
					int iMapNo = 0;
					m_kAddonPacket.Pop(iMapNo);
					SReqMapMove_MT kRMM;
					kRMM.kCasterKey = m_pkGround->GroundKey();
					kRMM.kTargetKey.GroundNo(iMapNo);
					kRMM.kCasterSI = g_kProcessCfg.ServerIdentity();
					kRMM.nTargetPortal = 1;
					ProcessReqMapMove(pkCaster,kRMM);
				}
			}break;
		case PT_M_I_REQ_AFTER_QUEST_ACCEPT: // GMĿ�ǵ� ��
			{
				int iQuestID = 0;
				m_kAddonPacket.Pop( iQuestID );

				PgActionQuestUtil::AfterBeginCheckQuest(pkCaster, iQuestID, m_pkGround->GroundKey(), 0);
			}break;
		case PT_M_I_REQ_QUEST_REWARD:
		case PT_M_I_REQ_QUEST_ACCEPT:
		case PT_M_I_REQ_QUEST_EVENT:
			{
				BM::GUID kObjectGuid;
				int iQuestID = 0;
				int iNextDialogID = 0;
				int iFailedDailogID = 0;
				std::wstring kNpcName; // Log�� NPC�̸�
				CONT_QUEST_EFFECT kContAddEffect;

				m_kAddonPacket.Pop( kObjectGuid );
				m_kAddonPacket.Pop( iQuestID );
				m_kAddonPacket.Pop( iNextDialogID );
				m_kAddonPacket.Pop( iFailedDailogID );
				m_kAddonPacket.Pop( kNpcName );
				m_kAddonPacket.Pop( kContAddEffect );

				EQuestShowDialogType iShowDialogType = QSDT_NormalDialog;
				PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
				PgMyQuest const *pkMyQuest = (pkPlayer)? pkPlayer->GetMyQuest(): NULL;
				PgInventory const *pkInven = (pkPlayer)? pkPlayer->GetInven(): NULL;
				PgQuestInfo const *pkQuestInfo = NULL;
				if( pkPlayer && pkMyQuest && pkInven
				&&	g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
				{
					switch( Ret() )
					{
					case S_OK:
						{
							switch( wType )
							{
							case PT_M_I_REQ_QUEST_ACCEPT:
								{
									iShowDialogType = QSDT_BeginDialog;

									PgActionQuestUtil::AfterBeginCheckQuest(pkCaster, iQuestID, m_pkGround->GroundKey(), 0);
								}break;
							case PT_M_I_REQ_QUEST_REWARD:
								{
									iShowDialogType = QSDT_CompleteDialog;

									int const iAddCompleteCount = -1;  // �� ó���� Ƚ���� 1 ���� ���Ѿ� �Ѵ�
									int const iRewardGuildExp = PgQuestInfoUtil::GetQuestRewardGuildExp(pkQuestInfo, pkPlayer->GetMyQuest(), iAddCompleteCount);
									if( iRewardGuildExp )
									{
										BM::Stream kNPacket(PT_M_N_REQ_GUILD_COMMAND, pkCaster->GetID());
										kNPacket.Push( static_cast< BYTE >(GC_AddExp) );
										kNPacket.Push( iRewardGuildExp );
										::SendToGuildMgr(kNPacket);
									}

									ContRewardEffectVec const& rkEffectVec = pkQuestInfo->m_kReward.kEffect;
									if( !rkEffectVec.empty() )
									{
										ContRewardEffectVec::const_iterator loop_iter = rkEffectVec.begin();
										SActArg kArg;
										PgGroundUtil::SetActArgGround(kArg, m_pkGround);
										while( rkEffectVec.end() != loop_iter )
										{
											int const& iEffectNo = (*loop_iter);
											pkCaster->AddEffect(iEffectNo, 0, &kArg, pkCaster);
											++loop_iter;
										}
									}

									if( !pkQuestInfo->m_kReward.kRealmQuetID.empty() )
									{
										BM::Stream kPacket(PT_N_N_REQ_REALM_QUEST_ADD_COUNT);
										kPacket.Push( pkCaster->GetID() );
										kPacket.Push( pkQuestInfo->m_kReward.kRealmQuetID );
										kPacket.Push( static_cast< size_t >(0) );
										::SendToRealmContents(PMET_REALM_EVENT, kPacket);
									}

									if( pkQuestInfo->m_kReward.kUnlockCharacters )
									{
										BM::Stream kPacket(PT_N_N_REQ_EVENT_QUEST_UNLOCK_CHARS);
										kPacket.Push( pkPlayer->GetMemberGUID() );
										::SendToRealmContents(PMET_EVENTQUEST, kPacket);
									}
								}break;
							default:
								{
								}break;
							}

							int iNpcEventNo = 0;
							EQuestState eState = QS_None;
							{
								SUserQuestState const *pkState = pkMyQuest->Get(iQuestID);
								if( pkState )
								{
									eState = (EQuestState)pkState->byQuestState;
								}
							}

							// ���� �����ؾ� ����Ʈ�� �ش�
							PgAction_QuestDialogEvent::NfyAddEffectToMap(pkCaster, kContAddEffect);

							::NfyShowQuestDialog(pkCaster, kObjectGuid, iShowDialogType, iQuestID, iNextDialogID);
						}break;
					case E_CANT_BEGINQUEST:
						{
							::NfyShowQuestDialog(pkCaster, kObjectGuid, QSDT_ErrorDialog, iQuestID, QRDID_MaxQuestSlot);
						}break;
					case E_CANT_DEFEND_REWORD_FAIL:
						{
							::NfyShowQuestDialog(pkCaster, kObjectGuid, QSDT_ErrorDialog, iQuestID, QRDID_TiredLimitCantReward);
						}break;
					case E_CANT_DELQUEST:
					case E_CANT_UPDATEQUEST:
					case E_CANT_UPDATEQUESTPARAM:
					default:
						{
							::NfyShowQuestDialog(pkCaster, kObjectGuid, QSDT_ErrorDialog, iQuestID, iFailedDailogID);
						}break;
					}
				}
			}break;
		case PT_M_I_REQ_RESUME_GUILD_COMMAND:
			{
				BYTE cGuildCmd = 0;
				m_kAddonPacket.Pop(cGuildCmd);

				if( S_OK == Ret() )
				{
					BM::Stream kPacket(PT_C_N_REQ_GUILD_COMMAND, pkCaster->GetID());
					kPacket.Push(cGuildCmd);
					kPacket.Push(m_kAddonPacket);
					SendToGuildMgr(kPacket);
				}
				else
				{
					BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cGuildCmd);
					kPacket.Push((BYTE)GCR_Money);
					pkCaster->Send(kPacket);
				}
			}break;
		case PT_A_M_ADDON_WARPPED_PACKET:
			{
				BM::Stream kPacket;
				m_kAddonPacket.Pop( kPacket.Data() );
				kPacket.PosAdjust();

				m_kAddonPacket = kPacket; // ���� ��Ŷ���� ��ü

				DoAction(pkCaster, pkTarget); // ��� ȣ��
			}break;
		case PT_M_C_ANS_JOBSKILL3_CREATEITEM:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		case PT_M_C_ANS_EXTRACTION_MONSTERCARD:
			{
				BM::Stream kResult(wType);
				kResult.Push(Ret());
				kResult.Push(m_kAddonPacket);
				pkCaster->Send(kResult);
			}break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}break;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ModifyItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ModifyPlayerData::PgAction_ModifyPlayerData(EItemModifyParentEventType const kCause, HRESULT const hRet, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray,
										 PgGround * const pkGround, PgLocalPartyMgr &kLocalPartyMgr, PgEventItemSetMgr &rkEventItemSetMgr, BM::Stream const &kAddonPacket)
	:	m_kCause(kCause), m_kChangeArray(kChangeArray), m_kAddonPacket(kAddonPacket), m_kRet(hRet), m_pkGround(pkGround), m_kLocalPartyMgr(kLocalPartyMgr), m_kEventItemSetMgr(rkEventItemSetMgr)
{
}

PgAction_ModifyPlayerData::PgAction_ModifyPlayerData( PgAction_ModifyPlayerData const& rhs )
:	m_kCause(rhs.m_kCause)
,	m_kRet(rhs.m_kRet)
,	m_kChangeArray(rhs.m_kChangeArray)
,	m_kAddonPacket(rhs.m_kAddonPacket)
,	m_pkGround(rhs.m_pkGround)
,	m_kLocalPartyMgr(rhs.m_kLocalPartyMgr)
,	m_kEventItemSetMgr(rhs.m_kEventItemSetMgr)
{
}

bool PgAction_ModifyPlayerData::AchievementProcess(int iAbilType,int iValue,CUnit* pkCaster)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iAchievementType = iAbilType;

	switch( iAbilType )
	{
	case AT_CLASS:
		{
			const CONT_DEFUPGRADECLASS* pkUpClass = NULL;
			g_kTblDataMgr.GetContDef(pkUpClass);
			if(pkUpClass)
			{
				CONT_DEFUPGRADECLASS::const_iterator iter = pkUpClass->find(iValue);
				if(iter != pkUpClass->end())
				{
					PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_CHANGE_CLASS,(*iter).second.byGrade,m_pkGround->GroundKey());
					kSA.DoAction(pkCaster,NULL);
				}
			}

			{// ������ �⺻
				PgCheckAchievements<PgCheckComplete_CrazyLevelup,PgValueCtrl_Age2Minute> kCheckAchievements(AT_ACHIEVEMENT_CRAZYLEVELUP, m_pkGround->GroundKey() );
				kCheckAchievements.DoAction(pkCaster,NULL);
			}

			{// �ŵ鸰 ��Ʈ��
				PgCheckAchievements<PgCheckComplete_Equality,PgValueCtrl_CtrlOfGod> kCheckAchievements(AT_ACHIEVEMENT_CTRLOFGOD, m_pkGround->GroundKey() );
				kCheckAchievements.DoAction(pkCaster,NULL);
			}
		}break;
	case AT_LEVEL:
		{
			// �ѹ��� ���� �ʰ� ���� ���� �޼� ����
			if( 0 == pkCaster->GetAbil( AT_ACHIEVEMENT_ZOMBI) )
			{
				PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_INVINCIBLE, pkCaster->GetAbil(AT_LEVEL), m_pkGround->GroundKey() );
				kSA.DoAction( pkCaster, NULL );
			}

			{// ���� ����
				PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_LEVEL, pkCaster->GetAbil(AT_LEVEL), m_pkGround->GroundKey());
				kSA.DoAction(pkCaster,NULL);
			}

			// ����Ŭ��
			int const iMCCount = pkCaster->GetAbil(AT_ACHIEVEMENT_MARRY) + pkCaster->GetAbil(AT_ACHIEVEMENT_COUPLE) + pkCaster->GetAbil(AT_ACHIEVEMENT_1DAYCOUPLE);
			if(0 == iMCCount)
			{
				PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_SINGLECLUB, pkCaster->GetAbil(AT_LEVEL), m_pkGround->GroundKey());
				kSA.DoAction(pkCaster,NULL);
			}

			{// �⺻ �������� ���� �����ϸ� ȹ���ϴ� ����
				const CONT_DEFUPGRADECLASS* pkUpClass = NULL;
				g_kTblDataMgr.GetContDef(pkUpClass);
				if(pkUpClass)
				{
					CONT_DEFUPGRADECLASS::const_iterator iter = pkUpClass->find(pkCaster->GetAbil(AT_CLASS));
					if(iter != pkUpClass->end())
					{
						if(1 == (*iter).second.byGrade) 
						{
							PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_FIGHTEROFWIND, pkCaster->GetAbil(AT_LEVEL), m_pkGround->GroundKey());
							kSA.DoAction(pkCaster,NULL);
						}
					}
				}
			}
		}break;
	case AT_ACHIEVEMENT_COUPLE_START_DATE:
		{
			PgCheckAchievements<PgCheckComplete_Base,PgValueCtrl_CoupleTime> kCheckAchievements(iAchievementType, m_pkGround->GroundKey() );
			kCheckAchievements.DoAction(pkCaster,NULL);
		}break;
	case AT_ACHIEVEMENT_TEXTIME:
		{
			PgCheckAchievements<PgCheckComplete_Base,PgValueCtrl_TexTime> kCheckAchievements(iAchievementType, m_pkGround->GroundKey() );
			kCheckAchievements.DoAction(pkCaster,NULL);
		}break;
	case AT_ACHIEVEMENT_COMPLETE_FIRST:
		{
			PgCheckAchievements<PgCheckComplete_Equality> kCheckAchievements(iAchievementType, m_pkGround->GroundKey() );
			kCheckAchievements.DoAction(pkCaster,NULL);
		}break;
	case AT_ACHIEVEMENT_ATTR_ATTACK_5ELEMENT:
		{
			PgCheckAchievements_Attr5Element kCheckAchievements(iAchievementType, true);
			kCheckAchievements.DoAction(pkCaster,NULL);
		}break;
	case AT_ACHIEVEMENT_ATTR_RESIST_5ELEMENT:
		{
			PgCheckAchievements_Attr5Element kCheckAchievements(iAchievementType, false);
			kCheckAchievements.DoAction(pkCaster,NULL);
		}break;
	default:
		{
			PgCheckAchievements<> kCheckAchievements(iAchievementType, m_pkGround->GroundKey() );
			kCheckAchievements.DoAction(pkCaster,NULL);
		}break;
	}

	return true;
}

SGroundKey const& PgAction_ModifyPlayerData::GndKey(void)const
{
	return m_pkGround->GroundKey();
}

bool PgAction_ModifyPlayerData::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{//!!!!!!!!!!!! ������ �ý��� ���� �𸣸� �� ���� ���ÿ�.

	typedef std::set< int > ContQuestIDSet;

	// pkCaster�� PgPlayer�� �ƴҼ��� �ִ�(Pet�ϼ� �ִ�)
	if( pkCaster )
	{
		PgInventory* pInv = pkCaster->GetInven();
		if( !pInv )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Inventory is NULL"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		CONT_ITEM_VEC kItemVec;//��Ƽ������ ���� ������ �˸���
		ContItemNoSet kItemSet;//������ ������ ��ȣ�� ��ġ�� �ʰ� Set ���� ����(����Ʈ���� ��)
		ContQuestIDSet kChangeParamQuestIDSet;
		bool bAbilRefresh = false;
		bool bNeedGroundQuestRefresh = false;

		//DB_ITEM_STATE_CHANGE_ARRAY kBBSArray;

		DB_ITEM_STATE_CHANGE_ARRAY::const_iterator change_itor = m_kChangeArray.begin();
		while(change_itor != m_kChangeArray.end())
		{
			DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkItemChange = (*change_itor);

			BM::Stream kAddonData = rkItemChange.kAddonData;

			switch(rkItemChange.State())
			{
			case DISCT_SIDEJOB_INSERT:
				{
					BM::GUID kOwnerGuid;
					kAddonData.Pop(kOwnerGuid);
					SMOD_MyHome_SideJob_Insert kData;
					kData.ReadFromPacket(kAddonData);
					pkCaster->SetAbil(AT_HOME_SIDEJOB,pkCaster->GetAbil(AT_HOME_SIDEJOB) | kData.SideJob());
					pkCaster->SendAbil(AT_HOME_SIDEJOB,E_SENDTYPE_BROADALL);
				}break;
			case DISCT_SIDEJOB_MODIFY:
				{
				}break;
			case DISCT_SIDEJOB_REMOVE:
				{
					BM::GUID kOwnerGuid;
					kAddonData.Pop(kOwnerGuid);
					SMOD_MyHome_SideJob_Remove kData;
					kData.ReadFromPacket(kAddonData);

					pkCaster->SetAbil(AT_HOME_SIDEJOB,pkCaster->GetAbil(AT_HOME_SIDEJOB) & ~kData.SideJob());
					pkCaster->SendAbil(AT_HOME_SIDEJOB,E_SENDTYPE_BROADALL);
				}break;
			case DISCT_SIDEJOB_EQUIPCOUNT:
				{
				}break;
			case DISCT_GAMBLE_MIXUP_INSERT:
				{
					SPMOD_Gamble_Insert kData;
					kData.ReadFromPacket(kAddonData);

					BM::Stream kPacket(PT_M_C_ANS_RELOAD_ROULETTE_MIXUP);
					CONT_PLAYER_MODIFY_ORDER kOrder;
					kOrder.push_back(SPMO(IMET_GAMBLE_MIXUP_MODIFY,pkCaster->GetID()));
					PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine_Mixup, GndKey(), kOrder, kPacket);
					kItemModifyAction.DoAction(pkCaster, NULL);

					BM::Stream kResult(PT_M_C_ANS_USE_GAMBLEMACHINE_MIXUP_READY);
					kResult.Push(S_OK);
					kResult.Push(kData.RouletteCount());
					pkCaster->Send(kResult);
				}break;
			case DISCT_GAMBLE_MIXUP_MODIFY:
				{
					CONT_GAMBLEMACHINERESULT kContResult;
					CONT_GAMBLEITEM kContDumy;
					bool bBroadcast = false;
					int iRouletteCount = 0;
					PU::TLoadArray_M(kAddonData, kContResult);
					kAddonData.Pop(bBroadcast);
					kAddonData.Pop(iRouletteCount);
					PU::TLoadArray_A(kAddonData, kContDumy);

					BM::Stream kResult(PT_M_C_ANS_RELOAD_ROULETTE_MIXUP);
					kResult.Push(S_OK);
					PU::TWriteArray_M(kResult, kContResult);
					PU::TWriteArray_A(kResult, kContDumy);
					kResult.Push(iRouletteCount);
					pkCaster->Send(kResult);
				}break;
			case DISCT_GAMBLE_MIXUP_REMOVE:
				{
					CONT_GAMBLEMACHINERESULT kContResult;
					PU::TLoadArray_M(kAddonData, kContResult);
					bool bBroadcast = false;
					kAddonData.Pop(bBroadcast);

					BM::Stream kResult(PT_M_C_ANS_ROULETTE_MIXUP_RESULT);
					kResult.Push(S_OK);
					PU::TWriteArray_M(kResult, kContResult);
					pkCaster->Send(kResult);

					if(true == bBroadcast)
					{
						BM::Stream kNotiPacket(PT_M_C_NFY_GAMBLEMACHINE_MIXUP_RESULT);
						kNotiPacket.Push(pkCaster->Name());
						PU::TWriteArray_M(kNotiPacket, kContResult);
						SendToItem(GndKey(),kNotiPacket);
					}

					GET_DEF(CItemDefMgr, kItemDefMgr);
					CONT_PLAYER_MODIFY_ORDER kOrder;

					for(CONT_GAMBLEMACHINERESULT::const_iterator iter = kContResult.begin();iter != kContResult.end();++iter)
					{
						PgBase_Item const & kItem = (*iter);

						CItemDef const *pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
						if(!pkItemDef)
						{
							continue;
						}

						// �� �������̴�. �������� ������ �ʴ´�.
						if(UICT_BLANK == pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
						{
							continue;
						}

						kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkCaster->GetID(), SPMOD_Insert_Fixed(kItem, SItemPos(), true)));
					}

					PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine_Mixup, GndKey(), kOrder);
					kItemModifyAction.DoAction(pkCaster, NULL);
				}break;
			case DISCT_GAMBLE_REMOVE:
				{
					CONT_GAMBLEMACHINERESULT kContResult;
					PU::TLoadArray_M(kAddonData, kContResult);
					bool bBroadcast = false;
					kAddonData.Pop(bBroadcast);

					BM::Stream kResult(PT_M_C_ANS_ROULETTE_RESULT);
					kResult.Push(S_OK);
					PU::TWriteArray_M(kResult, kContResult);
					pkCaster->Send(kResult);


					if(false == kContResult.empty())
					{// �м� ���� ���� : �ΰ��Ӱ�í�� �ڽ�Ƭ ������ ȹ�� ������ ���� ����
						PgBase_Item kItem = (*kContResult.begin());
						GET_DEF( CItemDefMgr, kItemDefMgr );
						CItemDef const *pItemDef = kItemDefMgr.GetDef( kItem.ItemNo() );

						if(pItemDef && pItemDef->CanEquip() && pItemDef->IsType( ITEM_TYPE_AIDS ) )
						{// ���������ϸ鼭 ĳ�þ������̸� �ڽ�Ƭ �������̴�.

							PgAddAchievementValue kMA( AT_ACHIEVEMENT_CASHSHOPBUY_EQUIP, 1, m_pkGround->GroundKey() );
							kMA.DoAction( pkCaster, NULL );
						}
					}

					if(true == bBroadcast)
					{
						BM::Stream kNotiPacket(PT_M_C_NFY_GAMBLEMACHINE_RESULT);
						kNotiPacket.Push(pkCaster->Name());
						PU::TWriteArray_M(kNotiPacket, kContResult);
						SendToItem(GndKey(),kNotiPacket);
					}

					CONT_PLAYER_MODIFY_ORDER kOrder;
					for(CONT_GAMBLEMACHINERESULT::const_iterator iter = kContResult.begin();iter != kContResult.end();++iter)
					{
						PgBase_Item const & kItem = (*iter);
						kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkCaster->GetID(), SPMOD_Insert_Fixed(kItem, SItemPos(), true)));
					}

					PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine_Shop, GndKey(), kOrder);
					kItemModifyAction.DoAction(pkCaster, NULL);
				}break;
			case DISCT_GAMBLE_MODIFY:
				{
					CONT_GAMBLEMACHINERESULT kContResult;
					CONT_GAMBLEITEM kContDumy;
					bool bBroadcast = false;
					int iRouletteCount = 0;
					PU::TLoadArray_M(kAddonData, kContResult);
					kAddonData.Pop(bBroadcast);
					kAddonData.Pop(iRouletteCount);
					PU::TLoadArray_A(kAddonData, kContDumy);

					BM::Stream kResult(PT_M_C_ANS_RELOAD_ROULETTE);
					kResult.Push(S_OK);
					PU::TWriteArray_M(kResult, kContResult);
					PU::TWriteArray_A(kResult, kContDumy);
					kResult.Push(iRouletteCount);
					pkCaster->Send(kResult);
				}break;
			case DISCT_GAMBLE_INSERT:
				{
					SPMOD_Gamble_Insert kData;
					kData.ReadFromPacket(kAddonData);

					BM::Stream kPacket(PT_M_C_ANS_RELOAD_ROULETTE);
					CONT_PLAYER_MODIFY_ORDER kOrder;
					kOrder.push_back(SPMO(IMET_GAMBLE_MODIFY,pkCaster->GetID()));
					PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine_Shop, GndKey(), kOrder, kPacket);
					kItemModifyAction.DoAction(pkCaster, NULL);

					BM::Stream kResult(PT_M_C_ANS_USE_GAMBLEMACHINE_READY);
					kResult.Push(S_OK);
					kResult.Push(kData.RouletteCount());
					pkCaster->Send(kResult);
				}break;
			case DISCT_SET_HOME_ADDR:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SMOD_MyHome_MyHome_Set_Addr kData;
						kData.ReadFromPacket(kAddonData);
						SHOMEADDR const kNewHomeAddr(kData.StreetNo(),kData.HouseNo());
						pkPlayer->HomeAddr(kNewHomeAddr);

						// ����Ȩ ��������, ����Ȩ ����Ʈ ����
						if( kNewHomeAddr.IsNull() )
						{
							PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
							if( pkMyQuest )
							{
								ContUserQuestState kVec;
								pkMyQuest->GetQuestList(kVec);
								if( !kVec.empty() )
								{
									CONT_PLAYER_MODIFY_ORDER kFailedOrder;
									ContUserQuestState::const_iterator iter = kVec.begin();
									while( kVec.end() != iter )
									{
										int const iQuestID = (*iter).iQuestID;
										PgQuestInfo const* pkQuestInfo = NULL;
										if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
										{
											if( QET_MYHOME_INVITEUSERCOUNT == pkQuestInfo->m_kDepend_MyHome.iType )
											{
												kFailedOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), SPMOD_AddIngQuest(iQuestID, QS_Failed)) ); // ���� ó��
											}
										}
										++iter;
									}

									if( !kFailedOrder.empty() )
									{
										PgAction_ReqModifyItem kCreateAction(IMEPT_QUEST, GndKey(), kFailedOrder);
										kCreateAction.DoAction(pkPlayer, NULL);
									}
								}
							}
						}
						bNeedGroundQuestRefresh = true;
					}
				}break;
			case DISCT_SET_HOME_STATE:
				{
					BYTE bState = 0;
					kAddonData.Pop(bState);

					PgMyHome * pkHome = dynamic_cast<PgMyHome*>(pkCaster);
					if(pkHome)
					{
						pkHome->SetAbil(AT_MYHOME_STATE,bState);
					}
				}break;
			case DISCT_SET_HOME_VISITFLAG:
				{
					BYTE bEnableVisitBit = 0;
					kAddonData.Pop(bEnableVisitBit);

					PgMyHome * pkHome = dynamic_cast<PgMyHome*>(pkCaster);
					if(pkHome)
					{
						pkHome->SetAbil(AT_MYHOME_VISITFLAG,bEnableVisitBit);
					}
				}break;
			case DISCT_SET_HOME_OWNER_INFO:
				{
					SMOD_MyHome_MyHome_Set_OwnerInfo kData;
					kData.ReadFromPacket(kAddonData);

					PgMyHome * pkHome = dynamic_cast<PgMyHome*>(pkCaster);
					if(pkHome)
					{
						pkHome->OwnerGuid(kData.OwnerGuid());
						pkHome->OwnerName(kData.OwnerName());
					}
				}break;
			case DISCT_SET_HOME_DEFAULT_ITEM:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SMOD_SetHomeStyle kData;
						kData.ReadFromPacket(kAddonData);
						pkPlayer->SetDefaultItem(kData.EquipPos(),kData.ItemNo());
					}
				}break;
			case DISCT_ADD_EMOTION:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SPMOD_AddEmotion kData;
						kData.ReadFromPacket(kAddonData);
						PgBitArray<MAX_DB_EMOTICON_SIZE> kCont;
						if(true == pkPlayer->GetContEmotion(kData.Type(),kCont))
						{
							kCont.Set(kData.GroupNo(),true);
							pkPlayer->SetContEmotion(kData.Type(),kCont);
						}
					}
				}break;
			case DISCT_CREATE_CHARACTERCARD:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SMOD_CreateCard kData;
						kData.ReadFromPacket(kAddonData);
						VEC_CARD_ABIL kContAbil;
						kAddonData.Pop(kContAbil);
						pkPlayer->SetCardAbil(kContAbil);
					}
				}break;
			case DISCT_MODIFY_CHARACTERCARD:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SMOD_ModifyCard kData;
						kData.ReadFromPacket(kAddonData);
						VEC_CARD_ABIL kContAbil;
						kAddonData.Pop(kContAbil);
						pkPlayer->SetCardAbil(kContAbil);
					}
				}break;
			case DISCT_CREATE_PORTAL:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SMOD_Portal_Create kData;
						kData.ReadFromPacket(kAddonData);
						pkPlayer->InsertPortal(kData.Guid(),CONT_USER_PORTAL::mapped_type(kData.Comment(),kData.GroundNo(),kData.Pos()));
					}
				}break;
			case DISCT_DELETE_PORTAL:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SMOD_Portal_Delete kData;
						kData.ReadFromPacket(kAddonData);
						pkPlayer->RemovePortal(kData.Guid());
					}
				}break;
			case DISCT_MODIFY_POPULARPOINT:
				{
					__int64 i64TotalPP = 0;
					__int64 i64TodayPP = 0;
					std::wstring kOwner;
					kAddonData.Pop(i64TotalPP);
					kAddonData.Pop(i64TodayPP);
					kAddonData.Pop(kOwner);

					{// ���� �α⵵ ����
						int const iMaxValue = std::min<__int64>(i64TotalPP,std::numeric_limits<int>::max());// 20�� �̻��� ������ ������ ���� 
						PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_POPULARPOINT,iMaxValue,m_pkGround->GroundKey());
					}
					{// ������ �α⵵ ����
						int const iMaxValue = std::min<__int64>(i64TodayPP,std::numeric_limits<int>::max());// 20�� �̻��� ������ ������ ���� 
						PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_TODAYPOPULARPOINT,iMaxValue,m_pkGround->GroundKey());
					}

					int iEffectNum = 0;
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						CONT_DEFCHARCARDEFFECT const * pkTable = NULL;
						g_kTblDataMgr.GetContDef(pkTable);
						if(pkTable)
						{
							CONT_DEFCHARCARDEFFECT::const_iterator iter = pkTable->find(i64TodayPP);
							if(iter != pkTable->end())
							{
								CONT_DEFCHARCARDEFFECT::mapped_type kCont = (*iter).second;

								std::random_shuffle(kCont.begin(), kCont.end(), BM::Rand_Index);

								CONT_DEFCHARCARDEFFECT::mapped_type::iterator effect_iter = kCont.begin();
								if(effect_iter != kCont.end())
								{
									SEffectCreateInfo kCreate;
									kCreate.eType = EFFECT_TYPE_NORMAL;
									kCreate.iEffectNum = (*effect_iter);
									kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
									pkPlayer->AddEffect(kCreate);
									::CheckSkillFilter_Delete_Effect(pkPlayer, pkPlayer, kCreate.iEffectNum);

									iEffectNum = (*effect_iter);
								}
							}
						}
					}

					if(!kOwner.empty())
					{
						ECharacterCardRecommendType kType = CCRT_POPULARER;

						BM::Stream kPacket(PT_M_C_ANS_RECOMMEND_CHARACTER);
						kPacket.Push(static_cast<BYTE>(kType));
						kPacket.Push(kOwner);
						kPacket.Push(iEffectNum);
						pkPlayer->Send(kPacket);
					}
				}break;
			case DISCT_MODIFY_REFRESHDATE:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						BM::PgPackedTime kDate;
						kAddonData.Pop(kDate);
						pkPlayer->RefreshDate(kDate);
					}
				}break;
			case DISCT_MODIFY_RECOMMENDPOINT:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iRecommendPoint = 0;
						kAddonData.Pop(iRecommendPoint);
						pkPlayer->RecommendPoint(iRecommendPoint);
					}
				}break;
			case DISCT_INVENTORY_EXTEND:
				{
					EInvType kInvType;
					BYTE kReductionNum;
					kAddonData.Pop(kInvType);
					kAddonData.Pop(kReductionNum);
					pInv->InvExtend(kInvType,kReductionNum);
				}break;
			case DISCT_EXTEND_MAX_IDX:
				{
					EInvType kInvType;
					BYTE kReductionNum;
					kAddonData.Pop(kInvType);
					kAddonData.Pop(kReductionNum);
					pInv->ExtendMaxIdx(kInvType,kReductionNum);
				}break;
			case DISCT_SET_DEFAULT_ITEM:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iEquipLimit = 0;
						int iItemNo = 0;
						kAddonData.Pop(iEquipLimit);
						kAddonData.Pop(iItemNo);
						pkPlayer->SetDefaultItem(iEquipLimit,iItemNo);

						switch(iEquipLimit)
						{
						case EQUIP_POS_HAIR:
							{
								PgAddAchievementValue kMA(AT_ACHIEVEMENT_USE_STYLEITEM_HAIR,1,m_pkGround->GroundKey());
								kMA.DoAction(pkPlayer,NULL);
							}break;
						case EQUIP_POS_FACE:
							{
								PgAddAchievementValue kMA(AT_ACHIEVEMENT_USE_STYLEITEM_FACE,1,m_pkGround->GroundKey());
								kMA.DoAction(pkPlayer,NULL);
							}break;
						default:
							{
							}break;
						}
					}
				}break;
			case DISCT_ADD_UNBIND_DATE:
				{
					BM::GUID kItemGuid;
					BYTE kDelayDays = 0;
					BM::DBTIMESTAMP_EX kDBTimeEx;
					kAddonData.Pop(kItemGuid);
					kAddonData.Pop(kDelayDays);
					kAddonData.Pop(kDBTimeEx);
					pInv->AddUnbindDate(kItemGuid,kDBTimeEx);
				}break;
			case DISCT_DEL_UNBIND_DATE:
				{
					BM::GUID kItemGuid;
					kAddonData.Pop(kItemGuid);
					pInv->DelUnbindDate(kItemGuid);
				}break;
			case DISCT_SET_RENTALSAFETIME:
				{
					EInvType kInvType;
					BM::DBTIMESTAMP_EX kDBTime;
					kAddonData.Pop(kInvType);
					kAddonData.Pop(kDBTime);
					pInv->SetRentalSafeEnableTime(kInvType,kDBTime);
				}break;
			case DISCT_ACHIEVEMENT_TIMELIMIT_MODIFY:
				{
					int iSaveIdx = 0;
					BM::PgPackedTime kPackedTime;
					kAddonData.Pop(iSaveIdx);
					kAddonData.Pop(kPackedTime);

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						pkPlayer->GetAchievements()->SetAchievementTimeLimit(iSaveIdx,kPackedTime);
					}
				}break;
			case DISCT_ACHIEVEMENT_TIMELIMIT_DELETE:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iSaveIdx = 0;
						kAddonData.Pop(iSaveIdx);
						pkPlayer->GetAchievements()->ResetAchievementTimeLimit(iSaveIdx);
					}
				}break;
			case DISCT_SET_ACHIEVEMENT:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iSaveIdx = 0;
						BYTE bValue = 0;

						kAddonData.Pop(iSaveIdx);
						kAddonData.Pop(bValue);

						if(bValue)
						{
							pkPlayer->GetAchievements()->Complete(iSaveIdx);
						}
						else
						{
							pkPlayer->GetAchievements()->Reset(iSaveIdx);
						}
					}
				}break;
			case DISCT_ACHIEVEMENT2INV:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iSaveIdx = 0;
						kAddonData.Pop(iSaveIdx);
						pkPlayer->GetAchievements()->ResetItem(iSaveIdx,true);
					}
				}break;
			case DISCT_INV2ACHIEVEMENT:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iSaveIdx = 0;
						kAddonData.Pop(iSaveIdx);
						pkPlayer->GetAchievements()->ResetItem(iSaveIdx,false);
					}
				}break;
			case DISCT_COMPLETE_ACHIEVEMENT:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						SPMOD_Complete_Achievement kData;
						kData.ReadFromPacket( kAddonData );
						pkPlayer->GetAchievements()->Complete(kData.SaveIdx());
						
						{// ���� ���� ����
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_COLLECT_COUNT, 1, GndKey() );
							kMA.DoAction( pkPlayer, NULL );
						}						
						pkPlayer->GetAchievements()->AddAchievementPoint(static_cast< EAchievementsCategory >(kData.Category()), kData.RankPoint());

						{
							int const iAbilType = AT_ACHIEVEMENT_TOTAL_POINT + kData.Category();
							PgAction_QuestAbil kAbilEvent(GndKey(), iAbilType);
							kAbilEvent.DoAction(pkPlayer, NULL); // �κ� ����
						}
						{
							PgAction_QuestAbil kAbilEvent(GndKey(), AT_ACHIEVEMENT_TOTAL_POINT);
							kAbilEvent.DoAction(pkPlayer, NULL); // ��ü ����
						}

						{// ���� �޼� �α�
							PgLogCont kLogCont( ELogMain_Contents_Achievements, ELogSub_Achievements );
							kLogCont.MemberKey( pkPlayer->GetMemberGUID() );
							kLogCont.UID( pkPlayer->UID() );
							kLogCont.CharacterKey( pkPlayer->GetID() );
							kLogCont.ID( pkPlayer->MemberID() );
							kLogCont.Name( pkPlayer->Name() );
							kLogCont.Class( pkPlayer->GetAbil( AT_CLASS ) );
							kLogCont.Level( pkPlayer->GetAbil( AT_LEVEL ) );
							kLogCont.ChannelNo( pkPlayer->GetChannel() );
							kLogCont.GroundNo( pkPlayer->GetAbil( AT_RECENT ) );
							
							const CONT_DEF_ACHIEVEMENTS_SAVEIDX* pkCont = NULL;
							g_kTblDataMgr.GetContDef( pkCont );
							if( pkCont )
							{
								CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator find_iter = pkCont->find( kData.SaveIdx() );
								if( find_iter != pkCont->end() )
								{
									CONT_DEF_ACHIEVEMENTS_SAVEIDX::mapped_type const c_kAchievement = (*find_iter).second;

									std::wstring kAchievementName;
									if( !GetDefString( c_kAchievement.iTitleNo, kAchievementName ) )
									{
										INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't Get Achievements TitleNo["<< c_kAchievement.iTitleNo <<L"]");										
										LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
										return false;
									}

									PgLog kAchievementsLog( ELOrderMain_Achievements, ELOrderSub_Complete );
									kAchievementsLog.Set( 0, kAchievementName );
									kAchievementsLog.Set( 0, c_kAchievement.iCategory );
									kAchievementsLog.Set( 1, kData.SaveIdx() );
									kLogCont.Add(kAchievementsLog);
									kLogCont.Commit();
								}
							}
						}
					}
				}break;
			case DISCT_SET_HIDDEN_OPEN:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						BM::DBTIMESTAMP_EX kLastHiddenUpdate;

						kAddonData.Pop( kLastHiddenUpdate );

						pkPlayer->GetHiddenOpen()->Init();
						pkPlayer->GetHiddenOpen()->SetLastDateUpdate( kLastHiddenUpdate );
					}
				}
				break;
			case DISCT_COMPLETE_HIDDEN_OPEN:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						int iSaveIdx = 0;
						kAddonData.Pop(iSaveIdx);
						pkPlayer->GetHiddenOpen()->Complete(iSaveIdx);
					}
				}break;
			case DISCT_MODIFY_VALUE:
			case DISCT_MODIFY_VALUEEX:
			case DISCT_ADD_ABIL:
				{
					int iType = 0;
					kAddonData.Pop(iType);

					switch(rkItemChange.Cause())
					{
					case IMET_SET_ABIL:
						{
							int iValue = 0;
							kAddonData.Pop(iValue);

							int const iPrevClass = pkCaster->GetAbil(AT_CLASS);

							pkCaster->SetAbil(iType, iValue);
							
							bool bSend = false;
							DWORD dwSendType = E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND;
							switch( iType )
							{
							case AT_CLASS:
								{
									// ��Ƽ ���� Refresh
									PgAction_PartyBufRefresh kPartyRefreshAction(m_pkGround, m_pkGround->GroundKey(), m_kLocalPartyMgr, iPrevClass);
									kPartyRefreshAction.DoAction(pkCaster, NULL);
									bNeedGroundQuestRefresh = true;
								}break;
							case AT_LEVEL:
								{
									{//������ ������ ���� �� �� �ִ� ����Ʈ�� ������ �� �ִ�.
										PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
										if( pkPlayer )
										{
											CONT_PLAYER_MODIFY_ORDER kOrder;
											kOrder.push_back( SPMO(IMET_PRE_CHECK_WANTED_QUEST, pkPlayer->GetID()) );

											PgAction_ReqModifyItem kCreateAction(IMEPT_QUEST, m_pkGround->GroundKey(), kOrder);
											kCreateAction.DoAction(pkPlayer, NULL);
										}
									}
									// Class �� Level �� SetAbil���� �˾Ƽ� ������.
									bNeedGroundQuestRefresh = true;
								}break;
							case AT_STR_ADD:
							case AT_INT_ADD:
							case AT_CON_ADD:
							case AT_DEX_ADD:
							case AT_MYHOME_TODAYHIT:
							case AT_MYHOME_TOTALHIT:
							case AT_MYHOME_VISITLOGCOUNT:
								{ 
									bSend = true; 
									dwSendType |= E_SENDTYPE_BROADCAST;
								}break;
							case AT_LEVEL_RANK:
								{
									bSend = true;
								
									pkCaster->DeleteEffect( EFFECTNO_CHARACTER_BONUS_EFFECT_BASENO, true );

									if ( iValue )
									{
										int const iNewEffectNo = ( EFFECTNO_CHARACTER_BONUS_EFFECT_BASENO + iValue - 1 );

										SActArg kArg;
										PgGroundUtil::SetActArgGround(kArg, m_pkGround);

										pkCaster->AddEffect( iNewEffectNo, 0, &kArg, NULL, EFFECT_TYPE_ABIL );
									}
								}break;
							default:
								{
									bSend = true;
								}break;
							}
							if (bSend)
							{
								pkCaster->SendAbil((EAbilType)iType, dwSendType);//���̵� ���̶� ������.
							}
							AchievementProcess( iType, iValue, pkCaster );

						}break;
					case IMET_SET_ABIL64:
						{
							__int64 i64Value = 0;
							kAddonData.Pop(i64Value);

							pkCaster->SetAbil64(iType, i64Value);
							pkCaster->SendAbil64((EAbilType)iType);
						}break;
					case IMET_ADD_ABIL://Add �� ����� Set.
						{
							int iValue = 0;
							kAddonData.Pop(iValue);

							DWORD dwSendType = E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND;

							switch(iType)
							{
							case AT_MYHOME_TODAYHIT:
							case AT_MYHOME_TOTALHIT:
							case AT_MYHOME_VISITLOGCOUNT:
								{ 
									dwSendType |= E_SENDTYPE_BROADCAST;
								}break;
							}

							pkCaster->SetAbil(iType, iValue);
							pkCaster->SendAbil((EAbilType)iType, dwSendType);//���̵� ���̶� ������.

							AchievementProcess(iType,iValue,pkCaster);
						}break;
					case IMET_ADD_ABIL64EX:
					case IMET_ADD_ABIL64://Add �� ����� Set.
						{
							__int64 i64Value = 0;
							kAddonData.Pop(i64Value);
							pkCaster->SetAbil64(iType, i64Value);
							pkCaster->SendAbil64((EAbilType)iType, E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);//���̵� ���̶� ������.
						}break;
					case IMET_ADD_ABIL64_MAPUSE:
						{
							__int64 i64Value = 0;
							kAddonData.Pop(i64Value);
							i64Value = pkCaster->GetAbil64(iType) + i64Value;
							switch (iType)
							{
							case AT_REST_EXP_ADD_MAX:
								{
									g_kEventView.VariableCont().iExpAdd_MaxExperienceRate;
									GET_DEF(PgClassDefMgr, kClassDefMgr);
									int const iClass = pkCaster->GetAbil(AT_CLASS);
									__int64 const i64LvExp = kClassDefMgr.GetExperience4Levelup(SClassKey(iClass, pkCaster->GetAbil(AT_LEVEL)));
									__int64 const i64CurExp = pkCaster->GetAbil64(AT_EXPERIENCE);
									__int64 const i64MaxExpAdd = std::min<__int64>( kClassDefMgr.GetMaxExperience(iClass), i64CurExp + static_cast<__int64>(g_kEventView.VariableCont().iExpAdd_MaxExperienceRate / 100.0f * i64LvExp));
									i64Value = std::min<__int64>(i64MaxExpAdd, std::max<__int64>(i64Value, 0));
								}break;
							}
							pkCaster->SetAbil64(iType,  i64Value, true);
						}break;
					}
				}break;
			case DISCT_MODIFY_GM_INIT_SKILL:
				{
					SPlayerStrategySkillData kData;
					kAddonData.Pop( kData );

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						PgMySkill* pkMySkill = pkPlayer->GetMySkill();
						if( pkMySkill )
						{
							pkMySkill->Init();
							pkMySkill->Add(MAX_DB_SKILL_SIZE, kData.abySkills);
							pkMySkill->InitSkillExtend(MAX_DB_SKILL_EXTEND_SIZE, kData.abySkillExtends);
						}
					}
				}break;
			case DISCT_MODIFY_SKILLEXTEND:
				{
					SPMOD_SetSkillExtend kData;
					kData.ReadFromPacket(kAddonData);
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						pkPlayer->GetMySkill()->SetExtendLevel(kData.SkillNo(), kData.ExtendLevel());
					}
				}break;
			case DISCT_MODIFY_STRATEGYSKILL:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						int iValue = 0;
						iValue = 0; kAddonData.Pop(iValue); pkPlayer->SetAbil(AT_STRATEGYSKILL_TABNO, iValue);
						iValue = 0; kAddonData.Pop(iValue); pkPlayer->SetAbil(AT_SP, iValue);
						pkPlayer->GetMySkill()->ReadFromPacket(kAddonData);
						pkPlayer->GetQInven()->ReadFromPacket(kAddonData);
					}
				}break;
			case DISCT_MODIFY_SKILL:
				{
					int iType = 0;
					int iValue = 0;
					kAddonData.Pop(iType);
					kAddonData.Pop(iValue);

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						GET_DEF( CSkillDefMgr, kSkillDefMgr);

						SActArg kArg;
						PgGroundUtil::SetActArgGround(kArg, m_pkGround);

						if(iType)//����.
						{
							size_t szIndex = 0;
							int iSkillNo = 0;
							while ((iSkillNo = pkPlayer->GetMySkill()->GetSkillNo(EST_PASSIVE, szIndex++)) > 0)
							{
								CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);			
								if (NULL != pkSkillDef)
								{
									CEffect const* pkEffect = pkPlayer->GetEffect(iSkillNo, true);
									if(NULL != pkEffect)
									{
										//������ �ɷ��ִ� EffectNo�� �ش�Ǵ� ��ų Def�� ��� �;� �Ѵ�.
										//OverLevel�� ���ؼ� ����� ������ �ɷ��ִ� ������ �ٸ� �� �ִ�.
										int const iEffectNo = pkEffect->GetEffectNo();
										CSkillDef const* pkOverSkillDef = kSkillDefMgr.GetDef(iEffectNo);
										if(pkSkillDef && pkOverSkillDef)
										{
											if(pkOverSkillDef->No() != pkSkillDef->No())
											{
												pkSkillDef = pkOverSkillDef;
											}
										}
									}

									g_kSkillAbilHandleMgr.SkillEnd(pkPlayer, pkSkillDef, &kArg);
								}
							}

							szIndex = 0;
							while ((iSkillNo = pkPlayer->GetMySkill()->GetSkillNo(EST_TOGGLE_ON, szIndex++)) > 0)
							{
								CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);			
								if (NULL != pkSkillDef)
								{
									CEffect const* pkEffect = pkPlayer->GetEffect(iSkillNo, true);
									if(NULL != pkEffect)
									{
										//������ �ɷ��ִ� EffectNo�� �ش�Ǵ� ��ų Def�� ��� �;� �Ѵ�.
										//OverLevel�� ���ؼ� ����� ������ �ɷ��ִ� ������ �ٸ� �� �ִ�.
										int const iEffectNo = pkEffect->GetEffectNo();
										CSkillDef const* pkOverSkillDef = kSkillDefMgr.GetDef(iEffectNo);
										if(pkSkillDef && pkOverSkillDef)
										{
											if(pkOverSkillDef->No() != pkSkillDef->No())
											{
												pkSkillDef = pkOverSkillDef;
											}
										}
									}

									g_kSkillAbilHandleMgr.SkillToggle(pkPlayer, pkSkillDef->No(), &kArg, false);
								}
							}

							//pkPlayer->GetMySkill()->Init(UT_PLAYER);//Init ���� ����µ�.
							PgMySkill* pkMySkill = pkPlayer->GetMySkill();
							if( pkMySkill )
							{
								pkMySkill->EraseSkill(SDT_Normal);	// Skill �����
								pkMySkill->EraseSkill(SDT_Special);
								PgAction_SyncSkillState kAction;
								kAction.DoAction(pkPlayer, NULL);
							}
						}
						else
						{
							int const iSkillNo = iValue;
							CSkillDef const* pkDef = kSkillDefMgr.GetDef(iSkillNo);
							int const iBasicSkillNum = kSkillDefMgr.GetCallSkillNum(iSkillNo);
							bool const bPassiveSkill = ((ESkillType) pkDef->GetAbil(AT_TYPE) == EST_PASSIVE) ? true : false;
							if( bPassiveSkill )
							{
								// ������ ��� ��ų�� ������ �� ��ų�� ȿ���� ���ֱ� ���� �۾��� �ʿ��ϴ�.
								int const iLearn = pkPlayer->GetMySkill()->GetLearnedSkill(iSkillNo);
								if ( iLearn > 0 )
								{
									//Passive Skill�� SkillNo �� EffectNo�� ����.
									CSkillDef const* pkOldSkillDef = kSkillDefMgr.GetDef(iLearn);										
									CEffect* pkOldEffect = pkPlayer->GetEffect(iLearn, true);
									if(pkOldEffect)
									{
										//������ �ɷ��ִ� EffectNo�� �ش�Ǵ� ��ų Def�� ��� �;� �Ѵ�.
										//OverLevel�� ���ؼ� ����� ������ �ɷ��ִ� ������ �ٸ� �� �ִ�.
										int const iOldEffectNo = pkOldEffect->GetEffectNo();
										CSkillDef const* pkOldSkillDef2 = kSkillDefMgr.GetDef(iOldEffectNo);
										if(pkOldSkillDef && pkOldSkillDef2)
										{
											if(pkOldSkillDef2->No() != pkOldSkillDef->No())
											{
												pkOldSkillDef = pkOldSkillDef2;
											}
										}
									}

									if ( pkOldSkillDef )
									{
										SActArg kArg;
										g_kSkillAbilHandleMgr.SkillEnd( pkCaster, pkOldSkillDef, &kArg);
									}
								}
							}

							if(S_OK != pkPlayer->GetMySkill()->LearnNewSkill(iSkillNo))
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Error SkillNo-1[" << iSkillNo << L"] [" << pkCaster->Name() << L"-" << pkCaster->GetID() << L"]");
							}

							if(bPassiveSkill)
							{
								int const iOverLevel = pkPlayer->GetMySkill()->GetOverSkillLevel(iSkillNo);
								g_kSkillAbilHandleMgr.SkillPassive( pkCaster, iSkillNo + iOverLevel, &kArg );
							}

							BM::Stream kPacket(PT_M_C_RES_LEARN_SKILL, iSkillNo );
							kPacket.Push( LS_RET_SUCCEEDED );
							kPacket.Push( (short)pkCaster->GetAbil(AT_SP) );
							pkCaster->Send( kPacket, E_SENDTYPE_SELF| E_SENDTYPE_MUSTSEND );

							RefreshGroundQuestInfo(pkCaster);	//��ų ���� ����Ʈ �ٽ� Ȯ������. ��ų ���� ���η� ����Ʈ üũ�ϴ°� �߰��Ǿ���(2011.11.14) By ������
						}
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Error SkillNo-2[" << iValue << L"] [" << pkCaster->Name() << L"-" << pkCaster->GetID() << L"]");
					}
				}break;
			case DISCT_DELETE_SKILL:
				{
					int iType = 0;
					kAddonData.Pop(iType);
					int iValue = 0;
					kAddonData.Pop(iValue);
					int const iSkillNo = iValue;

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						bool const bHave = pkPlayer->GetMySkill()->IsExist(iSkillNo);
						if( bHave)
						{
							bool const bDelete = pkPlayer->GetMySkill()->Delete(iSkillNo);
							if(bDelete)
							{
								BM::Stream kPacket(PT_M_C_RES_DELETE_SKILL, iSkillNo );
								kPacket.Push( LS_RET_SUCCEEDED );
								pkCaster->Send( kPacket, E_SENDTYPE_SELF| E_SENDTYPE_MUSTSEND );
							}
						}
					}
				}break;
			case DISCT_MODIFY_MONEY:
				{
					__int64 i64NewMoney = 0;

					kAddonData.Pop(i64NewMoney);

					__int64 const iPrevMoney = pkCaster->GetAbil64(AT_MONEY);
					__int64 const iDiffMoney = i64NewMoney - iPrevMoney;
					pkCaster->SetAbil64(AT_MONEY, i64NewMoney);//�� ����.
					pkCaster->SendAbil64(AT_MONEY, E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);

					if( iDiffMoney )//Ŭ���̾�Ʈ �ý��� �޽���
					{
						BM::Stream kPacket(PT_M_C_NFY_CHANGE_MONEY, (BYTE)m_kCause);
						kPacket.Push(iDiffMoney);
						pkCaster->Send(kPacket);

						int const iGold = int(i64NewMoney / iGoldToBronze);
						if (iGold < 0)
						{
							INFO_LOG(BM::LOG_LV5, __FL__ << L"[WARNING] TypeCast int_Gold is overflowed Player[" << pkCaster->Name() << L"] CurrentMoney[" << i64NewMoney << L"]");
						}

						PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_THERICH, iGold, m_pkGround->GroundKey());
						kSA.DoAction(pkCaster,NULL);
					}
				}break;
			case DISCT_MODIFY_CP:
				{
					int iNewCP = 0;
					kAddonData.Pop(iNewCP);

					const int iPrevCP = pkCaster->GetAbil(AT_CP);
					const int iDiffCP = int(iNewCP - iPrevCP);
					pkCaster->SetAbil(AT_CP, iNewCP);//�� ����.
					pkCaster->SendAbil(AT_CP, E_SENDTYPE_SELF);

					if( iDiffCP )//Ŭ���̾�Ʈ �ý��� �޽���
					{
						BM::Stream kPacket(PT_M_C_NFY_CHANGE_CP, (BYTE)m_kCause);
						kPacket.Push(iDiffCP);
						pkCaster->Send(kPacket);

						PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_PVP_RESULT_CP, iNewCP, m_pkGround->GroundKey());
						kSA.DoAction(pkCaster,NULL);
					}
				}break;
			case DISCT_MODIFY_SET_DATE_CONTENTS:
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						EDateContentsType eType = DCT_NONE;
						BM::DBTIMESTAMP_EX kDateTime;

						kAddonData.Pop( eType );
						kAddonData.Pop( kDateTime );

						pkPlayer->SetDateContents(eType, kDateTime);
					}
				}break;
			case DISCT_UPDATE_QUEST_CLEAR_COUNT:
			case DISCT_MODIFY_QUEST_END:
			case DISCT_MODIFY_QUEST_ING: //Begin/Drop(none)/Complete(finished)/Fail��
			case DISCT_MODIFY_QUEST_PARAM:
			case DISCT_MODIFY_QUEST_EXT:
				{
					int iType = 0;
					kAddonData.Pop(iType);
					int const iQuestID = iType;

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						switch( rkItemChange.State() )
						{
							case DISCT_MODIFY_QUEST_EXT:
							case DISCT_MODIFY_QUEST_END:
								{
									bNeedGroundQuestRefresh = true;
								}break;
							case DISCT_MODIFY_QUEST_ING: //Begin/Drop(none)/Complete(finished)/Fail��
								{
									int iValue = 0;
									kAddonData.Pop(iValue);

									bNeedGroundQuestRefresh = true;
									
									switch( iValue )
									{
									case QS_Begin:
										{
											PgQuestInfo const * pkOut;
											if( g_kQuestMan.GetQuest(iQuestID, pkOut) )
											{
												switch(pkOut->Type())
												{
												case QT_Couple:
												case QT_SweetHeart:
													{
														__int64 const iPlayTime = pkOut->m_kDepend_Couple.iTime;
														if( iPlayTime )
														{
															BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND);
															kPacket.Push( pkPlayer->GetID() );
															kPacket.Push( (BYTE) CC_SweetHeartQuestTimeInfo );
															kPacket.Push( false );
															kPacket.Push( iQuestID );
															kPacket.Push( iPlayTime );
															SendToCoupleMgr( kPacket );
														}
													}
													break;
												default:
													{
													}break;
												}
											}
										}break;
									case QS_Finished:
										{
											PgQuestInfo const * pkOut;
											if( g_kQuestMan.GetQuest(iQuestID, pkOut) )
											{
												switch(pkOut->Type())
												{
												case QT_Normal_Day:
												case QT_Event_Normal_Day:
												case QT_Normal: // ����
												case QT_Event_Normal:
													{
														PgAddAchievementValue kMA(AT_ACHIEVEMENT_NORMAL_QUEST_COMPLETE,1,GndKey());
														kMA.DoAction(pkPlayer,NULL);
													}break;
												case QT_Scenario: // ����
													{
														PgAddAchievementValue kMA(AT_ACHIEVEMENT_SCENARIO_QUEST_COMPLETE,1,GndKey());
														kMA.DoAction(pkPlayer,NULL);
													}break;
												case QT_Loop: // �ݺ�
												case QT_Event_Loop: // �ݺ�
												case QT_Scroll:
													{
														PgAddAchievementValue kMA(AT_ACHIEVEMENT_LOOP_QUEST_COMPLETE,1,GndKey());
														kMA.DoAction(pkPlayer,NULL);
													}break;
												case QT_RandomTactics:
												case QT_GuildTactics: // ���/�뺴
													{
														PgAddAchievementValue kMA(AT_ACHIEVEMENT_GUILD_QUEST_COMPLETE,1,GndKey());
														kMA.DoAction(pkPlayer,NULL);
													}break;
												case QT_Random: // ����G
													{
														PgAddAchievementValue kMA(AT_ACHIEVEMENT_RANDOM_QUEST_COMPLETE,1,GndKey());
														kMA.DoAction(pkPlayer,NULL);
													}break;
												case QT_Wanted:
												case QT_Day:
												case QT_Couple:		// Ŀ�� ����Ʈ
												case QT_SweetHeart: // ���� ����Ʈ												
												case QT_Soul:		// Soul Quest
												case QT_Week:
													{
													}break;
												default:
													{
														//
													}break;
												}
											}
										}break;
									case QS_None:
										{
											PgQuestInfo const * pkOut;
											if( g_kQuestMan.GetQuest(iQuestID, pkOut) )
											{
												switch(pkOut->Type())
												{
												case QT_Couple:
												case QT_SweetHeart:
													{
														if( pkOut->m_kDepend_Couple.iTime )
														{
															BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND);
															kPacket.Push( pkPlayer->GetID() );
															kPacket.Push( (BYTE) CC_SweetHeartQuestTimeInfo );
															kPacket.Push( true );
															SendToCoupleMgr( kPacket );
														}
													}
													break;
												default:
													{
													}break;
												}
											}
										}break;
									}
								}break;
							case DISCT_MODIFY_QUEST_PARAM:
								{
									int iParamNo = 0;
									int iValue = 0;

									kAddonData.Pop(iParamNo);
									kAddonData.Pop(iValue);

									SUserQuestState const *pkState = pkPlayer->GetQuestState(iQuestID);
									int const iOldParamValue = (pkState)? pkState->byParam[iParamNo]: 0;

									kChangeParamQuestIDSet.insert( iQuestID );
								}break;
						}

						pkPlayer->UpdateQuestResult(rkItemChange);

						if( DISCT_MODIFY_QUEST_END==rkItemChange.State() )
						{
							if(S_PST_QuestOnceMore * pkPremium = pkPlayer->GetPremium().SetType<S_PST_QuestOnceMore>())
							{
								PgQuestInfo const * pkOut;
								if( g_kQuestMan.GetQuest(iQuestID, pkOut) && pkPremium->IsCheck(pkOut->Type()) )
								{
									if( (pkOut->Type()==QT_Random && RandomQuest::IsCanRebuild(pkPlayer->GetMyQuest(), QT_Random))
									 || (pkOut->Type()==QT_RandomTactics && RandomQuest::IsCanRebuild(pkPlayer->GetMyQuest(), QT_RandomTactics)) )
									{
										CONT_PLAYER_MODIFY_ORDER kOrder;

										SPMOD_PremiumArticle kData(PST_QUEST_ONCE_MORE);
										kData.m_kPacket.Push(pkOut->Type());
										kOrder.push_back( SPMO(IMET_PREMIUM_ARTICLE_MODIFY, pkPlayer->GetID(), kData));

										int iFlag = BLQF_NONE;
										iFlag |= pkOut->Type()==QT_RandomTactics	? BLQF_TACTICS_RANDOM_QUEST : BLQF_NONE;
										iFlag |= pkOut->Type()==QT_Random			? BLQF_RANDOM_QUEST : BLQF_NONE;
										kOrder.push_back( SPMO(IMET_BUILD_DAYLOOP_FORCE, pkPlayer->GetID(), SPMOD_AddPacket(iFlag)) );

										PgAction_ReqModifyItem kItemModifyAction(CIE_PREMIUM_SERVICE, GndKey(), kOrder);
										kItemModifyAction.DoAction(pkPlayer, NULL);
									}
								}
							}
						}
					}
				}break;
			case DISCT_MODIFY_CLEAR_ING_QUEST:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						pkPlayer->ClearIngQuest();
					}
				}break;
			case DISCT_MODIFY_SET_GUID:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);				
					PgMySkill* pkMySkill = ( pkPlayer ? pkPlayer->GetMySkill() : NULL );

					if( pkMySkill )
					{
						int iType = 0;
						BM::GUID kGuid;
						kAddonData.Pop(iType);
						kAddonData.Pop(kGuid);

						switch( iType )
						{
						case SGT_Guild:
							{
								pkPlayer->GuildGuid( kGuid );

								if( pkMySkill
								&&	BM::GUID::IsNull(kGuid) )
								{
									pkMySkill->EraseSkill(SDT_Guild);
								}
								PgMyQuest const* pkMyQuest = (NULL != pkPlayer)? pkPlayer->GetMyQuest(): NULL;
								if( pkMyQuest
								&&	BM::GUID::IsNull(kGuid) )
								{
									ContUserQuestState kVec;
									pkMyQuest->GetQuestList(kVec);
									CONT_PLAYER_MODIFY_ORDER kFailedOrder;
									ContUserQuestState::const_iterator iter = kVec.begin();
									while( kVec.end() != iter )
									{
										int const iQuestID = (*iter).iQuestID;
										PgQuestInfo const* pkQuestInfo = NULL;
										if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
										{
											if( QT_GuildTactics == pkQuestInfo->Type()
											||	QT_RandomTactics == pkQuestInfo->Type() )
											{
												kFailedOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), SPMOD_AddIngQuest(iQuestID, QS_Failed)) ); // ���� ó��
											}
										}
										++iter;
									}

									if( !kFailedOrder.empty() )
									{
										PgAction_ReqModifyItem kCreateAction(IMEPT_QUEST, GndKey(), kFailedOrder);
										kCreateAction.DoAction(pkPlayer, NULL);
									}
								}
								bNeedGroundQuestRefresh = true;
							}break;
						case SGT_Couple:
							{
								pkPlayer->CoupleGuid( kGuid );

								if(	pkMySkill
								&&	BM::GUID::IsNull(kGuid) )
								{
									pkMySkill->EraseSkill(SDT_Couple);
								}
								bNeedGroundQuestRefresh = true;
							}break;
						case SGT_CoupleColor:
							{
								//if( BM::GUID::IsNotNull( kGuid ) )
								{
									pkPlayer->ForceSetCoupleColorGuid( kGuid );
								}
							}break;
						}
					}
				}break;
			case DISCT_MODIFY_ADD_RANK_POINT:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
 					if( pkPlayer )
 					{
 						int iType = 0;
 						int iAddRankPoint = 0;
 						kAddonData.Pop(iType);
 						kAddonData.Pop(iAddRankPoint);

						if ( E_RANKPOINT_MAXCOMBO == iType )
						{
							pkPlayer->SetAbil( AT_MAX_COMBO, pkPlayer->GetAbil(AT_MAX_COMBO) + iAddRankPoint );
						}
 					}
				}break;
			case DISCT_MODIFY_MISSION_EVENT:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						if( !m_kAddonPacket.IsEmpty() )
						{
							int iType = 0;
							int	iValue = 0;

							m_kAddonPacket.Pop(iType);
							m_kAddonPacket.Pop(iValue);
							
							int iSetValue = 0;

							switch( iType )
							{
							case E_MISSION_EVENT_SET:
								{
									if( 0 < iValue )
									{
										iSetValue = pkPlayer->GetAbil(AT_MISSION_EVENT);
										iSetValue |= (iValue);
										pkPlayer->SetAbil(AT_MISSION_EVENT, iSetValue);
									}
								}break;
							case E_MISSION_EVENT_RESET:
								{
									iSetValue = iValue;
									pkPlayer->SetAbil(AT_MISSION_EVENT, iSetValue);
								}break;
							case E_MISSION_ABIL_AWAKE_STATE:
								{
									if( 0 <= iValue )
									{
										pkPlayer->SetAbil(AT_AWAKE_STATE, iValue, true);
									}
								}break;
							case E_MISSION_ABIL_AWAKE_VALUE:
								{
									if( 0 <= iValue )
									{
										pkPlayer->SetAbil(AT_AWAKE_VALUE, iValue, true);
									}
								}break;
							default:
								{
								}break;
							}							
						}
					}
				}break;
			case DISCT_MODIFY_SWEETHEART:
				{
				}break;
			case DISCT_MODIFY_SET_PVP_RECORD:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if( pkPlayer )
					{
						SPMOD_SAbil kData;
						kData.ReadFromPacket( kAddonData );

						pkPlayer->SetPvPRecord( kData.Type(), kData.Value() );
					}
				}break;
			case DISCT_CREATE_MAIL:
				{
					PgAddAchievementValue kMA(AT_ACHIEVEMENT_SEND_MAIL,1,m_pkGround->GroundKey());
					kMA.DoAction(pkCaster,NULL);
				}break;
			case DISCT_REMOVE_DEALING:
			case DISCT_REMOVE_MARKET:
			case DISCT_OPEN_MARKET:
			case DISCT_ADD_ARTICLE:
			case DISCT_REMOVE_ARTICLE:
			case DISCT_BUY_ARTICLE:
			case DISCT_ADD_DEALING:
			case DISCT_MODIFY_MARKET_INFO:
			case DISCT_MODIFY_MARKET_STATE:
			case DISCT_EMPORIAFUNC_UPDATE:
				{//���� ó�� �Ҳ� ����. ���̵� ��Ŷ�� ��� ���� �Ǿ�����.
				}break;
			case DISCT_RENAME_PET:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						if ( true == pInv->Modify( rkItemChange.ItemWrapper().Pos(), rkItemChange.ItemWrapper() ) )
						{
							if ( rkItemChange.ItemWrapper().Guid() == pkPlayer->CreatePetID() )
							{
								PgItem_PetInfo *pkPetInfo = NULL;
								if ( true == rkItemChange.ItemWrapper().GetExtInfo( pkPetInfo ) )
								{
									PgPet *pkPet = m_pkGround->GetPet( pkPlayer );
									if ( pkPet )
									{
										pkPet->Name( pkPetInfo->Name() );
									}
								}	
							}		
						}
					}
				}break;
			case DISCT_SETABIL_PET:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						if ( true == pInv->Modify( rkItemChange.ItemWrapper().Pos(), rkItemChange.ItemWrapper() ) )
						{
							if ( PgItem_PetInfo::ms_kPetItemEquipPos == rkItemChange.ItemWrapper().Pos() )
							{
								PgPet *pkPet = m_pkGround->GetPet( pkPlayer );
								if ( pkPet && (pkPet->GetID() == rkItemChange.ItemWrapper().Guid()) )
								{
									PgItem_PetInfo *pkPetInfo = NULL;
									if ( true == rkItemChange.ItemWrapper().GetExtInfo( pkPetInfo ) )
									{
										SPMOD_AddAbilPet::CONT_ABILLIST kAbilList;
										kAddonData.Pop( kAbilList );
										SPMOD_AddAbilPet::CONT_ABILLIST::const_iterator abil_itr = kAbilList.begin();
										for ( ; abil_itr != kAbilList.end() ; ++abil_itr )
										{
											CAbilObject::DYN_ABIL::key_type const& rType = abil_itr->Type();

											switch ( rType )
											{
											case AT_EXPERIENCE:
												{
													pkPet->SetAbil64( rType, abil_itr->Value() );
												}break;
											case AT_HEALTH:
												{
													PgItem_PetInfo::SStateValue const kHealth( static_cast<int>(abil_itr->Value()) );
													pkPet->SetState_Health( kHealth );
												}break;
											case AT_MENTAL:
												{
													PgItem_PetInfo::SStateValue const kMental( static_cast<int>(abil_itr->Value()) );
													pkPet->SetState_Mental( kMental );
												}break;
											case AT_MON_SKILL_01:
											case AT_MON_SKILL_02:
											case AT_MON_SKILL_03:
												{
													// ����Ʈ�� �ٽ� �ɾ��־�� �Ѵ�.
													PgItem_PetInfo::SStateValue const kStateValue( static_cast<int>(abil_itr->Value()) );
													if ( PgPetSkillDataSet::SetSkill( pkPet, kStateValue ) )
													{
														PgActPet_RefreshPassiveSkill kRefreshPassiveSkillAction( m_pkGround );
														kRefreshPassiveSkillAction.DoAction( pkPet );
													}
												}break;
											case AT_LEVEL:
											case AT_GRADE:
												{
													bNeedGroundQuestRefresh = true;
												} // break ������� �ʰ� ������ default
											default:
												{
													pkPet->SetAbil( rType, static_cast<int>(abil_itr->Value()) );
												}break;
											}
										}
									}
								}
// 								else
// 								{
// 									if ( m_pkGround->CreatePet( pkPlayer, NULL ) )
// 									{
// 										BM::Stream kRevivePacket( PT_M_C_NFY_PET_REVIVE );
// 										pkPlayer->Send( kRevivePacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
// 									}
// 								}
								auto kRet = kItemSet.insert(rkItemChange.ItemWrapper().ItemNo());
							}
						}
					}
				}break;//DISCT_SETABIL_PET
			case DISCT_CREATE_PET:
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
					if ( pkPlayer )
					{
						BM::GUID kPetID;
						kAddonData.Pop( kPetID );
						
						SPetMapMoveData kPetData;
						kPetData.kInventory.OwnerGuid( kPetID );
						kPetData.kInventory.ReadFromPacket( kAddonData, WT_DEFAULT );
						PU::TLoadTable_AA( kAddonData, kPetData.kSkillCoolTime );

						PgPet *pkPet = m_pkGround->CreatePet( pkPlayer, kPetID, kPetData );
						if ( pkPet )
						{
							// Passive skill setting						
							PgActPet_RefreshPassiveSkill kRefreshPassiveSkillAction( m_pkGround );
							kRefreshPassiveSkillAction.DoAction( pkPet );
						}
					}
				}break;//DISCT_CREATE_PET
			case DISCT_REMOVE_PET:
				{
					BM::GUID kPetID;
					kAddonData.Pop( kPetID );
					m_pkGround->DeletePet( kPetID );
				}break;//DISCT_REMOVE_PET
			case DISCT_MODIFY_WORLD_MAP:
				{
					PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if ( pkPlayer )
					{
						int iMapNo = 0;
						kAddonData.Pop( iMapNo );
						pkPlayer->UpdateWorldMap( iMapNo );
					}
				}break;
			case DISCT_TRANSTOWER_SAVE_RECENT:
				{
					PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if ( pkPlayer )
					{
						SRecentInfo kRecentInfo;
						kRecentInfo.ReadFromPacket( kAddonData );
						pkPlayer->TransTowerSaveRct( kRecentInfo );
					}
				}break;//DISCT_TRANSTOWER_SAVE_RECENT
			case DISCT_JOBSKILL_SET_SKILL_EXPERTNESS:
			case DISCT_JOBSKILL_DEL_SKILL_EXPERTNESS:
			case DISCT_JOBSKILL_SAVE_EXHAUSTION:
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						JobSkillExpertnessUtil::Update(rkItemChange, kAddonData, *pkPlayer);
					}
				}break;
			case DISCT_PREMIUM_SERVICE_INSERT:
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						int iServiceNo = 0;
						BM::DBTIMESTAMP_EX kStartDate;
						BM::DBTIMESTAMP_EX kEndDate;
						BM::Stream kCustomData;
						kAddonData.Pop( iServiceNo );
						kAddonData.Pop( kStartDate );
						kAddonData.Pop( kEndDate );
						kAddonData.Pop( kCustomData.Data() );
						kCustomData.PosAdjust();

						pkPlayer->GetPremium().SetService(iServiceNo, kCustomData);
						pkPlayer->GetPremium().StartDate(kStartDate);
						pkPlayer->GetPremium().EndDate(kEndDate);

						//�����̾� ����Ʈ ���̱�
						if( S_PST_ApplyEffect const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_ApplyEffect>() )
						{
							SActArg kArg;
							PgGroundUtil::SetActArgGround(kArg, m_pkGround);
							for(VEC_INT::const_iterator c_it=pkPremium->kContEffect.begin(); c_it!=pkPremium->kContEffect.end(); ++c_it)
							{
								pkPlayer->AddEffect(*c_it, 0, &kArg, pkPlayer);
							}
						}
					}
				}break;
			case DISCT_PREMIUM_SERVICE_MODIFY:
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						int iServiceNo = 0;
						BM::DBTIMESTAMP_EX kStartDate;
						BM::DBTIMESTAMP_EX kEndDate;
						kAddonData.Pop( iServiceNo );
						kAddonData.Pop( kStartDate );
						kAddonData.Pop( kEndDate );

						pkPlayer->GetPremium().EndDate(kEndDate);
					}
				}break;
			case DISCT_PREMIUM_SERVICE_REMOVE:
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						if( S_PST_ApplyEffect const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_ApplyEffect>() )
						{
							for(VEC_INT::const_iterator c_it=pkPremium->kContEffect.begin(); c_it!=pkPremium->kContEffect.end(); ++c_it)
							{
								pkPlayer->DeleteEffect(*c_it);
							}
						}

						pkPlayer->GetPremium().Clear();
					}
				}break;
			case DISCT_PREMIUM_ARTICLE_MODIFY:
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						int iArticleType = 0;
						kAddonData.Pop(iArticleType);

						switch(iArticleType)
						{
						case PST_QUEST_ONCE_MORE:
							{
								if(S_PST_QuestOnceMore * pkPremium = pkPlayer->GetPremium().SetType<S_PST_QuestOnceMore>())
								{
									EQuestType eType = QT_None;
									BM::PgPackedTime kNextTime;
									kAddonData.Pop(eType);								
									kAddonData.Pop(kNextTime);
									pkPremium->AddQuestType(eType, kNextTime);
								}
							}break;
						}
					}
				}break;
			default:
				{
					PgBase_Item kPrevItem;
					if (SUCCEEDED(pInv->GetItem(rkItemChange.ItemWrapper().Pos(), kPrevItem)))
					{
						//INFO_LOG(BM::LOG_LV9, __FL__ <<  L"Item Changed Duration[" << pkPrevItem->Count() << L"]"));
					}

					SItemPos const &rkItemPos = (rkItemChange.State() == DISCT_REMOVE) ? rkItemChange.PrevItemWrapper().Pos(): rkItemChange.ItemWrapper().Pos();
					int iChangedItemNo = rkItemChange.ItemWrapper().ItemNo();

					if(	DISCT_REMOVE == rkItemChange.State()
					||	DISCT_REMOVE_IMAGE == rkItemChange.State())
					{
						if(	SUCCEEDED(pInv->GetItem(rkItemChange.ItemWrapper().Pos(), kPrevItem))
						&&	kPrevItem.ItemNo() )//���� �������� ��ġ�� �־�����
						{
							iChangedItemNo = kPrevItem.ItemNo();
						}
					}

					bool const bModifyRet = pInv->Modify(rkItemPos, rkItemChange.ItemWrapper());
					auto kRet = kItemSet.insert(iChangedItemNo);//������ �־��ٸ�.		

					if( (IT_FIT == rkItemChange.ItemWrapper().Pos().x) || 
						(IT_FIT_CASH == rkItemChange.ItemWrapper().Pos().x) ||
						(IT_FIT_COSTUME == rkItemChange.ItemWrapper().Pos().x)
					&&	0 != (rkItemChange.Cause() & IMC_UNIT_ABIL_CHANGED) )
					{
						bAbilRefresh = true;
					}

					AddNotifyItem(rkItemChange, kItemVec);

					if(IMC_DEC_DUR_BY_USE & rkItemChange.Cause())
					{
						PgBase_Item const & kUsedItem = rkItemChange.PrevItemWrapper();
						PgAction_UseItem kAction( m_pkGround, kUsedItem, kUsedItem.Count() - rkItemChange.ItemWrapper().Count(), m_kAddonPacket);
						kAction.DoAction(pkCaster, NULL);

						GET_DEF(CItemDefMgr, kItemDefMgr);
						CItemDef const* pItemDef = kItemDefMgr.GetDef(kUsedItem.ItemNo());
						if(NULL != pItemDef)
						{
							int const iTargetType = pItemDef->GetAbil(AT_TARGET_TYPE);
							if(ESTARGET_NONE != iTargetType)
							{
								SEventMessage kEventMsg;
								kEventMsg.PriType(PMET_GROUND_MGR);
								kEventMsg.SecType(PT_M_T_USEITEM);
								kEventMsg.Push(pkCaster->GetID());
								kEventMsg.Push(m_pkGround->GroundKey());
								kUsedItem.WriteToPacket(kEventMsg);
								kEventMsg.Push(m_kAddonPacket);
								g_kTask.PutMsg(kEventMsg);
							}

							{// ������ ��뿡 ���� ������ �ִٸ� �̰��� �߰�
								int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
								switch(iCustomType)
								{
								case UICT_MOVETOPARTYMEMBER: // ��Ƽ���� ������ ��� ����
									{
										PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_PARTYMOVE, 1, m_pkGround->GroundKey() );
										kMA.DoAction(pkCaster,NULL);
									}break;
								default:
									{
									}break;
								}
							}
						}
					}

					// �������� �ٴڳ��� ��Ȱ��ȭ �ǰų� �����ؼ� �ٽ� Ȱ��ȭ �ɶ��� ó��
					bool bFitItem = (IT_FIT == rkItemChange.ItemWrapper().Pos().x) || (IT_FIT_CASH == rkItemChange.ItemWrapper().Pos().x) || (IT_FIT_COSTUME == rkItemChange.ItemWrapper().Pos().x);
					if((bFitItem) && (IMC_DEC_DUR_BY_BATTLE | IMC_INC_DUR_BY_REPAIR | IMC_DEC_DUR_BY_PENALTY) & rkItemChange.Cause())
					{
						if(rkItemChange.ItemWrapper().EnchantInfo().IsNeedRepair() != rkItemChange.PrevItemWrapper().EnchantInfo().IsNeedRepair())
						{
							bAbilRefresh = true;
							DoEquipEffect(pkCaster,rkItemChange);
						}
					}

					// ItemChange ���� SItemPos() �������� �Ѿ� �´�.
					// ��, ��������ġ ��ȯ�� ��쿡
					//	ItemPos()���� ������� �ʰ�
					//	ItemWrapper() ���� ���ο� �����۹�ȣ / PrevItemWrapper() ���� ���������۹�ȣ
					//	�� �ԷµǾ� �ִ�.
					if ( 0 != (IMET_MODIFY_POS & rkItemChange.Cause()))
					{
						if ( bFitItem )
						{
							/*
							// Item �̵� ��Ŷ�� �ʼ������� �޾��� ��, AddCoolTime �ϵ��� ����Ǿ���.
							// �ֳ��ϸ� : Item ������ ���� ������ ���� ����, Client�� ���� Action ��û�� �� �� �ֱ� �����̴�.
							// CoolTime : ����������
							pkCaster->GetInven()->AddCoolTime(rkItemChange.ItemWrapper().ItemNo(), PgInventory::EICool_Equip);
							// CoolTime : ��������������
							pkCaster->GetInven()->AddCoolTime(rkItemChange.PrevItemWrapper().ItemNo(), PgInventory::EICool_Unequip);
							*/
							DoEquipEffect(pkCaster, rkItemChange);

							switch( rkItemChange.ItemWrapper().Pos().y )
							{
							case EQUIP_POS_PET:
								{
									bNeedGroundQuestRefresh = true; // ĳ������ ����� ����Ʈ�� �ٽ� üũ �Ѵ�
								}break;
							default:
								{
								}break;
							}

							CheckAchievementEquipItem(pkCaster);
						}
						//else if (IT_FIT == rkItemChange.PrevItemWrapper().Pos().x)	//���� ���� �Ͽ���.
						//{
						//	pkCaster->GetInven()->AddCoolTime(rkItemChange.PrevItemWrapper().ItemNo(), PgInventory::EICool_Unequip);
						//}
					}
				}break;
			}

			++change_itor;
		}

		ResultProcess(pkCaster, pkTarget);//������ ���� �̺�Ʈ�� ���ؼ� ��� ó��

		PgQuestActionUtil::CheckItemQuestOrder(GndKey(), kItemSet, pkCaster);//����Ʈ üũ
		NotifyPartyMember(kItemVec, pkCaster, pkTarget);//��Ƽ������ �˸�

		PgAction_SendItemChange kSendAction(m_kCause, GndKey(), m_kChangeArray, m_kLocalPartyMgr);
		kSendAction.DoAction(pkCaster, pkTarget);//�˸�.

		if( bAbilRefresh )// Equip�� �ٲ���� ���� �� ����.
		{
			pkCaster->NftChangedAbil(AT_REFRESH_ABIL_INV, E_SENDTYPE_SELF);

			{// ��Ʈ ������ ���� ����
				PgInventory* pkInv = pkCaster->GetInven();
				if( pkInv )
				{
					// Set item
					CAbilObject kSetItemAbil;

					GET_DEF( CItemDefMgr, kItemDefMgr );
					GET_DEF( CItemSetDefMgr, kItemSetDefMgr );

					CONT_HAVE_ITEM_DATA kFitCont;			// ������ ��� ������
					CONT_HAVE_ITEM_DATA kFitSetCont;		// ������ ��Ʈ ������
					PgInventory::CONT_SETITEM_NO kSetCont;	// ������ �����۵��� ��Ʈ��ȣ

					pkInv->GetItems( IT_FIT, kFitCont );
					CONT_HAVE_ITEM_DATA::iterator kFitItor = kFitCont.begin();
					while( kFitItor != kFitCont.end() )
					{
						PgBase_Item const &kItem = (*kFitItor).second;
						if( false == PgInventoryUtil::CheckDisableItem( kItem, pkCaster ) )
						{
							int const iSetNo = kItemSetDefMgr.GetItemSetNo( kItem.ItemNo() );
							if( iSetNo)
							{
								kFitSetCont.insert( std::make_pair( (*kFitItor).first, (*kFitItor).second ) );
								kSetCont.insert( iSetNo );
							}
						}

						++kFitItor;
					}

					PgInventory::CONT_SETITEM_NO::iterator set_itor = kSetCont.begin();
					while(set_itor != kSetCont.end())
					{
						int const iSetNo = (*set_itor);

						CItemSetDef const *pOrgSetDef = kItemSetDefMgr.GetDef(iSetNo);

						if(pOrgSetDef)
						{
							bool bCompleteSet = false;
							int const iEquipPiece = pOrgSetDef->CheckNeedItem(kFitCont, pkCaster, bCompleteSet);
							if( iEquipPiece )
							{
								CItemSetDef const *pEquipSetDef = kItemSetDefMgr.GetEquipAbilDef(iSetNo, iEquipPiece);
								if( pEquipSetDef )
								{
									kSetItemAbil.operator +=(*pEquipSetDef);
									if( bCompleteSet )
									{// Ǯ���̴� 

										int iMaxLevelLimit = 0; // ��Ʈ������ ������ �䱸������ Ʋ���� ������ ���� ū���� �������� ������ üũ.
										CONT_HAVE_ITEM_DATA::iterator kFitSetItor = kFitSetCont.begin();
										while( kFitSetItor != kFitSetCont.end() )
										{
											const PgBase_Item &kItem = (*kFitSetItor).second;
											const CItemDef* pkItemDef = kItemDefMgr.GetDef( kItem.ItemNo() );
											int iLevelLimit = pkItemDef->GetAbil( AT_LEVELLIMIT );
											iMaxLevelLimit = std::max( iMaxLevelLimit, iLevelLimit );

											++kFitSetItor;
										}

										PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
										if ( pkPlayer )
										{
											if( iMaxLevelLimit >= 30 && iMaxLevelLimit <= 39 )
											{// 30���� ��Ʈ
												PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_EQUIP_SET30, 0, pkPlayer->GroundKey() );
												kSA.DoAction( pkCaster, NULL );
											}
											else if( iMaxLevelLimit >= 40 && iMaxLevelLimit <= 49 )
											{// 40���� ��Ʈ
												PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_EQUIP_SET40, 0, pkPlayer->GroundKey() );
												kSA.DoAction( pkCaster, NULL );
											}
											else if( iMaxLevelLimit >= 50 && iMaxLevelLimit <= 59 )
											{// 50���� ��Ʈ
												PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_EQUIP_SET50, 0, pkPlayer->GroundKey() );
												kSA.DoAction( pkCaster, NULL );
											}
											else if( iMaxLevelLimit >= 60 && iMaxLevelLimit <= 69 )
											{// 60���� ��Ʈ
												PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_EQUIP_SET60, 0, pkPlayer->GroundKey() );
												kSA.DoAction( pkCaster, NULL );
											}
											else
											{

											}
										}
									}
								}
							}
						}

						++set_itor;
					}

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if ( pkPlayer )
					{
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_ATTACK_FIRE, pkPlayer->GetAbil(AT_C_ATTACK_ADD_FIRE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_ATTACK_ICE, pkPlayer->GetAbil(AT_C_ATTACK_ADD_ICE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_ATTACK_NATURE, pkPlayer->GetAbil(AT_C_ATTACK_ADD_NATURE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_ATTACK_CURSE, pkPlayer->GetAbil(AT_C_ATTACK_ADD_CURSE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_ATTACK_DESTORY, pkPlayer->GetAbil(AT_C_ATTACK_ADD_DESTROY));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_ATTACK_5ELEMENT, 0);

						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_RESIST_FIRE, pkPlayer->GetAbil(AT_C_RESIST_ADD_FIRE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_RESIST_ICE, pkPlayer->GetAbil(AT_C_RESIST_ADD_ICE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_RESIST_NATURE, pkPlayer->GetAbil(AT_C_RESIST_ADD_NATURE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_RESIST_CURSE, pkPlayer->GetAbil(AT_C_RESIST_ADD_CURSE));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_RESIST_DESTORY, pkPlayer->GetAbil(AT_C_RESIST_ADD_DESTROY));
						DoAction_SetAchievementValue(pkPlayer, AT_ACHIEVEMENT_ATTR_RESIST_5ELEMENT, 0);
					}
				}
			}									
			::RefrashElemStatusEffect(pkCaster, m_pkGround);
		}
		if( bNeedGroundQuestRefresh )
		{
			RefreshGroundQuestInfo(pkCaster);
		}
		if( !kChangeParamQuestIDSet.empty() )
		{
			CONT_PLAYER_MODIFY_ORDER kNextOrder;

			ContQuestIDSet::const_iterator loop_iter = kChangeParamQuestIDSet.begin();
			while( kChangeParamQuestIDSet.end() != loop_iter )
			{
				PgCheckQuestComplete kCheckAction((*loop_iter), kNextOrder, NULL);
				kCheckAction.DoAction(pkCaster, NULL);

				++loop_iter;
			}

			if( !kNextOrder.empty() )
			{
				PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, GndKey(), kNextOrder, BM::Stream(), true);//�������� �𸣰ڴµ� ��� ���� ��� ������ ����.
				kItemModifyAction.DoAction(pkCaster, NULL);
			}
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgAction_ModifyPlayerData::CheckAchievementEquipItem(CUnit * pUnit)
{
	PgInventory * pInv = pUnit->GetInven();

	CONT_HAVE_ITEM_DATA kFitCont;
	pInv->GetItems(IT_FIT, kFitCont);

	std::map<E_ITEM_GRADE,int> kContItemGrade;
	for(CONT_HAVE_ITEM_DATA::const_iterator iter = kFitCont.begin();iter != kFitCont.end();++iter)
	{
		E_ITEM_GRADE const kGrade = GetItemGrade((*iter).second);
		kContItemGrade[kGrade]++;
	}

	for(std::map<E_ITEM_GRADE,int>::const_iterator iter = kContItemGrade.begin();iter != kContItemGrade.end();++iter)
	{
		int const iAchievementType = AT_ACHIEVEMENT_EQUIP_ITEMGRADE0+(*iter).first;
		if(AT_ACHIEVEMENT_EQUIP_ITEMGRADE4 >= iAchievementType)
		{
			PgSyncClinetAchievementHandler<> kSA(iAchievementType,(*iter).second, m_pkGround->GroundKey());
			kSA.DoAction(pUnit,NULL);
		}
	}
}

void PgAction_ModifyPlayerData::AddNotifyItem(DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkItemChange, CONT_ITEM_VEC& rkVec)
{
	switch( m_kCause )
	{
	case CIE_Loot://����
	case CIE_QuestItem:
	case CIE_ShineStone:
	case CIE_Mission://��Ƽ������ �˸���.
		{
			int const iDiffCount = rkItemChange.ItemWrapper().Count() - rkItemChange.PrevItemWrapper().Count();
			if( 0 < iDiffCount)
			{
				PgBase_Item kNfyItem = rkItemChange.ItemWrapper();
				kNfyItem.Count(iDiffCount);
				rkVec.push_back(kNfyItem);
			}
		}break;
	}
}

void PgAction_ModifyPlayerData::NotifyPartyMember(const CONT_ITEM_VEC& rkItemVec, CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
		return;
	}

	if( rkItemVec.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ItemVec is Empty"));
		return;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
		return;
	}

	if( GUID_NULL == pkPC->PartyGuid() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("PartyGuid is NULL"));
		return;
	}

	BM::Stream kTempPacket(PT_M_C_NFY_PARTY_MEMBER_ITEM);
	kTempPacket.Push(pkCaster->GetID()); // ����
	PU::TWriteArray_M(kTempPacket, rkItemVec);

	VEC_GUID kGuidVec;
	m_kLocalPartyMgr.GetPartyMemberGround(pkPC->PartyGuid(), GndKey(), kGuidVec, pkPC->GetID());

	BM::Stream kNfyPacket(PT_U_G_SEND_TO_USERS);
	kNfyPacket.Push(kGuidVec);
	kNfyPacket.Push(kTempPacket.Data());
	
	pkCaster->VNotify(&kNfyPacket);
}

void PgAction_ModifyPlayerData::ResultProcess(CUnit* pkCaster, CUnit* pkTarget)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);

	switch(m_kCause)
	{
	case CIE_Dump:
		{
			if(S_OK == m_kRet)
			{
				std::vector<int> kContSoule;
				m_kAddonPacket.Pop(kContSoule);
				for(std::vector<int>::const_iterator iter = kContSoule.begin();iter != kContSoule.end();++iter)
				{
					PgAddAchievementValue kMA(AT_ACHIEVEMENT_ITEM_BREAKER, (*iter), GndKey());
					kMA.DoAction(pkCaster,NULL);
				}
			}
		}break;
	case CIE_ExtractElement:
		{
			int iType = 0;
			int iCount = 0;
			m_kAddonPacket.Pop(iType);
			m_kAddonPacket.Pop(iCount);
			
			if(iType)
			{
				PgAddAchievementValue kMA(iType, iCount, GndKey());
				kMA.DoAction(pkCaster,NULL);
			}
		}break;
	case CAE_Achievement2Inv:
		{
			BM::Stream kPacket(PT_M_C_ANS_ACHIEVEMENT_TO_ITEM);

			if(S_OK == m_kRet)
			{
				kPacket.Push(AR_SUCCESS);
			}
			else if(E_NOT_ENOUGH_INV_SPACE == m_kRet)
			{
				kPacket.Push(AR_INVENTORY_FULL);
			}
			else
			{
				kPacket.Push(AR_DB_ERROR);
			}

			kPacket.Push(m_kAddonPacket);

			pkCaster->Send(kPacket);
		}break;
	case CIE_ChangeClass_ByNormal:
		{
		}break;
	case CIE_ChangeClass_ByGMCmd://Ŀ���� Ŭ���� �ٲ� ����. ���� �ʱ�ȭ.
		{
			CONT_PLAYER_MODIFY_ORDER kOrder;
			SPMOD_AddSkill kAddSkillData(0, true);//����. 
			kOrder.push_back(SPMO(IMET_ADD_SKILL, pkPlayer->GetID(), kAddSkillData));
			PgAction_ReqModifyItem kItemModifyAction(IMEPT_GODCMD, m_pkGround->GroundKey(), kOrder);
			kItemModifyAction.DoAction(pkPlayer, NULL);
		}break;
	case MCE_Skill:
		{
		}break;
	case CIE_CoupleLearnSkill:
		{
			if( S_OK == m_kRet )
			{
				BYTE cCmdType = 0;
				m_kAddonPacket.Pop(cCmdType);

				switch( cCmdType )
				{
				case CC_LearnSkill:
					{
					}break;
				case CC_Nfy_AddSkill:
					{
						pkCaster->Send(m_kAddonPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);
					}break;
				case CC_Ans_Couple:
				case CC_Ans_SweetHeart_Complete:
					{
						bool bFirstPacket = false;

						m_kAddonPacket.Pop(bFirstPacket);

						DWORD const dwSendType = (bFirstPacket)? E_SENDTYPE_BROADALL: E_SENDTYPE_SELF;
						pkCaster->Send(m_kAddonPacket, dwSendType);
					}break;
				default:
					{
					}break;
				}
			}
		}break;
	case CIE_SafeMode:
		{
			if( S_OK == m_kRet )
			{
				//pkCaster->Send(m_kAddonPacket);
			}
		}break;
	case MCE_Fran:
		{
			if( S_OK == m_kRet )//���� ������ �׶��� �ڽ� ����
			{
				pkCaster->Send(m_kAddonPacket);
			}
		}break;
	case MCE_MarryMoney:
		{
			BYTE cCmdType = 0;
			__int64 i64SendAdd = 0;

			m_kAddonPacket.Pop( cCmdType );
			m_kAddonPacket.Pop( i64SendAdd );
			if( S_OK == m_kRet )//���� ������ �׶��� �ڽ� ����
			{
				switch( cCmdType )
				{
				case MC_AnsMarry:
					{
						BM::GUID kCharGuid;
						BM::GUID kCoupleGuid;

						m_kAddonPacket.Pop( kCharGuid );
						m_kAddonPacket.Pop( kCoupleGuid );

						CUnit* pkCoupleUnit = m_pkGround->GetUnit(kCoupleGuid);
						if( !pkCoupleUnit )
						{
							break;
						}


						BM::Stream kPacket(PT_C_M_REQ_MARRY_COMMAND);
						kPacket.Push((BYTE)MC_Init);
						kPacket.Push(pkPlayer->GetID());
						kPacket.Push(GndKey());

						kPacket.Push( i64SendAdd );
						kPacket.Push( kCharGuid );
						kPacket.Push( kCoupleGuid );
						kPacket.Push(pkPlayer->Name());
						kPacket.Push(pkCoupleUnit->Name());
						kPacket.Push(pkPlayer->GetAbil(AT_GENDER));
						kPacket.Push(pkCoupleUnit->GetAbil(AT_GENDER));

						SendToCenter(kPacket);


						BM::Stream kNfyPacket;
						kNfyPacket.Push( PT_T_C_NFY_MARRY );
						GndKey().WriteToPacket(kNfyPacket);
						kNfyPacket.Push( static_cast<BYTE>(EM_MARRY) );
						kNfyPacket.Push( kCharGuid );
						kNfyPacket.Push( kCoupleGuid );
						kNfyPacket.Push( GndKey() );
						SendToGround(GndKey(), kNfyPacket);

						pkCaster->Send(m_kAddonPacket);
						pkCoupleUnit->Send(m_kAddonPacket);
					}break;
				case MC_AnsSendMoney:
					{
						BM::Stream kPacket(PT_C_M_REQ_MARRY_COMMAND);
						kPacket.Push((BYTE)MC_AnsSendMoney);
						kPacket.Push(pkPlayer->GetID());
						kPacket.Push(GndKey());
						kPacket.Push( i64SendAdd );

						SendToCenter(kPacket);

						pkCaster->Send(m_kAddonPacket);

						PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_MARRYMONEY,i64SendAdd,m_pkGround->GroundKey());
						kSA.DoAction(pkCaster,NULL);
					}break;
				default:
					{
					}break;
				}
			}
		}break;
	case MCE_Loot:
		{
			if( S_OK == m_kRet )//���� ������ �׶��� �ڽ� ����
			{
			}
		}break;
	case CIE_Loot:
		{
			if( S_OK == m_kRet )//���� ������ �׶��� �ڽ� ����
			{
			}
			else
			{
				pkCaster->SendWarnMessage(400715);// �κ��丮�� ������ �����մϴ�
			}
		}break;
	case MCE_CreateGuild:
		{
			if( S_OK == m_kRet )// ��� ����� ��
			{
				BM::Stream kPacket(PT_C_N_REQ_GUILD_COMMAND, pkCaster->GetID());
				kPacket.Push((BYTE)GC_Create);
				kPacket.Push(m_kAddonPacket);
				SendToGuildMgr(kPacket);
			}
			else// ��� ����µ� ��� �����۰� ���� ����
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L"[GUILD-Create: " << pkCaster->Name() << L"] Failed - Not enough money");
				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_PreCreate);
				kPacket.Push((BYTE)GCR_Money);
				pkCaster->Send(kPacket);
			}
		}break;
	case CIE_QuestItem:
		{
			if( S_OK != m_kRet )// ����
			{
				pkCaster->SendWarnMessage(700099);// ������ ����Ʈ ������ ���Ծ�
			}
		}break;
	case CIE_GateWayUnLock:
		{
			m_pkGround->ProcessGateWayUnLock(m_kRet, pkCaster, m_kAddonPacket);
		}break;
	//case IMEPT_QUEST_REWARD://����Ʈ
	case IMEPT_QUEST_DIALOG:
	case IMEPT_QUEST_DIALOG_Accept:
	case IMEPT_QUEST_DIALOG_End:
	//case CIE_MissionQuestCard://�̼�����Ʈī��
	case CIE_GuildLevelUp://��� ������
	case CIE_GuildInventoryCreate:
	case CIE_Post_Mail_Send:
	case CIE_GuildLearnSkill:
	case CIE_Sys2Inv:
	case CIE_Delete_SysItem:
	case CIE_UserMapMove:
	case CIE_MoveToPartyMember:
	case CIE_RentalSafeExtend:
	case CIE_UM_Market_Open:
	case CIE_UM_Article_Reg:
	case CIE_UM_Article_Dereg:
	case CIE_UM_Article_Query:
	case CIE_UM_Article_Buy:
	case CIE_UM_Dealing_Query:
	case CIE_UM_Dealing_Read:
	case CIE_UM_Modify_Market:
	case CIE_UM_Modify_Market_State:
	case CIE_UM_Market_Remove:
	case CIE_SummonPartyMember:
	case MIE_UseItem:
	case CIE_OpenChest:
	case CIE_OpenPack:
	case CIE_OpenPack2:
	case CIE_HiddenPack:
	case CIE_Create_CharacterCard:
	case CIE_Modify_RecommendPoint:
	case CIE_Modify_CharacterCard:
	case CIE_Delete_CharacterCard:
	case CIE_Modify_UserPortal:
	case CIE_Gen_Socket:
	case CIE_Set_MonsterCard:
	case CIE_Remove_MonsterCard:
	case CIE_Del_MonsterCard:
	case CIE_GemStore_Buy:
	case CIE_CollectAntique:
	case CIE_ExchangeGem:
	case CIE_Rollback:
	case CIE_EnchantLvUp:
	case CIE_SoulCraft:
	case CIE_BasicOptionAmp:
	case CIE_Open_Gamble:
	case CIE_Locked_Chest:
	case CIE_Convert_Item:
	case CIE_Use_ExpCard:
	case CIE_Buy_MyHome:
	case CIE_MixupItem:
	case CIE_Home_Equip:
	case CIE_Home_UnEquip:
	case CIE_Home_Auction_Reg:
	case CIE_Home_Auction_Unreg:
	case CIE_Home_Auction_End:
	case CIE_Home_Modify:
	case CIE_Home_Bidding:
	case CIE_Home_Item_Modify:
	case CIE_Home_PayTex:
	case CIE_Home_Unit_Sync:
	case CIE_GambleMachine:
	case CIE_OpenEventItemReward:
	case CIE_SendEventItemReward:
	case CIE_GambleMachine_Shop:
	case CIE_Home_SideJob_Insert:
	case CIE_Home_SideJob_Remove:
	case CIE_Home_SideJob_Modify:
	case CIE_Home_SideJob_Enter:
	case CIE_Inventory_Sort:
	case CIE_Item_Enchant_Shift:
	case CIE_JOBSKILL3_CREATE_ITEM:	
	case IMEPT_GODCMD:
	case CIE_PREMIUM_SERVICE:
	case CIE_Constellation:
	case CIE_EXTRACTION_MonsterCard:
	case CIE_UserQuestComplete:
		{
			switch(m_kRet)
			{
			case S_OK:
				{
					DB_ITEM_STATE_CHANGE_ARRAY::const_iterator iter = m_kChangeArray.begin();//����Ʈ üũ
					while(m_kChangeArray.end() != iter)
					{
						DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkElement = (*iter);
						PgItemWrapper const& kResultModify = rkElement.ItemWrapper();

						if( kResultModify.ItemNo() )
						{
							PgAction_QuestUpgrade kQuestUpgradeAction(GndKey(), kResultModify);
							kQuestUpgradeAction.DoAction(pkCaster, NULL);
						}

						++iter;
					}

					if(CIE_Home_Equip == m_kCause) // ����Ȩ ������ ���� Ƚ�� ����
					{
						PgAddAchievementValue kMA(AT_ACHIEVEMENT_MYHOME_EQUIP,1,m_pkGround->GroundKey());
						kMA.DoAction(pkTarget,NULL);
					}

					if(CIE_JOBSKILL3_CREATE_ITEM == m_kCause)
					{
						BM::Stream kTempPacket(m_kAddonPacket);
						kTempPacket.RdPos( sizeof(PACKET_ID_TYPE) );
						HRESULT hRet = S_OK;
						int iSaveIdx = 0;
						kTempPacket.Pop( hRet );
						kTempPacket.Pop( iSaveIdx );

						BM::Stream kSendContents(PT_N_T_REQ_JOBSKILL3_HISTORYITEM, pkCaster->GetID());
						kSendContents.Push(iSaveIdx);
						SendToContents(kSendContents);


						//���õ� ���
						SJobSkillSaveIdx const * const pkDefSaveIdx = JobSkill_Third::GetJobSkillSaveIdx(iSaveIdx);
						if( pkDefSaveIdx )
						{
							PgAction_JobSkill3_AddExpertness kAddExpertness(CIE_JOBSKILL, pkDefSaveIdx->iRecipeItemNo, GndKey());
							kAddExpertness.DoAction(pkCaster, NULL);
						}
					}
				}break;
			case E_NOT_ENOUGH_INV_SPACE:
				{
					pkCaster->SendWarnMessage(700037);
				}break;
			case E_CANT_DEFEND_REWORD_FAIL:
				{
					pkCaster->SendWarnMessage(799016);
				}break;
			default:
				{
				}break;
			}

			PgAction_ModifyItem_AddOnPacketProcess kAddOnAction(Ret(), m_pkGround, m_kAddonPacket);
			kAddOnAction.DoAction(pkCaster, NULL);
		}break;
	case CIE_CoupleWarp:
		{
			POINT3			pt3TargetPos;//���� �� �ڸ�
			SGroundKey		kTargetKey;

			m_kAddonPacket.Pop( pt3TargetPos );
			m_kAddonPacket.Pop( kTargetKey );

			if( S_OK == m_kRet )
			{
				BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND, pkCaster->GetID());
				kPacket.Push( (BYTE)CC_Req_Warp2 );
				kPacket.Push( pt3TargetPos );
				kPacket.Push( kTargetKey );
				SendToCoupleMgr(kPacket);
			}
			else
			{
				BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Warp);
				kPacket.Push( (BYTE)CoupleCR_None );
				pkCaster->Send(kPacket);
			}
		}break;
	case MCE_BuyItem:
	case CIE_CoinChange_Buy:
		{
			switch(m_kRet)
			{
			case E_NOT_ENOUGH_MONEY:
				{
					pkCaster->SendWarnMessage(700036);
				}break;
			case E_NOT_ENOUGH_INV_SPACE:
				{
					pkCaster->SendWarnMessage(700037);
				}break;
			case E_NOT_ENOUGH_CP:
				{
					pkCaster->SendWarnMessage(700127);
				}
				break;
			case S_OK:
				{
					// do nothing
				}break;
			default:
				{
					pkCaster->SendWarnMessage(700114);
				}break;
			}
		}break;
	case CIE_Exchange:
		{//-> ���⼭.
			BM::Stream kPacket(PT_M_C_NFY_EXCHANGE_ITEM_RESULT);
			if (S_OK==m_kRet)
			{
				kPacket.Push((int)TR_SUCCESS);
			}
			else
			{
				kPacket.Push((int)TR_FAIL);
			}
			pkCaster->Send(kPacket);
		}break;
	case CIE_Make:
		{
			// ���� Ƚ��(n)   = m_kAddonPacket
			// {
			//   ���� ��ȣ
			//   ���õ� ����
			// }

			if(S_OK == m_kRet)
			{
				CONT_RESULT_ITEM_DATA kContResultItem;
				int iItemMakingType = 0;
				int iMakingSuccessCount = 0;
				PU::TLoadTable_MM(m_kAddonPacket, kContResultItem);
				m_kAddonPacket.Pop(iItemMakingType);
				m_kAddonPacket.Pop(iMakingSuccessCount);
				if( 0 < iMakingSuccessCount )
				{
					//���� �ؾ߸�. ���� ���
					PgAddAchievementValue kMA(AT_ACHIEVEMENT_COOKER, iMakingSuccessCount, GndKey()); // ���� ���
					kMA.DoAction(pkCaster,NULL);
				}

				BM::Stream kPacket(PT_M_C_NFY_ITEM_MAKING_RESULT);
				PU::TWriteTable_MM(kPacket, kContResultItem);
				kPacket.Push(iItemMakingType);
				pkCaster->Send(kPacket);
			}
			else
			{
				pkPlayer->SendWarnMessage(400715); //�κ��丮�� ������ �����մϴ�.
			}

		}break;
	case CIE_Repair:
		{
			if( S_OK == m_kRet )
			{
				DB_ITEM_STATE_CHANGE_ARRAY::const_iterator iter = m_kChangeArray.begin();//����Ʈ üũ
				while(m_kChangeArray.end() != iter)
				{
					DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkElement = (*iter);
					PgItemWrapper const& kResultModify = rkElement.ItemWrapper();

					if( kResultModify.ItemNo() )
					{
						PgAction_QuestRepair kQuestRepairAction(GndKey(), kResultModify);
						kQuestRepairAction.DoAction(pkCaster, NULL);
					}
					++iter;
				}
			}
		}break;
	case CIE_OpenTreasureChestReward:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					int iRewardItemNo = 0;					
					CONT_EVENTITEM_REWARD_RESULT kItemResultvec;

					PU::TLoadArray_M(m_kAddonPacket,kItemResultvec);					
					m_kAddonPacket.Pop(iRewardItemNo);

					CONT_PLAYER_MODIFY_ORDER kOrder;

					for(CONT_EVENTITEM_REWARD_RESULT::const_iterator result_iter = kItemResultvec.begin();result_iter != kItemResultvec.end();++result_iter)
					{
						kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkCaster->GetID(), SPMOD_Insert_Fixed((*result_iter), SItemPos(), true)));		
					}

					BM::Stream kPacket( PT_M_C_ANS_TREASURE_CHEST );
					kPacket.Push( static_cast<HRESULT>(S_OK) );
					kPacket.Push( iRewardItemNo );

					PgAction_ReqModifyItem kItemModifyAction(CIE_SendTreasureChestReward, GndKey(), kOrder, kPacket);
					kItemModifyAction.DoAction(pkCaster, NULL);
				}
			}
		}break;
	case CIE_SendTreasureChestReward:
		{
			if( (S_OK == m_kRet) || (E_NOT_ENOUGH_INV_SPACE == m_kRet) )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					pkCaster->Send(m_kAddonPacket);
				}
			}
		}break;
/*	case CIE_Manufacture:
		{
			if( ((S_OK == m_kRet) || (E_NOT_ENOUGH_INV_SPACE == m_kRet)) && !m_kAddonPacket.IsEmpty() )
			{
				int iRewardItemNo = 0;
				int iRewardItemCount = 1;
				m_kAddonPacket.Pop(iRewardItemNo);
				m_kAddonPacket.Pop(iRewardItemCount);
				BM::Stream kPacket(PT_M_C_ANS_MANUFACTURE);
				kPacket.Push( iRewardItemNo );
				kPacket.Push( iRewardItemCount );
				pkCaster->Send(kPacket);
			}
		}break;*/
	case CIE_Mission1:
	case CIE_Mission2:
	case CIE_Mission3:
	case CIE_Mission4:
	case CIE_Mission:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					PgBase_Item kItem;
					SItemPos kItemPos;

					kItem.ReadFromPacket( m_kAddonPacket );
					m_kAddonPacket.Pop(kItemPos);

					CONT_PLAYER_MODIFY_ORDER kOrder;
					tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, kItemPos, true);
					SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddItem);
					kOrder.push_back(kIMO);

					BM::Stream kPacket;
					kItem.WriteToPacket( kPacket );

					PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_GadaCoin, GndKey(), kOrder, kPacket);
					kItemModifyAction.DoAction(pkCaster, NULL);
				}
			}
		}break;
	case CIE_Mission_GadaCoin:
		{
			if( (S_OK == m_kRet) || (E_NOT_ENOUGH_INV_SPACE == m_kRet) )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					PgBase_Item kItem;
					kItem.ReadFromPacket( m_kAddonPacket );

					BM::Stream kPacket(PT_M_C_REQ_MISSION_GADACOIN_ITEM);
					kItem.WriteToPacket( kPacket );
					pkCaster->Send(kPacket);
				}
			}
			else
			{
				pkPlayer->SendWarnMessage(400715); //�κ��丮�� ������ �����մϴ�.
			}
		}break;
	case CIE_Mission_InfallibleSelection:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					BYTE kDirection = 0;
					m_kAddonPacket.Pop(kDirection);

					BM::Stream kNfyGndPacket(PT_U_G_NFY_DEFENCE_INFALLIBLE_SELECTION);
					kNfyGndPacket.Push(kDirection);
					pkCaster->VNotify(&kNfyGndPacket);
				}
			}
		}break;
	case CIE_Mission_DefencePotion:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					int iHpPlus = 0;

					m_kAddonPacket.Pop(iHpPlus);

					BM::Stream kNfyGndPacket(PT_U_G_NFY_DEFENCE_POTION);
					kNfyGndPacket.Push(iHpPlus);
					pkCaster->VNotify(&kNfyGndPacket);
				}
			}
		}break;
	case CIE_Mission_DefenceTimePlus:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					int iTimePlus = 0;

					m_kAddonPacket.Pop(iTimePlus);

					BM::Stream kNfyGndPacket(PT_U_G_NFY_DEFENCE_TIMEPLUS);
					kNfyGndPacket.Push(iTimePlus);
					pkCaster->VNotify(&kNfyGndPacket);
				}
			}
		}break;
	case CIE_HiddenReword:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					PgBase_Item kItem;
					SItemPos kItemPos;

					kItem.ReadFromPacket( m_kAddonPacket );
					m_kAddonPacket.Pop(kItemPos);

					CONT_PLAYER_MODIFY_ORDER kOrder;
					tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, kItemPos, true);
					SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddItem);
					kOrder.push_back(kIMO);

					PgAction_ReqModifyItem kItemModifyAction(CIE_HiddenRewordItem, GndKey(), kOrder);
					kItemModifyAction.DoAction(pkCaster, NULL);
				}
			}
		}break;
	case CIE_HiddenRewordItem:
		{
			if( (S_OK == m_kRet) || (E_NOT_ENOUGH_INV_SPACE == m_kRet) )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
				}
			}
			else
			{
				pkPlayer->SendWarnMessage(400715); //�κ��丮�� ������ �����մϴ�.
			}
		}break;
	case IMEPT_EVENT_TW_EFFECTQUEST:
		{
			if( S_OK == m_kRet )
			{
				BM::Stream kPacket(PT_U_G_RUN_ACTION);
				kPacket.Push( static_cast< short >(GAN_AddEffect) );
				kPacket.Push( EventTaiwanEffectQuest::iEventEffectNo );
				pkCaster->VNotify( &kPacket );
			}
			else
			{
				pkCaster->SendWarnMessage(700503);
			}
		}break;
	case IMEPT_QUEST_SHARE:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					int iQuestID = 0;
					m_kAddonPacket.Pop( iQuestID );

					PgActionQuestUtil::AfterBeginCheckQuest(pkCaster, iQuestID, GndKey(), 0);
				}
			}
			else
			{
				BM::Stream kNfyPacket(PT_M_C_ANS_ACCEPT_SHAREQUEST);
				kNfyPacket.Push( static_cast< BYTE >(QSR_Max) );
				pkCaster->Send( kNfyPacket );
			}
		}break;
	case IMEPT_QUEST_BATTLE_PASS:
		{
			if( S_OK == m_kRet )
			{
				if( !m_kAddonPacket.IsEmpty() )
				{
					int iQuestID = 0;
					m_kAddonPacket.Pop( iQuestID );

					PgActionQuestUtil::AfterBeginCheckQuest(pkCaster, iQuestID, GndKey(), 0);
				}
			}
			else
			{
				BM::Stream kNfyPacket(PT_M_C_NFY_BATTLE_PASS_QUEST);
				kNfyPacket.Push( static_cast< BYTE >(QSR_Max) );
				pkCaster->Send( kNfyPacket );
			}
		}break;
	case IMEPT_EVENTQUEST:
		{
			BM::GUID kNpcGuid;

			m_kAddonPacket.Pop( kNpcGuid );

			{
				BM::Stream kPacket(PT_M_C_ANS_EVENT_QUEST_TALK);
				kPacket.Push( static_cast< int >((S_OK != m_kRet)? EQTR_NotEnd: EQTR_End) );
				kPacket.Push( kNpcGuid );
				pkCaster->Send( kPacket );
			}
			if( S_OK == m_kRet )
			{
				BM::Stream kNPacket(PT_M_N_REQ_EVENT_QUEST_ADDUSER);
				kNPacket.Push( pkCaster->GetID() );
				::SendToRealmContents(PMET_EVENTQUEST, kNPacket);
			}
		}break;
	case IMEPT_QUEST_REMOTE_COMPLETE:
		{
			switch( m_kRet )
			{
			case S_OK:
				{
				}break;
			case E_CANT_DELQUEST:
				{
					// ���� �޽��� ����
				}break;
			default:
				{
					int const iFullInven = 401440;
					pkCaster->SendWarnMessage( iFullInven );
				}break;
			}
		}break;
	case MCE_LuckyStar_CostMoney:
		{
			BM::Stream kPacket(PT_M_M_ANS_LUCKYSTAR_COSTMONEY);
			kPacket.Push(pkPlayer->GetMemberGUID());
			kPacket.Push(pkPlayer->GetID());
			kPacket.Push(m_kRet);
			SendToRealmContents(PMET_LuckyStarEvent, kPacket);
		}break;
	case CIE_ACTIVE_STATUS_SET:
		{
			BM::Stream kFPacket(PT_M_C_ANS_STATUS_CHANGE);
			kFPacket.Push(Ret());
			pkCaster->Send(kFPacket);
		}break;
	case CIE_JOBSKILL:
		{
			if( S_OK == Ret() )
			{
			}
			else if(E_NOT_ENOUGH_INV_SPACE == m_kRet)
			{
				pkPlayer->SendWarnMessage(400715); //�κ��丮�� ������ �����մϴ�.
			}
		}break;
	case CIE_JOBSKILL_LEARN:
		{
			BM::Stream kPacket(PT_M_C_ANS_LEARN_JOBSKILL);
			if( S_OK == Ret() )
			{
				kPacket.Push(JSLR_OK);
				RefreshGroundQuestInfo(pkCaster);
			}
			else
			{
				kPacket.Push(JSLR_ERR);
			}
			pkCaster->Send(kPacket);
		}break;
	case CIE_JOBSKILL_DELETE:
		{
			BM::Stream kPacket(PT_M_C_ANS_DELETE_JOBSKILL);
			kPacket.Push(Ret());
			kPacket.Push(m_kAddonPacket);
			pkCaster->Send(kPacket);
		
			RefreshGroundQuestInfo(pkCaster);	//������ų �����ϸ� ����Ʈ �ٽ� üũ����. ��ų ���� ���η� ����Ʈ üũ�ϴ°� �߰��Ǿ���(2011.11.14) By ������
		}break;
	case CIE_SoulTransfer_Extract:
		{
			//if( S_OK == Ret() )
			//{
			//	BM::Stream kPacket(PT_M_C_ANS_SOULTRANSFER_EXTRACT);
			//	kPacket.Push(Ret());
			//	kPacket.Push(ESTM_SUCCESS_EXTRACT);
			//	pkCaster->Send(kPacket);
			//}
		}break;
	case CIE_SoulTransfer_Transition:
		{	
			//if( S_OK == Ret() )
			//{
			//	BM::Stream kPacket(PT_M_C_ANS_SOULTRANSFER_TRANSITION);
			//	kPacket.Push(Ret());
			//	kPacket.Push(ESTM_SUCCESS_TRANSITION);
			//	pkCaster->Send(kPacket);
			//}
		}break;
	case CIE_Defence7_Relay_Stage:
		{
			if( S_OK == Ret() )
			{
				if( m_pkGround->GetAttr() & GATTR_MISSION )
				{
					if( PgMissionGround * pkMissionGround = dynamic_cast<PgMissionGround*>(m_pkGround) )
					{
						int iOption = 0;
						m_kAddonPacket.Pop(iOption);
						pkMissionGround->Defence7RelayStage(pkCaster, iOption>0);
					}
				}
			}
		}break;
	case CIE_Defence7_Point_Copy:
		{
			PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if( pPlayer )
			{
				if( S_OK == Ret() )
				{
					if( m_pkGround->GetAttr() & GATTR_MISSION )
					{
						if( PgMissionGround * pkMissionGround = dynamic_cast<PgMissionGround*>(m_pkGround) )
						{
							pkMissionGround->Defence7PointCopy(pPlayer);
						}
					}
				}
			}
		}
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}

	if(S_OK == m_kRet)
	{//Ư�������� ȹ��� ���ٰ��� üũ.
		GET_DEF(CItemDefMgr, kItemDefMgr);
		DB_ITEM_STATE_CHANGE_ARRAY::const_iterator iter = m_kChangeArray.begin();//���ϸ޽��� üũ
		while(m_kChangeArray.end() != iter)
		{
			DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkElement = (*iter);

			if(rkElement.State() == DISCT_CREATE || rkElement.State() == DISCT_MODIFY)
			{
				PgItemWrapper const& kResultModify = rkElement.ItemWrapper();
				SEnchantInfo kEnchantInfo = kResultModify.EnchantInfo();
				if(kEnchantInfo.IsNotice() == 0)
				{
					CItemDef const *pDefItem = kItemDefMgr.GetDef(kResultModify.ItemNo());
					if(pDefItem && pDefItem->GetAbil(AT_GET_ITEM_MESSAGE))
					{
						kEnchantInfo.IsNotice(1);
				
						CONT_PLAYER_MODIFY_ORDER kOrder;
						kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant(kResultModify,kResultModify.Pos(),kEnchantInfo)));

						PgAction_ReqModifyItem kItemModifyAction(CIE_Notice,m_pkGround->GroundKey(), kOrder);
						kItemModifyAction.DoAction(pkCaster, NULL);

						std::wstring ItemName;
						if( true == ::GetItemName(kResultModify.ItemNo(), ItemName) )
						{
							BM::Stream Packet(PT_M_N_NFY_GENERIC_ACHIEVEMENT_NOTICE);
							Packet.Push(EAchievementType::E_ACHI_GET_ITEM_MESSAGE);
							Packet.Push(pkPlayer->Name());
							Packet.Push(ItemName);
							::SendToContents(Packet);
						}
					}
				}
			}
			++iter;
		}
	}
}

void PgAction_ModifyPlayerData::DoEquipEffect(CUnit* pkCaster, DB_ITEM_STATE_CHANGE_ARRAY::value_type const &rkItemChange)
{
	if(NULL == pkCaster)
	{
		return;
	}
	
	bool const bEquip = (rkItemChange.ItemWrapper().ItemNo() > 0) && (!rkItemChange.ItemWrapper().EnchantInfo().IsNeedRepair());
	bool const bUnequip = (rkItemChange.PrevItemWrapper().ItemNo() > 0) && (!rkItemChange.PrevItemWrapper().EnchantInfo().IsNeedRepair());
	EInvType const eType =  static_cast<EInvType>( rkItemChange.ItemWrapper().Pos().x );
	switch( eType )
	{
	case IT_FIT:
	case IT_FIT_CASH:
		{
			GET_DEF(CItemDefMgr, kItemDefMgr);
			if( bEquip )
			{// ������ ����
				int const iEffectNum = kItemDefMgr.GetAbil(rkItemChange.ItemWrapper().ItemNo(), AT_EFFECTNUM1);
				if(0 < iEffectNum)
				{
					SEffectCreateInfo kCreate;
					kCreate.eType = EFFECT_TYPE_NORMAL;
					kCreate.iEffectNum = iEffectNum;
					kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					pkCaster->AddEffect(kCreate);
				}
				/*int const iCardIndex = rkItemChange.ItemWrapper().EnchantInfo().MonsterCard();
				if( iCardIndex > 0 )
				{
				pkCaster->DoEquipEffectMonsterCard(EQT_ADD_EFFECT, iCardIndex);
				}*/
			}
			if( bUnequip )
			{// ������ ��������
				int const iEffectNum = kItemDefMgr.GetAbil(rkItemChange.PrevItemWrapper().ItemNo(), AT_EFFECTNUM1);
				if (0 < iEffectNum)
				{
					pkCaster->DeleteEffect(iEffectNum);
				}
				/*int const iCardIndex = rkItemChange.PrevItemWrapper().EnchantInfo().MonsterCard();
				if( iCardIndex > 0 )
				{
				pkCaster->DoEquipEffectMonsterCard(EQT_DELETE_EFFECT, iCardIndex);
				}*/
			}
		}break;
	}
	
	switch( eType )
	{
	case IT_FIT:
	case IT_FIT_CASH:
		{
			pkCaster->DoEquipEffectSetItem();
		}break;
	case IT_FIT_COSTUME: // do nothing
	default:
		{
		}break;
	}
	
	if( IT_FIT == eType
		||	IT_FIT_CASH == eType
		||	IT_FIT_COSTUME == eType
	)
	{
		if( bEquip )
		{
			m_kEventItemSetMgr.Equip(dynamic_cast< PgPlayer* >(pkCaster), rkItemChange.ItemWrapper().ItemNo());
		}
		if( bUnequip )
		{
			m_kEventItemSetMgr.Unequip(dynamic_cast< PgPlayer* >(pkCaster), rkItemChange.PrevItemWrapper().ItemNo());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ShareItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ItemShareCheck::PgAction_ItemShareCheck(PgLocalPartyMgr &kLocalPartyMgr)
	:m_kLocalPartyMgr(kLocalPartyMgr)
{
}

bool PgAction_ItemShareCheck::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
		return Pass();
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
		return Pass();
	}

	BM::GUID const & rkPartyGuid = pkPC->PartyGuid();
	if( BM::GUID::NullData() == rkPartyGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("rkPartyGuid is NullData"));
		return Pass();
	}

	BM::GUID const & ExpeditionGuid = pkPC->ExpeditionGuid();
	if( BM::GUID::IsNotNull(ExpeditionGuid) )
	{	// �����뿡 ���� ������.
		SExpeditionOption ExpeditionOption;
		bool const GetExpeditionOption = m_kLocalPartyMgr.GetExpeditionOption(ExpeditionGuid, ExpeditionOption);
		if( !GetExpeditionOption )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetExpeditionOption is NULL"));
			return Pass();
		}

		ExpeditionOptionItem(ExpeditionOption.GetOptionItem());
		if( EOI_LikeSolo == ExpeditionOptionItem() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ExpeditionOptionItem is POI_LikeSolo"));
			return Pass();
		}
	}
	else // �Ϲ� ��Ƽ��.
	{
		SPartyOption kPartyOption;
		//bool const bGetPartyOption = g_kLocalPartyMgr.GetPartyOption(rkPartyGuid, kPartyOption);
		bool const bGetPartyOption = m_kLocalPartyMgr.GetPartyOption(rkPartyGuid, kPartyOption);
		if( !bGetPartyOption )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bGetPartyOption is NULL"));
			return Pass();
		}

		OptionItem(kPartyOption.GetOptionItem());
		if( POI_LikeSolo == OptionItem() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("OptionItem is POI_LikeSolo"));
			return Pass();
		}
	}
	return true;//Share�Ѵ�
}

bool PgAction_ItemShareCheck::Pass()
{
	return false;//Share���Ѵ�.
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ShareItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ShareItem::PgAction_ShareItem(const EPartyOptionItem ePartyOptionItem, SGroundKey const &kGroundKey, PgLocalPartyMgr &kLocalPartyMgr)
	:m_eOptionItem(ePartyOptionItem), m_kGndKey(kGroundKey), m_kLocalPartyMgr(kLocalPartyMgr)
{

}

bool PgAction_ShareItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//�ƹ��͵� ���Ѵ�.
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID const & rkPartyGuid = pkPC->PartyGuid();
	if( BM::GUID::NullData() == rkPartyGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("rkPartyGuid is NullData"));
		return false;
	}

	switch(m_eOptionItem)
	{
	case POI_Order:
		{
			return m_kLocalPartyMgr.GetPartyShareItem_NextOwner(rkPartyGuid, m_kGndKey, m_kNextItemOwner);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ExpeditionShareItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ExpeditionShareItem::PgAction_ExpeditionShareItem(EExpeditionOptionItem const ExpeditionOptionItem, SGroundKey const & GroundKey, PgLocalPartyMgr & LocalPartyMgr)
	:m_OptionItem(ExpeditionOptionItem), m_GndKey(GroundKey), m_LocalPartyMgr(LocalPartyMgr)
{

}

bool PgAction_ExpeditionShareItem::DoAction(CUnit* pCaster, CUnit* pTarget)
{
	if( !pCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//�ƹ��͵� ���Ѵ�.
	}

	PgPlayer* pPC = dynamic_cast<PgPlayer*>(pCaster);
	if( !pPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID const & ExpeditionGuid = pPC->ExpeditionGuid();
	if( BM::GUID::NullData() == ExpeditionGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("rkPartyGuid is NullData"));
		return false;
	}

	switch(m_OptionItem)
	{
	case EOI_Order:
		{
			return m_LocalPartyMgr.GetExpeditionShareItem_NextOwner(ExpeditionGuid, m_GndKey, m_kNextItemOwner);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////////
//		PgAction_CreateItem
//////////////////////////////////////////////////////////////////////////////
PgAction_CreateItem::PgAction_CreateItem(const EItemModifyParentEventType eCause, SGroundKey const &kGroundKey, CONT_ITEM_CREATE_ORDER const &kContItemCreateOrder, BM::Stream const &kPacket)
	:	m_eCause(eCause), m_kGndKey(kGroundKey), m_kContItemCreateOrder(kContItemCreateOrder), m_kAddonPacket(kPacket)
{
}

bool PgAction_CreateItem::DoCreateItem(CUnit* pkCaster,CONT_PLAYER_MODIFY_ORDER & kOrder) const
{
	if ( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SItemPos kItemPos;//Temp Value
	CONT_ITEM_CREATE_ORDER::const_iterator craete_order_itor = m_kContItemCreateOrder.begin();
	while(craete_order_itor != m_kContItemCreateOrder.end())
	{
		tagPlayerModifyOrderData_Insert_Fixed kAddData((*craete_order_itor), kItemPos, true);//1�� ����.
		SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddData);

		kOrder.push_back(kIMO);

		++craete_order_itor;
	}
	return true;
}

bool PgAction_CreateItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if ( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	if(false == DoCreateItem(pkCaster,kOrder))
	{
		return false;
	}

	PgAction_ReqModifyItem kItemModifyAction(m_eCause, m_kGndKey, kOrder, m_kAddonPacket);
	kItemModifyAction.DoAction(pkCaster, pkTarget);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_PickUpItem
//////////////////////////////////////////////////////////////////////////////
DWORD PgAction_PickUpItem::m_dwCanAnyPickUpItemTime = 0;
PgAction_PickUpItem::PgAction_PickUpItem( PgGround * const pkGnd, PgLocalPartyMgr &rkLocalPartyMgr, CUnit* pkLooter)
:	m_pkGround(pkGnd)
,	m_rkLocalPartyMgr(rkLocalPartyMgr)
,	m_pkLooter(pkLooter)
{
}

bool PgAction_PickUpItem::InitStaticValue()
{
	int iTimeVal = 10000; //�⺻�� 10000 (10��)
 	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_DropBox_CanAnyPickUpItem, iTimeVal) )
 	{
 		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Init AnyPickUpItemTime [EVar_DropBox_CanAnyPickUpItem]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
 		return false;
 	}
	m_dwCanAnyPickUpItemTime = iTimeVal;
	return true;
}

bool PgAction_PickUpItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !m_pkGround )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Ground is NULL") );
		LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ << _T("Return false") );
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( true == pkPC->IsDead() )
	{
		pkPC->SendWarnMessage(20102);//���� �� �ִ� ���°� �ƴմϴ�
		return false;
	}

	PgGroundItemBox* pkGndItemBox = dynamic_cast<PgGroundItemBox*>(pkTarget);
	if( !pkGndItemBox )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// ������ ������ �� �ִ� �ð� Ȯ��
	if( m_dwCanAnyPickUpItemTime > ::DifftimeGetTime( pkGndItemBox->CreateDate(), BM::GetTime32() ) )
	{
		bool bMine = pkGndItemBox->IsOwner( pkPC->GetID() );
		if( !bMine ) // ���� �ƴ϶��.
		{
			pkPC->SendWarnMessage(20101);//�������� �����ϴ�.
			return false;
		}
	}

	float const fDistance = POINT3::Distance(pkGndItemBox->GetPos(),pkPC->GetPos());
	if( fDistance > Item_PickUp_Distance )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; // �Ÿ��� �ִ�
	}

	if( false == pkPC->IsTakeUpItem() )
	{
		if( (0 == pkPC->GetMoneyRate()) || (pkGndItemBox->Money() <= 0) )
		{
			pkPC->SendWarnMessage(20102);//���� �� �ִ� ���°� �ƴմϴ�
			return false;
		}
	}

	int const iNotHaveEmptyIven = 400715;
	if( 0 == pkGndItemBox->ElementSize()
	&&	0 == pkGndItemBox->Money() )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Ground[") << m_pkGround->GetGroundNo() << _T("] Null Item/Money GroundItemBox[") << pkTarget->GetID() << _T("]") );
	}

	if( pkGndItemBox->IsInstanceItem() )
	{	// �ν��Ͻ� �������̶�� ���⼭ �ٷ� ����Ʈ �ɰ� ������ ������.
		bool bRemoveItem = true;
		PgBase_Item InstanceItem;
		pkGndItemBox->PopItem(InstanceItem, true);
		if( (pkCaster->GetAbil(AT_TEAM) == pkGndItemBox->Team()) || (TEAM_NONE == pkGndItemBox->Team()) )
		{	// ���� �� ������ �̰ų�, �߸� �������� ��츸 ����� �� ����.
			GET_DEF(CItemDefMgr, ItemDefMgr);
			
			if( ItemDefMgr.GetAbil(InstanceItem.ItemNo(), AT_BEAR_ITEM) )
			{	// ���귯�� ��� ���������� ȹ���ϴ� �����.
				// �ɷ��ִ� ��� ����Ʈ�� �����ش�.
				pkCaster->ClearAllEffect(true);
			}

			// ����Ʈ�� �ɾ��ش�.
			for( int i = 0 ; i < 10 ; ++i )
			{
				int iEffectNum = ItemDefMgr.GetAbil(InstanceItem.ItemNo(), AT_EFFECTNUM1 + i);
				if (iEffectNum <= 0)
				{
					break;
				}
				SEffectCreateInfo Create;
				Create.eType = EFFECT_TYPE_NORMAL;
				Create.iEffectNum = iEffectNum;
				Create.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				pkCaster->AddEffect(Create);
			}

			m_pkGround->UpdateDeleInsItemOwner(pkCaster->GetID(), pkGndItemBox->Caller());
			
			PgWarGround * pWarGnd = NULL;
			pWarGnd = dynamic_cast<PgWarGround*>(m_pkGround);
			if( pWarGnd )
			{
				pWarGnd->RemoveInsItemDropInfo(pkGndItemBox->Caller());
			}
		}
		else
		{	// �� ������ ����. ���� ����.
			PgWarGround * pWarGnd = NULL;
			pWarGnd = dynamic_cast<PgWarGround*>(m_pkGround);
			if( pWarGnd )
			{
				if( pWarGnd->IsModeType(PVP_TYPE_KTH) )
				{
					pWarGnd->SendNfyMessage(pkCaster, 74104);
					pWarGnd->RemoveEnemeyInstanceItem(pkCaster);
				}
				else
				{
					bRemoveItem = false;
				}
			}
		}		

		if( bRemoveItem )
		{	// �ڽ��� ������ٰ� �˸�
			pkGndItemBox->PopItem(InstanceItem, false);

			if( pkGndItemBox->GenInfo().dwPeriod )
			{	// ���� ������ 0 �̻��� ����, 0�� ���� ���ϴ� ������ ������ ��Ű�� ���� �� �����ϱ� ������.
				m_pkGround->RemoveInstanceItem( pkGndItemBox->GenInfo(), pkGndItemBox->GetID() );
			}
			else
			{
				m_pkGround->ReserveDeleteInsItem(pkCaster->GetID(), pkGndItemBox->GenInfo(), pkGndItemBox->GetID());
			}

			BM::Stream kPacket( PT_M_C_ANS_PICKUPGBOX, m_pkLooter ? m_pkLooter->GetID() : pkCaster->GetID() );
			kPacket.Push( pkTarget->GetID() );
			pkCaster->Send( kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND );

			PgWarGround * pWarGnd = NULL;
			pWarGnd = dynamic_cast<PgWarGround*>(m_pkGround);
			if( pWarGnd )
			{
				if( pWarGnd->IsModeType(PVP_TYPE_LOVE) )
				{
					pWarGnd->PickupBear(pkCaster);
				}
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	// �ϴ��� ������ �ڽ��� �������� �ϳ� ���̴� �����ϰ��� ���� (��ȹ ����� ���� �Ǿ�� �ϴ� �κ�)
	if( 0 != pkGndItemBox->ElementSize() )
	{
		PgBase_Item kItem;
		if( S_OK == pkGndItemBox->PopItem(kItem, true) )
		{
			CUnit *pkItemOwner = pkCaster; // ���� Ȯ��

			{ // ��Ƽ�� �߿��� ��Ƽ �ɼǿ� ����, ������ ���� �ִ�.
				PgAction_ItemShareCheck kItemShareCheck( m_rkLocalPartyMgr );
				if( kItemShareCheck.DoAction( pkCaster, NULL ) )
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if( pPlayer )
					{
						if( pPlayer->HaveExpedition() )			// ������ ������ ������ ���� ������ ȹ��.
						{
							PgAction_ExpeditionShareItem ShareItem( kItemShareCheck.ExpeditionOptionItem(), m_pkGround->GroundKey(), m_rkLocalPartyMgr );
							if( ShareItem.DoAction( pkCaster, NULL ) )
							{
								pkItemOwner = m_pkGround->GetUnit( ShareItem.NextItemOwner() );
								if( NULL == pkItemOwner 
								||	pkItemOwner->IsDead() )
								{
									pkItemOwner = pkCaster; // �׾��ų� ������ Caster�� �ٽ� ���ʷ� ����
								}
							}
						}
						else		// �ƴϸ� �Ϲ� ��Ƽ ����.
						{
							PgAction_ShareItem kShareItem( kItemShareCheck.OptionItem(), m_pkGround->GroundKey(), m_rkLocalPartyMgr );
							if( kShareItem.DoAction( pkCaster, NULL ) )
							{
								pkItemOwner = m_pkGround->GetUnit( kShareItem.NextItemOwner() );
								if( NULL == pkItemOwner
								||	pkItemOwner->IsDead() )
								{
									pkItemOwner = pkCaster; // �׾��ų� ������ Caster�� �ٽ� ���ʷ� ����
								}
							}
						}
					}
				}
			}

			if( m_pkGround->IsBSItem(kItem.ItemNo()) )
			{ // FreePVP �߿��� �ӽ� �κ��丮��
				BM::Stream kPacket(PT_U_G_RUN_ACTION);
				kPacket.Push( static_cast< short >(GAN_PushVolatileInven_Item) );
				kItem.WriteToPacket( kPacket );
				pkItemOwner->VNotify( &kPacket );
			}
			else if( m_pkGround->IsDefenceItemList(kItem.ItemNo()) )
			{
				BM::Stream kPacket(PT_U_G_RUN_ACTION);
				kPacket.Push( static_cast< short >(GAN_PushMissionDefenceInven_Item) );
				kItem.WriteToPacket( kPacket );
				pkItemOwner->VNotify( &kPacket );
			}
			else
			{ // ���� ���´� ������ �κ��丮��
				PgInventory* pkInven = pkPC->GetInven();
				if( pkInven
				&&	S_OK != pkInven->CanInsert(kItem) )
				{
					pkPC->SendWarnMessage(iNotHaveEmptyIven);//�κ��丮�� ���� á���ϴ�.
					return false;
				}
				pkInven = pkItemOwner->GetInven();
				if( pkInven
				&&	S_OK != pkInven->CanInsert(kItem) )
				{
					pkItemOwner->SendWarnMessage(iNotHaveEmptyIven);//�κ��丮�� ���� á���ϴ�.
					// �̳��� ���� �� ������ �ֿ���� �� ���� �Դ´�.
					pkItemOwner = pkPC;
				}
				if( false == kItem.IsEmpty() )
				{
					CONT_ITEM_CREATE_ORDER	kContItemCreateOrder;
					kContItemCreateOrder.push_back( kItem );

					PgAction_CreateItem kCreateAction( CIE_Loot, m_pkGround->GroundKey(), kContItemCreateOrder );
					kCreateAction.DoAction( pkItemOwner, NULL );
				}
			}
			pkGndItemBox->PopItem( kItem, false ); // �������� �����°� ���߿� �Ѵ�

			ALRAM_MISSION::SCheckObejct< __int64 > kCheckObject( ALRAM_MISSION::OBJECT_ITEMNO, static_cast<__int64>(kItem.ItemNo()) );
			ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_PICKUP_ITEM>()( pkPC, static_cast<int>(kItem.Count()), m_pkGround, kCheckObject );
		}
	}

	// ���
	__int64 i64AddMoney = pkGndItemBox->Money();
	if( 0 != i64AddMoney )
	{
		PgAction_ShareGold kMoneyAction( m_pkGround, i64AddMoney, MCE_Loot, m_rkLocalPartyMgr );
		kMoneyAction.DoAction( pkCaster, NULL );
	}

	// �ڽ��� ������ٰ� �˸�
	BM::Stream kPacket( PT_M_C_ANS_PICKUPGBOX, m_pkLooter ? m_pkLooter->GetID() : pkCaster->GetID() );
	kPacket.Push( pkTarget->GetID() );
	pkCaster->Send( kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND );

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_PopPetItem
//////////////////////////////////////////////////////////////////////////////
PgAction_PopPetItem::PgAction_PopPetItem( BM::GUID const &kPetID, SItemPos const &kPetPos, SItemPos const &kPlayerPos, PgGround const *pkGnd )
:	m_kPetID(kPetID), m_kPetPos(kPetPos), m_kPlayerPos(kPlayerPos), m_pkGround(pkGnd)
{
}

bool PgAction_PopPetItem::DoAction( CUnit* pkCaster, CUnit *pkNothing )
{
	if ( pkCaster )
	{
		PgBase_Item kPlayerItem;
		pkCaster->GetInven()->GetItem(m_kPlayerPos, kPlayerItem);

		if ( true == kPlayerItem.IsEmpty() )
		{
			// ��� �ִ� �������̴�~
			SPMOD_Modify_Pos_Pet kCasterData( m_kPlayerPos, m_kPetPos, kPlayerItem, kPlayerItem, m_kPetID );
			kCasterData.CasterIsPlayer( true );

			CONT_PLAYER_MODIFY_ORDER kOrder;
			SPMO kIMO(IMET_PET_ITEM, pkCaster->GetID(), kCasterData );
			kOrder.push_back( kIMO );

			PgAction_ReqModifyItem kItemModifyAction(CIE_ItemPetOnlyPop, m_pkGround->GroundKey(), kOrder);
			return kItemModifyAction.DoAction( pkCaster, NULL );
		}
		else
		{
			// �� �Լ��� Pet���׼� �̱⸸ �ϴ� ���̹Ƿ�, Player�� ���ڸ��̾�߸� �Ѵ�.
			pkCaster->SendWarnMessage( 355 );
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_MoveItem_Guild
//////////////////////////////////////////////////////////////////////////////
PgAction_MoveItem_Guild::PgAction_MoveItem_Guild(SItemPos const &kCasterPos, SItemPos const &kTargetPos, PgBase_Item const &rkCasterItem, PgBase_Item const &rkTargetItem,
												 PgGround const *pkGnd, DWORD const dwCurrentTime, const int iType, const bool bAddonPacket, const SGuild_Inventory_Log& rkLog)
	:m_kCasterPos(kCasterPos), m_kTargetPos(kTargetPos), m_kCasterItem(rkCasterItem), m_kTargetItem(rkTargetItem),
	m_pkGround(pkGnd), m_dwCurrentTime(dwCurrentTime), m_iType(iType), m_bAddonPacket(bAddonPacket), m_kLog(rkLog)
{
}

bool PgAction_MoveItem_Guild::DoAction( CUnit* pkCaster, CUnit* pkTarget )
{
	if(!pkCaster)
	{// ���⼭ ���г��� ���ݰ� ��ġ ��ٰ� Ǯ �� �� ���µ�. ���� ����.
		return false;
	}

	BM::Stream kAddonPacket;
	if( m_bAddonPacket )
	{
		m_kLog.WriteToPacket(kAddonPacket);
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	BM::GUID kOwnerGuid;
	
	if( EGIT_ITEM_MOVE == m_iType )
	{// ���ݰ��� ������ �̵�(���ݰ� ==> ���ݰ�)
		kOwnerGuid = pkCaster->GetGuildGuid();
		if( kOwnerGuid == BM::GUID::NullData() 
		|| m_kCasterItem.IsEmpty() )
		{
			goto __LABEL_ERROR;
		}
		
		SPMOD_Modify_Pos kCasterData( m_kCasterPos, m_kTargetPos, m_kCasterItem, m_kTargetItem );
		SPMO kIMO( IMET_MODIFY_POS | IMC_GUILD_INV, SModifyOrderOwner( kOwnerGuid, OOT_Guild), kCasterData );	
		kOrder.push_back(kIMO);
		PgAction_ReqModifyItem kItemModifyAction( CIE_MoveFromGuildInvToGuildInv, m_pkGround->GroundKey(), kOrder );
		return kItemModifyAction.DoAction( kOwnerGuid );
	}
	else if( EGIT_ITEM_OUT == m_iType )
	{// ���ݰ� ������ ������(���ݰ� ==> �κ��丮)
		kOwnerGuid = pkCaster->GetGuildGuid();
		if( kOwnerGuid == BM::GUID::NullData()
		|| m_kCasterItem.IsEmpty() )
		{
			goto __LABEL_ERROR;
		}

		TradeListToOrderCont( pkCaster->GetGuildGuid(), pkCaster->GetID(), m_kCasterItem, m_kCasterPos, m_kTargetItem, m_kTargetPos, kOrder, OOT_Guild, OOT_Player );
		PgAction_ReqModifyItem kItemModifyAction( CIE_MoveFromGuildInvToInv, m_pkGround->GroundKey(), kOrder, kAddonPacket );
		return kItemModifyAction.DoAction( kOwnerGuid );
	}
	else if( EGIT_ITEM_IN == m_iType )
	{// ���ݰ� ������ ����(�κ��丮 ==> ���ݰ�)
		kOwnerGuid = pkCaster->GetID();
		if( kOwnerGuid == BM::GUID::NullData() )
		{
			goto __LABEL_ERROR;
		}
		
		{// ���ݰ��� ���� �� �ִ� �������ΰ�?			
			if( !IsMoveGuildInv( pkCaster, m_kCasterPos ) )			
			{
				goto __LABEL_ERROR;
			}
		}

		TradeListToOrderCont( pkCaster->GetID(), pkCaster->GetGuildGuid(), m_kCasterItem, m_kCasterPos, m_kTargetItem, m_kTargetPos, kOrder, OOT_Player, OOT_Guild );
		PgAction_ReqModifyItem kItemModifyAction( CIE_MoveFromInvToGuildInv, m_pkGround->GroundKey(), kOrder, kAddonPacket );
		return kItemModifyAction.DoAction( kOwnerGuid );
	}
	else
	{	
		goto __LABEL_ERROR;
	}

__LABEL_ERROR:
	{
		{// �������� ���� �˸�
			BM::Stream kNPacket(PT_M_N_NFY_GUILD_INVENTORY_FAIL);
			kNPacket.Push(pkCaster->GetGuildGuid());
			kNPacket.Push(m_iType);
			kNPacket.Push(m_kCasterPos);
			kNPacket.Push(m_kTargetPos);
			::SendToGuildMgr(kNPacket);
		}

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

bool PgAction_MoveItem_Guild::IsMoveGuildInv(CUnit* pkCaster, SItemPos const &kCasterPos)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgBase_Item kCasterItem;
	int iErrorRet = 0;
	if(SUCCEEDED(pkCaster->GetInven()->GetItem(kCasterPos, kCasterItem)))
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kCasterItem.ItemNo());
	}

	// �ŷ� �Ұ� ������ �������� ���ݰ��� ���� �� ����.
	if( false == CheckEnableTrade(kCasterItem,ICMET_Cant_PlayerTrade) )
	{
		pkCaster->SendWarnMessage(20025);
		
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

void PgAction_MoveItem_Guild::TradeListToOrderCont( const BM::GUID& rkCasterGuid, 
													const BM::GUID& rkTargetGuid, 
													const PgBase_Item& rkCasterItem,
													const SItemPos& rkCasterItemPos,
													const PgBase_Item& rkTargetItem,
													const SItemPos& rkTargetItemPos,
													CONT_PLAYER_MODIFY_ORDER& rkOrder, 
													const EOrderOwnerType eCasterType,
													const EOrderOwnerType eTargetType )
{
	{// Caster ����
		if( !rkCasterItem.IsEmpty() )
		{
			SPMOD_Modify_Count kDelData(rkCasterItem, rkCasterItemPos, 0, true);

			if( eCasterType == OOT_Guild )
			{
				SPMO kIMO( IMET_MODIFY_COUNT | IMC_POS_BY_EXCHANGE | IMC_GUILD_INV, SModifyOrderOwner(rkCasterGuid, eCasterType), kDelData );
				rkOrder.push_back(kIMO);
			}
			else
			{
				SPMO kIMO( IMET_MODIFY_COUNT | IMC_POS_BY_EXCHANGE, SModifyOrderOwner(rkCasterGuid, eCasterType), kDelData );
				rkOrder.push_back(kIMO);
			}
		}
	}
	
	{// Target ����
		if( !rkTargetItem.IsEmpty() )
		{
			SPMOD_Modify_Count kDelData(rkTargetItem, rkTargetItemPos, 0, true);

			if( eTargetType == OOT_Guild )
			{
				SPMO kIMO( IMET_MODIFY_COUNT | IMC_POS_BY_EXCHANGE | IMC_GUILD_INV, SModifyOrderOwner(rkTargetGuid, eTargetType), kDelData );
				rkOrder.push_back(kIMO);
			}
			else
			{
				SPMO kIMO( IMET_MODIFY_COUNT | IMC_POS_BY_EXCHANGE, SModifyOrderOwner(rkTargetGuid, eTargetType), kDelData );
				rkOrder.push_back(kIMO);
			}
		}
	}
	
	{// Caster �������� Target���� ���ϱ�
		if( !rkCasterItem.IsEmpty() )
		{
			SItemPos kItemPos;
			SPMOD_Insert_Fixed kAddData(rkCasterItem, rkTargetItemPos, false);

			if( eTargetType == OOT_Guild )
			{
				SPMO kIMO( IMET_INSERT_FIXED | IMC_POS_BY_EXCHANGE | IMC_GUILD_INV, SModifyOrderOwner(rkTargetGuid, eTargetType), kAddData );
				rkOrder.push_back(kIMO);
			}
			else
			{
				SPMO kIMO( IMET_INSERT_FIXED | IMC_POS_BY_EXCHANGE, SModifyOrderOwner(rkTargetGuid, eTargetType), kAddData );
				rkOrder.push_back(kIMO);
			}
		}
	}
	
	{// Target �������� Caster���� ���ϱ�
		if( !rkTargetItem.IsEmpty() )
		{
			SItemPos kItemPos;
			SPMOD_Insert_Fixed kAddData(rkTargetItem, rkCasterItemPos, false);

			if( eCasterType == OOT_Guild )
			{
				SPMO kIMO( IMET_INSERT_FIXED | IMC_POS_BY_EXCHANGE | IMC_GUILD_INV, SModifyOrderOwner(rkCasterGuid, eCasterType), kAddData );
				rkOrder.push_back(kIMO);
			}
			else
			{
				SPMO kIMO( IMET_INSERT_FIXED | IMC_POS_BY_EXCHANGE, SModifyOrderOwner(rkCasterGuid, eCasterType), kAddData );
				rkOrder.push_back(kIMO);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MoveMoney_Guild
//////////////////////////////////////////////////////////////////////////////
PgAction_MoveMoney_Guild::PgAction_MoveMoney_Guild(PgGround const *pkGnd, const BM::GUID& kGuildGuid, const int iType, const __int64 i64Money, const bool bAddonPacket, const SGuild_Inventory_Log& rkLog)
	:m_pkGround(pkGnd), m_kGuildGuid(kGuildGuid), m_iType(iType), m_i64Money(i64Money), m_bAddonPacket(bAddonPacket), m_kLog(rkLog)
{
}

bool PgAction_MoveMoney_Guild::DoAction(CUnit *pkCaster, CUnit *pkTarget)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kAddonPacket;
	if( m_bAddonPacket )
	{
		m_kLog.WriteToPacket(kAddonPacket);
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	
	if( EGIT_MONEY_IN == m_iType )
	{// ���ݰ��� ��� �ֱ�(�κ��丮 ==> ���ݰ�)
		
		{// ĳ���� �κ����� ��� ����
			SPMOD_Add_Money kDelMoneyData(-m_i64Money);
			SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
			kOrder.push_back(kIMO);
		}

		{// ��� �κ��� ��� ���ϱ�
			SPMOD_Add_Money kDelMoneyData(m_i64Money);
			SPMO kIMO(IMET_ADD_MONEY | IMC_GUILD_INV, SModifyOrderOwner( m_kGuildGuid, OOT_Guild), kDelMoneyData);
			kOrder.push_back(kIMO);
		}

		PgAction_ReqModifyItem kItemModifyAction(CIE_MoveFromInvToGuildInv, m_pkGround->GroundKey(), kOrder, kAddonPacket );
		return kItemModifyAction.DoAction( m_kGuildGuid );
	}
	else if( EGIT_MONEY_OUT == m_iType )
	{// ���ݰ����� ��� ������(���ݰ� ==> �κ��丮)

		{// ��� �κ����� ��� ����
			SPMOD_Add_Money kDelMoneyData(-m_i64Money);
			SPMO kIMO(IMET_ADD_MONEY | IMC_GUILD_INV, SModifyOrderOwner( m_kGuildGuid, OOT_Guild), kDelMoneyData);
			kOrder.push_back(kIMO);
		}

		{// ĳ���� �κ��� ��� ���ϱ�
			SPMOD_Add_Money kDelMoneyData(m_i64Money);
			SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
			kOrder.push_back(kIMO);
		}

		PgAction_ReqModifyItem kItemModifyAction(CIE_MoveFromGuildInvToInv, m_pkGround->GroundKey(), kOrder, kAddonPacket);
		return kItemModifyAction.DoAction( pkCaster->GetID() );
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MoveItem
//////////////////////////////////////////////////////////////////////////////
PgAction_MoveItem::PgAction_MoveItem(SItemPos const &kCasterPos, SItemPos const &kTargetPos, PgGround const *pkGnd, DWORD const dwCurrentTime, bool const bMovePet )
	:m_kCasterPos(kCasterPos), m_kTargetPos(kTargetPos), m_pkGround(pkGnd), m_dwCurrentTime(dwCurrentTime), m_bMovePet(bMovePet)
{
}

bool PgAction_MoveItem::DoAction( CUnit* pkCaster, CUnit* pkPet )
{
	if(!pkCaster)
	{
		return false;
	}

	bool const bIsPet = ( NULL != pkPet );

	PgInventory* pkCasterInven = pkCaster->GetInven();
	PgInventory* pkTargetInven = ( bIsPet ? pkPet->GetInven(): pkCasterInven );

	PgBase_Item kCasterItem;
	pkCasterInven->GetItem(m_kCasterPos, kCasterItem);

	PgBase_Item kTargetItem;
	pkTargetInven->GetItem(m_kTargetPos, kTargetItem);

	if(		kCasterItem.IsEmpty()
		&&	kTargetItem.IsEmpty()
	)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	CONT_PLAYER_MODIFY_ORDER kOrder;
	EItemModifyParentEventType kEventType = CIE_Mouse_Event;

	if ( true == bIsPet )
	{
		int iError = IsCanEquipPet( pkCaster, m_kCasterPos, kCasterItem, pkPet, m_kTargetPos, kOrder );
		if ( iError )
		{
			if ( 0 < iError )
			{
				pkCaster->SendWarnMessage( iError );
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		iError = IsCanEquipPet( pkPet, m_kTargetPos, kTargetItem, pkCaster, m_kCasterPos, kOrder );
		if ( iError )
		{
			if ( 0 < iError )
			{
				pkCaster->SendWarnMessage( iError );
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		// ItemGuid �� Pos�� �Բ� ������ ���� ?
		// ContentsServer ���� Pos�� GUID�� �Բ� Ȯ���ϵ��� �ϱ� ����.
		// ContentsServer �� ���� ���, Pos�� �����ϸ�, ����

		SPMOD_Modify_Pos_Pet kCasterData( m_kCasterPos, m_kTargetPos, kCasterItem, kTargetItem, pkPet->GetID() );
		kCasterData.CasterIsPlayer( m_bMovePet );

		SPMO kIMO(IMET_PET_ITEM, pkPet->Caller(), kCasterData);
		kOrder.push_back( kIMO );

		AddCoolTime( pkTargetInven, pkCaster);

		kEventType = CIE_ItemPet;
	}
	else
	{
		if(		!IsCanEquip(pkCaster, m_kCasterPos, m_kTargetPos, kOrder)
			||	!IsCanEquip(pkCaster, m_kTargetPos, m_kCasterPos, kOrder))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		// ItemGuid �� Pos�� �Բ� ������ ���� ?
		// ContentsServer ���� Pos�� GUID�� �Բ� Ȯ���ϵ��� �ϱ� ����.
		// ContentsServer �� ���� ���, Pos�� �����ϸ�, ����
		SPMOD_Modify_Pos kCasterData( m_kTargetPos, m_kCasterPos, kTargetItem, kCasterItem );

		SPMO kIMO_Cst(IMET_MODIFY_POS, pkCaster->GetID(), kCasterData);
		//	SPMO kIMO_Tgt(IMET_MODIFY_POS, pkCaster->GetID(), kTargetData);

		kOrder.push_back(kIMO_Cst);
		//	kOrder.push_back(kIMO_Tgt);

		AddCoolTime( pkCasterInven, pkCaster );

		switch(m_kTargetPos.x)
		{
		case IT_FIT:
			{
				kEventType = CIE_Equip;
				if ( EQUIP_POS_PET == m_kTargetPos.y )
				{
					// Pet Item�� �����ϳ�...��¿�� ���� ����ó��..(�������� MP�� Save���־�� �Ѵ�.)
					MakeUnEquipPetOrder( m_kCasterPos, kTargetItem, kOrder );// Order�� �������� �̵��� ������ ó���� �Ǵϱ� Pos�� �̵����� ��ġ��
				}
			}break;
		case IT_FIT_COSTUME:
		case IT_FIT_CASH:
			{
				kEventType = CIE_Equip;
			}break;
		case IT_SAFE:
		case IT_SAFE_ADDITION:
		case IT_CASH_SAFE:
		case IT_RENTAL_SAFE1:
		case IT_RENTAL_SAFE2:
		case IT_RENTAL_SAFE3:
		case IT_SHARE_RENTAL_SAFE1:
		case IT_SHARE_RENTAL_SAFE2:
		case IT_SHARE_RENTAL_SAFE3:
		case IT_SHARE_RENTAL_SAFE4:
			{
				kEventType = CIE_MoveToSafe;

				bool const bPremiumService = IsShareRentalPremium(m_kTargetPos.x, dynamic_cast<PgPlayer*>(pkCaster));;
				if(false==bPremiumService && E_ENABLE_TIMEOUT == pkCaster->GetInven()->CheckEnableUseRentalSafe(static_cast<EInvType>(m_kTargetPos.x)))
				{
					return false;
				}

				if(IT_SAFE_ADDITION==m_kTargetPos.x && KUIG_FIT==m_kCasterPos.x && EQUIP_POS_PET==m_kCasterPos.y)
				{//�����ϰ� �ִ� �߰��κ��� �ڱ� �ڽſ��� ���� �� ����
					return false;
				}
				if( (IT_SAFE_ADDITION == m_kTargetPos.x) && (KUIG_FIT_CASH == m_kCasterPos.x) 
					&& ((EQUIP_POS_RING_L == m_kCasterPos.y) || (EQUIP_POS_RING_R == m_kCasterPos.y)) )
				{// �߰� �κ��� �ִ� ���� ĳ�þ������� ������ ����.
					return false;
				}
			}break;
		default:
			{
				switch(m_kCasterPos.x)
				{
				case IT_FIT:
					{
						kEventType = CIE_UnEquip;

						if ( EQUIP_POS_PET == m_kCasterPos.y )
						{
							// Pet Item�� �����ϳ�...��¿�� ���� ����ó��..(�������� MP�� Save���־�� �Ѵ�.)
							MakeUnEquipPetOrder( m_kTargetPos, kCasterItem, kOrder );// Order�� �������� �̵��� ������ ó���� �Ǵϱ� Pos�� �̵����� ��ġ��
						}

					}break;
				case IT_FIT_CASH:
				case IT_FIT_COSTUME:
					{
						kEventType = CIE_UnEquip;
					}break;
				case IT_SAFE:
				case IT_SAFE_ADDITION:
				case IT_CASH_SAFE:
				case IT_RENTAL_SAFE1:
				case IT_RENTAL_SAFE2:
				case IT_RENTAL_SAFE3:
				case IT_SHARE_RENTAL_SAFE1:
				case IT_SHARE_RENTAL_SAFE2:
				case IT_SHARE_RENTAL_SAFE3:
				case IT_SHARE_RENTAL_SAFE4:
					{
						kEventType = CIE_MoveToInv;

						switch(g_kLocal.ServiceRegion())
						{
						case LOCAL_MGR::NC_CHINA:
							{
								// �߱��� �����°� �Ⱓ�� ����Ǿ ��� ���� ������ �־�� �Ѵ�.
							}break;
						default:
							{
								bool const bPremiumService = IsShareRentalPremium(m_kCasterPos.x, dynamic_cast<PgPlayer*>(pkCaster));
								if(false==bPremiumService && E_ENABLE_TIMEOUT == pkCaster->GetInven()->CheckEnableUseRentalSafe(static_cast<EInvType>(m_kTargetPos.x)))
								{
									return false;
								}
							}break;
						}

					}break;
				}
			}break;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return kItemModifyAction.DoAction"));
	PgAction_ReqModifyItem kItemModifyAction(kEventType, m_pkGround->GroundKey(), kOrder);
	return kItemModifyAction.DoAction( pkCaster, NULL );
}

int PgAction_MoveItem::IsCanEquipPet(	CUnit *pkCasterUnit
									,	SItemPos const &kCasterPos
									,	PgBase_Item const &kCasterItem
									,	CUnit *pkTargetUnit
									,	SItemPos const &kTargetPos
									,	CONT_PLAYER_MODIFY_ORDER& rkOrder
									)
// ���ϰ��� ������ȣ(-1�̸�...)
{
	if ( pkCasterUnit && pkTargetUnit )
	{
		if ( kCasterItem.IsEmpty() )
		{
			return 0;
		}

		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kCasterItem.ItemNo());
		if(pItemDef)
		{
			if ( UT_PET == pkCasterUnit->UnitType() )
			{
				if ( UT_PLAYER == pkTargetUnit->UnitType() )
				{
					if ( kTargetPos.x != pItemDef->PrimaryInvType() )
					{
						return 20025;//�̵� �� �� ���� ��ġ �Դϴ�.
					}

					switch( kCasterPos.x )
					{
					case IT_FIT:
						{
							// �� ������ ����
							if ( pkCasterUnit->GetInven()->CheckCoolTime( kCasterItem.ItemNo(), PgInventory::EICool_Unequip, m_dwCurrentTime ) )
							{
								SaveCoolTime( kCasterItem.ItemNo(), PgInventory::EICool_Unequip, m_dwCurrentTime );
							}
							else
							{
								// CoolTime Check
								// ��������Ÿ���� ���� �ֽ��ϴ�.
								return 20028;
							}
						}break;
					case IT_FIT_CASH:
					case IT_FIT_COSTUME:
						{
							// ���� �̰� ��� ����
							return 20025;
						}break;
					default:
						{
						}break;
					}
				}
				else if ( UT_PET == pkTargetUnit->UnitType() )
				{
					switch( kTargetPos.x )
					{
					case IT_FIT:
						{
							//Ÿ�� ������ Ÿ���� ���̶�� Ȯ��.
							PgPet* pkPet = dynamic_cast<PgPet*>(pkTargetUnit);
							if(pkPet->GetPetType() == EPET_TYPE_3) //���̵����� ���� �Ұ�.
							{
								return 358;
							}

							// CoolTime : ����������
							if ( pkTargetUnit->GetInven()->CheckCoolTime(kCasterItem.ItemNo(), PgInventory::EICool_Equip, m_dwCurrentTime))
							{
								SaveCoolTime( kCasterItem.ItemNo(), PgInventory::EICool_Equip, m_dwCurrentTime );
							}
							else
							{
								// CoolTime Check
								// ��������Ÿ���� ���� �ֽ��ϴ�.
								return 20028;
							}

							if( true == kCasterItem.IsUseTimeOut() )
							{
								return 20026;	// ���� �� �� ���� �����Դϴ�.
							}

							if ( 0 < pkTargetUnit->GetAbil(AT_CANNOT_EQUIP) )
							{
								return 20026;	// ���� �� �� ���� �����Դϴ�.
							}

							if( !pItemDef->CanEquip() )
							{
								return 22020;// ������ �������� �ƴϴ�.
							}

							int const iGenderLimit = pItemDef->GetAbil(AT_GENDERLIMIT);
							if(0 == (iGenderLimit & pkTargetUnit->GetAbil(AT_GENDER)))//���� �˻�.
							{
								return 22030;// �� ���� �������� �ƴմϴ�.
							}

							__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
							switch ( PgClassPetDef::IsClassLimit( i64ClassLimit, pkTargetUnit->GetAbil(AT_CLASS) ) )
							{
							case S_OK:
								{
								}break;
							case E_ACCESSDENIED:
								{
									return 359;//���� ��ȭ�� ���� ���� �� �� �����ϴ�.
								}break;
							default:
								{
									return 358;//���� ������ ���� �ʴ� ������ �Դϴ�.
								}break;
							}

							int const iLevel = pkTargetUnit->GetAbil(AT_LEVEL);
							int const iLvLimit = pItemDef->GetAbil(AT_LEVELLIMIT);
							if(iLvLimit > iLevel)//�����˻�
							{
								return 22023;
							}

							int const iLvMaxLimit = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
							if (iLvMaxLimit != 0 &&  iLevel > iLvMaxLimit)
							{
								return 22025;
							}
						}break;
					case IT_FIT_CASH:
						{
							// ���� �̰� ��� ����
							return 20025;
						}break;
					default:
						{
							switch( kCasterPos.x )
							{
							case IT_FIT:
								{
									// �� ������ ����
									if ( pkCasterUnit->GetInven()->CheckCoolTime( kCasterItem.ItemNo(), PgInventory::EICool_Unequip, m_dwCurrentTime ) )
									{
										SaveCoolTime( kCasterItem.ItemNo(), PgInventory::EICool_Unequip, m_dwCurrentTime );
									}
									else
									{
										// CoolTime Check
										// ��������Ÿ���� ���� �ֽ��ϴ�.
										return 20028;
									}
								}break;
							case IT_FIT_CASH:
								{
									// ���� �̰� ��� ����
									return 20025;
								}break;
							}

							if ( kTargetPos.x != pItemDef->PetInvType() )
							{
								return 20025;//�̵� �� �� ���� ��ġ �Դϴ�.
							}
						}break;
					}
				}
			}
			else if (	UT_PLAYER == pkCasterUnit->UnitType()
					&&	UT_PET == pkTargetUnit->UnitType() )
			{
				switch ( kTargetPos.x )
				{
				case IT_FIT:
					{
						//Ÿ�� ������ Ÿ���� ���̶�� Ȯ��.
						PgPet* pkPet = dynamic_cast<PgPet*>(pkTargetUnit);
						if(pkPet->GetPetType() == EPET_TYPE_3) //���̵����� ���� �Ұ�.
						{
							return 358;
						}

						// CoolTime : ����������
						if ( pkTargetUnit->GetInven()->CheckCoolTime(kCasterItem.ItemNo(), PgInventory::EICool_Equip, m_dwCurrentTime))
						{
							SaveCoolTime( kCasterItem.ItemNo(), PgInventory::EICool_Equip, m_dwCurrentTime );
						}
						else
						{
							// CoolTime Check
							// ��������Ÿ���� ���� �ֽ��ϴ�.
							return 20028;
						}

						if( true == kCasterItem.IsUseTimeOut() )
						{
							return 20026;	// ���� �� �� ���� �����Դϴ�.
						}

						if ( 0 < pkTargetUnit->GetAbil(AT_CANNOT_EQUIP) )
						{
							return 20026;	// ���� �� �� ���� �����Դϴ�.
						}

						if( !pItemDef->CanEquip() )
						{
							return 22020;// ������ �������� �ƴϴ�.
						}

						int const iGenderLimit = pItemDef->GetAbil(AT_GENDERLIMIT);
						if(0 == (iGenderLimit & pkTargetUnit->GetAbil(AT_GENDER)))//���� �˻�.
						{
							return 22030;// �� ���� �������� �ƴմϴ�.
						}

						__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
						switch ( PgClassPetDef::IsClassLimit( i64ClassLimit, pkTargetUnit->GetAbil(AT_CLASS) ) )
						{
						case S_OK:
							{
							}break;
						case E_ACCESSDENIED:
							{
								return 359;//���� ��ȭ�� ���� ���� �� �� �����ϴ�.
							}break;
						default:
							{
								return 358;//���� ������ ���� �ʴ� ������ �Դϴ�.
							}break;
						}

						int const iLevel = pkTargetUnit->GetAbil(AT_LEVEL);
						int const iLvLimit = pItemDef->GetAbil(AT_LEVELLIMIT);
						if(iLvLimit > iLevel)//�����˻�
						{
							return 22023;
						}

						int const iLvMaxLimit = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
						if (iLvMaxLimit != 0 &&  iLevel > iLvMaxLimit)
						{
							return 22025;
						}
					}break;
				case IT_FIT_CASH:
					{
						return 20025;
						// ���� �̰� ��� ����
					}break;
				default:
					{
						if ( kTargetPos.x != pItemDef->PetInvType() )
						{
							return 20025;//�̵� �� �� ���� ��ġ �Դϴ�.
						}
					}break;
				}
			}
			else
			{
				return 20025;//�̵� �� �� ���� ��ġ �Դϴ�.
			}

			return 0;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return -1;
}


bool PgAction_MoveItem::IsCanEquip( CUnit* pkCaster, SItemPos const &kCasterPos, SItemPos const &kTargetPos, CONT_PLAYER_MODIFY_ORDER& rkOrder )
{//caster �� �������� Ÿ�� �ڸ��� �� �� �ִ°�.
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	PgBase_Item kCasterItem;
	int iErrorRet = 0;
	if(SUCCEEDED(pkCaster->GetInven()->GetItem(kCasterPos, kCasterItem)))
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kCasterItem.ItemNo());

		if(pItemDef)
		{
			EEquipPos const eEquipPos = (EEquipPos)pItemDef->EquipPos();
			
			if (kTargetPos.x == IT_FIT
				|| kTargetPos.x == IT_FIT_CASH
				|| kTargetPos.x == IT_FIT_COSTUME)
			{
				if(EQUIP_POS_NONE != eEquipPos )
				{
					if (0 < pkCaster->GetAbil(AT_CANNOT_EQUIP))
					{// ���� ���� �Ұ� ����� ������
						iErrorRet = 20026;	// ����/���� �� �� ���� �����Դϴ�.
						goto __SEND_ERROR;
					}
				}

				// CoolTime : ����������
				if (pkCaster->GetInven()->CheckCoolTime(kCasterItem.ItemNo(), PgInventory::EICool_Equip, m_dwCurrentTime))
				{
					SaveCoolTime(kCasterItem.ItemNo(), PgInventory::EICool_Equip, m_dwCurrentTime);
				}
				else
				{
					// CoolTime Check
					// ��������Ÿ���� ���� �ֽ��ϴ�.
					iErrorRet = 20028;
					goto __SEND_ERROR;
				}
			}

			if (kCasterPos.x == IT_FIT
				|| kCasterPos.x == IT_FIT_CASH
				|| kCasterPos.x == IT_FIT_COSTUME)
			{
				if(EQUIP_POS_NONE != eEquipPos )
				{
					if (0 < pkCaster->GetAbil(AT_CANNOT_EQUIP))
					{// ���� ���� �Ұ� ����� ������
						iErrorRet = 20026;	// ����/���� �� �� ���� �����Դϴ�.
						goto __SEND_ERROR;
					}
				}

				// CoolTime : ����������
				if (pkCaster->GetInven()->CheckCoolTime(kCasterItem.ItemNo(), PgInventory::EICool_Unequip, m_dwCurrentTime))
				{
					SaveCoolTime(kCasterItem.ItemNo(), PgInventory::EICool_Unequip, m_dwCurrentTime);
				}
				else
				{
					// CoolTime Check
					// ��������Ÿ���� ���� �ֽ��ϴ�.
					iErrorRet = 20028;
					goto __SEND_ERROR;
				}
			}

			switch(kTargetPos.x)
			{
			case IT_FIT_CASH:
				{
					if( true == kCasterItem.IsUseTimeOut() || 
						true == kCasterItem.EnchantInfo().IsCurse())
					{
						iErrorRet = 20026;	// ����/���� �� �� ���� �����Դϴ�.
						break;
					}
					else if( true == kCasterItem.EnchantInfo().IsSeal())
					{
						iErrorRet = 1409;	// ���� ������ �����ؾ� �մϴ�.
						break;
					}

					if(!pItemDef->IsType(ITEM_TYPE_AIDS))
					{
						iErrorRet = 20026;	// ����/���� �� �� ���� �����Դϴ�.
						break;
					}

					if(!pItemDef->CanEquip())//������ �������̸�
					{
						iErrorRet = 22020;
						break;
					}

					if(pItemDef->GetAbil(AT_REVERSION_ITEM))
					{
						SEnchantInfo kEnchantInfo = kCasterItem.EnchantInfo();
						if(!kEnchantInfo.IsAttached())
						{
							kEnchantInfo.IsAttached(1);
							rkOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant(kCasterItem,kCasterPos,kEnchantInfo)));
						}
					}

					int const iGenderLimit = pItemDef->GetAbil(AT_GENDERLIMIT);
					if(0 == (iGenderLimit & pkCaster->GetAbil(AT_GENDER)))//���� �˻�.
					{
						switch( iGenderLimit )
						{
						case GWL_MALE:{iErrorRet = 22021;}break;
						case GWL_FEMALE:{iErrorRet = 22022;}break;
						case GWL_PET_MALE:
						case GWL_PET_FEMALE:
						case GWL_PET_UNISEX:
							{
								iErrorRet = 22029;
							}break;
						default:{iErrorRet = 22030;}break;
						}				
						break;
					}

					int const iLvLimit = std::max<int>(0,pItemDef->GetAbil(AT_LEVELLIMIT) - pkCaster->GetAbil(AT_EQUIP_LEVELLIMIT_MIN + kTargetPos.y));
					int const iLevel = pkCaster->GetAbil(AT_LEVEL);
					if(iLvLimit > iLevel)//�����˻�
					{
						iErrorRet = 22023;
						break;
					}

					int const iLvMaxLimit = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
					if (iLvMaxLimit != 0 &&  iLevel > iLvMaxLimit)
					{
						iErrorRet = 22025;
						break;
					}

					// Class Limit
					__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
					//if(0 == (i64ClassLimit & (__int64)pow((double)2, (double)pkCaster->GetAbil(AT_CLASS))))//Ŭ���� �˻�
					if (! IS_CLASS_LIMIT(i64ClassLimit, pkCaster->GetAbil(AT_CLASS)))
					{
						iErrorRet = 22024;
						break;
					}
					
					// ���� ��ġ �˻�
					switch(eEquipPos)
					{
					//case EQUIP_POS_EARRING:	// �Ͱ����� ����/������ �ΰ��� ���� ����
					//	{
					//		EEquipPos eTgPos = (EEquipPos) kTargetPos.y;
					//		if(	eTgPos != EQUIP_POS_EARRING_L 
					//		&&	eTgPos != EQUIP_POS_EARRING_R)
					//		{
					//			iErrorRet = 20025;
					//		}
					//	}break;
					case EQUIP_POS_RING: // ������ ����/������ �ΰ��� ���� ����
						{
							EEquipPos eTgPos = (EEquipPos) kTargetPos.y;
							if(	eTgPos != EQUIP_POS_RING_L 
							&&	eTgPos != EQUIP_POS_RING_R)
							{
								iErrorRet = 20025;
							}
						}break;
					case EQUIP_POS_WEAPON:
						{
							//ĳ�� ������ ��� �⺻ ������ ���Ⱑ �־�� �Ѵ�.
							//�����ϰ� �ִ� �⺻ ����� �� �Ѵ�.
							PgBase_Item kWeapon;
							if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kWeapon)))
							{
								CItemDef const* pBaseWeaponDef = kItemDefMgr.GetDef(kWeapon.ItemNo());
								if(NULL == pBaseWeaponDef)
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"ItemDef is NULL ItemNo[" << kWeapon.ItemNo() << L"]");
									iErrorRet = 20024;
									break;
								}

								if(pItemDef->GetAbil(AT_WEAPON_TYPE) != pBaseWeaponDef->GetAbil(AT_WEAPON_TYPE))
								{
									//�����ϰ� �ִ� �⺻ ����� �ٸ� Ÿ���̶�� �������� ���Ѵ�.
									iErrorRet = 22028;
									break;
								}
							}
							//�⺻ ���Ⱑ �����Ǿ����� ���� ���
							else
							{
								//�����ϰ� �ִ� �⺻ ����� �ٸ� Ÿ���̶�� �������� ���Ѵ�.
								iErrorRet = 22028;
								break;
							}

							// ä�������� �����Ϸ��� �ϸ�
							int const iMainToolType = JobSkillToolUtil::GetToolGatherType( kCasterItem.ItemNo() );
							if( 0 != iMainToolType )
							{
								PgPlayer* pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
								if( !pkPlayer || !JobSkillToolUtil::IsCanEquip(pkPlayer, kCasterItem, iErrorRet) )
								{
									break;
								}
								else
								{
									int const CHECK_SUBTOOL_COUNT = 2;
									EEquipPos const iArrSubToolPos[CHECK_SUBTOOL_COUNT] = {EQUIP_POS_SHEILD, EQUIP_POS_KICKBALL};
									for( int iCount = 0; iCount < CHECK_SUBTOOL_COUNT; ++iCount )
									{
										PgBase_Item kShield;
										if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, iArrSubToolPos[iCount]), kShield)))
										{//���������� �ۿ��ϰ� �ְ�, ���� �����Ϸ��� �� ������ �ٸ� ä��Ÿ���̶�� ���� ���� �Ѵ�.
											CItemDef const *pkShieldDef = kItemDefMgr.GetDef(kShield.ItemNo());
											if (pkShieldDef == NULL)
											{
												VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"ItemDef is NULL ItemNo[" << kShield.ItemNo() << L"]");
												iErrorRet = 20024;
												break;
											}
											if (pkShieldDef->EquipPos() == iArrSubToolPos[iCount])
											{
												int const iSubToolType = JobSkillToolUtil::GetToolGatherType( kShield.ItemNo() );
												if( 0 != iSubToolType 
													&& iMainToolType != iSubToolType )
												{
													SItemPos kNewTarget;
													if (GetEmptyPos(pkCaster, kCasterPos, kTargetPos, kNewTarget))
													{
														SPMOD_Modify_Pos kNewModify(kNewTarget, SItemPos(IT_FIT_CASH, iArrSubToolPos[iCount]), PgBase_Item::NullData(), kShield);
														SPMO kModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewModify);
														rkOrder.push_back(kModify);
													}
													else
													{
														iErrorRet = 20023;
													}
												}
											}
										}
									}
								}
							}
							else
							{// �����Ϸ��� �������� ä�� �ֵ����� �ƴϰ� 
								PgBase_Item kMainWeapon;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, EQUIP_POS_WEAPON), kMainWeapon)))
								{// �������� ���Ⱑ ä�� �������
									if( JobSkillToolUtil::GetToolType( kMainWeapon.ItemNo() ) )
									{
										int const CHECK_SUBTOOL_COUNT = 2;
										EEquipPos const iArrSubToolPos[CHECK_SUBTOOL_COUNT] = {EQUIP_POS_SHEILD, EQUIP_POS_KICKBALL};
										for( int iCount = 0; iCount < CHECK_SUBTOOL_COUNT; ++iCount )
										{
											PgBase_Item kSubToolItem;
											if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, iArrSubToolPos[iCount]), kSubToolItem))
												&& JobSkillToolUtil::GetToolType( kSubToolItem.ItemNo() ) )
											{// ���������� �������̶�� ���� ���� �����Ѵ�.								
												SItemPos kFitTargetPos(kTargetPos.x, kTargetPos.y+1);

												//��� �κ��� ��� �ִ� ���� ���� ��� ���������� ���� ���� �Ѵ�.
												if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_EQUIP, kFitTargetPos) )
												{
													// �������� ���� ����
													SPMOD_Modify_Pos kNewFitModify(kFitTargetPos, SItemPos(IT_FIT_CASH, iArrSubToolPos[iCount]), PgBase_Item::NullData(), kSubToolItem);
													SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
													rkOrder.push_back(kFitModify);
												}
												else
												{
													// �κ��丮�� �� ���ִ� �޽��� ���
													iErrorRet = 20023;
													break;
												}
											}
										}
									}
								}
							}
						}break;
					case EQUIP_POS_SHEILD:
						{
							// ���������� �����Ϸ��� �ϸ�
							// ������ ���� ���¿��� �Ѵ�.
							if( JobSkillToolUtil::GetToolType( kCasterItem.ItemNo() ) )
							{
								PgBase_Item kToolItem;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, EQUIP_POS_WEAPON), kToolItem)))
								{
									if( !JobSkillToolUtil::GetToolType( kToolItem.ItemNo() ) )
									{
										iErrorRet = 25001;
										break;
									}
									else
									{
										PgPlayer* pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
										if( !pkPlayer )
										{
											break;
										}
										if(!JobSkillToolUtil::IsCanEquip(pkPlayer, kCasterItem, iErrorRet) )
										{
											break;
										}
									}
								}
								else
								{//���������� �����Ϸ���, ���� �� ������ �����ؾ� �մϴ�.
									iErrorRet = 25001;
									break;
								}
							}
							{//���밡���� ��
								PgBase_Item kKickBall;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, EQUIP_POS_KICKBALL), kKickBall)))
								{//ű����ġ�� ������ �����Ǿ� �ִٸ�																		
									SItemPos kEmptyInvPos;
									if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_CASH, kEmptyInvPos) )
									{//ű�� ���� ����
										SPMOD_Modify_Pos kNewFitModify(kEmptyInvPos, SItemPos(IT_FIT_CASH, EQUIP_POS_KICKBALL), PgBase_Item::NullData(), kKickBall);
										SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
										rkOrder.push_back(kFitModify);
									}
									else
									{
										// �κ��丮�� �� ���ִ� �޽��� ���
										iErrorRet = 20023;
										break;
									}
								}
							}
						}break;
					case EQUIP_POS_KICKBALL:
						{//ű�������Ǹ� �ǵ� ����
							PgBase_Item kKickBall;
							if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, EQUIP_POS_SHEILD), kKickBall)))
							{//ű����ġ�� ������ �����Ǿ� �ִٸ�																		
								SItemPos kEmptyInvPos;
								if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_CASH, kEmptyInvPos) )
								{//ű�� ���� ����
									SPMOD_Modify_Pos kNewFitModify(kEmptyInvPos, SItemPos(IT_FIT_CASH, EQUIP_POS_SHEILD), PgBase_Item::NullData(), kKickBall);
									SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
									rkOrder.push_back(kFitModify);
								}
								else
								{
									// �κ��丮�� �� ���ִ� �޽��� ���
									iErrorRet = 20023;
									break;
								}
							}
						}break;
					default:
						{
							if(eEquipPos != kTargetPos.y)
							{
								iErrorRet = 20025;
							}
						}break;
					}
					if (iErrorRet != 0)
					{
						break;
					}
					return true;
				}break;
			case IT_FIT:
				{
					if(	true == kCasterItem.EnchantInfo().IsCurse() ||
						(UIT_STATE_PET != kCasterItem.State() && kCasterItem.IsUseTimeOut())
					)
					{
						iErrorRet = 20026;	// ����/���� �� �� ���� �����Դϴ�.
						break;
					}
					else if( true == kCasterItem.EnchantInfo().IsSeal())
					{
						iErrorRet = 1409;	// ���� ������ �����ؾ� �մϴ�.
						break;
					}

					if(pItemDef->GetAbil(AT_REVERSION_ITEM))
					{
						SEnchantInfo kEnchantInfo = kCasterItem.EnchantInfo();
						if(!kEnchantInfo.IsAttached())
						{
							kEnchantInfo.IsAttached(1);
							rkOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant(kCasterItem,kCasterPos,kEnchantInfo)));
						}
					}

					if(!pItemDef->CanEquip())//������ �������̸�
					{
						iErrorRet = 22020;
						break;
					}

					int const iGenderLimit = pItemDef->GetAbil(AT_GENDERLIMIT);
					if(0 == (iGenderLimit & pkCaster->GetAbil(AT_GENDER)))//���� �˻�.
					{
						switch( iGenderLimit )
						{
						case GWL_MALE:{iErrorRet = 22021;}break;
						case GWL_FEMALE:{iErrorRet = 22022;}break;
						case GWL_PET_MALE:
						case GWL_PET_FEMALE:
						case GWL_PET_UNISEX:
							{
								iErrorRet = 22029;
							}break;
						default:{iErrorRet = 22030;}break;
						}
						break;
					}

					int const iLvLimit = std::max<int>(0,pItemDef->GetAbil(AT_LEVELLIMIT) - pkCaster->GetAbil(AT_EQUIP_LEVELLIMIT_MIN + kTargetPos.y));
					int const iLevel = pkCaster->GetAbil(AT_LEVEL);
					if(iLvLimit > iLevel)//�����˻�
					{
						iErrorRet = 22023;
						break;
					}

					int const iLvMaxLimit = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
					if (iLvMaxLimit != 0 &&  iLevel > iLvMaxLimit)
					{
						iErrorRet = 22025;
						break;
					}

					// Class Limit
					__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
					//if(0 == (i64ClassLimit & (__int64)pow((double)2, (double)pkCaster->GetAbil(AT_CLASS))))//Ŭ���� �˻�
					if (! IS_CLASS_LIMIT(i64ClassLimit, pkCaster->GetAbil(AT_CLASS)))
					{
						iErrorRet = 22024;
						break;
					}
					
					// ���� ��ġ �˻�
					switch(eEquipPos)
					{
					//case EQUIP_POS_EARRING:	// �Ͱ����� ����/������ �ΰ��� ���� ����
					//	{
					//		EEquipPos eTgPos = (EEquipPos) kTargetPos.y;
					//		if(	eTgPos != EQUIP_POS_EARRING_L 
					//		&&	eTgPos != EQUIP_POS_EARRING_R)
					//		{
					//			iErrorRet = 20025;
					//		}
					//	}break;
					case EQUIP_POS_RING: // ������ ����/������ �ΰ��� ���� ����
						{
							EEquipPos eTgPos = (EEquipPos) kTargetPos.y;
							if(	eTgPos != EQUIP_POS_RING_L 
							&&	eTgPos != EQUIP_POS_RING_R)
							{
								iErrorRet = 20025;
							}
						}break;
					case EQUIP_POS_WEAPON:
						{
							//���� ���� ĳ�� ����� �� �Ѵ�.
							PgBase_Item kWeapon;
							if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, EQUIP_POS_WEAPON), kWeapon)))
							{
								CItemDef const* pBaseWeaponDef = kItemDefMgr.GetDef(kWeapon.ItemNo());
								if(NULL == pBaseWeaponDef)
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"ItemDef is NULL ItemNo[" << kWeapon.ItemNo() << L"]");
									iErrorRet = 20024;
									break;
								}

								if(pItemDef->GetAbil(AT_WEAPON_TYPE) != pBaseWeaponDef->GetAbil(AT_WEAPON_TYPE))
								{
									//����� ĳ�� ������ �ٸ� Ÿ���̶�� �������� ���Ѵ�.
									iErrorRet = 22028;
									break;
								}
							}

							if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_COSTUME, EQUIP_POS_WEAPON), kWeapon)))
							{
								CItemDef const* pBaseWeaponDef = kItemDefMgr.GetDef(kWeapon.ItemNo());
								if (NULL == pBaseWeaponDef)
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"ItemDef is NULL ItemNo[" << kWeapon.ItemNo() << L"]");
									iErrorRet = 20024;
									break;
								}

								if (pItemDef->GetAbil(AT_WEAPON_TYPE) != pBaseWeaponDef->GetAbil(AT_WEAPON_TYPE))
								{
									iErrorRet = 22028; // TODO : UnEquip weapon
									break;
								}
							}
							
							// ä�������� �����Ϸ��� �ϸ�
							int const iMainToolType = JobSkillToolUtil::GetToolGatherType( kCasterItem.ItemNo() );
							if( 0 != iMainToolType )
							{
								PgPlayer* pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
								if( !pkPlayer || !JobSkillToolUtil::IsCanEquip(pkPlayer, kCasterItem, iErrorRet) )
								{
									break;
								}
								else
								{
									int const CHECK_SUBTOOL_COUNT = 2;
									EEquipPos const iArrSubToolPos[CHECK_SUBTOOL_COUNT] = {EQUIP_POS_SHEILD, EQUIP_POS_KICKBALL};
									for( int iCount = 0; iCount < CHECK_SUBTOOL_COUNT; ++iCount )
									{
										PgBase_Item kShield;
										if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, iArrSubToolPos[iCount]), kShield)))
										{//���������� �ۿ��ϰ� �ְ�, ���� �����Ϸ��� �� ������ �ٸ� ä��Ÿ���̶�� ���� ���� �Ѵ�.
											CItemDef const *pkShieldDef = kItemDefMgr.GetDef(kShield.ItemNo());
											if (pkShieldDef == NULL)
											{
												VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"ItemDef is NULL ItemNo[" << kShield.ItemNo() << L"]");
												iErrorRet = 20024;
												break;
											}
											if (pkShieldDef->EquipPos() == iArrSubToolPos[iCount])
											{
												int const iSubToolType = JobSkillToolUtil::GetToolGatherType( kShield.ItemNo() );
												if( 0 != iSubToolType 
													&& iMainToolType != iSubToolType )
												{
													SItemPos kNewTarget;
													if (GetEmptyPos(pkCaster, kCasterPos, kTargetPos, kNewTarget))
													{
														SPMOD_Modify_Pos kNewModify(kNewTarget, SItemPos(IT_FIT, iArrSubToolPos[iCount]), PgBase_Item::NullData(), kShield);
														SPMO kModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewModify);
														rkOrder.push_back(kModify);
													}
													else
													{
														iErrorRet = 20023;
														break;
													}
												}
											}
										}
									}
								}
							}
							else
							{// �����Ϸ��� �������� ä�� �ֵ����� �ƴϰ� 
								PgBase_Item kMainWeapon;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kMainWeapon)))
								{// �������� ���Ⱑ ä�� �������
									if( JobSkillToolUtil::GetToolType( kMainWeapon.ItemNo() ) )
									{
										int const CHECK_SUBTOOL_COUNT = 2;
										EEquipPos const iArrSubToolPos[CHECK_SUBTOOL_COUNT] = {EQUIP_POS_SHEILD, EQUIP_POS_KICKBALL};
										for( int iCount = 0; iCount < CHECK_SUBTOOL_COUNT; ++iCount )
										{
											PgBase_Item kSubToolItem;
											if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, iArrSubToolPos[iCount]), kSubToolItem))
												&& JobSkillToolUtil::GetToolType( kSubToolItem.ItemNo() ) )
											{// ���������� �������̶�� ���� ���� �����Ѵ�.								
												SItemPos kFitTargetPos(kTargetPos.x, kTargetPos.y+1);

												//��� �κ��� ��� �ִ� ���� ���� ��� ���������� ���� ���� �Ѵ�.
												if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_EQUIP, kFitTargetPos) )
												{
													// �������� ���� ����
													SPMOD_Modify_Pos kNewFitModify(kFitTargetPos, SItemPos(IT_FIT, iArrSubToolPos[iCount]), PgBase_Item::NullData(), kSubToolItem);
													SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
													rkOrder.push_back(kFitModify);
												}
												else
												{
													// �κ��丮�� �� ���ִ� �޽��� ���
													iErrorRet = 20023;
													break;
												}
											}
										}
									}
								}
							}
						}break;
					case EQUIP_POS_SHEILD:
						{	// ���������� �����Ϸ��� �ϸ�
							// ������ ���� ���¿��� �Ѵ�.
							if( JobSkillToolUtil::GetToolType( kCasterItem.ItemNo() ) )
							{
								PgBase_Item kToolItem;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kToolItem)))
								{
									if( !JobSkillToolUtil::GetToolType( kToolItem.ItemNo() ) )
									{
										iErrorRet = 25001;
										break;
									}
									else
									{
										if( JobSkillToolUtil::GetToolGatherType(kToolItem.ItemNo()) == JobSkillToolUtil::GetToolGatherType(kCasterItem.ItemNo()) )
										{
											PgPlayer* pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
											if( !pkPlayer )
											{
												break;
											}
											if(!JobSkillToolUtil::IsCanEquip(pkPlayer, kCasterItem, iErrorRet) )
											{
												break;
											}
										}
										else
										{
											iErrorRet = 25018;
											break;
										}
									}
								}
								else
								{//���������� �����Ϸ���, ���� �� ������ �����ؾ� �մϴ�.
									iErrorRet = 25001;
									break;
								}
							}
							{//���밡���� ��
								PgBase_Item kKickBall;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_KICKBALL), kKickBall)))
								{//ű����ġ�� ������ �����Ǿ� �ִٸ�																		
									SItemPos kEmptyInvPos;
									if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_EQUIP, kEmptyInvPos) )
									{//ű�� ���� ����
										SPMOD_Modify_Pos kNewFitModify(kEmptyInvPos, SItemPos(IT_FIT, EQUIP_POS_KICKBALL), PgBase_Item::NullData(), kKickBall);
										SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
										rkOrder.push_back(kFitModify);
									}
									else
									{
										// �κ��丮�� �� ���ִ� �޽��� ���
										iErrorRet = 20023;
										break;
									}
								}
							}
						}break;
					case EQUIP_POS_KICKBALL:
						{//ű�������Ǹ� �ǵ� ����
							PgBase_Item kKickBall;
							if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_SHEILD), kKickBall)))
							{//�ǵ���ġ�� ������ �����Ǿ� �ִٸ�																		
								SItemPos kEmptyInvPos;
								if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_EQUIP, kEmptyInvPos) )
								{//�ǵ� ���� ����
									SPMOD_Modify_Pos kNewFitModify(kEmptyInvPos, SItemPos(IT_FIT, EQUIP_POS_SHEILD), PgBase_Item::NullData(), kKickBall);
									SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
									rkOrder.push_back(kFitModify);
								}
								else
								{
									// �κ��丮�� �� ���ִ� �޽��� ���
									iErrorRet = 20023;
									break;
								}
							}
						}break;
					default:
						{
							if(eEquipPos != kTargetPos.y)
							{
								iErrorRet = 20025;
							}
						}break;
					}
					if (iErrorRet != 0)
					{
						break;
					}
					return true;
				}break;
			case IT_FIT_COSTUME:
				{
					//! New costume it maked by reOiL skype: imidg787mini
					if (true == kCasterItem.IsTimeOuted() ||
						true == kCasterItem.EnchantInfo().IsCurse())
					{
						iErrorRet = 20026; // can't equip item, he is timedout or Cruse
						break;
					}
					else if (kCasterItem.EnchantInfo().IsSeal())
					{
						iErrorRet = 1409; // Item is Seal, can't equip
						break;
					}

					//! Cheking is item is cash item
					/*
					if (!pItemDef->IsType(ITEM_TYPE_AIDS))
					{
						iErrorRet = 22026;
						break;
					}
					*/

					//! Can't equip item
					if (!pItemDef->CanEquip())
					{
						iErrorRet = 22020;
						break;
					}

					//! Check Gender
					int const iGenderLimit = pItemDef->GetAbil(AT_GENDERLIMIT);
					if (!(iGenderLimit & pkCaster->GetAbil(AT_GENDER)))
					{
						switch (iGenderLimit)
						{
						case GWL_MALE: { iErrorRet = 22021; }break;
						case GWL_FEMALE: { iErrorRet = 22022; }break;
						case GWL_PET_MALE:
						case GWL_PET_FEMALE:
						case GWL_PET_UNISEX:
						{
							iErrorRet = 22029;
						}break;
						default: { iErrorRet = 22030; }break;
						}
						break;
					}

					//! Level Limit Min
					int const iLvLimit = std::max<int>(0, pItemDef->GetAbil(AT_LEVELLIMIT) - pkCaster->GetAbil(AT_EQUIP_LEVELLIMIT_MIN + kTargetPos.y));
					int const iLevel = pkCaster->GetAbil(AT_LEVEL);
					if (iLvLimit > iLevel)
					{
						iErrorRet = 22023;
						break;
					}

					//! Check Level Limit Max
					int const iLvMaxLimit = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
					if (iLvMaxLimit != 0 && iLevel > iLvMaxLimit)
					{
						iErrorRet = 22025;
						break;
					}

					//! ClassLimit check
					__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
					if (!IS_CLASS_LIMIT(i64ClassLimit, pkCaster->GetAbil(AT_CLASS)))
					{
						iErrorRet = 22024;
						break;
					}

					//! Equip item started
					switch (eEquipPos)
					{
						// can't use this item for costume
						//case EQUIP_POS_WEAPON:
					case EQUIP_POS_SHEILD:
					case EQUIP_POS_KICKBALL:
					case EQUIP_POS_RING_L:
					case EQUIP_POS_RING_R:
					case EQUIP_POS_EARRING:
					case EQUIP_POS_NECKLACE:
					case EQUIP_POS_BELT:
						{
							iErrorRet = 20025;
						}break;
					//! Special for weapon, because weapon by class
					case EQUIP_POS_WEAPON:
						{
							PgBase_Item kWeapon;
							if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kWeapon)))
							{
								CItemDef const* pBaseWeaponDef = kItemDefMgr.GetDef(kWeapon.ItemNo());
								if (pBaseWeaponDef == NULL)
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"ItemDef is NULL ItemNo[" << kWeapon.ItemNo() << L"]");
									iErrorRet = 20024;
									break;
								}

								if (pItemDef->GetAbil(AT_WEAPON_TYPE) != pBaseWeaponDef->GetAbil(AT_WEAPON_TYPE))
								{
									iErrorRet = 22028;
									break;
								}
							}
							else
							{
								iErrorRet = 22028;
								break;
							}

							// Job item
							if (0 != JobSkillToolUtil::GetToolGatherType(kCasterItem.ItemNo()))
							{
								iErrorRet = 22028;
								break;
							}
						}break;
					default:
						{
							if (eEquipPos != kTargetPos.y)
							{
								iErrorRet = 20025;
							}
						}break;
					}

					if (iErrorRet != 0)
					{
						break;
					}
					return true;
				}break;
			default:
				{
					//���� ��ġ�� ���� + ���� �϶�
					if(EQUIP_POS_WEAPON == (EEquipPos)pItemDef->EquipPos())
					{
						if(IT_FIT == kCasterPos.x)
						{
							//���⸦ �ٲ� �����ϴ� ���� �ƴ϶� ���⸦ �����Ϸ��� ����� ����
							PgBase_Item kChangeWeapon;
							if (FAILED(pkCaster->GetInven()->GetItem(SItemPos(kTargetPos.x, kTargetPos.y), kChangeWeapon)))
							{
								CONT_PLAYER_MODIFY_ORDER kTempOrder;
								SItemPos kNewTargetPos;
								//���� ���� ĳ���� ���Ⱑ ���� ��� ĳ���� ���⵵ ��ü �Ѵ�.
								PgBase_Item kWeapon;
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_CASH, EQUIP_POS_WEAPON), kWeapon)))
								{								
									//ĳ�� �κ��� ��� �ִ� ���� ���� ��� ĳ�� �����۵� ���� ���� �Ѵ�.
									if(true == pkCaster->GetInven()->GetFirstEmptyPos(IT_CASH, kNewTargetPos))
									{
										SPMOD_Modify_Pos kNewModify(kNewTargetPos, SItemPos(IT_FIT_CASH, EQUIP_POS_WEAPON), PgBase_Item::NullData(), kWeapon);
										SPMO kModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewModify);
										kTempOrder.push_back(kModify);
									}
									else
									{
										// �κ��丮�� �� ���ִ� �޽��� ���
										iErrorRet = 20023;
										break;
									}
								}

								//Costume weapon unqeuipp whit normal weapon
								if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT_COSTUME, EQUIP_POS_WEAPON), kWeapon)))
								{
									CItemDef const *pItemDef = kItemDefMgr.GetDef(kWeapon.ItemNo());
									//SItemPos kNewTargetPos; // new positon for item
									kNewTargetPos.y += 1; // for next position
									if (true == pkCaster->GetInven()->GetNextEmptyPos(pItemDef->IsType(ITEM_TYPE_AIDS) ? IT_CASH : IT_EQUIP, kNewTargetPos))
									{
										SPMOD_Modify_Pos kNewModify(kNewTargetPos, SItemPos(IT_FIT_COSTUME, EQUIP_POS_WEAPON), PgBase_Item::NullData(), kWeapon);
										SPMO kModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewModify);
										kTempOrder.push_back(kModify);
									}
									else
									{
										iErrorRet = 20023; // Inventory is full
										break;
									}
								}

								while (kTempOrder.size() != 0)
								{
									rkOrder.push_back(kTempOrder.front());
									kTempOrder.pop_front();
								}
								// �������� ���Ⱑ ä�� ������ ���
								if( JobSkillToolUtil::GetToolType( kCasterItem.ItemNo() ) )
								{
									int const CHECK_SUBTOOL_COUNT = 2;
									EEquipPos const iArrSubToolPos[CHECK_SUBTOOL_COUNT] = {EQUIP_POS_SHEILD, EQUIP_POS_KICKBALL};
									for( int iCount = 0; iCount < CHECK_SUBTOOL_COUNT; ++iCount )
									{
										PgBase_Item kSubToolItem;
										if (SUCCEEDED(pkCaster->GetInven()->GetItem(SItemPos(IT_FIT, iArrSubToolPos[iCount]), kSubToolItem))
											&& JobSkillToolUtil::GetToolType( kSubToolItem.ItemNo() ) )
										{// ���������� �������̶�� ���� ���� �����Ѵ�.								
											SItemPos kFitTargetPos(kTargetPos.x, kTargetPos.y+1);

											//��� �κ��� ��� �ִ� ���� ���� ��� ���������� ���� ���� �Ѵ�.
											if( true == pkCaster->GetInven()->GetNextEmptyPos(IT_EQUIP, kFitTargetPos) )
											{
												// �������� ���� ����
												SPMOD_Modify_Pos kNewFitModify(kFitTargetPos, SItemPos(IT_FIT, iArrSubToolPos[iCount]), PgBase_Item::NullData(), kSubToolItem);
												SPMO kFitModify(IMET_MODIFY_POS, pkCaster->GetID(), kNewFitModify);
												rkOrder.push_back(kFitModify);
											}
											else
											{
												// �κ��丮�� �� ���ִ� �޽��� ���
												iErrorRet = 20023;
												break;
											}
										}
									}
								}
							}
						}
					}

					if(pItemDef->PrimaryInvType() == kTargetPos.x)//�´� �κ�?
					{
						return true;
					}
					else
					{
						switch(kTargetPos.x)
						{
						case IT_SHARE_RENTAL_SAFE1:
						case IT_SHARE_RENTAL_SAFE2:
						case IT_SHARE_RENTAL_SAFE3:
						case IT_SHARE_RENTAL_SAFE4:
							{
								// �ŷ� �Ұ� ������ �������� ���� â���� ���� �� ����.
								if(false == CheckEnableTrade(kCasterItem,ICMET_Cant_UseShareRental))
								{
									iErrorRet = 20025;//<TEXT ID="20025" Text="�̵��� �� ���� ��ġ�Դϴ�."/>
									break;
								}
							}// break; ���� ������� ����
						case IT_SAFE:
						case IT_SAFE_ADDITION:
						case IT_CASH_SAFE:
						case IT_RENTAL_SAFE1:
						case IT_RENTAL_SAFE2:
						case IT_RENTAL_SAFE3:
							{
								if((kTargetPos.x == IT_CASH_SAFE) && !pItemDef->IsType(ITEM_TYPE_AIDS))
								{
									iErrorRet = 20025;//<TEXT ID="20025" Text="�̵��� �� ���� ��ġ�Դϴ�."/>
									break;
								}

								if(pItemDef->GetAbil(AT_ATTRIBUTE) & ICMET_Cant_MoveItemBox)
								{
									iErrorRet = 20025;//<TEXT ID="20025" Text="�̵��� �� ���� ��ġ�Դϴ�."/>
									break;
								}
								return true;
							}break;
						default:
							{
								iErrorRet = 20025;//<TEXT ID="20025" Text="�̵��� �� ���� ��ġ�Դϴ�."/>
							}break;
						}
					}

					if(0 != iErrorRet)
					{
						break;
					}
					return true;
				}break;
			}
		}

__SEND_ERROR :
		if(0 < iErrorRet)
		{
			pkCaster->SendWarnMessage(iErrorRet);
		}

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;//������ �������ϱ�.
}

bool PgAction_MoveItem::GetEmptyPos(CUnit* pkCaster, SItemPos const &kCasterPos, SItemPos const &kTargetPos, SItemPos& rkOutPos) const
{
	PgInventory* pkInven = pkCaster->GetInven();
	
	rkOutPos.x = IT_EQUIP;
	rkOutPos.y = 0;
	EInvType eInvType = (EInvType) rkOutPos.x;
	while (pkInven->GetNextEmptyPos(eInvType, rkOutPos))
	{
		if (rkOutPos != kCasterPos && rkOutPos != kTargetPos)
		{
			return true;
		}
		++rkOutPos.y;
	}

	rkOutPos.y = 0;
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgAction_MoveItem::SaveCoolTime(int const iItemNo, BYTE const byCoolTimeType, DWORD const dwCoolTime)
{
	// ���߿� CoolTime�� �߰��� �ֱ� ���� �ӽ÷� ����
	CONT_ITEM_COOLTIME::iterator itor = m_kCoolTime.find(iItemNo);
	if (itor == m_kCoolTime.end())
	{
		SItemCoolTimeInfo sCoolTimeInfo;
		sCoolTimeInfo.byCoolTimeType = byCoolTimeType;
		sCoolTimeInfo.dwCoolTime = dwCoolTime;
		m_kCoolTime.insert(std::make_pair(iItemNo, sCoolTimeInfo));
	}
}

void PgAction_MoveItem::AddCoolTime( PgInventory * pkInv ,CUnit * pkCaster)
{
	if ( pkInv )
	{
		// Item Cooltime �� �߰�
		//	MapServer���� �̻� ������, ������ CoolTime�� �����Ű��.
		CONT_ITEM_COOLTIME::iterator itor = m_kCoolTime.begin();
		while (itor != m_kCoolTime.end())
		{
			pkInv->AddCoolTime(itor->first, (PgInventory::EItemCoolTimeType)itor->second.byCoolTimeType, itor->second.dwCoolTime, pkCaster->GetAbil(AT_R_COOLTIME_RATE_ITEM));
			++itor;
		}
	}
}

void PgAction_MoveItem::MakeUnEquipPetOrder( SItemPos const &kItemPos, PgBase_Item const &kUnEquipPetItem, CONT_PLAYER_MODIFY_ORDER &kOrder )
{
	// ���� ���� ������ ü���� ������ ó���� �Ŀ� �̿����� ó���Ǵϱ� �������� �ݴ�� �־��־�� �Ѵ�.
	PgPet* pkPet = m_pkGround->GetPet( kUnEquipPetItem.Guid() );
	if ( pkPet )
	{
		CSkill::MAP_COOLTIME kSkillCoolTime;
		pkPet->GetSkill()->GetCoolTimeMap( kSkillCoolTime );

		DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime() + 2000;//2�������� ������
		CSkill::MAP_COOLTIME::iterator itr = kSkillCoolTime.begin();
		while ( itr != kSkillCoolTime.end() )
		{
			if ( itr->second > dwServerElapsedTime )
			{
				++itr;
			}
			else
			{
				itr = kSkillCoolTime.erase( itr );
			}
		}

		SPMO kIMO_Pet( IMET_PET, pkPet->Caller() );
		kIMO_Pet.m_kPacket.Push( kItemPos );
		kIMO_Pet.m_kPacket.Push( kUnEquipPetItem.Guid() );
		kIMO_Pet.m_kPacket.Push( pkPet->GetAbil(AT_MP) );
		PU::TWriteTable_AA( kIMO_Pet.m_kPacket, kSkillCoolTime );

		kOrder.push_back( kIMO_Pet );
	}
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_UseItem
//////////////////////////////////////////////////////////////////////////////
PgAction_UseItem::PgAction_UseItem(PgGround * const pkGround, PgBase_Item const &kItem, WORD const usUseCount, BM::Stream const& rkPacket)
	:	m_pkGround(pkGround), m_kItem(kItem), m_usUseCount(usUseCount), m_kPacket(rkPacket)
{
}

CUnit* PgAction_UseItem::GetTargetUnit( CItemDef const *pkItemDef, CUnit * pkCaster, PgGround const *pkGround )
{
	if ( UT_PLAYER == pkCaster->UnitType() )
	{
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if ( !pkPlayer )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!");
			return NULL;
		}

		if ( pkItemDef->IsType( ITEM_TYPE_PET ) )
		{
			PgPet * pkPet = pkGround->GetPet( pkPlayer );
			if ( !pkPet )
			{
				return NULL;
			}
			return dynamic_cast<CUnit*>(pkPet);
		}
	}
	return pkCaster;
}

bool PgAction_UseItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	////////////////////////////////////////
	// CAUTION :::::::::::::::::::::::::::
	// m_bRealAction == true �϶��� ������ �������� ȿ���� ������� �־�� �Ѵ�.
	// ���� : ������ �Ҹ�Ǵ� �������� �ƴѵ�, �Ҹ���������� ��ϵǾ� �ִ� ���� ��� �ִ�(��:Racipe, ���ν��� ��)
	// �� �����۵��� ���콺 ������ Ŭ������ �Ҹ����� ��, ������ �Ҹ���� �ʱ� ����, ������ �Ҹ𰡴��� ���������� �˻��ؾ� �Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	if ( !m_pkGround )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Ground is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(m_kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(pkCaster->IsAlive())
	{
		if(ITEM_USE_STATE_DEAD == pkDef->GetAbil(AT_ITEM_USE_STATUS))
		{//��� �������� �� �� ����.
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else
	{//��� �ߴٸ�.

		if(UICT_REVIVE != pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)) // ������ ���ڰ��� ����ڴ� ��� �־�� �ϰ� ȿ���� �޴� ����� �׾��־�� �Ѵ�.
		{
			if(ITEM_USE_STATE_DEAD != pkDef->GetAbil(AT_ITEM_USE_STATUS))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	if(true == pkDef->IsType(ITEM_TYPE_COOK))
	{
		PgAddAchievementValue kMA( AT_ACHIEVEMENT_EATER, 1, m_pkGround->GroundKey() );
		kMA.DoAction(pkCaster, NULL);
	}

	CUnit *pkTargetUnit = GetTargetUnit( pkDef, pkCaster, m_pkGround );
	if ( pkTargetUnit )
	{
		if ( true == DoEffect( pkDef, pkCaster, pkTargetUnit ) )
		{
			PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if ( pkPlayer )
			{
				ALRAM_MISSION::SCheckObejct< __int64 > kCheckObject( ALRAM_MISSION::OBJECT_ITEMNO, static_cast<__int64>(m_kItem.ItemNo()) );
				ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_USEITEM>()( pkPlayer, static_cast<int>(m_usUseCount), m_pkGround, kCheckObject );
			}
			return true;
		}
	}
	return false;
}

bool PgAction_UseItem::DoEffect( CItemDef const *pkDef, CUnit * pkCaster, CUnit * pkTarget )
{
	bool bResult = false;
	switch ( pkTarget->UnitType() )
	{
	case UT_PLAYER:
		{
			bResult = DoInstantEffect( pkDef, pkTarget, NULL );
		}break;
	case UT_PET:
		{
			bResult = DoInstantEffect_Pet( pkDef, pkCaster, pkTarget );
		}break;
	default:
		{
			return false;
		}break;
	}

	bResult |= DoMaintenanceEffect( pkDef, pkCaster, pkTarget );
	return bResult;
}

bool PgAction_UseItem::DoInstantEffect(CItemDef const* pkDef, CUnit* pkCaster, CUnit* pkTarget)
{
	int const iItemHP = pkDef->GetAbil(AT_HP);
	int const iItemMP = pkDef->GetAbil(AT_MP);
	int const iRateHP = pkDef->GetAbil(AT_R_MAX_HP);
	int const iRateMP = pkDef->GetAbil(AT_R_MAX_MP);
	if(	iItemHP	||	iItemMP || iRateHP || iRateMP)
	{
		int const iNowHP = pkCaster->GetAbil(AT_HP); 
		int const iNowMP = pkCaster->GetAbil(AT_MP);

		int const iMaxHP = pkCaster->GetAbil(AT_C_MAX_HP); 
		int const iMaxMP = pkCaster->GetAbil(AT_C_MAX_MP);

		int const iAddHP = iItemHP + (iItemHP * pkCaster->GetAbil(AT_C_HP_POTION_ADD_RATE) + iRateHP * iMaxHP) / ABILITY_RATE_VALUE;
		int const iAddMP = iItemMP + (iItemMP * pkCaster->GetAbil(AT_C_MP_POTION_ADD_RATE) + iRateMP * iMaxMP) / ABILITY_RATE_VALUE;

		int const iRetHP = std::min(iMaxHP, iAddHP+iNowHP);
		int const iRetMP = std::min(iMaxMP, iAddMP+iNowMP);

		if (iRetHP != iNowHP || iRetMP != iNowMP)
		{
			pkCaster->SetAbil(AT_HP,iRetHP,(iRetHP != iNowHP));
			pkCaster->SetAbil(AT_MP,iRetMP,(iRetMP != iNowMP));

			PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_POTION, 1, m_pkGround->GroundKey() );
			kMA.DoAction(pkCaster,NULL);
		}
	}

	int const iCustomType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
	int const iCustomValue3 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);

	switch(iCustomType)
	{
	case UICT_EVENT_ITEM_REWARD:
	case UICT_EVENT_ITEM_REWARD_GAMBLE:
		{
			PgAction_OpenEventItemReward kAction(m_pkGround->GroundKey(),pkDef->No());
			kAction.DoAction(pkCaster, pkTarget);
		}break;
	case UICT_COOLTIME_RATE_ITEM:
		{
			DWORD const dwCurTime = g_kEventView.GetServerElapsedTime();
			pkCaster->GetInven()->ReCalcCoolTime(dwCurTime, iCustomValue1);
		}break;
	case UICT_ADD_COOLTIME_ITEM:
		{
			DWORD const dwCurTime = g_kEventView.GetServerElapsedTime();
			pkCaster->GetInven()->ReCalcCoolTime(dwCurTime, iCustomValue1);
		}break;
	case UICT_COOLTIME_RATE_SKILL:
		{
			DWORD const dwCurTime = g_kEventView.GetServerElapsedTime();
			pkCaster->GetSkill()->ReCalcCoolTime(dwCurTime, iCustomValue1);
		}break;
	case UICT_ADD_COOLTIME_SKILL:
		{
			DWORD const dwCurTime = g_kEventView.GetServerElapsedTime();
			pkCaster->GetSkill()->ReCalcCoolTime(dwCurTime, iCustomValue1);
		}break;
	case UICT_CALL_STORE:
		{
			pkCaster->SetAbil(AT_CALL_STORE,1,true);

			PgStore kOutStore;
			GET_DEF(PgStoreMgr, kStoreMgr);
			BM::GUID kNpcGuid;
			if(S_OK == kStoreMgr.FindShopNpcGuid(iCustomValue1,kNpcGuid))
			{
				PgAction_ReqStoreItemList kAction(kNpcGuid);
				kAction.DoAction(pkCaster, NULL);
			}
		}break;
	case UICT_CALL_MARKET:
		{
			pkCaster->SetAbil(AT_CALL_MARKET,1,true);
		}break;
	case UICT_CALL_SAFE:
		{
			pkCaster->SetAbil(AT_CALL_SAFE,1,true);
		}break;
	case UICT_CALL_SHARE_SAFE:
		{
			pkCaster->SetAbil(AT_CALL_SHARE_SAFE,1,true);
		}break;
	case UICT_OPEN_CHEST:
		{
			PgAction_PopItemBag kPopItemBag(CIE_OpenChest, m_pkGround->GroundKey(),iCustomType, iCustomValue1, iCustomValue2, m_pkGround->GetEventAbil(), iCustomValue3);
			kPopItemBag.DoAction(pkCaster,NULL);
		}break;
	case UICT_OPEN_PACK:
		{
			PgAction_PopItemBag kPopItemBag(CIE_OpenPack, m_pkGround->GroundKey(),iCustomType, iCustomValue1, iCustomValue2, m_pkGround->GetEventAbil(), iCustomValue3);
			kPopItemBag.DoCreatePack(pkCaster);
		}break;
	case UICT_OPEN_PACK2:
		{
			PgAction_PopItemBag kPopItemBag(CIE_OpenPack2, m_pkGround->GroundKey(),iCustomType, iCustomValue1, iCustomValue2, m_pkGround->GetEventAbil(), iCustomValue3);
			kPopItemBag.DoCreatePack(pkCaster);
		}break;
	case UICT_LOCKED_CHEST:
		{
			PgAction_PopItemBag kPopItemBag(CIE_Locked_Chest, m_pkGround->GroundKey(),iCustomType, iCustomValue1, iCustomValue2, m_pkGround->GetEventAbil(), iCustomValue3);
			kPopItemBag.DoCreatePack(pkCaster);
		}break;
	case UICT_BONUS_RATE_BUF:
	case UICT_ADD_SP:
	case UICT_SAFEMODE:
	case UICT_RESET_SKILL_POINT:
		{
			return true;
		}break;
	case UICT_ELIXIR:
		{
			int iAbil = 0;
			int iNewValue = 0;
			m_kPacket.Pop(iAbil);
			m_kPacket.Pop(iNewValue);
			if ( (AT_HP != iAbil) || ( true == pkCaster->IsAlive() ) )
			{// ��¿�� ���� �׾��ָ� HP�� ä������ �ʴ¼��ۿ�...
				pkCaster->AddAbil(iAbil,iNewValue);
				pkCaster->SendAbil(static_cast<EAbilType>(iAbil));
			}
		}break;
	case UICT_SMS:
		{
			PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_SMS, 1, m_pkGround->GroundKey() );
			kMA.DoAction(pkCaster,NULL);

			SMS_HEADER kSMSHeader;
			kSMSHeader.eType = static_cast<eSMSType>(iCustomValue1);
			kSMSHeader.eCType = static_cast<eCardType>(iCustomValue2);
			kSMSHeader.kSenderName = pkCaster->Name();
			kSMSHeader.iUseItemNo = pkDef->No();

			m_kPacket.Pop(kSMSHeader.kMsg);
			
			PgPlayer const* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
			if(pkPlayer)
			{
				switch(iCustomValue2)
				{
				default:
				case CARD_FIRE:
					{
						std::wstring kToName,
									 kTitle;

						m_kPacket.Pop(kToName);
						m_kPacket.Pop(kTitle);

						kSMSHeader.kRecverName = kToName;
					}break;
				case CARD_POLORING:
				case CARD_LOVE:
					{
						std::wstring kToName,
									 kTitle;

						m_kPacket.Pop(kToName);
						m_kPacket.Pop(kTitle);

						kSMSHeader.kRecverName = kToName;

						SSendMailInfo kMailInfo;
						kMailInfo.FromGuid(pkCaster->GetID());
						kMailInfo.FromName(pkCaster->Name());
						kMailInfo.TargetName(kToName);
						kMailInfo.MailTitle(kTitle);
						kMailInfo.MailText(kSMSHeader.kMsg);
						kMailInfo.Money(0);
						kMailInfo.PaymentType(false);
						
						BM::Stream kPacket(PT_M_I_POST_SYSTEM_MAIL);
						kMailInfo.WriteToPacket(kPacket);

						SendToItem(m_pkGround->GroundKey(), kPacket);
					}break;
				}

				
				XUI::PgExtraDataPackInfo kSendExtraDataPackInfo;
				GetExtraDataPackInfo( const_cast<PgInventory*>(pkPlayer->GetInven()), &m_kPacket, kSendExtraDataPackInfo );

				switch(iCustomValue1)
				{
				case SMS_IN_GROUND:	// �ڽ��� ��ġ�� �ʿ��� ������.
					{
						BM::Stream kPacket(PT_T_C_NFY_BULLHORN);
						kSMSHeader.WriteToPacket(kPacket);
						kSendExtraDataPackInfo.PushToPacket(kPacket);

						SendToGround( m_pkGround->GroundKey(), kPacket );

						PgChatLogUtil::Log(ELogSub_Chat_Shout, pkPlayer, kSMSHeader.kMsg, m_pkGround->GetGroundNo() );
					}break;
				case SMS_IN_CHANNEL: // ü�� ��ü �ʿ� ������.
					{
						BM::Stream kPacket(PT_M_T_REQ_SMS);
						kSMSHeader.WriteToPacket(kPacket);
						kSendExtraDataPackInfo.PushToPacket(kPacket);

						SendToCenter(kPacket);

						PgChatLogUtil::Log(ELogSub_Chat_ShoutChannel, pkPlayer, kSMSHeader.kMsg, m_pkGround->GetGroundNo() );
					}break;
				case SMS_IN_REALM:	 // ���� ��ü �ʿ� ������.
					{
						BM::Stream kPacket(PT_M_T_REQ_SMS);
						kSMSHeader.WriteToPacket(kPacket);
						kSendExtraDataPackInfo.PushToPacket(kPacket);

						SendToItem( m_pkGround->GroundKey(), kPacket );
						PgChatLogUtil::Log(ELogSub_Chat_ShoutRealm, pkPlayer, kSMSHeader.kMsg, m_pkGround->GetGroundNo() );
					}break;
				}
			}
		}break;
	case UICT_PARTY:
		{
			PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
			if( pkPC &&	pkPC->PartyGuid() != BM::GUID::NullData() )
			{
				BM::Stream kPacket(PT_M_N_REQ_PARTY_COMMAND);
				kPacket.Push(pkCaster->GetID());
				kPacket.Push((BYTE)iCustomValue1);
				SendToGlobalPartyMgr(kPacket);
			}
		}break;
	case UICT_COUPLE:
		{
			PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_COUPLEWARP, 1, m_pkGround->GroundKey() );
			kMA.DoAction(pkCaster,NULL);
		}break;
	case UICT_REVIVE:
	case UICT_SUPER_GROUND_FEATHER:
		{
			if(false == pkCaster->IsAlive())	// �̹� �ٸ� ���� �������� ��� ���ٸ� ó�� �� �ʿ� ����... �����۸� ������?
			{
				int const iHPRate = pkDef->GetAbil(AT_R_MAX_HP_DEAD);
				int const iNewHP = pkCaster->GetAbil(AT_C_MAX_HP) * iHPRate / ABILITY_RATE_VALUE;

				int const iMPRate = pkDef->GetAbil(AT_R_MAX_MP_DEAD);
				int const iNewMP = pkCaster->GetAbil(AT_C_MAX_MP) * iMPRate / ABILITY_RATE_VALUE;

				int const iIsIgnoreDeathPanalty = pkDef->GetAbil(AT_IS_DEATH_PENALTY);
				
				switch( iCustomType )
				{
				case UICT_SUPER_GROUND_FEATHER:
					{
						BM::Stream kPacket(PT_U_G_RUN_ACTION);
						kPacket.Push( static_cast< short >(GAN_SuperGroundFeather) );
						kPacket.Push( iHPRate );
						kPacket.Push( iMPRate );
						pkCaster->VNotify(&kPacket);
					}break;
				case UICT_REVIVE:
				default:
					{
						PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
						if(pkPlayer)
						{
							pkPlayer->Alive(EALIVE_INSURANCE, E_SENDTYPE_BROADALL | E_SENDTYPE_MUSTSEND, iNewHP, iNewMP);

							if( m_pkGround->GetAttr() & GATTR_EVENT_GROUND )
							{
								PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(m_pkGround);
								if( pStaticGround && (pStaticGround->GetEventGroundState() & EGS_PLAY) )
								{
									pStaticGround->RevivePlayer();
								}
							}

							PgDeathPenalty kAction( m_pkGround->GroundKey(), LURT_Item, ((iIsIgnoreDeathPanalty)?false:true) );
							kAction.DoAction(pkPlayer, NULL);

							if( pkPlayer->HaveExpedition() )	// �����뿡 ���ԵǾ� �ִٸ�, �ڽ��� ���� ��ȭ�� �˷��� ��.
							{
								BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
								Packet.Push(pkPlayer->ExpeditionGuid());
								Packet.Push(pkPlayer->GetID());
								Packet.Push(pkPlayer->IsAlive());
								::SendToGlobalPartyMgr(Packet);
							}
						}
					}break;
				}

				PgAddAchievementValue kMA( AT_ACHIEVEMENT_IMMORTALITY, 1, m_pkGround->GroundKey() );
				kMA.DoAction(pkCaster,NULL);
			}
		}break;
	case UICT_AWAKE_INCREASE_ITEM:
		{
			if ( pkCaster->GetAbil(AT_ENABLE_AWAKE_SKILL) )
			{
				int const iAddValue = pkDef->GetAbil(AT_AWAKE_VALUE);
				int const iNowAwakeValue = pkCaster->GetAbil(AT_AWAKE_VALUE);
				int const iMaxAwakeValue = AWAKE_VALUE_MAX;

				int const iRetValue = std::min< int >( iNowAwakeValue + iAddValue, iMaxAwakeValue );

				if ( iRetValue != iNowAwakeValue )
				{
					pkCaster->SetAbil( AT_AWAKE_VALUE, iRetValue, true );
				}
			}
		}break;
	case UICT_SUPER_GROUND_MOVE_FLOOR:
		{
			BM::Stream kPacket(PT_U_G_RUN_ACTION);
			kPacket.Push( static_cast< short >(GAN_SuperGroundMoveFloor) );
			kPacket.Push( pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1) );
			pkCaster->VNotify(&kPacket);
		}break;
	case UICT_VENDOR:
		{
			pkCaster->SetAbil(AT_CALL_MARKET,1,true);
		}break;
	}

	return true;
}

bool PgAction_UseItem::DoInstantEffect_Pet(CItemDef const* pkDef, CUnit* pkCaster, CUnit* pkTargetPet )
{
	int const iItemMP = pkDef->GetAbil(AT_MP);
	int const iRateMP = pkDef->GetAbil(AT_R_MAX_MP);
	if(	iItemMP || iRateMP )
	{
		int const iNowMP = pkTargetPet->GetAbil(AT_MP);
		int const iMaxMP = pkTargetPet->GetAbil(AT_C_MAX_MP);
		int const iAddMP = iItemMP + (iItemMP * pkTargetPet->GetAbil(AT_C_MP_POTION_ADD_RATE) + iRateMP * iMaxMP) / ABILITY_RATE_VALUE;
		int const iRetMP = std::min(iMaxMP, iAddMP+iNowMP);

		if (iRetMP != iNowMP)
		{
			pkTargetPet->SetAbil(AT_MP,iRetMP,(iRetMP != iNowMP));

			PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_POTION, 1, m_pkGround->GroundKey() );
			kMA.DoAction( pkCaster, NULL );
		}
	}

	int const iCustomType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
	int const iCustomValue3 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);

	switch(iCustomType)
	{
	case UICT_ELIXIR:
		{
			int iAbil = 0;
			int iNewValue = 0;
			m_kPacket.Pop(iAbil);
			m_kPacket.Pop(iNewValue);
			if ( AT_HP != iAbil )
			{// ��¿�� ���� �׾��ָ� HP�� ä������ �ʴ¼��ۿ�...
				pkTargetPet->AddAbil(iAbil,iNewValue);
				pkTargetPet->SendAbil(static_cast<EAbilType>(iAbil));
			}
		}break;
	case UICT_PET_FEED:
		{
			if ( PET_FEED_MP_DRINK == iCustomValue2 )
			{
				int const iNowMP = pkTargetPet->GetAbil( AT_MP );
				int const iMaxMP = pkTargetPet->GetAbil( AT_C_MAX_MP );
				int const iNewMP = std::min( iNowMP + iCustomValue3, iMaxMP );
				pkTargetPet->SetAbil( AT_MP, iNewMP, true );
			}
		}break;
	}

	return true;
}

bool PgAction_UseItem::DoMaintenanceEffect(CItemDef const* pkDef, CUnit* pkCaster, CUnit* pkTarget)
{
	int const iNewEffect1 = pkDef->GetAbil(AT_EFFECTNUM1);
//	int const iNewEffect2 = pkDef->GetAbil(AT_EFFECTNUM2);
//	int const iNewEffect3 = pkDef->GetAbil(AT_EFFECTNUM3);
//	int const iNewEffect4 = pkDef->GetAbil(AT_EFFECTNUM4);

	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);

	SActArg kActArg;
	PgGroundUtil::SetActArgGround(kActArg, m_pkGround);
	kActArg.Set(ACTARG_ITEMNO, m_kItem.ItemNo());
	if(0 == m_kItem.ItemNo())
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"ItemNo : " << m_kItem.ItemNo());
	}

	if(iNewEffect1)
	{
		SEffectCreateInfo kCreate;
		kCreate.eType = EFFECT_TYPE_ITEM;
		kCreate.iEffectNum = iNewEffect1;
		kCreate.kActArg = kActArg;
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

		int const iEffectTimeType = pkDef->GetAbil( AT_DURATION_TIME_TYPE );
		if ( E_TIME_ELAPSED_TYPE_WORLDTIME == iEffectTimeType )
		{
			int const iDurTime = pkDef->GetAbil( AT_DURATION_TIME );

			SYSTEMTIME kLocalTime;
			g_kEventView.GetLocalTime(&kLocalTime);
			CGameTime::AddTime(kLocalTime, iDurTime * CGameTime::MILLISECOND );
			kCreate.kWorldExpireTime = BM::DBTIMESTAMP_EX(kLocalTime);
		}

		pkTarget->AddEffect(kCreate);
		::CheckSkillFilter_Delete_Effect(pkCaster, pkTarget, iNewEffect1);
	} 

	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect1);
	if (pkEffectDef)
	{
		if((0 < pkEffectDef->GetAbil(AT_HP)) || (0 < pkEffectDef->GetAbil(AT_MP)))
		{
			PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_POTION, 1, m_pkGround->GroundKey() );
			kMA.DoAction(pkCaster,NULL);
		}
	}

/*	if(iNewEffect2)
	{
		pkCaster->AddEffect((BYTE)EFFECT_TYPE_ITEM, iNewEffect2, 0, &kActArg, 0, 0);
		::CheckSkillFilter_Delete_Effect(pkCaster, pkCaster, iNewEffect2);
	}
	if(iNewEffect3)
	{
		pkCaster->AddEffect((BYTE)EFFECT_TYPE_ITEM, iNewEffect3, 0, &kActArg, 0, 0);
		::CheckSkillFilter_Delete_Effect(pkCaster, pkCaster, iNewEffect3);
	}
	if(iNewEffect4)
	{
		pkCaster->AddEffect((BYTE)EFFECT_TYPE_ITEM, iNewEffect4, 0, &kActArg, 0, 0);
		::CheckSkillFilter_Delete_Effect(pkCaster, pkCaster, iNewEffect4);
	}
*/		
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgRegQuickSlot
//////////////////////////////////////////////////////////////////////////////
PgRegQuickSlot::PgRegQuickSlot(size_t const slot_idx, SQuickInvInfo& kQuickInvInfo)
	:m_rkQuickInvInfo(kQuickInvInfo)
{
	m_slot_idx = slot_idx;
}

bool PgRegQuickSlot::DoAction(CUnit* pkUser, CUnit* pkNothing)
{
	if(!pkUser)
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; 
	}

	if(!pkUser->IsUnitType(UT_PLAYER))
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; 
	}

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUser);

	if(pkPlayer)
	{
		PgQuickInventory* pkQInven = pkPlayer->GetQInven();
		if(S_OK == pkQInven->Modify(m_slot_idx, m_rkQuickInvInfo))
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}



//////////////////////////////////////////////////////////////////////////////
//		PgRegQuickSlotViewPage
//////////////////////////////////////////////////////////////////////////////
PgRegQuickSlotViewPage::PgRegQuickSlotViewPage(char const cViewPage)
	:	m_cViewPage(cViewPage)
{
}

bool PgRegQuickSlotViewPage::DoAction(CUnit* pkUser, CUnit* pkNothing)
{
	if(!pkUser)
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; 
	}

	if(!pkUser->IsUnitType(UT_PLAYER))
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; 
	}

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUser);

	if(pkPlayer)
	{
		PgQuickInventory* pkQInven = pkPlayer->GetQInven();
		if(S_OK == pkQInven->Modify(m_cViewPage))
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

// �Ʋ����� �����ϴ� �˶��� ��� ���� üũ
bool IsAllNeedRepair( CUnit* pkCaster )
{
	GET_DEF( CItemDefMgr, kItemDefMgr );

	PgInventory* pkInv = pkCaster->GetInven();	
	CONT_HAVE_ITEM_DATA kFitCont;
	pkInv->GetItems( IT_FIT, kFitCont );	// ������ ��� ������	

	int iNeedRepairCount = 0;	
	CONT_HAVE_ITEM_DATA::iterator kFitItor = kFitCont.begin();
	while( kFitItor != kFitCont.end() )
	{
		PgBase_Item const kItem = (*kFitItor).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef( kItem.ItemNo() );
		EEquipPos const eEquipPos = (EEquipPos)pItemDef->EquipPos();
		
		switch( eEquipPos )
		{
		case EQUIP_POS_HELMET:		// ����
		case EQUIP_POS_SHOULDER:	// ���
		case EQUIP_POS_SHIRTS:		// ����
		case EQUIP_POS_PANTS:		// ����
		case EQUIP_POS_GLOVE:		// �Ź�
		case EQUIP_POS_BOOTS:		// �尩
		case EQUIP_POS_WEAPON:		// ����
			{
				if( !kItem.EnchantInfo().IsNeedRepair() )
				{// �ϳ��� �����䱸 �÷��װ� ���������� ���� �޼� ���� �ȵ�
					return false;
				}
				else
				{
					++iNeedRepairCount;
				}

			}break;
		default:
			{
				// �׿� �������� ������ ������ ��� ����.
			}break;
		}
		
		++kFitItor;
	}

	return ( iNeedRepairCount > 6 );
}

//////////////////////////////////////////////////////////////////////////////
//		PgDeathPenalty : Adapt Death Penalty (Decrease EXP) to Player
//////////////////////////////////////////////////////////////////////////////
PgDeathPenalty::PgDeathPenalty(SGroundKey const& kGndKey, ELogUserResurrectType const eResurrectType, bool const bIsPenalty)
	:m_kGndKey(kGndKey), m_eResurrectType(eResurrectType), m_bIsPenalty(bIsPenalty)
{
}

void PgDeathPenalty::ProcessEquipPenalty(CUnit* pkCaster, CONT_PLAYER_MODIFY_ORDER &kOrder)
{
	if(pkCaster->GetAbil(AT_PROTECTION_GALL))
	{
		return;// ������ �Ҹ� ���� ����� ������ ���⼭ ����
	}

	PgInventory *pkInv = pkCaster->GetInven();
	static int const iCandiSize = sizeof(g_aiDeathPenaltyCandidate)/sizeof(int);

	GET_DEF(CItemDefMgr, kItemDefMgr);
	
	int iNeedRepairCount = 0;
	int iIndex = iCandiSize;
	while(iIndex)
	{
		--iIndex;//�ε��� ���� ���̹Ƿ�. ���� ���� �Ұ�

		PgBase_Item kItem;
		SItemPos kItemPos(IT_FIT, g_aiDeathPenaltyCandidate[iIndex]);
		if(S_OK == pkInv->GetItem(kItemPos, kItem))
		{
			SEnchantInfo kEnchantInfo = kItem.EnchantInfo();

			CItemDef const* pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
			if(	pkItemDef 
			&&	!pkItemDef->IsAmountItem() 
			&&	!kItem.EnchantInfo().IsNeedRepair())
			{
				WORD const wDecCount = static_cast<WORD const>((float)(pkItemDef->MaxAmount()*(10+BM::Rand_Range(3, -3))) /(float)100);//15~25%
		//		WORD const wDecCount = static_cast<WORD const>((float)(pkItemDef->GetAbil(AT_DEFAULT_AMOUNT)*(100)) /(float)100);//100% �׽�Ʈ��.
				WORD const wNowCount = kItem.Count();
				if(wNowCount > wDecCount)
				{//Now �� ������ �ܼ� ����.-> - �Ǿ �����÷ο� ������.
					SPMOD_Modify_Count kDelData(kItem, kItemPos, -wDecCount);//��.
					
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_PENALTY, pkCaster->GetID(), kDelData);
					kOrder.push_back(kIMO);
				}
				else if( wNowCount <= wDecCount && JobSkillToolUtil::GetToolGatherType(kItem.ItemNo()))
				{//ä�������ε� ���� �������� 0���Ϸ� �������� ������ ����
					SPMOD_Modify_Count kDelData(kItem, kItemPos, 0, true);//.
					
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_PENALTY, pkCaster->GetID(), kDelData);
					kOrder.push_back(kIMO);
					pkCaster->SendWarnMessage(25012);
				}
				else
				{//�Ѿ��. NeedRepair ���·� ��ȯ. ������ �� �ʿ�� ����.
					kEnchantInfo.IsNeedRepair(true);
					SPMOD_Enchant kSPMOEnchant(kItem, kItemPos, kEnchantInfo);

					SPMO kIMO(IMET_MODIFY_ENCHANT|IMC_DEC_DUR_BY_PENALTY, pkCaster->GetID(), kSPMOEnchant);//
					kOrder.push_back(kIMO);
				}
			}

			EEquipPos const eEquipPos = (EEquipPos)pkItemDef->EquipPos();
			switch( eEquipPos )
			{
			case EQUIP_POS_HELMET:		// ����
			case EQUIP_POS_SHOULDER:	// ���
			case EQUIP_POS_SHIRTS:		// ����
			case EQUIP_POS_PANTS:		// ����
			case EQUIP_POS_GLOVE:		// �Ź�
			case EQUIP_POS_BOOTS:		// �尩
			case EQUIP_POS_WEAPON:		// ����
				{
					if( kEnchantInfo.IsNeedRepair() )
					{
						++iNeedRepairCount;
					}
				}break;
			default:
				{// �׿� �������� ������ ������ ��� ����.
				}break;
			}
		}
	}

	{//�Ʋ����� �����ϴ� �˶��� ��� ���� üũ

		if( iNeedRepairCount > 6 )
		{
			PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_DURABLENESS_ZERO, 0, m_kGndKey );
			kSA.DoAction( pkCaster, NULL );
		}
	}
}


bool PgDeathPenalty::DoAction(CUnit* pkUnit, CUnit* pkNothing)
{
	if( !pkUnit )
	{
		return false;
	}

//	INFO_LOG(BM::LOG_LV9, __FL__ << _T("ID=") << pkUnit->Name());
//	if (pkUnit->GetState() == US_DEAD)//���̵��� ��� dead ���°� �ƴ� ���� �ֱ� ������.
	__int64 i64Exp = pkUnit->GetAbil64(AT_EXPERIENCE);
	__int64 i64NewExp = i64Exp;

	if(m_bIsPenalty)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		int const iClass = pkUnit->GetAbil(AT_CLASS);
		int const iLevel = pkUnit->GetAbil(AT_LEVEL);

		GET_DEF(PgClassDefMgr, kClassDefMgr);
		
		int iPer = kClassDefMgr.GetAbil(SClassKey(iClass, iLevel), AT_DEATH_EXP_PENALTY);
		
		__int64 i64MinExp = kClassDefMgr.GetAbil64(SClassKey(iClass, iLevel), AT_EXPERIENCE);
		__int64 i64MaxExp = kClassDefMgr.GetAbil64(SClassKey(iClass, iLevel+1), AT_EXPERIENCE);
		i64Exp = pkUnit->GetAbil64(AT_EXPERIENCE);
		i64NewExp = __max(i64MinExp, i64Exp - (i64MaxExp - i64MinExp) * iPer / ABILITY_RATE_VALUE);
		//	INFO_LOG(BM::LOG_LV6, __FL__ << L"[DeathPenalty] [" << pkUnit->Name() << L"-" << pkUnit->GetID() << L"] OldExp[" << i64Exp << L"] --> NewExp[" << i64NewExp << L"]");
		if(i64NewExp != i64Exp)
		{
			__int64 const iDecExp = i64NewExp - i64Exp;
			SPMOD_AddAbil kAddExpData(AT_EXPERIENCE, iDecExp);
			kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkUnit->GetID(), kAddExpData));

			__int64 const i64NewExp = std::max(pkUnit->GetAbil64(AT_EXPERIENCE) + iDecExp,0i64);
			__int64 const i64CurRestExp = std::max(pkUnit->GetAbil64(AT_REST_EXP_ADD_MAX) - pkUnit->GetAbil64(AT_EXPERIENCE), 0i64);
			__int64 const i64NewRestExp = i64NewExp + i64CurRestExp;

			SPMOD_AddAbil kAddRestData(AT_REST_EXP_ADD_MAX, iDecExp);
			kOrder.push_back(SPMO(IMET_ADD_ABIL64_MAPUSE, pkUnit->GetID(), kAddRestData));
		}

		ProcessEquipPenalty(pkUnit, kOrder);// ����� ������ �г�Ƽ.

		PgAction_ReqModifyItem kItemModifyAction(IMEPT_DEATHPENALTY, m_kGndKey, kOrder, BM::Stream(), true);
		kItemModifyAction.DoAction(pkUnit, NULL);
	}

	DoDefDeathPenalty(pkUnit);

	PgPlayerLogUtil::AliveLog(dynamic_cast< PgPlayer* >( pkUnit ), m_eResurrectType, m_kGndKey, std::wstring(), BM::GUID::NullData(), i64Exp, i64NewExp);

	CONT_DEFMAP const* pkDefMap = NULL;	
	g_kTblDataMgr.GetContDef(pkDefMap);
	CONT_DEFMAP::const_iterator itor_map = pkDefMap->find(m_kGndKey.GroundNo());
	
	if (itor_map!=pkDefMap->end())
	{
		if((*itor_map).second.iAttr & GATTR_FLAG_BATTLESQUARE)
		{
			return true;	//��Ʋ������, ������� ��� ���� ����Ʈ ���� ����
		}
	}

	SEffectCreateInfo kCreate;
	kCreate.eType = EFFECT_TYPE_PENALTY;
	kCreate.iEffectNum = EFFECTNO_CANNOT_DAMAGE;
	kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
	pkUnit->AddEffect( kCreate);//���� ����Ʈ�� �ɾ���

	return true;
}

void PgDeathPenalty::DoDefDeathPenalty(CUnit* pkCaster)
{
	int iGroundAttr = 0;
	{
		CONT_DEFMAP const* pkDefMap = NULL;	
		g_kTblDataMgr.GetContDef(pkDefMap);
		CONT_DEFMAP::const_iterator itor_map = pkDefMap->find(m_kGndKey.GroundNo());
		if (itor_map == pkDefMap->end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot find DefMap MapNo=") << m_kGndKey.GroundNo());
			return;
		}
		iGroundAttr = (*itor_map).second.iAttr;
	}

	// TB_DefDeathPenalty �����ϱ�
	CONT_DEFDEATHPENALTY const* pkDefDeathPenalty = NULL;
	g_kTblDataMgr.GetContDef(pkDefDeathPenalty);
	
	{
		// Ground = 0
		CONT_DEFDEATHPENALTY::const_iterator itor_zero = pkDefDeathPenalty->find(0);
		if (itor_zero != pkDefDeathPenalty->end())
		{
			VEC_DEFDEATHPENALTY const& kPenaltyVec = itor_zero->second;
			VEC_DEFDEATHPENALTY::const_iterator itor_penalty = kPenaltyVec.begin();
			while (itor_penalty != kPenaltyVec.end())
			{
				if (CheckPenalty(pkCaster, *itor_penalty, iGroundAttr))
				{
					DoPenalty(pkCaster, *itor_penalty);
				}
				++itor_penalty;
			}
		}
	}

	{
		// Ground No
		CONT_DEFDEATHPENALTY::const_iterator itor_zero = pkDefDeathPenalty->find(m_kGndKey.GroundNo());
		if (itor_zero != pkDefDeathPenalty->end())
		{
			VEC_DEFDEATHPENALTY const& kPenaltyVec = itor_zero->second;
			VEC_DEFDEATHPENALTY::const_iterator itor_penalty = kPenaltyVec.begin();
			while (itor_penalty != kPenaltyVec.end())
			{
				if (CheckPenalty(pkCaster, *itor_penalty, iGroundAttr))
				{
					DoPenalty(pkCaster, *itor_penalty);
				}
				++itor_penalty;
			}
		}
	}

}

bool PgDeathPenalty::CheckPenalty(CUnit* pkCaster, VEC_DEFDEATHPENALTY::value_type const& rkPenalty, int const iGroundAttr)
{
	if (rkPenalty.iGroundAttr != 0 && rkPenalty.iGroundAttr != iGroundAttr)
	{
		return false;
	}
	if (rkPenalty.iGroundNo != 0 && rkPenalty.iGroundNo != m_kGndKey.GroundNo())
	{
		return false;
	}

	if (rkPenalty.sLevelMin != 0 || rkPenalty.sLevelMax != 0)
	{
		int const iLevel = pkCaster->GetAbil(AT_LEVEL);
		if (rkPenalty.sLevelMin > iLevel || rkPenalty.sLevelMax < iLevel)
		{
			return false;
		}
	}

	if (rkPenalty.bySuccessionalDeath != 0 && rkPenalty.bySuccessionalDeath != pkCaster->GetAbil(AT_SUCCESSIONAL_DEATH_COUNT))
	{
		return false;
	}

	return true;
}

void PgDeathPenalty::DoPenalty(CUnit* pkCaster, VEC_DEFDEATHPENALTY::value_type const& rkPenalty)
{
	for (int i=0; i<TBL_DEFDEATHPENALTY::MAX_DEATH_PENALTY_EFFECTSIZE; i++)
	{
		if (0 != rkPenalty.iEffect[i])
		{
			SEffectCreateInfo kCreate;
			kCreate.eType = EFFECT_TYPE_NORMAL;
			kCreate.iEffectNum = rkPenalty.iEffect[i];
			kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
			pkCaster->AddEffect(kCreate);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//		PgThrowUpPenalty : Adapt ThrowUp Penalty (HP=1,MP=1,EXP RATE) to Player
//////////////////////////////////////////////////////////////////////////////
PgThrowUpPenalty::PgThrowUpPenalty(SGroundKey const &kGndKey, bool const bMissionOutMove)// �̼� ���� �г�Ƽ.
	:m_kGndKey(kGndKey)
	,m_bMissionOutMapMove(bMissionOutMove)
{
}

bool PgThrowUpPenalty::DoAction(CUnit* pkUser, int const iExpRate)
{
	if ( !pkUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// ���� �����̸� HP/MP�г�Ƽ���ش�
	if( true == pkUser->IsDead() )
	{//�̼ǿ��� ������ ���� ��Ȱ�� ��Ű�� ������ ���̶�. ���������� �г�Ƽ�� ���⼭ ����.
		if (m_bMissionOutMapMove)
		{
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUser);
			pkPlayer->Alive(EALIVE_NONE, E_SENDTYPE_BROADALL | E_SENDTYPE_MUSTSEND, pkUser->GetAbil(AT_C_MAX_HP)/2, pkUser->GetAbil(AT_C_MAX_MP)/2);//�پ˷�����

			if( pkPlayer->HaveExpedition() )	// �����뿡 ���ԵǾ� �ִٸ�, �ڽ��� ���� ��ȭ�� �˷��� ��.
			{
				BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
				Packet.Push(pkPlayer->ExpeditionGuid());
				Packet.Push(pkPlayer->GetID());
				Packet.Push(pkPlayer->IsAlive());
				::SendToGlobalPartyMgr(Packet);
			}

			PgDeathPenalty kAction(m_kGndKey, LURT_MapMove, true);//��Ե� �����°� �г�Ƽ�� ����.
			kAction.DoAction(pkUser, NULL);
		}
		else
		{
			// Mission���� �׾��µ�, �ٷ� Logout ����.
			// Penalty�� �ٷ� ������ �� ����, Penalty�� �־�� �Ѵٰ� ǥ�ø� �Ѵ�.
			//	���� : Penalty������ ContentsServer�� �޾��� ��, player������ �̹� ������� ��찡 �ִ�.
			pkUser->SetAbil(AT_HP, HP_VALUE_MISSION_MAP_PENALTY);
		}
	}
	else
	{//��� �ƴ� ����. ��� �ƴϴ� ��� �г�Ƽ ����.
		SEffectCreateInfo kCreate;
		kCreate.eType = EFFECT_TYPE_PENALTY;
		kCreate.iEffectNum = ms_iEffectNo;
		kCreate.iValue = iExpRate;
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;	
		pkUser->AddEffect( kCreate );// �г�Ƽ ����Ʈ�� ���̰�. 

		int iValue = 0;
		g_kVariableContainer.Get( EVar_Kind_Mission, EVar_Mission_ThrowUpPenaltyCount, iValue );

		if ( iValue > 0 )
		{
			WORD const kAbilType = AT_MISSION_THROWUP_PENALTY;

			// �̼��� ���������� ���� �г�Ƽ�� �־�� �Ѵ�.
			int iAbil = pkUser->GetAbil( kAbilType );
			++iAbil;
			if ( iValue <= iAbil )
			{
				iAbil = INT_MAX;
				pkUser->SetAbil( kAbilType, iAbil );

				int iPenaltyTimeSec = 0;
				g_kVariableContainer.Get( EVar_Kind_Mission, EVar_Mission_ThrowUpPenaltyTime, iPenaltyTimeSec );

				if ( iPenaltyTimeSec > 0 )
				{
					// �г�Ƽ ������
					BM::Stream kPenalty( PT_M_N_REQ_SAVE_PENALTY, pkUser->GetID() );
					kPenalty.Push( kAbilType );
					kPenalty.Push( iAbil );
					kPenalty.Push( iPenaltyTimeSec );
					::SendToContents( kPenalty );
				}
			}
			else
			{
				pkUser->SetAbil( kAbilType, iAbil );
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqUseItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqUseItem::PgAction_ReqUseItem(SItemPos const &kCasterPos, PgGround* const pkGround, bool const bAnsServerCheck, BM::Stream &rkOrgPacket, BM::Stream const &kAddonPacket)
	:m_kCasterPos(kCasterPos), m_pkGround(pkGround), m_bAnsServerCheck(bAnsServerCheck), m_kOrgPacket(rkOrgPacket), m_kAddonPacket(kAddonPacket),m_kCause(MIE_UseItem)
{
	if ( pkGround != NULL )
	{
		m_kGndKey = pkGround->GroundKey();
		m_kGndAttr = pkGround->GetAttr();
	}
	m_dwCoolTime = 0;
}

inline int GetDefSkillSetItemLimit(int const iLevel)
{
	const CONT_DEFSKILLSET *pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);
	if(pkContDefMap)
	{
		int iUseLimit = 0;
		CONT_DEFSKILLSET::const_iterator c_iter=pkContDefMap->begin();
		while(c_iter!=pkContDefMap->end())
		{
			if(SSCT_ITEM==c_iter->second.byConditionType)
			{
				++iUseLimit;
			}
			else if(SSCT_LEVEL==c_iter->second.byConditionType)
			{
				if(iLevel>=c_iter->second.byConditionValue)
				{
					++iUseLimit;
				}
			}
			++c_iter;
		}
		return iUseLimit;
	}
	return 0;
}

bool PgAction_ReqUseItem::CheckItemEffect( CItemDef const *pkItemDef, CUnit* pkItemOwner, CUnit* pkTargetUnit, PgBase_Item const & kItem, DWORD & Result )
{
	switch ( pkTargetUnit->UnitType() )
	{
	case UT_PLAYER:
		{	
			return CheckItemEffect_Player( pkItemDef, pkTargetUnit, kItem, Result );
		}break;
	case UT_PET:
		{
			PgPet * pkPet = dynamic_cast<PgPet*>(pkTargetUnit);
			if ( pkPet )
			{
				return CheckItemEffect_Pet( pkItemDef, pkItemOwner, pkPet, kItem );
			}
		}break;
	}

	return false;
}

bool PgAction_ReqUseItem::CheckItemEffect_Player(CItemDef const* const pkDef, CUnit* pkCaster, PgBase_Item const & kItem, DWORD & Result)
{
	int iUseEffectCount = 0;

	int const iItemHP = pkDef->GetAbil(AT_HP);
	int const iItemMP = pkDef->GetAbil(AT_MP);
	int const iRateHP = pkDef->GetAbil(AT_R_MAX_HP);
	int const iRateMP = pkDef->GetAbil(AT_R_MAX_MP);
	if(	iItemHP	||	iItemMP || iRateHP || iRateMP)
	{
		int const iNowHP = pkCaster->GetAbil(AT_HP); 
		int const iNowMP = pkCaster->GetAbil(AT_MP);

		int const iMaxHP = pkCaster->GetAbil(AT_C_MAX_HP); 
		int const iMaxMP = pkCaster->GetAbil(AT_C_MAX_MP);

		int const iAddHP = iItemHP + (iItemHP * pkCaster->GetAbil(AT_C_HP_POTION_ADD_RATE) + iRateHP * iMaxHP) / ABILITY_RATE_VALUE;
		int const iAddMP = iItemMP + (iItemMP * pkCaster->GetAbil(AT_C_MP_POTION_ADD_RATE) + iRateMP * iMaxMP) / ABILITY_RATE_VALUE;

		int const iRetHP = std::min(iMaxHP, iAddHP+iNowHP);
		int const iRetMP = std::min(iMaxMP, iAddMP+iNowMP);

		if (iRetHP != iNowHP || iRetMP != iNowMP)
		{
			++iUseEffectCount;
		}
		else
		{
			if( iItemHP || iRateHP )
			{
				if( iRetHP == iNowHP )
				{
					Result |= CIEffectResult::CIER_FULL_HP;
				}
			}
			if( iItemMP || iRateMP )
			{
				if( iRetMP == iNowMP )
				{
					Result |= CIEffectResult::CIER_FULL_MP;
				}
			}
		}
	}

	{
		int const iRecoveryJSExhaustionRate = pkDef->GetAbil(AT_JS_RECOVERY_EXHAUSTION_R);
		int const iRecoveryJSExhaustionValue = pkDef->GetAbil(AT_JS_RECOVERY_EXHAUSTION_C);
		int const iRecoveryJSBlessPointRate = pkDef->GetAbil(AT_JS_RECOVERY_BLESSPOINT_R);
		int const iRecoveryJSBlessPointValue = pkDef->GetAbil(AT_JS_RECOVERY_BLESSPOINT_C);
		int const iRecoveryJSProductPointRate = pkDef->GetAbil(AT_JS_RECOVERY_PRODUCTPOINT_R);
		int const iRecoveryJSProductPointValue = pkDef->GetAbil(AT_JS_RECOVERY_PRODUCTPOINT_C);
		if( iRecoveryJSExhaustionRate
		||	iRecoveryJSExhaustionValue
		||	iRecoveryJSBlessPointRate
		||	iRecoveryJSBlessPointValue
		||	iRecoveryJSProductPointRate
		||	iRecoveryJSProductPointValue )
		{
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
			if( pkPlayer )
			{
				if( iRecoveryJSExhaustionRate || iRecoveryJSExhaustionValue )
				{
					int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1�� �� ��ų�� �Ƿε��� �ִ�
					int iAddValue = (iMaxExhaustion * iRecoveryJSExhaustionRate / ABILITY_RATE_VALUE) + iRecoveryJSExhaustionValue;
					if( S_PST_JS1_RecoveryExhaustion const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_JS1_RecoveryExhaustion>() )
					{
						iAddValue += SRateControl::GetValueRate(iAddValue, pkPremium->iRate);
					}
					if( 0 < iMaxExhaustion
					&&	0 < pkPlayer->JobSkillExpertness().CurExhaustion() )
					{
						SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pkCaster->GetID(), SPMOD_JobSkillExhaustion(-iAddValue)); // �Ƿε� ȸ��
						m_kOrder.push_back( kIMO );
						++iUseEffectCount;
					}
					else
					{
						int const iErrorMessage = 25028;
						pkCaster->SendWarnMessage(iErrorMessage);
					}
				}
				if( iRecoveryJSBlessPointRate || iRecoveryJSBlessPointValue )
				{
					int const iMaxBlessPoint = JobSkillExpertnessUtil::GetMaxExhaustion_2ND(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 2�� �� ��ų�� �ູ�������� �ִ�
					int const iAddValue = (iMaxBlessPoint * iRecoveryJSBlessPointRate / ABILITY_RATE_VALUE) + iRecoveryJSBlessPointValue;
					if( 0 < iMaxBlessPoint
					&&	0 < pkPlayer->JobSkillExpertness().CurBlessPoint() )
					{
						SPMO kIMO(IMET_JOBSKILL_ADD_BLESSPOINT, pkCaster->GetID(), SPMOD_JobSkillExhaustion(-iAddValue)); // �Ƿε� ȸ��
						m_kOrder.push_back( kIMO );
						++iUseEffectCount;
					}
					else
					{
						int const iErrorMessage = 25029;
						pkCaster->SendWarnMessage(iErrorMessage);
					}
				}
				if( iRecoveryJSProductPointRate || iRecoveryJSProductPointValue )
				{//�����->�Ƿε��� ȸ���ǵ���			
					int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1�� �� ��ų�� �Ƿε��� �ִ�
					int iAddValue = (iMaxExhaustion * iRecoveryJSProductPointRate / ABILITY_RATE_VALUE) + iRecoveryJSProductPointValue;
					if( S_PST_JS1_RecoveryExhaustion const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_JS1_RecoveryExhaustion>() )
					{
						iAddValue += SRateControl::GetValueRate(iAddValue, pkPremium->iRate);
					}
					if( 0 < iMaxExhaustion
					&&	0 < pkPlayer->JobSkillExpertness().CurExhaustion() )
					{
						SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pkCaster->GetID(), SPMOD_JobSkillExhaustion(-iAddValue)); // �Ƿε� ȸ��
						m_kOrder.push_back( kIMO );
						++iUseEffectCount;
					}
					else
					{
						int const iErrorMessage = 25028;
						pkCaster->SendWarnMessage(iErrorMessage);
					}
				}
			}
		}
	}

	int const iCustomType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
	int const iCustomValue3 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);
	
	switch(iCustomType)
	{
	case UICT_RESET_ATTACHED:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);

			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_RESET_ATTACHED_NOT_FOUND_ITEM);
				return false;
			}

			if(false == kTargetItem.EnchantInfo().IsAttached())
			{
				Ret(E_RESET_ATTACHED_NOT_ATTACHED);
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
			if(NULL == pItemDef)
			{
				Ret(E_RESET_ATTACHED_NOT_FOUND_ITEM);
				return false;
			}

			if( (iCustomValue1 > pItemDef->GetAbil(AT_COSTUME_GRADE)) ||
				(iCustomValue2 < pItemDef->GetAbil(AT_COSTUME_GRADE)) ||
				(true == kTargetItem.EnchantInfo().IsBinding()))
			{
				Ret(E_RESET_ATTACHED_CANT_RESET);
				return false;
			}
			
			SEnchantInfo kEnchant = kTargetItem.EnchantInfo();
			kEnchant.IsAttached(0);

			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant( kTargetItem, kTargetPos, kEnchant)));
			++iUseEffectCount;
		}break;
	case UICT_SKILL_EXTEND:
		{
			int iSkillNo;
			m_kOrgPacket.Pop(iSkillNo);

			PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if(!pkPlayer)
			{
				Ret(E_SKILLEXTEND_INVALID_EXTEND);
				return false;
			}

			if(iCustomValue2 <= pkPlayer->GetMySkill()->GetExtendLevel(iSkillNo))
			{
				Ret(E_SKILLEXTEND_CANT_EXTEND_LEVEL);
				return false;
			}

			CONT_DEFSKILLEXTENDITEM const * pkContSkillExtend = NULL;
			g_kTblDataMgr.GetContDef(pkContSkillExtend);
			if(!pkContSkillExtend || 0 == iCustomValue2)
			{
				Ret(E_SKILLEXTEND_INVALID_EXTEND);
				return false;
			}

			CONT_DEFSKILLEXTENDITEM::const_iterator iter = pkContSkillExtend->find(iCustomValue1);
			if(iter == pkContSkillExtend->end())
			{
				Ret(E_SKILLEXTEND_INVALID_EXTEND);
				return false;
			}

			CONT_EXTEND_SKILLSET::const_iterator set_iter = (*iter).second.kCont.find(iSkillNo);
			if(set_iter == (*iter).second.kCont.end())
			{
				Ret(E_SKILLEXTEND_INVALID_EXTEND);
				return false;
			}

			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);			
			if (NULL == pkSkillDef)
			{
				Ret(E_SKILLEXTEND_INVALID_SKILL);
				return false;
			}

			__int64 const i64ClassLimit = pkSkillDef->GetAbil64(AT_CLASSLIMIT);
			if(!IS_CLASS_LIMIT(i64ClassLimit, pkCaster->GetAbil(AT_CLASS)))
			{
				Ret(E_SKILLEXTEND_INVALID_CLASSLIMIT);
				return false;
			}

			m_kOrder.push_back(SPMO(IMET_SET_SKILLEXTEND, pkCaster->GetID(), SPMOD_SetSkillExtend( iSkillNo, static_cast<BYTE>(iCustomValue2))));
			++iUseEffectCount;
		}break;
	case UICT_STRATEGYSKILL_OPEN:
		{
			PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if(!pkPlayer)
			{
				return false;
			}

			if(pkPlayer->IsOpenStrategySkill(ESTT_SECOND))
			{
				Ret(E_STRATEGYSKILL_ALREADY_OPEN);
				return false;
			}

			SPMOD_AddAbil kSetData(AT_STRATEGYSKILL_OPEN, ESTOT_SECOND);
			m_kOrder.push_back(SPMO(IMET_SET_ABIL, pkCaster->GetID(), kSetData));

			++iUseEffectCount;
		}break;
	case UICT_UPGRADE_OPTION:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);

			int iOptIdx = 0;
			m_kOrgPacket.Pop(iOptIdx);

			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_UPGRADE_OPTION_NOT_FOUND_ITEM);
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
			if(NULL == pItemDef)
			{
				Ret(E_UPGRADE_OPTION_NOT_FOUND_ITEM);
				return false;
			}

			if(false == pItemDef->CanEquip())
			{
				Ret(E_UPGRADE_OPTION_NOT_EQUIP_ITEM);
				return false;
			}
			
			int const iMaxOptLevel = std::max<int>(pItemDef->GetAbil(AT_LEVELLIMIT)/5 + 6, MAX_DICE_ITEM_OPTION_LEVEL);

			int iMinOptLevel = 1;
			int iOptType = 0;

			switch(iOptIdx)
			{
			case 0:
				{
					iMinOptLevel = kTargetItem.EnchantInfo().BasicLv1();
					iOptType = kTargetItem.EnchantInfo().BasicType1();
				}break;
			case 1:
				{
					iMinOptLevel = kTargetItem.EnchantInfo().BasicLv2();
					iOptType = kTargetItem.EnchantInfo().BasicType2();
				}break;
			case 2:
				{
					iMinOptLevel = kTargetItem.EnchantInfo().BasicLv3();
					iOptType = kTargetItem.EnchantInfo().BasicType3();
				}break;
			case 3:
				{
					iMinOptLevel = kTargetItem.EnchantInfo().BasicLv4();
					iOptType = kTargetItem.EnchantInfo().BasicType4();
				}break;
			}

			if(0 == iOptType)
			{
				Ret(E_UPGRADE_OPTION_NOT_HAVE_OPTION);
				return false;
			}

			if(	true == kTargetItem.EnchantInfo().IsCurse() 
				|| true == kTargetItem.EnchantInfo().IsBinding() 
				|| true == kTargetItem.EnchantInfo().IsSeal())
			{
				Ret(E_UPGRADE_OPTION_CANT_UPGRADE_ITEM);
				return false;
			}

			size_t iNewLevel = 0;
			if(false == RouletteRate(static_cast<int>(DICE_ITEM_OPTION_BASE + iMinOptLevel), iNewLevel, MAX_ITEM_CONTAINER_LIST, 0))
			{
				Ret(E_UPGRADE_OPTION_CANT_UPGRADE_ITEM);
				return false;
			}

			SEnchantInfo kEnchant = kTargetItem.EnchantInfo();
			iNewLevel = std::min<int>(iMinOptLevel + iNewLevel, MAX_DICE_ITEM_OPTION_LEVEL);

			switch(iOptIdx)
			{
			case 0:{kEnchant.BasicLv1(iNewLevel);}break;
			case 1:{kEnchant.BasicLv2(iNewLevel);}break;
			case 2:{kEnchant.BasicLv3(iNewLevel);}break;
			case 3:{kEnchant.BasicLv4(iNewLevel);}break;
			}
			
			m_kAddonPacket.Push(iOptIdx);
			m_kAddonPacket.Push(iNewLevel);

			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant( kTargetItem, kTargetPos, kEnchant)));
			++iUseEffectCount;
		}break;
	case UICT_REDICE_PET_OPTION:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);

			PgBase_Item kTargetItem;
			if( S_OK != pkCaster->GetInven()->GetItem( kTargetPos, kTargetItem ) )
			{
				Ret(E_REDICE_NOT_FOUND_PET);
				return false;
			}

			PgItem_PetInfo *pkPetInfo = NULL;
			if ( !kTargetItem.GetExtInfo( pkPetInfo ) )
			{
				Ret(E_REDICE_NOT_FOUND_PET);
				return false;
			}

			GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
			PgClassPetDef kPetDef;
			if ( !kClassPetDefMgr.GetDef( pkPetInfo->ClassKey(), &kPetDef ) )
			{
				Ret(E_REDICE_NOT_FOUND_PET);
				return false;
			}

			if ( EPET_TYPE_1 != kPetDef.GetPetType() && EPET_TYPE_3 != kPetDef.GetPetType() )
			{
				Ret(E_REDICE_CANT_GENOPT_PET);
				return false;
			}

			SEnchantInfo kNewEnchantInfo;

			GET_DEF(PgPetHatchMgr, kPetHatchMgr);
			kPetHatchMgr.RedicePetOption(kTargetItem,kNewEnchantInfo);
			m_kAddonPacket.Push(kNewEnchantInfo);

			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant( kTargetItem, kTargetPos, kNewEnchantInfo)));
			++iUseEffectCount;
		}break;
	case UICT_REDICE_ITEM_OPTION:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_REDICE_NOT_FOUND_ITEM);
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
			if(NULL == pItemDef)
			{
				Ret(E_REDICE_NOT_FOUND_ITEM);
				return false;
			}

			if(false == pItemDef->CanEquip())
			{
				Ret(E_REDICE_NOT_EQUIP_ITEM);
				return false;
			}

			CONT_DEFREDICEOPTIONCOST const * pkCostDef = NULL;
			g_kTblDataMgr.GetContDef(pkCostDef);

			if(NULL == pkCostDef)
			{
				Ret(E_REDICE_NOT_EQUIP_ITEM);
				return false;
			}

			E_ITEM_GRADE const kItemGrade = GetItemGrade(kTargetItem);
			CONT_DEFREDICEOPTIONCOST::const_iterator iter = pkCostDef->find(kItemGrade);
			if(iter == pkCostDef->end())
			{
				Ret(E_REDICE_NOT_EQUIP_ITEM);
				return false;
			}

			int const iCount = pkCaster->GetInven()->GetTotalCount(pkDef->No());
			int const iUseCount = (*iter).second;

			if(iCount < iUseCount)
			{
				Ret(E_REDICE_NOT_ENOUGH_ITEM);
				return false;
			}
			
			if( LOCAL_MGR::NC_JAPAN == g_kLocal.ServiceRegion() 
				&& CheckIsCashItem(kTargetItem))
			{//�Ϻ��� ��� ĳ�� �������� ��æƮ ���� �۾� �Ұ�
				Ret(E_REDICE_NOT_EQUIP_ITEM);
				return false;
			}

			PgBase_Item kItemCopy = kTargetItem;

			GET_DEF(PgItemOptionMgr, kItemOptionMgr);
			kItemOptionMgr.GenerateOption_Sub(kItemCopy);
			SEnchantInfo kEnchant = kTargetItem.EnchantInfo();
			SEnchantInfo const kCopyEnchant = kItemCopy.EnchantInfo();

			kEnchant.BasicType1(kCopyEnchant.BasicType1());
			kEnchant.BasicType2(kCopyEnchant.BasicType2());
			kEnchant.BasicType3(kCopyEnchant.BasicType3());
			kEnchant.BasicType4(kCopyEnchant.BasicType4());

			kEnchant.BasicLv1(kCopyEnchant.BasicLv1());
			kEnchant.BasicLv2(kCopyEnchant.BasicLv2());
			kEnchant.BasicLv3(kCopyEnchant.BasicLv3());
			kEnchant.BasicLv4(kCopyEnchant.BasicLv4());

			m_kAddonPacket.Push(kEnchant);

			m_kOrder.push_back(SPMO(IMET_ADD_ANY|IMC_DEC_DUR_BY_USE, pkCaster->GetID(),SPMOD_Add_Any(pkDef->No(), -iUseCount)));
			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant( kTargetItem, kTargetPos, kEnchant)));
			++iUseEffectCount;
		}break;
	case UICT_SKILLSET_OPEN:
		{
			int const iUseLimit = GetDefSkillSetItemLimit( pkCaster->GetAbil(AT_LEVEL) );

			int const iCount = pkCaster->GetAbil(AT_SKILLSET_USE_COUNT);
			if(iCount >= iUseLimit)
			{
				Ret(E_SKILLSET_MAX_LIMIT);
				return false;
			}

			if(0==iCount)
			{
				SPMOD_AddAbil kSetData(AT_SKILLSET_USE_COUNT, 1);
				m_kOrder.push_back(SPMO(IMET_SET_ABIL, pkCaster->GetID(), kSetData));
			}
			else
			{
				SPMOD_AddAbil kAddData(AT_SKILLSET_USE_COUNT, 1);
				m_kOrder.push_back(SPMO(IMET_ADD_ABIL, pkCaster->GetID(), kAddData));
			}
			++iUseEffectCount;
		}break;
	case UICT_EVENT_ITEM_REWARD:
	case UICT_EVENT_ITEM_REWARD_GAMBLE:
		{
			m_kCause = CIE_OpenEventItemReward;
			++iUseEffectCount;
		}break;
	case UICT_REPAIR_MAX_DURATION:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_REPAIR_MAX_DURATION_NOT_FOUND_ITEM);
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
			if(NULL == pItemDef)
			{
				Ret(E_REPAIR_MAX_DURATION_NOT_FOUND_ITEM);
				return false;
			}

			if(false == pItemDef->CanEquip())
			{
				Ret(E_REPAIR_MAX_DURATION_NOT_FOUND_ITEM);
				return false;
			}

			SEnchantInfo kEnchant = kTargetItem.EnchantInfo();
			if(0 == kEnchant.DecDuration()
				|| !IsCanRepair(kTargetItem.ItemNo() ) )
			{
				Ret(E_REPAIR_MAX_DURATION_CANT_REPAIR);
				return false;
			}

			int const iRepairValue = (iCustomValue1 < kEnchant.DecDuration() ? iCustomValue1 : kEnchant.DecDuration());
			kEnchant.DecDuration(kEnchant.DecDuration() - iRepairValue);

			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT,pkCaster->GetID(), SPMOD_Enchant(kTargetItem, kTargetPos, kEnchant)));
			++iUseEffectCount;
		}break;
	case UICT_TELEPORT:
	case UICT_MISSION_DEFENCE_CONSUME:
	case UICT_MISSION_DEFENCE_CONSUME_ALL:
		{
			++iUseEffectCount;
		}break;
	case UICT_EXTEND_CHRACTERNUM:
		{
			m_kOrder.push_back(SPMO(IMET_EXTEND_CHARACTER_NUM, pkCaster->GetID(),SMOD_Extend_CharacterNum(1)));
			++iUseEffectCount;
		}break;
	case UICT_REPAIR_ITEM_ALL:
		{
			GET_DEF(CItemDefMgr, kItemDefMgr);

			BYTE const bCount = pkCaster->GetInven()->GetMaxIDX(IT_FIT);

			int iRepairItemCount = 0;

			for(BYTE i = 0;i < bCount;++i)
			{
				PgBase_Item kTargetItem;
				SItemPos kTargetPos(IT_FIT,i);

				if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
				{
					continue;
				}

				CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
				if(NULL == pItemDef)
				{
					continue;
				}

				if( !IsCanRepair( kTargetItem.ItemNo()))
				{
					continue;
				}

				int const iRepairCount = pItemDef->MaxAmount() - kTargetItem.Count();
				if(0 >= iRepairCount)
				{
					continue;
				}

				m_kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_INC_DUR_BY_REPAIR, pkCaster->GetID(), SPMOD_Modify_Count(kTargetItem, kTargetPos, iRepairCount)));

				++iRepairItemCount;
			}

			if(0 < iRepairItemCount)
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_REPAIR_ITEM:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_REPAIR_NOT_FOUND_ITEM);
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
			if(NULL == pItemDef)
			{
				Ret(E_REPAIR_NOT_FOUND_ITEM);
				return false;
			}

			if(false == pItemDef->CanEquip())
			{
				Ret(E_REPAIR_NEED_NOT_REPAIR);
				return false;
			}

			int const iLevelLimit = pItemDef->GetAbil(AT_LEVELLIMIT);
			if(iLevelLimit < iCustomValue1 || iCustomValue2 < iLevelLimit)
			{
				Ret(E_REPAIR_NOT_MATCH_LEVEL);
				return false;
			}

			int const iDefaultDur = pItemDef->MaxAmount();
			int const iRepairCount = iDefaultDur - kTargetItem.Count();
			if(0 >= iRepairCount)
			{
				Ret(E_REPAIR_NEED_NOT_REPAIR);
				return false;
			}

			if( !IsCanRepair( kTargetItem.ItemNo() ) )
			{
				Ret(E_REPAIR_CANT_REPAIR);
				return false;
			}

			int const iUseCount = std::min<int>((iRepairCount + (iCustomValue3 - 1))/iCustomValue3,pkCaster->GetInven()->GetTotalCount(pkDef->No()));
			int const iAddCount = std::min<int>(iUseCount * iCustomValue3,iRepairCount);

			m_kOrder.push_back(SPMO(IMET_ADD_ANY|IMC_DEC_DUR_BY_USE, pkCaster->GetID(),SPMOD_Add_Any(pkDef->No(), -iUseCount)));
			m_kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_INC_DUR_BY_REPAIR, pkCaster->GetID(), SPMOD_Modify_Count(kTargetItem, kTargetPos, iAddCount)));

			++iUseEffectCount;
		}break;
	case UICT_EQUIP_LEVELLIMIT:
		{
			++iUseEffectCount;
		}break;
	case UICT_FORCED_ENCHANT:
		{
			int const iCustomValue4 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_4);
			int const iEnchantSteep = pkDef->GetAbil(AT_CUSTOMDATA2);

			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_ENCHANT_NOT_FOUND_ITEM);
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
			if(NULL == pItemDef)
			{
				Ret(E_ENCHANT_NOT_FOUND_ITEM);
				return false;
			}

			int iRequireLvMin = iCustomValue3;
			int iRequireLvMax = (iCustomValue4 == 0) ? (iCustomValue1 - 1) : std::max(iCustomValue3, iCustomValue4);
			if(kTargetItem.EnchantInfo().PlusLv() < iRequireLvMin || kTargetItem.EnchantInfo().PlusLv() > iRequireLvMax)
			{
				Ret(E_ENCHANT_REQUIRE_PLUS_LV_LIMIT);
				return false;
			}
			if(pkDef->GetAbil(AT_EQUIP_LIMIT))
			{ //�������� ���� ���� üũ
				if(!(pkDef->GetAbil(AT_EQUIP_LIMIT) & pItemDef->GetAbil(AT_EQUIP_LIMIT) ))
				{
					Ret(E_ENCHANT_REQUIRE_EQUIP_LIMIT);
					return false;
				}
			}

			int iRequireItemClass = pkDef->GetAbil(AT_CUSTOMDATA1);
			if (iRequireItemClass != 0 && pItemDef->GetAbil(AT_CUSTOMDATA1) != iRequireItemClass)
			{
				Ret(E_ENCHANT_CANT_UPGRADE);
				return false;
			}

			if(!pItemDef->CanEquip() 
				|| (ICMET_Cant_Enchant & pItemDef->GetAbil(AT_ATTRIBUTE))
				|| kTargetItem.EnchantInfo().IsBinding())
			{
				Ret(E_ENCHANT_CANT_UPGRADE);
				return false;
			}

			int iResultLvMin = iCustomValue1;
			int iResultLvMax = std::max(iCustomValue1, iCustomValue2);
			int const iLevel = iEnchantSteep <= 0
				? BM::Rand_Range(iResultLvMax,iResultLvMin)
				: reoil::clamp<int>(iEnchantSteep + kTargetItem.EnchantInfo().PlusLv(), iResultLvMin, iRequireLvMax);
			EPlusItemUpgradeResult rkOutRet = PIUR_NONE;

			TBL_DEF_ITEMPLUSUPGRADE const * pkUpgradeInfo = PgAction_ItemPlusUpgrade::GetPlusInfo(kTargetItem,iLevel, rkOutRet);
			if(NULL == pkUpgradeInfo)
			{
				Ret(E_ENCHANT_CANT_UPGRADE);
				return false;
			}

			SEnchantInfo kNewEnchant = kTargetItem.EnchantInfo();

			if(0 == kNewEnchant.PlusLv())
			{
				int iResultType = 0;
				if(false == PgAction_ItemPlusUpgrade::GenPlusType(pkUpgradeInfo,iResultType))
				{
					Ret(E_ENCHANT_CANT_UPGRADE);
					return false;
				}
				kNewEnchant.PlusType(iResultType);
			}

			kNewEnchant.PlusLv(iLevel);

			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT,pkCaster->GetID(), SPMOD_Enchant(kTargetItem, kTargetPos, kNewEnchant)));
			++iUseEffectCount;
		}break;
	case UICT_CALL_STORE:
		{
			GET_DEF(PgStoreMgr, kStoreMgr);
			BM::GUID kNpcGuid;
			if(S_OK == kStoreMgr.FindShopNpcGuid(iCustomValue1,kNpcGuid))
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_COOLTIME_RATE_SKILL:
	case UICT_COOLTIME_RATE_ITEM:
	case UICT_ADD_COOLTIME_SKILL:
	case UICT_ADD_COOLTIME_ITEM:
		{
			++iUseEffectCount;
		}break;
	case UICT_ACHIEVEMENT:
		{
			PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if(pkPlayer)
			{
				if(false == pkPlayer->GetAchievements()->IsComplete(iCustomValue1))
				{
					CONT_DEF_ACHIEVEMENTS_SAVEIDX const * pkCont = NULL;
					g_kTblDataMgr.GetContDef(pkCont);
					if(pkCont)
					{
						CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator iter = pkCont->find(iCustomValue1);
						if(iter != pkCont->end())
						{
							m_kOrder.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT,pkPlayer->GetID(),SPMOD_Complete_Achievement(iCustomValue1,(*iter).second.iCategory,(*iter).second.iRankPoint ,(*iter).second.iUseTime,(*iter).second.iGroupNo)));
							++iUseEffectCount;
						}
					}
				}
			}
		}break;
	case UICT_COUPLE_ITEM: //Ŀ�ÿ� �������� ��
		{//iCustomValue1 = Ŀ�� ������ �Ÿ�

			//Ŀ���� �ƴϸ� ��� �Ұ�
			if(BM::GUID::NullData() == pkCaster->GetCoupleGuid())
			{
				return false;
			}

			CUnit* pkTarget = m_pkGround->GetUnit(pkCaster->GetCoupleGuid());
			if(pkTarget)
			{
				float fDistance = ::GetDistance(pkCaster->GetPos(), pkTarget->GetPos());
				if(fDistance <= iCustomValue1)
				{
					++iUseEffectCount;
				}
			}
		}break;
	case UICT_CALL_MARKET:
		{
			if(0 == pkCaster->GetAbil(AT_CALL_MARKET))
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_CALL_SAFE:
		{
			if(0 == pkCaster->GetAbil(AT_CALL_SAFE))
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_CALL_SHARE_SAFE:
		{
			if(0 == pkCaster->GetAbil(AT_CALL_SHARE_SAFE))
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_PROTECTION_GALL:
		{
			//AT_PROTECTION_GALL
			++iUseEffectCount;
		}break;
	case UICT_CHANGE_CLASS:
		{
			int const iClass = pkCaster->GetAbil(AT_CLASS);
			short const sLevel = static_cast<short>(pkCaster->GetAbil(AT_LEVEL));

			const CONT_DEFUPGRADECLASS* pkUpClass = NULL;
			g_kTblDataMgr.GetContDef(pkUpClass);

			if(NULL == pkUpClass)
			{
				break;
			}

			CONT_DEFUPGRADECLASS::const_iterator kNewClass = pkUpClass->find(iCustomValue1);
			CONT_DEFUPGRADECLASS::const_iterator kCurClass = pkUpClass->find(iClass);

			if((kNewClass == pkUpClass->end()) || (kCurClass == pkUpClass->end()))
			{
				break;
			}

			if((*kNewClass).second.byGrade <= (*kCurClass).second.byGrade)
			{
				break;
			}

			if((*kNewClass).second.byKind != (*kCurClass).second.byKind)
			{
				break;
			}

			SPMOD_AddAbil kSetClassData(AT_CLASS, iCustomValue1);
			m_kOrder.push_back(SPMO(IMET_SET_ABIL, pkCaster->GetID(), kSetClassData));

			++iUseEffectCount;
		}break;
	case UICT_EMOTION:
		{
			PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
			if(NULL != pkPlayer)
			{
				PgBitArray<MAX_DB_EMOTICON_SIZE> kCont;
				if(true == pkPlayer->GetContEmotion(static_cast<BYTE>(iCustomValue1),kCont))
				{
					if(true == kCont.Get(iCustomValue2))
					{
						return false;
					}

					++iUseEffectCount;
					m_kOrder.push_back(SPMO(IMET_ADD_EMOTION,pkCaster->GetID(), SPMOD_AddEmotion(static_cast<BYTE>(iCustomValue1),iCustomValue2)));
				}
			}
		}break;
	case UICT_SEAL_REMOVE:
		{
			SItemPos kDischargeItemPos;
			m_kOrgPacket.Pop(kDischargeItemPos);
			PgAction_ItemDischarge kAction(m_pkGround->GroundKey(),kDischargeItemPos,m_kCasterPos,m_kOrder);
			if(true == kAction.DoAction(pkCaster, NULL))
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_SEAL:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK == pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
				if(pItemDef)
				{
					if( (ICMET_Cant_Seal != (ICMET_Cant_Seal & pItemDef->GetAbil(AT_ATTRIBUTE)))
						&& (true == pItemDef->CanEquip()) 
						&& (false == kTargetItem.EnchantInfo().IsSeal()))
					{
						SEnchantInfo kNewEnchant = kTargetItem.EnchantInfo();
						kNewEnchant.IsSeal(true);
						m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT,pkCaster->GetID(), SPMOD_Enchant(kTargetItem, kTargetPos, kNewEnchant)));
						++iUseEffectCount;
					}
				}
			}
		}break;
	case UICT_CASH_PACK:
		{
			PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
			if(NULL != pkPlayer)
			{
				//m_kOrder.push_back(SPMO(IMET_ADD_CASH,pkPlayer->GetID(), SPMOD_AddCash(pkPlayer->GetMemberGUID(),static_cast<__int64>(iCustomValue1),pkPlayer->Name(),CIE_Cash_Pack)));
				m_kCashAdd = SPMO(IMET_ADD_CASH,pkPlayer->GetID(), SPMOD_AddCash(pkPlayer->GetMemberGUID(),static_cast<__int64>(iCustomValue1),pkPlayer->Name(),CIE_Cash_Pack));
				++iUseEffectCount;

				BM::Stream kPacket(PT_M_C_NOTI_CASH_PACK);
				kPacket.Push(static_cast<__int64>(iCustomValue1));
				pkPlayer->Send(kPacket);
			}
		}break;
	case UICT_MONEY_PACK:
		{
			__int64 const i64AddMoney = std::max(BM::Rand_Range(iCustomValue2,iCustomValue1),1);
			SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), SPMOD_Add_Money(i64AddMoney));
			m_kOrder.push_back(kIMO);
			++iUseEffectCount;

			BM::Stream kPacket(PT_M_C_NOTI_MONEY_PACK);
			kPacket.Push(i64AddMoney);
			pkCaster->Send(kPacket);
		}break;
	case UICT_USER_PORTAL:
		{
			++iUseEffectCount;
		}break;
	case UICT_CREATE_CARD:
	case UICT_MODIFY_CARD:
		{
			BYTE			bYear,
							bSex;
			int				iLocal;
			BYTE			bConstellation,
							bHobby,
							bBlood,
							bStyle;
			std::wstring	kComment;

			m_kOrgPacket.Pop(bYear);
			m_kOrgPacket.Pop(bSex);
			m_kOrgPacket.Pop(iLocal);
			m_kOrgPacket.Pop(kComment);
			m_kOrgPacket.Pop(bConstellation);
			m_kOrgPacket.Pop(bHobby);
			m_kOrgPacket.Pop(bBlood);
			m_kOrgPacket.Pop(bStyle);

			bool const bCommentFiltered = g_kFilterString.Filter(kComment, false, FST_ALL);
			bool const bCommentUniCodeFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_CHAR_CARD_COMMENT, kComment));
			if( bCommentFiltered
			||	bCommentUniCodeFiltered )
			{
				Ret(E_BADSTRING);
				break;
			}
			
			SMOD_CreateCard kCard(bYear,bSex,iLocal,kComment,bConstellation,bHobby,bBlood,bStyle,pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1));

			++iUseEffectCount;

			switch(iCustomType)
			{
			case UICT_CREATE_CARD:
				{
					m_kOrder.push_back(SPMO(IMET_CREATE_CHARACTERCARD, pkCaster->GetID(),kCard));
				}break;
			case UICT_MODIFY_CARD:
				{
					m_kOrder.push_back(SPMO(IMET_MODIFY_CHARACTERCARD, pkCaster->GetID(),kCard));
				}break;
			}
		}break;
	case UICT_SAFEEXTEND:
		{
			EInvType kInvType;
			m_kOrgPacket.Pop(kInvType);
			BYTE const kReductionNum = pkCaster->GetInven()->GetInvExtendSize(kInvType);
			BYTE const kExpendNum = static_cast<BYTE>(iCustomValue1);
			if(kExpendNum <= kReductionNum)
			{
				++iUseEffectCount;
				m_kOrder.push_back(SPMO(IMET_INVENTORY_EXTEND, pkCaster->GetID(), SMOD_InvExtend(kInvType,kExpendNum)));
			}
		}break;
	case UICT_INVEXTEND:
		{
			EInvType const kInvType = static_cast<EInvType>(iCustomValue1);
			BYTE const kReductionNum = pkCaster->GetInven()->GetInvExtendSize(kInvType);
			BYTE const kExpendNum = static_cast<BYTE>(iCustomValue2);
			if(kExpendNum <= kReductionNum)
			{
				++iUseEffectCount;
				m_kOrder.push_back(SPMO(IMET_INVENTORY_EXTEND, pkCaster->GetID(), SMOD_InvExtend(kInvType,kExpendNum)));
			}
		}break;
	case UICT_SAFE_EXTEND_MAX_IDX:
		{
			EInvType kInvType;
			m_kOrgPacket.Pop(kInvType);
			BYTE const kExpendNum = static_cast<BYTE>(iCustomValue1);
			BYTE const kReductionNum = pkCaster->GetInven()->GetInvExtendSize(kInvType);
			BYTE const kEnableExtendIdx = pkCaster->GetInven()->GetEnableExtendMaxIdx(kInvType);

			switch(g_kLocal.ServiceRegion())
			{//�ű� ��Ī�Ǵ� �������� �����̾� �������� ����, ���� �κ��丮 Ȯ�� ������ ���
			case LOCAL_MGR::NC_DEVELOP:
			case LOCAL_MGR::NC_THAILAND:
			case LOCAL_MGR::NC_INDONESIA:
			case LOCAL_MGR::NC_JAPAN:
			case LOCAL_MGR::NC_TAIWAN:
			case LOCAL_MGR::NC_CHINA:
			case LOCAL_MGR::NC_RUSSIA:
			case LOCAL_MGR::NC_KOREA:
			case LOCAL_MGR::NC_EU:
			case LOCAL_MGR::NC_FRANCE:
			case LOCAL_MGR::NC_GERMANY:
			case LOCAL_MGR::NC_SINGAPORE:
			case LOCAL_MGR::NC_PHILIPPINES:
				{
					if(kExpendNum <= kEnableExtendIdx)
					{
						++iUseEffectCount;
						m_kOrder.push_back(SPMO(IMET_EXTEND_MAX_IDX, pkCaster->GetID(), SMOD_InvExtend(kInvType,kExpendNum)));
					}
				}break;
			default:
				{
					if((0 == kReductionNum) && kExpendNum <= kEnableExtendIdx)
					{
						++iUseEffectCount;
						m_kOrder.push_back(SPMO(IMET_EXTEND_MAX_IDX, pkCaster->GetID(), SMOD_InvExtend(kInvType,kExpendNum)));
					}
				}break;
			}
		}break;
	case UICT_EXTEND_MAX_IDX:
		{
			EInvType const kInvType = static_cast<EInvType>(iCustomValue1);
			BYTE const kExpendNum = static_cast<BYTE>(iCustomValue2);
			BYTE const kReductionNum = pkCaster->GetInven()->GetInvExtendSize(kInvType);
			BYTE const kEnableExtendIdx = pkCaster->GetInven()->GetEnableExtendMaxIdx(kInvType);

			if(kExpendNum <= kEnableExtendIdx)
			{
				++iUseEffectCount;
				m_kOrder.push_back(SPMO(IMET_EXTEND_MAX_IDX, pkCaster->GetID(), SMOD_InvExtend(kInvType,kExpendNum)));
			}
		}break;
	case UICT_STYLEITEM:
		{
			m_kOrder.push_back(SPMO(IMET_SET_DEFAULT_ITEM, pkCaster->GetID(), SMOD_SetDefaultItem(iCustomValue1,iCustomValue2)));
			++iUseEffectCount;
		}break;
	case UICT_HOME_STYLEITEM:
		{
			PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if(pkPlayer)
			{
				BM::GUID kHomeGuid;
				m_kOrgPacket.Pop(kHomeGuid);
				SHOMEADDR const & kAddr = pkPlayer->HomeAddr();
				m_kOrder.push_back(SPMO(IMET_MYHOME_STYLE_MODIFY, kHomeGuid, SMOD_SetHomeStyle(pkPlayer->GetID(),kAddr.StreetNo(),kAddr.HouseNo(),iCustomValue1,kItem.ItemNo())));
				++iUseEffectCount;
			}
		}break;
	case UICT_SAFEMODE:
		{
			int iIndex = 0;
			while(iIndex < EFFECTNUM_MAX)		//�߰�����Ʈ �ɱ�
			{
				int const iEffectNo = pkDef->GetAbil(AT_EFFECTNUM1+iIndex);
				if(0==iEffectNo)	{break;}
				if(pkCaster->GetItemTypeEffect(pkDef->No(), true))
				{
					pkCaster->SendWarnMessage(194);	//���� ������� �������Դϴ�.
					return false;//������ǰ�� �ߺ���� �Ұ�
				}
				++iIndex;
			}
		}//Break����. �Ʒ��� �������� ��
	case UICT_SMS:
	case UICT_BONUS_RATE_BUF:
		{
			++iUseEffectCount;
		}break;
	case UICT_RESET_SKILL_POINT:
		{
			SPMOD_AddSkill kAddSkillData(0, true);//����. 
			m_kOrder.push_back(SPMO(IMET_ADD_SKILL, pkCaster->GetID(), kAddSkillData));
			++iUseEffectCount;
		}break;
	case UICT_PARTY:
		{
			PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
			if( pkPC &&	pkPC->PartyGuid() != BM::GUID::NullData() )
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_OPEN_CHEST:
		{
			++iUseEffectCount;
		}break;
	case UICT_OPEN_PACK:
	case UICT_OPEN_PACK2:
		{
			++iUseEffectCount;
		}break;
	case UICT_ELIXIR:
		{//
			int iNowAbil = AT_HP;
			int iMaxAbil = AT_C_MAX_HP;
			if(iCustomValue1 != 1 )
			{
				iNowAbil = AT_MP;
				iMaxAbil = AT_C_MAX_MP;
			}

			int const iNowValue = pkCaster->GetAbil(iNowAbil);
			int const iMaxValue = pkCaster->GetAbil(iMaxAbil);

			if(iMaxValue > iNowValue)
			{
				//ȸ���� �ʿ���.
				int iRecoverValue = __min(iMaxValue - iNowValue, kItem.Count()*100);//1����Ʈ�� 100��ŭ. ȸ��
				int const iCheckValue = ( (LOCAL_MGR::NC_USA == g_kLocal.ServiceRegion()) ? 0 : 99 );// �Ϲ̴� ȸ���� ���� ������ ������ ���, �ٸ����� 100�̻���� ���
				if ( iCheckValue < iRecoverValue )
				{
					int iDecDur = iRecoverValue/100;
					if ( 0 == iDecDur )
					{
						iDecDur = 1;
					}

					SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -iDecDur);//1�� ����.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkCaster->GetID(), kDelData);
					m_kOrder.push_back(kIMO);

					int iNewValue = std::min((iNowValue + iRecoverValue),iMaxValue) - iNowValue;
					m_kAddonPacket.Push(iNowAbil);
					m_kAddonPacket.Push(iNewValue);
					++iUseEffectCount;
				}
			}
		}break;
	case UICT_ADD_SP:
		{
			SPMOD_AddAbil kSetSPData(AT_SP, iCustomValue1);
			m_kOrder.push_back(SPMO(IMET_ADD_ABIL, pkCaster->GetID(), kSetSPData));
			++iUseEffectCount;
		}break;
	case UICT_REVIVE:
		{
			if (m_kGndAttr & GATTR_BATTLESQUARE)
			{
				// ��Ȱ�������� BS���� ��� �Ұ�
				return false;
			}
			SPMOD_AddRankPoint kAddRank( E_RANKPOINT_USEPHOENIXFEATHER_COUNT, 1 );// ��ŷ �÷�.
			m_kOrder.push_back( SPMO(IMET_ADD_RANK_POINT, pkCaster->GetID(), kAddRank) );
			++iUseEffectCount;
		}break;
	case UICT_GUILDMarkChange1:
		{
			++iUseEffectCount;
		}break;
	case UICT_BIND:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK == pkCaster->GetInven()->GetItem(kTargetPos,kTargetItem))
			{
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
				if(pItemDef)
				{
					if(true == pItemDef->CanEquip())
					{
						if(false == kItem.EnchantInfo().IsBinding())
						{
							SEnchantInfo kNewEnchant = kTargetItem.EnchantInfo();
							kNewEnchant.IsBinding(1);
							m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT,pkCaster->GetID(), SPMOD_Enchant(kTargetItem, kTargetPos, kNewEnchant)));
							++iUseEffectCount;
						}
					}
				}
			}
		}break;
	case UICT_UNBIND:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);
			PgBase_Item kTargetItem;
			if(S_OK == pkCaster->GetInven()->GetItem(kTargetPos,kTargetItem))
			{
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const* pItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
				if(pItemDef)
				{
					if((true == kTargetItem.EnchantInfo().IsBinding()) && (false == kTargetItem.EnchantInfo().IsUnbindReq()))
					{
						SEnchantInfo kNewEnchant = kTargetItem.EnchantInfo();
						kNewEnchant.IsUnbindReq(1);
						m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT,pkCaster->GetID(), SPMOD_Enchant(kTargetItem, kTargetPos, kNewEnchant)));
						m_kOrder.push_back(SPMO(IMET_ADD_UNBIND_DATE,pkCaster->GetID(), SMOD_AddUnbindDate(kTargetPos,static_cast<BYTE>(iCustomValue1))));
						++iUseEffectCount;
					}
				}
			}
		}break;
	case UICT_PET_EGG:
	case UICT_PET_FEED:
	case UICT_PET_GROWTH:
		{
			++iUseEffectCount;
		}break;
	case UICT_HELP_END_QUEST:
		{
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
			if( pkPlayer )
			{
				PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
				if( pkMyQuest )
				{
					int const iMaxCount = 4;
					ContQuestID kVec(iMaxCount, 0);
					kVec[0] = pkDef->GetAbil(AT_HELP_END_QUEST1);
					kVec[1] = pkDef->GetAbil(AT_HELP_END_QUEST2);
					kVec[2] = pkDef->GetAbil(AT_HELP_END_QUEST3);
					kVec[3] = pkDef->GetAbil(AT_HELP_END_QUEST4);

					ContQuestID::const_iterator iter = kVec.begin();
					while( kVec.end() != iter )
					{
						int const iQuestID = (*iter);
						PgQuestInfo const* pkQuestInfo = NULL;
						if( 0 != iQuestID
						&&	g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
						{
							SUserQuestState const* pkUserState = pkMyQuest->Get(iQuestID);
							if( pkUserState
							&&	QS_End != pkUserState->byQuestState
							&&	false == pkQuestInfo->IsDependItemQuest() )
							{
								for( int iCur=0; QUEST_PARAMNUM>iCur; ++iCur )
								{
									int const iEndCount = pkQuestInfo->GetParamEndCondition(iCur);
									if( 0 != iEndCount )
									{
										PgAction_SetQuestParam::DoAction(pkCaster->GetID(), pkUserState, iCur, iEndCount, m_kOrder);
									}
								}

								++iUseEffectCount;
								break; // �ϳ��� ����Ʈ�� �ϼ����ش�
							}
						}
						++iter;
					}
				}
			}

			if( 0 == iUseEffectCount )
			{
				pkCaster->SendWarnMessage(700502);
			}
		}break;
	case UICT_LEARNSKILL:
		{
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
			if( pkPlayer )
			{
				PgMySkill* pkMySkill = pkPlayer->GetMySkill();
				if( pkMySkill )
				{
					if( 0 == pkMySkill->GetLearnedSkill( GET_BASICSKILL_NUM(iCustomValue1) ) )
					{
						m_kOrder.push_back( SPMO(IMET_ADD_SKILL, pkPlayer->GetID(), SPMOD_AddSkill(iCustomValue1)) );
						++iUseEffectCount;

						//Ư���� ��ų�� ��� ��� �����ϸ� �˷��ش�.
						GET_DEF(CSkillDefMgr, kSkillDefMgr);
						CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iCustomValue1);
						if(pkSkill)
						{
							int iSkillEvent = pkSkill->GetAbil(AT_LEARN_SKILL_EVENT);
							if(iSkillEvent != 0)
							{
								BM::Stream kPacket(PT_M_C_NFY_LEARN_SKILL_EVENT);
								kPacket.Push(iSkillEvent);
								pkPlayer->Send(kPacket);
							}
						}
					}
					else
					{
						pkCaster->SendWarnMessage(700509);
					}
				}
			}
		}break;
	case UICT_STATUS_RESET:
		{
			// Status Reset (AT_STR_ADD, AT_INT_ADD, AT_CON_ADD, AT_DEX_ADD, AT_ALL_STATUS)
			SPMO_ActiveStatus kReset;
			switch (iCustomValue1)
			{
			case AT_STR_ADD:
				{
					if (pkCaster->GetAbil(AT_STR_ADD) > 0)
					{
						kReset.Str(-1);
					}
				}break;
			case AT_INT_ADD:		
				{
					if (pkCaster->GetAbil(AT_INT_ADD) > 0)
					{
						kReset.Int(-1);
					}
				}break;
			case AT_CON_ADD:
				{
					if (pkCaster->GetAbil(AT_CON_ADD) > 0)
					{
						kReset.Con(-1);
					}
				}break;
			case AT_DEX_ADD:
				{
					if (pkCaster->GetAbil(AT_DEX_ADD) > 0)
					{
						kReset.Dex(-1);
					}
				}break;
			case AT_ALL_STATUS:
				{
					if (pkCaster->GetAbil(AT_STR_ADD) > 0)
					{
						kReset.Str(-1);
					}
					if (pkCaster->GetAbil(AT_INT_ADD) > 0)
					{
						kReset.Int(-1);
					}
					if (pkCaster->GetAbil(AT_CON_ADD) > 0)
					{
						kReset.Con(-1);
					}
					if (pkCaster->GetAbil(AT_DEX_ADD) > 0)
					{
						kReset.Dex(-1);
					}
				} break;
			}
			if (kReset.Str() == 0 && kReset.Int() == 0 && kReset.Con() == 0 && kReset.Dex() == 0)
			{
				pkCaster->SendWarnMessage(790657);	// �ʱ�ȭ �Ǿ� �־� ����� �� �����ϴ�.
			}
			else
			{
				m_kOrder.push_back(SPMO(IMET_ACTIVE_STATUS, pkCaster->GetID(), kReset));
				++iUseEffectCount;
			}
		}break;
	case UICT_TRANSTOWER_DISCOUNT:
	case UICT_PET_RENAME:
	case UICT_PET_COLORCHANGE:
	case UICT_HARDCORE_KEY:
		{// ���⼭ ����ϸ� �ȵ�
			return false;
		}break;
	case UICT_HARDCORE_BOSSKEY:
		{
			if ( GATTR_HARDCORE_DUNGEON == m_kGndAttr )
			{
				++iUseEffectCount;
			}
		}break;
	case UICT_TRANSTOWER_OPENMAP:
		{
			++iUseEffectCount;
		}break;
	case UICT_AWAKE_INCREASE_ITEM:
		{
			if ( pkCaster->GetAbil(AT_ENABLE_AWAKE_SKILL) )
			{
				if ( pkDef->GetAbil(AT_AWAKE_VALUE) )
				{
					int const iNowAwakeValue = pkCaster->GetAbil(AT_AWAKE_VALUE);
					int const iMaxAwakeValue = AWAKE_VALUE_MAX;
					if ( iMaxAwakeValue > iNowAwakeValue )
					{
						++iUseEffectCount;
					}
				}
			}
		}break;
	case UICT_SUPER_GROUND_FEATHER:
	case UICT_SUPER_GROUND_MOVE_FLOOR:
	case UICT_SUPER_GROUND_ENTER_FLOOR:
		{
			++iUseEffectCount;
		}break;
	case UICT_MISSION_DEFENCE_POTION:
		{
			if( m_kGndAttr & GATTR_MISSION )
			{
				PgMissionGround *pkMissionGround = dynamic_cast<PgMissionGround*>(m_pkGround);
				if( pkMissionGround )
				{
					if( true == pkMissionGround->IsDefenceMode() )
					{
						if( true == pkMissionGround->GetDefenceObjectHpCheck() )
						{
							// ���ҽ� ��常 �ȴ�.
							++iUseEffectCount;
							break;
						}
						else
						{
							HRESULT hRet(E_CANNOT_ITEM);

							BM::Stream kPacket(PT_M_C_ANS_DEFENCE_POTION);
							kPacket.Push(hRet);
							pkCaster->Send(kPacket);
							return false;
						}
					}
				}
			}
			pkCaster->SendWarnMessage(400917);
			return false;
		}break;
	case UICT_GUILD_INV_EXTEND_LINE:
	case UICT_GUILD_INV_EXTEND_TAB:
		{
			++iUseEffectCount;
		}break;
	case UICT_JOBSKILL_SAVEIDX:
		{
			pkCaster->SendWarnMessage(25006);
			return false;
		}break;
	case UICT_SCROLL_QUEST:
		{
			++iUseEffectCount;
		}break;
	case UICT_REVIVE_PARTY:
		{
			if(NULL == m_pkGround)
			{
				return false;
			}

			T_GNDATTR const iNotAttr = (T_GNDATTR)(pkDef->GetAbil(AT_NOTAPPLY_MAPATTR));
			T_GNDATTR const iCanAttr = (T_GNDATTR)(pkDef->GetAbil(AT_CAN_GROUND_ATTR));
			bool bCantUseGround = (0 != (iNotAttr & m_kGndAttr));
			bCantUseGround = ((0 != iCanAttr)? 0 == (m_kGndAttr & iCanAttr): false) || bCantUseGround;
			if( bCantUseGround )
			{// ����� �� ���� ����Դϴ�.
				pkCaster->SendWarnMessage(20027);
				return false;
			}

			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if( pkPlayer )
			{
				if( pkPlayer->HaveParty() )
				{//��Ƽ�����̰�
					BM::GUID const& kPartyGuid = pkPlayer->PartyGuid();

					VEC_GUID kGuidVec;
					if( m_pkGround->GetPartyMemberGround(kPartyGuid, m_kGndKey, kGuidVec, pkPlayer->GetID()) )
					{//�ٸ������ �ְ�
						int const iMaxReviveCount = pkDef->GetAbil(AT_COUNT);
						VEC_GUID kDeadGuidVec;
						if( 0 == iMaxReviveCount
						||	kGuidVec.size() <= iMaxReviveCount )
						{
							VEC_GUID::const_iterator iter_Vec = kGuidVec.begin();
							while( kGuidVec.end() != iter_Vec )
							{
								CUnit* pUnit = m_pkGround->GetUnit( *iter_Vec );
								if( pUnit && false==pUnit->IsAlive() )
								{
									kDeadGuidVec.push_back( *iter_Vec );
								}
								++iter_Vec;
							}
						}
						else
						{
							std::random_shuffle(kGuidVec.begin(), kGuidVec.end());

							int iCurrentReviveCount = 0;
							VEC_GUID::const_iterator iter_Vec = kGuidVec.begin();							
							while( kGuidVec.end() != iter_Vec )
							{
								CUnit* pUnit = m_pkGround->GetUnit( *iter_Vec );
								if( pUnit && !pUnit->IsAlive() )
								{
									kDeadGuidVec.push_back( *iter_Vec );
									++iCurrentReviveCount;
								}
								if( iMaxReviveCount <= iCurrentReviveCount )
								{
									break;
								}
								++iter_Vec;
							}
						}

						if(0 == kDeadGuidVec.size())
						{//���� ���� ������ ����
							pkCaster->SendWarnMessage(799420);
							return false;
						}
						int const iHP_Reserve = pkDef->GetAbil(AT_HP_RESERVED);
						int const iMp_Reserve = pkDef->GetAbil(AT_MP_RESERVED);
						int const iIsIgnoreDeathPanalty = pkDef->GetAbil(AT_IS_DEATH_PENALTY);
						VEC_GUID::const_iterator iter_DeadVec = kDeadGuidVec.begin();
						while( kDeadGuidVec.end() != iter_DeadVec )
						{
							if(PgPlayer * pkMemberPlayer = m_pkGround->GetUser(*iter_DeadVec))
							{
								int const iNewHP = pkMemberPlayer->GetAbil(AT_C_MAX_HP) * iHP_Reserve / ABILITY_RATE_VALUE;
								int const iNewMP = pkMemberPlayer->GetAbil(AT_C_MAX_MP) * iMp_Reserve / ABILITY_RATE_VALUE;
								pkMemberPlayer->Alive(EALIVE_INSURANCE, E_SENDTYPE_BROADALL | E_SENDTYPE_MUSTSEND, iNewHP, iNewMP);

								if( m_pkGround->GetAttr() & GATTR_EVENT_GROUND )
								{
									PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(m_pkGround);
									if( pStaticGround && (pStaticGround->GetEventGroundState() & EGS_PLAY) )
									{
										pStaticGround->RevivePlayer();
									}
								}

								PgDeathPenalty kAction( m_pkGround->GroundKey(), LURT_Item, ((iIsIgnoreDeathPanalty)?false:true) );
								kAction.DoAction(pkMemberPlayer, NULL);

								if( pkPlayer->HaveExpedition() )	// �����뿡 ���ԵǾ� �ִٸ�, �ڽ��� ���� ��ȭ�� �˷��� ��.
								{
									BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
									Packet.Push(pkMemberPlayer->ExpeditionGuid());
									Packet.Push(pkMemberPlayer->GetID());
									Packet.Push(pkMemberPlayer->IsAlive());
									::SendToGlobalPartyMgr(Packet);
								}
							}
							++iter_DeadVec;
						}
						
						++iUseEffectCount;
					}
					else
					{
						pkCaster->SendWarnMessage(799420);
						return false;
					}
				}
				else
				{
					pkCaster->SendWarnMessage(799421);
					return false;
				}
			}
		}break;
	case UICT_REVIVE_EXPEDITION:
		{
			if(NULL == m_pkGround)
			{
				return false;
			}

			T_GNDATTR const iNotAttr = (T_GNDATTR)(pkDef->GetAbil(AT_NOTAPPLY_MAPATTR));
			T_GNDATTR const iCanAttr = (T_GNDATTR)(pkDef->GetAbil(AT_CAN_GROUND_ATTR));
			bool bCantUseGround = (0 != (iNotAttr & m_kGndAttr));
			bCantUseGround = ((0 != iCanAttr)? 0 == (m_kGndAttr & iCanAttr): false) || bCantUseGround;
			if( bCantUseGround )
			{// ����� �� ���� ����Դϴ�.
				pkCaster->SendWarnMessage(20027);
				return false;
			}

			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if( pkPlayer )
			{
				if( pkPlayer->HaveExpedition() )
				{//����������̰�
					BM::GUID const& kExpeditionGuid = pkPlayer->ExpeditionGuid();

					VEC_GUID kGuidVec;
					if( m_pkGround->GetExpeditionMemberGround(kExpeditionGuid, m_kGndKey, kGuidVec, pkPlayer->GetID()) )
					{//�ٸ������ �ְ�
						int const iMaxReviveCount = pkDef->GetAbil(AT_COUNT);
						VEC_GUID kDeadGuidVec;
						if( 0 == iMaxReviveCount
						||	kGuidVec.size() <= iMaxReviveCount )
						{
							VEC_GUID::const_iterator iter_Vec = kGuidVec.begin();
							while( kGuidVec.end() != iter_Vec )
							{
								CUnit* pUnit = m_pkGround->GetUnit( *iter_Vec );
								if( pUnit && !pUnit->IsAlive() )
								{
									kDeadGuidVec.push_back( *iter_Vec );
								}
								++iter_Vec;
							}
						}
						else
						{
							std::random_shuffle(kGuidVec.begin(), kGuidVec.end());

							int iCurrentReviveCount = 0;
							VEC_GUID::const_iterator iter_Vec = kGuidVec.begin();
							while( kGuidVec.end() != iter_Vec )
							{
								CUnit* pUnit = m_pkGround->GetUnit( *iter_Vec );
								if( pUnit && !pUnit->IsAlive() )
								{
									kDeadGuidVec.push_back( *iter_Vec );
									++iCurrentReviveCount;
								}
								if( iMaxReviveCount <= iCurrentReviveCount )
								{
									break;
								}
								++iter_Vec;
							}
						}

						if(0 == kDeadGuidVec.size())
						{//���� ���� ������ ����
							pkCaster->SendWarnMessage(799422);
							return false;
						}
						int const iHP_Reserve = pkDef->GetAbil(AT_HP_RESERVED);
						int const iMp_Reserve = pkDef->GetAbil(AT_MP_RESERVED);
						int const iIsIgnoreDeathPanalty = pkDef->GetAbil(AT_IS_DEATH_PENALTY);
						VEC_GUID::const_iterator iter_DeadVec = kDeadGuidVec.begin();
						while( kDeadGuidVec.end() != iter_DeadVec )
						{
							if(PgPlayer * pkMemberPlayer = m_pkGround->GetUser( *iter_DeadVec ))
							{
								int const iNewHP = pkMemberPlayer->GetAbil(AT_C_MAX_HP) * iHP_Reserve / ABILITY_RATE_VALUE;
								int const iNewMP = pkMemberPlayer->GetAbil(AT_C_MAX_MP) * iMp_Reserve / ABILITY_RATE_VALUE;
								pkMemberPlayer->Alive(EALIVE_INSURANCE, E_SENDTYPE_BROADALL | E_SENDTYPE_MUSTSEND, iNewHP, iNewMP);

								if( m_pkGround->GetAttr() & GATTR_EVENT_GROUND )
								{
									PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(m_pkGround);
									if( pStaticGround && (pStaticGround->GetEventGroundState() & EGS_PLAY) )
									{
										pStaticGround->RevivePlayer();
									}
								}

								PgDeathPenalty kAction( m_pkGround->GroundKey(), LURT_Item, ((iIsIgnoreDeathPanalty)?false:true) );
								kAction.DoAction(pkMemberPlayer, NULL);

								if( pkPlayer->HaveExpedition() )	// �����뿡 ���ԵǾ� �ִٸ�, �ڽ��� ���� ��ȭ�� �˷��� ��.
								{
									BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
									Packet.Push(pkMemberPlayer->ExpeditionGuid());
									Packet.Push(pkMemberPlayer->GetID());
									Packet.Push(pkMemberPlayer->IsAlive());
									::SendToGlobalPartyMgr(Packet);
								}
							}
							++iter_DeadVec;
						}
						
						++iUseEffectCount;
					}
					else
					{
						pkCaster->SendWarnMessage(799422);
						return false;
					}
				}
				else
				{
					pkCaster->SendWarnMessage(799423);
					return false;
				}
			}
		}break;
	case UICT_RECOVERY_STRATEGYFATIGABILITY:
		{	// ���� �Ƿε� ȸ�� ������ ���.
			int const RecoveryAmount  = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);	// ���� �Ƿε� ȸ����.

			SSyncVariable SyncVarialbe = g_kEventView.VariableCont();
			int MaxStrategyFatigability = SyncVarialbe.iMaxStrategyFatigability;	// �ִ� �Ƿε�. �ִ� �Ƿε��� ���� �ʴ� ������ ȸ�����Ѿ� ��.

			int CurFatigability = pkCaster->GetAbil(AT_STRATEGY_FATIGABILITY);	// ���� �Ƿε�.
			if( CurFatigability == MaxStrategyFatigability )
			{	// ���� �Ƿε��� �ִ�ġ��� �������� ������� ����.
				pkCaster->SendWarnMessage(750046);
				return false;
			}
			
			int FinalValue = RecoveryAmount + CurFatigability;
			if( FinalValue > MaxStrategyFatigability )	// �Ƿε� �ִ밪 ���� ū��?
			{
				FinalValue = MaxStrategyFatigability; // ũ�� �ִ밪���� ����.
			}

			pkCaster->SetAbil(AT_STRATEGY_FATIGABILITY, FinalValue, true);	// �Ƿε� ����.
		}break;
	case UICT_R_RECOVERY_STRATEGYFATIGABILITY:
		{	// ���� �Ƿε� ȸ�� ������ ���.
			int const RecoveryRate = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

			SSyncVariable SyncVarialbe = g_kEventView.VariableCont();
			int MaxStrategyFatigability = SyncVarialbe.iMaxStrategyFatigability;	// �ִ� �Ƿε�. �ִ� �Ƿε��� ���� �ʴ� ������ ȸ�����Ѿ� ��.

			int CurFatigability = pkCaster->GetAbil(AT_STRATEGY_FATIGABILITY);	// ���� �Ƿε�.
			if( CurFatigability == MaxStrategyFatigability )
			{	// ���� �Ƿε��� �ִ�ġ��� �������� ������� ����.
				pkCaster->SendWarnMessage(750046);
				return false;
			}

			int RecoveryAmout = MaxStrategyFatigability + static_cast<int>(( static_cast<float>(MaxStrategyFatigability) * ( static_cast<float>(RecoveryRate) / 10000 ) ));

			if( RecoveryAmout > MaxStrategyFatigability )	// �Ƿε� �ִ밪 ���� ū��?
			{
				RecoveryAmout = MaxStrategyFatigability; // ũ�� �ִ밪���� ����.
			}

			pkCaster->SetAbil(AT_STRATEGY_FATIGABILITY, RecoveryAmout, true);	// �Ƿε� ����.
		}break;
	case UICT_PREMIUM_SERVICE:
		{
			GET_DEF(PgDefPremiumMgr, kDefPremium);
			SPremiumData const* pkDefPremium = kDefPremium.GetDef(iCustomValue1);			
			if(!pkDefPremium)
			{
				return false;
			}
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if( !pkPlayer )
			{
				return false;
			}

			m_kCause = CIE_PREMIUM_SERVICE;
			if(pkPlayer->GetPremium().IsUserService())
			{
				if(pkDefPremium->iServiceNo == pkPlayer->GetPremium().GetServiceNo())
				{
					int const iUseDate = pkDefPremium->wUseDate * 24 * 60;
					//����
					m_kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_MODIFY, pkCaster->GetID(), SPMOD_PremiumServiceModify(pkDefPremium->iServiceNo,iUseDate)));
				}
				else
				{
					//�ٸ� ��ȣ�� ���񽺴� ����Ǳ� ������ ���� �Ұ�
					return false;
				}
			}
			else
			{
				//���� �ű� ���
				m_kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_INSERT, pkCaster->GetID(), SPMOD_PremiumService(pkDefPremium->iServiceNo)));
			}
			++iUseEffectCount;
		}break;
	case UICT_SUMMON_SKILL_ACTOR:
		{

			SCreateEntity kCreateInfo;
			kCreateInfo.kClassKey.iClass = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
			kCreateInfo.kClassKey.nLv = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
			kCreateInfo.bUniqueClass = false;
			kCreateInfo.ptPos = pkCaster->GetPos();

			CUnit* pkEntity = NULL;
			kCreateInfo.kGuid.Generate();		// GUID ����
			pkEntity = m_pkGround->CreateEntity( pkCaster, &kCreateInfo, _T("Entity") );
			if( pkEntity )
			{
				return true;
			}

			return false;
		}break;
	case UICT_JOBSKILL_LEARN:
		{
			PgAction_JobSkill_Learn kAction(m_pkGround->GroundKey());
			if( kAction.DoAction(pkCaster, NULL) )
			{
				return true;
			}
			else
			{
				pkCaster->SendWarnMessage( 799525 );
			}
			return false;
		}break;
	default:
		{
			
		}break;
	}

	__int64 const iAddExp = pkDef->GetAbil64(AT_EXPERIENCE);
	if (iAddExp)
	{
		__int64 iExp = pkCaster->GetAbil64(AT_EXPERIENCE);
		SPMOD_AddAbil kAddAbilData(AT_EXPERIENCE, iAddExp);
		m_kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkCaster->GetID(), kAddAbilData));

		if (pkCaster->GetAbil64(AT_REST_EXP_ADD_MAX) > 0)
		{
			SPMOD_AddAbil kAddRestData(AT_REST_EXP_ADD_MAX, iAddExp);
			m_kOrder.push_back( SPMO(IMET_ADD_ABIL64_MAPUSE, pkCaster->GetID(), kAddRestData) );
		}

		++iUseEffectCount;
	}

	int const iNewEffect1 = pkDef->GetAbil(AT_EFFECTNUM1);
	if(iNewEffect1)
	{
		switch(iCustomValue2)
		{
		case UICT2_CHECK_PLAYER_CONDITION:
			{
				int const iError = CheckEnableUseEffect( pkDef, kItem.ItemNo(), iNewEffect1, pkCaster );
				if( 0 == iError )
				{
					++iUseEffectCount;
				}
				else
				{
					if ( 0 < iError )
					{// -1�ΰ�� Send�� ���� �ʴ´�.
						pkCaster->SendWarnMessage( iError );
					}
					iUseEffectCount = 0;
				}
			}break;
		default:
			{
				++iUseEffectCount;
			}break;
		}
		if( iUseEffectCount )
		{
			int iError = 0;
			GET_DEF(CEffectDefMgr, kEffectDefMgr);
			CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect1);
			if( pkEffectDef )
			{
				int const iRecoveryJSExhaustionRate = pkEffectDef->GetAbil(AT_JS_RECOVERY_EXHAUSTION_R);
				int const iRecoveryJSExhaustionValue = pkEffectDef->GetAbil(AT_JS_RECOVERY_EXHAUSTION_C);
				int const iRecoveryJSBlessPointRate = pkEffectDef->GetAbil(AT_JS_RECOVERY_BLESSPOINT_R);
				int const iRecoveryJSBlessPointValue = pkEffectDef->GetAbil(AT_JS_RECOVERY_BLESSPOINT_C);
				int const iRecoveryJSProductPointRate = pkEffectDef->GetAbil(AT_JS_RECOVERY_PRODUCTPOINT_R);
				int const iRecoveryJSProductPointValue = pkEffectDef->GetAbil(AT_JS_RECOVERY_PRODUCTPOINT_C);
				if( iRecoveryJSExhaustionRate
				||	iRecoveryJSExhaustionValue
				||	iRecoveryJSBlessPointRate
				||	iRecoveryJSBlessPointValue
				||	iRecoveryJSProductPointRate
				||	iRecoveryJSProductPointValue )
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
					if( pkPlayer )
					{
						if( iRecoveryJSExhaustionRate || iRecoveryJSExhaustionValue )
						{
							int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1�� �� ��ų�� �Ƿε��� �ִ�
							if( 0 >= iMaxExhaustion
							||	0 >= pkPlayer->JobSkillExpertness().CurExhaustion() )
							{
								iError = 25028;
							}
						}
						if( iRecoveryJSBlessPointRate || iRecoveryJSBlessPointValue )
						{
							int const iMaxBlessPoint = JobSkillExpertnessUtil::GetMaxExhaustion_2ND(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 2�� �� ��ų�� �ູ�������� �ִ�
							if( 0 >= iMaxBlessPoint
							&&	0 >= pkPlayer->JobSkillExpertness().CurBlessPoint() )
							{
								iError = 25029;
							}
						}
						if( iRecoveryJSProductPointRate || iRecoveryJSProductPointValue )
						{
							int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1�� �� ��ų�� �Ƿε��� �ִ�
							if( 0 >= iMaxExhaustion
							||	0 >= pkPlayer->JobSkillExpertness().CurExhaustion() )
							{
								iError = 25028;
							}
						}
					}
				}
			}
			if( 0 != iError )
			{
				iUseEffectCount = 0;
				pkCaster->SendWarnMessage( iError );
			}
		}
	}

	return ( 0 < iUseEffectCount );
}

bool PgAction_ReqUseItem::CheckItemEffect_Pet(CItemDef const* const pkDef, CUnit* pkCaster, PgPet *pkTargetPet, PgBase_Item const & kItem)
{
	int iUseEffectCount = 0;

	int const iItemMP = pkDef->GetAbil(AT_MP);
	int const iRateMP = pkDef->GetAbil(AT_R_MAX_MP);
	if(	iItemMP || iRateMP)
	{
		int const iNowMP = pkTargetPet->GetAbil(AT_MP);
		int const iMaxMP = pkTargetPet->GetAbil(AT_C_MAX_MP);
		int const iAddMP = iItemMP + (iItemMP * pkTargetPet->GetAbil(AT_C_MP_POTION_ADD_RATE) + iRateMP * iMaxMP) / ABILITY_RATE_VALUE;
		int const iRetMP = std::min(iMaxMP, iAddMP+iNowMP);

		if ( iRetMP != iNowMP )
		{
			++iUseEffectCount;
		}
	}

	int const iCustomType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
	int const iCustomValue3 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);

	switch( iCustomType )
	{
	case UICT_REDICE_PET_OPTION:
		{
			SItemPos kTargetPos;
			m_kOrgPacket.Pop(kTargetPos);

			PgBase_Item kTargetItem;
			if(S_OK != pkCaster->GetInven()->GetItem(kTargetPos, kTargetItem))
			{
				Ret(E_REDICE_NOT_FOUND_PET);
				return false;
			}

			if ( PgItem_PetInfo::ms_kPetItemEquipPos == kTargetPos )
			{
				if ( EPET_TYPE_1 != pkTargetPet->GetPetType() )
				{
					Ret(E_REDICE_CANT_GENOPT_PET);
					return false;
				}
			}
			else
			{
				PgItem_PetInfo *pkPetInfo = NULL;
				if ( !kTargetItem.GetExtInfo( pkPetInfo ) )
				{
					Ret(E_REDICE_NOT_FOUND_PET);
					return false;
				}

				GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
				PgClassPetDef kPetDef;
				if ( !kClassPetDefMgr.GetDef( pkPetInfo->ClassKey(), &kPetDef ) )
				{
					Ret(E_REDICE_NOT_FOUND_PET);
					return false;
				}

				if ( EPET_TYPE_1 != kPetDef.GetPetType() )
				{
					Ret(E_REDICE_CANT_GENOPT_PET);
					return false;
				}
			}

			SEnchantInfo kNewEnchantInfo;

			GET_DEF(PgPetHatchMgr, kPetHatchMgr);
			kPetHatchMgr.RedicePetOption( kTargetItem, kNewEnchantInfo );
			m_kAddonPacket.Push(kNewEnchantInfo);

			m_kOrder.push_back( SPMO(IMET_MODIFY_ENCHANT, pkCaster->GetID(), SPMOD_Enchant( kTargetItem, kTargetPos, kNewEnchantInfo)) );
			++iUseEffectCount;
		}break;
	case UICT_PET_RENAME:
	case UICT_PET_COLORCHANGE:
	case UICT_HARDCORE_KEY:
		{// ���⼭ ����ϸ� �ȵ�
			return false;
		}break;
	case UICT_PET_FEED:
	case UICT_ACTIVATE_SKILL:
		{
			++iUseEffectCount;
		}break;
	case UICT_ELIXIR:
		{//
			int iNowAbil = AT_MP;
			int iMaxAbil = AT_C_MAX_MP;

			int const iNowValue = pkTargetPet->GetAbil(iNowAbil);
			int const iMaxValue = pkTargetPet->GetAbil(iMaxAbil);
			if(iMaxValue > iNowValue)
			{
				//ȸ���� �ʿ���.
				int iRecoverValue = std::min<int>( iMaxValue - iNowValue, kItem.Count() * 10 );// ���� 1����Ʈ�� 10��ŭ. ȸ��
				int const iCheckValue = 9;
				if ( iCheckValue < iRecoverValue )
				{
					int iDecDur = iRecoverValue / 10;
					if ( 0 == iDecDur )
					{
						iDecDur = 1;
					}

					if ( UT_PET == pkCaster->UnitType() )
					{
						SPMOD_Modify_Count_Pet kDelData( pkCaster->GetID(), kItem, m_kCasterPos, -iDecDur );//1�� ����.
						SPMO kIMO( IMC_MODIFY_PET_INV|IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkCaster->Caller(), kDelData);
						m_kOrder.push_back(kIMO);
					}
					else
					{
						SPMOD_Modify_Count kDelData( kItem, m_kCasterPos, -iDecDur );//1�� ����.
						SPMO kIMO( IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkCaster->GetID(), kDelData);
						m_kOrder.push_back(kIMO);

					}

					int iNewValue = std::min( (iNowValue + iRecoverValue), iMaxValue ) - iNowValue;
					m_kAddonPacket.Push(iNowAbil);
					m_kAddonPacket.Push(iNewValue);
					++iUseEffectCount;
				}
			}
		}break;	
	}

	__int64 const i64AddExp = pkDef->GetAbil64(AT_EXPERIENCE);
	if ( i64AddExp )
	{
		GET_DEF(PgClassPetDefMgr,kClassPetDefMgr);
		PgActPet_AddExp kAction(kClassPetDefMgr);

		SPMOD_AddAbilPet kAddAbilPet( pkTargetPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
		if ( true == kAction.PopAbilList( pkTargetPet, i64AddExp, kAddAbilPet.kAbilList ) )
		{
			m_kOrder.push_back( SPMO( IMET_PET, pkCaster->GetID(), kAddAbilPet ) );
			++iUseEffectCount;
		}
	}

	int const iNewEffect1 = pkDef->GetAbil(AT_EFFECTNUM1);
	if(iNewEffect1)
	{
		switch(iCustomValue2)
		{
		case UICT2_CHECK_PLAYER_CONDITION:
			{
			}break;
		default:
			{
				++iUseEffectCount;
			}break;
		}
	}

	return ( 0 < iUseEffectCount );
}

int PgAction_ReqUseItem::CheckEnableUseEffect( CItemDef const* const pkDef, int const iItemNo, int const iEffectNo, CUnit* pkTarget )
{
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iEffectNo);
	if (NULL == pkEffectDef)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << L"EffectDef Cannot find EffectNo[" << iEffectNo << L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return -1;
	}

	int iError = -1;
	bool bCheckHP = false;
	bool bCheckMP = false;
	int iValue = pkEffectDef->GetAbil(AT_HP);
	if(iValue != 0)
	{	// HP
		bCheckHP = true;
		if(pkTarget->GetAbil(AT_HP) < pkTarget->GetAbil(AT_C_MAX_HP))
		{
			iError = 0;
		}
		else
		{
			iError = 310100;
		}
	}

	if( iError && ((iValue = pkEffectDef->GetAbil(AT_MP)) > 0) )
	{	// MP
		bCheckMP = true;
		if(pkTarget->GetAbil(AT_MP) < pkTarget->GetAbil(AT_C_MAX_MP))
		{
			iError = 0;
		}
		else
		{
			iError = 310101;
		}
	}


	if( (true == bCheckHP || true == bCheckMP ) && iError )
	{// HP, MP ȸ������ ��� HP,MP �� MAX�̸� ������ ���� �ؾ� �Ѵ�.
		return iError;
	}
	else
	{
		// Effect checking....
		if(!pkDef)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return -1;
		}

		int const iNewEffect1 = pkDef->GetAbil(AT_EFFECTNUM1);
		if (iNewEffect1)
		{
			CEffect const * const pkEffect = pkTarget->GetEffectMgr().FindInGroup(iItemNo, true);
			if (pkEffect != NULL)
			{
				GET_DEF(CEffectDefMgr, kEffectDefMgr);
				CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect1);
				if (pkEffectDef == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot get ItemEffect Def ItemNo=") << iItemNo << _T(", Effect=") << iNewEffect1);
					return -1;
				}
				int const iOldLevel = (pkEffect->GetType() == EFFECT_TYPE_ITEM) ? CItemEffect::GetLevel(pkEffect->GetKey()): CEffect::GetLevel(pkEffect->GetKey());
				if (iOldLevel < CItemEffect::GetLevel(iItemNo))
				{
					iError = 0;
				}
			}
			else
			{
				iError = 0;
			}

			// ����ġ ������ �˻�
			GET_DEF(CEffectDefMgr, kEffectDefMgr);
			CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect1);
			if( pkEffectDef )
			{
				if( 0 != pkEffectDef->GetAbil(AT_ADD_EXP_PER)
				&&	0 < pkTarget->GetAbil(AT_ADD_EXP_PER) )
				{
					iError = EventTaiwanEffectQuest::iCantDupExpPotionMsg;
				}
			}
		}
	}

	return iError;
}

bool PgAction_ReqUseItem::FilterQuest(CUnit * pkUnit,CItemDef const* const pItemDef)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(NULL == pkPlayer)
	{
		return false;
	}

	PgMyQuest const * pkQuest = pkPlayer->GetMyQuest();

	int iQuestNo = 0;

	for(int i = AT_CHECK_EXIST_ENDQUEST_MIN;i < AT_CHECK_EXIST_ENDQUEST_MAX;++i)
	{
		iQuestNo = pItemDef->GetAbil(i);		// �ش� ����Ʈ�� �Ϸ� �ؾ� ��� ���� �ϴ�.
		if(iQuestNo && false == pkQuest->IsEndedQuest(iQuestNo))
		{
			pkPlayer->SendWarnMessage2(699993,iQuestNo);
			return false;
		}
	}

	for(int i = AT_CHECK_WANT_ENDQUEST_MIN;i < AT_CHECK_WANT_ENDQUEST_MAX;++i)
	{
		iQuestNo = pItemDef->GetAbil(i);		// �ش� ����Ʈ�� �Ϸ� ���� �ʾƾ� ��� ���� �ϴ�.
		if(iQuestNo && true == pkQuest->IsEndedQuest(iQuestNo))
		{
			pkPlayer->SendWarnMessage2(699994,iQuestNo);
			return false;
		}
	}

	for(int i = AT_CHECK_EXIST_INGQUEST_MIN;i < AT_CHECK_EXIST_INGQUEST_MAX;++i)
	{
		iQuestNo = pItemDef->GetAbil(i);		// �ش� ����Ʈ�� �������̾�߸� ��� ���� �ϴ�.
		if(iQuestNo && pkQuest->IsIngQuest(iQuestNo))
		{
			pkPlayer->SendWarnMessage2(699991,iQuestNo);
			return false;
		}
	}

	for(int i = AT_CHECK_WANT_INGQUEST_MIN;i < AT_CHECK_WANT_INGQUEST_MAX;++i)
	{
		iQuestNo = pItemDef->GetAbil(i);		// �ش� ����Ʈ�� ���� ���̸� ��� �Ұ��� �ϴ�.
		if(iQuestNo && pkQuest->IsIngQuest(iQuestNo))
		{
			pkPlayer->SendWarnMessage2(699992,iQuestNo);
			return false;
		}
	}

	return true;
}

int PgAction_ReqUseItem::CheckUseTime( CItemDef const &rkItemDef )
{
	int const iStartTime = rkItemDef.GetAbil(AT_ENABLE_USE_START_TIME);
	int const iEndTime = rkItemDef.GetAbil(AT_ENABLE_USE_END_TIME);

	if(iStartTime > 0 || iEndTime > 0)	// Start/End ��� 0�� ������ �ȵȴ�.(�ϰ� ������ 24~24�� �����ؾ� �Ѵ�.)
	{
		if (iStartTime > 24 || iStartTime > iEndTime)	// 24�óְ� ������ 20��~26�� �̷������� �Է��ϸ� �ȴ�.
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("AT_ENABLE_USE_START_TIME/AT_ENABLE_USE_END_TIME Error StartTime=") << iStartTime << _T(", EndTime=") << iEndTime);
			return 10410;
		}

		BM::DBTIMESTAMP_EX kDBTime;
		kDBTime.SetLocalTime();
		if (iEndTime < 24)
		{
			//if ((iStartTime <= kDBTime.hour) && (kDBTime.hour <= iEndTime)) then TRUE
			if ((iStartTime > kDBTime.hour) || (kDBTime.hour > iEndTime))
			{
				return 10410;
			}
		}
		else
		{
			// if ( (iStartTime <= kDBTime.hour) || (kDBTime.hour <= iEndTime%24) ) then TRUE
			if ( (iStartTime > kDBTime.hour) && (kDBTime.hour > iEndTime%24) )
			{
				return 10410;
			}
		}
	}

	return 0;
}

bool PgAction_ReqUseItem::DoAction( CUnit* pkItemOwner, CUnit* pkPlayer )
{
	if ( !pkItemOwner )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !pkPlayer )
	{
		return DoAction( pkItemOwner, pkItemOwner );
	}

	if ( !m_pkGround )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_pkGround->IsUseItem() )
	{
		pkItemOwner->SendWarnMessage(302115);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pkItemOwner->GetAbil(AT_CANNOT_USEITEM))
	{
		pkPlayer->SendWarnMessage(10410);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory* pkInven = pkItemOwner->GetInven();
	if( NULL == pkInven )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bIsStateOk = false;
	PgBase_Item kItem;
	pkInven->GetItem( m_kCasterPos, kItem );

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const* pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iGenderLimit = pItemDef->GetAbil(AT_GENDERLIMIT);
	if(0 == (pkItemOwner->GetAbil(AT_GENDER) & iGenderLimit))
	{
		int iWarnMsg = 22028;
		switch( iGenderLimit )
		{
		case GWL_MALE:{iWarnMsg = 22021;}break;
		case GWL_FEMALE:{iWarnMsg = 22022;}break;
		case GWL_PET_MALE:
		case GWL_PET_FEMALE:
		case GWL_PET_UNISEX:
			{
				iWarnMsg = 22029;
			}break;
		default:{iWarnMsg = 22028;}break;
		}
		pkPlayer->SendWarnMessage(iWarnMsg);
		return false;
	}

	__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
	if ((0 != i64ClassLimit) && !IS_CLASS_LIMIT(i64ClassLimit, pkItemOwner->GetAbil(AT_CLASS)))
	{
		pkPlayer->SendWarnMessage(22024);
		return false;
	}

	int const iChkUseTime = CheckUseTime( *pItemDef );
	if ( iChkUseTime )
	{
		pkPlayer->SendWarnMessage(iChkUseTime);
		return false;
	}
	
	if( false == FilterQuest( pkPlayer, pItemDef ) )
	{
		return false;
	}

	if(!PgBase_Item::IsEmpty(&kItem))
	{
		if(true == kItem.IsUseTimeOut())
		{
			pkPlayer->SendWarnMessage(10410);
			return false;
		}

		if( pkItemOwner->IsAlive() )
		{
			if(ITEM_USE_STATE_DEAD != pItemDef->GetAbil(AT_ITEM_USE_STATUS))
			{//��� �������� �� �� ����.
				bIsStateOk = true;
			}
		}
		else
		{//��� �ߴٸ�.
			if(ITEM_USE_STATE_DEAD != pItemDef->GetAbil(AT_ITEM_USE_STATUS))
			{
				pkPlayer->SendWarnMessage(10410);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			else
			{//����ø� �� �� �ִ� ������.
				bIsStateOk = true;
			}
		}
	}

	if(!bIsStateOk)
	{//�� �� �ִ� ���°� �ƴ�.
		pkPlayer->SendWarnMessage(10410);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if((0 < pItemDef->GetAbil(AT_ENABLE_USE_GROUPNO)) && (m_pkGround->GroundKey().GroundNo() != pItemDef->GetAbil(AT_ENABLE_USE_GROUPNO)))
	{
		pkPlayer->SendWarnMessage(20027);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iMissionNo = pItemDef->GetAbil(AT_MISSIONITEM_MISSION_NO);
	int const iMissionLv = pItemDef->GetAbil(AT_MISSIONITEM_LEVELTYPE);
	if(iMissionNo || iMissionLv)
	{
		if(0 == (m_pkGround->GetAttr()&GATTR_MISSION))
		{
			pkPlayer->SendWarnMessage(20027);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		PgMissionGround * pkMissionGround = dynamic_cast<PgMissionGround*>(m_pkGround);
		if(NULL == pkMissionGround)
		{
			pkPlayer->SendWarnMessage(20027);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if(iMissionNo && iMissionNo!=pkMissionGround->GetMissionNo())
		{
			pkPlayer->SendWarnMessage(20027);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if(iMissionLv && false==pkMissionGround->IsMissionItemLevel(iMissionLv))
		{
			pkPlayer->SendWarnMessage(20027);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	const T_GNDATTR kGroundAttr = m_pkGround->GetAttr();
	if((ICMET_Cant_UsePVP & pItemDef->GetAbil(AT_ATTRIBUTE)) && (GATTR_PVP == kGroundAttr || GATTR_BATTLESQUARE == kGroundAttr))
	{	// ����� �� ���� ����Դϴ�.
		pkPlayer->SendWarnMessage(20027);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCurLevel = pkItemOwner->GetAbil(AT_LEVEL);
	int const iMinLv = pItemDef->GetAbil(AT_LEVELLIMIT);
	int const iMaxLv = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
	if( ((iMinLv > 0) && (iCurLevel < iMinLv))
	 || ((iMaxLv > 0) && (iCurLevel > iMaxLv)) )
	{
		// �����޽��� ����� �ʿ��ϸ�, �Ʒ� �Լ� ���
		pkPlayer->SendWarnMessage(22027);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	T_GNDATTR const iNotAttr = (T_GNDATTR)(pItemDef->GetAbil(AT_NOTAPPLY_MAPATTR));
	T_GNDATTR const iCanAttr = (T_GNDATTR)(pItemDef->GetAbil(AT_CAN_GROUND_ATTR));
	bool bCantUseGround = (0 != (iNotAttr & m_kGndAttr));
	bCantUseGround = ((0 != iCanAttr)? 0 == (m_kGndAttr & iCanAttr): false) || bCantUseGround;
	if( bCantUseGround )
	{
		// ����� �� ���� ����Դϴ�.
		pkPlayer->SendWarnMessage(20027);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		PgPlayer* pkTrgPlayer = dynamic_cast< PgPlayer* >(pkPlayer);
		if( pkTrgPlayer )
		{
			{
				int const iLimitUseExhaustion = pItemDef->GetAbil(AT_JS_USE_ITEM_LIMIT_EXHAUSTION_R);
				if( 0 < iLimitUseExhaustion )
				{
					int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkTrgPlayer->GetPremium(), pkTrgPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1�� �� ��ų�� �Ƿε��� �ִ�
					int const iCurExhaustion = pkTrgPlayer->JobSkillExpertness().CurExhaustion();
					int const iRateVal = iMaxExhaustion * iLimitUseExhaustion / ABILITY_RATE_VALUE;
					if( iRateVal > iCurExhaustion )
					{
						BM::Stream kErrPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_USE_RATE_EXHAUSTION);
						kErrPacket.Push( iLimitUseExhaustion );
						pkTrgPlayer->Send(kErrPacket);
						return false;
					}
				}
			}
			{
				int const iLimitUseBlessPoint = pItemDef->GetAbil(AT_JS_USE_ITEM_LIMIT_BLESSPOINT_R);
				if( 0 < iLimitUseBlessPoint )
				{
					int const iCurBlessPoint = pkTrgPlayer->JobSkillExpertness().CurBlessPoint();
					int const iMaxBlessPoint = JobSkillExpertnessUtil::GetMaxExhaustion_2ND(pkTrgPlayer->GetPremium(), pkTrgPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 2�� �� ��ų�� �ູ�������� �ִ�
					int const iRateVal = iMaxBlessPoint * iLimitUseBlessPoint / ABILITY_RATE_VALUE;
					if( iRateVal > iCurBlessPoint )
					{
						BM::Stream kErrPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_USE_RATE_BLESSPOINT);
						kErrPacket.Push( iLimitUseBlessPoint );
						pkTrgPlayer->Send(kErrPacket);
						return false;
					}
				}
			}
			{
				int const iLimitUseProductPoint = pItemDef->GetAbil(AT_JS_USE_ITEM_LIMIT_PRODUCTPOINT_R);
				if( 0 < iLimitUseProductPoint )
				{
					int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkTrgPlayer->GetPremium(), pkTrgPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1�� �� ��ų�� �Ƿε��� �ִ�
					int const iCurExhaustion = pkTrgPlayer->JobSkillExpertness().CurExhaustion();
					int const iRateVal = iMaxExhaustion * iLimitUseProductPoint / ABILITY_RATE_VALUE;
					if( iRateVal > iCurExhaustion )
					{
						BM::Stream kErrPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_USE_RATE_EXHAUSTION);
						kErrPacket.Push( iLimitUseProductPoint );
						pkTrgPlayer->Send(kErrPacket);
						return false;
					}
				}
			}
		}
	}

	if (!pkInven->CheckCoolTime(kItem.ItemNo(), PgInventory::EICool_UseItem, m_dwCoolTime))
	{
		// ������ ��Ÿ���� ���� �ֽ��ϴ�.
		pkPlayer->SendWarnMessage(20028);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BYTE const byUseInvenType = ( UT_PET == pkItemOwner->UnitType() ? pItemDef->PetInvType() : pItemDef->PrimaryInvType() );
	if(	pItemDef->IsType(ITEM_TYPE_CONSUME) 
	&& (byUseInvenType == m_kCasterPos.x))//���� ���� �κ� �ڸ��� �ƴϸ� ��� �Ұ�. -> ��. â�� �������� �ȵȴٴ� ���.
	{
		if ( !m_bAnsServerCheck )
		{
			int const iMapNo = pItemDef->GetAbil(AT_MAP_NUM);
			if ( iMapNo )
			{// Scroll Item�� ���ͷ� ���� üũ�� �޾ƾ� �Ѵ�.

				PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkItemOwner);
				if ( pkUser )
				{
					if( true == pkItemOwner->IsItemEffect(SAFE_FOAM_EFFECTNO) )
					{
						pkPlayer->SendWarnMessage(10410);
						return false;
					}

					BM::Stream kTPacket(PT_M_T_REQ_USEITEM_CHECK,UISCT_SCROLL);
					kTPacket.Push(pkUser->GetMemberGUID());
					kTPacket.Push(pkUser->GetID());
					kTPacket.Push(m_kGndKey);
					kTPacket.Push(m_kCasterPos);
					kTPacket.Push(iMapNo);
					SendToCenter(kTPacket);
				}
				else
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				// ���Ŀ� �ƹ��͵� ���ϰ� ����
				return true;
			}
		}

		CUnit *pkTargetUnit = PgAction_UseItem::GetTargetUnit( pItemDef, pkItemOwner, m_pkGround );
		if ( !pkTargetUnit )
		{
			return false;
		}

		// Item ����̶��, ������ ����� �� �ִ��� �˻� �� �ش�.
		DWORD Result = 0;
		if( false == CheckItemEffect( pItemDef, pkItemOwner, pkTargetUnit, kItem, Result ) )
		{
			if( (Result & CIEffectResult::CIER_FULL_HP)
				&& (Result & CIEffectResult::CIER_FULL_MP) )
			{
				pkPlayer->SendWarnMessage(310102);
			}
			else
			{
				if( Result & CIEffectResult::CIER_FULL_HP )
				{
					pkPlayer->SendWarnMessage(310100);
				}
				if( Result & CIEffectResult::CIER_FULL_MP )
				{
					pkPlayer->SendWarnMessage(310101);
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
		switch( iCustomType )
		{
		case UICT_REDICE_ITEM_OPTION:
		case UICT_REPAIR_ITEM:
			{//CheckInstantEffect �Լ� �ȿ��� ���ȴ�. ���⼱ ��� ���� ����
			}break;
		case UICT_CALL_SAFE:
		case UICT_CALL_SHARE_SAFE:
			{
				if(0 == kItem.EnchantInfo().IsTimeLimit()) // �Ⱓ�� �������� ������ ���
				{
					SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
					m_kOrder.push_back(kIMO);
				}
				else
				{
					m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), SPMOD_Enchant(kItem, m_kCasterPos, kItem.EnchantInfo())));
				}
			}break;
		case UICT_CALL_STORE:// �Ⱓ�� 
			{
				if(0 == kItem.EnchantInfo().IsTimeLimit()) // �Ⱓ�� �������� ������ ���
				{
					SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
					m_kOrder.push_back(kIMO);
				}
				else
				{
					PgAction_UseItem kAction( m_pkGround, kItem, 1);
					kAction.DoAction(pkItemOwner, NULL);
				}
			}break;
		case UICT_ELIXIR:
			{// ������ ������ CheckInstantEffect �Լ� �ȿ��� ���ȴ�. ���⼱ ��� ���� ����
			}break;
		case UICT_REST:
		case UICT_REST_EXP:
		case UICT_COUPLE_ITEM:
			{
				if(kItem.EnchantInfo().IsTimeLimit()) // �Ⱓ�� �������� ������ ���
				{
					PgAction_UseItem kAction( m_pkGround, kItem, 1);
					kAction.DoAction(pkItemOwner, NULL);
					m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkItemOwner->GetID(), SPMOD_Enchant(kItem, m_kCasterPos, kItem.EnchantInfo())));
				}
				else
				{
					SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
					m_kOrder.push_back(kIMO);
				}
			}break;
		case UICT_GUILDMarkChange1:
			{
				BYTE cNewGuildMark = 0;
				m_kOrgPacket.Pop(cNewGuildMark);

				BM::Stream kPacket(PT_M_I_REQ_RESUME_GUILD_COMMAND);
				kPacket.Push( static_cast< BYTE >(GC_M_ChangeMark1) );
				kPacket.Push( cNewGuildMark );

				m_kAddonPacket = kPacket;

				SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
				SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
				m_kOrder.push_back(kIMO);
			}break;
		case UICT_PET_EGG:
			{
				// ���� ��ȭ ���Ѿ� �ϳ�~~
				SPMOD_HatchPet kData( kItem, m_kCasterPos, -1 );// 1�� ����

				GET_DEF(PgPetHatchMgr, kPetHatchMgr);
				if ( !kPetHatchMgr.PopPet( pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1), kData.kPetItem ) )
				{
					return false;
				}
				
				SPMO kIMO( IMET_PET, pkItemOwner->GetID(), kData );
				m_kOrder.push_back( kIMO );

				m_kCause = CIE_HatchPet;
			}break;
		case UICT_PET_FEED:
			{
				SItemPos kPetItemPos = PgItem_PetInfo::ms_kPetItemEquipPos;// ������ �������� �ʿ��� �����ϰ� �ִ� ������
				m_kOrgPacket.Pop( kPetItemPos );
				
				PgBase_Item kPetItem;
				if ( S_OK != pkInven->GetItem( kPetItemPos, kPetItem ) )
				{
					return false;
				}

				PgItem_PetInfo *pkPetInfo = NULL;
				if ( !kPetItem.GetExtInfo( pkPetInfo ) )
				{
					return false;
				}

				GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
				PgClassPetDef kPetDef;
				if ( !kClassPetDefMgr.GetDef( pkPetInfo->ClassKey(), &kPetDef ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Fount PetDef<C:" << pkPetInfo->ClassKey().iClass << L",Lv:" << pkPetInfo->ClassKey().nLv << L"> ItemGuid<" << kItem.Guid() << L">" );
					return false;
				}

				int const iType2 = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_2 );
				int const iType3 = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_3 );
				switch( iType2 )
				{
				case PET_FEED_TYPE_1:
				case PET_FEED_TYPE_2:
					{
						if ( static_cast<BYTE>(iType2) != kPetDef.GetPetType() && 
							 static_cast<BYTE>(iType3) != kPetDef.GetPetType() ) //2���� ��Ÿ�� ������ ����� �� �ְ� ����
						{
							return false;
						}

						if ( !kPetItem.EnchantInfo().IsTimeLimit() )
						{
							return false;// �Ⱓ�� ���� �ƴϱ� ������ ������ �� �ʿ䰡 ����.
						}

						__int64 const i64AddTime = static_cast<__int64>(pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_1 ));
						__int64 const i64UseAbleTime = kPetItem.GetUseAbleTime();
						int const iRemainDay = ceil(i64UseAbleTime / static_cast<float>(86400));	//60*60*24=86400

						if( MAX_CASH_ITEM_TIMELIMIT < iRemainDay+i64AddTime)
						{
							pkPlayer->SendWarnMessage(162);
							return false;
						}

						SPMOD_Enchant kEnchant( kPetItem, kPetItemPos, kPetItem.EnchantInfo() );
						if ( 0i64 < i64UseAbleTime )
						{
							__int64 const i64NewUseTime = kPetItem.EnchantInfo().UseTime() + i64AddTime;
							if( MAX_CASH_ITEM_TIMELIMIT < i64NewUseTime)
							{
								if ( !kPetItem.SetUseTime( UIT_DAY, std::min(MAX_CASH_ITEM_TIMELIMIT,iRemainDay+i64AddTime) ) )
								{
									return false;
								}
							}
							else
							{
								if ( !kPetItem.AddUseTime( UIT_DAY, i64AddTime ) )
								{
									return false;
								}
							}
						}
						else
						{
							if ( !kPetItem.SetUseTime( UIT_DAY, i64AddTime ) )
							{
								return false;
							}
						}

						kEnchant.NewEnchantInfo( kPetItem.EnchantInfo() );
						kEnchant.NewGenDate( kPetItem.CreateDate() );

						SPMO kIMO( IMET_MODIFY_ENCHANT, pkItemOwner->GetID(), kEnchant );

						if (	 (kPetItemPos == PgItem_PetInfo::ms_kPetItemEquipPos) 
							&&	!m_pkGround->GetPet( kPetItem.Guid() )
						)
						{
							kIMO.Cause( kIMO.Cause() | IMET_PET );
						}

						m_kOrder.push_back( kIMO );
					}break;
				case PET_FEED_TRAINING:
					{
						if ( EPET_TYPE_2 != kPetDef.GetPetType() )
						{
							return false;
						}

						int iAddHealth = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_3 );
						int iAddTimeDay = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_1 );

						PgItem_PetInfo::SStateValue kHealth(pkPetInfo->GetState_Health());
						if ( true == kHealth.IsUseTimeOut(CGameTime::HOUR) )
						{
							__int64 const i64NowTime = (g_kEventView.GetLocalSecTime(CGameTime::HOUR) + 1i64 );// 1�ð��� ����� �д��� ���ظ� ��ȸ
							kHealth.Time(static_cast<int>(i64NowTime));
							kHealth.Value(iAddHealth);
						}
						else
						{
							if ( kHealth.Value() != iAddHealth )
							{// ���ƾ߸� ������ ����
								return false;
							}
						}

						kHealth.Time( kHealth.Time() + (iAddTimeDay * 24) );

						SPMOD_AddAbilPet kAddAbilPet( kPetItem.Guid(), kPetItemPos );
						kAddAbilPet.kAbilList.push_back( SPMOD_SAbil( AT_HEALTH, static_cast<__int64>(kHealth.Get()) ) );

						SPMO kIMO( IMET_PET, pkItemOwner->GetID(), kAddAbilPet );
						m_kOrder.push_back( kIMO );
						m_kCause = CIE_SetAbilPet;
					}break;
				case PET_FEED_PLAYING:
					{
						if ( EPET_TYPE_2 != kPetDef.GetPetType() )
						{
							return false;
						}

						int iAddMental = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_3 );
						int iAddTimeDay = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_1 );

						PgItem_PetInfo::SStateValue kMental(pkPetInfo->GetState_Mental());
						if ( true == kMental.IsUseTimeOut(CGameTime::HOUR) )
						{
							__int64 const i64NowTime = (g_kEventView.GetLocalSecTime( CGameTime::HOUR ) + 1i64 );// 1�ð��� ����� �д��� ���ظ� ��ȸ
							kMental.Time(static_cast<int>(i64NowTime));
							kMental.Value(iAddMental);
						}
						else
						{
							if ( kMental.Value() != iAddMental )
							{// ���ƾ߸� ������ ����
								return false;
							}
						}

						kMental.Time( kMental.Time() + (iAddTimeDay * 24) );

						SPMOD_AddAbilPet kAddAbilPet( kPetItem.Guid(), kPetItemPos );
						kAddAbilPet.kAbilList.push_back( SPMOD_SAbil( AT_MENTAL, static_cast<__int64>(kMental.Get()) ) );

						SPMO kIMO( IMET_PET, pkItemOwner->GetID(), kAddAbilPet );
						m_kOrder.push_back( kIMO );
						m_kCause = CIE_SetAbilPet;
					}break;

				case PET_FEED_MP_DRINK:
					{
						PgPet *pkPet = dynamic_cast<PgPet*>(pkTargetUnit);
						if ( !pkPet )
						{
							return false;
						}

						if ( kPetItem.Guid() != pkPet->GetID() )
						{
							return false;
						}

						SPMOD_AddAbilPet kAddAbilPet( kPetItem.Guid(), PgItem_PetInfo::ms_kPetItemEquipPos );

						int const iAddMP = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_3 );
						int const iNowMP = pkPet->GetAbil( AT_MP );
						int const iMaxMP = pkPet->GetAbil( AT_C_MAX_MP );
						int iNewMP = std::min( iNowMP + iAddMP, iMaxMP );
						if ( iNowMP == iNewMP )
						{
							return false;
						}
					}break;
				default:
					{
						CAUTION_LOG( BM::LOG_LV4, __FL__ << L"Unknown UICT_PET_FEED's AT_USE_ITEM_CUSTOM_VALUE_2<" << iType2 << L"> ItemNo<" << kItem.ItemNo() << L">" );
						return false;
					}break;
				}

				SPMO kIMO2( IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), SPMOD_Modify_Count( kItem, m_kCasterPos, -1) );
				m_kOrder.push_back( kIMO2 );
			}break;
		case UICT_ACTIVATE_SKILL:
			{
				SItemPos kPetItemPos = PgItem_PetInfo::ms_kPetItemEquipPos;// ������ �������� �ʿ��� �����ϰ� �ִ� ������
				m_kOrgPacket.Pop( kPetItemPos );

				PgBase_Item kPetItem;
				if ( S_OK != pkInven->GetItem( kPetItemPos, kPetItem ) )
				{
					return false;
				}

				PgItem_PetInfo *pkPetInfo = NULL;
				if ( !kPetItem.GetExtInfo( pkPetInfo ) )
				{
					return false;
				}

				GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
				PgClassPetDef kPetDef;
				SClassKey const kPetClassKey(pkPetInfo->ClassKey());
				if ( !kClassPetDefMgr.GetDef( kPetClassKey, &kPetDef ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Fount PetDef<C:" << kPetClassKey.iClass << L",Lv:" << kPetClassKey.nLv << L"> ItemGuid<" << kItem.Guid() << L">" );
					return false;
				}

				CONT_DEFCLASS_PET_SKILL const *pkDefPetSkill = NULL;
				g_kTblDataMgr.GetContDef(pkDefPetSkill);

				CONT_DEFCLASS_PET_SKILL::const_iterator skill_itr = pkDefPetSkill->find( kPetDef.GetSkillDefID() );
				if ( skill_itr == pkDefPetSkill->end() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Fount PetSkillDef<C:" << kPetClassKey.iClass << L",Lv:" << kPetClassKey.nLv << L"> SkillDefID<" << kPetDef.GetSkillDefID() << L">" );
					return false;
				}
				
				CONT_DEFCLASS_PET_SKILL::mapped_type const &kContElement = skill_itr->second;
				size_t const index = kContElement.find( PgDefClassPetSkillFinder(kPetClassKey.nLv) );
				if ( BM::PgApproximate::npos == index )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Fount PetSkillDef<C:" << kPetClassKey.iClass << L",Lv:" << kPetClassKey.nLv << L">" );
					return false;
				}

				unsigned int const iSkillIndex = static_cast<unsigned int>(pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_1 ));
				if ( !iSkillIndex || iSkillIndex > MAX_PET_SKILLCOUNT )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Error SkillIndex <" << iSkillIndex << L"> ItemNo<" << kItem.ItemNo() << L">" );
					return false;
				}

				CONT_DEFCLASS_PET_SKILL::mapped_type::value_type const &kElement = kContElement.at(index);
				if ( !kElement.iSkillNo[iSkillIndex-1] )
				{
					pkPlayer->SendWarnMessage(119);//�����Ҽ� ���� ��ų�Դϴ�.
					return false;
				}
				
				int const iAddMin = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_2 ) * 60;
				
				size_t iIndex;
				if ( !pkPetInfo->FindSkillIndex( iSkillIndex, iIndex ) )
				{
					return false;
				}

				PgItem_PetInfo::SStateValue kStateMaxValue;
				kStateMaxValue.Time(UINT_MAX);

				PgItem_PetInfo::SStateValue kStateValue;
				pkPetInfo->GetSkill( iIndex, kStateValue );
				kStateValue.Value( iSkillIndex );

				if ( kStateValue.Time() == kStateMaxValue.Time() )
				{
					return false;
				}

				unsigned int const iTime = static_cast<unsigned int>(g_kEventView.GetLocalSecTime(CGameTime::MINUTE) - PgItem_PetInfo::SKILL_BASE_TIME);
				if ( iTime < kStateValue.Time() )
				{
					if ( iAddMin > (kStateMaxValue.Time() - kStateValue.Time()) )
					{
						kStateValue.Time(kStateMaxValue.Time());
					}
					else
					{
						kStateValue.Time( kStateValue.Time() + iAddMin );
					}
				}
				else
				{
					if ( iAddMin > (kStateMaxValue.Time() - iTime) )
					{
						kStateValue.Time(kStateMaxValue.Time());
					}
					else
					{
						kStateValue.Time( iTime + iAddMin );
					}
				}

				SPMOD_AddAbilPet kAddAbilPet( kPetItem.Guid(), kPetItemPos );
				kAddAbilPet.kAbilList.push_back( SPMOD_SAbil( AT_MON_SKILL_01 + iIndex, static_cast<__int64>(kStateValue.Get()) ) );

				SPMO kIMO( IMET_PET, pkItemOwner->GetID(), kAddAbilPet );
				m_kOrder.push_back( kIMO );
				m_kCause = CIE_SetAbilPet;

				SPMO kIMO2( IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), SPMOD_Modify_Count( kItem, m_kCasterPos, -1) );
				m_kOrder.push_back( kIMO2 );
			}break;
		case UICT_PET_GROWTH:
			{
				SItemPos kPetItemPos = PgItem_PetInfo::ms_kPetItemEquipPos;// ������ �������� �ʿ��� �����ϰ� �ִ� ������
				m_kOrgPacket.Pop( kPetItemPos );
				
				PgBase_Item kPetItem;
				if ( S_OK != pkInven->GetItem( kPetItemPos, kPetItem ) )
				{
					return false;
				}

				PgItem_PetInfo *pkPetInfo = NULL;
				if ( !kPetItem.GetExtInfo( pkPetInfo ) )
				{
					return false;
				}

				GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
				PgClassPetDef kPetDef;
				if ( !kClassPetDefMgr.GetDef( pkPetInfo->ClassKey(), &kPetDef ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Fount PetDef<C:" << pkPetInfo->ClassKey().iClass << L",Lv:" << pkPetInfo->ClassKey().nLv << L"> ItemGuid<" << kItem.Guid() << L">" );
					return false;
				}

				int const iType2 = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_2 );
				int const iType3 = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_3 );
				if ( static_cast<BYTE>(iType2) != kPetDef.GetPetType() && 
					 static_cast<BYTE>(iType3) != kPetDef.GetPetType() ) //2���� ��Ÿ�� ������ ����� �� �ְ� ����
				{
					pkPlayer->SendWarnMessage(348);
					return false; //�� Ÿ�� ���� ���̸� ����
				}

				int const iGrowthGrade = pItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_1 );

				PgActPet_MakeClassChangeOrder kAction( m_kOrder );
				if ( !kAction.DoAction( pkItemOwner, iGrowthGrade ) )
				{
					return false;
				}

				m_kCause = CIE_SetAbilPet;

				SPMO kIMO2( IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), SPMOD_Modify_Count( kItem, m_kCasterPos, -1) );
				m_kOrder.push_back( kIMO2 );
			}break;
		case UICT_HARDCORE_BOSSKEY:
			{
				PgHardCoreDungeon *pkHDC = const_cast<PgHardCoreDungeon*>(dynamic_cast<PgHardCoreDungeon const*>(m_pkGround));
				if ( pkHDC )
				{
					SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
					return pkHDC->ReqJoinBossGround( dynamic_cast<PgPlayer*>(pkItemOwner), kIMO ); 
				}
				return false;
			}break;
		case UICT_SUPER_GROUND_FEATHER:
		case UICT_SUPER_GROUND_MOVE_FLOOR:
			{
				if( 0 == (m_kGndAttr&GATTR_FLAG_SUPER) )
				{
					pkPlayer->SendWarnMessage(20027);
					return false;
				}
				m_kOrder.push_back( SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), SPMOD_Modify_Count(kItem, m_kCasterPos, -1)) ); //1�� ����.
			}break;
		case UICT_MISSION_DEFENCE_POTION:
			{
				int const iCustomValue = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

				m_kCause = CIE_Mission_DefencePotion;
				BM::Stream kPacket;
				kPacket.Push(static_cast<int>(iCustomValue));
				m_kAddonPacket = kPacket;

				SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
				SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
				m_kOrder.push_back(kIMO);
			}break;
		case UICT_GUILD_INV_EXTEND_LINE:
		case UICT_GUILD_INV_EXTEND_TAB:
			{
				int const iCustomValue1 = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
				EInvType const kInvType = static_cast<EInvType>(IT_NONE); // �ӽ� ��: ���������� ó��
				BYTE const kExpendNum = static_cast<BYTE>(iCustomValue1); // Ȯ�� �� ĭ ��

				if( iCustomType == UICT_GUILD_INV_EXTEND_LINE )
				{
					m_kOrder.push_back( SPMO( IMET_GUILD_INV_EXTEND_LINE, SModifyOrderOwner( pkItemOwner->GetGuildGuid(), OOT_Guild), SMOD_InvExtend( kInvType, kExpendNum ) ) );
				}
				else if( iCustomType == UICT_GUILD_INV_EXTEND_TAB )
				{
					m_kOrder.push_back( SPMO( IMET_GUILD_INV_EXTEND_TAB, SModifyOrderOwner( pkItemOwner->GetGuildGuid(), OOT_Guild), SMOD_InvExtend( kInvType, kExpendNum ) ) );
				}

				// �������� ������ �Ѵ�.
				SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);
				SPMO kIMO( IMET_MODIFY_COUNT | IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData );
				m_kOrder.push_back(kIMO);

				BM::Stream kAddonPacket;
				kAddonPacket.Push(pkItemOwner->GetID());

				PgAction_ReqModifyItem kItemModifyAction( m_kCause, m_pkGround->GroundKey(), m_kOrder, kAddonPacket );
				return kItemModifyAction.DoAction( pkItemOwner->GetGuildGuid() );
			}break;	
		default:
			{//
				//������ ���� �㰡�� ����. ���� ������ ������ �����ִ� �۾����� �� ��.
				SPMOD_Modify_Count kDelData(kItem, m_kCasterPos, -1);//1�� ����.
				SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkItemOwner->GetID(), kDelData);
				m_kOrder.push_back(kIMO);
			}break;
		}

		pkInven->AddCoolTime(kItem.ItemNo(), PgInventory::EICool_UseItem, m_dwCoolTime, pkItemOwner->GetAbil(AT_R_COOLTIME_RATE_ITEM));

		int const iMapNo = pItemDef->GetAbil(AT_MAP_NUM);
		if(iMapNo || UICT_USER_PORTAL == iCustomType || UICT_TELEPORT == iCustomType || UICT_SUPER_GROUND_ENTER_FLOOR == iCustomType)
		{
			return true;// �� �̵� �������� ���⼭ �������ش�. ó���� �ۿ��� �Ѵ�. �� ���� �������� �����......
		}

		// �� �̵� �������� �ƴϸ� ���⼭ ������ ���� ��û ó��
		PgAction_ReqModifyItem kItemModifyAction(m_kCause, m_kGndKey, m_kOrder, m_kAddonPacket, bIsStateOk);
		kItemModifyAction.CashAdd(m_kCashAdd);
		kItemModifyAction.DoAction(pkItemOwner, NULL);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgAction_ReqUseItem::SetCoolTime(DWORD const dwCoolTime)
{
	m_dwCoolTime = dwCoolTime;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_BuildStone
//////////////////////////////////////////////////////////////////////////////
PgAction_BuildStone::PgAction_BuildStone(SGroundKey const &rkGndKey)
{
	m_kGndKey = rkGndKey;
}

bool PgAction_BuildStone::DoAction(CUnit* pUser, CUnit* Nothing)
{
//	bool const bRet = g_kShineStoneMgr.BuildPlayerStone(m_kGndKey, pUser);//���ν��� �ش�� �ʱ�ȭ.
//	return bRet;
	return true;
}

//		Trigger event
PgAction_TriggerStone::PgAction_TriggerStone(SGroundKey const &rkGndKey, BM::GUID const &rkStoneGuid, POINT3 const &rkPos)
{
	m_kGndKey = rkGndKey;
	m_kStoneGuid = rkStoneGuid;
	m_kPos = rkPos;
}

bool PgAction_TriggerStone::DoAction(CUnit *pUser, CUnit* Nothing)
{
//	ContStoneDataVec kVec;
//	if(S_OK != g_kShineStoneMgr.FindData(m_kGndKey, kVec))
//	{
//		return false;
//	}
/*
	ContStoneDataVec::const_iterator kStone_iter = kVec.begin();
	while(kVec.end() != kStone_iter)
	{
		ContStoneDataVec::value_type const pkElement = (*kStone_iter);
		if(	pkElement 
		&&	pkElement->iID == m_kPlayerStone.iID )
		{
			break;
		}
		++kStone_iter;
	}

	if( kVec.end() != kStone_iter )
	{
		SPlayerStone kResultStone;
		if( g_kShineStoneMgr.PlayerStoneUpdate(pUser->GetID(), m_kPlayerStone.kStoneGuid, m_kPos) )
		{
			CONT_ITEM_CREATE_ORDER kOrderList;
			PgBase_Item kItemShineStone;
			if( S_OK == CreateSItem(m_kPlayerStone.iItemNo, 1, kItemShineStone) )
			{
				kOrderList.push_back(kItemShineStone);
			}

			if( !kOrderList.empty() )
			{
				PgAction_CreateItem kCreateAction(CIE_ShineStone, m_kGndKey, kOrderList );
				kCreateAction.DoAction(pUser, NULL);
			}
		}
	}

	//�Ե� ���Ե� ������� �ϹǷ�. �Ʒ� ���� ó�� ����.
	BM::Stream kPacket(PT_M_C_NFY_SHINESTONE_MSG);//�������
	kPacket.Push((BYTE)m_kPlayerStone.iID);
	kPacket.Push(m_kPlayerStone.kStoneGuid);
	kPacket.Push(m_kPlayerStone.cState);
	kPacket.Push(m_kPlayerStone.iItemNo);
	pUser->Send(kPacket);
*/
	return true;
}

//		Pop item at item bag no
PgAction_PopItemBag::PgAction_PopItemBag(const EItemModifyParentEventType eCause, SGroundKey const &rkGndKey,int const iMode, int const iItemBagGrpNo, int const iItemBagNo, PgEventAbil const * const pkEventAbil ,int const iRarityControl)
:	m_eCause(eCause)
,	m_kGndKey(rkGndKey)
,	m_iMode(iMode)
,	m_iItemBagGrpNo(iItemBagGrpNo)
,	m_iItemBagNo(iItemBagNo)
,	m_pkEventAbil(pkEventAbil)
,	m_iRarityControlNo(iRarityControl)
{
}

bool PgAction_PopItemBag::DoAction(CUnit *pUser, CUnit* Nothing)
{
	return DoAction(pUser,1,true);
}

bool PgAction_PopItemBag::DoCreatePack(CUnit *pUser)
{
	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if ( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	/*if ( FAILED(pkPlayer->IsTakeUpItem()) )
	{// �ߵ�����
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}*/

	GET_DEF(CItemBagMgr, kItemBagMgr);

	m_kOrderList.clear();
	m_kConRetItemNo.clear();
		
	int const iRarityControlNo = (m_iRarityControlNo > 0 ? m_iRarityControlNo : PgItemRarityUpgradeFormula::GetItemRarityContorolType(m_eCause));

	switch(m_iMode)
	{
	case UICT_LOCKED_CHEST:
		{
		}break;
	case UICT_OPEN_CHEST:
		{
		}break;
	case UICT_OPEN_PACK:
	case UICT_OPEN_PACK2:
		{
			PgItemBag kItemBag;
			HRESULT const hRet = kItemBagMgr.GetItemBag( m_iItemBagNo, static_cast<short>(pkPlayer->GetAbil(AT_LEVEL)), kItemBag );
			if(S_OK == hRet)
			{
				PgItemBag::BagElementCont const & kElements = kItemBag.GetElements();
				for(PgItemBag::BagElementCont::const_iterator iter = kElements.begin();iter != kElements.end();++iter)
				{
					CONT_ITEM_CREATE_ORDER kOrder;
					if( OrderCreateItem(kOrder, (*iter).iItemNo, (*iter).nCount, iRarityControlNo) )
					{
						for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
						{
							m_kOrderList.push_back( *c_it );
							m_kConRetItemNo.push_back( (*c_it).ItemNo() );
						}
					}
				}
			}
		}break;
	default:
		{
			return false;
		}break;
	}

	int const iLevel = pkPlayer->GetAbil(AT_LEVEL);

	PgItemBag kItemBag;
	HRESULT const hBagRet = kItemBagMgr.GetItemBagByGrp(m_iItemBagGrpNo, static_cast<short>(iLevel), kItemBag);
	if( S_OK == hBagRet )
	{
		int iResultItem = 0;
		int iResultCount = 0;
		if( S_OK == kItemBag.PopItem( iLevel, iResultItem, iResultCount ) )
		{
			CONT_ITEM_CREATE_ORDER kOrder;
			if( OrderCreateItem(kOrder, iResultItem, iResultCount, iRarityControlNo) )
			{
				for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
				{
					m_kOrderList.push_back( *c_it );
					m_kConRetItemNo.push_back( (*c_it).ItemNo() );
				}
			}
		}
	}

	PgAction_CreateItem kCreateItem(m_eCause,m_kGndKey,m_kOrderList);//������ ���� ��û
	return kCreateItem.DoAction(pUser,NULL);
}

bool PgAction_PopItemBag::DoAction(CUnit *pUser,size_t iCount, bool bCreate)
{
	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if ( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	/*if ( FAILED(pkPlayer->IsTakeUpItem()) )
	{// �ߵ�����
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}*/

	int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
	PgItemBag kItemBag;
	GET_DEF(CItemBagMgr, kItemBagMgr);
	HRESULT const hBagRet = kItemBagMgr.GetItemBagByGrp(m_iItemBagGrpNo, static_cast<short>(iLevel), kItemBag);
	if(S_OK == hBagRet)
	{
		m_kOrderList.clear();
		m_kConRetItemNo.clear();
		int itemcount = 1;
		int const iRarityControlNo = PgItemRarityUpgradeFormula::GetItemRarityContorolType(m_eCause);
		while ( iCount-- )
		{
			itemcount = 1;
			int iResultItem = 0;
			if( S_OK == kItemBag.PopItem( iLevel, iResultItem, itemcount ) )
			{
				CONT_ITEM_CREATE_ORDER kOrder;
				if( OrderCreateItem(kOrder, iResultItem, itemcount, iRarityControlNo) )
				{
					for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
					{
						m_kOrderList.push_back( *c_it );
						m_kConRetItemNo.push_back( (*c_it).ItemNo() );
					}
				}
			}
		}
	}

	if ( bCreate )
	{
		return DoCreate( pUser );
	}

	if( m_kOrderList.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return !m_kOrderList.empty();
}

bool PgAction_PopItemBag::DoCreate(CUnit *pkUnit)
{
	if ( m_kOrderList.empty() || NULL == pkUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgAction_CreateItem kCreateItem( m_eCause, m_kGndKey, m_kOrderList );//������ ���� ��û
	kCreateItem.DoAction( pkUnit, NULL );
	m_kOrderList.clear();
	return true;
}

PgAction_PopItemContainer::PgAction_PopItemContainer(const EItemModifyParentEventType eCause, SGroundKey const &rkGndKey, int const iContainerNo, PgEventAbil const * const pkEventAbil )
:	PgAction_PopItemBag(eCause,rkGndKey,0,iContainerNo,0,pkEventAbil,0)
{
}

bool PgAction_PopItemContainer::DoAction(CUnit *pUser, size_t const in_count, bool const bCreate)
{
	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if ( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch(m_eCause)
	{
	case CIE_Open_Gamble:
	case CIE_Locked_Chest:
	case CIE_HiddenPack:
		{
		}break;
	default:
		{
			if( false == pkPlayer->IsTakeUpItem() )
			{// �ߵ�����
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	m_kOrderList.clear();
	m_kConRetItemNo.clear();
	CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;

	GET_DEF(CItemBagMgr, kItemBagMgr);
	
	g_kTblDataMgr.GetContDef(pContContainer);
	
	CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find(m_iItemBagGrpNo);

	if(item_cont_itor == pContContainer->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iRarityControlNo = PgItemRarityUpgradeFormula::GetItemRarityContorolType(m_eCause);
	int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
	int itemcount = 1;
	size_t count = in_count;
	while( count-- )
	{
		size_t iRetIDX = 0;
		if(::RouletteRate((*item_cont_itor).second.iSuccessRateControlNo, iRetIDX, MAX_ITEM_CONTAINER_LIST))
		{
			PgItemBag kItemBag;
			int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
			if( S_OK == kItemBagMgr.GetItemBagByGrp(iResultBagGroupNo, static_cast<short>(iLevel), kItemBag))
			{
				int const iAddMoneyDropRate = ( m_pkEventAbil ? m_pkEventAbil->GetAbil(AT_ADD_MONEY_RATE) : 0 );
				int const iMoneyAddRate = ( m_pkEventAbil ? std::max(m_pkEventAbil->GetAbil(AT_ADD_MONEY_PER), 0 ) : 0 );

				itemcount = 1;
				int iResultItem = 0;
				if(S_OK == kItemBag.PopItem( iLevel, iResultItem ) )
				{
					CONT_ITEM_CREATE_ORDER kOrder;
					if( OrderCreateItem(kOrder, iResultItem, itemcount, iRarityControlNo) )
					{
						for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
						{
							m_kOrderList.push_back( *c_it );
							m_kConRetItem.push_back( *c_it );
							m_kConRetItemNo.push_back( (*c_it).ItemNo() );
						}
					}
				}

				int iMoney = 0;
				if(S_OK == kItemBag.PopMoney(iMoney, iAddMoneyDropRate))
				{
					if(iMoneyAddRate)
					{
						iMoney += SRateControl::GetValueRate<int>( iMoney, iMoneyAddRate );//�̺�Ʈ ���ʽ�.
					}

					if(iMoney > 0)
					{
		//				pkMon->AddDropMoney(iMoney);
					}
				}
			}
		}
	}

	int const iRewordItemNo = (*item_cont_itor).second.iRewordItem;
	if( 0 != iRewordItemNo )
	{
		PgBase_Item kItem;
		if(S_OK == CreateSItem(iRewordItemNo, 1,PgItemRarityUpgradeFormula::GetItemRarityContorolType(m_eCause), kItem))
		{
			m_kOrderList.push_back(kItem);
			m_kConRetItem.push_back(kItem);
			m_kConRetItemNo.push_back(iRewordItemNo);
		}
	}

	if ( bCreate )
	{
		return DoCreate( pUser );
	}

	if( m_kOrderList.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return !m_kOrderList.empty();
}

void PgAction_PopItemBag::SwapRetItem(RESULT_SITEM_BAG_LIST_CONT & rkConRetItem)
{
	rkConRetItem.swap(m_kConRetItem);
	m_kConRetItem.clear();
}

void PgAction_PopItemBag::SwapRetItemNo(RESULT_ITEM_BAG_LIST_CONT& rkConRetItemNo)
{
	rkConRetItemNo.swap(m_kConRetItemNo);
	m_kConRetItemNo.clear();
}

int PgAction_PopItemBag::GetRetItemNo(size_t const iAt)const
{
	if ( iAt < m_kConRetItemNo.size() )
	{
		return m_kConRetItemNo.at(iAt);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}


PgAction_RewardItem::PgAction_RewardItem(const EItemModifyParentEventType eCause, SGroundKey const &rkGndKey, REWARD_ITEM_LIST const& rkConItem)
:	m_eCause(eCause)
,	m_kGndKey(rkGndKey)
{
	m_kContItem = rkConItem;
}

PgAction_RewardItem::PgAction_RewardItem(EItemModifyParentEventType const eCause, SGroundKey const &rkGndKey, RESULT_ITEM_BAG_LIST_CONT const& rkConItem)
:	m_eCause(eCause)
,	m_kGndKey(rkGndKey)
{
	m_kContItem.reserve(rkConItem.size());
	RESULT_ITEM_BAG_LIST_CONT::const_iterator item_itr;
	for ( item_itr=rkConItem.begin(); item_itr!=rkConItem.end(); ++item_itr )
	{
		m_kContItem.push_back( REWARD_ITEM_LIST::value_type( *item_itr, 1 ) );
	}
}

bool PgAction_RewardItem::SAction::operator()(REWARD_ITEM_LIST::value_type const &kValue)
{
	CONT_ITEM_CREATE_ORDER kOrder;
	if( OrderCreateItem(kOrder, kValue.iItemNo, kValue.iCount, PgItemRarityUpgradeFormula::GetItemRarityContorolType(m_eCause)) )
	{
		for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
		{
			m_rkOrderList.push_back( *c_it );
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAction_RewardItem::DoAction(CUnit *pkUser, CUnit* Nothing)
{
	if ( !pkUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_ITEM_CREATE_ORDER kItemOrderList;
	SAction kAction(kItemOrderList,m_eCause);
	std::for_each( m_kContItem.begin(), m_kContItem.end(), kAction);
	
	PgAction_CreateItem kCreateItem(m_eCause,m_kGndKey,kItemOrderList);//������ ���� ��û
	return kCreateItem.DoAction(pkUser, NULL);
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_StoreItemBuyResult
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqModifyItem::PgAction_ReqModifyItem(EItemModifyParentEventType const kCause, SGroundKey const &kGroundKey, CONT_PLAYER_MODIFY_ORDER const &kOrder, BM::Stream const &kPacket, bool const bIsIgnoreDeath)
	:	m_kCause(kCause), m_kGndKey(kGroundKey), m_kOrder(kOrder), m_kAddonPacket(kPacket), m_bIsIgnoreDeath(bIsIgnoreDeath)
{
	m_dwCoolTime = 0;
}

void PgAction_ReqModifyItem::SetCoolTime(DWORD const dwCoolTime)
{
	m_dwCoolTime = dwCoolTime;
}

bool PgAction_ReqModifyItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(	pkCaster
	&&	!m_bIsIgnoreDeath//���� ���� �÷���. ���� �����̰�
	&&	!pkCaster->IsAlive())//�׾��ִٸ� ����.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(	pkTarget
	&&	!m_bIsIgnoreDeath//���� ���� �÷���. ���� �����̰�
	&&	!pkTarget->IsAlive()//�׾��ִٸ� ����.
	&&	UT_MYHOME != pkTarget->UnitType()) // ����Ȩ�� �ƴϸ� ( ����Ȩ�� �� �׾� �ִ°��� ��... )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;	
	}

	BM::GUID kCasterGuid( BM::GUID::NullData() );
	if( pkCaster )
	{
		switch ( pkCaster->UnitType() )
		{
		case UT_ENTITY:
		case UT_PET:
			{
				kCasterGuid = pkCaster->Caller();
			}break;
		default:
			{
				kCasterGuid = pkCaster->GetID();
			}break;
		}
	}

	return DoAction( kCasterGuid );
}

bool PgAction_ReqModifyItem::DoAction( BM::GUID const &kCasterGuid )
{
	if ( m_kOrder.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket;
	if (CashAdd().Cause() != IMET_NONE)
	{
		kPacket.Push(PT_M_I_REQ_CASH_MODIFY);
		CashAdd().WriteToPacket(kPacket);
	}
	else
	{
		kPacket.Push(PT_M_I_REQ_MODIFY_ITEM);
	}

	kPacket.Push(m_kCause);
	kPacket.Push(kCasterGuid);
	m_kOrder.WriteToPacket(kPacket);

	if(m_kAddonPacket.IsEmpty())
	{
		kPacket.Push((bool)false);
	}
	else
	{
		kPacket.Push((bool)true);
		kPacket.Push(m_kAddonPacket.Data());
	}

	SendToItem(m_kGndKey, kPacket);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_InitUnitDrop
//////////////////////////////////////////////////////////////////////////////
/*
PgAction_InitUnitDrop::PgAction_InitUnitDrop( TBL_DQT_DEF_MAP_ITEM_BAG const &kTbl, PgEventAbil const * const pkEventAbil, T_GNDATTR const eGndAttr,  bool const bDropAllItem )
	:	m_kTbl(kTbl)
	,	m_pkEventAbil(pkEventAbil)
	,	m_eGndAttr(eGndAttr)
	,	m_bDropAllItem(bDropAllItem)
{
}

bool PgAction_InitUnitDrop::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
//Clear!!
	CONT_MONSTER_DROP_ITEM kTempContDropItem;

	pkCaster->SetAbil64(AT_MONEY, 0);
	if ( SUCCEEDED(pkCaster->PopDropItem(kTempContDropItem)) )//������ Ŭ����.
	{
		enum EMonsterItemBagIndex // ��ħ ��ȣ ��� ���� ����
		{
			MIBI_PRIMARY	= 0,
			MIBI_SECONDARY	= 1,
			MIBI_MAPBAG		= 2,
			MIBI_COUNT		= 3, // ���� ���� ����
		};

		int const iSuccessRateCtrlNo = pkCaster->GetAbil(AT_ITEM_BAG_SUCCESS_RATE_CONTROL);
		int const iAddMoneyDropRate = (m_pkEventAbil ? m_pkEventAbil->GetAbil(AT_ADD_MONEY_RATE) : 0);
		int const iMoneyAddRate = ( m_pkEventAbil ? std::max( m_pkEventAbil->GetAbil(AT_ADD_MONEY_PER), 0 ) : 0 );
		
		PgItemBag akItemBag[MIBI_COUNT];
		GET_DEF(CItemBagMgr, kItemBagMgr);

		{
			int iMinCount = __max(0, pkCaster->GetAbil(AT_MIN_DROP_ITEM_COUNT));
			int iMaxCount = __min(50, pkCaster->GetAbil(AT_MAX_DROP_ITEM_COUNT));

			if ( iMaxCount <= 0 )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if( iMinCount > iMaxCount )
			{
				std::swap(iMinCount,iMaxCount);
			}

			int iRetCount = BM::Rand_Range(iMaxCount, iMinCount);//Min(> 0) ~ Max( > Min)
			int const iItemContainerNo = pkCaster->GetAbil(AT_ITEM_DROP_CONTAINER);

			int const iItemBagNo_1 = pkCaster->GetAbil(AT_ITEM_BAG_PRIMARY);
			int const iItemBagNo_2 = pkCaster->GetAbil(AT_ITEM_BAG_SECONDARY);

			kItemBagMgr.GetItemBag(iItemBagNo_1, 1, akItemBag[MIBI_PRIMARY]);
			kItemBagMgr.GetItemBag(iItemBagNo_2, 1, akItemBag[MIBI_SECONDARY]);

			size_t iGrpIDX = 0;
			if(RouletteRate(m_kTbl.iSuccessRateNo, iGrpIDX, MAP_ITEM_BAG_GRP))
			{
				kItemBagMgr.GetItemBagByGrp(m_kTbl.aiBagGrpNo[iGrpIDX], 1, akItemBag[MIBI_MAPBAG]);
				int const iAddDropRate = g_kEventView.GetAbil(AT_ADD_ITEMDROP_RATE);
				if(0 < iAddDropRate && (0 == (GATTR_FLAG_NOADDITEMDROPRATE & m_eGndAttr)))
				{
					akItemBag[MIBI_MAPBAG].AddDropRate(iAddDropRate);
				}
			}

			if(0 == iItemContainerNo)
			{
				int iItemBagNo = 0;

				int iBaseDropMoney = BM::Rand_Range((int)(pkCaster->GetAbil64(AT_MONEY)), (int)(pkCaster->GetAbil(AT_MONEY_MIN)));
				int iBaseDropMoneyRate = pkCaster->GetAbil(AT_ADD_MONEY_RATE);

				if(	iBaseDropMoney > 0
				&&	lwIsRandSuccess(iBaseDropMoneyRate) )
				{
					if(iMoneyAddRate)
					{
						iBaseDropMoney = ((iBaseDropMoney*iMoneyAddRate)/100);//�̺�Ʈ ���ʽ�.
					}

					pkCaster->AddDropMoney(iBaseDropMoney);
				}

				size_t iIDX = 0;
				if(m_bDropAllItem)
				{
					for(iIDX=0; iIDX<MIBI_COUNT; ++iIDX)
					{
						for(int iBagIndex=0; iBagIndex<akItemBag[iIDX].GetElementsCount(); ++iBagIndex)
						{
							int iResultItem = 0;
							if(S_OK != akItemBag[iIDX].PopItemToIndex( 1, iResultItem, iBagIndex ))
							{
								continue;
							}

							PgBase_Item kItem;
							if(S_OK == CreateSItem(iResultItem, 1,GIOT_FIELD, kItem))
							{
								pkCaster->AddDropItem(kItem);
							}
						}
					}
				}

				while( iRetCount && !m_bDropAllItem)
				{
					int iItemBagNo = 0;
					if(RouletteRate(iSuccessRateCtrlNo, iIDX, MIBI_COUNT))
					{//Ȯ��ǥ�� ���� �ε����� ���Դ�.
						int iResultItem = 0;
						bool bCreateItemBag = true;
						if( GATTR_BATTLESQUARE == m_eGndAttr )
						{
							switch( iIDX )
							{
							case MIBI_PRIMARY:
							case MIBI_SECONDARY:
								{
									bCreateItemBag = false; // Monster bag disable
								}break;
							case MIBI_MAPBAG:
								{
									bCreateItemBag = true; // Map bag enable
								}break;
							}
						}
						if( bCreateItemBag
						&&	S_OK == akItemBag[iIDX].PopItem( 1, iResultItem ) )
						{
							PgBase_Item kItem;
							if(S_OK == CreateSItem(iResultItem, 1,GIOT_FIELD, kItem))
							{
								pkCaster->AddDropItem(kItem);
							}
						}
					}
					--iRetCount;
				}
			}
			else
			{
				CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;
				g_kTblDataMgr.GetContDef(pContContainer);

				if(!pContContainer)
				{
					return false;
				}

				CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find(iItemContainerNo);
				if(item_cont_itor == pContContainer->end())
				{
					return false;
				}

				if(m_bDropAllItem)
				{// ItemBag�� ��� �������� ADD
					for(size_t iRetIDX=0; iRetIDX<MIBI_COUNT; ++iRetIDX)
					{
						PgItemBag kItemBag;
						int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
						if( S_OK != kItemBagMgr.GetItemBagByGrp(iResultBagGroupNo, 1, kItemBag) )
						{
							continue;
						}

						int iResultItem = 0;
						for(int iBagIndex=0; iBagIndex<kItemBag.GetElementsCount(); ++iBagIndex)
						{
							if(S_OK != kItemBag.PopItemToIndex( 1, iResultItem, iBagIndex ))
							{
								continue;
							}

							PgBase_Item kItem;
							if(S_OK == CreateSItem(iResultItem, 1,GIOT_FIELD, kItem))
							{
								pkCaster->AddDropItem(kItem);
							}
						}

					}//END for(size_t iRetIDX=0; iRetIDX<iBagCandidateCount; ++iRetIDX)
				}

				size_t count = iRetCount;
				while( count-- && !m_bDropAllItem )
				{
					size_t iRetIDX = 0;
					if(::RouletteRate((*item_cont_itor).second.iSuccessRateControlNo, iRetIDX, MAX_ITEM_CONTAINER_LIST))
					{
						PgItemBag kItemBag;
						int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
						if( S_OK == kItemBagMgr.GetItemBagByGrp( iResultBagGroupNo, 1, kItemBag))
						{
							int iResultItem = 0;
							if(S_OK == kItemBag.PopItem( 1, iResultItem ) )
							{
								PgBase_Item kItem;
								if(S_OK == CreateSItem(iResultItem, 1,GIOT_FIELD, kItem))
								{
									pkCaster->AddDropItem(kItem);
								}
							}
						}
					}
				}//END while( count-- && !m_bDropAllItem ) 
			}
		}

=====================================================================================================================================
	�̺�Ʈ ������ ��� ó�� 
=====================================================================================================================================

		int const iEventItemContainer = g_kEventView.GetAbil(AT_EVENT_ITEM_CONTAINER);

		if(0 < iEventItemContainer)
		{
			CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;
			g_kTblDataMgr.GetContDef(pContContainer);

			if(pContContainer)
			{
				CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pContContainer->find(iEventItemContainer);
				if(item_cont_itor != pContContainer->end())
				{
					size_t iRetIDX = 0;
					if(::RouletteRate((*item_cont_itor).second.iSuccessRateControlNo, iRetIDX, MAX_ITEM_CONTAINER_LIST))
					{
						PgItemBag kItemBag;
						int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
						if( S_OK == kItemBagMgr.GetItemBagByGrp(iResultBagGroupNo, 1, kItemBag))
						{
							int iResultItem = 0;
							if(S_OK == kItemBag.PopItem( 1, iResultItem ) )
							{
								PgBase_Item kItem;
								if(S_OK == CreateSItem(iResultItem, 1,GIOT_FIELD, kItem))
								{
									pkCaster->AddDropItem(kItem);
								}
							}
						}
					}
				}
			}
		}

=====================================================================================================================================
	��ȹ ��û���� ������ ��� Ƚ���� �� ��� Ƚ���� �и� ������ �ش� ����� ��� ���� ������ �⺻ 1�� ������� ����
=====================================================================================================================================
		{
			int iMinCount = __max(1, pkCaster->GetAbil(AT_MIN_DROP_MONEY_COUNT));
			int iMaxCount = __min(50, pkCaster->GetAbil(AT_MAX_DROP_MONEY_COUNT));

			if ( iMaxCount <= 0 )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if ( iMinCount > iMaxCount )
			{
				std::swap(iMinCount,iMaxCount);
			}

			int iRetMoneyCount = BM::Rand_Range(iMaxCount, iMinCount);//Min(> 0) ~ Max( > Min)

			size_t iIDX = 0;
			int iMoney = 0;
			while( iRetMoneyCount )
			{
				int iItemBagNo = 0;
				if(RouletteRate(iSuccessRateCtrlNo, iIDX, MIBI_COUNT))
				{//Ȯ��ǥ�� ���� �ε����� ���Դ�.
					if(S_OK == akItemBag[iIDX].PopMoney(iMoney, iAddMoneyDropRate))
					{
						if(iMoneyAddRate)
						{
							iMoney = ((iMoney*iMoneyAddRate)/100);//�̺�Ʈ ���ʽ�.
						}

						if(iMoney > 0)
						{
							pkCaster->AddDropMoney(iMoney);
						}
					}
				}
				--iRetMoneyCount;
			}
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
*/

//////////////////////////////////////////////////////////////////////////////
//		PgAction_DecEquipDuration
//////////////////////////////////////////////////////////////////////////////
PgAction_DecEquipDuration::PgAction_DecEquipDuration(SGroundKey const &kGndKey, bool const bIsAttacker, int const iValue, bool const bIsDamageEvent)
	:	m_kGndKey(kGndKey), m_bIsAttacker(bIsAttacker), m_iValue(iValue), m_bIsDamageEvent(bIsDamageEvent)
{
}

bool PgAction_DecEquipDuration::GetEquipRandomPos(bool const bIsAttacker, CUnit* pkCaster, CONT_DEC_POS& rkOut, size_t const iMaxCount)//������ ������ �������� �޾ƿ��� �κ�
{
	if( pkCaster->GetAbil(AT_PROTECTION_GALL) )
	{
		return false;
	}

	if( bIsAttacker )
	{
		rkOut.insert( EQUIP_POS_WEAPON );
	}
	else
	{
		//static int const iCandidateCount = sizeof(g_aiDefenceCandidate)/sizeof(int);
//		int const pos = BM::Rand_Index(iCandidateCount);
//		return g_aiDefenceCandidate[pos]; 

		PgInventory* pkInv = pkCaster->GetInven();
		typedef std::vector< int > CONT_ITEM_POS;
		CONT_ITEM_POS kContItemPos;

		int i = 0;
		int const* pkBegin = g_aiDefenceCandidate;
		while( PgArrayUtil::IsInArray(pkBegin, g_aiDefenceCandidate) )
		{
			int const iPos = *pkBegin;
			PgBase_Item kOutItem;
			if( S_OK == pkInv->GetItem(IT_FIT, iPos, kOutItem)
			&&	!kOutItem.EnchantInfo().IsNeedRepair())//���� �䱸��. �� �� ����.
			{
				kContItemPos.push_back(iPos);
			}
			++pkBegin;
		}

		if( kContItemPos.size() )
		{
			std::random_shuffle(kContItemPos.begin(), kContItemPos.end());
			CONT_ITEM_POS::const_iterator iter = kContItemPos.begin();
			while( kContItemPos.end() != iter )
			{
				rkOut.insert( (*iter) );
				if( iMaxCount <= rkOut.size() )
				{
					break;
				}
				++iter;
			}
		}
	}
	return !rkOut.empty();
}
int PgAction_DecEquipDuration::GetEnchantDiffRate(int const iBaseRate, int const iEnchantDiff)
{
	float fScale = (1 + 0.1 * iEnchantDiff);
	float const fMaxScale = 2.5f;
	fScale = std::min(fScale, fMaxScale);
	return static_cast< int >(iBaseRate * fScale);
}
bool PgAction_DecEquipDuration::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{//���� -> caster,����,			Ÿ��: ����
// ���� -> caster,����,		Ÿ��: ����
// ���� -> caster,����,			Ÿ��: ����

	if(pkCaster == NULL || !pkCaster->IsUnitType(UT_PLAYER) || pkTarget == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

/*	if(	!pkCaster->IsUnitType(UT_PLAYER)
	&&	!pkTarget->IsUnitType(UT_PLAYER)	)
	{
		return false;
	}
*/
//#ifdef _DEBUG
//	short int const sWeaponDecLimit = 1;
//	short int const sArmorDecLimit = 17;
//#else
	int const iMinDecPosCount = 1;
	int const iWeaponDecLimit = 50; // 0.5%
	int const iArmorDecLimit = 700; // 7%
	int iMaxDecPosCount = iMinDecPosCount;
//#endif
	int ret = 0;
	if( m_bIsDamageEvent )
	{//������ �̺�Ʈ�� ���� ����.

		int const iRand = BM::Rand_Index(ABILITY_RATE_VALUE);

		if( 0 != pkTarget->GetAbil(AT_MON_ENCHANT_LEVEL)
		||	0 != pkCaster->GetAbil(AT_MON_ENCHANT_LEVEL) ) //��æƮ �� ���Ϳ� ������
		{
			if( m_bIsAttacker )
			{ // ����
				int const iEnchantDiff = pkTarget->GetAbil(AT_DEFENCE_ENCHANT_LEVEL) - pkCaster->GetAbil(AT_OFFENCE_ENCHANT_LEVEL);
				if( 0 < iEnchantDiff ) // ���Ͱ� �� ����
				{
					int const iRate = GetEnchantDiffRate(iWeaponDecLimit, iEnchantDiff);
					if( iRand > iRate )
					{
						return true;
					}
				}
				else
				{
					if( iRand > iWeaponDecLimit )
					{
						return true;
					}
				}
			}
			else
			{ // ��
				int const iEnchantDiff = pkTarget->GetAbil(AT_OFFENCE_ENCHANT_LEVEL) - pkCaster->GetAbil(AT_DEFENCE_ENCHANT_LEVEL);
				if( 0 < iEnchantDiff ) // ���Ͱ� �� ����
				{
					int const iRate = GetEnchantDiffRate(iArmorDecLimit, iEnchantDiff);
					if( iRand > iRate )
					{
						return true;
					}
					iMaxDecPosCount = iRate * (1.f / ABILITY_RATE_VALUE) + 0.5f;
					iMaxDecPosCount = std::max(iMaxDecPosCount, iMinDecPosCount);
				}
				else
				{
					if( iRand > iArmorDecLimit )
					{
						return true;
					}
				}
			}
		}
		else
		{
			if( m_bIsAttacker )
			{ // ����
				if( iRand > iWeaponDecLimit )
				{
					return true;
				}
			}
			else
			{ // ��
				if( iRand > iArmorDecLimit )
				{
					return true;
				}
			}
		}
	}
	else//ī��Ʈ �� log ���� �ٷ� ���ϴ� ��ŭ ����
	{
		ret = m_iValue;
	}

	CONT_DEC_POS kDecPos;
	if( GetEquipRandomPos(m_bIsAttacker, pkCaster, kDecPos, iMaxDecPosCount) )// �������� �־����.
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;
		PgInventory* kInv = pkCaster->GetInven();
		if( kInv )
		{
			CONT_DEC_POS::const_iterator iter = kDecPos.begin();
			while( kDecPos.end() != iter )
			{
				SItemPos const kItemPos(IT_FIT, (*iter));
				PgBase_Item kItem;
				if( SUCCEEDED(kInv->GetItem(kItemPos, kItem)) )
				{//	
					if( kItem.Count() == 1 )
					{//�������� 1 �̸� ���� �÷��� üũ �ؼ�. ������Ʈ
						if( !kItem.EnchantInfo().IsNeedRepair() )
						{//�����䱸 �÷��� ���� ������. ����.
							SEnchantInfo kEnchantInfo = kItem.EnchantInfo();
							kEnchantInfo.IsNeedRepair(true);
							SPMOD_Enchant kSPMOEnchant(kItem, kItemPos, kEnchantInfo);

							SPMO kIMO(IMET_MODIFY_ENCHANT|IMC_DEC_DUR_BY_BATTLE, pkCaster->GetID(), kSPMOEnchant);//
							kOrder.push_back(kIMO);
						}
					}
					else
					{
						SPMOD_Modify_Count kDelData(kItem, kItemPos, -1);//1�� ����.
						
						SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_BATTLE, pkCaster->GetID(), kDelData);
						kOrder.push_back(kIMO);
					}
				}
				++iter;
			}
		}

		if( kOrder.size() )
		{
			PgAction_ReqModifyItem kItemModifyAction(((m_bIsDamageEvent)?CIE_Dec_Dur_by_Defence:CIE_Dec_Dur_by_Attack), m_kGndKey, kOrder);
			kItemModifyAction.DoAction(pkCaster, pkTarget);
		}
	// ->
	// ���� ��ġ�� ���?.
	// �ƹ��ų� ��°�?????

	//	iSendValue = ItemMgr.DecItemDur_ForEquip(*item, ret);
	}

	{//�Ʋ����� �����ϴ� �˶��� ��� ���� üũ
		if( IsAllNeedRepair( pkCaster ) )
		{
			PgSyncClinetAchievementHandler<> kSA( AT_ACHIEVEMENT_DURABLENESS_ZERO, 0, m_kGndKey );
			kSA.DoAction( pkCaster, NULL );
		}
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ItemRepair
//////////////////////////////////////////////////////////////////////////////
PgAction_ItemRepair::PgAction_ItemRepair(SGroundKey const &kGndKey, CONT_REQ_ITEM_REPAIR const &kContRepair)
	:	m_kGndKey(kGndKey), m_kContRepair(kContRepair)
{
	if(m_kContRepair.empty())//Ÿ���� ������ ���� ����.
	{
		static int const iCandiSize = sizeof(g_aiDeathPenaltyCandidate)/sizeof(int);

		int i = 0;
		while(iCandiSize >i)
		{
			m_kContRepair.push_back(SItemPos(IT_FIT, g_aiDeathPenaltyCandidate[i]));
			++i;
		}
	}
}

bool PgAction_ItemRepair::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{//���� ����.
	if(pkCaster)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		PgInventory *pkInv = pkCaster->GetInven();

		CONT_REQ_ITEM_REPAIR::const_iterator itor = m_kContRepair.begin();
		GET_DEF(CItemDefMgr, kItemDefMgr);
		__int64 iNeedMoney = 0;
		while(itor != m_kContRepair.end())
		{
			SItemPos const &kItemPos = (*itor).TargetPos();
			PgBase_Item kItem;

			if( SUCCEEDED(pkInv->GetItem(kItemPos, kItem)) 
				&& IsCanRepair(kItem.ItemNo() ) )
			{
				// ������ 0�� ��� ���� ��, �����Ǵ� ����
				if( kItem.EnchantInfo().IsNeedRepair() )
				{
					E_ITEM_GRADE const kItemGrade = GetItemGrade( kItem );
					switch( kItemGrade )
					{
					case IG_NORMAL:
						{
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_NORMAL_REPAIR, 1, m_kGndKey );
							kMA.DoAction( pkCaster, NULL );
						}break;
					case IG_RARE:
						{
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_RARE_REPAIR, 1, m_kGndKey );
							kMA.DoAction( pkCaster, NULL );
						}break;
					case IG_UNIQUE:
						{
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_UNIQUE_REPAIR, 1, m_kGndKey );
							kMA.DoAction( pkCaster, NULL );
						}break;
					case IG_ARTIFACT:
						{
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_ARTIFACT_REPAIR, 1, m_kGndKey );
							kMA.DoAction( pkCaster, NULL );
						}break;
					case IG_LEGEND:
						{
							PgAddAchievementValue kMA( AT_ACHIEVEMENT_LEGEND_REPAIR, 1, m_kGndKey );
							kMA.DoAction( pkCaster, NULL );
						}break;
					default:
						{

						}break;
					}
				}

				CItemDef const *pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
				if(pItemDef)
				{
					int iNeedMoneyPerElement = 0;
					HRESULT const hRet = CalcRepairNeedMoney( kItem, pkCaster->GetAbil( AT_ADD_REPAIR_COST_RATE ), iNeedMoneyPerElement );
					if(S_OK == hRet)
					{	
						iNeedMoney += iNeedMoneyPerElement;

						{
							int const iDefaultDur = kItem.MaxDuration();
							int const iRepairCount = iDefaultDur - kItem.Count();
							if(iRepairCount)
							{
								SPMOD_Modify_Count kDelData(kItem, kItemPos, iRepairCount);
								SPMO kIMO(IMET_MODIFY_COUNT|IMC_INC_DUR_BY_REPAIR, pkCaster->GetID(), kDelData);
								kOrder.push_back(kIMO);
							}
						}
					}
				}
			}
			++itor;
		}

		eMyHomeSideJob const kSideJob = MSJ_REPAIR;

		iNeedMoney *= PgMyHomeFuncRate::GetCostRate(kSideJob, pkTarget);

		if(kOrder.size() 
		&& 0 < iNeedMoney)
		{
			{
				SPMOD_Add_Money kDelMoneyData(-iNeedMoney);//�ʿ�Ӵ� ����.
				SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
				kOrder.push_back(kIMO);
			}

			if(pkTarget && UT_MYHOME == pkTarget->UnitType())
			{
				PgMyHome const * pkMyHome = dynamic_cast<PgMyHome const *>(pkTarget);
				if(pkMyHome && 0 < (pkMyHome->GetAbil(AT_HOME_SIDEJOB) & kSideJob))
				{
					SHOMEADDR const & kHomeAddr = pkMyHome->HomeAddr();
					SPMO kIMO(IMET_SIDEJOB_MODIFY, pkTarget->GetID(), SMOD_MyHome_SideJob_Modify(kHomeAddr.StreetNo(), kHomeAddr.HouseNo(), kSideJob, iNeedMoney));
					kOrder.push_back(kIMO);
				}
			}

			PgAction_ReqModifyItem kItemModifyAction(CIE_Repair, m_kGndKey, kOrder);
			kItemModifyAction.DoAction(pkCaster, pkTarget);
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqRemoveInvItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqRemoveInvItem::PgAction_ReqRemoveInvItem(SGroundKey const &kGndKey, ContItemRemoveOrder const &kCont, int const iDeleteType, int const iAddSoulCountRate)
	:	m_kGndKey(kGndKey), m_kOrder(kCont), m_iDeleteType(iDeleteType), m_iAddSoulCountRate(iAddSoulCountRate)
{
}

bool PgAction_ReqRemoveInvItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(pkCaster)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;
		
		PgInventory* pkInven = pkCaster->GetInven();
		if(pkInven == NULL)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		
		BM::Stream kGenSoul;
		std::vector<int> kContSoule;

		ContItemRemoveOrder::const_iterator itor = m_kOrder.begin();
		while(itor != m_kOrder.end())
		{
			PgBase_Item kItem;
			if( SUCCEEDED(pkInven->GetItem((*itor).kCasterPos, kItem)) )
			{
				SPMOD_Modify_Count kDelData(kItem, (*itor).kCasterPos, 0, true);//����.
				SPMO kIMO(IMET_MODIFY_COUNT, pkCaster->GetID(), kDelData);
				kOrder.push_back(kIMO);

				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const *pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
				if(!pItemDef)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				if((ICMET_Cant_DropItem & pItemDef->GetAbil(AT_ATTRIBUTE)) || kItem.EnchantInfo().IsBinding())
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				if(0 != m_iDeleteType)
				{
					if(kItem.EnchantInfo().Rarity() > 0) // ������ ������ �ı��� ��û �������� ����� ���� �������� �ı� �� �� ����.
					{
						//������� ������ ���Դ°�.
						E_ITEM_GRADE const eItemGrade = ::GetItemGrade(kItem);

						CONT_DEF_ITEM_DISASSEMBLE const *pkContDef = NULL;
						g_kTblDataMgr.GetContDef(pkContDef);

						int const iLevel = pkCaster->GetAbil( AT_LEVEL );

						CONT_DEF_ITEM_DISASSEMBLE::const_iterator def_itor = pkContDef->find(eItemGrade);
						if(def_itor != pkContDef->end())
						{
							int const iItemBagGrpNo = (*def_itor).second.iItemBagGrpNo;

							PgItemBag kItemBag;
							GET_DEF(CItemBagMgr, kItemBagMgr);
							HRESULT const hBagRet = kItemBagMgr.GetItemBagByGrp( iItemBagGrpNo, static_cast<short>(iLevel), kItemBag);
							if(S_OK == hBagRet)
							{
								int iResultItem = 0;
								if(S_OK == kItemBag.PopItem( iLevel, iResultItem ))
								{					
									int iResultItemCount = static_cast<int>(1 + (double(pItemDef->GetAbil(AT_LEVELLIMIT))/5.0) + (double(kItem.EnchantInfo().Rarity())/15.0));
									iResultItemCount = static_cast<int>(iResultItemCount * PgItemRarityUpgradeFormula::GetRarityUpgradeCostRate(pItemDef->EquipPos()));
									iResultItemCount = std::max( iResultItemCount, 1 );

									int iPremiumSoulRefundRate = 0;
									PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
									if( pkPlayer )
									{
										if( S_PST_SoulRefund const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_SoulRefund>() )
										{
											iPremiumSoulRefundRate = pkPremium->iRate;
										}
									}

									int const iAddSoulCountRate = pkCaster->GetAbil( AT_ADD_SOUL_RATE_REFUND ) + m_iAddSoulCountRate + iPremiumSoulRefundRate;
									if ( iAddSoulCountRate )
									{
										iResultItemCount += SRateControl::GetValueRate( iResultItemCount, iAddSoulCountRate );
									}

									if ( 0 < iResultItemCount )
									{
										kContSoule.push_back(iResultItemCount);
										tagPlayerModifyOrderData_Add_Any kDelData(iResultItem, iResultItemCount);
										SPMO kIMO(IMET_ADD_ANY, pkCaster->GetID(), kDelData);
										kOrder.push_back(kIMO);
									}
								}
							}
						}
					}
				}

				if ( UIT_STATE_PET == kItem.State() )
				{
					SPMO kIMO( IMET_PET_DUMP, pkCaster->GetID() );
					kIMO.m_kPacket.Push(kItem.Guid());
					kOrder.push_back( kIMO );
				}
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			++itor;
		}

		kGenSoul.Push(kContSoule);
		PgAction_ReqModifyItem kItemModifyAction(CIE_Dump, m_kGndKey, kOrder,kGenSoul);
		kItemModifyAction.DoAction(pkCaster, pkTarget);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_AddExp
//////////////////////////////////////////////////////////////////////////////
__int64 PgAction_AddExp::ms_i64TakePCExpRate = 1000i64;//PC�� ���� �޴� ����ġ�� ������

PgAction_AddExp::PgAction_AddExp(SGroundKey const& rkGndKey, __int64 const iAddExp, const EAddExpCause eCause, PgGround *pkGround, int const iMonsterLv, int const iAddedExpRate)
	:m_kGndKey(rkGndKey), m_iAddExp(iAddExp), m_eCause(eCause), m_pkGround(pkGround), m_iMonsterLv(iMonsterLv), m_iAddedExpRate(iAddedExpRate)
{
}

bool PgAction_AddExp::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	// UT_ENTITY�� ���� ���� ��쿡�� ó���� �־�� �Ѵ�.
	//if( !m_iAddExp )//����ġ�� 0�̸�
	//{
	//	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	//	return false;
	//}

	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPlayer = NULL;
	BM::GUID kPartyGuid;
	BM::GUID ExpeditionGuid;
	bool IsExpedition = false;
	switch(pkCaster->UnitType())
	{
	case UT_ENTITY:
		{
			PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkCaster);
			if(pkEntity)
			{
				BM::GUID CallerGuid = pkEntity->Caller();
				PgPlayer * pPlayer = m_pkGround->GetUser(CallerGuid);
				if( pPlayer )
				{
					if( pPlayer->HaveExpedition() )
					{
						ExpeditionGuid = pPlayer->ExpeditionGuid();
						IsExpedition = true;
					}
					else
					{
						kPartyGuid = pkEntity->Party();
					}
				}
			}
		}break;
	case UT_SUMMONED:
		{
			PgSummoned* pSummoned = dynamic_cast<PgSummoned*>(pkCaster);
			if(pSummoned)
			{
				//kPartyGuid = pSummoned->Party();//�ʿ��ұ�?
			}
		}break;
	case UT_PLAYER:
		{
			pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if( pkPlayer )
			{
				int const iIgnoreCnt = pkPlayer->GetAbil(AT_IGNORE_MACRO_COUNT);
				if(0<iIgnoreCnt)
				{
					pkPlayer->AddAbil(AT_IGNORE_MACRO_COUNT, -1);
				}
				else if( 0 == (AEC_PetExp & m_eCause) ) // ���� �ִ� ����ġ�� ��ũ�� �˻翡 �ɸ��� ����
				{
					CGameTime kCurTime;
					pkPlayer->MacroLastRecvExpTime(kCurTime.GetLocalSecTime());
				}

				if( pkPlayer->HaveExpedition() )
				{
					ExpeditionGuid = pkPlayer->ExpeditionGuid();
					IsExpedition = true;
				}
				else
				{
					kPartyGuid = pkPlayer->PartyGuid();
				}
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Caster[" << pkCaster->Name() << L"] must be UT_PLAYER or UT_ENTITY, CurrentType[" << pkCaster->UnitType() << L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}

	bool bShareParty = false;
	if( IsExpedition )
	{
		bShareParty = ((BM::GUID::NullData() != ExpeditionGuid) && (m_eCause & AEC_CheckShareParty));
	}
	else
	{
		bShareParty = ((BM::GUID::NullData() != kPartyGuid) && (m_eCause & AEC_CheckShareParty));
	}
	
	if( !bShareParty		//��Ƽ�� ���ų�
	|| 0 > m_iAddExp	//����
	)
	{
		return PassAdvance(pkCaster, pkTarget);//��� ����
	}

	if( !m_pkGround )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"[" << pkCaster->Name() << L"] Caster must be UT_PLAYER or UT_ENTITY, GroundNo is not Find");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID kVec;
	//bool bFindParty = g_kLocalPartyMgr.GetPartyMemberGround(rkPartyGuid, m_kGndKey, kVec);

	bool bFindParty = false;
	if( IsExpedition )
	{
		bFindParty = m_pkGround->GetExpeditionMemberGround(ExpeditionGuid, m_kGndKey, kVec);
	}
	else
	{
		bFindParty = m_pkGround->GetPartyMemberGround(kPartyGuid, m_kGndKey, kVec);
	}
		
	if( bFindParty )
	{
		//Share_Party_Distance(pkCaster, kVec);

		__int64 const i64RetExp = PgAction_AddExp::ResultExp(m_iAddExp, 100);	// 100�� �⺻��
		BM::Stream kNfyPacket(PT_U_G_NFY_EXP_SHARE_PARTY_MEMBER);
		kNfyPacket.Push(kVec);
		kNfyPacket.Push(i64RetExp);
		kNfyPacket.Push(m_iMonsterLv);
		kNfyPacket.Push((m_eCause|AEC_ShareParty));
		kNfyPacket.Push(pkCaster->GetPos());
		pkCaster->VNotify(&kNfyPacket);
	}
	else
	{
		goto __PASS_ADVANCE;//���� ������ �н�
	}
	return true;

__PASS_ADVANCE://���� ���� ����
	return PassAdvance(pkCaster, pkTarget);
}

void PgAction_AddExp::Share_Party_Distance(CUnit* pkCaster, VEC_GUID& kVec)
{
	VEC_GUID::iterator iter = kVec.begin();
	while(kVec.end() != iter)
	{
		BM::GUID const & rkCharGuid = (*iter);
		CUnit* pkUnit = m_pkGround->GetUnit(rkCharGuid);
		if( pkUnit
		&&	pkUnit->IsUnitType(UT_PLAYER) )
		{
			POINT3 const &rkCurPos = pkUnit->GetPos();
			float const fDistance = POINT3::Distance(pkCaster->GetPos(), rkCurPos);
			float const fCanAddExpDistance = 3500.f;
			if( fCanAddExpDistance < fDistance )
			{
				iter = kVec.erase(iter);
				continue;
			}
		}
		++iter;
	}
}

__int64 PgAction_AddExp::ResultExp(__int64 const iAddExp, int const iAddExpRate)
{
	// iAddExpRate = PlayerBasic(100) + ��������(�޺�+����ġ������+ä���������+�޽İ���ġ+����Ȩ������ġ�������) + ��Ƽ�й�
	/* 
		��ɿ� ���� ����ġ ���� =
			���Ͱ���ġ * �����������Ѻ��� * �̺�Ʈ����ġ * iAddExpRate
	*/
	/*
	static int iVerifyServiceRegion = -1;
	if (iVerifyServiceRegion < 0)
	{
		switch (g_kLocal.ServiceRegion())
		{
		case LOCAL_MGR::NC_CHINA:
		case LOCAL_MGR::NC_TAIWAN:
		case LOCAL_MGR::NC_SINGAPORE:
		case LOCAL_MGR::NC_EU:
		case LOCAL_MGR::NC_ENGLAND:
		case LOCAL_MGR::NC_GERMANY:
		case LOCAL_MGR::NC_FRANCE:
		case LOCAL_MGR::NC_USA:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Wrong Exp calculation : Invalid for this ServiceRegion=") << g_kLocal.ServiceRegion());
			}break;
		default:
			{
				iVerifyServiceRegion = 0;
			}break;
		}
	}
	*/

	PgEventAbil const * const pkEventAbil = ( m_pkGround ? m_pkGround->GetEventAbil() : NULL );

	// 2�� �̺�Ʈ�� 200 ���� �Է��ؾ� ��.
	float const fEvent = (pkEventAbil ? std::max( pkEventAbil->GetAbil(AT_ADD_EXP_PER)/100.f, 1.0f) : 1.0f );
	float const fMutator = (m_pkGround ? std::max( m_pkGround->GetMutatorAbil(AT_ADD_EXP_PER)/100.f, 1.0f) : 1.0f);

	return static_cast<__int64>(iAddExp * fEvent * fMutator * iAddExpRate / 100.f);
}

bool PgAction_AddExp::PassAdvance(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (pkCaster->FindEffect(LOCK_EXP_EFFECT))
	{
		return true;
	}

	if( UT_PLAYER == pkCaster->UnitType() )
	{
		PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if( pPlayer )
		{
			float DefenceBonus = pPlayer->DefenceMssionExpBonusRate();	// ���� �Ƿε� ���ʽ� ����.
			if( DefenceBonus > 0 )
			{
				float TempExp = static_cast<float>(m_iAddExp);
				m_iAddExp = static_cast<int>( m_iAddExp * DefenceBonus );	// ����ġ ���ʽ� ���� ����. �Ҽ��� ���� ����.
			}
		}
	}

	int const iFinalAddExpRate = std::max(pkCaster->GetAbil(AT_ADDEXP_FINAL),0);
	int iAddExpRate = iFinalAddExpRate;

	bool IsRestMode = false;
	static int iRestBonusExpRate = g_kEventView.VariableCont().iExpAdd_AddedExpRate_Hunting;	//�޽İ���ġ ���ʽ�
	__int64 const i64MaxRestBonusExp = pkCaster->GetAbil64(AT_REST_EXP_ADD_MAX);
	__int64 const i64CurExp = pkCaster->GetAbil64(AT_EXPERIENCE);
	if ( 0 == (AEC_PetExp & m_eCause) )
	{
		// ���� �ִ� ���� �ƴ� ��쿡�� ���� �Ѵ�.
		if (i64CurExp < i64MaxRestBonusExp) // ���� �ִ� ����ġ�� �޽� ����ġ�� ���� ���� �ʴ´�.
		{
			iAddExpRate += iRestBonusExpRate;
			IsRestMode = true;
		}

		iAddExpRate += m_iAddedExpRate;
	}

	__int64 const i64RetExp = PgAction_AddExp::ResultExp(m_iAddExp,iAddExpRate);

	if(i64MaxRestBonusExp > 0 && (i64CurExp + i64RetExp) >= i64MaxRestBonusExp)
	{
		pkCaster->SetAbil64(AT_REST_EXP_ADD_MAX, 0i64, true);
	}

	HRESULT hAddExpRet = E_FAIL;
	switch( pkCaster->UnitType() )
	{
	case UT_PLAYER:
		{
			CONT_PLAYER_MODIFY_ORDER kOrder;

			int const iDiffLv = (m_iMonsterLv)? m_iMonsterLv - pkCaster->GetAbil(AT_LEVEL): 0;

			PgInventory * pkInv = pkCaster->GetInven();

			if( iCanAddExpMinLevel <= iDiffLv
			&&	iCanAddExpMaxLevel >= iDiffLv )//�������� ���� ���̸� ok
			{
				PgBase_Item kItem;
				SItemPos kItemPos;

				__int64 i64UserExp = i64RetExp;

				if(S_OK == pkInv->GetActivateExpCardItem(kItem,kItemPos))
				{
					SExpCard kExpCard;
					if(true == kItem.Get(kExpCard))
					{
						__int64 i64AddExp = (i64RetExp * kExpCard.ExpPer())/ABILITY_RATE_VALUE;

						__int64 const iNextExp = kExpCard.CurExp() + i64AddExp;
						if( kExpCard.MaxExp() <= iNextExp ) // �� á��. �ʱ�ȭ...
						{
							pkInv->ExpCardItem(BM::GUID::NullData());
						}

						if( 0 < i64AddExp )
						{
							i64UserExp -= i64AddExp;
						
							kExpCard.CurExp(i64AddExp);

							kOrder.push_back(SPMO(IMET_MODIFY_EXTEND_DATA,pkCaster->GetID(),SPMOD_ExtendData(kItem, kItemPos,kExpCard)));
						}
					}
				}

				if(0 < i64UserExp)// ����ġ ī��� �� ����ġ���� 100%�� �ƴϸ� ������ �������� �ش�.
				{
					if(true == IsRestMode)
					{
						double const dExpPer = i64UserExp/i64RetExp;

						__int64 const i64OrgExp = PgAction_AddExp::ResultExp(m_iAddExp,iFinalAddExpRate+m_iAddedExpRate) * dExpPer;	// ������, �̺�Ʈ ����� ���� ����ġ
						__int64 const i64ExtraExp = std::max<__int64>(0,i64RetExp - i64OrgExp) * dExpPer; // �߰� �޽� ����ġ 

						SPMOD_AddAbilEX kAddAbilData(AT_EXPERIENCE, i64UserExp, i64OrgExp, i64ExtraExp);
						kOrder.push_back(SPMO(IMET_ADD_ABIL64EX, pkCaster->GetID(), kAddAbilData));
					}
					else
					{
						SPMOD_AddAbil kAddAbilData(AT_EXPERIENCE, i64UserExp);
						kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkCaster->GetID(), kAddAbilData));
					}

					// Pet���� �߰� ����ġ�� �־����
					if ( AEC_KillMonster & m_eCause )
					{
						__int64 i64PetExp = (i64UserExp * ms_i64TakePCExpRate / ABILITY_RATE_VALUE64 );
						if ( 0i64 < i64PetExp )
						{
							PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
							if ( pkPlayer )
							{
								PgPet *pkPet = m_pkGround->GetPet( pkPlayer );
								if (	pkPet 
									&&	( EPET_TYPE_2 == pkPet->GetPetType() || EPET_TYPE_3 == pkPet->GetPetType() ) )
								{ 
									i64PetExp += ( i64PetExp * static_cast<__int64>(pkPet->GetAbil(AT_INVEN_PET_ADD_HUNT_EXP_RATE)) / ABILITY_RATE_VALUE64 );

									GET_DEF(PgClassPetDefMgr,kClassPetDefMgr);
									PgActPet_AddExp kAction(kClassPetDefMgr);

									SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
									if ( true == kAction.PopAbilList( pkPet, i64PetExp, kAddAbilPet.kAbilList ) )
									{
										kOrder.push_back( SPMO( IMET_PET, pkCaster->GetID(), kAddAbilPet ) );
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if( 0 != (m_eCause & AEC_KillMonster) )
				{
					BM::Stream kNfyPacket(PT_M_C_NFY_CHANGE_EXPERIENCE);
					pkCaster->Send(kNfyPacket);
				}
			}

			if( !kOrder.empty() )
			{
				if( (AEC_KillMonster|AEC_PetExp|AEC_AlramMission) & m_eCause )
				{
					PgAction_ReqModifyItem kItemModifyAction(MCE_EXP, m_kGndKey, kOrder);
					kItemModifyAction.DoAction(pkCaster, NULL);//���� �׾������� �߿�ġ �ʴ�.
				} 
				else if( AEC_MissionBonus & m_eCause )
				{
					PgAction_ReqModifyItem kItemModifyAction(IMEPT_MISSIONBONUS, m_kGndKey, kOrder);
					kItemModifyAction.DoAction(pkCaster, NULL);//���� �׾������� �߿�ġ �ʴ�.
				}
			}

			hAddExpRet = S_OK;
		}break;
	case UT_ENTITY:
		{
			BM::Stream kNfyPacket(PT_U_G_NFY_ADD_EXP);
			kNfyPacket.Push(i64RetExp);
			kNfyPacket.Push(m_iMonsterLv);
			pkCaster->VNotify(&kNfyPacket);
			hAddExpRet = S_OK;
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't Addexp Unit [Class:" << pkCaster->GetAbil(AT_CLASS) << L"][UnitType:" << pkCaster->UnitType() << L"]");
			hAddExpRet = E_FAIL;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}

	if( S_OK == hAddExpRet )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ShareExpParty
//////////////////////////////////////////////////////////////////////////////
PgAction_ShareExpParty::PgAction_ShareExpParty(__int64 const iAddedExp, const EAddExpCause eCause, int const iMonsterLv, POINT3 const &rkFromPos, PgGround const * const pkGround, int const iPartyMemberCount, PgLocalPartyMgr &kLocalPartyMgr, int const iAddExpRate)
	: m_iAddedExp(iAddedExp), m_eCause(eCause), m_iMonsterLv(iMonsterLv), m_kFromPos(rkFromPos)
	,m_pkGround(pkGround), m_iPartyMemberCount(iPartyMemberCount),  m_kLocalPartyMgr(kLocalPartyMgr), m_iAddExpRate(iAddExpRate)
{
}

float PgAction_ShareExpParty::PartyMemberBonusRate(int const iPartyMemberCount)
{
	// ���� ����ġ ���Ŀ��� ����ϴ� ��
	/*
	switch( iPartyMemberCount )
	{
	case 1:
		{
			return 1.0f;
		}break;
	case 2:
		{
			return 0.75f;
		}break;
	case 3:
		{
			return 0.78f;
		}break;
	case 4:
	default:
		{
			return 0.81f;
		}break;
	}
	return 0.f;
	*/

	// 2009.12.08 ����ġ ���� �ٲ�鼭 ����� ��
	switch( iPartyMemberCount )
	{
	case 1:
		{
			return 0.f;
		}break;
	case 2:
		{
			return -0.25f;
		}break;
	case 3:
		{
			return -0.3f;
		}break;
	case 4:
	default:
		{
			return -0.35f;
		}break;
	}
	return -1.0f;
}

bool PgAction_ShareExpParty::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pkPC->IsDead() )
	{
		return true; // �׾������� �� �Դ´�
	}

	if (pkCaster->FindEffect(LOCK_EXP_EFFECT))
	{
		return true;
	}

	POINT3 const &rkCurPos = pkPC->GetPos();
	float const fDistance = POINT3::Distance(m_kFromPos, rkCurPos);
	float const fCanAddExpDistance = 3500.f;
	/*if( fCanAddExpDistance < fDistance )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; //�Ÿ��� �ʹ� �ִ�
	}*/

	size_t const iPartyMemberCnt = m_iPartyMemberCount; //g_kLocalPartyMgr.GetMemberCount(pkPC->PartyGuid());

	int const iLevel = pkPC->GetAbil(AT_LEVEL);
	float const fPartyBonusRate = PgAction_ShareExpParty::PartyMemberBonusRate(iPartyMemberCnt);

	int const iDiffLv = (m_iMonsterLv)? m_iMonsterLv - iLevel: 0;
	if( iCanAddExpMinLevel <= iDiffLv
	&&	iCanAddExpMaxLevel >= iDiffLv )//�������� ���� ���̸� ok
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		int iPrivateAddExpPer = std::max(pkCaster->GetAbil(AT_ADDEXP_FINAL),0);
		static int iRestBonusExpRate = g_kEventView.VariableCont().iExpAdd_AddedExpRate_Hunting;	//�޽İ���ġ ���ʽ�
		__int64 const i64MaxRestBonusExp = pkPC->GetAbil64(AT_REST_EXP_ADD_MAX);
		__int64 const i64CurExp = pkPC->GetAbil64(AT_EXPERIENCE);
		bool bIsRestExpMode = false;

		if (i64CurExp < i64MaxRestBonusExp)
		{
			// �÷��̾��� ���� ����ġ ���� ContentsServer�� ���� update���� �ʾ�, ���ǹ��� �ణ ����Ȯ�� �� �ִ�.
			iPrivateAddExpPer += iRestBonusExpRate;
			bIsRestExpMode = true;
		}

		// static_cast<int>(fPartyBonusRate * 100.f) <-- fPartyBonusRate = 0.3000001f �� ����Ǿ� ����ġ�� 1��ŭ ���� �� �� �ִ�.
		__int64 iFinalExp = m_iAddedExp * (m_iAddExpRate + iPrivateAddExpPer + static_cast<int>(fPartyBonusRate * 100.f)) / 100;

		PgInventory* pkInv = pkPC->GetInven();
		PgBase_Item kItem;
		SItemPos kItemPos;

		if( pkInv && (S_OK == pkInv->GetActivateExpCardItem(kItem,kItemPos)) )
		{
			SExpCard kExpCard;
			if( true == kItem.Get(kExpCard) )
			{
				__int64 i64AddExp = (iFinalExp * kExpCard.ExpPer())/ABILITY_RATE_VALUE;
				__int64 const iNextExp = kExpCard.CurExp() + iFinalExp;
				if( kExpCard.MaxExp() <= iNextExp )
				{
					pkInv->ExpCardItem(BM::GUID::NullData());
				}

				if( 0 < i64AddExp )
				{
					iFinalExp &= 0;
					kExpCard.CurExp(i64AddExp);
					kOrder.push_back(SPMO(IMET_MODIFY_EXTEND_DATA,pkCaster->GetID(),SPMOD_ExtendData(kItem, kItemPos,kExpCard)));
					goto __EXP_END;
				}
			}
		}

		if(i64MaxRestBonusExp > 0 && (i64CurExp + iFinalExp) >= i64MaxRestBonusExp)
		{
			pkPC->SetAbil64(AT_REST_EXP_ADD_MAX, 0i64, true);
		}

		if (true == bIsRestExpMode)
		{
			int const iAddExpNoRest = iPrivateAddExpPer - iRestBonusExpRate;
			__int64 const i64FinalExpNoRest = m_iAddedExp * (m_iAddExpRate + iAddExpNoRest + static_cast<int>(fPartyBonusRate * 100.f)) / 100;
			__int64 const i64RestAddedExp = std::max<__int64>(0, iFinalExp - i64FinalExpNoRest);
			SPMOD_AddAbilEX kAddAbilData(AT_EXPERIENCE, iFinalExp, i64FinalExpNoRest, i64RestAddedExp);
			kOrder.push_back(SPMO(IMET_ADD_ABIL64EX, pkCaster->GetID(), kAddAbilData));
		}
		else
		{
			SPMOD_AddAbil kAddExpData(AT_EXPERIENCE, iFinalExp);
			kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkCaster->GetID(), kAddExpData));
		}

		// Pet���� �߰� ����ġ�� �־����
		if ( AEC_KillMonster & m_eCause )
		{
			__int64 i64PetExp = (iFinalExp * PgAction_AddExp::ms_i64TakePCExpRate / ABILITY_RATE_VALUE64 );
			if ( 0i64 < i64PetExp )
			{
				PgPet *pkPet = m_pkGround->GetPet( pkPC );
				if (	pkPet 
					&&	( EPET_TYPE_2 == pkPet->GetPetType() || EPET_TYPE_3 == pkPet->GetPetType() ) )
				{ 
					i64PetExp += ( i64PetExp * static_cast<__int64>(pkPet->GetAbil(AT_INVEN_PET_ADD_HUNT_EXP_RATE)) / ABILITY_RATE_VALUE64 );

					GET_DEF(PgClassPetDefMgr,kClassPetDefMgr);
					PgActPet_AddExp kAction(kClassPetDefMgr);

					SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );
					if ( true == kAction.PopAbilList( pkPet, i64PetExp, kAddAbilPet.kAbilList ) )
					{
						kOrder.push_back( SPMO( IMET_PET, pkCaster->GetID(), kAddAbilPet ) );
					}		
				}
			}
		}

		// Fran
		if( iPartyMemberCnt > 1 )
		{
			int iFriendCount = 0;
			//iFriendCount = g_kLocalPartyMgr.GetPartyMemberFriend(pkPC->PartyGuid(), pkPC->GetID());
			iFriendCount = m_kLocalPartyMgr.GetPartyMemberFriend(pkPC->PartyGuid(), pkPC->GetID());			

			if( iFriendCount )
			{				
				int iValue = 0;
				if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_FranExp, iValue) )
				{
					CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't Find 'FranExp_SystemEnable'");
				}

				__int64 iResultFran = 0;
				iResultFran = static_cast<__int64>(iFinalExp * iValue/100.0f);
				
				if(0 < pkPC->GetAbil(AT_R_FRAN))
				{
					iResultFran = (iResultFran * pkPC->GetAbil(AT_R_FRAN))/ABILITY_RATE_VALUE64;
				}

				if( pkCaster->GetAbil(AT_LEVEL) < PgClassDefMgr::s_sMaximumLevel )
				{
					SPMOD_AddAbil kAddExpData(AT_FRAN, std::max(iResultFran, static_cast<__int64>(1)));
					kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkCaster->GetID(), kAddExpData));
				}
			}
		}

		__EXP_END:
		PgAction_ReqModifyItem kItemModifyAction(MCE_EXP, m_pkGround->GroundKey(), kOrder);
		kItemModifyAction.DoAction(pkCaster, pkTarget);
	}
	else
	{
		if( 0 != (m_eCause & AEC_KillMonster) )
		{
			BM::Stream kNfyPacket(PT_M_C_NFY_CHANGE_EXPERIENCE);
			pkCaster->Send(kNfyPacket);
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_ShareGold
//////////////////////////////////////////////////////////////////////////////
PgAction_ShareGold::PgAction_ShareGold(PgGround* const pkGround, __int64 const iAddGold, EItemModifyParentEventType const kCause, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream const &kPacket)
	:m_pkGround(pkGround), m_iAddGold(iAddGold), m_kCause(kCause), m_kAddonPacket(kPacket), m_kLocalPartyMgr(kLocalPartyMgr)
{
}

bool PgAction_ShareGold::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if (m_pkGround == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Ground is NULL"));
		return false;
	}
	if( !m_iAddGold )//��尡 0�̸�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( INT_MAX < m_iAddGold
	&& INT_MIN > m_iAddGold )
	{
		INFO_LOG(BM::LOG_LV1, __FL__ << L"Critical Error: Add Gold value is over or minimal 32bit signed limit value [" << m_iAddGold << L"] ");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	BM::GUID const & rkPartyGuid = pkPC->PartyGuid();
	BM::GUID const & ExpeditionGuid = pkPC->ExpeditionGuid();
	bool bParty = false;
	bool bExpedition = false;

	bExpedition = (BM::GUID::NullData() != ExpeditionGuid);
	bParty = (BM::GUID::NullData() != rkPartyGuid);

	if(!bParty && !bExpedition )
	{
		if(m_kCause == MCE_Loot)// ��Ƽ ���� �ܵ� ����� ��� ����ġ
		{
			__int64 const iFinalGold = PgAction_ShareGold::PrivateShareGold(m_iAddGold,std::max(pkCaster->GetAbil(AT_ADD_MONEY_PER),0), m_pkGround->GetEventAbil());
			PgAction_ReqAddMoney kAddGoldAction(m_kCause, iFinalGold, m_pkGround->GroundKey(), m_kAddonPacket);
			return kAddGoldAction.DoAction(pkCaster, pkTarget);
		}
		else
		{
			return Pass(pkCaster, pkTarget);
		}
	}
	else
	{
		VEC_GUID kVec;
		bool bFindParty = false;
		if( bExpedition )		// �������� ������ ����� ��� ����.
		{
			bFindParty = m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, m_pkGround->GroundKey(), kVec);
		}
		else					// �Ϲ� ��Ƽ�� ���.
		{
			bFindParty = m_kLocalPartyMgr.GetPartyMemberGround(rkPartyGuid, m_pkGround->GroundKey(), kVec);
		}

		if( bFindParty )
		{
			int const iMemberCount = NiMax((int)kVec.size(), 1);
			int const iResultPerGold = (int)PgAction_ShareGold::PartyMemberShareGold(m_iAddGold, iMemberCount);

			BM::Stream kNfyPacket(PT_U_G_NFY_GOLD_SHARE_PARTY_MEMBER);
			kNfyPacket.Push(kVec);
			kNfyPacket.Push(iResultPerGold);
			kNfyPacket.Push(MCE_ShareParty);
			pkCaster->VNotify(&kNfyPacket);
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bFindParty is false"));
			return Pass(pkCaster, pkTarget);
		}
	}

	return true;
}

__int64 PgAction_ShareGold::PartyMemberShareGold(__int64 const iAddGold, int const iMemberCount)
{
	__int64 const iResultGold = iAddGold / iMemberCount;
	return std::max(iResultGold, 1i64); //�ּ� 1
}

__int64 PgAction_ShareGold::PrivateShareGold( __int64 const iAddGold, int const iAddGoldRate, PgEventAbil const * const pkEventAbil )
{
	__int64 iResultGold = iAddGold;

	int const iEventAddGoldRate = ( pkEventAbil ? std::max( pkEventAbil->GetAbil(AT_ADD_MONEY_PER),0 ) : 0 ) + iAddGoldRate;

	if(iEventAddGoldRate > 0)
	{
		iResultGold += SRateControl::GetValueRate<int>( iResultGold, iEventAddGoldRate );
	}

	return std::max(iResultGold, 1i64); //�ּ� 1
}

bool PgAction_ShareGold::Pass(CUnit* pkCaster, CUnit* pkTarget)
{
	if (m_pkGround != NULL)
	{
		__int64 iResultGold = m_iAddGold;
		int const iRate = pkCaster->GetAbil(AT_ADD_MONEY_PER);
		if( iRate )
		{
			iResultGold += SRateControl::GetValueRate<int>( iResultGold, iRate );
		}
		PgAction_ReqAddMoney kAddGoldAction(m_kCause, iResultGold, m_pkGround->GroundKey(), m_kAddonPacket);
		return kAddGoldAction.DoAction(pkCaster, pkTarget);
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Ground is NULL"));
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_RaceEventRewardItem
//////////////////////////////////////////////////////////////////////////////
bool PgAction_RaceEventRewardItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(pkCaster == NULL || pkTarget == NULL) { return false; }
	if(m_iRewardItemGroupNo == 0) { return false; }

	CONT_DEF_EVENT_REWARD_ITEM_GROUP const * pRewardItemGroup = NULL;
	g_kTblDataMgr.GetContDef(pRewardItemGroup);

	if( NULL == pRewardItemGroup )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Failed to Load TB_DefEventRewardItemGroup!") );
		return false;
	}
	CONT_DEF_EVENT_REWARD_ITEM_GROUP::const_iterator item_iter = pRewardItemGroup->find(m_iRewardItemGroupNo);
	if( item_iter == pRewardItemGroup->end() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot Find Reward Item Group No: ") << m_iRewardItemGroupNo);
		return false;
	}

	int iTotalRate = 0;
	VEC_EVENTREWARDITEMGROUP::const_iterator drop_iter = (*item_iter).second.begin();
	for( ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
	{
		iTotalRate += (*drop_iter).Rate;
	}

	int iRate = BM::Rand_Range(iTotalRate, 0);
	int AccumRate = 0;
	drop_iter = (*item_iter).second.begin();
	int iPopItemNo = 0;
	int iPopItemCount = 0;
	for( ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
	{
		AccumRate += (*drop_iter).Rate;
		if(iRate < AccumRate)
		{
			iPopItemNo = (*drop_iter).ItemNo;
			iPopItemCount = (*drop_iter).Count;
			m_iRewardItemNo = (*drop_iter).ItemNo;
			break;
		}
	}

	if(iPopItemNo <= 0) { return false; }

	CONT_PLAYER_MODIFY_ORDER kOrderModify;
	SPMOD_Add_Any AddData(iPopItemNo,iPopItemCount);
	SPMO MODS_Add(IMET_ADD_ANY,pkTarget->GetID(),AddData);
	kOrderModify.push_back(MODS_Add);

	PgAction_ReqModifyItem kItemModifyAction(CIE_RaceEvent, m_kGndKey, kOrderModify);
	return kItemModifyAction.DoAction(pkCaster, pkTarget);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_DropItemBox
//////////////////////////////////////////////////////////////////////////////
PgAction_DropItemBox::PgAction_DropItemBox( PgGround const * const pkGround, TBL_DQT_DEF_MAP_ITEM_BAG const &kTbl )
	:	m_pkGround(pkGround)
	,	m_rkMapBag(kTbl)
{
}

bool PgAction_DropItemBox::DoAction( CUnit* pkCaster, CUnit* pkTarget )
{
	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkTarget);
	if(	pkCaster && pkPC )
	{
		bool const bDropAllItem = ( 0 < pkCaster->GetAbil(AT_MON_DROP_ALLITEM) );

		m_kContDropItem.clear();

		PgEventAbil const * const pkEventAbil = m_pkGround->GetEventAbil();

		int const iLevel = pkPC->GetAbil( AT_LEVEL );
		int const iSuccessRateCtrlNo = pkCaster->GetAbil(AT_ITEM_BAG_SUCCESS_RATE_CONTROL);
		int const iAddMoneyDropRate = (pkEventAbil ? pkEventAbil->GetAbil(AT_ADD_MONEY_RATE) : 0) + m_pkGround->GetMutatorAbil(AT_ADD_MONEY_PER);

		PgItemBag akItemBag[MIBI_COUNT];
		GET_DEF(CItemBagMgr, kItemBagMgr);

		{
			int iMinCount = std::max<int>( 0, pkCaster->GetAbil(AT_MIN_DROP_ITEM_COUNT) );
			int iMaxCount = std::min<int>(50, pkCaster->GetAbil(AT_MAX_DROP_ITEM_COUNT) );

			if ( iMaxCount <= 0 )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if( iMinCount > iMaxCount )
			{
				std::swap( iMinCount, iMaxCount );
			}

			int const iRetCount = BM::Rand_Range( iMaxCount, iMinCount );//Min(> 0) ~ Max( > Min)
			int const iItemContainerNo = pkCaster->GetAbil(AT_ITEM_DROP_CONTAINER);
			bool const bIsOnlyMapBag = (GATTR_BATTLESQUARE == m_pkGround->GetAttr());

			int const iItemBagNo_1 = pkCaster->GetAbil(AT_ITEM_BAG_PRIMARY);
			int const iItemBagNo_2 = pkCaster->GetAbil(AT_ITEM_BAG_SECONDARY);

			kItemBagMgr.GetItemBag(iItemBagNo_1, static_cast<short>(iLevel), akItemBag[MIBI_PRIMARY]);
			kItemBagMgr.GetItemBag(iItemBagNo_2, static_cast<short>(iLevel), akItemBag[MIBI_SECONDARY]);

			size_t iGrpIDX = 0;
			if( ::RouletteRate(m_rkMapBag.iSuccessRateNo, iGrpIDX, MAP_ITEM_BAG_GRP) )
			{
				kItemBagMgr.GetItemBagByGrp( m_rkMapBag.aiBagGrpNo[iGrpIDX], static_cast<short>(iLevel), akItemBag[MIBI_MAPBAG] );
				int const iAddDropRate = ( pkEventAbil ? pkEventAbil->GetAbil(AT_ADD_ITEMDROP_RATE) : 0 );
				if( 0 < iAddDropRate && (0 == (GATTR_FLAG_NOADDITEMDROPRATE & m_pkGround->GetAttr())) )
				{
					akItemBag[MIBI_MAPBAG].AddDropRate(iAddDropRate);
				}
			}
			
			int const EventMonsterAbil = pkCaster->GetAbil(AT_EVENT_MONSTER_DROP);
			if( 0 != EventMonsterAbil )
			{
				int RewardItemGroupNo = 0;
				int RewardItemCount = 0;

				if( pkCaster->GetAbil(AT_EVENT_MONSTER_NO_HAVE_GENINFO) )
				{
					RewardItemGroupNo = pkCaster->GetAbil(AT_EVENT_MONSTER_DROP_GROUP_NO);
					RewardItemCount = pkCaster->GetAbil(AT_EVENT_MONSTER_DROP_ITEM_COUNT);
				}
				else
				{
					SEventMonsterGen MonsterInfo;
					int MonsterNo = pkCaster->GetAbil(AT_EVENT_PARENT_MONSTER);
					if( 0 == MonsterNo )
					{
						MonsterNo = pkCaster->GetAbil(AT_CLASS);
					}
					m_pkGround->GetEventMonsterGenInfo(MonsterNo, MonsterInfo);

					CGameTime GameTime;
					__int64 const CurTime = GameTime.GetLocalSecTime();

					if( CurTime > MonsterInfo.DelTime )
					{
						return true;
					}

					RewardItemGroupNo = MonsterInfo.RewardItemGroupNo;
					RewardItemCount = MonsterInfo.RewardCount;
				}

				if( 0 != RewardItemGroupNo )
				{
					CONT_DEF_EVENT_REWARD_ITEM_GROUP const * pRewardItemGroup = NULL;
					g_kTblDataMgr.GetContDef(pRewardItemGroup);

					if( pRewardItemGroup )
					{
						CONT_DEF_EVENT_REWARD_ITEM_GROUP::const_iterator item_iter = pRewardItemGroup->find(RewardItemGroupNo);
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
									CONT_ITEM_CREATE_ORDER kOrder;
									if( OrderCreateItem(kOrder, (*drop_iter).ItemNo, (*drop_iter).Count, m_pkGround->GetGroundItemRarityNo()) )
									{
										for(CONT_ITEM_CREATE_ORDER::iterator it=kOrder.begin(); it!=kOrder.end(); ++it)
										{
											m_pkGround->CheckEnchantControl( *it );
											m_kContDropItem.push_back( *it );
										}
									}
								}
							}
							else if( ExceptionCase )
							{	// ���� ���̽���, �� �������� ��з��� ���.
								for(int i = 0 ; i < RewardItemCount ; ++i)
								{
									int Rate = BM::Rand_Range(100, 0);

									for(drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
									{
										if( Rate <= (*drop_iter).Rate )
										{
											CONT_ITEM_CREATE_ORDER kOrder;
											if( OrderCreateItem(kOrder, (*drop_iter).ItemNo, (*drop_iter).Count, m_pkGround->GetGroundItemRarityNo()) )
											{
												for(CONT_ITEM_CREATE_ORDER::iterator it=kOrder.begin(); it!=kOrder.end(); ++it)
												{
													m_pkGround->CheckEnchantControl( *it );
													m_kContDropItem.push_back( *it );
												}
											}
											break;
										}
									}
								}
							}
							else
							{								
								for(int i = 0 ; i < RewardItemCount ; ++i)
								{
									int Rate = BM::Rand_Index(TotalRate);
									int CurRate = 0;

									for(drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
									{								
										CurRate += (*drop_iter).Rate;
										if( Rate < CurRate )
										{
											CONT_ITEM_CREATE_ORDER kOrder;
											if( OrderCreateItem(kOrder, (*drop_iter).ItemNo, (*drop_iter).Count, m_pkGround->GetGroundItemRarityNo()) )
											{
												for(CONT_ITEM_CREATE_ORDER::iterator it=kOrder.begin(); it!=kOrder.end(); ++it)
												{
													m_pkGround->CheckEnchantControl( *it );
													m_kContDropItem.push_back( *it );
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

			m_pkGround->DropAnyMonsterCheck(m_kContDropItem);

			if ( 0 == iItemContainerNo )
			{
				int iItemBagNo = 0;

				int iBaseDropMoney = BM::Rand_Range( static_cast<int>(pkCaster->GetAbil64(AT_MONEY)), pkCaster->GetAbil(AT_MONEY_MIN) );
				int const iBaseDropMoneyRate = pkCaster->GetAbil(AT_ADD_MONEY_RATE);

				if(	0 < iBaseDropMoney
					&&	lwIsRandSuccess(iBaseDropMoneyRate) )
				{
					SEnchantInfo kInfo;
					kInfo.Field_1(iBaseDropMoney);

					PgBase_Item kItem;
					kItem.EnchantInfo(kInfo);

					//Item ��ȣ�� 0 �̸� ����Ҷ� ���� �ش�.
					m_kContDropItem.push_back(kItem);
				}

				size_t iIDX = MIBI_PRIMARY;
				if( bDropAllItem )
				{
					for( iIDX = 0; iIDX < MIBI_COUNT; ++iIDX )
					{
						PgItemBag::BagElementCont const & kElements = akItemBag[iIDX].GetElements();
						for( PgItemBag::BagElementCont::const_iterator iter = kElements.begin(); iter != kElements.end(); ++iter )
						{
							int const & iResultItem = (*iter).iItemNo;
							if( iResultItem )
							{
								PgBase_Item kItem;
								if( S_OK == ::CreateSItem( iResultItem, 1, m_pkGround->GetGroundItemRarityNo(), kItem ) )
								{
									m_pkGround->CheckEnchantControl(kItem);
									m_kContDropItem.push_back( kItem );
								}
							}
						}
					}
				}
				else
				{
					
					for ( int count=0; count < iRetCount; ++count )
					{
						int iItemBagNo = 0;
						if( ::RouletteRate( iSuccessRateCtrlNo, iIDX, MIBI_COUNT ) )
						{//Ȯ��ǥ�� ���� �ε����� ���Դ�.

							if(		!bIsOnlyMapBag
								||	MIBI_MAPBAG == iIDX )
							{
								int iResultItem = 0;
								if( S_OK == akItemBag[iIDX].PopItem( iLevel, iResultItem ) )
								{
									PgBase_Item kItem;
									if( S_OK == ::CreateSItem( iResultItem, 1, m_pkGround->GetGroundItemRarityNo(), kItem) )
									{
										m_pkGround->CheckEnchantControl(kItem);
										m_kContDropItem.push_back( kItem );
									}
								}
							}
						}
					}
				}	
			}
			else
			{
				CONT_DEF_ITEM_CONTAINER const *pkContContainer = NULL;
				g_kTblDataMgr.GetContDef(pkContContainer);

				CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pkContContainer->find(iItemContainerNo);
				if( item_cont_itor != pkContContainer->end() )
				{
					if( bDropAllItem )
					{// ItemBag�� ��� �������� ADD
						for( size_t iRetIDX=MIBI_PRIMARY; iRetIDX<MIBI_COUNT; ++iRetIDX )
						{
							PgItemBag kItemBag;
							int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
							if( S_OK == kItemBagMgr.GetItemBagByGrp(iResultBagGroupNo, static_cast<short>(iLevel), kItemBag) )
							{
								int iResultItem = 0;
								size_t iResultItemCount = 1;
								for(int iBagIndex=0; iBagIndex<kItemBag.GetElementsCount(); ++iBagIndex)
								{
									if(S_OK == kItemBag.PopItemToIndex( iLevel, iResultItem, iResultItemCount, iBagIndex ))
									{
										if(iResultItemCount <= 0)
										{
											VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("DropItemBox: ItemBag Element Item Count Error\n") << _T("ItemNo: ") << iResultItem << _T(", ItemCount: ") << iResultItemCount);
											return false;
										}

										PgBase_Item kItem;
										if(S_OK == CreateSItem(iResultItem, iResultItemCount, m_pkGround->GetGroundItemRarityNo(), kItem))
										{
											m_pkGround->CheckEnchantControl(kItem);
											m_kContDropItem.push_back( kItem );
										}
									}
								}
							}
						}//END for(size_t iRetIDX=0; iRetIDX<iBagCandidateCount; ++iRetIDX)
					}
					else
					{
						for ( int count=0; count < iRetCount; ++count )
						{
							size_t iRetIDX = 0;
							if( ::RouletteRate((*item_cont_itor).second.iSuccessRateControlNo, iRetIDX, MAX_ITEM_CONTAINER_LIST) )
							{
								if(		!bIsOnlyMapBag
									||	MIBI_MAPBAG == iRetIDX )
								{
									PgItemBag kItemBag;
									int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
									if( S_OK == kItemBagMgr.GetItemBagByGrp( iResultBagGroupNo, static_cast<short>(iLevel), kItemBag))
									{
										int iResultItem = 0;
										int resultCount = 1;
										if(S_OK == kItemBag.PopItem( iLevel, iResultItem, resultCount ) )
										{
											PgBase_Item kItem;
											if(S_OK == CreateSItem(iResultItem, resultCount,
												m_pkGround->GetGroundItemRarityNo(), kItem))
											{
												m_pkGround->CheckEnchantControl(kItem);
												m_kContDropItem.push_back( kItem );
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		/*=====================================================================================================================================
		�̺�Ʈ ������ ��� ó�� 
		=====================================================================================================================================*/

		const bool bDropEventItems = !(pkCaster->UnitType() == UT_BOSSMONSTER ||
			(pkCaster->UnitType() == UT_MONSTER && pkCaster->GetAbil(AT_GRADE) == EMGRADE_BOSS));
		int const iEventItemContainer = (bDropEventItems && pkEventAbil ? pkEventAbil->GetAbil(AT_EVENT_ITEM_CONTAINER) : 0);
		if( 0 < iEventItemContainer )
		{
			CONT_DEF_ITEM_CONTAINER const *pkContContainer = NULL;
			g_kTblDataMgr.GetContDef(pkContContainer);

			CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_itor = pkContContainer->find(iEventItemContainer);
			if(item_cont_itor != pkContContainer->end())
			{
				size_t iRetIDX = 0;
				if(::RouletteRate((*item_cont_itor).second.iSuccessRateControlNo, iRetIDX, MAX_ITEM_CONTAINER_LIST))
				{
					PgItemBag kItemBag;
					int const iResultBagGroupNo = (*item_cont_itor).second.aiItemBagGrpNo[iRetIDX];
					if( S_OK == kItemBagMgr.GetItemBagByGrp(iResultBagGroupNo, static_cast<short>(iLevel), kItemBag))
					{
						int iResultItem = 0;
						int itemcount = 1;
						if(S_OK == kItemBag.PopItem( iLevel, iResultItem, itemcount ) )
						{
							PgBase_Item kItem;
							if(S_OK == ::CreateSItem(iResultItem, itemcount, m_pkGround->GetGroundItemRarityNo(), kItem))
							{
								m_pkGround->CheckEnchantControl(kItem);
								m_kContDropItem.push_back( kItem );
							}
						}
					}
				}
			}
		}

		/*=====================================================================================================================================
		��ȹ ��û���� ������ ��� Ƚ���� �� ��� Ƚ���� �и� ������ �ش� ����� ��� ���� ������ �⺻ 1�� ������� ����
		=====================================================================================================================================*/
		{
			int iMaxCount = std::min<int>(50, pkCaster->GetAbil(AT_MAX_DROP_MONEY_COUNT) );
			if ( 0 < iMaxCount )
			{
				int iMinCount = std::max<int>( 0, pkCaster->GetAbil(AT_MIN_DROP_MONEY_COUNT) );
				if ( iMinCount > iMaxCount )
				{
					std::swap(iMinCount,iMaxCount);
				}

				int iRetMoneyCount = BM::Rand_Range(iMaxCount, iMinCount);//Min(> 0) ~ Max( > Min)

				size_t iIDX = 0;
				int iMoney = 0;
				while( iRetMoneyCount )
				{
					if( ::RouletteRate(iSuccessRateCtrlNo, iIDX, MIBI_COUNT) )
					{//Ȯ��ǥ�� ���� �ε����� ���Դ�.
						if(S_OK == akItemBag[iIDX].PopMoney(iMoney, iAddMoneyDropRate))
						{
							if( 0 < iMoney )
							{
								SEnchantInfo kInfo;
								kInfo.Field_1(iMoney);

								PgBase_Item kItem;
								kItem.EnchantInfo(kInfo);

								//Item ��ȣ�� 0 �̸� ����Ҷ� ���� �ش�.
								m_kContDropItem.push_back(kItem);
							}
						}
					}
					--iRetMoneyCount;
				}
			}
		}
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}




//////////////////////////////////////////////////////////////////////////////
//		PgAction_ExchaneItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ExchangeItem::PgAction_ExchangeItem(SGroundKey const& rkGndKey, const STradeInfo &kInfo)
	: m_kGndKey(rkGndKey), m_kInfo(kInfo)
{
}

bool PgAction_ExchangeItem::IsCorrectTradeList(PgInventory* pkInv, CONT_TRADE_ITEM const &kContTrade)
{
	CONT_TRADE_ITEM::const_iterator itor = kContTrade.begin();
	while(itor != kContTrade.end())
	{
		PgBase_Item kItem;
		if(S_OK != pkInv->GetItem((*itor).kPos, kItem))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if(kItem != (*itor).kItem)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++itor;
	}
	return true;
}

void PgAction_ExchangeItem::TradeListToOrderCont(BM::GUID const &kCaster, BM::GUID const &kTarget, CONT_TRADE_ITEM const &kContCasterTrade, CONT_TRADE_ITEM const &kContTargetTrade, CONT_PLAYER_MODIFY_ORDER &kOrder)
{	//����� ��Ŷ.
	//�ֱ� ��Ŷ���� ����.
{
	CONT_TRADE_ITEM::const_iterator itor = kContCasterTrade.begin();
	while(itor != kContCasterTrade.end())
	{//�κ� ����
		SPMOD_Modify_Count kDelData((*itor).kItem, (*itor).kPos, 0, true );
		SPMO kIMO(IMET_MODIFY_COUNT|IMC_POS_BY_EXCHANGE, kCaster, kDelData);//��ȯ �÷��׷� ���� �������Դ� ����.
		kOrder.push_back(kIMO);

		++itor;
	}
}
{
	CONT_TRADE_ITEM::const_iterator itor = kContTargetTrade.begin();
	while(itor != kContTargetTrade.end())
	{//�κ� ����
		SPMOD_Modify_Count kDelData((*itor).kItem, (*itor).kPos, 0, true );
		SPMO kIMO(IMET_MODIFY_COUNT|IMC_POS_BY_EXCHANGE, kTarget, kDelData);//��ȯ �÷��׷� ���� �������Դ� ����.
		kOrder.push_back(kIMO);

		++itor;
	}
}
{
	CONT_TRADE_ITEM::const_iterator itor = kContCasterTrade.begin();
	while(itor != kContCasterTrade.end())
	{//������ �߰�
		SItemPos kItemPos;//Temp Value
		SPMOD_Insert_Fixed kAddData((*itor).kItem, kItemPos, true);//1�� ����.
		SPMO kIMO(IMET_INSERT_FIXED|IMC_POS_BY_EXCHANGE, kTarget, kAddData);//ĳ������ �������� Ÿ�ٿ���
		kOrder.push_back(kIMO);

		++itor;
	}
}
{
	CONT_TRADE_ITEM::const_iterator itor = kContTargetTrade.begin();
	while(itor != kContTargetTrade.end())
	{//������ �߰�
		SItemPos kItemPos;//Temp Value
		SPMOD_Insert_Fixed kAddData((*itor).kItem, kItemPos, true);//1�� ����.
		SPMO kIMO(IMET_INSERT_FIXED|IMC_POS_BY_EXCHANGE, kCaster, kAddData);//Ÿ���� �������� ĳ���Ϳ���
		kOrder.push_back(kIMO);

		++itor;
	}
}
}

bool PgAction_ExchangeItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(	pkCaster 
	&&	pkTarget)
	{
		PgInventory* pkCasterInv = pkCaster->GetInven();
		PgInventory* pkTargetInv = pkTarget->GetInven();
		
		//�ֳ� Ȯ��.
		if(	IsCorrectTradeList(pkCasterInv, m_kInfo.kCasterItemCont)
		&&	IsCorrectTradeList(pkTargetInv, m_kInfo.kTargetItemCont))
		{
			CONT_PLAYER_MODIFY_ORDER kOrder;

			TradeListToOrderCont(m_kInfo.kCasterGuid, m_kInfo.kTargetGuid, m_kInfo.kCasterItemCont, m_kInfo.kTargetItemCont, kOrder);
//			TradeListToOrderCont(m_kInfo.kTargetGuid, m_kInfo.kCasterGuid, m_kInfo.kTargetItemCont, kOrder);

			//�ٵ� ����
			//if (-m_kInfo.i64CasterMoney + m_kInfo.i64TargetMoney)//���� ���� ��ȯ�ϸ� �� ��ƾ�� Ÿ�� ����!
			{
				SPMOD_Add_Money kDelMoneyData(-m_kInfo.i64CasterMoney + m_kInfo.i64TargetMoney);
				SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
				kOrder.push_back(kIMO);
			}

			//if(-m_kInfo.i64TargetMoney + m_kInfo.i64CasterMoney)//
			{
				SPMOD_Add_Money kDelMoneyData(-m_kInfo.i64TargetMoney + m_kInfo.i64CasterMoney);
				SPMO kIMO(IMET_ADD_MONEY, pkTarget->GetID(), kDelMoneyData);
				kOrder.push_back(kIMO);
			}

			//��ȯ ����.
			PgAction_ReqModifyItem kItemModifyAction(CIE_Exchange, m_kGndKey, kOrder);
			return kItemModifyAction.DoAction(pkCaster, pkTarget);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}





//////////////////////////////////////////////////////////////////////////////
//		PgAction_ViewOtherEquip
//////////////////////////////////////////////////////////////////////////////

PgAction_ViewOtherEquip::PgAction_ViewOtherEquip(SGroundKey const& rkGndKey)
	:m_kGndKey(rkGndKey)
{

}

bool PgAction_ViewOtherEquip::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(!pkTarget)
	{
		//Error Msg 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory* pkTargetInv = pkTarget->GetInven();

	BM::Stream kPacket(PT_M_C_ANS_VIEW_OTHER_EQUIP);
	pkTargetInv->WriteToPacket(IT_FIT, kPacket);
	pkTargetInv->WriteToPacket(IT_FIT_CASH, kPacket);
	pkTargetInv->WriteToPacket(IT_FIT_COSTUME, kPacket);

	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_EffectControl
//////////////////////////////////////////////////////////////////////////////
PgAction_EffectControl::PgAction_EffectControl(SGroundKey const& rkGndKey, int const iEffectNo)
:	m_kGndKey(rkGndKey)
,	m_iEffectNo(iEffectNo)
{
}

bool PgAction_EffectControl::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	if ( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	const CEffectDef *pkDef = kEffectDefMgr.GetDef(m_iEffectNo);
	if ( !pkDef )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEffect *pkEffect = pkCaster->GetEffect(m_iEffectNo);
	if ( !pkEffect )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// ���˻�
	// �������� üũ
	int const iLevel = pkCaster->GetAbil(AT_LEVEL);
	__int64 const iNeedMoney = (__int64)(pkDef->GetAbil(AT_EFFCONTROL_DEL_MONEY)+(pkDef->GetAbil(AT_EFFCONTROL_DEL_MONEY_PERLEVEL)*iLevel));
	if ( iNeedMoney > 0 )
	{
		__int64 const iMyMoney = pkCaster->GetAbil64(AT_MONEY);
		if ( iNeedMoney > iMyMoney )
		{
			// ���� �����ϴ�
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if ( iNeedMoney )
	{
		// ���� ������ ���� ���
		PgAction_ReqAddMoney kMoneyAction(MCE_EffectControl,-iNeedMoney,m_kGndKey);
		kMoneyAction.DoAction(pkCaster,pkNothing);
	}

	return pkCaster->DeleteEffect(m_iEffectNo);
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_SpendMoney
//////////////////////////////////////////////////////////////////////////////
PgAction_SpendMoney::PgAction_SpendMoney()
:	m_pkGnd(NULL), m_kType(ESMT_NONE)
{}

PgAction_SpendMoney::PgAction_SpendMoney(const ESpendMoneyType kType,const PgGround* pkGnd)
:	m_kType(kType),m_pkGnd(pkGnd) 
{}

bool PgAction_SpendMoney::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	if ( !m_pkGnd || !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(m_pkGnd->GetAttr() & GATTR_EVENT_GROUND)
	{ //���� ���� �̺�Ʈ �����̰� ������ ��Ȱ ������ ����� �Ұ� ���¶��..
		PgStaticEventGround const* pkEventGround = dynamic_cast<PgStaticEventGround const*>(m_pkGnd);
		if(pkEventGround && pkEventGround->UnusableReviveItem())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkCaster);
	if ( !pkUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// �ʿ��� �� �� �˻�
	GET_DEF(PgDefSpendMoneyMgr, kDefSpendMoneyMgr);
	__int64 const iNeedMoney = (__int64)(kDefSpendMoneyMgr.GetSpendMoney(m_kType,pkCaster->GetAbil(AT_LEVEL)));

	// ���� ����Ѱ� �˻�
	if ( iNeedMoney > pkCaster->GetAbil64(AT_MONEY) )
	{
		// ���� �����ϴ�...
		pkCaster->SendWarnMessage(80024);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch(m_kType)
	{
	case ESMT_REVIVE_MISSION:
		{
			if(pkCaster->IsAlive())
			{	// ��� �������� ��� �� �� ����.
				pkCaster->SendWarnMessage(243);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if ( m_pkGnd->GetAttr() & GATTR_MISSION )
			{
				// ���� ������ ���� ���
				if ( iNeedMoney > 0 )
				{
					PgAction_ReqAddMoney kMoneyAction(MCE_SpendMoney,-iNeedMoney,m_pkGnd->GroundKey());
					if ( !kMoneyAction.DoAction(pkCaster,pkNothing) )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}

				pkUser->AddAbil(AT_ACHIEVEMENT_MISSION_REVIVE,1);

				pkUser->Alive(EALIVE_MONEY);

				if( pkUser->HaveExpedition() )	// �����뿡 ���ԵǾ� �ִٸ�, �ڽ��� ���� ��ȭ�� �˷��� ��.
				{
					BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
					Packet.Push(pkUser->ExpeditionGuid());
					Packet.Push(pkUser->GetID());
					Packet.Push(pkUser->IsAlive());
					::SendToGlobalPartyMgr(Packet);
				}

				PgDeathPenalty kAction(m_pkGnd->GroundKey(), LURT_Gold, true);//�� �Ἥ ��Ƴ��°���. �г�Ƽ.
				kAction.DoAction(pkUser, NULL);
			}
			else
			{
				// ��Ȱ �� �� ���� ���� �̴�.
				pkCaster->SendWarnMessage(98);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case ESMT_REVIVE_BY_INSURANCE:
		{
			INFO_LOG( BM::LOG_LV5, _T("[BadPacket] [ESMT_REVIVE_BY_INSURANCE] Player<") << pkCaster->Name() << _T(" / ") << pkCaster->GetID() << _T(">") );
// 			if(pkCaster->IsAlive())
// 			{	// ���� ���¿����� ��� �� �� �ִ�.
// 				pkCaster->SendWarnMessage(243);
// 				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
// 				return false;
// 			}
// 
// 			SItemPos kItemPos;
// 			if(S_OK == pkUser->GetInven()->GetFirstItem(IT_CASH, DEATH_INSURANCE_NO, kItemPos))
// 			{
// 				PgBase_Item kItem;
// 				if( S_OK == pkUser->GetInven()->GetItem(kItemPos, kItem) )
// 				{
// 					PgAction_ReqUseItem kAction(kItemPos, m_pkGnd); 
// 					kAction.DoAction(pkUser, NULL);
// 				}
// 			}
// 			else
// 			{//��������� �����ϴ�.
// 				pkCaster->SendWarnMessage(80031);
// 				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
// 				return false;
// 			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return true;
}


PgCreateSpreadPos::PgCreateSpreadPos(POINT3 const& rkStartPos)
:	m_kStartPos(rkStartPos)
,	m_iLength(0)
{	
}

void PgCreateSpreadPos::AddFrontRange(const NxScene* pkScene, POINT3 const& rkDirPos, float const fRange)
{
	if(fRange)
	{
		NxVec3 const kDirVec(rkDirPos.x,rkDirPos.y,rkDirPos.z);
		NxRay kRay(NxVec3(m_kStartPos.x, m_kStartPos.y, m_kStartPos.z), kDirVec);
		NxRaycastHit kHit;
		NxShape *pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, 0xFFFFFFFF, fRange);
		if(pkHitShape)
		{
			m_kStartPos.x = kHit.worldImpact.x;
			m_kStartPos.y = kHit.worldImpact.y;
		}
		else
		{
			m_kStartPos += rkDirPos * fRange;
		}
	}
}

void PgCreateSpreadPos::AddDir(const NxScene* pkScene,POINT3 const& rkDir, int const iLength, bool bBack)
{
	m_kDir = rkDir;
	m_iLength = iLength;
	if(pkScene)
	{
		NxRay kRay(NxVec3(m_kStartPos.x, m_kStartPos.y, m_kStartPos.z+1.0f), NxVec3(m_kDir.x, m_kDir.y, m_kDir.z));
		NxRaycastHit kHit;
		NxReal fLength = (float)m_iLength;
		NxShape *pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, 0xFFFFFFFF, fLength+30.0f);
		if ( pkHitShape )
		{
			fLength = kHit.distance - AI_Z_LIMIT;
		}

		if ( bBack )
		{
			kRay.orig += (kRay.dir*fLength);
			m_kStartPos.x = kRay.orig.x;
			m_kStartPos.y = kRay.orig.y;
			m_kStartPos.z = kRay.orig.z;
			kRay.dir = -kRay.dir;
			fLength += (float)m_iLength;
			pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, 0xFFFFFFFF, fLength+30.0f);

			if ( pkHitShape )
			{
				fLength = kHit.distance - AI_Z_LIMIT;
			}
			m_kDir = m_kDir * (-1.0f);
		}
		m_iLength = (int)fLength;
	}

	m_iLength = __max(2, m_iLength);
}

void PgCreateSpreadPos::PopPos(POINT3& rkOutPos, int const iMinLength)
{
	m_iLength = __max(2, m_iLength);
	int const iRandScale = BM::Rand_Range(iMinLength, m_iLength);
	rkOutPos = m_kStartPos + (m_kDir * (float)iRandScale); 
//	INFO_LOG(BM::LOG_LV3,_T("[%s] Scale : %d, New Pos : %.2f,  %.2f, %.2f"),__FUNCTIONW__,iRandScale,rkOutPos.x, rkOutPos.y, rkOutPos.z);
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ResetSkill
//////////////////////////////////////////////////////////////////////////////
//PgAction_ResetSkill::PgAction_ResetSkill(SGroundKey const &kGndKey,CONT_PLAYER_MODIFY_ORDER & rkOrder)
//:	m_kGndKey(kGndKey),m_rkOrder(rkOrder)
//{
//}
//
//bool PgAction_ResetSkill::DoAction(CUnit* pkCaster, CUnit* pkTarget)
//{	//���� * ��ų ����Ʈ
//	//�ʱ� ���� ��ų�� NeedSkillPoint�� 0 �̾����.
//	
//	//PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
//	//PgMySkill* pkMySkill = pkPlayer->GetMySkill();
//
//	//int const iClass = pkPlayer->GetAbil(AT_CLASS);
//	//int const iNowLv = pkPlayer->GetAbil(AT_LEVEL);
//	//const SClassKey kKey(iClass, iNowLv);//
//	
//	//int iRetSkillPoint = 0;
//	//GET_DEF(PgClassDefMgr, kClassDefMgr);
//	//if(S_OK == kClassDefMgr.AccSkillPoint(kKey, iRetSkillPoint))
//	{
//		CONT_PLAYER_MODIFY_ORDER kOrder;
//
//		//SPMOD_AddAbil kSetSPData(AT_SP, iRetSkillPoint);
//		//kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), kSetSPData));
//		
//		SPMOD_AddSkill kAddSkillData(0, true);//����. 
//		kOrder.push_back(SPMO(IMET_ADD_SKILL, pkCaster->GetID(), kAddSkillData));
//
//		PgAction_ReqModifyItem kItemModifyAction(IMEPT_NONE, m_kGndKey, kOrder);
//		return kItemModifyAction.DoAction(pkCaster, pkTarget);
//	}
//	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
//	return false;
//}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_SyncSkillState
//////////////////////////////////////////////////////////////////////////////
PgAction_SyncSkillState::PgAction_SyncSkillState()
{
}

bool PgAction_SyncSkillState::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{	//���� * ��ų ����Ʈ
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if(pkPlayer)
	{
		PgMySkill* pkMySkill = pkPlayer->GetMySkill();

		{//to target
			BM::Stream kPacket(PT_M_C_SKILL_STATE_CHANGE, pkPlayer->GetID());
			pkMySkill->WriteToPacket(WT_DEFAULT, kPacket);
			pkPlayer->Send(kPacket, E_SENDTYPE_SELF);
		}

		{// -> Broadcast
			BM::Stream kPacket(PT_M_C_SKILL_STATE_CHANGE, pkPlayer->GetID());
			pkMySkill->WriteToPacket(WT_SIMPLE, kPacket);
			pkPlayer->Send(kPacket, E_SENDTYPE_BROADCAST);
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqDivideItem
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqDivideItem::PgAction_ReqDivideItem(SGroundKey const& rkGndKey, SItemPos const &kItemPos, int const iItemNo, BM::GUID const &kItemGuid, int const iCount)
	:	m_kGndKey(rkGndKey),
		m_kItemPos(kItemPos),
		m_iItemNo(iItemNo),
		m_kItemGuid(kItemGuid),
		m_iDivCount(iCount)
{
}

bool PgAction_ReqDivideItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	PgInventory *pkInv = pkCaster->GetInven();

	PgBase_Item kItem;
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(m_iItemNo);

	if(	(S_OK != pkInv->GetItem(m_kItemPos, kItem))
	||	!pItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	SItemPos kEmptyItemPos;

	if(!pkInv->GetFirstEmptyPos((EInvType)pItemDef->PrimaryInvType(), kEmptyItemPos))
	{
		pkCaster->SendWarnMessage(799559);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(	!pItemDef->IsAmountItem()
	||	kItem.Guid() != m_kItemGuid
	||	kItem.ItemNo() != m_iItemNo
	||	kItem.Count() <= m_iDivCount
	|| kItem.EnchantInfo().IsBinding()
	|| (0 >= m_iDivCount))
	{//Can't Div
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
{	
	SPMOD_Modify_Count kAddItem(kItem, m_kItemPos, -m_iDivCount);
	SPMO kIMO(IMET_MODIFY_COUNT, pkCaster->GetID(), kAddItem);
	kOrder.push_back(kIMO);
}
{
	kItem.Guid(BM::GUID::Create());//New Guid
	kItem.Count(m_iDivCount);

	tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, kEmptyItemPos, false);
	SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddItem);
	kOrder.push_back(kIMO);
}

	PgAction_ReqModifyItem kItemModifyAction(CIE_Divide, m_kGndKey, kOrder);
	kItemModifyAction.DoAction(pkCaster, pkTarget);
	return true;
}





//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqSMS
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqSMS::PgAction_ReqSMS( PgGround* const pkGround, SItemPos const &kItemPos, int const iItemNo, BM::GUID const &kItemGuid, BM::Stream const & kPacket)
	:	m_kItemPos(kItemPos),
		m_iItemNo(iItemNo),
		m_kItemGuid(kItemGuid),
		m_kPacket(kPacket),
		m_pkGround(pkGround)
{
}

bool PgAction_ReqSMS::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if (m_pkGround == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	PgInventory *pkInv = pkCaster->GetInven();

	PgBase_Item kItem;
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(m_iItemNo);

	if(	(S_OK != pkInv->GetItem(m_kItemPos, kItem))
	||	!pItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(	kItem.Guid() != m_kItemGuid
	||	kItem.ItemNo() != m_iItemNo
	||	!kItem.Count()
	||	!pItemDef->IsAmountItem())
	{//Can't Div
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket;
	//kPacket.Push(m_kPacket);

	std::wstring strMemo;
	std::wstring TargetName;
	std::wstring MailTitle;
	XUI::PgExtraDataPackInfo kExtraDataPackInfo;

	m_kPacket.Pop(strMemo);
	m_kPacket.Pop(TargetName);
	m_kPacket.Pop(MailTitle);	
	kExtraDataPackInfo.PopFromPacket(m_kPacket);

	kPacket.Push(strMemo);
	kPacket.Push(TargetName);
	kPacket.Push(MailTitle);
	kExtraDataPackInfo.PushToPacket(kPacket);

	PgAction_ReqUseItem kAction(m_kItemPos, m_pkGround, false, BM::Stream(), kPacket);
	return kAction.DoAction(pkCaster, pkTarget);
}







//////////////////////////////////////////////////////////////////////////////
//		PgAction_EventSystem
//////////////////////////////////////////////////////////////////////////////
PgAction_EventSystem::PgAction_EventSystem()
{
}

void PgAction_EventSystem::DoAction()
{
	static PgAction_EventStart kStart;
	static PgAction_EventEnd kEnd;

	g_kEventView.CallbackStart(&kStart);
	g_kEventView.CallbackEnd(&kEnd);
	g_kEventView.ProcessEvent();
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_FollowingMode
//////////////////////////////////////////////////////////////////////////////
PgAction_FollowingMode::PgAction_FollowingMode(EPlayer_Follow_Mode eMode, bool bFriend, PgGround* pkGround)
	: m_eMode(eMode), m_bFriend(bFriend), m_pkGround(pkGround)
{
	m_eResult = eMode;
}

bool PgAction_FollowingMode::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	PgPlayer* pkCasterPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	PgPlayer* pkTargetPlayer = dynamic_cast<PgPlayer*>(pkTarget);
	if (pkCasterPlayer == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iMoveSpeed = 0;

	switch(m_eMode)
	{
	case EFollow_Request:
		{
			if (pkTargetPlayer == NULL)
			{
				m_eResult = EFollow_Failed;
				goto __SEND_FOLLOW_RESULT;
			}
			// 1. Following ���� Ȯ���ϰ�
			if (pkCasterPlayer->PartyGuid() != pkTargetPlayer->PartyGuid())
			{
				if (pkCasterPlayer->GuildGuid() != pkTargetPlayer->GuildGuid())
				{
					if( pkCasterPlayer->GetCoupleGuid() != pkTargetPlayer->GetID() )
					{
						if( false == m_bFriend  )
						{
							// TODO : ģ���� ��ϵǾ� �ִ°� �˻��ؾ� �Ѵ�.
							m_eResult = EFollow_Req_Refused;
							goto __SEND_FOLLOW_RESULT;
						}
					}
				}
			}
			// 2. Head ���� �����ϰ�
			pkCasterPlayer->FollowingHeadGuid(pkTarget->GetID());
			// 3. �������� Head Player�� ã��
			BM::GUID const & rkHeadPlayer = m_pkGround->GetFollowingHead(pkCaster, true);
			if (rkHeadPlayer == BM::GUID::NullData())
			{
				m_eResult = EFollow_Failed;
				goto __SEND_FOLLOW_RESULT;
			}
			// 4. MoveSpeed �����ϰ� �����.
			iMoveSpeed = m_pkGround->GetUnitAbil(rkHeadPlayer, AT_C_MOVESPEED);
			//pkCaster->SetAbil(AT_C_MOVESPEED, iMoveSpeed, true, true);
		}break;
	case EFollow_Cancel:
		{
			BM::GUID const & rkHead = pkCasterPlayer->FollowingHeadGuid();
			//if (pkTargetPlayer == NULL || pkTargetPlayer->GetID() == rkHead)
			{
				// pkTargetPlayer �� ���� ���� �ʴ��� �����ؾ� �Ѵ�.
				pkCasterPlayer->FollowingHeadGuid(BM::GUID::NullData());
				pkCasterPlayer->NftChangedAbil(AT_C_MOVESPEED, E_SENDTYPE_BROADALL);
			}
		}break;
	default:
		{
			m_eResult = EFollow_Failed;
			goto __SEND_FOLLOW_RESULT;
		}break;
	}

__SEND_FOLLOW_RESULT:
	BM::Stream kFPacket(PT_M_C_RES_FOLLOWING, m_eResult);
	kFPacket.Push((pkTargetPlayer != NULL) ? pkTargetPlayer->GetID() : BM::GUID::NullData());
	kFPacket.Push((pkCasterPlayer != NULL) ? pkCasterPlayer->GetID() : BM::GUID::NullData());
	pkCasterPlayer->Send(kFPacket);
	if (m_eResult != EFollow_Failed && pkTargetPlayer != NULL)
	{
		pkTargetPlayer->Send(kFPacket);
	}

	switch( m_eResult )
	{
	case EFollow_Request:
		{
			// 4. MoveSpeed �����ϰ� �����.
			if( m_pkGround )
			{
				if( pkCaster )
				{
					pkCaster->SetAbil(AT_C_MOVESPEED, iMoveSpeed, true, true);
				}
			}
		}break;
	default:
		{
		}break;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqGuildLevelUp
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqGuildLevelUp::PgAction_ReqGuildLevelUp(SGroundKey const &rkGndKey, unsigned short const sGuildLv)
	:m_kGndKey(rkGndKey), m_sGuildLv(sGuildLv), m_kResult(GCR_Failed)
{
}

bool PgAction_ReqGuildLevelUp::DoAction(CUnit *pkCaster, CUnit *Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Somthing wrong");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory *pkInven = pkPC->GetInven();
	if( !pkInven )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	const CONT_DEF_GUILD_LEVEL *pkDefGuildLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildLevel);
	if( !pkDefGuildLevel )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_LEVEL::const_iterator def_iter = pkDefGuildLevel->find(m_sGuildLv);//���� ����
	if( pkDefGuildLevel->end() == def_iter )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_LEVEL::const_iterator nextdef_iter = pkDefGuildLevel->find(m_sGuildLv+1);//���� ����
	if( pkDefGuildLevel->end() == nextdef_iter )
	{
		m_kResult = GCR_Max;//���簡 �ִ� ����
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_LEVEL::mapped_type const &rkNextDefGuildLevel = (*nextdef_iter).second;
	if( rkNextDefGuildLevel.iGold
	&&	pkInven->Money() < rkNextDefGuildLevel.iGold )
	{
		//�� ����
		m_kResult = GCR_Money;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	SPMOD_Add_Money kDelMoneyData(-rkNextDefGuildLevel.iGold);//�ʿ�Ӵ� ����.
	SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
	kOrder.push_back(kIMO);

	size_t iCur = 0;
	while(MAX_GUILD_LEVEL_ITEM_COUNT > iCur)
	{
		if( rkNextDefGuildLevel.iItemNo[iCur]
		&&	rkNextDefGuildLevel.iCount[iCur] )
		{
			size_t const iCount = pkInven->GetTotalCount(rkNextDefGuildLevel.iItemNo[iCur]);
			if( (size_t)rkNextDefGuildLevel.iCount[iCur] > iCount )
			{
				//������ ����
				m_kResult = GCR_Money;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			tagPlayerModifyOrderData_Add_Any kDelData(rkNextDefGuildLevel.iItemNo[iCur], -rkNextDefGuildLevel.iCount[iCur]);

			SPMO kIMO(IMET_ADD_ANY, pkPC->GetID(), kDelData);
			kOrder.push_back(kIMO);
		}

		++iCur;
	}

	//������ ��� �Ҹ� ��û
	BM::Stream kAddonPacket(PT_M_I_REQ_RESUME_GUILD_COMMAND, (BYTE)GC_M_LvUp);
	kAddonPacket.Push(false);//�׽�Ʈ �ƴ�
	PgAction_ReqModifyItem kDeleteAction(CIE_GuildLevelUp, m_kGndKey, kOrder, kAddonPacket);
	if( !kDeleteAction.DoAction(pkCaster, NULL) )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//���� ��� ���� �˻� ���
	m_kResult = GCR_Success;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqGuildInventoryCreate
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqGuildInventoryCreate::PgAction_ReqGuildInventoryCreate(SGroundKey const &rkGndKey, unsigned short const sGuildLv)
	:m_kGndKey(rkGndKey), m_sGuildLv(sGuildLv), m_kResult(GCR_Failed)
{
}

bool PgAction_ReqGuildInventoryCreate::DoAction(CUnit *pkCaster, CUnit *Nothing)
{	
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Somthing wrong");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory *pkInven = pkPC->GetInven();
	if( !pkInven )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pkInven->Money() < iCreateMoney )
	{//�� ����
		m_kResult = GCR_Money;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( m_sGuildLv < iCreateLevel )
	{// ������ �ȵȴ�.
		m_kResult = GCR_Level;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	SPMOD_Add_Money kDelMoneyData(-iCreateMoney);//�ʿ�Ӵ� ����.
	SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
	kOrder.push_back(kIMO);

	// ��� �Ҹ� ��û
	BM::Stream kAddonPacket(PT_M_I_REQ_RESUME_GUILD_COMMAND, (BYTE)GC_M_InventoryCreate);
	kAddonPacket.Push(false);//�׽�Ʈ �ƴ�
	PgAction_ReqModifyItem kDeleteAction(CIE_GuildInventoryCreate, m_kGndKey, kOrder, kAddonPacket);
	if( !kDeleteAction.DoAction(pkCaster, NULL) )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//���� ��� ���� �˻� ���
	m_kResult = GCR_Success;
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqGuildLearnSkill
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqGuildLearnSkill::PgAction_ReqGuildLearnSkill(SGroundKey const &rkGndKey, unsigned short const sGuildLv, int const iSkillNo)
	:m_sGuildLv(sGuildLv), m_kGndKey(rkGndKey), m_iSkillNo(iSkillNo)
{
}

PgAction_ReqGuildLearnSkill::~PgAction_ReqGuildLearnSkill()
{
}

bool PgAction_ReqGuildLearnSkill::DoAction(CUnit *pkCaster, CUnit *Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Somthing wrong");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkCaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory *pkInven = pkPC->GetInven();
	if( !pkInven )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_SKILL const *pkDefGuildSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildSkill);
	if( !pkDefGuildSkill )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_SKILL::const_iterator def_iter = pkDefGuildSkill->find(m_iSkillNo);
	if( pkDefGuildSkill->end() == def_iter )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_SKILL::mapped_type const &rkGuildSkillDef = (*def_iter).second;

	if( rkGuildSkillDef.sGuildLv > m_sGuildLv )
	{
		m_kResult = GCR_Level;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( rkGuildSkillDef.iGold
	&&	pkInven->Money() < rkGuildSkillDef.iGold )
	{
		//�� ����
		m_kResult = GCR_Money;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	
	CONT_PLAYER_MODIFY_ORDER kOrder;
	SPMOD_Add_Money kDelMoneyData(-rkGuildSkillDef.iGold);//�ʿ�Ӵ� ����.
	SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelMoneyData);
	kOrder.push_back(kIMO);

	size_t iCur = 0;
	while(MAX_GUILD_SKILL_ITEM_COUNT > iCur)
	{
		if( rkGuildSkillDef.iItemNo[iCur]
		&&	rkGuildSkillDef.iCount[iCur] )
		{
			size_t const iCount = pkInven->GetTotalCount(rkGuildSkillDef.iItemNo[iCur]);
			if( (size_t)rkGuildSkillDef.iCount[iCur] > iCount )
			{
				//������ ����
				m_kResult = GCR_Money;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			tagPlayerModifyOrderData_Add_Any kDelData(rkGuildSkillDef.iItemNo[iCur], -rkGuildSkillDef.iCount[iCur]);

			SPMO kIMO(IMET_ADD_ANY, pkPC->GetID(), kDelData);
			kOrder.push_back(kIMO);
		}

		++iCur;
	}

	//��� ��ų�� SP ����
	//SPMOD_AddAbil kAddAbilData(AT_SP, -iNeedSP);//SP ����
	//kOrder.push_back(SPMO(IMET_ADD_ABIL, pkPlayer->GetID(), kAddAbilData));

	SPMOD_AddSkill kAddSkillData(m_iSkillNo);//�� SP�� ���� ����.
	kOrder.push_back(SPMO(IMET_ADD_SKILL, pkCaster->GetID(), kAddSkillData));

	//������ ��� �Ҹ� ��û
	BM::Stream kAddonPacket(PT_M_I_REQ_RESUME_GUILD_COMMAND, (BYTE)GC_M_AddSkill);
	kAddonPacket.Push(m_iSkillNo);
	kAddonPacket.Push(false);//�׽�Ʈ �ƴ�
	PgAction_ReqModifyItem kDeleteAction(CIE_GuildLearnSkill, m_kGndKey, kOrder, kAddonPacket);
	if( !kDeleteAction.DoAction(pkCaster, NULL) )
	{
		m_kResult = GCR_Failed;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//���� ��� ���� �˻� ���
	m_kResult = GCR_Success;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqTrapDamage
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqTrapDamage::PgAction_ReqTrapDamage(SGroundKey const& rkGndKey, PgGround const* pkGround, int iTrapSkillNo)
	:m_kGndKey(rkGndKey), m_pkGround(pkGround), m_iTrapSkillNo(iTrapSkillNo)
{
}

PgAction_ReqTrapDamage::~PgAction_ReqTrapDamage()
{
}

bool PgAction_ReqTrapDamage::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(m_iTrapSkillNo > 0)
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(m_iTrapSkillNo);
		if(pkSkillDef)
		{
			int iTrapEffectNo = pkSkillDef->GetEffectNo();
			if(iTrapEffectNo > 0)
			{
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, m_pkGround);
				pkTarget->AddEffect(iTrapEffectNo, 0, &kArg, pkTarget);
			}

		}
	}
	if(NULL == m_pkGround) { return false; }
	// pkCaster : ����� NULL
	// pkTarget : Trap �� ���� Unit
	float const fDamageRate = 0.08f;	// Damage ũ�� (���� HP���� �ش� Rate �� ��ŭ)
	int const iCurrentHP = pkTarget->GetAbil(AT_HP);
	int const iDamage = (m_pkGround->GetAttr() == GATTR_RACE_GROUND) ? 0 : __max(10, int(iCurrentHP * fDamageRate)); //�޸��� �̺�Ʈ���� Ʈ���� �ɷ��� ����� ���� ����
	int const iRemainHP = __max(0, iCurrentHP-iDamage);
	int const iDelta = iRemainHP - iCurrentHP;

	BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
	kPacket.Push(pkTarget->GetID());
	kPacket.Push((short)AT_HP);
	kPacket.Push(iRemainHP);
	kPacket.Push(BM::GUID::NullData());
	kPacket.Push((int)0);
	kPacket.Push(iDelta);
	pkTarget->Send(kPacket, E_SENDTYPE_BROADALL);

	if ( 0 >= iRemainHP )
	{
		// ������ �ڻ��̴ϱ� Target�� Clear
		// SetAbil���� VOnDie�� ȣ���ϴϱ� �̸� Clear�س��ƾ� �Ѵ�.
		pkTarget->SetTarget( BM::GUID::NullData() );
	}

	pkTarget->SetAbil(AT_HP, iRemainHP);

	if( 0 >= pkTarget->GetAbil(AT_HP) )
	{
		PgPlayerLogUtil::DeathLog(dynamic_cast< PgPlayer* >(pkTarget), LUDT_Trap, m_kGndKey, std::wstring(), BM::GUID::NullData());
	}
	//INFO_LOG(BM::LOG_LV9, _T("[%s] HP [%d -> %d]"), __FUNCTIONW__, iCurrentHP, iRemainHP);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_ReqCollisionDamage
//////////////////////////////////////////////////////////////////////////////
PgAction_ReqCollisionDamage::PgAction_ReqCollisionDamage(SGroundKey const& rkGndKey, PgGround const* pkGround)
	:m_kGndKey(rkGndKey)
	,m_pkGround(pkGround)
{
}

PgAction_ReqCollisionDamage::~PgAction_ReqCollisionDamage()
{
}
extern void OnAttacked(CUnit* pkUnit, UNIT_PTR_ARRAY* pkUnitArray, int const iSkillNo, PgActionResultVector* pkResult, SActArg* pArg, DWORD const dwTimeStamp);
bool PgAction_ReqCollisionDamage::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(!pkCaster || !pkTarget)
	{
		return false;
	}

	if ( !pkCaster->IsTarget( pkTarget ) )
	{
		return false;
	}
	
	{// ������ ���� Ȯ��
		int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
		bool const bBlock = iRandValue < pkTarget->GetAbil(AT_100PERECNT_BLOCK_RATE); // ���� Block Rate ���
		if(bBlock)
		{
			SActArg kArg;
			PgGroundUtil::SetActArgGround(kArg, m_pkGround);
			pkTarget->AddEffect(BLOCK_RATE_EFFECT_NO, 0, &kArg, pkTarget);
			return false;
		}
	}

	// pkCaster : �浹�������� �� ��
	// pkTarget : �浹�������� ���� Unit
	
	int iSkillNo = pkCaster->GetAbil(AT_COLLISION_SKILL);	//��ġ ������ ���� �� ��� �� ��ų
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	if(0==iSkillNo)	//������ �⺻
	{
		iSkillNo = kSkillDefMgr.GetSkillNoFromActionName(_T("a_mon_collision_01"));
	}

	int iEffectNo = 0;
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if(pkSkill)
	{
		iEffectNo = pkSkill->GetEffectNo();
	}

	BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
	kPacket.Push(pkTarget->GetID());
	kPacket.Push((short)AT_HP);

	if((pkSkill != NULL) && (0!=pkSkill->GetAbil(AT_COLLISION_SKILL_FIRE)))
	{
		UNIT_PTR_ARRAY kUnitArray;
		kUnitArray.Add(pkTarget);
		SActArg kActArg;
		PgGroundUtil::SetActArgGround(kActArg, m_pkGround);
		PgActionResultVector kResultVec;

		int const iOriginHP = pkTarget->GetAbil(AT_HP);
		g_kSkillAbilHandleMgr.SkillFire(pkCaster, iSkillNo, &kActArg, &kUnitArray, &kResultVec);
		OnAttacked(pkCaster, &kUnitArray, iSkillNo, &kResultVec, &kActArg, g_kEventView.GetServerElapsedTime());
		
		int const iNowHP = pkTarget->GetAbil(AT_HP);
		kPacket.Push(iNowHP);
		kPacket.Push(pkCaster->GetID());
		kPacket.Push(iEffectNo);
		kPacket.Push(iNowHP - iOriginHP);
	}
	else
	{
		PgActionResult kResult;
		kResult.Init();

		// ��ġ ������ ���� ȸ�ǰ� ���� �ʾƾ� �Ѵ�.
		// �ӽ÷� ���߷��� �������� �÷� ȸ�ǰ� �����ʵ��� ó��
		int const iHitRate = pkCaster->GetAbil(AT_FINAL_HIT_SUCCESS_RATE);
		pkCaster->SetAbil(AT_FINAL_HIT_SUCCESS_RATE, 999999999, false, false);

		bool const bRet = CS_GetDmgResult(iSkillNo, pkCaster, pkTarget, ABILITY_RATE_VALUE, 0, &kResult) || kResult.GetMissed();
		// ���� �Ŀ��� ���� ������ ����
		pkCaster->SetAbil(AT_FINAL_HIT_SUCCESS_RATE, iHitRate, false, false);
		if(!bRet)
		{
			return false;
		}

		int const iCurrentHP = pkTarget->GetAbil(AT_HP);
		int const iDamage = __max(10, kResult.GetValue());
		int const iRemainHP = __max(0, iCurrentHP-iDamage);
		int const iDelta = iRemainHP - iCurrentHP;


		kPacket.Push(iRemainHP);
		kPacket.Push(pkCaster->GetID());
		kPacket.Push(iEffectNo);
		kPacket.Push(iDelta);

		pkTarget->SetAbil(AT_HP, iRemainHP);
	}

	pkTarget->Send(kPacket, E_SENDTYPE_BROADALL);

	if( 0 >= pkTarget->GetAbil(AT_HP) )
	{
		PgPlayerLogUtil::DeathLog(dynamic_cast< PgPlayer* >(pkTarget), LUDT_Trap, m_kGndKey, std::wstring(), BM::GUID::NullData());
	}
	//INFO_LOG(BM::LOG_LV9, _T("[%s] HP [%d -> %d]"), __FUNCTIONW__, iCurrentHP, iRemainHP);
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_GroundEffect
//////////////////////////////////////////////////////////////////////////////
PgAction_GroundEffect::PgAction_GroundEffect(PgGround const *pkGround, EEffectType const eEffectType)
: m_pkGround(pkGround), m_eEffectType(eEffectType)
{
}

PgAction_GroundEffect::~PgAction_GroundEffect()
{
}

bool PgAction_GroundEffect::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	// Ư�� Ground �� �����ϸ� ����� ����(=Effect) ȿ�� �ֱ�
	if (m_pkGround == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( m_pkGround->IsGroundEffect() )
	{
		int iIndex = 0;
		GET_DEF(PgGroundEffectMgr, kGEffectMgr);

		while (1)
		{
			PgGroundEffect const *pkGEffect = kGEffectMgr.GetGroundEffect(iIndex, m_pkGround->GetGroundNo());
			if (pkGEffect == NULL)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGEffect is NULL"));
				break;
			}

			if ( CheckGroundEffect(pkGEffect, pkCaster) )
			{
				SEffectCreateInfo kCreate;
				kCreate.eType = m_eEffectType;
				kCreate.iEffectNum = pkGEffect->EffectNo();
				kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				PgGroundUtil::SetActArgGround(kCreate.kActArg, m_pkGround);
				pkCaster->AddEffect(kCreate);
			}

			++iIndex;
		}
		
	}
	return true;	
}

bool PgAction_GroundEffect::CheckGroundEffect(PgGroundEffect const *pkGEffect, CUnit* pkCaster)
{
	if(!pkGEffect)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	EUnitType eTargetType = static_cast<EUnitType>(pkGEffect->GetAbil(AT_TARGET_TYPE));
	if(UT_NONETYPE == eTargetType)
	{
		eTargetType = UT_PLAYER;
	}
	if(!pkCaster->IsInUnitType(eTargetType))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// Gender Limit
	int const iGenderLimit = pkGEffect->GetAbil(AT_GENDERLIMIT);
	if(iGenderLimit != 0 && 0 == (iGenderLimit & pkCaster->GetAbil(AT_GENDER)))//���� �˻�.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// Level Limit
	int const iLvLimit = pkGEffect->GetAbil(AT_LEVELLIMIT);
	int const iLevel = pkCaster->GetAbil(AT_LEVEL);
	if(iLvLimit > iLevel)//�����˻�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iLvMaxLimit = pkGEffect->GetAbil(AT_MAX_LEVELLIMIT);
	if (iLvMaxLimit != 0 &&  iLevel > iLvMaxLimit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// Class Limit
	__int64 const i64ClassLimit = pkGEffect->GetAbil64(AT_CLASSLIMIT);
	if (i64ClassLimit != 0 && !IS_CLASS_LIMIT(i64ClassLimit, pkCaster->GetAbil(AT_CLASS)))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_GroundDeleteEffect
//////////////////////////////////////////////////////////////////////////////
PgAction_GroundDeleteEffect::PgAction_GroundDeleteEffect(PgGround const *pkGround)
: m_pkGround(pkGround)
{
}

PgAction_GroundDeleteEffect::~PgAction_GroundDeleteEffect()
{
}

bool PgAction_GroundDeleteEffect::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	// Ư�� Ground �� �����ϸ� ����� ����(=Effect) ȿ�� �ֱ�
	if (m_pkGround == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( m_pkGround->IsGroundEffect() )
	{
		int iIndex = 0;
		GET_DEF(PgGroundEffectMgr, kGEffectMgr);

		while (1)
		{
			PgGroundEffect const *pkGEffect = kGEffectMgr.GetGroundEffect(iIndex, m_pkGround->GetGroundNo());
			if (pkGEffect == NULL)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGEffect is NULL"));
				break;
			}

			SActArg kActArg;
			PgGroundUtil::SetActArgGround(kActArg, m_pkGround);
			pkCaster->DeleteEffect(pkGEffect->EffectNo());

			++iIndex;
		}
	}
	return true;	
}

PgAction_ReqAddCp::PgAction_ReqAddCp(EItemModifyParentEventType const kCause, int const iAddCp, SGroundKey const &kGroundKey, BM::Stream const& rkPacket)
	:	m_kCause(kCause), m_iAddCp(iAddCp), m_kGndKey(kGroundKey), m_kPacket(rkPacket)
{
}

bool PgAction_ReqAddCp::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(pkCaster && 0 != m_iAddCp)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;
		SPMOD_Add_CP kModifyCP((int)m_iAddCp);//cp ����
		SPMO kIMO(IMET_ADD_CP, pkCaster->GetID(), kModifyCP);
		kOrder.push_back(kIMO);

		PgAction_ReqModifyItem kItemModifyAction(m_kCause, m_kGndKey, kOrder, m_kPacket);
		return kItemModifyAction.DoAction(pkCaster, pkTarget);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAction_Sys2Inv::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	CONT_SYS2INV_INFO kSys2InvInfo;
	m_kPacket.Pop(kSys2InvInfo);

	CONT_PLAYER_MODIFY_ORDER kModifyOrder;

	CONT_SYS2INV_INFO::const_iterator iter = kSys2InvInfo.begin();
	while(iter != kSys2InvInfo.end())
	{
		SPMO kOrder(IMET_MODIFY_SYS2INV,pkCaster->GetID(), SPMOD_Sys2Inv((*iter).kItemGuid,(*iter).kItemPos,(*iter).bAnyPos));
		kModifyOrder.push_back(kOrder);
		++iter;
	}

	BM::Stream kAddon(PT_M_C_ANS_SYSTEM_INVENTORY_RECV);
	kAddon.Push(kSys2InvInfo);

	PgAction_ReqModifyItem kItemModifyAction(CIE_Sys2Inv, m_kGndKey, kModifyOrder,kAddon);
	return kItemModifyAction.DoAction(pkCaster, pkTarget);
}

bool PgAction_SysItemRemove::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	CONT_ITEMGUID kContItemGuid;
	m_kPacket.Pop(kContItemGuid);

	CONT_PLAYER_MODIFY_ORDER kModifyOrder;

	CONT_ITEMGUID::const_iterator iter = kContItemGuid.begin();
	while(iter != kContItemGuid.end())
	{
		SPMO kOrder(IMET_DELETE_SYSITEM,pkCaster->GetID(), SPMOD_SysItemRemove((*iter)));
		kModifyOrder.push_back(kOrder);
		++iter;
	}

	BM::Stream kAddon(PT_M_C_ANS_SYSTEM_INVENTORY_REMOVE);
	kAddon.Push(kContItemGuid);

	PgAction_ReqModifyItem kItemModifyAction(CIE_Delete_SysItem, m_kGndKey, kModifyOrder,kAddon);
	return kItemModifyAction.DoAction(pkCaster, pkTarget);
}

// PgAction_LearnSkill
PgAction_LearnSkill::PgAction_LearnSkill( int const iSkillNo, SGroundKey const &kGroundKey, BYTE const cCmdType, BM::Stream const& rkPacket )
:	m_iSkillNo(iSkillNo), m_kGndKey(kGroundKey), m_cCmdType(cCmdType), m_kPacket(rkPacket)
{}

int PgAction_LearnSkill::DoAction( CUnit* pkCaster )const
{
	// Contents Server�� �ű�(üũ �κ�)

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if ( !pkPlayer || !pkPlayer->IsUnitType(UT_PLAYER) )
	{
		if (pkPlayer)
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << L"Error UnitType[" << pkCaster->UnitType() << L"] [" << pkCaster->Name() << L"-" << pkCaster->GetID() << L"]");
		}
		else
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << L"Error UnitType[NULL]");
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_ERROR_UNITTYPE"));
		return LS_RET_ERROR_UNITTYPE;
	}

	PgMySkill *pkMySkill = pkPlayer->GetMySkill();
	if ( !pkMySkill )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"PlayerSkill is NULL [" << pkCaster->Name() << L"-" << pkCaster->GetID() << L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_ERROR_SYSTEM"));
		return LS_RET_ERROR_SYSTEM;
	}

	// Do Real SkillLearn
	int const iNeedSP = pkMySkill->GetNeedSP(m_iSkillNo);

	CONT_PLAYER_MODIFY_ORDER kOrder;
	if ( iNeedSP )
	{	// �ʿ��� SP�� ���ٸ� ������ ������ �ʰ� ����...
		// DB���� ����, ����, ����!!!
		SPMOD_AddAbil kAddAbilData(AT_SP, -iNeedSP);//SP ����
		kOrder.push_back(SPMO(IMET_ADD_ABIL, pkPlayer->GetID(), kAddAbilData));
	}
	
	SPMOD_AddSkill kAddSkillData(m_iSkillNo);//�� SP�� ���� ����.
	kOrder.push_back(SPMO(IMET_ADD_SKILL, pkCaster->GetID(), kAddSkillData));

	BM::Stream kPacket;
	kPacket.Push((BYTE)m_cCmdType);
	kPacket.Push(m_kPacket);

	PgAction_ReqModifyItem kItemModifyAction(IMEPT_LEARNSKILL, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pkCaster, NULL);

	return LS_RET_SUCCEEDED;
}

HRESULT PgAction_ReqUserMapMove::Process(CUnit* pkCaster)
{
	int iGroundNo = 0;
	SItemPos kItemPos;
	m_kPacket.Pop(iGroundNo);
	m_kPacket.Pop(kItemPos);

	if( pkCaster->IsDead() )
	{
		return E_CANNOT_STATE_MAPMOVE;
	}

	const CONT_DEFMAP* pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	if(!pkContDefMap)
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find ContDefMap");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MOVE_MAP"));
		return E_CANNOT_MOVE_MAP;
	}

	CONT_DEFMAP::const_iterator itor = pkContDefMap->find(iGroundNo);
	if (itor == pkContDefMap->end())
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find DefMap MapNo[" << iGroundNo << "]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MOVE_MAP"));
		return E_CANNOT_MOVE_MAP;
	}

	const TBL_DEF_MAP& rkDefMap = itor->second;

	if( 0 != (rkDefMap.iAttr & GATTR_FLAG_CANT_WARP) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MOVE_MAP"));
		return E_CANNOT_MOVE_MAP;
	}

	PgBase_Item kItem;
	if(S_OK != pkCaster->GetInven()->GetItem(kItemPos,kItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
		return E_INVALID_ITEMPOS;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	if(UICT_USERMAPMOVE != pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_OPENED_MAP"));
		return E_NOT_OPENED_MAP;
	}

	if(S_OK != pkPlayer->IsOpenWorldMap(iGroundNo))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_OPENED_MAP"));
		return E_NOT_OPENED_MAP;
	}

	if( true == pkPlayer->IsItemEffect(SAFE_FOAM_EFFECTNO) )
	{		
		return E_CANNOT_STATE_MAPMOVE;
	}

	SReqMapMove_MT kRMM(MMET_None);
	kRMM.kTargetKey.GroundNo(iGroundNo);
	kRMM.nTargetPortal = 1;

	PgReqMapMove kMapMove(m_pkGround, kRMM, NULL );
	if(kMapMove.Add( pkPlayer ))
	{
		kMapMove.AddModifyOrder(SPMO(IMET_MODIFY_COUNT,pkCaster->GetID(), SPMOD_Modify_Count(kItem, kItemPos, -1)));
		if(!kMapMove.DoAction())
		{
			return E_CANNOT_MOVE_MAP;
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_USER_MAP_MOVE);
	kPacket.Push(S_OK);

	pkCaster->Send(kPacket);

	return S_OK;
}

bool PgAction_ReqUserMapMove::DoAction(CUnit* pkCaster,CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);

	if(S_OK == hRet)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_USER_MAP_MOVE);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAction_ReqMoveToPartyMember::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);

	if(S_OK == hRet)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MOVETOPARTYMEMBER);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_ReqMoveToPartyMember::Process(CUnit* pkCaster)
{
	BM::GUID kMemberGuid;
	SItemPos kItemPos;
	m_kPacket.Pop(kMemberGuid);
	m_kPacket.Pop(kItemPos);

	if( pkCaster->IsDead() )
	{
		return E_CANNOT_STATE_MAPMOVE;
	}

	PgBase_Item kItem;
	if(S_OK != pkCaster->GetInven()->GetItem(kItemPos,kItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
		return E_INVALID_ITEMPOS;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	if(UICT_MOVETOPARTYMEMBER != pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	if(BM::GUID::NullData() == pkPlayer->PartyGuid())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	if( true == pkPlayer->IsItemEffect(SAFE_FOAM_EFFECTNO) )
	{		
		return E_CANNOT_STATE_MAPMOVE;
	}

	VEC_GUID kVec;
	//bool bFindParty = g_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(),kVec);
	bool bFindParty = m_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(),kVec);	
	if(!bFindParty || (0 == kVec.size()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	bool bFindMember = false;

	for(VEC_GUID::iterator iter = kVec.begin();iter != kVec.end();++iter)
	{
		if((*iter) == kMemberGuid)
		{
			bFindMember = true;
			break;
		}
	}

	if(!bFindMember)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	BM::Stream kPacket(PT_M_T_REQ_PARTYMEMBERPOS);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(kMemberGuid);
	kPacket.Push(m_kGndKey);
	kPacket.Push(kItemPos);

	SendToCenter(kPacket);

	return S_OK;
}

bool PgAction_AnsMoveToPartyMember::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);

	if(S_OK == hRet)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MOVETOPARTYMEMBER);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_AnsMoveToPartyMember::Process(CUnit* pkCaster)
{
	HRESULT hRet = E_FAIL;
	m_kPacket.Pop(hRet);

	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hRet);
		return hRet;
	}

	SItemPos	kItemPos;
	SGroundKey	kGndKey;
	POINT3		kPos;

	m_kPacket.Pop(kGndKey);
	m_kPacket.Pop(kPos);
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pkCaster->GetInven()->GetItem(kItemPos,kItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
		return E_INVALID_ITEMPOS;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	if(UICT_MOVETOPARTYMEMBER != pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if(!pkPlayer)
	{
		return E_CANNOT_MOVE_MAP;// ������ �ѷ���� �� ������ ���� �̵� �Ұ� ó�� ����.
	}

	SReqMapMove_MT kRMM(MMET_None);
	kRMM.kTargetKey = kGndKey;
	kRMM.nTargetPortal = 0;
	kRMM.pt3TargetPos = kPos;

	PgReqMapMove kMapMove(m_pkGround, kRMM, NULL );
	if(kMapMove.Add( pkPlayer ))
	{
		kMapMove.AddModifyOrder(SPMO(IMET_MODIFY_COUNT | IMC_DEC_DUR_BY_USE, pkCaster->GetID(), SPMOD_Modify_Count(kItem,kItemPos,-1)));
		if(!kMapMove.DoAction())
		{
			return E_CANNOT_MOVE_MAP;
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_MOVETOPARTYMEMBER);
	kPacket.Push(S_OK);
	pkCaster->Send(kPacket);

	return S_OK;
}

bool PgAction_ReqMoveToPartyMemberGround::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( NULL == pkCaster )
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);

	if( S_OK == hRet )
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MOVETOPARTYMASTERGROUND);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Retrun false"));
	return false;
}

HRESULT PgAction_ReqMoveToPartyMemberGround::Process(CUnit* pkCaster)
{
	BM::GUID kMemberGuid;
	BM::GUID	kTowerGuid;
	TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;
	m_kPacket.Pop(kMemberGuid);
	m_kPacket.Pop(kTowerGuid);
	m_kPacket.Pop(kTargetKey);

	if( pkCaster->IsDead() )
	{
		return E_CANNOT_STATE_MAPMOVE;
	}

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	if( BM::GUID::NullData() == pkPlayer->PartyGuid() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	VEC_GUID kVec;
	bool bFindParty = m_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(), kVec);
	if( !bFindParty || (0 == kVec.size()) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	bool bFindMember = false;
	for(VEC_GUID::iterator iter = kVec.begin() ; iter != kVec.end() ; ++iter)
	{
		if( (*iter) == kMemberGuid )
		{
			bFindMember = true;
			break;
		}
	}

	if( !bFindMember )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	BM::Stream kPacket(PT_M_T_REQ_PARTYMASTERGROUNDKEY);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(kMemberGuid);
	kPacket.Push(m_kGndKey);
	kPacket.Push(kTowerGuid);
	kPacket.Push(kTargetKey);
	
	SendToCenter(kPacket);

	return S_OK;
}

bool PgAction_AnsMoveToPartyMemberGround::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( NULL == pkCaster )
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);

	if( S_OK == hRet )
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MOVETOPARTYMASTERGROUND);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_AnsMoveToPartyMemberGround::Process(CUnit* pkCaster)
{
	HRESULT hRet = E_FAIL;
	SGroundKey	kGndKey;
	POINT3		p3Position;
	BM::GUID	kTowerGuid;
	TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;

	m_kPacket.Pop(hRet);
	m_kPacket.Pop(kGndKey);
	m_kPacket.Pop(p3Position);
	m_kPacket.Pop(kTowerGuid);
	m_kPacket.Pop(kTargetKey);

	CONT_DEF_TRANSTOWER const *pkDefTransTower = NULL;
	g_kTblDataMgr.GetContDef( pkDefTransTower );

	CONT_DEF_TRANSTOWER::const_iterator def_itr = pkDefTransTower->find( kTowerGuid );
	if ( def_itr == pkDefTransTower->end() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found TransTower ID<" << kTowerGuid << L"> in DEF_TRANSTOWER");
		return E_CANNOT_MOVE_MAP;
	}

	TBL_DEF_TRANSTOWER_TARGET kTarget( kTargetKey );
	CONT_DEF_TRANSTOWER_TARGET::const_iterator target_itr = def_itr->second.find( kTarget );
	if ( target_itr == def_itr->second.end() )
	{
		return E_CANNOT_MOVE_MAP;
	}
	kTarget = *target_itr;

	if( S_OK != hRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hRet);
		return hRet;
	}

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MOVE_MAP"));
		return E_CANNOT_MOVE_MAP;
	}

	__int64 i64HaveMoney = pkPlayer->GetAbil64(AT_MONEY);

	if( kTarget.i64Price > i64HaveMoney )
	{
		pkPlayer->SendWarnMessage( 700036 );
		return E_CANNOT_MOVE_MAP;
	}

	SReqMapMove_MT kRMM( MMET_Normal, p3Position, kGndKey );

	PgReqMapMove kMapMove(m_pkGround, kRMM, NULL);
	if( kMapMove.Add(pkPlayer) )
	{
		if ( kTarget.i64Price )
		{
			SPMOD_Add_Money kDelMoneyData( -kTarget.i64Price );//�ʿ�Ӵ� ����.
			SPMO kIMO(IMET_ADD_MONEY, pkPlayer->GetID(), kDelMoneyData);
			kMapMove.AddModifyOrder( kIMO );
		}

		if( !kMapMove.DoAction() )
		{
			return E_CANNOT_MOVE_MAP;
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_MOVETOPARTYMASTERGROUND);
	kPacket.Push(S_OK);
	pkCaster->Send(kPacket);

	return S_OK;
}

bool PgAction_ReqRentalSafeExtend::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);
	if(S_OK == hRet)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_RENTALSAFE_EXTEND);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_ReqRentalSafeExtend::Process(CUnit* pkCaster)
{
	EInvType	kInvType;
	SItemPos	kItemPos;

	m_kPacket.Pop(kInvType);
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pkCaster->GetInven()->GetItem(kItemPos,kItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
		return E_INVALID_ITEMPOS;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	PgInventory * pkInven = pkCaster->GetInven();

	const int iUictType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	switch( iUictType )
	{
	case UICT_RENTALSAFE:
	case UICT_SHARE_RENTALSAFE_CASH:
	case UICT_SHARE_RENTALSAFE_GOLD:
		{
			if(IsShareRentalPremium(kInvType, dynamic_cast<PgPlayer*>(pkCaster)))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ALREADY_EXTEND"));
				return E_ALREADY_EXTEND;
			}

			if(E_ENABLE_TIMEOUT != pkInven->CheckEnableUseRentalSafe(kInvType))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ALREADY_EXTEND"));
				return E_ALREADY_EXTEND;
			}

			// ��� ���������� Ȯ�� �� �� �ִ� �� only �����ݰ�1�� ��!!
			if( UICT_SHARE_RENTALSAFE_GOLD == iUictType
			&&	IT_SHARE_RENTAL_SAFE2 <= kInvType )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
				return E_INCORRECT_ITEM;
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
			return E_INCORRECT_ITEM;
		}break;
	}

	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	if(iCustomValue1 <= 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	kOrder.push_back(SPMO(IMET_SET_RENTALSAFETIME, pkCaster->GetID(), tagPlayerModifyOrderData_ModifyRentalSafeTime(kInvType,iCustomValue1)));
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkCaster->GetID(), SPMOD_Modify_Count(kItem, kItemPos, -1)));

	BM::Stream kPacket(PT_M_C_ANS_RENTALSAFE_EXTEND);
	bool bIsShareSafe = false;
	if( pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) == UICT_SHARE_RENTALSAFE_CASH
	||	pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) == UICT_SHARE_RENTALSAFE_GOLD )
	{
		bIsShareSafe = true;
	}
	kPacket.Push(bIsShareSafe);
	PgAction_ReqModifyItem kItemModifyAction(CIE_RentalSafeExtend, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pkCaster, NULL);

	return S_OK;
}


bool PgAction_ReqSummonPartyMember::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	HRESULT hRet = Process(pkCaster);

	if(S_OK == hRet)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_SUMMONPARTYMEMBER);
	kPacket.Push(hRet);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_ReqSummonPartyMember::Process(CUnit* pkCaster)
{
	if(m_pkGround->GetAttr() & GATTR_INSTANCE)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MOVE_MAP"));
		return E_CANNOT_MOVE_MAP;
	}

	BM::GUID kMemberGuid;
	SItemPos kItemPos;
	m_kPacket.Pop(kMemberGuid);
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pkCaster->GetInven()->GetItem(kItemPos,kItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
		return E_INVALID_ITEMPOS;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	if(UICT_SUMMONPARTYMEMBER != pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	if(BM::GUID::NullData() == pkPlayer->PartyGuid())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	VEC_GUID kVec;
	//bool bFindParty = g_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(),kVec);
	bool bFindParty = m_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(),kVec);	
	if(!bFindParty || (0 == kVec.size()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	bool bFindMember = false;

	for(VEC_GUID::iterator iter = kVec.begin();iter != kVec.end();++iter)
	{
		if((*iter) == kMemberGuid)
		{
			bFindMember = true;
			break;
		}
	}

	if(!bFindMember)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	BM::Stream kPacket(PT_M_T_REQ_SUMMONPARTYMEMBER);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(kMemberGuid);
	kPacket.Push(m_kGndKey);
	kPacket.Push(kItemPos);

	SendToCenter(kPacket);

	return S_OK;
}

bool PgAction_ReqMoveToSummoner::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	BM::Stream kCopyPacket = m_kPacket;

	BM::GUID	kMemberGuid;
	SItemPos	kItemPos;
	SGroundKey	kGndKey;
	bool		bYesNo = false;

	kCopyPacket.Pop(bYesNo);
	kCopyPacket.Pop(kMemberGuid);
	kCopyPacket.Pop(kGndKey);
	kCopyPacket.Pop(kItemPos);

	HRESULT hRet = Process(pkCaster);

	if(S_OK == hRet)
	{
		return true;
	}

	if(E_RETURN_REJECT == hRet)
	{
		BM::Stream kPacket(PT_M_T_ANS_SUMMONPARTYMEMBER);
		kPacket.Push(kMemberGuid);
		kPacket.Push(kGndKey);
		kPacket.Push(hRet);

		SendToCenter(kPacket);

		BM::Stream kAnsPacket(PT_M_C_ANS_MOVETOSUMMONER);
		kAnsPacket.Push(S_OK);
		pkCaster->Send(kAnsPacket);
	}
	else
	{
		BM::Stream kPacket(PT_M_C_ANS_MOVETOSUMMONER);
		kPacket.Push(hRet);
		pkCaster->Send(kPacket);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_ReqMoveToSummoner::Process(CUnit* pkCaster)
{
	BM::GUID	kMemberGuid;
	SItemPos	kItemPos;
	SGroundKey	kGndKey;
	bool		bYesNo = false;
	m_kPacket.Pop(bYesNo);
	m_kPacket.Pop(kMemberGuid);
	m_kPacket.Pop(kGndKey);
	m_kPacket.Pop(kItemPos);

	if(!bYesNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_RETURN_REJECT"));
		return E_RETURN_REJECT;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	if(BM::GUID::NullData() == pkPlayer->PartyGuid())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_JOINED_PARTY"));
		return E_NOT_JOINED_PARTY;
	}

	VEC_GUID kVec;
	//bool bFindParty = g_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(),kVec);
	bool bFindParty = m_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(),kVec);	
	if(!bFindParty || (0 == kVec.size()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	VEC_GUID::iterator iter = std::find(kVec.begin(),kVec.end(),kMemberGuid);
	if(iter == kVec.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return E_NOT_FOUND_MEMBER;
	}

	BM::Stream kPacket(PT_M_T_REQ_MOVETOSUMMONER);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(kMemberGuid);
	kPacket.Push(m_kGndKey);
	kPacket.Push(kGndKey);
	kPacket.Push(kItemPos);

	SendToCenter(kPacket);

	return S_OK;
}

bool PgAction_AnsMoveToSummoner::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if(NULL == pkCaster)
	{
		return false;
	}

	BM::Stream kPacket = m_kPacket;
	HRESULT hRet = Process(pkCaster);

	if(S_OK == hRet)
	{
		return true;
	}

	BM::GUID	kCharGuid,
				kMemberGuid;
	SGroundKey	kCastGndKey;

	kPacket.Pop(kCharGuid);
	kPacket.Pop(kMemberGuid);
	kPacket.Pop(kCastGndKey);

	BM::Stream kAnsPacket1(PT_M_T_ANS_MOVETOSUMMONER);
	kAnsPacket1.Push(kCharGuid);
	kAnsPacket1.Push(kCastGndKey);
	kAnsPacket1.Push(hRet);
	SendToCenter(kAnsPacket1);

	BM::Stream kAnsPacket2(PT_M_C_ANS_SUMMONPARTYMEMBER);
	kAnsPacket2.Push(hRet);
	pkCaster->Send(kAnsPacket2);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgAction_AnsMoveToSummoner::Process(CUnit* pkCaster)
{
	if(m_pkGround->GetAttr() & GATTR_INSTANCE)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MOVE_MAP"));
		return E_CANNOT_MOVE_MAP;
	}

	BM::GUID	kCharGuid,
				kMemberGuid;

	SGroundKey	kCastGndKey;
	SItemPos	kItemPos;

	m_kPacket.Pop(kCharGuid);
	m_kPacket.Pop(kMemberGuid);
	m_kPacket.Pop(kCastGndKey);
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pkCaster->GetInven()->GetItem(kItemPos,kItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_ITEM"));
		return E_NOT_FOUND_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	if(UICT_SUMMONPARTYMEMBER != pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	tagPlayerModifyOrderData_Modify_Count kModifyItem(kItem, kItemPos, -1);
	SPMO kIMO(IMET_MODIFY_COUNT, pkCaster->GetID(), kModifyItem);
	kOrder.push_back(kIMO);

	BM::Stream kPacket(PT_M_C_ANS_SUMMONPARTYMEMBER);
	kPacket.Push(kCharGuid);
	kPacket.Push(kCastGndKey);

	PgAction_ReqModifyItem kItemModifyAction(CIE_SummonPartyMember, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pkCaster, NULL);

	return S_OK;
}

void PgAction_EventStart::DoAction(CONT_EVENT_STATE::mapped_type & element)
{
	TBL_EVENT const &kTbl = element.m_kTable;
	switch(kTbl.iEventType)
	{
	case ET_EXP_BONUS_RATE:
	case ET_MONEY_DROP_RATE:
	case ET_MONEY_GIVE_RATE:
		{
			if(		!element.bEventStarted// �ѹ� �߻� ��Ű�� �ٽ� �߻��ϸ� �ȵȴ�.
				&&	kTbl.aIntData[1]	
			)	
			{
				element.bEventStarted = true;

				SGroundKey const kTargetGndKey( kTbl.aIntData[1] );

				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_GROUND_MGR);
				kEventMsg.SecType(PT_M_G_NFY_EVENT_ABIL);

				kTargetGndKey.WriteToPacket(kEventMsg);
				kEventMsg.Push( true );//�׶��� ��ȣ�� üũ
				kTbl.WriteToPacket(kEventMsg);
				g_kTask.PutMsg(kEventMsg);
			}
		}break;
	case ET_NOTIFY_MSG:
		{
			if(element.CanEventPeriod())
			{
				element.timeLastRaise = ACE_OS::gettimeofday();
//				m_kContNfy.push_back(NFY_EVENT(element.m_kTable.aIntData[0], element.m_kTable.aStringData[0]));// �޼��� ���
				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_GROUND_MGR);
				kEventMsg.SecType(PT_T_C_NFY_NOTICE);
				kEventMsg.Push(element.m_kTable.aStringData[0]);
				g_kTask.PutMsg(kEventMsg);
			}	
		}break;
	case ET_SUMMON_MONSTER:
		{
			if(element.bEventStarted)	// �ѹ� �߻� ��Ű�� �ٽ� �߻��ϸ� �ȵȴ�.
			{
				return;
			}
			
			element.bEventStarted = true;
			element.kEventMonsterGen.iMonsterGonGroupNo = element.m_kTable.aIntData[0];
			element.kEventMonsterGen.kMonsterGenMode = MGM_GEN;

			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(PT_T_M_NFY_MONSTERGEN);
			kEventMsg.Push(element.m_kTable.aIntData[0]);
			kEventMsg.Push(MGM_GEN);
			g_kTask.PutMsg(kEventMsg);

//			m_kContMonsterGenNfy.push_back(NFY_MONSTERGEN(element.m_kTable.aIntData[0],MGM_GEN));
		}break;
	}
}

void PgAction_EventEnd::DoAction(CONT_EVENT_STATE::mapped_type & element)
{
	TBL_EVENT const &kTbl = element.m_kTable;
	switch(kTbl.iEventType)
	{
	case ET_EXP_BONUS_RATE:
	case ET_MONEY_DROP_RATE:
	case ET_MONEY_GIVE_RATE:
		{
			if (	element.bEventStarted // �߻����� ���� �̺�Ʈ�� ��� ��ų �ʿ� ����.
				&&	kTbl.aIntData[1] 
			)
			{
				SGroundKey const kTargetGndKey( kTbl.aIntData[1] );

				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_GROUND_MGR);
				kEventMsg.SecType(PT_M_G_NFY_EVENT_ABIL);

				TBL_EVENT kTblEmptyEvent = kTbl;
				kTblEmptyEvent.aIntData[0] *= -1;

				kTargetGndKey.WriteToPacket(kEventMsg);
				kEventMsg.Push( true );//�׶��� ��ȣ�� üũ
				kTblEmptyEvent.WriteToPacket(kEventMsg);
				g_kTask.PutMsg(kEventMsg);
			}
		}break;
	case ET_SUMMON_MONSTER:
		{
			if(!element.bEventStarted)	// �߻����� ���� �̺�Ʈ�� ��� ��ų �ʿ� ����.
			{
				return;
			}
			
			element.bEventStarted = false;

			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(PT_T_M_NFY_MONSTERGEN);
			kEventMsg.Push(element.m_kTable.aIntData[0]);
			kEventMsg.Push(MGM_REMOVE);
			g_kTask.PutMsg(kEventMsg);
//			m_kContMonsterGenNfy.push_back(NFY_MONSTERGEN(element.m_kTable.aIntData[0],MGM_REMOVE));
		}break;
	}
}

E_ERROR_FIT_TO_INV const PgFitToPrimaryInv::Process(CUnit * pUser)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return EF2I_NOT_FOUND_ITEM;
	}

	PgInventory * pkInv = pkPlayer->GetInven();

	PgBase_Item kItem;
	if(S_OK != pkInv->GetItem(m_kItemPos,kItem))
	{
		return EF2I_NOT_FOUND_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pkDef)
	{
		return EF2I_NOT_FOUND_ITEM;
	}

	if(pkDef->PrimaryInvType() == m_kItemPos.x)
	{
		return EF2I_ALREADY_PRIMARYINV;
	}

	if((IT_FIT == m_kItemPos.x) && (EQUIP_POS_MEDAL == m_kItemPos.y))	// ������ �κ��丮�� ���� �ʰ� ������ ���� �� -_-;; ��~~~~~~~~~~~~~~~~~~~~~!!!!!!!!!!!!!!!!!
	{
		PgItemToAchievement kItemToAchievement(kItem,m_kItemPos,m_pkGround->GroundKey());
		if(!kItemToAchievement.DoAction(pUser, NULL))
		{
			return EF2I_CANT_UNEQUIP_OLDITEM;
		}
	}
	else
	{
		SItemPos kTmpPos;
		if(!pkInv->GetFirstEmptyPos(static_cast<EInvType>(pkDef->PrimaryInvType()),kTmpPos))	// ���� �������� ��ġ�°� ���� ����� ������ �⺻ �κ����� ������ ����
		{
			return EF2I_NOT_HAVE_EMPTY_INV;
		}

		PgAction_MoveItem kMoveItem( m_kItemPos, kTmpPos, m_pkGround, g_kEventView.GetServerElapsedTime() );
		kMoveItem.DoAction(pUser, NULL);
	}

	return EF2I_SUCCESS;
}

bool PgFitToPrimaryInv::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(!pUser)
	{
		return false;
	}

	E_ERROR_FIT_TO_INV const kError = Process(pUser);
	if(EF2I_SUCCESS == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_FIT2PRIMARYINV);
	kPacket.Push(static_cast<HRESULT>(kError));
	pUser->Send(kPacket);
	return false;
}


bool PgAction_CashItem_Add_TimeLimit::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	ECashShopResult const kError = Process(pUser);

	if(CSR_SUCCESS == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_CS_ANS_ADD_TIMELIMIT);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	return false;
}

ECashShopResult const PgAction_CashItem_Add_TimeLimit::Process(CUnit * pUser)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return CSR_INVALID_CHARACTER;
	}

	int iArticleIdx = 0,
		iUseTime = 0;
	BYTE bTimeType = 0;
	SItemPos kItemPos;
	__int64 i64BonusUse = 0;
	m_kPacket.Pop(iArticleIdx);
	m_kPacket.Pop(bTimeType);
	m_kPacket.Pop(iUseTime);
	m_kPacket.Pop(kItemPos);
	m_kPacket.Pop(i64BonusUse);

	PgBase_Item kItem;
	if( S_OK != pUser->GetInven()->GetItem(kItemPos,kItem) )
	{
		return CSR_INVALID_ARTICLE;
	}

	if( !kItem.EnchantInfo().IsTimeLimit() )
	{
		return CSR_NOT_HAVE_TIMELIMIT;
	}

	if ( UIT_STATE_PET == kItem.State() )
	{
		return CSR_NOT_HAVE_TIMELIMIT;
	}

	TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_DEF_CASH_SHOP_ARTICLE const * pCont = kObjLock.Get();

	if(!pCont)
	{
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find(iArticleIdx);
	if(iter == pCont->end())
	{
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;

	bool bFindItem = false;
	for(CONT_CASH_SHOP_ITEM::const_iterator item_it = kArticle.kContCashItem.begin(); item_it != kArticle.kContCashItem.end(); ++item_it)
	{
		if((*item_it).iItemNo == kItem.ItemNo())
		{
			bFindItem = true;
			break;
		}
	}

	if(false == bFindItem)
	{
		return CSR_INVALID_ARTICLE;
	}

	BM::PgPackedTime kCurTime;
	kCurTime.SetLocalTime();
	BM::PgPackedTime kEmpty;
	kEmpty.Clear();
/*
	if(0 != kArticle.bState)
	{
		return CSR_INVALID_ARTICLE;
	}

	if(!(kEmpty == kArticle.kSaleStartDate) && !(kEmpty == kArticle.kSaleEndDate))
	{
		if((kCurTime < kArticle.kSaleStartDate) || (kArticle.kSaleEndDate < kCurTime))
		{
			return CSR_INVALID_ARTICLE;
		}
	}
*/
	CONT_CASH_SHOP_ITEM_PRICE::const_iterator priceiter = kArticle.kContCashItemPrice.find(TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(bTimeType,iUseTime));
	if(priceiter == kArticle.kContCashItemPrice.end())
	{
		return CSR_INVALID_ARTICLE;
	}

	CONT_CASH_SHOP_ITEM_PRICE::mapped_type const & kPrice = (*priceiter).second;

	if(true == kPrice.IsExtendDiscount)
	{
		if(0 == kPrice.iDiscountExtendCash)
		{
			return CSR_INVALID_ARTICLE;
		}
	}
	else
	{
		if(0 == kPrice.iExtendCash)
		{
			return CSR_INVALID_ARTICLE;
		}
	}

	PgBase_Item kItemCopy = kItem;
	if(0 == iUseTime)
	{
		SEnchantInfo kEnchant = kItemCopy.EnchantInfo();
		kEnchant.IsTimeLimit(0);
		kEnchant.TimeType(0);
		kEnchant.UseTime(0);
		kItemCopy.EnchantInfo(kEnchant);
	}
	else
	{
		if((UIT_DAY != bTimeType) || (kItemCopy.EnchantInfo().TimeType() != bTimeType))
		{
			return CSR_NOT_MATCH_TIMELIMIT;
		}

		if(true == kItemCopy.IsUseTimeOut())
		{
			if(MAX_CASH_ITEM_TIMELIMIT < iUseTime)
			{
				return CSR_TIMELIMIT_OVERFLOW;
			}

			kItemCopy.SetUseTime(bTimeType,iUseTime);
		}
		else
		{
			__int64 const i64NewUseTime = (kItemCopy.EnchantInfo().UseTime() + iUseTime);	// ����... 
			if(MAX_CASH_ITEM_TIMELIMIT < i64NewUseTime)
			{
				return CSR_TIMELIMIT_OVERFLOW;
			}

			kItemCopy.AddUseTime(bTimeType,iUseTime);
		}
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	SPMOD_Enchant kEnchantData( kItem, kItemPos, kItemCopy.EnchantInfo(),kItemCopy.CreateDate());//����� ��þƮ
	kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pkPlayer->GetID(), kEnchantData));

	BM::Stream kPacket(PT_M_I_CS_REQ_ADD_TIMELIMIT);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(iArticleIdx);
	kPacket.Push(bTimeType);
	kPacket.Push(iUseTime);
	kPacket.Push(i64BonusUse);
	kPacket.Push(m_kPacket);
	kPacket.Push(pkPlayer->CashShopGuid());
	kOrder.WriteToPacket(kPacket);
	SendToItem(m_kGndKey,kPacket);

	return CSR_SUCCESS;
}

bool PgAction_ReqRegUserPortal::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(NULL == pUser)
	{
		return false;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(NULL == pkPlayer)
	{
		return false;
	}

	if( 0 != (m_pkGround->GetAttr() & GATTR_FLAG_CANT_WARP) )
	{
		BM::Stream kPacket(PT_M_C_ANS_REG_PORTAL);
		kPacket.Push(E_CANNOT_SAVE);
		pUser->Send(kPacket);
		return false;
	}

	std::wstring kComment;
	m_kPacket.Pop(kComment);

	CONT_USER_PORTAL const kCont = pkPlayer->ContPortal();
	CONT_USER_PORTAL::const_iterator iter = kCont.begin();

	while(iter != kCont.end())	// �ӽ� -_-;; ������ �ϳ��� �����Ѵ�.
	{// �ϳ��� �����ϴ°��̹Ƿ� ���ﶩ ��� �������.
		m_kOrder.push_back(SPMO(IMET_DELETE_PORTAL,pkPlayer->GetID(), SMOD_Portal_Delete((*iter).first)));
		++iter;
	}

	m_kOrder.push_back(SPMO(IMET_CREATE_PORTAL,pkPlayer->GetID(), SMOD_Portal_Create(BM::GUID::Create(),kComment,m_pkGround->GroundKey().GroundNo(),pkPlayer->GetPos())));

	BM::Stream kPacket(PT_M_C_ANS_REG_PORTAL);
	PgAction_ReqModifyItem kItemModifyAction(CIE_Modify_UserPortal, m_pkGround->GroundKey(), m_kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MissionEffectCheck
//////////////////////////////////////////////////////////////////////////////
bool PgAction_MissionEffectCheck::DoAction(CUnit* pUser, CUnit* Nothing)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUser);

	if( !pkPlayer )
	{
		return false;
	}

	if( VecEffectValue.size() || (true == m_bCheckPenalty))
	{								
		VEC_GUID kGuidVec;
		m_pkGround->GetPartyMemberGround(pkPlayer->PartyGuid(), m_kGndKey, kGuidVec);

		bool bCheck = true;
		if( 0 != kGuidVec.size() )
		{
			PgPlayer *pkUser = NULL;
			VEC_GUID::iterator user_itr;
			for ( user_itr=kGuidVec.begin(); user_itr!=kGuidVec.end(); ++user_itr )
			{
				pkUser = dynamic_cast<PgPlayer*>(m_pkGround->GetUnit( *user_itr ));
				if( pkUser )
				{
					if ( pkUser->GetID() != pkPlayer->GetID() )
					{
						if ( (true == m_bCheckPenalty) && (INT_MAX == pkUser->GetAbil( AT_MISSION_THROWUP_PENALTY )) )
						{
							PgRequest_CheckPenalty kCheckPenalty( m_pkGround->GroundKey(), static_cast<WORD>(AT_MISSION_THROWUP_PENALTY), NULL );
							kCheckPenalty.DoAction( pkUser );
						}
					}

					bool bKindEffect = false;
					VEC_INT::const_iterator iter = VecEffectValue.begin();
					while( VecEffectValue.end() != iter )
					{
						CEffect *pkEffect = pkUser->GetEffect((*iter));
						if( pkEffect )
						{
							bKindEffect = true;
							break;
						}

						++iter;
					}

					if( bKindEffect )
					{
						continue;
					}
					else
					{
						bCheck = false;
					}
				}
			}
		}
		else
		{// ȥ�ڸ�...
			if( pkPlayer )
			{
				bool bKindEffect = false;
				VEC_INT::const_iterator iter = VecEffectValue.begin();
				while( VecEffectValue.end() != iter )
				{
					CEffect *pkEffect = pkPlayer->GetEffect((*iter));
					if( pkEffect )
					{
						bKindEffect = true;
						break;
					}

					++iter;
				}

				if( !bKindEffect )
				{
					bCheck = false;
				}
			}
		}
		if( false == bCheck )
		{
			if( 0 != VecEffectValue.size() )
			{
				return bCheck;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MissionEventHiddenLevelClearCheck
//////////////////////////////////////////////////////////////////////////////
bool PgAction_MissionEventHiddenLevelClearCheck::DoAction(CUnit* pUser, CUnit* Nothing)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUser);

	if( !pkPlayer )
	{
		return false;
	}

	if( !m_pkGround )
	{
		return false;
	}

	if( (MAX_MISSION_LEVEL-1) != m_iLevel )
	{
		// ������ ������ ��� ������ �Ҷ��� üũ �ؾߵȴ�.
		return true;
	}

	VEC_GUID kGuidVec;
	m_pkGround->GetPartyMemberGround(pkPlayer->PartyGuid(), m_kGndKey, kGuidVec);

	bool bCheck = true;
	if( 0 != kGuidVec.size() )
	{
		PgPlayer *pkUser = NULL;
		VEC_GUID::iterator user_itr;
		for( user_itr=kGuidVec.begin(); user_itr!=kGuidVec.end(); ++user_itr )
		{
			pkUser = dynamic_cast<PgPlayer*>(m_pkGround->GetUnit( *user_itr ));
			if( pkUser )
			{
				PgPlayer_MissionData const *pkMissionData = pkUser->GetMissionData( static_cast<unsigned int>(m_iMissionKey) );
				if( pkMissionData )
				{
					// ������ ������ ��� ����5�� üũ �ؾߵȴ�.
					if( false == pkMissionData->IsClearLevel(m_iLevel-1) )
					{
						return false;
					}
				}
			}
		}
	}
	else
	{// ȥ���� ���...
		return true;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MissionPartyMemberCheck
//////////////////////////////////////////////////////////////////////////////
bool PgAction_MissionPartyMemberCheck::DoAction(CUnit* pUser, CUnit* Nothing)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUser);

	if( !pkPlayer )
	{
		return false;
	}

	if( !m_pkGround )
	{
		return false;
	}

	if( pkPlayer->HaveParty() )
	{
		size_t iMemberCount = m_pkGround->GetPartyMemberCount( pkPlayer->PartyGuid() );		

		VEC_GUID kGuidVec;
		kGuidVec.clear();
		m_pkGround->GetPartyMemberGround( pkPlayer->PartyGuid(), m_pkGround->GroundKey(), kGuidVec );

		if( iMemberCount != kGuidVec.size() )
		{
			// ���� �׶��尡 �ƴ� ��Ƽ���� ����� ���� ���� �ʱ� ������
			return false;
		}

		PgPlayer *pkPartyMember = NULL;
		VEC_GUID::iterator user_itr = kGuidVec.begin();
		for ( ; user_itr!=kGuidVec.end(); ++user_itr )
		{
			pkPartyMember = dynamic_cast<PgPlayer*>(m_pkGround->GetUnit( *user_itr ));
			if( !pkPartyMember )
			{
				return false;								
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MissionEventHiddenRankCheck
//////////////////////////////////////////////////////////////////////////////
bool PgAction_MissionEventHiddenRankCheck::DoAction(CUnit* pUser, CUnit* Nothing)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUser);

	if( !pkPlayer )
	{
		return false;
	}

	if( !m_pkGround )
	{
		return false;
	}

	int const iMissionEventAllClear = 0x000F;

	VEC_GUID kGuidVec;
	m_pkGround->GetPartyMemberGround(pkPlayer->PartyGuid(), m_kGndKey, kGuidVec);

	bool bCheck = true;
	if( 0 != kGuidVec.size() )
	{
		PgPlayer *pkUser = NULL;
		VEC_GUID::iterator user_itr;
		for( user_itr=kGuidVec.begin(); user_itr!=kGuidVec.end(); ++user_itr )
		{
			pkUser = dynamic_cast<PgPlayer*>(m_pkGround->GetUnit( *user_itr ));
			if( pkUser )
			{				
				if( iMissionEventAllClear == (iMissionEventAllClear & pkUser->GetAbil(AT_MISSION_EVENT)) )
				{
					// �̼� �̺�Ʈ ���� ��� "SSS" ������� Ŭ���� �ߴ�. ���� ���� �����Ѵ�.
					continue;
				}
				else
				{
					return false;
				}
			}
		}
	}
	else
	{// ȥ���� ���...
		if( pkPlayer )
		{
			if( iMissionEventAllClear == (iMissionEventAllClear & pkPlayer->GetAbil(AT_MISSION_EVENT)) )
			{
				// �̼� �̺�Ʈ ���� ��� "SSS" ������� Ŭ���� �ߴ�. ���� ���� �����Ѵ�.
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MissionEffectCheck
//////////////////////////////////////////////////////////////////////////////
bool PgAction_MissionEventQuestCheck::DoAction(CUnit* pUser, CUnit* Nothing)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUser);

	if( !pkPlayer )
	{
		return false;
	}

	if( !m_pkGround )
	{
		return false;
	}

	if( VecQuestValue.size() )
	{
		VEC_GUID kGuidVec;
		m_pkGround->GetPartyMemberGround(pkPlayer->PartyGuid(), m_kGndKey, kGuidVec);

		bool bCheck = true;
		if( 0 != kGuidVec.size() )
		{
			PgPlayer *pkUser = NULL;
			VEC_GUID::iterator user_itr;
			for( user_itr=kGuidVec.begin(); user_itr!=kGuidVec.end(); ++user_itr )
			{
				pkUser = dynamic_cast<PgPlayer*>(m_pkGround->GetUnit( *user_itr ));
				if( pkUser )
				{
					bool bKindQuest = false;
					PgMyQuest const *pkMyQuest = pkUser->GetMyQuest();
					if(!pkMyQuest)
					{
						return false;
					}

					bool const bIngRet = pkMyQuest->IsIngQuestVec_All(VecQuestValue);
					bool const bEndRet = pkMyQuest->IsEndedQuestVec_All(VecQuestValue);

					if( bIngRet || bEndRet )
					{
						bKindQuest = true;
					}

					if( bKindQuest )
					{
						continue;
					}
					else
					{
						bCheck = false;
					}
				}
			}
		}
		else
		{// ȥ�ڸ�...
			if( pkPlayer )
			{
				bool bKindQuest = false;
				PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
				if(!pkMyQuest)
				{
					return false;
				}

				bool const bIngRet = pkMyQuest->IsIngQuestVec_All(VecQuestValue);
				bool const bEndRet = pkMyQuest->IsEndedQuestVec_All(VecQuestValue);

				if( bIngRet || bEndRet )
				{
					bKindQuest = true;
				}

				if( !bKindQuest )
				{
					bCheck = false;
				}
			}
		}
		if( false == bCheck )
		{
			if( 0 != VecQuestValue.size() )
			{
				return bCheck;
			}
		}
	}
	return true;
}

HRESULT const PgAction_ReqGemStoreBuy::Process(CUnit* pUser)
{
	int iItemNo = 0;
	m_kPacket.Pop(iItemNo);

	CONT_GEMSTORE const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if(NULL == pkDef)
	{
		return E_NOT_FOUND_GEMSTORE;
	}

	CONT_GEMSTORE::const_iterator iter = pkDef->find(m_kNpcGuid);
	if(iter == pkDef->end())
	{
		return E_NOT_FOUND_GEMSTORE;
	}

	CONT_GEMSTORE::mapped_type const & kStore = (*iter).second;
	CONT_GEMSTORE_ARTICLE::const_iterator articleiter = kStore.kContArticles.find(iItemNo);
	
	if(articleiter == kStore.kContArticles.end())
	{
		return E_NOT_FOUND_ARTICLE;
	}

	PgInventory * pkInven = pUser->GetInven();

	CONT_PLAYER_MODIFY_ORDER kOrder;

	CONT_GEMSTORE_ARTICLE::mapped_type const & kArticle = (*articleiter).second;

	if(0 < kArticle.iCP)
	{
		if(pUser->GetAbil(AT_CP) < kArticle.iCP)
		{
			return E_NOT_ENOUGH_CP;
		}

		kOrder.push_back(SPMO(IMET_ADD_CP,pUser->GetID(), SPMOD_Add_CP(-kArticle.iCP)));
	}

	int iUseGemCount = 0;
	for(CONT_GEMS::const_iterator gemiter = kArticle.kContGems.begin();gemiter != kArticle.kContGems.end();++gemiter)
	{
		if((*gemiter).second > static_cast<int>(pkInven->GetTotalCount((*gemiter).first)))
		{
			return E_NOT_ENOUGH_GEMS;
		}

		iUseGemCount += (*gemiter).second;

		SPMO kIMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any((*gemiter).first,-(*gemiter).second));
		kOrder.push_back(kIMO);
	}

	{// ������ ��ǥ ��� ������ ���� ����		
		PgAddAchievementValue kMA( AT_ACHIEVEMENT_CERTIFICATEOFHERO, iUseGemCount, m_kGndKey );
		kMA.DoAction( pUser, NULL );
	}

	PgBase_Item kItem;
	if(S_OK != CreateSItem(iItemNo,1,GIOT_GEMSTORE,kItem))
	{
		return E_NOT_FOUND_ARTICLE;
	}

	tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, SItemPos(0,0), true);
	SPMO kIMO(IMET_INSERT_FIXED, pUser->GetID(), kAddItem);
	kOrder.push_back(kIMO);

	BM::Stream kPacket(PT_M_C_ANS_GEMSTORE_BUY);
	PgAction_ReqModifyItem kItemModifyAction(CIE_GemStore_Buy, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqGemStoreBuy::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kRes = Process(pUser);
	if(S_OK == kRes)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_GEMSTORE_BUY);
	kPacket.Push(kRes);
	pUser->Send(kPacket);
	return false;
}

bool PgAction_ReqUseEmotion::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kRes = Process(pUser);
	if(S_OK == kRes)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_USE_EMOTION);
	kPacket.Push(kRes);
	pUser->Send(kPacket);


	return false;
}

HRESULT const PgAction_ReqUseEmotion::Process(CUnit* pUser)
{
	BYTE bType = 0;
	int iEmotionNo = 0;
	POINT3 ptPos;

	m_kPacket.Pop(bType);	
	m_kPacket.Pop(iEmotionNo);
	m_kPacket.Pop(ptPos);

	CONT_EMOTION const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if(NULL == pkDef)
	{
		return E_EM_NOT_FOUND_EMOTION;
	}

	CONT_EMOTION::const_iterator iter = pkDef->find(CONT_EMOTION::key_type(bType,iEmotionNo));
	if(iter == pkDef->end())
	{
		return E_EM_NOT_FOUND_EMOTION;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(NULL == pkPlayer)
	{
		return E_EM_NOT_FOUND_EMOTION;
	}

	if(0 != (*iter).second)
	{
		PgBitArray<MAX_DB_EMOTICON_SIZE> kCont;
		if(false == pkPlayer->GetContEmotion(bType,kCont))
		{
			return E_EM_NOT_FOUND_EMOTION;
		}

		if(false == kCont.Get((*iter).second))
		{
			return E_EM_NOT_HAVE_EMOTION;
		}
	}

	BM::Stream kPacket(PT_C_M_NFY_USE_EMOTION);
	kPacket.Push(pUser->GetID());
	kPacket.Push(bType);
	kPacket.Push(iEmotionNo);
	pUser->Send(kPacket,E_SENDTYPE_BROADCAST | E_SENDTYPE_SELF);
	return S_OK;
}

bool PgAction_ReqRollbackEnchant::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kRes = Process(pUser);
	if(S_OK == kRes)
	{
		return true;
	}

	BM::Stream kPacket(PT_C_M_REQ_ROLLBACK_ENCHANT);
	kPacket.Push(kRes);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqRollbackEnchant::Process(CUnit* pUser)
{
	int iIdx = 0;
	SItemPos kItemPos,
			 kRollbackItemPos;

	m_kPacket.Pop(iIdx);
	m_kPacket.Pop(kItemPos);
	m_kPacket.Pop(kRollbackItemPos);

	PgBase_Item kItem;
	if(S_OK != pUser->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_RBE_NOT_FOUND_TARGET_ITEM;
	}
	
	if( LOCAL_MGR::NC_JAPAN == g_kLocal.ServiceRegion() 
		&& CheckIsCashItem(kItem))
	{//�Ϻ��� ��� ĳ�� �������� �ҿ� ũ����Ʈ ���� �۾� �Ұ�
		return E_RBE_CANNOT_REPLACE_OPTION;
	}

	if(kItem.EnchantInfo().IsSeal() || kItem.EnchantInfo().IsBinding())
	{
		return E_RBE_NOT_FOUND_ROLLBACK_ITEM;
	}

	PgBase_Item kRollbackItem;
	if(S_OK != pUser->GetInven()->GetItem(kRollbackItemPos,kRollbackItem))
	{
		return E_RBE_NOT_FOUND_ROLLBACK_ITEM;
	}
	
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pItemDef = kItemDefMgr.GetDef(kRollbackItem.ItemNo());
	if(!pItemDef)
	{
		return E_RBE_NOT_FOUND_ROLLBACK_ITEM;
	}

	CItemDef const * pTargetDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pTargetDef || (ICMET_Cant_Rollback == (ICMET_Cant_Rollback & pTargetDef->GetAbil(AT_ATTRIBUTE))))
	{
		return E_RBE_NOT_FOUND_ROLLBACK_ITEM;
	}

	if(UICT_ROLLBACK != pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		return E_RBE_NOT_FOUND_ROLLBACK_ITEM;
	}

	PgBase_Item kCopyItem = kItem;

	GET_DEF(PgItemOptionMgr, kItemOptionMgr);
	if(false == kItemOptionMgr.DownGrade(iIdx,kCopyItem))
	{
		return E_RBE_CANNOT_REPLACE_OPTION;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	SPMOD_Modify_Count ModifyCount(kRollbackItem,kRollbackItemPos,-1);
	SPMO MOF(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE,pUser->GetID(),ModifyCount);
	kOrder.push_back(MOF);

	SPMOD_Enchant kEnchantData( kItem, kItemPos, kCopyItem.EnchantInfo());//����� ��þƮ
	SPMO kIMO(IMET_MODIFY_ENCHANT, pUser->GetID(), kEnchantData);
	kOrder.push_back(kIMO);

	BM::Stream kPacket(PT_M_C_ANS_ROLLBACK_ENCHANT);
	PgAction_ReqModifyItem kAction( CIE_Rollback, m_kGndKey, kOrder,kPacket );
	kAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqOpenLockedChest::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kError = Process(pUser);
	if(S_OK == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_OPEN_LOCKED_CHEST);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqOpenLockedChest::Process(CUnit* pUser)
{
	SItemPos kItemPos,
			 kKeyItemPos;

	m_kPacket.Pop(kItemPos);
	m_kPacket.Pop(kKeyItemPos);

	PgBase_Item kItem;
	if(S_OK != pUser->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_OC_NOT_FOUND_TARGET_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pItemDef)
	{
		return E_OC_NOT_FOUND_TARGET_ITEM;
	}

	int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);

	if(UICT_LOCKED_CHEST != iCustomType)
	{
		return E_OC_NOT_FOUND_TARGET_ITEM;
	}

	PgBase_Item kKeyItem;
	if(S_OK != pUser->GetInven()->GetItem(kKeyItemPos,kKeyItem))
	{
		return E_OC_NOT_FOUND_KEY_ITEM;
	}

	CItemDef const * pKeyDef = kItemDefMgr.GetDef(kKeyItem.ItemNo());
	if(!pKeyDef)
	{
		return E_OC_NOT_FOUND_KEY_ITEM;
	}

	if(UICT_CHEST_KEY != pKeyDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
	{
		return E_OC_NOT_FOUND_KEY_ITEM;
	}

	if(pKeyDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2) != iCustomValue2)
	{
		return E_OC_NOT_MATCH_KEY;
	}

	RESULT_ITEM_BAG_LIST_CONT kConRetItemNo;

	PgAction_PopItemContainer kPopItem( CIE_Locked_Chest, m_kGndKey, iCustomValue1, m_pkEventAbil );
	kPopItem.DoAction(pUser,1,false);
	kPopItem.SwapRetItemNo(kConRetItemNo);

	if(true == kConRetItemNo.empty())
	{
		CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[E_OC_EMPTY_ITEMBAG]"));
		return E_OC_EMPTY_ITEMBAG;
	}

	int const iItemNo = kConRetItemNo.front();
	CItemDef const * pRewardDef = kItemDefMgr.GetDef(iItemNo);
	if(!pRewardDef)
	{
		CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[E_OC_EMPTY_ITEMBAG]"));
		return E_OC_EMPTY_ITEMBAG;
	}

	BM::Stream kPacket(PT_M_C_ANS_OPEN_LOCKED_CHEST);

	int const iRewardType = pRewardDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);


	switch(iRewardType)
	{
	case UICT_BLANK:
		{// ��!! �ƹ��͵� ���� �׳� ���ڶ� ���踸 ����!!
			kPacket.Push(iRewardType);
			kPacket.Push(iItemNo);
		}break;
	case UICT_ONECEMORE:
		{// �ٽ� �ѹ� ���Դ�. �� �׳� Ŭ���̾�Ʈ���Ը� �ѹ� �� �϶�� �˸���� ���� ������ ���� �ʿ� ����!!
			kPacket.Push(static_cast<HRESULT>(S_OK));
			kPacket.Push(iRewardType);
			kPacket.Push(iItemNo);
			pUser->Send(kPacket);
			return S_OK;
		}break;
	default:
		{// ���� �� �������̴�. �κ��� ������ ��츦 ����ؼ� ������ ���� ���� ������ ������ ���� ������ ������ �ٽ� ������ ������.
			kPacket.Push(iRewardType);
			kPacket.Push(iItemNo);

			CONT_PLAYER_MODIFY_ORDER kOrder;
			kOrder.push_back(SPMO(IMET_ADD_ANY,pUser->GetID(), SPMOD_Add_Any(iItemNo,1)));
			kOrder.WriteToPacket(kPacket);
		}break;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	{
		SPMOD_Modify_Count ModifyCount(kItem,kItemPos,-1);
		SPMO MOF(IMET_MODIFY_COUNT,pUser->GetID(),ModifyCount);
		kOrder.push_back(MOF);
	}
	
	{
		SPMOD_Modify_Count ModifyCount(kKeyItem,kKeyItemPos,-1);
		SPMO MOF(IMET_MODIFY_COUNT,pUser->GetID(),ModifyCount);
		kOrder.push_back(MOF);
	}

	PgAction_ReqModifyItem kAction( CIE_Locked_Chest, m_kGndKey, kOrder,kPacket );
	kAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqOpenGamble::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kError = Process(pUser);
	if(S_OK == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_OPEN_GAMBLE);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	return true;
}

HRESULT PgAction_ReqOpenGamble::PopItem( CUnit *pUser, int const iGamebleID, PgBase_Item &rkRetItem, PgEventAbil const * const pkEventAbil )
{
	CONT_GAMBLE const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( pkDef )
	{
		CONT_GAMBLE::const_iterator gamble_itr = pkDef->find(iGamebleID);
		if( gamble_itr != pkDef->end() )
		{
			CONT_GAMBLE::mapped_type const & kGamble = gamble_itr->second;

			RESULT_ITEM_BAG_LIST_CONT kConRetItemNo;
			PgAction_PopItemContainer kPopItem( CIE_Open_Gamble, SGroundKey(), kGamble.iContainerNo, pkEventAbil );
			kPopItem.DoAction( pUser, 1, false);
			kPopItem.SwapRetItemNo(kConRetItemNo);

			if( kConRetItemNo.empty() )
			{
				CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[OUT OF RANGE SUCCESS RATE CONTROL]"));
				return E_GAMBLE_OUTOFRANGESRCTR;
			}

			int const iItemNo = kConRetItemNo.front();

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pRewardDef = kItemDefMgr.GetDef(iItemNo);
			if(!pRewardDef)
			{
				CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[GAMBLE ITEMBAG IS EMPTY!!]"));
				return E_GAMBLE_OUTOFRANGESRCTR;
			}

			if( S_OK != ::CreateSItem( iItemNo, 1, kGamble.iRarityControl, rkRetItem ) )
			{
				CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[CREATESITEM FAIL]"));
				return E_GAMBLE_CREATESITEMFAIL;
			}

			if( kGamble.bTimeType > 0 && kGamble.sUseTime > 0 )
			{
				if((0 < pRewardDef->GetAbil(AT_ENABLE_USE_TIMELIMIT)) || (true == pRewardDef->CanEquip()))
				{
					if( false == rkRetItem.SetUseTime( static_cast<__int64>(kGamble.bTimeType), static_cast<__int64>(kGamble.sUseTime)) )
					{
						rkRetItem = PgBase_Item();// �ʱ�ȭ ��������.
						return E_GAMBLE_INVALIDITEMTIME;
					}
				}
			}

			// Enchant
			if ( kGamble.iPlusUpControl )
			{
				int iMaxEnchantLv = 0;
				if ( true == PgItemRarityUpgradeFormula::GetMaxGradeLevel( ::GetItemGrade( rkRetItem ), pRewardDef->IsPetItem(), iMaxEnchantLv ) )
				{
					int const iEnchantLv = PgItemOptionMgr::DicePlusUp( kGamble.iPlusUpControl, iMaxEnchantLv );
					if ( iEnchantLv > 0 )
					{
						EPlusItemUpgradeResult kRet; 
						TBL_DEF_ITEMPLUSUPGRADE const *pkPlusInfo = PgAction_ItemPlusUpgrade::GetPlusInfo( rkRetItem, iEnchantLv, kRet );
						if ( pkPlusInfo )
						{
							SEnchantInfo kNewEnchantInfo = rkRetItem.EnchantInfo();

							size_t iRetIndex = 0;
							if( true == ::RouletteRate( pkPlusInfo->RareGroupSuccessRate, iRetIndex, MAX_ITEM_RARE_KIND_ARRAY) )
							{
								int const iResultType = PgAction_ItemPlusUpgrade::GetRareType( pkPlusInfo->RareGroupNo, iRetIndex );
								kNewEnchantInfo.PlusType(iResultType);
								kNewEnchantInfo.PlusLv(iEnchantLv);
								rkRetItem.EnchantInfo( kNewEnchantInfo );
							}
						}
					}
				}
			}

			return S_OK;
		}
	}
	return E_GAMBLE_NOT_FOUND_ITEM;
}

HRESULT const PgAction_ReqOpenGamble::Process(CUnit* pUser)
{
	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kGambleItem;
	if( S_OK == pUser->GetInven()->GetItem( kItemPos, kGambleItem ) )
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const * pItemDef = kItemDefMgr.GetDef(kGambleItem.ItemNo());
		if( pItemDef )
		{
			int const iCurLevel = pUser->GetAbil(AT_LEVEL);
			int const iMinLv = pItemDef->GetAbil(AT_LEVELLIMIT);
			int const iMaxLv = pItemDef->GetAbil(AT_MAX_LEVELLIMIT);
			if( ((iMinLv > 0) && (iCurLevel < iMinLv))
			 || ((iMaxLv > 0) && (iCurLevel > iMaxLv)) )
			{
				pUser->SendWarnMessage(22027);
				return E_GAMBLE_NO_MESSAGE;
			}

			CONT_GAMBLE_SHOUT_ITEM const * pkDefShoutItem = NULL;
			g_kTblDataMgr.GetContDef(pkDefShoutItem);
			if( pkDefShoutItem )
			{
				int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if( UICT_GAMBLE == iCustomType)
				{
					int const iCustomValue1 = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
//					int const iCustomValue2 = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);

					bool bBroadCast = false;

					PgBase_Item kRewardItem;
					HRESULT const hRet = PopItem( pUser, iCustomValue1, kRewardItem, m_pkEventAbil );
					if ( S_OK == hRet )
					{
						CONT_GAMBLE_SHOUT_ITEM::const_iterator shout_itr = pkDefShoutItem->find( CONT_GAMBLE_SHOUT_ITEM::key_type( iCustomValue1, kRewardItem.ItemNo() ) );
						if( shout_itr != pkDefShoutItem->end() )
						{
							PgBase_Item kTmpItem = kRewardItem;

							SEnchantInfo kEnchant = kTmpItem.EnchantInfo();
							kEnchant.IsCurse(0);
							kEnchant.IsSeal(0);
							kTmpItem.EnchantInfo(kEnchant);

							E_ITEM_GRADE const kGrade = GetItemGrade(kTmpItem);

							if( kGrade >= shout_itr->second )
							{
								bBroadCast = true;
							}
						}

						BM::Stream kPacket(PT_M_C_ANS_OPEN_GAMBLE);

						{
							// ������ �������� ������ �Һ� �����ǰ� ���� �̳༮�� �ٽ� ������ ������ ������ ������ ��û�Ѵ�.
							// �κ��丮�� ������� ���� ��� ��� �ޱ⸦ ���� �ϱ� ���ؼ� ������ ������ ó�� ��
							CONT_PLAYER_MODIFY_ORDER kOrder;
							kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(),SPMOD_Insert_Fixed(kRewardItem, SItemPos(), true)));
							kOrder.WriteToPacket(kPacket);
						}

						kPacket.Push(bBroadCast);
						kPacket.Push(pUser->Name());
						kPacket.Push(kGambleItem.ItemNo());
						kPacket.Push(kRewardItem.ItemNo());

						CONT_PLAYER_MODIFY_ORDER kOrder;
						kOrder.push_back(SPMO(IMET_ADD_ANY|IMC_DEC_DUR_BY_USE, pUser->GetID(),SPMOD_Add_Any(kGambleItem.ItemNo(),-1)));
					//	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE,pUser->GetID(),SPMOD_Modify_Count(kItem,kItemPos,-1)));

						PgAction_ReqModifyItem kAction( CIE_Open_Gamble, m_kGndKey, kOrder, kPacket );
						kAction.DoAction( pUser, NULL );
					}
					return hRet;
				}
			}	
		}
	}
	return E_GAMBLE_NOT_FOUND_ITEM;
}

bool PgAction_HiddenItemBag::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kError = Process(pUser);
	if(S_OK == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_HIDDEN_ITEM_PACK);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	return true;
}

HRESULT PgAction_HiddenItemBag::PopItem( CUnit *pUser, int const iBagGrpNoID, PgBase_Item &rkRetItem, PgEventAbil const * const pkEventAbil )
{
	CONT_HIDDENREWORDBAG const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( pkDef )
	{
		CONT_HIDDENREWORDBAG::const_iterator hidden_iter = pkDef->find(iBagGrpNoID);
		if( hidden_iter != pkDef->end() )
		{
			CONT_HIDDENREWORDBAG::mapped_type const & kHidden = hidden_iter->second;

			RESULT_ITEM_BAG_LIST_CONT kConRetItemNo;
			PgAction_PopItemContainer kPopItem( CIE_HiddenPack, SGroundKey(), kHidden.iContainerNo, pkEventAbil );
			kPopItem.DoAction( pUser, 1, false);
			kPopItem.SwapRetItemNo(kConRetItemNo);

			if( kConRetItemNo.empty() )
			{
				//CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[OUT OF RANGE SUCCESS RATE CONTROL]"));
				return E_GAMBLE_OUTOFRANGESRCTR;
			}

			int const iItemNo = kConRetItemNo.front();

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pRewardDef = kItemDefMgr.GetDef(iItemNo);
			if(!pRewardDef)
			{
				CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[GAMBLE ITEMBAG IS EMPTY!!]"));
				return E_GAMBLE_OUTOFRANGESRCTR;
			}

			if( S_OK != ::CreateSItem( iItemNo, 1, kHidden.iRarityControl, rkRetItem ) )
			{
				CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("[CREATESITEM FAIL]"));
				return E_GAMBLE_CREATESITEMFAIL;
			}

			// Enchant
			if ( kHidden.iPlusUpControl )
			{
				int iMaxEnchantLv = 0;
				if ( true == PgItemRarityUpgradeFormula::GetMaxGradeLevel( ::GetItemGrade( rkRetItem ), pRewardDef->IsPetItem(), iMaxEnchantLv ) )
				{
					int const iEnchantLv = PgItemOptionMgr::DicePlusUp( kHidden.iPlusUpControl, iMaxEnchantLv );
					if ( iEnchantLv > 0 )
					{
						EPlusItemUpgradeResult kRet; 
						TBL_DEF_ITEMPLUSUPGRADE const *pkPlusInfo = PgAction_ItemPlusUpgrade::GetPlusInfo( rkRetItem, iEnchantLv, kRet );
						if ( pkPlusInfo )
						{
							SEnchantInfo kNewEnchantInfo = rkRetItem.EnchantInfo();

							size_t iRetIndex = 0;
							if( true == ::RouletteRate( pkPlusInfo->RareGroupSuccessRate, iRetIndex, MAX_ITEM_RARE_KIND_ARRAY) )
							{
								int const iResultType = PgAction_ItemPlusUpgrade::GetRareType( pkPlusInfo->RareGroupNo, iRetIndex );
								kNewEnchantInfo.PlusType(iResultType);
								kNewEnchantInfo.PlusLv(iEnchantLv);
								rkRetItem.EnchantInfo( kNewEnchantInfo );
							}
						}
					}
				}
			}

			return S_OK;
		}
	}
	return E_GAMBLE_NOT_FOUND_ITEM;
}

HRESULT const PgAction_HiddenItemBag::Process(CUnit* pUser)
{
	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kHiddenItem;
	if( S_OK == pUser->GetInven()->GetItem( kItemPos, kHiddenItem ) )
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const * pItemDef = kItemDefMgr.GetDef(kHiddenItem.ItemNo());
		if( pItemDef )
		{
			int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
			if( UICT_HIDDEN_REWORD == iCustomType)
			{
				bool bBroadCast = false;

				CONT_PLAYER_MODIFY_ORDER kOrder;					

				int const CUSTOM_VALUE_MAX = 4;
				int const iCustomValueArray[CUSTOM_VALUE_MAX] = {AT_USE_ITEM_CUSTOM_VALUE_1, AT_USE_ITEM_CUSTOM_VALUE_2, AT_USE_ITEM_CUSTOM_VALUE_3, AT_USE_ITEM_CUSTOM_VALUE_4};
				for(int i=0; i<CUSTOM_VALUE_MAX; ++i)
				{
					PgBase_Item kRewardItem;
					int const iCustomValue = pItemDef->GetAbil(iCustomValueArray[i]);	// Index
					HRESULT const hRet = PopItem( pUser, iCustomValue, kRewardItem, m_pkEventAbil );			
					if( S_OK == hRet )
					{
						// ������ �������� ������ �Һ� �����ǰ� ���� �̳༮�� �ٽ� ������ ������ ������ ������ ��û�Ѵ�.
						// �κ��丮�� ������� ���� ��� ��� �ޱ⸦ ���� �ϱ� ���ؼ� ������ ������ ó�� ��						
						kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(),SPMOD_Insert_Fixed(kRewardItem, SItemPos(), true)));
					}
				}

				if( !kOrder.empty() )
				{
					BM::Stream kPacket(PT_M_C_ANS_HIDDEN_ITEM_PACK);
					kOrder.WriteToPacket(kPacket);

					CONT_PLAYER_MODIFY_ORDER kSendOrder;
					kSendOrder.push_back(SPMO(IMET_ADD_ANY|IMC_DEC_DUR_BY_USE, pUser->GetID(),SPMOD_Add_Any(kHiddenItem.ItemNo(),-1)));

					PgAction_ReqModifyItem kAction( CIE_HiddenPack, m_kGndKey, kSendOrder, kPacket );
					kAction.DoAction( pUser, NULL );

					return S_OK;
				}					
			}
		}
	}
	return E_GAMBLE_NOT_FOUND_ITEM;
}

PgAction_DropLimitedItem::PgAction_DropLimitedItem(SGroundKey const& rkGndKey, BM::GUID const &kOwnerGuid, POINT3 const &ptPos, int const kObjectType, int const kObjectNo)
	:m_kGndKey(rkGndKey), m_kOwnerGuid(kOwnerGuid), m_kObjectType(kObjectType), m_kObjectNo(kObjectNo), m_kptPos(ptPos)
{

}

bool PgAction_DropLimitedItem::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{	//Ȯ��ǥ�� ����.
	//��û�� ����.
	CONT_LIMITED_ITEM_CONTROL const * pkContLimitedItemCtrl = NULL;
	g_kTblDataMgr.GetContDef(pkContLimitedItemCtrl);

	CONT_LIMITED_ITEM_CONTROL::const_iterator ctrl_itor = pkContLimitedItemCtrl->find( tagLimitItemControlKey(m_kObjectType, m_kObjectNo) );
	if(ctrl_itor != pkContLimitedItemCtrl->end())
	{
		CONT_LIMITED_ITEM_CONTROL::mapped_type const& rkVec = (*ctrl_itor).second;
		CONT_LIMITED_ITEM_CONTROL::mapped_type::const_iterator itor_value = rkVec.begin();
		while (itor_value != rkVec.end())
		{
			CONT_LIMITED_ITEM_CONTROL::mapped_type::value_type const &kCtrlElement = (*itor_value);
			if(::lwIsRandSuccess(kCtrlElement.iRate, 1000000))
			{
				SREQ_GIVE_LIMITED_ITEM kData;

				kData.m_kGndKey = m_kGndKey;
				kData.m_kptPos = m_kptPos;
				kData.m_kOwnerGuid = m_kOwnerGuid;
				kData.m_kTryLimitEventNo = kCtrlElement.iEventNo;
				kData.m_nOwnerLevel = static_cast<short>(pkCaster->GetAbil(AT_LEVEL));

				BM::Stream kPacket(PT_M_N_REQ_GIVE_LIMITED_ITEM, kData);
				kPacket.Push(kData);
				SendToItem(m_kGndKey, kPacket);
			}
			++itor_value;
		}
	}
	return true;
}
	
void PgAction_EventProcess::DoAction(CONT_EVENT_STATE::mapped_type & kEvent,PgPlayer * pkPlayer)
{
	switch(kEvent.m_kTable.iEventType)
	{
	case ET_PCROOMEVENT_COUPON:
		{
			BM::Stream kPacket(PT_M_I_GEN_COUPON);
			kPacket.Push(pkPlayer->GetMemberGUID());
			kPacket.Push(pkPlayer->GetID());
			kPacket.Push(kEvent.m_kTable.aIntData[0]);
			kPacket.Push(kEvent.m_kTable.aStringData[0]);
			kPacket.Push(kEvent.m_kTable.aStringData[1]);
			SendToItem(pkPlayer->GroundKey(),kPacket);
		}break;
	case ET_PCROOMEVENT_EFFECT:
		{
			for(int i = 0;i < MAX_EVENT_ARG;++i)
			{
				if(0 < kEvent.m_kTable.aIntData[i])
				{
					SEffectCreateInfo kCreate;
					kCreate.eType = EFFECT_TYPE_NORMAL;
					kCreate.iEffectNum = kEvent.m_kTable.aIntData[i];
					kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					pkPlayer->AddEffect(kCreate);// �г�Ƽ ����Ʈ�� ���̰�. 
				}
			}
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5,__FL__ << _T("NOT MANUAL EVENT CALL : ") << kEvent.m_kTable.iEventNo << _T(" : ") << kEvent.m_kTable.kTitle);
		}break;
	}
}


bool PgAction_ReqJoinEvent::DoAction(CUnit* pUser, CUnit* Nothing)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(NULL == pkPlayer)
	{
		return false;
	}

	int iEventNo = 0;
	m_kPacket.Pop(iEventNo);

	g_kEventView.CallbackProcessEvent(&m_kEventProcess);
	
	HRESULT kError = g_kEventView.ProcessManualEvent(iEventNo,pkPlayer);

	BM::Stream kPacket(PT_M_C_ANS_JOIN_EVENT);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	return false;
}

bool PgAction_ReqConvertItem::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kError = Process(pUser);
	if(S_OK == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_CONVERTITEM);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqConvertItem::Process(CUnit* pUser)
{
	int iItemNo = 0;
	m_kPacket.Pop(iItemNo);
	int iConvertNum = 0;
	m_kPacket.Pop(iConvertNum);

	CONT_DEF_CONVERTITEM const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if(NULL == pkDef)
	{
		return E_CI_IS_NOT_CONVERTITEM;
	}

	CONT_DEF_CONVERTITEM::const_iterator iter = pkDef->find(iItemNo);
	if(iter == pkDef->end())
	{
		return E_CI_IS_NOT_CONVERTITEM;
	}

	if(0 >= iConvertNum)
	{
		return E_CI_INVALID_CONVERTNUM;
	}

	int const iHasItemNum = pUser->GetInven()->GetTotalCount(iItemNo);
	int const iNeedItemNum = (*iter).second.sSourceItemNum * iConvertNum;
	int const iCreateItemNum = (*iter).second.sTargetItemNum * iConvertNum;

	if(iHasItemNum < iNeedItemNum)
	{
		return E_CI_NOT_ENOUGH_SOURCEITEM;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_ADD_ANY,pUser->GetID(), SPMOD_Add_Any((*iter).second.iSourceItemNo,-iNeedItemNum)));
	kOrder.push_back(SPMO(IMET_ADD_ANY,pUser->GetID(), SPMOD_Add_Any((*iter).second.iTargetItemNo,iCreateItemNum)));

	BM::Stream kPacket(PT_M_C_ANS_CONVERTITEM);
	PgAction_ReqModifyItem kAction( CIE_Convert_Item, m_kGndKey, kOrder,kPacket );
	kAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqSetExpCard::DoAction(CUnit* pUser, CUnit* Nothing)
{
	if(!pUser)
	{
		return false;
	}

	HRESULT const kErr = Process(pUser);
	BM::Stream kPacket(PT_M_C_ANS_EXPCARD_ACTIVATE);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return true;
}

HRESULT const PgAction_ReqSetExpCard::Process(CUnit* pUser)
{
	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);
	PgInventory * pkInv = pUser->GetInven();

	PgBase_Item kItem;
	if(S_OK != pkInv->GetItem(kItemPos,kItem))
	{
		return E_EXPCARD_NOT_FOUND_ITEM;
	}

	SExpCard kExpCard;
	if(!kItem.Get(kExpCard))
	{
		return E_EXPCARD_NOT_EXPCARD;
	}

	pkInv->ExpCardItem(kItem.Guid());
	return S_OK;
}

bool PgAction_ReqUseExpCard::DoAction(CUnit* pUser, CUnit* Nothing)
{
	if(!pUser)
	{
		return false;
	}

	HRESULT const kErr = Process(pUser);

	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_EXPCARD_USE);
	kPacket.Push(kErr);
	pUser->Send(kPacket);

	return false;
}

HRESULT const PgAction_ReqUseExpCard::Process(CUnit* pUser)
{
	SItemPos	kExpCardPos,
				kCardBreakPos;

	m_kPacket.Pop(kExpCardPos);
	m_kPacket.Pop(kCardBreakPos);

	PgInventory * pkInv = pUser->GetInven();

	PgBase_Item kExpCard;
	if(S_OK != pkInv->GetItem(kExpCardPos,kExpCard))
	{
		return E_EXPCARD_NOT_FOUND_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CItemDef const * pkExpCardDef = kItemDefMgr.GetDef(kExpCard.ItemNo());
	if(!pkExpCardDef || (UICT_EXPCARD != pkExpCardDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)))
	{
		return E_EXPCARD_NOT_FOUND_ITEM;
	}

	BM::PgPackedTime kTime;
	kTime.SetTime(pUser->GetAbil(AT_ACHIEVEMENT_EXPCARD_DATE));// ���� ���� ������ ���� �Լ� ����� �����Ұ�

	BM::DBTIMESTAMP_EX kLocalTime;
	g_kEventView.GetLocalTime(kLocalTime);
	BM::PgPackedTime kToday(kLocalTime);

	kTime.Hour(0);
	kTime.Min(0);
	kTime.Sec(0);

	kToday.Hour(0);
	kToday.Min(0);
	kToday.Sec(0);

	CONT_PLAYER_MODIFY_ORDER kOrder;

	if(!(kTime == kToday))
	{
		int const iUseDate = kToday.GetTime();// ���� ���� ������ ���� �Լ� ����� �����Ұ�

		pUser->SetAbil(AT_ACHIEVEMENT_EXPCARD_DATE,iUseDate);
		pUser->SetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT,0);

		kOrder.push_back(SPMO(IMET_SET_ABIL, pUser->GetID(),SPMOD_SetAbil(AT_ACHIEVEMENT_EXPCARD_DATE,iUseDate)));
		kOrder.push_back(SPMO(IMET_SET_ABIL, pUser->GetID(),SPMOD_SetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT,0)));
	}

	int iValue = DEFAULT_ENABLE_USE_EXPCARD_NUM;

	g_kVariableContainer.Get(EVar_Kind_ExpCard, EVar_ExpCard_Enable_Use_Num, iValue);

	if(iValue <= pUser->GetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT))
	{
		return E_EXPCARD_OVER_ENABLE_USE;
	}

	SExpCard kCard;
	if(false == kExpCard.Get(kCard))
	{
		return E_EXPCARD_NOT_EXPCARD;
	}

	if(kCard.CurExp() < kCard.MaxExp())
	{
		return E_EXPCARD_NOT_COMPLETE;
	}

	PgBase_Item kCardBreak;
	if(S_OK != pkInv->GetItem(kCardBreakPos, kCardBreak))
	{
		return E_EXPCARD_NOT_FOUND_ITEM;
	}

	CItemDef const * pkCardBreakDef = kItemDefMgr.GetDef(kCardBreak.ItemNo());
	if(!pkCardBreakDef || (UICT_EXPCARD_BREAKER != pkCardBreakDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)))
	{
		return E_EXPCARD_NOT_FOUND_ITEM;
	}

	if(pkExpCardDef->GetAbil(AT_MAX_LEVELLIMIT) != pkCardBreakDef->GetAbil(AT_MAX_LEVELLIMIT))
	{
		return E_EXPCARD_NOT_MATCH;
	}

	int const iCurLevel = pUser->GetAbil(AT_LEVEL);
	int const iMinLv = pkExpCardDef->GetAbil(AT_LEVELLIMIT);
	int const iMaxLv = pkExpCardDef->GetAbil(AT_MAX_LEVELLIMIT);
	if( ((iMinLv > 0) && (iCurLevel < iMinLv))
	 || ((iMaxLv > 0) && (iCurLevel > iMaxLv)) )
	{
		return E_EXPCARD_NOT_ENOUGH_LEVEL;
	}

	pUser->SetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT,pUser->GetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT)+1);// �̸� ����� �����Ͽ� �ι� ��� ���� ���ϰ� ���´�

	kOrder.push_back(SPMO(IMET_SET_ABIL, pUser->GetID(),SPMOD_SetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT,pUser->GetAbil(AT_ACHIEVEMENT_EXPCARD_COUNT))));
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), SPMOD_Modify_Count(kExpCard,kExpCardPos,-1)));
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), SPMOD_Modify_Count(kCardBreak,kCardBreakPos,-1)));
	kOrder.push_back(SPMO(IMET_ADD_ABIL64, pUser->GetID(), SPMOD_AddAbil(AT_EXPERIENCE,kCard.CurExp())));
	
	BM::Stream kPacket(PT_M_C_ANS_EXPCARD_USE);
	kPacket.Push(kCard.CurExp());

	PgAction_ReqModifyItem kItemModifyAction(CIE_Use_ExpCard, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

int PgAction_PopEmporiaCapsule::GetEmporiaFuncValue( SEmporiaKey const &kKey, short const nFuncNo )
{
	CONT_DEF_EMPORIA const *pkDefEmporia = NULL;
	g_kTblDataMgr.GetContDef( pkDefEmporia );

	if ( pkDefEmporia )
	{
		CONT_DEF_EMPORIA::const_iterator emporia_itr = pkDefEmporia->find( kKey.kID );
		if ( emporia_itr != pkDefEmporia->end() )
		{
			int const iIndex = static_cast<int>( kKey.byGrade - 1 );
			if ( -1 < iIndex && iIndex < MAX_EMPORIA_GRADE )
			{
				CONT_DEF_EMPORIA_FUNCTION::const_iterator func_itr = emporia_itr->second.m_kContDefFunc[iIndex].find( nFuncNo );
				if ( func_itr != emporia_itr->second.m_kContDefFunc[iIndex].end() )
				{
					return func_itr->second.iValue;
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found EmporiaID<" << kKey.kID << L"> Grade<" << kKey.byGrade << L"> FunctionNo<" << nFuncNo << L">" );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error EmporiaID<" << kKey.kID << L"> Index<" << iIndex << L">" );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found EmporiaID<" << kKey.kID << L">" );
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"CONT_DEF_EMPORIA is NULL" );
	}

	return 0;
}

bool PgAction_PopEmporiaCapsule::PopItem( PgBase_Item & rkOutItem )const
{
	int const iValue = GetEmporiaFuncValue( m_kEmporia.GetKey(), m_nFuncNo );

	if ( iValue )
	{
		int const iLevel = m_pkPlayer->GetAbil(AT_LEVEL);
		PgItemBag kItemBag;

		GET_DEF(CItemBagMgr, kItemBagMgr);
		if ( S_OK == kItemBagMgr.GetItemBag( iValue, static_cast<short>(iLevel), kItemBag ) )
		{
			int iItemNo = 0;
			int iItemCount = 0;
			if ( SUCCEEDED(kItemBag.PopItem( iLevel, iItemNo, iItemCount )) )
			{
				if ( S_OK == ::CreateSItem( iItemNo, iItemCount, GIOT_NONE,  rkOutItem ) )
				{
					return true;
				}
			}
		}
	}
	
	VERIFY_INFO_LOG( false, BM::LOG_LV4, L"Not Found ItemBag<" << iValue << L"EmporiaID<" << m_kEmporia.GetKey().kID << L"> Grade<" << m_kEmporia.GetKey().byGrade << L">" );
	return false;
}

bool PgAction_ReqMixupItem::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kErr = Process(pUser);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MIXUPITEM);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqMixupItem::Process(CUnit* pUser)
{
	SItemPos	kItemPos1,
				kItemPos2,
				kRateItemPos;

	m_kPacket.Pop(kItemPos1);
	m_kPacket.Pop(kItemPos2);
	m_kPacket.Pop(kRateItemPos);

	CONT_MIXUPITEM const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if(NULL == pkDef)
	{
		return E_MIXUP_NOT_FOUND_ITEM;
	}

	PgInventory * pkInv = pUser->GetInven();

	PgBase_Item kItem1,
				kItem2;
	if((S_OK != pkInv->GetItem(kItemPos1,kItem1)) || (S_OK != pkInv->GetItem(kItemPos2,kItem2)))
	{
		return E_MIXUP_NOT_FOUND_ITEM;
	}

	E_ITEM_GRADE const kItemGrade1 = GetItemGrade(kItem1);
	E_ITEM_GRADE const kItemGrade2 = GetItemGrade(kItem2);

	if( true == kItem1.EnchantInfo().IsBinding() || 
		true == kItem2.EnchantInfo().IsBinding() ||
		true == kItem1.EnchantInfo().IsSeal() ||
		true == kItem2.EnchantInfo().IsSeal() ||
		IG_NORMAL > kItemGrade1 ||
		IG_NORMAL > kItemGrade2)
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CItemDef const * pkItemDef1 = kItemDefMgr.GetDef(kItem1.ItemNo());
	CItemDef const * pkItemDef2 = kItemDefMgr.GetDef(kItem2.ItemNo());

	if((NULL == pkItemDef1) || (NULL == pkItemDef2))
	{
		return E_MIXUP_NOT_FOUND_ITEM;
	}

	if(false == pkItemDef1->IsType(static_cast<EItemType>(ITEM_TYPE_AIDS | ITEM_TYPE_EQUIP)) || false == pkItemDef2->IsType(static_cast<EItemType>(ITEM_TYPE_AIDS | ITEM_TYPE_EQUIP)))
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	if(pkItemDef1->EquipPos() != pkItemDef2->EquipPos())
	{
		return E_MIXUP_NOT_MATCH_EQUIP;
	}
	
	if(pkItemDef1->GetAbil(AT_GRADE) != pkItemDef2->GetAbil(AT_GRADE))
	{
		return E_MIXUP_NOT_MATCH_GRADE;
	}

	if(pkItemDef1->GetAbil(AT_GENDERLIMIT) != pkItemDef2->GetAbil(AT_GENDERLIMIT))
	{
		return E_MIXUP_NOT_MATCH_GENDER;
	}

	CONT_MIXUPITEM::const_iterator iter = pkDef->find(CONT_MIXUPITEM::key_type(pkItemDef1->GetAbil(AT_GRADE),pkItemDef1->GetAbil(AT_GENDERLIMIT),pkItemDef1->EquipPos()));
	if(iter == pkDef->end())
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	int iAddRate = 0;

	PgBase_Item kRateItem;
	if(S_OK == pkInv->GetItem(kRateItemPos,kRateItem))
	{
		CItemDef const * pkRateItem = kItemDefMgr.GetDef(kRateItem.ItemNo());
		if(!pkRateItem)
		{
			return E_MIXUP_NOT_FOUND_ITEM;
		}

		if(pkItemDef1->GetAbil(AT_GRADE) != pkRateItem->GetAbil(AT_GRADE))
		{
			return E_MIXUP_CANT_USE_RATE_ITEM;
		}

		iAddRate = pkRateItem->GetAbil(AT_SUCCESSRATE);
	}

	CONT_MIXUPITEM::mapped_type const & kMixupItem = (*iter).second;

	GET_DEF(CItemBagMgr, kItemBagMgr);

	const CONT_DEF_ITEM_BAG * pContItemBag = NULL;
	g_kTblDataMgr.GetContDef(pContItemBag);

	const CONT_DEF_ITEM_BAG_ELEMENTS * pContElement = NULL;
	g_kTblDataMgr.GetContDef(pContElement);

	if(NULL == pContItemBag || NULL == pContElement)
	{
		return E_MIXUP_CANT_USE_RATE_ITEM;
	}

	size_t kResultIdx = 0;
	if(false == ::RouletteRate(kMixupItem.iSuccessControlNo,kResultIdx,MAX_MIXUP_ITEM_ARRAY_NUM,iAddRate))
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	short const nLevel = static_cast<short>(pUser->GetAbil(AT_LEVEL));
	CONT_DEF_ITEM_BAG::const_iterator bag_iter = pContItemBag->find( CONT_DEF_ITEM_BAG::key_type( kMixupItem.iItemBagNo[kResultIdx], nLevel) );
	if(bag_iter == pContItemBag->end())
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	CONT_DEF_ITEM_BAG::mapped_type const & kItemBag = (*bag_iter).second;

	CONT_DEF_ITEM_BAG_ELEMENTS::const_iterator element_iter = pContElement->find(kItemBag.iElementsNo);
	if(element_iter == pContElement->end())
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	if(false == ::RouletteRate(kItemBag.iRaseRateNo,kResultIdx,MAX_SUCCESS_RATE_ARRAY))
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	int const iItemNo = (*element_iter).second.aElement[kResultIdx];

	PgBase_Item kItem;
	if(S_OK != ::CreateSItem(iItemNo,1,kMixupItem.iRarityControlNo,kItem))
	{
		return E_MIXUP_CANNOT_MIXUP_ITEM;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), SPMOD_Modify_Count(kItem1,kItemPos1,-1,true)));
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), SPMOD_Modify_Count(kItem2,kItemPos2,-1,true)));
	kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(), SPMOD_Insert_Fixed(kItem, SItemPos(), true)));

	BM::Stream kPacket(PT_M_C_ANS_MIXUPITEM);
	kItem.WriteToPacket(kPacket);

	PgAction_ReqModifyItem kItemModifyAction(CIE_MixupItem, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqHometownEnter::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kErr = Process(pUser);
	if(S_OK == kErr)
	{
		return  true;
	}

	BM::Stream kPacket(PT_M_C_ANS_HOMETOWN_ENTER);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqHometownEnter::Process(CUnit* pUser)
{
	int iGroundNo = 0;
	int iPortalNo = 0;
	m_kPacket.Pop(iGroundNo);
	m_kPacket.Pop(iPortalNo);

	CONT_DEFMAP const * pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	if(!pkContDefMap)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	CONT_DEFMAP::const_iterator iter = pkContDefMap->find(iGroundNo);
	if((iter == pkContDefMap->end()) || (0 == (*iter).second.sHometownNo))
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	SReqMapMove_MT kRMM(MMET_None);

	if(!g_kProcessCfg.IsPublicChannel())
	{
		kRMM.cType = MMET_GoToPublicGround;
	}

	kRMM.kTargetKey.GroundNo(iGroundNo);
	kRMM.nTargetPortal = iPortalNo;


	PgReqMapMove kMapMove( m_pkGround, kRMM, NULL );

	if ( kMapMove.Add( pkPlayer ) )
	{
		kMapMove.DoAction();
	}
	return S_OK;
}

bool PgAction_ReqHomeEquipItem::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser || NULL == pTarget)
	{
		return false;
	}

	HRESULT kErr = Process(pUser,pTarget);

	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_HOME_EQUIP);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqHomeEquipItem::Process(CUnit* pUser,CUnit * pTarget)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return E_MYHOME_NOT_FOUND;
	}

	PgMyHome * pkMyHome = dynamic_cast<PgMyHome*>(pTarget);
	if(NULL == pkMyHome)
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(pkMyHome->OwnerGuid() != pUser->GetID() || (MAS_NOT_BIDDING != pkMyHome->GetAbil(AT_MYHOME_STATE)))// ������ �ƴϰų� ��� �Ǵ� ���� ���� �����̸� ������ ��ġ�� ����
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	SItemPos	kItemPos;
	POINT3		kLocalPos;
	short		siDir;
	BM::GUID	kParentGuid;
	BYTE		bLinkIdx = 0;

	m_kPacket.Pop(kItemPos);
	m_kPacket.Pop(kLocalPos);
	m_kPacket.Pop(siDir);
	m_kPacket.Pop(kParentGuid);
	m_kPacket.Pop(bLinkIdx);

	SHomeItem kHomeItem;
	kHomeItem.Pos(kLocalPos);
	kHomeItem.Dir(siDir);
	kHomeItem.ParentGuid(kParentGuid);
	kHomeItem.LinkIdx(bLinkIdx);

	PgBase_Item kItem;
	if(S_OK != pUser->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkItem = kItemDefMgr.GetDef(kItem.ItemNo());
	if(NULL == pkItem)
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	const CONT_DEFMAP* pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	if(!pkContDefMap)
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	CONT_DEFMAP::const_iterator itor = pkContDefMap->find(pkPlayer->GroundKey().GroundNo());
	if (itor == pkContDefMap->end())
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	const TBL_DEF_MAP& rkDefMap = itor->second;

	if(GATTR_MYHOME == (rkDefMap.iAttr & GATTR_MYHOME))
	{
		if(false == pkItem->IsType(ITEM_TYPE_MYHOME_IN))
		{
			return E_MYHOME_IS_NOT_HOME_ITEM;
		}
	}
	else
	{
		if(false == pkItem->IsType(ITEM_TYPE_MYHOME_OUT))
		{
			return E_MYHOME_IS_NOT_HOME_ITEM;
		}
	}

	SItemPos kTargetPos;
	if(false == pTarget->GetInven()->GetFirstEmptyPos(IT_HOME,kTargetPos))
	{
		return E_MYHOME_HOME_INVEN_FULL;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	kOrder.push_back(SPMO(IMET_MODIFY_COUNT, pUser->GetID(),SPMOD_Modify_Count(kItem,kItemPos,0,true)));
	kItem.Set(kHomeItem);
	kOrder.push_back(SPMO(IMET_INSERT_FIXED, pTarget->GetID(),SPMOD_Insert_Fixed(kItem,kTargetPos)));

	if(true == pkMyHome->IsInSide())
	{
		__int64 const i64EndTime = g_kEventView.GetLocalSecTime() + kItem.GetUseAbleTime();

		BM::DBTIMESTAMP_EX kEndTime;
		CGameTime::SecTime2DBTimeEx(i64EndTime, kEndTime);

		SHOMEADDR const & kHomeAddr = pkPlayer->HomeAddr();

		if(UICT_HOME_SIDEJOB_NPC == pkItem->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
		{
			eMyHomeSideJob const kSideJob = static_cast<eMyHomeSideJob>(pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1));
			eMyHomeSideJobRateType const kJobRateType = static_cast<eMyHomeSideJobRateType>(pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2));
			int const iJobRateValue = pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);
			kOrder.push_back(SPMO(IMET_SIDEJOB_INSERT, pTarget->GetID(), SMOD_MyHome_SideJob_Insert(kHomeAddr.StreetNo(), kHomeAddr.HouseNo(), kSideJob, kJobRateType, iJobRateValue, static_cast<BM::PgPackedTime>(kEndTime))));
		}

		CONT_HAVE_ITEM_NO kEquipItem;
		pTarget->GetInven()->GetItems(IT_HOME,kEquipItem);
		kEquipItem.insert(kItem.ItemNo());
		kOrder.push_back(SPMO(IMET_MYHOME_SET_EQUIP_COUNT, pTarget->GetID(), SMOD_MyHome_Sync_EquipItemCount(kHomeAddr.StreetNo(), kHomeAddr.HouseNo(), kEquipItem.size())));
	}

	BM::Stream kPacket(PT_M_C_ANS_HOME_EQUIP, kItem.ItemNo());
	kPacket.Push( pkMyHome->GetID() );
	kPacket.Push( kItem.Guid() );
	kPacket.Push( static_cast< int >(kItem.Count()) );
	PgAction_ReqModifyItem kItemModifyAction(CIE_Home_Equip, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqHomeUnEquipItem::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser || NULL == pTarget)
	{
		return false;
	}

	HRESULT kErr = Process(pUser,pTarget);

	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_HOME_UNEQUIP);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqHomeUnEquipItem::Process(CUnit* pUser,CUnit * pTarget)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(NULL == pkPlayer)
	{
		return E_MYHOME_NOT_FOUND;
	}

	SHOMEADDR const & kHomeAddr = pkPlayer->HomeAddr();

	PgMyHome * pkMyHome = dynamic_cast<PgMyHome*>(pTarget);
	if(NULL == pkMyHome)
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(pkMyHome->OwnerGuid() != pUser->GetID() || (MAS_NOT_BIDDING != pkMyHome->GetAbil(AT_MYHOME_STATE)))// ������ �ƴϰų� ��� �Ǵ� ���� ���� �����̸� ������ ��ġ�� ����
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pTarget->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkItem = kItemDefMgr.GetDef(kItem.ItemNo());
	if(NULL == pkItem)
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	SItemPos kTargetPos;
	if(false == pUser->GetInven()->GetFirstEmptyPos(static_cast<EInvType>(pkItem->PrimaryInvType()),kTargetPos))
	{
		return E_MYHOME_HOME_INVEN_FULL;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT, pTarget->GetID(),SPMOD_Modify_Count(kItem,kItemPos,0,true)));
	kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(),SPMOD_Insert_Fixed(kItem,kTargetPos)));

	if(true == pkMyHome->IsInSide())
	{
		if(UICT_HOME_SIDEJOB_NPC == pkItem->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
		{
			eMyHomeSideJob const kSideJob = static_cast<eMyHomeSideJob>(pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1));
			if(true == PgGroundUtil::IsMyHomeHasSideJob(pUser,pTarget,kSideJob))
			{
				kOrder.push_back(SPMO(IMET_SIDEJOB_REMOVE, pTarget->GetID(), SMOD_MyHome_SideJob_Remove(kHomeAddr.StreetNo(), kHomeAddr.HouseNo(), kSideJob)));
				kOrder.push_back(SPMO(IMET_SIDEJOB_COMPLETE, pTarget->GetID(), SMOD_MyHome_SideJob_Complete(pkPlayer->GetID(), kSideJob, kHomeAddr.StreetNo(), kHomeAddr.HouseNo())));
			}
		}

		ContHaveItemNoCount kEquipItem;
		pTarget->GetInven()->GetItems(IT_HOME,kEquipItem);
		ContHaveItemNoCount::iterator delete_iter = kEquipItem.find(kItem.ItemNo());
		if(delete_iter != kEquipItem.end())
		{
			--(*delete_iter).second;
			if(0 >= (*delete_iter).second)
			{
				kEquipItem.erase(delete_iter);
				kOrder.push_back(SPMO(IMET_MYHOME_SET_EQUIP_COUNT, pTarget->GetID(), SMOD_MyHome_Sync_EquipItemCount(kHomeAddr.StreetNo(), kHomeAddr.HouseNo(), kEquipItem.size())));
			}
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_HOME_UNEQUIP);
	PgAction_ReqModifyItem kItemModifyAction(CIE_Home_UnEquip, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqHomeUseItemEffect::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser || NULL == pTarget)
	{
		return false;
	}

	HRESULT kErr = Process(pUser,pTarget);
	BM::Stream kPacket(PT_M_C_ANS_HOME_USE_ITEM_EFFECT);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqHomeUseItemEffect::Process(CUnit* pUser,CUnit * pTarget)
{
	PgMyHome * pkMyHome = dynamic_cast<PgMyHome*>(pTarget);
	if(NULL == pkMyHome)
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(MAS_IS_BLOCK == pkMyHome->GetAbil(AT_MYHOME_STATE))
	{
		return E_MYHOME_IS_BLOCKED;
	}

	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pTarget->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	if(true == kItem.IsUseTimeOut())
	{
		return E_MYHOME_IS_TIMEOUT;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkItem = kItemDefMgr.GetDef(kItem.ItemNo());
	if(NULL == pkItem)
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	if(0 == pkItem->GetAbil(AT_VISITOR_USE_ITEM))
	{
		if(pUser->GetID() != pkMyHome->OwnerGuid())
		{
			return E_MYHOME_NOT_HAVE_AUTHORITY;
		}
	}

	int const iNewEffect1 = pkItem->GetAbil(AT_EFFECTNUM1);

	if(iNewEffect1)
	{
		SEffectCreateInfo kCreate;
		kCreate.eType = EFFECT_TYPE_NORMAL;
		kCreate.iEffectNum = iNewEffect1;
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
		pUser->AddEffect(kCreate);
		::CheckSkillFilter_Delete_Effect(pUser, pUser, iNewEffect1);
	}

	if(pUser->GetID() != pkMyHome->OwnerGuid())
	{
		PgAddAchievementValue kMA(AT_ACHIEVEMENT_ILLEGALVISITOR,1,m_kGndKey);
		kMA.DoAction(pUser,NULL);
	}

	return S_OK;
}

bool PgAction_ReqMyHomeAuctionReg::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_AUCTION_REG);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

__int64 const PgAction_ReqMyHomeAuctionReg::CalcAuctionTex(__int64 i64AuctionCost)
{
	return (i64AuctionCost*MATR_REG)/100;
}

HRESULT const PgAction_ReqMyHomeAuctionReg::Process(CUnit* pUser)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer || pkPlayer->HomeAddr().IsNull())
	{
		return E_MYHOME_NOT_FOUND;
	}

	CONT_DEFMYHOMEBUILDINGS const * pkCont = NULL;
	g_kTblDataMgr.GetContDef(pkCont);
	if(!pkCont)
	{
		return E_MYHOME_NOT_FOUND;
	}

	CONT_DEFMYHOMEBUILDINGS::const_iterator iter = pkCont->find(pkPlayer->HomeAddr().StreetNo());
	if(iter == pkCont->end())
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(0 == (*iter).second.iGrade)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	__int64 i64Cost = 0;
	int		iHour = 0;
	
	m_kPacket.Pop(i64Cost);
	m_kPacket.Pop(iHour);

	int const iGold = int(i64Cost / iGoldToBronze);
	if(iGold <= 0)
	{
		return E_MYHOME_COST_ONLY_GOLD;
	}

	else if(iGold < g_iMyHomeAuctionLimitGold )
	{//��� ��� ������ �ּҰ� ���� �۴ٸ� ������ ����.
		return E_MYHOME_INVALID_COST_LIMIT;
	}

	PgInventory * pkInv = pUser->GetInven();

	__int64 const i64Tex = CalcAuctionTex(i64Cost);

	if(i64Tex > pkInv->Money())
	{
		return E_MYHOME_NOT_ENOUGH_MONEY;
	}

	switch(iHour)
	{
	case MAT_6:
	case MAT_12:
	case MAT_18:
	case MAT_1D:
	case MAT_6D:
		{
		}break;
	default:
		{
			return E_MYHOME_INVALID_AUCTION_TIME;
		}break;
	}

	BM::Stream kPacket(PT_M_I_REQ_MYHOME_AUCTION_REG);
	kPacket.Push(pUser->GetID());
	kPacket.Push(pkPlayer->HomeAddr());
	kPacket.Push(i64Cost);
	kPacket.Push(i64Tex);
	kPacket.Push(iHour);

	SendToItem(m_kGndKey,kPacket);
	return S_OK;
}

bool PgAction_ReqMyHomeVisitFlag::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser,pTarget);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_HOME_VISITFLAG_MODIFY);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqMyHomeVisitFlag::Process(CUnit* pUser,CUnit* pTarget)
{
	PgMyHome * pkHome = dynamic_cast<PgMyHome*>(pTarget);
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer || pkPlayer->HomeAddr().IsNull() || !pkHome || pkHome->OwnerGuid() != pkPlayer->GetID())
	{
		return E_MYHOME_NOT_FOUND;
	}

	BYTE bVisitFlag = 0;
	m_kPacket.Pop(bVisitFlag);

	CONT_PLAYER_MODIFY_ORDER kOrder;

	SHOMEADDR const & kAddr = pkPlayer->HomeAddr();

	kOrder.push_back(SPMO(IMET_MYHOME_VISITFLAG, pkPlayer->GetID(),SMOD_MyHome_VisitFlag(kAddr.StreetNo(),kAddr.HouseNo(),bVisitFlag)));
	kOrder.push_back(SPMO(IMET_MYHOME_VISITFLAG, pkHome->GetID(),SMOD_MyHome_VisitFlag(kAddr.StreetNo(),kAddr.HouseNo(),bVisitFlag)));

	BM::Stream kPacket(PT_M_C_ANS_HOME_VISITFLAG_MODIFY);
	kPacket.Push(pUser->GetID());
	PgAction_ReqModifyItem kItemModifyAction(CIE_Home_Modify, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqMyHomeBidding::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_BIDDING);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqMyHomeBidding::Process(CUnit* pUser)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(!pkPlayer->HomeAddr().IsNull())
	{
		return E_MYHOME_ALREADY_HAVE_HOME;
	}

	short	siStreetNo = 0;
	int		iHouseNo = 0;
	__int64 i64Cost = 0;

	m_kPacket.Pop(siStreetNo);
	m_kPacket.Pop(iHouseNo);
	m_kPacket.Pop(i64Cost);

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_ADD_MONEY, pUser->GetID(),SPMOD_Add_Money(-i64Cost)));
	kOrder.push_back(SPMO(IMET_MYHOME_BIDDING, pUser->GetID(),SMOD_MyHome_Bidding(siStreetNo,iHouseNo,i64Cost)));

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_BIDDING);
	kPacket.Push(siStreetNo);
	kPacket.Push(iHouseNo);
	kPacket.Push(i64Cost);
	PgAction_ReqModifyItem kItemModifyAction(CIE_Home_Bidding, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}


bool PgAction_ReqMyHomeAuctionUnreg::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_AUCTION_UNREG);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqMyHomeAuctionUnreg::Process(CUnit* pUser)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer || pkPlayer->HomeAddr().IsNull())
	{
		return E_MYHOME_NOT_FOUND;
	}

	BM::Stream kPacket(PT_M_I_REQ_MYHOME_AUCTION_UNREG);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(pkPlayer->HomeAddr());
	SendToItem(m_kGndKey,kPacket);
	return S_OK;
}

bool PgAction_ReqMyHomePostInvitationCard::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_POST_INVITATION_CARD);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqMyHomePostInvitationCard::Process(CUnit* pUser)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer || pkPlayer->HomeAddr().IsNull())
	{
		return E_MYHOME_NOT_FOUND;
	}

	SHOMEADDR const & kAddr = pkPlayer->HomeAddr();

	BM::Stream kPacket(PT_M_I_REQ_MYHOME_POST_INVITATION_CARD);
	kPacket.Push(pUser->GetID());
	kPacket.Push(kAddr.StreetNo());
	kPacket.Push(kAddr.HouseNo());
	kPacket.Push(m_kPacket);
	SendToItem(m_kGndKey,kPacket);
	return S_OK;
}

bool PgAction_ReqHomeItemModify::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser || NULL == pTarget)
	{
		return false;
	}

	HRESULT kErr = Process(pUser,pTarget);

	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_HOME_ITEM_MODIFY);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqHomeItemModify::Process(CUnit* pUser,CUnit * pTarget)
{
	PgMyHome * pkMyHome = dynamic_cast<PgMyHome*>(pTarget);
	if(NULL == pkMyHome)
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(pkMyHome->OwnerGuid() != pUser->GetID() || (MAS_NOT_BIDDING != pkMyHome->GetAbil(AT_MYHOME_STATE)))// ������ �ƴϰų� ��� �Ǵ� ���� ���� �����̸� ������ ��ġ�� ����
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pTarget->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_MYHOME_NOT_FOUND_ITEM;
	}

	POINT3 kPos;
	short kDir = 0;
	BM::GUID kParentGuid;
	BYTE bLinkIdx = 0;

	m_kPacket.Pop(kPos);
	m_kPacket.Pop(kDir);
	m_kPacket.Pop(kParentGuid);
	m_kPacket.Pop(bLinkIdx);

	SHomeItem kHomeItem;
	
	kHomeItem.Pos(kPos);
	kHomeItem.Dir(kDir);
	kHomeItem.ParentGuid(kParentGuid);
	kHomeItem.LinkIdx(bLinkIdx);

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_MODIFY_EXTEND_DATA,pTarget->GetID(),SPMOD_ExtendData(kItem, kItemPos,kHomeItem)));

	BM::Stream kPacket(PT_M_C_ANS_HOME_ITEM_MODIFY);
	PgAction_ReqModifyItem kItemModifyAction(CIE_Home_Item_Modify, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_ReqHomeExit::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser,pTarget);
	BM::Stream kPacket(PT_M_C_ANS_MYHOME_EXIT);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return true;
}

HRESULT const PgAction_ReqHomeExit::Process(CUnit* pUser,CUnit * pTarget)
{
	PgMyHome * pkHome = dynamic_cast<PgMyHome *>(pTarget);
	if(!pkHome)
	{
		return E_MYHOME_NOT_FOUND;
	}

	int iGroundNo = 0;
	POINT3 kPos;

	BM::Stream kPacket(PT_C_M_REQ_MYHOME_CHAT_EXIT);
	kPacket.Push(pUser->GetID());
	SendToMyhomeMgr(kPacket);

	bool bIsReturnRecentPos = false;
	m_kPacket.Pop(bIsReturnRecentPos);

	if(true == bIsReturnRecentPos)
	{
		PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
		if(!pkPlayer)
		{
			return E_MYHOME_NOT_FOUND;
		}

		iGroundNo = pkPlayer->GetRecentMapNo(GATTR_DEFAULT);
		kPos = pkPlayer->GetRecentPos(GATTR_DEFAULT);
	}
	else
	{
		CONT_DEFMYHOMEBUILDINGS const * pkCont = NULL;
		g_kTblDataMgr.GetContDef(pkCont);
		if(!pkCont)
		{
			return E_MYHOME_NOT_FOUND;
		}

		CONT_DEFMYHOMEBUILDINGS::const_iterator iter = pkCont->find(pkHome->HomeAddr().StreetNo());
		if(iter == pkCont->end())
		{
			return E_MYHOME_NOT_FOUND;
		}

		iGroundNo = (*iter).second.iGroundNo;
	}

	SReqMapMove_MT kRMM(MMET_None);
	kRMM.kTargetKey.GroundNo(iGroundNo);

	if(POINT3::NullData() == kPos)
	{
		kRMM.nTargetPortal = 1;
	}
	else
	{
		kRMM.nTargetPortal = 0;
		kRMM.pt3TargetPos = kPos;
	}

	PgReqMapMove kMapMove( m_pkGround, kRMM, NULL );

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pUser);
	if(pkPlayer)
	{
		if(true == kMapMove.Add( pkPlayer ))
		{
			kMapMove.DoAction();
		}
	}

	return S_OK;
}

bool PgAction_ReqHomeMapMove::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	HRESULT kErr = Process(pUser,pTarget);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_HOME_ANS_MAPMOVE);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqHomeMapMove::Process(CUnit* pUser,CUnit * pTarget)
{
	int iGroundNo = 0;

	m_kPacket.Pop(iGroundNo);

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pUser);
	if(!pkPlayer)
	{
		return E_MYHOME_NOT_OPENED_MAP;
	}

	if(GATTR_FLAG_HOMETOWN != (GATTR_FLAG_HOMETOWN & m_pkGround->GetAttr()))
	{
		return E_MYHOME_NOT_HOMETOWN;
	}

	__int64 i64Cost = 0;

	if(true == pkPlayer->HomeAddr().IsNull())
	{
		i64Cost = PgHometownPortalUtil::UsePortalCost(pUser->GetAbil(AT_LEVEL));

		if(i64Cost > pUser->GetInven()->Money())
		{
			return E_MYHOME_NOT_ENOUGH_MONEY;
		}
	}

	const CONT_DEFMAP* pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	if(!pkContDefMap)
	{
		return E_MYHOME_NOT_FOUND_MAP;
	}

	//���� �ִ� ������ ����Ȩ�̸�, ����Ȩ �̵� ���
	CONT_DEFMAP::const_iterator My_itor = pkContDefMap->find(m_kGndKey.GroundNo());
	if (My_itor == pkContDefMap->end())
	{
		return E_MYHOME_NOT_FOUND_MAP;
	}
	const TBL_DEF_MAP& rkDefMyMap = My_itor->second;

	if( 0 == (rkDefMyMap.iAttr & GATTR_FLAG_MYHOME) )
	{

		CONT_DEFMAP::const_iterator itor = pkContDefMap->find(iGroundNo);
		if (itor == pkContDefMap->end())
		{
			return E_MYHOME_NOT_FOUND_MAP;
		}

		const TBL_DEF_MAP& rkDefMap = itor->second;
		if( 0 != (rkDefMap.iAttr & GATTR_FLAG_CANT_WARP) )
		{
			return E_MYHOME_CANT_MOVE_MAP;
		}
	}

	if(S_OK != pkPlayer->IsOpenWorldMap(iGroundNo))
	{
		return E_MYHOME_NOT_OPENED_MAP;
	}

	if( true == pkPlayer->IsItemEffect(SAFE_FOAM_EFFECTNO) )
	{		
		return E_CANNOT_STATE_MAPMOVE;
	}

	SReqMapMove_MT kRMM(MMET_None);
	kRMM.kTargetKey.GroundNo(iGroundNo);
	kRMM.nTargetPortal = 1;

	PgReqMapMove kMapMove(m_pkGround, kRMM, NULL );
	if(kMapMove.Add( pkPlayer ))
	{
		if(true == pkPlayer->HomeAddr().IsNull())
		{
			kMapMove.AddModifyOrder(SPMO(IMET_ADD_MONEY, pkPlayer->GetID(),SPMOD_Add_Money(-i64Cost)));
		}
		if(!kMapMove.DoAction())
		{
			return E_CANNOT_STATE_MAPMOVE;
		}
	}

	BM::Stream kPacket(PT_M_C_HOME_ANS_MAPMOVE);
	kPacket.Push(S_OK);
	pUser->Send(kPacket);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_PartyBufRefresh
//////////////////////////////////////////////////////////////////////////////
PgAction_PartyBufRefresh::PgAction_PartyBufRefresh(PgGround *pkGround, SGroundKey const& rkGndKey, PgLocalPartyMgr &kLocalPartyMgr, int const iPrevClass)
	:m_pkGround(pkGround), m_kGndKey(rkGndKey), m_kLocalPartyMgr(kLocalPartyMgr), m_kPrevClass(iPrevClass)
{}
bool PgAction_PartyBufRefresh::DoAction(CUnit* pkUser, CUnit* pkTarget)
{
	if(NULL == pkUser)
	{
		return false;
	}
	
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkUser);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return not find PgPlayer"));
		return false;
	}

	if(BM::GUID::NullData() == pkPlayer->PartyGuid())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return not find PartyGuid"));
		return false;
	}

	VEC_GUID kVec;
	bool bFindParty = m_kLocalPartyMgr.GetPartyMember(pkPlayer->PartyGuid(), kVec);	
	if(!bFindParty || (0 == kVec.size()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_FOUND_MEMBER"));
		return false;
	}

	SActArg kArg;
	PgGroundUtil::SetActArgGround(kArg, m_pkGround);

	GET_DEF(PgClassDefMgr, kClassDefMgr);

	if( 0 < kVec.size() )
	{
		bool bRet = false;
		VEC_GUID::iterator user_itr;
		for( user_itr=kVec.begin(); user_itr!=kVec.end(); ++user_itr )
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(m_pkGround->GetUnit( (*user_itr) ));
			if( pkPlayer ) // ������ ���� �ִ� ��Ƽ���� �ƴϱ� ������ �н�.
			{
				// �� ����� ���� ������ �ߴ�. ���� ������ ������ �����ؾ� �Ѵ�.
				int iClassMember = iClassMember = PrevClass();
				int const iLevelMember = pkPlayer->GetAbil(AT_LEVEL);
				int const iPerMember = kClassDefMgr.GetAbil(SClassKey(iClassMember, iLevelMember), AT_CLASS_PARTY_BUFF_01);

				int iBuffLevel = 0;

				for(int i=0; i<static_cast<int>(kVec.size()); i++)
				{
					iBuffLevel = i + 1;
					int const iEffectNo = iPerMember+iBuffLevel-1;
					if( 0 < iEffectNo )
					{
						pkPlayer->DeleteEffect(iEffectNo);
						bRet = true;
					}
				}
			}
		}
		if( true == bRet )
		{
			BM::Stream kPacket( PT_M_T_REQ_PARTY_BUFF );
			kPacket.Push( pkUser->GetID() );
			kPacket.Push( GndKey() );
			SendToCenter(kPacket);
		}
	}

	return true;
}

bool PgAction_ReqUseGambleMachine::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(!pUser)
	{
		return false;
	}

	HRESULT kErr = Process(pUser,pTarget);

	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqUseGambleMachine::Process(CUnit* pUser,CUnit * pTarget)
{
	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pUser->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_GAMBLEMACHINE_NOT_FOUND_COIN;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(NULL == pkDef)
	{
		return E_GAMBLEMACHINE_NOT_FOUND_COIN;
	}

	int const iCustomType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);

	if(UICT_GAMBLE_COIN != iCustomType)
	{
		return E_GAMBLEMACHINE_NOT_FOUND_COIN;
	}

	bool bBroadcast = false;
	CONT_GAMBLEMACHINERESULT kContResult;

	PgPlayer const* const pkPlayer = dynamic_cast<PgPlayer const* const>(pTarget);
	if(pkPlayer == NULL || pTarget->UnitType() != UT_PLAYER)
	{
		return E_FAIL;
	}

	HRESULT kErr = g_kGambleMachine.GetGambleResult(static_cast<eGambleCoinGrade>(iCustomValue1), kContResult, pkPlayer->UnitRace(), bBroadcast);
	if(S_OK != kErr)
	{
		return kErr;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE,pUser->GetID(),SPMOD_Modify_Count(kItem,kItemPos,-1)));

	BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE);
	PU::TWriteArray_M(kPacket, kContResult);
	kPacket.Push(bBroadcast);

	PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, pTarget);
	return S_OK;
}

bool PgAction_ReqUseGambleMachine_CashShop::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(!pUser)
	{
		return false;
	}

	HRESULT kErr = Process(pUser, pTarget);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE_READY);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT const PgAction_ReqUseGambleMachine_CashShop::Process(CUnit* pUser,CUnit * pTarget)
{
	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	PgBase_Item kItem;
	if(S_OK != pUser->GetInven()->GetItem(kItemPos,kItem))
	{
		return E_GAMBLEMACHINE_NOT_FOUND_COIN;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(NULL == pkDef)
	{
		return E_GAMBLEMACHINE_NOT_FOUND_COIN;
	}

	int const iCustomType = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	int const iCustomValue1 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	int const iCustomValue2 = pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);

	if(UICT_GAMBLE_COIN != iCustomType || GCG_CASHSHOP != iCustomValue1)
	{
		return E_GAMBLEMACHINE_NOT_FOUND_COIN;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE,pUser->GetID(),SPMOD_Modify_Count(kItem,kItemPos,-1)));
	kOrder.push_back(SPMO(IMET_GAMBLE_INSERT,pUser->GetID(),SPMOD_Gamble_Insert(iCustomValue2 + 1)));

	BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE_READY);
	kPacket.Push(iCustomValue2);
	PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine_Shop, m_kGndKey, kOrder);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_OpenEventItemReward::DoAction(CUnit* pUser, CUnit* pTarget)
{
	TABLE_LOCK(CONT_EVENT_ITEM_REWARD) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_EVENT_ITEM_REWARD const * pkCont = kObjLock.Get();
	if(!pkCont)
	{
		return false;
	}

	CONT_EVENT_ITEM_REWARD::const_iterator iter = pkCont->find(m_kItemNo);
	if(iter == pkCont->end())
	{
		return false;
	}

	CONT_EVENT_ITEM_REWARD::mapped_type const & ktbl = (*iter).second;

	CONT_PLAYER_MODIFY_ORDER kOrder;

	CONT_EVENTITEM_REWARD_RESULT kResult;

	for(CONT_EVENT_ITEM_REWARD_ELEMENT::const_iterator item_iter = ktbl.kContAll.begin();item_iter != ktbl.kContAll.end();++item_iter)
	{
		CreateItemProcess((*item_iter), kResult);
	}

	int iRewardItemNo = 0;

	if(false == ktbl.kContRandom.empty())
	{
		int const iRndRate = BM::Rand_Index(ktbl.iTotalRate);
		int iResultRate = 0;

		for(CONT_EVENT_ITEM_REWARD_ELEMENT::const_iterator item_iter = ktbl.kContRandom.begin();item_iter != ktbl.kContRandom.end();++item_iter)
		{
			iResultRate += (*item_iter).iRate;
			if(iRndRate > iResultRate)
			{
				continue;
			}

			CreateItemProcess((*item_iter), kResult);

			iRewardItemNo = (*item_iter).iItemNo;

			if(true == (*item_iter).bBroadcast)
			{
				BM::Stream kPacket(PT_M_C_NOTI_EVENT_ITEM_REWARD);
				kPacket.Push(pUser->Name());
				kPacket.Push(m_kItemNo);
				kPacket.Push(iRewardItemNo);
				SendToCenter(kPacket);
			}
			break;
		}
	}

	for(CONT_EVENTITEM_REWARD_RESULT::const_iterator result_iter = kResult.begin();result_iter != kResult.end();++result_iter)
	{
		kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(), SPMOD_Insert_Fixed((*result_iter), SItemPos(), true)));
	}

	if(0 < iRewardItemNo)
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const * pkDef = kItemDefMgr.GetDef(m_kItemNo);
		if(pkDef)
		{
			if(UICT_EVENT_ITEM_REWARD_GAMBLE == pkDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
			{
				BM::Stream kPacket(PT_M_C_ANS_EVENT_ITEM_REWARD);
				kPacket.Push(m_kItemNo);
				kPacket.Push(iRewardItemNo);
				PgAction_ReqModifyItem kItemModifyAction(CIE_SendEventItemReward, m_kGndKey, kOrder, kPacket);
				return kItemModifyAction.DoAction(pUser, pTarget);
			}
		}
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_SendEventItemReward, m_kGndKey, kOrder);
	return kItemModifyAction.DoAction(pUser, pTarget);
}

bool PgAction_OpenEventItemReward::CreateItemProcess(SEVENT_ITEM_REWARD_ELEMENT const & kElement, CONT_EVENTITEM_REWARD_RESULT & kResult)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkDef = kItemDefMgr.GetDef(kElement.iItemNo);

	if(NULL == pkDef)
	{
		return false;
	}

	if(true == pkDef->IsAmountItem())
	{
		PgBase_Item kItem;
		if(true == CreateItemProcess_Sub(kElement, kElement.siCount, kItem))
		{
			kResult.push_back(kItem);
		}
	}
	else
	{
		for(short i = 0;i < kElement.siCount;++i)
		{
			PgBase_Item kItem;
			if(true == CreateItemProcess_Sub(kElement, 1, kItem))
			{
				kResult.push_back(kItem);
			}
		}
	}
	return true;
}

bool PgAction_OpenEventItemReward::CreateItemProcess_Sub(SEVENT_ITEM_REWARD_ELEMENT const & kElement, short const siCount, PgBase_Item & kItem)
{
	if(S_OK != CreateSItem(kElement.iItemNo, siCount, kElement.bRarity, kItem))
	{
		return false;
	}

	if(0 < kElement.siUseTime)
	{
		kItem.SetUseTime(kElement.bTimeType, kElement.siUseTime);
	}

	SEnchantInfo kCopyEnchant = kItem.EnchantInfo();
	kCopyEnchant.PlusType(kElement.bEnchantType);
	kCopyEnchant.PlusLv(kElement.bEnchantLv);
	kItem.EnchantInfo(kCopyEnchant);

	GET_DEF(PgItemOptionMgr, kItemOptionMgr);
	kItemOptionMgr.GenerateOption_Sub(kItem);
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		��������/����
//////////////////////////////////////////////////////////////////////////////
bool PgAction_ReqOpenTreasureChest::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(!pUser || !pTarget)
	{
		return false;
	}

	TABLE_LOCK(CONT_TREASURE_CHEST) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	const CONT_TREASURE_CHEST* pkCont = kObjLock.Get();
	if(!pkCont)
	{ //���̺� ���� �� �����
		return false;
	}

	PgInventory* pInv = pUser->GetInven();
	PgBase_Item kChestItem, kKeyItem;
	if( E_FAIL == pInv->GetItem(m_kChestPos, kChestItem) )
	{ //�κ����� ���� �� ã�Ҵ�
		SendErrorPacket(pUser, E_TREASURE_CHEST_NOT_FOUND_CHEST_INV);
		return false;
	}
	if( E_FAIL == pInv->GetItem(m_kKeyPos, kKeyItem) )
	{ //�κ����� Ű �� ã�Ҵ�
		SendErrorPacket(pUser, E_TREASURE_CHEST_NOT_FOUND_KEY_INV);
		return false;
	}


	CONT_PLAYER_MODIFY_ORDER kOrder;

	CONT_EVENTITEM_REWARD_RESULT kResult;

	CONT_TREASURE_CHEST::const_iterator iterChest = pkCont->find(kChestItem.ItemNo());
	if(iterChest == pkCont->end()) //DB ��ϵ��� ���� ����?
	{
		SendErrorPacket(pUser, E_TREASURE_CHEST_NOT_FOUND_CHEST);
		return false;
	}

	CONT_TREASURE_CHEST_KEY_GROUP::const_iterator iterKey = iterChest->second.kCont.find(kKeyItem.ItemNo());
	if(iterKey == iterChest->second.kCont.end()) //���ڿ� ���� �ʴ� ����
	{
		SendErrorPacket(pUser, E_TREASURE_CHEST_UNMATCH);
		return false;
	}

	int iRewardItemNo = 0;
	int const iRndRate = BM::Rand_Index(iterKey->second.iTotalRate);
	int iResultRate = 0;

	for(CONST_ELEM_ITER iterItem = iterKey->second.kContElem.begin();
		iterItem != iterKey->second.kContElem.end(); ++iterItem)
	{
		iResultRate += (*iterItem).iRate;
		if(iRndRate > iResultRate)
		{
			continue;
		}
		CreateItemProcess((*iterItem), kResult);
		iRewardItemNo = (*iterItem).iItemNo;

		if(true == (*iterItem).bBroadcast)
		{
			BM::Stream kPacket(PT_M_C_NOTI_EVENT_ITEM_REWARD);
			kPacket.Push(pUser->Name());
			kPacket.Push(kChestItem.ItemNo());
			kPacket.Push(iRewardItemNo);
			SendToCenter(kPacket);
		}
		break;
	}
	//kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(), SPMOD_Insert_Fixed((*result_iter), SItemPos(), true)));

	SPMOD_Modify_Count kDelChest(kChestItem, m_kChestPos, -1);
	SPMOD_Modify_Count kDelKey(kKeyItem, m_kKeyPos, -1);
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT , pUser->GetID(), kDelChest));
	kOrder.push_back(SPMO(IMET_MODIFY_COUNT , pUser->GetID(), kDelKey));
	/*for(CONT_EVENTITEM_REWARD_RESULT::const_iterator result_iter = kResult.begin();result_iter != kResult.end();++result_iter)
	{
		kOrder.push_back(SPMO(IMET_INSERT_FIXED, pUser->GetID(), SPMOD_Insert_Fixed((*result_iter), SItemPos(), true)));		
	}*/
	

	if(0 < iRewardItemNo)
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const * pkDefChest = kItemDefMgr.GetDef(kChestItem.ItemNo());
		CItemDef const * pkDefKey = kItemDefMgr.GetDef(kKeyItem.ItemNo());
		if(pkDefChest && UICT_TREASURE_CHEST == pkDefChest->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) &&
			pkDefKey && UICT_TREASURE_CHEST_KEY == pkDefKey->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) )
		{
			BM::Stream kPacket;

			PU::TWriteArray_M(kPacket,kResult);//--Push

			/*kPacket.Push( PT_M_C_ANS_TREASURE_CHEST );
			kPacket.Push(kChestItem.ItemNo());
			kPacket.Push(kKeyItem.ItemNo());
			kPacket.Push(iRewardItemNo);*/

			kPacket.Push(iRewardItemNo);

			PgAction_ReqModifyItem kItemModifyAction(CIE_OpenTreasureChestReward, m_kGndKey, kOrder, kPacket);
			return kItemModifyAction.DoAction(pUser, pTarget);
		}
		else
		{
			//..........
			return false;
		}
	}
	return false;

	//PgAction_ReqModifyItem kItemModifyAction(CIE_OpenTreasureChestReward, m_kGndKey, kOrder);
	//return kItemModifyAction.DoAction(pUser, pTarget);
}

void PgAction_ReqOpenTreasureChest::SendErrorPacket(CUnit* pUser, int iErrorNo)
{
	BM::Stream kPacket(PT_M_C_ANS_TREASURE_CHEST);
	kPacket.Push(iErrorNo);
	pUser->Send(kPacket);
}

bool PgAction_ReqOpenTreasureChest::CreateItemProcess(SEVENT_ITEM_REWARD_ELEMENT const & kElement, CONT_EVENTITEM_REWARD_RESULT & kResult)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkDef = kItemDefMgr.GetDef(kElement.iItemNo);

	if(NULL == pkDef)
	{
		return false;
	}

	if(true == pkDef->IsAmountItem())
	{
		PgBase_Item kItem;
		if(true == CreateItemProcess_Sub(kElement, kElement.siCount, kItem))
		{
			kResult.push_back(kItem);
		}
	}
	else
	{
		for(short i = 0;i < kElement.siCount;++i)
		{
			PgBase_Item kItem;
			if(true == CreateItemProcess_Sub(kElement, 1, kItem))
			{
				kResult.push_back(kItem);
			}
		}
	}
	return true;
}

bool PgAction_ReqOpenTreasureChest::CreateItemProcess_Sub(SEVENT_ITEM_REWARD_ELEMENT const & kElement, short const siCount, PgBase_Item & kItem)
{
	if(S_OK != CreateSItem(kElement.iItemNo, siCount, kElement.bRarity, kItem))
	{
		return false;
	}

	if(0 < kElement.siUseTime)
	{
		kItem.SetUseTime(kElement.bTimeType, kElement.siUseTime);
	}

	SEnchantInfo kCopyEnchant = kItem.EnchantInfo();
	kCopyEnchant.PlusType(kElement.bEnchantType);
	kCopyEnchant.PlusLv(kElement.bEnchantLv);
	kItem.EnchantInfo(kCopyEnchant);

	GET_DEF(PgItemOptionMgr, kItemOptionMgr);
	kItemOptionMgr.GenerateOption_Sub(kItem);
	return true;
}



////
PgAction_MonsterEnchantProbCalc::PgAction_MonsterEnchantProbCalc(int const iMonEnchantProbGroupNo)
	: m_iMonEnchantProbGroupNo(iMonEnchantProbGroupNo)
{
	g_kTblDataMgr.GetContDef(m_pkDefMonEnchantGradeProb);
}
PgAction_MonsterEnchantProbCalc::~PgAction_MonsterEnchantProbCalc()
{
}

int PgAction_MonsterEnchantProbCalc::Get()
{
	int iMonsterEnchantGradeNo = 0;
	if( m_iMonEnchantProbGroupNo )
	{
		CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::const_iterator find_iter = m_pkDefMonEnchantGradeProb->find(m_iMonEnchantProbGroupNo);
		if( m_pkDefMonEnchantGradeProb->end() != find_iter )
		{
			CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::mapped_type const& rkProbTable = (*find_iter).second;

			int const iRandRet = BM::Rand_Range(rkProbTable.iTotalProbability);
			int iAccumProb = 0;
			CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY::const_iterator prob_iter = rkProbTable.kContProbability.begin();
			while( rkProbTable.kContProbability.end() != prob_iter )
			{
				CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY::value_type const& rkProb = (*prob_iter);
				iAccumProb += rkProb.iProbability;
				if( iRandRet <= iAccumProb )
				{
					iMonsterEnchantGradeNo = rkProb.iMonsterEnchantGrade;
					break;
				}
				++prob_iter;
			}
		}
	}
	return iMonsterEnchantGradeNo;
}


//
bool PgAction_Myhome_Buy::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser, pTarget);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_BUY);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT PgAction_Myhome_Buy::Process(CUnit* pUser, CUnit* pTarget)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	if(false == pkPlayer->HomeAddr().IsNull())
	{
		return E_MYHOME_ALREADY_HAVE;
	}

	CONT_DEFMYHOMEBUILDINGS const * pkCont = NULL;
	g_kTblDataMgr.GetContDef(pkCont);
	if(!pkCont)
	{
		return E_MYHOME_NOT_FOUND;
	}

	short sBuildingNo = 0;
	m_kPacket.Pop(sBuildingNo);
	
	CONT_DEFMYHOMEBUILDINGS::const_iterator iter = pkCont->find(sBuildingNo);
	if(iter == pkCont->end())
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(0 < (*iter).second.iGrade)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	int iCost = DEFAULT_MYHOME_APT_COST;
	g_kVariableContainer.Get(EVar_Kind_MyHome, EVar_MyHome_Apt_Gold, iCost);

	if(pkPlayer->GetInven()->Money() < iCost)
	{
		return E_MYHOME_NOT_ENOUGH_MONEY;
	}

	int iLevelLimit = DEFAULT_MYHOME_APT_LEVELLIMIT;
	g_kVariableContainer.Get(EVar_Kind_MyHome, EVar_MyHome_Apt_LevelLimit, iLevelLimit);

	if(pkPlayer->GetAbil(AT_LEVEL) < iLevelLimit)
	{
		return E_MYHOME_INVALID_LEVEL_LIMIT;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_ADD_MONEY, pUser->GetID(), SPMOD_Add_Money(-iCost)));
	kOrder.push_back(SPMO(IMET_ADD_MYHOME, pUser->GetID(), SMOD_Add_MyHome(sBuildingNo, iCost)));

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_BUY);
	PgAction_ReqModifyItem kItemModifyAction(CIE_Buy_MyHome, m_kGndKey, kOrder, kPacket);
	kItemModifyAction.DoAction(pkPlayer, NULL);

	return S_OK;
}

bool PgAction_ReqUseGambleMachine_Mixup::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(!pUser)
	{
		return false;
	}

	HRESULT kErr = Process(pUser, pTarget);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE_MIXUP_READY);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

int const MAX_MIXUP_RULET_COUNT = 5;

HRESULT const PgAction_ReqUseGambleMachine_Mixup::Process(CUnit* pUser,CUnit * pTarget)
{
	CONT_ITEMPOS kContItemPos;
	PU::TLoadArray_A(m_kPacket, kContItemPos);

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_PLAYER_MODIFY_ORDER kOrder;

	int iMixPoint = 0;

	if(MAX_GAMBLEMACHINE_MIXUP_NUM != kContItemPos.size())
	{
		return E_GAMBLEMACHINE_INVALID_MIXPOINT;
	}

	for(CONT_ITEMPOS::const_iterator iter = kContItemPos.begin();iter != kContItemPos.end();++iter)
	{
		SItemPos const & kItemPos = (*iter);

		PgBase_Item kItem;
		if(S_OK != pUser->GetInven()->GetItem(kItemPos, kItem))
		{
			return E_GAMBLEMACHINE_NOT_FOUND_RESOURCE_ITEM;
		}

		if( LOCAL_MGR::NC_KOREA == g_kLocal.ServiceRegion()
			&& kItem.IsUseTimeOut() 
			)
		{
			return E_GAMBLEMACHINE_NOT_USE_OVERDATE_ITEM;
		}
		CItemDef const * pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if(NULL == pkDef)
		{
			return E_GAMBLEMACHINE_NOT_FOUND_RESOURCE_ITEM;
		}

		int const iGrade = pkDef->GetAbil(AT_COSTUME_GRADE);

		if(0 == iGrade)
		{
			return E_GAMBLEMACHINE_NEED_ITEM_GRADE;
		}

		iMixPoint += g_kGambleMachine.GetMixupPoint(iGrade);
	
		kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE,pUser->GetID(),SPMOD_Modify_Count(kItem,kItemPos,0,true)));
	}

	int iUseMixPoint = 0;
	if(true != g_kGambleMachine.MakeEnablePoint(iMixPoint, iUseMixPoint))
	{
		return E_GAMBLEMACHINE_INVALID_MIXPOINT;
	}
	
	kOrder.push_back(SPMO(IMET_GAMBLE_MIXUP_INSERT,pUser->GetID(),SPMOD_Gamble_Insert(MAX_MIXUP_RULET_COUNT+1,iUseMixPoint)));

	BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE_MIXUP_READY);
	kPacket.Push(MAX_MIXUP_RULET_COUNT);
	PgAction_ReqModifyItem kItemModifyAction(CIE_GambleMachine_Mixup, m_kGndKey, kOrder);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

bool PgAction_Myhome_Sell::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT kErr = Process(pUser, pTarget);
	if(S_OK == kErr)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_SELL);
	kPacket.Push(kErr);
	pUser->Send(kPacket);
	return false;
}

HRESULT PgAction_Myhome_Sell::Process(CUnit* pUser, CUnit* pTarget)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	SHOMEADDR const & kAddr = pkPlayer->HomeAddr();

	if(true == kAddr.IsNull())
	{
		return E_MYHOME_NOT_FOUND;
	}

	CONT_DEFMYHOMEBUILDINGS const * pkCont = NULL;
	g_kTblDataMgr.GetContDef(pkCont);
	if(!pkCont)
	{
		return E_MYHOME_NOT_FOUND;
	}

	CONT_DEFMYHOMEBUILDINGS::const_iterator iter = pkCont->find(kAddr.StreetNo());
	if(iter == pkCont->end())
	{
		return E_MYHOME_NOT_FOUND;
	}

	if(0 < (*iter).second.iGrade)
	{
		return E_MYHOME_NOT_HAVE_AUTHORITY;
	}

	BM::Stream kPacket(PT_M_I_REQ_MYHOME_SELL);
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(kAddr);
	SendToItem(m_kGndKey,kPacket);
	return S_OK;
}


struct SEnchantShiftInfo
{
	SEnchantShiftInfo():bShiftSoulcraft(false),bShiftSpecial(false),bShiftSocket(false){}

	bool bShiftSoulcraft;
	bool bShiftSpecial;
	bool bShiftSocket;
};

int const SHIFT_VALUE[ESR_NUM] = {2,1,0,-1,-2,0};

bool PgAction_Enchant_Shift::DoAction(CUnit* pUser, CUnit* pTarget)
{
	HRESULT hRes = Process(pUser, pTarget);

	if(S_OK == hRes)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_ENCHANT_SHIFT);
	kPacket.Push(hRes);
	pUser->Send(kPacket);
	return false;
}

HRESULT PgAction_Enchant_Shift::CheckEnableShift(SEnchantShiftInfo const & kInfo)
{
	if(m_kItem[ESIP_SOURCE].IsEmpty() || m_kItem[ESIP_TARGET].IsEmpty())
	{
		return E_ENCHANT_SHIFT_NOT_FOUND_ITEM;
	}

	SEnchantInfo const & kSEnchantInfo = m_kItem[ESIP_SOURCE].EnchantInfo();
	SEnchantInfo const & kDEnchantInfo = m_kItem[ESIP_TARGET].EnchantInfo();

	if( kSEnchantInfo.IsCurse() ||
		kSEnchantInfo.IsSeal() ||
		kSEnchantInfo.IsBinding() ||
		kDEnchantInfo.IsCurse() ||
		kDEnchantInfo.IsSeal() ||
		kDEnchantInfo.IsBinding() ||
		(kSEnchantInfo.PlusLv() < ENCHANT_SHIFT_MIN_PLUS_LEVEL))
	{
		return E_ENCHANT_SHIFT_CANT_SHIFT_ITEM;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CItemDef const * pkSItemDef = kItemDefMgr.GetDef(m_kItem[ESIP_SOURCE].ItemNo());
	CItemDef const * pkDItemDef = kItemDefMgr.GetDef(m_kItem[ESIP_TARGET].ItemNo());

	if(!pkSItemDef || !pkDItemDef)
	{
		return E_ENCHANT_SHIFT_NOT_FOUND_ITEM;
	}

	if(0 == (pkSItemDef->GetAbil(AT_EQUIP_LIMIT) & pkDItemDef->GetAbil(AT_EQUIP_LIMIT)))
	{
		return E_ENCHANT_SHIFT_NOT_MATCH_EQUIP_LIMIT;
	}

	/*if(0 < (EQUIP_LIMIT_WEAPON & pkSItemDef->GetAbil(AT_EQUIP_LIMIT)))
	{
		return E_ENCHANT_SHIFT_CANT_SHIFT_WEAPON;
	}*/

	int const iCheckLevelLimit = pkDItemDef->GetAbil(AT_LEVELLIMIT) - pkSItemDef->GetAbil(AT_LEVELLIMIT);

	/*if(ENCHANT_SHIFT_MIN_LEVEL > iCheckLevelLimit)
	{
		return E_ENCHANT_SHIFT_UNDER_SHIFT_LEVELLIMIT;
	}*/

	if(ENCHANT_SHIFT_MAX_LEVEL < iCheckLevelLimit)
	{
		return E_ENCHANT_SHIFT_OVER_SHIFT_LEVELLIMIT;
	}

	bool bAllDisable = true;
	for(int i = 0; i < ESR_NUM; ++i)
	{
		if( IsEnableShift(static_cast<eEnchantShiftRate>(i), kInfo) )
		{
			bAllDisable = false;
			break;
		}
	}
	if(bAllDisable)
	{
		return E_ENCHANT_SHIFT_CANT_SHIFT_ITEM;
	}
	return S_OK;
}

HRESULT PgAction_Enchant_Shift::CheckInsurance(CUnit* pUser)
{
	if(!pUser)
	{
		return E_FAIL;
	}

	if(!m_pkPlusInfo)
	{
		return E_ENCHANT_SHIFT_CANT_SHIFT_ITEM;
	}

	std::map<SItemPos,int> kContUseInsurance;

	for(int i = ESIP_INSURANCEP1;i <= ESIP_INSURANCEM3; ++i)
	{
		if(true == m_kItem[i].IsEmpty())
		{
			continue;
		}

		if( m_pkPlusInfo->InsuranceItemNo != m_kItem[i].ItemNo() )
		{//���� ��ũ���� ���� ���� ���
			if( false == g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN) )
			{//�Ϻ��� �ƴ� ��쿡�� ĳ�� ���� ���� �ٽ� üũ�ϵ��� ����.
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const *pItemDef = kItemDefMgr.GetDef(m_kItem[i].ItemNo());
				if( ESIP_INSURANCEM3 == i
					&& UICT_ENCHANT_INSURANCE != pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) )
				{//ĳ�� ����������� ��찡 �߰��Ǿ���.
					return E_ENCHANT_SHIFT_NOT_MATCH_INSURANCE;
				}
			}
			else
			{
				return E_ENCHANT_SHIFT_NOT_MATCH_INSURANCE;
			}
		}

		++kContUseInsurance[m_kItemPos[i]];
	}

	for(std::map<SItemPos,int>::const_iterator iter = kContUseInsurance.begin();iter != kContUseInsurance.end();++iter)
	{
		SItemPos const & kInsurancePos = (*iter).first;
		int const iInsuranceCount = (*iter).second;

		PgBase_Item kInsuranceItem;
		if(S_OK != pUser->GetInven()->GetItem(kInsurancePos, kInsuranceItem))
		{
			return E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_INSURANCE;
		}

		if(kInsuranceItem.Count() < iInsuranceCount)
		{
			return E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_INSURANCE;
		}

		m_kOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE,pUser->GetID(),SPMOD_Modify_Count(kInsuranceItem,kInsurancePos,-iInsuranceCount)));
	}

	return S_OK;
}

bool PgAction_Enchant_Shift::CheckShiftCost(CUnit* pUser, PgBase_Item const & kItem)
{
	if(!pUser || !m_pkPlusInfo)
	{
		return false;
	}

	// ��þƮ ���� ��� �˻�
	__int64 const iCost = m_pkPlusInfo->EnchantShiftCost;

	if(iCost > pUser->GetInven()->Money())
	{
		return false;
	}

	m_kOrder.push_back(SPMO(IMET_ADD_MONEY, pUser->GetID(), SPMOD_Add_Money(-iCost)));
	return true;	
}

bool PgAction_Enchant_Shift::CheckNeedItems(CUnit* pUser, int const iNeedItemCount, ContHaveItemNoCount const & kCont)
{
	int EnchantItemCount = iNeedItemCount;

	for(ContHaveItemNoCount::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
	{
		if(0 >= EnchantItemCount)
		{
			break;
		}

		int const iUseCount = std::min<int>(EnchantItemCount,(*iter).second);

		EnchantItemCount -= iUseCount;

		m_kOrder.push_back(SPMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any((*iter).first, -iUseCount)));
	}

	if(0 < EnchantItemCount)
	{
		return false;
	}

	return true;
}

bool MakeEnchantInfo(PgBase_Item const & kSrcItem, PgBase_Item & kTargetItem, bool const bShiftSoul, bool const bShiftSpecial, bool const bShiftSocket, bool const bShiftLv)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkDItemDef = kItemDefMgr.GetDef(kTargetItem.ItemNo());
	if( !pkDItemDef )
	{
		return false;
	}

	SEnchantInfo const & kSEnchantInfo = kSrcItem.EnchantInfo();
	SEnchantInfo kNewEnchantInfo = kTargetItem.EnchantInfo();

	if(true == bShiftSoul)
	{
		kNewEnchantInfo.Rarity(kSEnchantInfo.Rarity());
		kNewEnchantInfo.BasicType1(kSEnchantInfo.BasicType1());
		kNewEnchantInfo.BasicType2(kSEnchantInfo.BasicType2());
		kNewEnchantInfo.BasicType3(kSEnchantInfo.BasicType3());
		kNewEnchantInfo.BasicType4(kSEnchantInfo.BasicType4());

		int const iEquipPos = pkDItemDef->EquipPos();
		int const iLevelLimit = pkDItemDef->GetAbil(AT_LEVELLIMIT);

		if(kNewEnchantInfo.BasicType1()){kNewEnchantInfo.BasicLv1(FinddMaxAbilLevel(iEquipPos, kNewEnchantInfo.BasicType1(), iLevelLimit));}
		if(kNewEnchantInfo.BasicType2()){kNewEnchantInfo.BasicLv2(FinddMaxAbilLevel(iEquipPos, kNewEnchantInfo.BasicType2(), iLevelLimit));}
		if(kNewEnchantInfo.BasicType3()){kNewEnchantInfo.BasicLv3(FinddMaxAbilLevel(iEquipPos, kNewEnchantInfo.BasicType3(), iLevelLimit));}
		if(kNewEnchantInfo.BasicType4()){kNewEnchantInfo.BasicLv4(FinddMaxAbilLevel(iEquipPos, kNewEnchantInfo.BasicType4(), iLevelLimit));}

		kNewEnchantInfo.BasicAmpLv(kSEnchantInfo.BasicAmpLv());
	}

	if(true == bShiftSpecial)
	{
		kNewEnchantInfo.SkillOpt(kSEnchantInfo.SkillOpt());
		kNewEnchantInfo.SkillOptLv(kSEnchantInfo.SkillOptLv());

		kNewEnchantInfo.RareOptType1(kSEnchantInfo.RareOptType1());
		kNewEnchantInfo.RareOptType2(kSEnchantInfo.RareOptType2());
		kNewEnchantInfo.RareOptType3(kSEnchantInfo.RareOptType3());
		kNewEnchantInfo.RareOptType4(kSEnchantInfo.RareOptType4());

		kNewEnchantInfo.RareOptLv1(kSEnchantInfo.RareOptLv1());
		kNewEnchantInfo.RareOptLv2(kSEnchantInfo.RareOptLv2());
		kNewEnchantInfo.RareOptLv3(kSEnchantInfo.RareOptLv3());
		kNewEnchantInfo.RareOptLv4(kSEnchantInfo.RareOptLv4());

		kNewEnchantInfo.MagicOpt(kSEnchantInfo.MagicOpt());
	}

	if(true == bShiftSocket)
	{
		kNewEnchantInfo.GenSocketState(kSEnchantInfo.GenSocketState());
		kNewEnchantInfo.MonsterCard(kSEnchantInfo.MonsterCard());
		kNewEnchantInfo.GenSocketState2(kSEnchantInfo.GenSocketState2());
		kNewEnchantInfo.MonsterCard2(kSEnchantInfo.MonsterCard2());
		kNewEnchantInfo.GenSocketState3(kSEnchantInfo.GenSocketState3());
		kNewEnchantInfo.MonsterCard3(kSEnchantInfo.MonsterCard3());
	}
	
	if(true == bShiftLv)
	{
		E_ITEM_GRADE const eItemGrade = ::GetItemGrade(kTargetItem);

		int iMaxLv = 0;
		if ( false == PgItemRarityUpgradeFormula::GetMaxGradeLevel( eItemGrade, false, iMaxLv ) )
		{
			return false;
		}

		kNewEnchantInfo.PlusType( kSEnchantInfo.PlusType() );
		kNewEnchantInfo.PlusLv( std::min<int>(kSEnchantInfo.PlusLv(), iMaxLv) );
	}

	kTargetItem.EnchantInfo(kNewEnchantInfo);
	return true;
}

bool PgAction_Enchant_Shift::IsEnableShift(eEnchantShiftRate const & eRate, SEnchantShiftInfo const & kInfo)const
{
	PgBase_Item kCopyItem = m_kItem[ESIP_TARGET];
	if( false == MakeEnchantInfo(m_kItem[ESIP_SOURCE], kCopyItem, kInfo.bShiftSoulcraft, kInfo.bShiftSpecial, kInfo.bShiftSocket, false) )
	{
		return false;
	}

	E_ITEM_GRADE const eItemGrade = ::GetItemGrade(kCopyItem);

	int iMaxLv = 0;
	if( false == PgItemRarityUpgradeFormula::GetMaxGradeLevel( eItemGrade, false, iMaxLv ) )
	{
		return false;
	}

	int const iNewLevel = m_kItem[ESIP_SOURCE].EnchantInfo().PlusLv() + SHIFT_VALUE[eRate];
	if(iNewLevel > iMaxLv)
	{
		return false;
	}

	return true;
}

int PgAction_Enchant_Shift::BuildRandResult(SEnchantShiftInfo const & kInfo)
{
	if( !m_pkPlusInfo )
	{
		return 0;
	}

	int iTotalRate = 0;
	for(int i = 0;i < ESR_NUM;++i)
	{
		iTotalRate += m_pkPlusInfo->EnchantShiftRate[i];
	}

	int const iRate = BM::Rand_Index(iTotalRate);

	int iResultRate = 0;
	int iResultIdx = 0;

	for(int i = 0;i < ESR_NUM;++i,++iResultIdx)
	{
		iResultRate += m_pkPlusInfo->EnchantShiftRate[i];
		if(iResultRate < iRate)
		{
			continue;
		}
		if( false==IsEnableShift(static_cast<eEnchantShiftRate>(i), kInfo) )
		{
			continue;
		}
		break;
	}

	return iResultIdx;
}

HRESULT PgAction_Enchant_Shift::Process(CUnit* pUser, CUnit* pTarget)
{
	PgInventory * pkInv = pUser->GetInven();

	for(int i = 0;i < ESIP_NUM;++i)
	{
		m_kPacket.Pop(m_kItemPos[i]);
		pkInv->GetItem(m_kItemPos[i], m_kItem[i]);
	}

	if( LOCAL_MGR::NC_JAPAN == g_kLocal.ServiceRegion() 
		&& CheckIsCashItem(m_kItem[ESIP_SOURCE]))
	{//�Ϻ��� ��� ĳ�� �������� ��æƮ ���� �۾� �Ұ�
		return E_ENCHANT_SHIFT_CANT_SHIFT_ITEM;
	}


	SEnchantShiftInfo kInfo;
	/*�߰����̻��� ������� ����
	m_kPacket.Pop(kInfo.bShiftSoulcraft);
	m_kPacket.Pop(kInfo.bShiftSpecial);
	m_kPacket.Pop(kInfo.bShiftSocket);
	*/

	HRESULT hRes = CheckEnableShift(kInfo);
	if(S_OK != hRes)
	{
		return hRes;
	}

	int const iCurLevel = m_kItem[ESIP_SOURCE].EnchantInfo().PlusLv();
	GET_DEF(CItemDefMgr, kItemDefMgr);

	CItemDef const * pkSItemDef = kItemDefMgr.GetDef(m_kItem[ESIP_SOURCE].ItemNo());
	CItemDef const * pkDItemDef = kItemDefMgr.GetDef(m_kItem[ESIP_TARGET].ItemNo());

	if(!pkSItemDef || !pkDItemDef)
	{
		return E_ENCHANT_SHIFT_NOT_FOUND_ITEM;
	}

	m_pkPlusInfo = PgItemRarityUpgradeFormula::GetEnchantShiftPlusInfo(pkSItemDef->No(), iCurLevel, pkDItemDef->GetAbil(AT_LEVELLIMIT));
	if(!m_pkPlusInfo)
	{
		return E_ENCHANT_SHIFT_CANT_SHIFT_ITEM;
	}

	hRes = CheckInsurance(pUser);
	if(S_OK != hRes)
	{
		return hRes;
	}

	PgBase_Item kTargetCopyItem = m_kItem[ESIP_TARGET];

	SEnchantInfo kEnchantInfoCopy = kTargetCopyItem.EnchantInfo();
	kEnchantInfoCopy.PlusType(m_kItem[ESIP_SOURCE].EnchantInfo().PlusType());
	kEnchantInfoCopy.PlusLv(m_kItem[ESIP_SOURCE].EnchantInfo().PlusLv());
	kTargetCopyItem.EnchantInfo(kEnchantInfoCopy);

	if(false == CheckShiftCost(pUser, kTargetCopyItem))
	{
		return E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_COST;
	}


	{// ����/�� ��ȭ ���� ���� �˻�
		ContHaveItemNoCount rkOut;

		CItemDef const * pkEnchantItemDef = kItemDefMgr.GetDef(m_pkPlusInfo->EnchantItemNo);
		if(pkEnchantItemDef)
		{
			EUseItemCustomType const eType = static_cast<EUseItemCustomType>(pkEnchantItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE));
			pkInv->GetItems(eType, rkOut);
		}
		if(rkOut.empty())
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't Find EnchantItemNo["<< m_pkPlusInfo->EnchantItemNo <<L"]");
		}
		if(false == CheckNeedItems(pUser, m_pkPlusInfo->EnchantItemCount, rkOut))
		{
			return E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_ITEM;
		}
	}

	{// ��þƮ ���� ������ ���� �˻�
		int const iGemItemNo = m_pkPlusInfo->EnchantShiftGemNo;
		int const iHaveGemCount = pkInv->GetTotalCount(iGemItemNo);
		ContHaveItemNoCount rkOut;
		if(iGemItemNo && iHaveGemCount)
		{
			rkOut.insert(std::make_pair(iGemItemNo,iHaveGemCount));
		}

		if(false == CheckNeedItems(pUser, m_pkPlusInfo->EnchantShiftGemCount, rkOut))
		{
			return E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_GEM;
		}
	}

	int const iResultIdx = BuildRandResult(kInfo);

	BM::Stream kPacket(PT_M_C_ANS_ENCHANT_SHIFT);

	if(ESR_PLUS2 != iResultIdx)
	{
		int const iInsuranceIdx = ESIP_INSURANCEP2 + iResultIdx;
		if(false == m_kItem[iInsuranceIdx].IsEmpty())
		{
			kPacket.Push(E_ENCHANT_SHIFT_FAIL_WIDTH_INSURANCE);
			kPacket.Push(iResultIdx);
			PgAction_ReqModifyItem kItemModifyAction(CIE_Item_Enchant_Shift, m_kGndKey, m_kOrder, kPacket);
			kItemModifyAction.DoAction(pUser, NULL);
			return S_OK;
		}
	}

	SEnchantInfo kInitEnchantInfo = m_kItem[ESIP_SOURCE].EnchantInfo();
	kInitEnchantInfo.PlusLv(0);
	m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pUser->GetID(), SPMOD_Enchant( m_kItem[ESIP_SOURCE], m_kItemPos[ESIP_SOURCE], kInitEnchantInfo)));

	switch(iResultIdx)
	{
	case ESR_DELETE:
		{
			kPacket.Push(E_ENCHANT_SHIFT_FAIL_DESTROY);
		}break;
	default:
		{
			SEnchantInfo const & kSEnchantInfo = m_kItem[ESIP_SOURCE].EnchantInfo();

			PgBase_Item kCopyItem = m_kItem[ESIP_TARGET];
			MakeEnchantInfo(m_kItem[ESIP_SOURCE], kCopyItem, kInfo.bShiftSoulcraft, kInfo.bShiftSpecial, kInfo.bShiftSocket, false);
			SEnchantInfo kNewEnchantInfo = kCopyItem.EnchantInfo();

			E_ITEM_GRADE const eItemGrade = ::GetItemGrade(kCopyItem);

			int iMaxLv = 0;
			if ( false == PgItemRarityUpgradeFormula::GetMaxGradeLevel( eItemGrade, false, iMaxLv ) )
			{
				return E_ENCHANT_SHIFT_CANT_SHIFT_ITEM;
			}

			int iNewLevel = iCurLevel + SHIFT_VALUE[iResultIdx];

			iNewLevel = std::min<int>(iNewLevel, iMaxLv);

			kNewEnchantInfo.PlusType(kSEnchantInfo.PlusType());
			kNewEnchantInfo.PlusLv(iNewLevel);

			m_kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pUser->GetID(), SPMOD_Enchant( m_kItem[ESIP_TARGET], m_kItemPos[ESIP_TARGET], kNewEnchantInfo)));

			kPacket.Push(E_ENCHANT_SHIFT_SUCCESS);
			kPacket.Push(iResultIdx);
			kPacket.Push(iNewLevel);
		}break;
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_Item_Enchant_Shift, m_kGndKey, m_kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_JobSkill3_CreateItem
//////////////////////////////////////////////////////////////////////////////
bool PgAction_JobSkill3_CreateItem::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	HRESULT hRes = Process(pUser, pTarget);

	if(S_OK == hRes)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_JOBSKILL3_CREATEITEM);
	kPacket.Push(hRes);
	kPacket.Push(E_FAIL);
	kPacket.Push(m_iSaveIdx);
	pUser->Send(kPacket);
	return false;
}

SProbability ResultItem(int const iRecipeItemNo, CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, int const iProbabilityNo, int const iResourceProbabilityUp)
{
	SProbability kRet = ProbabilityUtil::GetOne(rkContDef, iProbabilityNo, iResourceProbabilityUp);
	if(kRet.iResultNo == 0)
	{
		JobSkill_Third::CONT_JS3_RESULT_ITEM kContItem;
		bool bRemainResultItem = false;
		JobSkill_Third::GetResProbabilityItem(iRecipeItemNo, iResourceProbabilityUp, kContItem, bRemainResultItem);
		int iIdx = kContItem.size() - 1;
		if(0 <= iIdx)
		{
			kRet = kContItem.at(iIdx).second;
		}
	}
	return kRet;
}

HRESULT PgAction_JobSkill3_CreateItem::Process(CUnit* pUser, CUnit* /*pTarget*/)
{
	int iSaveIdx = 0;
	CONT_JS3_RESITEM_INFO kContResItemInfo;
	m_kPacket.Pop(iSaveIdx);
	m_kPacket.Pop(kContResItemInfo);

	CONT_JS3_RESITEM_INFO::iterator res_it = kContResItemInfo.begin();
	while(res_it != kContResItemInfo.end())
	{
		if((*res_it).second < 0)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__ << L"Hacking ID<" << pUser->Name() << L"> CharGuid<" << pUser->GetID() << L"> SaveIdx<" << iSaveIdx << L"> ItemNo<" << (*res_it).first << L"> Count<" << (*res_it).second << L">");
			(*res_it).second = 0;
		}
		++res_it;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	PgInventory * pkInv = pkPlayer ? pkPlayer->GetInven() : NULL;
	if( !pkPlayer || !pkInv )
	{
		return E_FAIL;
	}

	CONT_DEF_JOBSKILL_PROBABILITY_BAG const* pkDefJSProb = NULL;
	g_kTblDataMgr.GetContDef(pkDefJSProb);
	if( !pkDefJSProb )
	{
		return E_FAIL;
	}

	SJobSkillSaveIdx const * const pkDefSaveIdx = JobSkill_Third::GetJobSkillSaveIdx(iSaveIdx);
	if( !pkDefSaveIdx )
	{
		return E_FAIL;
	}

	HRESULT hRes = S_OK;
	hRes = JobSkill_Third::CheckNeedSkill(pkPlayer, iSaveIdx);
	if(S_OK != hRes)
	{
		return hRes;
	}

	hRes = JobSkill_Third::CheckNeedProductPoint(pkPlayer, pkDefSaveIdx->iRecipeItemNo);
	if(S_OK != hRes)
	{
		return hRes;
	}
	int const iNeedProductPoint = JobSkill_Third::GetJobSkill3NeedProductPoint(pkDefSaveIdx->iRecipeItemNo);
	SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pUser->GetID(), SPMOD_JobSkillExhaustion(iNeedProductPoint)); // �Ƿε��Ҹ�
	m_kOrder.push_back( kIMO );

	hRes = JobSkill_Third::CheckNeedItems(pkInv, pkDefSaveIdx->iRecipeItemNo, kContResItemInfo);
	if(S_OK != hRes)
	{
		return hRes;
	}

	hRes = JobSkill_Third::CheckInvenSize(pkInv, pkDefSaveIdx->iRecipeItemNo, kContResItemInfo);
	if(S_OK != hRes)
	{
		return hRes;
	}

	PgBase_Item kNeedItem;
	CONT_JS3_RESITEM_INFO::const_iterator c_iter = kContResItemInfo.begin();
	while(c_iter != kContResItemInfo.end())
	{
		m_kOrder.push_back(SPMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any((*c_iter).first, -(*c_iter).second)));

		++c_iter;
	}

	int const iProbabilityNo = JobSkill_Third::GetJobSkill3ProbabilityNo(pkDefSaveIdx->iRecipeItemNo);
	int const iResourceProbabilityUp = JobSkill_Third::GetResourceProbabilityUp(pkInv, kContResItemInfo);
	SProbability const kRet = ResultItem(pkDefSaveIdx->iRecipeItemNo, *pkDefJSProb, iProbabilityNo, iResourceProbabilityUp);
	BM::Stream kPacket(PT_M_C_ANS_JOBSKILL3_CREATEITEM);

	if(kRet.iResultNo == 0)
	{
		kPacket.Push(E_JS3_CREATEITEM_FAIL_DESTORY);
		kPacket.Push(iSaveIdx);
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("CREATEITEM_FAIL_DESTORY <") << iSaveIdx << _T(">"));
	}
	else
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kRet.iResultNo);
		if(!pItemDef)
		{
			return E_JS3_CREATEITEM_FAIL_TARGETITEM;
		}

		PgBase_Item kItem;
		int iCount = kRet.iCount;
		while(iCount>0)
		{
			size_t const iMinimumAmount = 1;
			int iTmpCount = iCount;
			if(iMinimumAmount < pItemDef->GetAbil(AT_MAX_LIMIT)
			&& pItemDef->IsAmountItem()
			&& iCount > pItemDef->GetAbil(AT_MAX_LIMIT))
			{
				iTmpCount = pItemDef->GetAbil(AT_MAX_LIMIT);
			}

			if(S_OK == CreateSItem(kRet.iResultNo,iTmpCount,GIOT_NONE,kItem))
			{
				iCount -= iTmpCount;
				kPacket.Push(E_JS3_CREATEITEM_SUCCESS);
				kPacket.Push(iSaveIdx);
				kPacket.Push(kRet.iResultNo);
				kPacket.Push(kRet.iCount);

				tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, SItemPos(0,0), true);
				SPMO kIMO(IMET_INSERT_FIXED, pUser->GetID(), kAddItem);
				m_kOrder.push_back(kIMO);
			}
			else
			{
				return E_JS3_CREATEITEM_FAIL_TARGETITEM;
			}
		}
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL3_CREATE_ITEM, m_kGndKey, m_kOrder, kPacket);
	kItemModifyAction.DoAction(pUser, NULL);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////

HRESULT const PgAction_ReqDefGemStoreBuy::Process(CUnit* pUser)
{
	int iItemNo = 0;
	int iMenu = 0;
	int iOrderIndex = 0;
	
	m_kPacket.Pop(iItemNo);
	m_kPacket.Pop(iMenu);
	m_kPacket.Pop(iOrderIndex);

	CONT_DEFGEMSTORE const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if(NULL == pkDef)
	{
		return E_NOT_FOUND_GEMSTORE;
	}

	CONT_DEFGEMSTORE::const_iterator iter = pkDef->find(m_kNpcGuid);
	if(iter == pkDef->end())
	{
		return E_NOT_FOUND_GEMSTORE;
	}

	CONT_DEFGEMSTORE_ARTICLE::key_type	kKey(iItemNo, iMenu, iOrderIndex);
	CONT_DEFGEMSTORE::mapped_type const & kStore = (*iter).second;
	CONT_DEFGEMSTORE_ARTICLE::const_iterator articleiter = kStore.kContArticles.find(kKey);	
	if(articleiter == kStore.kContArticles.end())
	{
		return E_NOT_FOUND_ARTICLE;
	}

	int const iItemCount = articleiter->second.iItemCount;

	PgInventory * pkInven = pUser->GetInven();

	CONT_PLAYER_MODIFY_ORDER kOrder;

	CONT_DEFGEMSTORE_ARTICLE::mapped_type const & kArticle = (*articleiter).second;

	/*if(0 < kArticle.iCP)
	{
		if(pUser->GetAbil(AT_CP) < kArticle.iCP)
		{
			return E_NOT_ENOUGH_CP;
		}

		kOrder.push_back(SPMO(IMET_ADD_CP,pUser->GetID(), SPMOD_Add_CP(-kArticle.iCP)));
	}*/

	GET_DEF(CItemDefMgr, kItemDefMgr);
	int iUseGemCount = 0;
	for(CONT_DEFGEMS::const_iterator gemiter = kArticle.kContGems.begin();gemiter != kArticle.kContGems.end();++gemiter)
	{
		if((*gemiter).sCount > static_cast<int>(pkInven->GetTotalCount((*gemiter).iGemNo)))
		{
			return E_NOT_ENOUGH_GEMS;
		}

		iUseGemCount += (*gemiter).sCount;
		
		const CItemDef* pkItemDef = kItemDefMgr.GetDef((*gemiter).iGemNo);
		if( pkItemDef )
		{
			if( false == pkItemDef->IsAmountItem() ) // �������� ������
			{
				for(int count=0; count<(*gemiter).sCount; ++count)
				{
					SPMO kIMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any((*gemiter).iGemNo,-1));
					kOrder.push_back(kIMO);
				}
			}
			else
			{
				SPMO kIMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any((*gemiter).iGemNo,-(*gemiter).sCount));
				kOrder.push_back(kIMO);
			}
		}
		else
		{
			return E_NOT_FOUND_ARTICLE;
		}
	}

	{// ������ ��ǥ ��� ������ ���� ����		
		PgAddAchievementValue kMA( AT_ACHIEVEMENT_CERTIFICATEOFHERO, iUseGemCount, m_kGndKey );
		kMA.DoAction( pUser, NULL );
	}

	CONT_ITEM_CREATE_ORDER kItemOrder;
	if( OrderCreateItem(kItemOrder, iItemNo, iItemCount) )
	{
		for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kItemOrder.begin(); c_it!=kItemOrder.end(); ++c_it)
		{
			tagPlayerModifyOrderData_Insert_Fixed kAddItem(*c_it, SItemPos(0,0), true);
			SPMO kIMO(IMET_INSERT_FIXED, pUser->GetID(), kAddItem);
			kOrder.push_back(kIMO);
		}

		BM::Stream kPacket(PT_M_C_ANS_DEFGEMSTORE_BUY);
		PgAction_ReqModifyItem kItemModifyAction(CIE_GemStore_Buy, m_kGndKey, kOrder, kPacket);
		kItemModifyAction.DoAction(pUser, NULL);
		return S_OK;
	}
	return E_NOT_FOUND_ARTICLE;
}

bool PgAction_ReqDefGemStoreBuy::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kRes = Process(pUser);
	/*if(S_OK == kRes)
	{
		return true;
	}*/

	BM::Stream kPacket(PT_M_C_ANS_DEFGEMSTORE_BUY);
	kPacket.Push(kRes);
	pUser->Send(kPacket);
	return false;
}

/////////////////////////////////////////////////////////////////////////

HRESULT const PgAction_ReqDefCollectAntique::Process(CUnit* pUser)
{
	int iMenu = 0;
	SItemPos MaterialItemPos;
	int iItemNo = 0;
	int iIndex = 0;
	
	m_kPacket.Pop(iMenu);
	m_kPacket.Pop(MaterialItemPos);
	m_kPacket.Pop(iItemNo);
	m_kPacket.Pop(iIndex);

	
	TABLE_LOCK(CONT_DEFANTIQUE) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_DEFANTIQUE const * pkDef = kObjLock.Get();
	if(NULL == pkDef)
	{
		return E_NOT_FOUND_ANTIQUE;
	}

	TBL_PAIR_KEY_INT const kKey(iMenu,iItemNo);
	CONT_DEFANTIQUE::const_iterator iter = pkDef->find(kKey);
	if(iter == pkDef->end())
	{
		return E_NOT_FOUND_ANTIQUE;
	}

	CONT_DEFANTIQUE::mapped_type const & kStore = (*iter).second;
	CONT_DEF_ANTIQUE_ARTICLE::const_iterator articleiter = kStore.kContArticles.find(iIndex);	
	if(articleiter == kStore.kContArticles.end())
	{
		return E_NOT_FOUND_ARTICLE;
	}

	int const iResultItemNo = articleiter->second.first;
	int const iResultItemCount = articleiter->second.second;

	PgInventory * pkInven = pUser->GetInven();
	if(!pkInven)
	{
		return E_FAIL;
	}

	PgBase_Item	MaterialItem;
	if( S_OK != pkInven->GetItem(MaterialItemPos, MaterialItem) )
	{
		return E_FAIL;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef* pkItemDef = kItemDefMgr.GetDef(kStore.iNeedItem);
	if( !pkItemDef )
	{
		return E_NOT_FOUND_ARTICLE;
	}

	int iHaveNeedItemCount = MaterialItem.Count();
	int iAddCount = iHaveNeedItemCount / kStore.iNeedCount;
	if( !pkItemDef->IsAmountItem() )
	{
		iHaveNeedItemCount = 1;
		iAddCount = 1;
	}

	if( kStore.iNeedCount > iHaveNeedItemCount )
	{
		return E_NOT_ENOUGH_GEMS;
	}

	if( kStore.iNeedItem != MaterialItem.ItemNo() )
	{
		return E_FAIL;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	if( !pkItemDef->IsAmountItem() )
	{
		SPMOD_Modify_Count DelData(MaterialItem, MaterialItemPos, 0, true);
		SPMO kIMO(IMET_MODIFY_COUNT, pUser->GetID(), DelData);
		kOrder.push_back(kIMO);
	}
	else
	{
		SPMOD_Modify_Count DelData(MaterialItem, MaterialItemPos, -(kStore.iNeedCount*iAddCount));
		SPMO kIMO(IMET_MODIFY_COUNT, pUser->GetID(), DelData);
		kOrder.push_back(kIMO);
	}

	CONT_ITEM_CREATE_ORDER kItemOrder;
	if( OrderCreateItem(kItemOrder, iResultItemNo, iResultItemCount*iAddCount) )
	{
		for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kItemOrder.begin(); c_it!=kItemOrder.end(); ++c_it)
		{
			tagPlayerModifyOrderData_Insert_Fixed kAddItem(*c_it, SItemPos(0,0), true);
			SPMO kIMO(IMET_INSERT_FIXED, pUser->GetID(), kAddItem);
			kOrder.push_back(kIMO);
		}

		BM::Stream kPacket(PT_M_C_ANS_COLLECT_ANTIQUE);
		kPacket.Push(iMenu);
		kPacket.Push(iItemNo);
		kPacket.Push(iIndex);
		kPacket.Push(iResultItemNo);
		PgAction_ReqModifyItem kItemModifyAction(CIE_CollectAntique, m_kGndKey, kOrder, kPacket);
		kItemModifyAction.DoAction(pUser, NULL);
		return S_OK;
	}
	return E_NOT_FOUND_ARTICLE;
}

bool PgAction_ReqDefCollectAntique::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kRes = Process(pUser);

	if(S_OK!=kRes)
	{
		BM::Stream kPacket(PT_M_C_ANS_COLLECT_ANTIQUE);
		kPacket.Push(kRes);
		pUser->Send(kPacket);
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////

HRESULT const PgAction_ReqDefExchangeGemStore::Process(CUnit* pUser)
{
	if(NULL == pUser)
	{
		return E_FAIL;
	}

	CONT_DEFGEMSTORE const * pDef = NULL;
	g_kTblDataMgr.GetContDef(pDef);
	if(NULL == pDef)
	{
		return E_NOT_FOUND_GEMSTORE;
	}

	PgInventory * pInven = pUser->GetInven();

	int MenuType = 0;
	SItemPos kItemPos;
	int TargetItemNo = 0;
	m_kPacket.Pop(MenuType);
	m_kPacket.Pop(kItemPos);
	m_kPacket.Pop(TargetItemNo);

	PgBase_Item	kMaterialItem;
	if( S_OK != pInven->GetItem(kItemPos, kMaterialItem) )
	{
		return E_FAIL;
	}

	int const MaterialItemNo = kMaterialItem.ItemNo();
	if( !MaterialItemNo || !TargetItemNo )
	{
		return E_FAIL;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef* pItemDef = kItemDefMgr.GetDef(MaterialItemNo);
	if( NULL == pItemDef )
	{
		return E_NOT_FOUND_ARTICLE;
	}

	int NeedItemCnt = 0;
	int ResultItemCnt = 0;
	CONT_DEFGEMSTORE::const_iterator iter = pDef->begin();
	for( ; iter != pDef->end(); ++iter)
	{
		if( ResultItemCnt )
		{
			break;
		}
		CONT_DEFGEMSTORE_ARTICLE::const_iterator SubIter = iter->second.kContArticles.begin();
		for( ; SubIter != iter->second.kContArticles.end(); ++SubIter)
		{
			if( ResultItemCnt )
			{
				break;
			}
			if( MenuType == SubIter->first.iMenu
				&& TargetItemNo == SubIter->first.iItemNo )
			{
				CONT_DEFGEMS::const_iterator GemIter = SubIter->second.kContGems.begin();
				for( ; GemIter != SubIter->second.kContGems.end(); ++GemIter)
				{
					if( MaterialItemNo == GemIter->iGemNo )
					{ // DB�� �ش� �������� �����ϸ�
						NeedItemCnt = GemIter->sCount;
						ResultItemCnt = SubIter->second.iItemCount;
						break;
					}
				}
			}
		}
	}

	if( ResultItemCnt )
	{
		if(NeedItemCnt > static_cast<int>(pInven->GetTotalCount(MaterialItemNo)) )
		{
			return E_NOT_ENOUGH_GEMS;
		}

		CONT_PLAYER_MODIFY_ORDER Order;
		//������ ��ġ�� ������ ���� ����
		if( false == pItemDef->IsAmountItem() ) // �������� ������
		{
			SPMO IMO(IMET_MODIFY_COUNT, pUser->GetID(), SPMOD_Modify_Count(kMaterialItem, kItemPos, 0, true));
			Order.push_back(IMO);
			--NeedItemCnt;
		}
		else
		{
			if(0 <= kMaterialItem.Count()-NeedItemCnt)
			{
				SPMO IMO(IMET_MODIFY_COUNT, pUser->GetID(), SPMOD_Modify_Count(kMaterialItem, kItemPos, -NeedItemCnt));
				Order.push_back(IMO);

				NeedItemCnt = 0;
			}
			else
			{
				NeedItemCnt -= kMaterialItem.Count();

				SPMO IMO(IMET_MODIFY_COUNT, pUser->GetID(), SPMOD_Modify_Count(kMaterialItem, kItemPos, 0, true));
				Order.push_back(IMO);
			}
		}

		//���� ������ ��ġ�� ������� ����
		if(NeedItemCnt > 0)
		{
			if( false == pItemDef->IsAmountItem() ) // �������� ������
			{
				for(int count = 0; count < NeedItemCnt; ++count)
				{
					SPMO IMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any(MaterialItemNo,-1));
					Order.push_back(IMO);
				}
			}
			else
			{
				SPMO IMO(IMET_ADD_ANY, pUser->GetID(), SPMOD_Add_Any(MaterialItemNo,-NeedItemCnt));
				Order.push_back(IMO);
			}
		}

		for(int i = 0; i < ResultItemCnt; ++i)
		{
			PgBase_Item kItem;
			if(S_OK != CreateSItem(TargetItemNo, 1, GIOT_NONE, kItem))
			{
				return E_NOT_FOUND_ARTICLE;
			}

			tagPlayerModifyOrderData_Insert_Fixed AddItem(kItem, SItemPos(1,0), true);
			SPMO IMO(IMET_INSERT_FIXED, pUser->GetID(), AddItem);
			Order.push_back(IMO);
		}

		BM::Stream Packet(PT_M_C_ANS_EXCHANGE_GEMSTORE);
		Packet.Push(TargetItemNo);
		PgAction_ReqModifyItem ItemModifyAction(CIE_ExchangeGem, m_kGndKey, Order, Packet);
		ItemModifyAction.DoAction(pUser, NULL);
		return S_OK;
	}

	// DB���� ��ã��
	return E_NOT_FOUND_ARTICLE;
}

bool PgAction_ReqDefExchangeGemStore::DoAction(CUnit* pUser, CUnit* Nothing)
{
	HRESULT const kRes = Process(pUser);

	if(S_OK!=kRes)
	{
		BM::Stream kPacket(PT_M_C_ANS_EXCHANGE_GEMSTORE);
		kPacket.Push(kRes);
		pUser->Send(kPacket);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_SoulTransfer_Extract
//////////////////////////////////////////////////////////////////////////////
bool PgAction_SoulTransfer_Extract::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	HRESULT hRes = Process(pUser, pTarget);

	if(S_OK == hRes)
	{
		return true;
	}

	return false;
}

HRESULT PgAction_SoulTransfer_Extract::Process(CUnit* pUser, CUnit* pTarget)
{
	ESoulTransMsg kMessage = ESTM_SERVER_ERR;
	HRESULT hRes = E_FAIL;
	int iResultitemNo = 0;
	if(pUser)
	{
		while(1)
		{
			//�κ����� ��������
			PgInventory* pInv = pUser->GetInven();
			if(!pInv)
			{//�κ��� �־�¡?
				break;
			}
			SItemPos kResourcePos;
			m_kPacket.Pop(kResourcePos);
			//������ ������ ã��
			PgBase_Item kResourceItem;
			if( E_FAIL == pInv->GetItem(kResourcePos, kResourceItem) )
			{//���� - �����۾���
				kMessage = ESTM_NOT_SELECT_EXTRACT_ITEM;
				break;
			}
			if( false == SoulTransitionUtil::IsSoulExtractItem( kResourceItem ) )
			{
				kMessage = ESTM_CANT_EXTRACT_ITEM;
				break;
			}
			//���� �����޾ƿ���
			CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type kExtractInfo;
			if( !SoulTransitionUtil::GetSoulExtractInfo(kResourceItem.ItemNo(), kExtractInfo) )
			{
				kMessage = ESTM_FAILED_FIND_EXTRACT_INFO;
				break;
			}
			//���� ������ �´� ����⸦ ������ �ִ��� Ȯ��
			SItemPos kExtractItemPos;
			if( E_FAIL == pInv->GetFirstItem(kExtractInfo.iExtractItemNo, kExtractItemPos) )
			{
				kMessage = ESTM_NOT_HAVE_EXTRACT_ITEM;
				break;
			}
			PgBase_Item kExtractItem;
			if( E_FAIL == pInv->GetItem(kExtractItemPos, kExtractItem) )
			{
				kMessage = ESTM_NOT_HAVE_EXTRACT_ITEM;
				break;
			}
			SItemPos kEmptyItemPos;
			if( !pInv->GetNextEmptyPos(IT_CONSUME, kEmptyItemPos) )
			{
				kMessage = ESTM_INVENTORY_FULL;
				break;
			}
			//�����ϰ�
			PgBase_Item kTransitionItem;	//���º������� ����
			PgBase_Item kTempResourceItem =  kResourceItem;
			if(!SoulTransitionUtil::TransitionItem(kTempResourceItem, kTransitionItem))
			{
				kMessage = ESTM_SERVER_ERR;
				break;
			}
			CONT_PLAYER_MODIFY_ORDER kOrder;
			//Ÿ�� ������ ����
			kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pUser->GetID(), SPMOD_Enchant( kResourceItem, kResourcePos, kTempResourceItem.EnchantInfo())));
			{//����� ����
				SPMOD_Modify_Count kDelData(kExtractItem, kExtractItemPos, -1);
				SPMO kIMO(IMET_MODIFY_COUNT, pUser->GetID(), kDelData);
				kOrder.push_back(kIMO);
			}
			{//���º������� ������ ����
				tagPlayerModifyOrderData_Insert_Fixed kAddItem(kTransitionItem, SItemPos(), true);
				SPMO kIMO(IMET_INSERT_FIXED, pUser->GetID(), kAddItem);
				kOrder.push_back(kIMO);
			}
			PgAction_ReqModifyItem kItemModifyAction(CIE_SoulTransfer_Extract, m_kGndKey, kOrder);
			kItemModifyAction.DoAction(pUser, NULL);

			iResultitemNo = kTransitionItem.ItemNo();

			kMessage = ESTM_SUCCESS_EXTRACT;
			hRes = S_OK;
			break;
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_SOULTRANSFER_EXTRACT);
	kPacket.Push(hRes);
	kPacket.Push(kMessage);
	if(S_OK == hRes && 0 != iResultitemNo)
	{
		kPacket.Push(iResultitemNo);
	}
	pUser->Send(kPacket);

	return hRes;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_SoulTransfer_Transition
//////////////////////////////////////////////////////////////////////////////
bool PgAction_SoulTransfer_Transition::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	HRESULT hRes = Process(pUser, pTarget);

	if(S_OK == hRes)
	{
		return true;
	}
	return false;
}

HRESULT PgAction_SoulTransfer_Transition::Process(CUnit *pUser, CUnit *pTarget)
{
	ESoulTransMsg kMessage = ESTM_SERVER_ERR;
	HRESULT hRes = E_FAIL;
	int iResultItemNo = 0;
	if(pUser)
	{
		while(1)
		{
			//�κ����� ��������
			PgInventory* pInv = pUser->GetInven();
			if(!pInv)
			{//�κ��� �־�¡?
				break;
			}
			SItemPos kTransitionItemPos;
			m_kPacket.Pop(kTransitionItemPos);
			//���� ���� ������ ã��
			PgBase_Item kTransitionItem;
			if( E_FAIL == pInv->GetItem(kTransitionItemPos, kTransitionItem) )
			{
				kMessage = ESTM_NOT_SELECT_TRANSITION_ITEM;
				break;
			}
			int const iGrade = GetItemGrade(kTransitionItem);
			//���� ��� ������ ã��
			SItemPos kTargetItemPos;
			m_kPacket.Pop(kTargetItemPos);
			PgBase_Item kTargetItem;
			if( E_FAIL == pInv->GetItem(kTargetItemPos, kTargetItem) )
			{
				kMessage = ESTM_NOT_SELECT_DEST_ITEM;
				break;
			}
			if( false == SoulTransitionUtil::IsSoulInsertItem(kTargetItem) )
			{
				kMessage = ESTM_NOT_CORRECT_TRANSITION;
				break;
			}
			//���� ���� ã��
			CONT_DEF_JOBSKILL_SOUL_TRANSITION::value_type kTransitionInfo;
			if( !SoulTransitionUtil::GetSoulTransitionInfo(kTargetItem.ItemNo(), iGrade, kTransitionInfo) )
			{
				kMessage = ESTM_FAILED_FIND_TRANSITION_INFO;
				break;
			}
			if(!SoulTransitionUtil::IsTransition(kTransitionItem, kTargetItem) )
			{
				kMessage = ESTM_NOT_CORRECT_TRANSITION;
				break;
			}
			//���� ���������� ���� ������ �ִ� ���� ������ ã��
			bool bUseInsurance = false;
			m_kPacket.Pop(bUseInsurance);
			SItemPos kInsuranceItemPos;
			PgBase_Item kInsuranceItem;
			if(bUseInsurance)
			{
				if( E_FAIL == pInv->GetFirstItem(kTransitionInfo.iInsuranceitemNo, kInsuranceItemPos) )
				{//������ ����
					kMessage = ESTM_NOT_HAVE_INSURANCE;
					break;
				}
				if( E_FAIL == pInv->GetItem( kInsuranceItemPos, kInsuranceItem) )
				{
					kMessage = ESTM_NOT_HAVE_INSURANCE;
					break;
				}
			}
			//Ȯ�� ������ ����?(������ 0�� �̻��̳�)
			int iRateUpItemCount = 0;
			m_kPacket.Pop(iRateUpItemCount);
			SItemPos kRateUpItemPos;
			PgBase_Item kRateUpItem;
			if( 0 < iRateUpItemCount )
			{
				if( iRateUpItemCount > pInv->GetInvTotalCount( kTransitionInfo.iProbabilityUpItemNo ) )
				{//������ �ִ� ������ ��Ŷ���� ���� ���� ���� ������ ����
					kMessage = ESTM_NOT_HAVE_RATEUP;
					break;
				}
				if(E_FAIL == pInv->GetFirstItem(kTransitionInfo.iProbabilityUpItemNo, kRateUpItemPos) )
				{//������ ����
					kMessage = ESTM_NOT_HAVE_RATEUP;
					break;
				}
				if( E_FAIL == pInv->GetItem( kRateUpItemPos, kRateUpItem) )
				{
					kMessage = ESTM_NOT_HAVE_RATEUP;
					break;
				}
			}
			//�ҿ� ������� Ȯ��
			SItemPos kSoulItemPos;
			PgBase_Item kSoulItem;
			if( 0 < kTransitionInfo.iSoulItemCount )
			{
				if( kTransitionInfo.iSoulItemCount > pInv->GetInvTotalCount( ITEM_SOUL_NO ) )
				{//������ �ִ� ������ ��Ŷ���� ���� ���� ���� ������ ����
					kMessage = ESTM_NOT_ENOUGH_SOUL;
					break;
				}
				if( E_FAIL == pInv->GetFirstItem(ITEM_SOUL_NO, kSoulItemPos) )
				{//������ ����
					kMessage = ESTM_NOT_ENOUGH_SOUL;
					break;
				}
			}
			//������ ������� Ȯ��
			if( 0 < kTransitionInfo.iNeedMoney )
			{
				if( kTransitionInfo.iNeedMoney > pUser->GetAbil64(AT_MONEY) )
				{
					kMessage = ESTM_NOT_ENOUGH_MONEY;
					break;
				}
			}
			CONT_PLAYER_MODIFY_ORDER kOrder;
			//���� ������ Ȯ�� ������ ���� ������  �����ϰ�
			int const iTotalRate = kTransitionInfo.iSuccessRate + (kTransitionInfo.iProbabilityUpRate * iRateUpItemCount);
			int const iSuccessRate = BM::Rand_Range(SOUL_TRANSITION_RATE);
			if( iSuccessRate <= iTotalRate 
				|| pUser->GetAbil(AT_GM_GODHAND))
			{//����?
				//���� ������ ����
				PgBase_Item kTempTargetItem = kTargetItem;
				PgBase_Item kTempTransitionItem = kTransitionItem;
				if(!SoulTransitionUtil::TransitionItem(kTempTransitionItem, kTempTargetItem))
				{
					kMessage = ESTM_SERVER_ERR;
					break;
				}
				kOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, pUser->GetID(), SPMOD_Enchant( kTargetItem, kTargetItemPos, kTempTargetItem.EnchantInfo())));
				//���� ���� ������ ����
				SPMOD_Modify_Count kDelData(kTransitionItem, kTransitionItemPos, 0, true);
				SPMO kIMO(IMET_MODIFY_COUNT, pUser->GetID(), kDelData);
				kOrder.push_back(kIMO);
				
				iResultItemNo = kTargetItem.ItemNo();
				kMessage = ESTM_SUCCESS_TRANSITION;
				hRes = S_OK;
			}
			else
			{//����? 
				if( bUseInsurance )
				{
					kMessage = ESTM_FAILED_TRANSITION_INSURANCE;
				}
				else
				{//���� ������ ���±��������� ����
					SPMOD_Modify_Count kDelData(kTransitionItem, kTransitionItemPos, 0, true);
					SPMO kIMO(IMET_MODIFY_COUNT, pUser->GetID(), kDelData);
					kOrder.push_back(kIMO);

					kMessage = ESTM_FAILED_TRANSITION_NOT_INSURANCE;
				}
			}

			if( bUseInsurance)
			{//���� ���� ������ ������ ���� 
				SPMOD_Modify_Count kDelData(kInsuranceItem, kInsuranceItemPos, -1);
				SPMO kIMO(IMET_MODIFY_COUNT, pUser->GetID(), kDelData);
				kOrder.push_back(kIMO);
			}
			if( 0 < iRateUpItemCount )
			{//Ȯ�� ���� ���� ������ ������ ���� �� ŭ ����
				SPMOD_Add_Any DelRateUp(kRateUpItem.ItemNo() ,-iRateUpItemCount);
				SPMO MODS(IMET_ADD_ANY,pUser->GetID(),DelRateUp);
				kOrder.push_back(MODS);
			}
			//�ҿ� ����
			if( 0 < kTransitionInfo.iSoulItemCount )
			{
				SPMOD_Add_Any DelSoul(ITEM_SOUL_NO,-kTransitionInfo.iSoulItemCount);
				SPMO MODS(IMET_ADD_ANY,pUser->GetID(),DelSoul);
				kOrder.push_back(MODS);
			}
			//�ݾ� �谨
			if( 0 < kTransitionInfo.iNeedMoney )
			{
				SPMOD_Add_Money kDelMoneyData(-kTransitionInfo.iNeedMoney);//�ʿ�Ӵ� ����.
				SPMO kIMO(IMET_ADD_MONEY, pUser->GetID(), kDelMoneyData);
				kOrder.push_back(kIMO);
			}
			PgAction_ReqModifyItem kItemModifyAction(CIE_SoulTransfer_Transition, m_kGndKey, kOrder);
			kItemModifyAction.DoAction(pUser, NULL);
			break;
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_SOULTRANSFER_TRANSITION);
	kPacket.Push(hRes);
	kPacket.Push(kMessage);
	if(S_OK == hRes && 0 != iResultItemNo)
	{
		kPacket.Push(iResultItemNo);
	}
	pUser->Send(kPacket);

	return hRes;
}


bool PgAction_Manufacture::DoAction(CUnit *pUser, CUnit *pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	PgInventory* pInv = pUser->GetInven();
	if(!pInv)
	{//�κ��� �־�¡?
		return false;
	}
	SItemPos kSrcItemPos;
	m_kPacket.Pop(kSrcItemPos);
	PgBase_Item kItem;
	if( E_FAIL == pInv->GetItem(kSrcItemPos, kItem) )
	{//�κ��� ������ ������ ����...
		return false;
	}
	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef* pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pItemDef)
	{
		return false;
	}

	BYTE byScore = 9;
	m_kPacket.Pop(byScore);

	if(pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) != UICT_MANUFACTURE) { return false; }
/*
	//�����غ��� Ÿ���� ���⼭ ���� ����
	EManufactureType eType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	if(eType)
	{
	}
*/
	if(!IsLearnedSkill(pUser, pItemDef)) { return false; }

	int const iLevel = pUser->GetAbil(AT_LEVEL);
	int iItemBagNo = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
	int iResultItemNo = 0;
	int iResultItemCount = 0;
	if(!PopItemByScore(iItemBagNo, iLevel, byScore, iResultItemNo, iResultItemCount) || iResultItemNo == 0 || iResultItemCount == 0)
	{
		return false;
	}
	
	CONT_PLAYER_MODIFY_ORDER kOrderDelete;

	SPMOD_Modify_Count kDelData(kItem, kSrcItemPos, -1);//1�� ����.
	SPMO MODS_Del(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), kDelData);
	kOrderDelete.push_back(MODS_Del);

	PgAction_ReqModifyItem kItemDeleteAction(CIE_Manufacture, m_kGndKey, kOrderDelete);
	kItemDeleteAction.DoAction(pUser, NULL);

	CONT_PLAYER_MODIFY_ORDER kOrderModify;
	SPMOD_Add_Any AddData(iResultItemNo,iResultItemCount);
	SPMO MODS_Add(IMET_ADD_ANY,pUser->GetID(),AddData);
	kOrderModify.push_back(MODS_Add);

	PgAction_ReqModifyItem kItemModifyAction(CIE_Manufacture, m_kGndKey, kOrderModify);
	kItemModifyAction.DoAction(pUser, NULL);

/*
	BM::Stream kAddonPacket;
	kAddonPacket.Push(iResultItemNo);
	kAddonPacket.Push(iResultItemCount);

	PgAction_ReqModifyItem kItemModifyAction(CIE_Manufacture, m_kGndKey, kOrderModify, kAddonPacket);
	kItemModifyAction.DoAction(pUser, NULL);
*/
	return true;
}

bool PgAction_Manufacture::IsLearnedSkill(CUnit* pUser, CItemDef const* pItemDef)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return false;
	}
	PgMySkill* pkSkill = pkPlayer->GetMySkill();
	if(!pkSkill)
	{
		return false;
	}

	int iMT = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
	switch(iMT)
	{
	case EMANT_SPELL:
		{
			if(!pkSkill->GetLearnedSkill(11901))
			{
				return false;
			}
		}break;
	case EMANT_COOK:
		{
			if(!pkSkill->GetLearnedSkill(11701))
			{
				return false;
			}
		}break;
	case EMANT_WORKMANSHIP:
		{
			if(!pkSkill->GetLearnedSkill(11801))
			{
				return false;
			}
		}break;
	case EMANT_AUTO:
		{
		}break;
	default:
		return false;
	}
	return true;
}

bool PgAction_Manufacture::PopItemByScore(int iItemBagNo, int iLevel, BYTE byScore, int& iOutItemNo, int& iOutItemCount)
{ //0~9������ ���� ���� Rate������ ��ȯ�Ͽ� �ش� �����۹� ���Ҹ� ����
	GET_DEF(CItemBagMgr, kItemBagMgr);
	PgItemBag kItemBag;
	kItemBagMgr.GetItemBag(iItemBagNo, iLevel, kItemBag);

	int iMaxRate = 0;
	PgItemBag::BagElementCont const kContElem = kItemBag.GetElements();
	PgItemBag::BagElementCont::const_iterator iterElem = kContElem.begin();
	for( ; iterElem != kContElem.end(); ++iterElem )
	{
		PgItemBag::SBagElement const* pkElem = &(*iterElem);
		iMaxRate += pkElem->nRate;
	}

	int iRate = ++byScore;
	iRate = iRate * iMaxRate / 10; //byScore�� ������ �ݵ�� 0~9 �̳����� �Ѵ�.
	int iAccumRate = 0;
	for(iterElem = kContElem.begin(); iterElem != kContElem.end(); ++iterElem )
	{
		PgItemBag::SBagElement const* pkElem = &(*iterElem);
		iAccumRate += pkElem->nRate;
		if(iRate <= iAccumRate)
		{
			iOutItemNo = pkElem->iItemNo;
			iOutItemCount = pkElem->nCount;
			return true;
		}
	}


	return false;
}



bool PgAction_BundleManufacture::DoAction(CUnit* pUser, CUnit* pTarget)
{
	if(NULL == pUser)
	{
		return false;
	}

	PgInventory* pInv = pUser->GetInven();
	if(!pInv)
	{//�κ��� �־�¡?
		return false;
	}

	int iItemNo = 0;
	int iCount = 0;
	m_kPacket.Pop(iItemNo);
	m_kPacket.Pop(iCount);
	if(iItemNo == 0 || iCount == 0)
	{
		return false;
	}

	SItemPos kSrcItemPos;
	if(pInv->GetFirstItem(iItemNo, kSrcItemPos) != S_OK)
	{
		return false;
	}
	PgBase_Item kItem;
	if( E_FAIL == pInv->GetItem(kSrcItemPos, kItem) )
	{//�κ��� ������ ������ ����...
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef* pItemDef = kItemDefMgr.GetDef(iItemNo);
	if(!pItemDef)
	{
		return false;
	}

	int iItemBagNo = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2);
	int iLevel = pUser->GetAbil(AT_LEVEL);
	GET_DEF(CItemBagMgr, kItemBagMgr);
	PgItemBag kItemBag;
	kItemBagMgr.GetItemBag(iItemBagNo, iLevel, kItemBag);


	int iResultItemNo = kItemBag.GetElements().at(0).iItemNo;
	int iResultItemCount = kItemBag.GetElements().at(0).nCount;
	//kItemBag.PopItem(iLevel, iResultItemNo, iResultItemCount);




	CONT_PLAYER_MODIFY_ORDER kOrderDelete;
	CONT_PLAYER_MODIFY_ORDER kOrderModify;
	while(pItemDef && iCount--)
	{
		SPMOD_Modify_Count kDelData(kItem, kSrcItemPos, -1);//1�� ����.
		SPMO MODS_Del(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), kDelData);
		kOrderDelete.push_back(MODS_Del);

		SPMOD_Add_Any AddData(iResultItemNo,iResultItemCount);
		SPMO MODS_Add(IMET_ADD_ANY,pUser->GetID(),AddData);
		kOrderModify.push_back(MODS_Add);
	}

	PgAction_ReqModifyItem kItemDeleteAction(CIE_Manufacture, m_kGndKey, kOrderDelete);
	kItemDeleteAction.DoAction(pUser, NULL);

	PgAction_ReqModifyItem kItemModifyAction(CIE_Manufacture, m_kGndKey, kOrderModify);
	kItemModifyAction.DoAction(pUser, NULL);

/*
	CONT_PLAYER_MODIFY_ORDER kOrderDelete;

	SPMOD_Modify_Count kDelData(kItem, kSrcItemPos, -1);//1�� ����.
	SPMO MODS_Del(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pUser->GetID(), kDelData);
	kOrderDelete.push_back(MODS_Del);

	PgAction_ReqModifyItem kItemDeleteAction(CIE_Manufacture, m_kGndKey, kOrderDelete);
	kItemDeleteAction.DoAction(pUser, NULL);

	CONT_PLAYER_MODIFY_ORDER kOrderModify;
	SPMOD_Add_Any AddData(iResultItemNo,iResultItemCount);
	SPMO MODS_Add(IMET_ADD_ANY,pUser->GetID(),AddData);
	kOrderModify.push_back(MODS_Add);

	PgAction_ReqModifyItem kItemModifyAction(CIE_Manufacture, m_kGndKey, kOrderModify);
	kItemModifyAction.DoAction(pUser, NULL);
*/

	return true;
}