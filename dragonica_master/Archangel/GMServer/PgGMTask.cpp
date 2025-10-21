#include "StdAfx.h"
#include "PgGMTask.h"
#include "PgGMUserMgr.h"
#include "PgMsgHolder.h"
#include "PgPetitionDataMgr.h"
#include "PgGmChatMgr.h"
#include "PgNcGmCommand.h"
#include "PgGMTask.h"
#include "PgSendWrapper.h"
#include "Variant/PgNoticeAction.h"
#include "Variant/PgEventView.h"
#include "PgNoticeMgr.h"

extern std::wstring g_GMTool_IP;

void GetBitToRealm(unsigned short usRealm, VEC_INT & kOut)
{
	if( 0 == usRealm)
	{
		return;
	}
	int const COUNT = sizeof(unsigned short) * CHAR_BIT;
	for(int i = 0; i<COUNT; ++i)
	{
		if(usRealm & 0x01)
		{
			kOut.push_back(i+1);
		}
		usRealm >>= 1;
	}
}

tagGMEventOrder::tagGMEventOrder(PgMsgQue *pkMsgQue)
{
	m_pkMsgQue = pkMsgQue;
	m_kMsgGuid = pkMsgQue->MsgGuid();
}

void tagGMEventOrder::Clear()
{
}

PgGMTask::PgGMTask(void)
	: m_pkGMToolSession(NULL)
{
	if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA))
	{
		m_pkLocalProcess = new PgNcGmCommand;
	}
}

