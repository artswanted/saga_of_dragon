#include "stdafx.h"
#include "variant/PgDbCache.h"
#include "Variant/PgControlDefMgr.h"
#include "variant/PgItemRarityUpgradeFormula.h"
#include "Variant/Cashshop.h"
#include "item/PgCashShopManager.h"

#include "PgSendWrapper.h"
#include "JobDispatcher.h"
#include "Transaction.h"
#include "lohengrin/variablecontainer.h"

bool PgCashShopUserMgr::Locked_AddUser(SCASHSHOPUSER const & kUser)
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kCont.insert(std::make_pair(kUser.CashShopGuid(),kUser)).second;
}

bool PgCashShopUserMgr::Locked_RemoveUser(BM::GUID const & kShopUserGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SHOPUSER::iterator iter = m_kCont.find(kShopUserGuid);
	if(iter == m_kCont.end())
	{
		return false;
	}
	m_kCont.erase(iter);
	return true;
}

bool PgCashShopUserMgr::Locked_GetUser(BM::GUID const & kShopUserGuid,SCASHSHOPUSER & kUser) const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SHOPUSER::const_iterator iter = m_kCont.find(kShopUserGuid);
	if(iter == m_kCont.end())
	{
		return false;
	}
	
	kUser = (*iter).second;

	return true;
}

bool PgCashShopUserMgr::Locked_SetUser(SCASHSHOPUSER const & kUser)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SHOPUSER::iterator iter = m_kCont.find(kUser.CashShopGuid());
	if(iter == m_kCont.end())
	{
		return false;
	}
	
	(*iter).second = kUser;

	return true;
}

bool PgCashShopMgr::IsEnableUseBonus(__int64 const i64BonusUse)
{
	if( CashShopUtil::IsCanUseMileageServiceRegion() )
	{
		return true;
	}
	else
	{
		if( 0 < i64BonusUse )
		{
			return false;
		}
	}
	return true;
}

__int64 const PgCashShopMgr::CalcAddBonus(__int64 const i64Cost,int const iMileage)
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_USA:
		{
			return 0i64;
		}
	}
	return static_cast<__int64>((i64Cost * iMileage)/100);
}

__int64 const PgCashShopMgr::CalcLocalAddBonus(BM::GUID const & kOwnerGuid,BM::GUID const & kShopUserGuid,__int64 const i64Cost)
{
	if( CashShopUtil::IsCanUseMileageServiceRegion() )
	{
		SCASHSHOPUSER kSopUser;
		if(false == m_kShopUserMgr.Locked_GetUser(kShopUserGuid,kSopUser))
		{
			return 0;
		}

		__int64 const i64TotalUsedCash = kSopUser.TotalUsedCash() + i64Cost;
		
		__int64 const i64UsedCash_100 = i64TotalUsedCash - kSopUser.RecvPointCash_100();
		__int64 const i64UsedCash_1000 = i64TotalUsedCash - kSopUser.RecvPointCash_1000();

		int iUseCash100 = DEFAULT_MILEAGE_USECASH_100;
		g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_UseCash_100, iUseCash100);

		int iUseCash1000 = DEFAULT_MILEAGE_USECASH_1000;
		g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_UseCash_1000, iUseCash1000);

		if(0 >= iUseCash100)
		{
			iUseCash100 = DEFAULT_MILEAGE_USECASH_100;
		}

		if(0 >= iUseCash1000)
		{
			iUseCash1000 = DEFAULT_MILEAGE_USECASH_1000;
		}

		int iCalcMileage100 = DEFAULT_CALC_MILEAGE_100;
		g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_Mileage_Cash_100, iCalcMileage100);

		int iCalcMileage1000 = DEFAULT_CALC_MILEAGE_1000;
		g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_Mileage_Cash_1000, iCalcMileage1000);

		__int64 const i64BonusPoint_100 = (i64UsedCash_100/iUseCash100) * iCalcMileage100;
		__int64 const i64BonusPoint_1000 = (i64UsedCash_1000/iUseCash1000) * iCalcMileage1000;

		return i64BonusPoint_100 + i64BonusPoint_1000;
	}
	return 0;
}

bool PgCashShopMgr::UpdateUseCash(BM::GUID const & kOwnerGuid,BM::GUID const & kShopUserGuid,__int64 const i64Cost)
{
	if( CashShopUtil::IsCanUseMileageServiceRegion() )
	{
		SCASHSHOPUSER kSopUser;
		if(false == m_kShopUserMgr.Locked_GetUser(kShopUserGuid,kSopUser))
		{
			return false;
		}

		__int64 const i64TotalUsedCash = kSopUser.TotalUsedCash() + -i64Cost;
		
		kSopUser.TotalUsedCash(i64TotalUsedCash);

		int iUseCash100 = DEFAULT_MILEAGE_USECASH_100;
		g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_UseCash_100, iUseCash100);

		int iUseCash1000 = DEFAULT_MILEAGE_USECASH_1000;
		g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_UseCash_1000, iUseCash1000);

		if(0 >= iUseCash100)
		{
			iUseCash100 = DEFAULT_MILEAGE_USECASH_100;
		}

		if(0 >= iUseCash1000)
		{
			iUseCash1000 = DEFAULT_MILEAGE_USECASH_1000;
		}

		__int64 const i64RecvPointCash_100 = (i64TotalUsedCash/iUseCash100) * iUseCash100;
		kSopUser.RecvPointCash_100(i64RecvPointCash_100);

		__int64 const i64RecvPointCash_1000 = (i64TotalUsedCash/iUseCash1000) * iUseCash1000;
		kSopUser.RecvPointCash_1000(i64RecvPointCash_1000);

		m_kShopUserMgr.Locked_SetUser(kSopUser);

		BM::Stream kAbilPacket(PT_M_C_NFY_STATE_CHANGE64,kOwnerGuid);
		kAbilPacket.Push(static_cast<BYTE>(3));
		kAbilPacket.Push(SAbilInfo64(AT_MILEAGE_TOTAL_CASH,kSopUser.TotalUsedCash()));
		kAbilPacket.Push(SAbilInfo64(AT_MILEAGE_100,kSopUser.RecvPointCash_100()));
		kAbilPacket.Push(SAbilInfo64(AT_MILEAGE_1000,kSopUser.RecvPointCash_1000()));

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kOwnerGuid);
		pkActionOrder->kCause = CNE_CONTENTS_SENDTOUSER;
		
		ContentsActionEvent_SendPacket kEvent(ECEvent_SendToUser);
		kEvent.MemberGuid(false);
		kEvent.Guid(kOwnerGuid);
		kEvent.SendType(ContentsActionEvent_SendPacket::E_SendUser_ToOneUser);
		kEvent.m_kPacket.Push(kAbilPacket);

		SPMO kOrder(IMET_CONTENTS_EVENT, kOwnerGuid, kEvent);
		pkActionOrder->kContOrder.push_back(kOrder);
		g_kJobDispatcher.VPush(pkActionOrder);

		return true;
	}
	return false;
}

