#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_EVENT_PGREALMEVENT_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_EVENT_PGREALMEVENT_H

////////////////////////////////////////////////////////
//

enum EREALM_EVENT_TIME_LOAD_STATE : BYTE
{
	EETLS_NONE = 0,
	EETLS_STARTDATE = 1,
	EETLS_ENDDATE = 2,
	EETLS_STARTTIME = 4,
	EETLS_ENDTIME = 8,

	EETLS_COMPLETE = 15
};

typedef struct tagRealmEventTime
{
	tagRealmEventTime();
	tagRealmEventTime(tagRealmEventTime const& rhs);
	~tagRealmEventTime();

	void Clear();
	bool IsCanRun(BM::DBTIMESTAMP_EX const& rkCurTime) const;
	bool ParseXml(char const* szElementName, char const* szValue);

	static bool ParseDate(char const* szValue, BM::DBTIMESTAMP_EX& rkOut);
	static bool ParseTime(char const* szValue, BM::DBTIMESTAMP_EX& rkOut);

	bool IsLoadWholeData(void) { return (eLoadState == EETLS_COMPLETE); }

	BM::DBTIMESTAMP_EX kStartDateTime;
	BM::DBTIMESTAMP_EX kEndDateTime;

	BYTE eLoadState;
} SRealmEventTime;
typedef std::list<SRealmEventTime> CONT_REALM_EVENT_TIME;

////////////////////////////////////////////////////////
//
typedef std::vector< int > CONT_BUFF;
typedef std::list<SRealmEventTime> CONT_REALM_EVENT_TIME;

class PgRealmQuestEvent
{
	//
	typedef struct tagTargetCount
	{
		tagTargetCount();
		tagTargetCount(tagTargetCount const& rhs);
		~tagTargetCount();

		void GetBuff(CONT_BUFF & VecBuff) const;
		bool ParseXml(TiXmlElement const* pkRoot);

		size_t iTargetCount;
		CONT_BUFF kContBuff;
	} STargetCount;
	typedef std::map< size_t, STargetCount > CONT_TARGET_COUNT;
	//
	class PgNotifyHelper
	{
	public:
		PgNotifyHelper();
		PgNotifyHelper(PgNotifyHelper const& rhs);
		~PgNotifyHelper();

		void Clear();
		bool Tick(bool const bCurStatus, DWORD const dwEleapsedTime);
		void AnsInfo(BM::GUID const& rkCharGuid, SRealmQuestInfo const& rkRealmQuestInfo, BM::Stream::DEF_STREAM_TYPE const wType = PT_N_C_ANS_REALM_QUEST_INFO);
		void NfyInfo(SRealmQuestInfo const& rkRealmQuestInfo, bool const bAllowSendToUser = true);
	protected:
		void NfyUser(BM::GUID const& rkCharGuid, BM::Stream const& rkPacket) const;

	private:
		DWORD m_dwCurTickSec;
		CLASS_DECLARATION_S(DWORD, NotifyTickSec);
	};
	//
	class PgRewardBuffHelper
	{
	public:
		PgRewardBuffHelper();
		PgRewardBuffHelper(PgRewardBuffHelper const& rhs);
		~PgRewardBuffHelper();

		void Clear();
		void Set(size_t const iCount, CONT_BUFF & VecBuff);
		bool Tick(std::wstring const& rkRealmQuestID, DWORD const dwEleapsedTime);
		void Notice(bool const bSuccess, std::wstring const& rkRealmQuestID, DWORD const dwNowSec = 0);
		void GetBuff(CONT_BUFF & VecBuff);
	private:
		CLASS_DECLARATION_S_NO_SET(DWORD, EleapsedTime);
		CLASS_DECLARATION_S(size_t, Count);
		CONT_BUFF m_kBuff;
	};
	//
	class PgRealmQuestStatusHelper
	{
	public:
		PgRealmQuestStatusHelper();
		PgRealmQuestStatusHelper(PgRealmQuestStatusHelper const& rhs);
		~PgRealmQuestStatusHelper();

