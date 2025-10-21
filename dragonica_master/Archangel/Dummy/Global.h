
extern STICK_DELAY_TIME g_DelayTime[];

void MakeTimeString(time_t* pkTime, int iOutType, LPTSTR lpszOut, int iOutLen);
void CALLBACK TimerTick1m(DWORD dwUserData);
void CALLBACK TimerTick50msec(DWORD dwUserData);
LPCTSTR GetMainDirectory();