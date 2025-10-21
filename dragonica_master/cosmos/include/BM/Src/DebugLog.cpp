// Debug.cpp: implementation of the CDebugLog class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <assert.h>
#include <direct.h>
#include <Time.h>
#include <wchar.h>

#include "Threadobject.h"
#include "guid.h"
#include "XLogStream.h"
#include "Header/DebugLog.h"
#include "LogWorker_Base.h"

namespace BM
{
	/*
	Loki::Mutex CDebugLog::ms_console_mutex;
	HANDLE		CDebugLog::m_hConsole = INVALID_HANDLE_VALUE;//콘솔 핸들.
	bool		CDebugLog::m_bWillDestroyConsole = false;//죽을때 콘솔이 닫힐 것인가.

	#define out_color( fc, bc )	(WORD)( (fc << 0) + (bc << 4) )

	static WORD const s_log_bg_color[] = 
	{
		LC_RED,		//LV0
		LC_YELLOW,	//LV1
		LC_VIOLET,	//LV2
		LC_JADE,	//LV3
		LC_BLUE,	//LV4
		LC_GREEN,	//LV5
		LC_BLACK_I,	//LV6
		LC_BLACK,	//LV7
		LC_BLACK,	//LV8
		LC_BLACK,	//LV9
	};


	CDebugLog::CDebugLog()
	{
	//	m_hConsole	= INVALID_HANDLE_VALUE;
		Clear();
	}

	CDebugLog::~CDebugLog()
	{
		Clear();
		CloseConsole();
	}

	bool CDebugLog::Init(DWORD const OutputMode, E_LOG_FONT_COLOR const eFontColor, std::wstring const& wstrFolderName, std::wstring const& wstrFileName, std::string const& InLocale)
	{
		BM::CAutoMutex Lock(m_log_mutex);

		bool const bRet = SetOutputMode( OutputMode );
		if ( true == bRet )
		{
			m_wstrFileName = wstrFileName;
			m_wstrFolderName = wstrFolderName;
			SetLocale( InLocale );
			m_eFontColor = eFontColor;
		}

		return bRet;
	}

	void CDebugLog::Clear()
	{
		BM::CAutoMutex Lock(m_log_mutex);
		m_dwModeFlag = OUTPUT_NOT_SET;
		
		m_wstrFileName = L"";
		if( m_logfile.is_open() )
		{
			m_logfile.close();
		}
		m_eLogLevelLimit = LOG_LV_MAX;
		m_ulLogLineCount = 0;
		m_wCurrentDay = 0;
		m_bIgnoreConsoleOutput = false;
	}

	void CDebugLog::Log(E_LOG_LEVEL const LogLv, wchar_t const* format, ...)
	{
		if( OUTPUT_NOT_SET == m_dwModeFlag || m_eLogLevelLimit < LogLv)
		{
			return;
		}

		BM::CAutoMutex Lock(m_log_mutex);

		va_list args;
		va_start( args, format );
		int len = _vscwprintf( format, args ) // _vscprintf doesn't count
							   + 1; // terminating '\0'
		wchar_t *buffer = NULL;
		bool bUseArgs = true;
		int const iTimeLen = 20;
		if (len > 1)
		{
			buffer = new wchar_t[len + iTimeLen];
		}
		else
		{
			len = static_cast<int>(::wcslen(format)+1);
			buffer = new wchar_t[len + iTimeLen];
			bUseArgs = false;
		}
		if(!buffer)
		{//메모리 음따.
			return;
		}
		wchar_t* pWritePos = buffer;
		// Time 기록
		if( (OUTPUT_IGNORE_LOG_HEAD & m_dwModeFlag) == 0)
		{
			::swprintf_s(pWritePos, iTimeLen, L"LV-%02d,", LogLv);	pWritePos += 6;
			::_wstrtime_s(pWritePos, iTimeLen); pWritePos = buffer + wcslen(buffer);
			wcscpy_s(pWritePos, 2, L","); pWritePos += 1;
		}
		if (bUseArgs)
		{
			vswprintf_s( pWritePos, len, format, args );
		}
		else
		{
			wcscpy_s(pWritePos, len, format);
		}
		va_end(args);

		size_t const ret_len = wcslen(buffer);

		if(OUTPUT_JUST_FILE & m_dwModeFlag )
		{
			OutFile(buffer, ret_len);
		}
		if(OUTPUT_JUST_TRACE & m_dwModeFlag )
		{
			OutTrace(buffer);
		}
		if(OUTPUT_JUST_CONSOLE & m_dwModeFlag )
		{
			OutConsole(buffer, ret_len, LogLv);
		}
		++m_ulLogLineCount;

		delete []buffer;
	}

	void CDebugLog::LogNoArg(E_LOG_LEVEL const LogLv, std::wstring const& kLogMsg)
	{
		if( OUTPUT_NOT_SET == m_dwModeFlag || m_eLogLevelLimit < LogLv)
		{
			return;
		}

		BM::CAutoMutex Lock(m_log_mutex);

		std::wstring strOutMsg;
		int const iTimeLen = 20;
	
		if( (OUTPUT_IGNORE_LOG_HEAD & m_dwModeFlag) == 0)
		{
			TCHAR szTime[MAX_PATH] = {0,};

			::swprintf_s(szTime, iTimeLen, L"LV-%02d,", LogLv);
			::_wstrtime_s(szTime+6, iTimeLen);
			size_t const time_offset = wcslen(szTime);
			wcscpy_s(szTime + time_offset, 2, _T(","));
			
			strOutMsg = szTime;
		}

		strOutMsg += kLogMsg;

		if(OUTPUT_JUST_FILE & m_dwModeFlag )
		{
			OutFile(strOutMsg.c_str(), strOutMsg.length());
		}
		if(OUTPUT_JUST_TRACE & m_dwModeFlag )
		{
			OutTrace(strOutMsg.c_str());
		}
		if(OUTPUT_JUST_CONSOLE & m_dwModeFlag )
		{
			OutConsole(strOutMsg.c_str(), strOutMsg.length(), LogLv);
		}
		++m_ulLogLineCount;
	}

	void CDebugLog::OutFile(wchar_t const* wszLogMsg, size_t const msg_len)
	{
		if(!m_logfile.is_open())
		{
			SetWriteFile();
		}

		if(m_logfile.is_open())
		{
			if(m_ulLogLineCount > 100000)
			{
				SetWriteFile();
			}
			else
			{
				SYSTEMTIME kSystemTime;
				::GetLocalTime(&kSystemTime);
				if (kSystemTime.wDay != m_wCurrentDay)
				{
					SetWriteFile();
				}
			}
			m_logfile.write( wszLogMsg, static_cast<std::streamsize>(msg_len) );
			m_logfile.write( L"\n\r", 1 );

			m_logfile.flush();
		}
	}

	void CDebugLog::OutTrace(wchar_t const* wszLogMsg)
	{
		::OutputDebugStringW( wszLogMsg );
		::OutputDebugStringW( L"\n\r" );
	}

	void CDebugLog::OutConsole(wchar_t const* wszLogMsg, size_t const msg_len, E_LOG_LEVEL const LogLv)
	{
		CAutoMutex Lock(ms_console_mutex);
		if( INVALID_HANDLE_VALUE != m_hConsole && m_bIgnoreConsoleOutput == false)
		{
			::SetConsoleTextAttribute(m_hConsole, out_color( m_eFontColor, s_log_bg_color[LogLv] ) );	
			
			DWORD dwBytes = 0;
			::WriteConsoleW(m_hConsole, wszLogMsg , static_cast<DWORD>(msg_len), &dwBytes, NULL);
			::WriteConsoleW(m_hConsole, L"\n\r" , 1, &dwBytes, NULL);

			::SetConsoleTextAttribute(m_hConsole, out_color( m_eFontColor, LC_BLACK ) );
		}
	}

	bool CDebugLog::OpenConsole()
	{
		if( OUTPUT_JUST_CONSOLE & m_dwModeFlag )
		{
			CAutoMutex Lock(ms_console_mutex);

			HANDLE const hStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
			if(INVALID_HANDLE_VALUE == hStdHandle
			|| NULL == hStdHandle )//핸들 없음.
			{
				if(::AllocConsole())
				{//Alloc
					m_bWillDestroyConsole = true;
					m_hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
					return true;
				}
				else
				{
					return false;
				}
			}
			else //있음 그냥 만들면 됨.
			{
				m_hConsole = hStdHandle;
				return true;
			}
		}
		return false;
	}

	void CDebugLog::CloseConsole()
	{
		if( m_bWillDestroyConsole )
		{
			if(( OUTPUT_JUST_CONSOLE & m_dwModeFlag )
			&& ( INVALID_HANDLE_VALUE != m_hConsole) )
			{
				CAutoMutex Lock(ms_console_mutex);
				::FreeConsole();
				::CloseHandle(m_hConsole);
				m_hConsole	= INVALID_HANDLE_VALUE;
				m_bWillDestroyConsole = false;
			}
		}
	}

	void CDebugLog::SetWriteFile()//파일 관리부
	{
		time_t now_time = {0,};
		::time( &now_time );   // Get time in seconds
		struct tm new_local_time;
		errno_t const err = ::localtime_s( &new_local_time, &now_time );
		
		if (err == 0)
		{
			time_t now_time = {0,};
			::time( &now_time );   // Get time in seconds
			struct tm new_local_time;
			//errno_t const err = 
			::localtime_s( &new_local_time, &now_time );

			std::wstring kTempFolder = m_wstrFolderName;
			AddFolderMark(kTempFolder);//끝에 폴더마크를 붙이고.
			ReserveFolder(kTempFolder);//폴더 예약.
					
			wchar_t wszFileName[MAX_PATH] = {0,};

			::wsprintf(wszFileName, L"%s/%s_%02d%02d%02d_%02d%02d%02d.txt", 
				m_wstrFolderName.c_str(),
				m_wstrFileName.c_str(),
				(1900 + new_local_time.tm_year)%2000 ,
				1+ new_local_time.tm_mon,
				new_local_time.tm_mday,

				new_local_time.tm_hour,
				new_local_time.tm_min,
				new_local_time.tm_sec
				);

			if(m_logfile.is_open())
			{
				m_logfile.close();//닫고
			}
			
			m_logfile.open( wszFileName, std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary );//다시 연다.
			if( !m_logfile.is_open() )
			{
				assert(NULL && "SetWriteFile() Error" );//열리지 않았으면 assert
			}
			else
			{
				SetLocale(m_strLocale);
				//m_LogBeignDate = LOG_DATE(1900 + new_local_time.tm_year, 1+ new_local_time.tm_mon, new_local_time.tm_mday, new_local_time.tm_hour);
				m_ulLogLineCount = 0;
				m_wCurrentDay = new_local_time.tm_mday;
			}
		}
	}

	void CDebugLog::SetLocale(std::string const& strLocale)
	{
		m_strLocale = strLocale;
		if (m_logfile.is_open())
		{
			m_logfile.imbue(std::locale(m_strLocale.c_str()));
			//m_logfile.imbue(std::locale("Korean_Korea.949"));
		}
	}

	bool CDebugLog::SetOutputMode( DWORD const OutputMode )
	{
		if ( OutputMode != m_dwModeFlag )
		{
			BM::CAutoMutex Lock(m_log_mutex);

			switch( OutputMode )
			{
			case OUTPUT_NOT_SET:
				{
					assert(NULL && "CDebugLog Init Error(OUTPUT_NOT_SET)");
					return false;
				}break;
			default:
				{	
					m_dwModeFlag = OutputMode;
				}break;
			}//switch(OutputMode)

			if( OUTPUT_JUST_CONSOLE  & m_dwModeFlag )
			{
				OpenConsole();
			}
		}
		return true;
	}

	void CDebugLog::SetIgnoreConsoleOutput(bool bIgnore)
	{
		m_bIgnoreConsoleOutput = bIgnore;	
	}
	*/

