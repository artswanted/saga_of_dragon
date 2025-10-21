#ifndef MACHINE_SMC_PGSENDWRAPPER_H
#define MACHINE_SMC_PGSENDWRAPPER_H

extern Loki::Mutex g_kMutex;
extern CEL::CSession_Base *g_pkMMCSession;

extern bool SendToMMC(BM::Stream const &rkPacket);

#endif // MACHINE_SMC_PGSENDWRAPPER_H