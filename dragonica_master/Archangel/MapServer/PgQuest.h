#ifndef MAP_MAPSERVER_ACTION_QUEST_PGQUEST_H
#define MAP_MAPSERVER_ACTION_QUEST_PGQUEST_H

namespace PgGroundQuestUtil
{
	bool IsCanShowQusetDialog(EQuestShowDialogType const eShowType, int const iQuestID, int const iDialogID);
	bool IsHaveCoupleQuest(PgMyQuest const* pkMyQuest);
	bool MakeQuestReward(CONT_PLAYER_MODIFY_ORDER& rkOrder, PgQuestInfo const* pkQuestInfo, PgPlayer* pkPlayer, int const iSelect1 = 0, int const iSelect2 = 0);
};


typedef std::map< int, PgQuestInfo* > QuestContainer;
typedef std::map< int, ContQuestID > ContItemToQuest;
typedef std::map< SQuestTriggerInfo, int > ContLocationQuest;
typedef std::map< int, ContQuestID > QuestGroupContainer;
typedef std::map< int, ContQuestID > ContMonsterQuest;
typedef std::map< int, ContQuestID > ContGroundQuest;

typedef struct tagNpcQuestInfoKey
{
	tagNpcQuestInfoKey();
	tagNpcQuestInfoKey(EQuestType const& reType, int const& riQuestID);
	tagNpcQuestInfoKey(tagNpcQuestInfoKey const& rhs);
	~tagNpcQuestInfoKey();

	bool operator <(tagNpcQuestInfoKey const& rhs) const;

	EQuestType eType;
	int iQuestID;
} SNpcQuestInfoKey;
typedef std::map< SNpcQuestInfoKey, SQuestNpc const* > ContQuestNpcPtr;//Quest ID, Dialog Info

////////////////////////////////////////////////////////////////////
class PgNPCQuest
{
	friend class PgQuestManager;

public:
	BM::GUID const & NpcGuid() {return m_kNpcGuid;};

	PgNPCQuest(BM::GUID const & rkNpcGuid);
	~PgNPCQuest();

	int GetEventNo(int const iQuestID, const EQuestState eState) const;

private:
	void Build(PgQuestInfo const* pkQuestInfo, const ContQuestNpc &rkClient, const ContQuestNpc &rkAgent, const ContQuestNpc &rkPayer);
	static void Build(PgQuestInfo const* pkQuestInfo, const ContQuestNpc &rkSrcCont, ContQuestNpcPtr& rkTrgCont, BM::GUID const & rkNpcGuid);

	BM::GUID m_kNpcGuid;
	ContQuestNpcPtr m_kClient;//주는 퀘스트
	ContQuestNpcPtr m_kAgent;//진행 하는 퀘스트
	ContQuestNpcPtr m_kPayer;//보상 주는 퀘스트
};
typedef std::map< BM::GUID, PgNPCQuest > ContNpcQuest;//NPC Guid -> Quest IDs

////////////////////////////////////////////////////////////////////
class PgQuestManager
{
public:
	PgQuestManager();
	~PgQuestManager();

	bool Build(CONT_DEF_QUEST_REWARD const *pkQuestReward);//DB Data로 Build
	void Destroy();
	void Reload();

	void BuildNpc(BM::GUID const & rkNPCGuid);//NPC에 퀘스트정보를 설정
	size_t CheckQuest(PgPlayer* pkPC, BM::GUID const & rkNpcGuid, ContSimpleQuest& rkOutCont) const;

