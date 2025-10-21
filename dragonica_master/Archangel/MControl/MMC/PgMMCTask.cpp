#include "stdafx.h"
#include "PgMMCTask.h"
#include "PgMMCManager.h"
#include "PgFileCopyMgr.h"

void PgTask_MMC::Close()
{
	this->VDeactivate();
}

void PgTask_MMC::HandleMessage(SEventMessage *pkMsg)
{
	switch(pkMsg->PriType())
	{
	case PMET_MMC_MANAGER:
		{
			switch ( pkMsg->SecType() )
			{
			case PT_MMC_LOADPATCHFILE:
				{
					bool bInit = false;
					pkMsg->Pop( bInit );
					g_kMMCMgr.Locked_LoadPatchFileList( bInit );
				}break;
			}
		}break;
	case PMET_FILE_COPY_MANAGER:
		{
			CEL::SESSION_KEY kSessionKey;
			pkMsg->Pop( kSessionKey );

			switch ( pkMsg->SecType() )
			{
			case PT_SMC_MMC_REQ_GET_FILE:
				{
					g_kMMCMgr.Locked_GetFile( kSessionKey, pkMsg );
				}break;
			case PT_SMC_MMC_ANS_LOG_FILE_INFO:
				{//! SMC --[Log Info]--> MMC
					g_kLogCopyMgr.Locked_Recv_PT_SMC_MMC_ANS_LOG_FILE_INFO( kSessionKey, pkMsg );
				}break;
			case PT_MMC_SMC_ANS_GET_FILE:
				{//! MMC --[File Data]--> SMC
					g_kLogCopyMgr.Locked_Recv_PT_MMC_SMC_ANS_GET_FILE( kSessionKey, pkMsg );
				}break;
			case PT_MCT_REFRESH_LOG:
				{
					g_kLogCopyMgr.Locked_GetLogInfo( kSessionKey, pkMsg);
				}break;
			case PT_MCT_REQ_GET_FILE_INFO:
				{//! MCT --[Log Info]--> MMC
					g_kLogCopyMgr.Locked_PT_MCT_REQ_GET_FILE_INFO( kSessionKey, pkMsg );
				}break;
			case PT_MCT_REQ_GET_FILE:
				{//! MCT --[File Data]--> MMC
					g_kLogCopyMgr.Locked_Recv_MCT_REQ_GET_FILE( kSessionKey, pkMsg );
				}break;
			}

		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"invalid Task primary type["<<pkMsg->PriType()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}
