#ifndef WEAPON_AILEPACK_NIVAL_NIVAL_CONSTANT_H
#define WEAPON_AILEPACK_NIVAL_NIVAL_CONSTANT_H

namespace NIVAL {

typedef enum
{
	EAUTH_ERROR_SUCCESS = 0,
	EAUTH_ERROR_EMAIL_NOTCREATED = 1,	// Email was not created
	EAUTH_ERROR_EMAIL_NOTCREATED2 = 2,	// Email was not created
	EAUTH_ERROR_LOGIN_NOTCREATED2 = 3,	// Login was not created)
	EAUTH_ERROR_LOGIN_INVALID = 4,		// (Email was not created)
	EAUTH_ERROR_LOGIN_NOTCREATED3 = 6,	// Login was not created)
	EAUTH_ERROR_LOGIN_NOTCREATED = 8,	// Login was not created) ?? 뭔말이래?
	EAUTH_ERROR_EMAIL_NOTEXIST = 9,		// email is not existed yet
	EAUTH_ERROR_EMAIL_INVALID = 10,		// email is not valid
	EAUTH_ERROR_ACCOUNT_NOTCREATED = 38,// (Account was not created)
	EAUTH_ERROR_WRONG_PASSWORD = 39,	// Password error
	EAUTH_ERROR_NEED_BETA_TESTACCOUNT = 995,	// (You need a beta-test account)
	EAUTH_ERROR_NO_XMLDATA = 998,		// (XML data was not transfered)
	EAUTH_ERROR_METHOD_ERROR = 999,		//  (Method was not found)
} EAUTH_ERROR;

typedef enum
{
	EBILL_ERROR_SUCCESS = 0,
	EBILL_ERROR_TOO_SHORT_NAME = 2,		// Name is too short)
	EBILL_ERROR_USER_NOT_FOUND = 3,		// (User not found)
	EBILL_ERROR_CRC_INVALID = 4,		// CRC signature error)
	EBILL_ERROR_NO_CLIENTID = 5,		// ClientID is not entered.)
	EBILL_ERROR_CLIENTID_INVALID = 6,	// (ClientID is not valid.)
	EBILL_ERROR_TRANSFER_USER_NOT_FOUND = 8,	// (User needed for transfer is not found.)
	EBILL_ERROR_NOT_ENOUGH_CASH = 9,	// (Number of ZZima coins of ZZima account is less than it is needed.)
	EBILL_ERROR_NO_COINS = 10,			// (There is no such ZZima account or there are no ZZima coins on it.)
	EBILL_ERROR_USER_BANNED = 11,		//  (User is banned)
	// Barunson 에서 추가한 에러 코드
	EBILL_ERROR_SYSTEMERROR = 101,			// 기타 오동작
	EBILL_ERROR_INVALID_ITEM = 102,			// Item 정보가 없다.
	EBILL_ERROR_CANNOT_FIND_USER = 103,		// Account 정보가 없다(online 상태 아니다)
	EBILL_ERROR_CASHSERVER_MALFUNC = 104,	// Cash연동서버가 오동작 했다.
} EBILL_ERROR;

typedef struct _SCashResult
{
	_SCashResult()
	{
		wReqType = 0;
		eError = EBILL_ERROR_SUCCESS;
		i64Cash = i64Mileage = i64AddedMileage = 0;
	}

	explicit _SCashResult(std::wstring const& _ID, SERVER_IDENTITY_SITE const& _SI, PACKET_ID_TYPE const _ReqPacket, EBILL_ERROR const _Error, std::wstring const& _ErrorMsg,
		__int64 const _Cash, __int64 const _Mileage, __int64 const _AddedMileage, BM::Stream const& rkAddon)
		: strAccountID(_ID), kSI(_SI), wReqType(_ReqPacket), eError(_Error), strErrorMsg(_ErrorMsg),
		i64Cash(_Cash), i64Mileage(_Mileage), i64AddedMileage(_AddedMileage)
	{
		kAddonPacket.Push(rkAddon);
	}

	_SCashResult(_SCashResult const& rhs)
		: strAccountID(rhs.strAccountID), kSI(rhs.kSI), wReqType(rhs.wReqType), eError(rhs.eError), strErrorMsg(rhs.strErrorMsg),
		i64Cash(rhs.i64Cash), i64Mileage(rhs.i64Mileage), i64AddedMileage(rhs.i64AddedMileage)
	{
		kAddonPacket = rhs.kAddonPacket;
	}

	_SCashResult const& operator=(_SCashResult const& rhs)
	{
		strAccountID = rhs.strAccountID;
		wReqType = rhs.wReqType;
		eError = rhs.eError;
		strErrorMsg = rhs.strErrorMsg;
		i64Cash = rhs.i64Cash;
		i64Mileage = rhs.i64Mileage;
		i64AddedMileage = rhs.i64AddedMileage;
		kSI = rhs.kSI;
		kAddonPacket = rhs.kAddonPacket;
		return (*this);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(strAccountID);
		kPacket.Push(wReqType);
		kPacket.Push(eError);
		kPacket.Push(strErrorMsg);
		kPacket.Push(i64Cash);
		kPacket.Push(i64Mileage);
		kPacket.Push(i64AddedMileage);
		kSI.WriteToPacket(kPacket);
		kPacket.Push(kAddonPacket.Data());
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(strAccountID);
		kPacket.Pop(wReqType);
		kPacket.Pop(eError);
		kPacket.Pop(strErrorMsg);
		kPacket.Pop(i64Cash);
		kPacket.Pop(i64Mileage);
		kPacket.Pop(i64AddedMileage);
		kSI.ReadFromPacket(kPacket);
		kAddonPacket.Reset();
		kPacket.Pop(kAddonPacket.Data());
		kAddonPacket.PosAdjust();
	}

	std::wstring strAccountID;
	PACKET_ID_TYPE wReqType;
	EBILL_ERROR eError;
	std::wstring strErrorMsg;
	__int64 i64Cash;
	__int64 i64Mileage;
	__int64 i64AddedMileage;
	SERVER_IDENTITY_SITE kSI;
	BM::Stream kAddonPacket;
} SCashResult;

};




#endif // WEAPON_AILEPACK_NIVAL_NIVAL_CONSTANT_H