#include "stdafx.h"
#include <tchar.h>
#include "ErrorCode.h"
#include "GameTime.h"
#include "LogGroup.h"
#include "lohengrin/Packetstruct.h"

CGameTime::CGameTime(void)
:	m_dwSyncTime(0)	
,	m_dwLastUpdateTime(1)//very important
,	m_dwElapsedTimeHigh(0)
,	m_eOldRet(S_OK)
,	m_kBeginLocalTime(ACE_Time_Value::zero)
,	m_kBeginGameTime(ACE_Time_Value::zero)
,	m_kGameTimeSpeed(1.0f)
{
}

CGameTime::~CGameTime(void)
{
}

CGameTime::CGameTime( CGameTime const &rhs )
:	m_dwSyncTime(rhs.m_dwSyncTime)	
,	m_dwLastUpdateTime(rhs.m_dwLastUpdateTime)
,	m_dwElapsedTimeHigh(rhs.m_dwElapsedTimeHigh)
,	m_eOldRet(rhs.m_eOldRet)
,	m_kBeginLocalTime(rhs.m_kBeginLocalTime)
,	m_kBeginGameTime(rhs.m_kBeginGameTime)
,	m_kGameTimeSpeed(rhs.m_kGameTimeSpeed)
{

}
CGameTime& CGameTime::operator = ( CGameTime const &rhs )
{
	m_dwSyncTime = rhs.m_dwSyncTime;	
	m_dwLastUpdateTime = rhs.m_dwLastUpdateTime;
	m_dwElapsedTimeHigh = rhs.m_dwElapsedTimeHigh;
	m_eOldRet = rhs.m_eOldRet;
	m_kBeginLocalTime = rhs.m_kBeginLocalTime;
	m_kBeginGameTime = rhs.m_kBeginGameTime;
	m_kGameTimeSpeed = rhs.m_kGameTimeSpeed;
	return *this;
}


bool CGameTime::Begin( SYSTEMTIME const* pkLocalTime, SYSTEMTIME const* pkGameTime )
{
	if ( (NULL == pkLocalTime) && (NULL == pkGameTime) )
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"Cannot Begin GameTime because parameter time is NULL");
		return false;
	}

//	BM::CAutoMutex kLock( m_kMutex );

	if ( !SystemTime2AceTime( pkLocalTime, &m_kBeginLocalTime ) )
	{
		// 여기서 실패가 났으면 pkLocalTime이 NULL이다 게임타임만 바꾸려고 시도한것이다.
		// 싱크를 위해서 현재시간을 셋팅해야 한다.
		GetLocalTime( m_kBeginLocalTime );
	}

	if ( !SystemTime2AceTime(pkGameTime, &m_kBeginGameTime) )
	{
		// 여기서 실패가 났으면 pkLocalTime이 NULL이다 현재시간만 바꾸려고 시도한것이다.
		// 싱크를 위해서 게임시간을 셋팅해야 한다.
		GetGameTime( m_kBeginGameTime );
	}

	m_dwSyncTime = BM::GetTime32();
	m_dwLastUpdateTime = m_dwSyncTime + 1;// very important
	m_dwElapsedTimeHigh = 0;
	return true;
}

bool CGameTime::Update(void)const
{
	bool bUpdate = false;

	// 요기는 쓰레드 세이프가 되어야 한다.
	BM::CAutoMutex kLock( m_kMutex );
	DWORD const dwCurTime = BM::GetTime32();
	if ( m_dwLastUpdateTime <= m_dwSyncTime )
	{
		if ( dwCurTime > m_dwSyncTime )
		{
			DWORD *pdwElapsedTimeHigh = const_cast<DWORD*>(&m_dwElapsedTimeHigh);
			++(*pdwElapsedTimeHigh);
			bUpdate = true;

			CAUTION_LOG( BM::LOG_LV6, L"[CGameTime][" << __LINE__ << L"] Update High ElapsedTime" );
		}
	}
	else
	{
		if ( m_dwLastUpdateTime > dwCurTime )
		{
			if ( dwCurTime > m_dwSyncTime )
			{
				DWORD *pdwElapsedTimeHigh = const_cast<DWORD*>(&m_dwElapsedTimeHigh);
				++(*pdwElapsedTimeHigh);
				bUpdate = true;

				CAUTION_LOG( BM::LOG_LV6, L"[CGameTime][" << __LINE__ << L"] Update High ElapsedTime" );
			}
		}
	}

	m_dwLastUpdateTime = dwCurTime;
	if (m_dwLastUpdateTime == m_dwSyncTime)
	{
		m_dwLastUpdateTime = m_dwSyncTime + 1;// very important
	}
	return bUpdate;
}

