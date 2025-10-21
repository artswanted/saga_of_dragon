#include "stdafx.h"
#include "Variant/Global.h"
#include "OpenMarket.h"
#include "PgServerSetMgr.h"
#include "JobDispatcher.h"
#include "Transaction.h"
#include "lohengrin/VariableContainer.h"
#include <Variant/localization/def_strings.h>

enum
{
	ML_ENG = 0,
	ML_RU = 1,
	ML_MAX
};

namespace open_market
{

static CONT_DEFSTRINGS m_kLocDefStrings; // TODO: Support multi language, now only russian support

void load_def_strings()
{
	bool bUtf8 = false;
	defstrings::load("./XML/DefStrings/RU/tb_defstrings_item.xml", m_kLocDefStrings, bUtf8);
	defstrings::load("./XML/DefStrings/RU/tb_defstrings_pet.xml", m_kLocDefStrings, bUtf8);
}

static std::wstring get_item_name(int iItemNo)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if (!pkItemDef)
		return L"";

	CONT_DEFSTRINGS::const_iterator kItor = m_kLocDefStrings.find(pkItemDef->NameNo());
	if (kItor == m_kLocDefStrings.end())
		return L"";

	std::wstring tmp = kItor->second.strText;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), towupper);
	return tmp;
}

}

namespace OpenMarketSearchUtil
{
	bool SearchSub(CONT_ARTICLE_KEY_LIST::value_type const& rkSearchKey, CONT_SEARCH_MGR const& kContSearchMgr, CONT_MARKET_ARTICLE_GUID& kSearchResult)
	{
		CONT_SEARCH_MGR::const_iterator second_search_result = kContSearchMgr.find(rkSearchKey);
		if(kContSearchMgr.end() == second_search_result)
		{
			return false;
		}

		CONT_MARKET_ARTICLE_GUID::iterator my_search_result_iter = kSearchResult.begin();
		while(kSearchResult.end() != my_search_result_iter)
		{
			CONT_MARKET_ARTICLE_GUID const& rkSearchedMarketList = (*second_search_result).second;
			BM::GUID const& rkMarketID = (*my_search_result_iter).first;
			CONT_MARKET_ARTICLE_GUID::const_iterator is_exists_market_iter = rkSearchedMarketList.find(rkMarketID);
			if( rkSearchedMarketList.end() == is_exists_market_iter )
			{
				my_search_result_iter = kSearchResult.erase(my_search_result_iter);
			}
			else
			{
				CONT_ARTICLE_GUID const& rkSearchedArticleList = (*is_exists_market_iter).second;
				CONT_ARTICLE_GUID & kContItem = (*my_search_result_iter).second;
				CONT_ARTICLE_GUID::iterator item_iter = kContItem.begin();
				while(kContItem.end() != item_iter)
				{
					CONT_ARTICLE_GUID::value_type const& rkArticleID = (*item_iter);
					if( rkSearchedArticleList.end() == rkSearchedArticleList.find(rkArticleID) )
					{
						item_iter = kContItem.erase(item_iter);
					}
					else
					{
						++item_iter;
					}
				}

				++my_search_result_iter;
			}
		}
		return true;
	}

	bool SearchSub(CONT_ARTICLE_KEY_LIST const& kAndKeyList, CONT_SEARCH_MGR const& kContSearchMgr, CONT_MARKET_ARTICLE_GUID& kSearchResult)
	{
		if(true == kAndKeyList.empty())
		{
			return true;
		}


		//if(true == kSearchResult.empty())
		//{
		//	CONT_SEARCH_MGR::const_iterator firstresult = kContSearchMgr.find((*iter));
		//	if(firstresult == kContSearchMgr.end())	// 없는 필터 조건을 요청했다. 볼것없이 여기서 실패
		//	{
		//		return false;
		//	}

		//	kSearchResult = (*firstresult).second;

		//	++iter;	// 그 다음 결과들 안에서 같은것이 없으면 결과에서 제거한다.
		//}

		CONT_ARTICLE_KEY_LIST::const_iterator iter = kAndKeyList.begin();
		for(;iter != kAndKeyList.end();++iter)
		{
			if( !SearchSub((*iter), kContSearchMgr, kSearchResult) )
			{
				return false;
			}
		}

		return true;
	}

	bool Search(CONT_ARTICLE_KEY_LIST::value_type const& kKey, CONT_SEARCH_MGR const& kContSearchMgr, CONT_MARKET_ARTICLE_GUID& kResult)
	{
		CONT_SEARCH_MGR::const_iterator subiter = kContSearchMgr.find(kKey);
		if(subiter == kContSearchMgr.end())
		{
			return false;
		}

		CONT_MARKET_ARTICLE_GUID const & kArticles = (*subiter).second;
		for(CONT_MARKET_ARTICLE_GUID::const_iterator articleiter = kArticles.begin();articleiter != kArticles.end();++articleiter)
		{
			kResult[(*articleiter).first].insert((*articleiter).second.begin(),(*articleiter).second.end());
		}
		return true;
	}

	bool Search(CONT_ARTICLE_KEY_LIST const& kContKeyList, CONT_SEARCH_MGR const& kContSearchMgr, CONT_MARKET_ARTICLE_GUID& kResult)
	{
		for(CONT_ARTICLE_KEY_LIST::const_iterator iter = kContKeyList.begin(); iter != kContKeyList.end(); ++iter)
		{
			Search((*iter), kContSearchMgr, kResult);
		}
		return true;
	}
}

void PgOpenMarket::Clear()
{
	m_kMarketGrade = 0;
	m_kOpenTime = 0;
	m_kCloseTime = 0;
	m_kOnlineTime = 0;
	m_kOfflineTime = 0;
	m_kMarketHP = 0;
	m_kContArticle.clear();
	m_kContDealing.clear();
	LastBackupTime(g_kEventView.GetLocalSecTime());
	UseOfflineTime(false);
}

