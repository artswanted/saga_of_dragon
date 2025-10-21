#ifndef LOG_LOGSERVER_NCLOG_H
#define LOG_LOGSERVER_NCLOG_H

#include "NullLog.h"
#include "LogUtil.h"

int const NC_LOGD_SERVER_TYPE = 3;

typedef enum : BYTE
{
	RQ_LOG_SEND_MSG = 0x00, 
		// d server type
		// d log type
		// S log message
	RQ_SET_CHECK_STATUS = 0x01,
		// d set option ( 1:set check, 0:unset check)
	RQ_CHECK_LOGD = 0x02,
		// d check option
	RQ_SERVER_STARTED = 0x03,
		// d world id
		// d server type (1~4)
	RQ_LOG_MAX = 0x04
} ENcLogState;

typedef enum
{
	E_NC_LOG_GROUP_NONE = 0,
	E_NC_LOG_GROUP_GAMEDATA = 6,
	E_NC_LOG_GROUP_REALTIME = 7,
	E_NC_LOG_GROUP_CHAT = 9,
	E_NC_LOG_GROUP_STATISTICS = 10,
} E_NC_LOG_GROUP;

//typedef enum : short
//{
//	E_NC_LOG_FUNC_NONE = 0,
//	E_NC_LOG_FUNC_LOGOUT_TYPE = 1,	// Logout type
//	E_NC_LOG_FUNC_SEC2MIN = 2,	// Seconds -> Minutes
//	E_NC_LOG_FUNC_ITEM_ENCHANT1_STRING = 3,	// Enchant1 값 string 으로
//	E_NC_LOG_FUNC_ITEM_ENCHANT2_STRING = 4, // Enchant2 값 string 으로
//	E_NC_LOG_FUNC_INVEN_TYPE = 5,	// Inventory type
//	E_NC_LOG_FUNC_PARTY_OPEN_OPT = 6,	// Party 공개 여부 값 변환
//	E_NC_LOG_FUNC_PARTY_ITEM_OPT = 7,	// Party 아이템 먹기 여부
//	E_NC_LOG_FUNC_GROUNDNO_STRING = 8,	// GroundNo를 string 으로
//	E_NC_LOG_FUNC_GROUNDGUID_STRING = 9,// GroundGuid를 string 으로
//} E_NC_LOG_CONVERT_FUNC_TYPE;


#pragma pack(1)

//
namespace NCLogUtil
{
	int ConvertIdx(int const iIndex);

	template< typename _TYPE, size_t _ARRAY_SIZE >
	inline void Set(_TYPE (&abyArray)[_ARRAY_SIZE], int const iIndex, _TYPE const& kValue)
	{
		if( 0 <= iIndex
		&&	iIndex < _ARRAY_SIZE )
		{
			if( abyArray[iIndex] != kValue )
			{
				abyArray[iIndex] = kValue;
			}
		}
	}
};

//
typedef struct tagNcLog
{
	static int const MAX_INT_COUNT = 7;
	static int const MAX_I64INT_COUNT = 6;
	static int const MAX_STRING_COUNT = 15;

	explicit tagNcLog(int const iType);
	void Init();

	void SetColumn(int const iIndex, int const& kValue);
	void SetColumn(int const iIndex, __int64 const& kValue);
	void SetColumn(int const iIndex, std::wstring const& kValue);
	static DR2LogUtil::ELogValue_Type const GetColumnType(int iColumnIndex);

public:
	int const iLogType;
	int iData[MAX_INT_COUNT];
	__int64 i64Data[MAX_I64INT_COUNT];
	std::wstring wstrData[MAX_STRING_COUNT];
} SNcLog;

//
typedef struct tagNcChatLog
{
	static int const MAX_INT_COUNT = 9;
	static int const MAX_STRING_COUNT = 5;
	static int const MAX_I64INT_COUNT = 3;
	explicit tagNcChatLog(int const iType);
	void Init();

public:
	void SetColumn(int const iIndex, int const& kValue);
	void SetColumn(int const iIndex, __int64 const& kValue);
	void SetColumn(int const iIndex, std::wstring const& kValue);
	static DR2LogUtil::ELogValue_Type const GetColumnType(int iColumnIndex);

public:
	int const iLogType;
	int iData[MAX_INT_COUNT];
	std::wstring wstrData[MAX_STRING_COUNT];
	__int64 i64Data[MAX_I64INT_COUNT];
} SNcChatLog;

