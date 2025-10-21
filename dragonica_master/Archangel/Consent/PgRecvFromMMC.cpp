#include "stdafx.h"
#include "Variant/PgMCtrl.h"
#include "PgRecvFromMMC.h"
#include "PgNcServer.h"

bool OnRecvFromMCtrl( WORD wkType, BM::Stream * const pkPacket, CEL::CSession_Base *pkSession)
{
	switch( wkType )
	{
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	case PT_MMC_CONSENT_NFY_INFO:
		{
			g_kNc.Locked_SetServerState(pkPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Incorrect Packet Type : ") << wkType);
			LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return true;
}
