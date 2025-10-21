#include "stdafx.h"
#include "ImmTask.h"

PgImmTask::PgImmTask()
{
}

PgImmTask::~PgImmTask()
{
}

void PgImmTask::Close()
{
	this->VDeactivate();
}

void PgImmTask::HandleMessage(SEventMessage *pkMsg)
{
	EIMM_TASK_OBJ const eObj = static_cast<EIMM_TASK_OBJ const>(pkMsg->PriType());
	switch( eObj )
	{
	case EIMM_OBJ_AP:
		{
			m_kAP.HandleMessage(pkMsg);
		}break;
	case EIMM_OBJ_CONSENT:
		{
			m_kConsent.HandleMessage(pkMsg);
		}break;
	case EIMM_OBJ_GM:
		{
			m_kGM.HandleMessage(pkMsg);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown Task obj=") << eObj);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unknown Task obj=") << eObj);
		}break;
	}
}

void PgImmTask::Timer10s(DWORD dwUserData)
{
	m_kConsent.Locked_Timer10s(dwUserData);
}

// HRESULT PgImmTask::DBProcess_AP(EDBQueryType const eType, CEL::DB_RESULT &rkResult)
// {
// 	switch (eType)
// 	{
// 	case DQT_TRY_AUTH_CHECKPW_AP:
// 		{
// 			m_kAP.Q_DQT_TRY_AUTH_CHECKPW_AP(rkResult);
// 		}break;
// 	default:
// 		{
// 			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown DB Type = ") << eType);
// 		}break;
// 	}
// 	return S_OK;
// }
