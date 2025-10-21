#include "stdafx.h"
#include "Lohengrin/PacketStruct.h"
#include "AilePack/Nival_constant.h"
#include "AilePack/NC_Constant.h"
#include "Transaction.h"
#include "JobWorker.h"
#include "JobDispatcher.h"
#include "PgServerSetMgr.h"
#include "Item/PgCashShopManager.h"
#include "Item/CashManager.h"

////////////////////////////////////////////////////////////
// PgTransaction
//

PgTransaction::PgTransaction(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: m_kMemberGuid(_Member), m_kCharacterGuid(_Character), m_kGroundKey(_Ground), m_kTargetSI(_Target), m_kEventType(_Event)
{
	TransactionKey(BM::GUID::Create());
	m_i64CreateTime = BM::GetTime64();
	UID(0);
	InitCash(0);
	InitMileage(0);
	CurCash(0);
	CurMileage(0);
	AddedMileage(0);
	TranState(ETranState_None);
}

PgTransaction::PgTransaction(BM::Stream & _Packet)
{
	PgTransaction::ReadFromPacket(_Packet);
}

PgTransaction::PgTransaction()
{
	m_kTargetSI.Clear();
	EventType(IMEPT_NONE);
	UID(0);
	InitCash(0);
	InitMileage(0);
	CurCash(0);
	CurMileage(0);
	AddedMileage(0);
	OpenMarketDealingCash(0);
	m_i64CreateTime = BM::GetTime64();
	TranState(ETranState_None);
	WarehouseNo(0);
	ItemLimitCount(0);
	ItemBuyLimitCount(0);
	ItemBuyCount(0);
	ItemIdx(0);
	ItemTotalCount(0);
}

PgTransaction::~PgTransaction()
{
};

PgTransaction const& PgTransaction::operator=(PgTransaction const & rhs)
{
	TransactionKey(rhs.TransactionKey());
	MemberGuid(rhs.MemberGuid());
	CharacterGuid(rhs.CharacterGuid());
	GroundKey(rhs.GroundKey());
	TargetSI(rhs.TargetSI());
	EventType(rhs.EventType());
	UID(rhs.UID());
	m_i64CreateTime = rhs.m_i64CreateTime;
	AccountID(rhs.AccountID());
	RemoteAddr(rhs.RemoteAddr());
	InitCash(rhs.InitCash());
	InitMileage(rhs.InitMileage());
	CurCash(rhs.CurCash());
	CurMileage(rhs.CurMileage());
	AddedMileage(rhs.AddedMileage());
	OpenMarketDealingCash(rhs.OpenMarketDealingCash());
	m_kAddedPacket = rhs.m_kAddedPacket;
	m_kResultPacket = rhs.m_kResultPacket;
	m_kLogCont = rhs.m_kLogCont;
	TranState(rhs.TranState());
	ChargeNo(rhs.ChargeNo());
	WarehouseNo(rhs.WarehouseNo());
	ItemLimitCount(rhs.ItemLimitCount());
	ItemBuyLimitCount(rhs.ItemBuyLimitCount());
	ItemBuyCount(rhs.ItemBuyCount());
	ItemIdx(rhs.ItemIdx());
	ItemTotalCount(rhs.ItemTotalCount());
	return (*this);
}

void PgTransaction::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push(GetTransactionType());
	kPacket.Push(TransactionKey());
	kPacket.Push(MemberGuid());
	kPacket.Push(CharacterGuid());
	GroundKey().WriteToPacket(kPacket);
	kPacket.Push(TargetSI());
	kPacket.Push(EventType());
	kPacket.Push(UID());
	kPacket.Push(m_i64CreateTime);
	kPacket.Push(AccountID());
	RemoteAddr().WriteToPacket(kPacket);
	kPacket.Push(ShopGuid());
	kPacket.Push(InitCash());
	kPacket.Push(InitMileage());
	kPacket.Push(CurCash());
	kPacket.Push(CurMileage());
	kPacket.Push(AddedMileage());
	kPacket.Push(OpenMarketDealingCash());
	kPacket.Push(m_kAddedPacket.Data());
	kPacket.Push(m_kResultPacket.Data());
	m_kLogCont.WriteToPacket(kPacket);
	kPacket.Push(TranState());
	kPacket.Push(ChargeNo());
	kPacket.Push(WarehouseNo());	
	kPacket.Push(ItemLimitCount());
	kPacket.Push(ItemBuyLimitCount());	
	kPacket.Push(ItemBuyCount());	
	kPacket.Push(ItemIdx());
	kPacket.Push(ItemTotalCount());
}

bool PgTransaction::ReadFromPacket(BM::Stream & kPacket)
{
	ECASH_TRAN_TYPE eTran;
	kPacket.Pop(eTran);
	//if (eTran != GetTransactionType())
	//{
	//	VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Transaction type mismatch ObjType=") << static_cast<int>(GetTransactionType())
	//		<< _T(", PacketType=") << static_cast<int>(eTran));
	//}
	kPacket.Pop(m_kTransactionKey);
	kPacket.Pop(m_kMemberGuid);
	kPacket.Pop(m_kCharacterGuid);
	m_kGroundKey.ReadFromPacket(kPacket);
	kPacket.Pop(m_kTargetSI);
	kPacket.Pop(m_kEventType);
	kPacket.Pop(m_kUID);
	kPacket.Pop(m_i64CreateTime);
	kPacket.Pop(m_kAccountID);
	m_kRemoteAddr.ReadFromPacket(kPacket);
	kPacket.Pop(m_kShopGuid);
	kPacket.Pop(m_kInitCash);
	kPacket.Pop(m_kInitMileage);
	kPacket.Pop(m_kCurCash);
	kPacket.Pop(m_kCurMileage);
	kPacket.Pop(m_kAddedMileage);
	kPacket.Pop(m_kOpenMarketDealingCash);
	m_kAddedPacket.Reset();
	kPacket.Pop(m_kAddedPacket.Data());
	m_kAddedPacket.PosAdjust();
	m_kResultPacket.Reset();
	kPacket.Pop(m_kResultPacket.Data());
	m_kResultPacket.PosAdjust();
	m_kLogCont.ReadFromPacket(kPacket);
	kPacket.Pop(m_kTranState);
	kPacket.Pop(m_kChargeNo);
	kPacket.Pop(m_kWarehouseNo);
	kPacket.Pop(m_kItemLimitCount);
	kPacket.Pop(m_kItemBuyLimitCount);
	kPacket.Pop(m_kItemBuyCount);
	kPacket.Pop(m_kItemIdx);
	kPacket.Pop(m_kItemTotalCount);
	return true;
}

void PgTransaction::AddedPacketAssign(BM::Stream const& kPacket, bool const bPosAdjust)
{
	m_kAddedPacket = kPacket;
	if (bPosAdjust)
	{
		m_kAddedPacket.PosAdjust();
	}
}

bool PgTransaction::Commit(bool const bIsCommit, BM::Stream::DEF_STREAM_TYPE const kSubType)
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_JAPAN:
		{
			BM::Stream::DEF_STREAM_TYPE const kType = ((bIsCommit)?PT_A_CN_NFY_TRAN_COMMIT:PT_A_CN_NFY_TRAN_ROLLBACK);

			BM::Stream kGPacket(kType);

			kGPacket.Push(TransactionKey());
			kGPacket.Push(kSubType);
			WriteToPacket(kGPacket);

			GALA::SendCashRequest(kGPacket);//Дї№Ф іЇёІ
		}break;
	case LOCAL_MGR::NC_KOREA:
		{
			if( WarehouseNo() && bIsCommit )
			{
				BM::Stream kPacket(PT_A_CN_REQ_DELETE_ITEMS);
				kPacket.Push(WarehouseNo());
				GALA::SendCashRequest(kPacket);
			}
		}break;
	default:
		{

		}break;
	}
	
	return true;
}

void PgTransaction::Log(BM::vstring &vLogString) const
{
	vLogString << _T("TranGuid=") << TransactionKey() << _T(", Member=") << AccountID() << _COMMA_ << MemberGuid() << _T(", CharacterGuid=") << CharacterGuid()
		<< _T(", EventType=") << EventType() << _T(", ShopGuid=") << ShopGuid() << _T(", TranState=") << TranState() << _T("\r\n")
		<< _T("\t\tInitCash=") << InitCash() << _T(", InitMileage=") << InitMileage() << _T(", CurCash=") << CurCash() << _T(", CurMileage=") << CurMileage()
		<< _T(", AddedMileage=") << AddedMileage() << _T(", ChargeNo=") << ChargeNo() << _T(", WarehouseNo=") << WarehouseNo() << _T(", ItemTotalCount=") << ItemTotalCount() << _T(", ItemLimitCount=") << ItemLimitCount() << _T(", ItemBuyLimitCount=") << ItemBuyLimitCount() << _T(", ItemBuyCount=") << ItemBuyCount() << _T(", ItemIdx=") << ItemIdx();
}

////////////////////////////////////////////////////////////
// PgSendGiftTran
//

PgSendGiftTran::PgSendGiftTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
	Cost(0i64);
	BonusUse(0i64);
	Bonus(0i64);
	ArticleIndex(0);
	TimeType(0);
	UseTime(0);
	PriceIdx(0);
}

PgSendGiftTran::PgSendGiftTran(BM::Stream & _Packet)
{
	PgSendGiftTran::ReadFromPacket(_Packet);
}


PgSendGiftTran::~PgSendGiftTran()
{
};

PgSendGiftTran const& PgSendGiftTran::operator=(PgSendGiftTran const& rhs)
{
	PgTransaction::operator=(rhs);

	CharacterName(rhs.CharacterName());
	Cost(rhs.Cost());
	BonusUse(rhs.BonusUse());
	Bonus(rhs.Bonus());
	SenderName(rhs.SenderName());
	ArticleName(rhs.ArticleName());
	ReceiverName(rhs.ReceiverName());
	ArticleIndex(rhs.ArticleIndex());
	PriceIdx(rhs.PriceIdx());
	TimeType(rhs.TimeType());
	UseTime(rhs.UseTime());
	Comment(rhs.Comment());
	ReceiverUID(rhs.ReceiverUID());

	return (*this);
}

void PgSendGiftTran::Delete()
{
	delete this;
}

void PgSendGiftTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);

	kPacket.Push(CharacterName());
	kPacket.Push(Cost());
	kPacket.Push(BonusUse());
	kPacket.Push(Bonus());
	kPacket.Push(SenderName());
	kPacket.Push(ArticleName());
	kPacket.Push(ReceiverName());
	kPacket.Push(ArticleIndex());
	kPacket.Push(PriceIdx());
	kPacket.Push(TimeType());
	kPacket.Push(UseTime());
	kPacket.Push(Comment());
	kPacket.Push(ReceiverUID());
}

bool PgSendGiftTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);

	kPacket.Pop(m_kCharacterName);
	kPacket.Pop(m_kCost);
	kPacket.Pop(m_kBonusUse);
	kPacket.Pop(m_kBonus);
	kPacket.Pop(m_kSenderName);
	kPacket.Pop(m_kArticleName);
	kPacket.Pop(m_kReceiverName);
	kPacket.Pop(m_kArticleIndex);
	kPacket.Pop(m_kPriceIdx);
	kPacket.Pop(m_kTimeType);
	kPacket.Pop(m_kUseTime);
	kPacket.Pop(m_kComment);
	kPacket.Pop(m_kReceiverUID);
	return true;
}

bool PgSendGiftTran::PayCash()
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_USA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				BM::Stream kGPacket(PT_A_CN_REQ_SENDGIFT);
				kGPacket.Push(UID());
				kGPacket.Push(AccountID());
				RemoteAddr().WriteToPacket(kGPacket);
				kGPacket.Push(CharacterGuid());
				if(LOCAL_MGR::NC_JAPAN==g_kLocal.ServiceRegion())
				{
					std::wstring addIP(CharacterName());
					addIP+=L"/";
					addIP+=RemoteAddr().IP();
					kGPacket.Push(addIP);
				}
				else
				{
					kGPacket.Push(CharacterName());
				}
				kGPacket.Push(ReceiverName());

				if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN))
				{
					kGPacket.Push(PriceIdx());
				}
				else
				{
					kGPacket.Push(ArticleIndex());
				}

				kGPacket.Push(ArticleName());
				kGPacket.Push(static_cast<WORD>(0));	// ItemTime
				CASH::SCashCost kCost(-Cost(), -BonusUse(), Bonus());
				kCost.WriteToPacket(kGPacket);
				if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN))
				{
					kGPacket.Push(TransactionKey());
				}
				if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_USA)
				||	g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA) )
				{
					kGPacket.Push( ReceiverUID() );
				}
				WriteToPacket(kGPacket);
				return GALA::SendCashRequest(kGPacket);
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.QueryOwner(CharacterGuid());
			
			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(0-Cost());
			kQuery.PushStrParam(0-BonusUse());
			kQuery.PushStrParam(Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_CS_SEND_GIFT));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_CASHUSE));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
		}break;
	}
	return false;
}

bool PgSendGiftTran::Rollback()
{
	if (0 == (TranState() & ETranState_CashModified))
	{
		return false;
	}

	PgTransaction::Commit(false, PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return GALA::SendCashRollback(*this);
			}
		} // no break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return NIVAL::SendCashRollback(*this, CASH::SCashCost(Cost(), BonusUse(), -Bonus()));
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.QueryOwner(CharacterGuid());
			
			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(Cost());
			kQuery.PushStrParam(BonusUse());
			kQuery.PushStrParam(0-Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_CS_SEND_GIFT_ROLLBACK));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
		}break;
	}
	return false;
}

void PgSendGiftTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);

	vLogString << _T("\r\n\t\t")
		<< _T("CharacterName=") << CharacterName() << _T(", Sender=") << SenderName() << _T(", Recv=") << ReceiverName() << _T("\r\n")
		<< _T("\t\tArticle=") << ArticleName() << _T(", TimeType=") << TimeType() << _T(", UseTime=") << UseTime() << _T(", Comment=") << Comment() << _T("\r\n")
		<< _T("\t\tCost=") << Cost() << _T("MileageCost=") << BonusUse() << _T("MileageAdded=") << Bonus();
}

bool PgSendGiftTran::GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder)
{
	m_kAddedPacket.PosAdjust();
	rkOrder.ReadFromPacket(m_kAddedPacket);
	return true;;
}

////////////////////////////////////////////////////////////
// PgExpandTimeLimitTran
//

PgExpandTimeLimitTran::PgExpandTimeLimitTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
	Cost(0i64);
	BonusUse(0i64);
	Bonus(0i64);
}

PgExpandTimeLimitTran::PgExpandTimeLimitTran(BM::Stream & _Packet)
{
	PgExpandTimeLimitTran::ReadFromPacket(_Packet);
}


PgExpandTimeLimitTran::~PgExpandTimeLimitTran()
{
};

PgExpandTimeLimitTran const& PgExpandTimeLimitTran::operator=(PgExpandTimeLimitTran const& rhs)
{
	PgTransaction::operator=(rhs);

	CharacterName(rhs.CharacterName());
	Cost(rhs.Cost());
	BonusUse(rhs.BonusUse());
	Bonus(rhs.Bonus());
	m_kBuyItem = rhs.m_kBuyItem;

	return (*this);
}

void PgExpandTimeLimitTran::Delete()
{
	delete this;
}

void PgExpandTimeLimitTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);

	kPacket.Push(CharacterName());
	kPacket.Push(Cost());
	kPacket.Push(BonusUse());
	kPacket.Push(Bonus());
	PU::TWriteArray_M(kPacket, m_kBuyItem);
}

bool PgExpandTimeLimitTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);

	kPacket.Pop(m_kCharacterName);
	kPacket.Pop(m_kCost);
	kPacket.Pop(m_kBonusUse);
	kPacket.Pop(m_kBonus);
	PU::TLoadArray_M(kPacket, m_kBuyItem);

	return true;
}

bool PgExpandTimeLimitTran::PayCash()
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_USA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				BM::Stream kGPacket(PT_A_CN_REQ_EXPANDTIEM);
				kGPacket.Push(AccountID());
				RemoteAddr().WriteToPacket(kGPacket);
				kGPacket.Push(CharacterGuid());
				if(LOCAL_MGR::NC_JAPAN==g_kLocal.ServiceRegion())
				{
					std::wstring addIP(CharacterName());
					addIP+=L"/";
					addIP+=RemoteAddr().IP();
					kGPacket.Push(addIP);
				}
				else
				{
					kGPacket.Push(CharacterName());
				}
				kGPacket.Push(UID());
				CASH::SCashCost kCost(-Cost(), -BonusUse(), Bonus());
				kCost.WriteToPacket(kGPacket);
				PU::TWriteKey_M(kGPacket, m_kBuyItem);
				if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN))
				{
					kGPacket.Push(TransactionKey());
				}
				WriteToPacket(kGPacket);
				return GALA::SendCashRequest(kGPacket);
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(-Cost());
			kQuery.PushStrParam(-BonusUse());
			kQuery.PushStrParam(Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_CS_ADD_TIME));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_CASHUSE));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
		}break;
	}
	return false;
}

bool PgExpandTimeLimitTran::GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder)
{
	m_kAddedPacket.PosAdjust();
	rkOrder.ReadFromPacket(m_kAddedPacket);
	return true;
}

bool PgExpandTimeLimitTran::Rollback()
{
	if (0 == (TranState() & ETranState_CashModified))
	{
		return false;
	}

	PgTransaction::Commit(false, PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return GALA::SendCashRollback(*this);
			}
		} // no break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return NIVAL::SendCashRollback(*this, CASH::SCashCost(Cost(), BonusUse(), -Bonus()));
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(Cost());
			kQuery.PushStrParam(BonusUse());
			kQuery.PushStrParam(-Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_CS_ADD_TIME_ROLLBACK));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
		}break;
	}
	return false;
}

void PgExpandTimeLimitTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);

	vLogString << _T("\r\n\t\t")
		<< _T("CharacterName=") << CharacterName() << _T("\r\n")
		<< _T("\t\tCost=") << Cost() << _T("MileageCost=") << BonusUse() << _T("MileageAdded=") << Bonus();
	CONT_BUYITEM::const_iterator item_itor = m_kBuyItem.begin();
	while (item_itor != m_kBuyItem.end())
	{
		vLogString << _T("\r\n\t\t\t\tItemInfo : ");
		(*item_itor).Log(vLogString);

		++item_itor;
	}
}

////////////////////////////////////////////////////////////
// PgOpenMarketBuyTran
//

PgCashShopBuyTran::PgCashShopBuyTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
	Cost(0i64);
	BonusUse(0i64);
	Bonus(0i64);
	BuyType(0);
}

PgCashShopBuyTran::PgCashShopBuyTran(BM::Stream & _Packet)
{
	PgCashShopBuyTran::ReadFromPacket(_Packet);
}


PgCashShopBuyTran::~PgCashShopBuyTran()
{
};

PgCashShopBuyTran const& PgCashShopBuyTran::operator=(PgCashShopBuyTran const& rhs)
{
	PgTransaction::operator=(rhs);

	CharacterName(rhs.CharacterName());
	Cost(rhs.Cost());
	BonusUse(rhs.BonusUse());
	Bonus(rhs.Bonus());
	m_kBuyItem = rhs.m_kBuyItem;
	BuyType(rhs.BuyType());

	return (*this);
}

void PgCashShopBuyTran::Delete()
{
	delete this;
}

void PgCashShopBuyTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);

	kPacket.Push(CharacterName());
	kPacket.Push(Cost());
	kPacket.Push(BonusUse());
	kPacket.Push(Bonus());
	PU::TWriteArray_M(kPacket, m_kBuyItem);
	kPacket.Push(BuyType());
}

bool PgCashShopBuyTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);

	kPacket.Pop(m_kCharacterName);
	kPacket.Pop(m_kCost);
	kPacket.Pop(m_kBonusUse);
	kPacket.Pop(m_kBonus);
	PU::TLoadArray_M(kPacket, m_kBuyItem);
	kPacket.Pop(m_kBuyType);
	return true;
}

