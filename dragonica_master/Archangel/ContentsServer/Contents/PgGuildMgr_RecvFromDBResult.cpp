#include "stdafx.h"
#include "Collins/Log.h"
#include "Variant/PgLogUtil.h"
#include "Variant/PgEmporiaFunction.h"
#include "Global.h"
#include "PgGuildMgr.h"
#include "PgServerSetMgr.h"

bool PgGuildMgr::ProcessTCommand(int const iCommand, BM::GUID const &rkGuid, BM::Stream *pkPacket)
{
	BM::Stream kZPacket;
	kZPacket.Push((BYTE)iCommand);
	switch(iCommand)
	{
	case GC_GM_SetLv:
	case GC_GM_SetExp:
	case GC_GM_SetMoney:
	case GC_GM_Rename: // From Center
	case GC_GM_ChangeOwner:
	case GC_GM_ChangeOwnerKick:
		{
			if( ProcessGMCommand(iCommand, pkPacket) )
			{
				return true;
			}
		}break;
	case GC_M_Rename: // From Ceter + GC_GM_Rename
		{
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkGuid, true, kZPacket) )
			{
				// do nothing
			}
		}break;
	case GC_PreCreate:
	case GC_Create:
	case GC_Leave:
	case GC_Join:
	case GC_M_Grade:
	case GC_M_MemberGrade:
	//case GC_M_TaxRate:
	case GC_M_Notice:
		{
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkGuid, true, kZPacket) )
			{
				return false;
			}
		}break;
	case GC_M_Kick:
	case GC_M_Destroy:
		{
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkGuid, false, kZPacket) )
			{
				//return false;
			}
		}break;
	case GC_M_SetLv:
	case GC_M_SetExp:
	case GC_DB_Basic:
	case GC_S_SaveBasic:
	case GC_AddExp:
	case GC_M_LvUp:
	case GC_M_InventoryCreate:
	case GC_M_AddSkill:
	case GC_M_ChangeMark1:
	case GC_M_ChangeOwner:
	case GC_M_InventorySupervision:
		{
			kZPacket.Push(rkGuid);//Guild Guid
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Invalid CommandType[") << iCommand << _T("] From CenterServer") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	kZPacket.Push( *pkPacket );

	return ProcessMsgFromServer(&kZPacket);//Recv From Center
}

bool PgGuildMgr::ProcessGMCommand(int const iCommandType, BM::Stream *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);
	switch( iCommandType )
	{
	case GC_GM_Rename://From Center GM Command
		{
			BM::GUID kGuildGuid;
			std::wstring kNewGuildName;

			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(kNewGuildName);
			
			EGuildCommandRet const eRet = ReqRename(kGuildGuid, BM::GUID::NullData(), kNewGuildName, true);
		}break;
	case GC_GM_ChangeOwner:
	case GC_GM_ChangeOwnerKick:
		{
			BM::GUID kGuildGuid;
			BM::GUID kOldGuildOwner;
			BM::GUID kNewGuildOwner;

			pkMsg->Pop( kGuildGuid );
			pkMsg->Pop( kOldGuildOwner );
			pkMsg->Pop( kNewGuildOwner );

			PgGuildMgrUtil::ChangeOwnerQuery(static_cast< EGuildCommand >(iCommandType), kGuildGuid, kOldGuildOwner, kNewGuildOwner);
		}break;
	case GC_GM_SetLv:
		{
			BM::GUID kGuildGuid;
			int iLv = 0;
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(iLv);

			PgGuild* pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				ReqSetLevel(pkGuild, iLv);
			}
			else
			{
				BM::Stream kPacket;
				kPacket.Push( GC_GM_SetLv );
				kPacket.Push(iLv);
				ReqLoadData(kGuildGuid, kPacket);
			}
		}break;
	case GC_GM_SetExp:
		{
			BM::GUID kGuildGuid;
			__int64 iExp = 0;
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(iExp);

			PgGuild* pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				ReqSetExp(pkGuild, iExp);
			}
			else
			{
				BM::Stream kPacket;
				kPacket.Push( GC_GM_SetExp );
				kPacket.Push(iExp);
				ReqLoadData(kGuildGuid, kPacket);
			}
		}break;
	case GC_GM_SetMoney:
		{
			BM::GUID kGuildGuid;
			__int64 iMoney = 0;
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(iMoney);

			PgGuild* pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				ReqSetMoney(pkGuild, iMoney);
			}
			else
			{
				BM::Stream kPacket;
				kPacket.Push( GC_GM_SetMoney );
				kPacket.Push(iMoney);
				ReqLoadData(kGuildGuid, kPacket);
			}
		}break;
	default:
		{
			return false;
		}break;
	}
	return true;
}

