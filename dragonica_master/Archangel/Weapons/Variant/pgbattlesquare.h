#ifndef WEAPON_VARIANT_BATTLESQUARE_PGBATTLESQUARE_H
#define WEAPON_VARIANT_BATTLESQUARE_PGBATTLESQUARE_H


typedef enum // DB에서 참조 한다 변경 하지 말것
{
	BSGS_NONE			= 0,
	BSGS_WAIT_START		= 1,
	BSGS_NOW_PREOPEN	= 2,
	BSGS_REQ_START		= 3,
	BSGS_NOW_GAME		= 4,		// Contents -> Map
	BSGS_REQ_END		= 5,		// Map
	//BSGS_REQ_END_ERASE	= 6,	사용하지 않음
	BSGS_NOW_END		= 7,
} EBS_GAME_STATUS;


typedef enum
{
	BSJR_SUCCESS	= 0,
	BSJR_MAX		= 1,
	BSJR_LEVEL,
	BSJR_NORESERVE,
	//BSJR_GOLD,
	BSJR_PARTY,
	BSJR_DUPLICATE,
	BSJR_NOTOPEN,
	BSJR_BALANCE,
	BSJR_WAITER,
	BSJR_ALREAD_JOIN,
	BSJR_NONE,
} EBS_JOIN_RETURN;

//
typedef enum eBattleSquareTeam
{
	BST_NONE	= 0,
	BST_RED		= 1,
	BST_BLUE	= 2,
} EBattleSquareTeam;

//
struct SBSItemPoint
{
	SBSItemPoint();
	SBSItemPoint(SBSItemPoint const& rhs);
	~SBSItemPoint();

	bool operator < (SBSItemPoint const& rhs) const;

	int iItemNo;
	int iEffect1;
	int iEffect2;
	int iEffect3;
	int iMinimapIcon;
	DEFAULT_TBL_PACKET_FUNC();
};
typedef std::map< int, SBSItemPoint > CONT_BS_ITEM;

//
typedef struct tagBSRewardItem
{
	tagBSRewardItem();
	tagBSRewardItem(tagBSRewardItem const& rhs);
	~tagBSRewardItem();
	bool operator <(tagBSRewardItem const& rhs) const;
	bool operator ==(tagBSRewardItem const& rhs) const;

	int iMinPoint;
	int iItemNo1;
	int iCount1;
	int iItemNo2;
	int iCount2;

	DEFAULT_TBL_PACKET_FUNC();
} SBSRewardItem;
typedef std::list< SBSRewardItem > CONT_BS_PRIVATE_REWARD_ITEM;
typedef std::list< SBSRewardItem > CONT_BS_PRIVATE_LEVEL_REWARD_ITEM; // iMinPoint는 레벨이다
typedef std::list< SBSRewardItem > CONT_BS_WIN_TEAM_REWARD_ITEM;
typedef std::list< SBSRewardItem > CONT_BS_WIN_BONUS_REWARD_ITEM; // iMinPoint는 레벨이다

typedef struct tagBSReward
{
	tagBSReward()
		: iItemNo(0), iCount(0)
	{
	}
	tagBSReward(int const& riItemNo, int const& riCount)
		: iItemNo(riItemNo), iCount(riCount)
	{
	}
	int iItemNo;
	int iCount;
} SBSReward;
typedef std::list< SBSReward > CONT_BS_REWARD; // 결과 반환 용
typedef std::map< BM::GUID, CONT_BS_REWARD > CONT_BS_USER_REWARD;

//
typedef struct tagBSGame
{
	tagBSGame();
	explicit tagBSGame(tagBSGame const& rhs);
	~tagBSGame();
	bool operator <(tagBSGame const& rhs) const;
	void WriteToPacket(BM::Stream& rkPacket) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	size_t min_size()const;

	int iGameIDX;
	bool bUse;
	int iChannelNameTextID;
	int iLevelMin;
	int iLevelMax;
	int iMaxUser;
	int iWeekOfDay;
	BM::PgPackedTime kStartTime;
	__int64 iGameSec;
	int iGroundNo;
	__int64 iPreOpenSec;
	int iMapBagItemGroundNo;
	int iGenGroupGroundNo;
	int iMonsterBagControlNo;
} SBSGame;