	PgDebugLog::PgDebugLog(void)
		:	m_iLogLevelLimit(0)
		,	m_bUseHeader(true)
	{
	}

	PgDebugLog::PgDebugLog( int const iLogLevelLimit )
		:	m_iLogLevelLimit(iLogLevelLimit)
		,	m_bUseHeader(true)
	{
	}

	PgDebugLog::~PgDebugLog(void)
	{
		Release();
	}

	void PgDebugLog::Release(void)
	{
		CONT_XLOGSTREAM::iterator itr = m_kContXLogStream.begin();
		for ( ; itr != m_kContXLogStream.end() ; ++itr )
		{
			itr->second->Release();
			SAFE_DELETE( itr->second );
		}
		m_kContXLogStream.clear();
	}

	HRESULT PgDebugLog::Regist( STREAM_TYPE const kStreamType, PgXLogStream * pkStream )
	{
		auto kPair = m_kContXLogStream.insert( std::make_pair( kStreamType, pkStream ) );
		if ( true == kPair.second )
		{
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT PgDebugLog::UnRegist( STREAM_TYPE const kStreamType )
	{
		CONT_XLOGSTREAM::iterator itr = m_kContXLogStream.find( kStreamType );
		if ( itr != m_kContXLogStream.end() )
		{
			itr->second->Release();
			SAFE_DELETE( itr->second );
			m_kContXLogStream.erase( itr );
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT PgDebugLog::GetLog( std::wstring &rkOutLog, int const iLogLv, wchar_t const* format, ... )const
	{
		if( m_iLogLevelLimit < iLogLv )
		{
			return E_FAIL;
		}

		va_list args;
		va_start( args, format );
		int len = _vscwprintf( format, args ) // _vscprintf doesn't count
			+ 1; // terminating '\0'
		wchar_t *buffer = NULL;
		bool bUseArgs = true;
		if (len > 1)
		{
			buffer = new wchar_t[len];
		}
		else
		{
			len = static_cast<int>(::wcslen(format)+1);
			buffer = new wchar_t[len];
			bUseArgs = false;
		}

		if( buffer)
		{
			if ( true == bUseArgs )
			{
				::vswprintf_s( buffer, len, format, args );
			}
			else
			{
				::wcscpy_s( buffer, len, format );
			}
			va_end(args);

			HRESULT const hRet = GetLogNoArg( rkOutLog, iLogLv, std::wstring(buffer) );
			delete[] buffer;
			return hRet;
		}

		return E_OUTOFMEMORY;
	}

	HRESULT PgDebugLog::GetLogNoArg( std::wstring &rkOutLog, int const iLogLv, std::wstring const& kLogMsg )const
	{
		if( m_iLogLevelLimit < iLogLv )
		{
			return E_FAIL;
		}

		if ( true == m_bUseHeader )
		{
			wchar_t wszHeader[27] = {0,};
			size_t iLen = 27;
			::swprintf_s( wszHeader, iLen, L"LV-%02d,", iLogLv );
			::_wstrtime_s( wszHeader + 6, iLen - 6 );

			size_t const offset = ::wcslen(wszHeader);
			if ( offset < ( iLen - 1 ) )
			{
				::wcscpy_s( wszHeader + offset, iLen - offset, L"," );
			}

			rkOutLog = wszHeader;
		}
		else
		{
			rkOutLog.clear();
		}

		rkOutLog += kLogMsg;
		return S_OK;
	}

	void PgDebugLog::Log( std::wstring const &wstrLog, int const iLogLv )
	{
		CONT_XLOGSTREAM::iterator stream_itr = m_kContXLogStream.begin();
		for( ; stream_itr != m_kContXLogStream.end() ; ++stream_itr )
		{
			stream_itr->second->Push( wstrLog, iLogLv );
		}
	}

	HRESULT PgDebugLog::SetEnable( STREAM_TYPE const kStreamType, bool const bEnable )
	{
		CONT_XLOGSTREAM::iterator stream_itr = m_kContXLogStream.find( kStreamType );
		if ( stream_itr != m_kContXLogStream.end() )
		{
			stream_itr->second->SetEnable( bEnable );
			return S_OK;
		}
		return E_FAIL;
	}



	//
	PgDebugLog_Wrapper::PgDebugLog_Wrapper(void)
	{}

	PgDebugLog_Wrapper::~PgDebugLog_Wrapper(void)
	{
		Instance()->Release();
	}

	HRESULT PgDebugLog_Wrapper::Regist( PgDebugLog::STREAM_TYPE const kStreamType, PgXLogStream * pkStream )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		return Instance()->Regist( kStreamType, pkStream );
	}

	HRESULT PgDebugLog_Wrapper::UnRegist( PgDebugLog::STREAM_TYPE const kStreamType )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		return Instance()->UnRegist( kStreamType );
	}

	void PgDebugLog_Wrapper::SetUseHeader( bool const bUseHeader )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		Instance()->SetUseHeader( bUseHeader );
	}

	void PgDebugLog_Wrapper::SetLogLimitLv( int const iLogLv )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		Instance()->SetLogLimitLv( iLogLv );
	}

