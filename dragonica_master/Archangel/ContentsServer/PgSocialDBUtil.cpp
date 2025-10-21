#include "stdafx.h"
#include "variant/PgSocialUtil.h"
#include "Global.h"
#include "PgSocialDBUtil.h"

namespace SocialDBUtil
{
	//
	struct SSocialCharacterInfo_CommitHelper : public SocialDB::SSocialCharacterInfo
	{
		explicit SSocialCharacterInfo_CommitHelper(BM::Stream& rhs)
		{
			ReadFromPacket(rhs);
		}
		HRESULT Commit(SocialDB::SSocialCharacterKey const& rkKey) const
		{
			if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_UpdateWebCharacterInfo]");
				rkKey.PushToQueryStr(kQuery);
				kQuery.PushStrParam(iStrength);
				kQuery.PushStrParam(iIntelligence);
				kQuery.PushStrParam(iConstitution);
				kQuery.PushStrParam(iDexterity);
				kQuery.PushStrParam(iPhysical_Attack_Min);
				kQuery.PushStrParam(iPhysical_Attack_Max);
				kQuery.PushStrParam(iMagical_Attack_Min);
				kQuery.PushStrParam(iMagical_Attack_Max);
				kQuery.PushStrParam(iAccuracy_Rate);
				kQuery.PushStrParam(iDodge_Rate);
				kQuery.PushStrParam(iPhysical_Defence);
				kQuery.PushStrParam(iMagical_Defence);
				kQuery.PushStrParam(iRecovery_HP);
				kQuery.PushStrParam(iRecovery_MP);
				kQuery.PushStrParam(iMax_HP);
				kQuery.PushStrParam(iMax_MP);
				kQuery.InsertQueryTarget( rkKey.kCharGuid );
				return g_kCoreCenter.PushQuery(kQuery);
			}

