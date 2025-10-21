#include "stdafx.h"
#include "PgRecvFromImmigration.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromUser.h"
#include "PgLoginUserMgr.h"
#include "PgSendWrapper.h"
#include "Variant/PgMctrl.h"
#include "Variant/GM_Const.h"

void CALLBACK OnAcceptFromServer(CEL::CSession_Base *pkSession)
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		INFO_LOG(BM::LOG_LV6, __FL__ <<  L"Send EncryptKey");

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Get EncryptKey Failed. Session Terminate!"));
		pkSession->VTerminate();
	}
}
void CALLBACK OnDisConnectToServer(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG(BM::LOG_LV6, __FL__ << L"Close Session Success");
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	if(pkSession->m_kSessionData.Size())
	{
		SERVER_IDENTITY kSI;
		kSI.ReadFromPacket(pkSession->m_kSessionData);
		if(kSI.nServerType == CEL::ST_GMSERVER)
			g_kLoginUserMgr.ClearGmUserData();
	}
}

void CALLBACK OnRecvFromServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	// 세션키로 해당 채널을 찾아 메시지를  multiplexing 함.
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	//INFO_LOG( BM::LOG_LV7, __FL__ << L"Recved Start[" << usType << "]" );

	switch( usType )
	{
	case PT_A_S_REQ_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);


			std::wstring strVersion;
			pkPacket->Pop(strVersion);
			if( !CProcessConfig::IsCorrectVersion( strVersion, false ) )
			{
				//VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Incorrect Version[" << strVersion << L"]");
				INFO_LOG(BM::LOG_LV5, __FL__ << L"Incorrect Version[" << strVersion << L"]");
				pkSession->VTerminate();
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return;
			}

			if(S_OK != g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
			{
				//VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Add Server Session Failed Realm[" << kRecvSI.nRealm << L"] Chn[" << kRecvSI.nChannel << L"] No[" << kRecvSI.nServerNo << L"] Type[" << kRecvSI.nServerType << L"] ");
				INFO_LOG(BM::LOG_LV5, __FL__ << L"Add Server Session Failed Realm[" << kRecvSI.nRealm << L"] Chn[" << kRecvSI.nChannel << L"] No[" << kRecvSI.nServerNo << L"] Type[" << kRecvSI.nServerType << L"] ");
				pkSession->VTerminate();//세션 끊음.
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return;
			}

			if(SetSendWrapper(kRecvSI))
			{
				BM::Stream kSendPacket(PT_A_S_ANS_GREETING);
				SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();
				kSendSI.WriteToPacket(kSendPacket);
				pkSession->VSend(kSendPacket);
			}
		}break;
	case PT_L_O_ANS_TRY_GM_LOGIN:
		{
			g_kLoginUserMgr.ProcessGmLogin(pkPacket);
		}break; 
	case PT_N_O_ANS_CHECK_LOGINED_USER:
	case PT_G_O_ANS_GMCOMMAND:
	case PT_C_O_ANS_SEND_GM_CHAT:
		{
			BM::GUID kReqGuid;
			pkPacket->Pop(kReqGuid);
			BM::Stream kPacket(usType);
			kPacket.Push(*pkPacket);
			g_kLoginUserMgr.SendToGmUser(kReqGuid, kPacket);			
		}break;	
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	}
}
