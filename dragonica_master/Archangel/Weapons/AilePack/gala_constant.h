#ifndef WEAPON_AILEPACK_GALA_GALA_CONSTANT_H
#define WEAPON_AILEPACK_GALA_GALA_CONSTANT_H

namespace GALA {

#pragma pack(1)
	
typedef enum : short
{
	EUSER_REQ_NONE = 0,
	EUSER_REQ_LOGIN = 1,
	EUSER_REQ_LOGOUT = 2,
	EUSER_REQ_LOGIN_CASHQUERY = 3,
	// GTDX Request type (GTDX order type과 일치하는 값)
	EUSER_REQ_CASH_QUERY = 10,
	EUSER_REQ_ITEM_BUY = 20,
	EUSER_REQ_GIFT = 30,
	EUSER_REQ_CANCEL_ITEM_BUY = 40,
	EUSER_REQ_SERVER_STATE = 90,
} EUSER_REQ_TYPE;

typedef enum
{
	EGALA_ERROR_SUCESS = 0,
	EGALA_ERROR_SYSTEM_ERROR = 1000,
	EGALA_ERROR_OUTOFSERICE = 2000,
	EGALA_ERROR_BLOCKED_USER = 3000,
} EGALA_LOGIN_ERROR_CODE;

typedef enum
{
	EGTDX_ERROR_SUCCESS = 0,
	EGTDX_ERROR_CASH_SHORTAGE = 100,	// 잔액부족
	EGTDX_ERROR_CANNOT_BUY = 101,		// 구매불가(차단된 유저 등)
	EGTDX_ERROR_NOT_EXIST = 200,		// 존재하지 않는 유저
	EGTDX_ERROR_BILLINGSERVER_ERROR = 400,	// 빌링서버 내부 오류
	EGTDX_ERROR_ALREADY_CANCEL = 500,	// 이미 취소된 건

	// Dragonica에서 자체적으로 만든 에러 코드
	EGTDX_ERROR_DUPLICATE_REQUEST = 60101,
	EGTDX_ERROR_SYSTEM_ERROR = 60102,
	EGTDX_ERROR_UNKNOWN = 60103,
} EGTDX_ERROR_CODE;	// GTDX 연동과 일치하는 Error Code

size_t const GTDX_MAX_USER_ID_LEN = 50;
size_t const GTDX_MAX_LANGUAGE_CODE_LEN = 2;
size_t const GTDX_MAX_IP_LEN = 20;
size_t const GTDX_MAX_CHARACTER_NAME_LEN = 20;
size_t const GTDX_MAX_ITEM_NAME_LEN = 100;
size_t const GTDX_MAX_CHARGE_NO_LEN = 19;

typedef struct _GTDX_HEADER
{
	explicit _GTDX_HEADER(GALA::EUSER_REQ_TYPE const eReq)
		: ReqType(eReq)
	{
		ReqKey = 0;
	}

	//WORD ReqLen;
	WORD const ReqType;
	DWORD ReqKey;
private:
	_GTDX_HEADER();
} GTDX_HEADER;

typedef struct _BILL_PACK_BALANCE
{
	_BILL_PACK_BALANCE()
		: kHeader(EUSER_REQ_CASH_QUERY) 
	{
		RetCode = 0;
		UserID[0] = _T('\0');
		UserNo = 0; CashBalance = 0;
	}

	GTDX_HEADER kHeader;

	WORD RetCode;
	WCHAR UserID[GTDX_MAX_USER_ID_LEN+1];
	DWORD UserNo;
	DWORD CashBalance;

	BM::vstring ToVString() const;
} BILL_PACK_BALANCE, *PBILL_PACK_BALANCE;

typedef struct _BILL_PACK_BUY
{
	_BILL_PACK_BUY()
		: kHeader(EUSER_REQ_ITEM_BUY) 
	{
		RetCode = 0;
		UserNo = 0;
		LangCode[0] = _T('\0');
		ClientIP[0] = _T('\0');
		Server_index = 0;
		Character_name[0] = _T('\0');
		Item_code = 0;
		Item_name[0] = _T('\0');
		Item_term = 0;
		Item_price = 0;
		CashBalance = 0;
		ChargeNo[0] = '\0';
	}

	GTDX_HEADER kHeader;

	WORD RetCode;
	DWORD UserNo;
	WCHAR UserID[GTDX_MAX_USER_ID_LEN+1];
	WCHAR LangCode[GTDX_MAX_LANGUAGE_CODE_LEN+1];
	WCHAR ClientIP[GTDX_MAX_IP_LEN+1];
	WORD Server_index;
	GUID Character_id;
	WCHAR Character_name[GTDX_MAX_CHARACTER_NAME_LEN+1];
	DWORD Item_code;
	WCHAR Item_name[GTDX_MAX_ITEM_NAME_LEN+1];
	WORD Item_term;
	DWORD Item_price;
	DWORD CashBalance;
	char ChargeNo[GTDX_MAX_CHARGE_NO_LEN+1];

	BM::vstring ToVString() const;
} BILL_PACK_BUY, *PBILL_PACK_BUY;

typedef struct _BILL_PACK_GIFT
{
	_BILL_PACK_GIFT()
		: kHeader(EUSER_REQ_GIFT) 
	{
		RetCode = 0;
		UserNo = 0;
		UserID[0] = _T('\0');
		LangCode[0] = _T('\0');
		ClientIP[0] = _T('\0');
		Server_index = 0;
		Character_name[0] = _T('\0');
		Friend_name[0] = _T('\0');
		Item_code = 0;
		Item_name[0] = _T('\0');
		Item_term = 0;
		Item_price = 0;
		CashBalance = 0;
		ChargeNo[0] = '\0';
	}

	GTDX_HEADER kHeader;

	WORD RetCode;
	DWORD UserNo;
	WCHAR UserID[GTDX_MAX_USER_ID_LEN+1];
	WCHAR LangCode[GTDX_MAX_LANGUAGE_CODE_LEN+1];
	WCHAR ClientIP[GTDX_MAX_IP_LEN+1];
	WORD Server_index;
	GUID Character_id;
	WCHAR Character_name[GTDX_MAX_CHARACTER_NAME_LEN+1];
	GUID Friend_id;
	WCHAR Friend_name[GTDX_MAX_CHARACTER_NAME_LEN+1];
	DWORD Item_code;
	WCHAR Item_name[GTDX_MAX_ITEM_NAME_LEN+1];
	WORD Item_term;
	DWORD Item_price;
	DWORD CashBalance;
	char ChargeNo[GTDX_MAX_CHARGE_NO_LEN+1];

	BM::vstring ToVString() const;
} BILL_PACK_GIFT, *PBILL_PACK_GIFT;

typedef struct _BILL_PACK_BUY_CNL
{
	_BILL_PACK_BUY_CNL()
		: kHeader(EUSER_REQ_CANCEL_ITEM_BUY)
	{
		RetCode = 0;
		UserNo = 0;
		UserID[0] = _T('\0');
		CashBalance = 0;
		ChargeNo[0] = '\0';
	}

	GTDX_HEADER kHeader;

	WORD RetCode;
	DWORD UserNo;
	WCHAR UserID[GTDX_MAX_USER_ID_LEN+1];
	DWORD CashBalance;
	char ChargeNo[GTDX_MAX_CHARGE_NO_LEN+1];

	BM::vstring ToVString() const;
} BILL_PACK_BUY_CNL, *PBILL_PACK_BUY_CNL;

typedef struct _BILL_PACK_ISALIVE
{
	_BILL_PACK_ISALIVE()
		: kHeader(EUSER_REQ_SERVER_STATE)
	{
		RetCode = 0;
	}

	GTDX_HEADER kHeader;

	WORD RetCode;
	BM::vstring ToVString() const;
} BILL_PACK_ISALIVE, *PBILL_PACK_ISALIVE;

#pragma pack()

};





#endif // WEAPON_AILEPACK_GALA_GALA_CONSTANT_H