void CGameTime::GetLocalTime(ACE_Time_Value &rkAceTime)const
{
	ACE_Time_Value kElapsedTime;
	GetElapsedTime(&kElapsedTime);
	rkAceTime = m_kBeginLocalTime + kElapsedTime;
}

bool CGameTime::GetLocalTime(SYSTEMTIME* pkSystemTime)const
{
	if (pkSystemTime == NULL)
	{
		return false;
	}
	
	ACE_Time_Value kLocalAce;
	GetLocalTime(kLocalAce);
	if (!AceTime2SystemTime(&kLocalAce, pkSystemTime))
	{
		return false;
	}
	return true;
}

bool CGameTime::GetLocalTime(BM::DBTIMESTAMP_EX &rkDBTime)const
{
	__int64 i64SecTime = GetLocalSecTime( DEFAULT );
	SecTime2DBTimeEx( i64SecTime, rkDBTime, DEFAULT );
	return true;
}

void CGameTime::GetLocalTime(FILETIME &rkFileTime)const
{
	ACE_Time_Value kLocalAce;
	GetLocalTime(kLocalAce);
	rkFileTime = kLocalAce;
}

void CGameTime::GetLocalTime( BM::PgPackedTime &rkPackedTime )const
{
	SYSTEMTIME kSystemTime;
	GetLocalTime( &kSystemTime );
	rkPackedTime = kSystemTime;
}

__int64 CGameTime::GetLocalSecTime(__int64 const op)const
{
	__int64 i64SecTime = 0;
	ACE_Time_Value kElapsedTime;
	GetElapsedTime(&kElapsedTime);
	ACE_Time_Value kLocalAce = m_kBeginLocalTime + kElapsedTime;
	AceTime2SecTime(kLocalAce,i64SecTime,op);
	return i64SecTime;
}

__int64 CGameTime::GetLocalSecTimeInDay(__int64 const op)const
{
	__int64 i64SecTime = GetLocalSecTime(op);
	return SecTime2InDayTime( i64SecTime, op );
}

void CGameTime::GetGameTime(ACE_Time_Value &rkAceTime)const
{
	ACE_Time_Value kElapsedTime;
	GetElapsedTime(&kElapsedTime);
	rkAceTime = m_kBeginGameTime + kElapsedTime * GameTimeSpeed();
}

bool CGameTime::GetGameTime(SYSTEMTIME* pkSystemTime)const
{
	if (pkSystemTime == NULL)
	{
		return false;
	}

	ACE_Time_Value kAceTime;
	GetGameTime(kAceTime);
	if (!AceTime2SystemTime(&kAceTime, pkSystemTime))
	{
		return false;
	}
	return true;
}

bool CGameTime::GetGameTime(BM::DBTIMESTAMP_EX &rkDBTime)const
{
	__int64 i64SecTime = GetGameSecTime( DEFAULT );
	SecTime2DBTimeEx( i64SecTime, rkDBTime, DEFAULT );
	return true;
}

void CGameTime::GetGameTime(FILETIME &rkFileTime)const
{
	ACE_Time_Value kLocalAce;
	GetGameTime(kLocalAce);
	rkFileTime = kLocalAce;
}

void CGameTime::GetGameTime( BM::PgPackedTime &rkPackedTime )const
{
	SYSTEMTIME kSystemTime;
	GetGameTime( &kSystemTime );
	rkPackedTime = kSystemTime;
}

__int64 CGameTime::GetGameSecTime(__int64 const op)const
{
	__int64 i64SecTime = 0;
	ACE_Time_Value kElapsedTime;
	GetElapsedTime(&kElapsedTime);
	ACE_Time_Value kAceTime = m_kBeginGameTime + kElapsedTime * GameTimeSpeed();
	AceTime2SecTime(kAceTime,i64SecTime,op);
	return i64SecTime;
}

