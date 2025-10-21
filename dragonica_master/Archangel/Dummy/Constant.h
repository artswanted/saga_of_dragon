#pragma once

#pragma pack (1)

typedef enum
{
	ETEST_LOGIN	= 1,
} EDummyAIState;

typedef enum
{
	ECS_NONE			= 0,
	ECS_DISCONNECTED	= 1,
	ECS_TRYCONNECT,
	ECS_CONNECTED,
	ECS_TRYLOGIN,
	ECS_LOGINED,
	ECS_SWITCHRESERVED,
	ECS_TRYCONNECTSWITCH,
	ECS_CONNECTEDSWITCH,
	ECS_TRYACCESSSWITCH,
	ECS_REQ_CHARACTERLIST,
	ECS_RECV_CHARACTERLIST,
	ECS_SELECTEDCHARACTER,
	ECS_REQ_ENTERMAP,
	ECS_MAP_ENTERED,
	ECS_REQ_MAP_LOADED,
	ECS_MAP_LOADED,
}EClientStatus;

typedef struct tagCLIENT_INFO
{
	tagCLIENT_INFO()
	{
//		chID[0] = _T('\0');
//		kSessionKey = BM::GUID();
		eStatus = ECS_DISCONNECTED;
	}

	std::wstring kStr;
	
	CEL::SESSION_KEY kSessionKey;
	EClientStatus eStatus;
	
	DWORD dwLastTickTime;
	DWORD dwTimeCounter;

	CEL::ADDR_INFO kSwitchAddr;
	BM::GUID kSwitchGuid;

	BM::GUID kCharacterGuid;
} CLIENT_INFO;

typedef std::map<BM::GUID, CLIENT_INFO> MAP_CLIENT_INFO;	// <SessoinGuid, ClientInfo>
typedef std::vector< CLIENT_INFO > VECTOR_CLIENT_INFO;


#define ERROR_ELAPSEDTIME	99999999
typedef struct
{
	unsigned long ulTime;
	EClientStatus eState;
	unsigned long ulElapsedTime;	// ERROR_ELAPSEDTIME : Error
} ELAPSED_TIME_INFO;

typedef std::vector<ELAPSED_TIME_INFO> VECTOR_ELAPSED_TIME_INFO;

typedef struct tagSTAT_DATA
{
	tagSTAT_DATA(unsigned long ulTimeInterval, EClientStatus eState)
	{
		time(&tTime);
		ulInterval = ulTimeInterval;
		eStatus = eState;
		sCount = sErrorCount = 0;
		ulElapsedTime = 0;
	}

	time_t tTime;					// 통계작성된 시간
	unsigned long ulInterval;		// 통계가 작성되는 간격
	short int sCount;				// 개수
	short int sErrorCount;			// Error 개수
	EClientStatus eStatus;			// 통계 타입
	unsigned long ulElapsedTime;	// 평균지연시간
} STAT_DATA;

typedef std::map<EClientStatus, STAT_DATA> MAP_STAT_DATA;
typedef std::vector<STAT_DATA> VECTOR_STAT_DATA;

typedef struct
{
	EClientStatus eStatus;
	DWORD dwTickDelay;
} STICK_DELAY_TIME;


#pragma pack() 