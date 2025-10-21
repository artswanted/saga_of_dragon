#include "stdafx.h"
#include "BM/LocalMgr.h"
#include "Global.h"
#include "PgPlayer.h"
#include "Collins/Log.h"
#include "PgLogUtil.h"
#include "tabledatamanager.h"

//
namespace PgLogUtil
{
	//
	PgLogWrapperPlayer::PgLogWrapperPlayer(ELogMainType const eMainType, ELogSubType const eSubType, PgPlayer const& rkEventUser, int const iGroundNo)
		: PgLogCont(eMainType, eSubType, rkEventUser.GetMemberGUID(), rkEventUser.GetID(), rkEventUser.MemberID(), rkEventUser.Name(), rkEventUser.GetAbil(AT_CLASS), rkEventUser.GetAbil(AT_LEVEL), iGroundNo, static_cast< EUnitGender >(rkEventUser.GetAbil(AT_GENDER)))
		, m_kEventUser(rkEventUser)
	{
		m_kChannelNo = rkEventUser.GetChannel();		
		UID(rkEventUser.UID());
	}

	PgLogWrapperPlayer::~PgLogWrapperPlayer()
	{
	}


	//
	PgLogWrapperContents::PgLogWrapperContents(ELogMainType const eMainType, ELogSubType const eSubType, SContentsUser const& rkEventUser)
		: PgLogCont(eMainType, eSubType, rkEventUser.kMemGuid, rkEventUser.kCharGuid, rkEventUser.kAccountID, rkEventUser.kName, rkEventUser.iClass, rkEventUser.sLevel, rkEventUser.kGndKey.GroundNo(), static_cast< EUnitGender >(rkEventUser.cGender))
		, m_kEventUser(rkEventUser)
	{
		m_kChannelNo = rkEventUser.sChannel;
		UID(rkEventUser.iUID);
	}

	PgLogWrapperContents::~PgLogWrapperContents()
	{
	}

	std::wstring const GetPartyOptionItemString(EPartyOptionItem const eValue)
	{
		// Log 남길때 쓰려고 함수.. 
		switch (eValue)
		{
		case POI_LikeSolo:
			{
				return _T("ItemOwner=Picker");
			}break;
		case POI_Order:
			{
				return _T("ItemOwner=sequence");
			}break;
		default:
			{
				return _T("ItemOwner=unknown");
			}break;
		}
		return _T("ItemOwner=unknown");
	}

	std::wstring const GetPartyOptionPublic(EPartyOptionPublicTitle const eValue)
	{
		switch (eValue)
		{
		case POT_Public:
			{
				return _T("Public");
			}break;
		case POT_Private:
			{
				return _T("Private");
			}break;
		default:
			{
				return _T("Unknown_Public");
			}break;
		}
		return  _T("Unknown_Public");
	}

	std::wstring const GetExpeditionOptionItemString(EExpeditionOptionItem const eValue)
	{
		switch(eValue)
		{
		case EOI_LikeSolo:
			{
				return _T("ItemOwner=Picker");
			}break;
		case EOI_Order:
			{
				return _T("ItemOwner=unknown");
			}break;
		default:
			{
				return _T("ItemOwner=unknown");
			}break;
		}
		return _T("ItemOwner=unknown");
	}

	std::wstring const GetExpeditionOptionPublic(EExpeditionOptionPublicTitle const eValue)
	{
		switch(eValue)
		{
		case EOT_Public:
			{
				return _T("Public");
			}break;
		case EOT_Private:
			{
				return _T("Private");
			}break;
		default:
			{
				return _T("Unknown_Public");
			}break;
		}
		return  _T("Unknown_Public");
	}
}


