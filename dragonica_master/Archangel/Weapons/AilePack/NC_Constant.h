#ifndef WEAPON_AILEPACK_NC_NC_CONSTANT_H
#define WEAPON_AILEPACK_NC_NC_CONSTANT_H

#include "AilePack/constant.h"

namespace NC
{
#pragma pack(1)

	typedef enum eNC_SA_PACKET
		:	short
	{
		/* 
		GS : GameServer (Consent 서버를 의미)
		SA : NC의 ShopAgent
		*/

		// Initial Message
		IBI_PROTOCOL_VER				= 0x00, // GS가 SA에 접속할 때 전송, 같은 Connection에 대해 한번 이상 요청 할 수 없다
		IBO_PROTOCOL_VER				= 0x00, // SA 서버의 버전정보
		
		IBI_GET_ITEMS_FOR_SERVER_REQ	= 0x14, // SA와 연결이 끊어져 있는 동안 반영된 아이템에 대해서 아이템 리스트 요청
		IBO_GET_ITEMS_FOR_SERVER_ACK	= 0x14, // 아이템 목록이 많으면 여러번에 나눠서 보냄, 한번데 보내는 최대 수량50개(전체페이지 수와 현재페이지(순서보장 안한다) 수를 함께 보내줌)

		// Item Message
		IBI_GET_ITEMS_REQ			= 0x16, // 아이템 조회 요청
		IBO_GET_ITEMS_ACK			= 0x16, // 사용자의 아이템 리스트 응답

		IBI_BUY_ITEM_REQ			= 0x0c, // 게임 내 구매시 요청
		IBO_BUY_ITEM_ACK			= 0x0c, // 아이템 구매 결과 응답

		IBI_GIFT_ITEM_REQ			= 0x0d, // 게임 내 구매(선물)시 요청
		IBO_GIFT_ITEM_ACK			= 0x0d,
		
		IBO_NEW_ITEM_DELIVERED_NOTI	= 0x06, // Notify: 선물(게임내 선물 아님, GM이 SA에서 생성한 의미로 보면 됨), 아이템 배송, 웹에서의 구매가 생겼을 때 게임으로 전달

		IBI_DELETE_ITEMS_REQ		= 0x17, // 아이템 구매등이 게임 내에서 정상적으로 이루어졌으면 이 패킷을 꼭 SA로 보내 주어야 함.
		IBO_DELETE_ITEMS_ACK		= 0x17,

		// Point Message
		IBI_GET_POINT_REQ			= 0x1a,	// 포인트 조회 요청
		IBO_GET_POINT_ACK			= 0x1a,

		//IBI_ADD_POINT_REQ			= 0x19, // 포인트 증가 요청
		//IBO_ADD_POINT_ACK			= 0x19,

		//IBI_SUB_POINT_REQ			= 0x18, // 포인트 감소 요청
		//IBO_SUB_POINT_ACK			= 0x18,

		IBO_POINT_CHANGED_NOTI		= 0x1b, // Notify: 웹이나 다른곳에서 포인트 가감이 일어났을 때 전달
	} E_NC_SA_PACKET;

	typedef enum eNC_SA_Point_Type
	{
		SAPT_CASH = 4,
		SAPT_HAPPYCOIN = 14,
		SAPT_UNITCOIN = 15,
	}E_NC_POINT_TYPE;

	typedef enum eNC_SA_Result_Code
	{
		// SA Definitions
		SARC_SUCCESS					= 0,
		SARC_INVALID_ACOUNT				= 1,
		SARC_NOT_ENOUGH_POINT			= 2,
		SARC_DB_ERROR					= 3,
		SARC_SYSTEM_ERROR				= 4,
		SARC_PORTAL_AUTH_DISCONNECT		= 5,
		SARC_IDW_AUTH_DISCONNECT		= 6,
		SARC_SHOPSERVER_DISCONNECT		= 7,
		SARC_POINTSERVER_DISCONNECT		= 8,

		// 자체적으로 정의한 에러
		SARC_SA_ERROR					= 9,	// SA서버로 SendToServer() 실패하면 이걸 사용.
	} E_NC_SA_RESULT_CODE;

	// 응답할때 필요한 정보 저장
	typedef struct tagNcCashTranInfo
	{
		SERVER_IDENTITY_SITE kSI;				// 요청한 서버 정보
		BM::Stream::DEF_STREAM_TYPE wSubType;	// 서브 패킷 타입
		CASH::SCashCost kCost;					// 사용한 캐시, 사용한 마일리지, 적립된 마일리지 --> NC는 결과값으로 이게 안와..-_- 그래서 저장해 둬야 함.
		BM::Stream OrgPacket;					// 요청때 보낸 원본 패킷


		tagNcCashTranInfo()
		{

		}

		explicit tagNcCashTranInfo(SERVER_IDENTITY_SITE const& rkSI, BM::Stream::DEF_STREAM_TYPE ret_type, CASH::SCashCost const& rkCost, BM::Stream const *pkPacket = NULL )
		{
			kSI = rkSI;
			wSubType = ret_type;
			kCost = rkCost;
			if( NULL != pkPacket )
			{
				OrgPacket.Push(*pkPacket);
			}
		}
	} SNcCashTranInfo;

	typedef struct tagNcCash
	{	
		unsigned int		uiResult;	
		unsigned int		uiPointCount;
		unsigned short		usCashCategory;
		unsigned short		usCashId;
		unsigned __int64	ui64Cash;
		unsigned short		usHappyCoinId;
		unsigned __int64	ui64HappyCoin;

		tagNcCash()
		{
			uiResult = 0;
			uiPointCount = 0;
			usCashCategory = 0;
			usCashId = 0;
			ui64Cash = 0i64;
			usHappyCoinId = 0;
			ui64HappyCoin = 0i64;
		}
		
		void ReadFromPacket(BM::Stream& rkPacket)
		{	
			rkPacket.Pop(uiResult);
			if( SARC_SUCCESS == uiResult )
			{
				rkPacket.Pop(uiPointCount);
				rkPacket.Pop(usCashCategory);
				rkPacket.Pop(usCashId);
				rkPacket.Pop(ui64Cash);
				rkPacket.Pop(usCashCategory);
				rkPacket.Pop(usHappyCoinId);
				rkPacket.Pop(ui64HappyCoin);
			}
		}
	} SNcCash;

	typedef struct tagItemDelivered
	{
		
	}ItemDelivered;

#pragma pack()

};

#endif // WEAPON_AILEPACK_NC_NC_CONSTANT_H