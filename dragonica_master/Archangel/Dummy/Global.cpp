#include "stdafx.h"
#include "Global.h"
#include "ai_Action.h"
#include "DummyClientMgr.h"
// #include "PgClientMng.h"
/*
STICK_DELAY_TIME g_DelayTime[] = {	{ECS_TRYCONNECT, 0}, {ECS_LOGINED, 0}, {ECS_TRYCONNECTSWITCH, 0 },
		{ECS_RECV_CHARACTERLIST, 0}, {ECS_REQ_ENTERMAP, 0}, {ECS_NONE, 0}	};
*/
void CALLBACK TimerTick50msec(DWORD dwUserData)
{
	// g_kDummyMgr.DummyProc();
/*	static int iEndThread = g_kNetwork.GetTestInfo()->byTimer1s;
	unsigned long ulNow = ::timeGetTime();
	int iEnd = (dwUserData == iEndThread-1) ? (dwUserData+2) * g_ClientMng.GetThreadClientNum() : (dwUserData+1) * g_ClientMng.GetThreadClientNum();
	for (int iIndex = dwUserData * g_ClientMng.GetThreadClientNum(); iIndex < iEnd; iIndex++)
	{
		if ( !g_ClientMng.GetClient(iIndex, &kClient) )
		{
			break;
		}
		//INFO_LOG(BM::LOG_LV8, _T("TimerTick1s...UserData[%d], UserIndex[%d]"), dwUserData, iIndex);
		
		if (kClient.m_pkObject->dwLastTickTime+g_kNetwork.GetTestInfo()->ulInterval < ulNow)
		{
			unsigned long ulDelay = 0;
			switch (kClient.m_pkObject->eStatus)
			{
			case ECS_DISCONNECTED:
				{
					g_kNetwork.DoConnectLogin(kClient.m_pkObject);
				}break;
			case ECS_TRYCONNECT:
				{
					g_ClientMng.LoginConnectTimeOut(kClient.m_pkObject);
				}break;
			case ECS_CONNECTED:
				{
					ulDelay = g_ClientMng.DoLogin(kClient.m_pkObject);
				}break;
			case ECS_TRYLOGIN:
				{
					g_ClientMng.LoginTimeOut(kClient.m_pkObject);
				}break;
			case ECS_TRYCONNECTSWITCH:
				{
					g_ClientMng.ConnectSwitchTimeOut(kClient.m_pkObject);
				}break;
			case ECS_SWITCHRESERVED:
				{
					g_kNetwork.DoConnectSwitch(kClient.m_pkObject);
				}break;
			case ECS_CONNECTEDSWITCH:
				{
					g_ClientMng.TryAccessSwitch(kClient.m_pkObject);
				}break;
			case ECS_RECV_CHARACTERLIST:
				{
					g_ClientMng.DoSelectCharacter(kClient.m_pkObject);
				}break;
			case ECS_SELECTEDCHARACTER:
				{
					g_ClientMng.SelectCharacterTimeOut(kClient.m_pkObject);
				}break;
			case ECS_REQ_ENTERMAP:
				{
					g_ClientMng.ReqEnterMapTimeOut(kClient.m_pkObject);
				}break;
			case ECS_MAP_ENTERED:
				{
					g_ClientMng.ReqMapLoaded(kClient.m_pkObject);
				}break;
			case ECS_MAP_LOADED:
				{
					if(g_kNetwork.GetTestInfo()->eTestType == ETEST_LOGIN)
					{
						g_ClientMng.RestartClient(kClient.m_pkObject);
					}
					kClient.m_pkObject->dwLastTickTime = ulNow + ulDelay;
				}break;
			}
		}
	}
*/
}

// 통계를 만들어 보자
void CALLBACK TimerTick1m(DWORD dwUserData)
{
	// g_ClientMng.MakeStatistic1m();
}

//void MakeTimeString(unsigned long ulTime, int iOutType, LPTSTR lpszOut, int iOutLen)
void MakeTimeString(time_t* pkTime, int iOutType, LPTSTR lpszOut, int iOutLen)
{
	//__time32_t kTime = _time32((__time32_t*)&ulTime);
	//struct tm* pkTM = _localtime32(&kTime);
	//struct tm* pkTM = _localtime32((__time32_t*)&ulTime);
	struct tm kTM;
	localtime_s(&kTM, pkTime);
	switch (iOutType)
	{
	case 1:
		_tcsftime(lpszOut, iOutLen, _T("%m/%d %H:%M:%S"), &kTM);
		break;
	case 2:
		_tcsftime(lpszOut, iOutLen, _T("%Y %m/%d %H:%M:%S"), &kTM);
		break;
	}
}

// [RETURN]
//	example : _T("C:\MyFolder\DummyClient_Folder")
LPCTSTR GetMainDirectory()
{
	static TCHAR chMainPath[MAX_PATH] = { _T('\0'), };
	if (chMainPath[0] == _T('\0'))
	{
		GetModuleFileName(NULL, chMainPath, MAX_PATH);
		PathRemoveFileSpec(chMainPath);
	}
	return chMainPath;
}