//
namespace PgChatLogUtil
{
	void Log(ELogSubType const eLogSubType, SContentsUser const &rkFromUser, SContentsUser const &rkToUser, std::wstring const &rkContents)
	{
		switch( eLogSubType )
		{
		case ELogSub_Chat_Whisper:
		case ELogSub_Chat_ManToMan:
			{
				if( !rkFromUser.Empty()
				&&	!rkToUser.Empty() )
				{
					PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Chatting, eLogSubType, rkFromUser);
					{
						PgLog kLogSub;
						kLogSub.Set( PgLogUtil::AtIndex(1), rkToUser.kName );
						kLogSub.Set( PgLogUtil::AtIndex(2), rkContents );
						kLogSub.Set( PgLogUtil::AtIndex(3), rkToUser.kMemGuid.str() );
						kLogSub.Set( PgLogUtil::AtIndex(4), rkToUser.kCharGuid.str() );
						kLogAction.Add(kLogSub);
					}
					kLogAction.Commit();
				}
			}break;
		case ELogSub_Chat_Guild:
		case ELogSub_Chat_Friend:
		case ELogSub_Chat_PvpRoom:
		case ELogSub_Chat_Party:
		case ELogSub_Chat_Trade:
			{
				if( !rkFromUser.Empty() )
				{
					PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Chatting, eLogSubType, rkFromUser);
					{
						PgLog kLogSub;
						kLogSub.Set( PgLogUtil::AtIndex(2), rkContents );
						kLogAction.Add(kLogSub);
					}
					kLogAction.Commit();
				}
			}break;
		//case ELogSub_Chat_Nomal: // 이 함수론 출력 안되
		//case ELogSub_Chat_Shout:
		//case ELogSub_Chat_ShoutChannel:
		//case ELogSub_Chat_ShoutRealm:
		//case ELogSub_Chat_TeamChat:
		default:
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("ELogSubType[") << static_cast< int >(eLogSubType) << _T("] is invalid no"));
			}break;
		}
	}

	void Log(ELogSubType const eLogSubType, PgPlayer const* pkPlayer, std::wstring const &rkContents, int const iGroundNo)
	{
		switch( eLogSubType )
		{
		case ELogSub_Chat_Nomal:
		case ELogSub_Chat_TeamChat:
		case ELogSub_Chat_Shout:
		case ELogSub_Chat_ShoutChannel:
		case ELogSub_Chat_ShoutRealm:
			{
				if( pkPlayer )
				{
					PgLogUtil::PgLogWrapperPlayer kLogAction(ELogMain_Contents_Chatting, eLogSubType, *pkPlayer, iGroundNo);
					{
						PgLog kLogSub;
						kLogSub.Set( PgLogUtil::AtIndex(2), rkContents );
						kLogAction.Add(kLogSub);
					}
					kLogAction.Commit();
				}
			}break;
		//case ELogSub_Chat_Whisper: // 이 함수론 출력 안되
		//case ELogSub_Chat_Party:
		//case ELogSub_Chat_Guild:
		//case ELogSub_Chat_Friend:
		//case ELogSub_Chat_PvpRoom:
		//case ELogSub_Chat_ManToMan:
		default:
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("ELogSubType[") << static_cast< int >(eLogSubType) << _T("] is invalid no"));
			}break;
		}
	}
};


namespace PgQuestLogUtil
{
	void ParamLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, int const iQuestID, int const iParamNo, int const iPreVal, int const iNextVal)
	{
		PgLog kSubLog(ELOrderMain_Quest, eType);
		kSubLog.Set( PgLogUtil::AtIndex(1), iQuestID );
		kSubLog.Set( PgLogUtil::AtIndex(2), iParamNo );
		kSubLog.Set( PgLogUtil::AtIndex(3), iPreVal );
		kSubLog.Set( PgLogUtil::AtIndex(4), iNextVal );
		rkContLogMgr.AddLog(rkCharGuid, kSubLog);
	}

	void IngLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, int const iQuestID, int const iVal3, int const iVal4, int const iVal5)
	{
		PgLog kSubLog(ELOrderMain_Quest, eType);
		kSubLog.Set( PgLogUtil::AtIndex(1), iQuestID );
		kSubLog.Set( PgLogUtil::AtIndex(3), iVal3 );
		kSubLog.Set( PgLogUtil::AtIndex(4), iVal4 );
		kSubLog.Set( PgLogUtil::AtIndex(5), iVal5 );
		rkContLogMgr.AddLog(rkCharGuid, kSubLog);
	}

	void EndLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, int const iQuestID)
	{
		PgLog kSubLog(ELOrderMain_Quest, eType);
		kSubLog.Set( PgLogUtil::AtIndex(1), iQuestID );
		rkContLogMgr.AddLog(rkCharGuid, kSubLog);
	}

	void EndLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, std::vector< int > const& rkVec, int const iIgnoreID)
	{
		typedef std::vector< int > ContInt;

		ContInt::const_iterator loop_iter = rkVec.begin();
		while( rkVec.end() != loop_iter )
		{
			int const iID = (*loop_iter);
			if( iIgnoreID != iID )
			{
				EndLog(rkContLogMgr, eType, rkCharGuid, iID);
			}
			++loop_iter;
		}
	}
};

