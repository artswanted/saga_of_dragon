#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGLUCKYSTAREVENT_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGLUCKYSTAREVENT_H

#include <map>

typedef enum eLuckyStarState
{
	LSS_NONE			= 0,
	LSS_READY			= 1,
	LSS_OPEN			= 2,
	LSS_RESULT			= 3,
	LSS_RESULT_CUSTOM	= 4,
	LSS_UPDATE_EVENT	= 5,

	LSS_RELOAD			= 200,// 이벤트 리로드
}ELuckyStarState;

typedef enum eLuckyStarResult
{
	LSR_NONE		= 0,	// 선택 하지 않음
	LSR_AQUARIUS	= 1,	// 물병자리
	LSR_PISCES		= 2,	// 물고기자리 
	LSR_ARIES		= 3,	// 양자리 
	LSR_TAURUS 		= 4,	// 황소자리 
	LSR_GEMINI 		= 5,	// 쌍둥이자리 
	LSR_CENCER 		= 6,	// 게자리 
	LSR_LEO 		= 7,	// 사자자리 
	LSR_VIRGO 		= 8,	// 처녀자리 
	LSR_LIBRA 		= 9,	// 천칭자리 
	LSR_SCORPIO 	= 10,	// 전갈자리 
	LSR_SAGITTARIUS = 11,	// 궁수자리 
	LSR_CAPRICORN 	= 12,	// 염소자리 
}ELuckyStarResult;

typedef enum eLuckyStarUserState
{
	LSUS_NONE		= 0,
	LSUS_WAIT		= 2,
	LSUS_INTO		= 3,
	LSUS_JOIN		= 4,
	LSUS_LOGOUT		= 5,
	LSUS_LOADDB		= 6,
}ELuckyStarUserState;

typedef enum eLuckyStarSendResultType
{
	LSSRT_NONE		   = 0,
	LSSRT_ALLUSER	   = 1,
	LSSRT_PARTICIPATION = 2,
}ELuckyStarSendResultType;

class PgLuckyStarEvent
{
public:
	typedef struct tagLuckyStarParticipation
	{
		BM::GUID kCharGuid;
		ELuckyStarUserState kState;
		ELuckyStarResult kSelectStar;
		int iLuckyStep;
		BM::PgPackedTime kJoinTime;		
		DWORD dwLogoutTime;
		bool bCostMoney;
		int iRewardCount;

		tagLuckyStarParticipation() 
		{
			Clear();
		}
		void Clear()
		{
			kState = LSUS_NONE;
			kSelectStar = LSR_NONE;
			iLuckyStep = 0;
			kJoinTime.Clear();
			dwLogoutTime = 0;
			bCostMoney = false;
			iRewardCount = 0;
		}
	}SLSP;
	typedef struct tagLuckyStarRewardMgr
	{
		int iStep;
		CONT_LUCKYSTARREWARD::mapped_type kContReward;

		tagLuckyStarRewardMgr() : iStep(0) {}
	}SLSRM;
	typedef std::map<BM::GUID,SLSP>					CONT_LUCKYSTAR_PARTICIPANT;
	typedef std::vector<SLSRM>						CONT_REWARD_CHARACTER;
	typedef std::vector<BM::GUID>					CONT_VEC_GUID;

	PgLuckyStarEvent();
	~PgLuckyStarEvent(){}

	void Locked_Tick(const DWORD dwCurTime);
	bool Locked_RecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType, BM::Stream * const pkPacket);

private:

	void ClearEvent();
	void LogoutUserCheck(DWORD dwCurTime);
	void SendResultCustomPacket(CONT_VEC_GUID const & kContUser, bool bResult) const;
	void UpdateJoinedUser(BM::GUID const & kMemberGuid, CONT_LUCKYSTAR_PARTICIPANT::mapped_type const & kMember, BYTE bState);
	void UpdateEventSub(int iType, int iValue);
	void BroadCast(BM::Stream const & rkPacket);	

	ELUCKYSTAR_EVENT_RESULT const OnRecvPT_M_I_REQ_LUCKYSTAR_ENTER(BM::GUID const & kCharGuid,BM::Stream * const pkPacket);
	ELUCKYSTAR_EVENT_RESULT const OnRecvPT_M_I_REQ_LUCKYSTAR_CHANGE_STAR(BM::GUID const & kCharGuid,BM::Stream * const pkPacket, ELuckyStarResult & kSelectStar);

	bool GetStartEvent(int const iEventID);
	bool BuildEvent(CONT_LUCKYSTAREVENTINFO::mapped_type const & kEventInfo);
	void ProcessReward(CONT_VEC_GUID const & kVec);
	void ProcessEndEvent();
	CONT_LUCKYSTARREWARD::mapped_type GetCurrentReward(int const iStep);

    void UpdateLastEvent();
    bool IsNewEvent()const;

private:
	bool				m_bLuckyStarEventRun;		//
	bool				m_bLuckyStarJoinedUserDBRead;	
	ELuckyStarState		m_kLuckyStarState;			// '행운의 별자리를 찾아서' 진행 상태
	ELuckyStarSendResultType m_kSendResultType;		// 결과 전송 대상타입

	CONT_LUCKYSTAREVENTINFO			m_kCopyEvent;
	CONT_LUCKYSTAR_PARTICIPANT		m_kContParticipant;		// 참가자 리스트
	CONT_REWARD_CHARACTER			m_kRewardMgr;			// 보상 받을 대상 관리자

	BM::GUID				m_kEventGuid;			// 이벤트 고유 아이디 매 이벤트 실행시 마다 새로 생성
	SLUCKYSTAREVENTINFO		m_kCurEvent;			// 이벤트 정보
	DWORD					m_dwNextEventTime;		//
	DWORD					m_dwOldEventTime;	
	
	ELuckyStarResult		m_kBeforResultStar;
	ELuckyStarResult		m_kNowResultStar;
	CONT_VEC_GUID			m_kContLuckyUser;
	CONT_VEC_GUID			m_kContNomalUser;

	mutable Loki::Mutex		m_kMutex;
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGLUCKYSTAREVENT_H