#pragma once

typedef enum eLog_Type
{
	LT_COMPLATE,
	LT_ERROR,
}ELOGTYPE;





typedef	struct	tagErrorData
{
	tagErrorData( PgQuestInfoVerifyUtil::EErrorType Type = PgQuestInfoVerifyUtil::ET_None, int ID = 0, const BM::vstring ErrorMsg = BM::vstring(L"") )
	{
		eType = Type;
		iQuestID = ID;		
		kErrorMsg = ErrorMsg;
	}
	PgQuestInfoVerifyUtil::EErrorType	eType;	
	int	iQuestID;
	size_t iLine;
	BM::vstring kErrorMsg;
}SErrorData;

typedef	struct	tagLogMessage
{
	tagLogMessage(int QuestID = 0, ELOGTYPE const Type = LT_COMPLATE, std::wstring const wstrText = L"", SErrorData Data = SErrorData())
	{
		iQuestID = QuestID;
		eType = Type;
		wstrLog = wstrText;
		kData = Data;
	}

	int	iQuestID;
	ELOGTYPE eType;
	std::wstring wstrLog;
	SErrorData	kData;
}SLOGMESSAGE;

class CLogMgr : public PgTask < SLOGMESSAGE >
{
public:
	CLogMgr() {}
	virtual ~CLogMgr() {}

	//	메세지 처리
	virtual	void	HandleMessage(MSG *rkMsg)
	{
		if(rkMsg)
		{ 
			switch(rkMsg->eType)
			{
			case LT_COMPLATE:
				{
					AddLog(rkMsg->iQuestID, rkMsg->wstrLog);
				}break;
			case LT_ERROR:
				{
					AddError(rkMsg->iQuestID, rkMsg->kData);
				}break;
			}
		}
	}

	void	OutError(int iID);

	void Stop()
	{
		m_kIsStop = true;
	}

private:
	typedef struct	tagLogData
	{
		tagLogData()
		{
			wstrLog = L"";
			Data.clear();
		}
		typedef	std::vector< SErrorData >	kDataContainer;
		std::wstring	wstrLog;
		kDataContainer	Data;
	}SLogData;
	typedef	std::map< int, SLogData >	kLogContainer;
	kLogContainer	m_kLogCont;

	Loki::Mutex m_kAddError;
	void	AddLog(int QuestID, std::wstring const& wstrLog);
	void	AddError(int QuestID, SErrorData const& Data);
};


#define g_kLogMgr SINGLETON_STATIC(CLogMgr)
