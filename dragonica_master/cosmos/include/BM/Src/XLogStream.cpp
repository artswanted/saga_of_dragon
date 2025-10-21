#include "stdafx.h"
#include "ThreadObject.h"
#include "FileSupport.h"
#include "XLogStream.h"

namespace BM
{

// -------------------------------------------------
// PgXLogStream_Trace
// -------------------------------------------------
PgXLogStream_Trace::PgXLogStream_Trace(void)
{

}

PgXLogStream_Trace::~PgXLogStream_Trace(void)
{

}

bool PgXLogStream_Trace::Push( std::wstring const & wstr, size_t const iLv )
{
	if ( true == IsEnable() )
	{
		::OutputDebugStringW( wstr.c_str() );
		::OutputDebugStringW( L"\n" );
	}
	return true;
}

// -------------------------------------------------
// PgXLogStream_Console
// -------------------------------------------------
Loki::Mutex PgXLogStream_Console::ms_console_mutex;
HANDLE PgXLogStream_Console::ms_hConsole = INVALID_HANDLE_VALUE;

PgXLogStream_Console::PgXLogStream_Console(void)
:	m_bWillDestroyConsole(false)
{
	OpenConsole();
}

PgXLogStream_Console::~PgXLogStream_Console(void)
{
	CloseConsole();
}

HRESULT PgXLogStream_Console::OpenConsole(void)
{
	CAutoMutex Lock(ms_console_mutex);

	HANDLE const hStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if(		INVALID_HANDLE_VALUE == hStdHandle
		||	NULL == hStdHandle )//핸들 없음.
	{
		if( !::AllocConsole() )
		{
			return E_FAIL;
		}
		
		//Alloc
		m_bWillDestroyConsole = true;
		ms_hConsole = hStdHandle;
	}
	else //있음 그냥 만들면 됨.
	{
		ms_hConsole = hStdHandle;
		m_bWillDestroyConsole = false;
	}

	return S_OK;
}

HRESULT PgXLogStream_Console::CloseConsole(void)
{
	CAutoMutex Lock(ms_console_mutex);

	if ( true == m_bWillDestroyConsole )
	{
		::FreeConsole();
		::CloseHandle(ms_hConsole);
		ms_hConsole	= INVALID_HANDLE_VALUE;
		m_bWillDestroyConsole = false;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgXLogStream_Console::Init( E_LOG_FONT_COLOR eFontColor, CONT_LEVEL_BG const &kContLevelBG )
{
	m_eFontColor = eFontColor;
	m_kContLevelBG = kContLevelBG;
	return OpenConsole();
}

void PgXLogStream_Console::Release(void)
{
	CloseConsole();
}

bool PgXLogStream_Console::Push( std::wstring const & wstr, size_t const iLv )
{
	if ( !IsEnable() )
	{
		return true;
	}

	CAutoMutex Lock(ms_console_mutex);
	if( INVALID_HANDLE_VALUE != ms_hConsole )
	{
		if ( m_kContLevelBG.size() > iLv )
		{
			::SetConsoleTextAttribute(ms_hConsole, out_color( m_eFontColor, m_kContLevelBG.at(iLv) ) );	

			DWORD dwBytes = 0;
			::WriteConsoleW( ms_hConsole, wstr.c_str() , static_cast<DWORD>(wstr.length()), &dwBytes, NULL );
			::WriteConsoleW( ms_hConsole, L"\n\r" , 1, &dwBytes, NULL);

			::SetConsoleTextAttribute( ms_hConsole, out_color( m_eFontColor, LC_BLACK ) );
			return true;
		}
	}
	return false;
};


// -------------------------------------------------
// PgXLogStream_File
// -------------------------------------------------
PgXLogStream_File::PgXLogStream_File(void)
:	m_ulLogLineCount(0)
,	m_ulMaxLogLineCount(100000)
{
	::memset( &m_kLastFileTime, 0, sizeof(m_kLastFileTime) );
}

PgXLogStream_File::~PgXLogStream_File(void)
{
	PgXLogStream_File::Release();// virtual function named call
}

HRESULT PgXLogStream_File::Init( std::wstring const &wstrFileName, std::wstring const &wstrFolderName, std::string const &strLocale, unsigned long const ulMaxLogLingCount )
{
	BM::CAutoMutex kLock( m_kMutex );
	m_wstrFileName = wstrFileName;
	m_wstrFolderName = wstrFolderName;
	m_strLocale = strLocale;
	m_ulMaxLogLineCount = ulMaxLogLingCount;
	return S_OK;
}

void PgXLogStream_File::Release(void)
{
	BM::CAutoMutex kLock( m_kMutex );
	CloseLogFile();
}

bool PgXLogStream_File::Push( std::wstring const & wstr, size_t const iLv )
{
	if ( !IsEnable() )
	{
		return true;
	}

	BM::CAutoMutex kLock( m_kMutex );

	if(	!m_kLogFile.is_open() )
	{
		OpenLogFile();
	}
	else
	{
		time_t now_time = {0,};
		::time( &now_time );
		tm new_local_time;
		::localtime_s( &new_local_time, &now_time );
		if ( new_local_time.tm_mday != m_kLastFileTime.tm_mday )
		{
			OpenLogFile();
		}
	}

	if( true == m_kLogFile.is_open() )
	{
		m_kLogFile.write( wstr.c_str(), static_cast<std::streamsize>(wstr.length()) );
		m_kLogFile.write( L"\n\r", 1 );
		m_kLogFile.flush();

		if ( m_ulMaxLogLineCount < ++m_ulLogLineCount )
		{
			OpenLogFile();
		}
		return true;
	}

	return false;
}

bool PgXLogStream_File::OpenLogFile(void)
{
	time_t now_time = {0,};
	::time( &now_time );   // Get time in seconds
	tm new_local_time;
	errno_t const err = ::localtime_s( &new_local_time, &now_time );
	if ( 0 == err )
	{
		std::wstring kTempFolder = m_wstrFolderName;
		AddFolderMark(kTempFolder);//끝에 폴더마크를 붙이고.
		ReserveFolder(kTempFolder);//폴더 예약.

		wchar_t wszFileName[MAX_PATH] = {0,};

		::swprintf_s(	wszFileName, MAX_PATH, L"%s/%s_%02d%02d%02d_%02d%02d%02d.txt"
					,	m_wstrFolderName.c_str()
					,	m_wstrFileName.c_str()
					,	(1900 + new_local_time.tm_year) % 2000
					,	1 + new_local_time.tm_mon
					,	new_local_time.tm_mday
					,	new_local_time.tm_hour
					,	new_local_time.tm_min
					,	new_local_time.tm_sec
		);

		if( m_kLogFile.is_open() )
		{
			m_kLogFile.close();//닫고
		}

		m_kLogFile.open( wszFileName, std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary );//다시 연다.
		if( m_kLogFile.is_open() )
		{
			m_kLogFile.imbue( std::locale(m_strLocale.c_str()) );
			m_ulLogLineCount = 0;
			m_kLastFileTime = new_local_time;
			return true;
		}

		assert(NULL && "SetWriteFile() Error" );//열리지 않았으면 assert
	}
	return false;
}

bool PgXLogStream_File::CloseLogFile(void)
{
	if( true == m_kLogFile.is_open() )
	{
		m_kLogFile.close();
		return true;
	}
	return false;
}


// -------------------------------------------------
// PgXLogStream_FileMM
// -------------------------------------------------
PgXLogStream_FileMM::PgXLogStream_FileMM(void)
:	m_kFMMStream(32768)
,	m_ulLogLineCount(0)
,	m_ulMaxLogLineCount(100000)
{
	::memset( &m_kLastFileTime, 0, sizeof(m_kLastFileTime) );
}

PgXLogStream_FileMM::~PgXLogStream_FileMM(void)
{
}

HRESULT PgXLogStream_FileMM::Init( std::wstring const &wstrFileName, std::wstring const &wstrFolderName, unsigned long const ulMaxLogLingCount )
{
	BM::CAutoMutex kLock( m_kMutex );
	m_wstrFileName = wstrFileName;
	m_wstrFolderName = wstrFolderName;
	m_ulLogLineCount = 0;
	m_ulMaxLogLineCount = ulMaxLogLingCount;
	return S_OK;
}

void PgXLogStream_FileMM::Release(void)
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( true ==m_kFMMStream.IsOpen() )
	{
		m_kFMMStream.Close();
		m_ulLogLineCount = 0;
	}
}

bool PgXLogStream_FileMM::Push( std::wstring const & wstr, size_t const iLv )
{
	if ( !IsEnable() )
	{
		return true;
	}

	BM::CAutoMutex kLock( m_kMutex );

	if ( !m_kFMMStream.IsOpen() )
	{
		OpenLogFile();
	}
	else
	{
		time_t now_time = {0,};
		::time( &now_time );
		tm new_local_time;
		::localtime_s( &new_local_time, &now_time );
		if ( new_local_time.tm_mday != m_kLastFileTime.tm_mday )
		{
			OpenLogFile();
		}
	}

	std::string str = vstring::ConvToMultiByte(wstr);
	str += "\n";

	bool bRet = m_kFMMStream.Push( str );
	if ( !bRet )
	{
		OpenLogFile();
		bRet = m_kFMMStream.Push( str );
	}

	if ( true == bRet )
	{
		m_kFMMStream.SetWritePos( m_kFMMStream.GetWritePos() - 1 );
	}

	if ( m_ulMaxLogLineCount < ++m_ulLogLineCount )
	{
		OpenLogFile();
	}

	return bRet;
}

bool PgXLogStream_FileMM::OpenLogFile(void)
{
	time_t now_time = {0,};
	::time( &now_time );   // Get time in seconds
	tm new_local_time;
	errno_t const err = ::localtime_s( &new_local_time, &now_time );
	if ( 0 == err )
	{
		std::wstring kTempFolder = m_wstrFolderName;
		AddFolderMark(kTempFolder);//끝에 폴더마크를 붙이고.
		ReserveFolder(kTempFolder);//폴더 예약.

		wchar_t wszFileName[MAX_PATH] = {0,};

		::swprintf_s(	wszFileName, MAX_PATH, L"%s/%s_%02d%02d%02d_%02d%02d%02d.txt"
					,	m_wstrFolderName.c_str()
					,	m_wstrFileName.c_str()
					,	(1900 + new_local_time.tm_year) % 2000
					,	1 + new_local_time.tm_mon
					,	new_local_time.tm_mday
					,	new_local_time.tm_hour
					,	new_local_time.tm_min
					,	new_local_time.tm_sec
		);

		Release();
		if ( NO_ERROR == m_kFMMStream.Open( std::wstring(wszFileName), 32768 ) )
		{
			m_kLastFileTime = new_local_time;
			m_ulLogLineCount = 0;
			return true;
		}
	}
	return false;
}

};//<- namespace BM
