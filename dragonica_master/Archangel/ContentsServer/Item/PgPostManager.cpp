#include "stdafx.h"
#include "variant/PgDbCache.h"
#include "Variant/PgControlDefMgr.h"
#include "item/PgPostManager.h"
#include "PgServerSetMgr.h"

void PgPostManager::OnRecvPT_M_I_POST_REQ_MAIL_SEND(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	CONT_PLAYER_MODIFY_ORDER kContOrder;
	EItemModifyParentEventType kCause;
	std::wstring	kFromName,
					kTargetName;
	pkPacket->Pop(kCause);
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kFromName);
	pkPacket->Pop(kTargetName);
	kContOrder.ReadFromPacket(*pkPacket);

	bool bHaveItem = false;

	for(CONT_PLAYER_MODIFY_ORDER::const_iterator iter = kContOrder.begin();iter != kContOrder.end();++iter)
	{
		if(IMET_CREATE_MAIL != (*iter).Cause())
		{
			continue;
		}

		SSendMailInfo kMailInfo;
		(*iter).Read(kMailInfo);

		bHaveItem = ((true == kMailInfo.ItemGuid().IsNotNull()) || (0 < kMailInfo.Money()));
		break;
	}

	CEL::DB_QUERY kQuery(DT_PLAYER,DQT_POST_CHECK_EANBLE_SEND,L"EXEC [dbo].[up_Post_Check_EanbleSend2]");
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.QueryOwner(kOwnerGuid);
	kQuery.PushStrParam(kFromName);
	kQuery.PushStrParam(kOwnerGuid);
	kQuery.PushStrParam(kTargetName);
	kQuery.PushStrParam(bHaveItem);

	kQuery.contUserData.Push(kCause);
	kQuery.contUserData.Push(kOwnerGuid);
	kContOrder.WriteToPacket(kQuery.contUserData);
	kQuery.contUserData.Push(kGndKey);
	kSI.WriteToPacket(kQuery.contUserData);

	g_kCoreCenter.PushQuery(kQuery);
}

void PgPostManager::OnRecvPT_M_I_POST_REQ_MAIL_RECV(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	__int64 i64StartIndex;
	char	cRequestCount;

	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(i64StartIndex);
	pkPacket->Pop(cRequestCount);

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_GET_MAIL, L"EXEC [dbo].[up_Post_GetUserMail]" );
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.PushStrParam(kOwnerGuid);
	kQuery.PushStrParam(i64StartIndex);
	kQuery.PushStrParam(static_cast<int>(cRequestCount));
	kQuery.QueryOwner(kOwnerGuid);

	kQuery.contUserData.Push(kGndKey);
	kSI.WriteToPacket(kQuery.contUserData);

	g_kCoreCenter.PushQuery(kQuery);
}

void PgPostManager::OnRecvPT_M_I_POST_REQ_MAIL_MODIFY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kMailGuid,kOwnerGuid;
	EItemModifyParentEventType kCause;
	EPostMailModifyType kMailModifyType;

	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kCause);
	pkPacket->Pop(kMailGuid);
	pkPacket->Pop(kMailModifyType);

	if(!Locked_ReserveMail(kMailGuid))
	{
		BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
		kPacket.Push(kOwnerGuid);
		kPacket.Push(PMSR_DB_ERROR);
		SendToGround(kSI,kGndKey,kPacket);
		return;
	}

	switch(kMailModifyType)
	{
	case PMMT_READ:
		{
			//E_RANKPOINT_RECVMAIL_COUNT
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;
				kOrder.push_back(SPMO(IMET_ADD_RANK_POINT, kOwnerGuid, SPMOD_AddRankPoint(E_RANKPOINT_RECVMAIL_COUNT, 1))); // 랭킹 올려.

				BM::Stream kPacket(PT_M_I_REQ_MODIFY_ITEM);
				kPacket.Push( static_cast< int >(CIE_Rank_Point) );
				kPacket.Push( kOwnerGuid );
				kOrder.WriteToPacket(kPacket);
				kPacket.Push( static_cast< bool >(false) ); // No Addon Packet
				::SendToItem(kSI, kGndKey, kPacket);
			}
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_MODIFY_MAIL, L"EXEC [dbo].[UP_Post_ReadMail]" );
			kQuery.InsertQueryTarget(kOwnerGuid);
			kQuery.PushStrParam(kMailGuid);
			kQuery.PushStrParam(static_cast<BYTE>(kMailModifyType));
			kQuery.PushStrParam(kOwnerGuid);

			kQuery.QueryOwner(kOwnerGuid);
			kQuery.contUserData.Push(kGndKey);
			kSI.WriteToPacket(kQuery.contUserData);
			kQuery.contUserData.Push(kMailGuid);

			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PMMT_DELETE:
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_MODIFY_MAIL, L"EXEC [dbo].[UP_Post_DeleteMail]" );
			kQuery.InsertQueryTarget(kOwnerGuid);
			kQuery.PushStrParam(kMailGuid);
			kQuery.PushStrParam(static_cast<BYTE>(kMailModifyType));
			kQuery.PushStrParam(kOwnerGuid);

			kQuery.QueryOwner(kOwnerGuid);
			kQuery.contUserData.Push(kGndKey);
			kSI.WriteToPacket(kQuery.contUserData);
			kQuery.contUserData.Push(kMailGuid);

			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PMMT_RETURN:
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_MODIFY_MAIL, L"EXEC [dbo].[UP_Post_ReturnMail]" );
			kQuery.InsertQueryTarget(kOwnerGuid);
			kQuery.PushStrParam(kMailGuid);
			kQuery.PushStrParam(static_cast<BYTE>(kMailModifyType));
			kQuery.PushStrParam(kOwnerGuid);

			kQuery.QueryOwner(kOwnerGuid);
			kQuery.contUserData.Push(kGndKey);
			kSI.WriteToPacket(kQuery.contUserData);
			kQuery.contUserData.Push(kMailGuid);

			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PMMT_GET_ANNEX:
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_GET_MAIL_ITEM_RESERVE,L"EXEC [dbo].[up_Post_GetMailItemInfo]");
			kQuery.contUserData.Push(kGndKey);
			kQuery.contUserData.Push(kCause);
			kQuery.contUserData.Push(kMailGuid);
			kSI.WriteToPacket(kQuery.contUserData);

			kQuery.PushStrParam(kMailGuid);

			kQuery.QueryOwner(kOwnerGuid);

			g_kCoreCenter.PushQuery(kQuery);
		}break;
	}
}

