#include "stdafx.h"
#include "Lohengrin/Packetstruct_GFAP.h"
#include "Lohengrin/packetstruct.h"
#include "GFMobileLockPacketDef.h"
#include "PgBillingAPServer.h"
#include "PgMsgWorker.h"

PgPacketWorker::PgPacketWorker()
{}

PgPacketWorker::~PgPacketWorker()
{

}

HRESULT PgPacketWorker::VProcess( SMsg *pkMsg )
{
	BM::Stream::DEF_STREAM_TYPE kType = 0;
	pkMsg->m_kPacket.Pop( kType );

	switch ( kType )
	{
	case PT_GF_AP_ANS_MOBILE_AUTH:
		{
			g_kGFServer.Locked_RecvFromMobileLock( kType, &(pkMsg->m_kPacket) );
		}break;
	default:
		{
			g_kBillingAPServer.Locked_OnRecvFromImmigration( kType, pkMsg->m_kPacket );
		}break;
	}
	
	return S_OK;
}