bool PgCashShopBuyTran::PayCash()
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_USA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				BM::Stream kGPacket(PT_A_CN_REQ_BUYCASHITEM);
				kGPacket.Push(AccountID());
				RemoteAddr().WriteToPacket(kGPacket);
				kGPacket.Push(CharacterGuid());
				if(LOCAL_MGR::NC_JAPAN==g_kLocal.ServiceRegion())
				{
					std::wstring addIP(CharacterName());
					addIP+=L"/";
					addIP+=RemoteAddr().IP();
					kGPacket.Push(addIP);
				}
				else
				{
					kGPacket.Push(CharacterName());
				}
				kGPacket.Push(UID());
				CASH::SCashCost kCost(-Cost(), -BonusUse(), Bonus());
				kCost.WriteToPacket(kGPacket);
				PU::TWriteKey_M(kGPacket, m_kBuyItem);
				if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN))
				{
					kGPacket.Push(TransactionKey());
				}
				WriteToPacket(kGPacket);
				return GALA::SendCashRequest(kGPacket);
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(-Cost());
			kQuery.PushStrParam(-BonusUse());
			kQuery.PushStrParam(Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_CS_BUY_ARTICLE));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_CASHUSE));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PushQuery failed Query=") << kQuery.Command());
		}break;
	}
	return false;
}

bool PgCashShopBuyTran::Rollback()
{
	if (0 == (TranState() & ETranState_CashModified))
	{
		return false;
	}
	
	PgTransaction::Commit(false, PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return GALA::SendCashRollback(*this);
			}
		} // no break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return NIVAL::SendCashRollback(*this, CASH::SCashCost(Cost(), BonusUse(), -Bonus()));
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(Cost());
			kQuery.PushStrParam(BonusUse());
			kQuery.PushStrParam(-Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_CS_BUY_ARTICLE_ROLLBACK));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PushQuery failed Query=") << kQuery.Command());
		}break;
	}
	return false;
}


bool PgCashShopBuyTran::GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder)
{
	m_kAddedPacket.PosAdjust();
	rkOrder.ReadFromPacket(m_kAddedPacket);
	return true;;
}

void PgCashShopBuyTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);

	vLogString << _T("\r\n\t\t")
		<< _T("CharacterName=") << CharacterName() << _T("\r\n")
		<< _T("\t\tCost=") << Cost() << _T("MileageCost=") << BonusUse() << _T("MileageAdded=") << Bonus();
	CONT_BUYITEM::const_iterator item_itor = m_kBuyItem.begin();
	while (item_itor != m_kBuyItem.end())
	{
		vLogString << _T("\r\n\t\t\t\tItemInfo : ");
		(*item_itor).Log(vLogString);

		++item_itor;
	}
}


////////////////////////////////////////////////////////////
// PgEnterCashShopTran
//

PgEnterCashShopTran::PgEnterCashShopTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
}

PgEnterCashShopTran::PgEnterCashShopTran(BM::Stream & _Packet)
{
	PgEnterCashShopTran::ReadFromPacket(_Packet);
}


PgEnterCashShopTran::~PgEnterCashShopTran()
{
};

PgEnterCashShopTran const& PgEnterCashShopTran::operator=(PgEnterCashShopTran const& rhs)
{
	PgTransaction::operator=(rhs);

	return (*this);
}

void PgEnterCashShopTran::Delete()
{
	delete this;
}

/*
bool PgEnterCashShopTran::RequestCurerntCash()
{
	if (g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU))
	{
		return GALA::RequestCurrentCash(*this);
	}
	else
	{
		CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
		kQuery.InsertQueryTarget(CharacterGuid());
		kQuery.PushStrParam(MemberGuid());

		kQuery.QueryOwner(CharacterGuid());
		WriteToPacket(kQuery.contUserData);
	
		if (S_OK != g_kCoreCenter.PushQuery(kQuery))
		{
			return false;
		}
		return true;
	}
	return false;
}
*/

void PgEnterCashShopTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);
}

bool PgEnterCashShopTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);
	return true;
}

void PgEnterCashShopTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);
}


////////////////////////////////////////////////////////////
// PgEnterOpenMarketTran
//

PgEnterOpenMarketTran::PgEnterOpenMarketTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
}

PgEnterOpenMarketTran::PgEnterOpenMarketTran(BM::Stream & _Packet)
{
	PgEnterOpenMarketTran::ReadFromPacket(_Packet);
}


PgEnterOpenMarketTran::~PgEnterOpenMarketTran()
{
};

PgEnterOpenMarketTran const& PgEnterOpenMarketTran::operator=(PgEnterOpenMarketTran const& rhs)
{
	PgTransaction::operator=(rhs);

	return (*this);
}

void PgEnterOpenMarketTran::Delete()
{
	delete this;
}

/*
bool PgEnterOpenMarketTran::RequestCurerntCash()
{
	if (g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU))
	{
		return GALA::RequestCurrentCash(*this);
	}
	else
	{
		CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
		kQuery.InsertQueryTarget(CharacterGuid());
		kQuery.PushStrParam(MemberGuid());

		kQuery.QueryOwner(CharacterGuid());
		WriteToPacket(kQuery.contUserData);
	
		if (S_OK != g_kCoreCenter.PushQuery(kQuery))
		{
			return false;
		}
		return true;
	}
	return false;
}
*/

void PgEnterOpenMarketTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);
}

bool PgEnterOpenMarketTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);
	return true;
}

void PgEnterOpenMarketTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);
}

////////////////////////////////////////////////////////////
// PgOpenMarketBuyTran
//

PgOpenMarketBuyTran::PgOpenMarketBuyTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
	Cost(0);
	Bonus(0);
	BonusUse(0);
}

PgOpenMarketBuyTran::PgOpenMarketBuyTran(BM::Stream & _Packet)
{
	PgOpenMarketBuyTran::ReadFromPacket(_Packet);
}


PgOpenMarketBuyTran::~PgOpenMarketBuyTran()
{
};

PgOpenMarketBuyTran const& PgOpenMarketBuyTran::operator=(PgOpenMarketBuyTran const& rhs)
{
	PgTransaction::operator=(rhs);

	MarketGuid(rhs.MarketGuid());
	ArticleGuid(rhs.ArticleGuid());
	BuyNum(rhs.BuyNum());
	BuyerName(rhs.BuyerName());
	Cost(rhs.Cost());
	Bonus(rhs.Bonus());
	BonusUse(rhs.BonusUse());

	return (*this);
}

void PgOpenMarketBuyTran::Delete()
{
	delete this;
}

/*
bool PgOpenMarketBuyTran::RequestCurerntCash()
{
	if (g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU))
	{
		return GALA::RequestCurrentCash(*this);
	}
	else
	{
		CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
		kQuery.InsertQueryTarget(CharacterGuid());
		kQuery.PushStrParam(MemberGuid());

		kQuery.QueryOwner(CharacterGuid());
		WriteToPacket(kQuery.contUserData);
	
		if (S_OK != g_kCoreCenter.PushQuery(kQuery))
		{
			return false;
		}
		return true;
	}
	return false;
}
*/

void PgOpenMarketBuyTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);
	kPacket.Push(MarketGuid());
	kPacket.Push(ArticleGuid());
	kPacket.Push(BuyNum());
	kPacket.Push(BuyerName());
	kPacket.Push(Cost());
	kPacket.Push(BonusUse());
	kPacket.Push(Bonus());
}

bool PgOpenMarketBuyTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);
	kPacket.Pop(m_kMarketGuid);
	kPacket.Pop(m_kArticleGuid);
	kPacket.Pop(m_kBuyNum);
	kPacket.Pop(m_kBuyerName);
	kPacket.Pop(m_kCost);
	kPacket.Pop(m_kBonusUse);
	kPacket.Pop(m_kBonus);
	return true;
}

bool PgOpenMarketBuyTran::PayCash()
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Not supported function"));
			return false;
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				BM::Stream kGPacket(PT_A_CN_REQ_OM_BUY_ARTICLE);
				kGPacket.Push(AccountID());
				RemoteAddr().WriteToPacket(kGPacket);
				kGPacket.Push(BuyerName());
				CASH::SCashCost kCost(-Cost(), -BonusUse(), Bonus());
				kCost.WriteToPacket(kGPacket);
				WriteToPacket(kGPacket);
				return GALA::SendCashRequest(kGPacket);
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(-Cost());
			kQuery.PushStrParam(-BonusUse());
			kQuery.PushStrParam(Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_OM_BUY_ARTICLE));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_CASHUSE));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PushQuery failed Query=") << kQuery.Command());
		}break;
	}
	return false;
}

bool PgOpenMarketBuyTran::Rollback()
{
	if (0 == (TranState() & ETranState_CashModified))
	{
		return false;
	}

	PgTransaction::Commit(false, PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Not supported function"));
			return false;
		} // no break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return NIVAL::SendCashRollback(*this, CASH::SCashCost(Cost(), BonusUse(), -Bonus()));
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(Cost());
			kQuery.PushStrParam(BonusUse());
			kQuery.PushStrParam(-Bonus());
			kQuery.PushStrParam(static_cast<BYTE>(MCT_GAME_OM_BUY_ARTICLE_ROLLBACK));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PushQuery failed Query=") << kQuery.Command());
		}break;
	}
	return false;
}


bool PgOpenMarketBuyTran::GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder)
{
	m_kAddedPacket.PosAdjust();
	rkOrder.ReadFromPacket(m_kAddedPacket);
	return true;
}

void PgOpenMarketBuyTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);
	vLogString << _T("\r\n\t\t") << _T(", Buyer=") << BuyerName() << _T("Market=") << MarketGuid() << _T(", Article=") << ArticleGuid() << _T(", BuyCount=") << BuyNum() << _T("\r\n")
		<< _T("\r\rCost=") << Cost() << _T(", MileageCost=") << BonusUse() << _T(", AddedMileage=") << Bonus();
}


////////////////////////////////////////////////////////////
// PgAddCashTran
//

PgAddCashTran::PgAddCashTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
	Cash(0);
}

PgAddCashTran::PgAddCashTran(BM::Stream & _Packet)
{
	PgAddCashTran::ReadFromPacket(_Packet);
}

PgAddCashTran::~PgAddCashTran()
{
}

PgAddCashTran const& PgAddCashTran::operator=(PgAddCashTran const& rhs)
{
	PgTransaction::operator=(rhs);

	CharacterName(rhs.CharacterName());
	Cash(rhs.Cash());

	return (*this);
}