void PgPostManager::OnRecvPT_M_I_POST_REQ_MAIL_MIN(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_GET_MAIL_MIN, L"EXEC [dbo].[UP_Post_GetUserMailMin2]");
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.PushStrParam(kOwnerGuid);
	kQuery.QueryOwner(kOwnerGuid);
	kQuery.contUserData.Push(kGndKey);
	kSI.WriteToPacket(kQuery.contUserData);

	g_kCoreCenter.PushQuery(kQuery);
}

bool PgPostManager::PostSystemMail(BM::GUID const & kCharGuid,std::wstring const & kTo,std::wstring const & kTitle,std::wstring const & kText,int const iItemNo,short const iItemCount,__int64 const & i64Money)
{
	SContentsUser rkOut;
	if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
	{
		return false;
	}
	return PostSystemMail(kCharGuid,rkOut.Name(),kTo,kTitle,kText,iItemNo,iItemCount,i64Money);
}

bool PgPostManager::PostSystemMail(BM::GUID const & kCharGuid,std::wstring const & kFrom,std::wstring const & kTo,std::wstring const & kTitle,std::wstring const & kText,int const iItemNo,short const iItemCount,__int64 const & i64Money, BM::GUID const & kGMCmdGuid)
{
	PgBase_Item kItem;
	kItem.Clear();
	if(iItemNo > 0 && iItemCount > 0)
	{
		if(E_FAIL == CreateSItem(iItemNo,iItemCount, GIOT_NONE, kItem))
		{
			return false;
		}
	}

	BM::GUID kMailGuid = BM::GUID::Create();

	CEL::DB_QUERY kQueryMail( DT_PLAYER, DQT_POST_ADD_MAIL,L"EXEC [dbo].[up_Post_Create_UserMail]");
	kQueryMail.InsertQueryTarget(kMailGuid);
	kQueryMail.QueryOwner(kMailGuid);

	kQueryMail.PushStrParam(kMailGuid);
	kQueryMail.PushStrParam(kCharGuid);

	if(0 == kTo.length())
	{
		kQueryMail.PushStrParam(kFrom);
	}
	else
	{
		kQueryMail.PushStrParam(kTo);
	}

	kQueryMail.PushStrParam(kTitle);
	kQueryMail.PushStrParam(kText);

	kQueryMail.PushStrParam(kItem.Guid());

	kQueryMail.PushStrParam(i64Money);
	kQueryMail.PushStrParam(false);
	kQueryMail.PushStrParam(kFrom);

	kQueryMail.contUserData.Push(kGMCmdGuid);
	kQueryMail.contUserData.Push(ORDER_CREATE_MAIL);

	g_kCoreCenter.PushQuery(kQueryMail);

	if(!kItem.IsEmpty())
	{
		CEL::DB_QUERY kQueryItem( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Create]");
		kQueryItem.InsertQueryTarget(kMailGuid);
		kQueryItem.QueryOwner(kMailGuid);

		kQueryItem.PushStrParam(kItem.Guid());
		kQueryItem.PushStrParam(kCharGuid);
		kQueryItem.PushStrParam(static_cast<BYTE>(IT_POST));
		kQueryItem.PushStrParam(static_cast<BYTE>(0));
		kQueryItem.PushStrParam(kItem.ItemNo());
		kQueryItem.PushStrParam(kItem.Count());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_1());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_2());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_3());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_4());
		kQueryItem.PushStrParam(kItem.State());
		kQueryItem.PushStrParam(BM::DBTIMESTAMP_EX(kItem.CreateDate()));

		// Stat track by reOiL
		const bool bHasStatTrack = kItem.StatTrackInfo().HasStatTrack();
		kQueryItem.PushStrParam(bHasStatTrack ? AT_STAT_TRACK : 0); // Type01
		kQueryItem.PushStrParam(bHasStatTrack ? 1 : 0);				// Value01
		kQueryItem.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_MON : 0); // Type02
		kQueryItem.PushStrParam(kItem.StatTrackInfo().MonsterKillCount());	   // Value02
		kQueryItem.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_PLAYER : 0); // Type03
		kQueryItem.PushStrParam(kItem.StatTrackInfo().PlayerKillCount());		  // Value03
		kQueryItem.PushStrParam(0); // Type04
		kQueryItem.PushStrParam(0); // Value04
		kQueryItem.PushStrParam(0); // Type05
		kQueryItem.PushStrParam(0); // Value05

		g_kCoreCenter.PushQuery(kQueryItem);
	}
	return true;
}

