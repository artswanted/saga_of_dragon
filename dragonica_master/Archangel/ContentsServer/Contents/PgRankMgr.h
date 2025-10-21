#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGRANKMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGRANKMGR_H

#include "BM/PgTask.h"
#include "Variant/PgMission_Report.h"

class PgMission_Rank
	:	public PgMission_Report
{
public:
	static DWORD const ms_SAVE_TIME_DELAY = 1000000;//16분
	static size_t const ms_MAX_SAVE_COUNT = 10;

	typedef std::multimap<SMissionRankKey,PgMission_RankItem>	ConRankList;

	PgMission_Rank();
	virtual ~PgMission_Rank();

	void Clear();
	bool Init(SMissionKey const& rkKey,TBL_MISSION_REPORT& rkReport);
	bool Init(SMissionKey const& rkKey,CONT_MISSION_RANK const&rkCon);

	bool DoAction(ConPlayRankInfo& rkPlayerRankList,int& iClearTime_Top,int& iClearTime_Avg);
	PgMission_RankItem* UpdateMemo(const SMissionRankKey& rkkey, BM::GUID const &rkMemoGuid, std::wstring const& wstrMemo, bool const bSave = false);

	void Tick(DWORD const dwCurTime);
	void Get(CONT_MISSION_RANK &rkRankList, size_t iCount=1);

	virtual void ReadFromPacket(BM::Stream &kPacket);
	virtual void WriteToPacket(BM::Stream &kPacket) const;

	void ReadFromPacket_Rank(BM::Stream &kPacket);
	void WriteToPacket_Rank(BM::Stream &kPacket) const;

	void AddRankItem(SMissionRankKey const &rkKey, BM::GUID const &rkRankItemID, PgMission_RankItem const &rkRankItem);
	bool ProcessMsg(SEventMessage *pkMsg);
	void MissionRankClear();

	CLASS_DECLARATION_S(SMissionKey,MissionKey);
	CLASS_DECLARATION(int,m_iRankLevel_Max,RankLevel_Max);
	CLASS_DECLARATION(int,m_iRankLevel_Min,RankLevel_Min);
protected:
	void Order();
	void SaveReport(DWORD const dwCurTime);

	void RecvPT_T_N_REQ_MISSION_RANKING(BM::Stream const *pkPacket);


	Loki::Mutex			m_kMutex;
	ConRankList			m_kRankList;
	DWORD				m_dwLastSaveTime;
};

class PgRankRegister
{
public:
	PgRankRegister()
		:	m_pkRank(NULL)
		,	m_dwStartTime(0)
	{
	}

	~PgRankRegister()
	{
		m_pkRank = NULL;
	}

	BM::GUID const &Init(PgMission_Rank* pkRank, SPlayerRankInfo const &rPlayerRankInfo)
	{
		m_pkRank = pkRank;
		m_dwStartTime = BM::GetTime32();
		m_kKey = rPlayerRankInfo.kRankKey;
		m_kMemoGuid = rPlayerRankInfo.kMemoGuid;
		return GetID();
	}

	BM::GUID const &GetID()const{return m_kMemoGuid;}

	void Clear()
	{
		m_pkRank = NULL;
		m_dwStartTime = 0;
		m_kKey.Clear();
		m_kMemoGuid.Clear();
	}

	void Update(std::wstring& wstrMemo);
	bool IsEnd(DWORD const dwNow);

	SMissionKey const * GetMissionKey();
	SMissionRankKey const& GetMissionRankKey();

protected:
	PgMission_Rank*	m_pkRank;
	SMissionRankKey		m_kKey;
	BM::GUID			m_kMemoGuid;
	DWORD				m_dwStartTime;
};

class PgRankMgr
{
public:
	PgRankMgr();
	~PgRankMgr();

	typedef std::map<SMissionKey, PgMission_Rank*>		ConRank;
	typedef std::map< BM::GUID,PgRankRegister*>			ConRegister;
	typedef std::queue<PgRankRegister*>					ConRegisterQueue;

	void OnRegisterMemoTick(DWORD const dwCurTime);
	void OnTick(DWORD const dwCurTime);

	bool Build(CONT_DEF_MISSION_RESULT const &rkResult,
		CONT_DEF_MISSION_CANDIDATE const &rkCandi,
		CONT_DEF_MISSION_ROOT const &rkRoot);
	void AddRank(SMissionKey const &rkMissionKey, int const iRankLevel_Min, int const iRankLevel_Max);
	bool ProcessMsg(SEventMessage *pkMsg);

protected:
	PgMission_Rank* CreateMission_Rank(SMissionKey const &rkMissionKey);
	PgMission_Rank* GetRank(SMissionKey const &rkMissionKey);
	bool AddRegister(PgMission_Rank* pkRank,SPlayerRankInfo const &rPlayerRankInfo);
	bool SendRankingList(SMissionKey const& rkMissionKey, BM::GUID const &rkMemberGuid);
	bool UpdateRegister(BM::GUID const &rkGuid, std::wstring& wstrMemo);
	void InitRank(SMissionKey const &rkKey, CONT_MISSION_RANK const &rkRank);
	void RecvPT_T_N_REQ_MISSION_RANKING(BM::Stream * const pkPacket);
	bool SendRankingTop(SMissionKey const& rkMissionKey, BM::GUID const &rkMemberGuid);
	void ClearRank();

protected:
	BM::TObjectPool<PgMission_Rank>		m_kRankPool;
	BM::TObjectPool<PgRankRegister>		m_kRegisterPool;

	ConRank				m_kConRank;
	ConRegisterQueue	m_kQueue;
	ConRegister			m_kConRegister;
	PgMissionContMgr	m_kMissionMgr;

	SMissionKey			m_kNextKey;
	Loki::Mutex			m_kMutex;
};

//#define g_kRankMgr SINGLETON_STATIC(PgRankMgr)
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGRANKMGR_H