bool PgGuildMgr::ProcessMsgFromServer(BM::Stream *pkMsg)
{
	if( !pkMsg ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CAutoMutex kLock(m_kMutex);

	SContentsUser kMaster;
	SContentsUser kUser;
	BM::GUID kGuildGuid;
	BYTE cCmdType = 0;

	pkMsg->Pop(cCmdType);
	BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
	BYTE cTRet = 0;
	switch(cCmdType)
	{
//////////////////////////////////////////
//	Converting ( Ended ..... )
	case GC_DB_Basic:
		{
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(cTRet);

			if( GCR_Success == cTRet )
			{
				SGuildBasicInfo kBasicInfo;
				kBasicInfo.ReadFromPacket(*pkMsg);

				// Emporia
				SGuildEmporiaInfo kEmporiaInfo;
				pkMsg->Pop( kEmporiaInfo );

				PgGuild *pkGuild = NULL;
				if( S_OK != Get(kGuildGuid, pkGuild) )//길드가 있으면 정보 교체
				{
					if( !pkGuild )
					{
						pkGuild = NewGuild();
						auto kRet = m_kGuild.insert( std::make_pair(kBasicInfo.kGuildGuid, pkGuild) );
						if( !kRet.second )
						{
							m_kGuildPool.Delete(pkGuild);
							pkGuild = (*kRet.first).second; // 이미 메모리가 있어
						}
					}
				}
				if(!pkGuild)
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("Null Guild [") << kGuildGuid << _T("]") );
					break;
				}

				pkGuild->Set( kBasicInfo );
				pkGuild->EmporiaInfo(kEmporiaInfo);
				AddGuildName(pkGuild->Name(), pkGuild->Guid());

				{ // Extern info
					int iCount = 0;
					std::wstring kNotice;

					pkMsg->Pop( iCount );
					for( int iCur = 0; iCount > iCur; ++iCur )
					{
						pkMsg->Pop(kNotice);
						pkGuild->Notice(kNotice);
					}
				}
				{ // Member grade
					int iCount = 0;
					SGuildMemberGradeInfo kTemp;
					
					pkMsg->Pop( iCount );
					for( int iCur = 0; iCount > iCur; ++iCur )
					{
						kTemp.ReadFromPacket(*pkMsg);
						pkGuild->AddGradeGroup(kTemp);
					}
				}
				{ // Mercenary
					PgLimitClass kSetMercenary;
					kSetMercenary.ReadFromPacket( *pkMsg );
					pkGuild->SetMercenary( kSetMercenary );
				}
				{ // Member
					int iCount = 0;
					SGuildMemberInfo kTemp;
					SContentsUser kUserInfo;

					pkMsg->Pop( iCount );
					for( int iCur = 0; iCount > iCur; ++iCur )
					{
						kTemp.ReadFromPacket(*pkMsg);

						if( !pkGuild->Add(kTemp) )
						{
							INFO_LOG(BM::LOG_LV5, __FL__ << _T("[GC_DB_Member] Can't add member Member=") << kTemp.kCharName << L" Guid=" << kTemp.kCharGuid);
						}

						AddCharToGuild(kTemp.kCharGuid, pkGuild->Guid()); // CharGuid To GuildGuid 리스트에 추가
						if( S_OK == ::GetPlayerByGuid(kTemp.kCharGuid, false, kUserInfo) ) // 현재 로그인 중이면
						{
							ChangePosGuildUser(kUserInfo); // 위치 정보 업데이트
							PgGuildMgrUtil::JoinGuildOrder(pkGuild, kUserInfo); // 길드 관련 Guid, Skill 적용
						}
					}
				}
				{// 길드 가입 설정 정보
					SSetGuildEntrancedOpen kGuildEntranceOpen;
					kGuildEntranceOpen.ReadFromPacket( *pkMsg );
					pkGuild->SetGuildEntranceOpen( kGuildEntranceOpen );
				}
				ProcessWaiter(kGuildGuid); // 길드 정보 기다리고 있는 사람들 처리

				{// 길드 금고 아이템
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_INVENTORY_LOAD, _T("EXEC [dbo].[up_Item_Select]"));
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam(kGuildGuid);
					kQuery.QueryOwner(kGuildGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}
				{//contUserData
					BM::Stream kPacket;
					pkMsg->Pop(kPacket.Data());
					kPacket.PosAdjust();

					EGuildCommand eType = GC_None;
					kPacket.Pop(eType);
					switch(eType)
					{
					case GC_GM_SetLv:
						{
							int iLv = 0;
							kPacket.Pop(iLv);

							ReqSetLevel(pkGuild, iLv);
						}break;
					case GC_GM_SetExp:
						{
							__int64 iExp = 0;
							kPacket.Pop(iExp);

							ReqSetExp(pkGuild, iExp);
						}break;
					case GC_GM_SetMoney:
						{
							__int64 iMoney = 0;
							kPacket.Pop(iMoney);

							ReqSetMoney(pkGuild, iMoney);
						}break;
					}
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV3, __FL__ << _T("Trash Guild guid.... [") << kGuildGuid.str().c_str() << _T("]") );
				Delete(kGuildGuid); // 혹시 생성되어 있을지 모르니, 삭제 하도록 한다.
				ContInfoWaiter::ContWaiterVec kInfoWaiter;
				// 길드 GUID Null Set, 맵서버에 추방 당했어
				if( m_kWaiter.GetWaiter(kGuildGuid, kInfoWaiter) )
				{
					ContInfoWaiter::ContWaiterVec::const_iterator waiter_iter = kInfoWaiter.begin();
					while( kInfoWaiter.end() != waiter_iter )
					{
						BM::GUID const& rkMembGuid = (*waiter_iter);
						SContentsUser kUser;
						if( S_OK == GetPlayerByGuid(rkMembGuid, true, kUser) )
						{
							PgGuildMgrUtil::LeaveGuildOrder(kUser);

							BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, static_cast< BYTE >(GC_M_Destroy)); // 맵서버에서 BroadCasting
							kMapPacket.Push( static_cast< BYTE >(GCR_Success) );
							kMapPacket.Push(kUser.kCharGuid);
							g_kRealmUserMgr.Locked_SendToUserGround(rkMembGuid, kMapPacket, true, true);

							DelCharToGuild(kUser.kCharGuid);
						}
						++waiter_iter;
					}
				}

				m_kOtherWaiter.DelWaiter(kGuildGuid);
			}
		}break;
	case GC_Join:
		{
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildGuid);

			// Join을 위한 DB 작업 완료되었음...
			// 실제 내부 Data 작업하고, 새로운 Member Join 한 것을 알려야 함.
			const EGuildCommandRet eRet = Join(kGuildGuid, kUser, true, cTRet);
			if( GCR_Success != eRet )
			{
				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_Join);//Return Code
				kPacket.Push((BYTE)GCR_Failed);
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
			}
		}break;
	case GC_Create:
		{
			SGuildOtherInfo kInfo;
			BYTE cRet = GCR_Failed;
			std::wstring kGuildName;
			kMaster.ReadFromPacket(*pkMsg);

			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(kGuildName);

			INFO_LOG( BM::LOG_LV1, __FL__ << _T("[GUILD-Create: ") << kMaster.Name().c_str() << _T("] Success - [after DB job] Ret: [") << static_cast<int>(cTRet) << _T("]") );

			PgGuild* pkGuild = NULL;
			if( GetCharToGuild(kMaster.kCharGuid, kGuildGuid) )
			{
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					if( cTRet == GCR_Success )
					{
						ChangePosGuildUser(kMaster);

						SGuildOtherInfo const kInfo = pkGuild->GetOtherInfo();

						//맵서버로 통보해서 맵서버에서 Unit에 Guild Guid를 설정하고 BroadCasting
						BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_Create);//
						kMapPacket.Push((BYTE)GCR_Success);
						kMapPacket.Push(kMaster.kCharGuid);
						kInfo.WriteToPacket(kMapPacket);
						pkGuild->WriteSkillToPacket(kMapPacket);

						PgGuildMgrUtil::JoinGuildOrder(pkGuild, kMaster);

						g_kRealmUserMgr.Locked_SendToUserGround(kMaster.kMemGuid, kMapPacket, true, true);

						cRet = GCR_Success;

						if( !kMaster.Empty() )
						{
							PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_Create, kMaster);
							{
								PgLog kSubLog(ELOrderMain_Guild, ELOrderSub_Create);
								kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
								kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(cRet) );
								kSubLog.Set( PgLogUtil::AtIndex(4), pkGuild->Guid().str() );
								kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Master().str() );
								kLogAction.Add(kSubLog);
							}
							kLogAction.Commit();
						}
					}
				}
			}
			
			if( GCR_Success != cRet )
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("[GUILD-Create: ") << kMaster.Name().c_str() << _T("] Failed - [after DB job] Ret: [") << static_cast<int>(cRet) << _T("]") );

				DelCharToGuild(kMaster.kCharGuid);
				DelGuildName(kGuildName);
				Delete(pkGuild);

				BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_Create);
				kMapPacket.Push((BYTE)GCR_Failed);
				kMapPacket.Push(kMaster.kCharGuid);
				g_kRealmUserMgr.Locked_SendToUserGround(kMaster.kMemGuid, kMapPacket, true, true);
			}
		}break;
	case GC_PreCreate:
		{
			std::wstring kGuildName;
			BM::GUID kGuildGuid;

			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildName);
			pkMsg->Pop(kGuildGuid);

			bool const bFindGuildName = FindGuildName(kGuildName);
			if( GCR_Success == cTRet && bFindGuildName )
			{
				cTRet = GCR_Failed;
			}
			else if( GCR_Failed == cTRet && !bFindGuildName )//실패시 메모리에 없으면 기억 해두자
			{
				AddGuildName(kGuildName, kGuildGuid);
			}

			if( GCR_Success == cTRet
			&&	GE_Name_Max < kGuildName.size() )
			{
				cTRet = GCR_Max;
			}
			if( GCR_Success == cTRet
			&&	(false == g_kUnicodeFilter.IsCorrect(UFFC_GUILD_NAME, kGuildName) || (true == g_kFilterString.Filter(kGuildName, false, FST_ALL))) )
			{
				cTRet = GCR_BadName;
			}

			kPacket.Push(cTRet);
			g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
		}break;
	case GC_Leave:
	case GC_M_Kick:
		{
			BM::GUID kCharGuid;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(kCharGuid);

			EGuildCommandRet eRet = GCR_Failed;
			BM::GUID kReceivedGuid = kCharGuid;//킥 일땐 실패 패킷은 마스터에게
			if( GCR_Success == cTRet )
			{
				PgGuild *pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)cCmdType);//맵서버에서 BroadCasting
					kMapPacket.Push((BYTE)GCR_Success);
					kMapPacket.Push(kCharGuid);
					g_kRealmUserMgr.Locked_SendToUserGround(kCharGuid, kMapPacket, false, true);

					PgGuildMgrUtil::LeaveGuildOrder(kCharGuid);
					
					BM::Stream kNfyPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)cCmdType);//길드 맴버들에게 통보
					kNfyPacket.Push((BYTE)GCR_Notify);
					bool const bWriteRet = pkGuild->WriteMemberPacket(kCharGuid, kNfyPacket);
					if( bWriteRet )
					{
						SendToGuild(pkGuild, kNfyPacket);
					}

					SGuildMemberInfo kLeaverInfo;
					if( pkGuild->Get(kCharGuid, kLeaverInfo) )
					{
						PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_None, kMaster);
						switch( cCmdType )
						{
						case GC_M_Kick:
							{
								kLogAction.LogSubType(ELogSub_Guild_Kick);
								PgLog kSubLog(ELOrderMain_Guild, ELOrderSub_Break);
								kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
								kSubLog.Set( PgLogUtil::AtIndex(2), kLeaverInfo.kCharName );
								kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(GCR_Success) );
								kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
								kSubLog.Set( PgLogUtil::AtIndex(4), kLeaverInfo.kCharGuid.str() );
								kLogAction.Add(kSubLog);
							}break;
						case GC_Leave:
						default:
							{
								kLogAction.LogSubType(ELogSub_Guild_Leave);
								PgLog kSubLog(ELOrderMain_Guild, ELOrderSub_Break);
								kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
								kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(GCR_Success) );
								kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
								kSubLog.Set( PgLogUtil::AtIndex(4), pkGuild->Master().str() );
								kLogAction.Add(kSubLog);
							}break;
						}
						kLogAction.Commit();
					}

					bool const bDeleteRet = pkGuild->Del(kCharGuid);//메모리 삭제 성공해야 Unit Guid 설정
				}
				DelCharToGuild(kCharGuid);
				eRet = GCR_Success;//실패 상관 없음, DB에선 이미 삭제
			}
			else
			{
				if( GC_M_Kick == cCmdType)
				{
					kReceivedGuid = kMaster.kCharGuid;
				}
			}

			if( GCR_Success != cTRet )//실패면 무조건
			{
				kPacket.Push((BYTE)eRet);//Leave면 나에게, Kick이면 마스터에게
				kPacket.Push(kCharGuid);
				g_kRealmUserMgr.Locked_SendToUser(kReceivedGuid, kPacket, false);
			}
		}break;
	case GC_M_InventorySupervision:
		{
			BM::GUID kGuildGuid;
			BYTE byAuthority[MAX_DB_GUILD_INV_AUTHORITY] = {0,};
			
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(byAuthority);

			if(GCR_Success == cTRet)
			{
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGuildGuid, pkGuild))
				{
					pkGuild->SetGuildInvAuthority(byAuthority[1], byAuthority[3]);
					BM::Stream kNfyPacket(PT_N_C_ANS_GUILD_COMMAND);
					kNfyPacket.Push( cCmdType );
					kNfyPacket.Push( cTRet );
					kNfyPacket.Push( byAuthority );
					pkGuild->BroadCast(kNfyPacket);
				}
			}
		}break;
	case GC_M_Grade:
		{
			SGuildMemberGradeInfo kGradeInfo;
			BM::GUID kCharGuid;
			//
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			BYTE cRet = (BYTE)GCR_Failed;
			if(GCR_Success == cTRet)
			{
				kGradeInfo.ReadFromPacket(*pkMsg);
				PgGuild *pkGuild = NULL;
				if(S_OK == Get(kGradeInfo.kGuildGuid, pkGuild))
				{
					if(pkGuild->SetGradeGroup(kGradeInfo))
					{
						cRet = (BYTE)GCR_Success;
					}
				}
			}
			
			if( 1 )//항상 전송
			{
				kPacket.Push(cRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}
	case GC_M_MemberGrade:
		{
			BYTE cGuildMemberGrade = 0;
			BM::GUID kCharGuid;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(cGuildMemberGrade);

			BYTE cRet = (BYTE)GCR_Failed;
			if(GCR_Success == cTRet)
			{
				if(GetCharToGuild(kMaster.kCharGuid, kGuildGuid))
				{
					PgGuild *pkGuild = NULL;
					if(S_OK == Get(kGuildGuid, pkGuild))
					{
						if(pkGuild->SetMember(kCharGuid, (int)cGuildMemberGrade))
						{
							cRet = (BYTE)GCR_Success;

							SGuildMemberInfo kSubMasterInfo;
							if( !kMaster.Empty()
							&&	BM::GUID::IsNotNull(kCharGuid)
							&&	pkGuild->Get(kCharGuid, kSubMasterInfo) )
							{
								PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_Change_Officer, kMaster);
								{
									PgLog kSubLog(ELOrderMain_Guild_Officer, ELOrderSub_Create);
									kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
									kSubLog.Set( PgLogUtil::AtIndex(2), kSubMasterInfo.kCharName );
									kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(cRet) );
									kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
									kSubLog.Set( PgLogUtil::AtIndex(4), kCharGuid.str() );
									kLogAction.Add(kSubLog);
								}
								kLogAction.Commit();
							}

							BM::Stream kNfyPacket(PT_N_C_ANS_GUILD_COMMAND);
							kNfyPacket.Push( cCmdType );
							kNfyPacket.Push( cRet );
							kNfyPacket.Push( kCharGuid );
							kNfyPacket.Push( cGuildMemberGrade );
							pkGuild->BroadCast(kNfyPacket);
						}
					}
				}
			}

			if( GCR_Success != cRet )//항상 전송
			{
				kPacket.Push( cRet );
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	//case GC_M_TaxRate:
	//	{
	//		BYTE cTaxRate = 0;
	//		kMaster.ReadFromPacket(*pkMsg);
	//		pkMsg->Pop(cTRet);
	//		pkMsg->Pop(kGuildGuid);
	//		pkMsg->Pop(cTaxRate);
	//		
	//		EGuildCommandRet eRet = GCR_Failed;
	//		if(GCR_Success == cTRet)
	//		{
	//			PgGuild *pkGuild = NULL;
	//			if(S_OK == Get(kGuildGuid, pkGuild))
	//			{
	//				if(pkGuild->SetTaxRate(cTaxRate))
	//				{
	//					eRet = GCR_Success;
	//				}
	//			}
	//		}
	//		kPacket.Push((BYTE)eRet);
	//		g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
	//	}break;
	case GC_M_Destroy:
		{
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildGuid);

			EGuildCommandRet eRet = GCR_Failed;
			if( GCR_Success == cTRet )//성공은 맵서버에서
			{
				PgGuild *pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					g_kGuildMgr.NotifyGuildApplicationState(BM::GUID::NullData(), kGuildGuid, true);// 길드가 삭제되었으니, 길드에 가입 신청한 유저들은 모두 길드가입 거부 처리
					g_kRealmUserMgr.Locked_UnregistGuildInfo( kGuildGuid );

					std::wstring const kDeleteGuildName = pkGuild->Name();
					BM::GUID const kDeleteGuildGuid = pkGuild->Guid();
					int const iDeleteGuildLv = pkGuild->GuildLevel();

					DelGuildName(kDeleteGuildName);//길드 이름 삭제
					if( Delete(pkGuild) )
					{
						eRet = GCR_Success;

						//if( !kMaster.Empty() )
						{
							PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_Delete, kMaster);
							{
								PgLog kSubLog(ELOrderMain_Guild, ELOrderSub_Delete);
								kSubLog.Set( PgLogUtil::AtIndex(1), kDeleteGuildName );
								kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(eRet) );
								kSubLog.Set( PgLogUtil::AtIndex(3), kDeleteGuildGuid.str() );
								kSubLog.Set( PgLogUtil::AtIndex(2), iDeleteGuildLv );
								kLogAction.Add(kSubLog);
							}
							kLogAction.Commit();
						}

						BM::Stream kNfyPacket( PT_N_N_NFY_GUILD_REMOVE, kGuildGuid );
						SendToRealmContents( PMET_EMPORIA, kNfyPacket );
					}
				}
				DelCharToGuild(kMaster.kCharGuid);

				PgGuildMgrUtil::LeaveGuildOrder(kMaster.kCharGuid);

				BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_M_Destroy);//여기까지 오면 이미 지워진것이다 -_-;
				kMapPacket.Push((BYTE)GCR_Success);
				kMapPacket.Push(kMaster.kCharGuid);
				g_kRealmUserMgr.Locked_SendToUserGround(kMaster.kMemGuid, kMapPacket, true, true);

			}
			else//실패 때만
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Notice:
		{
			std::wstring kNotice;
			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(kNotice);

			EGuildCommandRet eRet = GCR_Failed;
			if( GCR_Success == cTRet )
			{
				PgGuild *pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					eRet = GCR_Success;

					if( !kMaster.Empty() )
					{
						PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_Notice, kMaster);
						{
							PgLog kSubLog(ELOrderMain_Guild_Notice, ELOrderSub_End);
							kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
							kSubLog.Set( PgLogUtil::AtIndex(2), kNotice );
							kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(eRet) );
							kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
							kSubLog.Set( PgLogUtil::AtIndex(4), pkGuild->Master().str() );
							kLogAction.Add(kSubLog);
						}
						kLogAction.Commit();
					}

					pkGuild->Notice(kNotice);

					BM::Stream kNfyPacket(PT_N_C_ANS_GUILD_COMMAND);
					kNfyPacket.Push( cCmdType );
					kNfyPacket.Push( (BYTE)eRet );
					kNfyPacket.Push( kNotice );
					kNfyPacket.Push( kMaster.kCharGuid );
					pkGuild->BroadCast(kNfyPacket);
				}
			}
			if( GCR_Success != GCR_Success )
			{
				kPacket.Push((BYTE)eRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_Rename://From Center DB Result
		{
			std::wstring kGuildName;
			bool bGMCmd = false;

			kMaster.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(kGuildName);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(bGMCmd);

			if( GCR_Success == cTRet )
			{
				cTRet = GCR_Failed;//임시 실패 처리

				PgGuild* pkGuild = NULL;
				if( S_OK == Get(kGuildGuid, pkGuild) )
				{
					AddGuildName(kGuildName, pkGuild->Guid());//새로운 이름 추가
					DelGuildName(pkGuild->Name());//기존 이름 삭제
					
					pkGuild->Rename(kGuildName);

					VEC_GUID kVec;
					bool const bGerRet = pkGuild->GetMemberGuid(kVec);
					if( bGerRet )
					{
						VEC_GUID::const_iterator guid_iter = kVec.begin();
						while(kVec.end() != guid_iter)
						{
							VEC_GUID::value_type const& rkCharGuid = (*guid_iter);
							BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, cCmdType);
							kMapPacket.Push(cTRet);
							kMapPacket.Push(rkCharGuid);
							kMapPacket.Push(kGuildName);
							g_kRealmUserMgr.Locked_SendToUserGround(rkCharGuid, kMapPacket, false, true);
							++guid_iter;
						}

						cTRet = GCR_Success;//성공
					}
				}
			}

			if( GCR_Success != cTRet &&	!bGMCmd )
			{
				kPacket.Push(cTRet);
				g_kRealmUserMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case GC_M_SetLv:
	case GC_M_SetExp:
	case GC_AddExp:
	case GC_M_LvUp:
	case GC_S_SaveBasic:
	case GC_M_AddSkill:
	case GC_M_ChangeMark1:
		{
			unsigned short sLevel = 0;
			__int64 iExperience = 0;
			unsigned short sSkillPoint = 0;
			BYTE cSkills[MAX_DB_GUILD_SKILL_SIZE] = {0, };
			int iExternInt = 0;
			BM::GUID kOperatorGuid;
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(sLevel);
			pkMsg->Pop(iExperience);
			pkMsg->Pop(sSkillPoint);
			pkMsg->Pop(cSkills);
			pkMsg->Pop(iExternInt);
			pkMsg->Pop(kOperatorGuid);

			PgGuild *pkGuild = NULL;

			if(S_OK == Get(kGuildGuid, pkGuild))
			{
				pkGuild->LastSaveTime(BM::PgPackedTime::LocalTime());//마지막으로 업데이트한 시간을 기록한다.
				pkGuild->LastSaveSuccess(GCR_Success == cTRet);
				switch( cCmdType )
				{
				case GC_M_SetExp:
					{
						BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_AddExp);
						kPacket.Push( (BYTE)GCR_Success );
						kPacket.Push( pkGuild->GetExp() );
						pkGuild->BroadCast(kPacket);
					}break;
				case GC_M_SetLv:
				case GC_M_LvUp:
					{
						BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
						kPacket.Push((BYTE)GCR_Success);
						kPacket.Push(kOperatorGuid);
						pkGuild->WriteInfoToPacket(kPacket);
						pkGuild->BroadCast(kPacket);
					}break;
				case GC_M_ChangeMark1:
					{
						VEC_GUID kVec;
						pkGuild->GetMemberGuid(kVec);

						VEC_GUID::const_iterator guid_iter = kVec.begin();
						while( kVec.end() != guid_iter )
						{
							VEC_GUID::value_type const& rkGuid = (*guid_iter);

							BM::Stream kPacket(PT_N_M_NFY_GUILD_COMMAND, cCmdType);
							kPacket.Push( (BYTE)GCR_Success );
							kPacket.Push( rkGuid );
							kPacket.Push( pkGuild->Emblem() );
							g_kRealmUserMgr.Locked_SendToUserGround(rkGuid, kPacket, false, true);

							++guid_iter;
						}
					}break;
				case GC_M_AddSkill:
					{
						VEC_GUID kVec;
						pkGuild->GetMemberGuid(kVec);

						VEC_GUID::const_iterator guid_iter = kVec.begin();
						while( kVec.end() != guid_iter )
						{
							VEC_GUID::value_type const &rkGuid = (*guid_iter);

							CONT_PLAYER_MODIFY_ORDER kOrder;
							SPMOD_AddSkill kAddSkillData(iExternInt);
							kOrder.push_back( SPMO(IMET_ADD_SKILL, rkGuid, kAddSkillData) );

							PgGuildMgrUtil::SendToItemOrder((*guid_iter), kOrder);

							BM::Stream kPacket(PT_N_M_NFY_GUILD_COMMAND, cCmdType);
							kPacket.Push( (BYTE)GCR_Success );
							kPacket.Push( rkGuid );
							kPacket.Push( iExternInt );
							pkGuild->WriteInfoToPacket(kPacket);
							g_kRealmUserMgr.Locked_SendToUserGround(rkGuid, kPacket, false, true);
							++guid_iter;
						}
					}break;
				}
			}
		}break;
	case GC_M_InventoryCreate:
		{// 길드금고 생성에 성공 했다.
			BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE] = {0,};
			BYTE abyInvExternIdx[MAX_DB_INVEXTEND_SIZE] = {0,};

			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(cTRet);
			pkMsg->Pop(abyInvExtern);
			pkMsg->Pop(abyInvExternIdx);

			PgGuild *pkGuild = NULL;
			if(S_OK == Get(kGuildGuid, pkGuild))
			{
				PgInventory* pkInv = pkGuild->GetInven();
				if( pkInv )
				{
					// 인벤 셋팅.
					pkGuild->SetGuildInv( abyInvExtern, abyInvExternIdx );
					pkInv->Init( abyInvExtern, abyInvExternIdx );
					pkInv->OwnerGuid(pkGuild->Guid());
					g_kRealmUserMgr.Locked_RegistGuildInfo( pkGuild->Guid(), pkGuild->Name(), abyInvExtern, abyInvExternIdx ); //캐릭터와 동급으로 사용하도록 길드등록
				}

				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
				kPacket.Push(static_cast<BYTE>(GCR_Success));
				kPacket.Push(abyInvExtern);
				kPacket.Push(abyInvExternIdx);
				g_kRealmUserMgr.Locked_SendToUser(pkGuild->Master(), kPacket, false);
			}
		}break;
	case GC_Moved:
		{
			kUser.ReadFromPacket(*pkMsg);
			ChangePosGuildUser(kUser);
		}break;
	case GC_M_ChangeOwner:
		{
			BYTE cTRet = 0;
			BM::GUID kNewGuildOwner, kOldGuildOwner;
			int iCmdType = 0;

			pkMsg->Pop( kGuildGuid );
			pkMsg->Pop( cTRet );
			pkMsg->Pop( kNewGuildOwner );
			pkMsg->Pop( iCmdType );
			pkMsg->Pop( kOldGuildOwner );

			PgGuild* pkGuild = NULL;
			if( S_OK == Get(kGuildGuid, pkGuild) )
			{
				if( GCR_Success == cTRet )
				{
					EGuildCommandRet eRet = pkGuild->ChangeOwner(kNewGuildOwner, true);
					if( GCR_Success == eRet )
					{
						BM::Stream kNfyPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast< BYTE >(GC_M_ChangeOwner));
						kNfyPacket.Push( static_cast< BYTE >(GCR_Success) );
						kNfyPacket.Push( kNewGuildOwner );
						pkGuild->BroadCast(kNfyPacket);
					}
					else
					{
						cTRet = GCR_Failed;
					}
				}

				switch( iCmdType ) // 후처리
				{
				case GC_GM_ChangeOwnerKick:
					{
						Kick(pkGuild->Guid(), SContentsUser(), kOldGuildOwner, true); // 이전 길마 강퇴
						PgGuildMgrUtil::UpdateLastLoingDate(kGuildGuid, kNewGuildOwner);
					}break;
				default:
					{
					}break;
				}

				if( GCR_Success != cTRet )
				{
					BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
					kPacket.Push( static_cast< BYTE >(GCR_Failed) );
					g_kRealmUserMgr.Locked_SendToUser(kOldGuildOwner, kPacket);
				}

				{//Log
					SGuildMemberInfo kMemberInfo;
					SGuildMemberInfo kMasterInfo;

					if( pkGuild->Get( kOldGuildOwner, kMasterInfo)
					 && pkGuild->Get( kNewGuildOwner, kMemberInfo) )
					{
						kMaster.kName = kMasterInfo.kCharName;
						kMaster.kCharGuid = kMasterInfo.kCharGuid;
						kMaster.cGender = kMasterInfo.cGrade;
						kMaster.sLevel = kMasterInfo.sLevel;
						kMaster.iClass = kMasterInfo.cClass;
						
						kUser.kName = kMemberInfo.kCharName;
						kUser.kCharGuid = kMemberInfo.kCharGuid;
						PgGuildMgrLogUtil::ChangeOwnerLog(pkGuild, kMaster, kUser, static_cast<EGuildCommandRet>(cTRet));
					}
				}
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Invalid Guild Command[") << static_cast<int>(cCmdType) << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}