__int64 CGameTime::GetGameSecTimeInDay(__int64 const op)const
{
	__int64 i64SecTime = GetGameSecTime(op);
	return SecTime2InDayTime( i64SecTime, op );
}

bool CGameTime::SystemTime2SecTime(SYSTEMTIME const& kSystemTime,__int64& i64SecTime,__int64 const op)
{
	FILETIME kFileTime;
	if ( ::SystemTimeToFileTime(&kSystemTime,&kFileTime) )
	{
		FileTime2SecTime(kFileTime,i64SecTime,op);
		return true;
	}
	return false;
}

bool CGameTime::SecTime2SystemTime(__int64 const i64SecTime,SYSTEMTIME& kSystemTime,__int64 const op)
{
	FILETIME kFileTime;
	SecTime2FileTime(i64SecTime,kFileTime,op);
	return ::FileTimeToSystemTime(&kFileTime,&kSystemTime);
}

void CGameTime::AceTime2SecTime(const ACE_Time_Value& kAceTime,__int64& i64SecTime,__int64 const op)
{
	FILETIME kFileTime = kAceTime;
	FileTime2SecTime(kFileTime,i64SecTime,op);
}

void CGameTime::SecTime2AceTime( __int64 const i64SecTime, ACE_Time_Value& kAceTime )
{
	FILETIME kFileTime;
	SecTime2FileTime( i64SecTime, kFileTime, DEFAULT );
	kAceTime.set(kFileTime);
}

void CGameTime::FileTime2SecTime(const FILETIME& kFileTime,__int64& i64SecTime,__int64 const op)
{
	__int64 const *pTime = (__int64 const*)(&kFileTime);
	i64SecTime = *pTime / op;
}

void CGameTime::SecTime2FileTime(__int64 const i64SecTime,FILETIME& kFileTime,__int64 const op)
{
	__int64 *pTime = (__int64*)(&kFileTime);
	*pTime = i64SecTime * op;
}

bool CGameTime::SystemTime2AceTime(SYSTEMTIME const* pkSystemTime, ACE_Time_Value* pkAceTime)
{
	if (pkSystemTime == NULL || pkAceTime == NULL)
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"Parameter is NULL");
		return false;
	}
	FILETIME kFileTime;
	if (!::SystemTimeToFileTime(pkSystemTime, &kFileTime))
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"SystemTimeToFileTime error["<<GetLastError()<<L"]");
		return false;
	}
	pkAceTime->set(kFileTime);
	return true;
}

bool CGameTime::AceTime2SystemTime(const ACE_Time_Value* pkAceTime, SYSTEMTIME* pkSystemTime)
{
	if (pkSystemTime == NULL || pkAceTime == NULL)
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"Parameter is NULL");
		return false;
	}
	FILETIME kFileTime = *pkAceTime;
	if (!::FileTimeToSystemTime(&kFileTime, pkSystemTime))
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"FileTimeToSystemTime error["<<GetLastError()<<L"]");
		return false;
	}
	return true;
}

void CGameTime::DBTimeEx2SecTime( BM::DBTIMESTAMP_EX const& kDBTimeEx, __int64& i64SecTime, __int64 const op )
{
	SYSTEMTIME kSystemTime;
	kSystemTime.wYear = (WORD)kDBTimeEx.year;
	kSystemTime.wMonth = (WORD)kDBTimeEx.month;
	kSystemTime.wDay = (WORD)kDBTimeEx.day;
	kSystemTime.wDayOfWeek = 0;
	kSystemTime.wHour = (WORD)kDBTimeEx.hour;
	kSystemTime.wMinute = (WORD)kDBTimeEx.minute;
	kSystemTime.wSecond = (WORD)kDBTimeEx.second;
	kSystemTime.wMilliseconds = 0;
	kSystemTime.wDay = (WORD)kDBTimeEx.day;
	SystemTime2SecTime( kSystemTime, i64SecTime, op );
}


void CGameTime::SecTime2DBTimeEx( __int64 const i64SecTime, BM::DBTIMESTAMP_EX& kDBTimeEx, __int64 const op )
{
	SYSTEMTIME kSystemTime;
	SecTime2SystemTime( i64SecTime, kSystemTime, op );
	kDBTimeEx = kSystemTime;
}