size_t const PgOpenMarket::GetMaxArticleNum(int const kGrade)
{
	switch(kGrade)
	{
	case MG_NORMAL:
		{
			return 5;
		}break;
	case MG_GOOD:
		{
			return 10;
		}break;
	case MG_HISTORY:
		{
			return 20;
		}break;
	}

	CAUTION_LOG(BM::LOG_LV5, __FL__ << _T(" UNHANDLED MARKET GRADE TYPE ") << MarketGrade());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int const PgOpenMarket::GetCostRate()
{
	switch(MarketGrade())
	{
	case MG_NORMAL:
		{
			return 5;
		}break;
	case MG_GOOD:
		{
			return 3;
		}break;
	case MG_HISTORY:
		{
			return 2;
		}break;
	}

	CAUTION_LOG(BM::LOG_LV5, __FL__ << _T(" UNHANDLED MARKET GRADE TYPE ") << MarketGrade());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

__int64 const PgOpenMarket::CalcArticleCost(__int64 const i64Cost,int const iCostRate)
{
	return i64Cost - static_cast<__int64>(i64Cost * static_cast<double>(iCostRate)/100.0);
}

bool PgOpenMarket::CanRemove() const
{
	return (m_kContArticle.empty() && m_kContDealing.empty());
}

void PgOpenMarket::SetState(int const iState,bool const bSave)
{
	if(m_kState == iState)
	{
		return;
	}

	if(bSave)
	{
		CEL::DB_QUERY kQueryArticle(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_ModifyUserMarketState2]");
		kQueryArticle.InsertQueryTarget(OwnerGuId());
		kQueryArticle.PushStrParam(OwnerGuId());
		kQueryArticle.PushStrParam(static_cast<BYTE>(iState));
		g_kCoreCenter.PushQuery(kQueryArticle);
	}

	m_kState = iState;

	__int64 i64CloseTime = g_kEventView.GetLocalSecTime() + MARKET_DELETE_TIME;
	CloseTime(i64CloseTime);
}

int const PgOpenMarket::State()const
{
	return m_kState;
}

bool PgOpenMarket::ArticleAdd(SUserMarketArticleInfo const & kArticleInfo)
{
	if(m_kContArticle.size() >= GetMaxArticleNum(MarketGrade()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kContArticle.insert(std::make_pair(kArticleInfo.kItemGuId,kArticleInfo));
	return true;
}

SUserMarketArticleInfo * PgOpenMarket::GetArticleInfo(BM::GUID const & kArticleGuId)
{
	CONT_ARTICLEINFO_EX::iterator iter = m_kContArticle.find(kArticleGuId);
	if(iter == m_kContArticle.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return &(*iter).second;
}

SUserMarketArticleInfo const * PgOpenMarket::GetArticleInfo(BM::GUID const & kArticleGuId) const
{
	CONT_ARTICLEINFO_EX::const_iterator iter = m_kContArticle.find(kArticleGuId);
	if(iter == m_kContArticle.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return &(*iter).second;
}

bool PgOpenMarket::ArticleRemove(BM::GUID const & kArticleGuId)
{
	CONT_ARTICLEINFO_EX::iterator iter = m_kContArticle.find(kArticleGuId);
	if(iter == m_kContArticle.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	m_kContArticle.erase(iter);
	return true;
}

void PgOpenMarket::DealingAdd(SUserMarketDealingInfo const & kDealing)
{
	m_kContDealing.insert(std::make_pair(kDealing.kDealingGuid,kDealing));
}

SUserMarketDealingInfo * PgOpenMarket::GetDealing(BM::GUID const & kDealGuId)
{
	CONT_MARKET_DEALING::iterator iter = m_kContDealing.find(kDealGuId);
	if(iter == m_kContDealing.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	return &(*iter).second;
}

bool PgOpenMarket::DealingRemove(BM::GUID const & kDealGuId)
{
	CONT_MARKET_DEALING::iterator iter = m_kContDealing.find(kDealGuId);
	if(iter == m_kContDealing.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kContDealing.erase(iter);

	return true;
}

void PgOpenMarket::WriteToPacket_MarketInfo(BM::Stream & kPacket, bool bDealing) const
{
	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

	kPacket.Push(m_kMarketName);
	kPacket.Push(m_kCharName);
	kPacket.Push(m_kOwnerGuId);
	kPacket.Push(m_kMarketGrade);
	kPacket.Push(m_kOpenTime);
	kPacket.Push(std::max((m_kCloseTime - i64CurTime),static_cast<__int64>(0)));

	if(MarketGrade()>=MG_NORMAL)
	{
		kPacket.Push(std::max((m_kOnlineTime - i64CurTime),static_cast<__int64>(0)));
		kPacket.Push(m_kOfflineTime);
	}
	else
	{
		kPacket.Push(static_cast<__int64>(0));
		kPacket.Push(static_cast<__int64>(0));
	}

	kPacket.Push(m_kMarketHP);
	kPacket.Push(m_kState);
	
	WriteToPacket_Article(kPacket);
	if( true == bDealing )
	{
		WriteToPacket_Dealing(kPacket);
	}
}

void PgOpenMarket::ReadFromPacket_MarketInfo(BM::Stream & kPacket, bool bDealing)
{
	bool const bUseOfflineTime = UseOfflineTime();
	Clear();

	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

	kPacket.Pop(m_kMarketName);
	kPacket.Pop(m_kCharName);
	kPacket.Pop(m_kOwnerGuId);
	kPacket.Pop(m_kMarketGrade);
	kPacket.Pop(m_kOpenTime);
	kPacket.Pop(m_kCloseTime);
	CloseTime(CloseTime()+i64CurTime);
	kPacket.Pop(m_kOnlineTime);
	OnlineTime(OnlineTime()+i64CurTime);
	kPacket.Pop(m_kOfflineTime);
	kPacket.Pop(m_kMarketHP);
	kPacket.Pop(m_kState);
	
	ReadFromPacket_Article(kPacket);
	if( true == bDealing )
	{
		ReadFromPacket_Dealing(kPacket);
	}
	if( true == bUseOfflineTime && 0 < m_kOfflineTime )
	{//Clear() 전의 UseOfflineTime이 true였고, 현재 OfflineTime이 있다면 이전 상태를 복원시킨다.
		UseOfflineTime(true);
	}
}

void PgOpenMarket::WriteToPacket_Dealing(BM::Stream & kPacket)const
{
	kPacket.Push(m_kContDealing.size());

	for(CONT_MARKET_DEALING::const_iterator iter = m_kContDealing.begin();iter != m_kContDealing.end();++iter)
	{
		(*iter).second.WriteToPacket(kPacket);
	}
}

__int64 PgOpenMarket::GetClossTime() const
{
	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();
	return std::max((m_kCloseTime - i64CurTime),static_cast<__int64>(0));
}

int PgOpenMarket::GetTotalSize() const
{
	return static_cast<int>(m_kContDealing.size());
}

void PgOpenMarket::GetContDealingIterFirst(CONT_MARKET_DEALING::const_iterator &iterDealing)
{
	iterDealing = m_kContDealing.begin();
}

void PgOpenMarket::WriteToPacket_DivDealing(BM::Stream & kPacket, CONT_MARKET_DEALING::const_iterator &iterDealing, int iTotalSize, int const iDivSize)const
{	
	CONT_MARKET_DEALING::const_iterator iterDealingEnd = m_kContDealing.end();

	int iSendSize = 0;
	if( iTotalSize <= iDivSize )
	{
		iSendSize = iTotalSize;
	}
	else
	{
		iSendSize = iDivSize;
	}

	if( 0 < iSendSize )
	{
		kPacket.Push(iSendSize);

		for(int i=0; i<iSendSize; ++i)
		{
			if( iterDealingEnd != iterDealing )
			{
				(*iterDealing).second.WriteToPacket(kPacket);
				++iterDealing;
			}
		}
	}

}

void PgOpenMarket::ReadFromPacket_Dealing(BM::Stream & kPacket)
{
	CONT_MARKET_DEALING::size_type kCount = 0;
	CONT_MARKET_DEALING::mapped_type kDealing;
	kPacket.Pop(kCount);

	for(CONT_MARKET_DEALING::size_type i = 0;i < kCount;i++)
	{
		kDealing.ReadFromPacket(kPacket);
		m_kContDealing.insert(std::make_pair(kDealing.kDealingGuid,kDealing));
	}
}

void PgOpenMarket::WriteToPacket_Article(BM::Stream & kPacket)const
{
	kPacket.Push(m_kContArticle.size());

	for(CONT_ARTICLEINFO_EX::const_iterator iter = m_kContArticle.begin();iter != m_kContArticle.end();++iter)
	{
		(*iter).second.WriteToPacket(kPacket);
	}
}

void PgOpenMarket::ReadFromPacket_Article(BM::Stream & kPacket)
{
	CONT_ARTICLEINFO_EX::size_type kCount = 0;
	CONT_ARTICLEINFO_EX::mapped_type kArticle;

	kPacket.Pop(kCount);

	for(CONT_ARTICLEINFO_EX::size_type i = 0;i < kCount;i++)
	{
		kArticle.ReadFromPacket(kPacket);
		m_kContArticle.insert(std::make_pair(kArticle.kItemGuId,kArticle));
	}
}

void PgOpenMarket::Tick(__int64 const & i64CurTime)
{
	if( !g_kEventView.VariableCont().bCashShopOpen )
	{// 상점 점검 중일 때는 오프라인 유지 시간 소모 하지 않음
		return ;
	}

	if(MS_EDIT == State())
	{
		return;
	}

	if(MG_NORMAL == MarketGrade() && false==UseOfflineTime())	// 일반 상점은 온라인동안 시간 제한 없이 사용가능
	{
		return;
	}

	if(true == UseOfflineTime())
	{
		__int64 const i64ElapsedTime = i64CurTime - LastBackupTime();

		if(i64ElapsedTime >= 60i64)
		{
			LastBackupTime(i64CurTime);
			__int64 i64NewOfflineTime = OfflineTime() - i64ElapsedTime;
			i64NewOfflineTime = std::max(i64NewOfflineTime,0i64);

			OfflineTime(i64NewOfflineTime);

			if(i64ElapsedTime >= 3600i64 || OfflineTime() <= 0i64)
			{
				PgOpenMarketMgr::FlushMarketInfoToDB(*this);
			}
		}

		if(OfflineTime() > 0)	// 오프라인 유지 시간이 지나면 판매 불가
		{
			return;
		}

		SetState(MS_EDIT);
	}
	else
	{
		if(OnlineTime() > i64CurTime)	// 일반 이상 상점은 온라인 이더라도 유지 시간이 지나면 판매 불가
		{
			return;
		}
		
		SetState(MS_EDIT);
	}
}

bool PgOpenMarket::LoadDB(CEL::DB_DATA_ARRAY::const_iterator & itor,int const iArticleCount,int const iDealingCount,CONT_SELECTED_CHARACTER_ITEM_LIST const & kItemList)
{
	std::wstring	kMarketName,
					kCharName;
	BM::GUID		kOwnerGuId;
	BYTE			bMarketGrade,
					bMarketState;
	__int64			i64Time;
	int				iMarketHP;
	__int64			i64OnlineTime,
					i64OfflineTime;

	CGameTime		kGameTime;
	BM::DBTIMESTAMP_EX	kGenTime,
						kCloseTime;

	__int64 iCurTime = g_kEventView.GetLocalSecTime();
	(*itor).Pop(kOwnerGuId);	++itor;
	(*itor).Pop(kMarketName);	++itor;
	(*itor).Pop(kCharName);		++itor;
	(*itor).Pop(bMarketGrade);	++itor;
	(*itor).Pop(bMarketState);	++itor;
	(*itor).Pop(kGenTime);		++itor;
	(*itor).Pop(i64OfflineTime); ++itor;
	(*itor).Pop(i64OnlineTime); ++itor;
	(*itor).Pop(kCloseTime);	++itor;
	(*itor).Pop(iMarketHP);		++itor;

	OwnerGuId(kOwnerGuId);
	MarketName(kMarketName);
	CharName(kCharName);
	MarketGrade(bMarketGrade);
	SetState(bMarketState,false);

	kGameTime.DBTimeEx2SecTime(kCloseTime,i64Time);
	CloseTime(i64Time);

	kGameTime.DBTimeEx2SecTime(kGenTime,i64Time);
	OpenTime(i64Time);

	OfflineTime(i64OfflineTime);
	OnlineTime(i64OnlineTime);
	MarketHP(iMarketHP);

	CONT_ARTICLEINFO_EX::mapped_type kArticle;
	CONT_ARTICLEINFO_EX kContArticle;

	for(int i = 0;i < iArticleCount;i++)
	{
		(*itor).Pop(kArticle.kArticleCost);		++itor;
		(*itor).Pop(kArticle.kCostType);		++itor;
		(*itor).Pop(kArticle.kItemGuId);		++itor;
		(*itor).Pop(kArticle.kItemName);		++itor;
		(*itor).Pop(kArticle.kGrade);			++itor;
		(*itor).Pop(kArticle.kLevelLimit);		++itor;
		(*itor).Pop(kArticle.kInvType);			++itor;
		(*itor).Pop(kArticle.kEquipPos);		++itor;
		(*itor).Pop(kArticle.kClassLimit);		++itor;

		CONT_SELECTED_CHARACTER_ITEM_LIST::const_iterator iter = kItemList.find(kArticle.kItemGuId);
		if(iter != kItemList.end())
		{
			kArticle.kItem = (*iter).second;
		}

		if( kArticle.kItemName.empty() )
		{
			::GetItemName(kArticle.kItem.ItemNo(), kArticle.kItemName);
		}

		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pkItemDef = kItemDefMgr.GetDef(kArticle.kItem.ItemNo());
		kArticle.kDisplayGrade = pkItemDef ? pkItemDef->GetAbil(AT_ITEM_DISPLAY_GRADE) : 0;

		kContArticle.insert(std::make_pair(kArticle.kItemGuId,kArticle));
	}

	ContArticle(kContArticle);

	CONT_MARKET_DEALING kContDealing;
	CONT_MARKET_DEALING::mapped_type kDealing;
	double dCostRate = 0.0;
	for(int i = 0;i < iDealingCount;i++)
	{
		(*itor).Pop(kDealing.kDealingGuid);		++itor;
		(*itor).Pop(kDealing.kDealerName);		++itor;
		(*itor).Pop(kDealing.dwItemNo);			++itor;
		(*itor).Pop(kDealing.wItemNum);			++itor;
		(*itor).Pop(kDealing.i64Enchant_01);	++itor;
		(*itor).Pop(kDealing.i64Enchant_02);	++itor;
		(*itor).Pop(kDealing.i64Enchant_03);	++itor;
		(*itor).Pop(kDealing.i64Enchant_04);	++itor;
		(*itor).Pop(kDealing.i64SellCost);		++itor;
		(*itor).Pop(kDealing.cCostType);		++itor;
		(*itor).Pop(kDealing.cItemState);		++itor;
		(*itor).Pop(kDealing.kItemGenTime);		++itor;
		(*itor).Pop(kDealing.iCostRate);		++itor;

		kContDealing.insert(std::make_pair(kDealing.kDealingGuid,kDealing));
	}
	
	ContDealing(kContDealing);

	return true;
}

//////////////////////////////////////////////////////////////////
//	PgOpenMarketMgr

PgOpenMarketMgr::PgOpenMarketMgr()
{
	CheckCount(0);
	m_iBiggestItemLevel = 0;
	for(unsigned short i = 0; i <= ML_MAX; i++)
		m_kLocItemNameMap.insert(std::make_pair(i, ITEM_NAME_MAP()));
}

PgOpenMarketMgr::~PgOpenMarketMgr()
{
}


bool PgOpenMarketMgr::SetMarketInfo(PgOpenMarket const & kUserMarket)
{
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kUserMarket.OwnerGuId());
	if(iter == m_kContMarket.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket;
	kUserMarket.WriteToPacket_MarketInfo(kPacket);
	(*iter).second.ReadFromPacket_MarketInfo(kPacket);

	BuildSearchKey(kUserMarket);

	return true;
}

bool PgOpenMarketMgr::RemoveMarket(BM::GUID const & kOwnerGuid)
{
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
	if(iter == m_kContMarket.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	(*iter).second.SetState(MS_EDIT,false);

	BuildSearchKey((*iter).second);

	m_kContMarket.erase(iter);

	RemoveMarketQuery(kOwnerGuid);
	return true;
}

bool PgOpenMarketMgr::InsertMarket(PgOpenMarket const & kUserMarket)
{
	auto kRet = m_kContMarket.insert(std::make_pair(kUserMarket.OwnerGuId(),kUserMarket));
	if(!kRet.second)
	{
		bool needFlush = (*kRet.first).second.UseOfflineTime();//오프라인이었다가 온라인이 되면 지금까지의 오프라인 시간을 저장
		(*kRet.first).second.UseOfflineTime(false);// 이미 상점 정보가 메모리에 있으면 오프라인 사용 플레그를 꺼준다.
		if (needFlush)
		{
			PgOpenMarketMgr::FlushMarketInfoToDB((*kRet.first).second);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BuildSearchKey(kUserMarket);

	return true;
}


HRESULT PgOpenMarketMgr::RemoveMarketQuery(BM::GUID const & kOwnerGuid)
{
	CEL::DB_QUERY kQueryRemove( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UM_MarketRemove2]");
	kQueryRemove.InsertQueryTarget(kOwnerGuid);
	kQueryRemove.PushStrParam(kOwnerGuid);
	return g_kCoreCenter.PushQuery(kQueryRemove);
}

void PgOpenMarketMgr::ClearSearchKey(BM::GUID const & kOwnerGuId)
{
	CONT_MARKET_KEY::iterator keyiter = m_kContMarketKey.find(kOwnerGuId);
	if(keyiter != m_kContMarketKey.end())
	{
		m_kContMarketRank.erase((*keyiter).second);
		m_kContMarketKey.erase(keyiter);
	}

	for(CONT_SEARCH_MGR::iterator iter = m_kContSearchMgr.begin();iter != m_kContSearchMgr.end();++iter)
	{
		(*iter).second.erase(kOwnerGuId);
	}
}

void PgOpenMarketMgr::BuildSearchKey(PgOpenMarket const & kUserMarket)
{
	BM::GUID const & kOwnerGuid = kUserMarket.OwnerGuId();

	ClearSearchKey(kOwnerGuid);

	if((MS_OPEN != kUserMarket.State()) || kUserMarket.ContArticle().empty())	// 상점 오픈 상태가 아니면 검색 조건에서 제외 시킨다.
	{
		return;
	}

	SMARKET_KEY kKey;
	kKey.iGrade = kUserMarket.MarketGrade();
	kKey.iHP = kUserMarket.MarketHP();
	kKey.kOwnerId = kUserMarket.OwnerGuId();
	kKey.kCheckCount = CheckCount();
	CheckCount(CheckCount()+1);
	m_kContMarketKey.insert(std::make_pair(kOwnerGuid,kKey));

	kKey.kCharName = kUserMarket.CharName();
	kKey.kMarketName = kUserMarket.MarketName();
	m_kContMarketRank.insert(kKey);

	std::wstring kCharName(kUserMarket.CharName());
	std::transform(kCharName.begin(), kCharName.end(), kCharName.begin(), towupper);	//대문자변환
	m_kCharNameMap.BuildNameKey(kCharName,kOwnerGuid);

	CONT_ARTICLEINFO_EX const & kContArticleInfo = kUserMarket.ContArticle();

	GET_DEF(CItemDefMgr, kItemDefMgr);

	for(CONT_ARTICLEINFO_EX::const_iterator iter = kContArticleInfo.begin();iter != kContArticleInfo.end();++iter)
	{
		CONT_ARTICLEINFO_EX::mapped_type const & kArticle = (*iter).second;
		const int iItemNo = kArticle.kItem.ItemNo();
		CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
		if(NULL == pkItemDef)
		{
			continue;
		}
		if(MARKET_ARTICLE_CASHNO == iItemNo)
		{
			m_kContSearchMgr[SARTICLE_KEY(AKT_CASH,kArticle.kItem.Count())][kOwnerGuid].insert(kArticle.kItemGuId);
		}

		if(MARKET_ARTICLE_MONEYNO == iItemNo)
		{
			m_kContSearchMgr[SARTICLE_KEY(AKT_GOLD,kArticle.kItem.Count())][kOwnerGuid].insert(kArticle.kItemGuId);
		}


		std::wstring kItemName(kArticle.kItemName);
		std::transform(kItemName.begin(), kItemName.end(), kItemName.begin(), towupper);	//대문자변환
		m_kLocItemNameMap[ML_ENG].BuildNameKey(kItemName, iItemNo);
		m_kLocItemNameMap[ML_RU].BuildNameKey(open_market::get_item_name(iItemNo), iItemNo);

		m_kContSearchMgr[SARTICLE_KEY(AKT_OWNER_GUID,kOwnerGuid)][kOwnerGuid].insert(kArticle.kItemGuId);
		m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_NO,iItemNo)][kOwnerGuid].insert(kArticle.kItemGuId);

		if(pkItemDef->CanEquip())
		{
			m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_GRADE,kArticle.kGrade)][kOwnerGuid].insert(kArticle.kItemGuId);
			m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_DISPLAY_GRADE,kArticle.kDisplayGrade)][kOwnerGuid].insert(kArticle.kItemGuId);
			m_kContSearchMgr[SARTICLE_KEY(AKT_SMALL_CATEGORY,kArticle.kEquipPos)][kOwnerGuid].insert(kArticle.kItemGuId);
			__int64 i64ClassLimit = kArticle.kClassLimit;
			CalcClassLimit(i64ClassLimit);
			if( i64ClassLimit == UCLIMIT_ALL )
			{
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_FIGHTER)][kOwnerGuid].insert(kArticle.kItemGuId);
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_MAGICIAN)][kOwnerGuid].insert(kArticle.kItemGuId);
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_ARCHER)][kOwnerGuid].insert(kArticle.kItemGuId);
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_THIEF)][kOwnerGuid].insert(kArticle.kItemGuId);
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS,UCLIMIT_MARKET_SHAMAN)][kOwnerGuid].insert(kArticle.kItemGuId);
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS,UCLIMIT_MARKET_DOUBLE_FIGHTER)][kOwnerGuid].insert(kArticle.kItemGuId);
			}
			else if( i64ClassLimit == UCLIMIT_ALL_HUMAN)
			{
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_FIGHTER)][kOwnerGuid].insert(kArticle.kItemGuId);				
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_MAGICIAN)][kOwnerGuid].insert(kArticle.kItemGuId);				
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_ARCHER)][kOwnerGuid].insert(kArticle.kItemGuId);				
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS, UCLIMIT_MARKET_THIEF)][kOwnerGuid].insert(kArticle.kItemGuId);
			}
			else if(i64ClassLimit == UCLIMIT_ALL_DRAGONIAN)
			{
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS,UCLIMIT_MARKET_SHAMAN)][kOwnerGuid].insert(kArticle.kItemGuId);				
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS,UCLIMIT_MARKET_DOUBLE_FIGHTER)][kOwnerGuid].insert(kArticle.kItemGuId);
			}
			else
			{
				m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_CALSS,i64ClassLimit)][kOwnerGuid].insert(kArticle.kItemGuId);
			}
		}
		else
		{
			if( ITEM_SOUL_NO == iItemNo ) // 예외 소울은 장비탭에 들어가서, 장착위치로 SearchKey를 빌드 해준다(소울과 다스소울무기가 같이 검색되는 불편함 개선)
			{
				m_kContSearchMgr[SARTICLE_KEY(AKT_SMALL_CATEGORY,kArticle.kEquipPos)][kOwnerGuid].insert(kArticle.kItemGuId);
			}
		}
		m_iBiggestItemLevel = std::max(m_iBiggestItemLevel, kArticle.kLevelLimit);
		m_kContSearchMgr[SARTICLE_KEY(AKT_ITEM_LEVEL,kArticle.kLevelLimit)][kOwnerGuid].insert(kArticle.kItemGuId);
		m_kContSearchMgr[SARTICLE_KEY(AKT_LARGE_CATEGORY,kArticle.kInvType)][kOwnerGuid].insert(kArticle.kItemGuId);
		m_kContSearchMgr[SARTICLE_KEY(AKT_COST_TYPE,kArticle.kCostType)][kOwnerGuid].insert(kArticle.kItemGuId);

		auto kRet = m_kContMinimumCost.insert(std::make_pair(iItemNo,kArticle.kArticleCost));
		if((*kRet.first).second > kArticle.kArticleCost)
		{
			(*kRet.first).second = kArticle.kArticleCost;
		}
	}
}