namespace PgPlayerLogUtil
{
	void DeathLog(PgPlayer* pkPlayer, ELogUserDieType const eKillerType, SGroundKey const& rkGndKey, std::wstring const& rkCasterName, BM::GUID const& rkCasterGuid)
	{
		if( pkPlayer )
		{
			PgLogUtil::PgLogWrapperPlayer kLogAction(ELogMain_User_Character, ELogSub_Character_Change_Info, *pkPlayer, rkGndKey.GroundNo());
			{
				BM::vstring vstrPos;
				vstrPos += pkPlayer->GetPos().x;
				vstrPos += _COMMA_;
				vstrPos += pkPlayer->GetPos().y;
				vstrPos += _COMMA_;
				vstrPos += pkPlayer->GetPos().z;

				PgLog kSubLog(ELOrderMain_Die, ELOrderSub_Death);
				kSubLog.Set( PgLogUtil::AtIndex(1), rkCasterName ); // 누가?
				kSubLog.Set( PgLogUtil::AtIndex(2), static_cast<std::wstring>(vstrPos) );
				kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(eKillerType) ); // Why?
				kSubLog.Set( PgLogUtil::AtIndex(3), pkPlayer->GetAbil(AT_GENDER) );
				kSubLog.Set( PgLogUtil::AtIndex(3), rkGndKey.Guid().str() );
				kSubLog.Set( PgLogUtil::AtIndex(4), rkCasterGuid.str() );
				kLogAction.Add(kSubLog);
			}
			kLogAction.Commit();
		}
	}

	void AliveLog(PgPlayer* pkPlayer, ELogUserResurrectType const eResurrectType, SGroundKey const& rkGndKey, std::wstring const& rkCasterName, BM::GUID const& rkCasterGuid, __int64 const iPreExp, __int64 const iResultExp)
	{
		if( pkPlayer )
		{
			PgLogUtil::PgLogWrapperPlayer kLogAction(ELogMain_User_Character, ELogSub_Character_Change_Info, *pkPlayer, rkGndKey.GroundNo() );
			{
				BM::vstring vstrPos;
				vstrPos += pkPlayer->GetPos().x;
				vstrPos += _COMMA_;
				vstrPos += pkPlayer->GetPos().y;
				vstrPos += _COMMA_;
				vstrPos += pkPlayer->GetPos().z;

				PgLog kSubLog(ELOrderMain_Alive, ELOrderSub_Alive);
				kSubLog.Set( PgLogUtil::AtIndex(1), rkCasterName ); // 누가?
				kSubLog.Set( PgLogUtil::AtIndex(2), static_cast<std::wstring>(vstrPos) );
				kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(eResurrectType) ); // Why?
				kSubLog.Set( PgLogUtil::AtIndex(3), pkPlayer->GetAbil(AT_GENDER) );
				kSubLog.Set( PgLogUtil::AtIndex(3), rkGndKey.Guid().str() );
				kSubLog.Set( PgLogUtil::AtIndex(4), rkCasterGuid.str() );
				kSubLog.Set( PgLogUtil::AtIndex(1), iPreExp );
				kSubLog.Set( PgLogUtil::AtIndex(2), iResultExp );
				kSubLog.Set( PgLogUtil::AtIndex(3), (iPreExp - iResultExp) );
				kLogAction.Add(kSubLog);
			}
			kLogAction.Commit();
		}
	}
};

namespace PgItemLogUtil
{
	int ConvertItemPosToLog(SItemPos const& rkItemPos)
	{
		int const iInvenTypeScale = 10000;
		return static_cast< int >( (rkItemPos.x * iInvenTypeScale) + rkItemPos.y );
	}
	void CreateLog(PgLog& kLog, PgItemWrapper const & kCurItem, EOrderMainType const eMainType, EOrderSubType const eSubType)
	{
		kLog.OrderMainType(eMainType);
		kLog.OrderSubType(eSubType);

		std::wstring kItemName;
		if( ::GetItemName(kCurItem.ItemNo(),kItemName) )
		{
			kLog.Set(0,kItemName);
		}

		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kCurItem.ItemNo());
		if(pItemDef)
		{
			if(pItemDef->IsAmountItem())
			{
				kLog.Set(1,_T("COUNTABLE"));
			}
			else
			{
				kLog.Set(1,_T("DURATION"));
			}
		}

