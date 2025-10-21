#include "stdafx.h"
#include <ActorEventSystem.h>
#include "PgSendWrapper.h"
#include "PgStaticRaceGround.h"

static void PT_C_T_REQ_BS_CHANNEL_INFO__AES(AES::EVENT_RECIVER pkUnit, AES::EVENT_BODY pkBody, PgGround*)
{
    BM::Stream kPacket(PT_C_T_REQ_BS_CHANNEL_INFO);
	kPacket.Push( pkUnit->GetID() );
	::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
}
AES_REGISTER_TYPED_SYSTEM(PT_C_T_REQ_BS_CHANNEL_INFO);