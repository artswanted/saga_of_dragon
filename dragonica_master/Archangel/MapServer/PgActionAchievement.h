#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTIONACHIEVEMENT_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTIONACHIEVEMENT_H

class PgAddAchievementValue
	:	public PgUtilAction
{
public:

	explicit PgAddAchievementValue(int const iType,int const iValue,SGroundKey const &kGroundKey,bool const & bSyncDB = true):
	m_iType(iType),m_iValue(iValue),m_kGndKey(kGroundKey),m_kSyncDB(bSyncDB){}
	~PgAddAchievementValue() {};
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

protected:

	bool FindAchievementType(int const iType, CUnit * pkUnit);

	SGroundKey const & m_kGndKey;
	int const m_iType;
	int const m_iValue;
	bool const & m_kSyncDB;
private:
	PgAddAchievementValue();
};

class PgSetAchievementValue
	: public PgAddAchievementValue
{
public:
	explicit PgSetAchievementValue(int const iType,int const iValue,SGroundKey const &kGroundKey):PgAddAchievementValue(iType,iValue,kGroundKey){}
	~PgSetAchievementValue(){};
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);
};

typedef std::vector<TBL_DEF_ACHIEVEMENTS> CONT_ENABLE_COMPLETE_ACHIEVEMENTS;

class PgCheckComplete_Base
{
	PgCheckComplete_Base(){}
	~PgCheckComplete_Base(){}
public:
	static bool CheckComplete(int const iCheckLimit,int const iCheckValue)
	{
		return iCheckLimit <= iCheckValue;
	}
};

// PgCheckComplete_Base 반대
// Value 보다 작거나 같아야 업적 성공
class PgCheckComplete_Reverse
{
	PgCheckComplete_Reverse(){}
	~PgCheckComplete_Reverse(){}
public:
	static bool CheckComplete(int const iCheckLimit,int const iCheckValue)
	{
		return iCheckLimit >= iCheckValue;
	}
};

// CustomValue 체크용
class PgCheckComplete_CustomValue
{
	PgCheckComplete_CustomValue(){}
	~PgCheckComplete_CustomValue(){}
public:
	static bool CheckComplete( __int64 const i64DefCustomValue, __int64 const i64CustomValue )
	{
		return i64DefCustomValue == i64CustomValue;
	}
};

class PgCheckComplete_CrazyLevelup
{
	PgCheckComplete_CrazyLevelup(){}
	~PgCheckComplete_CrazyLevelup(){}
public:
	static bool CheckComplete(int const iCheckLimit,int const iCheckValue)
	{
		return iCheckLimit >= iCheckValue;
	}
};

class PgCheckComplete_Equality
{
	PgCheckComplete_Equality(){}
	~PgCheckComplete_Equality(){}
public:
	static bool CheckComplete(int const iCheckLimit,int const iCheckValue)
	{
		return iCheckLimit == iCheckValue;
	}
};

// 대표몬스터 번호 체크
class PgCheckComplete_DelegateMonster
{
	PgCheckComplete_DelegateMonster(){}
	~PgCheckComplete_DelegateMonster(){}
public:
	static bool CheckComplete(int const iCheckLimit, int const iCheckValue)
	{
		if((iCheckLimit % 100) == 0)
		{
			return iCheckLimit <= iCheckValue && iCheckValue <= iCheckLimit+99;
		}

		return iCheckLimit == iCheckValue;
	}
};

//====================================================================================================================================

class PgValueCtrl_Base
{
	PgValueCtrl_Base(){}
	~PgValueCtrl_Base(){}
public:
	static int GetValue(WORD const iType,CUnit const * pkUnit)
	{
		return pkUnit->GetAbil(iType);
	}
};

class PgValueCtrl_CtrlOfGod
{
	PgValueCtrl_CtrlOfGod(){}
	~PgValueCtrl_CtrlOfGod(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		if(NULL!=pkUnit)
		{
			int const iClass = pkUnit->GetAbil(AT_CLASS);
			if( IS_CLASS_LIMIT(UCLIMIT_COMMON_DOUBLE_FIGHTER, iClass) 
				|| IS_CLASS_LIMIT(UCLIMIT_COMMON_SHAMAN, iClass) )
			{
				return 1;//용족일 경우 죽은 횟수를 강제로 1로 돌려줌. 0일경우 업적달성 됨으로
			}
		}
		return pkUnit->GetAbil(AT_ACHIEVEMENT_ZOMBI);
	}
};

