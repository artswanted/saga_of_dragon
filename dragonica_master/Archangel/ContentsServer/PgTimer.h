#ifndef CONTENTS_CONTENTSSERVER_MAINFRAME_PGTIMER_H
#define CONTENTS_CONTENTSSERVER_MAINFRAME_PGTIMER_H

extern void CALLBACK Timer100ms(DWORD dwUserData);
extern void CALLBACK Timer1s(DWORD dwUserData);
extern void CALLBACK Timer10s(DWORD dwUserData);
extern void CALLBACK Timer1m(DWORD dwUserData);
extern void CALLBACK Timer5m(DWORD dwUserData);
extern void CALLBACK Timer30s(DWORD dwUserData);
extern void CALLBACK TimerDay(DWORD dwUserData);
extern void CALLBACK Timer1m_2(DWORD dwUserData);

namespace ItemCountLogHelper
{
	void Init();
	HRESULT Q_DQT_ADMIN_LOAD_DEFLOGITEMCOUNT(CEL::DB_RESULT &rkResult);
}

#endif // CONTENTS_CONTENTSSERVER_MAINFRAME_PGTIMER_H