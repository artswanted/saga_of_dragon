#include "stdafx.h"
#include "Collins/Log.h"
#include "Variant/PgLogUtil.h"
#include "Variant/PgEmporiaFunction.h"
#include "Global.h"
#include "PgTask_Contents.h"
#include "PgGuildMgr.h"
#include "PgServerSetMgr.h"
#include "Variant/PgStringUtil.h"

namespace PgGuildMgrUtil
{
	void SendAutoChangeOwnerMail(BM::GUID const& rkGuildGuid, EGuildAutoChangeOwnerResult const eResult, int const iGuildLevel, std::wstring const& rkNewOwnerName, bool const bDel = false)
	{
		int const iGuildLevel1 = 1;
		int const iGuildLevel2 = 2;
		int const iGuildLevel3 = 3;
		int const iGuildLevel4 = 4;

		std::wstring kTitle, kContents;
		switch( eResult )
		{
		case GACOR_WARNNING_1:
			{
				GetDefString(12000, kTitle);
				switch( iGuildLevel )
				{
				case iGuildLevel1:		{ GetDefString(12003, kContents); }break;
				case iGuildLevel2:		{ GetDefString(12004, kContents); }break;
				case iGuildLevel3:		{ GetDefString(12005, kContents); }break;
				case iGuildLevel4:
				default:				{ GetDefString(12006, kContents); }break;
				}
			}break;
		case GACOR_WARNNING_2:
			{
				GetDefString(12001, kTitle);
				if( !bDel )
				{
					// 변경
					std::wstring kTemp1, kTemp2;
					switch( iGuildLevel )
					{
					case iGuildLevel1:		{ GetDefString(12007, kTemp1); GetDefString(12008, kTemp2); }break;
					case iGuildLevel2:		{ GetDefString(12009, kTemp1); GetDefString(12010, kTemp2); }break;
					case iGuildLevel3:		{ GetDefString(12011, kTemp1); GetDefString(12012, kTemp2); }break;
					case iGuildLevel4:
					default:				{ GetDefString(12013, kTemp1); GetDefString(12014, kTemp2); }break;
					}
					kContents = kTemp1 + rkNewOwnerName + kTemp2;
				}
				else
				{
					// 삭제
					GetDefString(12017, kContents);
				}
			}break;
		case GACOR_RUN:
			{
				GetDefString(12002, kTitle);
				if( !bDel )
				{
					// 변경
					std::wstring kTemp1, kTemp2;
					GetDefString(12015, kTemp1);
					GetDefString(12016, kTemp2);
					kContents = kTemp1 + rkNewOwnerName + kTemp2;
				}
				else
				{
					// 삭제
					GetDefString(12018, kContents);
				}
			}break;
		case GACOR_NONE:
		default:
			{
				// nothing
			}break;
		}

		if( !kTitle.empty()
			&&	!kContents.empty() )
		{
			BM::Stream kPacket(PT_A_N_REQ_GUILD_MAIL);
			kPacket.Push( rkGuildGuid );
			kPacket.Push( static_cast< BYTE >(GMG_Membmer) );
			kPacket.Push( std::wstring() ); // 보낸 사람 없음
			kPacket.Push( kTitle );
			kPacket.Push( kContents );
			::SendToGuildMgr(kPacket);
		}
	}
};

