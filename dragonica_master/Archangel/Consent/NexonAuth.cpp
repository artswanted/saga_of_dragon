#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "AilePack/Constant.h"
#include "AilePack/GALA_Constant.h"
#include "Constant.h"
#include "GALAServer.h"

#include "Nexon.h"
#include "Nexon/AuthCheck.h"
#pragma comment(lib, "Nexon/AuthCheck.lib")

#include "BM/TimeCheck.h"

PgMsgDispatcher< SNexonAuthInfo, CNXLoginMsgWorker> g_kNxLoginDis;

HRESULT CNXLoginMsgWorker::VProcess(SNexonAuthInfo *pkWorkData)
{
	DWORD const dwPrev = ::timeGetTime();
	ETryLoginResult const eRet = CheckNexonPassport(pkWorkData);

	DWORD const dwNow = ::timeGetTime();
	
	DWORD const dwGab = dwNow - dwPrev;
	if(dwGab > 300)
	{
		INFO_LOG(BM::LOG_LV0, _T("Too Slow Login Process Gab:")<<dwGab);
	}

	g_kJapanDaemon.SendLoginResult(NULL, PgJapanServerImpl::SUserInfo(pkWorkData->iSiteNo, pkWorkData->kAuthReq, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), eRet);
	return S_OK;
}