void PgOpenMarketMgr::ProcessDisableMarket(BM::GUID const & kCharGuid)
{
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kCharGuid);
	if(iter == m_kContMarket.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find OPenMarket"));
		return;
	}

	PgOpenMarket & kMarket = (*iter).second;

	if(kMarket.MarketGrade()>=MG_NORMAL && kMarket.OfflineTime()>0)
	{
		kMarket.UseOfflineTime(true);// 오프라인 시간 사용 시작 플레그
		kMarket.LastBackupTime(g_kEventView.GetLocalSecTime());// 오프라인 검사 시간도 초기화 시켜 준다.
		return;
	}
	
	kMarket.SetState(MS_EDIT);

	SetMarketInfo(kMarket);
}

void PgOpenMarketMgr::CalcClassLimit(__int64 & i64ClassLimit)
{
	if( i64ClassLimit == UCLIMIT_ALL)
	{
		i64ClassLimit = UCLIMIT_ALL;
	}
	else if(i64ClassLimit == UCLIMIT_ALL_HUMAN)
	{
		i64ClassLimit = UCLIMIT_ALL_HUMAN;
	}
	else if(i64ClassLimit == UCLIMIT_ALL_DRAGONIAN)
	{
		i64ClassLimit = UCLIMIT_ALL_DRAGONIAN;
	}
	else if(i64ClassLimit & UCLIMIT_MARKET_FIGHTER)
	{
		i64ClassLimit = UCLIMIT_MARKET_FIGHTER;
	}
	else if(i64ClassLimit & UCLIMIT_MARKET_MAGICIAN)
	{
		i64ClassLimit = UCLIMIT_MARKET_MAGICIAN;
	}
	else if(i64ClassLimit & UCLIMIT_MARKET_ARCHER)
	{
		i64ClassLimit = UCLIMIT_MARKET_ARCHER;
	}
	else if(i64ClassLimit & UCLIMIT_MARKET_THIEF)
	{
		i64ClassLimit = UCLIMIT_MARKET_THIEF;
	}
	else if(i64ClassLimit & UCLIMIT_MARKET_SHAMAN)
	{
		i64ClassLimit = UCLIMIT_MARKET_SHAMAN;
	}
	else if(i64ClassLimit & UCLIMIT_MARKET_DOUBLE_FIGHTER)
	{
		i64ClassLimit = UCLIMIT_MARKET_DOUBLE_FIGHTER;
	}
	else
	{
		i64ClassLimit = 0;
	}
}

void PgOpenMarketMgr::SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid)
{
	if (kGuidKey == BM::GUID::NullData())
	{
		return;
	}
	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kGuidKey);
	//pkActionOrder->kGndKey = SRealmGroundKey(kContUser.sChannel, kContUser.kGndKey);
	pkActionOrder->kCause = CNE_CONTENTS_SENDTOUSER;
	
	ContentsActionEvent_SendPacket kEvent(ECEvent_SendToUser);
	kEvent.MemberGuid(IsMemberGuid);
	kEvent.Guid(kGuidKey);
	kEvent.SendType(ContentsActionEvent_SendPacket::E_SendUser_ToOneUser);
	kEvent.m_kPacket.Push(rkPacket);

	SPMO kOrder(IMET_CONTENTS_EVENT, kGuidKey, kEvent);
	pkActionOrder->kContOrder.push_back(kOrder);
	g_kJobDispatcher.VPush(pkActionOrder);
}


