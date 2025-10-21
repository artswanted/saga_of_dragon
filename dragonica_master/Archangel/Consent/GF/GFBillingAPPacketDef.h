#ifndef CONSENT_CONSENTSERVER_LINKAGE_GFBILLINGAPPACKETDEF_H
#define CONSENT_CONSENTSERVER_LINKAGE_GFBILLINGAPPACKETDEF_H

#include "Lohengrin/PacketStruct_GFAP.h"

using namespace GFAP_FUNCTION;

#pragma pack(1)

int const MAX_STRING_LEN_BILL = 20;

typedef struct tagPT_GF_AP_REQ_OVERLAP_ACCOUNT
{
	typedef S_GFAP_F_OVERLAP_ACCOUNT		REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_OVERLAP_ACCOUNT()
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	BM::GUID	kRequestID;
	wchar_t		wszAccount[MAX_STRING_LEN_BILL];

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
			kData.kOrderID = kRequestID;
			kData.wstrID = wszAccount;
			UPR( kData.wstrID );
			return true;
		}
		
		iOutErr = GFAP_RET_ID_STRING_ERROR;
		return false;
	}

}S_PT_GF_AP_REQ_OVERLAP_ACCOUNT;

typedef struct tagPT_AP_GF_ANS_OVERLAP_ACCOUNT
{
	typedef S_GFAP_F_OVERLAP_ACCOUNT_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_OVERLAP_ACCOUNT	REQ_TYPE;

	tagPT_AP_GF_ANS_OVERLAP_ACCOUNT()
		:	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_OVERLAP_ACCOUNT( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_OVERLAP_ACCOUNT( REQ_TYPE const & rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID	kRequestID;
	wchar_t		wszAccount[MAX_STRING_LEN_BILL];
	int			iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_OVERLAP_ACCOUNT;

typedef struct tagPT_GF_AP_REQ_CREATE_ACCOUNT
{
	typedef S_GFAP_CREATE_ACCOUNT	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_CREATE_ACCOUNT()
		:	cGender(0)
		,	byUseMobileLock(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
		::memset( wszPassword, 0, sizeof(wszPassword) );
		::memset( &kBirthDay, 0, sizeof(kBirthDay) );
	}

	BM::GUID	kRequestID;
	wchar_t		wszAccount[MAX_STRING_LEN_BILL];
	wchar_t		wszPassword[MAX_STRING_LEN_BILL];
	char		cGender;
	DBTIMESTAMP	kBirthDay;
	BYTE		byUseMobileLock;

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
			if ( true ==  StrIsCorrect( wszPassword ) )
			{
				kData.kOrderID = kRequestID;
				kData.wstrID = wszAccount;
				kData.wstrPW = wszPassword;
				kData.cGender = cGender;
				kData.kBirthDay = kBirthDay;
				kData.byUseMobileLock = byUseMobileLock;
				UPR( kData.wstrID );
				return true;
			}
			iOutErr = GFAP_RET_PW_STRING_ERROR;
		}
		else
		{
			iOutErr = GFAP_RET_ID_STRING_ERROR;
		}
		return false;
	}

}S_PT_GF_AP_REQ_CREATE_ACCOUNT;

typedef struct tagPT_AP_GF_ANS_CREATE_ACCOUNT
{
	typedef S_GFAP_F_OVERLAP_ACCOUNT_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_CREATE_ACCOUNT	REQ_TYPE;

	tagPT_AP_GF_ANS_CREATE_ACCOUNT()
		:	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_CREATE_ACCOUNT( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_CREATE_ACCOUNT( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID	kRequestID;
	wchar_t		wszAccount[MAX_STRING_LEN_BILL];
	int			iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_CREATE_ACCOUNT;

typedef struct tagPT_GF_AP_REQ_CHECK_ACCOUNT
{
	typedef S_GFAP_F_CHECK_ACCOUNTPW	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_CHECK_ACCOUNT()
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
		::memset( wszPassword, 0, sizeof(wszPassword) );
	}

	BM::GUID	kRequestID;
	wchar_t		wszAccount[MAX_STRING_LEN_BILL];
	wchar_t		wszPassword[MAX_STRING_LEN_BILL];

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
			if ( true ==  StrIsCorrect( wszPassword ) )
			{
				kData.kOrderID = kRequestID;
				kData.wstrID = wszAccount;
				kData.wstrPW = wszPassword;
				UPR( kData.wstrID );
				return true;
			}
			iOutErr = GFAP_RET_PW_STRING_ERROR;
		}
		else
		{
			iOutErr = GFAP_RET_ID_STRING_ERROR;
		}
		return false;
	}

}S_PT_GF_AP_REQ_CHECK_ACCOUNT;

typedef struct tagPT_AP_GF_ANS_CHECK_ACCOUNT
{
	typedef S_GFAP_F_OVERLAP_ACCOUNT_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_CHECK_ACCOUNT	REQ_TYPE;

	tagPT_AP_GF_ANS_CHECK_ACCOUNT()
		:	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_CHECK_ACCOUNT( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_CHECK_ACCOUNT( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	int		iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_CHECK_ACCOUNT;

typedef struct tagPT_GF_AP_REQ_GET_GAMEPOINT
{
	typedef S_GFAP_F_OVERLAP_ACCOUNT	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_GET_GAMEPOINT()
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
			kData.kOrderID = kRequestID;
			kData.wstrID = wszAccount;
			UPR( kData.wstrID );
			return true;
		}
		iOutErr = GFAP_RET_ID_STRING_ERROR;
		return false;
	}

}S_PT_GF_AP_REQ_GET_GAMEPOINT;

typedef struct tagPT_AP_GF_ANS_GET_GAMEPOINT
{
	typedef S_GFAP_F_GET_CASH_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_GET_GAMEPOINT	REQ_TYPE;

	tagPT_AP_GF_ANS_GET_GAMEPOINT()
		:	i64GamePoint(0i64)
		,	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_GET_GAMEPOINT( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	i64GamePoint(rhs.i64NowCash)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_GET_GAMEPOINT( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
		,	i64GamePoint(0i64)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	__int64 i64GamePoint;
	int		iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_GET_GAMEPOINT;

typedef struct tagPT_GF_AP_REQ_ADD_GAMEPOINT
{
	typedef S_GFAP_F_ADD_CASH	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_ADD_GAMEPOINT()
		:	i64AddGamePoint(0i64)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	__int64	i64AddGamePoint;

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( i64AddGamePoint )
		{
			if ( true == StrIsCorrect( wszAccount ) )
			{
				kData.kOrderID = kRequestID;
				kData.wstrID = wszAccount;
				kData.i64AddCashValue = i64AddGamePoint;
				UPR( kData.wstrID );
				return true;
			}
			iOutErr = GFAP_RET_ID_STRING_ERROR;
		}
		else
		{
			iOutErr = GFAP_RET_VALUEERROR;
		}
		return false;
	}

}S_PT_GF_AP_REQ_ADD_GAMEPOINT;

typedef struct tagPT_AP_GF_ANS_ADD_GAMEPOINT
{
	typedef S_GFAP_F_ADD_CASH_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_ADD_GAMEPOINT REQ_TYPE;

	tagPT_AP_GF_ANS_ADD_GAMEPOINT()
		:	i64RetGamePoint(0i64)
		,	i64OldGamePoint(0i64)
		,	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_ADD_GAMEPOINT( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	i64RetGamePoint(rhs.i64NowCash)
		,	i64OldGamePoint(rhs.i64OldCash)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_ADD_GAMEPOINT( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
		,	i64RetGamePoint(0i64)
		,	i64OldGamePoint(0i64)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	__int64 i64RetGamePoint;
	__int64	i64OldGamePoint;
	int		iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_ADD_GAMEPOINT;

typedef struct tagPT_GF_AP_REQ_MODIFY_PASSWORD
{
	typedef S_GFAP_F_MODIFY_PASSWORD	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_MODIFY_PASSWORD()
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
		::memset( wszNowPassword, 0, sizeof(wszNowPassword) );
		::memset( wszNewPassword, 0, sizeof(wszNewPassword) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	wchar_t	wszNowPassword[MAX_STRING_LEN_BILL];
	wchar_t	wszNewPassword[MAX_STRING_LEN_BILL];

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
			if (	true == StrIsCorrect( wszNowPassword )
				&&	true == StrIsCorrect( wszNewPassword )
				)
			{
				kData.kOrderID = kRequestID;
				kData.wstrID = wszAccount;
				kData.wstrNowPW = wszNowPassword;
				kData.wstrNewPW = wszNewPassword;
				UPR( kData.wstrID );
				return true;
			}
			iOutErr = GFAP_RET_PW_STRING_ERROR;
		}
		else
		{
			iOutErr = GFAP_RET_ID_STRING_ERROR;
		}
		return false;
	}
}S_PT_GF_AP_REQ_MODIFY_PASSWORD;

typedef struct tagPT_AP_GF_ANS_MODIFY_PASSWORD
{
	typedef S_GFAP_F_MODIFY_PASSWORD_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_MODIFY_PASSWORD	REQ_TYPE;

	tagPT_AP_GF_ANS_MODIFY_PASSWORD()
		:	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
		::memset( wszNewPassword, 0, sizeof(wszNewPassword) );
	}

	explicit tagPT_AP_GF_ANS_MODIFY_PASSWORD( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
		::swprintf_s( wszNewPassword, MAX_STRING_LEN_BILL, rhs.wstrNowPW.c_str() );
	}

	explicit tagPT_AP_GF_ANS_MODIFY_PASSWORD( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
	{
		::memset( wszNewPassword, 0, sizeof(wszNewPassword) );
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	wchar_t	wszNewPassword[MAX_STRING_LEN_BILL];
	int		iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_MODIFY_PASSWORD;

typedef struct tagPT_GF_AP_REQ_USE_COUPON
{
	typedef S_GFAP_F_USE_COUPON	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_USE_COUPON()
		:	iEventKey(0)
		,	iRewardKey(0)
		,	nSiteNo(0)
		,	nRealmNo(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	BM::GUID		kRequestID;
	wchar_t			wszAccount[MAX_STRING_LEN_BILL];
	std::wstring	wstrCouponID;
	int				iEventKey;
	int				iRewardKey;
	short			nSiteNo;
	short			nRealmNo;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( kRequestID );
		kPacket.Push( wszAccount, sizeof(wszAccount) );
		kPacket.Push( wstrCouponID );
		kPacket.Push( iEventKey );
		kPacket.Push( iRewardKey );
		kPacket.Push( nSiteNo );
		kPacket.Push( nRealmNo );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() >= min_size() )
		{
			kPacket.Pop( kRequestID );
			if ( true == kPacket.PopMemory(wszAccount, sizeof(wszAccount)) )
				if ( true == kPacket.Pop( wstrCouponID ) )
					if ( true == kPacket.Pop( iEventKey ) )
						if ( true == kPacket.Pop( iRewardKey ) )
							if ( true == kPacket.Pop( nSiteNo ) )
								if ( true == kPacket.Pop( nRealmNo ) )
									return true;
		}
		return false;
	}

	size_t min_size()const
	{
		return	sizeof(kRequestID);
			+	sizeof(wszAccount)
			+	sizeof(size_t)
			+	sizeof(iEventKey)
			+	sizeof(iRewardKey);
			+	sizeof(nSiteNo);
			+	sizeof(nRealmNo);
	}

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
			kData.kOrderID = kRequestID;
			kData.wstrID = wszAccount;
			kData.wstrCouponID = wstrCouponID;
			kData.iEventKey = iEventKey;
			kData.iRewardKey = iRewardKey;
			kData.nRealmNo = nRealmNo;
			UPR( kData.wstrID );
			return true;
		}
		iOutErr = GFAP_RET_ID_STRING_ERROR;
		return false;
	}

}S_PT_GF_AP_REQ_USE_COUPON;

typedef struct tagPT_AP_GF_ANS_USE_COUPON
{
	typedef S_GFAP_F_USE_COUPON_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_USE_COUPON	REQ_TYPE;

	tagPT_AP_GF_ANS_USE_COUPON()
		:	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_USE_COUPON( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	iRet(rhs.iRet)
		,	wstrCouponID(rhs.wstrCouponID)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_USE_COUPON( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
		,	wstrCouponID(rhs.wstrCouponID)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( kRequestID );
		kPacket.Push( wszAccount, sizeof(wszAccount) );
		kPacket.Push( wstrCouponID );
		kPacket.Push( iRet );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() >= min_size() )
		{
			kPacket.Pop( kRequestID );
			if ( true == kPacket.PopMemory(wszAccount, sizeof(wszAccount)) )
				if ( true == kPacket.Pop( wstrCouponID ) )
					if ( true == kPacket.Pop( iRet ) )
						return true;
		}
		return false;
	}

	size_t min_size()const
	{
		return	sizeof(kRequestID);
			+	sizeof(wszAccount)
			+	sizeof(size_t)
			+	sizeof(iRet);
	}

	BM::GUID		kRequestID;
	wchar_t			wszAccount[MAX_STRING_LEN_BILL];
	std::wstring	wstrCouponID;
	int				iRet;
}S_PT_AP_GF_ANS_USE_COUPON;

typedef struct tagPT_GF_AP_REQ_MODIFY_MOBILELOCK
{
	typedef S_GFAP_F_MODIFY_MOBILELOCK	REQ_ORDER_TYPE;

	tagPT_GF_AP_REQ_MODIFY_MOBILELOCK()
		:	byUseMobileLock(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
//		::memset( wszPassword, 0, sizeof(wszPassword) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
//	wchar_t	wszPassword[MAX_STRING_LEN_BILL];
	BYTE	byUseMobileLock;

	DEFAULT_TBL_PACKET_FUNC();

	bool CopyTo( REQ_ORDER_TYPE &kData, int &iOutErr )const
	{
		if ( true == StrIsCorrect( wszAccount ) )
		{
//			if ( true == StrIsCorrect( wszPassword ) )
//			{
				kData.kOrderID = kRequestID;
				kData.wstrID = wszAccount;
//				kData.wstrPW = wszPassword;
				kData.byUseMobileLock = byUseMobileLock;
				UPR( kData.wstrID );
				return true;
//			}
			iOutErr = GFAP_RET_PW_STRING_ERROR;
		}
		else
		{
			iOutErr = GFAP_RET_ID_STRING_ERROR;
		}
		return false;
	}
}S_PT_GF_AP_REQ_MODIFY_MOBILELOCK;

typedef struct tagPT_AP_GF_ANS_MODIFY_MOBILELOCK
{
	typedef S_GFAP_F_MODIFY_MOBILELOCK_REPLY	REPLY_ORDER_TYPE;
	typedef S_PT_GF_AP_REQ_MODIFY_MOBILELOCK	REQ_TYPE;

	tagPT_AP_GF_ANS_MODIFY_MOBILELOCK()
		:	byUseMobileLock(0)
		,	iRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	explicit tagPT_AP_GF_ANS_MODIFY_MOBILELOCK( BM::GUID const &kOrderID, REPLY_ORDER_TYPE const &rhs )
		:	kRequestID(kOrderID)
		,	byUseMobileLock(rhs.byUseMobileLock)
		,	iRet(rhs.iRet)
	{
		::swprintf_s( wszAccount, MAX_STRING_LEN_BILL, rhs.wstrID.c_str() );
	}

	explicit tagPT_AP_GF_ANS_MODIFY_MOBILELOCK( REQ_TYPE const &rhs, int const iErrorCode )
		:	kRequestID(rhs.kRequestID)
		,	iRet(iErrorCode)
		,	byUseMobileLock(rhs.byUseMobileLock)
	{
		::memcpy( wszAccount, rhs.wszAccount, sizeof(wszAccount) );
	}

	BM::GUID kRequestID;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	BYTE	byUseMobileLock;
	int		iRet;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_ANS_MODIFY_MOBILELOCK;

typedef struct tagPT_GF_AP_REQ_SERVER_LOGIN
{
	tagPT_GF_AP_REQ_SERVER_LOGIN()
		:	iServerType(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
		::memset( wszPassword, 0, sizeof(wszPassword) );
	}

	int		iServerType;
	wchar_t	wszAccount[MAX_STRING_LEN_BILL];
	wchar_t	wszPassword[MAX_STRING_LEN_BILL];

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_GF_AP_REQ_SERVER_LOGIN;

#pragma pack()

#endif // CONSENT_CONSENTSERVER_LINKAGE_GFBILLINGAPPACKETDEF_H