bool PgAddCashTran::PayCash()
{
	switch(g_kLocal.ServiceRegion())
	{
	//TODO: Make check cash by site. It important bc dupe!
	/*
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not supported in this SERVICE_REGION"));
			return false;
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				BM::Stream kGPacket(PT_A_CN_REQ_ADDCASH);
				kGPacket.Push(AccountID());
				RemoteAddr().WriteToPacket(kGPacket);
				CASH::SCashCost kCost(Cash(), 0, 0);
				kCost.WriteToPacket(kGPacket);
				WriteToPacket(kGPacket);
				return GALA::SendCashRequest(kGPacket);
			}
		} // no break;
	*/
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(Cash());
			kQuery.PushStrParam(static_cast<__int64>(0));
			kQuery.PushStrParam(static_cast<__int64>(0));
			EMODIFY_CASH_TYPE eCashOrderType = MCT_NONE;
			switch(EventType())
			{
			case CIE_UM_Article_Buy:
				{
					eCashOrderType = MCT_GAME_OM_BUY_ARTICLE;
				}break;
			case CIE_UM_Article_CashBack:
				{
					eCashOrderType = MCT_GAME_OM_SELL_ARTICLE;
				}break;
			case CIE_Coupon:
				{
					eCashOrderType = MCT_GAME_COUPON;
				}break;
			case CIE_Cash_Pack:
				{
					eCashOrderType = MCT_GAME_Cash_Pack_ITEM;
				}break;
			case CIE_GambleMachine:
				{
					eCashOrderType = MCT_GAME_GM_PAY;
				}break;
			default:
				{
					eCashOrderType = MCT_NONE;
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown EventType=") << EventType());
					return false;
				}break;
			}
			kQuery.PushStrParam(static_cast<BYTE>(eCashOrderType));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_CASHADD));
			kQuery.PushStrParam(TransactionKey());
			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PushQuery failed Query=") << kQuery.Command());
		}break;
	}
	return false;
}

bool PgAddCashTran::Rollback()
{
	if (0 == (TranState() & ETranState_CashModified))
	{
		return false;
	}

	PgTransaction::Commit(false, PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not supported in this SERVICE_REGION"));
			return false;
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return NIVAL::SendCashRollback(*this, CASH::SCashCost(-Cash(), 0, 0));
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.QueryOwner(CharacterGuid());

			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(-Cash());
			kQuery.PushStrParam(static_cast<__int64>(0));
			kQuery.PushStrParam(static_cast<__int64>(0));
			kQuery.PushStrParam(static_cast<BYTE>(MCT_ADD_CASH_ROLLBACK));
			kQuery.PushStrParam(static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK));
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PushQuery failed Query=") << kQuery.Command());
		}break;
	}
	return false;
}

void PgAddCashTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);
	kPacket.Push(CharacterName());
	kPacket.Push(Cash());
}

bool PgAddCashTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);
	kPacket.Pop(m_kCharacterName);
	kPacket.Pop(m_kCash);
	return true;
}

void PgAddCashTran::Delete()
{
	delete this;
}

void PgAddCashTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);
	vLogString << _T("\r\n\t\tCharacterName=") << CharacterName() << _T(", Cash=") << Cash();
}

bool PgAddCashTran::GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder)
{
	m_kAddedPacket.PosAdjust();
	rkOrder.ReadFromPacket(m_kAddedPacket);
	// 이 뒤에 있는 패킷 정보는 AddonPacket 이다. --> AddonPacket 만 남아 있도록 한다.
	BM::Stream kAddon;
	kAddon.Push(m_kAddedPacket);
	m_kAddedPacket.Reset();
	m_kAddedPacket.Push(kAddon);
	return true;
}


////////////////////////////////////////////////////////////
// PgOpenMarketCashRegTran
//

PgOpenMarketCashRegTran::PgOpenMarketCashRegTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
: PgTransaction(_Member, _Character, _Ground, _Target, _Event)
{
	Cost(0i64);
}

PgOpenMarketCashRegTran::PgOpenMarketCashRegTran(BM::Stream & _Packet)
{
	PgOpenMarketCashRegTran::ReadFromPacket(_Packet);
}


PgOpenMarketCashRegTran::~PgOpenMarketCashRegTran()
{
};

PgOpenMarketCashRegTran const& PgOpenMarketCashRegTran::operator=(PgOpenMarketCashRegTran const& rhs)
{
	PgTransaction::operator=(rhs);

	Cost(rhs.Cost());
	TranType(rhs.TranType());
	return (*this);
}

void PgOpenMarketCashRegTran::Delete()
{
	delete this;
}

void PgOpenMarketCashRegTran::WriteToPacket(BM::Stream & kPacket) const
{
	PgTransaction::WriteToPacket(kPacket);

	kPacket.Push(Cost());
	kPacket.Push(TranType());
}

bool PgOpenMarketCashRegTran::ReadFromPacket(BM::Stream & kPacket)
{
	PgTransaction::ReadFromPacket(kPacket);

	kPacket.Pop(m_kCost);
	kPacket.Pop(m_kTranType);
	return true;
}

bool PgOpenMarketCashRegTran::PayCash()
{
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot use this function in this region"));
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				BM::Stream kGPacket(PT_A_CN_REQ_OM_REG_ARTICLE);
				kGPacket.Push(AccountID());
				RemoteAddr().WriteToPacket(kGPacket);
				CASH::SCashCost kCost(-Cost(), 0, 0);
				kCost.WriteToPacket(kGPacket);
				WriteToPacket(kGPacket);
				return GALA::SendCashRequest(kGPacket);
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.QueryOwner(CharacterGuid());
			
			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(0-Cost());
			kQuery.PushStrParam(0);
			kQuery.PushStrParam(0);
			BYTE byModifyCashType = 0;
			BYTE byCashLogState = 0;
			switch (TranType())
			{
			case ECASH_TRAN_OPENMARKET_SELLCASH:		// OpenMarket 에 Cash 등록
				{
					byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_REG_CASH);
					byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_CASHUSE);
				}break;
			case ECASH_TRAN_OPENMARKET_UNREGCASH:	// OpenMarket 에 등록된 Cash 등록 해지
				{
					byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_UNREG_CASH);
					byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_CASHADD);
				}break;
			//case ECASH_TRAN_OPENMARKET_PAYBACK:		// OpenMarket 에서 팔린 Money -> Cash로 수입얻기
			//	{
			//		byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_PAYBACK);
			//		byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_CASHADD);
			//	}break;
			case ECASH_TRAN_OPENMARKET_BUYCASH:
				{
					byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_BUYCASH);
					byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_CASHADD);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown TranType =") << static_cast<int>(TranType()));
					return false;
				}break;
			}
			kQuery.PushStrParam(byModifyCashType);
			kQuery.PushStrParam(byCashLogState);
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
		}break;
	}
	return false;
}

bool PgOpenMarketCashRegTran::Rollback()
{
	if (0 == (TranState() & ETranState_CashModified))
	{
		return false;
	}
	
	PgTransaction::Commit(false, PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_JAPAN:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot use this function in this region"));
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return NIVAL::SendCashRollback(*this, CASH::SCashCost(Cost(), 0, 0));
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_MODIFY,L"EXEC [dbo].[UP_CS_ADDMEMBERCASH]");
			kQuery.InsertQueryTarget(MemberGuid());
			kQuery.InsertQueryTarget(CharacterGuid());
			kQuery.QueryOwner(CharacterGuid());
			
			kQuery.PushStrParam(MemberGuid());
			kQuery.PushStrParam(CharacterGuid());
			kQuery.PushStrParam(Cost());
			kQuery.PushStrParam(0);
			kQuery.PushStrParam(0);
			BYTE byModifyCashType = 0;
			BYTE byCashLogState = 0;
			switch (TranType())
			{
			case ECASH_TRAN_OPENMARKET_SELLCASH:		// OpenMarket 에 Cash 등록
				{
					byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_REG_CASH_ROLLBACK);
					byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK);
				}break;
			case ECASH_TRAN_OPENMARKET_UNREGCASH:	// OpenMarket 에 등록된 Cash 등록 해지
				{
					byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_UNREG_CASH_ROLLBACK);
					byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK);
				}break;
			//case ECASH_TRAN_OPENMARKET_PAYBACK:		// OpenMarket 에서 팔린 Money -> Cash로 수입얻기
			//	{
			//		byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_PAYBACK_ROLLBACK);
			//		byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK);
			//	}break;
			case ECASH_TRAN_OPENMARKET_BUYCASH:
				{
					byModifyCashType = static_cast<BYTE>(MCT_GAME_OM_BUYCASH_ROLLBACK);
					byCashLogState = static_cast<BYTE>(ECASH_LOG_STATE_ROLLBACK);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown TranType =") << TranType());
					return false;
				}break;
			}
			kQuery.PushStrParam(byModifyCashType);
			kQuery.PushStrParam(byCashLogState);
			kQuery.PushStrParam(TransactionKey());

			WriteToPacket(kQuery.contUserData);
			if (S_OK == g_kCoreCenter.PushQuery(kQuery))
			{
				return true;
			}
		}break;
	}
	return false;
}

void PgOpenMarketCashRegTran::Log(BM::vstring &vLogString) const
{
	PgTransaction::Log(vLogString);

	vLogString << _T("\r\n\t\tTranType=") << TranType() << _T(", Cost=") << Cost();
}

bool PgOpenMarketCashRegTran::GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder)
{
	switch (TranType())
	{
	case ECASH_TRAN_OPENMARKET_SELLCASH:
	//case ECASH_TRAN_OPENMARKET_PAYBACK:
	case ECASH_TRAN_OPENMARKET_BUYCASH:
		{
			m_kAddedPacket.PosAdjust();
			rkOrder.ReadFromPacket(m_kAddedPacket);
		}break;
	case ECASH_TRAN_OPENMARKET_UNREGCASH:
		{
			rkOrder.clear();
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown TranType =") << TranType());
			return false;
		}break;
	}
	return true;;
}



////////////////////////////////////////////////////////////
// PgTranPointer
//

PgTranPointer::PgTranPointer(PgTransaction* _Tran)
{
	m_pkTransaction = _Tran;
}