HRESULT PgOpenMarketMgr::Locked_ProcessModifyPlayer_OpenMarket(EItemModifyParentEventType const kCause, SPMO const &kOrder, PgDoc_Player* pkDocPlayer,
		   BM::Stream const &kAddonPacket, BM::Stream & rkPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr & kContLogMgr)
{
	BM::CAutoMutex kLock(m_kMutex);

	EUserMarketResult eResult = UMR_SYSTEM_ERROR;
	switch(kOrder.Cause())
	{
	case IMET_OPEN_MARKET:
		{
			tagPlayerModifyOrderData_ModifyOpenMarket kData;

			kOrder.Read(kData);

			std::wstring kCpyName = kData.MarketName();
			bool const bFiltered = (true == g_kFilterString.Filter(kCpyName, false, FST_ALL));
			bool const bUniFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_OPEM_MARKET_NAME, kCpyName));
			if( bFiltered
			||	bUniFiltered )
			{
				 return UMR_USE_BADWORD;
			}

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOrder.OwnerGuid());
			if(iter != m_kContMarket.end())
			{
				return UMR_ALREADY_OPENED;
			}

			__int64 const i64OpenTime = g_kEventView.GetLocalSecTime();

			CONT_OPENMARKET::mapped_type kUserMarket;
			kUserMarket.MarketName(kData.MarketName());
			kUserMarket.CharName(kData.CharName());
			kUserMarket.OwnerGuId(kOrder.OwnerGuid());
			kUserMarket.MarketGrade(kData.MarketGrade());
			kUserMarket.OpenTime(i64OpenTime);
			kUserMarket.OnlineTime(i64OpenTime + kData.OnlineTime());
			kUserMarket.OfflineTime(kData.OfflineTime());
			kUserMarket.CloseTime(i64OpenTime + MARKET_DELETE_TIME);
			kUserMarket.ContArticle(kData.ContArticle());
			kUserMarket.SetState(MS_EDIT,false);

			tagDBItemStateChange kDBChange(DISCT_OPEN_MARKET, kOrder.Cause(), kOrder.OwnerGuid());
			kUserMarket.WriteToPacket_MarketInfo(kDBChange.kAddonData);
			kChangeArray.push_back(kDBChange);

			eResult = UMR_SUCCESS;
			{
				PgLog kLog(ELOrderMain_Market,ELOrderSub_Create);
				kLog.Set(0,kData.MarketName());
				kLog.Set(0,static_cast<int>(kData.MarketGrade()));
				kLog.Set(0,kData.OnlineTime());
				kLog.Set(1,kData.OfflineTime());
				kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
			}
		}break;
	case IMET_ADD_ARTICLE:
		{
			SUserMarketArticleInfo kData;

			kOrder.Read(kData);

			if(0 < kData.kCostType) // 캐시로 아이템 판매를 하려면
			{
				int iValue = 0;
				if( S_OK != g_kVariableContainer.Get(EVar_Kind_Contents, EVar_UM_UseCash, iValue) )
				{
					iValue = DEFAULT_USER_MARKET_USEABLE_CASH;
				}

				if(DEFAULT_USER_MARKET_USEABLE_CASH != iValue)
				{
					return UMR_NOT_SUPPORT;
				}
			}

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOrder.OwnerGuid());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kOpenMarket = (*iter).second;

			if(MS_OPEN == kOpenMarket.State())
			{
				return UMR_ALREADY_OPENED;
			}

			if(kOpenMarket.ContArticle().size() >= kOpenMarket.GetMaxArticleNum(kOpenMarket.MarketGrade()))
			{
				return UMR_MARKETITEM_FULL;
			}

			tagDBItemStateChange kAddArticle(DISCT_ADD_ARTICLE, kOrder.Cause(), kOrder.OwnerGuid());
			kData.WriteToPacket(kAddArticle.kAddonData);
			kChangeArray.push_back(kAddArticle);

			eResult = UMR_SUCCESS;

			PgLog kLog(ELOrderMain_Market_Article,ELOrderSub_Create);

			kLog.Set(0,kData.kItemName);
			kLog.Set(2,kData.kItemGuId.str().c_str());

			kLog.Set(0,static_cast<int>(kData.kItem.ItemNo()));
			kLog.Set(1,static_cast<int>(kData.kCostType));
			kLog.Set(2,static_cast<int>(kData.kItem.Count()));

			kLog.Set(0,kData.kArticleCost);
			kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
		}break;
	case IMET_REMOVE_ARTICLE:	// 등록된 물품 회수
		{
			SPlayerModifyOrderData_ModifyRemoveArticle kData;
			kOrder.Read(kData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOrder.OwnerGuid());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kOpenMarket = (*iter).second;

			if(MS_OPEN == kOpenMarket.State())
			{
				return UMR_ALREADY_OPENED;
			}

			SUserMarketArticleInfo * pkArticleInfo = kOpenMarket.GetArticleInfo(kData.ArticleGuid());
			if(!pkArticleInfo)
			{
				return UMR_NOT_FOUND_ARTICLE;
			}

			tagDBItemStateChange kRemoveArticle(DISCT_REMOVE_ARTICLE, kOrder.Cause(), kOrder.OwnerGuid());
			kRemoveArticle.kAddonData.Push(kData.ArticleGuid());
			kChangeArray.push_back(kRemoveArticle);

			switch(pkArticleInfo->kItem.ItemNo())
			{
			case MARKET_ARTICLE_MONEYNO:
				{
					__int64 const i64Money = pkArticleInfo->kItem.Count() * GOLD2BRONZE;
					SPMOD_Add_Money kAddMoney(i64Money);
					if (S_OK != pkDocPlayer->ItemProcess( SPMO(IMET_ADD_MONEY, kOrder.OwnerGuid(),kAddMoney), kChangeArray, kContLogMgr ))
					{
						return UMR_SYSTEM_ERROR;
					}
					{
						DB_ITEM_STATE_CHANGE kDBChange(DISCT_REMOVE, IMET_REMOVE_ARTICLE, kOrder.OwnerGuid(), kOrder.OwnerGuid(), PgItemWrapper(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0)), PgItemWrapper()); 
						kChangeArray.push_back(kDBChange);
					}
				}break;
			case MARKET_ARTICLE_CASHNO:
				{
					{
						DB_ITEM_STATE_CHANGE kDBChange(DISCT_REMOVE, IMET_REMOVE_ARTICLE_CASH, kOrder.OwnerGuid(), kOrder.OwnerGuid(), PgItemWrapper(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0)), PgItemWrapper()); 
						kChangeArray.push_back(kDBChange);
					}
					{
						PgTranPointer kTran(ECASH_TRAN_OPENMARKET_UNREGCASH, kData.MemberGuid(), kOrder.OwnerGuid(), pkDocPlayer->GroundKey(), SERVER_IDENTITY(), CIE_UM_Article_Dereg);
						DB_ITEM_STATE_CHANGE kDBChange(DISCT_ADD_CASH, kOrder.Cause(), kOrder.OwnerGuid());
						kTran.WriteToPacket(kDBChange.kAddonData);
						kDBChange.kAddonData.Push(static_cast<__int64>(pkArticleInfo->kItem.Count()));	// 회수되는 금액
						kChangeArray.push_back(kDBChange);
					}
				}break;
			default:
				{
					SPMOD_DB2Inv kAddItem(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0));
					SPMO kIMO(IMET_MODIFY_DB2INV,kOrder.OwnerGuid(),kAddItem);
					if (S_OK != pkDocPlayer->ItemProcess( kIMO, kChangeArray,kContLogMgr ) )
					{
						return UMR_SYSTEM_ERROR;
					}
				}break;
			}

			eResult = UMR_SUCCESS;

			std::wstring kItemName;
			GetItemName(pkArticleInfo->kItem.ItemNo(),kItemName);
			PgLog kLog(ELOrderMain_Market_Article,ELOrderSub_Delete);

			kLog.Set(0,kItemName);
			kLog.Set(2,pkArticleInfo->kItemGuId.str().c_str());

			kLog.Set(0,static_cast<int>(pkArticleInfo->kItem.ItemNo()));
			kLog.Set(1,static_cast<int>(pkArticleInfo->kCostType));
			kLog.Set(2,static_cast<int>(pkArticleInfo->kItem.Count()));

			kLog.Set(0,pkArticleInfo->kArticleCost);

			kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
		}break;
	case IMET_BUY_ARTICLE:
		{
			tagPlayerModifyOrderData_ModifyBuyArticle kData;

			kOrder.Read(kData);

			if(kOrder.OwnerGuid() == kData.MarketGuid())
			{
				return UMR_MY_ARTICLES;
			}

			if(kData.BuyNum() <= 0)
			{
				return UMR_ZERO_COUNT;
			}

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kData.MarketGuid());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kMarket = (*iter).second;

			if(MS_OPEN != kMarket.State())
			{
				return UMR_MARKET_CLOSED;
			}

			SUserMarketArticleInfo * pkArticleInfo = kMarket.GetArticleInfo(kData.ArticleGuid());
			if(!pkArticleInfo)
			{
				return UMR_NOT_FOUND_ARTICLE;
			}

			if(0 < pkArticleInfo->kCostType)
			{
				int iValue = 0;
				if( S_OK != g_kVariableContainer.Get(EVar_Kind_Contents, EVar_UM_UseCash, iValue) )
				{
					iValue = DEFAULT_USER_MARKET_USEABLE_CASH;
				}

				if(DEFAULT_USER_MARKET_USEABLE_CASH != iValue)
				{
					return UMR_NOT_SUPPORT;
				}
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const * pkItemDef = kItemDefMgr.GetDef(pkArticleInfo->kItem.ItemNo());
			if(!pkItemDef)
			{
				return UMR_INVALID_ITEM;
			}

			__int64 const i64Cost = (pkItemDef->IsAmountItem() ? kData.BuyNum() : 1) * pkArticleInfo->kArticleCost;

			switch(pkArticleInfo->kItem.ItemNo())
			{
			case MARKET_ARTICLE_MONEYNO:
				{
					__int64 const i64Money = pkArticleInfo->kItem.Count() * GOLD2BRONZE;
					if((kData.BuyNum() != pkArticleInfo->kItem.Count()))
					{
						return UMR_INVALID_ITEM;
					}

					SPMOD_Add_Money kAddMoney(i64Money);
					if (S_OK != pkDocPlayer->ItemProcess( SPMO(IMET_ADD_MONEY, kOrder.OwnerGuid(),kAddMoney), kChangeArray, kContLogMgr ))
					{
						return UMR_SYSTEM_ERROR;
					}

					{
						DB_ITEM_STATE_CHANGE kDBChange(DISCT_REMOVE, IMET_REMOVE_ARTICLE, kOrder.OwnerGuid(), kOrder.OwnerGuid(), PgItemWrapper(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0)), PgItemWrapper()); 
						kChangeArray.push_back(kDBChange);
					}

				}break;
			case MARKET_ARTICLE_CASHNO:
				{
					__int64 const i64Money = static_cast<__int64>(pkArticleInfo->kItem.Count());
					if((kData.BuyNum() != pkArticleInfo->kItem.Count()))
					{
						return UMR_SYSTEM_ERROR;
					}

					{
						DB_ITEM_STATE_CHANGE kDBChange(DISCT_REMOVE, IMET_REMOVE_ARTICLE, kOrder.OwnerGuid(), kOrder.OwnerGuid(), PgItemWrapper(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0)), PgItemWrapper()); 
						kChangeArray.push_back(kDBChange);
					}
					{
						PgTranPointer kTran(ECASH_TRAN_OPENMARKET_BUYCASH, pkDocPlayer->GetMemberGUID(), pkDocPlayer->GetID(), pkDocPlayer->GroundKey(), SERVER_IDENTITY(), CIE_UM_Article_CashBuy);
						DB_ITEM_STATE_CHANGE kDBChange(DISCT_ADD_CASH, kOrder.Cause(), kOrder.OwnerGuid());
						kTran.WriteToPacket(kDBChange.kAddonData);
						kDBChange.kAddonData.Push(i64Money);	// 회수되는 금액
						kChangeArray.push_back(kDBChange);
					}
				}break;
			default:
				{
					int const iCustomType = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
					if(iCustomType == UICT_ELIXIR && (kData.BuyNum() != pkArticleInfo->kItem.Count()))
					{
						return UMR_INVALID_ITEM;
					}

					if(!pkItemDef->IsAmountItem() && (kData.BuyNum() != pkArticleInfo->kItem.Count()))
					{
						return UMR_INVALID_ITEM;
					}

					if(kData.BuyNum() > pkArticleInfo->kItem.Count())
					{
						return UMR_OVER_COUNT;
					}

					WORD wLeftItemNum = pkArticleInfo->kItem.Count() - kData.BuyNum();

					CONT_PLAYER_MODIFY_ORDER kContOrder;
					if(wLeftItemNum <= 0)
					{
						SPMOD_DB2Inv kAddItem(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0));
						SPMO kIMOItem(IMET_MODIFY_DB2INV,kOrder.OwnerGuid(),kAddItem);

						if (S_OK != pkDocPlayer->ItemProcess( kIMOItem, kChangeArray, kContLogMgr))
						{
							return UMR_SYSTEM_ERROR;
						}
					}
					else
					{
						PgBase_Item kItem = pkArticleInfo->kItem;

						kItem.Guid(BM::GUID::Create());
						kItem.Count(kData.BuyNum());
						kItem.CreateDate(BM::PgPackedTime::LocalTime());

						tagPlayerModifyOrderData_Insert_Fixed kAddItem(kItem, SItemPos(0,0),true);
						SPMO kIMOItem1(IMET_INSERT_FIXED,kOrder.OwnerGuid(),kAddItem);

						if (S_OK != pkDocPlayer->ItemProcess( kIMOItem1, kChangeArray, kContLogMgr ))
						{
							return UMR_SYSTEM_ERROR;
						}

						PgBase_Item kLeftItem;
						kLeftItem = pkArticleInfo->kItem;
						kLeftItem.Count(wLeftItemNum);

						DB_ITEM_STATE_CHANGE kItemChange(DISCT_MODIFY,IMET_MODIFY_COUNT,kData.MarketGuid(),kData.MarketGuid(),
							PgItemWrapper(pkArticleInfo->kItem,SItemPos(IT_USER_MARKET,0)),
							PgItemWrapper(kLeftItem,SItemPos(IT_USER_MARKET,0)));

						kChangeArray.push_back(kItemChange);
					}
				}break;
			}

			if(false == pkArticleInfo->kCostType)
			{
				SPMOD_Add_Money kAddMoney(-i64Cost);
				SPMO kIMO(IMET_ADD_MONEY, kOrder.OwnerGuid(),kAddMoney);
				if (S_OK != pkDocPlayer->ItemProcess( kIMO, kChangeArray, kContLogMgr ))
				{
					return UMR_SYSTEM_ERROR;
				}

				{// 구매 업적(골드)
					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(pkDocPlayer->GetID());
					pkActionOrder->kCause = CAE_Achievement;
					pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,pkDocPlayer->GetID(),SPMOD_AddAbil(AT_ACHIEVEMENT_OPENMARKET_BUY_GOLD,i64Cost)));
					g_kJobDispatcher.VPush(pkActionOrder);
				}
			}
			else
			{// 구매 업적(캐시)
				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(pkDocPlayer->GetID());
				pkActionOrder->kCause = CAE_Achievement;
				pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,pkDocPlayer->GetID(),SPMOD_AddAbil(AT_ACHIEVEMENT_OPENMARKET_BUY_CASH,i64Cost)));
				g_kJobDispatcher.VPush(pkActionOrder);
			}

			SUserMarketDealingInfo kDealing;
			kDealing.kDealingGuid = BM::GUID::Create();
			kDealing.cCostType = pkArticleInfo->kCostType;
			kDealing.cItemState = pkArticleInfo->kItem.State();
			kDealing.dwItemNo = pkArticleInfo->kItem.ItemNo();
			kDealing.wItemNum = kData.BuyNum();
			kDealing.i64Enchant_01 = pkArticleInfo->kItem.EnchantInfo().Field_1();
			kDealing.i64Enchant_02 = pkArticleInfo->kItem.EnchantInfo().Field_2();
			kDealing.i64Enchant_03 = pkArticleInfo->kItem.EnchantInfo().Field_3();
			kDealing.i64Enchant_04 = pkArticleInfo->kItem.EnchantInfo().Field_4();
			kDealing.i64SellCost = i64Cost;
			kDealing.kDealerName = kData.BuyerName();
			kDealing.kItemGenTime = pkArticleInfo->kItem.CreateDate();
			kDealing.iCostRate = kMarket.GetCostRate();

			DB_ITEM_STATE_CHANGE kDBData(DISCT_ADD_DEALING, kOrder.Cause(), kOrder.OwnerGuid());
			kDBData.kAddonData.Push(kData.MarketGuid());
			kDBData.kAddonData.Push(kData.ArticleGuid());
			kDealing.WriteToPacket(kDBData.kAddonData);

			kChangeArray.push_back(kDBData);

			eResult = UMR_SUCCESS;

			std::wstring kItemName;
			GetItemName(pkArticleInfo->kItem.ItemNo(),kItemName);

			// 구매자 구매 정보 로그
			{
				PgLog kLog(ELOrderMain_Market_Article,ELOrderSub_Buy);
				kLog.Set(0,kItemName);
				kLog.Set(1,kMarket.CharName());		
				kLog.Set(2,pkArticleInfo->kItemGuId.str().c_str());
				kLog.Set(3,kMarket.OwnerGuId().str().c_str());

				kLog.Set(0,static_cast<int>(pkArticleInfo->kItem.ItemNo()));
				kLog.Set(1,static_cast<int>(pkArticleInfo->kCostType));
				kLog.Set(2,static_cast<int>(kData.BuyNum()));

				kLog.Set(0,i64Cost);

				kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
			}
		}break;
	case IMET_REMOVE_DEALING:	// 판매금액 회수
		{
			SPMOD_ReadDealing kData;
			kOrder.Read(kData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOrder.OwnerGuid());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kMarket = (*iter).second;

			SUserMarketDealingInfo * pkDealing = kMarket.GetDealing(kData.DealingGuId());
			if(!pkDealing)
			{
				return UMR_NOT_FOUND_DEALING;
			}

			tagDBItemStateChange kDBChange(DISCT_REMOVE_DEALING, kOrder.Cause(), kOrder.OwnerGuid());
			kDBChange.kAddonData.Push(kData.DealingGuId());
			kChangeArray.push_back(kDBChange);

			__int64 const i64Cost = PgOpenMarket::CalcArticleCost(pkDealing->i64SellCost,pkDealing->iCostRate);

			if(pkDealing->cCostType)
			{
				if (g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU))
				{
					return UMR_NOT_SUPPORT;
				}

				// Cash 를 올려주는 로직이므로 Item 작업을 먼저하고 Cash변경 한다.
				PgTranPointer kTran(ECASH_TRAN_ADD_CASH, pkDocPlayer->GetMemberGUID(), pkDocPlayer->GetID(), pkDocPlayer->GroundKey(), pkDocPlayer->GetSwitchServer(), CIE_UM_Article_CashBack);
				PgAddCashTran* pkTran = dynamic_cast<PgAddCashTran*>(kTran.GetTran());
				if (pkTran != NULL)
				{
					pkTran->UID(pkDocPlayer->UID());
					pkTran->AccountID(pkDocPlayer->MemberID());
					pkTran->CharacterName(pkDocPlayer->Name());
					pkTran->RemoteAddr(pkDocPlayer->addrRemote());
					pkTran->Cash(i64Cost);
					pkTran->OpenMarketDealingCash(pkDealing->i64SellCost);
				}
				kTran.RequestPayCash();

				{// 업적..
					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(pkDocPlayer->GetID());
					pkActionOrder->kCause = CAE_Achievement;
					pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,pkDocPlayer->GetID(),SPMOD_AddAbil(AT_ACHIEVEMENT_OPENMARKET_SELL_CASH,i64Cost)));
					g_kJobDispatcher.VPush(pkActionOrder);
				}
			}
			else
			{
				SPMOD_Add_Money kAddMoney(i64Cost);
				
				if (S_OK != pkDocPlayer->ItemProcess( SPMO(IMET_ADD_MONEY, kOrder.OwnerGuid(),kAddMoney), kChangeArray, kContLogMgr ))
				{
					return UMR_SYSTEM_ERROR;
				}

				{// 업적..
					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(pkDocPlayer->GetID());
					pkActionOrder->kCause = CAE_Achievement;
					pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,pkDocPlayer->GetID(),SPMOD_AddAbil(AT_ACHIEVEMENT_OPENMARKET_SELL_GOLD,i64Cost)));
					g_kJobDispatcher.VPush(pkActionOrder);
				}
			}

			eResult = UMR_SUCCESS;
		}break;
	case IMET_MODIFY_MARKET_INFO:
		{
			tagPlayerModifyOrderData_ModifyMarket kData;
			kOrder.Read(kData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOrder.OwnerGuid());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kMarket = (*iter).second;

			if(MS_EDIT != kMarket.State())
			{
				return UMR_NEED_EDIT_MODE;
			}

			bool const bPremiumService = pkDocPlayer->GetPremium().IsUserService(true) && (NULL != pkDocPlayer->GetPremium().GetType<S_PST_OpenmarketState>());
			if(false==bPremiumService && kData.Grade() >= MG_NORMAL)
			{
				if(kMarket.ContArticle().size() > kMarket.GetMaxArticleNum(kData.Grade()))
				{
					return UMR_MARKETITEM_FULL;
				}

				if(kData.Grade() != kMarket.MarketGrade())
				{
					if((MG_NORMAL != kData.Grade()) && ((kData.OnlineTime() + kData.OfflineTime()) <= 0))
					{
						return UMR_INVALID_ITEM;
					}
				}

				if(kData.HistoryPoint())
				{
					if(MG_NORMAL == kMarket.MarketGrade())
					{
						return UMR_INVALID_ITEM;
					}
				}
			}

			DB_ITEM_STATE_CHANGE kDBData(DISCT_MODIFY_MARKET_INFO, kOrder.Cause(), kOrder.OwnerGuid());
			kDBData.kAddonData.Push(kOrder.OwnerGuid());
			kData.WriteToPacket(kDBData.kAddonData);
			
			kChangeArray.push_back(kDBData);

			eResult = UMR_SUCCESS;

			{
				__int64 const	i64CurTime = g_kEventView.GetLocalSecTime();
				__int64 const	i64OldOfflineTime = std::max(kMarket.OfflineTime(),static_cast<__int64>(0));
				__int64 const	i64OldOnlineTime = std::max((kMarket.OnlineTime() - i64CurTime),static_cast<__int64>(0));
				__int64 		i64NewOfflineTime = i64OldOfflineTime + kData.OfflineTime();	// 이전 유지 시간 + 추가된 유지 시간
				__int64 		i64NewOnlineTime = i64OldOnlineTime + kData.OnlineTime();

				if(kData.Grade())	// 일반 상점으로 변경이 아니면
				{
					if(kData.Grade() != kMarket.MarketGrade()) // 이전 상점 등급과 다른 등급으로 변경 하면
					{
						i64NewOfflineTime = kData.OfflineTime();	// 상점 유지 시간을 새로운 시간을로 초기화
						i64NewOnlineTime = kData.OnlineTime();
					}
				}
				else
				{
					i64NewOfflineTime = kData.OfflineTime();	// 일반 상점으로 변경하면 Map_constant.ini 설정된 시간으로 초기화
					i64NewOnlineTime = kData.OnlineTime();
				}

				PgLog kLog(ELOrderMain_Market,ELOrderSub_Modify);
				kLog.Set(0,kData.Name());
				kLog.Set(1,kMarket.MarketName());
				kLog.Set(0,static_cast<int>(kData.Grade()));
				kLog.Set(1,static_cast<int>(kMarket.MarketGrade()));
				kLog.Set(2,static_cast<int>(kMarket.MarketHP() + kData.HistoryPoint()));
				kLog.Set(3,static_cast<int>(kMarket.MarketHP()));
				kLog.Set(0,i64NewOnlineTime);
				kLog.Set(1,i64OldOnlineTime);
				kLog.Set(2,i64NewOfflineTime);
				kLog.Set(3,i64OldOfflineTime);
				kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
			}
		}break;
	case IMET_MODIFY_MARKET_STATE:
		{
			tagPlayerModifyOrderData_ModifyMarketState kData;

			kOrder.Read(kData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOrder.OwnerGuid());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kMarket = (*iter).second;

			__int64 i64CurTime = g_kEventView.GetLocalSecTime();
			if(kMarket.MarketGrade() && (kMarket.OnlineTime() < i64CurTime))
			{
				if(MS_OPEN == kData.State())
				{
					return UMR_INVALID_REGTIME;
				}
			}

			if((MS_OPEN == kData.State()) && kMarket.ContArticle().empty())
			{
				return UMR_NOT_FOUND_ARTICLE;
			}

			DB_ITEM_STATE_CHANGE kDBData(DISCT_MODIFY_MARKET_STATE, kOrder.Cause(), kOrder.OwnerGuid());
			kDBData.kAddonData.Push(kOrder.OwnerGuid());
			kDBData.kAddonData.Push(kData.State());
			kChangeArray.push_back(kDBData);

			eResult = UMR_SUCCESS;

		}break;
	case IMET_REMOVE_MARKET:
		{
			SPMOD_RemoveMarket kData;
			kOrder.Read(kData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kData.MarketGuId());
			if(iter == m_kContMarket.end())
			{
				return UMR_NOT_FOUND_MARKET;
			}

			PgOpenMarket & kMarket = (*iter).second;

			if(!kMarket.CanRemove())
			{
				return UMR_MARKET_NOT_EMPTY;
			}

			tagDBItemStateChange kDBChange(DISCT_REMOVE_MARKET, kOrder.Cause(), kOrder.OwnerGuid());
			kDBChange.kAddonData.Push(kData.MarketGuId());

			kChangeArray.push_back(kDBChange);

			eResult = UMR_SUCCESS;//메일 보내는 부분은 무조건 성공 한다.

			{
				PgLog kLog(ELOrderMain_Market,ELOrderSub_Delete);
				kLog.Set(0,kMarket.MarketName());
				kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown Cause=") << kOrder.Cause());
		}break;
	}
	return (eResult == UMR_SUCCESS) ? S_OK : E_FAIL;
}

