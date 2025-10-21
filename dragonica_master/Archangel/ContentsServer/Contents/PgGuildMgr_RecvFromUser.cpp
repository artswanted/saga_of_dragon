#include "stdafx.h"
#include "Collins/Log.h"
#include "Variant/PgLogUtil.h"
#include "Variant/PgEmporiaFunction.h"
#include "Global.h"
#include "PgGuildMgr.h"
#include "PgServerSetMgr.h"
#include "JobDispatcher.h"
#include "Variant/PgStringUtil.h"

extern SGuild_Inventory_Log GetGuildInvLog(BM::GUID const & kGuildID, EGuildInvTradeType const eType, __int64 const i64Money);

bool PgGuildMgr::ProcessCommand(int const iCommand, BM::GUID const &rkCharGuid, BM::Stream *pkPacket)
{
	BM::Stream kAPacket;
	kAPacket.Push((BYTE)iCommand);

	switch(iCommand)
	{
	case GC_Create:
	case GC_Info:
	case GC_List:
	case GC_Join:
	case GC_Leave:
	case GC_M_AuthOwner:
	case GC_M_AuthMaster:
	case GC_M_Kick:
	case GC_M_Emblem:
	case GC_M_MemberGrade:
	case GC_M_InventorySupervision:
	//case GC_M_TaxRate:
	case GC_M_Destroy:
	case GC_OtherInfo:
	case GC_M_Rename:
	case GC_ReserveBattleForEmporia:
	case GC_M_LvUp:
	case GC_M_InventoryCreate:
	case GC_InventoryOpen:
	case GC_InventoryClose:	
	case GC_InventoryMoney:
	case GC_M_AddSkill:
	case GC_M_Notice:
	case GC_M_ChangeMark1:
	case GC_SetMercenary:
	case GC_SetGuildEntranceOpen:
	case GC_GuildEntranceApplicant_List:
	case GC_GuildEntranceApplicant_Accept:
	case GC_GuildEntranceApplicant_Reject:
	case GC_InventoryUpdate:
	case GC_InventoryLog:
    case GC_EmporiaThrow:
		{
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkCharGuid, false, kAPacket) )
			{
				return false;
			}
		}break;
	case GC_M_ChangeOwner:
	case GC_ReqJoin:
		{
			BM::GUID kSubCharGuid;
			pkPacket->Pop(kSubCharGuid);

			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkCharGuid, false, kAPacket) )
			{
				return false;
			}
			::WritePlayerInfoToPacket_ByGuid(kSubCharGuid, false, kAPacket);
			//{
			//	return false;
			//}
		}break;
	case GC_ReqJoin_Name:
		{
			std::wstring kCharName;
			pkPacket->Pop(kCharName);

			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkCharGuid, false, kAPacket) )
			{
				return false;
			}
			::WritePlayerInfoToPacket_ByName(kCharName, kAPacket); // 못찾으면 이후 처리를 위해서 통과한다
		}break;
	case GC_UpdateGuildList:
		{
		}break;
	case GC_None:
	case GC_GM_Rename:
	case GC_GM_ChangeOwner:
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Invalid Guild Command [") << iCommand << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	kAPacket.Push(*pkPacket);
	return ProcessMsgFromUser(&kAPacket);
}

inline bool IsChallengeCostLimit(BM::GUID const & kEmporiaID, __int64 const i64Cost)
{
	CONT_DEF_EMPORIA const *pkDefEmporia = NULL;
	g_kTblDataMgr.GetContDef( pkDefEmporia );
	if(pkDefEmporia)
	{
		CONT_DEF_EMPORIA::const_iterator emporia_itr = pkDefEmporia->find( kEmporiaID );
		if ( emporia_itr != pkDefEmporia->end() )
		{
			if( i64Cost >= (*emporia_itr).second.iLimitCost)
			{
				return true;
			}
		}
	}
	return false;
}

