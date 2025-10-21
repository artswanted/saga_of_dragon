#pragma once
#define _WIN32_DCOM 

#include <map>
#include <vector>
#include <mmsystem.h>

#include "BM/STLSupport.h"
#include "Loki/Threads.h"

#include "BM/Guid.h"
#include "BM/vstring.h"
#include "BM/LogWorker_Base.h"
#include "BM/ThreadObject.h"
#include "BM/BM.h"

#include "BM/Stream.h"

namespace CEL
{
	typedef enum eSessionType
		:short
	{
		ST_NONE			= 0,
		ST_CENTER		= 1,
		ST_MAP			= 2,
		ST_LOGIN		= 3,
		ST_SWITCH		= 4,
		ST_MACHINE_CONTROL= 5,	//MMC
//		ST_RELAY		= 6,
//		ST_USER			= 7,
		ST_ITEM			= 8,
		ST_CONTENTS		= 9,	// Contents server
		ST_LOG			= 10,	// Log server
		ST_CONSENT		= 11,	// Consent Server
		ST_GMSERVER		= 12,	// GM Server
		ST_GHOST_LOGIN	= 13,	// For First Center
		ST_IMMIGRATION	= 14,	// 


		ST_SUB_MACHINE_CONTROL = 20,	// SMC

		// EXTERNAL Server type
		ST_AP			= 30,	// 대만 연동용 AP Server
		ST_GALA			= 31,	// GALA Server
		ST_EXTERNAL1	= 32,
		ST_EXTERNAL2,
		ST_EXTERNAL3,
		ST_EXTERNAL4,
		ST_MAX			,	// Max Value
	}E_SESSION_TYPE;

	typedef enum eServiceHandlerType
		:int
	{
		SHT_DEFAULT					= 0,
		SHT_NC						= 1,
		SHT_SERVER					= 2,
		SHT_FILE_SERVER				= 3,
		SHT_GF						= 4,
		SHT_NOHEADER				= 5,
		SHT_JAPAN					= 6,
		SHT_GRAVITY					= 7,
		SHT_KINGWORLD				= 8,

		SHT_MAX,
	}E_SERVIEC_HANDLER_TYPE;

	typedef enum eNotifyCause
	{
		NC_IDLE			= 1,
		NC_OPEN			= 2,
		NC_CLOSE		= 3,
		NC_COMPLETE_R	= 4,

		NC_COMPLETE_S_SUCCESS	= 5,
		NC_COMPLETE_S_FAILED	= 6,
		
		NC_TRY_SEND		= 7,

		NC_8			= 8,
		NC_9			= 9,
		NC_10			= 10,
	}eNOTIFY_CAUSE;
	
	typedef enum eRegistType
	{
		RT_NONE = 0,
		RT_ACCEPTOR = 1,
		RT_CONNECTOR,
		RT_DB_WORKER,
		RT_TIMER_FUNC,
		
		RT_FINAL_SIGNAL,//등록 완료 체크용
	}ERegistType;

#pragma pack(1)
	typedef struct tagAddrInfo
	{
		tagAddrInfo();
		tagAddrInfo(std::wstring const& wstrInIP, WORD const wInPort);
		void Set(std::wstring const& wstrInIP, WORD const wInPort);
		void Clear();
		bool IsCorrect()const;
		bool IsCorrectSubNetMask( std::wstring const &wstrBaseIP, std::wstring const &wstrSubNetMask )const;
		bool IsCorrectSubNetMask( in_addr const &addrBaseIP, in_addr const &addrSubNetMask )const;
		bool operator < (const tagAddrInfo &rhs) const;	
		bool operator == (const tagAddrInfo &rhs) const;

		std::wstring IP()const
		{
			char* pszAddr = ::inet_ntoa(ip);
			if (pszAddr != NULL)
			{
				std::string str( pszAddr  );
				return UNI(str);
			}
			return _T("NoneIP");
		}

		std::wstring ToString()const
		{
			wchar_t szTemp[10] = {0,};
			::swprintf_s( szTemp, 10, L":%d", (int)wPort );
			return ( IP() + szTemp );
		}

		tagAddrInfo const& operator=(tagAddrInfo const& rhs)
		{
			ip = rhs.ip;
			wPort = rhs.wPort;
			return (*this);
		}
	
		in_addr			ip;
		WORD			wPort;

		size_t min_size()const
		{
			return sizeof(ip)+sizeof(wPort);
		}

		void WriteToPacket(BM::Stream &kPacket)const 
		{
			kPacket.Push(ip);
			kPacket.Push(wPort);
		}

		void ReadFromPacket(BM::Stream &kPacket)
		{
			kPacket.Pop(ip);
			kPacket.Pop(wPort);
		}
	}ADDR_INFO;

