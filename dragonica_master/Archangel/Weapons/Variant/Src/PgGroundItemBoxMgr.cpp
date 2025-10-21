#include "stdafx.h"
#include "PgGroundItemBoxMgr.h"

PgGroundItemBoxMgr::PgGroundItemBoxMgr(void)
	: m_kBoxPool(100)
{
}

PgGroundItemBoxMgr::~PgGroundItemBoxMgr(void)
{
}

PgGroundItemBox* PgGroundItemBoxMgr::Create()
{
	PgGroundItemBox* pkRet = m_kBoxPool.New();
	
	if(pkRet)
	{
//		pkRet->Init();
		return pkRet;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}