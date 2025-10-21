#include "StdAfx.h"
#include "PgDefPremiumMgr.h"
#include "PgEventView.h"

SP_SPremiumServiceType GetPremiumServiceType(EPremiumServiceType const eType)
{
	switch(eType)
	{
	case PST_EXPERIENCE_RATE:					return SP_SPremiumServiceType(new S_PST_AddExperience);
	case PST_ENCHANT_SUCCESS_RATE:				return SP_SPremiumServiceType(new S_PST_AddEnchant);
	case PST_SOULCRAFT_SUCCESS_RATE:			return SP_SPremiumServiceType(new S_PST_SoulCraft);
	case PST_TRANSTOWER_DISCOUNT:				return SP_SPremiumServiceType(new S_PST_TranstowerDiscount);
	case PST_STORE_SELL_PREMIUM:				return SP_SPremiumServiceType(new S_PST_StoreSellPremium);
	case PST_SHARE_RENTAL_SAFE_EXPAND:			return SP_SPremiumServiceType(new S_PST_ShareRentalSafeExpand);
	case PST_QUEST_ONCE_MORE:					return SP_SPremiumServiceType(new S_PST_QuestOnceMore);
	case PST_JS1_MAX_EXHAUSTION:				return SP_SPremiumServiceType(new S_PST_JS1_MaxExhaustion);
	case PST_JS1_RECOVERY_EXHAUSTION:			return SP_SPremiumServiceType(new S_PST_JS1_RecoveryExhaustion);
	case PST_JS1_ADD_EXPERTNESS_RATE:			return SP_SPremiumServiceType(new S_PST_JS1_AddExpertness);
	case PST_JS2_MAX_BLESS:						return SP_SPremiumServiceType(new S_PST_JS2_MaxBless);
	case PST_SOUL_REFUND_RATE:					return SP_SPremiumServiceType(new S_PST_SoulRefund);
	case PST_OPENMARKET_STATE:					return SP_SPremiumServiceType(new S_PST_OpenmarketState);
	case PST_APPLY_EFFECT:						return SP_SPremiumServiceType(new S_PST_ApplyEffect);
	case PST_GIFT_ITEM:							return SP_SPremiumServiceType(new S_PST_GiftItem);
	case QUEST_REMOTE_ACCEPT:					return SP_SPremiumServiceType(new S_PST_QuestRemoteAccept);
	case VIP_CHAT_DISPLAY:						return SP_SPremiumServiceType(new S_PST_VIP_ChatDisplay);
	}
	static SP_SPremiumServiceType NullData;
	return NullData;
}

PgDefPremiumMgr::PgDefPremiumMgr(void)
{
}

PgDefPremiumMgr::~PgDefPremiumMgr(void)
{
}

bool PgDefPremiumMgr::Build(CONT_DEF_PREMIUM_SERVICE const& kContPremium, CONT_DEF_PREMIUM_ARTICLE const& kContServiceType)
{
	CONT_DEF_PREMIUM_SERVICE::const_iterator premium_it = kContPremium.begin();
	while(premium_it!=kContPremium.end())
	{
		static const SPremiumData kNullData;
		auto kPair = m_kContPremium.insert(std::make_pair((*premium_it).second.iServiceNo, kNullData));
		if(kPair.second)
		{
			kPair.first->second.iServiceNo = (*premium_it).second.iServiceNo;
			kPair.first->second.wUseDate = (*premium_it).second.wUseDate;
			kPair.first->second.byGrade = (*premium_it).second.byGrade;
			kPair.first->second.kTitle = (*premium_it).second.kTitle;
		}

		CONT_DEF_PREMIUM_TYPE & rkPremiumData = kPair.first->second.kContType;
		for(int i=0; i<MAX_PREMIUM_ARTICLE; ++i)
		{
			CONT_DEF_PREMIUM_ARTICLE::const_iterator type_it = kContServiceType.find( (*premium_it).second.iArticle[i] );
			if(type_it != kContServiceType.end())
			{
				EPremiumServiceType const eType = static_cast<EPremiumServiceType>((*type_it).second.wType);
				SP_SPremiumServiceType spServiceType = GetPremiumServiceType(eType);
				if(spServiceType)
				{
					spServiceType->iTypeNo = (*type_it).second.wType;
					spServiceType->wLevel = (*type_it).second.wLv;
					for(int j=0; j<MAX_PREMIUM_ARTICLE_ABIL; ++j)
					{
						spServiceType->Parser( (*type_it).second.iAbil[j], (*type_it).second.iValue[j] );
					}
					
					rkPremiumData.insert(std::make_pair(eType, spServiceType));
				}
			}
		}
		
		++premium_it;
	}
	return true;
}

bool PgDefPremiumMgr::CreateCopy(int const iServiceNo, BM::DBTIMESTAMP_EX & kStartDate, BM::DBTIMESTAMP_EX & kEndDate, CONT_DEF_PREMIUM_TYPE & kContDef, CONT_PREMIUM_TYPE & kContCustom)const
{
	CONT_DEF_PREMIUM::const_iterator premium_it = m_kContPremium.find(iServiceNo);
	if(premium_it!=m_kContPremium.end())
	{
		SYSTEMTIME kLocalTime;
		g_kEventView.GetLocalTime(&kLocalTime);
		kStartDate = BM::DBTIMESTAMP_EX(kLocalTime);
		CGameTime::AddTime(kLocalTime, (*premium_it).second.wUseDate * CGameTime::OneDay );
		kEndDate = BM::DBTIMESTAMP_EX(kLocalTime);

		SP_SPremiumServiceType spServiceType;
		CONT_DEF_PREMIUM_TYPE::const_iterator c_it =  (*premium_it).second.kContType.begin();
		while(c_it != (*premium_it).second.kContType.end())
		{
			EPremiumServiceType const eType = static_cast<EPremiumServiceType>( (*c_it).second->iTypeNo );
			if((*c_it).second->IsUseWrite())
			{
				spServiceType.reset( (*c_it).second->Copy() );
				if(spServiceType)
				{
					kContCustom.insert(std::make_pair(eType, spServiceType));
				}
			}
			else
			{
				kContDef.insert(std::make_pair(eType, (*c_it).second));
			}
			++c_it;
		}
		return true;
	}
	return false;
}

SPremiumData const* PgDefPremiumMgr::GetDef(int const iServiceNo)const
{
	CONT_DEF_PREMIUM::const_iterator premium_it = m_kContPremium.find(iServiceNo);
	if(premium_it!=m_kContPremium.end())
	{
		return &(*premium_it).second;
	}
	return NULL;
}