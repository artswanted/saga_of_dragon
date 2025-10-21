#ifndef WEAPON_AILEPACK_GRAVITY_GRAVITY_CONSTANT_H
#define WEAPON_AILEPACK_GRAVITY_GRAVITY_CONSTANT_H

namespace GRAVITY
{
#pragma pack(1)

	typedef enum eDefValue
	{
		DEF_USERTYPE_SIZE	= 1,
		DEF_GAMETYPE_SIZE	= 2,
		DEF_GROUPNUM_SIZE	= 4,
		DEF_GAMENUM_SIZE	= 4,
		DEF_PACKET_SIZE		= 4,
		DEF_POINT_SIZE		= 8,
		DEF_RESULT_SIZE		= 4,
		DEF_ACCOUNTID_SIZE	= 24,
		DEF_ACCOUNTPWD_SIZE	= 32,
		DEF_SERIAL_SIZE		= 16,
		DEF_SEX_SIZE		= 1,
		DEF_BIRTH_SIZE		= 8,
		DEF_NICNAME_SIZE	= 32,
		DEF_ISPCROOM_SIZE	= 1,
		DEF_PCROOMNUM_SIZE	= 10,
		DEF_BLOCKREASON_SIZE= 2,
		DEF_BLOCKDATE_SIZE	= 14,
		DEF_ACCOUNTNUM_SIZE	= 10,
		DEF_RIGHT_SIZE		= 8,
		DEF_IP_SIZE			= 15,
		DEF_EMAIL_SIZE		= 50,
		DEF_ORDERNUM_SIZE	= 16,
		DEF_ITEMNO_SIZE		= 10,
		DEF_ITEMCOUNT_SIZE	= 4,
		DEF_TRADENO_SIZE	= 20,
		DEF_HYPER_SIZE		= 1,
	}EDefValue;

	static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 60000;
	static const int c_iGravityAuthServerNum = -1;
	static const int c_iGravityItemServerNum = -2;
	static const int c_iGravityGMServerNum = -3;
	static const char acGameType[DEF_GAMETYPE_SIZE+1] = "08";
	static const char acUserType[DEF_USERTYPE_SIZE+1] = "G";
	static const char acGroupNum[DEF_GROUPNUM_SIZE+1] = "G013";
	static const char acGameNum[DEF_GAMENUM_SIZE+1] = "0013";
	static const char acHyper[DEF_HYPER_SIZE+1] = "|";

	typedef enum tagGravityCommand
	{
		// Keep Alive
		EGC_KEEP_ALIVE_REQ		= 11,
		EGC_KEEP_ALIVE_SUCCESS	= 12,
		EGC_KEEP_ALIVE_FAILED	= 13,

		// 로그인 인증
		EGC_AUTH_REQ		= 1001,
		EGC_AUTH_ACK		= 1002,

		// 캐시 조회
		EGC_REMAIN_CASH_REQ = 1211,
		EGC_REMAIN_CASH_ACK = 1212,

		// 아이템 구매
		EGC_BUY_ITEM_REQ	= 3311,
		EGC_BUY_ITEM_ACK	= 3312,

		// 캐시 or 쿠폰 구매
		//EGC_CHARGE_REQ		= 4011,
		//EGC_CHARGE_ACK		= 4012,

		EGC_USER_BLOCK_REQ		= 8015,
		EGC_USER_BLOCK_PRE_ACK	= 102,	// 이 후에 EGC_USER_BLOCK_ACK 도착
		EGC_USER_BLOCK_ACK		= 8016,
	}EGravityCommand;

	typedef enum tagGravityAuthResult
	{
		EGAR_SUCCESS			= 0,	// 정상 유저
		EGAR_WRONG_PWD			= 43,	// 패스워드 불일치
		EGAR_NOT_CONFIRM_EMAIL	= 76,	// 이메일 인증 안함
		EGAR_PAYPAL_BLOCK		= 79,	// PAYPAL 블럭
		EGAR_WEB_GAME_BLOCK		= 8,	// Web, Game 블럭
		EGAR_RESTRICT_AGE		= 2017,	// 게임에 허용되지 않는 나이
		EGAR_NOT_AVALABLE		= 10,	// 탈퇴 유저
		EGAR_NOT_DATA			= 3,	// 데이터 없음( 없는 유저 )
		EGAR_SYSTEM_ERROR		= 9998,	// DB, 시스템 에러( 그 외 에러 )
	}EGravityAuthResult;

