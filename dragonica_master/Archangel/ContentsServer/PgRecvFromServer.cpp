#include "stdafx.h"
#include "Bm/TimeCheck.h"
#include "Lohengrin/PgRealmManager.h"
#include "FCS/AntiHack.h"
#include "variant/GM_const.h"
#include "Variant/PgMission_Report.h"
#include "Variant/defabiltype.h"
#include "Variant/PgEventView.h"
#include "Variant/PgNoticeAction.h"
#include "variant/pggamblemachine.h"
#include "Global.h"
#include "PgDBProcess.h"
#include "PgRecvFromServer.h"
#include "PgGMProcessMgr.h"
#include "PgGuild.h"
#include "PgRecvFromMap.h"
#include "JobDispatcher.h"
#include "PgEmporiaMgr.h"
#include "PgHardCoreDungeonSystemMgr.h"
#include "PgStatTrackMgr.h"
#include "Variant/PgBattlePassMgr.h"


extern bool CALLBACK OnRecvFromServer3( CEL::CSession_Base *pkSession, unsigned short usType, BM::Stream * const pkPacket );
extern bool CALLBACK OnRecvFromServer2( CEL::CSession_Base *pkSession, unsigned short usType, BM::Stream * const pkPacket );
extern void Recv_PT_A_S_REQ_GREETING(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
extern void BuilderCommandProcess(BM::Stream * const pkPacket);

void CALLBACK OnAcceptFromServer( CEL::CSession_Base *pkSession )
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		//INFO_LOG(BM::LOG_LV6, _T("[%s]-[%d] Send EncryptKey"), __FUNCTIONW__, __LINE__);

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Get EncryptKey Failed. Session Terminate!") );
		pkSession->VTerminate();
	}
}

void CALLBACK OnConnectFromServer( CEL::CSession_Base *pkSession )
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T("Other Server Connected Address[") << pkSession->Addr().ToString().c_str() << _T("]") );
		return;
	}
	else
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T("Other Server Connected Failed") );
	}
}