__int64 CGameTime::SecTime2InDayTime( __int64 const i64SecTime, __int64 const op )
{
	__int64 const i64OneDayTime = CGameTime::OneDay / op;
	return i64SecTime % i64OneDayTime;
}

bool CGameTime::IntoInDayTime( __int64 const i64BeginDayTime, __int64 const i64EndDayTime, __int64 const i64CheckTime )
{
	if ( i64BeginDayTime < i64EndDayTime )
	{
		return i64BeginDayTime <= i64CheckTime && i64EndDayTime >= i64CheckTime;
	}
	return i64BeginDayTime >= i64CheckTime || i64EndDayTime <= i64CheckTime;
}

HRESULT CGameTime::ReadFromPacket(BM::Stream& rkPacket)
{
	ACE_Time_Value kLocal, kGame;
	DWORD dwElapsedTime = 0;
	DWORD dwElapsedTimeHigh = 0;
	rkPacket.Pop(kLocal);
	rkPacket.Pop(kGame);
	rkPacket.Pop(dwElapsedTime);
	rkPacket.Pop(dwElapsedTimeHigh);
	rkPacket.Pop(m_kGameTimeSpeed);
	
	return SyncTime( kLocal, kGame, dwElapsedTime, dwElapsedTimeHigh );
}

void CGameTime::WriteToPacket(BM::Stream& rkPacket)const
{
	rkPacket.Push(m_kBeginLocalTime);
	rkPacket.Push(m_kBeginGameTime);
	rkPacket.Push(GetElapsedTime());
	rkPacket.Push(GetElapsedTimeHigh());
	rkPacket.Push(GameTimeSpeed());
}

HRESULT CGameTime::SyncTime( ACE_Time_Value const &rkLocalTime, ACE_Time_Value const &rkGameTime, DWORD const dwElapsedTime, DWORD const dwElapsedTimeHigh )
{
	if ( m_kBeginLocalTime == ACE_Time_Value::zero )	// First Time Sync
	{
		// Lock에서 손실되는 오차시간을 조금이라도 막아보기 위해 현재시간 체크를 락을 잡기 전에 한다.
		DWORD const dwCurTime = BM::GetTime32();

		// 요기는 쓰레드 세이프가 되어야 한다.
		BM::CAutoMutex kLock( m_kMutex );

		m_dwSyncTime = dwCurTime - dwElapsedTime;
		m_kBeginLocalTime = rkLocalTime;
		m_kBeginGameTime = rkGameTime;
		m_dwElapsedTimeHigh = dwElapsedTimeHigh;
		m_dwLastUpdateTime = m_dwSyncTime + 1;// very important

		TCHAR chTime[100];
		INFO_LOG(BM::LOG_LV6, __FL__<<L"GameTime First Synced");
		AceTime2String(rkLocalTime, chTime, 100);
		INFO_LOG(BM::LOG_LV6, __FL__<<L"\t\tLocalTime:"<<chTime);
		AceTime2String(rkGameTime, chTime, 100);
		INFO_LOG(BM::LOG_LV6, __FL__<<L"\t\tGameTime:"<<chTime);

		if ( m_eOldRet == E_SYNCHED_FIRST_GAMETIME )
		{
			// Dummy 에서는 계속 발생한다.
			//VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[%s]-[%d] Critical Error!!!"), __FUNCTIONW__, __LINE__ );
		}

		m_eOldRet = E_SYNCHED_FIRST_GAMETIME;
		return E_SYNCHED_FIRST_GAMETIME;
	}
	
	DWORD dwElapsedTimeGap = GetElapsedTime();

	if (dwElapsedTimeGap > dwElapsedTime)
	{
		dwElapsedTimeGap -= dwElapsedTime;
	}
	else
	{
		dwElapsedTimeGap = dwElapsedTime - dwElapsedTimeGap;
	}

	if ( dwElapsedTimeGap > 1000 )	// Too large Time gap
	{
		// Lock에서 손실되는 오차시간을 조금이라도 막아보기 위해 현재시간 체크를 락을 잡기 전에 한다.
		DWORD const dwCurTime = BM::GetTime32();

		// 요기는 쓰레드 세이프가 되어야 한다.
		BM::CAutoMutex kLock( m_kMutex );

		m_dwSyncTime = dwCurTime - dwElapsedTime;
		m_kBeginLocalTime = rkLocalTime;
		m_kBeginGameTime = rkGameTime;
		m_dwElapsedTimeHigh = dwElapsedTimeHigh;
		m_dwLastUpdateTime = m_dwSyncTime + 1;// very important

		TCHAR chTime[100];
		INFO_LOG(BM::LOG_LV5, __FL__<<L"GameTime Synced Again Time Different["<<dwElapsedTimeGap<<L"]");
		AceTime2String(rkLocalTime, chTime, 100);
		INFO_LOG(BM::LOG_LV6, __FL__<<L"\t\tLocalTime:"<<chTime);
		AceTime2String(rkGameTime, chTime, 100);
		INFO_LOG(BM::LOG_LV6, __FL__<<L"\t\tGameTime:"<<chTime);

		if ( m_eOldRet != E_SYNCHED_AGAIN_GAMETIME )
		{
			m_eOldRet = E_SYNCHED_AGAIN_GAMETIME;
		}
		else
		{
			INFO_LOG( BM::LOG_LV0, __FL__<<L"Critical Error!! GameTime Sync Error!!! loop...." );
		}
		return m_eOldRet;
	}

	if ( m_eOldRet == E_SYNCHED_AGAIN_GAMETIME )
	{
		m_eOldRet = E_SYNCHED_GAMETIME;
	}
	else
	{
		m_eOldRet = S_OK;
	}
	return m_eOldRet;
}

