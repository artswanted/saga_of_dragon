#ifndef WEAPON_VARIANT_PGEVENTVIEW_H
#define WEAPON_VARIANT_PGEVENTVIEW_H
/*
	아직 미구현
	Event Manager Build 하는데 필요한 정보
	1) When (주기적/한시적/영구적)
	2) Whom (UnitType / 특정Unit / UnitAbil을 보고 판단 / Ground )
	3) What (Abil/Value 로 정의)
	4) Trigger (Character생성/Map이동/Unit생성/Unit소멸 등)
	위의 정보들을 가지고 조합시키면 Event가 만들어짐.
*/

#include "ace/high_res_Timer.h"
//#include "BM\Packet.h"
#include "Lohengrin\GameTime.h"
#include "Lohengrin/DBTables.h"
#include "Variant/idobject.h"

#pragma pack(1)
int const MAX_EVENT_ARG = 3;

typedef struct tagTblEvent
{
	tagTblEvent()
	{
		iEventNo = 0;
		iEventType = 0;//E_USER_EVENT_TYPE

		iPeriod = 0;//밀리세크 단위.

		memset(aIntData, 0, sizeof(aIntData));
		memset(aBigintData, 0, sizeof(aBigintData));
	}
	int iEventNo;
	int iEventType;//E_USER_EVENT_TYPE

	std::wstring kTitle;
	std::wstring kDescription;

	BM::DBTIMESTAMP_EX timeStartDate;
	BM::DBTIMESTAMP_EX timeEndDate;
	int iPeriod;//밀리세크 단위.

	int aIntData[MAX_EVENT_ARG];
	__int64 aBigintData[MAX_EVENT_ARG];
	BM::GUID aGuidData[MAX_EVENT_ARG];
	std::wstring aStringData[MAX_EVENT_ARG];

	bool IsNull() const {return ((iEventNo == iEventType) == 0);} // 이벤트 번호와 이벤트 타입이 0 이면 널 데이터이다.

	bool operator == (const tagTblEvent &rhs)const
	{
		if(iEventNo != rhs.iEventNo){return false;}
		if(iEventType!= rhs.iEventType){return false;}//E_USER_EVENT_TYPE

		if(timeStartDate!= rhs.timeStartDate){return false;}
		if(timeEndDate!= rhs.timeEndDate){return false;}
		if(iPeriod!= rhs.iPeriod){return false;}//밀리세크 단위.

		for(int i = 0; MAX_EVENT_ARG > i;++i)
		{
			if(aIntData[i] != rhs.aIntData[i]){return false;}
			if(aBigintData[i] != rhs.aBigintData[i]){return false;}
			if(aGuidData[i] != rhs.aGuidData[i]){return false;}
			if(aStringData[i] != rhs.aStringData[i]){return false;}
		}

		return true;
	}

	bool operator != (const tagTblEvent &rhs)const
	{
		if(*this == rhs)
		{
			return false;
		}
		return true;
	}

	size_t min_size()const
	{
		return 
			sizeof(iEventNo)+
			sizeof(iEventType)+

			sizeof(size_t) + 
			sizeof(size_t) +

			sizeof(timeStartDate)+
			sizeof(timeEndDate)+
			sizeof(iPeriod)+

			(sizeof(int)*MAX_EVENT_ARG) + 
			(sizeof(__int64)*MAX_EVENT_ARG) +
			(sizeof(BM::GUID)*MAX_EVENT_ARG) +
			(sizeof(size_t)*MAX_EVENT_ARG);//string size_t
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{

		kPacket.Push(iEventNo);
		kPacket.Push(iEventType);//E_USER_EVENT_TYPE

		kPacket.Push(kTitle);
		kPacket.Push(kDescription);

		kPacket.Push(timeStartDate);
		kPacket.Push(timeEndDate);
		kPacket.Push(iPeriod);//밀리세크 단위.
		
		kPacket.Push(aIntData);
		kPacket.Push(aBigintData);
		kPacket.Push(aGuidData);
		
		for(int i = 0;MAX_EVENT_ARG > i ;++i)
		{
			kPacket.Push(aStringData[i]);
		}
	}
	
	void ReadFromPacket(BM::Stream &kPacket)
	{

		kPacket.Pop(iEventNo);
		kPacket.Pop(iEventType);//E_USER_EVENT_TYPE

		kPacket.Pop(kTitle);
		kPacket.Pop(kDescription);

		kPacket.Pop(timeStartDate);
		kPacket.Pop(timeEndDate);
		kPacket.Pop(iPeriod);//밀리세크 단위.
		
		kPacket.Pop(aIntData);
		kPacket.Pop(aBigintData);
		kPacket.Pop(aGuidData);
		
		for(int i = 0;MAX_EVENT_ARG > i ;++i)
		{
			kPacket.Pop(aStringData[i]);
		}
	}
}TBL_EVENT;
#pragma pack()

typedef std::vector<TBL_EVENT>	CONT_EVENT_LIST;

typedef enum eUserEventType//등록시의 타입.
{
	ET_AUTO_EVENT_MIN		= 1,// 자동으로 진행되는 이벤트 최소 타입

	ET_EXP_BONUS_RATE		= 1,// 경험치 보너스

	ET_MONEY_DROP_RATE		= 2,//
	ET_MONEY_GIVE_RATE		= 3,//

	ET_ITEM_DROP_RATE		= 4,// 아이템 드랍 확률

	ET_UNIT_EVENT			= 5,// 유닛 이벤트

	ET_MONSTER_MAX_HP_RATE	= 6,

	ET_CONNECT_TIME_RECORD	= 7,// 접속시간 기록 이벤트
//	ET_MONSTER_MAX_HP_RATE	= 6,

	ET_SUMMON_MONSTER		= 8,// 몬스터 소환 이벤트

	ET_CHANGE_LEVEL			= 9,// 캐릭터 레밸 변경 체크 

	ET_CHANGE_CLASS			= 10,// 전직 체크

	ET_NEW_CHARACTER		= 11,// 새로운 캐릭터

	ET_CONNECT_COUNT		= 12,// 접속 횟수

	ET_NOTIFY_MSG			= 100,// 공지 날리기

	ET_EVENT_ITEM_CONTAINER	= 104,// 이벤트용 아이템 드랍 컨테이너( RAND(몬백1,몬백2,맵백) + 이벤트 컨테이너 ) OR ( 몬컨테이너 + 이벤트 컨테이너 )

	ET_AUTO_EVENT_MAX		= 1000,// 자동 진행 이벤트 최대 타입

	ET_MANUAL_EVENT_MIN		= 1001,// 유저가 직접 참가 신청해야 동작하는 이벤트 최소 타입

	ET_PCROOMEVENT_COUPON	= 1001,// pc방 이벤트 COUPON 지급(아이템은 중복 지급을 막을수 없음)

	ET_PCROOMEVENT_EFFECT	= 1002,// PC방 이벤트 이펙트 지급

	ET_MANUAL_EVENT_MAX		= 2000,// 유저가 직접 참가 신청해야 동작하는 이벤트 최대 타입(현재까지는...)

}E_USER_EVENT_TYPE;

enum E_MONSTER_GEN_MODE
{
	MGM_NONE = 0,
	MGM_GEN,
	MGM_REMOVE
};

typedef struct tagNfyMonsterGen
{
	tagNfyMonsterGen()
	{
		iMonsterGonGroupNo = 0;
		kMonsterGenMode = MGM_NONE;
	}

	tagNfyMonsterGen(int const iGenGroupNo,E_MONSTER_GEN_MODE const kGenMode)
	{
		iMonsterGonGroupNo = iGenGroupNo;
		kMonsterGenMode = kGenMode;
	}

	bool const IsNull() const
	{
		if(!iMonsterGonGroupNo || MGM_NONE == kMonsterGenMode)
		{
			return true;
		}
		return false;
	}

	int					iMonsterGonGroupNo;
	E_MONSTER_GEN_MODE	kMonsterGenMode;
}NFY_MONSTERGEN;

typedef std::list<NFY_MONSTERGEN> CONT_NFY_MONSTERGEN;//몬스터 소환

typedef struct tagEventState
{
	tagEventState(const TBL_EVENT& kEvent)
		:	m_kTable(kEvent)
		,	bEventStarted(false)
	{
		timeLastRaise = ACE_OS::gettimeofday();
	}

	bool operator == (const TBL_EVENT& rhs)const
	{
		if(0 == ::memcmp( &m_kTable, &rhs, sizeof(rhs)))
		{
			return true;
		}
		return false;
	}

	bool operator != (const TBL_EVENT& rhs)const
	{
		if(*this == rhs)
		{
			return false;
		}
		return true;
	}
	
	bool CanEventPeriod()const
	{//주기 값이 없으면 에러.
		if(0 < m_kTable.iPeriod)
		{
			ACE_Time_Value timeNow = ACE_OS::gettimeofday();

			if(m_kTable.iPeriod < (timeNow - timeLastRaise).sec())
			{//주기 값을 지났다
				return true;
			}
		}
		return false;
	}

	bool IsCorrectTime(BM::DBTIMESTAMP_EX const &kNowTime)const
	{
		if(	!m_kTable.timeStartDate.IsNull()
		&&	!m_kTable.timeEndDate.IsNull() )
		{//시작값과 끝값 다 있다.
			if( m_kTable.timeStartDate < kNowTime
			&&  m_kTable.timeEndDate > kNowTime)
			{
				return true;
			}
		}
		
		if(m_kTable.timeStartDate.IsNull()//시작 시간 없는데 
		&& m_kTable.timeEndDate > kNowTime)//끝시간안됐다.
		{
			return true;
		}

		if(m_kTable.timeEndDate.IsNull()//끝 시간 없는데 
		&& m_kTable.timeStartDate < kNowTime)//시작시간 지남.
		{
			return true;
		}
		return false;
	}

	ACE_Time_Value timeLastRaise;//마지막 이벤트 발생시간
	
	const TBL_EVENT m_kTable;

	bool bEventStarted;

	NFY_MONSTERGEN kEventMonsterGen;
}EVENT_STATE;

typedef struct tagNfyEvent//공지관련 
{
	tagNfyEvent()
	{
		iNfyType = 0;
	}

	tagNfyEvent(int const iInNfyType, std::wstring const &kInMsg)
	{
		iNfyType = iInNfyType;
		kMsg = kInMsg;
	}

	int iNfyType;
	std::wstring kMsg;

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(iNfyType);
		kPacket.Push(kMsg);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iNfyType);
		kPacket.Pop(kMsg);
	}
}NFY_EVENT;