void CALLBACK OnDisconnectFromServer( CEL::CSession_Base *pkSession )
{//!  세션락
	INFO_LOG( BM::LOG_LV0, __FL__ << _T("Server Connection Close") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	
	if(pkSession->m_kSessionData.Size())
	{
		SERVER_IDENTITY kRecvSI;
		kRecvSI.ReadFromPacket(pkSession->m_kSessionData);

		if ( kRecvSI.nServerType == CEL::ST_CENTER)
		{
			CAUTION_LOG( BM::LOG_LV1, L"* Disconnect Center : Channel<" << kRecvSI.nChannel << L"> IP<" << pkSession->Addr().IP() << L">" );

			if ( CProcessConfig::IsPublicChannel( kRecvSI.nChannel) )
			{
				g_kEmporiaMgr.Locked_ConnectPublicCenter( false );
				g_kHardCoreDungeonMgr.RecvAllClose();
			}

			g_kRealmUserMgr.Locked_Recv_PT_A_A_SERVER_SHUTDOWN( kRecvSI );

			BM::Stream kEPacket(PT_A_A_SERVER_SHUTDOWN);
			kRecvSI.WriteToPacket(kEPacket);
			SendToImmigration(kEPacket);

			short int sNewPrimeChannel = g_kRealmMgr.AliveChannel(kRecvSI.nRealm, kRecvSI.nChannel, false);
			BM::Stream kCPacket(PT_N_A_NFY_CHANNEL_ALIVE);
			kRecvSI.WriteToPacket(kCPacket);
			kCPacket.Push((bool)false);
			SendToServerType(CEL::ST_CENTER, kCPacket);
			kCPacket.PosAdjust();
			SendToImmigration(kCPacket);
		}
	}
}

void CALLBACK OnRecvFromServer( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	PACKET_ID_TYPE usType = 0;
	pkPacket->Pop(usType);
	//std::cout<< "Received Packet Type(" << usType <<") Size["<< pkPacket->Size() << "]"<< std::endl;
	//INFO_LOG(BM::LOG_LV7, _T("OnRecvFromServer PacketType[%hd], Size[%d]"), usType, pkPacket->Size());
	int iLoginServerNo = -1;
	switch( usType )
	{
	case PT_A_S_REQ_GREETING:
		{
			Recv_PT_A_S_REQ_GREETING(pkSession, pkPacket);
		}break;
	case PT_S_T_ANS_RESERVE_MEMBER:
		{
			g_kRealmUserMgr.Locked_Recv_PT_S_T_ANS_RESERVE_MEMBER(pkPacket);
		}break;
	case PT_S_T_NFY_USER_CONNECT_SWITCH:
		{
			SERVER_IDENTITY  kRecvSI;
			size_t user_count = 0;

			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(user_count);

			if(S_OK != g_kProcessCfg.Locked_SetPlayerCount(kRecvSI, user_count))
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[ERROR] [") << __FL__ << _T("] Incorrect Server Send Nfy") );
			}
		}break;
	case PT_A_A_SERVER_SHUTDOWN:
		{
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket(*pkPacket);

			pkPacket->PosAdjust();
			SendToImmigration( *pkPacket );

			g_kRealmUserMgr.Locked_Recv_PT_A_A_SERVER_SHUTDOWN(kSI);

			SendToChannel( kSI.nChannel, *pkPacket );
			SendToChannel( CProcessConfig::GetPublicChannel(), *pkPacket );
		}break;
	case PT_T_N_ANS_KICK_USER:
		{
			BM::GUID kCmdGuid;
			pkPacket->Pop(kCmdGuid);
			g_kGMProcessMgr.ExecuteGmOrder(kCmdGuid);
		}break;
	case PT_S_T_TRY_ACCESS_SWITCH_RESULT:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);
			
			SContentsUser kContUser;
			if (S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo(kMemberGuid, true, kContUser))
			{
				INFO_LOG( BM::LOG_LV7, __FL__ << _T("[PT_S_T_TRY_ACCESS_SWITCH_RESULT] Cannot find user MemberID[") << kMemberGuid.str().c_str() << _T("]") );
				break;
			}
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->InsertTarget(kMemberGuid);
			pkActionOrder->kGndKey = SRealmGroundKey(kContUser.sChannel, kContUser.kGndKey);
			pkActionOrder->kCause = CNE_CONTENTS_EVENT;

			ContentsActionEvent kEvent(ECEvent_PT_I_T_KICKUSER);
			SPMO kOrder(IMET_CONTENTS_EVENT, kMemberGuid, kEvent);

			SRemoveWaiter kWaiter( true, kMemberGuid );
			kWaiter.WriteToPacket(pkActionOrder->kAddonPacket);

			g_kJobDispatcher.VPush(pkActionOrder);
		}break;
	case PT_S_T_REQ_CHARACTER_LIST:
		{
			BM::GUID kMemberGuid;
			short nChannelNo;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(nChannelNo);
			
			if(g_kRealmUserMgr.Locked_Recv_PT_S_T_REQ_CHARACTER_LIST(kMemberGuid, nChannelNo) )
			{
//				INFO_LOG( BM::LOG_LV6, _T("[%s]-[%d] RecvReqCharacterList complete"), __FUNCTIONW__, __LINE__ );
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV6, __FL__ << _T("RecvReqCharacterList failed : MemberGuid<") << kMemberGuid.str() << L"> ChannelNo<" << nChannelNo << L">");
			}
		}break;
	case PT_S_T_REQ_FIND_CHARACTOR_EXTEND_SLOT:
		{	
			BM::GUID kMemberGuid;
			short nChannelNo;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(nChannelNo);
			g_kRealmUserMgr.Locked_Recv_PT_S_T_REQ_FIND_CHARACTOR_EXTEND_SLOT(kMemberGuid, nChannelNo);
		}break;
	case PT_S_T_REQ_CREATE_CHARACTER:
		{
			BM::GUID kMemberGuid;
			SReqCreateCharacter kReqCreateCharacter;
			
			pkPacket->Pop(kMemberGuid);
			kReqCreateCharacter.ReadFromPacket(*pkPacket);
			if(kReqCreateCharacter.MakeCorrect()) //보안.
			{
				g_kRealmUserMgr.Locked_Recv_PT_S_T_REQ_CREATE_CHARACTER(kMemberGuid, kReqCreateCharacter);
			}
		}break;
	case PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP:
		{
			BM::GUID kMemberGuid;
			std::wstring kCharacterName;

			pkPacket->Pop( kMemberGuid );
			pkPacket->Pop( kCharacterName );

			g_kRealmUserMgr.Locked_Recv_PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP( kMemberGuid, kCharacterName );
		}break;
	case PT_S_T_REQ_DELETE_CHARACTER:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharacterGuid);
			g_kRealmUserMgr.Locked_Recv_PT_S_T_REQ_DELETE_CHARACTER(kMemberGuid, kCharacterGuid);
		}break;
	case PT_C_N_REQ_REALM_MERGE:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;
			std::wstring kNewName;
			pkPacket->Pop( kMemberGuid );
			pkPacket->Pop( kCharacterGuid );
			pkPacket->Pop( kNewName, MAX_CHARACTER_NAME_LEN );
			g_kRealmUserMgr.Locked_Recv_PT_C_N_REQ_REALM_MERGE(kMemberGuid, kCharacterGuid, kNewName);
		}break;
	case PT_S_T_REQ_SELECT_CHARACTER:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;
			bool bPassTutorial = false;

			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharacterGuid);
			pkPacket->Pop(bPassTutorial);

			g_kRealmUserMgr.Locked_Recv_PT_S_T_REQ_SELECT_CHARACTER(kMemberGuid, kCharacterGuid, bPassTutorial);
		}break;
	case PT_T_N_NFY_SELECT_CHARACTER_FAILED:
		{
			BM::GUID kCharacterGuid;
			pkPacket->Pop( kCharacterGuid );
			g_kRealmUserMgr.Locked_Recv_PT_T_N_NFY_SELECT_CHARACTER_FAILED(kCharacterGuid);
		}break;
	case PT_M_T_REQ_SAVE_CHARACTER:	// Request to SAVE Character
		{
			//	TODO :
			// 1.캐릭터 정보를 Center서버와 동기화 시킬까? 동기화 시켜야 맵서버가 다운되도 그나마 최신상태로 저장이 가능하다.
			// 2.맵에서 보내는 세이브요청 패킷말고도 맵이동간 등에도 세이브가 필요하니...
			//   UpdataPlayerData에서 마지막 세이브 시간을 체크하여 세이브를 하도록하자.
			// 2.로그아웃할 때 이 패킷을 받을 때는 이미 Character 정보가 Center 서버에서 삭제된 상태이다.
			// 2.추가적인 보안이 필요할 텐데 어떻게 할 것인가?

			g_kRealmUserMgr.Locked_Recv_PT_M_T_REQ_SAVE_CHARACTER( pkPacket );		
		}break;
	case PT_N_T_REQ_MSN_FRIENDCOMMAND:
		{
			BM::GUID kCharacterGuid;
			BYTE cCmdType = 0;
			pkPacket->Pop(kCharacterGuid);
			pkPacket->Pop(cCmdType);
			switch(cCmdType)
			{
			case FCT_ADD_BYGUID:
				{
					SFriendItem kFriendItem;
					kFriendItem.ReadFromDBPacket(*pkPacket);

					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_FRIENDLIST_ADD_BYGUID, _T("EXEC [dbo].[up_FriendList_Modify_Add]"));
					kQuery.InsertQueryTarget(kCharacterGuid);
					kQuery.PushStrParam(kCharacterGuid);
					kQuery.PushStrParam(kFriendItem.GroupName());
					kQuery.PushStrParam(kFriendItem.CharGuid());
					kQuery.PushStrParam((int)kFriendItem.ChatStatus());
					kQuery.QueryOwner(kCharacterGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case FCT_DELETE:
				{
					std::wstring kGroupName;
					BM::GUID kFriendGuid;
					SFriendItem kFriendItem;
					kFriendItem.ReadFromDBPacket(*pkPacket);

					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_FRIENDLIST_DEL, _T("EXEC [dbo].[up_FriendList_Modify_Delete]"));
					kQuery.InsertQueryTarget(kCharacterGuid);
					kQuery.PushStrParam(kCharacterGuid);
					kQuery.PushStrParam(kFriendItem.CharGuid());
					kQuery.QueryOwner(kCharacterGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case FCT_MODIFY:
				{
					SFriendItem kFriendItem;
					kFriendItem.ReadFromDBPacket(*pkPacket);

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_FRIENDLIST_MODIFY, _T("EXEC [dbo].[up_FriendList_Modify_Modify]"));
					kQuery.InsertQueryTarget(kCharacterGuid);
					kQuery.PushStrParam(kCharacterGuid);
					kQuery.PushStrParam(kFriendItem.GroupName());
					kQuery.PushStrParam(kFriendItem.CharGuid());
					kQuery.PushStrParam((int)kFriendItem.ChatStatus());
					kQuery.QueryOwner(kCharacterGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break; //만들어야 함
			default:
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("invalid friend command type [") << cCmdType << _T("]") );
				}
			}
		}break;
	case PT_N_T_REQ_MSN_FULL_DATA:
		{//첫 로그인을 했으니!. 갱신합시다.
			BM::GUID kCharacterGuid;
			pkPacket->Pop(kCharacterGuid);

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_FRIENDLIST_SELECT, _T("EXEC [dbo].[up_FriendList_Modify_SELECT]"));
			kQuery.InsertQueryTarget(kCharacterGuid);
			kQuery.PushStrParam(kCharacterGuid);
			kQuery.QueryOwner(kCharacterGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_MCTRL_A_MMC_ANS_SERVER_COMMAND:
		{
			BM::Stream kPacket(usType);
			kPacket.Push(*pkPacket);
			SendToImmigration(kPacket);
		}break;
	case PT_N_T_REQ_COUPLE_COMMAND:
		{
			BYTE cCmdType = 0;
			BYTE cCmdResult = 0;
			
			pkPacket->Pop( cCmdType );
			pkPacket->Pop( cCmdResult );

			switch( cCmdType )
			{
			case CC_Sys_Save:
				{
					BM::GUID kCharGuid;
					BM::GUID kCoupleGuid;
					BM::PgPackedTime kStartDate;
					BYTE cSaveInfoFlag = 0, cCoupleStatus = 0;
					BM::PgPackedTime kLimitDate;

					//
					pkPacket->Pop( kCharGuid );
					pkPacket->Pop( kCoupleGuid );
					pkPacket->Pop( cCoupleStatus );
					pkPacket->Pop( cSaveInfoFlag );

					bool const bSaveStartDate = (0 != (cSaveInfoFlag & CoupleSI_StartTime));
					bool const bSaveLimitDate = (0 != (cSaveInfoFlag & CoupleSI_LimitTime));

					if( bSaveStartDate )					{ pkPacket->Pop( kStartDate );}
					if( bSaveLimitDate && bSaveStartDate )	{ pkPacket->Pop( kLimitDate );}

					//
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_COUPLE_UPDATE, _T("EXEC [dbo].[up_Couple_Proc_Update2]"));
					kQuery.InsertQueryTarget(kCharGuid);
					if(kCoupleGuid.IsNotNull())
					{
						kQuery.InsertQueryTarget(kCoupleGuid);
					}
					kQuery.PushStrParam( kCharGuid );
					kQuery.PushStrParam( kCoupleGuid );
					kQuery.PushStrParam( cCoupleStatus );
					if( bSaveStartDate )					{ kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kStartDate) ); }
					if( bSaveStartDate && bSaveLimitDate )	{ kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kLimitDate) ); }
					kQuery.QueryOwner( kCharGuid );
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case CC_Req_Info:
				{
					BM::GUID kMemberGuid;
					BM::GUID kCharGuid;

					pkPacket->Pop( kMemberGuid );
					pkPacket->Pop( kCharGuid );

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_COUPLE_INFO_SELECT, _T("EXEC [dbo].[up_Couple_Proc_Select]"));
					kQuery.InsertQueryTarget(kCharGuid);
					kQuery.PushStrParam( kCharGuid );
					kQuery.QueryOwner( kMemberGuid );
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case CC_SweetHeartQuestTimeInfo:
				{
					BM::GUID kCharGuid;
					int iQuest = 0;
					int iLearnSkill = 0;
					__int64 SweetHeartQuestTime = 0;
					BM::PgPackedTime CouplePanaltyLimitDate, kEmptyDate;
					CouplePanaltyLimitDate.Clear();
					kEmptyDate.Clear();
					BM::GUID kColorGuid = BM::GUID::NullData();

					pkPacket->Pop( kCharGuid );
					pkPacket->Pop( iQuest );
					pkPacket->Pop( SweetHeartQuestTime );
					pkPacket->Pop( iLearnSkill );
					pkPacket->Pop( CouplePanaltyLimitDate );		
					pkPacket->Pop( kColorGuid );

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_USER_ITEM, _T("EXEC [dbo].[up_Couple_Proc_UpdateSweetHeartInfo]"));
					kQuery.InsertQueryTarget(kCharGuid);
					kQuery.PushStrParam(kCharGuid);
					kQuery.PushStrParam(iQuest);
					kQuery.PushStrParam(SweetHeartQuestTime);
					kQuery.PushStrParam(iLearnSkill);
					kQuery.PushStrParam(kColorGuid);
					if( !(kEmptyDate == CouplePanaltyLimitDate) ) { kQuery.PushStrParam(BM::DBTIMESTAMP_EX(CouplePanaltyLimitDate)); }					
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case CC_CoupleInit:
				{
					BM::GUID kCharGuid;

					pkPacket->Pop( kCharGuid );

					if( BM::GUID::IsNotNull(kCharGuid) )
					{
						BM::PgPackedTime const kEmptyDate;

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Couple_Proc_UpdateDateInfo]");
						kQuery.InsertQueryTarget(kCharGuid);
						kQuery.PushStrParam(kCharGuid);
						kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kEmptyDate) );
						g_kCoreCenter.PushQuery(kQuery);
					}
				}break;
			}

		}break;
	case PT_M_A_REQ_GMCOMMAND:
	case PT_GM_A_REQ_GMCOMMAND:
		{
			g_kGMProcessMgr.RecvGMCommand(pkPacket);
		}break;
 	case PT_C_GM_REQ_RECEIPT_PETITION:
 	case PT_C_GM_REQ_REMAINDER_PETITION:
 		{
 			//Imm서버로 보낸다.
 			BM::Stream kPacket(usType);
 			kPacket.Push(*pkPacket);
 			SendToImmigration(kPacket);
 		}break;
	case PT_M_T_ANS_CHARACTER_BASEINFO:
		{
			INFO_LOG( BM::LOG_LV2, __FL__ << _T("[PT_M_T_ANS_CHARACTER_BASEINFO] not handled packet") );
		}break;
	case PT_T_IM_NFY_RESERVED_SWITCH_INFO:
	case PT_T_IM_REQ_SWITCH_USER_ADD:
		{
			pkPacket->PosAdjust();
			SendToImmigration(*pkPacket);
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			g_kRealmUserMgr.Locked_RecvPT_A_NFY_USER_DISCONNECT(pkPacket, true);
		}break;
	case PT_N_M_REQ_COMPLETEACHIEVEMENT:
		{
			g_kRealmUserMgr.Locked_Recv_PT_N_M_REQ_COMPLETEACHIEVEMENT(pkPacket);
		}break;
	case PT_S_T_REQ_SAVE_CHARACTOR_SLOT:
		{	
			BM::GUID kMemberGuid;//저장 할 캐릭터의 캐릭터GUID
			BM::GUID kCharacterGuid;//저장 할 캐릭터의 캐릭터GUID
			int iSlot = 0;			//저장 할 슬롯 
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharacterGuid);
			pkPacket->Pop(iSlot);
			g_kRealmUserMgr.Locked_Recv_PT_S_T_REQ_SAVE_CHARACTOR_SLOT(kMemberGuid, kCharacterGuid, iSlot);
		}break;
	default:
		{
			if(OnRecvFromServer2(pkSession, usType, pkPacket))	{ return; }
			if(OnRecvFromServer3(pkSession, usType, pkPacket))	{ return; }

			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

bool CALLBACK OnRecvFromServer2( CEL::CSession_Base *pkSession, unsigned short usType, BM::Stream * const pkPacket )
{
	switch(usType)
	{
	case PT_A_RN_WRAPPED_PACKET:// 맵서버 -> 센터서버 -> 콘텐츠서버의 Contents Task
		{
			EContentsMessageType eType;
			pkPacket->Pop(eType);
			
			SendToRealmContents(eType, *pkPacket);
		}break;
	case PT_CN_CN_WRAPPED_PACKET:
		{
			short nTargetChannelNo = 0;
			if ( true == pkPacket->Pop( nTargetChannelNo ) )
			{
				EContentsMessageType eType = PMET_NONE;
				if ( true == pkPacket->Pop(eType) )
				{
					int iSecondType = 0;
					pkPacket->Pop( iSecondType );
					::SendToChannelContents(  nTargetChannelNo, eType, *pkPacket, iSecondType );
				}
			}
		}break;
	case PT_A_CNGND_WRAPPED_PACKET:
		{
			short nTargetChannelNo = 0;
			if ( true == pkPacket->Pop( nTargetChannelNo ) )
			{
				BM::Stream kTPacket( PT_A_CNGND_WRAPPED_PACKET, *pkPacket );
				::SendToCenter( nTargetChannelNo, kTPacket );
			}
		}break;
	case PT_A_I_WRAPPED_PACKET:
		{//ProcessCfg 에서 이렇게 들어오는데.
			SERVER_IDENTITY kSI;
			SGroundKey kKey;

			kSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(kKey); //-> 답 음네;..

			SendToItem( kSI, kKey, *pkPacket);
		}break;
	case PT_A_N_WRAPPED_PACKET:
		{
			PACKET_ID_TYPE usType = 0;
			pkPacket->Pop( usType );
			Recv_PT_A_N_WRAPPED_PACKET( usType, pkPacket );
		}break;
	case PT_A_GND_WRAPPED_PACKET:// 콘텐츠(or 맵)서버 -> 센터서버 -> 맵서버
		{//같은코드 두군데임
// 			SGroundKey kKey;
// 			pkPacket->Pop(kKey);
// 
// 			g_kRealmUserMgr.Locked_SendToMap(kKey, *pkPacket, true);
		}break;
	case PT_A_IM_WRAPPED_PACKET:	// Center -> Contents -> Immigration
		{
			SendToImmigration(*pkPacket);
		}break;
	case PT_A_C_WRAPPED_PACKET:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);
		
			g_kRealmUserMgr.Locked_SendToUser(kMemberGuid, *pkPacket);
		}break;
	case PT_A_A_WRAPPED_PACKET:// 받게 되는 모든 서버마다 해당 프로토콜을 선언해 줘야 함
		{
			INFO_LOG( BM::LOG_LV3, __FL__ << _T("[PT_A_A_WRAPPED_PACKET] Cannot handle this packet") );
			/*
			short nServerNo = 0;
			pkPacket->Pop(nServerNo);

			BM::Stream kPacket(PT_A_A_WRAPPED_PACKET);
			kPacket.Push(*pkPacket);

//			g_kRealmUserMgr.SendToServer(nServerNo, kPacket);
			*/
		}break;
	case PT_M_C_REQ_BUILDER_COMMAND:
		{
			// 맵 서버로 부터 빌더 커맨드가 왔다!
			BuilderCommandProcess(pkPacket);
		}break;
	case PT_M_T_NFY_REFRESH_USERCOUNT:
		{
		}break;
	case PT_A_SEND_NOTICE:
		{
			PgNoticeAction kNotice;
			kNotice.Recv(pkPacket);
		}break;
	case PT_A_N_REQ_BATTLE_PASS_INFO:
		{
			BM::Stream kGPacket(PT_N_A_NFY_BATTLE_PASS_INFO);
			g_kBattlePassMgr.Locked_WriteToPacket(kGPacket);
			pkSession->VSend(kGPacket);
		}break;
	case PT_A_N_REQ_GAMETIME:
		{
			BM::Stream kGPacket(PT_N_A_NFY_GAMETIME);
			g_kEventView.WriteToPacket(false, kGPacket);
			pkSession->VSend(kGPacket);
		}break;
	case PT_T_N_REQ_MAP_MOVE:
		{
			// Req MapMove
			g_kRealmUserMgr.Locked_Recv_PT_T_N_REQ_MAP_MOVE(pkPacket);
		}break;
	case PT_T_N_GODCMD:
		{
			EGMCmdType eGodCommandType = GMCMD_NONE;
			pkPacket->Pop(eGodCommandType);
			OnRecvGodCommand( eGodCommandType, pkPacket );
		}break;
	case PT_T_IM_REQ_CHANNEL_INFORMATION:
		{
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket( *pkPacket );

			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);

			BM::Stream kAnsPacket(PT_IM_T_ANS_CHANNEL_INFORMATION, kMemberGuid );
			g_kRealmMgr.WriteToPacket( kAnsPacket, g_kProcessCfg.RealmNo(), ERealm_SendType_Client_Channel );
			SendToServer( kSI, kAnsPacket );
		}break;
	case PT_T_N_REQ_GET_PVPLOBBY_INFO:
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_PVP_RANK, L"EXEC [dbo].[up_PvP_LoadRanking]" );
			kQuery.InsertQueryTarget( BM::GUID::Create() );
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_M_C_ANS_PARTY_LIST:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);
			g_kRealmUserMgr.Locked_SendToUser(kCharGuid, *pkPacket, false);
		}break;
	case PT_T_T_REQ_JOIN_OTHER_CHANNEL_PARTY:
	case PT_C_T_ANS_JOIN_PARTYFIND_ITEM_CHECK:
	case PT_T_T_ANS_JOIN_PARTYFIND_ITEM_CHECK:
		{
			short ChannelNo = 0;
			pkPacket->Pop(ChannelNo);

			BM::Stream Packet(usType);
			Packet.Push(*pkPacket);

			SendToGlobalPartyMgr( ChannelNo, Packet );
		}break;
	case PT_M_S_NFY_STAT_TRACK_INFO:
		{
			g_kStatTrackMgr.Locked_RecevInfo(*pkPacket);
		}break;
	default:
		{
			//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return true;
}

