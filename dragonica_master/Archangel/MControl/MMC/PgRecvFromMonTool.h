#ifndef MACHINE_MMC_NETWORK_PGRECVFROMMONTOOL_H
#define MACHINE_MMC_NETWORK_PGRECVFROMMONTOOL_H

extern void CALLBACK OnAcceptFromMonTool(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToMonTool(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromMonTool(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // MACHINE_MMC_NETWORK_PGRECVFROMMONTOOL_H