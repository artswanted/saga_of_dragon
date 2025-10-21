#ifndef GM_GMSERVER_GM_PGGMTASK_H
#define GM_GMSERVER_GM_PGGMTASK_H

#include "IGMLocaleProcess.h"
#include "PgMsgHolder.h"
#include "BM/PgMsgDispatcher.h"

typedef enum 
{
	GOET_KICK_USER = 1,
	GOET_CHANGE_PW = 2,
	GOET_CREATE_ACCOUNT = 3,
	GOET_FREEZE_ACCOUNT = 4,
	GOET_ADD_POINT = 5,
	GOET_GET_POINT= 6,
	GOET_CHANGE_BIRTH= 7,
	GOET_IS_USER_NAME= 8,
}EGMOrderEventType;

class PgMsgQue;

typedef struct tagGMEventOrder
	: public BM::Stream 
{
	typedef std::set< std::wstring > CONT_WORKDATA_TARGET;

	tagGMEventOrder(PgMsgQue *pkMsgQue = NULL);
	void Clear();

	BM::GUID const & MsgGuid()const{return m_kMsgGuid;}
	void InsertTarget(CONT_WORKDATA_TARGET::value_type const &rkKey)
	{
		std::wstring uprKey = rkKey;
		UPR(uprKey);
		m_kContTarget.insert(rkKey);
	}
	
	CONT_WORKDATA_TARGET m_kContTarget;//외부에서 건들지 

	EGMOrderEventType ePriMsgType;

	void * m_pkMsgQue;//메세지 큐의 포인터

protected:
	BM::GUID m_kMsgGuid;//이 메세지 자체의 GUID
}SGMEventOrder;

class PgGMWorker
	: public PgMsgWorker< SGMEventOrder >
{
public :
	PgGMWorker( IGMLocaleProcess* pkLocalProcess = NULL)
		: m_pkLocalProcess(pkLocalProcess)
	{
	}
	virtual ~PgGMWorker(){};

public:
	virtual HRESULT VProcess(SGMEventOrder *pkWorkData);

protected:
	IGMLocaleProcess* m_pkLocalProcess;
};

class PgGMTask
	: public PgMsgDispatcher< SGMEventOrder, PgGMWorker >
//	, public PgMsgHolder
{
	typedef PgMsgDispatcher< SGMEventOrder, PgGMWorker > PARENT_TYPE;
public:
	PgGMTask(void);
	virtual ~PgGMTask(void);

public:
	void static RecvGMToolProcess(CEL::CSession_Base * const pkSession, BM::Stream * const pkPacket);
	void static GetNowTime(BM::DBTIMESTAMP_EX &rkOut);

	void RecvGMLocaleProcess(BM::Stream * const pkPacket);

	virtual HRESULT VPush(WORK_DATA const* pkWorkData)//재정의.
	{
		if( S_OK == m_pkLocalProcess->VHookVPush(reinterpret_cast< void const* >(pkWorkData)))
		{
			if(S_OK == PARENT_TYPE::VPush(pkWorkData))
			{
				return S_OK;
			}
			else
			{//메세지 ready 시킨거 지워야 함.
				VCompleteMsg(pkWorkData->MsgGuid());
			}
		}
		return E_FAIL;
	}

	virtual HRESULT VCompleteMsg(BM::GUID const& kMsgGuid)//재정의. 
	{
		return m_pkLocalProcess->VCompleteMsg(kMsgGuid);
	}

public:
	bool Locked_LoginGMTool(CEL::CSession_Base * const pkSession);
	bool Locked_LogOutGMTool(CEL::CSession_Base * const pkSession);
	void Locked_FailedOrder(BM::GUID const& kCmdGuid, E_GM_ERR const eErrorCode)const;
	void Locked_SendOrderState(BM::GUID const& kCmdGuid, EOrderState const eState, E_GM_ERR const eErrorCode = GE_SUCCESS)const;
private:
	CEL::CSession_Base * m_pkGMToolSession;

protected:
	virtual MSG_WORKER* CreateMsgWorker()
	{
		return new PgGMWorker(m_pkLocalProcess);
	}

protected:
	IGMLocaleProcess* m_pkLocalProcess;
	CLASS_DECLARATION_S(int, iReflashTime);

private:
	mutable Loki::Mutex m_kMutex;
};

#define g_kGMTask SINGLETON_STATIC(PgGMTask)

#endif // GM_GMSERVER_GM_PGGMTASK_H