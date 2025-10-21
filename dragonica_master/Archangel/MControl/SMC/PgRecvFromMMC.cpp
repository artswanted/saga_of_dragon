#include "stdafx.h"
#include "PgSMCMgr.h"
#include "PgRecvFromMMC.h"
#include "PgSendWrapper.h"
#include "PgLogCopyer.h"

void CALLBACK OnConnectToMMC(CEL::CSession_Base *pkSession)
{
	bool const bIsSucc = pkSession->IsAlive();//서브()
	if( bIsSucc )
	{
		BM::CAutoMutex kLock( g_kMutex );
		if ( !g_pkMMCSession )
		{
			g_pkMMCSession = pkSession;
		}
	}
	else
	{
		std::wcout << L"Connect To MMC Failed. Connect Retry!" << std::endl;
		g_kSMCMgr.OnDisconnectMMC();
	}
}

void CALLBACK OnDisConnectToMMC(CEL::CSession_Base *pkSession)
{
	bool bDisconnect = false;

	{
		BM::CAutoMutex kLock( g_kMutex );
		if ( g_pkMMCSession == pkSession )
		{
			std::wcout << L"OnDisConnectToMMC" << std::endl;
			g_pkMMCSession = NULL;
			bDisconnect = true;
		}
	}
	
	if ( true == bDisconnect )
	{
		g_kSMCMgr.OnDisconnectMMC();
	}
}

void CALLBACK OnRecvFromMMC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	unsigned short usType = 0;
	pkPacket->Pop(usType);
	
	switch(usType)
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);
			
			short int RunMode = 0;
			pkPacket->Pop(RunMode);
			g_kProcessCfg.RunMode(RunMode);

			g_kSMCMgr.Recv_PT_A_S_ANS_GREETING( pkSession, pkPacket );
		}break;
	case PT_MMC_SMC_NFY_CMD:
		{
			EMMC_CMD_TYPE eCmdType;
			CEL::SESSION_KEY kCmdOwner;
			pkPacket->Pop(eCmdType);
			pkPacket->Pop(kCmdOwner);
			
			g_kSMCMgr.ProcessCmdMMCOrder(pkSession, kCmdOwner, eCmdType, pkPacket);
		}break;
	case PT_MMC_SMC_ANS_DATA_SYNC_INFO:
		{
			g_kSMCMgr.Recv_PT_MMC_SMC_ANS_DATA_SYNC_INFO(pkSession, pkPacket);
		}break;
	case PT_MMC_SMC_ANS_GET_FILE:
		{
			std::wstring strFileName;
			std::vector<char> kFileData;
			pkPacket->Pop(strFileName);
			pkPacket->Pop(kFileData);

			INFO_LOG(BM::LOG_LV2, _T("Recv File [") << strFileName.c_str() << _T("]") );

			if(!BM::MemToFile(strFileName, kFileData))
			{
				//싱크 못헀으면 로그찍고 그냥 넘어감.
				VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("Sync Failed [") << strFileName.c_str() << _T("]") );
			}
			g_kSMCMgr.ProcessDownload(pkSession);
		}break;
	case PT_SMC_MMC_REQ_GET_FILE:
		{
			std::wstring kFileName;
			pkPacket->Pop(kFileName);

			std::vector< char > kFileData;

			g_kLogCopyer.GetFile( kFileName, kFileData );

			BM::Stream kPacket(PT_MMC_SMC_ANS_GET_FILE);
			kPacket.Push(kFileName);
			kPacket.Push(kFileData);
			pkSession->VSend(kPacket);
		} // no break !!!!!!!!!!!
	case PT_SMC_MMC_ANS_DATA_SYNC_END:
		{
			std::wstring wstrOldFile;
			pkPacket->Pop(wstrOldFile);

			g_kLogCopyer.DeleteFile( wstrOldFile );
		}break;
	case PT_MCTRL_MMC_SMC_NFY_PROCESSID:
		{
			g_kSMCMgr.Recv_PT_MCTRL_MMC_SMC_NFY_PROCESSID( pkPacket );
		}break;
	case PT_MMC_SMC_GET_LOG_FILE_INFO:
		{
			size_t iSize = 0;
			pkPacket->Pop( iSize );

			while ( iSize-- )
			{
				std::wstring wstrPath;
				pkPacket->Pop( wstrPath );

				PgLogCopyer::CONT_LOG_FILE_INFO kContFileInfo;
				g_kLogCopyer.GetFileList( wstrPath, kContFileInfo );
				if(kContFileInfo.empty())
				{
					return;
				}

				BM::Stream kPacket( PT_SMC_MMC_ANS_LOG_FILE_INFO, wstrPath );
				PU::TWriteTable_AA( kPacket, kContFileInfo);
				pkSession->VSend( kPacket );
			}
		}break;
