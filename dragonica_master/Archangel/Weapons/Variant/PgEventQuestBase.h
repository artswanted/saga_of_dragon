#ifndef WEAPON_VARIANT_QUEST_EVENTQUEST_PGEVENTQUESTBASE_H
#define WEAPON_VARIANT_QUEST_EVENTQUEST_PGEVENTQUESTBASE_H

enum EventQuestTalkReturn
{
	EQTR_None		= 0,
	EQTR_Time		= 1,
	EQTR_NotEnd		= 2,
	EQTR_End		= 3,
	EQTR_Duplicate	= 4,
};

enum EventQuestEnvironment
{
	EQE_MAX_REWARD_ITEM_COUNT = 7,
};

struct SEventQuestItem
{
	SEventQuestItem();
	~SEventQuestItem();

	bool IsEmpty() const;
	size_t min_size() const;
	void WriteToPacket(BM::Stream& rkPacket) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	bool operator ==(int const rhs) const;

	int iItemNo;
	int iCount;
};
typedef std::list< SEventQuestItem > ContEventQuestReward;
typedef std::list< SEventQuestItem > ContEventQuestTarget;

struct SEventQuestNotice
{
	SEventQuestNotice();
	~SEventQuestNotice();

	size_t min_size() const;
	void WriteToPacket(BM::Stream& rkPacket) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	bool operator < (SEventQuestNotice const& rhs) const;

	int iTime;
	std::wstring kMessage;
};
typedef std::list< SEventQuestNotice > ContEventQuestNotice;


//
class PgEventQuest
{
public:
	PgEventQuest();
	~PgEventQuest();

	void Clear();
	bool IsEmpty() const;
	bool IsCanRun(BM::PgPackedTime const& rkCurTime) const;
	void Set(PgEventQuest const& rhs);
	void operator =(PgEventQuest const& rhs);

	void WriteToPacket(BM::Stream& rkPacket) const; // 전체
	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToClientPacket(BM::Stream& rkPacket) const; // 클라이언트
	void ReadFromClientPacket(BM::Stream& rkPacket);
	void WriteToMapPacket(BM::Stream& rkPacket) const; // 맵서버
	void ReadFromMapPacket(BM::Stream& rkPacket);

	void ReadFromDBResult(CEL::DB_RESULT_COUNT::const_iterator& count_iter, CEL::DB_DATA_ARRAY::const_iterator& result_iter);

private:
	CLASS_DECLARATION_S_NO_SET(BM::PgPackedTime, StartTime);
	CLASS_DECLARATION_S_NO_SET(__int64, EndTime);

	CLASS_DECLARATION_S_NO_SET(std::wstring, Title);
	CLASS_DECLARATION_S_NO_SET(std::wstring, Prologue);
	CLASS_DECLARATION_S_NO_SET(std::wstring, Info);
	CLASS_DECLARATION_S_NO_SET(std::wstring, NpcTalkPrologue);
	CLASS_DECLARATION_S_NO_SET(std::wstring, NpcTalkNotEnd);
	CLASS_DECLARATION_S_NO_SET(std::wstring, NpcTalkEnd);
	CLASS_DECLARATION_S_NO_SET(std::wstring, RewardMailTitle);
	CLASS_DECLARATION_S_NO_SET(std::wstring, RewardMailContents);

	CLASS_DECLARATION_S_NO_SET(ContEventQuestTarget, TargetItem);
	CLASS_DECLARATION_S_NO_SET(ContEventQuestReward, RewardItem);
	CLASS_DECLARATION_S_NO_SET(__int64, RewardGold);
};

#endif // WEAPON_VARIANT_QUEST_EVENTQUEST_PGEVENTQUESTBASE_H