bool PgCashShopMgr::Locked_HandleRecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	switch(kPacketType)
	{
	case PT_M_I_CS_REQ_MODIFY_VISABLE_RANK:
		{
			OnRecvPT_M_I_CS_REQ_MODIFY_VISABLE_RANK(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_BUY_ARTICLE:
		{
			OnRecvPT_M_I_CS_REQ_BUY_ARTICLE(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_SEND_GIFT:
		{
			OnRecvPT_M_I_CS_REQ_SEND_GIFT(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_SIMPLE_ENTER_CASHSHOP:
		{
			OnRecvPT_M_I_CS_REQ_SIMPLE_ENTER_CASHSHOP(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_ENTER_CASHSHOP:
		{
			OnRecvPT_M_I_CS_REQ_ENTER_CASHSHOP(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_EXIT_CASHSHOP:
		{
			OnRecvPT_M_I_CS_REQ_EXIT_CASHSHOP(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_RECV_GIFT:
		{
			OnRecvPT_M_I_CS_REQ_RECV_GIFT(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_ADD_TIMELIMIT:
		{
			OnRecvPT_M_I_CS_REQ_ADD_TIMELIMIT(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

bool PgCashShopMgr::UpdateCashShopRank(BM::GUID const & kOwnerGuid,BYTE const bState,__int64 const i64Cash,int const iMode)
{
	SContentsUser rkOut;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut);
	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_QUERY kQuery(DT_PLAYER,DQT_CS_MODIFY_VISABLE_RANK,L"EXEC [dbo].[UP_CS_MODIFY_CASH_RANK]");
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.PushStrParam(kOwnerGuid);
	kQuery.PushStrParam(bState);
	kQuery.PushStrParam(i64Cash);
	kQuery.QueryOwner(kOwnerGuid);
	kQuery.contUserData.Push(iMode);
	g_kCoreCenter.PushQuery(kQuery);
	return true;
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_ADD_TIMELIMIT(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	if(!g_kRealmUserMgr.Locked_EnterEvent(kOwnerGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Locked_EnterEvent is false"));
		return;
	}

	SContentsUser rkOut;
	SUserInfoExt kExtInfo;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
		return;
	}

	PgTranPointer kTran(ECASH_TRAN_EXPAND_TIMELIMIT, rkOut.kMemGuid, kOwnerGuid, kGndKey, kSI, CIE_CS_Add_TimeLimit);
	PgExpandTimeLimitTran* pkTran = dynamic_cast<PgExpandTimeLimitTran*>(kTran.GetTran());
	if (pkTran != NULL)
	{
		pkTran->UID(rkOut.iUID);
		pkTran->AccountID(rkOut.kAccountID);
		pkTran->AddedPacket().Push(*pkPacket);
		pkTran->CharacterName(rkOut.Name());
		pkTran->RemoteAddr(kExtInfo.kAddress);
	}
	kTran.RequestCurerntLimitSell();
	/*
	CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
	kQuery.InsertQueryTarget(kOwnerGuid);

	kQuery.PushStrParam(rkOut.kMemGuid);
	kQuery.QueryOwner(kOwnerGuid);
	kQuery.contUserData.Push(CIE_CS_Add_TimeLimit);
	kSI.WriteToPacket(kQuery.contUserData);
	kQuery.contUserData.Push(kGndKey);
	kQuery.contUserData.Push(rkOut.kMemGuid);
	kQuery.contUserData.Push(rkOut.Name());
	kQuery.contUserData.Push(*pkPacket);

	g_kCoreCenter.PushQuery(kQuery);
	*/
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_MODIFY_VISABLE_RANK(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);
	BYTE bState = 0;
	pkPacket->Pop(bState);

	UpdateCashShopRank(kOwnerGuid,bState,0,0);
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_BUY_ARTICLE(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	if(!g_kRealmUserMgr.Locked_EnterEvent(kOwnerGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Locked_EnterEvent is false"));
		return;
	}

	SContentsUser rkOut;
	SUserInfoExt kExtInfo;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
		return;
	}

	PgTranPointer kTran(ECASH_TRAN_CASHSHOP_BUY, rkOut.kMemGuid, kOwnerGuid, kGndKey, kSI, CIE_CS_Buy);
	PgCashShopBuyTran* pkTran = dynamic_cast<PgCashShopBuyTran*>(kTran.GetTran());
	if (pkTran != NULL)
	{
		BYTE bBuyType = 0;
		pkPacket->Pop(bBuyType);
		pkTran->UID(rkOut.iUID);
		pkTran->AccountID(rkOut.kAccountID);
		pkTran->AddedPacket().Push(*pkPacket);
		pkTran->CharacterName(rkOut.Name());
		pkTran->RemoteAddr(kExtInfo.kAddress);
		pkTran->BuyType(bBuyType);
	}
	kTran.RequestCurerntLimitSell();

	/*
	CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
	kQuery.InsertQueryTarget(kOwnerGuid);

	kQuery.PushStrParam(rkOut.kMemGuid);
	kQuery.QueryOwner(kOwnerGuid);
	kQuery.contUserData.Push(CIE_CS_Buy);
	kSI.WriteToPacket(kQuery.contUserData);
	kQuery.contUserData.Push(kGndKey);
	kQuery.contUserData.Push(rkOut.kMemGuid);
	kQuery.contUserData.Push(rkOut.Name());
	kQuery.contUserData.Push(*pkPacket);

	g_kCoreCenter.PushQuery(kQuery);
	*/
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_SEND_GIFT(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	if(!g_kRealmUserMgr.Locked_EnterEvent(kOwnerGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! EnterEvent is false"));
		return;
	}

	SContentsUser rkOut;
	SUserInfoExt kExtInfo;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
		return;
	}

	int iValue = 0;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Contents, EVar_CS_EanbleGiftLevel, iValue) )
	{
		iValue = 0;
	}

	if(iValue > rkOut.sLevel)
	{
		g_kRealmUserMgr.Locked_LeaveEvent(kOwnerGuid);
		BM::Stream kPacket(PT_M_C_CS_ANS_SEND_GIFT);
		kPacket.Push(CSR_NOT_ENOUGH_LEVEL);
		kPacket.Push(iValue);
		g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		return;
	}

	PgTranPointer kTran(ECASH_TRAN_SEND_GIFT, rkOut.kMemGuid, kOwnerGuid, kGndKey, kSI, CIE_CS_Gift);
	PgSendGiftTran* pkTran = dynamic_cast<PgSendGiftTran*>(kTran.GetTran());
	if (pkTran != NULL)
	{
		pkTran->UID(rkOut.iUID);
		pkTran->AccountID(rkOut.kAccountID);
		pkTran->AddedPacket().Push(*pkPacket);
		pkTran->CharacterName(rkOut.kName);
		pkTran->RemoteAddr(kExtInfo.kAddress);
	}
	kTran.RequestCurerntLimitSell();

	/*
	CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.PushStrParam(rkOut.kMemGuid);

	kQuery.QueryOwner(kOwnerGuid);
	kQuery.contUserData.Push(CIE_CS_Gift);
	kSI.WriteToPacket(kQuery.contUserData);
	kQuery.contUserData.Push(kGndKey);
	kQuery.contUserData.Push(rkOut.kMemGuid);
	kQuery.contUserData.Push(*pkPacket);
	g_kCoreCenter.PushQuery(kQuery);
	*/
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_ENTER_CASHSHOP(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid,
			 kShopUserGuid;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kShopUserGuid);

	SContentsUser rkOut;
	SUserInfoExt kExtInfo;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
	if(S_OK == hRet)
	{
		PgTranPointer kTran(ECASH_TRAN_CASHSHOP_ENTER, rkOut.kMemGuid, kOwnerGuid, kGndKey, kSI, CIE_CS_Select);
		PgEnterCashShopTran* pkTran = dynamic_cast<PgEnterCashShopTran*>(kTran.GetTran());
		if (pkTran != NULL)
		{
			pkTran->UID(rkOut.iUID);
			pkTran->AccountID(rkOut.kAccountID);
			pkTran->RemoteAddr(kExtInfo.kAddress);
		}
		kTran.RequestCurerntLimitSell();
		
		/*
		CEL::DB_QUERY kQuery2(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
		kQuery2.InsertQueryTarget(kOwnerGuid);
		kQuery2.PushStrParam(rkOut.kMemGuid);

		kQuery2.QueryOwner(kOwnerGuid);
		kQuery2.contUserData.Push(CIE_CS_Select);
		g_kCoreCenter.PushQuery(kQuery2);
		*/
	}

	CEL::DB_QUERY kQueryRank(DT_PLAYER,0,L"EXEC [dbo].[UP_CS_SELECT_RANK]");
	kQueryRank.InsertQueryTarget(kOwnerGuid);
	CEL::DB_QUERY kQueryGift(DT_PLAYER,1,L"EXEC [dbo].[UP_CS_SELECTGIFTLIST]");
	kQueryGift.InsertQueryTarget(kOwnerGuid);
	kQueryGift.PushStrParam(kOwnerGuid);
	
	CEL::DB_QUERY_TRAN kContQuery;
	kContQuery.DBIndex(DT_PLAYER);
	kContQuery.QueryType(DQT_CS_CASHSHOP);
	kContQuery.QueryOwner(kOwnerGuid);

	kContQuery.push_back(kQueryRank);
	kContQuery.push_back(kQueryGift);

	g_kCoreCenter.PushQuery(kContQuery);

	SCASHSHOPUSER kShopUser;
	kShopUser.CashShopGuid(kShopUserGuid);
	m_kShopUserMgr.Locked_AddUser(kShopUser);
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_SIMPLE_ENTER_CASHSHOP(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid,
			 kShopUserGuid;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kShopUserGuid);

	SContentsUser rkOut;
	SUserInfoExt kExtInfo;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
	if(S_OK == hRet)
	{
		PgTranPointer kTran(ECASH_TRAN_CASHSHOP_ENTER, rkOut.kMemGuid, kOwnerGuid, kGndKey, kSI, CIE_CS_Select);
		PgEnterCashShopTran* pkTran = dynamic_cast<PgEnterCashShopTran*>(kTran.GetTran());
		if (pkTran != NULL)
		{
			pkTran->UID(rkOut.iUID);
			pkTran->AccountID(rkOut.kAccountID);
			pkTran->RemoteAddr(kExtInfo.kAddress);
		}
		kTran.RequestCurerntLimitSell();
	}

	SCASHSHOPUSER kShopUser;
	kShopUser.CashShopGuid(kShopUserGuid);
	m_kShopUserMgr.Locked_AddUser(kShopUser);

	BM::Stream kPacket(PT_M_C_CS_ANS_SIMPLE_ENTER_CASHSHOP);
	kPacket.Push(CSR_SUCCESS);
	g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid, kPacket, false);
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_EXIT_CASHSHOP(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid,kShopUserGuid;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kShopUserGuid);
	m_kShopUserMgr.Locked_RemoveUser(kShopUserGuid);
}

void PgCashShopMgr::OnRecvPT_M_I_CS_REQ_RECV_GIFT(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID	kOwnerGuid,
				kGiftGuId;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kGiftGuId);

	if(!g_kRealmUserMgr.Locked_EnterEvent(kOwnerGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! EnterEvent is false"));
		return;
	}

	CEL::DB_QUERY kQuery(DT_PLAYER,DQT_CS_GIFT_QUEYR,L"EXEC [dbo].[UP_CS_SELECTGIFT]");
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.QueryOwner(kOwnerGuid);
	kQuery.PushStrParam(kGiftGuId);
	kSI.WriteToPacket(kQuery.contUserData);
	kQuery.contUserData.Push(kGndKey);
	g_kCoreCenter.PushQuery(kQuery);
}

typedef struct tagPRICE
{
	int		iPriceIdx;
	int		iTimeType;
	int		iUseTime;
	__int64 i64UseCash;
	__int64 i64UseMileage;
	tagPRICE():iPriceIdx(0),iTimeType(0),iUseTime(0),i64UseCash(0),i64UseMileage(0){}
	tagPRICE(int const __priceidx, int const __timetype,int const __usetime,__int64 const __usecash,__int64 const __usemileage):iPriceIdx(__priceidx),iTimeType(__timetype),iUseTime(__usetime),i64UseCash(__usecash),i64UseMileage(__usemileage){}
}SPRICE;

typedef std::vector<SPRICE> CONT_PRICE_RECORD;

ECashShopResult PgCashShopMgr::OnProcessReqBuyArticle(EItemModifyParentEventType const kCause, __int64 const i64Cash, __int64 const i64Bonus, BM::GUID const & kOwnerGuid,
	PgTranPointer &rkTran)
{
	PgCashShopBuyTran* pkTran = dynamic_cast<PgCashShopBuyTran*>(rkTran.GetTran());
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgCashShopBuyTran*> failed"));
		return CSR_SYSTEM_ERROR;
	}

	__int64 i64BonusUse = 0;
	BM::GUID kCashShopTableKey;

	CONT_REQ_BUY_ARTICLE kContBuyArticle;
	pkTran->AddedPacket().Pop(kContBuyArticle);
	pkTran->AddedPacket().Pop(i64BonusUse);
	pkTran->AddedPacket().Pop(kCashShopTableKey);

	if( kContBuyArticle.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Empty BuyArticle Container BuyType<")<<pkTran->BuyType()<<_T("> OwnerGuid<")<<kOwnerGuid<<_T(">"));
		return CSR_INVALID_ARTICLE;
	}

	BM::GUID kShopUserGuid;
	pkTran->AddedPacket().Pop(kShopUserGuid);

	if(false == IsEnableUseBonus(i64BonusUse))
	{
		return CSR_NOT_ENOUGH_BONUS;
	}

	if(kCashShopTableKey != g_kControlDefMgr.StoreValueKey())
	{
		return CSR_NEED_RELOAD_DATA;
	}

	if((0 > i64BonusUse) || (i64Bonus < i64BonusUse))
	{
		return CSR_NOT_ENOUGH_BONUS;
	}

	TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_DEF_CASH_SHOP_ARTICLE const * pCont = kObjLock.Get();

	if(!pCont)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	__int64 i64Use = 0, i64AddBonus = 0, i64TotalUseCash = 0, i64TotalUseBonus = i64BonusUse;

	CONT_PLAYER_MODIFY_ORDER kOrder;

	BM::PgPackedTime kCurTime;
	kCurTime.SetLocalTime();
	BM::PgPackedTime kEmpty;
	kEmpty.Clear();

	CONT_PRICE_RECORD kContRecord;
	CONT_BUYITEM kContPriceInfo;

	int iEquipIAritcleCount = 0;

	GET_DEF(CItemDefMgr, kItemDefMgr);

	for(CONT_REQ_BUY_ARTICLE::iterator articleiter = kContBuyArticle.begin();articleiter != kContBuyArticle.end();++articleiter)
	{
		SBuyItemInfo kGalaPrice;
		CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find((*articleiter).iArticleIdx);
		if(iter == pCont->end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
			return CSR_INVALID_ARTICLE;
		}

		CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;

		if(0 != kArticle.bState)
		{
			return CSR_INVALID_ARTICLE;
		}

		if(!(kEmpty == kArticle.kSaleStartDate) && !(kEmpty == kArticle.kSaleEndDate))
		{
			if((kCurTime < kArticle.kSaleStartDate) || (kArticle.kSaleEndDate < kCurTime))
			{
				return CSR_NOT_DATE_LIMIT;
			}
		}

		if( 0 < pkTran->ItemIdx() )
		{
			if( 0 != pkTran->ItemTotalCount() )		// 전체 수량이 0이면 무한 판매
			{
				if( 0 >= pkTran->ItemLimitCount() )
				{
					return CSR_NOT_BUY_LIMIT_OVEW;
				}
			}

			if( pkTran->ItemBuyCount() >= pkTran->ItemBuyLimitCount() )
			{
				return CSR_NOT_BUY_LIMIT;
			}
		}

		CONT_CASH_SHOP_ITEM_PRICE::const_iterator priceiter = kArticle.kContCashItemPrice.find(TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY((*articleiter).bTimeType,(*articleiter).iUseTime));
		if(priceiter == kArticle.kContCashItemPrice.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
			return CSR_INVALID_ARTICLE;
		}

		CONT_CASH_SHOP_ITEM_PRICE::mapped_type const & kPrice = (*priceiter).second;
		if(kPrice.IsDiscount)
		{
			i64Use = kPrice.iDiscountCash;
		}
		else
		{
			i64Use = kPrice.iCash;
		}

		kGalaPrice.iItemNo = kArticle.iIdx;
		kGalaPrice.strItemName = kArticle.wstrName;
		kGalaPrice.iPriceIdx = kPrice.iPriceIdx;

		SPRICE kPriceRecord;

		kPriceRecord.iPriceIdx = kPrice.iPriceIdx;
		kPriceRecord.iTimeType = static_cast<int>((*articleiter).bTimeType);
		kPriceRecord.iUseTime = (*articleiter).iUseTime;

		if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
		{
			i64TotalUseCash += i64Use;
			
			const __int64 i64UnitCoin = i64Cash + i64Bonus;
			
			if( i64UnitCoin < i64TotalUseCash )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
				return CSR_NOT_ENOUGH_CASH;
			}

			kPriceRecord.i64UseCash = i64Use;
			kGalaPrice.i64Cash = i64Use;
		}
		else
		{
			if(false == kPrice.IsOnlyMileage)
			{
				i64AddBonus += CalcAddBonus(i64Use,kArticle.iMileage);

				i64TotalUseCash += i64Use;

				if((i64Cash + i64BonusUse) < i64TotalUseCash)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
					return CSR_NOT_ENOUGH_CASH;
				}

				kPriceRecord.i64UseCash = i64Use;
				kGalaPrice.i64Cash = i64Use;

			}
			else
			{
				i64TotalUseBonus += i64Use;

				if(i64Bonus < i64TotalUseBonus)
				{
					return CSR_NOT_ENOUGH_BONUS;
				}

				kPriceRecord.i64UseMileage = i64Use;
				kGalaPrice.i64Mileage = i64Use;
			}
		}
		
		kGalaPrice.iItemHour = static_cast<int>( PgBase_Item::CalcTimeToSec(static_cast<EUseItemType>(kPrice.bTimeType), kPrice.iItemUseTime) / 3600i64 );

		CONT_CASH_SHOP_ITEM const & kContItem = kArticle.kContCashItem;

		bool bHasEquipItem = false;

		// 사용가능한 인벤 크기 선 체크
		if( !kContItem.empty() )
		{
			TBL_DEF_CASH_SHOP_ITEM kBuyArticle = kContItem.at(0);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(kBuyArticle.iItemNo);

			if( pItemDef )
			{
				int iBaseContainerPos = pItemDef->PrimaryInvType();

				PgDoc_Player kPlayer;
				if( g_kRealmUserMgr.Locked_GetDoc_Player( kOwnerGuid, false, kPlayer ) )
				{
					PgInventory* pkInven = kPlayer.GetInven();
					if( pkInven )
					{
						const int iEnableSize = pkInven->GetEmptyPosCount(static_cast<EInvType>(iBaseContainerPos));
						if( iEnableSize < kContItem.size() )
						{
							return CSR_BUY_INVENTORY_FULL;
						}
					}
				}
			}
		}

		for(CONT_CASH_SHOP_ITEM::const_iterator itemiter = kContItem.begin();itemiter != kContItem.end();++itemiter)
		{
			CItemDef const *pItemDef = kItemDefMgr.GetDef((*itemiter).iItemNo);
			if(NULL == pItemDef)
			{
				return CSR_INVALID_ARTICLE;
			}

			if(pItemDef->CanEquip())
			{
				bHasEquipItem = true;
			}

			if(UIT_COUNT == kPrice.bTimeType)
			{
				if(true == pItemDef->IsAmountItem())
				{
					int iTotalCount = (kPrice.iItemUseTime * (*itemiter).iItemCount);
					int iGenCount = iTotalCount / pItemDef->MaxAmount();
					int iLeftCount = iTotalCount % pItemDef->MaxAmount();

					for(int i = 0;i < iGenCount;++i)
					{
						PgBase_Item kItem;
						if(S_OK != CreateSItem((*itemiter).iItemNo, pItemDef->MaxAmount(), PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
						{
							return CSR_INVALID_ARTICLE;
						}

						tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
						kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
					}

					if(0 < iLeftCount)
					{
						PgBase_Item kItem;
						if(S_OK != CreateSItem((*itemiter).iItemNo, iLeftCount, PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
						{
							return CSR_INVALID_ARTICLE;
						}

						tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
						kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
					}
				}
				else
				{
					for(int i = 0;i < kPrice.iItemUseTime; ++i)
					{
						PgBase_Item kItem;
						if(S_OK != CreateSItem((*itemiter).iItemNo, (*itemiter).iItemCount, PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
						{
							return CSR_INVALID_ARTICLE;
						}

						tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
						kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
					}
				}
			}
			else
			{
				PgBase_Item kItem;
				if(S_OK != CreateSItem((*itemiter).iItemNo, (*itemiter).iItemCount,PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
					return CSR_INVALID_ARTICLE;
				}

				if(kPrice.iItemUseTime > 0)
				{
					if(!kItem.SetUseTime(kPrice.bTimeType,kPrice.iItemUseTime))
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
						return CSR_INVALID_ARTICLE;
					}
				}

				tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
				kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
			}
		}

		if(true == bHasEquipItem)
		{
			++iEquipIAritcleCount;
		}

		kContRecord.push_back(kPriceRecord);
		kContPriceInfo.push_back(kGalaPrice);
	}

	__int64 const i64Cost = i64TotalUseCash - i64BonusUse;
	if(i64Cost < 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
		return CSR_NOT_ENOUGH_CASH;
	}

	i64AddBonus += CalcLocalAddBonus(kOwnerGuid,kShopUserGuid,i64Cost);

	pkTran->ShopGuid(kShopUserGuid);
	pkTran->Cost(i64Cost);
	pkTran->Bonus(i64AddBonus);
	pkTran->BonusUse(i64TotalUseBonus);
	pkTran->SetBuyItem(kContPriceInfo);
	pkTran->AddedPacket().Reset();
	kOrder.WriteToPacket(pkTran->AddedPacket());

	// 물품 구매 로그
	PgDoc_Player rkCopyPlayer;
	if(g_kRealmUserMgr.Locked_GetDoc_Player(kOwnerGuid,false,rkCopyPlayer))
	{
		PgLogCont kLogCont(ELogMain_Cash,ELogSub_Cash_Buy,rkCopyPlayer.GetMemberGUID(), 
			rkCopyPlayer.GetID(),rkCopyPlayer.MemberID(),rkCopyPlayer.Name(),
			rkCopyPlayer.GetAbil(AT_CLASS),rkCopyPlayer.GetAbil(AT_LEVEL),
			rkCopyPlayer.GroundKey().GroundNo());
		kLogCont.UID(rkCopyPlayer.UID());

		CONT_REQ_BUY_ARTICLE::iterator articleiter = kContBuyArticle.begin();
		CONT_PRICE_RECORD::iterator recorditer = kContRecord.begin();

		for(;articleiter != kContBuyArticle.end(), recorditer != kContRecord.end();++articleiter,++recorditer)
		{
			CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find((*articleiter).iArticleIdx);
			if(iter != pCont->end())
			{
				CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;
				PgLog kLog(ELOrderMain_Product,ELOrderSub_Create);
				kLog.Set(0,kArticle.wstrName);
				kLog.Set(1,rkCopyPlayer.Name());
				kLog.Set(2,rkCopyPlayer.GetID().str().c_str());
				kLog.Set(3,kShopUserGuid.str().c_str());
				kLog.Set(0,kArticle.iIdx);
				kLog.Set(1,(*recorditer).iPriceIdx);		// 아이템 가격 인덱스
				kLog.Set(2,(*recorditer).iTimeType);		// 기간 타입
				kLog.Set(3,(*recorditer).iUseTime);			// 사용 기간
				kLog.Set(0,(*recorditer).i64UseCash);		// 각 상품별 사용 캐시
				kLog.Set(1,(*recorditer).i64UseMileage);	// 각 상품별 사용 마일리지
				kLogCont.Add(kLog);
			}
		}

		/*
		{
			PgLog kLog(ELOrderMain_Mileage,ELOrderSub_Modify);
			kLog.Set(0,i64AddBonus);
			kLog.Set(1,i64Bonus);
			kLog.Set(2,(i64Bonus+i64AddBonus)-i64TotalUseBonus);
			kLogCont.Add(kLog);
		}


		{
			PgLog kLog(ELOrderMain_Cash,ELOrderSub_Modify);
			kLog.Set(0,i64Cost);
			kLog.Set(1,i64Cash);
			kLog.Set(2,i64Cash - i64Cost);
			kLogCont.Add(kLog);
		}
		*/

		pkTran->LogCont(kLogCont);
	}

	if (true == rkTran.RequestPayCash())
	{
		return CSR_SUCCESS;
	}
	return CSR_SYSTEM_ERROR;
}

ECashShopResult PgCashShopMgr::OnProcessReqGiftArticle(EItemModifyParentEventType const kCause,__int64 const i64Cash,__int64 const i64Bonus,BM::GUID const & kOwnerGuid,
	PgTranPointer &rkTran)
{
	PgSendGiftTran* pkTran = dynamic_cast<PgSendGiftTran*>(rkTran.GetTran());
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgSendGiftTran*> failed"));
		return CSR_SYSTEM_ERROR;
	}

	//SERVER_IDENTITY kSI;
	//SGroundKey kGndKey;

	//BM::GUID kMemberGuId;

	//kSI.ReadFromPacket(rkPacket);
	//rkPacket.Pop(kGndKey);
	//rkPacket.Pop(kMemberGuId);

	int iArticleIdx = 0,
		iItemUseTime = 0;
	BYTE bTimeType = 0;

	std::wstring	kSenderName,
					kRecverName,
					kComment;
	__int64 i64BonusUse = 0;
	BM::GUID		kCashShopTableKey;
	BM::GUID kShopUserGuid;

	pkTran->AddedPacket().Pop(kSenderName);
	pkTran->AddedPacket().Pop(iArticleIdx);
	pkTran->AddedPacket().Pop(bTimeType);
	pkTran->AddedPacket().Pop(iItemUseTime);
	pkTran->AddedPacket().Pop(kRecverName);
	pkTran->AddedPacket().Pop(kComment);
	pkTran->AddedPacket().Pop(i64BonusUse);
	pkTran->AddedPacket().Pop(kCashShopTableKey);
	pkTran->AddedPacket().Pop(kShopUserGuid);

	int iUID = 0;
	SContentsUser rkRecvContentsUser;
	if( S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo( kRecverName, rkRecvContentsUser ) )
	{// 받는 유저의 UID ( NC, Gravity 사용 )
		iUID = rkRecvContentsUser.iUID;
	}

	if(false == IsEnableUseBonus(i64BonusUse))
	{
		return CSR_NOT_ENOUGH_BONUS;
	}

	if(kCashShopTableKey != g_kControlDefMgr.StoreValueKey())
	{
		return CSR_NEED_RELOAD_DATA;
	}

	bool const bNameFiltered = g_kFilterString.Filter(kRecverName, false, FST_ALL);
	bool const bNameUniFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_CHARACTER_NAME, kRecverName));
	if( bNameFiltered
	||	bNameUniFiltered )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_USE_BADWORD"));
		return CSR_USE_BADWORD;
	}

	bool const bCommentFiltered = g_kFilterString.Filter(kComment, false, FST_ALL);
	bool const bCommentUniFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_CASHSHOP_PRESENT_COMMENT, kComment));
	if( bCommentFiltered
	||	bCommentUniFiltered )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_USE_BADWORD"));
		return CSR_USE_BADWORD;
	}

	TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_DEF_CASH_SHOP_ARTICLE const * pCont = kObjLock.Get();

	if(!pCont)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find(iArticleIdx);
	if(iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_IVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;

	BM::PgPackedTime kCurTime;
	kCurTime.SetLocalTime();
	BM::PgPackedTime kEmpty;
	kEmpty.Clear();

	if(0 != kArticle.bState)
	{
		return CSR_INVALID_ARTICLE;
	}

	if(!(kEmpty == kArticle.kSaleStartDate) && !(kEmpty == kArticle.kSaleEndDate))
	{
		if((kCurTime < kArticle.kSaleStartDate) || (kArticle.kSaleEndDate < kCurTime))
		{
			return CSR_NOT_DATE_LIMIT;
		}
	}

	if( 0 < pkTran->ItemIdx() )
	{
		if( 0 != pkTran->ItemTotalCount() )		// 전체 수량이 0이면 무한 판매
		{
			if( 0 >= pkTran->ItemLimitCount() )
			{
				return CSR_NOT_BUY_LIMIT_OVEW;
			}
		}

		if( pkTran->ItemBuyCount() >= pkTran->ItemBuyLimitCount() )
		{
			return CSR_NOT_BUY_LIMIT;
		}
	}

	CONT_CASH_SHOP_ITEM_PRICE::const_iterator priceiter = kArticle.kContCashItemPrice.find(TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(bTimeType,iItemUseTime));
	if(priceiter == kArticle.kContCashItemPrice.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	__int64 i64Price = 0;

	CONT_CASH_SHOP_ITEM_PRICE::mapped_type const & kPrice = (*priceiter).second;

	if(kPrice.IsDiscount)
	{
		i64Price = kPrice.iDiscountCash;
	}
	else
	{
		i64Price = kPrice.iCash;
	}

	__int64 i64Cost = 0;
	__int64 i64AddBonus = 0;

	if(false == kPrice.IsOnlyMileage)
	{
		if((0 > i64BonusUse) || (i64Bonus < i64BonusUse))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
			return CSR_NOT_ENOUGH_BONUS;
		}

		if((i64BonusUse + i64Cash) < i64Price)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
			return CSR_NOT_ENOUGH_CASH;
		}

		i64Cost = i64Price - i64BonusUse;

		if(0 > i64Cost)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
			return CSR_NOT_ENOUGH_CASH;
		}

		i64AddBonus += CalcAddBonus(i64Price,kArticle.iMileage);
		i64AddBonus += CalcLocalAddBonus(kOwnerGuid,kShopUserGuid,i64Cost);
	}
	else
	{
		if(i64Bonus < i64Price)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_BONUS"));
			return CSR_NOT_ENOUGH_BONUS;
		}

		i64BonusUse = i64Price;
	}

	/*
	CEL::DB_QUERY kQuery(DT_PLAYER,DQT_CS_ADD_GIFT,L"EXEC [dbo].[UP_CS_ADDGIFT2]");
	kQuery.InsertQueryTarget(kOwnerGuid);
	kQuery.QueryOwner(kOwnerGuid);

	kQuery.PushStrParam(kOwnerGuid);
	kQuery.PushStrParam(kSenderName);
	kQuery.PushStrParam(kRecverName);
	kQuery.PushStrParam(kComment);
	kQuery.PushStrParam(iArticleIdx);

	kQuery.PushStrParam(bTimeType);
	kQuery.PushStrParam(iItemUseTime);
	*/

	pkTran->ShopGuid(kShopUserGuid);
	pkTran->Cost(i64Cost);
	pkTran->Bonus(i64AddBonus);
	pkTran->BonusUse(i64BonusUse);
	pkTran->SenderName(kSenderName);
	pkTran->ArticleName(kArticle.wstrName);
	pkTran->ReceiverName(kRecverName);
	pkTran->ArticleIndex(kArticle.iIdx);
	pkTran->PriceIdx(kPrice.iPriceIdx);
	pkTran->TimeType(static_cast<int>(bTimeType));
	pkTran->UseTime(iItemUseTime);
	pkTran->Comment(kComment);
	pkTran->ReceiverUID(iUID);


	SPlayerModifyOrderData_ModifyCashShopGift kGiftOrder(kRecverName);
	CONT_PLAYER_MODIFY_ORDER kContOrder;
	kContOrder.push_back(SPMO(IMET_CASH_SHOP_GIFT, pkTran->CharacterGuid(), kGiftOrder));
	pkTran->AddedPacket().Reset();	// AddedPacket 비워주고....
	kContOrder.WriteToPacket(pkTran->AddedPacket());

	// 선물 로그
	PgDoc_Player rkCopyPlayer;
	if(g_kRealmUserMgr.Locked_GetDoc_Player(kOwnerGuid,false,rkCopyPlayer))
	{
		PgLogCont kLogCont(ELogMain_Cash,ELogSub_Cash_Present,rkCopyPlayer.GetMemberGUID(), 
			rkCopyPlayer.GetID(),rkCopyPlayer.MemberID(),rkCopyPlayer.Name(),
			rkCopyPlayer.GetAbil(AT_CLASS),rkCopyPlayer.GetAbil(AT_LEVEL),
			rkCopyPlayer.GroundKey().GroundNo());
		kLogCont.UID(rkCopyPlayer.UID());

		{
			PgLog kLog(ELOrderMain_Product,ELOrderSub_Create);
			kLog.Set(0,kArticle.wstrName);
			kLog.Set(1,kRecverName);
			kLog.Set(2,rkCopyPlayer.GetID().str());
			kLog.Set(3,kShopUserGuid.str().c_str());

			kLog.Set(0,kArticle.iIdx);
			kLog.Set(1,kPrice.iPriceIdx);		// 아이템 가격 인덱스
			kLog.Set(2,static_cast<int>(bTimeType));
			kLog.Set(3,iItemUseTime);
			kLog.Set(0,i64Cost);
			kLog.Set(1,i64BonusUse);

			kLogCont.Add(kLog);
		}
		/*
		{
			PgLog kLog(ELOrderMain_Mileage,ELOrderSub_Modify);
			kLog.Set(0,i64AddBonus);		// 마일리지 적립
			kLog.Set(1,i64Bonus);
			kLog.Set(2,(i64Bonus + i64AddBonus) - i64BonusUse);
			kLogCont.Add(kLog);
		}

		{
			PgLog kLog(ELOrderMain_Cash,ELOrderSub_Modify);
			kLog.Set(0,i64Cost);
			kLog.Set(1,i64Cash);
			kLog.Set(2,i64Cash - i64Cost);
			kLogCont.Add(kLog);
		}
		*/

		pkTran->LogCont(kLogCont);
	}

	if (true == rkTran.RequestPayCash())
	{
		return CSR_SUCCESS;
	}
	return CSR_SYSTEM_ERROR;
}

ECashShopResult PgCashShopMgr::OnProcessReqGiftArticle(PgTranPointer &rkTran)
{
	PgSendGiftTran* pkTran = dynamic_cast<PgSendGiftTran*>(rkTran.GetTran());
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgSendGiftTran*> failed"));
		return CSR_SYSTEM_ERROR;
	}

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CS_ADD_GIFT, L"EXEC [dbo].[UP_CS_ADDGIFT2]");
	kQuery.InsertQueryTarget(pkTran->CharacterGuid());
	kQuery.QueryOwner(pkTran->CharacterGuid());

	kQuery.PushStrParam(pkTran->CharacterGuid());
	kQuery.PushStrParam(pkTran->SenderName());
	kQuery.PushStrParam(pkTran->ReceiverName());
	kQuery.PushStrParam(pkTran->Comment());
	kQuery.PushStrParam(pkTran->ArticleIndex());
	kQuery.PushStrParam(static_cast<BYTE>(pkTran->TimeType()));
	kQuery.PushStrParam(pkTran->UseTime());
	pkTran->WriteToPacket(kQuery.contUserData);
	if (S_OK != g_kCoreCenter.PushQuery(kQuery))
	{
		return CSR_SYSTEM_ERROR;
	}
	return CSR_SUCCESS;
}

ECashShopResult PgCashShopMgr::OnProcessReqRecvGift(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey,BM::GUID const & kOwnerGuid,SCASHGIFTINFO const & kGift)
{
	EItemModifyParentEventType const kCause = CIE_CS_Gift_Recv;

	TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_DEF_CASH_SHOP_ARTICLE const * pCont = kObjLock.Get();

	if(!pCont)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find(kGift.iCashItemIdx);
	if(iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;

	CONT_CASH_SHOP_ITEM_PRICE::const_iterator priceiter = kArticle.kContCashItemPrice.find(TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(kGift.bTimeType,kGift.iUseTime));
	if(priceiter == kArticle.kContCashItemPrice.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
		return CSR_INVALID_ARTICLE;
	}

	CONT_CASH_SHOP_ITEM_PRICE::mapped_type const & kPrice = (*priceiter).second;
	CONT_CASH_SHOP_ITEM const & kContItem = kArticle.kContCashItem;

	CONT_PLAYER_MODIFY_ORDER kOrder;
	GET_DEF(CItemDefMgr, kItemDefMgr);

	for(CONT_CASH_SHOP_ITEM::const_iterator itemiter = kContItem.begin();itemiter != kContItem.end();++itemiter)
	{
		CItemDef const *pItemDef = kItemDefMgr.GetDef((*itemiter).iItemNo);
		if(NULL == pItemDef)
		{
			return CSR_INVALID_ARTICLE;
		}

		if(UIT_COUNT == kPrice.bTimeType)
		{
			if(true == pItemDef->IsAmountItem())
			{
				int iTotalCount = (kPrice.iItemUseTime * (*itemiter).iItemCount);
				int iGenCount = iTotalCount / pItemDef->MaxAmount();
				int iLeftCount = iTotalCount % pItemDef->MaxAmount();

				for(int i = 0;i < iGenCount;++i)
				{
					PgBase_Item kItem;
					if(S_OK != CreateSItem((*itemiter).iItemNo, pItemDef->MaxAmount(), PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
					{
						return CSR_INVALID_ARTICLE;
					}

					tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
					kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
				}

				if(0 < iLeftCount)
				{
					PgBase_Item kItem;
					if(S_OK != CreateSItem((*itemiter).iItemNo, iLeftCount, PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
					{
						return CSR_INVALID_ARTICLE;
					}

					tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
					kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
				}
			}
			else
			{
				for(int i = 0;i < kPrice.iItemUseTime; ++i)
				{
					PgBase_Item kItem;
					if(S_OK != CreateSItem((*itemiter).iItemNo, (*itemiter).iItemCount, PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
					{
						return CSR_INVALID_ARTICLE;
					}

					tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
					kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
				}
			}
		}
		else
		{
			PgBase_Item kItem;
			if(S_OK != CreateSItem((*itemiter).iItemNo, (*itemiter).iItemCount,PgItemRarityUpgradeFormula::GetItemRarityContorolType(kCause), kItem))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
				return CSR_INVALID_ARTICLE;
			}

			if(kPrice.iItemUseTime > 0)
			{
				if(!kItem.SetUseTime(kPrice.bTimeType,kPrice.iItemUseTime))
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
					return CSR_INVALID_ARTICLE;
				}
			}

			tagPlayerModifyOrderData_Insert_Fixed kIMO(kItem, SItemPos(0,0),true);
			kOrder.push_back(SPMO(IMET_INSERT_FIXED,kOwnerGuid,kIMO));
		}
	}

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = kCause;
	pkActionOrder->kContOrder = kOrder;
	pkActionOrder->kPacket2nd.Push(kCause);
	pkActionOrder->kPacket2nd.Push(kOwnerGuid);
	pkActionOrder->kPacket2nd.Push(kGift.kGiftGuId);
	g_kJobDispatcher.VPush(pkActionOrder);

	PgDoc_Player rkCopyPlayer;
	if(g_kRealmUserMgr.Locked_GetDoc_Player(kOwnerGuid,false,rkCopyPlayer))
	{
		PgLogCont kLogCont(ELogMain_Cash,ELogSub_Cash_Receipt,rkCopyPlayer.GetMemberGUID(), 
			rkCopyPlayer.GetID(),rkCopyPlayer.MemberID(),rkCopyPlayer.Name(),
			rkCopyPlayer.GetAbil(AT_CLASS),rkCopyPlayer.GetAbil(AT_LEVEL),
			rkCopyPlayer.GroundKey().GroundNo());
		kLogCont.UID(rkCopyPlayer.UID());

		PgLog kLog(ELOrderMain_Product,ELOrderSub_Delete);
		kLog.Set(0,kArticle.wstrName);
		kLog.Set(0,kArticle.iIdx);
		kLogCont.Add(kLog);

		kLogCont.Commit();
	}

	return CSR_SUCCESS;
}

ECashShopResult PgCashShopMgr::OnProcessReqAddTimeLimit(EItemModifyParentEventType const kCause,__int64 const i64Cash,__int64 const i64Bonus,
	BM::GUID const & kOwnerGuid,PgTranPointer &rkTran)
{
	PgExpandTimeLimitTran *const pkTimeTran = dynamic_cast<PgExpandTimeLimitTran *>(rkTran.GetTran());
	if (pkTimeTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("dynamic_cast failed PgTransaction"));
		return CSR_SYSTEM_ERROR;
	}

	int							iArticleIdx = 0;
	BYTE						bTimeType = 0;
	int							iUseTime = 0;
	__int64						i64BonusUse = 0;
	BM::GUID					kCashShopTableKey;
	CONT_PLAYER_MODIFY_ORDER	kOrder;

	BM::Stream& rkAddedPacket = pkTimeTran->AddedPacket();
	rkAddedPacket.Pop(iArticleIdx);
	rkAddedPacket.Pop(bTimeType);
	rkAddedPacket.Pop(iUseTime);
	rkAddedPacket.Pop(i64BonusUse);
	rkAddedPacket.Pop(kCashShopTableKey);

	BM::GUID kShopUserGuid;
	rkAddedPacket.Pop(kShopUserGuid);
	kOrder.ReadFromPacket(rkAddedPacket);

	if(false == IsEnableUseBonus(i64BonusUse))
	{
		return CSR_NOT_ENOUGH_BONUS;
	}

	if(kCashShopTableKey != g_kControlDefMgr.StoreValueKey())
	{
		return CSR_NEED_RELOAD_DATA;
	}

	TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_DEF_CASH_SHOP_ARTICLE const * pCont = kObjLock.Get();

	if(!pCont)
	{
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find(iArticleIdx);
	if(iter == pCont->end())
	{
		return CSR_INVALID_ARTICLE;
	}

	CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;

	CONT_CASH_SHOP_ITEM_PRICE::const_iterator priceiter = kArticle.kContCashItemPrice.find(TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(bTimeType,iUseTime));
	if(priceiter == kArticle.kContCashItemPrice.end())
	{
		return CSR_INVALID_ARTICLE;
	}

	CONT_CASH_SHOP_ITEM_PRICE::mapped_type const & kPrice = (*priceiter).second;

	if(i64BonusUse > i64Bonus)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
		return CSR_NOT_ENOUGH_BONUS;
	}

	__int64 i64Price = 0;

	if(true == kPrice.IsExtendDiscount)
	{
		i64Price = kPrice.iDiscountExtendCash;
	}
	else
	{
		i64Price = kPrice.iExtendCash;
	}

	__int64 i64AddBonus = 0;
	__int64 i64Cost = 0;

	if( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_KOREA ) )
	{
		const __int64 i64UnitCoin = i64Cash + i64Bonus;

		if( i64UnitCoin < i64Price )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
			return CSR_NOT_ENOUGH_CASH;
		}

		i64Cost = i64Price;
	}
	else
	{
		if(false == kPrice.IsOnlyMileage)
		{
			if((i64Cash + i64BonusUse) < i64Price)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
				return CSR_NOT_ENOUGH_CASH;
			}

			i64Cost = i64Price - i64BonusUse;
			if(0 > i64Cost)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_NOT_ENOUGH_CASH"));
				return CSR_NOT_ENOUGH_CASH;
			}

			i64AddBonus += CalcAddBonus(i64Price,kArticle.iMileage);
			i64AddBonus += CalcLocalAddBonus(kOwnerGuid,kShopUserGuid,i64Cost);	// 기간 연장 대만 마일리지 적립
		}
		else
		{
			i64BonusUse = i64Price;
			i64Cost = 0;
		}
	}

	SBuyItemInfo kGalaPrice;
	kGalaPrice.iPriceIdx = kPrice.iPriceIdx;
	kGalaPrice.iItemNo = iArticleIdx;
	kGalaPrice.strItemName = kArticle.wstrName;
	kGalaPrice.iItemHour = static_cast<int>( PgBase_Item::CalcTimeToSec(static_cast<EUseItemType>(kPrice.bTimeType), kPrice.iItemUseTime) / 3600i64 );
	kGalaPrice.i64Cash = i64Cost;
	kGalaPrice.i64Mileage = i64BonusUse;
	CONT_BUYITEM kContBuy;
	kContBuy.push_back(kGalaPrice);

	pkTimeTran->ShopGuid(kShopUserGuid);
	pkTimeTran->Cost(i64Cost);
	pkTimeTran->BonusUse(i64BonusUse);
	pkTimeTran->Bonus(i64AddBonus);
	pkTimeTran->SetBuyItem(kContBuy);
	pkTimeTran->AddedPacket().Reset();
	kOrder.WriteToPacket(pkTimeTran->AddedPacket());


	PgDoc_Player rkCopyPlayer;
	if(g_kRealmUserMgr.Locked_GetDoc_Player(kOwnerGuid,false,rkCopyPlayer))
	{
		PgLogCont kLogCont(ELogMain_Cash,ELogSub_Cash_AddTime,rkCopyPlayer.GetMemberGUID(), 
			rkCopyPlayer.GetID(),rkCopyPlayer.MemberID(),rkCopyPlayer.Name(),
			rkCopyPlayer.GetAbil(AT_CLASS),rkCopyPlayer.GetAbil(AT_LEVEL),
			rkCopyPlayer.GroundKey().GroundNo());
		kLogCont.UID(rkCopyPlayer.UID());

		{
			PgLog kLog(ELOrderMain_Product,ELOrderSub_Modify);
			kLog.Set(0,kArticle.wstrName);
			kLog.Set(1, rkCopyPlayer.Name());
			kLog.Set(2, rkCopyPlayer.GetID().str());
			kLog.Set(3, kShopUserGuid.str().c_str());

			kLog.Set(0,kArticle.iIdx);
			kLog.Set(1,kPrice.iPriceIdx);
			kLog.Set(2,static_cast<int>(bTimeType));
			kLog.Set(3,iUseTime);
			kLog.Set(0,i64Cost);
			kLog.Set(1,i64BonusUse);

			kLogCont.Add(kLog);
		}

		/*
		{
			PgLog kLog(ELOrderMain_Mileage,ELOrderSub_Modify);
			kLog.Set(0,i64AddBonus);
			kLog.Set(1,i64Bonus);
			kLog.Set(2,(i64Bonus+i64AddBonus)-i64BonusUse);
			kLogCont.Add(kLog);
		}

		{
			PgLog kLog(ELOrderMain_Cash,ELOrderSub_Modify);
			kLog.Set(0,i64Cost);
			kLog.Set(1,i64Cash);
			kLog.Set(2,i64Cash - i64Cost);
			kLogCont.Add(kLog);
		}
		*/

		pkTimeTran->LogCont(kLogCont);
	}

	if (true == rkTran.RequestPayCash())
	{
		return CSR_SUCCESS;
	}
	return CSR_SYSTEM_ERROR;
}
