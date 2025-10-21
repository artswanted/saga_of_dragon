#ifndef IMMIGRATION_IMMIGRATIONSERVER_TASK_IMMTASK_H
#define IMMIGRATION_IMMIGRATIONSERVER_TASK_IMMTASK_H

#include "BM/PgTask.h"
#include "PgAPProcessMgr.h"
#include "PgConsentControl.h"
#include "PgGMProcessMgr.h"

typedef enum 
{
	EIMM_OBJ_NONE = 0,
	EIMM_OBJ_AP = 1,		// PgAPProcessMgr
	EIMM_OBJ_CONSENT = 2,	// PgConsentControl
	EIMM_OBJ_GM = 3,		// PgGMProcessMgr
} EIMM_TASK_OBJ;	// SEventMessage::PrimaryType

class PgImmTask
	: public PgTask<>
{
public:
	PgImmTask();
	virtual ~PgImmTask();

	// 더이상 public interface 만들지 말라.
	virtual void HandleMessage(SEventMessage *pkMsg);
	void Close();
	void Timer10s(DWORD dwUSerData);
//	HRESULT DBProcess_AP(EDBQueryType const eType, CEL::DB_RESULT &rkResult);

private:
	PgAPProcessMgr m_kAP;
	PgConsentControl m_kConsent;
	PgGMProcessMgr m_kGM;
};

#define g_kImmTask SINGLETON_STATIC( PgImmTask )

#endif // IMMIGRATION_IMMIGRATIONSERVER_TASK_IMMTASK_H