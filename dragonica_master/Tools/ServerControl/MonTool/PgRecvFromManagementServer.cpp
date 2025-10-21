#include "stdafx.h"
#include "PgNetwork.h"
#include "PgServerStateDoc.h"
#include "GeneralDlg.h"
#include "PgRecvFromManagementServer.h"
#include "PgSendWrapper.h"
#include "ServerControlDlg.h"
#include "PgTimer.h"
#include "PgLogCopyMgr.h"
#include "PgMCTTask.h"
#include "PgServerCmdMgr.h"

void CALLBACK OnConnectToMMC(CEL::CSession_Base *pkSession)
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
//		pkSession->SessionKey();
		g_kLogCopyMgr.Locked_Connect(pkSession->SessionKey());
	}
	else
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Connect To MMC Failed.!") );
		g_kNetwork.IsConnect(false);
		MessageBox(NULL, _T("Can't Connect Server"), _T("ERROR"), MB_OK);
		
		g_kMainDlg.CallLoginDlg();
//		g_kMainDlg.Close();//무조건 종료 
	}
}

void CALLBACK OnDisconnectFromMMC(CEL::CSession_Base *pkSession)
{//!  세션락
//	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	g_kGameServerMgr.Clear();
	g_kMainDlg.Clear();
	g_kNetwork.IsConnect(false);

	if ( true == g_kNetwork.IsServiceStart() )
	{
		g_kMainDlg.CallLoginDlg(); 
	}
	else
	{
		g_kLogWorker.VDeactivate();
		g_kCoreCenter.Close();
	}
}

void CALLBACK OnRecvFromMMC(CEL::CSession_Base *pkSession, BM::CPacket * const pkPacket)
{
	static int i = 0;
	//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] 패킷 받음 : %d, %d"), _T(__FUNCTION__), __LINE__, i++, pkPacket->Size());

	unsigned short usType = 0;
	pkPacket->Pop(usType);
	
	switch(usType)
	{
	case PT_A_ENCRYPT_KEY:
		{// 중앙에서 암호키<PT_A_ENCRYPT_KEY>가 도착 했다
			std::vector<char> kKey;
			pkPacket->Pop(kKey);
			
			if(S_OK != pkSession->VSetEncodeKey(kKey))
			{
				INFO_LOG( BM::LOG_LV0, __FL__ << _T("Incorect EncodeKey Session Terminate") );
				pkSession->VTerminate();
			}
			else
			{//인사를 하자.
				INFO_LOG(BM::LOG_RED, _T("SEND GREETING"));

				BM::CPacket kPacket(PT_A_S_REQ_GREETING);
				kPacket.Push(g_kMainDlg.m_wstrID);
				kPacket.Push(g_kMainDlg.m_wstrPW);
				pkSession->VSend(kPacket);

				g_kMainDlg.m_wstrID.clear();
				g_kMainDlg.m_wstrPW.clear();

				SYSTEMTIME kNowTime;
				::GetLocalTime( &kNowTime );

				g_wNextRecordMinTime = 0;
				while ( g_wNextRecordMinTime <= kNowTime.wMinute )
				{
					g_wNextRecordMinTime += g_kGameServerMgr.RecordCCUTimeMin();
					if ( g_wNextRecordMinTime > 59 )
					{
						g_wNextRecordMinTime = 0;
						break;
					}
				}
			}
		}break;
	case PT_A_S_ANS_GREETING:
		{// 중앙 머신컨트롤러로 부터 응답이 왔다.
			//중앙 머신 컨트롤러부터 Greeting 인사를 받았다.-> 주석이 ㅋㅋ
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			bool bControl = false;
			pkPacket->Pop(bControl);

			std::wstring wstrPatchVer;
			pkPacket->Pop(wstrPatchVer);
			g_kMainDlg.SetTitleText(wstrPatchVer);
//			g_kGameServerMgr.ReadFromPacketSMCInfo(pkPacket);

			if( SetSendWrapper(kRecvSI) )
			{		
				g_kGameServerMgr.bControlLevel(bControl);
				g_kServerCmdMgr.OnCommand(MCT_REFRESH_LIST);

				g_kNetwork.IsConnect(true);
				g_kMainDlg.SetControlBtn();
				g_kMainDlg.ShowWindow(SW_SHOW);		
			}
		}break;
	case PT_MMC_TOOL_NFY_INFO:
		{//! Tool의 커맨드 결과 - STATE / LOG / ETC... REFRESH 
			EMMC_CMD_TYPE eCmdType;
			pkPacket->Pop(eCmdType);
			g_kServerCmdMgr.ProcessInfoCmd(eCmdType, pkSession, pkPacket);
		}break;
	case PT_ANS_MON_TOOL_CMD:
		{//커멘드 결과.(갱신요청)
			g_kServerCmdMgr.OnCommand(MCT_REFRESH_STATE);
		}break;
	case PT_ANS_MMC_TOOL_CMD_RESULT:
		{// 에러
			int iErrorCode = 0;
			std::wstring kErrorMsg;
			pkPacket->Pop(iErrorCode);
			switch (iErrorCode)
			{
			case 1:
				{
					kErrorMsg = _T("Can`t Use ServerControlTool!!\nNow ServerData Sync");
				}break;
			case 2:
				{
					kErrorMsg = _T(" Can`t Use ServerControlTool!!\nCheck Your Gm Level");
				}break;
			default:break;
			}
			INFO_LOG( BM::LOG_LV2, kErrorMsg.c_str() );
			MessageBox(NULL, kErrorMsg.c_str(), NULL, 0);
		}break;
	default:
		{
		}break;
	}
}