DWORD CGameTime::GetElapsedTime(void)const
{
	DWORD dwCurTime = BM::GetTime32();
	DWORD const dwSyncTime = m_dwSyncTime;

	if ( dwCurTime < dwSyncTime )
	{
		dwCurTime += ( ms_dwMaxTime - dwSyncTime );
	}
	else
	{
		dwCurTime -= dwSyncTime;
	}

	return dwCurTime;
// 	ACE_Time_Value kElapsed;
// 	GetElapsedTime(&kElapsed);
// 	unsigned __int64 u64Time;
// 	((const ACE_Time_Value)kElapsed).msec(u64Time);
// 	//INFO_LOG(BM::LOG_LV9, _T("[%s] ElapsedTime[%I64d]"), __FUNCTIONW__, u64Time);
// 	return u64Time;
}


// DWORD CGameTime::GetElapsedTime32() const
// {
// 	ACE_Time_Value kElapsed;
// 	GetElapsedTime(&kElapsed);
// 	return ((const ACE_Time_Value)kElapsed).msec();
// }

bool CGameTime::GetLocalMachineTime(ACE_Time_Value* pkAceTime)
{
	SYSTEMTIME kLocalSystem;
	::GetLocalTime(&kLocalSystem);
	if (!SystemTime2AceTime(&kLocalSystem, pkAceTime))
	{
		return false;
	}
	return true;
}

unsigned __int64 CGameTime::GetElapsedTime64(void)const
{
	Update();
	__int64 const i64HighTime = static_cast<__int64>(GetElapsedTimeHigh()) * ms_i64MaxLowTime;
	return static_cast<__int64>(GetElapsedTime()) + i64HighTime;
}

void CGameTime::GetElapsedTime(ACE_Time_Value* pkAceTime)const
{
	__int64 const iElapsedTime64 = static_cast<__int64>(GetElapsedTime64());
	pkAceTime->set( iElapsedTime64 / 1000i64, static_cast<suseconds_t>(iElapsedTime64 % 1000i64) * 1000 );
}

void CGameTime::SystemTime2String( SYSTEMTIME const &rkTime, LPTSTR lpszBuffer, int iBufferSize)
{
	_stprintf_s(lpszBuffer, iBufferSize, _T("%04hd-%02hd-%02hd %02hd:%02hd:%02hd %02hd\""), rkTime.wYear, rkTime.wMonth, rkTime.wDay, rkTime.wHour, rkTime.wMinute, rkTime.wSecond, rkTime.wMilliseconds);
}

void CGameTime::AceTime2String( ACE_Time_Value const &rkTime, LPTSTR lpszBuffer, int iBufferSize)
{
	SYSTEMTIME kSystemTime;
	if ( AceTime2SystemTime(&rkTime, &kSystemTime) )
	{
		SystemTime2String( kSystemTime, lpszBuffer, iBufferSize );
	}
}