typedef std::map<TBL_KEY_INT, TBL_EVENT> CONT_TBL_EVENT;
typedef std::map<TBL_KEY_INT, EVENT_STATE> CONT_EVENT_STATE;//EventNo. Value
typedef std::list<NFY_EVENT> CONT_NFY_EVENT;//공지 이벤트용

class PgPlayer;

class PgAction_Callback
{
public:
	PgAction_Callback(){}
	virtual ~PgAction_Callback(){}
public:
	virtual void DoAction(CONT_EVENT_STATE::mapped_type &){}
	virtual void DoAction(CONT_EVENT_STATE::mapped_type &,PgPlayer *){};
};

class PgEventAbil
{
public:
	PgEventAbil(void);
	~PgEventAbil(void){}

	PgEventAbil& operator = ( PgEventAbil const & );

	int GetAbil( CAbilObject::DYN_ABIL::key_type const ) const;
	__int64 GetAbil64( CAbilObject::DYN_ABIL64::key_type const ) const;

	bool AddAbil( CAbilObject::DYN_ABIL::key_type const, CAbilObject::DYN_ABIL::mapped_type const, bool const );
	bool AddAbil64( CAbilObject::DYN_ABIL64::key_type const, CAbilObject::DYN_ABIL64::mapped_type const, bool const );

