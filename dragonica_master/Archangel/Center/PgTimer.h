#ifndef CENTER_CENTERSERVER_MAINFRAME_PGTIMER_H
#define CENTER_CENTERSERVER_MAINFRAME_PGTIMER_H

extern void CALLBACK Timer1s(DWORD dwUserData);
extern void CALLBACK Timer30s(DWORD dwUserData);
extern void CALLBACK Timer1m(DWORD dwUserData);
extern void CALLBACK TimerPvP(DWORD dwUserData);
extern void CALLBACK TimerCheckShutDown(DWORD dwUserData);
extern void UpdateCurrentUserCount();

#endif // CENTER_CENTERSERVER_MAINFRAME_PGTIMER_H