#include "StdAfx.h"
#include "PgGMProcessMgr.h"
#include "PgWaitingLobby.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/gm_const.h"
#include "Variant/PgStringUtil.h"
#include "HellDart/PgIpChecker.h"
#include "PgSiteMgr.h"
#include "Collins/Log.h"
#include "ImmTask.h"

namespace PgGmProcessMgrUtil
{
	void SetOrderState(SGmOrder const& rkOrderData, EOrderState const eState)
	{
		SEventMessage kMsg(EIMM_OBJ_GM, PgGMProcessMgr::EGM_2ND_ORDERSTATE);
		kMsg.Push( rkOrderData.kCmdGuid );
		kMsg.Push( eState );
		g_kImmTask.PutMsg(kMsg);
	}
};

PgGMProcessMgr::PgGMProcessMgr(void)
: m_kMgrGuid(BM::GUID::Create())
{
}

PgGMProcessMgr::~PgGMProcessMgr(void)
{
}

bool PgGMProcessMgr::AnalysisOderData(BM::Stream * const pkPacket)
{
	SGmOrder kOrderData;
	SERVER_IDENTITY kSI;
	kOrderData.ReadFromPacket(pkPacket);

	switch(kOrderData.Type)
	{
	case ORDER_PATCH_VERSION_EDIT:
		{
			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_PATCH_VERSION_EDIT, L"EXEC [dbo].[UP_UpdatePatchVersion3]");
			kQuery.QueryOwner(kOrderData.kCmdGuid);
			kQuery.PushStrParam(kOrderData.iValue[0]);
			kQuery.PushStrParam(kOrderData.iValue[1]);
			kQuery.PushStrParam(kOrderData.iValue[2]);
			kQuery.PushStrParam(kOrderData.wsString1);
			g_kCoreCenter.PushQuery(kQuery);//쿼리 결과를 받았을때 성공/실패 처리함.
		}break;
	case ORDER_BLOCKIP_ADD_ACCEPT:
		{
			VEC_WSTRING kVec;
			PgStringUtil::BreakSep(kOrderData.wsString1, kVec, L"/");

			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_ADD_IP_FOR_ACCEPT, L"EXEC [dbo].[up_Add_DefIPForAccept]");
			kQuery.QueryOwner( kOrderData.kCmdGuid );
			kQuery.PushStrParam( kVec[0] ); // StartIP
			kQuery.PushStrParam( kVec[1] );	// EndIP
			kQuery.PushStrParam( kVec[2] );	// NationKey
			kQuery.PushStrParam( kVec[3] );	// NationName
			kQuery.PushStrParam( kOrderData.dtTime[0] ); // ExpireDate
			g_kCoreCenter.PushQuery( kQuery, true );

			SIpRange const kNewRange( PgIPCheckerUtil::QueryResultToSIPRange(kVec[0], kVec[1], kVec[2], kVec[3]) );

			bool const bIsBlockIP = false;
			BM::Stream kPacket(PT_IM_L_ADD_ACCEPT_IP);
			kPacket.Push( bIsBlockIP );
			kNewRange.WriteToPacket( kPacket );
			SendToServerType(CEL::ST_LOGIN, kPacket);

			PgGmProcessMgrUtil::SetOrderState(kOrderData, OS_DONE);
		}break;
	case ORDER_BLOCK_USER:
		{
			kOrderData.WriteToPacket(*pkPacket);
			ProcessBlockUser(ECK_ORDER, pkPacket);
		}break;
	case ORDER_KICK_USER:
		{
			ProcessKickUser(ECK_ORDER_KICK, kOrderData.kCmdGuid, kOrderData.kGuid[0]);
		}break;
	case ORDER_EVENT_QUEST_RELOAD:
	case ORDER_BATTLE_SQUARE_RELOAD:
	case ORDER_SUSPEND_BATTLESQUARE:
		{
			BM::Stream kPacket(PT_GM_A_REQ_GMORDER);
			kOrderData.WriteToPacket(kPacket);
			SendToContentsServer(kOrderData.usRealm, kPacket); // Send to all realm

			PgGmProcessMgrUtil::SetOrderState(kOrderData, OS_DONE);
		}break;
	case ORDER_CHANGE_PLAYERPLAYTIME:
		{
			switch( kOrderData.iValue[0] )
			{
			case 1:
				{//리로드
					CEL::DB_QUERY kQuery(DT_MEMBER, DQT_LOAD_DEF_PLAYERPLAYTIME, _T("EXEC [dbo].[up_LoadDefPlayerPlayTime]"));
					kQuery.QueryOwner(kOrderData.kCmdGuid);
					kQuery.contUserData.Push(true);
					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case 3:
				{//전유저 리셋
					g_kWaitingLobby.ProcessResetPlayTime();
				}break;
			default:
				{
					BM::Stream kPacket(PT_GM_A_REQ_GMORDER);
					kOrderData.WriteToPacket(kPacket);
					SendToContentsServer(kOrderData.usRealm, kPacket);
				}break;
			}
		}break;
	case ORDER_CHANGE_NAME: 
	case ORDER_CHANGE_STATE:
	case ORDER_CHANGE_ITEM_INFO:
	case ORDER_CHANGE_SKILL_INFO:
	case ORDER_CHANGE_POS:
	case ORDER_CHANGE_QUEST_INFO:
	case ORDER_CHANGE_CHARACTER_FACE:
	case ORDER_CHANGE_CP:
	case ORDER_CHANGE_GM_LEVEL:
	case ORDER_CHANGE_ACHIEVEMENT:
	case ORDER_CHANGE_QUEST_ENDED:
		{
			if(ProcessKickUser(ECK_ORDER, kOrderData.kCmdGuid, kOrderData.kGuid[0]))
			{//! 해당 아이디가 로그인 중일 경우 Kick을 하고 Order는 실패로 처리.
				SendOrderFailed(kOrderData.kCmdGuid, GE_LOGINED_ID);
				return true;
			}
		}//! No Breake!!!
	default:
		{
			BM::Stream kPacket(PT_GM_A_REQ_GMORDER);
			kOrderData.WriteToPacket(kPacket);
			SendToContentsServer(kOrderData.usRealm, kPacket);
		}break;
	}
	return true;
}