	typedef struct tagRegistResult
	{
		BM::GUID guidObj;
		BM::GUID guidOrder;
		ERegistType eType;
		int iRet;//결과
		int iIdentityValue;//추가 값.(FINAL에서 주로씀)
	}SRegistResult;

	class CSession_Base;

	typedef void (CALLBACK *LP_CALLBACK_SESSION_OPEN)( CSession_Base *pSession );
	typedef void (CALLBACK *LP_CALLBACK_DISCONNECT)( CSession_Base *pSession );
	typedef void (CALLBACK *LP_CALLBACK_RECV)( CSession_Base *pSession, BM::Stream * const pPacket );
	
	typedef void (CALLBACK *LP_CALLBACK_TIMER)(DWORD dwUserData);
	typedef void (CALLBACK *LP_CALLBACK_ON_REGIST)(const SRegistResult &rArg);

	typedef std::list< ADDR_INFO > ADDR_ARRAY;

	typedef struct tagRegist_Base
	{
		tagRegist_Base(){}
		virtual ~tagRegist_Base(){}
	}REGIST_BASE;

	typedef struct tagInitCenterDesc
		: public tagRegist_Base
	{
		BM::E_OUPUT_TYPE eOT;//BM::OUTPUT_ALL
		LP_CALLBACK_ON_REGIST pOnRegist;
		DWORD dwProactorThreadCount;
		bool bIsUseDBWorker;
		std::wstring m_kCelLogFolder;

		tagInitCenterDesc();
		virtual ~tagInitCenterDesc(){}
	}INIT_CENTER_DESC;

	typedef struct tagInitCoreDesc
		: public tagRegist_Base
	{
		typedef enum eValue
		{
			E_DEFAULT_PACKET_BUFFER_SIZE = 1024*1024*512,//!  500MB
		}E_VALUE;
		tagInitCoreDesc();
		virtual ~tagInitCoreDesc(){}
		bool IsCorrect()const;

		bool bIsAllow;
		BM::GUID kOrderGuid;
		ADDR_INFO kBindAddr;
		ADDR_INFO kNATAddr;

		DWORD dwBandwidthLimit;
		DWORD dwPackerBufferSize;//! 세션당 패킷 버퍼 사이즈
		size_t nMaxPacketLen;//! 한개의 패킷이 가질 수 있는 최대 길이. //->이거는 헤더가 int 가 되면 쓴다.
		bool bIsImmidiateActivate;
		DWORD dwThreadCount;//! 해당 요소의 스레드 갯수
		CLASS_DECLARATION_S(int, ServiceHandlerType);//기본값 0

		CLASS_DECLARATION_S(int, IdentityValue);//기본값 0

		LP_CALLBACK_SESSION_OPEN	OnSessionOpen;
		LP_CALLBACK_DISCONNECT		OnDisconnect;
		LP_CALLBACK_RECV			OnRecv;

		void WriteToPacket(BM::Stream &kPacket)const;
		void ReadFromPacket(BM::Stream &kPacket);
	}INIT_CORE_DESC;

	typedef struct tagRegistTimerDesc
		: public tagRegist_Base
	{
		tagRegistTimerDesc()
		{
			kOrderGuid.Generate();
			pFunc = NULL;
			dwInterval = 0;
			dwUserData = 0;
			bIsImmidiate = false;
			dwThreadCount = 1;
		}
		virtual ~tagRegistTimerDesc(){}

		BM::GUID kOrderGuid;
		BM::GUID kTimerGuid;//등록된 타이머 핸들

		LP_CALLBACK_TIMER pFunc;
		DWORD dwInterval;
		DWORD dwUserData;

		DWORD dwThreadCount;//타이머당 스레드 하나. 다중 타이머는 숫자 늘리도록.
		bool bIsImmidiate;// 즉시시작?
		
		void WriteToPacket(BM::Stream &kPacket)const;
		void ReadFromPacket(BM::Stream &kPacket);
	}REGIST_TIMER_DESC;

	typedef struct tagInitFinalSignal
		: public tagRegist_Base
	{
		tagInitFinalSignal();
		virtual ~tagInitFinalSignal(){}
		
		BM::GUID kOrderGuid;
		int kIdentity;

		void WriteToPacket(BM::Stream &kPacket)const;
		void ReadFromPacket(BM::Stream &kPacket);	
	}INIT_FINAL_SIGNAL;

