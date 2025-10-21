#include "stdafx.h"
#include "PgRecvFromLog.h"

void CALLBACK OnConnectToLog(CEL::CSession_Base *pkSession)
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{	
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Connect Success [") << pkSession->Addr().ToString().c_str() << _T("]") );
	}
	else
	{
#ifdef _MDo_
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Connect Failed!!!") );
#endif
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	//	g_kProcessCfg.Locked_ConnectLog();
	}
}

void CALLBACK OnDisConnectToLog(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Close Session Success") );

	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
//	g_kProcessCfg.Locked_ConnectLog();
}

void CALLBACK OnRecvFromLog(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	// 세션키로 해당 채널을 찾아 메시지를  multiplexing 함.
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	switch( usType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
			{
				SetSendWrapper(kRecvSI);
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("Greeting Failed"));
			}
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
		}break;
	}
}