void PgGMProcessMgr::RecvGMCommand(BM::Stream * const pkPacket )
{
	EGMCommandType eCommand;
	BM::GUID kReqGuid;
	pkPacket->Pop(eCommand);
	switch(eCommand)
	{
	case EGMC_KICKUSER:
		{
			int iCase;
			std::wstring kID;
			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(iCase);
			pkPacket->Pop(kID);
			ProcessKickUser(iCase, kReqGuid, kID);
		}break;
	case EGMC_CHANGEPASSWORD:
		{
			std::wstring wAccountID;
			std::wstring wPassword;
			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(wAccountID);
			pkPacket->Pop(wPassword);

			TCHAR chID[MAX_CHARACTERNAME_LEN+1] = {0, };
			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_CHANGE_PASSWORD, _T("EXEC [dbo].[UP_UpdateMemberPWByID]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(kReqGuid);
			kQuery.PushStrParam(wAccountID);
			kQuery.PushStrParam(wPassword);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case EGMC_CREATE_USER_ACCOUNT:	//중국쪽 RPC모듈에서 유저ID만드는부분 지움 안됨
		{
			std::wstring kId;
			std::wstring kPw;
			std::wstring kBirthday;
			int iUid = 0;
			BYTE sGender = 1;

			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(kId);
			pkPacket->Pop(kPw);
			pkPacket->Pop(kBirthday);

			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_USER_CREATE_ACCOUNT, _T("EXEC [dbo].[UP_CreateMemberAccount]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(kReqGuid);
			kQuery.PushStrParam(iUid);		//UID
			kQuery.PushStrParam(kId);
			kQuery.PushStrParam(kPw);
			kQuery.PushStrParam(sGender);		//gender
			kQuery.PushStrParam(kBirthday);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case EGMC_FREEZEACCOUNT:
		{
			int iCase;
		
			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(iCase);
			BM::Stream kPacket;
			kPacket.Push(kReqGuid);
			kPacket.Push(*pkPacket);
			ProcessBlockUser(iCase, &kPacket);	
		}break;
	case EGMC_CHN_ADDCASH:
		{
			 std::wstring wID;
			int iAddCash;

			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(wID);
			pkPacket->Pop(iAddCash);
			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_GM_ADD_CASH, _T("EXEC [dbo].[UP_AddMemberCash]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(kReqGuid);
			kQuery.PushStrParam(wID);
			kQuery.PushStrParam(iAddCash);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case EGMC_CHN_GETCASH:
		{
			std::wstring wID;

			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(wID);
			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_GM_GET_CASH, _T("EXEC [dbo].[UP_SelectMemberCash]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(kReqGuid);
			//TCHAR chID[MAX_CHARACTERNAME_LEN+1];
			//SAFE_STRNCPY(chID, wID.c_str());
			//kQuery.PushParam(chID);
			kQuery.PushStrParam(wID);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case EGMC_CHN_CHANGE_BIRTHDAY:
		{
			std::wstring kId;
			std::wstring kBirthday;

			pkPacket->Pop(kReqGuid);
			pkPacket->Pop(kId);
			pkPacket->Pop(kBirthday);

			if(kBirthday == L"0" )
				kBirthday = L"";

			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_GM_CHANGE_BIRTHDAY, _T("EXEC [dbo].[up_ChagneUserBirthday]"));
			kQuery.InsertQueryTarget(m_kMgrGuid);
			kQuery.QueryOwner(kReqGuid);
			kQuery.PushStrParam(kId);
			kQuery.PushStrParam(kBirthday);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case EGMC_NOTICE:
		{
			unsigned short usRealm = 0;
			pkPacket->Pop(usRealm);

			BM::Stream kPacket(PT_GM_A_REQ_GMCOMMAND, EGMC_NOTICE);
			kReqGuid.Generate();
			kPacket.Push(kReqGuid);
			kPacket.Push(*pkPacket);
			if(usRealm > 0)
			{
				SendToContentsServer(usRealm, kPacket);
			}
			else
			{
				SendToServerType(CEL::ST_CONTENTS, kPacket);
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unknown GMCommand received CommandType[") << (int)eCommand << _T("]") );
		}break;
	}
}


void PgGMProcessMgr::SendOrderState(BM::GUID const &rkOrderId, unsigned short usState)
{
	BM::Stream kPacket(PT_A_GM_ANS_GMORDER);
	kPacket.Push(rkOrderId);
	kPacket.Push(usState);
	SendToGmServer(kPacket);
}

void PgGMProcessMgr::SendOrderFailed(BM::GUID const &rkOrderId, E_GM_ERR const eErrorCode)
{
	BM::Stream kPacket(PT_A_GM_ANS_FAILED_GMORDER);
	kPacket.Push(rkOrderId);
	kPacket.Push(eErrorCode);
	SendToGmServer(kPacket);
}

bool PgGMProcessMgr::CommonGMOrderQueryResult( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Success") );
		int iError = GE_SUCCESS;
		
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iError);			++kIter;

			if(iError == GE_SUCCESS)
			{
				SendOrderState(rkResult.QueryOwner(), OS_DONE);
			}
			else
			{
				SendOrderFailed(rkResult.QueryOwner(), GE_QUERY_FAILED);
			}
			return true;
		}
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}


bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_NAME( CEL::DB_RESULT &rkResult )
{
	return CommonGMOrderQueryResult(rkResult);
}


bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_STATE( CEL::DB_RESULT &rkResult )
{
	return CommonGMOrderQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_POS( CEL::DB_RESULT &rkResult )
{
	return CommonGMOrderQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_GM_LEVEL(CEL::DB_RESULT &rkResult)
{
	return CommonGMOrderQueryResult(rkResult);
}


bool PgGMProcessMgr::Q_DQT_GM_FREEZE_ACCOUNT( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Success") );
		int iError = GE_SUCCESS;
		int iCase = ECK_NONE;
		std::wstring kID;
		BYTE byBlock = 0;
		int iBlockType = 0;
		BM::DBTIMESTAMP_EX dtBlockStart;
		BM::DBTIMESTAMP_EX dtBlockEnd;
		
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iError);			++kIter;
			kIter->Pop(iCase);			++kIter;
			kIter->Pop(kID);			++kIter;
			kIter->Pop(byBlock);		++kIter;
			kIter->Pop(dtBlockStart);	++kIter;
			kIter->Pop(dtBlockEnd);		++kIter;

			switch(iCase)
			{
			case ECK_NONE:
				{//블럭 해제
				}break;
			case ECK_ORDER:
				{
					if(iError == GE_SUCCESS)
					{
						SendOrderState(rkResult.QueryOwner());
					}
					else 
					{
						SendOrderFailed(rkResult.QueryOwner(), GE_QUERY_FAILED);
					}
				}break;
			case ECK_CHN:
				{
					BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_FREEZEACCOUNT);
					kPacket.Push(rkResult.QueryOwner());
					kPacket.Push(iError);
					SendToGmServer(kPacket);
				}break;
			case ECK_GMC:
				{
				}break;
			default : 
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Unknown GM Command: [") << iCase << _T("]") );
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				}break;
			}

			PgLogCont kLogCont(ELogMain_System_Block);
			kLogCont.ID(kID);
			PgLog kLog;
			kLog.Set(0, iError);
			kLog.Set(1, static_cast<int>(byBlock));

			if( (byBlock > 0) 
				&&	(dtBlockStart > dtBlockEnd) )
			{
				iBlockType = 1;
			}
			else if( (byBlock > 0) 
				&& (dtBlockStart < dtBlockEnd) )
			{
					iBlockType = 2;
			}
			kLog.Set(2, iBlockType);
			kLog.Set(0, dtBlockStart);
			kLog.Set(1, dtBlockEnd);
			kLogCont.Add(kLog);
			kLogCont.Commit();

			return true;
		}
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;

}

bool PgGMProcessMgr::Q_DQT_CHANGE_PASSWORD( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Success") );
		int iError = GE_SUCCESS;
		std::wstring kID;
		
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iError);			++kIter;
			kIter->Pop(kID);			++kIter;

			if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA))
			{
				BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_CHANGEPASSWORD);
				kPacket.Push(rkResult.QueryOwner());
				kPacket.Push(iError);
				SendToGmServer(kPacket);
			}

			PgLogCont kLogCont(ELogMain_System_Change_Password);
			kLogCont.ID(kID);
			PgLog kLog;
			kLog.Set(0, iError);
			kLogCont.Add(kLog);
			kLogCont.Commit();

			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Failed") );
	return false;
}

bool PgGMProcessMgr::Q_DQT_USER_CREATE_ACCOUNT( CEL::DB_RESULT &rkResult )
{
	int iResult;
	BM::GUID kMemberGuid;
	std::wstring kID;
	BYTE byGender = 0;
	std::wstring kBrithday;
	int iUID = 0;

	BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_CREATE_USER_ACCOUNT);
	kPacket.Push(rkResult.QueryOwner());
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if ( rkResult.vecArray.end() != kIter )
		{
			kIter->Pop(iResult);		++kIter;
			kIter->Pop(kMemberGuid);	++kIter;
			kIter->Pop(kID);			++kIter;
			kIter->Pop(byGender);		++kIter;
			kIter->Pop(kBrithday);		++kIter;
			kIter->Pop(iUID);			++kIter;

			switch(iResult)
			{
			case 0: {kPacket.Push(GE_SUCCESS);}break;
			case 99: {kPacket.Push(GE_USING_ID);}break;
			default: 
				{
					kPacket.Push(GE_UNKNOWN_ERROR);
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				}break;
			}
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		kPacket.Push(GE_SYSTEM_ERR);
	}

	SendToGmServer(kPacket);

	//계정 생성 로그
	PgLogCont kLogCont(ELogMain_System_Create_NewAccount);
	kLogCont.MemberKey(kMemberGuid);
	kLogCont.ID(kID);
	kLogCont.UID(iUID);
	PgLog kLog;
	kLog.Set(0, static_cast<int>(byGender));
	kLog.Set(2, iResult);
	kLog.Set(0, kBrithday);
	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}


