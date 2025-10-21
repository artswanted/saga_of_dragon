#include "StdAfx.h"
#include "constant.h"
#include "Global.h"
#include "Network.h"
#include "PgClientMng.h"

PgClientMng::PgClientMng(void)
{
}

PgClientMng::~PgClientMng(void)
{
}

void PgClientMng::Init()
{
/*	
	const TEST_INFO* pkTestInfo = g_kNetwork.GetTestInfo();

	unsigned long ulNow = ::timeGetTime();
	unsigned long ulElapsed = 0;
	m_sThreadClientNum = (pkTestInfo->sEndIndex - pkTestInfo->sBeginIndex+1) / pkTestInfo->byTimer1s;
	BM::CAutoMutex kLock(m_kClientLock.kObjectLock);
	for (int iIndex=pkTestInfo->sBeginIndex; iIndex<=pkTestInfo->sEndIndex; iIndex++)
	{
		CLIENT_INFO kInfo;
		_stprintf_s(kInfo.chID, 20, _T("%s%d"), pkTestInfo->chID, iIndex);
		kInfo.eStatus = ECS_DISCONNECTED;
		kInfo.kSessionKey = BM::GUID();
		kInfo.dwLastTickTime = ulNow - pkTestInfo->ulInterval + ulElapsed;

		m_kReadyClient.push_back(kInfo);

		if (((iIndex-pkTestInfo->sBeginIndex) % m_sThreadClientNum) == 0)
		{
			ulElapsed = 0;
		}
		else
		{
			ulElapsed += 200;
		}
		
	}
*/
}

void PgClientMng::ConnectedLogin(CEL::CSession_Base* pkSession, unsigned long ulNow)
{
	const bool bIsSucc = pkSession->IsAlive();

	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}
	
	kClient.m_pkObject->eStatus = ECS_TRYCONNECT;
	CollectData(kClient.m_pkObject, ulNow, bIsSucc);

	if (bIsSucc)
	{
		kClient.m_pkObject->dwLastTickTime = 0;		// Do Next Step Next Tick
		kClient.m_pkObject->eStatus = ECS_CONNECTED;
	}
	else
	{
		kClient.m_pkObject->dwLastTickTime = ulNow;	// Retry Next Interval
		kClient.m_pkObject->eStatus = ECS_DISCONNECTED;
	}
}

bool PgClientMng::GetClient(const CEL::SESSION_KEY& rkSession, ObjectLock<CLIENT_INFO*>* pkClient)
{
/*	m_kClientLock.IncRefCount();

	int iSize = m_kReadyClient.size();
	for (int i=0; i<iSize; i++)
	{
		CLIENT_INFO& rkClient = m_kReadyClient.at(i);
		if (rkClient.kSessionKey == rkSession)
		{
			pkClient->SetValue(&m_kClientLock, &rkClient);
			m_kClientLock.DecRefCount();
			return true;
		}
	}
	m_kClientLock.DecRefCount();
*/
	return false;
}

void PgClientMng::CollectData(CLIENT_INFO* pkClient, unsigned long ulNowTime, bool bSuccess)
{
	ELAPSED_TIME_INFO kInfo;
	kInfo.ulTime = ulNowTime;
	kInfo.eState = pkClient->eStatus;
	kInfo.ulElapsedTime = (bSuccess) ? ulNowTime - pkClient->dwTimeCounter : ERROR_ELAPSEDTIME;

	{
		BM::CAutoMutex kLock(m_kDataLock.kObjectLock);
		m_kCollect.push_back(kInfo);
	}

	pkClient->dwTimeCounter = ulNowTime;
}

