#pragma once
#ifdef EXTERNAL_RELEASE
	#error Client side packet handle allow only in offline mode!
#endif

#include "PgMobileSuit.h"
extern bool Recv_PT_Single_Mode(BM::Stream *pkPacket);