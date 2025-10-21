#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGOXQUIZEVENT_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGOXQUIZEVENT_H

#include <map>

typedef enum eOXQuizState
{
	OXQS_NONE				= 0,
	OXQS_QUIZ_NOTI_01		= 1,
	OXQS_QUIZ_NOTI_02		= 2,
	OXQS_QUIZ_NOTI_03		= 3,
	OXQS_QUIZ_NOTI_04		= 4,
	OXQS_QUIZ_NOTI_05		= 5,
	OXQS_QUIZ_NOTI_06		= 6,
	OXQS_QUIZ_NOTI_07		= 7,
	OXQS_QUIZ_NOTI_08		= 8,
	OXQS_QUIZ_NOTI_09		= 9,
	OXQS_QUIZ_NOTI_10		= 10,
	OXQS_QUIZ_READY			= 11,
	OXQS_ENTER_PARTICIPANT	= 12,
	OXQS_CLOSE_ROOM			= 13,
	OXQS_QUIZ_START			= 14,
	OXQS_QUIZ_RESULT		= 15,
	OXQS_QUIZ_CLOSE			= 16,

	OXQS_QUIZ_RELOAD		= 200,// 이벤트 리로드
}EOXQuizState;

typedef enum eOXQuizResult
{
	OXQR_NONE = 0,	// 선택 하지 않음
	OXQR_O = 1,
	OXQR_X = 2,
}EOXQiuzResult;

typedef enum eOXEVENTSTATE
{
	OXES_NONE = 0,
	OXES_RUN = 1,
	OXES_END = 2,
}EOXEVENTSTATE;

class PgOXQuizEvent
{
public:
	typedef std::map<BM::GUID,EOXQiuzResult>	CONT_QUIZ_PARTICIPANT;
	typedef std::map<BM::GUID,SOXQUIZREWARDKEY>	CONT_REWARD_CHARACTER;
	typedef std::vector<BM::GUID>				CONT_VEC_GUID;
	typedef std::vector<SOXQUIZINFO>			CONT_LIST_QUIZ;

	PgOXQuizEvent();

	~PgOXQuizEvent(){}

	void Locked_Tick();
	bool Locked_RecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

private:

	void ClearEvent();
	void BroadCast(BM::Stream const & rkPacket);
	bool GetStartEvent(int const iEventID);
	bool BuildEvent(CONT_OXQUIZEVENTINFO::mapped_type const & kEventInfo);

	EOXQUIZ_EVENT_RESULT const OnRecvPT_M_I_REQ_OXQUIZ_ENTER(BM::GUID const & kCharGuid,BM::Stream * const pkPacket);
	EOXQUIZ_EVENT_RESULT const OnRecvPT_M_I_REQ_OXQUIZ_EXIT(BM::GUID const & kCharGuid);
	EOXQUIZ_EVENT_RESULT const OnRecvPT_M_I_REQ_OXQUIZ_ANS_QUIZ(BM::GUID const & kCharGuid,BM::Stream * const pkPacket);

	void ProcessRewardSet(CONT_VEC_GUID const & kVec);
	void ProcessEndEvent();

private:

	EOXQuizState			m_kQuizState;			// 퀴즈 진행 상태
	CONT_QUIZ_PARTICIPANT	m_kContParticipant;		// 퀴즈 참가자 리스트
	BM::GUID				m_kEventGuid;			// 이벤트 고유 아이디 매 이벤트 실행시 마다 새로 생성
	SOXQUIZEVENTINFO		m_kCurEvent;			// 이벤트 정보
	int						m_iQuizLevel;			// 현재 문제 출제 횟수
	EOXQiuzResult			m_kCurQuizResult;		// 현재 퀴즈 정답
	__int64					m_i64StateTime;			// 이벤트 각 진행 단계별 대기시간
	CONT_REWARD_CHARACTER	m_kRewardMgr;			// 보상 받을 대상 관리자
	CONT_LIST_QUIZ			m_kContQuiz;			// 이 퀘스트에서 사용될 퀴즈 리스트
	CONT_OXQUIZEVENTINFO	m_kCopyEvent;
	SOXQUIZINFO				m_kCurQuiz;
	BM::PgPackedTime		m_kLastTickDateTime;	// 날자 변화를 감지하기 위한
	mutable Loki::Mutex		m_kMutex;
};


//#define g_kOXQuizEvent SINGLETON_STATIC(PgOXQuizEvent)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGOXQUIZEVENT_H