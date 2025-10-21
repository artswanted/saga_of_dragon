#include "stdafx.h"
#include "Variant/PgPlayer.h"
#include "Variant/ItemDefMgr.h"
#include "Variant/Item.h"
#include "lohengrin/db.h"
#include "PgSocialUtil.h"

namespace SocialDB
{
	//
	SSocialCharacterKey::SSocialCharacterKey(int const& riRealmNo, BM::GUID const& rkCharGuid)
		: iRealmNo(riRealmNo), kCharGuid(rkCharGuid)
	{
	}
	SSocialCharacterKey::SSocialCharacterKey(BM::Stream& rhs)
	{
		ReadFromPacket(rhs);
	}
	SSocialCharacterKey::SSocialCharacterKey(SSocialCharacterKey const& rhs)
		: iRealmNo(rhs.iRealmNo), kCharGuid(rhs.kCharGuid)
	{
	}
	bool SSocialCharacterKey::operator <(SSocialCharacterKey const& rhs) const
	{
		if( iRealmNo < rhs.iRealmNo )
		{
			return true;
		}
		else if( iRealmNo == rhs.iRealmNo )
		{
			if( kCharGuid < rhs.kCharGuid )
			{
				return true;
			}
		}
		return false;
	}
	void SSocialCharacterKey::PushToQueryStr(CEL::DB_QUERY& kQuery) const
	{
		kQuery.PushStrParam(iRealmNo);
		kQuery.PushStrParam(kCharGuid);
	}


	//
	SSocialCharacterInfo::SSocialCharacterInfo()
	{
	}
	SSocialCharacterInfo::SSocialCharacterInfo(PgPlayer const& rkPlayer)
	{
		iStrength				= rkPlayer.GetAbil(AT_C_STR);
		iIntelligence			= rkPlayer.GetAbil(AT_C_INT);
		iConstitution			= rkPlayer.GetAbil(AT_C_CON);
		iDexterity				= rkPlayer.GetAbil(AT_C_DEX);
		iPhysical_Attack_Min	= rkPlayer.GetAbil(AT_C_PHY_ATTACK_MIN);
		iPhysical_Attack_Max	= rkPlayer.GetAbil(AT_C_PHY_ATTACK_MAX);
		iMagical_Attack_Min		= rkPlayer.GetAbil(AT_C_MAGIC_ATTACK_MIN);
		iMagical_Attack_Max		= rkPlayer.GetAbil(AT_C_MAGIC_ATTACK_MAX);
		iAccuracy_Rate			= rkPlayer.GetAbil(AT_C_HIT_SUCCESS_VALUE);
		iDodge_Rate				= rkPlayer.GetAbil(AT_C_DODGE_SUCCESS_VALUE);
		iPhysical_Defence		= rkPlayer.GetAbil(AT_C_PHY_DEFENCE);
		iMagical_Defence		= rkPlayer.GetAbil(AT_C_MAGIC_DEFENCE);
		iRecovery_HP			= rkPlayer.GetAbil(AT_C_HP_RECOVERY);
		iRecovery_MP			= rkPlayer.GetAbil(AT_C_MP_RECOVERY);
		iMax_HP					= rkPlayer.GetAbil(AT_C_MAX_HP);
		iMax_MP					= rkPlayer.GetAbil(AT_C_MAX_MP);
	}