PgTranPointer::PgTranPointer(ECASH_TRAN_TYPE const _Tran, BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event)
{
	switch (_Tran)
	{
	case ECASH_TRAN_SEND_GIFT:
		{
			m_pkTransaction = new PgSendGiftTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_EXPAND_TIMELIMIT:
		{
			m_pkTransaction = new PgExpandTimeLimitTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_CASHSHOP_BUY:
		{
			m_pkTransaction = new PgCashShopBuyTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_CASHSHOP_ENTER:
		{
			m_pkTransaction = new PgEnterCashShopTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_OPENMARKET_ENTER:
		{
			m_pkTransaction = new PgEnterOpenMarketTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_OPENMARKET_BUY:
		{
			m_pkTransaction = new PgOpenMarketBuyTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_ADD_CASH:
		{
			m_pkTransaction = new PgAddCashTran(_Member, _Character, _Ground, _Target, _Event);
		}break;
	case ECASH_TRAN_OPENMARKET_SELLCASH:
	case ECASH_TRAN_OPENMARKET_UNREGCASH:
	//case ECASH_TRAN_OPENMARKET_PAYBACK:
	case ECASH_TRAN_OPENMARKET_BUYCASH:
		{
			PgOpenMarketCashRegTran* pkTran = new PgOpenMarketCashRegTran(_Member, _Character, _Ground, _Target, _Event);
			m_pkTransaction = pkTran;
			pkTran->TranType(_Tran);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("unknown Transaction type =") << _Tran);
			m_pkTransaction = NULL;
		}break;
	}
}

PgTranPointer::PgTranPointer(BM::Stream & _Packet)
{
	m_pkTransaction = NULL;
	PgTranPointer::ReadFromPacket(_Packet);
}

PgTranPointer::PgTranPointer(BM::Stream const& _Packet)
{
	m_pkTransaction = NULL;
	BM::Stream& rkReadPacket = const_cast<BM::Stream&>(_Packet);
	PgTranPointer::ReadFromPacket(rkReadPacket);
}

PgTranPointer::~PgTranPointer()
{
	if (m_pkTransaction != NULL)
	{
		m_pkTransaction->Delete();
	}
}

PgTransaction* PgTranPointer::GetTran()
{
	return m_pkTransaction;
}

PgTransaction* const PgTranPointer::GetTran() const
{
	return m_pkTransaction;
}

void PgTranPointer::WriteToPacket(BM::Stream & kPacket) const
{
	if (m_pkTransaction == NULL)
	{
		kPacket.Push(ECASH_TRAN_NONE);
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Transaction is NULL"));
	}
	else
	{
		m_pkTransaction->WriteToPacket(kPacket);
	}
}

bool PgTranPointer::ReadFromPacket(BM::Stream & kPacket)
{
	ECASH_TRAN_TYPE eType = ECASH_TRAN_NONE;
	size_t szRdPos = kPacket.RdPos();
	kPacket.Pop(eType);
	kPacket.RdPos(szRdPos);

	if (m_pkTransaction != NULL)
	{
		m_pkTransaction->Delete();
		m_pkTransaction = NULL;
	}

	switch(eType)
	{
	case ECASH_TRAN_SEND_GIFT:
		{
			m_pkTransaction = new PgSendGiftTran(kPacket);
		}break;
	case ECASH_TRAN_EXPAND_TIMELIMIT:
		{
			m_pkTransaction = new PgExpandTimeLimitTran(kPacket);
		}break;
	case ECASH_TRAN_CASHSHOP_BUY:
		{
			m_pkTransaction = new PgCashShopBuyTran(kPacket);
		}break;
	case ECASH_TRAN_CASHSHOP_ENTER:
		{
			m_pkTransaction = new PgEnterCashShopTran(kPacket);
		}break;
	case ECASH_TRAN_OPENMARKET_ENTER:
		{
			m_pkTransaction = new PgEnterOpenMarketTran(kPacket);
		}break;
	case ECASH_TRAN_OPENMARKET_BUY:
		{
			m_pkTransaction = new PgOpenMarketBuyTran(kPacket);
		}break;
	case ECASH_TRAN_ADD_CASH:
		{
			m_pkTransaction = new PgAddCashTran(kPacket);
		}break;
	case ECASH_TRAN_OPENMARKET_SELLCASH:
	case ECASH_TRAN_OPENMARKET_UNREGCASH:
	//case ECASH_TRAN_OPENMARKET_PAYBACK:
	case ECASH_TRAN_OPENMARKET_BUYCASH:
		{
			m_pkTransaction = new PgOpenMarketCashRegTran(kPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown Transaction type = ") << static_cast<int>(eType));
			m_pkTransaction = NULL;
		}break;
	}
	return true;
}


bool PgTranPointer::RequestCurerntCash() const
{
	if (m_pkTransaction != NULL)
	{
		return PushOrderJob(CIE_CASH_BALANCE_REQ);		
	}
	return false;
}


ECASH_TRAN_TYPE PgTranPointer::GetTransactionType()
{
	if (m_pkTransaction != NULL)
	{
		return m_pkTransaction->GetTransactionType();
	}
	return ECASH_TRAN_NONE;
}

bool PgTranPointer::RequestCurerntLimitSell() const
{
	if (m_pkTransaction != NULL)
	{
		return PushOrderJob(CIE_CASH_LIMIT_ITEM_REQ);
	}
	return false;
}

bool PgTranPointer::RecvCurerntLimitSell(ECashShopResult const eResult, int const iItemTotalCount, int const iItemLimitCount, int const iItemBuyLimitCount, int const iItemBuyCount)
{
	if (m_pkTransaction != NULL)
	{
		BM::Stream& rkResult = m_pkTransaction->ResultPacket();
		rkResult.Reset();
		rkResult.Push(eResult);
		m_pkTransaction->ItemTotalCount(iItemTotalCount);
		m_pkTransaction->ItemLimitCount(iItemLimitCount);
		m_pkTransaction->ItemBuyLimitCount(iItemBuyLimitCount);
		m_pkTransaction->ItemBuyCount(iItemBuyCount);
		return PushOrderJob(CIE_CASH_LIMIT_ITEM_RCV);
	}
	return false;
}

bool PgTranPointer::RecvCurrentCash(ECashShopResult const eResult, __int64 const i64Cash, __int64 const i64Mileage)
{
	if (m_pkTransaction != NULL)
	{
		BM::Stream& rkResult = m_pkTransaction->ResultPacket();
		rkResult.Reset();
		rkResult.Push(eResult);
		m_pkTransaction->InitCash(i64Cash);
		m_pkTransaction->InitMileage(i64Mileage);
		return PushOrderJob(CIE_CASH_BALANCE_RCV);
	}
	return false;
}

bool PgTranPointer::PushOrderJob(EItemModifyParentEventType const eCause, CONT_PLAYER_MODIFY_ORDER const* const pkOrder, BM::Stream * const pkAddonPacket, BM::Stream * const pk2ndPacket) const
{
	if (m_pkTransaction != NULL)
	{
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(m_pkTransaction->MemberGuid());
		pkActionOrder->InsertTarget(m_pkTransaction->CharacterGuid());
		pkActionOrder->kGndKey.Set(m_pkTransaction->TargetSI().nChannel, m_pkTransaction->GroundKey());
		pkActionOrder->kCause = eCause;
		if (pkOrder != NULL)
		{
			pkActionOrder->kContOrder = *pkOrder;
		}
		if (pkAddonPacket != NULL)
		{
			pkActionOrder->kAddonPacket.Push(pkAddonPacket->Data());
		}
		if (pk2ndPacket != NULL)
		{
			pkActionOrder->kPacket2nd = *pk2ndPacket;
		}
		m_pkTransaction->WriteToPacket(pkActionOrder->kPacket2nd);
		if (S_OK == g_kJobDispatcher.VPush(pkActionOrder))
		{
			return true;
		}
	}
	return false;
}

bool PgTranPointer::RequestPayCash() const
{
	return PushOrderJob(CIE_CASH_USE_REQ);
}

bool PgTranPointer::RecvPayCash(ECashShopResult const eResult, __int64 const i64CashCur, __int64 const i64MileageCur, __int64 const i64AddedMileage)
{
	if (m_pkTransaction != NULL)
	{
		m_pkTransaction->TranState(m_pkTransaction->TranState() | PgTransaction::ETranState_CashModified);

		BM::Stream &rkResult = m_pkTransaction->ResultPacket();
		rkResult.Reset();
		rkResult.Push(eResult);
		rkResult.Push(i64CashCur);
		rkResult.Push(i64MileageCur);
		rkResult.Push(i64AddedMileage);

		return PushOrderJob(CIE_CASH_USE_RCV);
	}
	return false;
}

bool PgTranPointer::RequestItemModify(CONT_PLAYER_MODIFY_ORDER const& rkOrder)
{
	if (m_pkTransaction != NULL)
	{
		BM::Stream k2ndPacket;
		k2ndPacket.Push(m_pkTransaction->EventType());

		switch(m_pkTransaction->GetTransactionType())
		{
		case ECASH_TRAN_OPENMARKET_BUY:
			{
				PgOpenMarketBuyTran* pkTran = dynamic_cast<PgOpenMarketBuyTran*>(m_pkTransaction);
				if (pkTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("dynamic_cast<PgOpenMarketBuyTran*> is NULL"));
					return false;
				}
				BM::Stream kAnsPacket(PT_M_C_UM_ANS_ARTICLE_BUY);
				kAnsPacket.Push(pkTran->ArticleGuid());
				kAnsPacket.Push(pkTran->BuyNum());
				return PushOrderJob(pkTran->EventType(), &rkOrder, &kAnsPacket, &k2ndPacket);
			}break;
		case ECASH_TRAN_ADD_CASH:
			{
				bool bAddonPacket = false;
				m_pkTransaction->AddedPacket().Pop(bAddonPacket);
				if (bAddonPacket)
				{
					BM::Stream kAddonPacket;
					kAddonPacket.Push(m_pkTransaction->AddedPacket());
					return PushOrderJob(m_pkTransaction->EventType(), &rkOrder, &kAddonPacket, &k2ndPacket);
				}
			}break;
		}
		return PushOrderJob(m_pkTransaction->EventType(), &rkOrder, NULL, &k2ndPacket);
	}
	return false;
}

BM::GUID PgTranPointer::TransactionKey()
{
	if (m_pkTransaction != NULL)
	{
		return m_pkTransaction->TransactionKey();
	}
	return BM::GUID::NullData();
}

bool PgTranPointer::Commit()
{
	if (m_pkTransaction != NULL)
	{
		m_pkTransaction->Commit(true, PT_CN_A_ANS_TRAN_COMMIT);

		m_pkTransaction->TranState(m_pkTransaction->TranState() | PgTransaction::ETranState_CommitTry);

		CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CASH_TRAN_COMMIT,L"EXEC [dbo].[up_CS_CashTranCommit]");
		kQuery.InsertQueryTarget(m_pkTransaction->MemberGuid());
		kQuery.InsertQueryTarget(m_pkTransaction->CharacterGuid());
		kQuery.QueryOwner(m_pkTransaction->CharacterGuid());
		
		kQuery.PushStrParam(m_pkTransaction->TransactionKey());
		if (S_OK == g_kCoreCenter.PushQuery(kQuery))
		{
			return true;
		}
	}
	return false;
}

bool PgTranPointer::Rollback()
{
	if (m_pkTransaction != NULL)
	{
		return m_pkTransaction->Rollback();
	}
	return false;
}

bool PgTranPointer::Finish(HRESULT const hResult)
{
	BM::Stream k2ndPacket;
	k2ndPacket.Push(hResult);
	return PushOrderJob(CIE_CASH_PROCESS_END, NULL, NULL, &k2ndPacket );
}

void PgTranPointer::Log(BM::vstring& rkLog) const
{
	if (m_pkTransaction != NULL)
	{
		m_pkTransaction->Log(rkLog);
	}
	else
	{
		rkLog << _T("Transaction is NULL");
	}
}

bool PgTranPointer::SendLog()
{
	if (m_pkTransaction != NULL)
	{
		PgLogCont& rkLogCont = m_pkTransaction->LogCont();
		rkLogCont.LogKey(m_pkTransaction->TransactionKey());
		rkLogCont.Commit();
		return true;
	}
	return false;
}

namespace TRAN_HELPER
{
	HRESULT RecvCashBalance(PgTranPointer &rkTran)
	{
		PgTransaction* pkTran = rkTran.GetTran();
		if (pkTran == NULL)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Transaction information is NULL"));
			return E_FAIL;
		}

		__int64 const i64Cash = pkTran->InitCash();
		__int64 const i64Mileage = pkTran->InitMileage();

		switch(pkTran->EventType())
		{
		case CIE_UM_Article_CashReg:
			{
				if (UMR_SUCCESS != g_kRealmUserMgr.Locked_Cash_Article_Reg(rkTran))
				{
					g_kRealmUserMgr.Locked_LeaveEvent(pkTran->CharacterGuid());
				}
			}break;
		case CIE_CS_Add_TimeLimit:
			{
				ECashShopResult const kError = g_kCashShopMgr.OnProcessReqAddTimeLimit(pkTran->EventType(), i64Cash, i64Mileage, pkTran->CharacterGuid(), rkTran);
				if(CSR_SUCCESS == kError)
				{
					return S_OK;
				}

				BM::Stream kPacket(PT_M_C_CS_ANS_ADD_TIMELIMIT);
				kPacket.Push(kError);
				g_kRealmUserMgr.Locked_SendToUser(pkTran->CharacterGuid(), kPacket, false);
			}break;
		case CIE_UM_Article_Buy:
			{
				PgOpenMarketBuyTran* const pkBuyTran = dynamic_cast<PgOpenMarketBuyTran*>(pkTran);
				if (pkBuyTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgOpenMarketBuyTran*> failed"));
					g_kRealmUserMgr.Locked_LeaveEvent(pkTran->CharacterGuid());
					return E_FAIL;
				}

				CONT_PLAYER_MODIFY_ORDER kContOrder;

				tagPlayerModifyOrderData_ModifyBuyArticle kBuyArticle(pkBuyTran->MemberGuid(), pkBuyTran->MarketGuid(), pkBuyTran->ArticleGuid(),
					pkBuyTran->BuyNum(), pkBuyTran->BuyerName(), i64Cash, i64Mileage);
				kContOrder.push_back(SPMO(IMET_BUY_ARTICLE, pkBuyTran->CharacterGuid(), kBuyArticle));

				PgOpenMarketMgr::SCostInfo kCostInfo;
				EUserMarketResult eCostResult = g_kRealmUserMgr.Locked_OpenMarketVerifyArticlePrice(pkBuyTran->CharacterGuid(), pkBuyTran->MarketGuid(), pkBuyTran->BuyNum(),
					pkBuyTran->ArticleGuid(), kCostInfo);
				if (eCostResult != UMR_SUCCESS)
				{
					BM::Stream kErrorPacket(PT_M_C_UM_ANS_ARTICLE_BUY);
					kErrorPacket.Push(eCostResult);
					g_kRealmUserMgr.Locked_SendToUser(pkBuyTran->CharacterGuid(), kErrorPacket, false);
					return E_FAIL;
				}

				kContOrder.WriteToPacket(pkBuyTran->AddedPacket());
				if (kCostInfo.i64Cash > 0)
				{
					pkBuyTran->Cost(kCostInfo.i64Cash);
					pkBuyTran->BonusUse(0);
					pkBuyTran->Bonus(0);
					rkTran.RequestPayCash();
				}
				else
				{
					if (false == rkTran.RequestItemModify(kContOrder))
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("RequestItemModify order failed TransactionType=") << pkTran->GetTransactionType());
						return false;
					}
				}
			}break;
		case CIE_CS_Select:
			{
				BM::Stream kPacket(PT_I_M_CS_NOTI_CASH_MODIFY);
				kPacket.Push(pkTran->CharacterGuid());
				kPacket.Push(i64Cash);
				kPacket.Push(i64Mileage);
				kPacket.Push(static_cast<int>(0));
				kPacket.Push(static_cast<int>(0));
				g_kRealmUserMgr.Locked_SendToUserGround(pkTran->CharacterGuid(),kPacket,false,true);
			}break;
		case CIE_CS_Buy:
			{
				PgCashShopBuyTran* const pkBuyTran = dynamic_cast<PgCashShopBuyTran*>(pkTran);
				if (pkBuyTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgCashShopBuyTran*> failed"));
					return E_FAIL;
				}

				int iError = g_kCashShopMgr.OnProcessReqBuyArticle(pkBuyTran->EventType(),i64Cash,i64Mileage,pkBuyTran->CharacterGuid(), rkTran);

				if(CSR_SUCCESS == iError)
				{
					return S_OK;
				}

				g_kRealmUserMgr.Locked_LeaveEvent(pkBuyTran->CharacterGuid());

				BM::Stream kPacket(PT_M_C_CS_ANS_BUY_ARTICLE);
				kPacket.Push(iError);
				kPacket.Push(pkBuyTran->BuyType());
				g_kRealmUserMgr.Locked_SendToUser(pkBuyTran->CharacterGuid(),kPacket,false);
			}break;
		case CIE_CS_Gift:
			{
				PgSendGiftTran* pkGiftTran = dynamic_cast<PgSendGiftTran*>(pkTran);
				if (pkGiftTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgSendGiftTran*> failed"));
					return E_FAIL;
				}
				int iError = g_kCashShopMgr.OnProcessReqGiftArticle(pkGiftTran->EventType(),i64Cash,i64Mileage,pkGiftTran->CharacterGuid(), rkTran);

				if(CSR_SUCCESS == iError)
				{
					return S_OK;
				}

				g_kRealmUserMgr.Locked_LeaveEvent(pkGiftTran->CharacterGuid());

				BM::Stream kPacket(PT_M_C_CS_ANS_SEND_GIFT);
				kPacket.Push(iError);
				g_kRealmUserMgr.Locked_SendToUser(pkGiftTran->CharacterGuid(),kPacket,false);
			}break;
		case CIE_UM_Article_Dereg:	// OM에서 등록한 cash item 회수 (Cash증가)
		case CIE_UM_Article_CashBuy:// OM에서 cash item 구매 (Cash증가)
			{
				PgOpenMarketCashRegTran* pkUnregTran = dynamic_cast<PgOpenMarketCashRegTran*>(pkTran);
				if (pkUnregTran == NULL)
				{
					BM::vstring kTranLog;
					pkTran->Log(kTranLog);
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgOpenMarketCashRegTran*> failed Tran=") << kTranLog);
					return E_FAIL;
				}
				if (pkUnregTran->InitCash() - pkUnregTran->Cost() < 0)	// CASH부족
				{
					return E_FAIL;
				}
				pkTran->PayCash();

			}break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}break;
		}
		return S_OK;
	}

	HRESULT RecvCashModify(ECashShopResult const eResult, PgTranPointer &rkTran, __int64 const i64CashCur, __int64 const i64MileageCur, __int64 const i64AddedMileage)
	{
		PgTransaction* pkTran = rkTran.GetTran();
		if (pkTran == NULL)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
			return E_FAIL;
		}

		if ( true == rkTran.RecvPayCash(eResult, i64CashCur, i64MileageCur, i64AddedMileage) )
		{
			return S_OK;
		}

		return E_FAIL;
	}

};


