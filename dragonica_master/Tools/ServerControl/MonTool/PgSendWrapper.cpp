#include "StdAfx.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kManagementServerSI;

bool SetSendWrapper(const SERVER_IDENTITY &kRecvSI)
{
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_MACHINE_CONTROL:
		{
			g_kManagementServerSI = kRecvSI;
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Incorrect ServerIdentity") );
			return false;
		}break;
	}
	return true;
}

bool SendToServerType(const CEL::E_SESSION_TYPE eServerType, const BM::CPacket &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServerType(eServerType, rkPacket);
}

bool SendToServer(SERVER_IDENTITY const &kSI, BM::CPacket const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServer(kSI, rkPacket);
}