bool PgGMProcessMgr::Q_DQT_GM_ADD_CASH( CEL::DB_RESULT &rkResult )
{
	int iResult = 0;
	int iOldCash = 0;
	int iAddCash = 0;
	int iUID = 0;
	std::wstring kID;

	BM::Stream kAPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_CHN_ADDCASH);
	kAPacket.Push(rkResult.QueryOwner());
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if ( rkResult.vecArray.end() != itor )
		{
			(*itor).Pop(iResult);	++itor;
			(*itor).Pop(kID);		++itor;
			(*itor).Pop(iOldCash);	++itor;
			(*itor).Pop(iAddCash);	++itor;
			(*itor).Pop(iUID);		++itor;

			if(0 == iResult)
			{
				kAPacket.Push(GE_SUCCESS);
			}
			else
			{
				kAPacket.Push(GE_INCORRECT_ID);				
			}
		}
	}
	else
	{
		kAPacket.Push(GE_SYSTEM_ERR);
	}
	SendToGmServer(kAPacket);

	//캐쉬 추가 로그
	PgLogCont kLogCont(ELogMain_Cash, ELogSub_Cash);
	kLogCont.ID(kID);
	kLogCont.UID(iUID);
	PgLog kLog(ELOrderMain_Charge);
	kLog.Set(0, iResult);
	kLog.Set(2, iAddCash);
	kLog.Set(3, iOldCash);
	kLog.Set(4, iAddCash + iOldCash);
	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}