bool PgGuildMgr::ProcessMsgFromUser(BM::Stream *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !pkMsg )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SContentsUser kMaster;
	SContentsUser kUser;
	BM::GUID kGuildGuid;
	BYTE cCmdType = 0;

	pkMsg->Pop(cCmdType);
	BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
	switch(cCmdType)
	{
	case GC_Create:
		{
			// Guild 생성 요청
			std::wstring kGuildName;
			BYTE cEmblem = 0;

			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildName);
			pkMsg->Pop(cEmblem);

			PgStringUtil::Trim<std::wstring>( kGuildName, L" " );

			EGuildCommandRet eRet = Create(kMaster, kGuildName, cEmblem);
			if( GCR_Success != eRet )
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("[GUILD-Create: ") << kMaster.Name().c_str() << _T("] Failed - Can't Create [pre DB job] Ret: [") << static_cast<int>(eRet) << _T("]") );
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_ReqJoin:
	case GC_ReqJoin_Name:
		{
			kMaster.ReadFromPacket(*pkMsg);
			kUser.ReadFromPacket(*pkMsg);

			EGuildCommandRet eRet = GCR_Error;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				eRet = ReqJoin(kMaster, kGuildGuid, kUser);
				if( GCR_Success == eRet )
				{
					AddCharToGuild(kUser.kCharGuid, kGuildGuid);
				}
			}
			
			if( GCR_Success != eRet )//
			{
				BM::Stream kRetPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast< BYTE >(GC_ReqJoin));//Return Code
				kRetPacket.Push(static_cast< BYTE >(eRet));
				switch( eRet ) // 오류에 따른 추가 정보
				{
				case GCR_Date_Limit:
					{
						kRetPacket.Push( PgGuildMgrUtil::iAfterCanRejoinHour );
					}break;
				default:
					{
					}break;
				}
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kRetPacket);
			}
		}break;
	case GC_InventoryLog:
		{	
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);
			if( BM::GUID::NullData() == kGuildGuid )
			{
				break;
			}

			int iPage = 0;
			pkMsg->Pop(iPage);

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_INVENTORY_LOG_SELECT, _T("EXEC [dbo].[up_SELECT_Guild_Inventory_Log]"));
			kQuery.InsertQueryTarget(kGuildGuid);
			kQuery.QueryOwner(kUser.kCharGuid);

			kQuery.PushStrParam(kGuildGuid);
			kQuery.PushStrParam(iPage); // 페이지 번호 요청(0베이스 아님)
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case GC_InventoryUpdate:
		{
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);
			if( BM::GUID::NullData() == kGuildGuid )
			{
				break;//씹자
			}

			EGuildCommandRet eRet = GCR_Failed;
			PgGuild* pkGuild = NULL;
			PgInventory* pkGuildInv = NULL;
			PgInventory* pkUserInv = NULL;
			PgBase_Item kCasterItem;
			PgBase_Item kTargetItem;

			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				pkGuildInv = pkGuild->GetInven();
				if( !pkGuildInv )
				{
					return false;
				}

				PgDoc_Player rkCopyPlayer;
				if( g_kRealmUserMgr.Locked_GetDoc_Player(kUser.kCharGuid, false, rkCopyPlayer) )
				{
					pkUserInv = rkCopyPlayer.GetInven();
					if( !pkUserInv )
					{
						return false;
					}
				}
				else
				{
					return false;
				}

				int iType;
				__int64 i64Money = 0i64;
				SItemPos kSourcePos;
				SItemPos kTargetPos;
				DWORD dwClientTime;
				BM::GUID kNpcGuid;

				pkMsg->Pop(iType);

				// 맵서버로 보낼 패킷
				BM::Stream kMPacket(PT_N_M_REQ_ITEM_CHANGE_GUILD);
				kMPacket.Push(kGuildGuid);
				kMPacket.Push(kUser.kCharGuid);
				kMPacket.Push(iType);

				// 로그 전용
				BM::GUID kItemGuid;
				int iItemNo = 0;
				__int64 i64Count = 0i64;

				switch( iType )
				{
				case EGIT_MONEY_IN:
				case EGIT_MONEY_OUT:
					{
						pkMsg->Pop(i64Money);
						if( i64Money <= 0 )
						{// 1골드 이상 입력해 주세요.
							kPacket.Push((BYTE)GCR_Error);
							g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
							return false;
						}

						kMPacket.Push(i64Money);

						kItemGuid = BM::GUID::NullData();
						iItemNo = 0;
						i64Count = i64Money;
					}break;
				case EGIT_ITEM_IN:
				case EGIT_ITEM_OUT:
				case EGIT_ITEM_MOVE:
					{
						pkMsg->Pop( kSourcePos );
						pkMsg->Pop( kTargetPos );
						pkMsg->Pop( dwClientTime );
						pkMsg->Pop( kNpcGuid );

						kMPacket.Push(kSourcePos);
						kMPacket.Push(kTargetPos);
						kMPacket.Push(dwClientTime);
						kMPacket.Push(kNpcGuid);

						if( EGIT_ITEM_OUT == iType ) // 길드금고에서 꺼내는거니까 캐스터: 길드인벤 타겟: 유저인벤
						{
							if( FAILED( pkGuildInv->GetItem( kSourcePos, kCasterItem ) ) )
							{
								return false;
							}
							
							if( SUCCEEDED( pkUserInv->GetItem( kTargetPos, kTargetItem ) ) )
							{// 대상위치에 아이템이 있으면 안된다.(머징,위치교환 허용안한다)
								kPacket.Push((BYTE)GCR_NotInvMove);
								g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
								return false;
							}
							
							kCasterItem.WriteToPacket(kMPacket); // 길드
							kTargetItem.WriteToPacket(kMPacket); // 유저
						}
						else if( EGIT_ITEM_IN == iType ) // 길드금고에 넣는거니까 캐스터: 유저인벤 타겟: 길드인벤
						{
							if( FAILED( pkUserInv->GetItem( kSourcePos, kCasterItem ) ) )
							{
								return false;
							}

							if( SUCCEEDED( pkGuildInv->GetItem( kTargetPos, kTargetItem ) ) )
							{// 대상위치에 아이템이 있으면 안된다.(머징,위치교환 허용안한다)
								kPacket.Push((BYTE)GCR_NotInvMove);
								g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
								return false;
							}

							kCasterItem.WriteToPacket(kMPacket); // 유저
							kTargetItem.WriteToPacket(kMPacket); // 길드
						}
						else
						{// 길드금고 내 이동
							if( FAILED( pkGuildInv->GetItem( kSourcePos, kCasterItem ) ) )
							{
								return false;
							}
							
							if( SUCCEEDED( pkGuildInv->GetItem( kTargetPos, kTargetItem ) ) )
							{// 대상위치에 아이템이 있으면 안된다.(머징,위치교환 허용안한다)
								kPacket.Push((BYTE)GCR_NotInvMove);
								g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
								return false;
							}
							
							kCasterItem.WriteToPacket(kMPacket); // 길드
							kTargetItem.WriteToPacket(kMPacket); // 길드
						}

						// 로그 전용: 항상 캐스터의 아이템 이다.(캐스터는 길드일 수도, 유저 인벤일수도) ==> 행동의 주체
						kItemGuid = kCasterItem.Guid();
						iItemNo = kCasterItem.ItemNo();

						GET_DEF(CItemDefMgr, kItemDefMgr);
						CItemDef const *pItemDef = kItemDefMgr.GetDef(kCasterItem.ItemNo());
						if( pItemDef )
						{
							if( false == pItemDef->IsAmountItem() )
							{
								i64Count = 1; // 내구도 아이템은 1개
							}
							else
							{
								i64Count = kCasterItem.Count();
							}
						}
					}break;
				default:
					{
					}break;
				}

				switch( iType )
				{
				case EGIT_MONEY_IN:
					{// 길드금고 골드 입금
						if( !pkGuild->IsHaveGuildInvAuthority_In(kUser.kCharGuid) )
						{
							kPacket.Push((BYTE)GCR_NotInvAuth);//권한이 없습니다.
							g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
							return false;
						}
						PgDoc_Player rkCopyPlayer;
						if( g_kRealmUserMgr.Locked_GetDoc_Player(kUser.kCharGuid, false, rkCopyPlayer) )
						{
							__int64 i64PlayerMoney = rkCopyPlayer.GetAbil64(AT_MONEY);
							if( i64PlayerMoney < i64Money )
							{
								kPacket.Push((BYTE)GCR_Money); // 지가 가진 골드보다 많은 골드를 넣으려고 한다.
								g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
								return false;
							}
						}
						else
						{
							return false;
						}
					}break;
				case EGIT_MONEY_OUT:
					{// 길드글고 골드 출금
						if( !pkGuild->IsHaveGuildInvAuthority_Out(kUser.kCharGuid) )
						{
							kPacket.Push((BYTE)GCR_NotInvAuth);//권한이 없습니다.
							g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
							return false;
						}
						__int64 i64GuildMoney = pkGuildInv->Money();
						if( i64GuildMoney < i64Money )
						{	
							kPacket.Push((BYTE)GCR_Money); // 길드금고의 골드보다 큰 골드를 요구했다.
							g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
							return false;
						}
					}break;
				case EGIT_ITEM_OUT:
					{//길드금고 아이템 꺼내기(길드금고 ==> 인벤토리)
						if( !pkGuild->IsHaveGuildInvAuthority_Out(kUser.kCharGuid) )
						{
							kPacket.Push((BYTE)GCR_NotInvAuth);//권한이 없습니다.
							g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
							return false;
						}
						else
						{// 인벤토리의 올바른 위치인가?
							GET_DEF(CItemDefMgr, kItemDefMgr);
							CItemDef const *pItemDef = kItemDefMgr.GetDef(kCasterItem.ItemNo());
							if( pItemDef )
							{
								if( pItemDef->PrimaryInvType() != kTargetPos.x )//맞는 인벤?
								{
									kPacket.Push((BYTE)GCR_NotInvMove);//이동할 수 없는 위치
									g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
									return false;
								}
							}
						}

						if( FAILED( pkGuildInv->GetItem( kSourcePos, kCasterItem ) ) )
						{
							return false;
						}

						if( !pkGuild->AddFixingItem( kSourcePos ) ) // 수정중인 길드금고의 아이템의 위치
						{// 수정중이면 딴 놈이 못건드려.
							return false;
						}
					}break;
				case EGIT_ITEM_IN:
					{// 길드금고 아이템 보관(인벤토리 ==> 길드금고)
						if( !pkGuild->IsHaveGuildInvAuthority_In(kUser.kCharGuid) )
						{
							kPacket.Push((BYTE)GCR_NotInvAuth);//권한이 없습니다.
							g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
							return false;
						}

						if( !pkGuild->AddFixingItem( kTargetPos ) ) // 길드금고의 이동할 위치
						{// 수정중이면 딴 놈이 못건드려.
							return false;
						}
					}break;
				case EGIT_ITEM_MOVE:
					{	
						if( !pkGuild->AddFixingItem( kSourcePos, kTargetPos ) )						
						{// 수정중이면 딴 놈이 못건드려.
							return false;
						}
					}break;
				default:
					{
						return false;
					}break;
				}

				switch( iType )
				{//길드금고 전용 로그 TB_Guild_Inventory_Log
				case EGIT_ITEM_IN:
				case EGIT_ITEM_OUT:
				case EGIT_MONEY_IN:
				case EGIT_MONEY_OUT:
					{
						kMPacket.Push(true);

						BM::DBTIMESTAMP_EX kNowTime;
						kNowTime.SetLocalTime();					
						SGuild_Inventory_Log kLog( kGuildGuid, kItemGuid, kNowTime, kUser.Name(), static_cast<BYTE>(iType), iItemNo, i64Count );
						kLog.WriteToPacket(kMPacket);
					}break;
				default:
					{
						kMPacket.Push(false);
					}break;
				}

				g_kRealmUserMgr.Locked_SendToUserGround( kUser.kCharGuid, kMPacket, false, true);
			}
		}break;
	case GC_OtherInfo: //남에 길드 정보 요청
		{
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);
			if( BM::GUID::NullData() == kGuildGuid )
			{
				break;//씹자
			}

			EGuildCommandRet eRet = GCR_Failed;
			PgGuild* pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				SGuildOtherInfo const kInfo = pkGuild->GetOtherInfo();

				kPacket.Push((BYTE)GCR_Success);
				kInfo.WriteToPacket(kPacket);
				eRet = GCR_Success;
			}
			if (eRet != GCR_Success)
			{
				kPacket.Push((BYTE)GCR_Wait);//없는거다 -_-;;
				kPacket.Push(kGuildGuid);

				m_kOtherWaiter.AddWaiter(kGuildGuid, kUser.kMemGuid);//기다려라
			}
			g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
		}break;
	case GC_List:
	case GC_Info:
	case GC_GuildEntranceApplicant_List:
		{
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);

			PgGuild *pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				kPacket.Push((BYTE)GCR_Success);
				switch( cCmdType )
				{
				case GC_List:
					{
						pkGuild->WriteListToPacket(kPacket);
						g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);//전송
					}break;
				case GC_Info:
					{
						pkGuild->WriteInfoToPacket(kPacket);
						g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);//전송
					}break;
				case GC_GuildEntranceApplicant_List:
					{
						g_kGuildMgr.WriteGuildEntranceApplicantListToPacket(kGuildGuid,kPacket);
						g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
					}break;
				default:
					{
					}break;
				}
			}
			else
			{
				// 아무튼 못 찾았다.
				m_kWaiter.AddWaiter(kGuildGuid, kUser.kMemGuid);//대기 해달라 부탁하고

				kPacket.Push( static_cast<BYTE>(GCR_Wait) );
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
			}
		}break;
	case GC_UpdateGuildList:
		{
			pkMsg->Pop(kGuildGuid);

			PgGuild *pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				kPacket.Push((BYTE)GCR_Moved);

				pkGuild->WriteMemberListToPacket(kPacket, kUser.kCharGuid);
				SendToGuild(kGuildGuid, kPacket);
			}
			else
			{
				// 아무튼 못 찾았다.
				m_kWaiter.AddWaiter(kGuildGuid, kUser.kMemGuid);//대기 해달라 부탁하고

				kPacket.Push( static_cast<BYTE>(GCR_Wait) );
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
			}
		}break;
	case GC_Join:
		{
			bool bAccept = false;
			__int64 iMoney = 0i64; // 길드가입 시스템을 통해 가입할 때만 가입비용이 발생 할 수 있다.

			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(bAccept);//가입 할꺼냐?
			pkMsg->Pop(iMoney);

			PgGuild *pkGuild = NULL;
			EGuildCommandRet const eRet = Join(kGuildGuid, kUser, bAccept, GCR_None);
			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
			}

			if( 0i64 < iMoney 
			&&	GCR_Success == eRet )
			{// 길드 가입비용이 있다면 맵서버로 보내서 오더를 만들자.				
				BM::Stream kMPacket(PT_N_M_REQ_ITEM_CHANGE_GUILD);
				kMPacket.Push(kGuildGuid);
				kMPacket.Push(kUser.kCharGuid);
				kMPacket.Push(EGIT_MONEY_IN);
				kMPacket.Push(iMoney);
				kMPacket.Push(true); // 로그를 남기자.

				BM::DBTIMESTAMP_EX kNowTime;
				kNowTime.SetLocalTime();					
				SGuild_Inventory_Log kLog( kGuildGuid, BM::GUID::NullData(), kNowTime, kUser.Name(), EGIT_MONEY_IN, 0, iMoney );
				kLog.WriteToPacket(kMPacket);
				g_kRealmUserMgr.Locked_SendToUserGround( kUser.kCharGuid, kMPacket, false, true);
			}
		}break;
	case GC_Leave://길드 탈퇴
		{
			kUser.ReadFromPacket(*pkMsg);
			EGuildCommandRet const eRet = Leave(kUser);
			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
			}
			else
			{// 배신자 업적
				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(kUser.kCharGuid);
				pkActionOrder->kCause = CAE_Achievement;
				pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kUser.kCharGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_BETRAYER,1)));
				g_kJobDispatcher.VPush(pkActionOrder);
			}
		}break;
	case GC_M_AuthOwner:
	case GC_M_AuthMaster:
		{
			kMaster.ReadFromPacket(*pkMsg);
			
			EGuildCommandRet eRet = GCR_Failed;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					switch( cCmdType )
					{
					case GC_M_AuthOwner:
						{
							if( pkGuild->IsOwner(kMaster.kCharGuid) )//마스터 인증
							{
								eRet = GCR_Success;
							}
						}break;
					case GC_M_AuthMaster:
						{
							if( pkGuild->IsMaster(kMaster.kCharGuid) )//마스터 인증
							{
								eRet = GCR_Success;
							}
						}break;
					}
				}
			}

			if( GCR_Success == eRet )//항상전송
			{
				BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND);
				kMapPacket.Push( static_cast< BYTE >(GC_M_ChangeMark1) );
				kMapPacket.Push( static_cast< BYTE >(GCR_None) );
				kMapPacket.Push( kMaster.kCharGuid );
				kMapPacket.Push< BM::Stream >( *pkMsg ); // 이후 작업본 잉계
				g_kRealmUserMgr.Locked_SendToUserGround(kMaster.kMemGuid, kMapPacket, true, true);
			}
			else
			{
				kPacket.Push((BYTE)GCR_NotAuth); // 실패 메시지
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Kick:
		{
			BM::GUID kCharGuid;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kCharGuid);

			EGuildCommandRet const eRet = Kick(kMaster.kGuildGuid, kMaster, kCharGuid);
			if( GCR_Success != eRet )
			{
				// 실패했으면, 요청자에게 실패 메시지 보여주기
				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_M_Kick);
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_InventorySupervision:
		{	
			BYTE byAuthority_In = 0;
			BYTE byAuthority_Out = 0;

			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(byAuthority_In);
			pkMsg->Pop(byAuthority_Out);

			EGuildCommandRet eRet = GCR_Failed;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kMaster.kMemGuid);
					kDBPacket.Push((BYTE)GC_M_InventorySupervision);
					kDBPacket.Push(pkGuild->Guid());					
					kDBPacket.Push(byAuthority_In);
					kDBPacket.Push(byAuthority_Out);
					SendToGuildMgr(kDBPacket);
				}
			}
			if( GCR_Success !=  eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Grade:
		{
			BYTE cGradeLevel = 0;
			SGuildMemberGradeInfo kGradeInfo;
			kMaster.ReadFromPacket(*pkMsg);
			kGradeInfo.ReadFromPacket(*pkMsg);
			kGradeInfo.kLastModifyDate.LocalTime();
			
			EGuildCommandRet eRet = GCR_Failed;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					if( pkGuild->IsMaster(kMaster.kCharGuid) )
					{
						BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kMaster.kMemGuid);
						kDBPacket.Push((BYTE)cCmdType);
						kGradeInfo.WriteToPacket(kDBPacket);
						SendToGuildMgr(kDBPacket);
					}
					else
					{
						kPacket.Push((BYTE)GCR_NotAuth);
						g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
					}
				}
			}
		}break;
	case GC_M_MemberGrade:
		{
			BM::GUID kCharGuid;
			BYTE cGuildMemberGrade = 0;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(cGuildMemberGrade);

			EGuildCommandRet eRet = GCR_Failed;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					eRet = pkGuild->TestSetGrade(kMaster.kCharGuid, kCharGuid, cGuildMemberGrade);
					if( GCR_Success == eRet )
					{
						BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kMaster.kMemGuid);
						kDBPacket.Push((BYTE)GC_M_MemberGrade);
						kDBPacket.Push(pkGuild->Guid());
						kDBPacket.Push(kCharGuid);
						kDBPacket.Push(cGuildMemberGrade);
						SendToGuildMgr(kDBPacket);
					}
				}
			}

			if( GCR_Success !=  eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Emblem:
		{
			kMaster.ReadFromPacket(*pkMsg);
			//pkMsg->Pop(엠블렘 데이터를 받고);

			EGuildCommandRet eRet = GCR_Failed;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
			{
				PgGuild *pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					if( pkGuild->IsMaster(kMaster.kCharGuid) )
					{
						//if( pkGuild->SetEmblem() )
						{
							//TODO: 엠블렘 파일을 저장
							//		길드 전체에 엠블렘 설정
							eRet = GCR_Success;
						}
						// DB 또는 서버쪽에 엠블렘 저장...
						// View에 Broadcasting
					}
				}
			}
			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	//case GC_M_TaxRate:
	//	{
	//		BYTE cTaxRate = 0;

	//		kMaster.ReadFromPacket(*pkMsg);
	//		pkMsg->Pop(cTaxRate);

	//		EGuildCommandRet eRet = GCR_Failed;
	//		if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
	//		{
	//			PgGuild *pkGuild = NULL;
	//			if( S_OK == Get(kGuildGuid, pkGuild) )
	//			{
	//				if ( pkGuild->IsOwner(kMaster.kCharGuid) )
	//				{
	//					if( cTaxRate != pkGuild->TaxRate() )
	//					{
	//						if( GE_TaxRate_Max < cTaxRate )
	//						{
	//							BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kMaster.kMemGuid);
	//							kDBPacket.Push((BYTE)GC_M_TaxRate);
	//							kDBPacket.Push(pkGuild->Guid());
	//							kDBPacket.Push(cTaxRate);//New Tax Rate
	//							SendToGuildMgr(kDBPacket);

	//							eRet = GCR_Success;
	//						}
	//						else
	//						{
	//							eRet = GCR_Max;
	//						}
	//					}
	//					else
	//					{
	//						eRet = GCR_Same;
	//					}
	//				}
	//				else
	//				{
	//					eRet = GCR_NotAuth;
	//				}
	//			}
	//		}

	//		if( GCR_Success != eRet )
	//		{
	//			kPacket.Push((BYTE)eRet);
	//			g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
	//		}
	//	}break;
	case GC_M_Destroy:
		{
			kMaster.ReadFromPacket(*pkMsg);

			EGuildCommandRet eRet = GCR_Failed;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					if ( pkGuild->IsMaster(kMaster.kCharGuid) )
					{
						if ( EMPORIA_KEY_NONE != pkGuild->EmporiaInfo().byType )
						{
							eRet = GCR_HaveEmporia;
						}
						else if(1 == pkGuild->MemberCount())//마스터 혼자 만 있을 때
						{
							//DB쿼리
							BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kMaster.kCharGuid);
							kDBPacket.Push((BYTE)GC_M_Destroy);
							kDBPacket.Push(pkGuild->Guid());
							SendToGuildMgr(kDBPacket);

							eRet = GCR_Success;
						}
						else
						{
							eRet = GCR_NotEmpty;
						}
					}
				}
			}

			if(GCR_Success != eRet)
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Rename:
		{
			std::wstring kNewGuildName;
			bool bGMCmd = false;

			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kNewGuildName);			
			PgStringUtil::Trim<std::wstring>( kNewGuildName, L" " );

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kUser.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					eRet = ReqRename(pkGuild, kUser, kNewGuildName);
				}
			}
			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
			}
		}break;
	case GC_ReserveBattleForEmporia:
		{
			kUser.ReadFromPacket(*pkMsg);

			if( GetCharToGuild(kUser.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if ( SUCCEEDED( Get(kGuildGuid, pkGuild) ) )
				{
					if( pkGuild->IsOwner(kUser.kCharGuid) )
					{
						SGuildEmporiaInfo const &kEmporiaInfo = pkGuild->EmporiaInfo();

						bool bThrow = false;// 포기유무(false=도전, true=도전포기)
						pkMsg->Pop( bThrow );

						if ( bThrow )
						{
							// 예약되어 있는 엠포리아 쟁탈전을 포기한거다.
							if ( EMPORIA_KEY_BATTLERESERVE == kEmporiaInfo.byType )
							{
								SEmporiaChallenge	kChallenge;
								kChallenge.kGuildInfo.kGuildID = kGuildGuid;
								kChallenge.kGuildInfo.nEmblem = pkGuild->Emblem();
								kChallenge.kGuildInfo.wstrName = pkGuild->Name();

								BM::Stream kNPacket( PT_N_N_REQ_EMPORIA_BATTLE_RESERVE, bThrow );
                                kNPacket.Push( kUser.kCharGuid );
								kChallenge.WriteToPacket( kNPacket );

								::SendToRealmContents( PMET_EMPORIA, kNPacket );
							}
						}
						else
						{
							BM::GUID kEmporiaID;
							__int64 i64CostMoney = 0i64;
							pkMsg->Pop( kEmporiaID );
							pkMsg->Pop( i64CostMoney );

							// 보유한 엠포리아가 없는지 확인한다.
							// 보유한 엠포리아가 있으면 신청 할 수 없다.
							if ( EMPORIA_KEY_NONE == kEmporiaInfo.byType )
							{
								// 길드 레벨을 체크한다.
								if ( pkGuild->GuildLevel() > 1 )
								{
									if ( IsChallengeCostLimit(kEmporiaID, i64CostMoney) )
									{
                                    	SEmporiaChallenge	kChallenge;
										kChallenge.kGuildInfo.kGuildID = kGuildGuid;
										kChallenge.kGuildInfo.nEmblem = pkGuild->Emblem();
										kChallenge.kGuildInfo.wstrName = pkGuild->Name();
										kChallenge.i64Cost = i64CostMoney;
										kChallenge.i64ChallengeTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

                                        {// 길드 인벤에서 골드 빼기
                                            SGuild_Inventory_Log kLog( GetGuildInvLog(kGuildGuid, EGIT_EMPORIA_RESERVE, i64CostMoney) );

                                            BM::Stream kPacket;
                                            kLog.WriteToPacket(kPacket);
                                            kPacket.Push(kUser.kCharGuid);
                                            kPacket.Push(kEmporiaID);
                                            kChallenge.WriteToPacket( kPacket );

                                            SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				                            pkActionOrder->InsertTarget(kUser.kCharGuid);
				                            pkActionOrder->kCause = CIE_EmporiaReserve;
				                            pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_MONEY|IMC_GUILD_INV,SModifyOrderOwner(kGuildGuid,OOT_Guild),SPMOD_Add_Money(-i64CostMoney)) );
                                            pkActionOrder->kAddonPacket.Push(kPacket.Data());                                            
				                            g_kJobDispatcher.VPush(pkActionOrder);
		                                }
									}
									else
									{
										// 길드골드가 부족하여 신청할 수 없다.
										BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReserveBattleForEmporia );
										kAnsPacket.Push((BYTE)GCR_LimitCost);
										g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kAnsPacket, false );
									}
								}
								else
								{
									// 길드 레벨이 부족하여 신청할 수 없다.
									BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReserveBattleForEmporia );
									kAnsPacket.Push((BYTE)GCR_Level);
									g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kAnsPacket, false );
								}
							}
							else
							{// 보유하거나 신청중에 있어서 신청할 수 없다.
								BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReserveBattleForEmporia );
								kAnsPacket.Push( (BYTE)GCR_Duplicate );
								g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kAnsPacket, false );
							}	
						}
					}
					else
					{
						// 길마가 아니면 신청 할 수 없다
						BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReserveBattleForEmporia );
						kAnsPacket.Push((BYTE)GCR_NotAuth);
						g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
					}
				}
				else
				{
					assert(false);
				}
			}
		}break;
    case GC_EmporiaThrow:
        {
            __int64 i64Money = 0i64;
			kMaster.ReadFromPacket(*pkMsg);
            pkMsg->Pop(i64Money);

            PgGuild *pkGuild = NULL;
			if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))//No Block
			if(S_OK == Get(kGuildGuid, pkGuild))
			{
                BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_ReserveBattleForEmporia) );
                kAnsPacket.Push((BYTE)GCR_Success);
                kAnsPacket.Push(pkGuild->EmporiaInfo());
                kAnsPacket.Push(pkGuild->GetInven()->Money());
                kAnsPacket.Push(i64Money);
                pkGuild->BroadCast(kAnsPacket);
            }
        }break;
	case GC_AddExp: // 경험치 증가
		{
			int iAddExp = 0;
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(iAddExp);

			if( iAddExp )
			{
				if( GetCharToGuild(kUser.kCharGuid, kGuildGuid) )
				{
					PgGuild *pkGuild = NULL;
					if( S_OK == Get(kGuildGuid, pkGuild) )
					{
						__int64 const iPreExp = pkGuild->GetExp();

						EGuildCommandRet const eRet = pkGuild->AddExp(iAddExp);
						__int64 const iResultExp = pkGuild->GetExp();
						if( GCR_Success == eRet )
						{
							if( !kUser.Empty() )
							{
								PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Get_Exp, kUser);
								{
									PgLog kSubLog(ELOrderMain_Exp, ELOrderSub_Modify);
									kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
									kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< __int64 >(iAddExp) );
									kSubLog.Set( PgLogUtil::AtIndex(2), iPreExp );
									kSubLog.Set( PgLogUtil::AtIndex(3), iResultExp );
									kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
									kLogAction.Add(kSubLog);
								}
								kLogAction.Commit();
							}

							BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_AddExp);
							kPacket.Push( (BYTE)eRet );
							kPacket.Push( pkGuild->GetExp() );
							pkGuild->BroadCast(kPacket);

							Save(pkGuild, kUser.kCharGuid, GC_AddExp);
						}
					}
				}
			}
		}break;
	case GC_M_LvUp://길드 레벨업
		{
			bool bTest = false;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(bTest);

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					__int64 const iPreLevel = static_cast< __int64 >(pkGuild->GuildLevel());
					__int64 const iPreExp = pkGuild->GetExp();
					eRet = ReqLevelup(pkGuild, kMaster, bTest);
					if( GCR_Success == eRet )
					{
						if( !bTest )
						{
							pkGuild->Levelup(false);

							__int64 const iResultLevel = static_cast< __int64 >(pkGuild->GuildLevel());
							__int64 const iResultExp = pkGuild->GetExp();

							if( !kMaster.Empty() )
							{
								PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Change_Level, kMaster);
								{
									PgLog kSubLog(ELOrderMain_Level, ELOrderSub_Modify);
									kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
									kSubLog.Set( PgLogUtil::AtIndex(1), iResultLevel );
									kSubLog.Set( PgLogUtil::AtIndex(2), iPreLevel );
									kSubLog.Set( PgLogUtil::AtIndex(3), iResultExp );
									kSubLog.Set( PgLogUtil::AtIndex(4), iPreExp );
									kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
									kLogAction.Add(kSubLog);
								}
								kLogAction.Commit();
							}
						}
					}
				}
			}

			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_InventoryCreate: // 길드금고 생성
		{
			bool bTest = false;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(bTest);

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					eRet = ReqGuildInventoryCreate(pkGuild, kMaster, bTest);
					if( GCR_Success == eRet )
					{
						if( !bTest )
						{// 테스트 끝났다. 길드 금고 생성 완료다.
							if( !kMaster.Empty() )
							{
								PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_InventoryCreate, kMaster);
								{
									PgLog kSubLog(ELOrderMain_Inventory, ELOrderSub_Modify);
									kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
									kLogAction.Add(kSubLog);
								}
								kLogAction.Commit();
							}
						}
					}
				}
			}

			if( GCR_Success != eRet )
			{
				kPacket.Push(static_cast<BYTE>(eRet));
				kPacket.Push(static_cast<BYTE>(1));
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_InventoryOpen:
		{
			BYTE byInvType = 0;

			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(byInvType);

			PgGuild *pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild ) )
			{
				PgInventory* pkInv = pkGuild->GetInven();
				if( pkInv )
				{	
					pkGuild->AddGuildInventoryObserver(kUser.kCharGuid); // 길드금고 정보를 받을 브로드캐스트 대상이 된다.
					pkGuild->SendGuildInventory(kUser.kCharGuid, byInvType);
				}
				else
				{
					INFO_LOG( BM::LOG_LV6, __FL__ << L" GetInven() Fail. Guild Guid : "<< kGuildGuid.str() << L" Char Guid : "<< kUser.kCharGuid.str() << L" "<< kUser.kName);
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV6, __FL__ << L" GC_InventoryOpen Fail. Guild Guid : "<< kGuildGuid.str() << L" Char Guid : "<< kUser.kCharGuid.str() << L" "<< kUser.kName);
			}
		}break;
	case GC_InventoryClose:
		{
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);

			PgGuild *pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild ) )
			{
				pkGuild->RemoveGuildInventoryObserver(kUser.kCharGuid); // 길드금고를 닫았으니 얘는 더이상 브로드캐스트 대상이 아니야.
			}
		}break;
	case GC_InventoryMoney:
		{
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kGuildGuid);

			PgGuild *pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild ) )
			{
				PgInventory* pkInv = pkGuild->GetInven();
				if( pkInv )
				{	
					BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_InventoryMoney));
					kPacket.Push(static_cast<BYTE>(GCR_Success));
					kPacket.Push( pkInv->Money() );
					
					g_kRealmUserMgr.Locked_SendToUser( kUser.kCharGuid, kPacket, false );
				}
			}
		}break;
	case GC_M_AddSkill:
		{
			kMaster.ReadFromPacket(*pkMsg);
			bool bTest = false;
			int iSkillNo = 0;
			pkMsg->Pop(iSkillNo);
			pkMsg->Pop(bTest);

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) ) 
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					eRet = ReqAddSkill(pkGuild, kMaster, iSkillNo, bTest);
				}
			}
			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_ChangeOwner:
		{
			kMaster.ReadFromPacket(*pkMsg);
			kUser.ReadFromPacket(*pkMsg);

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				PgGuildMgrUtil::ChangeOwnerQuery(GC_M_ChangeOwner, kGuildGuid, kMaster.kCharGuid, kUser.kCharGuid);
			}
			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Notice:
		{
			kMaster.ReadFromPacket(*pkMsg);

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					eRet = ReqSetGuildNotice(pkGuild, kMaster, *pkMsg);
				}
			}
			if( GCR_Success != eRet )
			{
				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_ChangeMark1:
		{
			BYTE cNewGuildEmblem = 0;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop( cNewGuildEmblem );

			EGuildCommandRet eRet = GCR_Failed;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					if( pkGuild->IsOwner(kMaster.kCharGuid) )
					{
						eRet = pkGuild->ChangeEmblem(cNewGuildEmblem);
						if( GCR_Success == eRet )
						{
							Save(pkGuild, kMaster.kCharGuid, static_cast< EGuildCommand >(cCmdType));
						}
					}
				}
			}

			if( GCR_Success != eRet )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_SetMercenary:
		{
			kUser.ReadFromPacket(*pkMsg);

			if( GetCharToGuild(kUser.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;
				if ( SUCCEEDED( Get(kGuildGuid, pkGuild) ) )
				{
					BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_SetMercenary );

					if( pkGuild->IsMaster(kUser.kCharGuid) )
					{
						PgLimitClass kLimitClass;
						kLimitClass.ReadFromPacket(*pkMsg);

						pkGuild->SetMercenary(kLimitClass);

						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_MERCENARY_SAVE, _T("EXEC [dbo].[up_Update_Guild_Set_Mercenary]"));
						kQuery.InsertQueryTarget(kGuildGuid);
						kQuery.QueryOwner(kGuildGuid);

						kQuery.PushStrParam(kGuildGuid);
						kQuery.PushStrParam(kLimitClass.GetClass());
						kQuery.PushStrParam(kLimitClass.GetLevel());
						g_kCoreCenter.PushQuery(kQuery);

						kAnsPacket.Push((BYTE)GCR_Success);
						kLimitClass.WriteToPacket(kAnsPacket);
					}
					else
					{
						// 길마, 부길마가 아니면 신청 할 수 없다
						kAnsPacket.Push((BYTE)GCR_NotAuth);
					}

					pkGuild->BroadCast(kAnsPacket, BM::GUID::NullData(), GMG_Master);					
				}
			}
		}break;
	case GC_SetGuildEntranceOpen:
		{
			kUser.ReadFromPacket(*pkMsg);
			if( GetCharToGuild(kUser.kCharGuid, kGuildGuid) )
			{
				PgGuild *pkGuild = NULL;

				if( SUCCEEDED( Get(kGuildGuid, pkGuild) ) )
				{
					BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_SetGuildEntranceOpen) );

					if( pkGuild->IsMaster(kUser.kCharGuid) )
					{
						SSetGuildEntrancedOpen kGuildEntranceOpen;
						kGuildEntranceOpen.ReadFromPacket(*pkMsg);
						pkGuild->SetGuildEntranceOpen(kGuildEntranceOpen);

						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_ENTRANCEOPEN_SAVE, _T("EXEC [dbo].[up_Update_TB_Guild_EntranceOpen]"));
						kQuery.InsertQueryTarget(kGuildGuid);

						kQuery.PushStrParam(kGuildGuid);
						kQuery.PushStrParam(kGuildEntranceOpen.bIsGuildEntrance);
						kQuery.PushStrParam(kGuildEntranceOpen.sGuildEntranceLevel);
						kQuery.PushStrParam(kGuildEntranceOpen.i64GuildEntranceClass);
						kQuery.PushStrParam(kGuildEntranceOpen.i64GuildEntranceFee);
						kQuery.PushStrParam(kGuildEntranceOpen.wstrGuildPR);
						g_kCoreCenter.PushQuery(kQuery);

						kAnsPacket.Push(static_cast<BYTE>(GCR_Success));
						kGuildEntranceOpen.WriteToPacket(kAnsPacket);
					}
					else
					{
						kAnsPacket.Push(static_cast<BYTE>(GCR_NotAuth));
					}

					// 설정이 변경되면 길드 마스터 이상에게 모두 보내주자.
					pkGuild->BroadCast(kAnsPacket, BM::GUID::NullData(), GMG_Master);
				}
			}
		}break;
	case GC_GuildEntranceApplicant_Accept: // 길드가입 허용.
	case GC_GuildEntranceApplicant_Reject: // 길드가입 거절.
		{
			BM::GUID kCharGuid;
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(kCharGuid);

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_ENTRANCE_PROCESS, _T("EXEC [dbo].[up_Guild_Entrance_Process]"));
			kQuery.InsertQueryTarget(kUser.kGuildGuid);
			kQuery.QueryOwner(kUser.kGuildGuid);

			BYTE byState = AS_NONE;
			switch( cCmdType )
			{
			case GC_GuildEntranceApplicant_Accept:
				{
					byState = AS_ACCEPT;
				}break;
			case GC_GuildEntranceApplicant_Reject:
				{
					byState = AS_REJECT;
				}break;
			default:
				{

				}break;
			}
			kQuery.PushStrParam(kCharGuid);
			kQuery.PushStrParam(byState);
			g_kCoreCenter.PushQuery(kQuery);
			g_kGuildMgr.ChangeApplicantState(kUser.kGuildGuid, kCharGuid, byState); // 상태 변경 후 길드오너,길드마스터에게 전송
			g_kGuildMgr.NotifyGuildApplicationState(kCharGuid); // 가입 신청자에게 전송
			
		}break;
	case GC_None:
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("invalid message type [") << cCmdType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}//End switch() PT_C_N_REQ_GUILD_COMMAND
	return true;
}