		void Load(SRealmQuestInfo const& rkRealmQuestInfo) const;
		void Save(SRealmQuestInfo const& rkRealmQuestInfo) const;
	private:
		BM::GUID m_kMngGuid;
	};

public:
	// DB Query
	static bool Q_DQT_LOAD_REALM_QUEST(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_UPDATE_REALM_QUEST(CEL::DB_RESULT &rkResult);

public:
	PgRealmQuestEvent();
	PgRealmQuestEvent(PgRealmQuestEvent const& rhs);
	~PgRealmQuestEvent();

	void Tick(SYSTEMTIME const& rkCurTime, DWORD const dwEleapsedTime);
	void AddCount(BM::GUID const& rkCharGuid, size_t const iSetCount = 0);
	void ReqInfo(BM::GUID const& rkCharGuid);
	void Load(__int64 const iCount);
	bool ParseXml(TiXmlElement const* pkRoot);
	std::wstring const& RealmQuestID() const		{ return m_kRealmQuestInfo.kRealmQuestID; }

protected:
	static void UpdateNextCount(SRealmQuestInfo& rkRealmQuestInfo, CONT_TARGET_COUNT const& rkContTargetCount);
	bool IsCanRun(BM::DBTIMESTAMP_EX const& rkCurTime) const;

private:
	SRealmQuestInfo m_kRealmQuestInfo;
	SRealmEventTime m_kTime;
	CONT_REALM_EVENT_TIME m_kContTime;
	PgNotifyHelper m_kNotifyHelper;
	CONT_TARGET_COUNT m_kContTargetCount;
	PgRewardBuffHelper m_kRewardBuffHelper;
	PgRealmQuestStatusHelper m_kStatusHelper;
};
typedef std::map< std::wstring, PgRealmQuestEvent > CONT_REALM_QUEST;

////////////////////////////////////////////////////////
//
class PgDiceEvent
{
	typedef std::vector< int > CONT_DICE_BUFF;

	//
	typedef struct tagDice
	{
		tagDice();
		bool ParseXml(TiXmlElement const* pkRoot);
		bool IsCanTick(DWORD const dwEleapsedTime) const;
		bool RunDice() const;
		bool CheckFailCount(size_t const iCurFailed) const;

		int m_iMinDice;
		int m_iMaxDice;
		int m_iHitDice;
		DWORD m_dwTick;
		size_t m_iMaxFailCount;
	} SDice;
public:
	PgDiceEvent();
	~PgDiceEvent();

	void Tick(SYSTEMTIME const& rkCurTime, DWORD const dwEleapsedTime);
	bool ParseXml(TiXmlElement const* pkRoot);

protected:
	void NfyAddBuff();
	bool DiceEvent();

private:
	// Status
	DWORD	m_dwLastTick;
	size_t	m_iCurFailCount;
	int		m_iSumOfDiceBuffRate;

	// Info
	SDice					m_kInfo;
	SRealmEventTime			m_kTime;
	CONT_DICE_BUFF			m_kContDiceBuff;
	SWorldEnvironmentStatus m_kWorldEnvStatus;
};


////////////////////////////////////////////////////////
//
class PgRealmEventMngImpl
{
public:
	PgRealmEventMngImpl();
	~PgRealmEventMngImpl();

	bool ParseXml();
	bool ProcessPacket(BM::Stream& rkPacket);
	void Tick();
	
private:
	bool ParseXmlImp(TiXmlElement const *pkElement );
	PgDiceEvent m_kDiceEvent;
	CONT_REALM_QUEST m_kContRealmQuest;
	DWORD m_dwTickTime;
};

////////////////////////////////////////////////////////
//
class PgRealmEventMng : public TWrapper< PgRealmEventMngImpl, Loki::Mutex >
{
public:
	PgRealmEventMng();
	~PgRealmEventMng();

	bool ParseXml();
	bool ProcessPacket(BM::Stream& rkPacket);
	void Tick();
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_EVENT_PGREALMEVENT_H