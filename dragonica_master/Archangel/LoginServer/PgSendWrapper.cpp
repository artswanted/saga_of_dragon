#include "stdafx.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kImmigrationSI;
SERVER_IDENTITY g_kLogSI;
SERVER_IDENTITY g_kGMSI;

bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI)
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
	case CEL::ST_GMSERVER:
		{
			g_kGMSI = kRecvSI;
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Incorrect ServerIdentity[" << C2L(kRecvSI) << L"]");
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

bool SendToGM(BM::Stream const &rkPacket)
{
	return SendToServer(g_kGMSI, rkPacket);
}