#include <stdafx.h>
#include "GALA_Constant.h"

namespace GALA {
BM::vstring BILL_PACK_BALANCE::ToVString() const
{
	BM::vstring vOut;
	vOut << _T("Type=Balance, Key=") << kHeader.ReqKey;
	vOut << _T("\n\tRet=") << RetCode << _T(", UserID=") << UserID << _T(", UserNo=") << UserNo << _T(", Cash=") << CashBalance;
	return vOut;
}

BM::vstring BILL_PACK_BUY::ToVString() const
{
	BM::vstring vOut;
	vOut << _T("Type=BUY_ITEM, Key=") << kHeader.ReqKey;
	vOut << _T("\n\tRet=") << RetCode << _T(", UserID=") << UserID << _T(", UserNo=") << UserNo << _T(", Lang=") << LangCode
		<< _T(", IP=") << ClientIP << _T(", Server=") << Server_index << _T(", CharGuid=") << Character_id << _T(", CharName=") << Character_name
		<< _T(", ItemID=") << Item_code << _T(", Itemname=") << Item_name << _T(", ItemHour=") << Item_term << _T(", Price=") << Item_price 
		<< _T(", Cash=") << CashBalance << _T(", CharageNo=") << ChargeNo;
	return vOut;
}

BM::vstring BILL_PACK_GIFT::ToVString() const
{
	BM::vstring vOut;
	vOut << _T("Type=GIFT_ITEM, Key=") << kHeader.ReqKey;
	vOut << _T("\n\tRet=") << RetCode << _T(", UserID=") << UserID << _T(", UserNo=") << UserNo << _T(", Lang=") << LangCode
		<< _T(", IP=") << ClientIP << _T(", Server=") << Server_index << _T(", CharGuid=") << Character_id << _T(", CharName=") << Character_name
		<< _T(", RecvGuid=") << Friend_id << _T(", RecvName=") << Friend_name
		<< _T(", ItemID=") << Item_code << _T(", Itemname=") << Item_name << _T(", ItemHour=") << Item_term << _T(", Price=") << Item_price 
		<< _T(", Cash=") << CashBalance << _T(", CharageNo=") << ChargeNo;
	return vOut;
}

BM::vstring BILL_PACK_BUY_CNL::ToVString() const
{
	BM::vstring vOut;
	vOut << _T("Type=BUY_CANCEL, Key=") << kHeader.ReqKey;
	vOut << _T("\n\tRet=") << RetCode << _T(", UserID=") << UserID << _T(", UserNo=") << UserNo 
		<< _T(", Cash=") << CashBalance << _T(", CharageNo=") << ChargeNo;
	return vOut;
}

BM::vstring BILL_PACK_ISALIVE::ToVString() const
{
	BM::vstring vOut;
	vOut << _T("Type=ISALIVE, Key=") << kHeader.ReqKey;
	vOut << _T("\n\tRet=") << RetCode;
	return vOut;
}

};	// namespace GALA