#ifndef WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT_GFAP_H
#define WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT_GFAP_H

namespace GFAP_FUNCTION
{
#pragma pack(1)
	typedef enum eGFAPFunction
	{
		GFAP_F_NONE,

		GFAP_F_OVERLAP_ACCOUNT,		// 계정체크
		GFAP_F_CREATE_ACCOUNT,		// 계정생성
		GFAP_F_CHECK_ACCOUNTPW,		// 계정비밀번호+Password
		GFAP_F_GET_CASH,			// 캐쉬 검색
		GFAP_F_ADD_CASH,			// 캐쉬 추가
		GFAP_F_MODIFY_PASSWORD,		// 패스워드 수정
		GFAP_F_USE_COUPON,			// 쿠폰 사용
		GFAP_F_MODIFY_MOBILELOCK,	// 모바일락 사용 여부 수정

		GFAP_F_MAX,
	}E_GFAP_FUNCTION;

	typedef enum eGFAPTableType
	{
		GFAP_T_EVENT_TYPE			= 1,
		GFAP_T_EVENT_REWARD			= 2,
		GFAP_T_EVENT_TYPE_REWARD	= 3,
	}E_GFAP_TABLE_TYPE;

	typedef enum eGFAPTableControl
	{
		GFAP_T_CON_INSERT			= 1,
		GFAP_T_CON_UPDATE			= 2,
		GFAP_T_CON_DELETE			= 3,
	}E_GFAP_TABLE_CONTROL;

	typedef enum eGFAPResult
	{
		GFAP_RET_SUCCESS				= 0x00000000,
		
		GFAP_RET_STRING_ERROR			= 0x00000100,
		GFAP_RET_EVENTKEY_ERROR			= 0x00000200,
		GFAP_RET_REWARDKEY_ERROR		= 0x00000400,

		GFAP_RET_BLOCKED				= 0x00010000,
		
		GFAP_RET_SERVERERROR			= 0x01000000,
		GFAP_RET_DBERROR				= 0x02000000,
		GFAP_RET_VALUEERROR				= 0x04000000,

		GFAP_RET_ID						= 0x00000001,// ID Error(Already)
		GFAP_RET_PW						= 0x00000002,//	PW Error(Wrong Password)
		GFAP_RET_COUPON					= 0x00000004,// CouponID Error(Already)

		GFAP_RET_ID_STRING_ERROR		= GFAP_RET_ID|GFAP_RET_STRING_ERROR,
		GFAP_RET_PW_STRING_ERROR		= GFAP_RET_PW|GFAP_RET_STRING_ERROR,
		GFAP_RET_COUPON_STRING_ERROR	= GFAP_RET_COUPON|GFAP_RET_STRING_ERROR,
		GFAP_RET_COUPON_EVENTKEY_ERROR	= GFAP_RET_COUPON|GFAP_RET_EVENTKEY_ERROR,
		GFAP_RET_COUPON_REWARDKEY_ERROR	= GFAP_RET_COUPON|GFAP_RET_REWARDKEY_ERROR,

	}E_GFAP_RESULT;

	typedef enum eGFMobileAuthRet
	{
		GF_MOBILE_AUTH_TIMEOUT			= 0,// 개통시간 경과
		GF_MOBILE_AUTH_SUCCESS			= 1,// 성공
		GF_MOBILE_AUTH_OVERLAPPING		= 2,// 중복 로그인
		GF_MOBILE_AUTH_OVERLAPPING2		= 3,// 해당 계정은 기타 다른 계정이 로그인 시도중(중복로그인이자나-_-a)
		GF_MOBILE_AUTH_WRONGDATA		= 4,// 유저 데이터가 이상하다
		GF_MOBILE_AUTH_SERVERERROR		= 5,// 모바일 인증서버에 문제가 있다.
		GF_MOBILE_AUTH_SERVERTIMEOUT	= 6,// 모바일 인증서버로 부터 응답이 없다.
	}E_GF_MOBILE_AUTH_RET;

