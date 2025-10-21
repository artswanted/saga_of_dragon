#ifndef IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGGMPROCESSMGR_H
#define IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGGMPROCESSMGR_H

#include "Variant/GM_const.h"

class PgGMProcessMgr
{
public:

	PgGMProcessMgr(void);
	~PgGMProcessMgr(void);

	typedef enum
	{
		EGM_2ND_NONE = 0,
		EGM_2ND_PACKET = 1,
		EGM_2ND_DBRESULT = 2,
		EGM_2ND_ORDERSTATE = 3,
	} EGM_TASK_TYPE;	// SEventMessage::SecondaryType

	void HandleMessage(SEventMessage *pkMsg);

	static void SendOrderState(BM::GUID const &rkOrderId, unsigned short usState = OS_DONE );	//2 = ORDER_DONE
	static void SendOrderFailed(BM::GUID const &rkOrderId, E_GM_ERR const eErrorCode);

protected:
	static bool SendToGmServer(BM::Stream const&rkPacket);
	void RecvGMCommand(BM::Stream * const pkPacket );
	bool AnalysisOderData(BM::Stream * const pkPacket);

	// GM Command
	bool Q_DQT_GM_FREEZE_ACCOUNT( CEL::DB_RESULT &rkResult );
	bool Q_DQT_GM_ADD_CASH( CEL::DB_RESULT &rkResult );
	bool Q_DQT_GM_GET_CASH( CEL::DB_RESULT &rkResult );
	bool Q_DQT_USER_CREATE_ACCOUNT( CEL::DB_RESULT &rkResult );
	bool Q_DQT_CHANGE_CHARACTER_NAME( CEL::DB_RESULT &rkResult );
	bool Q_DQT_CHANGE_CHARACTER_STATE( CEL::DB_RESULT &rkResult );
	bool Q_DQT_CHANGE_CHARACTER_POS( CEL::DB_RESULT &rkResult );
	bool Q_DQT_CHANGE_GM_LEVEL(CEL::DB_RESULT &rkResult);
	bool Q_DQT_CHANGE_PASSWORD( CEL::DB_RESULT &rkResult );
	bool Q_DQT_GM_CHANGE_BIRTHDAY( CEL::DB_RESULT &rkResult );

private:
	void ProcessKickUser(int iCase, BM::GUID const &kReqGuid, std::wstring const &kID, bool const bAfterBlock = false);
	bool ProcessKickUser(int iCase, BM::GUID const &kReqGuid, BM::GUID const &kMemberID, bool const bAfterBlock = false);
	void ProcessBlockUser(int iCase, BM::Stream *pkPacket);
	
	bool CommonGMOrderQueryResult( CEL::DB_RESULT &rkResult );
	const BM::GUID m_kMgrGuid;
protected:
	Loki::Mutex m_kMutex;
};

//#define g_kGMProcessMgr SINGLETON_STATIC(PgGMProcessMgr)

#endif // IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGGMPROCESSMGR_H