	//
	SSocialItemBaseStatusInfo::SSocialItemBaseStatusInfo()
	{
		iBasePhysAttackMin		= 0;
		iBasePhysAttackMax		= 0;
		iBaseMagicAttackMin		= 0;
		iBaseMagicAttackMax		= 0;
		iBasePhysDefence		= 0;
		iBaseMagicDefence		= 0;
	}
	SSocialItemBaseStatusInfo::SSocialItemBaseStatusInfo(CAbilObject const& rkAbilObject)
	{
		iBasePhysAttackMin		= rkAbilObject.GetAbil(AT_PHY_ATTACK_MIN);
		iBasePhysAttackMax		= rkAbilObject.GetAbil(AT_PHY_ATTACK_MAX);
		iBaseMagicAttackMin		= rkAbilObject.GetAbil(AT_MAGIC_ATTACK_MIN);
		iBaseMagicAttackMax		= rkAbilObject.GetAbil(AT_MAGIC_ATTACK_MAX);
		iBasePhysDefence		= rkAbilObject.GetAbil(AT_PHY_DEFENCE);
		iBaseMagicDefence		= rkAbilObject.GetAbil(AT_MAGIC_DEFENCE);
	}
	SSocialItemBaseStatusInfo::SSocialItemBaseStatusInfo(CItemDef const& rkItemDef, PgBase_Item const& rkItem)
	{
		iBasePhysAttackMin		= rkItemDef.ImproveAbil(AT_PHY_ATTACK_MIN, rkItem);
		iBasePhysAttackMax		= rkItemDef.ImproveAbil(AT_PHY_ATTACK_MAX, rkItem);
		iBaseMagicAttackMin		= rkItemDef.ImproveAbil(AT_MAGIC_ATTACK_MIN, rkItem);
		iBaseMagicAttackMax		= rkItemDef.ImproveAbil(AT_MAGIC_ATTACK_MAX, rkItem);
		iBasePhysDefence		= rkItemDef.ImproveAbil(AT_PHY_DEFENCE, rkItem);
		iBaseMagicDefence		= rkItemDef.ImproveAbil(AT_MAGIC_DEFENCE, rkItem);
	}
	void SSocialItemBaseStatusInfo::PushToQueryStr(CEL::DB_QUERY& kQuery) const
	{
		kQuery.PushStrParam(iBasePhysAttackMin);
		kQuery.PushStrParam(iBasePhysAttackMax);
		kQuery.PushStrParam(iBaseMagicAttackMin);
		kQuery.PushStrParam(iBaseMagicAttackMax);
		kQuery.PushStrParam(iBasePhysDefence);
		kQuery.PushStrParam(iBaseMagicDefence);
	}

	//
	SSocialItemUserInfo::SSocialItemUserInfo()
	{
	}
	SSocialItemUserInfo::SSocialItemUserInfo(EInvType const eInvType, CItemDef const& rkDefItem, PgBase_Item const& rkItem, int const iPos)
		: kStatusInfo(rkDefItem, rkItem), iRapairMoney(0)
	{
		iItemNo			= rkItem.ItemNo();
		kItemGuid		= (rkDefItem.CanEquip())? rkItem.Guid(): BM::GUID::NullData();
		iCurCount		= rkItem.Count();
		iInvType		= eInvType;
		iInvPos			= iPos;
		iEnchantLevel	= rkItem.EnchantInfo().PlusLv();
		iRarityLevel	= ::GetItemGrade(rkItem);
		iRarityValue	= rkItem.EnchantInfo().Rarity();
		::CalcRepairNeedMoney(rkItem, 0, iRapairMoney);
	}
	

	//
	CONT_SOCIAL_INV_ITEM_ALL::mapped_type& GetSubCont(EInvType const eInvType, CONT_SOCIAL_INV_ITEM_ALL& rkCont)
	{
		CONT_SOCIAL_INV_ITEM_ALL::iterator find_iter = rkCont.find( eInvType );
		if( rkCont.end() == find_iter )
		{
			auto kRet = rkCont.insert( std::make_pair(eInvType, CONT_SOCIAL_INV_ITEM_ALL::mapped_type()) );
			return (*kRet.first).second;
		}
		return (*find_iter).second;
	}
	void PushItemInfo(EInvType const eInvType, EEquipPos const eEquipPos, PgInventory& rkInven, CItemDefMgr const& kItemDefMgr, PgBase_Item& kItem, CONT_SOCIAL_INV_ITEM_ALL::mapped_type& rkContOut)
	{
		if( S_OK == rkInven.GetItem(SItemPos(eInvType, eEquipPos), kItem) )
		{
			CItemDef const* pkDefItem = kItemDefMgr.GetDef(kItem.ItemNo());
			if( pkDefItem )
			{
				rkContOut.push_back( CONT_SOCIAL_INV_ITEM::value_type(eInvType, *pkDefItem, kItem, eEquipPos) );
			}
		}
	}
	void BuildFitItem(CItemDefMgr const& kItemDefMgr, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut)
	{
		EInvType const eInvType = IT_FIT;
		PgBase_Item kItem;
		CONT_SOCIAL_INV_ITEM_ALL::mapped_type& rkContItemInfo = GetSubCont( eInvType, kContOut );
		PushItemInfo(eInvType, EQUIP_POS_SHOULDER, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_CLOAK, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_WEAPON, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_SHEILD, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_NECKLACE, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_EARRING, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_RING, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_RING_R, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_BELT, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_HELMET, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_SHIRTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_PANTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_BOOTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_GLOVE, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_MEDAL, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_PET, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_KICKBALL, rkInven, kItemDefMgr, kItem, rkContItemInfo);
	}
	void BuildFitCastItem(CItemDefMgr const& kItemDefMgr, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut)
	{
		EInvType const eInvType = IT_FIT_CASH;
		PgBase_Item kItem;
		CONT_SOCIAL_INV_ITEM_ALL::mapped_type& rkContItemInfo = GetSubCont( eInvType, kContOut );
		PushItemInfo(eInvType, EQUIP_POS_SHOULDER, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_CLOAK, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_WEAPON, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_SHEILD, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_NECKLACE, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_EARRING, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_RING, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_RING_R, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_BELT, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_HELMET, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_SHIRTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_PANTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_BOOTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_GLOVE, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_MEDAL, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_GLASS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_KICKBALL, rkInven, kItemDefMgr, kItem, rkContItemInfo);
	}