//
namespace ConvertLog2NC
{
	typedef enum eNCLogModelType : int
	{
		NCLMT_NONE		= 0,
		NCLMT_NORMAL	= 1,
		NCLMT_CHAT		= 2,
	} ENCLogModelType;

	//
	typedef struct tagDRLogKey
	{
		tagDRLogKey();
		tagDRLogKey(int const& riDRLogType, int const& riDRSubLogType, int const& riDROrderType, int const& riDRActionType);
		~tagDRLogKey();

		bool operator < (tagDRLogKey const& rhs) const;
		bool operator ==(tagDRLogKey const& rhs) const;
		bool IsHaveAny() const;
		bool IsEqualAny(tagDRLogKey const& rhs) const;
		BM::vstring ToString() const;

		int iDRLogType;
		int iDRSubLogType;
		int iDROrderType;
		int iDRActionType;
	} SDRLogKey;

	typedef std::list< int > CONT_DR_LOG_INDEX;
	typedef std::map< int, CONT_DR_LOG_INDEX > CONT_NC_LOG_FIELD;
	typedef struct tagDefConvertLog2NC
	{
		ENCLogModelType iNCLogModelType;
		int iNcLogID;
		CONT_NC_LOG_FIELD kContLogField;
	} SDefConvertLog2NC;
	typedef std::map< SDRLogKey, SDefConvertLog2NC > CONT_DEF_CONVERT_LOG2NC;
};
#pragma pack()


//
class PgNcLog : public PgNullLog
{
public:
	explicit PgNcLog( bool const bInterNalTest );
	virtual ~PgNcLog();
	
	static int const CONNECTOR_TYPE = 101;
	static size_t const NCLOGD_MAX_PACKET_SIZE = 8192 - 2;

	virtual E_Logger_Type GetLoggerType() { return E_Logger_Type_NC; }
	virtual void Locked_Timer5s();
	void Locked_OnRegist(CEL::SRegistResult const &rkArg);
	void Locked_OnConnect(CEL::CSession_Base *pkSession);
	void Locked_OnDisConnect(CEL::CSession_Base *pkSession);
	void Locked_OnReceive(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	virtual void Locked_WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, PgLogCont const &kLogCont); // no lock
	virtual HRESULT Locked_OnDB_Execute(CEL::DB_RESULT &rkResult);

	virtual void CheckDailyLogTable()const;

protected:

	bool InitConnector();
	bool Send(BM::Stream& rkPacket);

	HRESULT Q_DQT_CHECK_LOG_TABLE(CEL::DB_RESULT &rkResult);
	HRESULT Q_DQT_LOAD_DEF_CONVERT2NC(CEL::DB_RESULT &rkResult);

	template< typename _T_NC_LOG_STRUCT >
	HRESULT WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, PgLogCont const &kLogCont, PgLog const* const pkLog, ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC::mapped_type const& rkMapped);
	HRESULT WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcLog const &rkLog); // Send 부분에 Lock
	HRESULT WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcChatLog const &rkLog); // Send 부분에 Lock
	void TestLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcLog const &rkLog);
	void TestLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcChatLog const &rkLog);
private:
	CLASS_DECLARATION_S(CEL::ADDR_INFO, Address);
	CLASS_DECLARATION_S(CEL::SESSION_KEY, SessionKey);

	typedef enum {
		E_SPLOG_NONE = 0,
		E_SPLOG_READY,
		E_SPLOG_CONNECT_TRY,
		E_SPLOG_CONNECTED,
	} E_SPLOG_SESSION;
	CLASS_DECLARATION_S(E_SPLOG_SESSION, SessionState);

private:
};

#include "NCLog.inl"

#endif // LOG_LOGSERVER_NCLOG_H