			return S_OK;			
		}
	};

	//
	struct SSocialItemBaseInfo_CommitHelper : public SocialDB::SSocialItemBaseStatusInfo
	{
		SSocialItemBaseInfo_CommitHelper(CItemDef const& rkItemDef)
			: SSocialItemBaseStatusInfo(rkItemDef)
		{
			iItemNo = rkItemDef.No();
			::GetDefString(rkItemDef.NameNo(), kItemName);
			bIsEquipItem = rkItemDef.CanEquip();
			iMaxCount = (bIsEquipItem)? rkItemDef.MaxAmount(): 0;
		}
		HRESULT Commit() const
		{
			if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_UpdateWebBaseItemInfo]");
				kQuery.PushStrParam(iItemNo);
				kQuery.PushStrParam(kItemName);
				kQuery.PushStrParam(bIsEquipItem);
				kQuery.PushStrParam(iMaxCount);
				SocialDB::SSocialItemBaseStatusInfo::PushToQueryStr(kQuery);
				kQuery.InsertQueryTarget( BM::GUID::NullData() );
				return g_kCoreCenter.PushQuery(kQuery);
			}

			return S_OK;			
		}

		int iItemNo;
		std::wstring kItemName;
		bool bIsEquipItem;
		int iMaxCount;
	};

	struct SSocialItemUserInfo_CommitHelper : public SocialDB::SSocialItemUserInfo
	{
		SSocialItemUserInfo_CommitHelper(BM::Stream& rkPacket)
		{
			ReadFromPacket(rkPacket);
		}
		static HRESULT TruncateTable(SocialDB::SSocialCharacterKey const& rkKey)
		{
			if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_DeleteWebCharacterItemInfo]");
				rkKey.PushToQueryStr(kQuery);
				kQuery.InsertQueryTarget( rkKey.kCharGuid );
				return g_kCoreCenter.PushQuery(kQuery);
			}

			return S_OK;			
		}
		HRESULT Commit(SocialDB::SSocialCharacterKey const& rkKey) const
		{
			if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_UpdateWebCharacterItemInfo]");
				rkKey.PushToQueryStr(kQuery);
				kQuery.PushStrParam( iItemNo );
				kQuery.PushStrParam( kItemGuid );
				kQuery.PushStrParam( iCurCount );
				kQuery.PushStrParam( iInvType );
				kQuery.PushStrParam( iInvPos );
				kStatusInfo.PushToQueryStr(kQuery);
				kQuery.PushStrParam( iEnchantLevel );
				kQuery.PushStrParam( iRarityLevel );
				kQuery.PushStrParam( iRarityValue );
				kQuery.PushStrParam( iRapairMoney );
				kQuery.InsertQueryTarget( rkKey.kCharGuid );
				return g_kCoreCenter.PushQuery(kQuery);
			}

			return S_OK;			
		};
	};

	struct SSocialUserCharacterComboInfo_CommitHelper
	{
		SSocialUserCharacterComboInfo_CommitHelper(BM::Stream& rkPacket)
		{
			rkPacket.Pop( iComboCount );
			rkPacket.Pop( kTimeStamp );
		}
		HRESULT Commit(SocialDB::SSocialCharacterKey const& rkKey) const
		{
			if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
			{
				if( 0 < iComboCount )
				{
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_UpdateWebCharacterComboInfo]");
					rkKey.PushToQueryStr(kQuery);
					kQuery.PushStrParam( iComboCount );
					kQuery.PushStrParam( kTimeStamp );
					kQuery.InsertQueryTarget( rkKey.kCharGuid );
					return g_kCoreCenter.PushQuery(kQuery);
				}
			}
			
			return S_OK;
		}

		int iComboCount;
		BM::DBTIMESTAMP_EX kTimeStamp;
	};

	void ProcessPacket(BM::Stream::DEF_STREAM_TYPE const wType, BM::Stream& rkPacket)
	{
		switch( g_kLocal.ServiceRegion() )
		{
		case LOCAL_MGR::NC_KOREA:
		case LOCAL_MGR::NC_DEVELOP:
			{
			}break;
		default:
			{
				return ;
			}break;
		}

		switch( wType )
		{
		case PT_A_N_NFY_CHARACTER_INFO:
			{
				SocialDB::SSocialCharacterKey const kKey(rkPacket);
				SSocialCharacterInfo_CommitHelper const kCharInfo(rkPacket);
				kCharInfo.Commit(kKey);

				SSocialItemUserInfo_CommitHelper::TruncateTable(kKey);
				size_t iInvenCount = 0;
				rkPacket.Pop( iInvenCount );
				while( iInvenCount-- )
				{
					SocialDB::CONT_SOCIAL_INV_ITEM_ALL::key_type eType = IT_NONE;
					rkPacket.Pop( eType );
					size_t iItemCount = 0;
					rkPacket.Pop( iItemCount );
					while( iItemCount-- )
					{
						SSocialItemUserInfo_CommitHelper const kItemInfo(rkPacket);
						kItemInfo.Commit(kKey);
					}
				}
			}break;
		case PT_A_N_NFY_CHARACTER_COMBO_INFO:
			{
				SocialDB::SSocialCharacterKey const kKey(rkPacket);
				SSocialUserCharacterComboInfo_CommitHelper const kComboInfo(rkPacket);
				kComboInfo.Commit(kKey);
			}break;
		case PT_S_T_REQ_DELETE_CHARACTER:
			{
				BM::GUID kMemberGuid, kCharGuid;
				rkPacket.Pop( kMemberGuid );
				rkPacket.Pop( kCharGuid );

				if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
				{
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_DeleteWebCharacterInfo]");
					kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.RealmNo()) );
					kQuery.PushStrParam( kCharGuid );
					kQuery.InsertQueryTarget( kMemberGuid );
					kQuery.InsertQueryTarget( kCharGuid );
					g_kCoreCenter.PushQuery(kQuery);
				}
			}break;
		case PT_A_N_NFY_GROUND_PLAYER_COUNT_INFO:
			{
				int iGroundNo = 0;
				size_t iPlayerCount = 0;
				rkPacket.Pop( iGroundNo );
				rkPacket.Pop( iPlayerCount );

				if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
				{
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_UpdateWebIndunClearUserInfo]");
					kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.RealmNo()) );
					kQuery.PushStrParam( iGroundNo );
					kQuery.PushStrParam( iPlayerCount );
					kQuery.InsertQueryTarget( BM::GUID::NullData() );
					g_kCoreCenter.PushQuery(kQuery);
				}
			}break;
		case PT_N_N_NFY_BUILD_BASE_ITEM_INFO:
			{
				// Character Item Info의 INNER JOIN용 NULL Guid data
				if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
				{
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_NONE_RESULT_PROCESS, L"EXEC dbo.[UP_UpdateWebCharacterItemInfo_Null]");
					kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.RealmNo()) );
					kQuery.InsertQueryTarget( BM::GUID::NullData() );
					g_kCoreCenter.PushQuery(kQuery);
				}
				
				// Def 기준 아이템 기본 정보
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CONT_DEFITEM const* pkContDefItem = NULL;
				g_kTblDataMgr.GetContDef(pkContDefItem);
				if( pkContDefItem )
				{
					CONT_DEFITEM::const_iterator iter = pkContDefItem->begin();
					while( pkContDefItem->end() != iter )
					{
						CItemDef const* pkDefItem = kItemDefMgr.GetDef((*iter).first);
						if( pkDefItem )
						{
							SSocialItemBaseInfo_CommitHelper const kTemp(*pkDefItem);
							kTemp.Commit(); // insert
						}
						++iter;
					}
				}
			}break;
		default:
			{
			}break;
		}
	}
}