bool PgPostManager::PostSystemMailByGuid(BM::GUID const & kCharGuid, std::wstring const & kFrom, std::wstring const & kTitle,std::wstring const & kText,int const iItemNo,short const iItemCount,__int64 const & i64Money)
{
	PgBase_Item kItem;
	kItem.Clear();
	if(iItemNo > 0 && iItemCount > 0)
	{
		if(E_FAIL == CreateSItem(iItemNo,iItemCount, GIOT_NONE, kItem))
		{
			return false;
		}
	}

	return PostSystemMailByGuid(kCharGuid, kFrom, kTitle, kText, kItem, i64Money);
}

bool PgPostManager::PostSystemMailByGuid(BM::GUID const & kCharGuid, std::wstring const & kFrom, std::wstring const & kTitle, std::wstring const & kText,PgBase_Item const & kItem, __int64 const & i64Money)
{
	BM::GUID kMailGuid = BM::GUID::Create();

	CEL::DB_QUERY kQueryMail( DT_PLAYER, DQT_POST_ADD_MAIL,L"EXEC [dbo].[up_Post_Create_UserMail_ByGuid]");
	kQueryMail.InsertQueryTarget(kMailGuid);
	kQueryMail.QueryOwner(kMailGuid);

	kQueryMail.PushStrParam(kMailGuid);

	kQueryMail.PushStrParam(kCharGuid);
	kQueryMail.PushStrParam(kCharGuid);

	kQueryMail.PushStrParam(kTitle);
	kQueryMail.PushStrParam(kText);

	kQueryMail.PushStrParam(kItem.Guid());

	kQueryMail.PushStrParam(i64Money);
	kQueryMail.PushStrParam(false);
	kQueryMail.PushStrParam(kFrom);

	g_kCoreCenter.PushQuery(kQueryMail);

	if(!kItem.IsEmpty())
	{
		CEL::DB_QUERY kQueryItem( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Create]");
		kQueryItem.InsertQueryTarget(kMailGuid);
		kQueryItem.QueryOwner(kMailGuid);

		kQueryItem.PushStrParam(kItem.Guid());
		kQueryItem.PushStrParam(kCharGuid);
		kQueryItem.PushStrParam(static_cast<BYTE>(IT_POST));
		kQueryItem.PushStrParam(static_cast<BYTE>(0));
		kQueryItem.PushStrParam(kItem.ItemNo());
		kQueryItem.PushStrParam(kItem.Count());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_1());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_2());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_3());
		kQueryItem.PushStrParam(kItem.EnchantInfo().Field_4());
		kQueryItem.PushStrParam(kItem.State());
		kQueryItem.PushStrParam(BM::DBTIMESTAMP_EX(kItem.CreateDate()));

		// Stat track by reOiL
		const bool bHasStatTrack = kItem.StatTrackInfo().HasStatTrack();
		kQueryItem.PushStrParam(bHasStatTrack ? AT_STAT_TRACK : 0); // Type01
		kQueryItem.PushStrParam(bHasStatTrack ? 1 : 0);				// Value01
		kQueryItem.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_MON : 0); // Type02
		kQueryItem.PushStrParam(kItem.StatTrackInfo().MonsterKillCount());	   // Value02
		kQueryItem.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_PLAYER : 0); // Type03
		kQueryItem.PushStrParam(kItem.StatTrackInfo().PlayerKillCount());		  // Value03
		kQueryItem.PushStrParam(0); // Type04
		kQueryItem.PushStrParam(0); // Value04
		kQueryItem.PushStrParam(0); // Type05
		kQueryItem.PushStrParam(0); // Value05
		
		g_kCoreCenter.PushQuery(kQueryItem);
	}
	return true;
}