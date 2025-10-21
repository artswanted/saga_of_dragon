#ifndef CONSENT_CONSENTSERVER_MAINFRAME_PGTIMER_H
#define CONSENT_CONSENTSERVER_MAINFRAME_PGTIMER_H

extern void CALLBACK Timer_5s_JAPAN(DWORD dwUserData);
// GF
extern void CALLBACK Timer_5s_GF(DWORD dwUserData);
extern void CALLBACK Timer_30s_GF(DWORD dwUserData);
extern void CALLBACK Timer_Ping_GF(DWORD dwUserData);

// GALA
extern void CALLBACK Timer_5s_GALA(DWORD dwUserData);

// Nival
extern void CALLBACK Timer_5s_Nival(DWORD dwUserData);

// FPT
extern void CALLBACK Timer_5s_FPT(DWORD dwUserData);

// NC
extern void CALLBACK Timer_5s_NC(DWORD dwUserData);

// Gravity
extern void CALLBACK Timer_5s_Gravity(DWORD dwUserData);

#endif // CONSENT_CONSENTSERVER_MAINFRAME_PGTIMER_H