#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <tchar.h>

#include "Loki/Threads.h"
#include "BM/TWrapper.h"

namespace BM
{	
	class PgXLogStream;
	class PgDebugLog
	{
	public:
		typedef WORD									STREAM_TYPE;
		typedef std::map< STREAM_TYPE, PgXLogStream* >	CONT_XLOGSTREAM;

	public:
		PgDebugLog(void);
		explicit PgDebugLog( int const iLogLevelLimit );
		~PgDebugLog(void);

		void Release(void);

		HRESULT Regist( STREAM_TYPE const kStreamType, PgXLogStream * pkStream );
		HRESULT UnRegist( STREAM_TYPE const kStreamType );

	public:
		HRESULT GetLog( std::wstring &rkOutLog, int const iLogLv, wchar_t const* format, ... )const;
		HRESULT GetLogNoArg( std::wstring &rkOutLog, int const iLogLv, std::wstring const& kLogMsg )const;

		void SetUseHeader( bool const bUseHeader ){m_bUseHeader = bUseHeader;}
		void SetLogLimitLv( int const iLogLv ){m_iLogLevelLimit = iLogLv;}
		HRESULT SetEnable( STREAM_TYPE const kStreamType, bool const bEnable );

		void Log( std::wstring const &wstrLog, int const iLogLv );

	protected:
		CONT_XLOGSTREAM	m_kContXLogStream;

		int 			m_iLogLevelLimit;
		bool			m_bUseHeader;

	private:
		PgDebugLog& operator=( PgDebugLog const &rhs );//대입연산자 사용 불가.
		PgDebugLog( PgDebugLog const &rhs );//복사생성자 사용 불가.
	};

	class PgDebugLog_Wrapper
		: public TWrapper< PgDebugLog >
	{
	public:
		PgDebugLog_Wrapper(void);
		virtual ~PgDebugLog_Wrapper(void);

		HRESULT Regist( PgDebugLog::STREAM_TYPE const kStreamType, PgXLogStream * pkStream );
		HRESULT UnRegist( PgDebugLog::STREAM_TYPE const kStreamType );

		void SetUseHeader( bool const bUseHeader );
		void SetLogLimitLv( int const iLogLv );
		HRESULT SetEnable( PgDebugLog::STREAM_TYPE const kStreamType, bool const bEnable );

		void Log( int const iLogLv, wchar_t const* szLogMsg, ... );
		void LogNoArg( int const iLogLv, std::wstring const& kLogMsg );
	};

	/*
	class CDebugLog
	{
		Loki::Mutex m_log_mutex;
		static Loki::Mutex ms_console_mutex;//콘솔은 자원이 하나이므로 static 으로 처리
	public:
		CDebugLog();
		virtual ~CDebugLog();

	public:
		bool Init(DWORD const OutputMode, E_LOG_FONT_COLOR const eFontColor, std::wstring const& wstrFolderName = _T("./Log"), std::wstring const& wstrFileName = _T("Log"), std::string const& InLocale = "eng");
		void Log(E_LOG_LEVEL const LogLv, wchar_t const* szLogMsg, ... );// 레벨 포함 기록
		void LogNoArg(E_LOG_LEVEL const LogLv, std::wstring const& kLogMsg);
		
		bool SetOutputMode( DWORD const OutputMode );
		void SetLogLevelLimit(E_LOG_LEVEL const eLevel) { m_eLogLevelLimit = eLevel; }
		void SetLocale(std::string const& strLocale);
		void SetIgnoreConsoleOutput(bool bIgnore);

	private:
		void Clear();
		void SetWriteFile();//기록될 파일 설정// 반드시 호출 전에 SetTime을 호출 하세요 SetTime 뒤에 오게 하세요
		
		void OutFile(wchar_t const* wszLogMsg, size_t const msg_len);
		void OutTrace(wchar_t const* wszLogMsg);	
		void OutConsole(wchar_t const* wszLogMsg, size_t const msg_len, E_LOG_LEVEL const LogLv);
		
		bool OpenConsole();
		void CloseConsole();
		
	private:
		typedef struct t_LogDate
		{
			t_LogDate(__int32 const iYear = 0, __int32 const iMonth = 0, __int32 const iDay = 0)
			{
				m_iYear = iYear;
				m_iMonth = iMonth;
				m_iDay = iDay;
			}
			__int32 m_iYear;
			__int32 m_iMonth;
			__int32 m_iDay;
		}LOG_DATE, *LP_LOG_DATE;

	private:
		E_LOG_FONT_COLOR	m_eFontColor;	
		DWORD	m_dwModeFlag;
		
		std::wstring m_wstrFileName;
		std::wstring m_wstrFolderName;
		std::string	m_strLocale;

		unsigned long m_ulLogLineCount;

		std::wfstream m_logfile;
		static HANDLE	m_hConsole;//콘솔 핸들.
		static bool	m_bWillDestroyConsole;//죽을때 콘솔이 닫힐 것인가.
		bool		m_bIgnoreConsoleOutput;//죽을때 콘솔이 닫힐 것인가.
		E_LOG_LEVEL m_eLogLevelLimit;
		WORD		m_wCurrentDay;	// 오늘 날짜만..


	private:
		CDebugLog& operator=(const CDebugLog &rhs);//대입연산자 사용 불가.
		CDebugLog(const CDebugLog &rhs);//복사생성자 사용 불가.
	};
	*/

};// <- namespace BM
