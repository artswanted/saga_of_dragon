#pragma once

#include "PgPremiumType.h"

class PgPlayer;

#pragma pack (1)

//////
//경험치 획득량 증가율
struct S_PST_AddExperience : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_AddExperience, PST_EXPERIENCE_RATE, false);
	S_PST_AddExperience():iRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iRate = iValue;
			}break;
		}
	}

public:
	int iRate;
};

//인챈트 성공률 증가율
struct S_PST_AddEnchant : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_AddEnchant, PST_ENCHANT_SUCCESS_RATE, false);
	S_PST_AddEnchant():iRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iRate = iValue;
			}break;
		}
	}

public:
	int iRate;
};

//소울크래프트 성공률 증가율
struct S_PST_SoulCraft : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_SoulCraft, PST_SOULCRAFT_SUCCESS_RATE, false);
	S_PST_SoulCraft():iRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iRate = iValue;
			}break;
		}
	}

public:
	int iRate;
};

//워프타워 이용료 할인율
struct S_PST_TranstowerDiscount : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_TranstowerDiscount, PST_TRANSTOWER_DISCOUNT, false);
	S_PST_TranstowerDiscount():iDiscount(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iDiscount = iValue;
			}break;
		}
	}

public:	
	int iDiscount;
};

//NPC에게 아이템 팔 때 웃된 획득율
struct S_PST_StoreSellPremium : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_StoreSellPremium, PST_STORE_SELL_PREMIUM, false);
	S_PST_StoreSellPremium():iPremiumRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iPremiumRate = iValue;
			}break;
		}
	}

public:
	int iPremiumRate;
};

//계정 금고 확장
struct S_PST_ShareRentalSafeExpand : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_ShareRentalSafeExpand, PST_SHARE_RENTAL_SAFE_EXPAND, true);
	S_PST_ShareRentalSafeExpand():byExpand(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_SHARE_RENTAL_SAFE_EXPAND:
			{
				byExpand = static_cast<BYTE>(iValue);
			}break;
		}
	}

	virtual int ToDB(BM::Stream & rkPacket)const;
	virtual int FromDB(BM::Stream & rkPacket, size_t const iSize);
	virtual int Setup(PgPlayer * pkPlayer);
	bool IsRental(int const iInvType)const;

public:
	BYTE byExpand;

private:
	SET_INT m_kContRental;	//금고가 확장된 적이 있다면 우선순위 부여
};

//헌터 퀘스트 & 길드 퀘스트 끝까지 완료 후 한번 더 진행 가능
struct S_PST_QuestOnceMore : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_QuestOnceMore, PST_QUEST_ONCE_MORE, true);
	S_PST_QuestOnceMore() {}

	virtual int ToDB(BM::Stream & rkPacket)const;
	virtual int FromDB(BM::Stream & rkPacket, size_t const iSize);

public:
	bool IsCheck(int const iQuestType)const;
	void AddQuestType(int const iQuestType);
	void AddQuestType(int const iQuestType, BM::PgPackedTime const& rkNextTime);
	bool GetNextTime(int const iQuestType, BM::PgPackedTime & rkNextTime)const;

private:
	typedef std::map<int, BM::PgPackedTime> CONT_QUEST_TYPE;
	CONT_QUEST_TYPE m_kContQuest;
};

//직업생산 1차 : 피로도 증가값
struct S_PST_JS1_MaxExhaustion : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_JS1_MaxExhaustion, PST_JS1_MAX_EXHAUSTION, false);
	S_PST_JS1_MaxExhaustion():iMax(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_MAX_PLUS:
			{
				iMax = iValue;
			}break;
		}
	}

public:
	int iMax;
};

//직업생산 1차 : 피로도 회복량 증가
struct S_PST_JS1_RecoveryExhaustion : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_JS1_RecoveryExhaustion, PST_JS1_RECOVERY_EXHAUSTION, false);
	S_PST_JS1_RecoveryExhaustion():iRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iRate = iValue;
			}break;
		}
	}

public:
	int iRate;
};

//직업생산 1차 : 채집 숙련도 획득량 증가율
struct S_PST_JS1_AddExpertness : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_JS1_AddExpertness, PST_JS1_ADD_EXPERTNESS_RATE, false);
	S_PST_JS1_AddExpertness():iRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iRate = iValue;
			}break;
		}
	}

public:
	int iRate;
};

//직업생산 2차 : 최대 축복 게이지 증가값
struct S_PST_JS2_MaxBless : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_JS2_MaxBless, PST_JS2_MAX_BLESS, false);
	S_PST_JS2_MaxBless():iMax(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_MAX_PLUS:
			{
				iMax = iValue;
			}break;
		}
	}

public:
	int iMax;
};

//아이템 분해시 획득 소울 양 증가율
struct S_PST_SoulRefund : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_SoulRefund, PST_SOUL_REFUND_RATE, false);
	S_PST_SoulRefund():iRate(0) {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ADD_RATE:
			{
				iRate = iValue;
			}break;
		}
	}

public:
	int iRate;
};

//오픈마켓에 상점 등록시 최고급 상점으로 등록됨(거상 포인트는 따로 구매해야 함)
struct S_PST_OpenmarketState : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_OpenmarketState, PST_OPENMARKET_STATE, false);
	S_PST_OpenmarketState() {}
};

//이펙트적용
struct S_PST_ApplyEffect : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_ApplyEffect, PST_APPLY_EFFECT, false);
	S_PST_ApplyEffect() {}

	virtual void Parser(int const iType, int const iValue)
	{
		if(PSA_EFFECTNUM1<=iType && iType<=PSA_EFFECTNUM10 && 0<iValue)
		{
			kContEffect.push_back(iValue);
		}
	}

public:
	VEC_INT kContEffect;
};

//하루에 한번 아이템 지급
struct S_PST_GiftItem : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_GiftItem, PST_GIFT_ITEM, false);
	S_PST_GiftItem():iItemNo(0)  {}

	virtual void Parser(int const iType, int const iValue)
	{
		switch(iType)
		{
		case PSA_ITEMNO:
			{
				iItemNo = iValue;
			}break;
		}
	}

public:
	int iItemNo;
};

//퀘스트 원격 수락
struct S_PST_QuestRemoteAccept : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_QuestRemoteAccept, QUEST_REMOTE_ACCEPT, false);
	S_PST_QuestRemoteAccept() {}
};

//채팅창에 붙는 VIP 표시
struct S_PST_VIP_ChatDisplay : public SPremiumServiceType
{
	SET_PREMIUM_ARTICLE(S_PST_VIP_ChatDisplay, VIP_CHAT_DISPLAY, false);
	S_PST_VIP_ChatDisplay() {}
};

#pragma pack() 