namespace GALA 
{
	bool SendCashRequest(BM::Stream const& kPacket)
	{
		BM::Stream kEPacket(PT_N_X_REQ_CASH);
		g_kProcessCfg.ServerIdentity().WriteToPacket(kEPacket);
		kEPacket.Push(kPacket);
		return SendToImmigration(kEPacket);
	}

	ECashShopResult GetCashShopResult(EGTDX_ERROR_CODE const eCode)
	{
		ECashShopResult eReturn = CSR_SYSTEM_ERROR;
		switch (eCode)
		{
		case EGTDX_ERROR_SUCCESS: { eReturn = CSR_SUCCESS; }break;
		case EGTDX_ERROR_CANNOT_BUY: { eReturn = CSR_BLOCKED_USER; }break;
		case EGTDX_ERROR_NOT_EXIST: { eReturn = CSR_INVALID_CHARACTER; }break;
		case EGTDX_ERROR_BILLINGSERVER_ERROR:
		case EGTDX_ERROR_ALREADY_CANCEL:
		case EGTDX_ERROR_DUPLICATE_REQUEST:
		case EGTDX_ERROR_SYSTEM_ERROR: { eReturn = CSR_SYSTEM_ERROR; }break;
		case EGTDX_ERROR_CASH_SHORTAGE:{ eReturn = CSR_NOT_ENOUGH_CASH;}break;
		default: { eReturn = CSR_SYSTEM_ERROR; }break;
		}
		return eReturn;
	}

