#pragma once

#include "BM/Guid.h"
#include "BM/XFStream.h"

namespace BM
{
	class PgXLogStream
	{
	public:
		PgXLogStream(void):m_bEnable(true){}
		virtual ~PgXLogStream(void){}

		virtual void Release(void) = 0;

		virtual bool Push( std::wstring const &wstr, size_t const iLv = 0 ) = 0;

		void SetEnable( bool const bEnable ){m_bEnable=bEnable;}
		bool IsEnable(void)const{return m_bEnable;}

	protected:
		bool m_bEnable;
	};

	
	class PgXLogStream_Trace
		:	public PgXLogStream
	{
	public:
		PgXLogStream_Trace(void);
		virtual ~PgXLogStream_Trace(void);

		virtual void Release(void){}

		virtual bool Push( std::wstring const & wstr, size_t const iLv = 0 );
	};

	#define console_rgb(d, r, g, b)			( (d << 3) + (r << 2) + (g << 1) + (b << 0) )
	#define out_color( fc, bc )	(WORD)( (fc << 0) + (bc << 4) )

	typedef enum eLogColor
	{
		LC_INTENSITY=	(console_rgb( 1,0,0,0)),
		LC_WHITE	=	(console_rgb( 0,1,1,1)),
		LC_BLACK	=	(console_rgb( 0,0,0,0)),
		LC_RED		=	(console_rgb( 0,1,0,0)),
		LC_GREEN	=	(console_rgb( 0,0,1,0)),
		LC_BLUE		=	(console_rgb( 0,0,0,1)),
		LC_YELLOW	=	(LC_RED	| LC_GREEN),
		LC_VIOLET	=	(LC_RED	| LC_BLUE),
		LC_JADE		=	(LC_BLUE	| LC_GREEN),
		LC_WHITE_I	=	(LC_WHITE	| LC_INTENSITY),
		LC_BLACK_I	=	(LC_BLACK	| LC_INTENSITY),
		LC_RED_I	=	(LC_RED		| LC_INTENSITY),
		LC_GREEN_I	=	(LC_GREEN	| LC_INTENSITY),
		LC_BLUE_I	=	(LC_BLUE	| LC_INTENSITY),
		LC_YELLOW_I	=	(LC_YELLOW	| LC_INTENSITY),
		LC_VIOLET_I	=	(LC_VIOLET	| LC_INTENSITY),
		LC_JADE_I	=	(LC_JADE	| LC_INTENSITY),
	}E_LOG_COLOR;

	typedef enum eLogBgColor
	{
		//		LBC_WHITE	=	LC_WHITE	,//배경 흰색 안됨.
		LBC_BLACK	=	LC_BLACK	,
		LBC_RED		=	LC_RED		,	
		LBC_GREEN	=	LC_GREEN	,	
		LBC_BLUE	=	LC_BLUE		,
		LBC_YELLOW	=	LC_YELLOW	,
		LBC_VIOLET	=	LC_VIOLET	,
		LBC_JADE	=	LC_JADE		,
	}E_LOG_BG_COLOR;

	typedef enum eLogFontColor
	{
		LFC_WHITE		=	LC_WHITE_I	,
		LFC_WHITE_I		=	LC_WHITE_I	,
		//		LFC_BLACK_I		=	LC_BLACK_I	,//글자는 검정 안됨.
		LFC_RED_I		=	LC_RED_I	,
		LFC_GREEN_I		=	LC_GREEN_I	,
		LFC_BLUE_I		=	LC_BLUE_I	,
		LFC_YELLOW_I	=	LC_YELLOW_I	,
		LFC_VIOLET_I	=	LC_VIOLET_I	,
		LFC_JADE_I		=	LC_JADE_I	,
	}E_LOG_FONT_COLOR;

	class PgXLogStream_Console
		:	public PgXLogStream
	{
	public:
		typedef std::vector<WORD>	CONT_LEVEL_BG;

	public:
		PgXLogStream_Console(void);
		virtual ~PgXLogStream_Console(void);

		HRESULT Init( E_LOG_FONT_COLOR eFontColor, CONT_LEVEL_BG const &kContLevelBG );

		virtual void Release(void);

		virtual bool Push( std::wstring const & wstr, size_t const iLv = 0 );

	protected:
		HRESULT OpenConsole(void);
		HRESULT CloseConsole(void);

	protected:
		static Loki::Mutex	ms_console_mutex;	//콘솔은 자원이 하나이므로 static 으로 처리
		static HANDLE		ms_hConsole;		//콘솔 핸들.
		bool				m_bWillDestroyConsole;//죽을때 콘솔이 닫힐 것인가.

		E_LOG_FONT_COLOR	m_eFontColor;
		CONT_LEVEL_BG		m_kContLevelBG;
	};

	class PgXLogStream_File
		:	public PgXLogStream
	{
	public:
		PgXLogStream_File(void);
		virtual ~PgXLogStream_File(void);

		HRESULT Init( std::wstring const &wstrFileName, std::wstring const &wstrFolderName, std::string const &strLocale, unsigned long const ulMaxLogLingCount = 100000 );
		virtual void Release(void);

		virtual bool Push( std::wstring const & wstr, size_t const iLv = 0 );

	protected:
		bool OpenLogFile(void);
		bool CloseLogFile(void);

	protected:
		Loki::Mutex		m_kMutex;
		std::wfstream	m_kLogFile;
		std::wstring	m_wstrFileName;
		std::wstring	m_wstrFolderName;
		std::string		m_strLocale;

		unsigned long	m_ulLogLineCount;
		unsigned long	m_ulMaxLogLineCount;
		tm				m_kLastFileTime;
	};

	class PgXLogStream_FileMM
		:	public PgXLogStream
	{
	public:
		PgXLogStream_FileMM(void);
		virtual ~PgXLogStream_FileMM(void);

		HRESULT Init( std::wstring const &wstrFileName, std::wstring const &wstrFolderName, unsigned long const ulMaxLogLingCount = 100000 );
		virtual void Release(void);

		virtual bool Push( std::wstring const & wstr, size_t const iLv = 0 );

	protected:
		bool OpenLogFile(void);

	protected:
		Loki::Mutex		m_kMutex;
		std::wstring	m_wstrFileName;
		std::wstring	m_wstrFolderName;

		tm				m_kLastFileTime;
		unsigned long	m_ulLogLineCount;
		unsigned long	m_ulMaxLogLineCount;

		PgXFStream_FMM	m_kFMMStream;
	};

};// <- namespace BM

