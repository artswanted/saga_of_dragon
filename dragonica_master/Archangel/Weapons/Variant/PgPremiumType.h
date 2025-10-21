#pragma once

#include <boost/tr1/memory.hpp>

#define SET_PREMIUM_ARTICLE(_ClassName, _Type, _UseWrite)	\
public:		\
	static EPremiumServiceType Type() {return _Type;}	\
	static _ClassName const& NullData()	\
	{		\
		static _ClassName kNullData;	\
		return kNullData;	\
	}		\
	virtual bool IsUseWrite()const {return _UseWrite;}		\
	virtual SPremiumServiceType * Copy()const	\
	{		\
		_ClassName * pkData = new _ClassName;	\
		if(pkData)		\
		{	\
			*pkData = *this;	\
		}	\
		return pkData;	\
	}


typedef enum ePremiumServiceType
{
	PST_NONE							= 0,
	PST_EXPERIENCE_RATE					= 1,	//경험치 획득량 증가
	PST_ENCHANT_SUCCESS_RATE			= 2,	//인챈트 성공률 증가
	PST_SOULCRAFT_SUCCESS_RATE			= 3,	//소울크래프트 성공률 증가
	PST_TRANSTOWER_DISCOUNT				= 4,	//워프타워 이용료 할인
	PST_STORE_SELL_PREMIUM				= 5,	//NPC에게 아이템 팔 때 웃돈 획득
	PST_SHARE_RENTAL_SAFE_EXPAND		= 6,	//계정 금고 확장
	//헌터랑 퀘스트랑 구분이 되면 구별하기
	PST_QUEST_ONCE_MORE					= 7,	//헌터 퀘스트 & 길드 퀘스트 끝까지 완료 후 한번 더 진행 가능
	PST_JS1_MAX_EXHAUSTION				= 8,	//직업생산 1차 : 피로도 증가
	PST_JS1_RECOVERY_EXHAUSTION			= 9,	//직업생산 1차 : 피로도 회복량 증가
	PST_JS1_ADD_EXPERTNESS_RATE			= 10,	//직업생산 1차 : 채집 숙련도 획득량 증가
	PST_JS2_MAX_BLESS					= 11,	//직업생산 2차 : 최대 축복 게이지 증가
	PST_SOUL_REFUND_RATE				= 12,	//아이템 분해시 획득 소울 양 증가(%)
	PST_OPENMARKET_STATE				= 13,	//오픈마켓에 상점 등록시 최고급 상점으로 등록됨(거상 포인트는 따로 구매해야 함)
	//아직 지원하지 않음
	PST_APPLY_EFFECT					= 14,	//이펙트적용
	PST_GIFT_ITEM						= 15,	//아이템선물
	//
	QUEST_REMOTE_ACCEPT					= 16,	//퀘스트 원격 수락
	VIP_CHAT_DISPLAY					= 18,	//채팅창에 붙는 VIP 표시
}EPremiumServiceType;

typedef enum ePremiumServiceAbil
{
	PSA_NONE							= 0,
	PSA_ADD_RATE						= 1,
	PSA_MAX_PLUS						= 2,
	PSA_ADD_RECOVERY					= 3,
	PSA_SHARE_RENTAL_SAFE_EXPAND		= 4,
	PSA_ITEMNO							= 5,
	PSA_EFFECTNUM1						= 10,
	PSA_EFFECTNUM10						= 19,
}EPremiumServiceAbil;


class PgPlayer;

struct SPremiumServiceType
{
	SPremiumServiceType():iTypeNo(0),wLevel(0) {}
	virtual ~SPremiumServiceType() {}

	virtual void Parser(int const iType, int const iValue) {}
	virtual bool IsUseWrite()const {return false;}
	virtual SPremiumServiceType * Copy()const {return NULL;}
	virtual int FromDB(BM::Stream & rkPacket, size_t const iSize) {return 0;}
	virtual int ToDB(BM::Stream & rkPacket)const {return 0;}
	virtual int Setup(PgPlayer * pkPlayer) {return 0;}

	int iTypeNo;
	WORD wLevel;
};


typedef std::tr1::shared_ptr<SPremiumServiceType const> C_SP_SPremiumServiceType;
typedef std::tr1::shared_ptr<SPremiumServiceType> SP_SPremiumServiceType;
typedef std::map<EPremiumServiceType, C_SP_SPremiumServiceType> CONT_DEF_PREMIUM_TYPE;
typedef std::map<EPremiumServiceType, SP_SPremiumServiceType> CONT_PREMIUM_TYPE;