void PgClientMng::MakeStatistic1m()
{
	BM::CAutoMutex kLock(m_kDataLock.kObjectLock);
	VECTOR_ELAPSED_TIME_INFO::iterator itor = m_kCollect.begin();
	unsigned long ulNow = ::timeGetTime();
	MAP_STAT_DATA kTempStat;
	while (itor != m_kCollect.end())
	{
		MAP_STAT_DATA::iterator itor2 = kTempStat.find(itor->eState);
		if (itor2 == kTempStat.end())
		{
			STAT_DATA kTemp(60000, itor->eState);
			auto bRet = kTempStat.insert(std::make_pair(itor->eState, kTemp));
			itor2 = bRet.first;
		}
		if (itor->ulElapsedTime == ERROR_ELAPSEDTIME)	// ERROR
		{
			itor2->second.sErrorCount++;
		}
		else
		{
			itor2->second.sCount++;
			itor2->second.ulElapsedTime = (itor2->second.ulElapsedTime * (itor2->second.sCount-1) + itor->ulElapsedTime) / itor2->second.sCount;
		}

		++itor;
	}

	// Clear
	m_kCollect.clear();

	MAP_STAT_DATA::iterator itor2 = kTempStat.begin();
	while (itor2 != kTempStat.end())
	{
		m_kStatistic1m.push_back(itor2->second);

		++itor2;
	}
}

void PgClientMng::LoginConnectTimeOut(CLIENT_INFO* pkClient)
{
	unsigned long ulNow = ::timeGetTime();
	pkClient->eStatus = ECS_TRYCONNECT;
	CollectData(pkClient, ulNow, false);

	g_kCoreCenter.Terminate(pkClient->kSessionKey);
//	pkClient->kSessionKey = BM::GUID();
	pkClient->eStatus = ECS_DISCONNECTED;
	pkClient->dwLastTickTime = ulNow;
}

void PgClientMng::WriteStatistic()
{
	/*
	{
		MakeStatistic1m();
	}
	{
		TCHAR chTime[40];
		time_t tNow;
		time(&tNow);
		MakeTimeString(&tNow, 2, chTime, 40);
		const TEST_INFO* pkTestInfo = g_kNetwork.GetTestInfo();
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("\t Dummy Client Testing"));
		S_STATE_LOG(BM::LOG_LV6, _T("\t\tDATE : %s"), chTime);
		S_STATE_LOG(BM::LOG_LV6, _T("\t\tLOGINSERVER : %s"), pkTestInfo->sLoginServer.ToString());
		S_STATE_LOG(BM::LOG_LV6, _T("\t\tTEST ACCOUNT : %s[%d-%d]"), pkTestInfo->chID, pkTestInfo->sBeginIndex, pkTestInfo->sEndIndex);
		S_STATE_LOG(BM::LOG_LV6, _T("\t\tTEST TYPE : %d, INTERVAL : %d"), pkTestInfo->eTestType, pkTestInfo->ulInterval);
		S_STATE_LOG(BM::LOG_LV6, _T("\t\tTimer Thread Num (1 seconds) : %d"), (int)pkTestInfo->byTimer1s);
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));

		BM::CAutoMutex kLock(m_kDataLock.kObjectLock);
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("\tInterval : 1 minute,	CONNECT_LOGIN Process"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("%30s %20s %20s %20s"), _T("TIME"), _T("Count"), _T("Elapsed"), _T("Error"));
		VECTOR_STAT_DATA::iterator itor = m_kStatistic1m.begin();
		while (itor != m_kStatistic1m.end())
		{
			if (itor->eStatus == ECS_TRYCONNECT)
			{
				//time_t kTime = time((time_t*)&(itor->second.ulTime));
				MakeTimeString(&(itor->tTime), 1, chTime, 40);
				S_STATE_LOG(BM::LOG_LV6, _T("%30s %20hd %20u %20hd"), chTime, itor->sCount, itor->ulElapsedTime, itor->sErrorCount);
			}
			++itor;
		}
		S_STATE_LOG(BM::LOG_LV6, _T("\n\n"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("\tInterval : 1 minute,	RESERVED_SWITCH Process"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("%30s %20s %20s %20s"), _T("TIME"), _T("Count"), _T("Elapsed"), _T("Error"));
		itor = m_kStatistic1m.begin();
		while (itor != m_kStatistic1m.end())
		{
			if (itor->eStatus == ECS_LOGINED || itor->eStatus == ECS_TRYLOGIN)
			{
				MakeTimeString(&(itor->tTime), 1, chTime, 40);
				S_STATE_LOG(BM::LOG_LV6, _T("%30s %20hd %20u %20hd"), chTime, itor->sCount, itor->ulElapsedTime, itor->sErrorCount);
			}
			++itor;
		}
		S_STATE_LOG(BM::LOG_LV6, _T("\n\n"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("\tInterval : 1 minute,	CONNECT_SWITCH Process"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("%30s %20s %20s %20s"), _T("TIME"), _T("Count"), _T("Elapsed"), _T("Error"));
		itor = m_kStatistic1m.begin();
		while (itor != m_kStatistic1m.end())
		{
			if (itor->eStatus == ECS_TRYCONNECTSWITCH)
			{
				MakeTimeString(&(itor->tTime), 1, chTime, 40);
				S_STATE_LOG(BM::LOG_LV6, _T("%30s %20hd %20u %20hd"), chTime, itor->sCount, itor->ulElapsedTime, itor->sErrorCount);
			}
			++itor;
		}
		S_STATE_LOG(BM::LOG_LV6, _T("\n\n"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("\tInterval : 1 minute,	RECV_CHARACTERLIST Process"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("%30s %20s %20s %20s"), _T("TIME"), _T("Count"), _T("Elapsed"), _T("Error"));
		itor = m_kStatistic1m.begin();
		while (itor != m_kStatistic1m.end())
		{
			if (itor->eStatus == ECS_RECV_CHARACTERLIST)
			{
				MakeTimeString(&(itor->tTime), 1, chTime, 40);
				S_STATE_LOG(BM::LOG_LV6, _T("%30s %20hd %20u %20hd"), chTime, itor->sCount, itor->ulElapsedTime, itor->sErrorCount);
			}
			++itor;
		}
		S_STATE_LOG(BM::LOG_LV6, _T("\n\n"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("\tInterval : 1 minute,	ENTER_MAPSERVER Process"));
		S_STATE_LOG(BM::LOG_LV6, _T("============================================================================="));
		S_STATE_LOG(BM::LOG_LV6, _T("%30s %20s %20s %20s"), _T("TIME"), _T("Count"), _T("Elapsed"), _T("Error"));
		itor = m_kStatistic1m.begin();
		while (itor != m_kStatistic1m.end())
		{
			if (itor->eStatus == ECS_REQ_ENTERMAP)
			{
				MakeTimeString(&(itor->tTime), 1, chTime, 40);
				S_STATE_LOG(BM::LOG_LV6, _T("%30s %20hd %20u %20hd"), chTime, itor->sCount, itor->ulElapsedTime, itor->sErrorCount);
			}
			++itor;
		}

	}
	*/
}

