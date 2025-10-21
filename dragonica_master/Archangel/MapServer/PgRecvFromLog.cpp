#include "stdafx.h"
#include "PgRecvFromLog.h"

void CALLBACK OnConnectToLog( CEL::CSession_Base *pkSession )
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Connect Success!!!");
		return;
	}
	else
	{
#ifdef _MDo_
		INFO_LOG(BM::LOG_LV7, __FL__<<L"Connect Failed!!!");
#endif
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectLog();
		return;
	}
}

void CALLBACK OnDisConnectToLog( CEL::CSession_Base *pkSession )
{
	INFO_LOG(BM::LOG_LV5, __FL__<<L"Close Session Success");
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	Sleep(1000);
	g_kProcessCfg.Locked_ConnectLog();
}

void CALLBACK OnRecvFromLog(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	WORD wkType = 0;
	pkPacket->Pop(wkType);

	switch( wkType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{//	
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket(*pkPacket);
			if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kSI, pkSession))
			{
				SetSendWrapper(kSI);
			}
 		}break;
//	case PT_A_SEND_LOGMODE:
//		{
//			PgLogFormat::RecvWriteLogType( pkPacket );
//		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << wkType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}
}
