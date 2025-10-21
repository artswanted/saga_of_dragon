#ifndef LOG_LOGSERVER_PGLOGMGR_H
#define LOG_LOGSERVER_PGLOGMGR_H

#include "BM/LogWorker_Base.h"
#include "Collins/Log.h"
#include "NullLog.h"

#pragma pack(1)

typedef struct tagSLogTypeKey {

	explicit tagSLogTypeKey(ELogMainType const _LogMain = ELogMain_None, ELogSubType const _LogSub = ELogSub_None,
		EOrderMainType const _OrderMain = ELOrderMain_None, EOrderSubType const _OrderSub = ELOrderSub_None)
		: eLogMain(_LogMain), eLogSub(_LogSub), eOrderMain(_OrderMain), eOrderSub(_OrderSub)
	{
	};

	tagSLogTypeKey const & operator =( tagSLogTypeKey const & rhs )
	{
		eLogMain = rhs.eLogMain;
		eLogSub = rhs.eLogSub;
		eOrderMain = rhs.eOrderMain;
		eOrderSub = rhs.eOrderSub;
		return (*this);
	};

	bool operator < (tagSLogTypeKey const &rhs) const
	{
		if (eLogMain < rhs.eLogMain) { return true;}
		if (eLogMain > rhs.eLogMain) { return false; }

		if (eLogSub < rhs.eLogSub) { return true;}
		if (eLogSub > rhs.eLogSub) { return false; }

		if (eOrderMain < rhs.eOrderMain) { return true;}
		if (eOrderMain > rhs.eOrderMain) { return false; }

		if (eOrderSub < rhs.eOrderSub) { return true;}
		if (eOrderSub > rhs.eOrderSub) { return false; }

		return false;
	}	
	
	ELogMainType eLogMain;
	ELogSubType eLogSub;
	EOrderMainType eOrderMain;
	EOrderSubType eOrderSub;
} SLogTypeKey;


#pragma pack()


class PgLogManager
{
public:
	PgLogManager();
	virtual ~PgLogManager();

public:
	bool Locked_ReadConfigFile();
	void Locked_WriteLog(BM::GUID const &kQueryIndex, PgLogCont const &kLogCont);
	void Locked_Timer5s();
	void Locked_OnRegist(CEL::SRegistResult const &rkArg);

	void Locked_OnConnectSP(CEL::CSession_Base *pkSession);
	void Locked_OnDisConnectSP(CEL::CSession_Base *pkSession);
	void Locked_OnReceiveSP(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	HRESULT Locked_OnDB_Execute(CEL::DB_RESULT &rkResult);

	void CheckDailyLogTable() const;

protected:
	typedef enum : short
	{
		EW_None = 0x00,
		EW_DB_Write = 0x01,
		EW_File_Write = 0x02,
		EW_Write_All = EW_DB_Write|EW_File_Write,
		EW_Max = EW_Write_All,
	} EWriteMode;
	typedef std::map<SLogTypeKey, EWriteMode>	CONT_LOG_WRITE_MODE;

	void Init();
	bool WriteLogToFile(std::wstring const &kLogData);
	
	bool Q_DQT_DEF_WRITEMODE( CEL::DB_RESULT &rkResult );
	bool Q_DQT_CHECK_LOG_TABLE( CEL::DB_RESULT &rkResult );
	bool Q_DQT_CHECK_LOG_TABLE_NEXTDAY( CEL::DB_RESULT &rkResult );

private:
	PgNullLog* m_pkServiceProviderLog;
	CONT_LOG_WRITE_MODE m_kContWriteMode;		// DB에서 읽은 각 LogType 별로의 WriteMode
	CLASS_DECLARATION_S(EWriteMode, WriteMode);	// ini 파일에서 읽어온 WriteMode
	CLASS_DECLARATION_S(bool, Ready);

//	BM::CDebugLog	m_kFileLog;
	mutable ACE_RW_Thread_Mutex m_kMutex;
};

#define g_kLogMgr SINGLETON_STATIC(PgLogManager)

#endif // LOG_LOGSERVER_PGLOGMGR_H