	typedef enum tagGravityItemResult
	{
		EGIR_SUCCESS				= 0,
		EGIR_ITEM_INFO_ERROR		= 3017,	// 상품 정보 오류
		EGIR_NOT_ENOUGH_POINT		= 3018, // 캐시 부족
		EGIR_OTHER_ERROR1			= 3019,	// other error
		EGIR_OTHER_ERROR2			= 3020,	// other error
		EGIR_OTHER_ERROR3			= 3089,	// other error
		EGIR_IS_NOT_ITEM1			= 3071, // 아이템 상품이 아님
		EGIR_IS_NOT_ITEM2			= 3088,	// 아이템 상품이 아님
		EGIR_INVALID_COUNT			= 3075,	// 수량이 0 이상이어야 함
		EGIR_INVALID_TOTAL_POINT	= 3076,	// Total 금액이 다름
		EGIR_INVALID_ACCOUNT		= 3090,	// 유저 정보 오류
		EGIR_INVALID_TRADE			= 3086,	// 거래 정보 오류
		EGIR_IMPOSSIBLE_CANCLE_ITEM	= 3087,	// 취소 불가능 상품(쿠폰)
	}EGravityItemResult;

	typedef struct tagGravityAuth
	{
		char acUserId[DEF_ACCOUNTID_SIZE+1];
		char acUserPwd[DEF_ACCOUNTPWD_SIZE+1];
		char acSerial[DEF_SERIAL_SIZE+1];
		char acGameType[DEF_GAMETYPE_SIZE+1];
		char acUserType[DEF_USERTYPE_SIZE+1];
		char acIP[DEF_IP_SIZE+1];

		tagGravityAuth()
		{
			memset( acUserId, 0, sizeof( acUserId ) );
			memset( acUserPwd, 0, sizeof( acUserPwd ) );
			memset( acSerial, 0, sizeof( acSerial ) );
			memset( acGameType, 0, sizeof( acGameType ) );
			memset( acUserType, 0, sizeof( acUserType ) );
			memset( acIP, 0, sizeof( acIP ) );
		}

		void WriteToPacket(BM::Stream& rkPacket) const
		{// 주의! sizeof( variable ) 아님!!
			rkPacket.Push( acUserId, DEF_ACCOUNTID_SIZE );
			rkPacket.Push( acUserPwd, DEF_ACCOUNTPWD_SIZE );
			rkPacket.Push( acSerial, DEF_SERIAL_SIZE );
			rkPacket.Push( acGameType, DEF_GAMETYPE_SIZE );
			rkPacket.Push( acUserType, DEF_USERTYPE_SIZE );
			rkPacket.Push( acIP, DEF_IP_SIZE );
		}
	}SGravityAuth;

