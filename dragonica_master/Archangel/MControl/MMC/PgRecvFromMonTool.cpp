#include "stdafx.h"
#include "PgMMCManager.h"
#include "PgRecvFromMonTool.h"

void CALLBACK OnAcceptFromMonTool(CEL::CSession_Base *pkSession)
{
	//여기는 <중앙> 터미널로 암호키(PT_A_ENCRYPT_KEY)를 보내자
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Send EncryptKey To MonTool") );

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Get EncryptKey Failed. Session Terminate!") );
		pkSession->VTerminate();
	}
}

void CALLBACK OnDisConnectToMonTool(CEL::CSession_Base *pkSession)
{
	g_kMMCMgr.Locked_LogOutMonTool( pkSession );
	INFO_LOG( BM::LOG_LV3, __FL__ << _T("MonTool Disconnect [") << pkSession->Addr().ToString() << _T("]") );
}

void CALLBACK OnRecvFromMonTool(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	switch ( usType )
	{
	case PT_A_S_REQ_GREETING:
		{
			//여기는 <중앙> 터미널로 부터 인사가 왔다.-> 주석이 뭐 이래 -_-;
			std::wstring kID, kPW;
			pkPacket->Pop(kID);
			pkPacket->Pop(kPW);

			BYTE const byGrant = g_kMMCMgr.Locked_LoginMonTool( pkSession, kID, kPW );
			if ( byGrant )
			{
				SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();

				BM::Stream kPacket(PT_A_S_ANS_GREETING);
				kSendSI.WriteToPacket(kPacket);

				kPacket.Push( byGrant == USE_COMMAND_LEVEL );

				std::wstring wstrPatchVer;
				g_kMMCMgr.Locked_GetPatchVersion(wstrPatchVer);
				kPacket.Push(wstrPatchVer);

				pkSession->VSend( kPacket );
			}
			else
			{
				INFO_LOG(BM::LOG_LV3, __FL__ << _T("invalid connect from [") << pkSession->Addr().ToString() << _T("]") );

				BM::Stream kPacket( PT_ANS_MMC_TOOL_CMD_RESULT );
				int iErrorCode = 2;
				kPacket.Push(iErrorCode);
				pkSession->VSend(kPacket);
				pkSession->VTerminate();//끊자
			}
		}break;
	case PT_REQ_TOOL_MON_CMD:
		{
			EMMC_CMD_TYPE eCmdType;
			pkPacket->Pop(eCmdType);
			g_kMMCMgr.Locked_RecvMonToolCmd(pkSession, eCmdType, pkPacket);//커멘드와 응답은 별개.
		}break;
	}
}