#ifndef CONTENTS_CONTENTSSERVER_DATABASE_PGGMPROCESSMGR_H
#define CONTENTS_CONTENTSSERVER_DATABASE_PGGMPROCESSMGR_H

typedef std::map< BM::GUID, SGmOrder*> CONT_GMORDER;

class PgGMProcessMgr
{
public:
	PgGMProcessMgr(void);
	~PgGMProcessMgr(void);

public:
	bool ExecuteGmOrder(BM::GUID const &rkCmdGuid);
	void FirstAnalysisGmOder(SGmOrder &rkOderData);
	void OnGMKickUser(bool bSuccess, int const iCase, BM::GUID const & rkOrderGuid, BM::GUID const &rkMemberGuid);
	void RecvGMCommand(BM::Stream * const pkPacket );

	static void SendOrderState(BM::GUID const &rkOrderId, unsigned short usState = OS_DONE );	//2 = ORDER_DONE
	static void SendOrderFailed(BM::GUID const &rkOrderId, E_GM_ERR const eErrorCode);

	////////////////////////////////////////////////////WebTool///////////////////////////////////////////////////////////
	static bool Q_DQT_CHANGE_CHARACTER_NAME( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_CHARACTER_STATE( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_GM_LEVEL(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_CHANGE_CHARACTER_POS( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_CHARACTER_FACE( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_QUEST_INFO( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_SKILL_INFO( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_CP( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_CHANGE_ACHIEVEMENT( CEL::DB_RESULT &rkResult);
	static bool Q_DQT_GMORDER_SELECT_MEMBER_GMLEVEL(CEL::DB_RESULT &rkResult);
private:
	static bool CommonQueryResult( CEL::DB_RESULT &rkResult );

private:
	bool AnalysisOderData(SGmOrder &rkOrderData);
	
protected:
	Loki::Mutex m_kMutex;
	CONT_GMORDER m_kContGMOrder;
};

#define g_kGMProcessMgr SINGLETON_STATIC(PgGMProcessMgr)

#endif // CONTENTS_CONTENTSSERVER_DATABASE_PGGMPROCESSMGR_H