bool PgGMProcessMgr::Q_DQT_GM_GET_CASH( CEL::DB_RESULT &rkResult )
{
	BM::Stream kAPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_CHN_GETCASH);
	kAPacket.Push(rkResult.QueryOwner());
	int iError = GE_SUCCESS;
	__int64 i64Cash = 0;
	std::wstring wID;
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if ( rkResult.vecArray.end() != itor )
		{
			(*itor).Pop(wID);	++itor;
			if (wID.length() == 0)
			{
				iError = GE_INCORRECT_ID;
			}
			else
			{
				iError = GE_SUCCESS;
				(*itor).Pop(i64Cash);	++itor;
			}
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		iError = GE_SYSTEM_ERR;
	}
	kAPacket.Push(iError);
	kAPacket.Push(i64Cash);
	SendToGmServer(kAPacket);

	//cash 정보 확인 로그
	PgLogCont kLogCont(ELogMain_Cash, ELogSub_Cash);
	kLogCont.ID(wID);
	PgLog kLog(ELOrderMain_Cash, ELOrderSub_Info);
	kLog.Set(0, iError);
	kLog.Set(2, i64Cash);
	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}

bool PgGMProcessMgr::Q_DQT_GM_CHANGE_BIRTHDAY( CEL::DB_RESULT &rkResult )
{
	BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_CHN_CHANGE_BIRTHDAY);
	kPacket.Push(rkResult.QueryOwner());
	int iError = 0;
	std::wstring kID;
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator Itr = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != Itr )
		{
			Itr->Pop(iError);	++Itr;
			Itr->Pop(kID);		++Itr;

			switch(iError)
			{
			case 0:{kPacket.Push(GE_SUCCESS);}break;
			case 1:{kPacket.Push(GE_INCORRECT_ID);}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
					kPacket.Push(GE_SYSTEM_ERR);
				}break;
			}
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		kPacket.Push(GE_SYSTEM_ERR);
	}
	SendToGmServer(kPacket);

	//생년월일 변경 로그
	PgLogCont kLogCont(ELogMain_System_Change_Birthday);
	kLogCont.ID(kID);
	PgLog kLog;
	kLog.Set(0, iError);
	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}