	typedef struct tagGFAP_F_BASE
	{
		tagGFAP_F_BASE( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	kReqServerSessionID(kReqSessionID)
			,	kOrderID(BM::GUID::Create())
		{}

		tagGFAP_F_BASE( tagGFAP_F_BASE const &rhs )
			:	kReqServerSessionID(rhs.kReqServerSessionID)
			,	kOrderID(rhs.kOrderID)
		{}

		BM::GUID kReqServerSessionID;
		BM::GUID kOrderID;

		DEFAULT_TBL_PACKET_FUNC();
	}S_GFAP_F_BASE;

	// GFAP_F_OVERLAP_ACCOUNT
	// GFAP_F_GET_CASH
	typedef struct tagGFAP_F_OVERLAP_ACCOUNT
		:	public S_GFAP_F_BASE
	{
		tagGFAP_F_OVERLAP_ACCOUNT( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE( kReqSessionID )
		{}

		std::wstring wstrID;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
			{
				return kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() );
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	sizeof(size_t);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}
	}S_GFAP_F_OVERLAP_ACCOUNT;

	// [Reply]
	//	GFAP_F_OVERLAP_ACCOUNT
	//	GFAP_F_CREATE_ACCOUNT
	//	GFAP_F_CHECK_ACCOUNTPW
	typedef struct tagGFAP_F_OVERLAP_ACCOUNT_REPLY
	//	:	public S_GFAP_F_BASE // Relpy는 상속 받지 않는다.
	{
		tagGFAP_F_OVERLAP_ACCOUNT_REPLY()
			:	iRet(GFAP_RET_SUCCESS)
		{}

		std::wstring	wstrID;
		int				iRet;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( wstrID );
			kPacket.Push( iRet );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
				{
					return kPacket.Pop( iRet );
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	sizeof(size_t)
				+	sizeof(iRet);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}
		
	}S_GFAP_F_OVERLAP_ACCOUNT_REPLY;

	// GFAP_F_CREATE_ACCOUNT
	typedef struct tagGFAP_CREATE_ACCOUNT
		:	public S_GFAP_F_BASE
	{
		tagGFAP_CREATE_ACCOUNT( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE(kReqSessionID)
			,	cGender(0)
			,	byUseMobileLock(0)
		{
			::memset( &kBirthDay, 0, sizeof(kBirthDay) );
		}

		std::wstring		wstrID;
		std::wstring		wstrPW;
		char				cGender;
		BM::DBTIMESTAMP_EX	kBirthDay;
		BYTE				byUseMobileLock;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
			kPacket.Push( wstrPW );
			kPacket.Push( cGender );
			kPacket.Push( kBirthDay );
			kPacket.Push( byUseMobileLock );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
				{
					if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
					{
						if ( true == kPacket.Pop( wstrPW, SClientTryLogin::GetPWMaxLength()) )
						{
							kPacket.Pop( cGender );
							kPacket.Pop( kBirthDay );
							return kPacket.Pop( byUseMobileLock );
						}
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	sizeof(size_t) * 2
				+	sizeof(cGender)
				+	sizeof(kBirthDay)
				+	sizeof(byUseMobileLock);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength())
				+	(sizeof(wchar_t) * SClientTryLogin::GetPWMaxLength());
		}

	}S_GFAP_CREATE_ACCOUNT;

	// GFAP_F_CHECK_ACCOUNTPW
	typedef struct tagGFAP_F_CHECK_ACCOUNTPW
		:	public S_GFAP_F_BASE
	{
		tagGFAP_F_CHECK_ACCOUNTPW( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE(kReqSessionID)
		{}

		std::wstring wstrID;
		std::wstring wstrPW;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
			kPacket.Push( wstrPW );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
				{
					if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
					{
						return kPacket.Pop( wstrPW, SClientTryLogin::GetPWMaxLength() );
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	sizeof(size_t) * 2;
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength())
				+	(sizeof(wchar_t) * SClientTryLogin::GetPWMaxLength());
		}
	}S_GFAP_F_CHECK_ACCOUNTPW;

	// [Reply]
	//	GFAP_F_GET_CASH
	typedef struct tagGFAP_F_GET_CASH_REPLY
		//	:	public S_GFAP_F_BASE // Relpy는 상속 받지 않는다.
	{
		tagGFAP_F_GET_CASH_REPLY()
			:	i64NowCash(0i64)
			,	iRet(GFAP_RET_SUCCESS)
		{}

		std::wstring	wstrID;
		__int64			i64NowCash;
		int				iRet;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( wstrID );
			kPacket.Push( i64NowCash );
			kPacket.Push( iRet );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
				{
					kPacket.Pop( i64NowCash );
					return kPacket.Pop( iRet );
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	sizeof(size_t)
				+	sizeof(i64NowCash)
				+	sizeof(iRet);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}

	}S_GFAP_F_GET_CASH_REPLY;

	typedef struct tagGFAP_F_ADD_CASH
		:	public S_GFAP_F_BASE
	{
		tagGFAP_F_ADD_CASH( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE(kReqSessionID)
			,	i64AddCashValue(0i64)
		{}

		std::wstring	wstrID;
		__int64			i64AddCashValue;//only add value

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
			kPacket.Push( i64AddCashValue );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( min_size() <= kPacket.RemainSize() )
			{
				if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
				{
					if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
					{
						return kPacket.Pop( i64AddCashValue );
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	sizeof(size_t)
				+	sizeof(i64AddCashValue);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}


	}S_GFAP_F_ADD_CASH;

	// [Reply]
	//	GFAP_F_ADD_CASH
	typedef struct tagGFAP_F_ADD_CASH_REPLY
		//	:	public S_GFAP_F_BASE // Relpy는 상속 받지 않는다.
	{
		tagGFAP_F_ADD_CASH_REPLY()
			:	i64OldCash(0i64)
			,	i64NowCash(0i64)
			,	iRet(GFAP_RET_SUCCESS)
		{}

		std::wstring	wstrID;
		__int64			i64OldCash;
		__int64			i64NowCash;
		int				iRet;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( wstrID );
			kPacket.Push( i64OldCash );
			kPacket.Push( i64NowCash );
			kPacket.Push( iRet );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
				{
					kPacket.Pop( i64OldCash );
					kPacket.Pop( i64NowCash );
					return kPacket.Pop( iRet );
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	sizeof(size_t)
				+	sizeof(i64OldCash)
				+	sizeof(i64NowCash)
				+	sizeof(iRet);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}

	}S_GFAP_F_ADD_CASH_REPLY;

	// GFAP_F_MODIFY_PASSWORD
	typedef struct tagGFAP_F_MODIFY_PASSWORD
		:	public S_GFAP_F_BASE
	{
		tagGFAP_F_MODIFY_PASSWORD( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE(kReqSessionID)
		{}

		std::wstring wstrID;
		std::wstring wstrNowPW;
		std::wstring wstrNewPW;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
			kPacket.Push( wstrNowPW );
			kPacket.Push( wstrNewPW );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
				{
					if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
					{
						if ( true == kPacket.Pop( wstrNowPW, SClientTryLogin::GetPWMaxLength() ) )
						{
							return kPacket.Pop( wstrNewPW, SClientTryLogin::GetPWMaxLength() );
						}
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	(sizeof(size_t) * 3);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength())
				+	(sizeof(wchar_t) * SClientTryLogin::GetPWMaxLength() * 2);
		}
	}S_GFAP_F_MODIFY_PASSWORD;

	// [Reply]
	//	GFAP_F_MODIFY_PASSWORD
	typedef struct tagGFAP_F_MODIFY_PASSWORD_REPLY
		//	:	public S_GFAP_F_BASE // Relpy는 상속 받지 않는다.
	{
		tagGFAP_F_MODIFY_PASSWORD_REPLY()
			:	iRet(GFAP_RET_SUCCESS)
		{}

		std::wstring	wstrID;
		std::wstring	wstrNowPW;
		int				iRet;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( wstrID );
			kPacket.Push( wstrNowPW );
			kPacket.Push( iRet );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
				{
					if ( true == kPacket.Pop( wstrNowPW, SClientTryLogin::GetPWMaxLength() ) )
					{
						return kPacket.Pop( iRet );
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	(sizeof(size_t) * 2)
				+	sizeof(iRet);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength())
				+	(sizeof(wchar_t) * SClientTryLogin::GetPWMaxLength());
		}

	}S_GFAP_F_MODIFY_PASSWORD_REPLY;

	// GFAP_F_USE_COUPON
	typedef struct tagGFAP_F_USE_COUPON
		:	public S_GFAP_F_BASE
	{
		tagGFAP_F_USE_COUPON( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE(kReqSessionID)
			,	iEventKey(0)
			,	iRewardKey(0)
			,	nRealmNo(0)
		{}

		std::wstring	wstrID;
		std::wstring	wstrCouponID;
		int				iEventKey;
		int				iRewardKey;
		short			nRealmNo;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
			kPacket.Push( wstrCouponID );
			kPacket.Push( iEventKey );
			kPacket.Push( iRewardKey );
			kPacket.Push( nRealmNo );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
				{
					if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
					{
						if ( true == kPacket.Pop( wstrCouponID ) )
						{
							kPacket.Pop( iEventKey );
							kPacket.Pop( iRewardKey );
							return kPacket.Pop( nRealmNo );
						}
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	(sizeof(size_t) * 2)
				+	(sizeof(int) * 2)
				+	sizeof(nRealmNo);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}
	}S_GFAP_F_USE_COUPON;

	// [Reply]
	//	GFAP_F_USE_COUPON
	typedef struct tagGFAP_F_USE_COUPON_REPLY
		//	:	public S_GFAP_F_BASE // Relpy는 상속 받지 않는다.
	{
		tagGFAP_F_USE_COUPON_REPLY()
			:	iRet(GFAP_RET_SUCCESS)
		{}

		std::wstring	wstrID;
		std::wstring	wstrCouponID;
		int				iRet;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( wstrID );
			kPacket.Push( wstrCouponID );
			kPacket.Push( iRet );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
				{
					if ( true == kPacket.Pop( wstrCouponID, MAX_COUPON_KEY_LEN ) )
					{
						return kPacket.Pop( iRet );
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	(sizeof(size_t) * 2)
				+	sizeof(iRet);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength())
				+	(sizeof(wchar_t) * MAX_COUPON_KEY_LEN);
		}

	}S_GFAP_F_USE_COUPON_REPLY;

	// GFAP_F_MODIFY_MOBILELOCK
	typedef struct tagGFAP_F_MODIFY_MOBILELOCK
		:	public S_GFAP_F_BASE
	{
		tagGFAP_F_MODIFY_MOBILELOCK( BM::GUID const &kReqSessionID=BM::GUID::NullData() )
			:	S_GFAP_F_BASE(kReqSessionID)
			,	byUseMobileLock(0)
		{}

		std::wstring	wstrID;
//		std::wstring	wstrPW;
		BYTE			byUseMobileLock;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			S_GFAP_F_BASE::WriteToPacket( kPacket );
			kPacket.Push( wstrID );
//			kPacket.Push( wstrPW );
			kPacket.Push( byUseMobileLock );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == S_GFAP_F_BASE::ReadFromPacket( kPacket ) )
				{
					if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
					{
//						if ( true == kPacket.Pop( wstrPW, SClientTryLogin::GetPWMaxLength() ) )
//						{
							return kPacket.Pop( byUseMobileLock );
//						}
					}
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	S_GFAP_F_BASE::min_size()
				+	sizeof(size_t) * 2
				+	sizeof(byUseMobileLock);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength())
//				+	(sizeof(wchar_t) * SClientTryLogin::GetPWMaxLength())
				;
		}
	}S_GFAP_F_MODIFY_MOBILELOCK;

	// [Reply]
	//	GFAP_F_MODIFY_PASSWORD
	typedef struct tagGFAP_F_MODIFY_MOBILELOCK_REPLY
		//	:	public S_GFAP_F_BASE // Relpy는 상속 받지 않는다.
	{
		tagGFAP_F_MODIFY_MOBILELOCK_REPLY()
			:	byUseMobileLock(0)
			,	iRet(GFAP_RET_SUCCESS)
		{}

		std::wstring	wstrID;
		BYTE			byUseMobileLock;
		int				iRet;

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( wstrID );
			kPacket.Push( byUseMobileLock );
			kPacket.Push( iRet );
		}

		bool ReadFromPacket( BM::Stream &kPacket )
		{
			if ( kPacket.RemainSize() >= min_size() )
			{
				if ( true == kPacket.Pop( wstrID, SClientTryLogin::GetIDMaxLength() ) )
				{
					kPacket.Pop(byUseMobileLock);
					return kPacket.Pop( iRet );
				}
			}
			return false;
		}

		size_t min_size()const
		{
			return	sizeof(size_t)
				+	sizeof(byUseMobileLock)
				+	sizeof(iRet);
		}

		size_t max_size()const
		{
			return	min_size()
				+	(sizeof(wchar_t) * SClientTryLogin::GetIDMaxLength());
		}

	}S_GFAP_F_MODIFY_MOBILELOCK_REPLY;

#pragma pack()
};

#endif // WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT_GFAP_H