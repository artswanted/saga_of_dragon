#include "stdafx.h"
#include "PgSendWrapper.h"

Loki::Mutex g_kMutex;
CEL::CSession_Base *g_pkMMCSession = NULL;

bool SendToMMC(BM::Stream const &rkPacket)
{	
	BM::CAutoMutex kLock( g_kMutex );
	if ( g_pkMMCSession )
	{
		return g_pkMMCSession->VSend( rkPacket );
	}
	return false;
}