bool PgGMProcessMgr::ProcessKickUser(int iCase, BM::GUID const &kReqGuid, BM::GUID const &kMemberID, bool const bAfterBlock)
{
	bool  bRet;

	if(BM::GUID::IsNull(kMemberID))
	{
		bRet = false;
	}
	else
	{
		 bRet = g_kWaitingLobby.IsConnectUser( kMemberID );
	}

	if( bRet )
	{//접속 중인 넘인지 확인.
		switch(iCase)
		{
		case ECK_ORDER_KICK:
			{
				g_kWaitingLobby.ProcessLogout( kMemberID, true );
				SendOrderState(kReqGuid, OS_DONE);
			}break;
// 		case ECK_GMC:
// 		case ECK_ORDER:
// 			{
// 				g_kWaitingLobby.ProcessLogout( kMemberID, true );
// 			}break;
		case ECK_CHN:
			{
				g_kWaitingLobby.ProcessLogout( kMemberID, true );

				if(!bAfterBlock)
				{
					BM::Stream kGMPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_KICKUSER);
					kGMPacket.Push(kReqGuid);
					kGMPacket.Push( (int)GE_SUCCESS );
					SendToGmServer(kGMPacket);
				}
			}break;
		default:
			{//ECK_ORDER 의 경우 이쪽.
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				g_kWaitingLobby.ProcessLogout( kMemberID, true );
			}break;
		}
	}
	else
	{
		switch(iCase)
		{
		case ECK_CHN:
			{
				if(!bAfterBlock)
				{
					BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_KICKUSER);
					kPacket.Push(kReqGuid);
					kPacket.Push((int)GE_NOT_ONLINE);
					SendToGmServer(kPacket);
				}
			}break;
		case ECK_ORDER_KICK:
			{//! 유저가 이미 로그아웃 했으므로 오더 실행 불가.
				SendOrderFailed(kReqGuid, GE_ALREADY_EXIST);
			}
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			}break;
		}
	}

	return bRet;
}

