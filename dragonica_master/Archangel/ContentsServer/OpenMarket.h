#ifndef CONTENTS_CONTENTSSERVER_SERVERSET_OPENMARKET_OPENMARKET_H
#define CONTENTS_CONTENTSSERVER_SERVERSET_OPENMARKET_OPENMARKET_H

#include "Doc/PgDoc_Player.h"
#include "constant.h"

class PgTranPointer;
typedef std::map<BM::GUID,SUserMarketDealingInfo> CONT_MARKET_DEALING;

size_t const MAX_ARTICLE_TABLE[] = {5,10,20};
float const MARKETE_COST_RATE[] = {0.05f,0.03f,0.02f};

__int64 const MARKET_DELETE_TIME = 30 * 24 * 60 * 60;

class PgOpenMarket
{
public:
	PgOpenMarket()
	{
		Clear();
	}
	~PgOpenMarket(){}

public:

	CLASS_DECLARATION_S(__int64,LastBackupTime);
	CLASS_DECLARATION_S(bool,UseOfflineTime);
	CLASS_DECLARATION_S(std::wstring,MarketName);
	CLASS_DECLARATION_S(std::wstring,CharName);
	CLASS_DECLARATION_S(BM::GUID,OwnerGuId);
	CLASS_DECLARATION_S(int,MarketGrade);
	CLASS_DECLARATION_S(__int64,OpenTime);
	CLASS_DECLARATION_S(__int64,CloseTime);
	CLASS_DECLARATION_S(__int64,OnlineTime);
	CLASS_DECLARATION_S(__int64,OfflineTime);
	CLASS_DECLARATION_S(int,MarketHP);
	CLASS_DECLARATION_S(CONT_ARTICLEINFO_EX,ContArticle);
	CLASS_DECLARATION_S(CONT_MARKET_DEALING,ContDealing);

	void Clear();
	size_t const GetMaxArticleNum(int const kGrade);
	bool CanRemove()const;
	void SetState(int const iState,bool const bSave = true);
	int const State()const;
	bool LoadDB(CEL::DB_DATA_ARRAY::const_iterator & itor,int const iArticleCount,int const iDealingCount,CONT_SELECTED_CHARACTER_ITEM_LIST const & kItemList);

	static __int64 const CalcArticleCost(__int64 const i64Cost,int const iCostRate);

protected:

	int m_kState;

public:

	int const GetCostRate();
	bool ArticleAdd(SUserMarketArticleInfo const & kArticleInfo);
	SUserMarketArticleInfo * GetArticleInfo(BM::GUID const & kArticleGuId);
	SUserMarketArticleInfo const * GetArticleInfo(BM::GUID const & kArticleGuId) const;
	bool ArticleRemove(BM::GUID const & kArticleGuId);
	void DealingAdd(SUserMarketDealingInfo const & kDealing);
	SUserMarketDealingInfo * GetDealing(BM::GUID const & kDealGuId);
	bool DealingRemove(BM::GUID const & kDealGuId);
	void WriteToPacket_MarketInfo(BM::Stream & kPacket, bool bDealing=true)const;
	void ReadFromPacket_MarketInfo(BM::Stream & kPacket, bool bDealing=true);
	void WriteToPacket_Dealing(BM::Stream & kPacket)const;
	void WriteToPacket_Article(BM::Stream & kPacket)const;
	void ReadFromPacket_Dealing(BM::Stream & kPacket);
	void ReadFromPacket_Article(BM::Stream & kPacket);
	void Tick(__int64 const & i64CurTime);
	void WriteToPacket_DivDealing(BM::Stream & kPacket, CONT_MARKET_DEALING::const_iterator &iterDealing, int iTotalSize, int const iDivSize)const;
	__int64 GetClossTime() const;
	int GetTotalSize() const;
	void GetContDealingIterFirst(CONT_MARKET_DEALING::const_iterator &iterDealing);
};

typedef std::map<BM::GUID,PgOpenMarket> CONT_OPENMARKET;

typedef enum E_ARTICLE_KEY_TYPE
{
	AKT_NONE = 0,
	AKT_ITEM_NO,
	AKT_ITEM_GRADE,
	AKT_ITEM_DISPLAY_GRADE,
	AKT_ITEM_LEVEL,
	AKT_ITEM_CALSS,
	AKT_LARGE_CATEGORY,
	AKT_SMALL_CATEGORY,
	AKT_COST_TYPE,
	AKT_OWNER_GUID,
	AKT_GOLD,
	AKT_CASH,
}EArticleKeyType;

