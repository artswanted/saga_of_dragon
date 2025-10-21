#ifndef LOG_LOGSERVER_NULLLOG_H
#define LOG_LOGSERVER_NULLLOG_H

typedef enum
{
	E_Logger_Type_Null = 0,
	E_Logger_Type_NC = 1,
} E_Logger_Type;

class PgNullLog
{
public:
	explicit PgNullLog( bool const bInternalTest );
	virtual ~PgNullLog();

	virtual E_Logger_Type GetLoggerType() { return E_Logger_Type_Null; }
	virtual void Locked_Timer5s() = 0;
	virtual void Locked_WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, PgLogCont const &kLogCont) = 0;
	virtual HRESULT Locked_OnDB_Execute(CEL::DB_RESULT &rkResult) = 0;
	
	virtual void CheckDailyLogTable()const{}

protected:
	bool LogInit(BM::E_OUPUT_TYPE const eOutType, std::wstring const& strFolder, std::wstring const &strFile);

protected:
	mutable Loki::Mutex m_kMutex;
	bool	m_bInternalTest;

private:
	PgNullLog();
};

#endif // LOG_LOGSERVER_NULLLOG_H