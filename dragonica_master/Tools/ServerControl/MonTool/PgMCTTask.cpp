#include "stdafx.h"
#include "PgMCTTask.h"
#include "PgLogCopyMgr.h"

PgTask_MCT::~PgTask_MCT()
{
	Close();
}

void PgTask_MCT::Close()
{
	this->VDeactivate();
}

void PgTask_MCT::HandleMessage(SEventMessage *pkMsg)
{
	BM::CPacket* pkPacket = NULL;
	pkMsg->Pop(pkPacket);

	switch(pkMsg->PriType())
	{
	case PMET_MMC_ANS_LOG_FILE_INFO:
		{//! MMC에 요청한 [로그파일정보] 패킷을 받았다.
			g_kLogCopyMgr.Locked_Recv_PT_MMC_MCT_LOG_FILE_INFO(pkPacket);
		}break;
	case PMET_MMC_ANS_GET_FILE:
	case PMET_SMC_ANS_GET_FILE:
		{//! MMC/SMC 로부터 [파일]을 받았다.
			g_kLogCopyMgr.Locked_Recv_PT_MMC_MCT_ANS_GET_FILE(pkPacket);
		}break;
	case PMET_SMC_ANS_LOG_FILE_INFO:
		{ //! SMC에 요청한 [로그파일정보] 패킷을 받았다. ( 물론 MMC 거쳐서 오는것임 )
			g_kLogCopyMgr.Locked_Recv_PT_SMC_MCT_LOG_FILE_INFO(pkPacket);
		}break;
	default:
		{
			//VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"invalid Task primary type["<<pkMsg->PriType()<<L"]");
			//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}