//
class PgBSGame
{
public:
	explicit PgBSGame();
	explicit PgBSGame(SBSGame const& rhs);
	explicit PgBSGame(BM::Stream& rkPacket);
	explicit PgBSGame(PgBSGame const& rhs);
	~PgBSGame();

	void Set(PgBSGame const& rhs);
	void Clear();

	bool AddWinTeamRewardItem(SBSRewardItem const& rkRewardItem);
	bool AddLoseTeamRewardItem(SBSRewardItem const& rkRewardItem);
	bool AddPrivateRewardItem(SBSRewardItem const& rkRewardItem);
	bool AddPrivateLevelRewardItem(SBSRewardItem const& rkRewardItem);
	bool AddWinBonusRewardItem(SBSRewardItem const& rkRewardItem);
	bool AddWinTeamRewardItem(CONT_BS_WIN_TEAM_REWARD_ITEM const& rkContRewardItem);
	bool AddLoseTeamRewardItem(CONT_BS_WIN_TEAM_REWARD_ITEM const& rkContRewardItem);
	bool AddPrivateRewardItem(CONT_BS_PRIVATE_REWARD_ITEM const& rkContRewardItem);
	bool AddPrivateLevelRewardItem(CONT_BS_PRIVATE_LEVEL_REWARD_ITEM const& rkContRewardItem);
	bool AddWinBonusRewardItem(CONT_BS_WIN_BONUS_REWARD_ITEM const& rkContRewardItem);
	void SetBSItem(CONT_BS_ITEM const& rkContBSItem);

	void WriteToPacket(BM::Stream &rkPacket) const;
	void ReadFromPacket(BM::Stream &rkPacket);
	size_t min_size() const;

public:
	static bool AddRewardItem(std::list< SBSRewardItem >& rkCont, SBSRewardItem const& rkRewardItem);
	static bool AddRewardItem(std::list< SBSRewardItem >& rkCont, std::list< SBSRewardItem > const& rkContSource);

private:
	CLASS_DECLARATION_S_NO_SET(SBSGame, GameInfo);
	CLASS_DECLARATION_S_NO_SET(CONT_BS_ITEM, ContBSItem);
	CLASS_DECLARATION_S_NO_SET(CONT_BS_PRIVATE_REWARD_ITEM, ContPrivateRewardItem);
	CLASS_DECLARATION_S_NO_SET(CONT_BS_PRIVATE_LEVEL_REWARD_ITEM, ContPrivateLevelRewardItem);
	CLASS_DECLARATION_S_NO_SET(CONT_BS_WIN_TEAM_REWARD_ITEM, ContWinTeamRewardItem);
	CLASS_DECLARATION_S_NO_SET(CONT_BS_WIN_TEAM_REWARD_ITEM, ContLoseTeamRewardItem);
	CLASS_DECLARATION_S_NO_SET(CONT_BS_WIN_BONUS_REWARD_ITEM, ContWinBonusRewardItem)
};
typedef std::map< int, PgBSGame > CONT_BS_GAME;


//
class PgBSContentsGame
	: public PgBSGame
{
public:
	PgBSContentsGame();
	explicit PgBSContentsGame(PgBSGame const& rhs);
	~PgBSContentsGame();

	void Set(PgBSContentsGame const& rhs);
	void ReadFromServerPacket(BM::Stream& rkPacket);
	void WriteToServerPacket(BM::Stream& rkPacket) const;

private:
	CLASS_DECLARATION_S_NO_SET(EBS_GAME_STATUS, Status);
	CLASS_DECLARATION_S(__int64, StartTime);
	CLASS_DECLARATION_S(__int64, EndTime);
	CLASS_DECLARATION_S(__int64, PreOpenTime);
	CLASS_DECLARATION_S(__int64, LastNoticeTime);
};
typedef std::list< PgBSContentsGame > CONT_BS_CONTENTS_GAME;