class PgValueCtrl_LongTimeNoSee
{
	PgValueCtrl_LongTimeNoSee(){}
	~PgValueCtrl_LongTimeNoSee(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		int const iLogOutTime = pkUnit->GetAbil(AT_ACHIEVEMENT_LONGTIMENOSEE);
		if(0 != iLogOutTime)
		{
			BM::PgPackedTime kLogoutTime;
			kLogoutTime.SetTime(iLogOutTime);

			__int64 i64LogoutTime = 0;
			CGameTime::DBTimeEx2SecTime(static_cast<BM::DBTIMESTAMP_EX>(kLogoutTime),i64LogoutTime);
			__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

			return static_cast<int>((i64CurTime - i64LogoutTime)/(24*60*60));
		}
		return 0;
	}
};

class PgValueCtrl_MyHomeHit
{
	PgValueCtrl_MyHomeHit(){}
	~PgValueCtrl_MyHomeHit(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		return pkUnit->GetAbil(AT_MYHOME_TOTALHIT);
	}
};

class PgValueCtrl_PlayTime
{
	PgValueCtrl_PlayTime(){}
	~PgValueCtrl_PlayTime(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		PgPlayer const * pkPlayer = dynamic_cast<PgPlayer const *>(pkUnit);
		if(!pkPlayer)
		{
			return 0;
		}
		return pkPlayer->GetTotalConnSec_Character() / 60i64;// 분단위 체크
	}
};

class PgValueCtrl_Age2Minute
{
	PgValueCtrl_Age2Minute(){}
	~PgValueCtrl_Age2Minute(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		PgPlayer const * pkPlayer = dynamic_cast<PgPlayer const *>(pkUnit);
		if(!pkPlayer)
		{
			return 0;
		}

		int const iClass = pkUnit->GetAbil(AT_CLASS);
		if( IS_CLASS_LIMIT(UCLIMIT_COMMON_DOUBLE_FIGHTER, iClass) 
			|| IS_CLASS_LIMIT(UCLIMIT_COMMON_SHAMAN, iClass) )
		{
			return 14400;//용족일 경우 필요값의 10배를 돌려줌. 1440이면 업적 달성됨으로
		}

		__int64 i64SecTime = 0;
		CGameTime::DBTimeEx2SecTime(static_cast<BM::DBTIMESTAMP_EX>(pkPlayer->BirthDate()), i64SecTime);

		__int64 const i64CurSecTime = g_kEventView.GetLocalSecTime();

		return (i64CurSecTime - i64SecTime)/60i64;// 분단위 체크
	}
};

class PgValueCtrl_ComboCount
{
	PgValueCtrl_ComboCount(){}
	~PgValueCtrl_ComboCount(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		return pkUnit->GetAbil(AT_COMBO_HIT_COUNT);
	}
};

class PgValueCtrl_CoupleTime
{
	PgValueCtrl_CoupleTime(){}
	~PgValueCtrl_CoupleTime(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		int const iStartTime = pkUnit->GetAbil(AT_ACHIEVEMENT_COUPLE_START_DATE);
		if(0 != iStartTime)
		{
			BM::PgPackedTime kStartTime;
			kStartTime.SetTime(iStartTime);

			__int64 i64CoupleTime = 0;
			CGameTime::DBTimeEx2SecTime(static_cast<BM::DBTIMESTAMP_EX>(kStartTime),i64CoupleTime);
			__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

			return static_cast<int>((i64CurTime - i64CoupleTime)/(24*60*60));
		}
		return 0;
	}
};

class PgValueCtrl_TexTime
{
	PgValueCtrl_TexTime(){}
	~PgValueCtrl_TexTime(){}
public:
	static int GetValue(WORD const,CUnit const * pkUnit)
	{
		return static_cast<int>(pkUnit->GetAbil(AT_ACHIEVEMENT_TEXTIME)/(24*60*60));
	}
};


