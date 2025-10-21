#ifndef WEAPON_VARIANT_BASIC_CONSTANT_CONSTANT_AI_H
#define WEAPON_VARIANT_BASIC_CONSTANT_CONSTANT_AI_H

typedef enum 
{
	EAI_ACTION_NONE = 0,
	EAI_ACTION_OPENING = 1,
	EAI_ACTION_IDLE = 2,
	EAI_ACTION_PATROL = 3,
	EAI_ACTION_CHASEENEMY = 4,	// Target을 따라가는 상태
	EAI_ACTION_FIRESKILL = 5,	// Skill사용 상태
	EAI_ACTION_FIRESKILL_DELAY = 6,	// Skill 사용후 Delay 상태
	EAI_ACTION_RETREAT	= 7,	// 더이상 Target을 따라갈 수 없는 상황이라 뒤로 돌아가는 상태
	EAI_ACTION_DAMAGEDELAY = 8,
	EAI_ACTION_BLOWUP = 9,
	EAI_ACTION_CALLHELP = 10,
	EAI_ACTION_HIDE = 11,	// AI가 숨었다.
	EAI_ACTION_WAIT_COOLTIME = 12,
	EAI_ACTION_DELAY = 13,
	EAI_ACTION_AROUND = 14,
	EAI_ACTION_KEEP_DISTANCE = 15,
    EAI_ACTION_ROAD = 16, // 길 따라가는 상태
	EAI_ACTION_FOLLOW_CALLER = 17, // Caller를 따라다니는 상태
	EAI_ACTION_MAX
} EAIActionType;

typedef enum
{
	// Default Chase Pattern
	ECPattern_MoonWalk = 0,			// 뒤로 돌아 가기 위해 Target으로 부터 멀어져서 뒤로 가능 상태
	ECPattern_Y_FAR_MOVE = 1,		// 뒤로 돌아 가기 위해 Y축 수평 이동하는 상태
	ECPattern_X_GOBACK = 2,			// 뒤로 돌아 가고 있는 상태 (X축 수평이동)
	ECPattern_Y_GOBACK = 3,			// 뒤로 와서 X축을 Target과 나란하게 맞추는 중(Y축 수평이동)
	ECPattern_APPROACH_TARGET = 4,	// Target에게 다가가는 상태 (X축 수평이동)

	// Prowl Chase Pattern
	ECPattern_ProwlInit = 10,		// Prowl 초기 상태
	ECPattern_AccessProwlArea = 11,	// ProwlArea 영역으로 이동중
	ECPattern_InProwlArea = 12,		// ProwlArea 영역으로 들어왔음.
	ECPattern_TakeProwl = 13,		// 새로운 배회 위치를 잡아서 이동
	ECPattern_GotoTarget_X = 14,	// Target을 공격하기 위해 수직이동 중(Y축이동)
	ECPattern_GotoTarget = 15,		// Target을 공격하기 위해 TargetPos로 이동중
	ECPattern_BattleIdle = 16,		// Prowl 중에 잠시 쉬고 있는 중
	ECPattern_KeepDistance = 17,	// 타겟과의 거리를 유지함.
	ECPattern_Ended = 99,
} EChasePattern;

typedef enum
{
	EFPTN_NONE		= 0,
	EFPTN_BACK		= 0,
	EFPTN_FRONT		= 1,
} EFollowPattern;

typedef enum
{
	EAI_EVENT_NONE = 0,
	EAI_EVENT_CANNOTMOVE_FORWORD	= 1,	// 더이상 앞으로 전진할 수 없는 상태
	EAI_EVENT_DAMAGED = 2,		// 얻어 맞았다.
	EAI_EVENT_BLOWUP = 3,		// Blowup 되었다
	EAI_EVENT_CALLED_HELP = 4,	// 다른AI로 부터 도움 요청 받았다.
	EAI_EVENT_CHASE = 5,		// 현재 EventCaster를 Target으로 잡아야 한다.
	EAI_EVENT_DIE_SKILL = 6,	//죽을때 쓰는 스킬을 쓰자
	EAI_EVENT_FORCE_SKILL = 7,	//강제로 스킬을 사용하도록
	EAI_EVENT_RETURN_WAYPOINT = 8,	//강제로 가까운 웨이포인트로 돌아가도록 하자
	EAI_EVENT_IDLE = 9,			//강제로 아이들 상태로 바꾸자.
} EAIEventType;

typedef enum
{
	EAI_EVENT_PARAM_NONE							= 0,
	EAI_EVENT_PARAM_CHASE_UNCONDITIONALLY			= 0x0001,	//AI CHASE 발동시 실패하지 않음
	EAI_EVENT_PARAM_CHASE_NO_DAMAGEACTION			= 0x0002,
	EAI_EVENT_PARAM_CHASE_GOAL_POS					= 0x0004,	//AI Chase OnEnter시 GoalPos값을 사용하도록 함
} EAIEventParamType;

typedef enum
{
	EAI_CHOOSE_SKILL_WEIGHT		= 1,	// 가중치를 두고 가중치 높은 것부터 검사하는 방식
	EAI_CHOOSE_SKILL_RANDOM		= 2,	// Random(각 Skill마다 가중치를 가지고 Random
	EAI_CHOOSE_SKILL_FREQUENCY	= 3,	// 사용 빈도를 맞추어 사용하는 방식	
	EAI_CHOOSE_SKILL_SEQUENSE	= 4,	// 스킬이 순서대로 사용됨.
} EAIChooseSkillType;

