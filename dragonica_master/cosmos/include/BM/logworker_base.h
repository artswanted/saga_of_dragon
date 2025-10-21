#pragma once
#include "Loki/Threads.h"
#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"
#include "BM/Stream.h"
#include "BM/PgTask.h"
#include "BM/XLogStream.h"

//로그용 공통 선언
#define __FL2__(Func, Line)		_T("[") << Func << _T("]-[")<< Line << _T("] ")
#define __FL__					__FL2__(__FUNCTIONW__, __LINE__)
#define _COMMA_					_T(", ")
#define __F_P__(PcaketType)	(__FUNCTIONW__ L"::" L#PcaketType)
#define __F_P2__(PacketType)	_T("[") << __FUNCTIONW__ << _T("::") << PacketType << _T("] ")

namespace BM
{
	typedef enum eOuputType
	{
		OUTPUT_NOT_SET					=	0x0000,
		OUTPUT_JUST_FILE				=	0x0001,
		OUTPUT_JUST_TRACE				=	0x0002,
		OUTPUT_JUST_CONSOLE				=	0x0004,
		OUTPUT_JUST_FILE_MEMMAPPING		=	0x0008,

		OUTPUT_IGNORE_LOG_HEAD			=	0x0100,

		OUTPUT_FILE_AND_TRACE			=	OUTPUT_JUST_FILE | OUTPUT_JUST_TRACE,
		OUTPUT_FILE_AND_CONSOLE			=	OUTPUT_JUST_FILE | OUTPUT_JUST_CONSOLE,
		OUTPUT_ALL						=	OUTPUT_JUST_FILE | OUTPUT_JUST_TRACE | OUTPUT_JUST_CONSOLE,	
	}E_OUPUT_TYPE;

	typedef enum eLogLevel
	{
		LOG_LV0		= 0,
			LOG_LV1		= 1,	// Critical Error
			LOG_LV2		= 2,	// General Error
			LOG_LV3		= 3,	// Logical Error
			LOG_LV4		= 4,	// Processing Failure (Packet data error...etc) 
			LOG_LV5		= 5,	// Warning
			LOG_LV6		= 6,	// Important Log
			LOG_LV7		= 7,	// General Log
			LOG_LV8		= 8,	// Debugging Message
			LOG_LV9		= 9,	// Temporary Debugging Message
		LOG_LV_MAX	= 9,//마지막것의 +1 을 쓰지 않는다.

		LOG_RED	= LOG_LV0,		//LV0
		LOG_YELLOW= LOG_LV1,	//LV1
		LOG_VIOLET= LOG_LV2,	//LV2
		LOG_JADE= LOG_LV3,	//LV3
		LOG_BLUE= LOG_LV4,	//LV4
		LOG_GREEN= LOG_LV5,	//LV5
	}E_LOG_LEVEL;
}

typedef struct tagInitLogDesc
{
	tagInitLogDesc()
	{
		dwOutputMode = 0;
	}

	DWORD dwOutputMode;
	
	void WriteToPacket(BM::Stream &kStream)const
	{
		kStream.Push(dwOutputMode);
	}

	void ReadFromPacket(BM::Stream &kStream)
	{
		kStream.Pop(dwOutputMode);
	}
}INIT_LOG_DESC;

typedef struct tagRegLogInstance
{
	tagRegLogInstance(DWORD const dwInLogWorkerType=0,
		DWORD const dwInOutputMode = 0,
		BM::E_LOG_FONT_COLOR const eInFontColor = BM::LFC_WHITE,
		std::wstring const wstrInFolderName = _T("./Log"),
		std::wstring const wstrInFileName = _T("Log"),
		std::string const strInLocale = "eng",
		BM::E_LOG_LEVEL const kInLogLimitLv = BM::LOG_LV9 );
	
	DWORD dwLogWorkerType;
	DWORD dwOutputMode;
	BM::E_LOG_FONT_COLOR eFontColor; 
	std::wstring wstrFolderName;
	std::wstring wstrFileName;
	std::string strLocale;
	BM::E_LOG_LEVEL	kLogLimitLv;
	
	void WriteToPacket(BM::Stream &kStream)const;
	void ReadFromPacket(BM::Stream &kStream);
	
}REG_LOG_INSTANCE;

typedef struct tagDebugLogMessage
	:	public BM::Stream
{
	tagDebugLogMessage(DWORD const &dwLogWorkerType = 0, BM::E_LOG_LEVEL const &eLogLv = BM::LOG_LV0, std::wstring const &kLogMsg = _T(""))
	{
		Set(dwLogWorkerType, eLogLv, kLogMsg);
	}

	void Set(DWORD const &dwLogWorkerType, BM::E_LOG_LEVEL const &eLogLv, std::wstring const &kLogMsg, std::wstring const &kFileName = _T(""))
	{
		LogWorkerType(dwLogWorkerType);
		FileName(kFileName);
		LogMsg(kLogMsg);
		LogLv(eLogLv);
	}

	void operator = (tagDebugLogMessage const& rhs)
	{
		BM::Stream::operator = (rhs);
		LogWorkerType(rhs.LogWorkerType());
		FileName(rhs.FileName());
		LogMsg(rhs.LogMsg());
		LogLv(rhs.LogLv());
	}

	void WriteToPacket(BM::Stream &kStream)const
	{
		kStream.Push(LogWorkerType());
		kStream.Push(FileName());
		kStream.Push(LogMsg());
		kStream.Push(LogLv());
		
		kStream.Push(Size());
		kStream.Push((BM::Stream)*this);
		// *this 로 임시개체 만들어 Push 하는 것이 맞음.
		// 이유 : WriteToPacket 했을때, wrpos 값이 변경되지 않기를 원하기 때문.
	}
	
	void ReadFromPacket(BM::Stream &kStream)
	{
		Reset();

		kStream.Pop(m_kLogWorkerType);
		kStream.Pop(m_kFileName);
		kStream.Pop(m_kLogMsg);
		kStream.Pop(m_kLogLv);

		size_t iSize;
		kStream.Pop(iSize);
		if(iSize)
		{
			this->Data().resize(iSize);
			kStream.PopMemory(&this->Data().at(0), iSize);
			PosAdjust();
		}
	}

	CLASS_DECLARATION_S(DWORD, LogWorkerType);
	CLASS_DECLARATION_S(std::wstring, FileName);
	CLASS_DECLARATION_S(std::wstring, LogMsg);
	CLASS_DECLARATION_S(BM::E_LOG_LEVEL, LogLv);
}SDebugLogMessage;

class PgLogWorker_Base
{
public:
	PgLogWorker_Base(void)
	{}
	virtual ~PgLogWorker_Base(void){};

public:	
	virtual HRESULT PushLog(SDebugLogMessage &kLogMsg) = 0;
	virtual HRESULT VRegistInstance(REG_LOG_INSTANCE const &kRegInfo) = 0;
};


typedef enum eLogWorkerType
{
	LWT_CEL_IN	= 1000,
	LWT_CEL_CA	,
	LWT_DB		, 
}E_LOG_WORKER_TYPE;
