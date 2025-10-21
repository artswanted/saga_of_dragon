#ifndef MACHINE_MMC_NETWORK_PGRECVFROMSMC_H
#define MACHINE_MMC_NETWORK_PGRECVFROMSMC_H

extern void CALLBACK OnAcceptFromSMC(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToSMC(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromSMC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // MACHINE_MMC_NETWORK_PGRECVFROMSMC_H