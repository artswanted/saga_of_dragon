#include "stdafx.h"
#include "PgRecvFromGMTool.h"
#include "PgGMTask.h"

void CALLBACK OnAcceptFromGMTool(CEL::CSession_Base * pkSession)
{
	if( g_kGMTask.Locked_LoginGMTool( pkSession ) )
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("GM Tool success connect! [") << pkSession->Addr().ToString() << _T("]") );
		
		BM::Stream kPacket(PT_A_S_ANS_GREETING);
		pkSession->VSend( kPacket );
	}
	else
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("GM Tool fail connect! [") << pkSession->Addr().ToString() << _T("]") );
		pkSession->VTerminate();
	}
}

void CALLBACK OnDisConnectToGMTool(CEL::CSession_Base * pkSession)
{
	g_kGMTask.Locked_LogOutGMTool( pkSession );
	INFO_LOG( BM::LOG_LV3, __FL__ << _T("GM Tool Disconnect [") << pkSession->Addr().ToString() << _T("]") );
}

void CALLBACK OnRecvFromGMTool(CEL::CSession_Base * pkSession, BM::Stream * const pkPacket)
{
	PgGMTask::RecvGMToolProcess(pkSession, pkPacket);
}