unsigned long PgClientMng::DoLogin(CLIENT_INFO* pkClient)
{
	pkClient->eStatus = ECS_TRYLOGIN;

	const TEST_INFO* pkTestInfo = g_kNetwork.GetTestInfo();
	SClientTryLogin kLogin;
//	kLogin.guidVersion = BM::GUID();
//	kLogin.SetID(pkClient->chID);
//	kLogin.SetPW(pkTestInfo->chPassword);

	BM::Stream kLPacket(PT_C_L_TRY_LOGIN, kLogin);
	
	g_kNetwork.SendToLogin(pkClient->kSessionKey, kLPacket);

	return 20000;
}

void PgClientMng::LoginResult(CEL::CSession_Base* pkSession, ETryLoginResult eResult)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}
	unsigned long ulNow = ::timeGetTime();

	if (eResult == E_TLR_SUCCESS)
	{
		kClient.m_pkObject->dwLastTickTime = 0;
		kClient.m_pkObject->eStatus = ECS_LOGINED;
	}
	else
	{
		pkSession->VTerminate();
//		kClient.m_pkObject->kSessionKey = BM::GUID();
		kClient.m_pkObject->dwLastTickTime = ulNow;	// Retry Next Interval
		kClient.m_pkObject->eStatus = ECS_DISCONNECTED;
	}

}

void PgClientMng::LoginTimeOut(CLIENT_INFO* pkClient)
{
	unsigned long ulNow = ::timeGetTime();
	pkClient->eStatus = ECS_TRYLOGIN;
	CollectData(pkClient, ulNow, false);

	g_kCoreCenter.Terminate(pkClient->kSessionKey);
//	pkClient->kSessionKey = BM::GUID();
	pkClient->eStatus = ECS_DISCONNECTED;
	g_kCoreCenter.Terminate(pkClient->kSessionKey);
}

