#ifndef WEAPON_LOHENGRIN_UTILITY_LOGGROUP_H
#define WEAPON_LOHENGRIN_UTILITY_LOGGROUP_H

#include "BM/LogWorker.h"

typedef enum eLogType
{
	LT_NONE				= 0,
	LT_MAIN				= 1,
	LT_CORE				,
	LT_CAUTION			,
	LT_INFO				,
	LT_S_STATE		,
	LT_USER_STATE		,
	LT_CURR_USER		,
	LT_LIVE_CHECK		,//프로세스가 살아있는지 확인.
	LT_HEARTBEAT	,//프로세스가 살아있는지 확인.
	LT_CHATTING			,
	LT_HACKING			,// 해킹 관련 로그 출력
	LT_FILE_LOG			,//로그서버 전용.
	LT_FILE_LOG_NC		,//NC연동 전용.
	MAX_LOG_TYPE		,
}E_LOG_TYPE;
	
#define INFO_LOG_LEVEL(lv)					g_kLogWorker.SetLogLimitLv( LT_INFO, lv );

#define MAIN_LOG(a,b)						{SDebugLogMessage kMsg(LT_MAIN, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define CORE_LOG(a,b)						{SDebugLogMessage kMsg(LT_CORE, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define CAUTION_LOG(a,b)					{SDebugLogMessage kMsg(LT_CAUTION, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define INFO_LOG(a,b)						{SDebugLogMessage kMsg(LT_INFO, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define S_STATE_LOG(a,b)					{SDebugLogMessage kMsg(LT_S_STATE, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define U_STATE_LOG(a,b)					{SDebugLogMessage kMsg(LT_USER_STATE, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define CURR_USER_LOG(a,b)					{SDebugLogMessage kMsg(LT_CURR_USER, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define LIVE_CHECK_LOG(a,b)					/*SDebugLogMessage kMsg(LT_LIVE_CHECK, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);*/
#define SERVER_HEARTBEAT_LOG(a,b)			{SDebugLogMessage kMsg(LT_HEARTBEAT, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define CHAT_LOG(a,b)						{SDebugLogMessage kMsg(LT_CHATTING, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define HACKING_LOG(a,b)					{SDebugLogMessage kMsg(LT_HACKING, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}

#define MAIN_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_MAIN, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define CORE_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_CORE, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define CAUTION_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_CAUTION, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define INFO_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_INFO, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define S_STATE_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_S_STATE, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define U_STATE_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_USER_STATE, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define CURR_USER_LOG_DIRECT(a,b)			{SDebugLogMessage kMsg(LT_CURR_USER, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define LIVE_CHECK_LOG_DIRECT(a,b)			/*SDebugLogMessage kMsg(LT_LIVE_CHECK, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);*/
#define SERVER_HEARTBEAT_LOG_DIRECT(a,b)	{SDebugLogMessage kMsg(LT_HEARTBEAT, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define CHAT_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_CHATTING, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}
#define HACKING_LOG_DIRECT(a,b)				{SDebugLogMessage kMsg(LT_HACKING, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PgLogWorker_Wrapper::PushLog(kMsg);}

extern HRESULT InitLog(std::wstring &kRetFolder, std::wstring const &strSub, DWORD const dwMode = BM::OUTPUT_ALL, BM::E_LOG_LEVEL const kLogLimitLv = BM::LOG_LV9, std::wstring const &strRoot = _T("../../LogFiles/") );
extern void SetLogOutPutType(BM::E_OUPUT_TYPE const kOutPutType);
extern void SetLogLeveLimit(BM::E_LOG_LEVEL const eLevel);

extern void _Call_ASM_INT3();

#define VERIFY_INFO_LOG(bExp, eLevel, vMsg)	{\
	if ( !(bExp) )	\
	{\
		INFO_LOG(eLevel, _T("-=-=-=-=-=-  VERIFY_INFO_LOG  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		INFO_LOG(eLevel, vMsg);	\
		INFO_LOG(eLevel, _T("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		CAUTION_LOG(eLevel, _T("-=-=-=-=-=-  VERIFY_INFO_LOG  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		CAUTION_LOG(eLevel, vMsg);	\
		CAUTION_LOG(eLevel, _T("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
	}	}
		
#define ASSERT_LOG(bExp, eLevel, vMsg)	{\
	if ( !(bExp) )	\
	{\
		INFO_LOG(eLevel, _T("-=-=-=-=-=-  ASSERT_LOG  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		INFO_LOG(eLevel, vMsg);	\
		INFO_LOG(eLevel, _T("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		CAUTION_LOG(eLevel, _T("-=-=-=-=-=-  ASSERT_LOG  -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		CAUTION_LOG(eLevel, vMsg);	\
		CAUTION_LOG(eLevel, _T("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));	\
		 _Call_ASM_INT3();	\
	} }


#define CONVERT_TEXT(T) #T
#define LOG_CONTENTS(T) CONVERT_TEXT([%s]-[%d] T == NULL)
//#define POINTER_VERIFY_INFO_LOG(P)	\
//	if(NULL == P)\
//	{	\
//		VERIFY_INFO_LOG(false, BM::LOG_LV1, _T( LOG_CONTENTS(P) ), __FUNCTIONW__, __LINE__);	\
//	}
#define POINTER_VERIFY_INFO_LOG(P)	\
	if(NULL == P)\
	{	\
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<< _T(LOG_CONTENTS(P)));	\
	}
// POINTER_VERIFY_INFO_LOG( m_pkPointer ); 라 적으면 아래와 같다
//if(NULL == m_pkPointer)
//{
//	VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] m_pkPointer == NULL"), __FUNCTIONW__, __LINE__);
//}=======

#define VPARAM
#define VERIFY_INFO_LOG_RETURN(ret, verify)	\
	if (!VERIFY_INFO_LOG verify)	\
	{	\
		return ret;	\
	}
#define VERIFY_INFO_LOG_RUN(run, verify) \
	if (!VERIFY_INFO_LOG verify)	\
	{	\
		##run;	\
	}


class CFunctionMonitor
{
public:
	CFunctionMonitor(char const* pFunc, int const Line, int LockCount  = 0);
	~CFunctionMonitor();
	
	std::wstring const m_Func;
	int const m_Line;
	int const m_LockCount;
};

#endif // WEAPON_LOHENGRIN_UTILITY_LOGGROUP_H