bool CGameTime::AddTime( FILETIME *pkFileTime, __int64 const iIncreaseVal )
{
	if ( pkFileTime )
	{
		__int64 *pkTime = reinterpret_cast<__int64*>(pkFileTime);
		*pkTime += iIncreaseVal;
		return true;
	}
	return false;
}

bool CGameTime::AddTime(SYSTEMTIME &rkTime, __int64 const iIncreaseVal)
{
	FILETIME kFileTime;
	if( ! ::SystemTimeToFileTime(&rkTime, &kFileTime) )
	{
		return false;
	}

	AddTime( &kFileTime, iIncreaseVal );
	if( ! ::FileTimeToSystemTime( &kFileTime, &rkTime) )
	{
		return false;
	}
	return true;
}

bool CGameTime::AddTime(BM::PgPackedTime &rkTime, __int64 const iIncreaseVal)
{
	SYSTEMTIME kSysTime = (SYSTEMTIME)rkTime;
	if( !AddTime(kSysTime, iIncreaseVal) )
	{
		return false;
	}
	rkTime = BM::PgPackedTime(kSysTime);
	return true;
}

// Time Cycle
bool CGameTime::GetNextTime( __int64& i64RetTime, SYSTEMTIME kConvertTime, ETIMECYCLE_TYPE const _type, __int64 op )
{
	if ( _type < EVERY_WEEK_CHK )
	{
		SystemTime2SecTime( kConvertTime, i64RetTime, op );
		__int64 iAddTime = (__int64)( kConvertTime.wDayOfWeek - (WORD)_type);
		if ( iAddTime < 0 )
		{
			iAddTime = (__int64)EVERY_WEEK_CHK - iAddTime;
		}
		iAddTime *= (OneDay / op);
		i64RetTime += iAddTime;
		return true;
	}
	else if ( (_type > EVERY_DAY) && (_type < EVERY_DAY_CHK) )
	{
		kConvertTime.wDay = (WORD)_type;
		if ( kConvertTime.wMonth < 12 )
		{
			switch ( ++kConvertTime.wMonth )
			{
			case 1:case 3:case 5:case 7:case 8:case 10:case 12:
				{
				}break;
			case 2:
				{
					if ( kConvertTime.wMonth > 29 )
					{
						return false;
					}
					if ( kConvertTime.wMonth > 28 )
					{
						// 윤달을 계산해야 하는데.... 짱난다.
						return false;
					}
				}break;
			default:
				{
					if ( kConvertTime.wMonth > 30 )
					{
						return false;
					}
				}break;
			}	
		}
		else
		{
			// 1월은 31일 까지 있으니까 체크할 필요도 없네~~
			++kConvertTime.wYear;
			kConvertTime.wMonth = 1;
		}

		// 이제 제대로된 시간을 찾아보자.
		SystemTime2SecTime( kConvertTime, i64RetTime, op );
		return true;
	}
	else if ( EVERY_DAY == _type )
	{// 매일
		SystemTime2SecTime( kConvertTime, i64RetTime, op );
		i64RetTime += ( OneDay / op );
		return true;
	}
	return false;
}

bool CGameTime::GetNextTime( SYSTEMTIME& kConvertTime, ETIMECYCLE_TYPE const _type, __int64 const op )
{
	__int64 i64Time = 0;
	if ( GetNextTime( i64Time, kConvertTime, _type, op ) )
	{
		SecTime2SystemTime( i64Time, kConvertTime, op );
		return true;
	}
	return false;
}

bool CGameTime::GetNextTime( __int64& i64ConvertTime, ETIMECYCLE_TYPE const _type, __int64 const op )
{
	SYSTEMTIME kSysTime;
	SecTime2SystemTime( i64ConvertTime, kSysTime, op );
	return GetNextTime( i64ConvertTime, kSysTime, _type, op );
}

__int64 CGameTime::GetElapsedTime( BM::DBTIMESTAMP_EX const& kStartTime, BM::DBTIMESTAMP_EX const& kEndTime, __int64 const op )
{
	__int64 i64StartTime = 0;
	__int64 i64EndTime = 0;
	DBTimeEx2SecTime( kStartTime, i64StartTime, op );
	DBTimeEx2SecTime( kEndTime, i64EndTime, op );
	return i64StartTime - i64EndTime;
}