void PgClientMng::ReservedSwitchInfo(CEL::CSession_Base* pkSession, SSwitchReserveResult* pkResult)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}
	
	kClient.m_pkObject->eStatus = ECS_LOGINED;
	CollectData(kClient.m_pkObject, ::timeGetTime(), true);

	kClient.m_pkObject->eStatus = ECS_SWITCHRESERVED;
	kClient.m_pkObject->kSwitchAddr = pkResult->addrSwitch;
	kClient.m_pkObject->kSwitchGuid = pkResult->guidSwitchKey;
	kClient.m_pkObject->dwLastTickTime = 0;
}

void PgClientMng::ConnectedSwitch(CEL::CSession_Base* pkSession, unsigned long ulNow)
{
	const bool bIsSucc = pkSession->IsAlive();

	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}
	
	kClient.m_pkObject->eStatus = ECS_TRYCONNECTSWITCH;
	CollectData(kClient.m_pkObject, ulNow, bIsSucc);

	if (bIsSucc)
	{
		kClient.m_pkObject->dwLastTickTime = ulNow;		// Do Next Step Next Tick
		kClient.m_pkObject->eStatus = ECS_CONNECTEDSWITCH;
	}
	else
	{
		kClient.m_pkObject->eStatus = ECS_DISCONNECTED;
	}
}

void PgClientMng::ConnectSwitchTimeOut(CLIENT_INFO* pkClient)
{
	unsigned long ulNow = ::timeGetTime();
	pkClient->eStatus = ECS_TRYCONNECTSWITCH;
	pkClient->dwLastTickTime = ulNow;
	CollectData(pkClient, ulNow, false);
	
	RestartClient(pkClient);
}

void PgClientMng::TryAccessSwitch(CLIENT_INFO* pkClient)
{
	pkClient->eStatus = ECS_TRYACCESSSWITCH;
	STryAccessSwitch kAccess;
//	kAccess.guidVersion = BM::GUID();
//	kAccess.SetID(pkClient->chID);
//	kAccess.SetPW(g_kNetwork.GetTestInfo()->chPassword);
	kAccess.guidSwitchKey = pkClient->kSwitchGuid;

	BM::Stream kAPacket(PT_C_S_TRY_ACCESS_SWITCH, kAccess);
	g_kNetwork.SendToSwitch(pkClient->kSessionKey, kAPacket);
}

void PgClientMng::ReqCharacterList(CEL::CSession_Base* pkSession, ETryLoginResult eRet)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}
	if (eRet != E_TLR_SUCCESS)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] TraAccessSwitch Failure UserID[%s], ReturnCode[%d]"), __FUNCTIONW__, kClient.m_pkObject->chID, eRet);
		RestartClient(kClient.m_pkObject);
		return;
	}
	kClient.m_pkObject->eStatus = ECS_REQ_CHARACTERLIST;
	BM::Stream kCPacket(PT_C_S_REQ_CHARACTER_LIST);
	g_kNetwork.SendToSwitch(kClient.m_pkObject->kSessionKey, kCPacket);
}

/*
void PgClientMng::AnsCharacterList(CEL::CSession_Base* pkSession, SPlayerDBSimpleData* pkSimple)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}
	
	kClient.m_pkObject->eStatus = ECS_RECV_CHARACTERLIST;
	CollectData(kClient.m_pkObject, ::timeGetTime(), true);
	kClient.m_pkObject->kCharacterGuid = pkSimple->guidCharacter;
	kClient.m_pkObject->dwLastTickTime = ::timeGetTime();
}
*/

void PgClientMng::DoSelectCharacter(CLIENT_INFO* pkClient)
{
	pkClient->eStatus = ECS_SELECTEDCHARACTER;
	pkClient->dwLastTickTime = ::timeGetTime() + 30000;

	BM::Stream kSPacket(PT_C_S_REQ_SELECT_CHARACTER, pkClient->kCharacterGuid);
	g_kNetwork.SendToSwitch(pkClient->kSessionKey, kSPacket);
}

void PgClientMng::AnsSelectCharacter(CEL::CSession_Base* pkSession)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}

	kClient.m_pkObject->eStatus = ECS_RECV_CHARACTERLIST;