	bool SetAbil( CAbilObject::DYN_ABIL::key_type const, CAbilObject::DYN_ABIL::mapped_type const );
	bool SetAbil64( CAbilObject::DYN_ABIL64::key_type const, CAbilObject::DYN_ABIL64::mapped_type const );

	bool IsEmpty(void)const;
	void Clear(void);

private:
	CAbilObject			m_kAbil;//이벤트에 의한 어빌.

protected:
	mutable Loki::Mutex m_kMutex;
};

typedef struct tagSyncVariable
{
	tagSyncVariable()
	{
		iExpAdd_MaxExperienceRate = iExpAdd_AddedExpRate_Hunting = iMaxCharacterLevel = 0;
		bCashShopOpen = true;
		iReviveFeather_Login_Time = 0;
		iReviveFeather_LevelUp_Use = 0;
		iJobSkill_ExhaustionGap = iJobSkill_ExhaustionRestore = 0;
		iJobSkill_BlessRate = 0;
		iJobSkill_BlessRate_DurationSec = 0;
		iMaxStrategyFatigability = 0;
		iDecPerStageFatigability_1 = 0;
		iDecPerStageFatigability_2 = 0;
		iDecPerStageFatigability_3 = 0;
		iDecPerStageFatigability_4 = 0;
		fBonusExpRate = 0.0f;
		iFatigabilityDivValue = 0;
		SpecificRewardNotifyMessageInterval = 0;
		CommunityEventWaitTime = 0;
		CommunityEventReadyTime = 0;
	}

	tagSyncVariable(tagSyncVariable const& rhs)
	{
		*this = rhs;
	}

	tagSyncVariable const& operator=(tagSyncVariable const& rhs)
	{
		iExpAdd_MaxExperienceRate = rhs.iExpAdd_MaxExperienceRate;
		iExpAdd_AddedExpRate_Hunting = rhs.iExpAdd_AddedExpRate_Hunting;
		iMaxCharacterLevel = rhs.iMaxCharacterLevel;
		bCashShopOpen = rhs.bCashShopOpen;
		iReviveFeather_Login_Time = rhs.iReviveFeather_Login_Time;
		iReviveFeather_LevelUp_Use = rhs.iReviveFeather_LevelUp_Use;
		iJobSkill_ExhaustionGap = rhs.iJobSkill_ExhaustionGap;
		iJobSkill_ExhaustionRestore = rhs.iJobSkill_ExhaustionRestore;
		iJobSkill_BlessRate = rhs.iJobSkill_BlessRate;
		iJobSkill_BlessRate_DurationSec = rhs.iJobSkill_BlessRate_DurationSec;
		iMaxStrategyFatigability = rhs.iMaxStrategyFatigability;
		iDecPerStageFatigability_1 = rhs.iDecPerStageFatigability_1;
		iDecPerStageFatigability_2 = rhs.iDecPerStageFatigability_2;
		iDecPerStageFatigability_3 = rhs.iDecPerStageFatigability_3;
		iDecPerStageFatigability_4 = rhs.iDecPerStageFatigability_4;
		fBonusExpRate = rhs.fBonusExpRate;
		iFatigabilityDivValue = rhs.iFatigabilityDivValue;
		SpecificRewardNotifyMessageInterval = rhs.SpecificRewardNotifyMessageInterval;
		CommunityEventWaitTime = rhs.CommunityEventWaitTime;
		CommunityEventReadyTime = rhs.CommunityEventReadyTime;
		return (*this);
	}
	// 휴식 경험치 관련 값
	int iExpAdd_MaxExperienceRate;		//  얻을 수 있는 최대 휴식 경험치 값 (현재 레벨의 경험치에 대한 %값)
	int iExpAdd_AddedExpRate_Hunting;	// 휴식경험치에 의해 얻게 되는 사냥  경험치 증가 % (50% 증가이면 50 으로 입력)
	int iMaxCharacterLevel;

	// 캐시샵 OPEN/CLOSE 설정 값
	bool bCashShopOpen;

	int iReviveFeather_Login_Time;
	int iReviveFeather_LevelUp_Use;

	int iJobSkill_ExhaustionGap;
	int iJobSkill_ExhaustionRestore;
	int iJobSkill_BlessRate;
	int iJobSkill_BlessRate_DurationSec;

	int iMaxStrategyFatigability;		// 전략피로도 최대치.
	int iDecPerStageFatigability_1;		// 스테이지당 전략 피로도 감소치(혼자일때).
	int iDecPerStageFatigability_2;		// 스테이지당 전략 피로도 감소치(파티원 2명).
	int iDecPerStageFatigability_3;		// 스테이지당 전략 피로도 감소치(파티원 3명).
	int iDecPerStageFatigability_4;		// 스테이지당 전략 피로도 감소치(파티원 4명).
	float fBonusExpRate;				// 전략 피로도에 따른 추가 경험치 최소 비율.
	int	iFatigabilityDivValue;			// 전략 피로도에 따른 추가 경험치 계산 계수.

	int SpecificRewardNotifyMessageInterval;

	int CommunityEventWaitTime;		// 커뮤니티 이벤트 입장 가능 시간.
	int CommunityEventReadyTime;	// 커뮤니티 이벤트 준비 시간.

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(iExpAdd_MaxExperienceRate);
		rkPacket.Push(iExpAdd_AddedExpRate_Hunting);
		rkPacket.Push(iMaxCharacterLevel);
		rkPacket.Push(bCashShopOpen);
		rkPacket.Push(iReviveFeather_Login_Time);
		rkPacket.Push(iReviveFeather_LevelUp_Use);
		rkPacket.Push(iJobSkill_ExhaustionGap);
		rkPacket.Push(iJobSkill_ExhaustionRestore);
		rkPacket.Push(iJobSkill_BlessRate);
		rkPacket.Push(iJobSkill_BlessRate_DurationSec);
		rkPacket.Push(iMaxStrategyFatigability);
		rkPacket.Push(iDecPerStageFatigability_1);
		rkPacket.Push(iDecPerStageFatigability_2);
		rkPacket.Push(iDecPerStageFatigability_3);
		rkPacket.Push(iDecPerStageFatigability_4);
		rkPacket.Push(fBonusExpRate);
		rkPacket.Push(iFatigabilityDivValue);
		rkPacket.Push(SpecificRewardNotifyMessageInterval);
		rkPacket.Push(CommunityEventWaitTime);
		rkPacket.Push(CommunityEventReadyTime);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(iExpAdd_MaxExperienceRate);
		rkPacket.Pop(iExpAdd_AddedExpRate_Hunting);
		rkPacket.Pop(iMaxCharacterLevel);
		rkPacket.Pop(bCashShopOpen);
		rkPacket.Pop(iReviveFeather_Login_Time);
		rkPacket.Pop(iReviveFeather_LevelUp_Use);
		rkPacket.Pop(iJobSkill_ExhaustionGap);
		rkPacket.Pop(iJobSkill_ExhaustionRestore);
		rkPacket.Pop(iJobSkill_BlessRate);
		rkPacket.Pop(iJobSkill_BlessRate_DurationSec);
		rkPacket.Pop(iMaxStrategyFatigability);
		rkPacket.Pop(iDecPerStageFatigability_1);
		rkPacket.Pop(iDecPerStageFatigability_2);
		rkPacket.Pop(iDecPerStageFatigability_3);
		rkPacket.Pop(iDecPerStageFatigability_4);
		rkPacket.Pop(fBonusExpRate);
		rkPacket.Pop(iFatigabilityDivValue);
		rkPacket.Pop(SpecificRewardNotifyMessageInterval);
		rkPacket.Pop(CommunityEventWaitTime);
		rkPacket.Pop(CommunityEventReadyTime);
	}

} SSyncVariable;	// 서버간에 공유가 필요한 변수 (g_kVariableContainer update되면 그쪽으로 이전해야 할 값)