		kLog.Set(0,static_cast<int>(kCurItem.ItemNo()));
		kLog.Set(2,static_cast<int>(kCurItem.Count()));
		kLog.Set(4,ConvertItemPosToLog(kCurItem.Pos()));

		kLog.Set(1,kCurItem.EnchantInfo().Field_1());
		kLog.Set(2,kCurItem.EnchantInfo().Field_2());
		kLog.Set(3,kCurItem.EnchantInfo().Field_3());
		kLog.Set(4,kCurItem.EnchantInfo().Field_4());
		kLog.Set(2,kCurItem.Guid().str());
	}
	void CreateLog(PgLogCont& rkLogCont, PgItemWrapper const & kCurItem)
	{
		PgLog kLog;
		CreateLog(kLog, kCurItem);
		rkLogCont.Add(kLog);
	}
	void CreateLog(BM::GUID const & kOwnerGuid,PgContLogMgr &rkContLogMgr,PgItemWrapper const & kCurItem)
	{
		PgLog kLog;
		CreateLog(kLog, kCurItem);
		rkContLogMgr.AddLog(kOwnerGuid,kLog);	// 인벤토리 아이템 수정 로그 등록
	}
	void ModifyLog(BM::GUID const & kOwnerGuid,PgContLogMgr &rkContLogMgr,PgItemWrapper const & kCurItem,PgItemWrapper const & kOldItem)
	{
		PgLog kLog(ELOrderMain_Item,ELOrderSub_Modify);

		std::wstring kItemName;
		if( ::GetItemName(kCurItem.ItemNo(),kItemName) )
		{
			kLog.Set(0,kItemName);
		}

		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kCurItem.ItemNo());
		if(pItemDef)
		{
			if(pItemDef->IsAmountItem())
			{
				kLog.Set(1,_T("COUNTABLE"));
			}
			else
			{
				kLog.Set(1,_T("DURATION"));
			}
		}

		kLog.Set(0,static_cast<int>(kCurItem.ItemNo()));
		kLog.Set(1,static_cast<int>(kOldItem.Count()));
		kLog.Set(2,static_cast<int>(kCurItem.Count()));
		kLog.Set(3,ConvertItemPosToLog(kOldItem.Pos()));
		kLog.Set(4,ConvertItemPosToLog(kCurItem.Pos()));

		kLog.Set(1,kCurItem.EnchantInfo().Field_1());
		kLog.Set(2,kCurItem.EnchantInfo().Field_2());
		kLog.Set(3,kCurItem.EnchantInfo().Field_3());
		kLog.Set(4,kCurItem.EnchantInfo().Field_4());

		kLog.Set(2,kCurItem.Guid().str());

		rkContLogMgr.AddLog(kOwnerGuid,kLog);
	}
	void DeleteLog(BM::GUID const & kOwnerGuid,PgContLogMgr &rkContLogMgr,PgItemWrapper const & kOldItem)
	{
		PgLog kLog(ELOrderMain_Item,ELOrderSub_Delete);

		std::wstring kItemName;
		if( ::GetItemName(kOldItem.ItemNo(),kItemName) )
		{
			kLog.Set(0,kItemName);
		}

		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kOldItem.ItemNo());
		if(pItemDef)
		{
			if(pItemDef->IsAmountItem())
			{
				kLog.Set(1,_T("COUNTABLE"));
			}
			else
			{
				kLog.Set(1,_T("DURATION"));
			}
		}

		kLog.Set(0,static_cast<int>(kOldItem.ItemNo()));
		kLog.Set(2,static_cast<int>(kOldItem.Count()));
		kLog.Set(4,ConvertItemPosToLog(kOldItem.Pos()));

		kLog.Set(1,kOldItem.EnchantInfo().Field_1());
		kLog.Set(2,kOldItem.EnchantInfo().Field_2());
		kLog.Set(3,kOldItem.EnchantInfo().Field_3());
		kLog.Set(4,kOldItem.EnchantInfo().Field_4());
		kLog.Set(2,kOldItem.Guid().str());

		rkContLogMgr.AddLog(kOwnerGuid,kLog);	// 인벤토리 아이템 수정 로그 등록
	}
};