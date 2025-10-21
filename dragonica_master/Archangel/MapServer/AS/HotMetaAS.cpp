#include "stdafx.h"
#include "hotmeta/hotmeta.h"
#include "ActorEventSystem.h"
#include "PgGroundMgr.h"

using namespace hotmeta;

static void PT_I_M_HOTMETA_SYNC__AES(AES::EVENT_RECIVER pkUnit, AES::EVENT_BODY pkBody, void*)
{
    g_kGndMgr.BroadcastAll(BM::Stream(hotmeta::PT_I_M_HOTMETA_SYNC, *pkBody));
	hotmeta::apply(*pkBody);
}
AES_REGISTER_TYPED_SYSTEM(PT_I_M_HOTMETA_SYNC);