HRESULT PgOpenMarketMgr::Locked_ProcessModifyItemToDB_OpenMarket(EItemModifyParentEventType const kEventCause, DB_ITEM_STATE_CHANGE const &kElement,
		BM::Stream& kAddonData, CEL::DB_QUERY_TRAN& kContItemModifyQuery)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(kElement.State())
	{
	case DISCT_OPEN_MARKET:
		{
			CONT_OPENMARKET::mapped_type kUserMarket;
			kUserMarket.ReadFromPacket_MarketInfo(kAddonData);

			auto kRet = m_kContMarket.insert(std::make_pair(kUserMarket.OwnerGuId(),kUserMarket));
			if(!kRet.second)
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Already Exists Market") << kUserMarket.OwnerGuId());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			SetMarketInfo(kUserMarket);

			CEL::DB_QUERY kQueryOpenMarket( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_UserMarketOpen2]");
			kQueryOpenMarket.InsertQueryTarget(kUserMarket.OwnerGuId());
			kQueryOpenMarket.PushStrParam(kUserMarket.OwnerGuId());
			kQueryOpenMarket.PushStrParam(kUserMarket.MarketName());
			kQueryOpenMarket.PushStrParam(kUserMarket.OnlineTime());
			kQueryOpenMarket.PushStrParam(kUserMarket.OfflineTime());
			kQueryOpenMarket.PushStrParam(static_cast<BYTE>(kUserMarket.MarketGrade()));

			kContItemModifyQuery.push_back(kQueryOpenMarket);
			kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());

			CONT_ARTICLEINFO_EX const & kContArticleInfo = kUserMarket.ContArticle();

			for(CONT_ARTICLEINFO_EX::const_iterator citer = kContArticleInfo.begin();citer != kContArticleInfo.end();++citer)
			{
				SUserMarketArticleInfo const & kArticleInfo = (*citer).second;

				CEL::DB_QUERY kQueryAddArticle( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UM_ArticleAdd2]");
				kQueryAddArticle.InsertQueryTarget(kElement.RetOwnerGuid());

				kQueryAddArticle.PushStrParam(kElement.RetOwnerGuid());
				kQueryAddArticle.PushStrParam(kArticleInfo.kArticleCost);
				kQueryAddArticle.PushStrParam(kArticleInfo.kCostType);
				kQueryAddArticle.PushStrParam(kArticleInfo.kItemGuId);
				kQueryAddArticle.PushStrParam(kArticleInfo.kItemName);
				kQueryAddArticle.PushStrParam(kArticleInfo.kLevelLimit);

				__int64 i64ClassLimit = kArticleInfo.kClassLimit;
				CalcClassLimit(i64ClassLimit);

				kQueryAddArticle.PushStrParam(i64ClassLimit);
				kQueryAddArticle.PushStrParam(kArticleInfo.kGrade);
				kQueryAddArticle.PushStrParam(kArticleInfo.kInvType);
				kQueryAddArticle.PushStrParam(kArticleInfo.kEquipPos);

				kContItemModifyQuery.push_back(kQueryAddArticle);
			}

		}break;
	case DISCT_ADD_ARTICLE:
		{
			SUserMarketArticleInfo kArticleInfo;
			kArticleInfo.ReadFromPacket(kAddonData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kElement.RetOwnerGuid());
			if(iter == m_kContMarket.end())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Market") << kElement.RetOwnerGuid());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			if(!(*iter).second.ArticleAdd(kArticleInfo))
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Add Article Failed") << kArticleInfo.kItemGuId);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			CEL::DB_QUERY kQueryAddArticle( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UM_ArticleAdd2]");
			kQueryAddArticle.InsertQueryTarget(kElement.RetOwnerGuid());

			kQueryAddArticle.PushStrParam(kElement.RetOwnerGuid());
			kQueryAddArticle.PushStrParam(kArticleInfo.kArticleCost);
			kQueryAddArticle.PushStrParam(kArticleInfo.kCostType);
			kQueryAddArticle.PushStrParam(kArticleInfo.kItemGuId);
			kQueryAddArticle.PushStrParam(kArticleInfo.kItemName);
			kQueryAddArticle.PushStrParam(kArticleInfo.kLevelLimit);

			__int64 i64ClassLimit = kArticleInfo.kClassLimit;
			CalcClassLimit(i64ClassLimit);

			kQueryAddArticle.PushStrParam(i64ClassLimit);
			kQueryAddArticle.PushStrParam(kArticleInfo.kGrade);
			kQueryAddArticle.PushStrParam(kArticleInfo.kInvType);
			kQueryAddArticle.PushStrParam(kArticleInfo.kEquipPos);

			kContItemModifyQuery.push_back(kQueryAddArticle);
			kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			kArticleInfo.WriteToPacket(kContItemModifyQuery.contUserData);
		}break;
	case DISCT_REMOVE_ARTICLE:
		{
			BM::GUID kArticleGuid;
			kAddonData.Pop(kArticleGuid);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kElement.RetOwnerGuid());
			if(iter == m_kContMarket.end())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Market") << kElement.RetOwnerGuid());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			PgOpenMarket & kOpenMarket = (*iter).second;

			if(MS_OPEN == kOpenMarket.State())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T(" Market Opened ") << kElement.RetOwnerGuid());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			if(!kOpenMarket.ArticleRemove(kArticleGuid))
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T(" Remove Article Fail ") << kArticleGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			CEL::DB_QUERY kQueryArticle(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_ArticleDereg2]");
			kQueryArticle.InsertQueryTarget(kElement.RetOwnerGuid());
			kQueryArticle.InsertQueryTarget(kArticleGuid);
			kQueryArticle.PushStrParam(kArticleGuid);

			kContItemModifyQuery.push_back(kQueryArticle);
			kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
		}break;
	case DISCT_ADD_DEALING:
		{
			BM::GUID kMarketGuid,kArticleGuid;
			SUserMarketDealingInfo kDealing;

			kAddonData.Pop(kMarketGuid);
			kAddonData.Pop(kArticleGuid);
			kDealing.ReadFromPacket(kAddonData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kMarketGuid);
			if(iter == m_kContMarket.end())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Market") << kMarketGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			PgOpenMarket & kOpenMarket = (*iter).second;

			SUserMarketArticleInfo * pkArticleInfo = kOpenMarket.GetArticleInfo(kArticleGuid);
			if(!pkArticleInfo)
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Article") << kArticleGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			int iLeftItem = pkArticleInfo->kItem.Count() - kDealing.wItemNum;
			iLeftItem = std::max(0,iLeftItem);

			CEL::DB_QUERY kQueryBuy(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_ArticleBuy]");
			kQueryBuy.InsertQueryTarget(kArticleGuid);
			kQueryBuy.PushStrParam(kArticleGuid);
			kQueryBuy.PushStrParam(static_cast<WORD>(iLeftItem));

			kContItemModifyQuery.push_back(kQueryBuy);
			kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());

			CEL::DB_QUERY kQueryDealing(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_AddDealings2]");
			kQueryDealing.InsertQueryTarget(kArticleGuid);
			kQueryDealing.InsertQueryTarget(kMarketGuid);
			kQueryDealing.PushStrParam(kDealing.kDealingGuid);
			kQueryDealing.PushStrParam(kMarketGuid);
			kQueryDealing.PushStrParam(kDealing.kDealerName);
			kQueryDealing.PushStrParam(kDealing.dwItemNo);
			kQueryDealing.PushStrParam(kDealing.wItemNum);

			kQueryDealing.PushStrParam(kDealing.i64Enchant_01);
			kQueryDealing.PushStrParam(kDealing.i64Enchant_02);
			kQueryDealing.PushStrParam(kDealing.i64Enchant_03);
			kQueryDealing.PushStrParam(kDealing.i64Enchant_04);
			kQueryDealing.PushStrParam(kDealing.i64SellCost);
			kQueryDealing.PushStrParam(kDealing.cCostType);
			kQueryDealing.PushStrParam(kDealing.cItemState);

			kQueryDealing.PushStrParam(kDealing.kItemGenTime);

			kQueryDealing.PushStrParam(kDealing.iCostRate);

			kContItemModifyQuery.push_back(kQueryDealing);

			kOpenMarket.DealingAdd(kDealing);

			if(iLeftItem <= 0)
			{
				kOpenMarket.ArticleRemove(kArticleGuid);
			}
			else
			{
				pkArticleInfo->kItem.Count(iLeftItem);
			}

			if(kOpenMarket.ContArticle().empty())
			{
				kOpenMarket.SetState(MS_EDIT);
			}

			SetMarketInfo(kOpenMarket);

		}break;
	case DISCT_MODIFY_MARKET_INFO:
		{
			tagPlayerModifyOrderData_ModifyMarket kData;
			BM::GUID kOwnerGuid;
			kAddonData.Pop(kOwnerGuid);
			kData.ReadFromPacket(kAddonData);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
			if(iter == m_kContMarket.end())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Market") << kOwnerGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			PgOpenMarket & kMarket = (*iter).second;

			__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

			if(kData.Name().length())
			{
				kMarket.MarketName(kData.Name());
			}

			kMarket.LastBackupTime(i64CurTime);

			if(kData.Grade() >= MG_NORMAL)
			{
				__int64 i64NewOfflineTime = 0i64;
				__int64 i64NewOnlineTime = 0i64;
				int iMarketHP = 0;

				if(kData.Grade() > MG_NORMAL)
				{
					iMarketHP = kMarket.MarketHP() + kData.HistoryPoint();

					if(kData.Grade() != kMarket.MarketGrade())
					{
						i64NewOfflineTime = kData.OfflineTime();
						i64NewOnlineTime = i64CurTime + kData.OnlineTime();
					}
					else
					{
						i64NewOfflineTime = kMarket.OfflineTime() + kData.OfflineTime();
						i64NewOnlineTime = (kMarket.OnlineTime() < i64CurTime ? i64CurTime : kMarket.OnlineTime()) + kData.OnlineTime();
					}
				}
				else
				{
					iMarketHP = 0;
					i64NewOfflineTime = kData.OfflineTime();
					i64NewOnlineTime = i64CurTime + kData.OnlineTime();
				}

				kMarket.MarketGrade(kData.Grade());
				kMarket.UseOfflineTime(false);
				kMarket.OfflineTime(i64NewOfflineTime);
				kMarket.OnlineTime(i64NewOnlineTime);
				kMarket.MarketHP(iMarketHP);
			}

			CEL::DB_QUERY kQueryArticle(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_ModifyUserMarketInfo2]");
			kQueryArticle.InsertQueryTarget(kOwnerGuid);
			kQueryArticle.PushStrParam(kOwnerGuid);
			kQueryArticle.PushStrParam(kMarket.MarketName());
			kQueryArticle.PushStrParam(kMarket.MarketGrade());
			kQueryArticle.PushStrParam(kMarket.OnlineTime());
			kQueryArticle.PushStrParam(kMarket.OfflineTime());
			kQueryArticle.PushStrParam(kMarket.MarketHP());

			kContItemModifyQuery.push_back(kQueryArticle);
			kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
		}break;
	case DISCT_MODIFY_MARKET_STATE:
		{
			BM::GUID kOwnerGuid;
			BYTE bState = 0;
			kAddonData.Pop(kOwnerGuid);
			kAddonData.Pop(bState);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
			if(iter == m_kContMarket.end())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Market") << kOwnerGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			PgOpenMarket & kOpenMarket = (*iter).second;

			if(bState == MS_OPEN)
			{
				kOpenMarket.UseOfflineTime(false);
			}

			kOpenMarket.SetState(bState,false);

			SetMarketInfo(kOpenMarket);

			CEL::DB_QUERY kQueryArticle(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_ModifyUserMarketState2]");
			kQueryArticle.InsertQueryTarget(kOwnerGuid);
			kQueryArticle.PushStrParam(kOwnerGuid);
			kQueryArticle.PushStrParam(bState);
			g_kCoreCenter.PushQuery(kQueryArticle);
		}break;
	case DISCT_REMOVE_DEALING:
		{
			BM::GUID kGuid;
			kAddonData.Pop(kGuid);

			CONT_OPENMARKET::iterator iter = m_kContMarket.find(kElement.RetOwnerGuid());
			if(iter == m_kContMarket.end())
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Not Found Market") << kElement.RetOwnerGuid());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			PgOpenMarket & kOpenMarket = (*iter).second;

			if(!kOpenMarket.DealingRemove(kGuid))
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T(" Remove Dealing Fail ") << kGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			CEL::DB_QUERY kQuery(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UM_DealingRemove]");
			kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
			kQuery.QueryOwner(kElement.RetOwnerGuid());
			kQuery.PushStrParam(kGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case DISCT_REMOVE_MARKET:
		{
			BM::GUID kGuid;
			kAddonData.Pop(kGuid);

			if(!RemoveMarket(kGuid))
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__ << _T(" DISCT_REMOVE_MARKET FAIL ") << kGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown DBChange state=") << kElement.State());
		}break;
	}

	return S_OK;
}