void PgGMProcessMgr::ProcessKickUser(int iCase, BM::GUID const &kReqGuid, std::wstring const &kID, bool const bAfterBlock)
{
	BM::GUID kMemberGuid;
	std::wstring kTempAccountID(kID);
	switch( g_kLocal.ServiceRegion() )
	{
	case LOCAL_MGR::NC_JAPAN:
		{
			LWR(kTempAccountID);
		}break;
	default:
		{
			UPR(kTempAccountID);
		}break;
	}
	g_kWaitingLobby.GetMemberGuid( kTempAccountID, kMemberGuid );
	ProcessKickUser( iCase, kReqGuid, kMemberGuid, bAfterBlock );
}

void PgGMProcessMgr::ProcessBlockUser(int iCase, BM::Stream *pkPacket)
{
	SGmOrder kOrder;
	switch(iCase)
	{
	case ECK_CHN:
		{
			pkPacket->Pop(kOrder.kReqGuid);
			pkPacket->Pop(kOrder.wsString1);
			pkPacket->Pop(kOrder.iValue[0]);
			pkPacket->Pop(kOrder.dtTime[0]);
			pkPacket->Pop(kOrder.dtTime[1]);

			ProcessKickUser(iCase, kOrder.kReqGuid, kOrder.wsString1, true);
		}break;
	case ECK_ORDER:
		{
			kOrder.ReadFromPacket(pkPacket);
			ProcessKickUser(iCase, kOrder.kReqGuid, kOrder.wsString1, true);

			if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_USA ) )
			{
				BM::Stream kPacket( PT_IM_CN_NFY_BLOCKUSER_GRAVITY );
				kPacket.Push( kOrder.wsString2 ); //
				if( !::SendToConsentServer( kPacket ) )
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
				}
			}
		}break;
	case ECK_GMC:
		{
			pkPacket->Pop(kOrder.kReqGuid);// MemberGUID (미접속시 NULL GUID)
			pkPacket->Pop(kOrder.wsString1);// 계정 명
			pkPacket->Pop(kOrder.iValue[0]);// 블럭 값 (100이상이면 블럭)
			pkPacket->Pop(kOrder.dtTime[0]);
			pkPacket->Pop(kOrder.dtTime[1]);

//			SLoginedUser kUser;
//			if(g_kWaitingLobby.FindConnectUser(kOrder.kGuid[0], kUser))
				ProcessKickUser(iCase, kOrder.kReqGuid, kOrder.wsString1, true);
//			kOrder.wsString1 = kUser.szID;	
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	}

	CEL::DB_QUERY kQuery;

	//널값이 들어오면 시간을 1900-0-0으로 맞춘다.
	if(kOrder.dtTime[0].year == 0)
	{
		kOrder.dtTime[0].year = 1901;		
		kOrder.dtTime[0].month = 1;
		kOrder.dtTime[0].day = 1;
	}

	if(kOrder.dtTime[1].year == 0)
	{
		kOrder.dtTime[1].year = 1900;		
		kOrder.dtTime[1].month = 1;
		kOrder.dtTime[1].day = 1;
	}


	kQuery.Set( DT_MEMBER, DQT_GM_FREEZE_ACCOUNT, L"EXEC [dbo].[UP_UpdateMemberBlock_ID]");
	kQuery.InsertQueryTarget(m_kMgrGuid);
	kQuery.QueryOwner(kOrder.kReqGuid);
	kQuery.PushStrParam(iCase);
	kQuery.PushStrParam(kOrder.wsString1);
	kQuery.PushStrParam((BYTE)kOrder.iValue[0]);
	kQuery.PushStrParam(kOrder.dtTime[0]);
	kQuery.PushStrParam(kOrder.dtTime[1]);
	g_kCoreCenter.PushQuery(kQuery);
}