bool PgGuildMgr::ProcessMsg(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);

	SContentsUser kUser;
	PACKET_ID_TYPE wType = 0;
	pkMsg->Pop(wType);
	switch(wType)
	{
	case PT_T_N_NFY_USER_ENTER_GROUND:
		{
			SAnsMapMove_MT kAMM;

			pkMsg->Pop(kAMM);
			kUser.ReadFromPacket(*pkMsg);

			LoginGuildUser(kUser);
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			kUser.ReadFromPacket(*pkMsg);
			LogoutGuildUser(kUser);
		}break;
	case PT_A_U_SEND_TOGUILD_BYCHARGUID:
		{
			BM::GUID kCharGuid;
			BM::Stream::STREAM_DATA kData;
			BM::Stream kPacket;

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kData);

			kPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));

			SendToGuild_ByChar(kCharGuid, kPacket);
		}break;
	case PT_A_U_SEND_BROADCAST_GUILD:
		{
			VEC_GUID kGuildList;
			pkMsg->Pop(kGuildList);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				BM::Stream kPacket;
				kPacket.Push(*pkPacket);

				VEC_GUID::const_iterator guild_itr;
				for ( guild_itr=kGuildList.begin(); guild_itr!=kGuildList.end(); ++guild_itr )
				{
					PgGuild* pkGuild = NULL;
					if ( SUCCEEDED(Get(*guild_itr, pkGuild)) )
					{
						pkGuild->BroadCast(kPacket);
					}
				}
			}	
		}break;
	case PT_A_U_SEND_CHANGE_PLACE:
		{
			kUser.ReadFromPacket(*pkMsg);
			ChangePosGuildUser(kUser);
		}break;
	case PT_T_N_ANS_GUILD_COMMAND_RAW:
		{
			BYTE cCmd = 0;
			BM::GUID kGuid;
			pkMsg->Pop(kGuid);
			pkMsg->Pop(cCmd);
			ProcessTCommand(cCmd, kGuid, pkMsg);
		}break;
	case PT_A_N_REQ_INIT_GUILD_OWNER_LAST_LOGIN_DAY:
		{
			m_kAutoChangeOwnerMgr.GetOwnerLastLoginDay(m_kMgrGuid, true);
		}break;
	case PT_N_C_NFY_GUILD_INV_EXTEND:
		{
			BM::GUID kGuildGuid;
			HRESULT hRet;
			__int64 iCause;
			EInvType kInvType;
			BYTE byExtendSize;

			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(hRet);
			pkMsg->Pop(iCause);
			pkMsg->Pop(kInvType);
			pkMsg->Pop(byExtendSize);

			PgGuild* pkGuild;
			if( S_OK == Get(kGuildGuid, pkGuild ) )
			{
				pkGuild->BroadCastGuildInventoryExtend( hRet, iCause, kInvType, byExtendSize );
			}			
		}break;
	case PT_A_N_REQ_MODIFY_GUILD_ITEM:
		{
			BM::GUID kGuildGuid;
			size_t iCount;
			bool bAddonPacket;
			DB_ITEM_STATE_CHANGE_ARRAY kContDBItemChangeState;

			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(iCount);

			for( size_t i = 0; i < iCount; ++i )
			{
				DB_ITEM_STATE_CHANGE kDBItemChangeState;
				kDBItemChangeState.ReadFromPacket(*pkMsg);
				kContDBItemChangeState.push_back(kDBItemChangeState);
			}

			if( kContDBItemChangeState.empty() )
			{
				break;
			}

			SGuild_Inventory_Log kLog;

			pkMsg->Pop(bAddonPacket);
			if( bAddonPacket )
			{
				BM::Stream kTempPacket(*pkMsg);
				BM::Stream::STREAM_DATA kData;
				kTempPacket.Pop(kData);

				BM::Stream kPacket;
				kPacket.Push(&kData.at(0), kData.size() * sizeof(BM::Stream::STREAM_DATA::value_type));
				
				kLog.ReadFromPacket(kPacket);
			}

			if( SUCCEEDED( ModifyGuildInventory( kGuildGuid, kContDBItemChangeState, kLog.m_kCharName ) )
			&& bAddonPacket )
			{
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_GUILD_INVENTORY_LOG_INSERT, L"EXEC [dbo].[up_Update_Guild_Inventory_Log]");
				kQuery.InsertQueryTarget(kLog.m_kGuildGuid);
				kQuery.QueryOwner(kLog.m_kGuildGuid);
				kQuery.PushStrParam(kLog.m_kGuildGuid);
				kQuery.PushStrParam(kLog.m_kItemGuid);
				kQuery.PushStrParam(kLog.m_kDateTime);
				kQuery.PushStrParam(kLog.m_kCharName);
				kQuery.PushStrParam(kLog.m_kType);
				kQuery.PushStrParam(kLog.m_kItemNo);
				kQuery.PushStrParam(kLog.m_kCount);
				g_kCoreCenter.PushQuery(kQuery);
			}		
		}break;
	case PT_A_N_REQ_GET_ENTRANCE_OPEN_GUILD:
		{ // 길드 목록
			bool bIsGMCommand = false;
			BM::GUID kCharGuid;

			pkMsg->Pop(bIsGMCommand);
			pkMsg->Pop(kCharGuid);

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_ENTRANCEOPEN_LIST, _T("EXEC [dbo].[up_Load_Entrance_OpenGuild]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(m_kMgrGuid);
			kQuery.contUserData.Push(bIsGMCommand);
			kQuery.contUserData.Push(kCharGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_A_N_REQ_DELETE_GUILD_INVENTORY_LOG:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_INVENTORY_LOG_DELETE, _T("EXEC [dbo].[up_Delete_2Week_Guild_Inventory_Log]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(m_kMgrGuid);			
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_A_N_REQ_GET_GUILD_APPLICANT_LIST:
		{ // 길드 신청자 목록
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_APPLICANT_LIST, _T("EXEC [dbo].[up_Load_Guild_Applicant_List]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(m_kMgrGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_A_N_REQ_GUILD_OWNER_LAST_LOGIN_DAY:
		{
			int iCommandType = 0;

			pkMsg->Pop( iCommandType );
			switch( iCommandType )
			{
			case GACOR_NONE:
				{
					int iCount = 0;
					bool bInit = false;
					pkMsg->Pop( iCount );
					pkMsg->Pop( bInit );

					CONT_GUILD_LAST_LOGIN_DAY_RESULT kContGuildOwnerLastLoginDay;
					for( int iCur = 0; iCount > iCur; ++iCur )
					{
						SGuildOwnerLoginDayResult kTemp(*pkMsg);
						kContGuildOwnerLastLoginDay.insert( std::make_pair(kTemp.kGuildGuid, kTemp) );
					}

					m_kAutoChangeOwnerMgr.CheckDay(kContGuildOwnerLastLoginDay);

					CONT_GUILD_LAST_LOGIN_DAY_RESULT::const_iterator iter = kContGuildOwnerLastLoginDay.begin();
					while( kContGuildOwnerLastLoginDay.end() != iter )
					{
						CONT_GUILD_LAST_LOGIN_DAY_RESULT::mapped_type const& rkLastLoginDay = (*iter).second;
						if( bInit )
						{
							// 첫 실행일 때, 삭제 대상 길들은 메모리에 로드 한다
							ReqLoadData(rkLastLoginDay.kGuildGuid);
						}
						else
						{
							switch( rkLastLoginDay.eResult )
							{
							case GACOR_WARNNING_1:
								{
									// Level 1의 경고 메시지
									PgGuildMgrUtil::SendAutoChangeOwnerMail(rkLastLoginDay.kGuildGuid, rkLastLoginDay.eResult, rkLastLoginDay.sGuildLevel, std::wstring());
								}break;
							case GACOR_WARNNING_2:
							case GACOR_RUN: 
								{
									// 다음 길마가 누가 될지 검색
									CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_SELECT_NEXT_OWNER, _T("EXEC [DBO].[UP_GUILD_SELECT_NextOwner]"));
									kQuery.InsertQueryTarget(rkLastLoginDay.kGuildGuid);
									kQuery.PushStrParam( static_cast< int >(rkLastLoginDay.eResult) );
									kQuery.PushStrParam( rkLastLoginDay.kGuildGuid );
									kQuery.PushStrParam( rkLastLoginDay.sGuildLevel );
									kQuery.QueryOwner(rkLastLoginDay.kGuildGuid);
									g_kCoreCenter.PushQuery(kQuery);
								}break;
							case GACOR_NONE:
							default:
								{
									// nothing
								}break;
							}
						}
						++iter;
					}
				}break;
			case GACOR_WARNNING_2:
			case GACOR_RUN:
				{
					int iGuildLevel = 0;
					BM::GUID kGuildGuid, kOldOwner, kNewOwner;
					std::wstring kOldOwnerName, kNewOwnerName;

					pkMsg->Pop( iGuildLevel );
					pkMsg->Pop( kGuildGuid );
					pkMsg->Pop( kOldOwner );
					pkMsg->Pop( kNewOwner );
					pkMsg->Pop( kOldOwnerName );
					pkMsg->Pop( kNewOwnerName );

					if( GACOR_RUN != iCommandType )
					{
						// GACOR_WARNNING_2
						if( BM::GUID::IsNull(kNewOwner) )
						{
							// 삭제 예정 메시지
							PgGuildMgrUtil::SendAutoChangeOwnerMail(kGuildGuid, GACOR_WARNNING_2, iGuildLevel, kNewOwnerName, true);
						}
						else
						{
							// 바뀔 예정 메시지
							PgGuildMgrUtil::SendAutoChangeOwnerMail(kGuildGuid, GACOR_WARNNING_2, iGuildLevel, kNewOwnerName);
						}
					}
					else
					{
						// GACOR_RUN
						if( BM::GUID::IsNull(kNewOwner) )
						{
							// 길드 삭제
							bool bDestroy = true;
							PgGuild* pkGuild = NULL;
							if( S_OK == Get(kGuildGuid, pkGuild) )
							{
								// 엠포리아를 소유중이라면 해체 하지 않는다
								bDestroy = (EMPORIA_KEY_NONE == pkGuild->EmporiaInfo().byType);
							}

							if( bDestroy )
							{
								// 삭제 메시지
								PgGuildMgrUtil::SendAutoChangeOwnerMail(kGuildGuid, GACOR_RUN, iGuildLevel, kNewOwnerName, true);

								// DB쿼리
								BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kOldOwner);
								kDBPacket.Push( static_cast< BYTE >(GC_M_Destroy) );
								kDBPacket.Push( kGuildGuid );
								SendToGuildMgr(kDBPacket);
							}
						}
						else
						{
							// 결과 메시지
							PgGuildMgrUtil::SendAutoChangeOwnerMail(kGuildGuid, GACOR_RUN, iGuildLevel, kNewOwnerName);
							{ // 길마 바꾸고
								BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, BM::GUID());
								kPacket.Push( static_cast< BYTE >(GC_GM_ChangeOwnerKick) );
								kPacket.Push( kGuildGuid );
								kPacket.Push( kOldOwner );
								kPacket.Push( kNewOwner );
								::SendToGuildMgr(kPacket);
							}
							// 원래 길마 킥은 이후에
						}
					}
				}break;
			case GACOR_WARNNING_1:
			default:
				{
					// nothing
				}break;
			}
		}break;
	case PT_M_T_REQ_GUILD_COMMAND:
	case PT_N_T_REQ_GUILD_COMMAND:
		{
			BM::GUID kMembGuid;
			BYTE cCmdType = 0;

			pkMsg->Pop(kMembGuid);
			pkMsg->Pop(cCmdType);

			switch(cCmdType)
			{
			case GC_M_Destroy:
				{
					BM::GUID kGuildGuid;
					pkMsg->Pop(kGuildGuid);

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_PROC, _T("EXEC [DBO].[UP_Guild_Proc_Delete]"));
					kQuery.InsertQueryTarget(MgrGuid());
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam(cCmdType);
					kQuery.PushStrParam(kGuildGuid);
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_Create://UP_GUild_Create
				{
					BM::GUID kGuildGuid;
					BM::GUID kMasterCharGuid;
					std::wstring kGuildName;
					BYTE cEmblem = 0;
					std::wstring kGradeName01;
					std::wstring kGradeName02;
					std::wstring kGradeName03;
					std::wstring kGradeName04;
					std::wstring kGradeName05;

					pkMsg->Pop(kGuildGuid);
					pkMsg->Pop(kMasterCharGuid);
					pkMsg->Pop(kGuildName);
					pkMsg->Pop(cEmblem);
					pkMsg->Pop(kGradeName01);
					pkMsg->Pop(kGradeName02);
					pkMsg->Pop(kGradeName03);
					pkMsg->Pop(kGradeName04);
					pkMsg->Pop(kGradeName05);

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_PROC, _T("EXEC [dbo].[UP_Guild_Proc_Create]"));
					kQuery.InsertQueryTarget(MgrGuid());
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam(cCmdType);
					kQuery.PushStrParam(kGuildGuid);
					kQuery.PushStrParam(kMasterCharGuid);
					kQuery.PushStrParam(kGuildName);
					kQuery.PushStrParam(cEmblem);
					kQuery.PushStrParam(kGradeName01);
					kQuery.PushStrParam(kGradeName02);
					kQuery.PushStrParam(kGradeName03);
					kQuery.PushStrParam(kGradeName04);
					kQuery.PushStrParam(kGradeName05);
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_M_InventorySupervision:
				{
					BM::GUID kGuildGuid;					
					BYTE byAuthority_In = 0;
					BYTE byAuthority_Out = 0;
					BYTE byAuthority[MAX_DB_GUILD_INV_AUTHORITY] = {0,};

					pkMsg->Pop(kGuildGuid);
					pkMsg->Pop(byAuthority_In);
					pkMsg->Pop(byAuthority_Out);

					byAuthority[0] = 1;
					byAuthority[1] = byAuthority_In;
					byAuthority[2] = 2;
					byAuthority[3] = byAuthority_Out;

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_INV_AUTHORITY, _T("EXEC [dbo].[up_Update_GuildInv_Authority]"));
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.QueryOwner(kGuildGuid);
					kQuery.PushStrParam(kGuildGuid);
					kQuery.PushStrParam(byAuthority, MAX_DB_GUILD_INV_AUTHORITY);
					g_kCoreCenter.PushQuery(kQuery);					
				}break;
			case GC_Join:
			case GC_Leave:
			case GC_M_Kick:
			case GC_M_MemberGrade:
				{
					BM::GUID kGuildGuid;
					BM::GUID kCharGuid;
					BYTE cMemberGrade = (BYTE)GMG_Membmer;
					pkMsg->Pop(kGuildGuid);
					pkMsg->Pop(kCharGuid);
					pkMsg->Pop(cMemberGrade);
					std::wstring kStrQuery;

					switch(cCmdType)
					{
					case GC_Join:
						{
							{// 길드가입 신청자 목록에서 지운다.
								DelGuildEntranceApplicant(kCharGuid);
								CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_ENTRANCE_PROCESS, _T("EXEC [dbo].[up_Guild_Entrance_Process]"));
								kQuery.InsertQueryTarget(kGuildGuid);
								kQuery.QueryOwner(kGuildGuid);
								kQuery.PushStrParam(kCharGuid);
								kQuery.PushStrParam(static_cast<BYTE>(AS_COMPLETE));
								g_kCoreCenter.PushQuery(kQuery);
							}

							kStrQuery = _T("EXEC [DBO].[UP_Guild_Member_Proc_Create]");
						}break;
					case GC_Leave:
					case GC_M_Kick:
						{
							kStrQuery = _T("EXEC [DBO].[UP_Guild_Member_Proc_Delete]");
						}break;
					case GC_M_MemberGrade:
						{
							kStrQuery = _T("EXEC [DBO].[UP_Guild_Member_Proc_Modify]");
						}break;
					}
					if( !kStrQuery.empty() )
					{
						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_MEMBER_PROC, kStrQuery);
						kQuery.InsertQueryTarget(kGuildGuid);
						kQuery.PushStrParam(cCmdType);
						kQuery.PushStrParam(kGuildGuid);
						kQuery.PushStrParam(kCharGuid);
						kQuery.PushStrParam(cMemberGrade);
						kQuery.QueryOwner(kMembGuid);
						g_kCoreCenter.PushQuery(kQuery);
					}
				}break;
			case GC_M_Grade:
				{
					SGuildMemberGradeInfo kGradeInfo;
					kGradeInfo.ReadFromPacket(*pkMsg);

					//
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_UPDATE_MEMBER_GRADE, _T("EXEC [DBO].[UP_UPDATE_Guild_Member_Grade]"));
					kQuery.InsertQueryTarget(kGradeInfo.kGuildGuid);
					kQuery.PushStrParam(kGradeInfo.kGuildGuid);
					kQuery.PushStrParam(kGradeInfo.cGradeGroupLevel);
					kQuery.PushStrParam(kGradeInfo.kGradeName[0]);
					kQuery.PushStrParam(kGradeInfo.kGradeName[1]);
					kQuery.PushStrParam(kGradeInfo.kGradeName[2]);
					kQuery.PushStrParam(kGradeInfo.kGradeName[3]);
					kQuery.PushStrParam(kGradeInfo.kGradeName[4]);
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
				//case GC_M_TaxRate:
				//	{
				//		BM::GUID kGuildGuid;
				//		BYTE cTaxRate = 0;
				//		pkMsg->Pop(kGuildGuid);
				//		pkMsg->Pop(cTaxRate);

				//		//
				//		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_UPDATE_TAX_RATE, _T("EXEC [DBO].[UP_UPDATE_Guild_Tax_Rate] ?,?"));
				//		kQuery.PushParam(kGuildGuid);
				//		kQuery.PushParam(cTaxRate);
				//		kQuery.QueryOwner(kMembGuid);
				//		g_kCoreCenter.PushQuery(kQuery);
				//	}break;
			case GC_M_Notice:
				{
					BM::GUID kGuildGuid;
					std::wstring kNotice;
					pkMsg->Pop(kGuildGuid);
					pkMsg->Pop(kNotice);

					//
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_UPDATE_NOTICE, _T("EXEC [DBO].[UP_UPDATE_Guild_Notice]"));
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam(kGuildGuid);
					kQuery.PushStrParam(kNotice);
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_GM_SetLv:
				{
					BM::GUID const& kGuildGuid = kMembGuid;
					int iLv = 0;
					unsigned short sSkillPoint = 0;
					pkMsg->Pop(iLv);
					pkMsg->Pop(sSkillPoint);

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_UPDATE_EXP_LEVEL, _T("EXEC [DBO].[UP_UPDATE_Guild_SetLevel]"));
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam( static_cast<BYTE>(GC_M_SetLv) );
					kQuery.PushStrParam(kGuildGuid);
					kQuery.PushStrParam(iLv);
					kQuery.PushStrParam(sSkillPoint);
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_GM_SetExp:
				{
					BM::GUID const& kGuildGuid = kMembGuid;
					__int64 i64Exp = 0i64;
					pkMsg->Pop(i64Exp);

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_UPDATE_EXP_LEVEL, _T("EXEC [DBO].[UP_UPDATE_Guild_SetExp]"));
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam( static_cast<BYTE>(GC_M_SetExp) );
					kQuery.PushStrParam(kGuildGuid);
					kQuery.PushStrParam(i64Exp);
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_AddExp:
			case GC_M_LvUp:
			case GC_S_SaveBasic:
			case GC_M_AddSkill:
			case GC_M_ChangeMark1:
				{
					BM::GUID const& kGuildGuid = kMembGuid;
					unsigned short sGuildLevel = 0;
					__int64 iGuildExperience = 0;
					unsigned short sSkillPoint = 0;
					BYTE cEmblem = 0;
					BYTE cGuildSkill[MAX_DB_GUILD_SKILL_SIZE] = {0, };
					int iExternInt = 0;
					BM::GUID kOperatorGuid;
					pkMsg->Pop(sGuildLevel);
					pkMsg->Pop(iGuildExperience);
					pkMsg->Pop(cEmblem);
					pkMsg->Pop(sSkillPoint);
					pkMsg->Pop(cGuildSkill);
					pkMsg->Pop(iExternInt);
					pkMsg->Pop(kOperatorGuid);

					//
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_UPDATE_EXP_LEVEL, _T("EXEC [DBO].[up_UPDATE_Guild_Exp_Level]"));
					kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam(cCmdType);
					kQuery.PushStrParam(kGuildGuid);
					kQuery.PushStrParam(sGuildLevel);
					kQuery.PushStrParam(iGuildExperience);
					kQuery.PushStrParam(cEmblem);
					kQuery.PushStrParam(sSkillPoint);
					kQuery.PushStrParam(cGuildSkill, sizeof(cGuildSkill));
					kQuery.PushStrParam(iExternInt);
					kQuery.PushStrParam(kOperatorGuid);
					kQuery.QueryOwner(kGuildGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_M_InventoryCreate:
				{
					BM::GUID const& kGuildGuid = kMembGuid;

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_INVENTORY_CREATE, _T("EXEC [DBO].[up_Guild_Inventory_Create]"));
					kQuery.InsertQueryTarget(kGuildGuid);					
					kQuery.PushStrParam(kGuildGuid);
					kQuery.QueryOwner(kGuildGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_PreCreate:
				{
					std::wstring kGuildName;
					pkMsg->Pop(kGuildName);
					PgStringUtil::Trim<std::wstring>( kGuildName, L" " );

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_CHECK_NAME, _T("EXEC [dbo].[up_Guild_Util_NameCheck]"));
					kQuery.InsertQueryTarget(MgrGuid());
					kQuery.PushStrParam( cCmdType );
					kQuery.PushStrParam( BM::GUID::NullData() );
					kQuery.PushStrParam( kGuildName );
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_M_Rename:
			case GC_GM_Rename:
				{
					BM::GUID kGuildGuid;
					std::wstring kNewGuildName;
					bool bGMCmd = false;

					pkMsg->Pop(kGuildGuid);
					pkMsg->Pop(kNewGuildName);
					pkMsg->Pop(bGMCmd);

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_RENAME, _T("EXEC [dbo].[up_Guild_Util_ReName]"));
					kQuery.InsertQueryTarget(MgrGuid());
					//kQuery.InsertQueryTarget(kGuildGuid);
					kQuery.PushStrParam( cCmdType );
					kQuery.PushStrParam( kGuildGuid );
					kQuery.PushStrParam( kNewGuildName );
					kQuery.PushStrParam( BM::GUID::NullData() );
					kQuery.PushStrParam( int((bGMCmd)? 1: 0) );
					kQuery.QueryOwner(kMembGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case GC_GM_ChangeOwner:
			case GC_GM_ChangeOwnerKick:
			case GC_M_ChangeOwner:
				{
					BM::GUID const &rkGuildGuid = kMembGuid;
					BM::GUID kOldOwnerGuid, kNewOwnerGuid;
					BYTE cOldOwnerGrade = 0, cNewOwnerGrade = 0;

					pkMsg->Pop( kOldOwnerGuid );
					pkMsg->Pop( cOldOwnerGrade );
					pkMsg->Pop( kNewOwnerGuid );
					pkMsg->Pop( cNewOwnerGrade );

					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_CHANGE_OWNER, _T("EXEC [dbo].[up_Guild_ChangeOwner]"));
					kQuery.InsertQueryTarget(rkGuildGuid);
					kQuery.PushStrParam( static_cast< int >(cCmdType) );
					kQuery.PushStrParam( rkGuildGuid );
					kQuery.PushStrParam( kOldOwnerGuid );
					kQuery.PushStrParam( cOldOwnerGrade );
					kQuery.PushStrParam( kNewOwnerGuid );
					kQuery.PushStrParam( cNewOwnerGrade );
					kQuery.QueryOwner(rkGuildGuid);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			default:
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV3, __FL__ << _T("Invalid Guild Command[") << static_cast<int>(cCmdType) << _T("] From Contents server") );
				}break;
			}
		}break;
	case PT_C_N_REQ_GET_ENTRANCE_OPEN_GUILD:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			NotifyEntranceOpenGuildList(kCharGuid);
		}break;		
	case PT_C_N_REQ_GUILD_ENTRANCE:
		{
			BM::GUID kCharGuid;
			BM::GUID kGuildGuid;
			std::wstring wstrCharName;
			short sLevel = 0;
			BYTE byClass = 0;
			std::wstring wStrMsg;
			__int64 i64GuildEntranceFee;

			BM::DBTIMESTAMP_EX kNowTime;
			kNowTime.SetLocalTime();

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(wstrCharName);
			pkMsg->Pop(sLevel);
			pkMsg->Pop(byClass);
			pkMsg->Pop(wStrMsg);
			pkMsg->Pop(i64GuildEntranceFee);

			SContentsUser kUser;
			if( S_OK != ::GetPlayerByGuid( kCharGuid, false, kUser ) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));				
				return false;
			}

			if( !PgGuildMgrUtil::IsCanRejoinUser( kUser ) )
			{
				BM::Stream kRetPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast< BYTE >(GC_ReqJoin));
				kRetPacket.Push( static_cast< BYTE >( GCR_Date_Limit ) );
				kRetPacket.Push( PgGuildMgrUtil::iAfterCanRejoinHour );
				g_kRealmUserMgr.Locked_SendToUser( kUser.kMemGuid, kRetPacket );
				return true;
			}

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_GUILD_REQ_ENTRANCE, _T("EXEC [dbo].[up_Guild_Entrance_Request]") );
			kQuery.InsertQueryTarget( kCharGuid );
			kQuery.QueryOwner( kCharGuid );
			kQuery.PushStrParam( kGuildGuid );
			kQuery.PushStrParam( kCharGuid );
			kQuery.PushStrParam( wStrMsg );
			kQuery.PushStrParam( i64GuildEntranceFee ); // 신청했을 당시의 가입비용
			kQuery.PushStrParam( kNowTime );				
			g_kCoreCenter.PushQuery( kQuery );

			SGuildEntranceApplicant kGuildEntranceApplicant;
			kGuildEntranceApplicant.kCharGuid = kCharGuid;
			kGuildEntranceApplicant.kGuildGuid = kGuildGuid;				
			kGuildEntranceApplicant.wstrName = wstrCharName;
			kGuildEntranceApplicant.sLevel = sLevel;
			kGuildEntranceApplicant.byClass = byClass;
			kGuildEntranceApplicant.wstrMessage = wStrMsg;
			kGuildEntranceApplicant.kDateTime = kNowTime;
			kGuildEntranceApplicant.byState = 0;
			kGuildEntranceApplicant.i64GuildEntranceFee = i64GuildEntranceFee;
			AddGuildEntranceApplicant(kGuildEntranceApplicant, true);
		}break;
	case PT_C_N_REQ_GUILD_ENTRANCE_CANCEL:
		{//신청 취소
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			DelGuildEntranceApplicant(kCharGuid);
		}break;
	case PT_C_N_REQ_GUILD_COMMAND:
		{
			// MapServer로 받은 요청 처리 하기
			BM::GUID kCharGuid;
			BYTE cCmdType = 0;
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(cCmdType);

			bool bRet = ProcessCommand(cCmdType, kCharGuid, pkMsg);
		}break;
	case PT_M_N_REQ_GUILD_COMMAND:
		{
			// MapServer에서만 올것 요청 처리 하기 (유저로 부터 오면 안되는 패킷)
			BM::GUID kCharGuid;
			BYTE cCmdType = 0;
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(cCmdType);

			bool bRet = ProcessMCommand(cCmdType, kCharGuid, pkMsg);
		}break;
	case PT_M_N_NFY_GUILD_INVENTORY_FAIL:
		{
			BM::GUID kGuildGuid;
			int iType;
			SItemPos kCasterPos;
			SItemPos kTargetPos;

			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(iType);
			pkMsg->Pop(kCasterPos);
			pkMsg->Pop(kTargetPos);

			PgGuild *pkGuild = NULL;
			if( SUCCEEDED( Get( kGuildGuid, pkGuild ) ) )
			{
				switch( iType )
				{
				case EGIT_ITEM_MOVE:
					{
						pkGuild->RemoveFixingItem(kCasterPos);
						pkGuild->RemoveFixingItem(kTargetPos);
					}break;
				case EGIT_ITEM_OUT:
					{
						pkGuild->RemoveFixingItem(kCasterPos);
					}break;
				case EGIT_ITEM_IN:
					{
						pkGuild->RemoveFixingItem(kTargetPos);
					}break;
				default:
					{
					}break;
				}
			}
		}break;
	case PT_A_N_REQ_GUILD_MAIL:
		{
			BM::GUID kGuildGuid;
			BYTE byGrade = GMG_Owner;
			std::wstring wstrMail_Sender;
			std::wstring wstrMail_Title;
			std::wstring wstrMail_Contents;
			pkMsg->Pop( kGuildGuid );
			pkMsg->Pop( byGrade );
			pkMsg->Pop( wstrMail_Sender );
			pkMsg->Pop( wstrMail_Title );
			pkMsg->Pop( wstrMail_Contents );

			PgGuild *pkGuild = NULL;
			if ( SUCCEEDED(Get( kGuildGuid, pkGuild ) ) )
			{
				// 길드를 찾으면
				pkGuild->SendMail( byGrade, wstrMail_Sender, wstrMail_Title, wstrMail_Contents );
			}
			else
			{
				// 못찾으면 DB에서 직접 해결 해야 한다.
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_GUILD_SENDMAIL, _T("EXEC [dbo].[up_Post_Create_GuildMail]") );
				kQuery.InsertQueryTarget( kGuildGuid );
				kQuery.QueryOwner( kGuildGuid );

				kQuery.PushStrParam( kGuildGuid );
				kQuery.PushStrParam( byGrade );
				kQuery.PushStrParam( wstrMail_Sender );
				kQuery.PushStrParam( wstrMail_Title );
				kQuery.PushStrParam( wstrMail_Contents );
				g_kCoreCenter.PushQuery( kQuery );
			}
		}break;
	case PT_M_N_NFY_CHANGEABIL:
		{
			BM::GUID kCharGuid;
			WORD sAbilType = 0;
			int iUpdatedVal = 0;

			pkMsg->Pop( kCharGuid );
			pkMsg->Pop( sAbilType );
			pkMsg->Pop( iUpdatedVal );

			ReqUpdateAbil(kCharGuid, sAbilType, iUpdatedVal);
		}break;
	case PT_M_N_REQ_GET_EMPORIA_ADMINISTRATOR:
		{
			BM::GUID kGuildID;
			BM::GUID kCharID;
			//			BM::GUID kNpcID;
			pkMsg->Pop( kGuildID );
			pkMsg->Pop( kCharID );
			// 			pkMsg->Pop( kNpcID );

			PgGuild* pkGuild = NULL;
			if ( SUCCEEDED(Get( kGuildID, pkGuild )) )
			{
				pkMsg->PosAdjust();
				pkMsg->PriType( PMET_EMPORIA );
				pkMsg->Push( pkGuild->IsOwner( kCharID ) );
				g_kContentsTask.PutMsg( *pkMsg );
			}
		}break;
	case PT_M_N_REQ_EMPORIA_ADMINISTRATION:
		{
			BM::GUID kGuildID;
			BM::GUID kCharID;
			BM::GUID kNpcID;
			bool bIsControlFunc = false;
			pkMsg->Pop( kGuildID );
			pkMsg->Pop( kCharID );
			pkMsg->Pop( kNpcID );
			pkMsg->Pop( bIsControlFunc );

			BM::GUID kCheckGuildID;
			if( GetCharToGuild( kCharID, kCheckGuildID) )
			{
				if ( kCheckGuildID == kGuildID )
				{
					PgGuild* pkGuild = NULL;
					if ( SUCCEEDED(Get( kGuildID, pkGuild )) )
					{
						bool const bIsOwner = pkGuild->IsOwner( kCharID );
						if ( true == bIsOwner )
						{
							if ( true == bIsControlFunc )
							{
								short nFuncNo = 0;
								short nWeekCount = 0;
								pkMsg->Pop( nFuncNo );
								pkMsg->Pop( nWeekCount );

								SGuildEmporiaInfo const &kEmporiaInfo = pkGuild->EmporiaInfo();
								if ( EMPORIA_KEY_MINE == kEmporiaInfo.byType )
								{
									int const iIndex = static_cast<int>(kEmporiaInfo.byGrade-1);
									if ( MAX_EMPORIA_GRADE > iIndex )
									{
										CONT_DEF_EMPORIA const *pkDefGuildEmporia = NULL;
										g_kTblDataMgr.GetContDef( pkDefGuildEmporia );
										if ( pkDefGuildEmporia )
										{
											CONT_DEF_EMPORIA::const_iterator def_itr = pkDefGuildEmporia->find( kEmporiaInfo.kID );
											if ( def_itr != pkDefGuildEmporia->end() )
											{
												CONT_DEF_EMPORIA_FUNCTION::const_iterator func_itr = def_itr->second.m_kContDefFunc[iIndex].find( nFuncNo );
												if ( func_itr != def_itr->second.m_kContDefFunc[iIndex].end() )
												{
													__int64 const i64UseExp = static_cast<__int64>(func_itr->second.nPrice_ForWeek) * static_cast<__int64>(nWeekCount);
													if ( true == pkGuild->UseExp( i64UseExp ) )
													{
														pkMsg->PosAdjust();
														pkMsg->PriType( PMET_EMPORIA );
														pkMsg->Push( static_cast<SEmporiaKey>(kEmporiaInfo) );
														pkMsg->Push( i64UseExp );
														g_kContentsTask.PutMsg( *pkMsg );
													}
													else
													{
														// 경험치 부족.
														BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
														kAnsPacket.Push( EFUNC_GUILDEXP );
														kAnsPacket.Push( bIsControlFunc );
														kAnsPacket.Push( nFuncNo );
														kAnsPacket.Push( nWeekCount );
														g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
													}
												}
												else
												{
													CAUTION_LOG( BM::LOG_LV5, __FL__ << L"Not Found Emporia<" << kEmporiaInfo.kID << L"> Grade<" << kEmporiaInfo.byGrade << L"> FucntionNo<" << nFuncNo << L"> CharacterID<" << kCharID << L">" );

													BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
													kAnsPacket.Push( EFUNC_NOT );
													kAnsPacket.Push( bIsControlFunc );
													kAnsPacket.Push( nFuncNo );
													kAnsPacket.Push( nWeekCount );
													g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
												}
											}
											else
											{
												VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found EmporiaID<" << kEmporiaInfo.kID << L">" );

												// 크리티컬 에러네.
												BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
												kAnsPacket.Push( EFUNC_ERROR );
												kAnsPacket.Push( bIsControlFunc );
												kAnsPacket.Push( nFuncNo );
												kAnsPacket.Push( nWeekCount );
												g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
											}
										}
										else
										{
											VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"CONT_DEF_EMPORIA is NULL" );

											// 크리티컬 에러네.
											BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
											kAnsPacket.Push( EFUNC_ERROR );
											kAnsPacket.Push( bIsControlFunc );
											kAnsPacket.Push( nFuncNo );
											kAnsPacket.Push( nWeekCount );
											g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
										}
									}
									else
									{
										VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error Index<" << iIndex << L"Guild<" << kGuildID << L">" );

										// 크리티컬 에러네.
										BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
										kAnsPacket.Push( EFUNC_ERROR );
										kAnsPacket.Push( bIsControlFunc );
										kAnsPacket.Push( nFuncNo );
										kAnsPacket.Push( nWeekCount );
										g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
									}
								}
							}
							else
							{
								pkMsg->PosAdjust();
								pkMsg->PriType( PMET_EMPORIA );
								g_kContentsTask.PutMsg( *pkMsg );
							}
						}
						else
						{
							BM::Stream *pkMsgPacketPoint = dynamic_cast<BM::Stream *>(pkMsg);

							// 오너가 아님
							BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
							kAnsPacket.Push( EFUNC_ACCESSDENIED );
							kAnsPacket.Push( bIsControlFunc );
							kAnsPacket.Push( *pkMsgPacketPoint );
							g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
						}
					}
				}
			}
		}break;
	case PT_N_N_NFY_EMPORIA_INFO:
		{
			BM::GUID kGuildGuid;
			SGuildEmporiaInfo kEmporiaInfo;
			pkMsg->Pop( kGuildGuid );
			pkMsg->Pop( kEmporiaInfo );

			PgGuild* pkGuild = NULL;
			if ( SUCCEEDED(Get(kGuildGuid, pkGuild)) )
			{
				pkGuild->EmporiaInfo(kEmporiaInfo);

				BM::Stream kNfyPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_EmporiaInfo );
				kNfyPacket.Push(static_cast<BYTE>(0));
				kNfyPacket.Push(kEmporiaInfo);
				pkGuild->BroadCast( kNfyPacket );
			}
		}break;
	case PT_N_M_REQ_COMPLETEACHIEVEMENT:
		{// 엠포리아 우승 길드마스터 추가 업적
			BM::GUID kGuildGuid;
			pkMsg->Pop(kGuildGuid);

			const size_t EMPORIA_MASTER_ACHIEVEMENTS_VALUE = 200;

			PgGuild * pkGuild = NULL;		
			if( SUCCEEDED(Get( kGuildGuid, pkGuild ) ) )
			{
				VEC_GUID kContList;
				kContList.push_back( pkGuild->Master() );

				CONT_DEF_CONT_ACHIEVEMENTS const *pkContDef = NULL;
				g_kTblDataMgr.GetContDef(pkContDef);
				if( !pkContDef ){ return false; }

				CONT_DEF_CONT_ACHIEVEMENTS::const_iterator type_citer = pkContDef->find(AT_ACHIEVEMENT_EMPORIA_VICTORY);
				if(type_citer == pkContDef->end()){ return false; }

				CONT_DEF_CONT_ACHIEVEMENTS::mapped_type::const_iterator value_citer = type_citer->second.find(EMPORIA_MASTER_ACHIEVEMENTS_VALUE);
				if(value_citer == type_citer->second.end()){ return false; }

				BM::Stream kPacket(PT_N_M_REQ_COMPLETEACHIEVEMENT);
				kPacket.Push(kContList);
				kPacket.Push( value_citer->second.iSaveIdx );
				::SendToRealmContents( PMET_SendToPacketHandler, kPacket );
			}
		}break;
	case PT_N_N_REQ_GET_SETTING_MERCENARY:
		{
			BM::GUID kEmproiaID;
			size_t iSize = 0;
			pkMsg->Pop( kEmproiaID );
			pkMsg->Pop( iSize );

			BM::Stream kAnsPacket( PT_N_N_ANS_GET_SETTING_MERCENARY, kEmproiaID );

			size_t const iWrPos = kAnsPacket.WrPos();
			kAnsPacket.Push( iSize );

			PgGuild * pkGuild = NULL;
			BM::GUID kGuildID;
			size_t iPushSize = 0;
			for ( size_t i=0; i < iSize ; ++i )
			{
				pkMsg->Pop( kGuildID );
				if ( SUCCEEDED(Get( kGuildID, pkGuild )) )
				{
					kAnsPacket.Push( kGuildID );
					pkGuild->SetMercenary().WriteToPacket( kAnsPacket );
					++iPushSize;
				}
			}

			if ( iPushSize != iSize )
			{
				kAnsPacket.ModifyData( iWrPos, &iPushSize, sizeof(iPushSize) );
			}

			::SendToRealmContents( PMET_EMPORIA, kAnsPacket );
		}break;
	case PT_N_N_ANS_EMPORIA_BATTLE_RESERVE:
		{
            BM::GUID kReqCharGuid;
			BM::GUID kGuildID;
			SGuildEmporiaInfo kEmporiaInfo;
			__int64 i64CostMoney = 0i64;
			bool bThrow = false;
			BYTE byType = GCR_None;
			pkMsg->Pop( kGuildID );
            pkMsg->Pop( kReqCharGuid );
			pkMsg->Pop( kEmporiaInfo );
			pkMsg->Pop( i64CostMoney );
			pkMsg->Pop( bThrow );
			pkMsg->Pop( byType );
			Locked_SetEmporiaChallengeInfo( kReqCharGuid, kGuildID, kEmporiaInfo, i64CostMoney, bThrow, byType );
		}break;
	case PT_N_N_NFY_REFUND_GUILD_EXP:
		{
			BM::GUID kGuildID;
			__int64 i64Exp = 0i64;
			pkMsg->Pop( kGuildID );
			pkMsg->Pop( i64Exp );

			PgGuild *pkGuild = NULL;
			if ( SUCCEEDED(Get(kGuildID, pkGuild)) )
			{
				pkGuild->AddExp( i64Exp );
			}
		}break;
	case PT_N_N_NFY_GUILD_EXP_BROADCAST:
		{
			BM::GUID kGuildID;
			pkMsg->Pop( kGuildID );

			PgGuild *pkGuild = NULL;
			if ( SUCCEEDED(Get(kGuildID, pkGuild)) )
			{
				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_AddExp);
				kPacket.Push( (BYTE)GCR_Success );
				kPacket.Push( pkGuild->GetExp() );
				pkGuild->BroadCast( kPacket );
			}
		}break;
	case PT_M_N_REQ_GET_GUILD_MEMBER_GRADE:
		{
			short nChannel = 0;
			SGroundKey kReqGndKey;
			BM::GUID kCharGuid;
			pkMsg->Pop( nChannel );
			kReqGndKey.ReadFromPacket( *pkMsg );
			pkMsg->Pop( kCharGuid );

			BYTE byGrade = GMG_Membmer;
			BM::GUID kGuildID;
			bool bSuccess = GetCharToGuild( kCharGuid, kGuildID );
			if ( true == bSuccess )
			{
				PgGuild *pkGuild = NULL;
				bSuccess = SUCCEEDED( Get( kGuildID, pkGuild ) );
				if ( true == bSuccess )
				{
					SGuildMemberInfo kGuildMemberInfo;
					bSuccess = pkGuild->Get( kCharGuid, kGuildMemberInfo );
					if ( true == bSuccess )
					{
						byGrade = kGuildMemberInfo.cGrade;
					}
				}
			}

			BM::Stream *pkMsgPacketPoint = dynamic_cast<BM::Stream *>(pkMsg);

			BM::Stream kAnsPacket( PT_N_M_ANS_GET_GUILD_MEMBER_GRADE, kCharGuid );
			kAnsPacket.Push( bSuccess );
			kAnsPacket.Push( byGrade );
			kAnsPacket.Push( *pkMsgPacketPoint );
			::SendToGround( nChannel, kReqGndKey, kAnsPacket, true );
		}break;
	case PT_N_C_NFY_NOTICE_PACKET:
		{
			size_t iSize = 0;
			BM::GUID kGuildGuid;
			pkMsg->Pop( iSize );
			pkMsg->Pop( kGuildGuid );
			PgGuild* pkGuild = NULL;
			if ( SUCCEEDED(Get(kGuildGuid, pkGuild)) )
			{
				BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
				if ( pkPacket )
				{
					BM::Stream kNoticePacket( PT_N_C_NFY_NOTICE_PACKET, iSize );
					kNoticePacket.Push( *pkPacket );
					pkGuild->BroadCast( kNoticePacket );
				}
			}
		}break;
	case PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_GUILD:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			SHOMEADDR kHomeAddr;
			kHomeAddr.ReadFromPacket(*pkMsg);

			BM::GUID kGuildID;
			bool bSuccess = GetCharToGuild( kCharGuid, kGuildID );
			if ( true == bSuccess )
			{
				PgGuild* pkGuild = NULL;
				if ( SUCCEEDED(Get(kGuildID, pkGuild)) )
				{
					if( true == pkGuild->SetMemberHomeAddr(kCharGuid, kHomeAddr) )
					{
						pkMsg->Push(kGuildID);
						ProcessCommand((BYTE)GC_UpdateGuildList, kCharGuid, pkMsg);
					}
				}
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Invalid PacketType[") << pkMsg->SecType() << _T("] for guild command") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}