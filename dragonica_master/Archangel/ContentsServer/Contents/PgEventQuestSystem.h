#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGEVENTQUESTSYSTEM_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGEVENTQUESTSYSTEM_H

//
class PgEventQuestUser
{
public:
	PgEventQuestUser();
	~PgEventQuestUser();

	bool CheckUser(BM::GUID const& rkGuid) const;
	void AddUser(BM::GUID const& rkGuid);
	void DelUser(BM::GUID const& rkGuid);
	void Clear();

private:
	ContGuidSet m_kUser;
};

//
class PgEventQuestSystemReward
{
public:
	PgEventQuestSystemReward(BM::GUID const& rkBaseGuid);
	~PgEventQuestSystemReward();

	void AddUser(ContGuidSet const& rkSet);
	void AddUser(BM::GUID const& rkGuid);
	bool CheckUser(BM::GUID const& rkGuid);

	// Reward
	void GetUserList(bool const bForReward);
	void TruncateUserList(BM::PgPackedTime const& rkStartTime);
	void GiveReward(PgEventQuest const& rkEventQuest);

private:
	BM::GUID const m_kBaseGuid;
	ContGuidSet m_kRewardUserList;
};


//
enum EEvnetQuestStatus
{
	EQS_None	= 0,
	EQS_Ing		= 1,
	EQS_End		= 2,
	EQS_Disable	= -1,
};

//
class PgEventQuestStauts
{
public:
	PgEventQuestStauts(BM::GUID const& rkBaseGuid);
	~PgEventQuestStauts();

	// Status
	//bool Init();
	bool Start();
	bool End();

	void Enable();
	void Disable();

	bool IsEnable() const;
	bool IsNowEvent() const;

protected:
	void Update(EEvnetQuestStatus const eUpdate);

private:
	BM::GUID const m_kBaseGuid;
	CLASS_DECLARATION_S_NO_SET(EEvnetQuestStatus, Status)
};

//
class PgEventQuestNotice
{
public:
	PgEventQuestNotice();
	~PgEventQuestNotice();

	void Tick();
	void ReadFromPacket(BM::PgPackedTime const& rkStartTime, BM::Stream& rkPacket);

protected:
	void NfyNotice(std::wstring const& rkNotice);
	__int64 InvertSign(__int64 const iValue);

private:
	ContEventQuestNotice m_kNotice;
	BM::PgPackedTime m_kStartTime;
};


//
class PgEventQuestContents : public PgEventQuest, public PgEventQuestStauts
{
public:
	PgEventQuestContents(BM::GUID const& rkGuid);
	~PgEventQuestContents();

	void ReadFromPacket(BM::Stream& rkPacket, bool const bNoInitStartTime);
};

//
class PgEventQuestSystem
{
public:
	PgEventQuestSystem();
	~PgEventQuestSystem();

	void OnTick();
	bool ProcessMsg(BM::Stream& rkPacket);

protected:
	void NfyAllMapServer(bool const bClearInfo);
	void NfyUser(SContentsUser const& rkUser, PgEventQuest const& rkEvent);

private:
	mutable Loki::Mutex m_kMutex;
	BM::GUID m_kBaseGuid;
	PgEventQuestContents m_kEvent;
	PgEventQuestNotice m_kNotice;
	PgEventQuestSystemReward m_kReward;
	PgEventQuestUser m_kUser;
};
//#define g_kEventQuestSys SINGLETON_STATIC(PgEventQuestSystem)

namespace PgEventQuestSystemUtil
{
	enum ELoadDataType
	{
		LDT_EVENT_QUEST,
		LDT_COMPLETESTATUS_LIST,
	};

	bool Q_DQT_LOAD_EVENT_QUEST(CEL::DB_RESULT &rkResult);
	bool Q_DQT_LOAD_EVENT_QUEST_COMPLETESTATUS(CEL::DB_RESULT &rkResult);
	bool Q_DQT_INIT_EVENT_QUEST_STATUS(CEL::DB_RESULT &rkResult);
	bool Q_DQT_EVENT_QUEST_COMMON(CEL::DB_RESULT &rkResult);
}

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGEVENTQUESTSYSTEM_H