#include "stdafx.h"
#include "Variant/PgPartyMgr.h"
#include "PgRecvFromUser.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgIndun.h"
#include "PgGroundMgr.h"

extern bool IsPacketType(BM::Stream::DEF_STREAM_TYPE wkType);

void CALLBACK OnRecvFromUser( SGroundKey const &rkGroundKey, BM::GUID const &rkCharacterGuid, BM::Stream * const pkPacket )
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	if ( true == IsPacketType( usType ) )
	{
		size_t const now_pos = pkPacket->RdPos();
		pkPacket->RdPos(now_pos - sizeof(BM::Stream::DEF_STREAM_TYPE) );

		if( !g_kGndMgr.GroundEventNfy(rkGroundKey, rkCharacterGuid, pkPacket) )
		{
			//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
		}
	}
}