	HRESULT PgDebugLog_Wrapper::SetEnable( PgDebugLog::STREAM_TYPE const kStreamType, bool const bEnable )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		return Instance()->SetEnable( kStreamType, bEnable );
	}

	void PgDebugLog_Wrapper::Log( int const iLogLv, wchar_t const* szLogMsg, ... )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
		PgDebugLog * pInstance = Instance();
		
		std::wstring wstrLog;

		va_list args;
		va_start( args, szLogMsg );
		HRESULT const hRet = pInstance->GetLog( wstrLog, iLogLv, szLogMsg, args );
		va_end( args );

		if ( SUCCEEDED(hRet) )
		{
			pInstance->Log( wstrLog, iLogLv );
		}
	}

	void PgDebugLog_Wrapper::LogNoArg( int const iLogLv, std::wstring const& kLogMsg )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
		PgDebugLog * pInstance = Instance();
		
		std::wstring wstrLog;
		HRESULT const hRet = pInstance->GetLogNoArg( wstrLog, iLogLv, kLogMsg );

		if ( SUCCEEDED(hRet) )
		{
			pInstance->Log( wstrLog, iLogLv );
		}
	}
};

tagRegLogInstance::tagRegLogInstance(DWORD const dwInLogWorkerType,
	DWORD const dwInOutputMode,
	BM::E_LOG_FONT_COLOR const eInFontColor,
	std::wstring const wstrInFolderName,
	std::wstring const wstrInFileName,
	std::string const strInLocale,
	BM::E_LOG_LEVEL const kInLogLimitLv
)
:	dwLogWorkerType(dwInLogWorkerType)
,	dwOutputMode(dwInOutputMode)
,	eFontColor(eInFontColor)
,	wstrFolderName(wstrInFolderName)
,	wstrFileName(wstrInFileName)
,	strLocale(strInLocale)
,	kLogLimitLv(kInLogLimitLv)
{
}

void tagRegLogInstance::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(dwLogWorkerType);
	kPacket.Push(dwOutputMode);
	kPacket.Push(eFontColor);
	kPacket.Push(wstrFolderName);
	kPacket.Push(wstrFileName);
	kPacket.Push(strLocale);
	kPacket.Push(kLogLimitLv);
}

void tagRegLogInstance::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(dwLogWorkerType);
	kPacket.Pop(dwOutputMode);
	kPacket.Pop(eFontColor);
	kPacket.Pop(wstrFolderName);
	kPacket.Pop(wstrFileName);
	kPacket.Pop(strLocale);
	kPacket.Pop(kLogLimitLv);
}