class PgEventView
	:	public PgEventAbil
{
public:
	PgEventView(void);
	~PgEventView(void);
	
	bool SetGameTime(LPSYSTEMTIME const pkLocalTime, LPSYSTEMTIME const pkGameTime);

	HRESULT ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(bool const bOnlyGameTime, BM::Stream& rkPacket)const;

	bool ReadConfigFile(LPCTSTR lpszFileName);

	bool GetLocalTime(SYSTEMTIME* pkSystemTime)const{return m_kGameTime.GetLocalTime(pkSystemTime);}
	bool GetGameTime(SYSTEMTIME* pkSystemTime)const{return m_kGameTime.GetGameTime(pkSystemTime);}
	bool GetLocalTime(BM::DBTIMESTAMP_EX& rkDBTime)const{return m_kGameTime.GetLocalTime(rkDBTime);}
	bool GetGameTime(BM::DBTIMESTAMP_EX& rkDBTime)const{return m_kGameTime.GetGameTime(rkDBTime);}
	void GetLocalTime(ACE_Time_Value& rkAceTime)const{return m_kGameTime.GetLocalTime(rkAceTime);}
	void GetGameTime(ACE_Time_Value& rkAceTime)const{return m_kGameTime.GetGameTime(rkAceTime);}

	__int64 GetLocalSecTime(__int64 const op=CGameTime::SECOND)const{return m_kGameTime.GetLocalSecTime(op);}
	__int64 GetLocalSecTimeInDay(__int64 const op)const{return m_kGameTime.GetLocalSecTimeInDay(op);}
	__int64 GetGameSecTime(__int64 const op=CGameTime::SECOND)const{return m_kGameTime.GetGameSecTime(op);}
	__int64 GetGameSecTimeInDay(__int64 const op)const{return m_kGameTime.GetGameSecTimeInDay(op);}

	DWORD GetServerElapsedTime(void)const;
	unsigned __int64 GetServerElapsedTime64(void)const;

	void WriteToPacketEventList(BM::Stream & kPacket)const;
protected:
	void SetGameTimeSpeed(float fSpeed);

private:
	CGameTime m_kGameTime;
	CLASS_DECLARATION_S(SSyncVariable, VariableCont);

///////////////////////////////////////////////////////////////////////////
// 신 버전
public:
	
	bool ProcessEvent();//notify등등 처리.
	bool ProcessEvent(E_USER_EVENT_TYPE const kEventType,PgPlayer * pkUnit);
	HRESULT ProcessManualEvent(int const iEventNo,PgPlayer * pkPlayer);
	void RecvSync(BM::Stream &kPacket);
	void GetContMonsterGenNfy(CONT_NFY_MONSTERGEN & kOutCont)const;

	void CallbackStart(PgAction_Callback * pkCallbackStart);
	void CallbackEnd(PgAction_Callback * pkCallbackEnd);
	void CallbackProcessEvent(PgAction_Callback * pkCallbackProcessEvent);

protected:
	
	void ProcessEvent_Sub(CONT_EVENT_STATE::mapped_type &element, BM::DBTIMESTAMP_EX const &kNowTime);
	void ProcessEventTerminate(CONT_EVENT_STATE::mapped_type &element);
	CONT_EVENT_STATE m_kContEventState;//결과적으로 이벤트 관련 값들에 대한것만 기록.

protected:

	PgAction_Callback * m_kCallbackStart;
	PgAction_Callback * m_kCallbackEnd;
	PgAction_Callback * m_kCallbackProcessEvent;

	typedef std::map<int,int> CONT_EVENT_KEY_TABLE;

	CONT_EVENT_KEY_TABLE m_kChangeLevel;
	CONT_EVENT_KEY_TABLE m_kChangeClass;
};

#define g_kEventView SINGLETON_STATIC(PgEventView)


#endif // WEAPON_VARIANT_PGEVENTVIEW_H