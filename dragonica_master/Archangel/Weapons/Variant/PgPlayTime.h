#include "BM/TWrapper.h"

#ifndef WEAPON_VARIANT_USERINFO_PGPLAYTIME_H
#define WEAPON_VARIANT_USERINFO_PGPLAYTIME_H

typedef enum eWallowDefendMode
{
	WDM_NONE			= 0x00,
	
	WDM_USEFLAG			= 0x01,//중독방지를 적용하느냐?
	WDM_VIEWCLOCKFLAG	= 0x02,//시계를 보여주느냐?
	WDM_CHILDFLAG		= 0x04,//미성년자
	WDM_GMFLAG			= 0x08,//GM
	WDM_TIMEOVER		= 0x10,//타임아웃
};

typedef enum ePlayerPlayTimeCondition
{
	PPTC_NONE				= 0x00,

	PPTC_NOT_TAKEITEM		= 0x01,//아이템 픽업 불가
	PPTC_NOT_QUESTFINISH	= 0x02,//퀘스트 완료 불가
};

typedef BYTE EWallowDefendMode;
typedef BYTE EPPTCondition;

class PgPlayer;

class PgPlayerPlayTime
{
	friend class PgPlayer;

public:
	PgPlayerPlayTime();
	virtual ~PgPlayerPlayTime();

	PgPlayerPlayTime& operator=( PgPlayerPlayTime const &rhs );

	void WriteToPacket_PlayTime(BM::Stream& rkPacket)const;
	void ReadFromPacket_PlayTime(BM::Stream& rkPacket);
	void WriteToPacket_PlayTimeSimple(BM::Stream& rkPacket)const;
	void ReadFromPacket_PlayTimeSimple(BM::Stream& rkPacket);

	void Clear();

	bool IsUse()const{return 0 != (WDM_USEFLAG&m_eDefendMode);}
	bool IsViewClock()const{return 0 != (WDM_VIEWCLOCKFLAG&m_eDefendMode);}
	bool IsChildCheck()const{return 0 != (WDM_CHILDFLAG&m_eDefendMode);}
	bool IsGmCheck()const{return 0 != (WDM_GMFLAG&m_eDefendMode);}
	bool IsTimeOverKick()const;

	EWallowDefendMode GetDefendMode()const{return m_eDefendMode;}
	void SetDefendMode(EWallowDefendMode eDefendMode){m_eDefendMode = eDefendMode;}
	void SetExpRate(int const iValue){m_iExpRate = iValue;}
	void SetMoneyRate(int const iValue){m_iMoneyRate = iValue;}
	void SetDropRate(int const iValue){m_iDropRate = iValue;}
	void SetEtcBoolean(EPPTCondition const kValue){m_eEtcBoolean = kValue;}
	int GetExpRate();
	int GetMoneyRate();
	int GetDropRate();
	EPPTCondition GetEtcBoolean();
	
	// 중독방지에서 사용하는..
	int GetAccConnSec_Member()const;
	int GetAccDisConnSec_Member()const{return m_iLastAccDisConnSec_Member;}
	__int64 GetThisConnSec_Character()const;
	__int64 GetLastSpecificReward() const;
	__int64 GetTotalConnSec_Character()const;
	__int64 GetSelectCharacterSec()const;
	__int64 GetLastSpecificRewardSec() const;
	void SetSelectCharacterSec(__int64 SelectCharacterSec);
	void SetLasSpecificRewardSec(__int64 LasSpecificRewardSec);
	__int64 GetTotalPlayTimeSec()const;
	template<typename T>
	void CalcExpRate(T& rkValue)const;
	template<typename T>
	void CalcMoneyRate(T& rkValue)const;
	bool IsTakeUpItem()const{return !(m_eEtcBoolean&PPTC_NOT_TAKEITEM);}//아이템을 집을 수 있느냐?
	bool IsQuestBegin()const;//새퀘스트를 받을 수 있느냐?
	bool IsQuestFinish()const{return !(m_eEtcBoolean&PPTC_NOT_QUESTFINISH);}//퀘스트를 완료할 수 있느냐?
	void SetPlayTime(int const iAccConnSec, int const iAccDisSec);	
	
	void SetAccTime(unsigned short const usAge, BM::DBTIMESTAMP_EX const &dtUserBirth, BYTE const bGmLevel, int& iAccConnSec, int& iAccDisSec, BM::PgPackedTime const& kLastLogout );
	void Update_PlayTime(DWORD const dwCurTime);
	
