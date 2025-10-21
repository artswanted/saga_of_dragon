#include "stdafx.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kLogSI;

bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI)
{
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_LOG:
		{
			g_kLogSI = kRecvSI;
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect ServerIdentity[") << C2L(kRecvSI) << _T("]") );
			
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("SERVER_IDENTITY Error"));
			return false;
		}break;
	}
	return true;
}

bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServer(kSI, rkPacket);
}

bool SendToLog(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kLogSI, rkPacket);
}

bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServerType(eServerType, rkPacket);
}

bool SendToContentsServer(short const sRealm, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToContentsServer(sRealm, rkPacket);
}

bool SendDisConnectUser( BYTE cCause, SERVER_IDENTITY const &kSwitchSI, BM::GUID const &kMemberGuid, BYTE byReason )
{
	BM::Stream kNPacket( PT_A_NFY_USER_DISCONNECT, cCause );
	kNPacket.Push(kMemberGuid);
	kNPacket.Push(BM::GUID::NullData());
	kNPacket.Push(byReason);
	kSwitchSI.WriteToPacket( kNPacket );
	return g_kProcessCfg.Locked_SendToContentsServer( kSwitchSI.nRealm, kNPacket );
}

bool SendToConsentServer(BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToConsentServer(rkPacket);
}