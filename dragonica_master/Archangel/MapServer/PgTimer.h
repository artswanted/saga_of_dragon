#ifndef MAP_MAPSERVER_FRAMEWORK_PGTIMER_H
#define MAP_MAPSERVER_FRAMEWORK_PGTIMER_H

extern void CALLBACK Timer100ms(DWORD dwUserData);
extern void CALLBACK Timer5s(DWORD dwUserData);
extern void CALLBACK Timer30s(DWORD dwUserData);
extern void CALLBACK Timer1s(DWORD dwUserData);
extern void CALLBACK Timer1m(DWORD dwUserData);

#endif // MAP_MAPSERVER_FRAMEWORK_PGTIMER_H