	typedef struct tagGravityAuth_Result
	{
		char acResult[DEF_RESULT_SIZE+1];
		char acUserId[DEF_ACCOUNTID_SIZE+1];
		char acSerial[DEF_SERIAL_SIZE+1];
		char acSex[DEF_SEX_SIZE+1];
		char acBirth[DEF_BIRTH_SIZE+1];
		char acNickName[DEF_NICNAME_SIZE+1];
		char acRecommandId[DEF_ACCOUNTID_SIZE+1];
		char acIsPCRoom[DEF_ISPCROOM_SIZE+1];
		char acPCRoomNum[DEF_PCROOMNUM_SIZE+1];
		char acBlockReason[DEF_BLOCKREASON_SIZE+1];
		char acBlockDate[DEF_BLOCKDATE_SIZE+1];
		char acAId[DEF_ACCOUNTNUM_SIZE+1];
		char acUserType[DEF_USERTYPE_SIZE+1];
		char acEMail[DEF_EMAIL_SIZE+1];
		char acRight[DEF_RIGHT_SIZE+1];

		tagGravityAuth_Result()
		{
			ZeroMemory( acResult, sizeof( acResult ) );
			ZeroMemory( acUserId, sizeof( acUserId ) );
			ZeroMemory( acSerial, sizeof( acSerial ) );			
			ZeroMemory( acSex, sizeof( acSex ) );
			ZeroMemory( acBirth, sizeof( acBirth ) );
			ZeroMemory( acNickName, sizeof( acNickName ) );
			ZeroMemory( acRecommandId, sizeof( acRecommandId ) );
			ZeroMemory( acIsPCRoom, sizeof( acIsPCRoom ) );
			ZeroMemory( acPCRoomNum, sizeof( acPCRoomNum ) );
			ZeroMemory( acBlockReason, sizeof( acBlockReason ) );
			ZeroMemory( acBlockDate, sizeof( acBlockDate ) );
			ZeroMemory( acAId, sizeof( acAId ) );
			ZeroMemory( acUserType, sizeof( acUserType ) );
			ZeroMemory( acEMail, sizeof( acEMail ) );			
			ZeroMemory( acRight, sizeof( acRight ) );
		}

		void ReadFromPacket(BM::Stream& rkPacket)
		{
			rkPacket.PopMemory( acResult, DEF_RESULT_SIZE );
			rkPacket.PopMemory( acUserId, DEF_ACCOUNTID_SIZE );
			rkPacket.PopMemory( acSerial, DEF_SERIAL_SIZE );			
			rkPacket.PopMemory( acSex, DEF_SEX_SIZE );
			rkPacket.PopMemory( acBirth, DEF_BIRTH_SIZE );
			rkPacket.PopMemory( acNickName, DEF_NICNAME_SIZE );
			rkPacket.PopMemory( acRecommandId, DEF_ACCOUNTID_SIZE);
			rkPacket.PopMemory( acIsPCRoom, DEF_ISPCROOM_SIZE );
			rkPacket.PopMemory( acPCRoomNum, DEF_PCROOMNUM_SIZE );
			rkPacket.PopMemory( acBlockReason, DEF_BLOCKREASON_SIZE );
			rkPacket.PopMemory( acBlockDate, DEF_BLOCKDATE_SIZE );
			rkPacket.PopMemory( acAId, DEF_ACCOUNTNUM_SIZE );
			rkPacket.PopMemory( acUserType, DEF_USERTYPE_SIZE );
			rkPacket.PopMemory( acEMail, DEF_EMAIL_SIZE );
			rkPacket.PopMemory( acRight, DEF_RIGHT_SIZE );
		}
	}SGravityAuth_Result;

	typedef struct tagGravityReqCash_Result
	{
		char acStatus[DEF_RESULT_SIZE+1];
		char acAId[DEF_ACCOUNTNUM_SIZE+1];
		char acOrderNum[DEF_ORDERNUM_SIZE+1];
		char acCashBalance[DEF_POINT_SIZE+1];
		char acCouponBalace[DEF_POINT_SIZE+1];

		tagGravityReqCash_Result()
		{
			ZeroMemory( acStatus, sizeof( acStatus ) );
			ZeroMemory( acAId, sizeof( acAId ) );
			ZeroMemory( acOrderNum, sizeof( acOrderNum ) );
			ZeroMemory( acCashBalance, sizeof( acCashBalance ) );
			ZeroMemory( acCouponBalace, sizeof( acCouponBalace ) );
		}

		void ReadFromPacket(BM::Stream& rkPacket)
		{
			rkPacket.PopMemory( acStatus, DEF_RESULT_SIZE);
			rkPacket.PopMemory( acAId, DEF_ACCOUNTNUM_SIZE );
			rkPacket.PopMemory( acOrderNum, DEF_ORDERNUM_SIZE );
			rkPacket.PopMemory( acCashBalance, DEF_POINT_SIZE );
			rkPacket.PopMemory( acCouponBalace, DEF_POINT_SIZE );
		}
	}SGravityReqCash_Result;

