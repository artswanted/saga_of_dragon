#ifndef MACHINE_SMC_PGRECVFROMMMC_H
#define MACHINE_SMC_PGRECVFROMMMC_H

extern void CALLBACK OnConnectToMMC(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToMMC(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromMMC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // MACHINE_SMC_PGRECVFROMMMC_H