void PgOpenMarketMgr::Locked_ProcessDisableMarket(BM::GUID const & kCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	ProcessDisableMarket(kCharGuid);
}

bool PgOpenMarketMgr::Locked_InsertMarket(PgOpenMarket const & kUserMarket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return InsertMarket(kUserMarket);
}

bool PgOpenMarketMgr::Locked_RemoveMarket(BM::GUID const & kOwnerGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	return RemoveMarket(kOwnerGuid);
}

void PgOpenMarketMgr::Locked_Tick()
{
	BM::CAutoMutex kLock(m_kMutex);
	
	__int64 i64CurTime = g_kEventView.GetLocalSecTime();

	for(CONT_OPENMARKET::iterator iter = m_kContMarket.begin();iter != m_kContMarket.end();++iter)
	{
		PgOpenMarket & kMarket = (*iter).second;

		int const kOldState = kMarket.State();

		kMarket.Tick(i64CurTime);

		if(kOldState != kMarket.State())
		{
			BuildSearchKey(kMarket);
		}
	}
}

bool PgOpenMarketMgr::Locked_IsSameGrade(BM::GUID const& kOwnerGuid, int const iGrade)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_OPENMARKET::const_iterator c_it = m_kContMarket.find(kOwnerGuid);
	if(c_it == m_kContMarket.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return (*c_it).second.MarketGrade()==iGrade;
}