PgGMTask::~PgGMTask(void)
{
	if (m_pkLocalProcess)
		delete m_pkLocalProcess;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void PgGMTask::RecvGMToolProcess(CEL::CSession_Base * const pkSession, BM::Stream * const pkPacket)
{
	EGMCommandType eCommand;
	BM::GUID kReqGuid;
	pkPacket->Pop(eCommand);

	switch (eCommand)
	{
	case EGMC_MATCH_GMID:
		{
			g_kGMUserMgr.WaitLogin(pkPacket);
		}break;
	case EGMC_GET_SERVER_LIST:
		{
			BM::Stream kPacket(PT_G_O_ANS_GMCOMMAND, eCommand);
			g_kProcessCfg.Locked_Write_ServerList(kPacket, 0);
		}break;
	case EGMC_GET_GMLIST:
		{
			pkPacket->Pop(kReqGuid);
			BM::Stream kPacket(PT_G_O_ANS_GMCOMMAND);
			kPacket.Push(kReqGuid);
			kPacket.Push(eCommand);
			g_kGMUserMgr.WriteToPacketGmUser(kPacket);
			g_kGMUserMgr.SendToLogin(kReqGuid, kPacket);
		}break;
	case EGMC_DELETE_GM:
		{
			g_kGMUserMgr.DeleteLoginedGM(pkPacket);			
		}break;
	case EGMC_NOTICE:
		{
			unsigned short usRealm;
			unsigned short usChannel;
			std::wstring wstrContent;

			pkPacket->Pop(usRealm);
			pkPacket->Pop(usChannel);
			pkPacket->Pop(wstrContent);

			PgNoticeAction kNotice;
			kNotice.SetNoticeData(1, usRealm, usChannel, 0, wstrContent);
			kNotice.Send(CEL::ST_IMMIGRATION);
		}break;
	case EGMC_GET_PETITIONDATA:
		{
			unsigned short usRealm = 0;
			BM::GUID kGmGuid;
			pkPacket->Pop(kGmGuid);
			pkPacket->Pop(usRealm);
			g_kPetitionDataMgr.SendPetitionDataToUser(kGmGuid, usRealm);
		}break;
	case EGMC_MODIFY_PETITIONDATA:
		{
			BM::GUID kReqGuid;
			pkPacket->Pop(kReqGuid);
			SPetitionData kModifyData;
			kModifyData.ReadFromPacket(*pkPacket);
			g_kPetitionDataMgr.UpdatePetitionData(kReqGuid, kModifyData);
		}break;
	case EGMC_SENDMSG_TO_GM:
		{
			// GM툴로 보내주자.
			BM::Stream kPacket(PT_G_O_ANS_GMCOMMAND, eCommand);
			kPacket.Push(*pkPacket);

			//로그 저장
			std::wstring wstrMsg;
			std::wstring wstrGmName;
			pkPacket->Pop(wstrGmName);
			pkPacket->Pop(wstrMsg);
			wstrGmName += L":" + wstrMsg;
			g_kGmChatMgr.WriteChatMsg(wstrGmName);
		}break;
	case EGMC_TOOL_REQ_ORDER:
		{
			SGmOrder kOrderData;
			pkPacket->Pop(kOrderData.usRealm);
			pkPacket->Pop(kOrderData.usChannel);
			pkPacket->Pop(kOrderData.kCmdGuid);
			pkPacket->Pop(kOrderData.Type);
			pkPacket->Pop(kOrderData.kGuid[0]);
			pkPacket->Pop(kOrderData.kGuid[1]);
			pkPacket->Pop(kOrderData.iValue[0]);
			pkPacket->Pop(kOrderData.iValue[1]);
			pkPacket->Pop(kOrderData.iValue[2]);
			pkPacket->Pop(kOrderData.iValue[3]);
			pkPacket->Pop(kOrderData.iValue[4]);
			pkPacket->Pop(kOrderData.biValue[0]);
			pkPacket->Pop(kOrderData.biValue[1]);
			pkPacket->Pop(kOrderData.biValue[2]);
			pkPacket->Pop(kOrderData.biValue[3]);
			pkPacket->Pop(kOrderData.biValue[4]);
			pkPacket->Pop(kOrderData.wsString1);
			pkPacket->Pop(kOrderData.wsString2);
			pkPacket->Pop(kOrderData.dtTime[0]);
			pkPacket->Pop(kOrderData.dtTime[1]);

			//공지면 공지 매니저로
			//SendOrderState(pkSession, kOrderData.kCmdGuid, OS_DOING);

			switch(kOrderData.Type)
			{
			case ORDER_DELETE_NOTICES:
				{
					g_kNoticeMgr.DeleteNoticeData(kOrderData);
				}break;
			case ORDER_NOTICES:
				{
					unsigned short const usRealm = kOrderData.usRealm;
					VEC_INT kCont;
					GetBitToRealm(usRealm, kCont);
					if(kCont.empty())
					{
						kOrderData.usRealm = 0;
						g_kNoticeMgr.SetNoticeData(kOrderData);
					}
					else
					{
						for(VEC_INT::const_iterator c_it=kCont.begin(); c_it!=kCont.end(); ++c_it)
						{
							kOrderData.usRealm = static_cast<unsigned short>(*c_it);
							g_kNoticeMgr.SetNoticeData(kOrderData);
						}
					}
					g_kGMTask.Locked_SendOrderState(kOrderData.kCmdGuid, OS_DOING);
				}break;
			default:
				{
					BM::Stream kPacket(PT_GM_A_REQ_GMORDER);
					kOrderData.WriteToPacket(kPacket);
					SendToImmigration(kPacket);
				}break;
			}
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unknown GMToolCommand received CommandType[") << (int)eCommand << _T("]") );
		}break;
	}
}

void PgGMTask::RecvGMLocaleProcess(BM::Stream * const pkPacket)
{
	if(m_pkLocalProcess)
	{
		m_pkLocalProcess->LocaleGMCommandProcess(pkPacket);
	}
}

void PgGMTask::GetNowTime(BM::DBTIMESTAMP_EX &rkOut)
{
	SYSTEMTIME kNowTime;
	g_kEventView.GetLocalTime(&kNowTime);
	rkOut = kNowTime;
}

bool PgGMTask::Locked_LoginGMTool(CEL::CSession_Base * const pkSession)
{
	if(NULL == pkSession)
	{
		return false;
	}

	BM::CAutoMutex kLock(m_kMutex);
	if( m_pkGMToolSession )
	{
		return false;
	}

	if( pkSession->Addr().IP() == g_GMTool_IP )
	{
		m_pkGMToolSession = pkSession;
		return true;
	}

	return false;
}

bool PgGMTask::Locked_LogOutGMTool( CEL::CSession_Base * const pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	if(m_pkGMToolSession == pkSession)
	{
		m_pkGMToolSession = NULL;
	}
	return true;
}

void PgGMTask::Locked_FailedOrder(BM::GUID const& kCmdGuid, E_GM_ERR const eErrorCode)const
{
	Locked_SendOrderState(kCmdGuid, OS_RESERVE, eErrorCode);
}

void PgGMTask::Locked_SendOrderState(BM::GUID const& kCmdGuid, EOrderState const eState, E_GM_ERR const eErrorCode)const
{
	BM::CAutoMutex kLock(m_kMutex);
	if(m_pkGMToolSession)
	{
		BM::Stream kPacket(EGMC_TOOL_ANS_ORDER);
		kPacket.Push(kCmdGuid);
		kPacket.Push(eState);
		kPacket.Push(eErrorCode);
		m_pkGMToolSession->VSend(kPacket);
	}
}

HRESULT PgGMWorker::VProcess(SGMEventOrder *pkMsg)
{
	bool bProcessRet = false;
	
	SGMEventOrder::CONT_WORKDATA_TARGET::const_iterator target_itor = pkMsg->m_kContTarget.begin();
	if(target_itor == pkMsg->m_kContTarget.end())
	{//이럴 수가 없기 때문.
		__asm int 3;
		return E_FAIL;
	}

	SGMEventOrder::CONT_WORKDATA_TARGET::value_type const &kStrAccountID = (*target_itor);

	BM::GUID const kMsgGuid = pkMsg->MsgGuid();
	
	switch(pkMsg->ePriMsgType)
	{
	case GOET_KICK_USER:
		{
			bProcessRet = m_pkLocalProcess->ReqKickUser(kMsgGuid, kStrAccountID.c_str());
		}break;
	case GOET_CHANGE_PW:
		{
			std::wstring kStrPW;
			pkMsg->Pop(kStrPW);	
			
			bProcessRet = m_pkLocalProcess->ReqChangePassWord(kMsgGuid, kStrAccountID.c_str(), kStrPW.c_str(), 0);
		}break;
	case GOET_CREATE_ACCOUNT:
		{	
			std::wstring kStrPW;
			std::wstring kStrBirth;
			pkMsg->Pop(kStrPW);	
			pkMsg->Pop(kStrBirth);	
			
			bProcessRet = m_pkLocalProcess->ReqCreateAccount(kMsgGuid, kStrAccountID.c_str(), kStrPW.c_str(), kStrBirth.c_str());
		}break;
	case GOET_FREEZE_ACCOUNT:
		{
			int iNewState = 0;
			pkMsg->Pop(iNewState);	
			
			bProcessRet = m_pkLocalProcess->ReqFreezeAccount(kMsgGuid, kStrAccountID.c_str(), iNewState, 0);
		}break;
	case GOET_ADD_POINT:
		{
			int iPoint = 0;
			pkMsg->Pop(iPoint);	
			
			bProcessRet = m_pkLocalProcess->ReqAddPoint(kMsgGuid, kStrAccountID.c_str(), iPoint);
		}break;
	case GOET_GET_POINT:
		{
			bProcessRet = m_pkLocalProcess->ReqGetPoint(kMsgGuid, kStrAccountID.c_str());
		}break;
	case GOET_CHANGE_BIRTH:
		{
			std::wstring kStrBirth;
			pkMsg->Pop(kStrBirth);

			bProcessRet = m_pkLocalProcess->ReqChangeBirthday(kMsgGuid, kStrAccountID.c_str(), kStrBirth.c_str());
		}break;
	}

	if(!bProcessRet)
	{//에러 발생시 오류 전달
		//로그 남길 필요 있겠음.
		return m_pkLocalProcess->VPostErrorMsg(kMsgGuid, kStrAccountID, 0, GE_SYSTEM_ERR);
	}

	return S_OK;
}