#ifndef SWITCH_SWITCHSERVER_PGRELOADABLEDATAMGR_H
#define SWITCH_SWITCHSERVER_PGRELOADABLEDATAMGR_H

#include "Loki/Threads.h"
#include "BM/STLSupport.h"
#include "variant/TableDataManager.h"

class PgReloadableDataMgr
{
public:
	PgReloadableDataMgr(){}
	virtual~PgReloadableDataMgr(){}

public:
	void SyncProcess(BM::Stream * const pkPacket);
	void ReqSyncDataFromUser(BM::GUID const &kUserStoreValueKey, CEL::CSession_Base *pkSession);//유저로 부터

	void DataCompress(BM::Stream const &kOrgPacket, BM::Stream &kTgtPacket);//압축 시도 해보고 실패 하면. 그냥 냅둠

protected:
	BM::GUID m_kStoreValueKey;
	BM::Stream m_kPacket_CashShop;

protected:
	mutable ACE_RW_Thread_Mutex m_kMutex;
};

#define g_kReloadMgr SINGLETON_STATIC(PgReloadableDataMgr)

#endif // SWITCH_SWITCHSERVER_PGRELOADABLEDATAMGR_H