//	CollectData(kClient.m_pkObject, ::timeGetTime(), !((kClient.m_pkObject->kSessionKey)==BM::GUID()));

	// PT_C_S_REQ_ENTER_MAP_SERVER
	SReqSelectMapServer kRSMS;
	kRSMS.iMapNo = 6;

	kClient.m_pkObject->eStatus = ECS_REQ_ENTERMAP;
	kClient.m_pkObject->dwLastTickTime = ::timeGetTime() + 30000;
/*
	BM::Stream kMPacket(PT_C_S_REQ_ENTER_MAP_SERVER, kRSMS);
	g_kNetwork.SendToSwitch(kClient.m_pkObject->kSessionKey, kMPacket);
*/
}

void PgClientMng::SelectCharacterTimeOut(CLIENT_INFO* pkClient)
{
	unsigned long ulNow = ::timeGetTime();
	pkClient->eStatus = ECS_REQ_ENTERMAP;
	pkClient->dwLastTickTime = ulNow;
	CollectData(pkClient, ulNow, false);
	
	RestartClient(pkClient);
}

void PgClientMng::AnsReservePlayer(CEL::CSession_Base* pkSession, SAnsReservePlayer* pkARP)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}

	unsigned long ulNow = ::timeGetTime();
	kClient.m_pkObject->eStatus = ECS_REQ_ENTERMAP;
	CollectData(kClient.m_pkObject, ulNow, pkARP->eRet==RPR_SUCCESS);
	kClient.m_pkObject->eStatus = ECS_MAP_ENTERED;
	kClient.m_pkObject->dwLastTickTime = ulNow;
}

void PgClientMng::ReqEnterMapTimeOut(CLIENT_INFO* pkClient)
{
	unsigned long ulNow = ::timeGetTime();
	pkClient->eStatus = ECS_REQ_ENTERMAP;
	pkClient->dwLastTickTime = ulNow;
	CollectData(pkClient, ulNow, false);
	
	RestartClient(pkClient);
}

void PgClientMng::RestartClient(CLIENT_INFO* pkClient)
{
	if(!pkClient->kSessionKey.IsEmpty())
	{
		EClientStatus eOld = pkClient->eStatus;
		pkClient->eStatus = ECS_DISCONNECTED;
		if (eOld < ECS_LOGINED)
		{
			g_kCoreCenter.Terminate(pkClient->kSessionKey);
		}
		else
		{
			g_kCoreCenter.Terminate(pkClient->kSessionKey);
		}
		pkClient->kSessionKey.Clear();
	}

	unsigned long ulNow = ::timeGetTime();
	pkClient->eStatus = ECS_DISCONNECTED;
	pkClient->dwLastTickTime = ulNow;
	pkClient->dwTimeCounter = ulNow;
}

void PgClientMng::DisconnectedLogin(CEL::CSession_Base* pkSession)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
		return;
	}
	if (kClient.m_pkObject->eStatus <= ECS_TRYLOGIN)
	{
		kClient.m_pkObject->kSessionKey.Clear();
		RestartClient(kClient.m_pkObject);
	}
}

void PgClientMng::DisconnectedSwitch(CEL::CSession_Base* pkSession)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
		return;
	}
	if (kClient.m_pkObject->eStatus != ECS_DISCONNECTED)
	{
		kClient.m_pkObject->kSessionKey.Clear();
		RestartClient(kClient.m_pkObject);
	}
}

void PgClientMng::ReqMapLoaded(CLIENT_INFO* pkClient)
{
	pkClient->eStatus = ECS_REQ_MAP_LOADED;
	pkClient->dwLastTickTime = ::timeGetTime();

	BM::Stream kOPacket(PT_C_M_NFY_MAPLOADED);
	g_kNetwork.SendToSwitch(pkClient->kSessionKey, kOPacket);
}

void PgClientMng::NfyMapLoaded(CEL::CSession_Base* pkSession)
{
	ObjectLock<CLIENT_INFO*> kClient;
	GetClient(pkSession->SessionKey(), &kClient);
	if (kClient.m_pkObject == NULL)
	{
//		INFO_LOG(BM::LOG_LV4, _T("[%s] Cannot find Client SessionGuid[%s]"), __FUNCTIONW__, pkSession->SessionKey().str().c_str());
		return;
	}

	kClient.m_pkObject->eStatus = ECS_MAP_LOADED;
	kClient.m_pkObject->dwLastTickTime = ::timeGetTime();
}