int const MAX_CASH_MONEY_SEARCH_COUNT = 100;

void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kOwnerGuid;
	int iOwnerLevel = 0;
	std::wstring kSearchName;
	bool bSearchNameType			= false;
	int iSearchType					= 0;
	eSearchModeType kSearchTypeType	= SMT_INVTYPE;
	int iLevelMin					= 0,
		iLevelMax					= 0;
	E_ITEM_GRADE kItemGrade			= IG_NORMAL;
	E_ITEM_DISPLAY_GRADE kDisGrade	= IDG_NORMAL;
	__int64 i64ClassLimit			= 0;
	bool bCostType					= false;
	__int64 i64ArticleIndex			= 0;

	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(iOwnerLevel);
	pkPacket->Pop(kSearchName);
	pkPacket->Pop(bSearchNameType);
	pkPacket->Pop(iSearchType);
	pkPacket->Pop(kSearchTypeType);
	pkPacket->Pop(iLevelMin);
	pkPacket->Pop(iLevelMax);
	pkPacket->Pop(kItemGrade);
	pkPacket->Pop(kDisGrade);
	pkPacket->Pop(i64ClassLimit);
	pkPacket->Pop(bCostType);
	pkPacket->Pop(i64ArticleIndex);

	CalcClassLimit(i64ClassLimit);

	CONT_ARTICLE_KEY_LIST kANDKeyList; // ,kORKeyList

/*========================================================================================================================
	여기는 검색 필터에 복수개의 결과가 리턴되는 내용임
========================================================================================================================*/

	CONT_MARKET_ARTICLE_GUID kSearchResult;
	if( !kSearchName.empty() )
	{
		std::transform(kSearchName.begin(), kSearchName.end(), kSearchName.begin(), towupper);	//대문자변환
		if( !bSearchNameType )
		{
			for (unsigned short i = 0; i <= ML_MAX; i++)
			{
				ITEM_NAME_MAP::SET_ID kItemId;
				m_kLocItemNameMap[i].FindItemId(kSearchName,kItemId);

				for(ITEM_NAME_MAP::SET_ID::iterator iter = kItemId.begin();iter != kItemId.end();++iter)
				{
					OpenMarketSearchUtil::Search( CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_NO,(*iter)), m_kContSearchMgr, kSearchResult );
				}
			}
		}
		else
		{
			CHAR_NAME_MAP::SET_ID kItemId;
			m_kCharNameMap.FindItemId(kSearchName,kItemId);
			for(CHAR_NAME_MAP::SET_ID::iterator iter = kItemId.begin();iter != kItemId.end();++iter)
			{
				//kORKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_OWNER_GUID,(*iter)));
				OpenMarketSearchUtil::Search( CONT_ARTICLE_KEY_LIST::value_type(AKT_OWNER_GUID,(*iter)), m_kContSearchMgr, kSearchResult );
			}
		}
	}

	if(iSearchType >= 0)
	{
		switch(kSearchTypeType)
		{
		case SMT_INVTYPE:
			{
				kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_LARGE_CATEGORY,iSearchType));
			}break;
		case SMT_EQUIPPOS:
			{
				kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_SMALL_CATEGORY,iSearchType));
			}break;
		case SMT_CASH:
			{
				int const iEnd = iSearchType + MAX_CASH_MONEY_SEARCH_COUNT;
				for(int i = iSearchType;i < iEnd;++i)
				{
					//kORKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_CASH,i));
					OpenMarketSearchUtil::Search( CONT_ARTICLE_KEY_LIST::value_type(AKT_CASH,i), m_kContSearchMgr, kSearchResult );
				}
			}break;
		case SMT_MONEY:
			{
				int const iEnd = iSearchType + MAX_CASH_MONEY_SEARCH_COUNT;
				for(int i = iSearchType;i < iEnd;++i)
				{
					//kORKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_GOLD,i));
					OpenMarketSearchUtil::Search( CONT_ARTICLE_KEY_LIST::value_type(AKT_GOLD,i), m_kContSearchMgr, kSearchResult );
				}
			}break;
		case SMT_PET:
			{
			}break;
		default:
			{
				CAUTION_LOG(BM::LOG_LV0, __FUNCTIONW__ << __LINE__ << _T(" INVALID SEARCH MODE TYPE : ") << static_cast<BYTE>(kSearchTypeType));
				return;
			}break;
		}
	}

	if(kItemGrade < IG_MAX)
	{//아이템등급이 설정되어있고, 모두 선택이 아닐 때
		kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_GRADE,kItemGrade));
	}

	if(kDisGrade < IDG_MAX)
	{
		kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_DISPLAY_GRADE,kDisGrade));
	}

	if(i64ClassLimit)
	{
		if(i64ClassLimit == UCLIMIT_ALL )
		{
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_FIGHTER));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_MAGICIAN));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_ARCHER));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_THIEF));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_SHAMAN));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_DOUBLE_FIGHTER));
		}
		if(i64ClassLimit == UCLIMIT_ALL_HUMAN )
		{
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_FIGHTER));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_MAGICIAN));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_ARCHER));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_THIEF));
		}
		else if(i64ClassLimit == UCLIMIT_ALL_DRAGONIAN )
		{
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_SHAMAN));
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,UCLIMIT_MARKET_DOUBLE_FIGHTER));
		}
		else
		{
			kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_CALSS,i64ClassLimit));
		}
	}

	if(bCostType)
	{
		kANDKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_COST_TYPE,bCostType));
	}

	if( 0 != iLevelMin
	&&	0 != iLevelMax )
	{
		int const iTempLvMin = iLevelMin;
		int const iTempLvMax = iLevelMax;
		iLevelMin = std::min(iTempLvMin, iTempLvMax);
		iLevelMax = std::max(iTempLvMin, iTempLvMax);
	}
	if( kSearchName.empty() ) // or 검색에 아무것도 걸리지 않으면
	{
		if( kSearchResult.empty() )
		{
			//SearchName이 없고 Result가 비면, AndKey 있음 --> Level범위를 기준으로 목록 작성
			//SearchName이 없고 Result가 비면, AndKey 없음 --> !!아무것도 입력하지 않고 검색 누름!! --> 레벨범위 기준으로 목록 작성

			// Min = 0, Max = 0 --> 아무 결과 없으면 (1 ~ MyLv+5)
			// Min = 0, Max = n --> 1~n
			// Min = n, Max = 0 --> n~MyLv+5
			// Min = n, Max = n' --> n~n'
			int const iLimitMinLevel = (0 != iLevelMin)? iLevelMin: 1;
			int const iLimitMaxLevel = (0 != iLevelMax)? iLevelMax: m_iBiggestItemLevel; // 레벨범위는 입력하지 않는이상 최대값으로 구한다
			for(int i = iLimitMinLevel;i <= iLimitMaxLevel;++i)
			{
				//kORKeyList.push_back(CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_LEVEL,i));
				OpenMarketSearchUtil::Search( CONT_ARTICLE_KEY_LIST::value_type(AKT_ITEM_LEVEL,i), m_kContSearchMgr, kSearchResult );
			}
		}
		else
		{
			//SearchName이 없고, Result가 있으면 --> 다른조건으로 검색 됨, AndKey 없음 --> AndKey 적용
			//SearchName이 없고, Result가 있으면 --> 다른조건으로 검색 됨, AndKey 있음 --> AndKey 적용
		}
	}
	else
	{
		//SearchName이 있고, Result가 있으면 --> 다른조건으로 검색 됨, AndKey 없음 --> AndKey 적용
		//SearchName이 있고, Result가 있으면 --> 다른조건으로 검색 됨, AndKey 있음 --> AndKey 적용
		//SearchName 있고 Result가 비면 --> 검색이 안된거다 --> AndKey 적용해도 빈다(결과 없음)
	}

	if( false == OpenMarketSearchUtil::SearchSub(kANDKeyList, m_kContSearchMgr, kSearchResult) )
	{
		kSearchResult.clear();
	}

	size_t kCheckCount = 0;
	CONT_MARKET_SEARCH_RESULT kContSearchResult;

	for(CONT_MARKET_ARTICLE_GUID::iterator iter = kSearchResult.begin();iter != kSearchResult.end();++iter)
	{
		CONT_OPENMARKET::iterator marketiter = m_kContMarket.find((*iter).first);
		if(marketiter == m_kContMarket.end())
		{
			continue;
		}

		PgOpenMarket & kMarket = (*marketiter).second;

		SMARKET_KEY kMarketKey;
		kMarketKey.kMarketName	= kMarket.MarketName();
		kMarketKey.kCharName	= kMarket.CharName();
		kMarketKey.kOwnerId		= kMarket.OwnerGuId();
		kMarketKey.iGrade		= kMarket.MarketGrade();
		kMarketKey.iHP			= kMarket.MarketHP();

		for(CONT_ARTICLE_GUID::iterator articleiter = (*iter).second.begin();articleiter != (*iter).second.end();++articleiter)
		{
			SUserMarketArticleInfo * pkArticle = kMarket.GetArticleInfo((*articleiter));
			if(NULL == pkArticle)
			{
				continue;
			}
			if( (0 < iLevelMin && iLevelMin > pkArticle->kLevelLimit)
			||	(0 < iLevelMax && iLevelMax < pkArticle->kLevelLimit) )
			{
				continue;
			}

			kMarketKey.kCheckCount	= kCheckCount;
			++kCheckCount;

			kContSearchResult.insert(std::make_pair(kMarketKey,*pkArticle));
		}
	}

	CONT_MARKET_SEARCH_RESULT kSendResult;

	__int64 i64 = 0;
	for(CONT_MARKET_SEARCH_RESULT::iterator iter = kContSearchResult.begin();iter != kContSearchResult.end();++iter,++i64)
	{
		if(i64 < i64ArticleIndex)
		{
			continue;
		}

		if(i64 < (i64ArticleIndex + MARET_ARTICLE_ONE_PAGE))
		{
			kSendResult.insert((*iter));
			continue;
		}
		break;
	}

	BM::Stream kPacket(PT_M_C_UM_ANS_MARKET_QUERY);
	kPacket.Push(UMR_SUCCESS);
	PU::TWriteTable_MM(kPacket,kSendResult);
	kPacket.Push(kContSearchResult.size());
	SendToUser(kOwnerGuid,kPacket,false);
}


void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_MY_VENDOR_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MY_VENDOR_QUERY);
	
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
	if(iter == m_kContMarket.end() || MS_OPEN != (*iter).second.State() )
	{//현재 오픈마켓 상점 목록이 없거나, 찾은 오픈마켓이 오픈된 상태가 아니면 에러
		kAnsPacket.Push(UMR_NOT_FOUND_MARKET);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find OpenMarket"));
		return;
	}

	kAnsPacket.Push(UMR_SUCCESS);
	(*iter).second.WriteToPacket_MarketInfo(kAnsPacket, false);

	SendToUser(kOwnerGuid,kAnsPacket,false);
}

