#ifndef WEAPON_VARIANT_UTIL_PGSOCIALUTIL_H
#define WEAPON_VARIANT_UTIL_PGSOCIALUTIL_H

namespace SocialDB
{
	struct SSocialCharacterKey
	{
		explicit SSocialCharacterKey(int const& riRealmNo, BM::GUID const& rkCharGuid);
		explicit SSocialCharacterKey(SSocialCharacterKey const& rhs);
		explicit SSocialCharacterKey(BM::Stream& rkPacket);
		bool operator <(SSocialCharacterKey const& rhs) const;
		void PushToQueryStr(CEL::DB_QUERY& kQuery) const;

		DEFAULT_TBL_PACKET_FUNC();

		int iRealmNo;
		BM::GUID kCharGuid;
	};

	struct SSocialCharacterInfo
	{
		explicit SSocialCharacterInfo();
		explicit SSocialCharacterInfo(PgPlayer const& rkPlayer);		
		DEFAULT_TBL_PACKET_FUNC();

		int iStrength;
		int iIntelligence;
		int iConstitution;
		int iDexterity;
		int iPhysical_Attack_Min;
		int iPhysical_Attack_Max;
		int iMagical_Attack_Min;
		int iMagical_Attack_Max;
		int iAccuracy_Rate;
		int iDodge_Rate;
		int iPhysical_Defence;
		int iMagical_Defence;
		int iRecovery_HP;
		int iRecovery_MP;
		int iMax_HP;
		int iMax_MP;
	};

	struct SSocialItemBaseStatusInfo
	{
		explicit SSocialItemBaseStatusInfo();
		explicit SSocialItemBaseStatusInfo(CAbilObject const& rkAbilObject);
		explicit SSocialItemBaseStatusInfo(CItemDef const& rkItemDef, PgBase_Item const& rkItem);
		void PushToQueryStr(CEL::DB_QUERY& kQuery) const;
		DEFAULT_TBL_PACKET_FUNC();

		int iBasePhysAttackMin;
		int iBasePhysAttackMax;
		int iBaseMagicAttackMin;
		int iBaseMagicAttackMax;
		int iBasePhysDefence;
		int iBaseMagicDefence;
	};

	struct SSocialItemUserInfo
	{
		SSocialItemUserInfo();
		SSocialItemUserInfo(EInvType const eInvType, CItemDef const& rkDefItem, PgBase_Item const& rkItem, int const iPos = 0);
		DEFAULT_TBL_PACKET_FUNC();

		int iItemNo;
		BM::GUID kItemGuid;
		int iCurCount;
		int iInvType;
		int iInvPos;
		SSocialItemBaseStatusInfo kStatusInfo;
		int iEnchantLevel;
		int iRarityLevel;
		int iRarityValue;
		int iRapairMoney;
	};
	typedef std::list< SSocialItemUserInfo > CONT_SOCIAL_INV_ITEM;
	typedef std::map< EInvType, CONT_SOCIAL_INV_ITEM > CONT_SOCIAL_INV_ITEM_ALL;

	void BuildFitItem(CItemDefMgr const& kItemDefMgr, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut);
	void BuildFitCastItem(CItemDefMgr const& kItemDefMgr, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut);
	void BuildFitCostumeItem(CItemDefMgr const& kItemDefMgr, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut);
	void BuildInvenItem(CItemDefMgr const& kItemDefMgr, EInvType const eInvType, PgInventory& rkInven, CONT_SOCIAL_INV_ITEM_ALL& kContOut);
};

#endif // WEAPON_VARIANT_UTIL_PGSOCIALUTIL_H	