#include "stdafx.h"
#include "PgMMCManager.h"
#include "PgRecvFromImmigration.h"

void CALLBACK OnConnectToImmigration(CEL::CSession_Base *pkSession)
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if(bIsSucc)
	{	
		if ( !g_kMMCMgr.Locked_SetConnectImmigration( pkSession, true ) )
		{
			pkSession->VTerminate();
		}
		else
		{
			INFO_LOG(BM::LOG_LV6, L"Connect to Immigration Server Success [" << pkSession->Addr().ToString() << L"]" );
		}
		
	}
	else
	{
		g_kMMCMgr.Locked_SetConnectImmigration( pkSession, false );
		std::wcout << L"Connect to Immigration Server Failed [" << pkSession->Addr().ToString().c_str() << L"]" << std::endl;
	}
}

void CALLBACK OnDisConnectToImmigration(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG(BM::LOG_LV5, L"DisConnect to Immigration Server Success [" << pkSession->Addr().ToString() << L"]" );
	g_kMMCMgr.Locked_SetConnectImmigration( pkSession, false );
}

void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
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
		}break;
	case PT_MCTRL_A_MMC_ANS_SERVER_COMMAND:
		{
			E_IMM_MCC_CMD_TYPE eCmdType;
			pkPacket->Pop(eCmdType);
			g_kMMCMgr.Locked_Recv_PT_MCTRL_A_MMC_ANS_SERVER_COMMAND(pkSession, eCmdType, pkPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << L" Invailed Packet Type [" << usType << L"]" );
		}break;
	}
}