	typedef struct tagGravityBuyItem
	{
		char acUserId[DEF_ACCOUNTNUM_SIZE+1];
		char acOrderNum[DEF_ORDERNUM_SIZE+1];
		char acTotalCashAmount[DEF_POINT_SIZE+1];
		char acTotalCouponAmount[DEF_POINT_SIZE+1];
		char acRecvGiftUserId[DEF_ACCOUNTNUM_SIZE+1]; // 선물일 경우 받는 유저의 AId
		char acIP[DEF_IP_SIZE+1];
		char acTotalItemCount[DEF_ITEMCOUNT_SIZE+1];
		
		char acItemNo[DEF_ITEMNO_SIZE+1];
		char acItemCount[DEF_ITEMCOUNT_SIZE+1];
		char acCash[DEF_POINT_SIZE+1];

		tagGravityBuyItem()
		{
			ZeroMemory( acUserId, sizeof( acUserId ) );
			ZeroMemory( acOrderNum, sizeof( acOrderNum ) );
			ZeroMemory( acTotalCashAmount, sizeof( acTotalCashAmount ) );
			ZeroMemory( acTotalCouponAmount, sizeof( acTotalCouponAmount ) );
			ZeroMemory( acRecvGiftUserId, sizeof( acRecvGiftUserId ) );
			ZeroMemory( acIP, sizeof( acIP ) );
			ZeroMemory( acTotalItemCount, sizeof( acTotalItemCount ) );
			ZeroMemory( acItemNo, sizeof( acItemNo ) );
			ZeroMemory( acItemCount, sizeof( acItemCount ) );
			ZeroMemory( acCash, sizeof( acCash ) );

			Init();
		}

		void Init()
		{
			strncpy( acTotalCashAmount, MB( BM::vstring(0) ), DEF_POINT_SIZE );
			strncpy( acTotalCouponAmount, MB( BM::vstring(0) ), DEF_POINT_SIZE );
			strncpy( acTotalItemCount, MB( BM::vstring(1) ), DEF_ITEMCOUNT_SIZE );
			strncpy( acItemCount, MB( BM::vstring(1) ), DEF_ITEMCOUNT_SIZE );
		}

		void WriteToPacket(BM::Stream& rkPacket) const
		{
			rkPacket.Push( acGroupNum, DEF_GROUPNUM_SIZE );
			rkPacket.Push( acGameNum, DEF_GAMENUM_SIZE );

			rkPacket.Push( acUserId, DEF_ACCOUNTNUM_SIZE );
			rkPacket.Push( acOrderNum, DEF_ORDERNUM_SIZE );
			rkPacket.Push( acTotalCashAmount, DEF_POINT_SIZE );
			rkPacket.Push( acTotalCouponAmount, DEF_POINT_SIZE );
			rkPacket.Push( acRecvGiftUserId, DEF_ACCOUNTNUM_SIZE );
			rkPacket.Push( acIP, DEF_IP_SIZE );
			rkPacket.Push( acTotalItemCount, DEF_ITEMCOUNT_SIZE );

			const int iTotalItemCount = atoi( acTotalItemCount );
			for( int i = 0; i < iTotalItemCount; ++i )
			{
				rkPacket.Push( acItemNo, DEF_ITEMNO_SIZE );
				rkPacket.Push( acItemCount, DEF_ITEMCOUNT_SIZE );
				rkPacket.Push( acCash, DEF_POINT_SIZE );
			}
		}

		void ReadFromPacket(BM::Stream& rkPacket)
		{
			rkPacket.PopMemory( acUserId, sizeof( acUserId ) );
			rkPacket.PopMemory( acOrderNum, sizeof( acOrderNum ) );
			rkPacket.PopMemory( acTotalCashAmount, sizeof( acTotalCashAmount ) );
			rkPacket.PopMemory( acTotalCouponAmount, sizeof( acTotalCouponAmount ) );
			rkPacket.PopMemory( acRecvGiftUserId, sizeof( acRecvGiftUserId ) );
			rkPacket.PopMemory( acIP, sizeof( acIP ) );
			rkPacket.PopMemory( acTotalItemCount, sizeof( acTotalItemCount ) );
			rkPacket.PopMemory( acItemNo, sizeof( acItemNo ) );
			rkPacket.PopMemory( acItemCount, sizeof( acItemCount ) );
			rkPacket.PopMemory( acCash, sizeof( acCash ) );
		}
	}SGravityBuyItem;