	bool SendCashRollback(PgTransaction const& rkTransaction)
	{
		BM::Stream kHPacket(PT_A_CN_REQ_BUYCASHITEM_ROLLBACK);
		kHPacket.Push(rkTransaction.UID());
		kHPacket.Push(rkTransaction.AccountID());
		kHPacket.Push(rkTransaction.ChargeNo());
		rkTransaction.WriteToPacket(kHPacket);
		return SendCashRequest(kHPacket);
	}

	EMODIFY_CASH_TYPE const GetModifyCashType(ECASH_TRAN_TYPE const eTranType, bool const bRollback)
	{
		EMODIFY_CASH_TYPE eCashType = MCT_NONE;
		switch (eTranType)
		{
		case ECASH_TRAN_SEND_GIFT:
			{
				eCashType = (bRollback) ? MCT_GAME_CS_SEND_GIFT_ROLLBACK : MCT_GAME_CS_SEND_GIFT;
			}break;
		case ECASH_TRAN_EXPAND_TIMELIMIT:
			{
				eCashType = (bRollback) ? MCT_GAME_CS_ADD_TIME_ROLLBACK : MCT_GAME_CS_ADD_TIME;
			}break;
		case ECASH_TRAN_CASHSHOP_BUY:
			{
				eCashType = (bRollback) ? MCT_GAME_CS_ADD_TIME_ROLLBACK : MCT_GAME_CS_BUY_ARTICLE;
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("not supported TranType =") << eTranType);
			}break;
		}
		return eCashType;
	}

	bool CashTransaction(PgTransaction const& rkTran, bool const bRollback, __int64 const i64Cash, __int64 const i64Mileage, __int64 const i64MileageAdd)
	{
		CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CASH_TRANSACTION,L"EXEC [dbo].[up_CS_CashTran]");
		kQuery.InsertQueryTarget(rkTran.MemberGuid());
		kQuery.InsertQueryTarget(rkTran.CharacterGuid());
		kQuery.QueryOwner(rkTran.CharacterGuid());
		
		kQuery.PushStrParam(rkTran.MemberGuid());
		kQuery.PushStrParam(rkTran.CharacterGuid());
		kQuery.PushStrParam(i64Cash);
		kQuery.PushStrParam(i64Mileage);
		kQuery.PushStrParam(i64MileageAdd);
		kQuery.PushStrParam(static_cast<BYTE>(GetModifyCashType(rkTran.GetTransactionType(), bRollback)));
		kQuery.PushStrParam(static_cast<BYTE>((bRollback) ? ECASH_LOG_STATE_ROLLBACK : ECASH_LOG_STATE_CASHUSE));
		kQuery.PushStrParam(rkTran.TransactionKey());

		rkTran.WriteToPacket(kQuery.contUserData);
		if (S_OK == g_kCoreCenter.PushQuery(kQuery))
		{
			return true;
		}
		return false;
	}

	void OnReceiveCashResult(BM::Stream* const pkPacket)
	{
		SERVER_IDENTITY kTargetSI;
		kTargetSI.ReadFromPacket(*pkPacket);
		BM::Stream::DEF_STREAM_TYPE wSubType = 0;
		pkPacket->Pop(wSubType);
		switch(wSubType)
		{
		case PT_CN_A_ANS_QUERY_CASH:
			{
				WORD wReturnCode;
				std::wstring strAccountID;
				DWORD dwCash, dwMileage;
				pkPacket->Pop(wReturnCode);
				pkPacket->Pop(strAccountID);
				pkPacket->Pop(dwCash);
				pkPacket->Pop(dwMileage);
				PgTranPointer kTran(*pkPacket);
				kTran.RecvCurrentCash(GALA::GetCashShopResult(static_cast<GALA::EGTDX_ERROR_CODE>(wReturnCode)), static_cast<__int64>(dwCash), static_cast<__int64>(dwMileage));
			}break;
		case PT_CN_A_ANS_BUYCASHITEM:
		case PT_CN_A_ANS_EXPANDTIEM:
		case PT_CN_A_ANS_SENDGIFT:
			{
				WORD wRetCode = 0;
				BM::GUID kCharacterGuid;
				std::wstring strAccountID;
				DWORD dwCash = 0;
				DWORD dwMileage = 0;
				DWORD dwAddedMileage = 0;
				std::wstring strChargeNo;
				pkPacket->Pop(wRetCode);
				pkPacket->Pop(kCharacterGuid);
				pkPacket->Pop(strAccountID);
				pkPacket->Pop(dwCash);
				pkPacket->Pop(dwMileage);
				pkPacket->Pop(dwAddedMileage);
				pkPacket->Pop(strChargeNo);

				std::wstring strCharacterName;
				PgTranPointer kTran(*pkPacket);
				PgTransaction* pkTran = kTran.GetTran();
				if (pkTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, SubType =") << wSubType);
					return;
				}
				if (wRetCode == GALA::EGTDX_ERROR_SUCCESS)
				{
					pkTran->CurCash(static_cast<__int64>(dwCash));
					pkTran->CurMileage(static_cast<__int64>(dwMileage));
					pkTran->AddedMileage(static_cast<__int64>(dwAddedMileage));
					pkTran->ChargeNo(strChargeNo);

					GALA::CashTransaction(*pkTran, false, pkTran->CurCash()-pkTran->InitCash(), pkTran->CurMileage()-pkTran->InitMileage(), dwAddedMileage);
				}
				else
				{
					TRAN_HELPER::RecvCashModify(GALA::GetCashShopResult(static_cast<GALA::EGTDX_ERROR_CODE>(wRetCode)), kTran, dwCash, dwMileage, dwAddedMileage);
				}
			}break;
		case PT_CN_A_ANS_BUYCASHITEM_ROLLBACK:
			{
				WORD wRetCode = 0;
				DWORD dwCashBalance = 0;
				DWORD dwMileage = 0;
				DWORD dwAddedMileage = 0;
				pkPacket->Pop(wRetCode);
				pkPacket->Pop(dwCashBalance);
				pkPacket->Pop(dwMileage);
				pkPacket->Pop(dwAddedMileage);
				PgTranPointer kTran(*pkPacket);
				PgTransaction* pkTran = kTran.GetTran();

				if (pkTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, SubType =") << wSubType);
					return;
				}
				if (wRetCode == GALA::EGTDX_ERROR_SUCCESS)
				{
					__int64 i64CurOldCash = pkTran->CurCash();
					pkTran->CurCash(static_cast<__int64>(dwCashBalance)) ;
					pkTran->CurMileage(static_cast<__int64>(dwMileage));
					pkTran->AddedMileage(static_cast<__int64>(dwAddedMileage));
					GALA::CashTransaction(*pkTran, true, pkTran->CurCash()-i64CurOldCash, 0, 0);
				}
				else
				{
					// Rollback 마저 실패하면?? 대책없다.. Log 라도 남겨주자.
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("PT_CN_A_ANS_BUYCASHITEM_ROLLBACK failed ErroCode =") << wRetCode);
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("PT_CN_A_ANS_BUYCASHITEM_ROLLBACK failed ErroCode =") << wRetCode);
					PgCashManager::OnCashRollback_Failed(GALA::GetCashShopResult(static_cast<GALA::EGTDX_ERROR_CODE>(wRetCode)), kTran);
				}
				
				if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN))
				{//롤백후 캐시 재요청
					PgTranPointer kSubTran(ECASH_TRAN_CASHSHOP_ENTER, pkTran->MemberGuid(), pkTran->CharacterGuid(), pkTran->GroundKey(), pkTran->TargetSI(), CIE_CS_Select);
		
					PgEnterCashShopTran* pkSubTran = dynamic_cast<PgEnterCashShopTran*>(kSubTran.GetTran());
					if(pkSubTran != NULL)
					{
						pkSubTran->UID(pkTran->UID());
						pkSubTran->AccountID(pkTran->AccountID());
						pkSubTran->RemoteAddr(pkTran->RemoteAddr());
					}
					kSubTran.RequestCurerntLimitSell();
				}
			}break;
		case PT_CN_A_ANS_TRAN_COMMIT:
			{//일본 전용/
				if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN))
				{//롤백후 캐시 재요청
					PgTranPointer kTran(*pkPacket);
					PgTransaction* pkTran = kTran.GetTran();
					PgTranPointer kSubTran(ECASH_TRAN_CASHSHOP_ENTER, pkTran->MemberGuid(), pkTran->CharacterGuid(), pkTran->GroundKey(), pkTran->TargetSI(), CIE_CS_Select);
		
					PgEnterCashShopTran* pkSubTran = dynamic_cast<PgEnterCashShopTran*>(kSubTran.GetTran());
					if(pkSubTran != NULL)
					{
						pkSubTran->UID(pkTran->UID());
						pkSubTran->AccountID(pkTran->AccountID());
						pkSubTran->RemoteAddr(pkTran->RemoteAddr());
					}
					kSubTran.RequestCurerntLimitSell();
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("[PT_X_N_ANS_CASH] unknown packet subtype = ") << wSubType);
			}break;
		}
	}
};

namespace NIVAL 
{
	bool SendCashRollback(PgTransaction const& rkTransaction, CASH::SCashCost const& rkCost)
	{
		BM::Stream kPacket(PT_A_CN_REQ_CASH_ROLLBACK);
		kPacket.Push(rkTransaction.AccountID());
		rkTransaction.RemoteAddr().WriteToPacket(kPacket);
		rkCost.WriteToPacket(kPacket);
		rkTransaction.WriteToPacket(kPacket);
		return SendCashRequest(kPacket);
	}

	bool SendCashRequest(BM::Stream const& kPacket)
	{
		BM::Stream kEPacket(PT_N_X_REQ_CASH);
		g_kProcessCfg.ServerIdentity().WriteToPacket(kEPacket);
		kEPacket.Push(kPacket);
		return SendToImmigration(kEPacket);
	}

