#ifndef WEAPON_VARIANT_BASIC_PGMCTRL_H
#define WEAPON_VARIANT_BASIC_PGMCTRL_H

extern bool MMCCommandProcess(BM::Stream* const pkPacket);
//extern bool ReadyToService(int iConnectionUser = 0, int iMaxUser = 0);
extern void WriteServerInfo(BM::Stream &rkPacket);
extern bool ReadyToService(CEL::eSessionType const rkType, int const iConnectionUser = 0, int const iMaxUser = 0, DWORD const dwProcessID = 0 );

extern bool (CALLBACK *g_kTerminateFunc)(WORD const&);

extern int (*g_kGetMaxUserFunc)();
extern int (*g_kGetNowUserFunc)();
extern void (*g_kSetMaxUserFunc)(int const&);
extern bool (*g_kCheckServerState)();

#endif // WEAPON_VARIANT_BASIC_PGMCTRL_H