typedef enum
{
	EAI_Func_FindEnemy = 0,
	EAI_Func_Max = 1,
} EAI_Function_Type;

typedef struct tagSAiFunctionCall_Info
{
	tagSAiFunctionCall_Info()
	{
		dwLastCallTime = 0;
	}

	DWORD dwLastCallTime;
	BM::Stream kResult;
}SAiFunctionCall_Info;

typedef std::map<EAIActionType, int> CONT_AI_ACTION_TYPE;
typedef std::vector<SAiFunctionCall_Info> VECTOR_FUNCTIONCALL_CACHE;	// AI 관련 Function이 너무 자주 호출되지 않도록 하기 위해 호출된 시간 기록

typedef struct tagSUnit_AI_Info
{
	tagSUnit_AI_Info()
	{
		sPattern = 0;
		eCurrentAction = EAI_ACTION_NONE;
		eEvent = EAI_EVENT_NONE;
		eEventParam = EAI_EVENT_PARAM_NONE;
	}
	short int sPattern;
	CONT_AI_ACTION_TYPE kActionType;

	EAIActionType eCurrentAction;
	EAIEventType eEvent;
	BM::GUID kEventCaster;
	EAIEventParamType eEventParam;

	VECTOR_FUNCTIONCALL_CACHE kFunctionTime;

	void Clear()
	{
		sPattern = 0;
		kActionType.clear();
		eCurrentAction = EAI_ACTION_NONE;
		ClearEvent();
		kFunctionTime.resize(EAI_Func_Max);
		VECTOR_FUNCTIONCALL_CACHE::iterator itor_func = kFunctionTime.begin();
		while (itor_func != kFunctionTime.end())
		{
			SAiFunctionCall_Info& rkInfo = *itor_func;
			rkInfo.dwLastCallTime = 0;
			rkInfo.kResult.Reset();
			++itor_func;
		}
	}
	void SetPattern(short int sValue) { sPattern = sValue; }
	short int GetPattern() { return sPattern; }
	void AddActionType(EAIActionType eAction, int iType)
	{
		CONT_AI_ACTION_TYPE::iterator itor = kActionType.find(eAction);
		if (itor != kActionType.end())
		{
			itor->second = iType;
		}
		else
		{
			kActionType.insert(std::make_pair(eAction, iType));
		}
	}
	int GetActionType(const EAIActionType eAction) const
	{
		CONT_AI_ACTION_TYPE::const_iterator itor = kActionType.find(eAction);
		if (itor != kActionType.end())
		{
			return itor->second;
		}
		return 0;
	}
	
	void SetCurrentAction(EAIActionType eNewAction)
	{
		eCurrentAction = eNewAction;
	}

	void SetEvent(BM::GUID const &rkCaster, EAIEventType const eNewType, int const iParam=0)
	{
		if(EAI_EVENT_DIE_SKILL == eEvent)
		{
			return;
		}

		kEventCaster = rkCaster;
		eEvent = eNewType;
		eEventParam = static_cast<EAIEventParamType>(iParam);
	}

	void ClearEvent()
	{
		kEventCaster.Clear();
		eEvent = EAI_EVENT_NONE;
		eEventParam = EAI_EVENT_PARAM_NONE;
	}

	EAIEventType GetEvent()const{ return eEvent; }
	const BM::GUID& GetEventCaster() const { return kEventCaster; }
	EAIEventParamType GetEventParam() const { return eEventParam; }
	bool IsEventParam(int const iParam) const { return eEventParam&iParam; }
	DWORD GetFunctionCallTime(EAI_Function_Type const eType) const
	{
		return kFunctionTime.at((int)eType).dwLastCallTime;
	}
	void SetFunctionCallInfo(EAI_Function_Type const eType, BM::Stream const &rkResult)
	{
		SAiFunctionCall_Info& rkInfo = kFunctionTime.at((int)eType);
		rkInfo.dwLastCallTime = BM::GetTime32();
		rkInfo.kResult.Reset();
		rkInfo.kResult.Push(rkResult);
	}

	BM::Stream& GetFunctionCallResult(EAI_Function_Type const eType)
	{
		SAiFunctionCall_Info& rkInfo = kFunctionTime.at((int)eType);
		rkInfo.kResult.PosAdjust();
		return rkInfo.kResult;
	}
	//PgAIAction* SetCurrentAction(PgAIAction* pkNewAction)
	//{
	//	PgAIAction* pkOld = pkCurrent;
	//	pkCurrent = pkNewAction;
	//	return pkOld;
	//} 
}SUnit_AI_Info;

// AI에서 GoalPos에 도착했는지 검사 할때, 도착판정 최대거리의 제곱
float const AI_GOALPOS_ARRIVE_DISTANCE_Q =		100.0f;
int const AI_MONSTER_MIN_DISTANCE_FROM_TARGET =	20;//45
int const AI_MONSTER_MIN_DISTANCE_CHASE_Q =		50;
float const AI_Z_LIMIT =						30.0f;
float const AI_BLOWUP_LEN =						160.0f;//블로우업 되었을때 이 거리 이상이면 해킹으로 판별하자
POINT3 const UP_VECTOR(0.0f,0.0f,1.0f);
POINT3BY const UP_VECTORBY(0,0,1);
int const AI_BACKDIR_FOR_TARGETTING	= 10;//캐릭터가 몬스터를 타겟팅 할 때 약간 뒤로 뺌

#endif // WEAPON_VARIANT_BASIC_CONSTANT_CONSTANT_AI_H