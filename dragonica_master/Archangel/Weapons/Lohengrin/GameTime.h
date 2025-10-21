#ifndef WEAPON_LOHENGRIN_UTILITY_GAMETIME_H
#define WEAPON_LOHENGRIN_UTILITY_GAMETIME_H

#include "ace/time_value.h"
#include "BM/ClassSupport.h"
#include "BM/Stream.h"

class CGameTime
{
public:

	typedef enum
	{
		EVERY_SUNDAY	= 0,	// 매주 일요일
		EVERY_MONDAY	= 1,	// 매주 월요일
		EVERY_TUEDAY	= 2,	// 매주 화요일
		EVERY_WEDDAY	= 3,	// 매주 수요일
		EVERY_THUDAY	= 4,	// 매주 목요일
		EVERY_FRIDAY	= 5,	// 매주 금요일
		EVERY_SATDAY	= 6,	// 매주 토요일
		EVERY_WEEK_CHK,			// 체크용도로만 사용

		EVERY_DAY		= 100,	// 매일
		EVERY_DAY01		= 101,	// 매월 1
		EVERY_DAY02,
		EVERY_DAY03,
		EVERY_DAY04,
		EVERY_DAY05,
		EVERY_DAY06,
		EVERY_DAY07,
		EVERY_DAY08,
		EVERY_DAY09,
		EVERY_DAY10,
		EVERY_DAY11,
		EVERY_DAY12,
		EVERY_DAY13,
		EVERY_DAY14,
		EVERY_DAY15,
		EVERY_DAY16,
		EVERY_DAY17,
		EVERY_DAY18,
		EVERY_DAY19,
		EVERY_DAY20,
		EVERY_DAY21,
		EVERY_DAY23,
		EVERY_DAY24,
		EVERY_DAY25,
		EVERY_DAY26,
		EVERY_DAY27,
		EVERY_DAY28,
		EVERY_DAY29,
		EVERY_DAY30,
		EVERY_DAY31,
		EVERY_DAY_CHK,

	}ETIMECYCLE_TYPE;

	static __int64 const DEFAULT		= 1i64;
	static __int64 const SECOND			= 10000000i64;
	static __int64 const MILLISECOND	= 10000i64;
	static __int64 const MINUTE			= 60 * SECOND;
	static __int64 const HOUR			= 60 * MINUTE;
	static __int64 const OneDay			= 24 * HOUR;
	static __int64 const OneWeek		= 7 * OneDay;
	static __int64 const OneYear		= 365 * OneDay;

	static __int64 const S20000101		= 125911584000000000i64;

public:
	static const DWORD				ms_dwMaxTime = 0xFFFFFFFF;
	static const unsigned __int64	ms_i64MaxLowTime = static_cast<__int64 const>(ms_dwMaxTime);

	CGameTime(void);
	~CGameTime(void);

	CGameTime( CGameTime const & );
	CGameTime& operator = ( CGameTime const & );

	// Contents Server 만 호출하는 함수
	bool Begin(SYSTEMTIME const* pkLocalTime, SYSTEMTIME const* pkGameTime);
	bool Update(void)const;
	
	// 현재 시간 구하기
	bool GetLocalTime(SYSTEMTIME* pkSystemTime)const;
	bool GetLocalTime(BM::DBTIMESTAMP_EX &rkDBTime)const;
	void GetLocalTime(ACE_Time_Value &rkAceTime)const;
	void GetLocalTime(FILETIME &rkFileTime)const;
	void GetLocalTime(BM::PgPackedTime &rkPackedTime)const;
	__int64 GetLocalSecTime(__int64 const op=SECOND)const;
	__int64 GetLocalSecTimeInDay(__int64 const op)const;

	bool GetGameTime(SYSTEMTIME* pkSystemTime)const;
	bool GetGameTime(BM::DBTIMESTAMP_EX &rkDBTime)const;
	void GetGameTime(ACE_Time_Value &rkAceTime)const;
	void GetGameTime(FILETIME &rkFileTime)const;
	void GetGameTime(BM::PgPackedTime &rkPackedTime)const;
	__int64 GetGameSecTime(__int64 const op=SECOND)const;
	__int64 GetGameSecTimeInDay(__int64 const op)const;