void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_VENDOR_ENTER(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kOrderGuid;
	pkPacket->Pop(kOrderGuid);

	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_VENDOR_ENTER);
	
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
	if(iter == m_kContMarket.end() || MS_OPEN != (*iter).second.State() )
	{//현재 오픈마켓 상점 목록이 없거나, 찾은 오픈마켓이 오픈된 상태가 아니면 에러
		kAnsPacket.Push(UMR_NOT_FOUND_MARKET);
		SendToUser(kOrderGuid,kAnsPacket,false);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find OpenMarket"));
		return;
	}

	kAnsPacket.Push(UMR_SUCCESS);
	(*iter).second.WriteToPacket_MarketInfo(kAnsPacket, false);

	SendToUser(kOrderGuid,kAnsPacket,false);
}

void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_VENDOR_REFRESH_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kOrderGuid;
	pkPacket->Pop(kOrderGuid);

	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	BM::Stream kNfyPacket(PT_I_M_UM_NFY_VENDOR_REFRESH_QUERY);
	
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
	if(iter == m_kContMarket.end() || MS_OPEN != (*iter).second.State() )
	{//현재 오픈마켓 상점 목록이 없거나, 찾은 오픈마켓이 오픈된 상태가 아니면 에러
		kNfyPacket.Push(kOwnerGuid);
		kNfyPacket.Push(UMR_NOT_FOUND_MARKET);
		SendToGround(kSI.nChannel, kGndKey, kNfyPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find OpenMarket"));
		return;
	}
	kNfyPacket.Push(kOwnerGuid);
	kNfyPacket.Push(UMR_SUCCESS);
	(*iter).second.WriteToPacket_MarketInfo(kNfyPacket, false);
	SendToGround(kSI.nChannel, kGndKey, kNfyPacket);
}
void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_MY_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MY_MARKET_QUERY);
	
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
	if(iter == m_kContMarket.end())
	{
		kAnsPacket.Push(UMR_NOT_FOUND_MARKET);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find OpenMarket"));
		return;
	}

	kAnsPacket.Push(UMR_SUCCESS);
	(*iter).second.WriteToPacket_MarketInfo(kAnsPacket, false);

	SendToUser(kOwnerGuid,kAnsPacket,false);

	// Dealing Send
	int const iDivSize = 500;
	int iTotalSize = (*iter).second.GetTotalSize();
	__int64 iCloseTime = (*iter).second.GetClossTime();

	if( iTotalSize <= iDivSize )
	{
		BM::Stream kDealingAnsPacket(PT_M_C_UM_ANS_MY_MARKET_DEALING_QUERY);
		kDealingAnsPacket.Push(UMR_DEALING_ING);
		(*iter).second.WriteToPacket_Dealing(kDealingAnsPacket);
		SendToUser(kOwnerGuid, kDealingAnsPacket, false);
	}
	else
	{
		CONT_MARKET_DEALING::const_iterator iterDealing;
		(*iter).second.GetContDealingIterFirst(iterDealing);

		while( 0 < iTotalSize )
		{
			BM::Stream kDealingAnsPacket(PT_M_C_UM_ANS_MY_MARKET_DEALING_QUERY);
			kDealingAnsPacket.Push(UMR_DEALING_ING);			

			(*iter).second.WriteToPacket_DivDealing(kDealingAnsPacket, iterDealing, iTotalSize, iDivSize);

			SendToUser(kOwnerGuid, kDealingAnsPacket, false);

			iTotalSize -= iDivSize;
			if( 0 >= iTotalSize )
			{
				break;
			}
		}
	}	

	int iDealingMax = 0;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_OpenMarket, EVar_OpenMarket_OpenMarketDealingCount, iDealingMax) )
	{
		CAUTION_LOG(BM::LOG_LV0, __FL__ << _T(" EVar_OpenMarket_OpenMarketDealingCount Not is Data"));
	}
	if( 0 > iDealingMax )
	{
		CAUTION_LOG(BM::LOG_LV0, __FL__ << _T(" EVar_OpenMarket_OpenMarketDealingCount Not is Data Minus"));
		iDealingMax = 10;
	}

	BM::Stream kDealingEndPacket(PT_M_C_UM_ANS_MY_MARKET_DEALING_QUERY);
	kDealingEndPacket.Push(UMR_DEALING_END);
	kDealingEndPacket.Push(static_cast<int>(0));
	kDealingEndPacket.Push(iCloseTime);
	kDealingEndPacket.Push(iDealingMax);
	SendToUser(kOwnerGuid, kDealingEndPacket, false);
}

void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_MINIMUM_COST_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	int iItemNo;
	BM::GUID kOwnerGuid;

	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(iItemNo);

	__int64 i64MinCost = 0;

	CONT_MINIMUM_COST::iterator iter = m_kContMinimumCost.find(iItemNo);
	if(iter != m_kContMinimumCost.end())
	{
		i64MinCost = (*iter).second;
	}

	BM::Stream kPacket(PT_M_C_UM_ANS_MINIMUM_COST_QUERY);
	kPacket.Push(i64MinCost);
	SendToUser(kOwnerGuid,kPacket,false);
}

void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_MARKET_ARTICLE_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID	kOwnerGuid,
				kMarketGuid;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kMarketGuid);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MARKET_ARTICLE_QUERY);
	
	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kMarketGuid);
	if(iter == m_kContMarket.end())
	{
		kAnsPacket.Push(UMR_NOT_FOUND_MARKET);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find OpenMarket"));
		return;
	}

	kAnsPacket.Push(UMR_SUCCESS);
	(*iter).second.WriteToPacket_Article(kAnsPacket);

	SendToUser(kOwnerGuid,kAnsPacket,false);

}

void PgOpenMarketMgr::Locked_RecvPT_M_I_UM_REQ_BEST_MARKET_LIST(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID	kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_BEST_MARKET_LIST);
	CONT_MARKET_KEY_LIST kMarketList;
	size_t kCount = 0;
	for(CONT_MARKET_RANK::const_iterator iter = m_kContMarketRank.begin();iter != m_kContMarketRank.end();++iter)
	{
		kMarketList.push_back((*iter));
		++kCount;
		//if(MAX_BEST_MARKET_LIST_NUM <= kCount)
		//{//보여줄 상점갯수 제한.....
		//	break;
		//}
	}
	
	kAnsPacket.Push(UMR_SUCCESS);
	PU::TWriteArray_M(kAnsPacket,kMarketList);
	SendToUser(kOwnerGuid,kAnsPacket,false);
}

void PgOpenMarketMgr::Locked_RecvORDER_MARKET_CLOSE(BM::GUID const & kGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kGuid);
	if(iter == m_kContMarket.end())
	{
		return;
	}

	(*iter).second.SetState(MS_EDIT);

	BuildSearchKey((*iter).second);
}


EUserMarketResult PgOpenMarketMgr::Locked_VerifyArticlePrice(BM::GUID const& kBuyerGuid, BM::GUID const& kMarketGuid, WORD const wBuyCount, BM::GUID const& kArticleGuid,
	  SCostInfo& rkCost) const
{
	BM::CAutoMutex kLock(m_kMutex);
	rkCost.Init();

	if(kBuyerGuid == kMarketGuid)
	{
		return UMR_MY_ARTICLES;
	}

	if(wBuyCount <= 0)
	{
		return UMR_ZERO_COUNT;
	}

	CONT_OPENMARKET::const_iterator iter_market = m_kContMarket.find(kMarketGuid);
	if(iter_market == m_kContMarket.end())
	{
		return UMR_NOT_FOUND_MARKET;
	}

	PgOpenMarket const& kMarket = (*iter_market).second;
	if(MS_OPEN != kMarket.State())
	{
		return UMR_MARKET_CLOSED;
	}

	SUserMarketArticleInfo const * pkArticleInfo = kMarket.GetArticleInfo(kArticleGuid);
	if(!pkArticleInfo)
	{
		return UMR_NOT_FOUND_ARTICLE;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkItemDef = kItemDefMgr.GetDef(pkArticleInfo->kItem.ItemNo());
	if(!pkItemDef)
	{
		return UMR_INVALID_ITEM;
	}

	int const iCustomType = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
	if(iCustomType == UICT_ELIXIR && (wBuyCount != pkArticleInfo->kItem.Count()))
	{
		return UMR_INVALID_ITEM;
	}

	if(!pkItemDef->IsAmountItem() && (wBuyCount != pkArticleInfo->kItem.Count()))
	{
		return UMR_INVALID_ITEM;
	}

	if(wBuyCount > pkArticleInfo->kItem.Count())
	{
		return UMR_OVER_COUNT;
	}

	__int64 i64Cost = (pkItemDef->IsAmountItem() ? wBuyCount : 1) * pkArticleInfo->kArticleCost;
	if(pkArticleInfo->kCostType)
	{
		// Cash 판매 하는 것이다.
		rkCost.i64Cash = i64Cost;
	}
	else
	{
		rkCost.i64Money = i64Cost;
	}
	return UMR_SUCCESS;
}

EUserMarketResult PgOpenMarketMgr::Locked_Cash_Article_Reg(PgTranPointer const& rkTran)
{
	BM::CAutoMutex kLock(m_kMutex);

	PgOpenMarketCashRegTran* pkTran = dynamic_cast<PgOpenMarketCashRegTran*>(rkTran.GetTran());
	if (pkTran == NULL)
	{
		BM::vstring vLogSub;
		rkTran.Log(vLogSub);
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("dynamic_cast<PgOpenMarketCashRegTran*> error Tran=") << vLogSub);
		return UMR_SYSTEM_ERROR;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	BM::GUID	kOwnerGuid;

	BM::Stream& rkPacket = pkTran->AddedPacket();
	size_t szRdPos = rkPacket.RdPos();
	kOrder.ReadFromPacket(rkPacket);
	rkPacket.Pop(kOwnerGuid);

	SUserMarketArticleInfo  kArticleInfo;
	kArticleInfo.ReadFromPacket(rkPacket);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_ARTICLE_REG);

	CONT_OPENMARKET::iterator iter = m_kContMarket.find(kOwnerGuid);
	if(iter == m_kContMarket.end())
	{
		kAnsPacket.Push(UMR_NOT_FOUND_MARKET);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		return UMR_NOT_FOUND_MARKET;
	}

	PgOpenMarket & kOpenMarket = (*iter).second;

	if(MS_OPEN == kOpenMarket.State())
	{
		kAnsPacket.Push(UMR_ALREADY_OPENED);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		return UMR_ALREADY_OPENED;
	}

	if(kOpenMarket.ContArticle().size() >= kOpenMarket.GetMaxArticleNum(kOpenMarket.MarketGrade()))
	{
		kAnsPacket.Push(UMR_MARKETITEM_FULL);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		return UMR_MARKETITEM_FULL;
	}

	if(kArticleInfo.kItem.Count() > pkTran->InitCash())
	{
		// Cash 모자른다.
		kAnsPacket.Push(UMR_NOT_ENOUGH_CASH);
		SendToUser(kOwnerGuid,kAnsPacket,false);
		return UMR_NOT_ENOUGH_CASH;
	}

		rkPacket.Reset();
		kOrder.WriteToPacket(rkPacket);

	pkTran->Cost(kArticleInfo.kItem.Count());
	if (false == rkTran.RequestPayCash())
	{
		return UMR_SYSTEM_ERROR;
	}
	return UMR_SUCCESS;
}

void PgOpenMarketMgr::FlushMarketInfoToDB(PgOpenMarket const & kUserMarket)
{
	CEL::DB_QUERY kQueryArticle(DT_PLAYER,DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_UM_ModifyUserMarketInfo2]");
	kQueryArticle.InsertQueryTarget(kUserMarket.OwnerGuId());
	kQueryArticle.PushStrParam(kUserMarket.OwnerGuId());
	kQueryArticle.PushStrParam(kUserMarket.MarketName());
	kQueryArticle.PushStrParam(kUserMarket.MarketGrade());
	kQueryArticle.PushStrParam(kUserMarket.OnlineTime());
	kQueryArticle.PushStrParam(kUserMarket.OfflineTime());
	kQueryArticle.PushStrParam(kUserMarket.MarketHP());

	g_kCoreCenter.PushQuery(kQueryArticle);
}

