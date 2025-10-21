#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGRECVFROMMAP_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGRECVFROMMAP_H

#include "Lohengrin/GMCommand.h"

extern void CALLBACK Recv_PT_A_N_WRAPPED_PACKET( PACKET_ID_TYPE const usType, BM::Stream * const pkPacket );
extern bool OnRecvGodCommand( EGMCmdType const eGodCommandType, BM::Stream * const pkPacket );

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGRECVFROMMAP_H