	HRESULT ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(BM::Stream& rkPacket)const;
//	unsigned __int64 GetElapsedTime()const;
//	DWORD GetElapsedTime32() const;
	DWORD GetElapsedTime(void)const;
	DWORD GetElapsedTimeHigh(void)const;
	unsigned __int64 GetElapsedTime64(void)const;

	// Util
	static bool SystemTime2SecTime(SYSTEMTIME const& kSystemTime, __int64& i64SecTime, __int64 const op=SECOND);
	static bool SecTime2SystemTime(__int64 const i64SecTime, SYSTEMTIME& kSystemTime,__int64 const op=SECOND);
	static void AceTime2SecTime(ACE_Time_Value const& kAceTime, __int64& i64SecTime, __int64 const op=SECOND);
	static void SecTime2AceTime(__int64 const i64SecTime, ACE_Time_Value& kAceTime );
	static void FileTime2SecTime(FILETIME const& kFileTime, __int64& i64SecTime, __int64 const op=SECOND);
	static void SecTime2FileTime(__int64 const i64SecTime, FILETIME& kFileTime, __int64 const op=SECOND);
	static void DBTimeEx2SecTime( BM::DBTIMESTAMP_EX const& kDBTimeEx, __int64& i64SecTime, __int64 const op=SECOND );
	static void SecTime2DBTimeEx( __int64 const i64SecTime, BM::DBTIMESTAMP_EX& kDBTimeEx, __int64 const op=SECOND );
	
	static bool SystemTime2AceTime(SYSTEMTIME const* pkSystemTime, ACE_Time_Value* pkAceTime);
	static bool AceTime2SystemTime(const ACE_Time_Value* pkAceTime, SYSTEMTIME* pkSystemTime);

	static __int64 SecTime2InDayTime( __int64 const i64SecTime, __int64 const op );
	static bool IntoInDayTime( __int64 const i64BeginDayTime, __int64 const i64EndDayTime, __int64 const i64CheckTime );

	static bool AddTime( FILETIME *pkFileTime, __int64 const iIncreaseVal );
	static bool AddTime(SYSTEMTIME &rkTime, __int64 const iIncreaseVal = OneDay);
	static bool AddTime(BM::PgPackedTime &rkTime, __int64 const iIncreaseVal = OneDay);

	static bool GetNextTime( __int64& i64RetTime, SYSTEMTIME kConvertTime, ETIMECYCLE_TYPE const _type, __int64 const op=SECOND );
	static bool GetNextTime( SYSTEMTIME& kConvertTime, ETIMECYCLE_TYPE const _type, __int64 const op=SECOND );
	static bool GetNextTime( __int64& i64ConvertTime, ETIMECYCLE_TYPE const _type, __int64 const op=SECOND );	

	static __int64 GetElapsedTime( BM::DBTIMESTAMP_EX const& kStartTime, BM::DBTIMESTAMP_EX const& kEndTime, __int64 const op=SECOND );

	static void SystemTime2String( SYSTEMTIME const &rkTime, LPTSTR lpszBuffer, int iBufferSize);
	static void AceTime2String(const ACE_Time_Value& rkTime, LPTSTR lpszBuffer, int iBufferSize);

protected:

	static bool GetLocalMachineTime(ACE_Time_Value* pkAceTime);
	void GetElapsedTime(ACE_Time_Value* pkAceTime)const;
	HRESULT SyncTime( ACE_Time_Value const& rkLocalTime, ACE_Time_Value const& rkGameTime, DWORD const dwElapsedTime, DWORD const dwElapsedTimeHigh );

private:
	mutable Loki::Mutex	m_kMutex;
	mutable DWORD m_dwLastUpdateTime;

	ACE_Time_Value m_kBeginLocalTime;	// 시작 LocalTime
	ACE_Time_Value m_kBeginGameTime;	// 시작 GameTime

	DWORD		m_dwSyncTime;			// Sync되었을 때의 BM::GetTime32();
	DWORD		m_dwElapsedTimeHigh;

	CLASS_DECLARATION_S(float, GameTimeSpeed);
	HRESULT m_eOldRet;
};

inline DWORD CGameTime::GetElapsedTimeHigh(void)const
{
	return m_dwElapsedTimeHigh;
}

#endif // WEAPON_LOHENGRIN_UTILITY_GAMETIME_H