struct tagARTICLE_KEY;

typedef struct tagARTICLE_KEY
{
	EArticleKeyType kKeyType;
	__int64	i64KeyValue;

	tagARTICLE_KEY():kKeyType(AKT_NONE),i64KeyValue(0){}
	tagARTICLE_KEY(EArticleKeyType const kType,__int64 const i64Value):kKeyType(kType),i64KeyValue(i64Value){}
	tagARTICLE_KEY(EArticleKeyType const kType,BM::GUID const & kGuid):kKeyType(kType)
	{
		memcpy(&i64KeyValue,&kGuid,sizeof(i64KeyValue));
	}
	~tagARTICLE_KEY(){}

	bool operator < (tagARTICLE_KEY const & kKey) const
	{
		if(kKeyType < kKey.kKeyType)
		{
			return true;
		}
		else if(kKeyType == kKey.kKeyType)
		{
			switch(kKeyType)
			{
			case AKT_OWNER_GUID:
				{
					BM::GUID kGuidA,kGuidB;
					memcpy(&kGuidA,&i64KeyValue,sizeof(i64KeyValue));
					memcpy(&kGuidB,&kKey.i64KeyValue,sizeof(kKey.i64KeyValue));
					if(kGuidA < kGuidB)
					{
						return true;
					}
				}break;
			default:
				{
					if(i64KeyValue < kKey.i64KeyValue)
					{
						return true;
					}
				}break;
			}
		}
		return false;
	}
}SARTICLE_KEY;

typedef std::set<BM::GUID>									CONT_ARTICLE_GUID;
typedef std::list< tagARTICLE_KEY >							CONT_ARTICLE_KEY_LIST;
typedef std::map< BM::GUID,CONT_ARTICLE_GUID>				CONT_MARKET_ARTICLE_GUID; // MARKET GUID , ITEM GUID SET
typedef std::map< SARTICLE_KEY, CONT_MARKET_ARTICLE_GUID >	CONT_SEARCH_MGR;
typedef std::set< SMARKET_KEY >								CONT_MARKET_RANK;

template<typename T>
class PgStringNameMap
{
public:

	struct tagITEM_NAME_NODE;
	typedef std::map<wchar_t,tagITEM_NAME_NODE> CONT_WCHAR_KEY;

	typedef std::set<T> SET_ID;
	typedef struct tagITEM_NAME_NODE
	{
		SET_ID			kSetId;
		CONT_WCHAR_KEY	kSubNode;
	}SITEM_NAME_NODE;

protected:

	CONT_WCHAR_KEY m_kCont;

public:

	void BuildNameKey(std::wstring const & kName,T const & tID)
	{
		wchar_t const * wstrChar = kName.c_str();
		std::wstring::size_type kLen = kName.length();
		for(std::wstring::size_type i = 0;i < kLen;i++)
		{
			Build(std::wstring((wstrChar + i)),tID);
		}
	}

	void FindItemId(std::wstring const & kName,SET_ID & kSetId)
	{
		CONT_WCHAR_KEY * pkNode = &m_kCont;

		wchar_t kKey = 0;
		
		SITEM_NAME_NODE * pkNameNode = NULL;

		std::wstring::const_iterator iter = kName.begin();

		while(iter != kName.end())
		{
			kKey = (*iter);

			CONT_WCHAR_KEY::iterator fiter = pkNode->find(kKey);
			
			if(fiter == pkNode->end())
			{
				return;
			}

			++iter;
			if(iter == kName.end())
			{
				pkNode = &fiter->second.kSubNode;
				kSetId = fiter->second.kSetId;
				break;
			}

			pkNode = &fiter->second.kSubNode;
		}

		DumpId(kSetId,*pkNode);
	}

private:

	bool Build(std::wstring const kName,T const & tID)
	{
		std::wstring::const_iterator iter = kName.begin();
	
		if(iter == kName.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		BuildNode(iter,kName.end(),tID,m_kCont);
		return true;
	}

	void BuildNode(std::wstring::const_iterator & curiter,std::wstring::const_iterator const & enditer,T const & tID,CONT_WCHAR_KEY & kNode)
	{
		wchar_t kKey = (*curiter);

		++curiter;

		if(curiter == enditer)
		{
			kNode[kKey].kSetId.insert(tID);
			return;
		}

		BuildNode(curiter,enditer,tID,kNode[kKey].kSubNode);
	}

	void DumpId(SET_ID & kSetId,CONT_WCHAR_KEY & kNode)
	{
		for(CONT_WCHAR_KEY::iterator iter = kNode.begin();iter != kNode.end();++iter)
		{
			kSetId.insert((*iter).second.kSetId.begin(),(*iter).second.kSetId.end());
			DumpId(kSetId,(*iter).second.kSubNode);
		}
	}
};

typedef PgStringNameMap<int> ITEM_NAME_MAP;
typedef PgStringNameMap<BM::GUID> CHAR_NAME_MAP;
typedef std::map<int,__int64>	CONT_MINIMUM_COST;
typedef std::map<unsigned short, ITEM_NAME_MAP> LOC_ITEM_NAME_MAP;

class PgOpenMarketMgr
{
public:
	PgOpenMarketMgr();
	~PgOpenMarketMgr();

public:
	HRESULT Locked_ProcessModifyPlayer_OpenMarket(EItemModifyParentEventType const kCause, SPMO const &kOrder, PgDoc_Player* pkDocPlayer,
		BM::Stream const &kAddonPacket, BM::Stream & rkPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr & kContLogMgr);
	HRESULT Locked_ProcessModifyItemToDB_OpenMarket(EItemModifyParentEventType const kEventCause, DB_ITEM_STATE_CHANGE const &kElement, BM::Stream& kAddonData,
		 CEL::DB_QUERY_TRAN& kContItemModifyQuery);
	void Locked_ProcessDisableMarket(BM::GUID const & kCharGuid);
	bool Locked_InsertMarket(PgOpenMarket const & kUserMarket);
	bool Locked_RemoveMarket(BM::GUID const & kOwnerGuid);
	void Locked_Tick();
	void Locked_RecvPT_M_I_UM_REQ_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MY_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MY_VENDOR_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_VENDOR_ENTER(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_VENDOR_REFRESH_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MINIMUM_COST_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_MARKET_ARTICLE_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_UM_REQ_BEST_MARKET_LIST(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvORDER_MARKET_CLOSE(BM::GUID const & kGuid);
	bool Locked_IsSameGrade(BM::GUID const& kOwnerGuid, int const iGrade)const;
	EUserMarketResult Locked_Cash_Article_Reg(PgTranPointer const& rkTran);
	typedef struct _SCostInfo
	{
		_SCostInfo()
		{
			Init();
		}

		void Init()
		{
			i64Money = i64Cash = i64Mileage = 0;
		}
		__int64 i64Money;
		__int64 i64Cash;
		__int64 i64Mileage;
	} SCostInfo;
	EUserMarketResult Locked_VerifyArticlePrice(BM::GUID const& kBuyerGuid, BM::GUID const& kMarketGuid, WORD const wBuyCount, BM::GUID const& kArticleGuid,
		SCostInfo& rkCost) const;

protected:

	bool InsertMarket(PgOpenMarket const & kUserMarket);

	bool SetMarketInfo(PgOpenMarket const & kUserMarket);
	bool RemoveMarket(BM::GUID const & kOwnerGuid);

	HRESULT RemoveMarketQuery(BM::GUID const & kOwnerGuid);

	void ClearSearchKey(BM::GUID const & kOwnerGuId);
	void BuildSearchKey(PgOpenMarket const & kUserMarket);
	void ProcessDisableMarket(BM::GUID const & kCharGuid);
	void CalcClassLimit(__int64 & i64ClassLimit);
	void SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid = true/* false 는 캐릭터guid */);

private:
	mutable Loki::Mutex m_kMutex;
	CONT_SEARCH_MGR		m_kContSearchMgr;
	CONT_OPENMARKET		m_kContMarket;
	ITEM_NAME_MAP		m_kItemNameMap;
	LOC_ITEM_NAME_MAP	m_kLocItemNameMap;
	CHAR_NAME_MAP		m_kCharNameMap;
	CONT_MINIMUM_COST	m_kContMinimumCost;
	CONT_MARKET_KEY		m_kContMarketKey;
	CONT_MARKET_RANK	m_kContMarketRank;
	CLASS_DECLARATION_S(size_t,CheckCount);
	int					m_iBiggestItemLevel; // 가장큰 아이템 레벨

	static void FlushMarketInfoToDB(PgOpenMarket const & kUserMarket);	//절대 외부에서 쓰지 말것. Locked함수 내부에서만 사용할 것!!!
};

#endif // CONTENTS_CONTENTSSERVER_SERVERSET_OPENMARKET_OPENMARKET_H