	static bool IsAdult(BM::DBTIMESTAMP_EX const &dtUserBirth);
protected:
	void SetTotalPlayTime_Character(__int64 const i64TotalPlayTimeSec){m_i64TotalPlayTimeSec =i64TotalPlayTimeSec;} 

protected:	
	__int64		m_i64TotalPlayTimeSec;			// 케릭터의 총 플레이 시간:DB
	int			m_iLastAccConnSec_Member;		// 이전 누적 접속 시간(초) : Member
	int			m_iLastAccDisConnSec_Member;	// 이전 누적 휴식 시간(초) : Member
	__int64		m_i64SelectCharacterSec;		// 케릭터를 선택하여 들어간 시간( LocalTime 초)
	__int64		m_i64LastSpecificRewardSec;		// 접속 시간 유지 이벤트로 아이템을 받은 마지막 시간.
	EWallowDefendMode	m_eDefendMode;
	int				m_iExpRate;
	int				m_iMoneyRate;
	int				m_iDropRate;	//드랍율
	EPPTCondition	m_eEtcBoolean;

protected:
	CLASS_DECLARATION_S(__int64, GroundEnterTimeSec);		// 캐릭터가 Ground에 들어온 시간(CGameTime::GetLocalSecTime())
};

template<typename T>
inline void PgPlayerPlayTime::CalcExpRate(T& rkValue)const
{
	if ( IsUse() )
	{
		float fTmp = rkValue * static_cast<float>(m_iExpRate)/100;
		if(0.f < fTmp && fTmp < 1.f)
		{
			rkValue = 1;
		}
		else
		{
			rkValue = static_cast<T>(fTmp);
		}
	}
}

template<typename T>
inline void PgPlayerPlayTime::CalcMoneyRate(T& rkValue)const
{
	if ( IsUse() )
	{
		float fTmp = rkValue * static_cast<float>(m_iMoneyRate)/100;
		if(0.f < fTmp && fTmp < 1.f)
		{
			rkValue = 1;
		}
		else
		{
			rkValue = static_cast<T>(fTmp);
		}
	}
}

//
//Character는 아직 구현되지 않은 기능
//
class PgDefPlayerPlayTimeImpl
{
public:
	typedef enum eDefPlayTimeApplyUser
	{
		EDPTAU_NONE			=0x00,
		EDPTAU_MEMBER		=0x00,//0이면 Member
		EDPTAU_CHARACTER	=0x01,//1이면 Character
		EDPTAU_CHILD		=0x02,//18세 미만
		EDPTAU_GMEXCEPT		=0x04,//GM 제외
	}EDefPlayTimeApplyUser;

	typedef enum eDefPlayTimeFlag
	{
		EDPTF_NONE				= 0x000,
		EDPTF_VIEWCLOCK			= 0x001,//시계 사용유무
		EDPTF_KICK				= 0x002,//타임아웃에 따른 강퇴유무
		EDPTF_LOGIN				= 0x004,//타임아웃후 로그인 가능유무
		EDPTF_SELECTCHARACTER	= 0x008,//타임아웃후 캐릭터 선택 가능유무
		EDPTF_VILLAGEMAP		= 0x010,//타임오버후 마을맵 진입 가능유무(필드맵 진입 불가시 마을로 이동)
		EDPTF_FILDMAP			= 0x020,//타임오버후 필드맵 진입 가능유무
		EDPTF_INDUN				= 0x030,//타임오버후 인던 진입 가능유무
		EDPTF_ACCTIME			= 0x080,//리셋시 플래이 타임 누적
		EDPTF_NOQUESTBEGIN		= 0x100,//퀘스트완료 상태에따라 새 퀘스트 수락 가능유무(0:가능,1:불가능)
	}EDefPlayTimeFlag;

	typedef enum eDefPlayTimeResetType
	{
		EDPTRT_NONE			= 0,
		EDPTRT_ACCDISCON	= 1,//누적 휴식후
		EDPTRT_MIN			= 2,//몇 분 주기(기준은 00:00+분)
		EDPTRT_DAY			= 3,//하루 주기
	}EDefPlayTimeResetType;

	typedef enum eDefPlayTimeSub_Type
	{
		EDPTS_T_NONE		= 0,
		EDPTS_T_CONTITION	= 1,//조건
		EDPTS_T_MESSAGE		= 2,//메시지
	}EDefPlayTimeSub_Type;

	typedef enum eDefPlayTimeSub_TypeSub
	{
		EDPTS_TS_NONE		= 0,
		EDPTS_TS_EXP		= 1,//경험치
		EDPTS_TS_MONEY		= 2,//돈
		EDPTS_TS_ITEM		= 3,//아이템 습득 유무
		EDPTS_TS_QUEST		= 4,//퀘스트 완료 유무
		EDPTS_TS_DROP		= 5,//드랍율
	}EDefPlayTimeSub_TypeSub;

	typedef struct tagSUBBODY
	{
		tagSUBBODY() : bType(0),iStart(0),iEnd(0),iValue(0) {}
		
		BYTE bType;
		int iStart;
		int iEnd;
		int iValue;

		bool operator < (tagSUBBODY const &rhs)const
		{
			if( 0!=rhs.iEnd && rhs.iStart >= rhs.iEnd) {return false;}

			if( iStart < rhs.iStart )	{return true;}
			if( iStart > rhs.iStart )	{return false;}

			if( iEnd < rhs.iEnd )	{return true;}
			if( iEnd > rhs.iEnd )	{return false;}
			return false;
		}
	}SSUBBODY;

