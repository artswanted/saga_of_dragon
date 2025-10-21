#include "stdafx.h"
#include "Variant/PgMCtrl.h"
#include "PgRecvFromMMC.h"
#include "PgSendWrapper.h"
#include "PgWaitingLobby.h"
#include "ImmTask.h"

bool OnRecvFromConsent(CEL::CSession_Base * const pkSession, WORD const wkType, BM::Stream * const pkPacket)
{
	switch( wkType )
	{
	case PT_AP_IM_REQ_ORDER:
	case PT_GF_AP_REQ_SET_TABLEDATA:
		{
			SEventMessage kMsg(EIMM_OBJ_AP, PgAPProcessMgr::EAP_2ND_PACKET);
			kMsg.Push(wkType);
			kMsg.Push(*pkPacket);
			g_kImmTask.PutMsg(kMsg);
		}break;
	case PT_CN_IM_ANS_LOGIN_AUTH:
		{
			ETryLoginResult eRet = E_TLR_NONE;
			SAuthInfo kAuthInfo;
			pkPacket->Pop( eRet );
			if ( true == kAuthInfo.ReadFromPacket(*pkPacket) )
			{
				g_kWaitingLobby.SendTryLoginResult( __FUNCTIONW__, __LINE__, kAuthInfo, eRet );
			}
		}break;
	case PT_CN_IM_ANS_LOGIN_LOGIN:
		{
			ETryLoginResult eRet = E_TLR_NONE;
			SAuthInfo kAuthInfo;
			pkPacket->Pop( eRet );
			if ( true == kAuthInfo.ReadFromPacket(*pkPacket) )
			{
				if( E_TLR_SUCCESS == eRet )
				{
					if ( E_TLR_SUCCESS != g_kWaitingLobby.ProcessTryLogin( kAuthInfo, true ) )
					{
						BM::Stream kWPacket( PT_IM_CN_ANS_LOGIN_LOGIN_FAILED, kAuthInfo.ID() );
						::SendToConsentServer( kWPacket );
					}
				}
				else
				{
					g_kWaitingLobby.SendTryLoginResult( __FUNCTIONW__, __LINE__, kAuthInfo, eRet );
				}
			}
		}break;
	case PT_CN_IM_NFY_LOGINBLOCKED:
		{
			bool bBlocked = false;
			if ( true == pkPacket->Pop( bBlocked ) )
			{
				g_kWaitingLobby.SetBlockedLogin( bBlocked );
			}
		}break;
	case PT_CN_IM_NFY_USER_TERMINATE:
		{
			EClientDisconnectedCause kCause = CDC_Normal;
			std::wstring wstrID;
			bool bDisconnected = false;
			BYTE byReason = 0;

			pkPacket->Pop( kCause );
			pkPacket->Pop( wstrID );
			pkPacket->Pop( bDisconnected );
			pkPacket->Pop( byReason );

			if( !g_kWaitingLobby.ProcessUserTerminate( wstrID, kCause, bDisconnected, byReason ) )
			{
				// Consent Server와 동기화에 문제가 있네...
				BM::Stream kWPacket( PT_IM_CN_ANS_USER_TERMINATE_ERROR, wstrID );
				::SendToConsentServer( kWPacket );
			}
		}break;
	case PT_CN_IM_ANS_LOGIN_GALA:
		{
			g_kWaitingLobby.RecvPT_CN_IM_ANS_LOGIN_GALA(pkPacket);
		}break;
	case PT_CN_IM_ANS_LOGIN_AUTH_NC:
		{			
			ETryLoginResult const eRet = g_kWaitingLobby.RecvPT_CN_IM_ANS_LOGIN_AUTH_NC(pkPacket);			
			INFO_LOG(BM::LOG_LV1, __F_P2__(PT_CN_IM_ANS_LOGIN_AUTH_NC) << L"Result = " << eRet);
		}break;
	case PT_CN_IM_ANS_LOGIN_AUTH_GRAVITY:
		{
			ETryLoginResult const eRet = g_kWaitingLobby.RecvPT_CN_IM_ANS_LOGIN_AUTH_GRAVITY(pkPacket);
			INFO_LOG(BM::LOG_LV1, __F_P2__(PT_CN_IM_ANS_LOGIN_AUTH_GRAVITY) << L"Result = " << eRet);
		}break;
	case PT_X_N_ANS_CASH:
		{
			size_t const iRdPos = pkPacket->RdPos();
			SERVER_IDENTITY kTargetSI;
			kTargetSI.ReadFromPacket(*pkPacket);
			pkPacket->RdPos(iRdPos);
			BM::Stream kGPacket(PT_X_N_ANS_CASH);
			kGPacket.Push(*pkPacket);
			SendToContentsServer(kTargetSI.nRealm, kGPacket);
		}break;
	case PT_CN_A_NFY_CASH_NC:
		{
			g_kWaitingLobby.ProcessNotifyCash(pkPacket);
		}break;
	case PT_CN_A_NFY_ITEM:
		{// 컨텐츠로 중계
			BM::Stream kPacket( PT_CN_A_NFY_ITEM );
			g_kProcessCfg.Locked_SendToServerType( CEL::ST_CONTENTS, kPacket );
		}break;
	default:
		{
			return false;
		}break;
	}

	return true;
}
