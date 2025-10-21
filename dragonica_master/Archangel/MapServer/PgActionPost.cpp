#include "stdafx.h"
#include "BM/PgFilterString.h"
#include "PgAction.h"
#include "PgActionPost.h"
#include "Global.h"

#ifdef _DEBUG
#pragma warning (push, 4)
#endif

PgPostReqMailSend::PgPostReqMailSend(SGroundKey const & kGndKey, BM::Stream & kPacket)
	: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

EPostMailSendResult PgPostReqMailSend::OnProcess(CUnit* pkCaster)
{
	PgInventory* pkCasterInv = pkCaster->GetInven();

	std::wstring sToName,sTitle,sText;
	BM::GUID kItemGuid;
	SItemPos kPos;
	__int64 i64Money = 0i64;
	bool	bPaymentType;

	m_krPacket.Pop(sToName);
	m_krPacket.Pop(sTitle);
	m_krPacket.Pop(sText);
	m_krPacket.Pop(kPos);
	m_krPacket.Pop(i64Money);
	m_krPacket.Pop(bPaymentType);

	i64Money = std::max(0i64, i64Money);

	if(sTitle.length() > MAX_MAIL_TITLE_LEN)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_TITLE_TOO_LONG"));
		return PMSR_TITLE_TOO_LONG;
	}

	if(sText.length() > MAX_MAIL_TEXT_LEN)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_TEXT_TOO_LONG"));
		return PMSR_TEXT_TOO_LONG;
	}

	if(sToName == pkCaster->Name())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_CANT_SEND_TO_SELF"));
		return PMSR_CANT_SEND_TO_SELF;
	}

	if( false == g_kUnicodeFilter.IsCorrect(UFFC_MAIL_SUBJECT, sTitle) )
	{
		return PMSR_BADWORD_SUBJECT;
	}

	if( false == g_kUnicodeFilter.IsCorrect(UFFC_MAIL_SUBJECT, sText) )
	{
		return PMSR_BADWORD_CONTENTS;
	}


	SSendMailInfo kMailInfo;
	kMailInfo.FromGuid(pkCaster->GetID());
	kMailInfo.FromName(pkCaster->Name());
	kMailInfo.TargetName(sToName);
	kMailInfo.MailTitle(sTitle);
	kMailInfo.MailText(sText);
	kMailInfo.Money(i64Money);
	kMailInfo.PaymentType(bPaymentType);

	CONT_PLAYER_MODIFY_ORDER kOrder;

	__int64 const i64MailSendCost = SEND_MAIL_COST;
	__int64 const iCasterMoney = pkCaster->GetAbil64(AT_MONEY);

	if(iCasterMoney < i64MailSendCost)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_NOT_ENOUGH_MONEY"));
		return PMSR_NOT_ENOUGH_MONEY;
	}

	{
		SPMOD_Add_Money kMailSendCost(-i64MailSendCost);
		SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(),kMailSendCost);
		kOrder.push_back(kIMO);
	}

	__int64 i64TotalMoney = i64MailSendCost;

	PgBase_Item kItem;
	pkCasterInv->GetItem(kPos, kItem);

	if(0 < i64Money)
	{
		if(kMailInfo.PaymentType())
		{
			// 다른 사람에게 대금을 청구 할 경우 아이템을 주지 않으면 ?? 오류인가??
			if(PgBase_Item::IsEmpty(&kItem))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_PAYMENT_NEED_ITEM"));
				return PMSR_PAYMENT_NEED_ITEM;
			}
		}
		else
		{
			//돈을 남에게 주는 경우 처리

			i64TotalMoney += i64Money;

			if(iCasterMoney < i64TotalMoney)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_NOT_ENOUGH_MONEY"));
				return PMSR_NOT_ENOUGH_MONEY;
			}

			// 자신의 돈을 깍아서 메일 아이템 보관함으로 옮긴다.
			SPMOD_Add_Money kModifyAdd(-i64Money);
			SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(),kModifyAdd);
			kOrder.push_back(kIMO);
		}
	}

	BM::Stream kSendPacket(PT_M_I_POST_REQ_MAIL_SEND);

	kSendPacket.Push(CIE_Post_Mail_Send);
	kSendPacket.Push(pkCaster->GetID());
	kSendPacket.Push(pkCaster->Name());
	kSendPacket.Push(sToName);

	if(!PgBase_Item::IsEmpty(&kItem))
	{
		if(false == CheckEnableTrade(kItem,static_cast<EItemCantModifyEventType>(ICMET_Cant_SendMail | ICMET_Cant_PlayerTrade)))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_CANT_SEND_ITEM"));
			return PMSR_CANT_SEND_ITEM;
		}

		kMailInfo.ItemGuid(kItem.Guid());

		SPMOD_Modify_Pos kModifyPos(SItemPos(IT_POST,0),kPos, PgBase_Item::NullData(), kItem);// 아이템을 인벤에서 우편함으로 옮긴다.
		kOrder.push_back(SPMO(IMET_MODIFY_POS, pkCaster->GetID(),kModifyPos));

		kOrder.push_back(SPMO(IMET_CREATE_MAIL,pkCaster->GetID(),kMailInfo));
	}
	else
	{
		kOrder.push_back(SPMO(IMET_CREATE_MAIL,pkCaster->GetID(),kMailInfo));
	}

	kOrder.WriteToPacket(kSendPacket);

	if(!SendToItem(m_kGndKey,kSendPacket))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMSR_NET_ERROR"));
		return PMSR_NET_ERROR;
	}

	return PMSR_SUCCESS;
}

bool PgPostReqMailSend::DoAction(CUnit* pkCaster, CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	EPostMailSendResult kErr = OnProcess(pkCaster);
	if(PMSR_SUCCESS == kErr)
		return true;

	BM::Stream kPacket;
	kPacket.Push(PT_M_C_POST_ANS_MAIL_SEND);
	kPacket.Push(kErr);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

PgPostReqMailRecv::PgPostReqMailRecv(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgPostReqMailRecv::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket(PT_M_I_POST_REQ_MAIL_RECV);
	kPacket.Push(pkCaster->GetID());
	kPacket.Push(m_krPacket);
	return SendToItem(m_kGndKey,kPacket);
}

PgPostReqMailModify::PgPostReqMailModify(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgPostReqMailModify::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kMailGuid;
	m_krPacket.Pop(kMailGuid);
	EPostMailModifyType kMailModifyType;
	m_krPacket.Pop(kMailModifyType);

	CONT_PLAYER_MODIFY_ORDER kOrder;

	BM::Stream kPacket(PT_M_I_POST_REQ_MAIL_MODIFY);
	kPacket.Push(pkCaster->GetID());

	switch(kMailModifyType)
	{
	case PMMT_READ:
	case PMMT_DELETE:
	case PMMT_RETURN:
		{
			kPacket.Push(CIE_Post_Mail_Modify);
		}break;
	case PMMT_GET_ANNEX:
		{
			kPacket.Push(CIE_Post_Mail_Item_Recv);
		}
		break;
	}

	kPacket.Push(kMailGuid);
	kPacket.Push(kMailModifyType);

	return SendToItem(m_kGndKey,kPacket);
}