/*
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			EMCtrlCommand eCmd;
			pkPacket->Pop(eCmd);

			if(eCmd == MCC_Heart_Beat)
			{
				SERVER_IDENTITY kSI, MySI;
				MySI = g_kProcessCfg.ServerIdentity();
				kSI.ReadFromPacket(*pkPacket);

				if(MySI == kSI)
				{
					BM::Stream kPacket(PT_MCTRL_A_MMC_ANS_SERVER_COMMAND, IMCT_STATE_NFY);
					kSI.WriteToPacket(kPacket);
					kPacket.Push(true);		//bIsConnect
					kPacket.Push(CMiniDump2::IsExcepted());		//bIsExecption
					SendToMMC(kPacket);
				}
			}
		}break;
*/
/*	case PT_MMC_SMC_ANS_DATA_SYNC_END:
		{
			int iRet = 0;
			pkPacket->Pop(iRet);

			if(iRet)
			{//실패. 싱크 재시도
				CAUTION_LOG(BM::LOG_LV0, _T("Sync Process Failed. Retry"));
				g_kSMCMgr.Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO(true);
			}
			else
			{//성공. 레디.
				INFO_LOG(BM::LOG_LV1, _T("Sync Process Success.."));
				BM::Stream kPacket(PT_SMC_MMC_ANS_DATA_SYNC_END_OK);
				pkSession->VSend(kPacket);
			}
		}break;
*/
	default:
		{
		}break;
	}
}

void PgSMCMgr::Recv_PT_MMC_SMC_ANS_DATA_SYNC_INFO(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	INFO_LOG(BM::LOG_LV2, _T("Recv Sync Info.... ") );
	INFO_LOG(BM::LOG_LV2, _T("SMC Folder Check Start... Plz Wait ..."));
	
	m_kNeedDownload.clear();

	bool IsSMC_Sync = false;
	pkPacket->Pop(IsSMC_Sync);

	g_kSMCMgr.SMC_Sync(IsSMC_Sync);

	CON_SYNC_PATH kContPath;
	PU::TLoadTable_MM(*pkPacket, kContPath);

	CON_SYNC_PATH::const_iterator path_itor = kContPath.begin();
	while(kContPath.end() != path_itor)
	{
		INFO_LOG(BM::LOG_LV2, _T("Check [") << (*path_itor).second.kSrcPath.c_str() << _T("] Start") );
		////////////// 패킷 처리
		BM::FolderHash kMMCFolders;
		PU::TLoadTable_AM(*pkPacket, kMMCFolders);//받은걸로 내꺼와 비교
		
		//본인것과 비교
		BM::CPackInfo kPackInfo;
		kPackInfo.Init();
		
		BM::PgDataPackManager kPackMgr;
		kPackMgr.Init(kPackInfo);

		BM::FolderHash kLocalFolders;
		kPackMgr.GetFileList((*path_itor).second.kSrcPath, kLocalFolders);

//받을 목록 뽑기.
		BM::FolderHash kResultHash;
		kPackMgr.MakeDiffList(kMMCFolders, kLocalFolders, kResultHash, BM::DIFF_ONLY_LEFT | BM::DIFF_ONLY_DIFF);

		m_kNeedDownload.insert( std::make_pair((*path_itor).second.kSrcPath, kResultHash));

		INFO_LOG(BM::LOG_LV2, _T("Check [") << (*path_itor).second.kSrcPath.c_str() << _T("] Complete") );
		++path_itor;
	}

	ProcessDownload(pkSession);
}