	// Costume by reOiL
	void BuildFitCostumeItem(CItemDefMgr const& kItemDefMgr, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut)
	{
		EInvType const eInvType = IT_FIT_COSTUME;
		PgBase_Item kItem;
		CONT_SOCIAL_INV_ITEM_ALL::mapped_type& rkContItemInfo = GetSubCont(eInvType, kContOut);
		PushItemInfo(eInvType, EQUIP_POS_SHOULDER, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_CLOAK, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_WEAPON, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_SHEILD, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_NECKLACE, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_EARRING, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_RING, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_RING_R, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_BELT, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_HELMET, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_SHIRTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_PANTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_BOOTS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_GLOVE, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_MEDAL, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_GLASS, rkInven, kItemDefMgr, kItem, rkContItemInfo);
		PushItemInfo(eInvType, EQUIP_POS_KICKBALL, rkInven, kItemDefMgr, kItem, rkContItemInfo);
	}
	
	void BuildInvenItem(CItemDefMgr const& kItemDefMgr, EInvType const eInvType, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut)
	{
		typedef std::map< int, CONT_SOCIAL_INV_ITEM::iterator > CONT_ITEMNO_TO_ITER;
		CONT_ITEMNO_TO_ITER kContItemIter;
		CONT_ITEM_CREATE_ORDER kContCharItemInfo;
		if( S_OK == rkInven.GetItems(eInvType, kContCharItemInfo) )
		{
			CONT_SOCIAL_INV_ITEM_ALL::mapped_type& rkContItemInfo = GetSubCont( eInvType, kContOut );
			
			CONT_ITEM_CREATE_ORDER::const_iterator item_iter = kContCharItemInfo.begin();
			while( kContCharItemInfo.end() != item_iter )
			{
				CONT_ITEM_CREATE_ORDER::value_type const& rkItem = (*item_iter);
				CItemDef const* pkDefItem = kItemDefMgr.GetDef(rkItem.ItemNo());
				if( pkDefItem )
				{
					if( pkDefItem->CanEquip() )
					{
						rkContItemInfo.push_back( CONT_SOCIAL_INV_ITEM::value_type(eInvType, *pkDefItem, rkItem) );
					}
					else
					{
						CONT_ITEMNO_TO_ITER::iterator find_info_iter = kContItemIter.find( rkItem.ItemNo() );
						if( kContItemIter.end() == find_info_iter )
						{
							CONT_SOCIAL_INV_ITEM::iterator ret_iter = rkContItemInfo.insert( rkContItemInfo.end(), CONT_SOCIAL_INV_ITEM::value_type(eInvType, *pkDefItem, rkItem) );
							kContItemIter.insert( std::make_pair(rkItem.ItemNo(), ret_iter) );
						}
						else
						{
							(*(*find_info_iter).second).iCurCount += rkItem.Count();
						}
					}
				}
				++item_iter;
			}
		}
	}
};