template<typename T_Compare = PgCheckComplete_Base, typename T_ValueCtrl = PgValueCtrl_Base, typename T_CustomCtrl = PgCheckComplete_CustomValue >
class PgCheckAchievements
	:	public PgUtilAction
{
public:

	PgCheckAchievements(int const iType,SGroundKey const &kGroundKey):m_iType(iType),m_i64CustomValue(0i64),m_kGndKey(kGroundKey){}
	PgCheckAchievements(int const iType, __int64 i64CustomValue, SGroundKey const &kGroundKey):m_iType(iType),m_i64CustomValue(i64CustomValue),m_kGndKey(kGroundKey){}
	~PgCheckAchievements() {};
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

private:

	bool FindCompleteAchievement(CUnit * pkUser,int const iType,int const iValue, __int64 i64CustomValue, CONT_ENABLE_COMPLETE_ACHIEVEMENTS & kDefAchievement);

	int const			m_iType;
	SGroundKey const &	m_kGndKey;
	__int64 m_i64CustomValue;
};

class PgCheckAchievements_Attr5Element
	:	public PgUtilAction
{
public:
	PgCheckAchievements_Attr5Element(int const iType, bool const bAttack):m_iType(iType),m_bAttack(bAttack){}
	~PgCheckAchievements_Attr5Element() {};
	virtual bool DoAction(CUnit* pkUser, CUnit* pkTargetUnit);

private:
	bool CheckComplete(int const iValue, CUnit* pkUser)const;

	int const		m_iType;
	bool const		m_bAttack;
};

class PgAchievementToItem
	:	public PgUtilAction
{
public:

	PgAchievementToItem(int const iAchievementIdx, SGroundKey const &kGroundKey)
		:m_iAchievementIdx(iAchievementIdx),m_kGndKey(kGroundKey)
	{
	}

	~PgAchievementToItem() {};
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

private:

	E_ACHIEVEMENT_RESULT Process(CUnit* pUser);

	int const m_iAchievementIdx;
	SGroundKey const & m_kGndKey;
};

class PgItemToAchievement
	:	public PgUtilAction
{
public:

	PgItemToAchievement(PgBase_Item const & kItem, SItemPos const & kItemPos, SGroundKey const &kGroundKey)
		:m_kItem(kItem), m_kItemPos(kItemPos), m_kGndKey(kGroundKey)
	{
	}

	~PgItemToAchievement() {};
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

private:

	E_ACHIEVEMENT_RESULT Process(CUnit* pUser);

	PgBase_Item const & m_kItem;
	SItemPos const & m_kItemPos;
	SGroundKey const & m_kGndKey;
};

#include "PgActionAchievement.inl"

template<typename T_Compare = PgCheckComplete_Base, typename T_ValueCtrl = PgValueCtrl_Base, typename T_CustomCtrl = PgCheckComplete_CustomValue >
class PgSyncClinetAchievementHandler
	:	public PgUtilAction
{
public:
	
	explicit PgSyncClinetAchievementHandler(int const iAchievementType,int const iChangedAbilValue,SGroundKey const & kGndKey):
		m_iAchievementType(iAchievementType),
		m_iChangedAbilValue(iChangedAbilValue),
		m_kGndKey(kGndKey){}
	~PgSyncClinetAchievementHandler(){}

	bool DoAction(CUnit* pUser, CUnit* pkTargetUnit)
	{
		pUser->SetAbil(m_iAchievementType,m_iChangedAbilValue);
		pUser->SendAbil(static_cast<EAbilType>(m_iAchievementType), E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);//맵이동 중이라도 보내야.
		PgCheckAchievements<T_Compare,T_ValueCtrl,T_CustomCtrl> kCheckAchievements(m_iAchievementType, m_kGndKey );
		return kCheckAchievements.DoAction(pUser,NULL);
	}

private:

	int const			m_iAchievementType;
	int const			m_iChangedAbilValue;
	SGroundKey const &	m_kGndKey;
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTIONACHIEVEMENT_H