typedef struct tagBSIconOwnInfo
{
	tagBSIconOwnInfo() : iIconNo(0), iCount(0), iTeam(0) {};
	explicit tagBSIconOwnInfo(BM::GUID const& _Owner, int const _Team, int const _IconNo, int const _Count) : kOwner(_Owner), iTeam(_Team), iIconNo(_IconNo), iCount(_Count) {};
	tagBSIconOwnInfo const& operator = (tagBSIconOwnInfo const& rhs)
	{
		kOwner = rhs.kOwner;
		iTeam = rhs.iTeam;
		iIconNo = rhs.iIconNo;
		iCount = rhs.iCount;
		return (*this);
	}

	BM::GUID kOwner;
	int iIconNo;
	int iCount;
	int iTeam;
} BSIconOwnInfo;

//
typedef std::map< size_t, PgBase_Item >				CONT_VOLATILE_INVEN_ITEM;
typedef std::map< int, int >						CONT_BS_BUFF_COUNT; // BuffNo, Count
typedef std::set< int >								CONT_BS_BUFF;
typedef std::map< int, int >						CONT_BS_MINIMAP_ICON; // IconNo, Count
typedef std::map< BM::GUID, BSIconOwnInfo >			CONT_BS_HAVE_MINIMAP_ICON;	// <CharGuid, BSIconOwnInfo>
typedef std::map< BM::GUID, int >					CONT_BS_MEMBER_POINT;	// <CharGuid, Point>




//
typedef struct tagBSTeamMember
{
	tagBSTeamMember();
	explicit tagBSTeamMember(CUnit* pkUnit);
	tagBSTeamMember(tagBSTeamMember const& rhs);

	bool operator <(tagBSTeamMember const& rhs) const;
	bool operator ==(tagBSTeamMember const& rhs) const;
	bool operator ==(BM::GUID const& rhs) const;
	void WriteToPacket(BM::Stream& rkPacket) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	size_t min_size() const;

	BM::GUID kCharGuid;
	std::wstring kCharName;
	unsigned short usLevel;
	unsigned short usClass;
	int iPoint;
	unsigned short usKill;
	unsigned short usDead;
	unsigned short usIconCount;
} SBSTeamMember;
typedef std::map< BM::GUID, SBSTeamMember > CONT_BS_TEAM_GUID_MEMBER;
typedef std::list< SBSTeamMember > CONT_BS_TEAM_MEMBER;

//
class PgBSTeam
{
public:
	explicit PgBSTeam(EBattleSquareTeam const eTeam);
	explicit PgBSTeam(PgBSTeam const& rhs);
	~PgBSTeam();

	void Clear();
	int const GetMemberCount() const;
	int const GetWaiterCount() const;
	bool const AddMember(CUnit* pkUnit);
	bool const AddMember(SBSTeamMember const& rkMember);
	void DelMember(BM::GUID const& rkCharGuid);
	bool const AddWaiter(CUnit* pkUnit);
	void DelWaiter(BM::GUID const& rkCharGuid);
	void WriteToPacket(BM::Stream& rkPacket) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToScorePacket(ContGuidSet const& rkContGuid, BM::Stream& rkPacket, bool const bSyncAll = false) const;
	void ReadFromScorePacket(BM::Stream& rkPacket);
	bool IsTeamMember(BM::GUID const& rkGuid) const;
	bool IsTeamWaiter(BM::GUID const& rkGuid) const;
	bool GetMember(BM::GUID const& rkGuid, SBSTeamMember& rkOut) const;

	void AddScore(BM::GUID const& rkGuid, int const iPoint, int const iKill, int const iDead);
	void IncreaseIcon(BM::GUID const& rkGuid);
	void DropAllIcon(BM::GUID const& rkGuid);

protected:
	int UpdateIconCount();

private:
	CLASS_DECLARATION_NO_SET(EBattleSquareTeam, m_eTeam, Team);
	CLASS_DECLARATION(int, m_iTeamPoint, TeamPoint);
	CLASS_DECLARATION_NO_SET(CONT_BS_TEAM_GUID_MEMBER, m_kContMember, ContMember);
	CLASS_DECLARATION_NO_SET(CONT_BS_TEAM_MEMBER, m_kContWaiter, ContWaiter);
	CLASS_DECLARATION_S(int, IconCount);	// 용자의 증표 개수
};

#endif // WEAPON_VARIANT_BATTLESQUARE_PGBATTLESQUARE_H