	typedef struct tagGravityBuyItem_Result
	{
		char acStatus[DEF_RESULT_SIZE+1];
		char acAId[DEF_ACCOUNTNUM_SIZE+1];
		char acOrderNum[DEF_ORDERNUM_SIZE+1];
		char acRemainCash[DEF_POINT_SIZE+1];
		char acRemainCoupon[DEF_POINT_SIZE+1];
		char acTotalBuyItemCount[DEF_ITEMCOUNT_SIZE+1];
		char acItemNo[DEF_ITEMNO_SIZE+1];
		char acBuyItemCount[DEF_ITEMCOUNT_SIZE+1];
		char acTradeNo[DEF_TRADENO_SIZE+1];

		tagGravityBuyItem_Result()
		{
			ZeroMemory( acStatus, sizeof( acStatus ) );
			ZeroMemory( acAId, sizeof( acAId ) );
			ZeroMemory( acOrderNum, sizeof( acOrderNum ) );
			ZeroMemory( acRemainCash, sizeof( acRemainCash ) );
			ZeroMemory( acRemainCoupon, sizeof( acRemainCoupon ) );
			ZeroMemory( acTotalBuyItemCount, sizeof( acTotalBuyItemCount ) );
			ZeroMemory( acItemNo, sizeof( acItemNo ) );
			ZeroMemory( acBuyItemCount, sizeof( acBuyItemCount ) );
			ZeroMemory( acTradeNo, sizeof( acTradeNo ) );
		}

		void ReadFromPacket(BM::Stream& rkPacket)
		{
			rkPacket.PopMemory( acStatus, DEF_RESULT_SIZE );
			rkPacket.PopMemory( acAId, DEF_ACCOUNTNUM_SIZE );
			rkPacket.PopMemory( acOrderNum, DEF_ORDERNUM_SIZE );
			rkPacket.PopMemory( acRemainCash, DEF_POINT_SIZE );
			rkPacket.PopMemory( acRemainCoupon,DEF_POINT_SIZE );
			rkPacket.PopMemory( acTotalBuyItemCount, DEF_ITEMCOUNT_SIZE );
			rkPacket.PopMemory( acItemNo, DEF_ITEMNO_SIZE );
			rkPacket.PopMemory( acBuyItemCount, DEF_ITEMCOUNT_SIZE );
			rkPacket.PopMemory( acTradeNo, DEF_TRADENO_SIZE );
		}
	}SGravityBuyItem_Result;
	
	typedef struct tagGravityCashTranInfo
	{
		SERVER_IDENTITY_SITE kSI;				// 요청한 서버 정보
		unsigned long TranId;					// Transaction ID
		BM::Stream::DEF_STREAM_TYPE wSubType;	// 서브 패킷 타입		
		BM::Stream OrgPacket;					// 요청때 보낸 원본 패킷

		tagGravityCashTranInfo()
		{	
			wSubType = 0;
			TranId = 0;
		}

		explicit tagGravityCashTranInfo( SERVER_IDENTITY_SITE const& rkSI, const unsigned long OrderNum, BM::Stream::DEF_STREAM_TYPE ret_type, BM::Stream const *pkPacket = NULL )			
		{
			kSI = rkSI;
			TranId = OrderNum;
			wSubType = ret_type;
			if( NULL != pkPacket )
			{
				OrgPacket.Push(*pkPacket);
			}
		}
	} SGravityCashTranInfo;
}

#endif // WEAPON_AILEPACK_GRAVITY_GRAVITY_CONSTANT_H