	//속도를 생각하려면 이 방법이 가장 좋다
	bool GetQuest(int const iQuestID, PgQuestInfo const*& pkOut) const;
	bool GetGroupQuestID(int const iGroupNo, ContQuestID const*& pkOut) const;//Group No를 가지고 퀘스트 목록
	//Object to quest iD
	bool GetTriggerToQuest(SQuestTriggerInfo const &rkTriggerInfo, int& rsQuestID) const;
	bool GetItemToQuest(int const iItemNo, ContQuestID const*& pkOut) const;
	bool GetItemToQuestRepair(int const iItemNo, ContQuestID const*& pkOut) const;
	bool GetItemToQuestEnchant(int const iItemNo, ContQuestID const*& pkOut) const;
	bool GetItemToQuestPet(int const iItemNo, ContQuestID const*& pkOut) const;
	//Npc To Quest Event
	int GetNPCEventNo(BM::GUID const &rkNPCGuid, int const iQuestID, const EQuestState eState) const;
	//Monster To Quest
	bool GetMonsterQuest(int const iMonsterClassNo, ContQuestID const*& pkOut)const;
	bool GetGroundQuest(int const iGroundNo, ContQuestID const*& pkOut)const;
	//KillCount To Quest
	ContQuestID const& GetKillCountQuest() const;

	void BuildQuestItem(PgQuestInfo const * pkQuest);

	bool GetItemQuests(int const iItemNo, CONT_QUEST_SET & kCont) const;

protected:
	void Swap(PgQuestManager& rhs);
	bool Create(CONT_DEF_QUEST_REWARD const* pkDefQuestReward);
	bool Verify() const;
	void BuildMapQuest();

	bool Add(std::wstring const& rkXmlPath, int iQuestId);
	void BuildDepend(PgQuestInfo const* pkQuest);
	bool BuildGroupQuest();

	inline void AddItemDependence(int const iItemNo, int const iQuestID, ContItemToQuest& rkTarget);
	inline bool GetItemToQuestDependence(int const iItemNo, ContQuestID const*& pkOut, const ContItemToQuest& rkTarget) const;

private:
	mutable Loki::Mutex m_kMutex;
	ContNpcQuest m_kNpcToQuest;						// NPC Guid -> Quest ID(Begin/Ing/End)

	QuestContainer m_kQuestMap;

	QuestGroupContainer m_kQuestGroupMap;			// Group to QuestList
	ContItemToQuest m_kItemToQuest;					// ItemNo -> Event QuestID (Only count)
	ContItemToQuest m_kItemToQuest_Repair;			// Repair Only
	ContItemToQuest m_kItemToQuest_Enchant;			// Enchant Only
	ContItemToQuest m_kItemToQuest_Pet;			// Enchant Only
	ContLocationQuest m_kLocationToQuest;			// Location Trigger -> Quest ID
	ContMonsterQuest m_kMonsterQuest;				// Monster ClassNo -> Quest ID
	ContGroundQuest m_kGroundQuest;					// GroundNo -> Quest ID
	ContQuestID m_kKillCountQuest;					// KillCount -> Quest ID

	CLASS_DECLARATION_S(CONT_QUEST_ITEM, ContQuestItems);
};

#define g_kQuestMan Loki::SingletonHolder<PgQuestManager>::Instance()


///////////////////////////
inline void PgQuestManager::AddItemDependence(int const iItemNo, int const iQuestID, ContItemToQuest& rkTarget)
{
	ContItemToQuest::iterator knew_iter = rkTarget.find(iItemNo);
	if( rkTarget.end() == knew_iter )
	{
		ContQuestID kNewVec;
		auto kRet = rkTarget.insert(std::make_pair(iItemNo, kNewVec));
		knew_iter = kRet.first;
	}
	
	ContItemToQuest::mapped_type& rkVec = knew_iter->second;
	rkVec.push_back(iQuestID);
}

inline bool PgQuestManager::GetItemToQuestDependence(int const iItemNo, ContQuestID const*& pkOut, const ContItemToQuest& rkTarget) const
{
	if(0 >= iItemNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContItemToQuest::const_iterator kIter = rkTarget.find(iItemNo);
	if(rkTarget.end() != kIter)
	{
		pkOut = &kIter->second;//찾았다
		return true;
	}
	return false;//못 찾았다
}

#endif // MAP_MAPSERVER_ACTION_QUEST_PGQUEST_H