	typedef struct tagSessionKey
	{
		tagSessionKey()
		{
			Clear();
		}

		tagSessionKey(BM::GUID const& kWorkerGuid, BM::GUID const& kSessoinGuid)
		{
			Set(kWorkerGuid, kSessoinGuid);
		}
	
		void Set(BM::GUID const& kWorkerGuid, BM::GUID const& kSessoinGuid)
		{
			WorkerGuid(kWorkerGuid);
			SessionGuid(kSessoinGuid);
		}

		bool IsEmpty()const
		{
			if(	WorkerGuid() == BM::GUID::NullData()
			||	SessionGuid() == BM::GUID::NullData() )
			{
				return true;
			}
			return false;
		}

		bool operator < (const tagSessionKey &rhs) const
		{
			if( WorkerGuid() < rhs.WorkerGuid() )	{return true;}
			if( WorkerGuid() > rhs.WorkerGuid() )	{return false;}

			if( SessionGuid() < rhs.SessionGuid() )	{return true;}
			if( SessionGuid() > rhs.SessionGuid() )	{return false;}
			return false;
		}

		bool operator == (const tagSessionKey &rhs) const
		{
			if(	WorkerGuid()	==	rhs.WorkerGuid()
			&&	SessionGuid()	==	rhs.SessionGuid() )
			{
				return true;
			}
			return false;
		}

		bool operator != ( const tagSessionKey &rhs )const
		{
			if(*this == rhs)
			{
				return false;
			}
			return true;
		}

		CLASS_DECLARATION_S(BM::GUID, WorkerGuid);
		CLASS_DECLARATION_S(BM::GUID, SessionGuid);

		void Clear()
		{
			m_kWorkerGuid.Clear();
			m_kSessionGuid.Clear();
		}

		BM::vstring ToString() const
		{
			return BM::vstring(L" SESSION_KEY[WG:")<<WorkerGuid()<<L",SG:"<<SessionGuid()<<L"]";
		}
	}SESSION_KEY;

	typedef struct tagSessionStatistics
	{
		tagSessionStatistics()
		{
			TotalSendCount(0);
			TotalSendBytes(0);
			CompleteSendBytes(0);
			TrySendBytes(0);
			TotalRecvCount(0);
			TotalRecvBytes(0);
			
			RemainMsgBytes(0);
			RemainMsgLength(0);
			RemainMsgCount(0);
		}
		
		tagSessionStatistics operator + (tagSessionStatistics const& rhs)const
		{
			tagSessionStatistics kTemp;
			kTemp.TotalSendCount(TotalSendCount() +rhs.TotalSendCount());
			kTemp.TotalSendBytes(TotalSendBytes() +rhs.TotalSendBytes());
			kTemp.CompleteSendBytes(CompleteSendBytes() +rhs.CompleteSendBytes());
			kTemp.TrySendBytes(TrySendBytes() +rhs.TrySendBytes());

			kTemp.RemainMsgBytes(RemainMsgBytes() +rhs.RemainMsgBytes());
			kTemp.RemainMsgLength(RemainMsgLength() +rhs.RemainMsgLength());
			kTemp.RemainMsgCount(RemainMsgCount() +rhs.RemainMsgCount());
			return kTemp;
		}

		void operator += (tagSessionStatistics const& rhs)
		{
			*this = (*this + rhs);
		}

		void IncSendCount(){++m_kTotalSendCount;}
		void AddSendBytes(__int64 const iSendedBytes)
		{
			TotalSendBytes(TotalSendBytes() + iSendedBytes);
		}

		void AddRecvBytes(__int64 const iSendedBytes)
		{
			TotalSendBytes(TotalSendBytes() + iSendedBytes);
		}

		CLASS_DECLARATION_S(__int64, TotalSendCount);
		CLASS_DECLARATION_S(__int64, TotalSendBytes);

		CLASS_DECLARATION_S(__int64, CompleteSendBytes);
		CLASS_DECLARATION_S(__int64, TrySendBytes);

		CLASS_DECLARATION_S(__int64, TotalRecvCount);//받은 횟수.
		CLASS_DECLARATION_S(__int64, TotalRecvBytes);//받은 양.

		CLASS_DECLARATION_S(size_t, RemainMsgBytes);
		CLASS_DECLARATION_S(size_t, RemainMsgLength);
		CLASS_DECLARATION_S(size_t, RemainMsgCount);
	}SSessionStatistics;
#pragma pack()	
//! #pragma pack ()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#define g_kCoreCenter SINGLETON_STATIC(CEL::CCoreCenter)