void BuilderCommandProcess(BM::Stream * const pkPacket)
{
	EBuilderCommand eCommand;
	pkPacket->Pop(eCommand);

	switch(eCommand)
	{
	case EBUILDERCOMMAND_ALLSERVER_NOTICE:
		{
			std::wstring wstrContent;
			pkPacket->Pop(wstrContent);

			BM::Stream kPacket(PT_T_C_NFY_NOTICE);
			kPacket.Push(wstrContent);

			INFO_LOG( BM::LOG_LV5, __FL__ << _T("not implimented EBUILDERCOMMAND_ALLSERVER_NOTICE") );
			// Contents Server에게도 보내자
			//SendToContents(g_kProcessCfg.ServerIdentity(), kPacket);
			// 모든 맵서버로 패킷을 보내자.
			SendToServerType(CEL::ST_MAP, kPacket);
		}break;
	case EBUILDERCOMMAND_KICKUSER:
		{
			// TODO : 유저에게 끊어졌다고 패킷을 보낸다.
			//bool bSuccess = g_kRealmUserMgr.RemoveSwitchUser(rkMemberGuid, SERVER_BIT_ALL);
		}break;
	}
}

void Recv_PT_A_S_REQ_GREETING(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SERVER_IDENTITY kRecvSI;
	kRecvSI.ReadFromPacket(*pkPacket);
	
	std::wstring strVersion;
	pkPacket->Pop(strVersion);
	if( !CProcessConfig::IsCorrectVersion( strVersion, false ) )
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T("Incorrect Version Mine[") << PACKET_VERSION_S << _T("] != There[") << strVersion.c_str() << _T("] R[")
			<< kRecvSI.nRealm << _T("]C[") << kRecvSI.nChannel << _T("]N[") << kRecvSI.nServerNo << _T("]T[" << kRecvSI.nServerType << _T("]") ) );
		pkSession->VTerminate();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Incorrect Version"));
		return;
	}

	if(	(0!=kRecvSI.nChannel) 
	&&	(kRecvSI.nRealm != g_kProcessCfg.RealmNo()))
