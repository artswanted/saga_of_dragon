#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "HellDart/PgIPChecker.h"
#include "PgRecvFromServer.h"
#include "PgSiteMgr.h"
#include "PgWaitingLobby.h"
#include "ImmTask.h"

extern PgIPChecker g_kIPChecker;

bool LoadDB()
{
	{	CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_IP_FOR_BLOCK, _T("EXEC [dbo].[up_LoadDefIPForBlock]"));	g_kCoreCenter.PushQuery(kQuery, true);	}
	{	CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_IP_FOR_ACCEPT, _T("EXEC [dbo].[up_LoadDefIPForAccept]"));	g_kCoreCenter.PushQuery(kQuery, true);	}
	{	CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_DEF_RESTRICTIONS, _T("EXEC [dbo].[UP_LoadDefRestrictions]"));	g_kCoreCenter.PushQuery(kQuery, true);	}
	{	CEL::DB_QUERY kQuery( DT_MEMBER, DQT_LOAD_DEF_PLAYERPLAYTIME, _T("EXEC [dbo].[up_LoadDefPlayerPlayTime]"));	g_kCoreCenter.PushQuery(kQuery, true);	}
	g_kSiteMgr.LoadFromDB();

	CONT_SERVER_HASH kContServer;
	g_kProcessCfg.Locked_GetServerInfo(CEL::ST_NONE, kContServer);

	SERVER_IDENTITY const &kSI = g_kProcessCfg.ServerIdentity();
	
	CONT_REALM_CANDIDATE kContRealmCandi;
	g_kSiteMgr.Get(kContRealmCandi);
	
	if(g_kRealmMgr.Init(kContRealmCandi, kContServer))//렐름 정보 빌드
	{
		{
			CONT_REALM_CANDIDATE::const_iterator itor_realmcandi = kContRealmCandi.begin();
			while (itor_realmcandi != kContRealmCandi.end())
			{
				if ((*itor_realmcandi).first > 0)
				{
					CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_CHANNEL_NOTICE, _T("EXEC [dbo].[up_LoadChannelNotice]"));
					kQuery.PushStrParam((*itor_realmcandi).second.Notice_TblName());
					kQuery.PushStrParam((*itor_realmcandi).first);
					g_kCoreCenter.PushQuery(kQuery, true);
				}

				++itor_realmcandi;
			}
		}
		{// 
			TCHAR szTBName[100] = _T("TB_Site_RestrictionsOnChannel");
			std::wstring kFileName = g_kProcessCfg.ConfigDir() + _T("DB_Config.ini");
			::GetPrivateProfileString( _T("ETC"), _T("RESTRICT_TBNAME "), szTBName, szTBName, 100, kFileName.c_str());
			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_RESTRICTIONS_ON_CHANNEL, _T("EXEC [dbo].[UP_Load_RestrictionsOnChannel]"));
			kQuery.PushStrParam(std::wstring(szTBName));
			kQuery.PushStrParam(short(0));
			g_kCoreCenter.PushQuery(kQuery);
		}
		
		TBL_SERVERLIST kTblServer;
		if(S_OK == g_kProcessCfg.Locked_GetServerInfo(kSI, kTblServer))//자신 정보로 서버엑셉터 등록함. DB로드는 SvcStart 전이라.
		{
			CEL::INIT_CORE_DESC kImmInit;
			kImmInit.kBindAddr = kTblServer.addrServerNat;
			kImmInit.kNATAddr = kTblServer.addrServerNat;

			kImmInit.OnSessionOpen	= OnAcceptFromServer;
			kImmInit.OnDisconnect	= OnDisConnectToServer;
			kImmInit.OnRecv			= OnRecvFromServer;
			kImmInit.bIsImmidiateActivate = true;
			kImmInit.ServiceHandlerType(CEL::SHT_SERVER);
			
			g_kCoreCenter.Regist( CEL::RT_ACCEPTOR, &kImmInit);
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("None Server Identity Type[") << kSI.nServerType << _T("] Chn[") << kSI.nChannel << _T("] No[") << kSI.nServerNo << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__);
			return false;
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__);
	}

	return true;
}