bool PgGMProcessMgr::SendToGmServer(BM::Stream const &rkPacket)
{
	CONT_SERVER_HASH kServer;
	g_kProcessCfg.Locked_GetServerInfo(CEL::ST_GMSERVER, kServer);
	CONT_SERVER_HASH::const_iterator Itr = kServer.begin();
	if(Itr != kServer.end())
	{
		CONT_SERVER_HASH::key_type kKey = Itr->first;
		return SendToServer(kKey, rkPacket);
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return  false;
}

void PgGMProcessMgr::HandleMessage(SEventMessage *pkMsg)
{
	EGM_TASK_TYPE const eSecType = static_cast<EGM_TASK_TYPE const>(pkMsg->SecType());
	switch(eSecType)
	{
	case EGM_2ND_PACKET:
		{
			PACKET_ID_TYPE wType;
			pkMsg->Pop(wType);
			BM::Stream * const pkPacket = dynamic_cast<BM::Stream*const>(pkMsg);
			if (pkPacket == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot convert SEventMessage->Stream"));
				return;
			}
			switch (wType)
			{
			case PT_GM_A_REQ_GMCOMMAND:
				{
					RecvGMCommand(pkPacket);
				}break;
			case PT_GM_A_REQ_GMORDER:
				{
					AnalysisOderData(pkPacket);
				}break;
			case PT_A_GM_ANS_GMORDER:
			case PT_A_GM_ANS_FAILED_GMORDER:
			case PT_A_GM_ANS_GMCOMMAND:
			case PT_N_O_ANS_CHECK_LOGINED_USER:
			case PT_C_O_ANS_SEND_GM_CHAT:
			case PT_C_GM_REQ_RECEIPT_PETITION:
			case PT_C_GM_REQ_REMAINDER_PETITION:
				{
					SendToGmServer(*pkPacket);
				}break;
			default:
				{
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown packet type=") << wType);
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Unknown packet type=") << wType);
				}break;
			}
		}break;
	case EGM_2ND_DBRESULT:
		{
			CEL::DB_QUERY kTemp;
			CEL::DB_RESULT kResult(kTemp);
			kResult.ReadFromPacket(*pkMsg);
			switch(kResult.QueryType())
			{
			case DQT_USER_CREATE_ACCOUNT:
				{
					Q_DQT_USER_CREATE_ACCOUNT(kResult); 
				}break;
			case DQT_GM_FREEZE_ACCOUNT:		
				{
					Q_DQT_GM_FREEZE_ACCOUNT(kResult); 
				}break;
			case DQT_GM_ADD_CASH:
				{
					Q_DQT_GM_ADD_CASH(kResult); 
				}break;
			case DQT_GM_GET_CASH:			
				{
					Q_DQT_GM_GET_CASH(kResult); 
				}break;
			case DQT_GM_CHANGE_BIRTHDAY:
				{
					Q_DQT_GM_CHANGE_BIRTHDAY(kResult);
				}break;
			case DQT_CHANGE_PASSWORD:		
				{
					Q_DQT_CHANGE_PASSWORD(kResult);
				}break;
			default:
				{
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown QueryType=") << kResult.QueryType());
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unknown QueryType=") << kResult.QueryType());
				}break;
			}

		}break;
	case EGM_2ND_ORDERSTATE:
		{
			BM::GUID kOwner;
			EOrderState eState;
			pkMsg->Pop(kOwner);
			pkMsg->Pop(eState);
			if(OS_DONE == eState)
			{
				SendOrderState(kOwner, OS_DONE);
			}
			else
			{
				SendOrderFailed(kOwner, GE_QUERY_FAILED);
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Event type=") << eSecType);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Event type=") << eSecType);
		}break;
	}
}