//	||	kRecvSI.nChannel != g_kProcessCfg.ChannelNo()) )//채널 번호가 없으므로.
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[")
			<< kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]")	);
		pkSession->VTerminate();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Add Server Session Failed"));
		return;
	}

	if(S_OK != g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[")
			<< kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]")	);
		pkSession->VTerminate();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Add Server Session Failed"));
		return;
	}
	
	if(!SetSendWrapper(kRecvSI))
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("SetSendWrapper Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
		pkSession->VTerminate();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("SetSendWrapper Failed"));
		return;
	}

	g_kRealmUserMgr.Locked_OnGreetingServer(kRecvSI, pkSession);

	kRecvSI.WriteToPacket(pkSession->m_kSessionData);//이 세션의 SERVER_IDENTITY 기록

	BM::Stream kSendPacket(PT_A_S_ANS_GREETING);
	SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();
	//모든 서버에 보낼 내용
	//나의 서버 구분자.
	//전체 서버 리스트.
	kSendSI.WriteToPacket(kSendPacket);
	g_kProcessCfg.Locked_Write_ServerList( kSendPacket, 0 );
	
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_CENTER:
		{
			INFO_LOG(BM::LOG_LV7, _T("Send StoreKey(Center)::")<< g_kControlDefMgr.StoreValueKey());
			
			kSendPacket.Push(g_kControlDefMgr.StoreValueKey());//StoreKey. 나감.
			g_kTblDataMgr.DataToPacket(kSendPacket);
			g_kRealmMgr.WriteToPacket(kSendPacket, 0, ERealm_SendType_Server);
			g_kAntiHackMgr.Locked_WriteToPacket(kSendPacket);
			g_kGambleMachine.WriteToPacket(kSendPacket);
			g_kDefPlayTime.WriteToPacket(kSendPacket);

			CONT_DEF_FILTER_UNICODE const* pkCont = NULL;
			g_kTblDataMgr.GetContDef(pkCont);
			PU::TWriteArray_M(kSendPacket, *pkCont);

			if ( CProcessConfig::IsPublicChannel(kRecvSI.nChannel) )
			{
				g_kEmporiaMgr.Locked_ConnectPublicCenter( true );
			}
		}break;
	case CEL::ST_LOG:
		{
 		}break;
	case CEL::ST_MACHINE_CONTROL:
		{
			
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Incorrect Server Type [") << kRecvSI.nServerType << _T("]") );
			pkSession->VTerminate();
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Incorrect Server Type"));
		}break;
	}

{
	//PgTimeCheck kTime( dynamic_cast<PgLogWorker_Base*>(&g_kLogWorker), LT_CAUTION, __FUNCTIONW__, __LINE__);
	DWORD const dwBegin = BM::GetTime32();
	INFO_LOG( BM::LOG_LV9, __FL__ << _T("Before VSend PacketSize[") << kSendPacket.Size() << _T("]") );
	pkSession->VSend(kSendPacket);
}
//	DWORD const dwEnd = BM::GetTime32();
//	INFO_LOG(BM::LOG_LV9, _T("[%s] End VSend ElapsedTime[%d]"), __FUNCTIONW__, dwEnd-dwBegin);
}