	typedef struct tagSUBMSG
	{
		tagSUBMSG() : bType(0),iSec(0),iStyle(0),iNo(0) {}

		BYTE bType;
		int iSec;
		int iStyle;
		int iNo;

		void Clear()
		{
			bType = 0;
			iSec = 0;
			iStyle = 0;
			iNo = 0;
		}

		bool operator < (tagSUBMSG const &rhs)const
		{
			if( bType < rhs.bType )	{return true;}
			if( bType > rhs.bType )	{return false;}

			if( iSec < rhs.iSec )	{return true;}
			if( iSec > rhs.iSec )	{return false;}
			return false;
		}
	}SSUBMSG;

	typedef std::set<SSUBBODY> CONT_BODY;
	typedef std::set<SSUBMSG> CONT_MSG;

	PgDefPlayerPlayTimeImpl();
	~PgDefPlayerPlayTimeImpl();

	void Clear();
	void SetDef(SPLAYERPLAYTIMEINFO const & kDef);
	bool Build();
	bool IsUse() const;
	EDefPlayTimeResetType ResetType() const;
	int ResetValue1() const;
	BM::PgPackedTime const& ResetValue2() const;
	BM::PgPackedTime const& LastApplyTime() const;
	void LastApplyTime(BM::PgPackedTime const & kTime);
	bool IsEnable(EDefPlayTimeFlag kFlag) const;
	bool IsEnable(EDefPlayTimeApplyUser kFlag) const;
	int PlayTimeMin() const;
	int PlayTimeSec() const;
	int CalcRemainSec(int const iAccConSec, int const iAccDisSce) const;
	bool IsResetTime() const;
	BM::PgPackedTime const & GetNextResetTime() const;
	void SetNextResetTime(BM::PgPackedTime &kTime);
	void UpdateNextResetTime(BM::PgPackedTime const & kNowTime);
	void WriteToPacket(BM::Stream& kPacket)const;
	void ReadFromPacket(BM::Stream& rkPacket);

	int GetExpRate(int const iAccConnSec) const;
	int GetMoneyRate(int const iAccConnSec) const;
	int GetDropRate(int const iAccConnSec) const;
	bool IsTakeUpItem(int const iAccConnSec) const;
	bool IsQuestFinish(int const iAccConnSec) const;
	bool IsTimeOver(int const iAccConnSec) const;
	int GetNextMsgSec(int const iAccConnSec, SSUBMSG & kMsg) const;

private:
	SPLAYERPLAYTIMEINFO m_kDef;
	BM::PgPackedTime m_kNextResetTime;
	
	CONT_BODY m_ContExp;
	CONT_BODY m_ContMoney;
	CONT_BODY m_ContItem;
	CONT_BODY m_ContQuest;
	CONT_BODY m_ContDrop;
	CONT_MSG m_ContMsg;
};

class PgDefPlayerPlayTime : public TWrapper<PgDefPlayerPlayTimeImpl, ACE_RW_Thread_Mutex>
{
public:
	PgDefPlayerPlayTime();
	~PgDefPlayerPlayTime();

public:
	void WriteToPacket(BM::Stream& kPacket)const;
	void ReadFromPacket(BM::Stream& rkPacket);
	bool IsEnable(PgDefPlayerPlayTimeImpl::EDefPlayTimeFlag kFlag) const;
	bool IsEnable(PgDefPlayerPlayTimeImpl::EDefPlayTimeApplyUser kFlag) const;
	bool IsUse() const;
	PgDefPlayerPlayTimeImpl::EDefPlayTimeResetType ResetType() const;
	int ResetValue1() const;
	int GetExpRate(int const iAccConnSec) const;
	int GetMoneyRate(int const iAccConnSec) const;
	int GetDropRate(int const iAccConnSec) const;
	bool IsTakeUpItem(int const iAccConnSec) const;
	bool IsQuestFinish(int const iAccConnSec) const;
	bool IsTimeOver(int const iAccConnSec) const;
	int PlayTimeSec() const;
	void LastApplyTime(BM::PgPackedTime const & kTime);
	BM::PgPackedTime const& LastApplyTime() const;
	void SetDef(SPLAYERPLAYTIMEINFO const & kDef);
	bool Build();
	BM::PgPackedTime const & GetNextResetTime() const;
	void UpdateNextResetTime(BM::PgPackedTime const & kNowTime);
	int CalcRemainSec(int const iAccConSec, int const iAccDisSce) const;
	int PlayTimeMin() const;
	int GetNextMsgSec(int const iAccConnSec, PgDefPlayerPlayTimeImpl::SSUBMSG & kMsg) const;

private:
	// DO NOT USE THIS function
	PgDefPlayerPlayTime(PgDefPlayerPlayTime const& rhs);
	PgDefPlayerPlayTime const& operator=(PgDefPlayerPlayTime const& rhs);
};

#define g_kDefPlayTime SINGLETON_STATIC(PgDefPlayerPlayTime)

#endif // WEAPON_VARIANT_USERINFO_PGPLAYTIME_H