HRESULT CALLBACK OnDB_EXECUTE(CEL::DB_RESULT &rkResult)
{
	switch(rkResult.QueryType())
	{
	case DQT_TRY_AUTH:
		{
			INFO_LOG(BM::LOG_LV1, __FL__ << L"Q_DQT_TRY_AUTH Begin");
			g_kWaitingLobby.Q_DQT_TRY_AUTH( rkResult );
			INFO_LOG(BM::LOG_LV1, __FL__ << L"Q_DQT_TRY_AUTH End");
		}break;
	case DQT_DISCONNECT_OLDLOGIN:
		{
			g_kWaitingLobby.Q_DQT_DISCONNECT_OLDLOGIN( rkResult );
		}break;
	case DQT_USER_CREATE_ACCOUNT:
	case DQT_GM_FREEZE_ACCOUNT:		
	case DQT_GM_ADD_CASH:
	case DQT_GM_GET_CASH:			
	case DQT_GM_CHANGE_BIRTHDAY:
	case DQT_CHANGE_PASSWORD:		
		{
			SEventMessage kMsg(EIMM_OBJ_GM, PgGMProcessMgr::EGM_2ND_DBRESULT);
			rkResult.WriteToPacket(kMsg);
			g_kImmTask.PutMsg(kMsg);
		}break;
	case DQT_AP_CHECK_ACCOUNT:
	case DQT_AP_CREATE_ACCOUNT:
	case DQT_AP_GET_CASH:
	case DQT_AP_ADD_CASH:
	case DQT_AP_MODIFY_PASSWORD:
	case DQT_AP_CREATE_COUPON:
	case DQT_AP_MODIFY_MOBILELOCK:
	case DQT_AP_TABLE_CONTROL:
	case DQT_TRY_AUTH_CHECKPW_AP:
		{
			SEventMessage kMsg(EIMM_OBJ_AP, PgAPProcessMgr::EAP_2ND_DBRESULT);
			rkResult.WriteToPacket(kMsg);
			g_kImmTask.PutMsg(kMsg);
		}break;
	case DQT_UPDATE_LOGOUT:
		{
			g_kWaitingLobby.Q_DQT_UPDATE_MEMBER_LOGOUT(rkResult);
		}break;
		//임시로 작업해둔다 추후 승현파트장님과 어떻게 처리해야하는지 결정하고 수정해야함.
	case DQT_UPDATE_CONNECTION_CHANNEL:
		{
			g_kWaitingLobby.Q_DQT_UPDATE_CONNECTION_CHANNEL(rkResult);
		}break;
	case DQT_CLEAR_CONNECTION_CHANNEL:
		{
			g_kWaitingLobby.Q_DQT_CLEAR_CONNECTION_CHANNEL(rkResult);		
		}break;
	case DQT_LOAD_SITE_CONFIG:
		{
			g_kSiteMgr.Q_DQT_LOAD_SITE_CONFIG(rkResult);
		}break;
	case DQT_LOAD_REALM_CONFIG:
		{
			g_kSiteMgr.Q_DQT_LOAD_REALM_CONFIG(rkResult);
		}break;
	case DQT_LOAD_RESTRICTIONS_ON_CHANNEL:
		{
			g_kRealmMgr.Q_DQT_LOAD_RESTRICTIONS_ON_CHANNEL(rkResult);
		}break;
	case DQT_LOAD_MAP_CONFIG_STATIC:
		{
			g_kSiteMgr.Q_DQT_LOAD_MAP_CONFIG_STATIC(rkResult);
		}break;
	case DQT_LOAD_MAP_CONFIG_MISSION:
		{
			g_kSiteMgr.Q_DQT_LOAD_MAP_CONFIG_MISSION(rkResult);
		}break;
	case DQT_LOAD_MAP_CONFIG_PUBLIC:
		{
			g_kSiteMgr.Q_DQT_LOAD_MAP_CONFIG_PUBLIC(rkResult);
		}break;
	case DQT_PATCH_VERSION_EDIT:
		{
			g_kSiteMgr.Q_DQT_PATCH_VERSION_EDIT(rkResult);
		}break;
	case DQT_LOAD_IP_FOR_BLOCK:
		{
			g_kIPChecker.Q_DQT_LOAD_IP_FOR_BLOCK(rkResult);
		}break;
	case DQT_LOAD_IP_FOR_ACCEPT:
		{
			g_kIPChecker.Q_DQT_LOAD_IP_FOR_ACCPET(rkResult);
		}break;
	case DQT_ADD_IP_FOR_ACCEPT:
		{
			g_kIPChecker.Q_DQT_ADD_IP_FOR_ACCEPT(rkResult);
		}break;
	case DQT_DEF_RESTRICTIONS:
		{
			g_kRealmMgr.Q_DQT_LOAD_DEF_RESTRICTIONS(rkResult);
		}break;
	case DQT_CREATE_ACCOUNT:
		{
			g_kWaitingLobby.Q_DQT_CREATE_ACCOUNT(rkResult);
		}break;
	case DQT_SAVE_MEMBER_1ST_LOGINED:
		{
			g_kWaitingLobby.Q_DQT_SAVE_MEMBER_1ST_LOGINED(rkResult);
		}break;
	case DQT_CREATE_GRAVITY_ACCOUNT:
	case DQT_CREATE_NC_ACCOUNT:
	case DQT_USER_CREATE_ACCOUNT_GALA:
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << L"Q_DQT_USER_CREATE_ACCOUNT_GALA Begin");
			g_kWaitingLobby.Q_DQT_USER_CREATE_ACCOUNT_GALA(rkResult);
			INFO_LOG(BM::LOG_LV7, __FL__ << L"Q_DQT_USER_CREATE_ACCOUNT_GALA End");
		}break;
	case DQT_UPDATE_MEMBER_PW:
		{
			g_kWaitingLobby.Q_DQT_UPDATE_MEMBER_PW(rkResult);
		}break;
	case DQT_TRY_AUTH_CHECKPW:
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << L"Q_DQT_TRY_AUTH_CHECKPW Begin");
			g_kWaitingLobby.Q_DQT_TRY_AUTH_CHECKPW(rkResult);
			INFO_LOG(BM::LOG_LV7, __FL__ << L"Q_DQT_TRY_AUTH_CHECKPW End");
		}break;
// 	case DQT_TRY_AUTH_CHECKPW_AP:
// 		{
// 			g_kImmTask.DBProcess_AP(static_cast<EDBQueryType>(rkResult.QueryType()), rkResult);
// 		}break;
	case DQT_TRY_AUTH_CHECKPW_OLDLOGIN:
		{
			g_kWaitingLobby.Q_DQT_TRY_AUTH_CHECKPW_OLDLOGIN(rkResult);
		}break;
	case DQT_LOAD_CHANNEL_NOTICE:
		{
			g_kRealmMgr.Q_DQT_LOAD_CHANNEL_NOTICE(rkResult);
		}break;
	case DQT_LOAD_DEF_PLAYERPLAYTIME:
		{
			bool const bResult = g_kWaitingLobby.Q_DQT_LOAD_DEF_PLAYERPLAYTIME(rkResult);
			if(false == bResult)
			{
				SPLAYERPLAYTIMEINFO kInfo;
				g_kDefPlayTime.SetDef(kInfo);
				g_kDefPlayTime.Build();
			}
		
			bool bGmCmd = false;
			if(false == rkResult.contUserData.IsEmpty())
			{
				rkResult.contUserData.Pop(bGmCmd);
			}

			if(bGmCmd)
			{
				BM::Stream kPacket(PT_SYNC_DEF_PLAYERPLAYTIME);
				g_kDefPlayTime.WriteToPacket(kPacket);
				g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kPacket);

				if(rkResult.QueryOwner().IsNotNull())
				{
					PgGMProcessMgr::SendOrderState(rkResult.QueryOwner(), OS_DONE);
				}
			}
		}break;
	case DQT_UPDATE_RESETPLAYERPLAYTIME:		
	case DQT_UPDATE_SETPLAYERPLAYTIMEBYID:
		{
			g_kWaitingLobby.Q_DQT_UPDATE_RESETPLAYERPLAYTIME(rkResult);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled DB Result Type[") << rkResult.QueryType() << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__);
		}break;
	}
	return S_OK;
}

HRESULT CALLBACK OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &)
{
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__);//뭐 리턴도 없고...
	return S_OK;
}
