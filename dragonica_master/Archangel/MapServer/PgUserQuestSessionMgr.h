#ifndef MAP_MAPSERVER_ACTION_QUEST_PGUSERQUESTSESSIONMGR_H
#define MAP_MAPSERVER_ACTION_QUEST_PGUSERQUESTSESSIONMGR_H

//
struct SUserQusetSessionInfo
{
	SUserQusetSessionInfo();
	SUserQusetSessionInfo(SUserQusetSessionInfo const& rhs);

	bool operator ==(SUserQusetSessionInfo const& rhs);

	static SUserQusetSessionInfo CreateNewSession(int const iCurDialog);

	BM::GUID kSessionGuid;
	int iCurDialogID;
	int iNextDialogID;
};
typedef std::map< BM::GUID, SUserQusetSessionInfo > ContUserQuestSessionInfo; // < Pre, Next >

//
class PgUserQuestSession
{
public:
	PgUserQuestSession();
	PgUserQuestSession(PgUserQuestSession const& rhs);
	~PgUserQuestSession();

	void Clear();
	bool IsEmpty();

	// 처음 대화 요청시 정보를 기록하고 SessionGuid를 생성한다.
	// 다음 퀘스트 다이얼로그로 이동시에 SessionGuid의 정보를 기준으로 보내온 정보를 검사하여 진행할지 말지를 결정하고 진행을 기록한다.(중복 체크 등).
	// 다음 퀘스트 대화 정보를 보낼 때에 새로운 SessionGuid를 생성하고, 만약 닫기(다이얼로글 찾을 수 없다면, Session정보 모두를 초기화 시킨다)
	void NewTalk(BM::GUID const& rkNpcGuid, int const iQuestID, int const iDialogID, BM::GUID& rkSessionGuid);
	bool SummitNextTalk(BM::GUID const& rkNpcGuid, BM::GUID const& rkSessionGuid, int const iQuestID, int const iDialogID, int const iNextDialogID);

private:
	mutable Loki::Mutex m_kMutex;
	BM::GUID m_kNpcGuid;
	int m_iQuestID;
	ContUserQuestSessionInfo m_kUserQuest;
};

//
class PgUserQuestSessionMgr
{
	typedef std::map< BM::GUID, PgUserQuestSession > ContSession;

public:
	PgUserQuestSessionMgr();
	virtual ~PgUserQuestSessionMgr();

	void AddQuestSession(BM::GUID const& rkCharGuid);
	void RemoveQuestSession(BM::GUID const& rkCharGuid);

	bool IsEmptyTalk(BM::GUID const& rkCharGuid);
	void ClearTalk(BM::GUID const& rkCharGuid);
	void NewTalk(BM::GUID const& rkCharGuid, BM::GUID const& rkNpcGuid, int const iQuestID, int const iDialogID, BM::GUID& rkSessionGuid);
	bool SummitNextTalk(BM::GUID const& rkCharGuid, BM::GUID const& rkNpcGuid, BM::GUID const& rkSessionGuid, int const iQuestID, int const iDialogID, int const iNextDialogID);
private:
	ContSession m_kSession;
};

#endif // MAP_MAPSERVER_ACTION_QUEST_PGUSERQUESTSESSIONMGR_H