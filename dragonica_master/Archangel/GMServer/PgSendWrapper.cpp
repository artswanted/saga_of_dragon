#include "stdafx.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kImmigrationSI;
SERVER_IDENTITY g_kLogSI;
SERVER_IDENTITY g_kLoginSI;

bool SetSendWrapper(const SERVER_IDENTITY &kRecvSI)
{
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_IMMIGRATION:
		{
			g_kImmigrationSI = kRecvSI;
		}break;
	case CEL::ST_LOG:
		{
			g_kLogSI = kRecvSI;
		}break;
	case CEL::ST_LOGIN:
		{
			g_kLoginSI = kRecvSI;
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Incorrect ServerIdentity[") << C2L(kRecvSI) << _T("]") );
			return false;
		}break;
	}
	return true;
}

bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServer(kSI, rkPacket);
}

bool SendToImmigration(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kImmigrationSI, rkPacket);
}

bool SendToLog(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kLogSI, rkPacket);
}