	ECashShopResult GetCashShopResult(NIVAL::EBILL_ERROR const eCode)
	{
		ECashShopResult eReturn = CSR_SYSTEM_ERROR;
		switch (eCode)
		{
		case EBILL_ERROR_SUCCESS: { eReturn = CSR_SUCCESS; }break;
		case EBILL_ERROR_USER_NOT_FOUND:
		case EBILL_ERROR_TRANSFER_USER_NOT_FOUND:
		case EBILL_ERROR_CANNOT_FIND_USER: { eReturn = CSR_INVALID_CHARACTER; }break;
		case EBILL_ERROR_NOT_ENOUGH_CASH:
		case EBILL_ERROR_NO_COINS: { eReturn = CSR_NOT_ENOUGH_CASH; }break;
		case EBILL_ERROR_USER_BANNED: { eReturn = CSR_BLOCKED_USER; }break;
		case EBILL_ERROR_INVALID_ITEM: { eReturn = CSR_INVALID_ARTICLE; }break;
		case EBILL_ERROR_TOO_SHORT_NAME:
		case EBILL_ERROR_CRC_INVALID:
		case EBILL_ERROR_NO_CLIENTID:
		case EBILL_ERROR_CLIENTID_INVALID:
		case EBILL_ERROR_SYSTEMERROR:
		case EBILL_ERROR_CASHSERVER_MALFUNC:
		default: { eReturn = CSR_SYSTEM_ERROR; }break;
		}
		return eReturn;
	}

	void OnReceiveCashResult(BM::Stream* const pkPacket)
	{
		SERVER_IDENTITY kTargetSI;
		BM::Stream::DEF_STREAM_TYPE wSubType = 0;
		NIVAL::SCashResult kResult;

		kTargetSI.ReadFromPacket(*pkPacket);
		pkPacket->Pop(wSubType);
		kResult.ReadFromPacket(*pkPacket);
		PgTranPointer kTran(kResult.kAddonPacket);


		switch(wSubType)
		{
		case PT_CN_A_ANS_QUERY_CASH:
			{
				kTran.RecvCurrentCash(NIVAL::GetCashShopResult(kResult.eError), kResult.i64Cash, kResult.i64Mileage);
			}break;
		case PT_CN_A_ANS_BUYCASHITEM:
		case PT_CN_A_ANS_EXPANDTIEM:
		case PT_CN_A_ANS_SENDGIFT:
		case PT_CN_A_ANS_OM_BUY_ARTICLE:
		case PT_CN_A_ANS_ADDCASH:
		case PT_CN_A_ANS_OM_REG_ARTICLE:
			{
				PgTransaction* pkTran = kTran.GetTran();
				if (pkTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, SubType =") << wSubType);
					return;
				}
				if (kResult.eError == NIVAL::EBILL_ERROR_SUCCESS)
				{
					pkTran->CurCash(kResult.i64Cash);
					pkTran->CurMileage(kResult.i64Mileage);
					pkTran->AddedMileage(kResult.i64AddedMileage);

					GALA::CashTransaction(*pkTran, false, pkTran->CurCash()-pkTran->InitCash(), pkTran->CurMileage()-pkTran->InitMileage(), kResult.i64AddedMileage);
				}
				else
				{
					TRAN_HELPER::RecvCashModify(NIVAL::GetCashShopResult(kResult.eError), kTran, kResult.i64Cash, kResult.i64Mileage, kResult.i64AddedMileage);
				}
			} break;
		case PT_CN_A_ANS_CASH_ROLLBACK:
			{
				PgTransaction* pkTran = kTran.GetTran();
				if (pkTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, SubType =") << wSubType);
					return;
				}
				if (kResult.eError == NIVAL::EBILL_ERROR_SUCCESS)
				{
					pkTran->CurCash(kResult.i64Cash);
					pkTran->CurMileage(kResult.i64Mileage);
					pkTran->AddedMileage(kResult.i64AddedMileage);
					GALA::CashTransaction(*pkTran, true, pkTran->CurCash()-pkTran->InitCash(), pkTran->CurMileage()-pkTran->InitMileage(), kResult.i64AddedMileage);
				}
				else
				{
					// Rollback 마저 실패하면?? 대책없다.. Log 라도 남겨주자.
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("PT_CN_A_ANS_CASH_ROLLBACK failed ErroCode =") << static_cast<int>(kResult.eError));
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("PT_CN_A_ANS_CASH_ROLLBACK failed ErroCode =") << static_cast<int>(kResult.eError));
					PgCashManager::OnCashRollback_Failed(NIVAL::GetCashShopResult(kResult.eError), kTran);
				}
			} break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unhandled packet Type=") << wSubType);
			}break;;
		}
	}
};

namespace NC
{
	ECashShopResult GetCashShopResult(NC::E_NC_SA_RESULT_CODE const eCode)
	{
		ECashShopResult eReturn = CSR_SYSTEM_ERROR;
		switch(eCode)
		{		
		case SARC_SUCCESS:			{ eReturn = CSR_SUCCESS; }break;
		case SARC_INVALID_ACOUNT:	{ eReturn = CSR_INVALID_CHARACTER; }break;
		case SARC_NOT_ENOUGH_POINT: { eReturn = CSR_NOT_ENOUGH_CASH; }break;
		case SARC_DB_ERROR:			{ eReturn = CSR_DB_ERROR; }break;
		default: { eReturn = CSR_SYSTEM_ERROR; }break;
		}
		return eReturn;
	}

	void OnReceiveCashResult(BM::Stream* const pkPacket)
	{
		SERVER_IDENTITY kTargetSI;
		kTargetSI.ReadFromPacket(*pkPacket);
		BM::Stream::DEF_STREAM_TYPE wSubType = 0;
		pkPacket->Pop(wSubType);
		CASH::SCashCost kCost;
		pkPacket->Pop(kCost);

		switch(wSubType)
		{
		case PT_CN_A_ANS_QUERY_CASH:
			{
				SNcCash kNcCash;
				kNcCash.ReadFromPacket(*pkPacket);
				PgTranPointer kTran(*pkPacket);
				kTran.RecvCurrentCash( NC::GetCashShopResult(static_cast<NC::E_NC_SA_RESULT_CODE>(kNcCash.uiResult)),
					kNcCash.ui64Cash, kNcCash.ui64HappyCoin ); // NC는 마일리지 대신 해피포인트 사용
			}break;
		case PT_CN_A_ANS_BUYCASHITEM:
		case PT_CN_A_ANS_SENDGIFT:
			{
				unsigned int uiResult = 0;
				unsigned __int64 ui64WarehouseNo = 0;

				pkPacket->Pop(uiResult);
				if( uiResult == NC::SARC_SUCCESS )
				{
					pkPacket->Pop(ui64WarehouseNo);
				}

				PgTranPointer kTran(*pkPacket);
				PgTransaction* pkTran = kTran.GetTran();
				if(pkTran == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, SubType =") << wSubType);
					return;
				}

				// NC는 캐시 가감을 IBO_POINT_CHANGED_NOTI 패킷으로 처리한다.
				// 여기는 남은 금액이 오지 않기 때문에 아이템 구매전 금액으로 셋팅한다.
				pkTran->CurCash( pkTran->InitCash() );
				pkTran->CurMileage( pkTran->InitMileage() );
				pkTran->AddedMileage( 0i64 );

				if(uiResult == NC::SARC_SUCCESS)
				{
					// SA는 IBI_DELETE_ITEMS_REQ 프로토콜을 받은 아이템들만 정상적으로 처리되었다고 판단.
					// 이값을 저장하고 있다가 정상적으로 아이템 구매가 완료 되면 이값을 다시 IBI_DELETE_ITEMS_REQ 패킷으로 보내야한다.				
					pkTran->WarehouseNo(ui64WarehouseNo);

					GALA::CashTransaction( *pkTran, false, pkTran->CurCash(), pkTran->CurMileage(), pkTran->AddedMileage() );
				}
				else
				{
					TRAN_HELPER::RecvCashModify( NC::GetCashShopResult(static_cast<NC::E_NC_SA_RESULT_CODE>(uiResult)), 
						kTran, pkTran->CurCash(), pkTran->CurMileage(), pkTran->AddedMileage() );
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unhandled packet Type=") << wSubType);
			}break;;
		}
	}
};

namespace GRAVITY
{
	void OnReceiveCashResult(BM::Stream* const pkPacket)
	{// 마일리지샵 오픈전까지 마일리지샵 관련 모두 0i64
		SERVER_IDENTITY kTargetSI;
		BM::Stream::DEF_STREAM_TYPE wSubType = 0;
		E_CASH_SHOP_RESULT eResult = CSR_SYSTEM_ERROR;
		__int64 i64RemainCash = 0i64;
		__int64 i64RemainMileage = 0i64;

		kTargetSI.ReadFromPacket(*pkPacket);
		pkPacket->Pop(wSubType);
		pkPacket->Pop( eResult );
		pkPacket->Pop( i64RemainCash );
		pkPacket->Pop( i64RemainMileage );
		PgTranPointer kTran(*pkPacket);

		switch(wSubType)
		{
		case PT_CN_A_ANS_QUERY_CASH:
			{
				kTran.RecvCurrentCash( eResult, i64RemainCash, 0i64 ); 
			}break;
		case PT_CN_A_ANS_BUYCASHITEM:
		case PT_CN_A_ANS_SENDGIFT:
			{
				PgTransaction* pkTran = kTran.GetTran();
				if( NULL == pkTran )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, SubType =") << wSubType);
					return;
				}

				if( CSR_SUCCESS == eResult )
				{
					pkTran->CurCash( i64RemainCash );
					pkTran->CurMileage( 0i64 );
					pkTran->AddedMileage( 0i64 );

					GALA::CashTransaction( *pkTran, false, pkTran->CurCash() - pkTran->InitCash(), pkTran->CurMileage() - pkTran->InitMileage(), 0i64 );
				}
				else
				{
					TRAN_HELPER::RecvCashModify( eResult, kTran, i64RemainCash, 0i64, 0i64 );
				}
			}break;		
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unhandled packet Type=") << wSubType);
			}break;
		}
	}
};