-----------------------------------------------
-- Ability Type
-----------------------------------------------
---- UNIT
AT_NONE=0

-- Unit Ability
AT_LEVEL	= 1
AT_CLASS	= 2
AT_GENDER	= 3
AT_RACE		= 4
AT_FIVE_ELEMENT_TYPE_AT_BODY = 5
AT_STATUS_BONUS			= 21	-- Status 보너스
AT_START_SET_NO			= 23	-- by reOiL

AT_HP					= 101
AT_DP					= 102
AT_CP					= 103
AT_MP					= 105
AT_REFRESH_ABIL_INV		= 106	-- Abil이 아니고 Abil이 다시 계산되어야 한다고 알리는 값으로 사용된다.(Inven 어빌을 기준으로)
AT_EVENT_LEVELUP		= 107	-- Abil아 아니고 Levelup이 되었으니 Abil을 다시 계산하라고 알리는 용도
AT_HIT_COUNT			= 108	-- 실제로 공격한 회수 (도적:DoubleAttack에 의해서 Hit수가 가변적으로 변경될 수가 있다.)
AT_HP_RESERVED			= 109	-- 부활할때의 채워지게 될 HP양
AT_MP_RESERVED			= 110	-- 부활할때의 채워지게 될 MP양
AT_REVIVED_BY_OTHER		= 111	-- 다른 사람에 의해 부활되었는가?

AT_ATTR_ATTACK			= 201
AT_ATTR_DEFENCE			= 202
AT_LOCK_HIDDEN_ATTACK	= 203	-- Hidden상태인데 Attack 하더라도 Hidden 상태가 풀리지 않는다.
AT_UNLOCK_HIDDEN_MOVE	= 204	-- Hidden상태인데 Move 하면 Hidden상태가 풀린다.
AT_GOALPOS_SET_TIME		= 205	-- AI : GoalPos() 값을 주기적으로 Update 시켜줘야 할 경우가 있는데 이것을 위해 GoalPos 변경된 시간(DWORD로 타입변경 필요)
AT_BOLDNESS_RATE		= 206	-- AI : 얼마나 대담한가..즉 Target을 발견했을 때 공격하려 갈 확률(기본 40%)
AT_PARENT_ITEM_NO		= 207	-- Item : Parent Item No (기능성 아이템에서 효과가 중복되지 않도록 하기 위해서 검사하는 ItemNo)
AT_DURATION_TIME		= 208	-- Item : 기능 아이템이 효과를 유지하는 시간
AT_SKILL_FILTER_TYPE	= 210	-- Effect : 스킬 사용을 제한 하기 위한 Filter type (ESkillFilterType)
AT_FILTER_EXCEPT_01		= 211	-- Effect : Skill Filter 에서 예외항목 리스트
-- 예약 : AT_FILTER_EXCEPT_01 - AT_FILTER_EXCEPT_10
AT_NEED_EFFECT_01		= 221	-- Skill : 스킬을 사용하기 위해서 필요한 Effect
-- 예약 : AT_NEED_EFFECT_01 - AT_NEED_EFFECT_10
AT_NEED_SKILL_01		= 231	-- Skill : 스킬을 배우기 위해 미리 배워야 하는 스킬ID
-- 예약 : AT_NEED_SKILL_01 - AT_NEED_SKILL_10
AT_PROJECTILE_SPEED_RATE	= 254	-- Client : 스킬발사체의 속도 증폭값(만분율, 10000 이상이면 속도 증가), 기본속도는 Client 발사체 XML에 있음.
AT_REST_EXP_ADD_MAX 	= 262 	-- 플레이어의 휴식 경험치

AT_TEAM					= 505
AT_DELAY				= 506
AT_AI_TYPE				= 507
AT_DISTANCE				= 508
AT_NOTAPPLY_MAPATTR		= 509	-- 사용 불가능한 그라운드 속성(ex 포탈아이템은 미션에서 사용 못해야 하니까...)
AT_EFFECT_SIZE			= 511
AT_EXPERIENCE			= 512
AT_MONEY				= 513
AT_GENERATE_STYLE		= 514
AT_DIE_MOTION			= 517	-- Monster Die Motion
AT_DAMAGE_PUSH_TYPE		= 518	-- Pushback by damage type
AT_DEATH_EXP_PENALTY	= 519	-- Player 죽었을 때 깍이는 경험치 (Percentage)
AT_LIFETIME				= 520	-- (DummyPlayer) Life time
AT_SKILL_EXTENDED		= 521	-- 스킬이 버프를 받아서 더욱 강력해진 상태(발사체 개수 변화)
AT_DMG_DELAY_TYPE		= 522	-- Damage 받았을 때 Delay값을 어디서 읽을 것인가? (공격Skill_Abil에 기록)(0=unit 1=skill 2=unit+skill)
AT_MAX_HP_EXPAND_RATE	= 523	-- Class별로 MaxHP 증가치(만분율)
AT_MAX_MP_EXPAND_RATE	= 524	-- Class별로 MaxMP 증가치(만분율)
AT_DICE_VALUE			= 525	-- 투사:Roulette 스킬 결과값(주사위값)
AT_REFLECTED_DAMAGE		= 526	-- Reflected Damage (Damage반사) 크기
AT_1ST_ATTACK_ADDED_RATE	= 527	-- 첫번째 공격에 대해서만 추가적인 공격치(최종공격력에 대한 만분율)
AT_INDEX				= 528	-- 그냥 Index값으로 쓰면 좋겠다.
AT_PDMG_DEC_RATE_2ND	= 529	-- 물리Damage에 대해서 2차적으로 Damage 감소 Rate (마나쉴드 사용)
AT_MDMG_DEC_RATE_2ND	= 530	-- 마법Damage에 대해서 2차적으로 Damage 감소 Rate (마나쉴드 사용)
AT_APPLY_ATTACK_SPEED	= 531	-- 스킬 : 이 스킬이 공격속도에 영향을 받을 것인가(0:안받는다 1:받는다)
AT_CLAW_DBL_ATTK_RATE	= 532	-- 스킬 : Claw 평타 왼속 공격시에 더블어택 발동 확률(만분율)
AT_ADD_MONEY_PER_LV		= 533	-- Monster Level에 따라 추가적인 추가지급 Money Rate량(만분율)
								-- Unit : 도적(소매치기) 스킬은 몬스터 잡을때 추가적인 Money를 지급해야 한다.
AT_ADD_MONEY_RATE		= 534	-- 추가적인 Money Drop이 발생할 확률
AT_MAP_NUM				= 535	-- 아이템 : 마을소환 스크롤일 경우 가야할 마을 Map번호
AT_PORTAL_NUM			= 536	-- 아이템 : 마을소환 스크롤일 경우 가야할 Portal 번호
AT_POSITION_X			= 537	-- 아이템 : 마을소환 스크롤일 경우 가야할 좌표값(X)
AT_POSITION_Y			= 538	-- 아이템 : 마을소환 스크롤일 경우 가야할 좌표값(Y)
AT_POSITION_Z			= 539	-- 아이템 : 마을소환 스크롤일 경우 가야할 좌표값(Z)
AT_PARENT_SKILL_NUM		= 540	-- Skill : Parent Skill number
								-- 사정거리 : Caster를 중심으로 하는 거리
								-- 효과거리 : Caster로 부터 AT_ATTACK_RANGE 떨어진 거리를 중심으로 한는거리
AT_1ST_AREA_TYPE		= 541	-- Skill : 사정거리 : Target을 잡을때 어떤 형태의 Target Area를 생성할 것인가?(ESkillArea)
AT_2ND_AREA_TYPE		= 542	-- Skill : 효과거리 : Target을 잡을때 어떤 형태의 Target Area를 생성할 것인가?(ESkillArea)
AT_1ST_AREA_PARAM1		= 543	-- Skill : 사정거리 : NOT USE
AT_2ND_AREA_PARAM1		= 544	-- Skill : 효과거리 : Range
AT_1ST_AREA_PARAM2		= 545	-- Skill : 사정거리 : 직육면체(폭)/구(반지름)/원뿔(각도)
AT_2ND_AREA_PARAM2		= 546	-- Skill : 효과거리 : 직육면체(폭)/구(반지름)/원뿔(각도)
AT_1ST_AREA_PARAM3		= 547	-- Skill : 사정거리 : 예약
AT_2ND_AREA_PARAM3		= 548	-- Skill : 효과거리 : 예약
AT_1ST_AREA_PARAM4		= 549	-- Skill : 사정거리 : 예약
AT_2ND_AREA_PARAM4		= 550	-- Skill : 효과거리 : 예약
AT_GLOBAL_COOLTIME		= 551	-- Skill : 스킬사용 후에는 어떤 스킬도 해당 시간동안 사용 금지.
AT_REFLECT_DMG_HP		= 552	-- Refeclted Damage 발생했을때 맞은넘의 남은 HP 값
AT_ABS_ADDED_DMG		= 553	-- Unit : 공격할 때 최종Dmg에 추가적으로 더해지는 Dmg
AT_DMG_DEC_RATE_COUNT	= 554	-- AT_PDMG_DEC_RATE_2ND or AT_MDMG_DEC_RATE_2ND 능력 적용 남은 개수(-1:Unlimited)
AT_AMOUNT				= 555	-- 량으로 사용
AT_COLLISION_SKILL		= 556	-- 몬스터 충돌 되었을 때 데미지 줄 스킬 번호(<>0 이면 Damage를 주어야 하며 Abil값의 Skill로 계산)
AT_TEMPLATE_SKILLNO		= 557	-- 스킬 : 스킬 함수 호출 될때 이 어빌값의 스킬함수를 호출하도록 해 준다.
AT_DEFAULT_DURATION		= 558	-- Item : 내구도 아이템 생성시에 초기설정되는 내구도값(0이면 AT_DEFAULT_AMOUNT 으로 세팅)
AT_DIE_ANI_PLAYTIME		= 559	-- Monster : 죽을때 죽는 Ani 플레이 시간
AT_FIRE_TYPE			= 561	-- Skill : Skill Fire 종류(언제 Damage를 줄 것인가 등의 정의) ESkillFireType
AT_FRAN					= 567

AT_CHILD_SKILL_NUM_01	= 580	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_02	= 581	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_03	= 582	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_04	= 583	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_05	= 584	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_06	= 585	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_07	= 586	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_08	= 587	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_09	= 588	-- 스킬 쓸때 영향 받는 자식 스킬들
AT_CHILD_SKILL_NUM_10	= 589	-- 스킬 쓸때 영향 받는 자식 스킬들

AT_SKILL_BACK_DISTANCE	= 591	-- 타겟을 잡을 때 뒤로 약간 빼 줄 거리

AT_AUTO_PET_SKILL		= 810	-- 펫 AI 토글을 위한 어빌
AT_MOUNTED_PET_SKILL	= 815	-- 펫에 탑승했는가
AT_TRANS_DC_PET_SKILL	= 816	-- 펫 순간이동 비용 감소 퍼센티지

AT_SP					= 1001
AT_HIDE					= 1002 -- 0: 캐릭터를 그림   1: 캐릭터를 그리지 않음(완전히 숨겨버림)
AT_EVENTSCRIPT_HIDE		= 1003 -- 이벤트 스크립트를 통한 연출에서 0: 캐릭터를 그림   1: 캐릭터를 그리지 않음(완전히 숨겨버림)
AT_ADD_EXP_PER			= 1004
AT_ADD_MONEY_PER		= 1005
AT_MAX_TARGETNUM		= 1007
AT_SKILLPOWER_RATE		= 1008	-- Skill의 공격력 Rate
AT_SKILL_MIN_RANGE		= 1009
AT_MON_MIN_RANGE		= 1010	-- 몬스터가 PC와 유지하는 최소 거리
AT_MAX_LOOTNUM			= 1016  -- 최대 줍기 갯수
AT_DUEL					= 1103  -- 유저가 결투중인가?
AT_DROPITEMGROUP		= 2008		-- Drop 하는 아이템 그룹 ID
AT_SKILL_ATT			= 2011	-- Skill 속성 (ESkillAttribute value bitmask value)
ATS_COOL_TIME			= 2012	-- Skill : Cooling Time (새로운 스킬을 사용할 수 있는 최소시간)
AT_MAINSKILL			= 2016	-- passive skill의 Main이 되는 active skill
AT_CASTTYPE				= 2018	-- ESkillCastType (Instant/Maintenance/CastShot)
AT_CAST_TIME			= 2019	-- 주문을 외우는데 필요한 시간
AT_ATT_DELAY			= 2020	-- 스킬쓰고 나서 Delay 시간
AT_TYPE					= 2022	-- Skill type
AT_WEAPON_LIMIT			= 2023	-- Weapon limit
AT_CLASSLIMIT			= 2024
AT_LEVELLIMIT			= 2025
AT_STATELIMIT			= 2026
AT_MAINTENANCE_TIME		= 2027	-- Maintenance skill일 경우 Maintenance time
AT_NEED_SP				= 2028
AT_MAINTENANCE_TIME2	= 2029	-- Skill : EFireType_TimeDelay 용 지연시간으로 사용. 값이 0이면 AT_MAINTENANCE_TIME으로 사용
AT_NAMENO				= 2030			-- TB_DefRes 참조값
--AT_ADDHEAL			= 2037	-- 응급치료 스킬 사용시 추가적인 HP 회복량
--AT_S101200701_PARAM1	= 2038 -- 스킬(101200701:찔러보기)에서 Critical 사용여부값
AT_ANIMATION_TIME		= 2039 -- 스킬의 애니메이션이 타격할 때까지의 시간
AT_BLOW_VALUE			= 2040	-- 뛰우는 힘 : Effect Abil
AT_DMG_MAX_UNIT			= 2041	-- Skill : Damage를 입는 Unit의 최대수
AT_DAMAGED_DELAY		= 2042 -- Monster/Skill : Damage를 입은 뒤 딜레이
AT_EFFECTNUM1			= 2051	-- 스킬에 추가적인 Effect Number #1
AT_EFFECTNUM2			= 2052	-- 스킬에 추가적인 Effect Number #2
AT_EFFECTNUM3			= 2053	-- 스킬에 추가적인 Effect Number #3
AT_EFFECTNUM4			= 2054	-- 스킬에 추가적인 Effect Number #4
AT_CUSTOMDATA1			= 2061	-- Customized data #1
AT_CUSTOMDATA2			= 2062	-- Customized data #2
AT_CUSTOMDATA3			= 2063	-- Customized data #3
AT_CUSTOMDATA4			= 2064	-- Customized data #4
AT_CUSTOMDATA5			= 2065	-- Customized data #5
AT_CUSTOMDATA6			= 2066	-- Customized data #6
AT_CUSTOMDATA7			= 2067	-- Customized data #7
AT_CUSTOMDATA8			= 2068	-- Customized data #8
AT_CUSTOMDATA9			= 2069	-- Customized data #9
AT_DROPITEMRES			= 2071	-- Drop Item Icon Resource Number (TB_DefRes)
AT_DAM_EFFECT_S			= 2072	-- Damage Motion Effect Num (Small Size)
AT_DAM_EFFECT_M			= 2073	-- Damage Motion Effect Num (Middle Size)
AT_DAM_EFFECT_L			= 2074	-- Damage Motion Effect Num (Large Size)
AT_PERCENTAGE			= 2075	-- Percentage value
AT_ADDVALUE				= 2076	-- 추가적인 값
AT_PERCENTAGE2			= 2077	-- Percentage Value #2
AT_MONSTER_SKILLTYPE	= 2078 -- 몬스터 스킬 Type
AT_SKILL_KIND			= 2079 -- Skill 분류값 (ESkillKind)
AT_PHY_DMG_PER			= 2080	-- Skill : Phyical Damage에 대한 Percentage 값
AT_MAGIC_DMG_PER		= 2081 -- Skill : Magic Damage에 대한 Percentage 값
AT_RANGE_TYPE			= 2082	-- Skill : Range를 어떤것을 사용할 것인가? (EAttackRangeSource)
AT_SPECIALABIL			= 2083	-- Skill or Effect : 부수적으로 생기는 특별한 능력 Ability Number (예:침묵하기)
AT_MP_BURN				= 2084	--
AT_GOD_TIME				= 2085	-- Effect : 스킬맞고 나서 Effect되면 무적되는 시간
AT_CASTER_MOVERANGE		= 2086	-- Skill : Caster가 전진하는 길이
AT_TARGET_MOVERANGE		= 2087	-- Skill : Target이 밀리는 길이
AT_DMG_CONSUME_MP		= 2088	-- Damage 받으면 HP대신 MP를 흡수하는 량(AT_C_MAGIC_DMG_DEC 값이 0이면 효과없음)
AT_COUNT				= 2089	-- 개수를 나타낼때 사용하면 좋겠다.(Skill:최대 몇번까지 때릴 수 있는가)
AT_ATTACK_UNIT_POS		= 2090	-- 스킬 : 공격할 수 있는 Target의 상태값(EAttackedUnitPos)(디폴트 0이 나오면 1로 처리)
AT_COMBO_HIT_COUNT		= 2091	-- 스킬 : 공격할 수 있는 스킬의 공격이 적중시 몇 콤보가 증가 되는 가
AT_SKILL_DIVIDE_TYPE	= 2092	-- 스킬 : 어떤 이유로 스킬을 가지게 되었는지 (기본, 길드, 등등)
AT_SKILL_SUMMONED_PARENT_SKILL		= 2150 --스킬 : 소환주가 소환체에게 지시한 스킬번호 (리게인/임팩트 언서먼용으로 사용)
-- Monster Ability
AT_WALK_TYPE			= 3001
AT_JOB					= 3002
AT_DETECT_RANGE			= 3003
AT_CHASE_RANGE			= 3004
AT_MAX_SKILL_NUM		= 3005
AT_NORMAL_SKILL_ID		= 3006 -- 디폴트 공격
AT_IDLE_TIME			= 3007	-- IDLE 유지 시간
AT_SKILL_CHOOSE_TYPE	= 3008 -- AI에서 Skill을 선택하는 방법
AT_MOVE_RANGE			= 3009
AT_MON_SKILL_01			= 3010	-- AT_MON_SKILL_01 - AT_MON_SKILL_10 (예약)
AT_MON_SKILL_RATE_01	= 3020
--	AT_MONSTER_TYPE			= 3031
AT_PATROL_MIN_TIME		= 3031	-- Monster Patrol Min time
AT_PATROL_MAX_TIME		= 3032	-- Monster Patrol Max time
--AT_NORMAL_SKILL_RATE	= 3032	-- 삭제 예정
AT_DIE_EXP				= 3033
AT_UNIT_SIZE			= 3034	-- 1:Small 2:Middle 3:Large-- 몬스터 크기에 따라 Damage받는 모습을 다르게 하기 위한 값
AT_WAYPOINT_INDEX		= 3035	-- 몬스터가 현재 가진 WayPoint의 index
AT_WAYPOINT_GROUP		= 3036 -- 몬스터가 현재 가진 WayPoint의 Group
AT_SUMMONCOMRADE_STATE	= 3037 -- 몬스터가 동료를 소환한 적이 있냐?
AT_SUMMON_COMRADE		= 3038 -- 이 몬스터는 소환된 놈이냐? 아니냐?
AT_DAMAGEACTION_TYPE	= 3039	-- Monster:Damaged Action type (0 = normal 1 = DamageAction without DamagedDelay 101 = no damaged delay(boss))
								-- Effect : 0(normal) 1(Knock down)
AT_IDLEACTION_TYPE		= 3040	-- Monster가 Idle 상태에서 앞을 보는것을 변경하고 싶을때 ( 0=앞을 본다 101 = 앞을 보지 않는다)
AT_ITEM_BAG_PRIMARY		= 3041	-- Monster Regen - Item Bag Primary
AT_ITEM_BAG_SECONDARY	= 3042	-- Monster Regen - Item Bag Secondary
AT_NOT_SEE_PLAYER		= 3043 -- 0 : 맞았을 때 플레이어를 본다! 101 : 맞아도 플레이어를 쳐다보지 않는다.
AT_MANUAL_DIEMOTION		= 3044 -- 0 : 일반적인 죽음 101 : 특수하게 죽음(WorldAction등)
AT_UNIT_SCALE			= 3045	-- 클라이언트에서 Unit을 만들때 사용하는 Scale 비율값(% 사용)
AT_WAYPOINT_INDEX_FROM	= 3046	-- WayPoint 이동경로 시작 Index
AT_RETREAT_MIN_TIME		= 3047	-- AI에서 도망가는 최소시간
AT_RETREAT_MAX_TIME		= 3048	-- AI에서 도망가는 최대시간
AT_RETREAT_MOVESPEED	= 3049	-- AI에서 도망가는 속도(보통속도에 대한 만분율)
AT_R_RETREAT_HP			= 3050	-- 도망가야하는 HP값
AT_CHASE_MOVESPEED		= 3051	-- AI에서 적을 ?는 속도(보통속도에 대한 만분율)
AT_HIDE_ANI_TIME		= 3052	-- AI : Hide하거나/나타날때 걸리는 Ani 시간
AT_MONSTER_TYPE			= 3053	-- EMonsterType(0:Normal 1:공중형)
AT_HEIGHT				= 3054	-- Monster : 공중형몬스터가 나는 높이
AT_MANUAL_OPENING		= 3055	-- 0 : 일반적인 오프닝 or 오프닝 스킬 번호

--AT_MAX_DROP_ITEM_COUNT	= 3055
AT_ITEM_BAG_SUCCESS_RATE_CONTROL= 3056

AT_CHASE_DONE			= 3057	-- 몬스터가 PC추적을 끝냈는가?
AT_CHASE_PATTERN		= 3058	-- 추적 패턴 ( 1 : 앞을 잡는다 그 외 : 뒤를 잡음)
AT_CHASE_ELAPSED_TIME	= 3059

AT_MON_SKILL_OPEN		= 3060	-- 몬스터의 오프닝 스킬
AT_MON_SKILL_DIE		= 3061 -- 몬스터가 죽는 스킬

AT_PENETRATION_COUNT	= 3067 -- 몇개를 관통할 것인가?
AT_DOUBLE_JUMP_USE		= 3068 -- 더블 점프 사용
AT_NOT_SEE_TARGET		= 3069	-- 0 : 타겟을 쳐다본다 101 : 타겟을 처다보지 않는다
AT_NOT_FLY_TO_CAMERA	= 3071	-- 죽었을 때 카메라로 날아오지 않는다. 클라이언트용

AT_SKILL_ON_DIE					= 3090	-- HP가 0일때 써야 할 스킬 번호
AT_DAMAGE_EFFECT_DURATION		= 3091	-- 몬스터의 데미지액션 시간 저장용
AT_DAMAGEACTION_RATE			= 3092	-- 몬스터 데미지 액션 할 확률. 10000분율. AT_DAMAGEACTION_TYPE(3039)어빌이 있어야 동작함
AT_AI_TARGETTING_TYPE			= 3093	-- AI가 어떤 타겟팅 타입을 가질지 결정하는 어빌
AT_AI_TARGETTING_SECOND_TYPE	= 3094	-- 타겟팅 타입중 Hi/Low를 결정
AT_AI_TARGETTING_THIRD_TYPE		= 3095	-- 타겟팅 세번재 옵션
AT_AI_TARGETTING_CHASE			= 3097	-- Chase할 때 사용할 타겟팅 옵션
AT_AI_TARGETTING_RATE			= 3100	-- 타겟팅을 할 확률. 만분률

AT_CLASS_PARTY_BUFF_01			= 3101	-- 클래스 별로 가지는 파티 버프 Def_Class에서 사용
AT_CLASS_PARTY_BUFF_02			= 3102
AT_CLASS_PARTY_BUFF_03			= 3103
AT_CLASS_PARTY_BUFF_04			= 3104
AT_CLASS_PARTY_BUFF_05			= 3105
AT_CLASS_PARTY_BUFF_06			= 3106
AT_CLASS_PARTY_BUFF_07			= 3107
AT_CLASS_PARTY_BUFF_08			= 3108
AT_CLASS_PARTY_BUFF_09			= 3109
AT_CLASS_PARTY_BUFF_10			= 3110	-- AT_CLASS_PARTY_BUFF_01 - 10 까지 사용

AT_EXCEPT_EFFECT_TICK			= 3111	-- 해당 번호의 이펙트의 틱 효과를 막아준다.
AT_EXCEPT_CHAOS_EFFECT_TICK		= 3112	-- 해당 번호의 이펙트의 카오스 버프를 막아준다.
AT_PENETRATION_USE_TYPE 		= 3125  -- 관통을 사용하는 타입 0 : 사용하지 않음 / 1 : 관통을 사용
AT_PROJECTILE_NUM				= 3126	-- 발사체 갯수
AT_NOT_DELETE_EFFECT			= 3121	-- 특정 조건에서 지워지지 않는 Effect Type인가?(0 : 죽으면 삭제 / 1 : Clear시에 사라지지 않음) / Duration및 직접적인 DeleteEffect 호출시에만 삭제
AT_DAMAGE_RESETTICK_EFFECT		= 3122	-- 해당 이펙트가 걸려있는 상태에서 Damage를 입으면 TickTime이 초기화 됨

AT_MON_OPENING_TIME				= 3182	-- 몬스터 오프팅 시간. 없으면 3000ms

AT_MON_SKILL_MONSTER_NO			= 3322  -- 스킬에서 몬스터 번호가 필요한 경우

AT_MON_CROSS_ATTACK_ANGLE		= 3339  -- x 공격의 회전 각도, ( ~ 90도)
AT_MON_PROJECTILE_NUM			= 3341	--  발사하는 발사체 갯수
AT_MON_PROJECTILE_ANGLE			= 3344  -- 발사체끼리의 각도
AT_DEFAULT_EFFECT				= 3140	-- Unit이 Add되자마자 걸려 있는 이펙트
AT_DETECT_RANGE_Z				= 3185	-- 타겟을 찾을 때 사용할 Z좌표 차이. 0이면 AI_Z_LIMIT을 사용
AT_ADD_ATTACK_COOL_TIME 		= 3401	-- 공격 쿨타임 보정치(천분률)
AT_ADD_TICK_ATTACK_TIME			= 3402	-- 클라이언트에서 Tick당 공격하는 스킬의 Tick 타임을 추가로 조절(천분률)
AT_USE_LOCK_SKILL 				= 3449	-- 락 스킬 사용유무
AT_LOCK_SKILLNO_01 				= 3450	-- 락걸 스킬 번호(10개 예약)
AT_ENABLE_AWAKE_SKILL			= 3500	-- 각성기 시스템을 활성화
AT_AWAKE_STATE					= 3501	-- 각성기 게이지 상태(0 : 일반상태(스킬 사용시 채워지거나, 시간당 자동 감소), 1: 맥스 상태, 2: 소모 상태(게이지 회복X)
AT_AWAKE_VALUE					= 3502	-- 각성기 게이지 값
AT_AWAKE_TICK_DEC_VALUE			= 3503	-- Tick마다 감소하는 게이지 값
AT_NEED_AWAKE					= 3504	-- 스킬 사용시 사용하는 각성기 게이지
AT_NEED_MAX_R_AWAKE				= 3505	-- 스킬 사용시 사용하는 각성기 게이지 (Max의 %로 사용)(만분률).
AT_ADD_NEED_R_AWAKE				= 3506  -- 스킬 시전시 사용하는 각성기 게이지를 %로 증감 시긴다.(만분률) AT_NEED_AWAKE * AT_ADD_R_NEED_HP / ABILITY_RATE_VALUE_FLOAT
AT_ALL_NEED_AWAKE				= 3507	-- 스킬 사용시 남아있는 모든 각성기 게이지 소모
AT_R_INC_AWAKE_VALUE			= 3508  -- 각성기 게이지 최대값에 %로 증가하는값(Max의 %로 증가)(만분률)

AT_SKILL_LINKAGE_POINT			= 3607	-- 연계효과 점수
AT_USE_SKILL_LINKAGE_POINT		= 3608	-- 연계효과 점수를 사용하는 스킬
	
-- 스킬 연출 다양화
AT_SKILL_CUSTOM_CHECK_LEARN_THIS_SKILL_NO	= 3650	-- 이 Value에 해당하는 스킬을 배웠다면(Parent SkillNo), 
AT_SKILL_CUSTOM_VALUE						= 3651	-- (Skill용!) AT_CUSTOM_SKILL_CHECK_LEARN_THIS_SKILL_NO에 해당하는 스킬을 배웠다면, 이 값에 대한 어빌을 세팅해줌(unit에 세팅)
AT_SKILL_CUSTOM_VALUE_STORAGE				= 3652	--[[ (Unit용!) 오직 클라에서만 이값을 사용하며 '비동기'값이다. 클라에서 받아서 처리하고 리셋 시킨다. 서버에서는 필요한 상황에 세팅해서 보내주기만 한다.
															***절대로 이값을 얻어서 서버에서 사용하지 않는다***
															Unit에 AT_CUSTOM_SKILL_CUSTOM_VALUE에 저장된 값을 저장해줄 저장소*/]]

AT_PUSH_DIST_WHEN_BLOCK = 3684 -- 블록시 밀릴거리
AT_PUSH_SPEED_WHEN_BLOCK = 3685	-- 블록시 밀릴속력
AT_PUSH_ACCEL_WHEN_BLOCK = 3686	-- 블록시 밀릴가속력
															
-- Player Ability
AT_BASE_CLASS	= 4009

-- Pet Ability
AT_LOYALTY		= 4001
AT_BLOOD		= 4002
AT_PLAYTIME		= 4003
AT_AGE			= 4004
AT_LIFE			= 4005
AT_HUNGRY		= 4006
AT_GRADE		= 4007	-- Pet/Monster/Item 의 등급
AT_PETNO		= 4008
AT_PETSTATE		= 4009
AT_ACCUMULATE_DAMAGED_NOW	= 4031	--현재 누적데미지
AT_ACCUMULATE_DAMAGED_MAX	= 4032	--최대 누적데미지
AT_TIME			= 4090
AT_COLOR_INDEX	= 4100
AT_HUNGER		= 4101
AT_HEALTH		= 4102
AT_MENTAL		= 4103


-- Unit의 특수한 상태 정의의의의~~~
AT_UNIT_HIDDEN			= 4201	-- 숨어있는 상태 (다른 Player에게 보이지 않는다)
AT_ENABLE_AUTOHEAL		= 4202	-- AutoHeal 할 것인가?
AT_FIXED_MOVESPEED		= 4203	-- MoveSpeed 변경 금지~~
AT_CANNOT_DAMAGE		= 4204	-- 공격 받지 않는다.
AT_FROZEN				= 4205	-- 움직일 수 없다(그러나 한대라도 맞으면 움직일 수 있다)
AT_CANNOT_ATTACK		= 4206 -- 공격 할 수 없다.
AT_FROZEN_DMG_WAKE		= 4207	-- Frozen 상태일 때 Damage를 받으면 Frozen 상태가 풀릴것인가? (>0:풀린다.)
AT_CANNOT_USEITEM		= 4208	-- 아이템을 사용 할 수 없는 상태이다.
AT_CANNOT_EQUIP			= 4209	-- 장비아이템(장착/해제) 금지된 상태
AT_CANNOT_CASTSKILL		= 4210	-- Casting Type 스킬 사용 금지
AT_CRITICAL_ONEHIT		= 4211	-- 다음 공격에 있어 무조건 Critical 공격이 된다.
AT_CANNOT_SEE			= 4212	-- 주변을 볼수 없게 됨
AT_QUEST_TALK			= 4213	-- 퀘스트 대화 중인 플레이어다 (0: IDLE 1: Talking)
--AT_LOOTED_ITEM			= 4214	-- 이미 주인이 결정된 그라운드 아이템이다 (0: None 1: Looted)
AT_FAKE_REMOVE_UNIT		= 4215	-- 가상으로 맵에서 삭제된 플레이어이다.(실제로는 HIDE 와 CANNOT ATTACK 등으로 유저에게 보이지 않게 구현되어 있음)
AT_EVENT_SCRIPT			= 4216	-- 클라이언트 측 이벤트 스크립트가 실행중이다
AT_EVENT_SCRIPT_TIME	= 4217	-- 클라이언트 측 이벤트 스크립트가 실행한 시간.
AT_INVINCIBLE			= 4218	-- AT_CANNOT_DAMAGE은 타겟이 되지 않지만, AT_INVINCIBLE은 때릴수 있으나 데미지가 0이 된다.
AT_INVINCIBLE2			= 4219	-- AT_INVINCIBLE와 틀린것은 헛침 처리가 되지 않고, 맞기까지 하나 HP만 안단다(현재 파괴오브젝트만  사용)

-- ITEM
ATI_EXPLAINID			= 5001	-- Item Tooltip text id
AT_EQUIP_LIMIT			= 5002
AT_CHARACTERFILTER		= 5003
AT_ITEMFILTER			= 5004
AT_GENDERLIMIT			= 5005
AT_PRICE				= 5006
AT_SELLPRICE			= 5007
AT_PVPSELL				= 5008
AT_ATTRIBUTE			= 5009
AT_MAX_LIMIT			= 5010	-- 인벤슬롯에 놓을수 있는 최대개수
AT_WEAPON_TYPE			= 5011
AT_HAIR_COLOR			= 5012
AT_HAIRBRIGHTNESS		= 5013
AT_DEFAULT_AMOUNT		= 5014	-- 수량 사용. 억단위가 0->내구도(최대내구도값) 1 ->수량(아이템 드랍될 때 기본적으로 생성되는 개수)
AT_PRIMARY_INV			= 5015	-- 메인인벤토리
AT_VIRTUAL_ITEM			= 5016	-- 수량 인벤에 들어가지 않는 가상 아이템.
AT_ITEMMAKING_NO		= 5017 -- 레시피 같은 아이템의 경우. 그 아이템이 할 수 있는 제조의 번호.
AT_MAKING_TYPE			= 5018 -- 제조의 타입 (요리 조합 분해 등...)
AT_DROP_TRANSFORM_TYPE	= 5030	--드롭될 때의 SRT 타입

-- SKILL
AT_SKILL_MAXIMUM_SPEED	= 5501	--이 스킬의 최대 속도. 이 속도 이상이면 해킹임

-- Boss Ability
AT_AI_DELAY			= 6001
AT_HP_GAGE			= 6002
AT_C_HP_GAGE		= 6003
AT_CURRENT_ACTION	= 6004
AT_MONSTER_APPEAR	= 6005
AT_MONSTER_DEAD		= 6006

AT_CALCUATEABIL_MIN			= 7000
AT_MAX_HP					= 7001 -- 기본값 or 절대값 (+)해주는 값
AT_R_MAX_HP					= 7002 -- _R_ == 천분율값
AT_C_MAX_HP					= 7003 -- _C_ = 최종값
AT_HP_RECOVERY_INTERVAL		= 7011	-- Auto Heal 되는 주기
AT_R_HP_RECOVERY_INTERVAL	= 7012
AT_C_HP_RECOVERY_INTERVAL	= 7013
AT_MAX_MP					= 7021
AT_R_MAX_MP					= 7022
AT_C_MAX_MP					= 7023
AT_MP_RECOVERY_INTERVAL		= 7031
AT_R_MP_RECOVERY_INTERVAL	= 7032
AT_C_MP_RECOVERY_INTERVAL	= 7033
AT_STR						= 7041
AT_R_STR					= 7042
AT_C_STR					= 7043
AT_INT						= 7051
AT_R_INT					= 7052
AT_C_INT					= 7053
AT_CON						= 7061
AT_R_CON					= 7062
AT_C_CON					= 7063
AT_DEX						= 7071
AT_R_DEX					= 7072
AT_C_DEX					= 7073
AT_MOVESPEED				= 7081
AT_R_MOVESPEED				= 7082
AT_C_MOVESPEED				= 7083
AT_PHY_DEFENCE				= 7091
AT_R_PHY_DEFENCE			= 7092
AT_C_PHY_DEFENCE			= 7093
AT_MAGIC_DEFENCE			= 7101
AT_R_MAGIC_DEFENCE			= 7102
AT_C_MAGIC_DEFENCE			= 7103
AT_ATTACK_SPEED				= 7111
AT_R_ATTACK_SPEED			= 7112
AT_C_ATTACK_SPEED			= 7113
AT_BLOCK_SUCCESS_VALUE				= 7121
AT_R_BLOCK_SUCCESS_VALUE				= 7122
AT_C_BLOCK_SUCCESS_VALUE				= 7123
AT_DODGE_SUCCESS_VALUE				= 7131
AT_R_DODGE_SUCCESS_VALUE				= 7132
AT_C_DODGE_SUCCESS_VALUE				= 7133
AT_CRITICAL_SUCCESS_VALUE			= 7141
AT_R_CRITICAL_SUCCESS_VALUE			= 7142
AT_C_CRITICAL_SUCCESS_VALUE			= 7143	
AT_CRITICAL_POWER			= 7151
AT_R_CRITICAL_POWER			= 7152
AT_C_CRITICAL_POWER			= 7153
AT_INVEN_SIZE				= 7161
AT_R_INVEN_SIZE				= 7162
AT_C_INVEN_SIZE				= 7163
AT_EQUIPS_SIZE				= 7171
AT_R_EQUIPS_SIZE			= 7172
AT_C_EQUIPS_SIZE			= 7173
AT_ATTACK_RANGE				= 7181	-- Item Skill
AT_R_ATTACK_RANGE			= 7182
AT_C_ATTACK_RANGE			= 7183
AT_HP_RECOVERY				= 7191	-- Amount of Auto Heal
AT_R_HP_RECOVERY			= 7192
AT_C_HP_RECOVERY			= 7193
AT_MP_RECOVERY				= 7201
AT_R_MP_RECOVERY			= 7202
AT_C_MP_RECOVERY			= 7203
AT_JUMP_HEIGHT				= 7211
AT_R_JUMP_HEIGHT			= 7212
AT_C_JUMP_HEIGHT			= 7213
AT_PHY_ATTACK_MAX			= 7221	-- EquipMonsterNPC
AT_R_PHY_ATTACK_MAX			= 7222
AT_C_PHY_ATTACK_MAX			= 7223
AT_PHY_ATTACK_MIN			= 7231 -- EquipMonsterNPC
AT_R_PHY_ATTACK_MIN			= 7232
AT_C_PHY_ATTACK_MIN			= 7233
AT_TARGET_BACK_DISTANCE 	= 7241--밀리는 길이
AT_TARGET_FLY_DISTANCE		= 7251--뜨는 높이.
AT_NEED_MP					= 7261	-- Skill 사용 할 때 필요한 MP
AT_R_NEED_MP				= 7262
AT_C_NEED_MP				= 7263
AT_NEED_HP					= 7271
AT_R_NEED_HP				= 7272
AT_C_NEED_HP				= 7273
AT_MAGIC_ATTACK				= 7281
AT_R_MAGIC_ATTACK			= 7282
AT_C_MAGIC_ATTACK			= 7283
AT_PHY_ATTACK				= 7291
AT_R_PHY_ATTACK				= 7292
AT_C_PHY_ATTACK				= 7293
AT_CRITICAL_MPOWER			= 7301
AT_R_CRITICAL_MPOWER		= 7302
AT_C_CRITICAL_MPOWER		= 7303
AT_HIT_SUCCESS_VALUE		= 7311
AT_R_HIT_SUCCESS_VALUE	= 7312
AT_C_HIT_SUCCESS_VALUE	= 7313
AT_WALK_SPEED				= 7321	-- 걷는 속도
AT_R_WALK_SPEED				= 7322
AT_C_WALK_SPEED				= 7323
AT_MAGIC_ATTACK_MAX			= 7331
AT_R_MAGIC_ATTACK_MAX		= 7332
AT_C_MAGIC_ATTACK_MAX		= 7333
AT_MAGIC_ATTACK_MIN			= 7341
AT_R_MAGIC_ATTACK_MIN		= 7342
AT_C_MAGIC_ATTACK_MIN		= 7343
AT_PHY_DMG_DEC				= 7351	-- 물리Damage 감소율
AT_R_PHY_DMG_DEC			= 7352
AT_C_PHY_DMG_DEC			= 7353
AT_MAGIC_DMG_DEC			= 7361	-- 마법Damage 감소율
AT_R_MAGIC_DMG_DEC			= 7362
AT_C_MAGIC_DMG_DEC			= 7363
AT_CASTING_SPEED			= 7371	-- Casting Speed
AT_R_CASTING_SPEED			= 7372
AT_C_CASTING_SPEED			= 7373
AT_RESIST_FIRE				= 7381	-- 속성저항(화)
AT_R_RESIST_FIRE			= 7382
AT_C_RESIST_FIRE			= 7383
AT_RESIST_WATER				= 7391	-- 속성저항(수)
AT_R_RESIST_WATER			= 7392
AT_C_RESIST_WATER			= 7393
AT_RESIST_TREE				= 7401	-- 속성저항(목)
AT_R_RESIST_TREE			= 7402
AT_C_RESIST_TREE			= 7403
AT_RESIST_IRON				= 7411	-- 속성저항(금)
AT_R_RESIST_IRON			= 7412
AT_C_RESIST_IRON			= 7413
AT_RESIST_EARTH				= 7421	-- 속성저항(토)
AT_R_RESIST_EARTH			= 7422
AT_C_RESIST_EARTH			= 7423
AT_HP_POTION_ADD_RATE		= 7431	--HP물약 사용시 추가적인 회복량
AT_R_HP_POTION_ADD_RATE		= 7432
AT_C_HP_POTION_ADD_RATE		= 7433
AT_MP_POTION_ADD_RATE		= 7441	--HP물약 사용시 추가적인 회복량
AT_R_MP_POTION_ADD_RATE		= 7442
AT_C_MP_POTION_ADD_RATE		= 7443
AT_DMG_REFLECT_RATE			= 7451	-- Damage 반사할 때 반사되는 양(Damage에 대한 만분율값)
AT_R_DMG_REFLECT_RATE		= 7452
AT_C_DMG_REFLECT_RATE		= 7453
AT_DMG_REFLECT_HITRATE		= 7461	-- Damage 반사할 때 반사가 일어날 확률
AT_R_DMG_REFLECT_HITRATE	= 7462
AT_C_DMG_REFLECT_HITRATE	= 7463
AT_PENETRATION_RATE			= 7471	-- 관통할 확률
AT_R_PENETRATION_RATE		= 7472
AT_C_PENETRATION_RATE		= 7473

AT_C_ATTACK_ADD_FIRE		= 30016--화염 추가 공격력(절대값)(최종값)
AT_C_ATTACK_ADD_ICE		= 30017--냉기 추가 공격력(절대값)(최종값)
AT_C_ATTACK_ADD_NATURE		= 30018--자연 추가 공격력(절대값)(최종값)
AT_C_ATTACK_ADD_CURSE		= 30019--저주 추가 공격력(절대값)(최종값)
AT_C_ATTACK_ADD_DESTROY		= 30020--파괴 추가 공격력(절대값)(최종값)

AT_C_RESIST_ADD_FIRE		= 30056--화염 추가 저항력(절대값)(최종값)
AT_C_RESIST_ADD_ICE		= 30057--냉기 추가 저항력(절대값)(최종값)
AT_C_RESIST_ADD_NATURE		= 30058--자연 추가 저항력(절대값)(최종값)
AT_C_RESIST_ADD_CURSE		= 30059--저주 추가 저항력(절대값)(최종값)
AT_C_RESIST_ADD_DESTROY		= 30060--파괴 추가 저항력(절대값)(최종값)

AT_ATTACK_ADD_4ELEMENT          = 30180--화염,냉기,자연,저주추가공격력(절대값)
AT_ATTACK_ADD_5ELEMENT          = 30181--화염,냉기,자연,저주,파괴추가공격력(절대값)

AT_RESIST_ADD_4ELEMENT          = 30220--화염,냉기,자연,저주추가저항력(절대값)
AT_RESIST_ADD_5ELEMENT          = 30221--화염,냉기,자연,저주,파괴추가저항력(절대값)

AT_RESIST_RATE_ADD_FIRE		= 30301--화염 추가 저항률(절대값.만분률)
AT_RESIST_RATE_ADD_ICE		= 30302--냉기 추가 저항률(절대값.만분률)
AT_RESIST_RATE_ADD_NATURE	= 30303--자연 추가 저항률(절대값.만분률)
AT_RESIST_RATE_ADD_CURSE	= 30304--저주 추가 저항률(절대값.만분률)
AT_RESIST_RATE_ADD_DESTROY	= 30305--파괴 추가 저항률(절대값.만분률)


AT_RESIST_RATE_ADD_4ELEMENT	= 30311--4대속성 저항률 증가(절대값.만분률)
AT_RESIST_RATE_ADD_5ELEMENT	= 30312--4대속성 저항률 증가(절대값.만분률)

AT_FINAL_HIT_SUCCESS_RATE		= 30501
AT_FINAL_DODGE_SUCCESS_RATE		= 30502
AT_FINAL_BLOCK_SUCCESS_RATE		= 30503
AT_FINAL_CRITICAL_SUCCESS_RATE	= 30504
	
AT_CALCUATEABIL_MAX			= 10000
-- Reserve : 10001 - 11000
-- AI Action type 값
--	실제 Action type 값 = GetAbil(AT_AI_ACTIONTYPE_MIN+Action)
AT_AI_ACTIONTYPE_MIN		= 10001	-- AI Action type 값

-- 20000~30000사이는 패널티 어빌입니다.
AT_PENALTY_HP_RATE			= 20000+AT_HP
AT_PENALTY_MP_RATE			= 20000+AT_MP
AT_ADD_EXPERIENCE_RATE		= 20000+AT_EXPERIENCE

-- 이펙트를 유저가 컨트롤 한다. 
AT_EFFCONTROL_DEL_MONEY		= 40001	-- 돈으로 삭제 가능(Value에 돈을 넣어줘야 합니다)
AT_EFFCONTROL_DEL_MONEY_PERLEVEL = 40002-- 돈으로 삭제를 하는데 레벨별 가중치 적용(Value에 가중치를 넣어주어야 합니다.)
AT_EFFCONTROL_DEL_ITEM		= 40003	-- 아이템으로 삭제 가능(Value에는 아이템 번호)

AT_MISSION_NO				= 50151
AT_MISSION_COUNT			= 50152
-- 기타 클라이언트에서 사용할 아이콘등의 어빌
AT_DEF_RES_NO				= 50001	

ABILITY_RATE_VALUE = 10000
AWAKE_VALUE_MAX = 5000					-- 각성 게이지 최대값

AT_BEAR_EFFECT_RED			= 50722 -- 이 어빌이 있는 플레이어는 레드 곰을 업은 상태.
AT_BEAR_EFFECT_BLUE			= 50723 -- 이 어빌이 있는 플레이어는 블루 곰을 업은 상태.

--커맨드 스킬 전이(각성 2차 작업에 사용. 사용자 입력 또는 상태에 따라 이후 연속된 스킬들을 변동 시킬수 있는 기능에 사용)
AT_TRANSLATE_SKILL_TYPE 	= 50800 -- 전이할 스킬들( 전이스킬수 == 최소 커맨드 수, 성공률 체크 == 100/전이스킬수, 이것들을 기반으로 다음 전이할 스킬이 무엇인지 설정함)
AT_TRANSLATE_SKILL01		= 50801
AT_TRANSLATE_SKILL10		= 50810
AT_TRANSLATE_SKILL_CMD_CNT	= 50811 -- 몇개의 커맨드 입력이 존재 하는가?
	
--러브 대작전
AT_BEAR_ITEM				= 50721 -- 이 어빌이 있는 아이템은 러브러브 모드 곰 아이템.
AT_BEAR_EFFECT_RED			= 50722 -- 이 어빌이 있는 플레이어는 곰을 엎은 상태.( 빨간 곰 )
AT_BEAR_EFFECT_BLUE			= 50723 -- 이 어빌이 있는 플레이어는 곰을 엎은 상태.( 파란 곰 )
AT_BEAR_EFFECT_SPEED_LOCK	= 50724 -- 이 어빌이 있으면 인스턴스 아이템, 포인트 스킬 외에는 이속을 변경할 수 없다.
AT_BEAR_EFFECT_ITEM_LOCK	= 50725 -- 이 어빌이 있으면 아이템을 사용 할 수 없다.

AT_INSTANCE_ITEM			= 65301 -- 인스턴스 아이템. 어빌이 1로 되있으면 인스턴스 아이템.
-----------------------------------------------
--                      PACKET TYPE
-----------------------------------------------
PT_C_M_NFT_ITEM_STATE  = 12423
PT_M_C_NFY_DAMAGE_RESULT = 12325
PT_M_C_NFY_REMOVE_MONSTER = 12351
PT_M_C_NFY_ATTACKDAMAGE = 12353
PT_M_C_NFY_ADD_NPC = 12424
PT_M_C_ITEMCHANGED = 12433
PT_M_C_RES_LEARN_SKILL = 12561
PT_M_C_NFY_ABILCHANGED = 12563
PT_M_C_NFY_BEGIN_WORLDACTION = 12602
PT_M_C_NFY_BOSSMONSTER = 12603
PT_M_C_NFY_TARGETUNIT = 12604
PT_C_M_GODCMD = 12801
PT_C_NT_GODCMD = 12806
PT_C_M_GOD_ADDMONSTER = 20301
PT_C_M_NFY_READY_ACTION = 46

PT_C_M_REQ_CANCEL_JOBSKILL = 20005
PT_M_C_RES_DELETE_SKILL = 20400
-----------------------------------------------
-- Unit State
-----------------------------------------------
US_BIT_CANT_MOVE			= 0x00010000
US_BIT_CANT_DAMAGED			= 0x00020000
US_BIT_CANT_CHANGE			= 0x00040000
US_BIT_WAITDELAY_BYFORCE	= 0x00080000

US_DEAD			= 1 + US_BIT_CANT_MOVE + US_BIT_CANT_DAMAGED
US_ATTACK		= 2
US_IDLE			= 4
US_WATCHPLAYER      = 8
US_ATTACKTIME	=	9
US_ATTACKDELAY	=	10

-----------------------------------------------
-- Unit Type
-----------------------------------------------
UT_NONETYPE		= 0
UT_PLAYER		= 0x00000001
UT_MONSTER		= 0x00000002
UT_GROUNDBOX	= 0x00000004
UT_PET			= 0x00000008
UT_NPC			= 0x00000010
UT_BOSSMONSTER	= 0x00000020 + UT_MONSTER
UT_SUMMONED		= 0x00000040 -- 소환체
UT_ENTITY 		= 0x00000100	-- 형태가 없는 개체(지역바법같은..)
UT_OBJECT		= 0x00000200
UT_SUB_PLAYER	= 0x00001000 -- 쌍둥이 보조 캐릭터 같은 타입
-----------------------------------------------
-- Action Argument type
-----------------------------------------------
ACTARG_MAPOBJ		= 1
ACTARG_ZONEOBJ		= 2
ACTARG_UNITMNG		= 3
ACTARG_PUPPETMNG = 4
ACTARG_PUPPET = 5
ACTARG_WORLDACTION = 6
ACTARG_PUPPETGROUP = 7
ACTARG_OLD_VALUE  = 8
ACTARG_CUSTOM_UNIT1 = 1001
-- RESERVER (10000 - 19999) : CUSTOM DATA
ACT_ARG_CUSTOMDATA1 = 10000


-----------------------------------------------
--Puppet Type
-----------------------------------------------
PUPPET_T_TRANSFORM = 1
PUPPET_T_PUPPETGROUP = 2
PUPPET_T_TRANSFORMCIRCLE = 3
-----------------------------------------------
-- Puppet Object	Type
-----------------------------------------------
PUPPET_OT_NONE = 0
PUPPET_OT_BOSSZONE_TILE = 1

-----------------------------------------------
--World	Action Object Type
-----------------------------------------------
WATYPE_NONE = 0
WATYPE_BASE = 1
WATYPE_ATTACKUNIT = 2

-----------------------------------------------
-- World Action
-----------------------------------------------
WA_SAPHIREBOSS_APPEAR	= 0
WA_SAPHIREBOSS_DIE			= 1
WA_SB_CRYSTAL_APPEAR		= 2
WA_SB_ATTACK1 = 3
WA_SB_ATTACK2	= 4
WA_SB_ATTACK3 = 5
WA_SB_ATTACK4 = 6
WA_SB_ATTACK5 = 7
WA_SB_ATTACK6 = 8
WA_SB_ROTATION = 9
WA_SB_DAMAGE = 10
WA_SB_ATTACK7 = 11
WA_SB_ATTACK8 = 12
WA_SB_ATTACK9 = 13
WA_SB_ATTACK10 = 14

-----------------------------------------------
-- Timer Interval
-----------------------------------------------
ETICK_INTERVAL_100MS = 0
ETICK_INTERVAL_30S = 1
ETICK_INTERVAL_1S = 2

-----------------------------------------------
-- Monster
-----------------------------------------------
M_SUSHI					=	1	-- 초밥
M_BROWNRAT				=	5	-- 시궁쥐
M_BUMBOW				=	7	-- 범보우
M_BIGSAILER				=	9	-- 빅세일러
M_SAPHIREBOSS			=	23 --	사파이어 드래곤	그렘그리스
M_PARELBOSS				=	6000920 --	파렐 경

SAT_PHYSICS		= 16777216	-- 물리공격 Skill (16777216)
SAT_MAGIC		= 33554432	-- 마법공격 skill (33554432)
SAT_PASSIVE		= 4	-- 수동적인 스킬
SAT_ACTIVE		= 8
SAT_CLIENT_CTRL_PROJECTILE = 2 --클라이언트가 컨트롤 하는 프로젝틸

-----------------------------------------------
-- Pet
----------------------------------------------
PET_NONE = 0
PET_SUMMON = 1
PET_RIDING = 2
PET_INROOM = 3

-----------------------------------------------
-- Weapon Sound Type
----------------------------------------------
WST_NONE = 0
WST_HIT = 1
WST_DAMAGE = 2

-----------------------------------------------
-- Direction
----------------------------------------------
DIR_NONE = 0
DIR_LEFT = 1
DIR_RIGHT = 2
DIR_HORIZONTAL = 3
DIR_UP = 4
DIR_DOWN = 8
DIR_LEFT_UP		= 5
DIR_RIGHT_UP	= 6
DIR_LEFT_DOWN	= 9
DIR_RIGHT_DOWN	= 10
DIR_VERTICAL = 12
DIR_ALL = 15
-----------------------------------------------
-- Weapon Type
----------------------------------------------
WT_FST = 0
WT_SWORD = 1
WT_BTS = 2
WT_STF = 3
WT_SPR = 4
WT_BOW = 5
WT_CROSSBOW = 6
WT_CLAW = 7
WT_KAT = 8
WT_SPECIAL = 9
WT_UNKNOWN = 100

-----------------------------------------------
-- World Type
----------------------------------------------
WDT_NONE		= 0
WDT_COMMON		= 1		--일반 맵
WDT_INDUN		= 2		--인스턴트던전 맵
WDT_PVP			= 4		--PVP전 맵 (사용안함)
WDT_VILLAGE		= 8		--마을 맵
WDT_MYROOM		= 16	--마이룸
WDT_PVP_KTH		= 32	--PVP전-KTH
WDT_PVP_DM		= 64	--PVP전-DM

-----------------------------------------------
-- ETC
----------------------------------------------
BOUNDARY_AMOUNT_N_PERCENTAGE = 10000

-----------------------------------------------
--	TargetAcquireType
-----------------------------------------------
TAT_RAY_DIR=0 	--	vStart 에서 vDir 방향으로 레이를 쏴서 걸리를 놈들을 찾아낸다.
TAT_RAY_DIR_COLL_PATH=1	-- vStart 에서 vDir 방향으로 레이를 쏴서 패스에 충돌하는 위치로부터 일정 영역 안에 있는 놈들을 찾아낸다.
TAT_SPHERE=2	--	vStart 에서 반지름 fEffectRadius 크기의 구 안에 있는 놈들을 찾아낸다.
TAT_BAR=3	--	사각통 안에 있는 놈들을 찾아낸다.

-----------------------------------------------
--	FindTargetOption
-----------------------------------------------
FTO_NORMAL=1
FTO_DOWN=2
FTO_BLOWUP = 4
FTO_OBSTACLE = 8

-----------------------------------------------------
--	KEYBOARD
-----------------------------------------------------
KEY_SPACE	=	1057
KEY_DOWN = 1126
KEY_LEFT = 1123
KEY_RIGHT = 1124
KEY_UP = 1121
KEY_DOWN_ATTACK	=	3004
KEY_UP_ATTACK	=	3003
KEY_LEFT_ATTACK	=	3009
KEY_RIGHT_ATTACK	=	3008
KEY_CHARGE = 1044
KEY_FIRE = 1045
KEY_ESC = 1001

KEY_CHARGING_UKEY = 1044


-----------------------------------------------------
-- PhysX Group 
-----------------------------------------------------
PG_PHYSX_GROUP_BASE_FLOOR = 0
PG_PHYSX_GROUP_BASE_WALL = 1
PG_PHYSX_GROUP_FILTERING_WALL = 5

-----------------------------------------------------
-- Item Type
-----------------------------------------------------
ITEM_UNKNOWN = 0
ITEM_HAIR_COLOR = 2^0
ITEM_HAIR = 2^1
ITEM_FACE = 2^2
ITEM_SHOULDER = 2^3
ITEM_MANTLE = 2^4
ITEM_GLASSES = 2^5
ITEM_WEAPON = 2^6
ITEM_SHIELD = 2^7
ITEM_NECKLESS = 2^8
ITEM_EARRING = 2^9
ITEM_RING = 2^10
ITEM_BELT = 2^11
ITEM_HELM = 2^20
ITEM_JACKET = 2^21
ITEM_PANTS = 2^22
ITEM_SHOES = 2^23
ITEM_GLOVES = 2^24

-----------------------------------------------------
-- Equip Pos Type
-----------------------------------------------------
EQUIP_POS_NONE			= 0
EQUIP_POS_HAIR_COLOR		= 	0
EQUIP_POS_HAIR			= 1	-- 머리카락
EQUIP_POS_FACE			= 2	-- 얼굴
EQUIP_POS_SHOULDER	= 	3	-- 어깨
EQUIP_POS_CLOAK		= 	4	-- 망토
EQUIP_POS_GLASS		= 	5	-- 안경
EQUIP_POS_WEAPON		= 6	-- 무기
EQUIP_POS_SHEILD		= 7	-- 방패
EQUIP_POS_NECKLACE	= 	8	-- 목걸이
EQUIP_POS_EARRING		= 9	-- 귀걸이
	EQUIP_POS_EARRING_L	= 	9
	EQUIP_POS_EARRING_R	= 	29
EQUIP_POS_RING			= 10	-- 반지
	EQUIP_POS_RING_L	= 	10
	EQUIP_POS_RING_R	= 	30
EQUIP_POS_BELT			= 11	-- 벨트
EQUIP_POS_ATTSTONE	= 	12	-- 속성석
EQUIP_POS_HELMET		= 20	-- 투구
EQUIP_POS_SHIRTS		= 21	-- 상의
EQUIP_POS_PANTS		= 	22	-- 하의
EQUIP_POS_BOOTS		= 	23	-- 부츠
EQUIP_POS_GLOVE		= 	24	-- 장갑
EQUIP_POS_MAX			= 31

----------------------------------------------------------
-- Trigger Action Type
----------------------------------------------------------
TRIGGER_ACTION_COLLISION	= 0x00000010 --충돌 ->곧 스킬ID 로 변경할 것이다.
TRIGGER_ACTION_CLICKED		= 0x00000001 --mouse click

----------------------------------------------------------
-- Trigger Type
----------------------------------------------------------
TRIGGER_TYPE_NONE 					= -1
TRIGGER_TYPE_PORTAL					= 0
TRIGGER_TYPE_MISSION				= 1
TRIGGER_TYPE_JUMP					= 2
TRIGGER_TYPE_TELEJUMP				= 3
TRIGGER_TYPE_ROPE_RIDING			= 4
TRIGGER_TYPE_LOCATION				= 5
TRIGGER_TYPE_EVENT					= 6
TRIGGER_TYPE_KEYEVENT				= 7
TRIGGER_TYPE_CAMERA					= 8
TRIGGER_TYPE_BATTLEAREA				= 9
TRIGGER_TYPE_PORTAL_EMPORIA			= 10
TRIGGER_TYPE_MISSION_NPC			= 11
TRIGGER_TYPE_ENTER_HOME				= 12
TRIGGER_TYPE_MISSION_EVENT_NPC		= 13
TRIGGER_TYPE_TRANSTOWER				= 14
TRIGGER_TYPE_MISSION_EASY			= 15
TRIGGER_TYPE_HIDDEN_PORTAL			= 16
TRIGGER_TYPE_TELEPORT				= 17
TRIGGER_TYPE_SUPERGROUND			= 18
TRIGGER_TYPE_PARTYPORTAL			= 19
TRIGGER_TYPE_PARTY_MEMBER_PORTAL	= 20
TRIGGER_TYPE_JOB_SKILL				= 21
TRIGGER_TYPE_DOUBLE_UP				= 22
TRIGGER_TYPE_ROADSIGN				= 23
TRIGGER_TYPE_CHANNELPORTAL			= 24
TRIGGER_TYPE_KING_OF_HILL			= 25
TRIGGER_TYPE_LOVE_FENCE				= 26
TRIGGER_TYPE_SCORE					= 27
TRIGGER_TYPE_RADAR					= 28
TRIGGER_TYPE_MISSION_EVENT_NOT_HAVE_ARCADE = 29
----------------------------------------------------------
-- Config.xml keys
----------------------------------------------------------
------------------------------Head key----------------------------
HEADKEY_OPTION		= "OPTION"
HEADKEY_USER_UI		= "USER_UI"
HEADKEY_GRAPHIC1	= "GRAPHIC1"
HEADKEY_GRAPHIC2	= "GRAPHIC2"
HEADKEY_GRAPHIC3	= "GRAPHIC3"
HEADKEY_ETC				= "ETC"
HEADKEY_GAME			= "GAME"
HEADKEY_SOUND		= "SOUND"
HEADKEY_CHATMODE_ALL = "CHATMODE_ALL"
HEADKEY_CHATMODE_NORMAL = "CHATMODE_NORMAL"
HEADKEY_CHATMODE_PARTY = "CHATMODE_PARTY"
HEADKEY_CHATMODE_GUILD = "CHATMODE_GUILD"
HEADKEY_CHATMODE_SYSTEM = "CHATMODE_SYSTEM"
HEADKEY_CHATMODE_TRADE = "CHATMODE_TRADE"
HEADKEY_CHARINFO	 = "CHARINFO"

HEADKEY_SYSTEMOUT_OPTION = "SYSTEMOUT_OPTION"

-----------------------------Subkey-----------------------------
SUBKEY_FRIEND_SHOWGROUP	= "FRIEND_SHOWGROUP"
SUBKEY_QUEST_QUICK_SHOW	= "QUEST_QUICK_SHOW"
SUBKEY_LAST_OPTION				= "LAST_OPTION"
SUBKEY_LAST_COMMUNITY		= "LAST_COMMUNITY"
SUBKEY_DEFAULT_CHAT_TAB	= "DEFAULT_CHAT_TAB"
SUBKEY_LAST_CHAT_TAB			= "LAST_CHAT_TAB"
SUBKEY_DEFAULT_CHATLOG_SHOW		= "DEFAULT_CHATLOG_SHOW"
SUBKEY_CHATLOG_SHOW			= "CHATLOG_SHOW"
SUBKEY_CURRENT_MINIMAP		= "CURRENT_MINIMAP"
SUBKEY_SUMMON_UI_POS_X ="SUMMON_UI_POS_X"
SUBKEY_SUMMON_UI_POS_Y ="SUMMON_UI_POS_Y"

--Game
SUBKEY_RESOLUTION_WIDTH = "RESOLUTION_WIDTH"
SUBKEY_RESOLUTION_HEIGHT = "RESOLUTION_HEIGHT"
SUBKEY_FULL_SCREEN			= "FULL_SCREEN"
SUBKEY_FLY_TO_CAMERA		= "FLY_TO_CAMERA"
SUBKEY_NIGHT_MODE			= "NIGHT_MODE"
--Option
SUBKEY_THEME_COLOR_SET		= "THEME_COLOR_SET"

--Sound
SUBKEY_BGM_VOLUME				= "BGM_VOLUME"
SUBKEY_EFFECT_VOLUME			= "EFFECT_VOLUME"
SUBKEY_MUTE_BGM_VOLUME		= "MUTE_BGM_VOLUME"
SUBKEY_MUTE_EFFECT_VOLUME	= "MUTE_EFFECT_VOLUME"

--Option ETC
SUBKEY_DENY_CHAT_WHISPER	= "DENY_CHAT_WHISPER"
SUBKEY_DENY_CHAT_PARTY		= "DENY_CHAT_PARTY"
SUBKEY_DENY_CHAT_FRIEND		= "DENY_CHAT_FRIEND"
SUBKEY_DENY_CHAT_GUILD		= "DENY_CHAT_GUILD"
SUBKEY_DENY_TRADE				= "DENY_TRADE"
SUBKEY_DENY_PARTY					= "DENY_PARTY"
SUBKEY_DENY_FRIEND				= "DENY_FRIEND"
SUBKEY_DENY_GUILD					= "DENY_GUILD"
SUBKEY_DENY_DUEL			= "DENY_DUEL"
SUBKEY_SHOW_NAME_NPC		= "SHOW_NAME_NPC"
SUBKEY_SHOW_NAME_PC			= "SHOW_NAME_PC"
SUBKEY_SIMPLY_TOOLTIP		= "SIMPLY_TOOLTIP"

SUBKEY_AUTOITEM_MONEY			= "AUTOITEM_MONEY"
SUBKEY_AUTOITEM_EQUIP			= "AUTOITEM_EQUIP"
SUBKEY_AUTOITEM_CONSUME			= "AUTOITEM_CONSUME"
SUBKEY_AUTOITEM_ETC			= "AUTOITEM_ETC"

SUBKEY_BATTLEOP_DISPLAY_HPBAR	= "BATTLEOP_DISPLAY_HPBAR"
SUBKEY_OFF_HELP				= "OFF_HELP"
SUBKEY_DISPLAY_HELMET		= "DISPLAY_HELMET"
SUBKEY_SHOW_ALL_QUEST		= "SHOW_ALL_QUEST"
SUBKEY_DISPLAY_PET_MPBAR	= "DISPLAY_PET_MPBAR"
SUBKEY_DISPLAY_ACHIEVE_STATUS	= "DISPLAY_ACHIEVE_STATUS"

SUBKEY_NOTIFY_FRIEND 		= "NOTIFY_FRIEND"
SUBKEY_NOTIFY_COUPLE 		= "NOTIFY_COUPLE"
SUBKEY_NOTIFY_GUILD 		= "NOTIFY_GUILD"

SUBKEY_NAVIGATION			= "NAVIGATION_VISIBLE"
--
SUBKEY_GRAPHIC_SET				= "GRAPHIC_SET"
--Graphic Perfomence
SUBKEY_WORLD_QUALITY			= "WORLD_QUALITY"
SUBKEY_VIEW_DISTANCE			= "VIEW_DISTANCE"
SUBKEY_WORLD_SHADER			= "WORLD_SHADER"
SUBKEY_ANTI_ALIAS					= "ANTI_ALIAS"
SUBKEY_GLOW_EFFECT				= "GLOW_EFFECT"
SUBKEY_TEXTURE_FILTERING				= "TEXTURE_FILTERING"
SUBKEY_TEXTURE_RES				= "TEXTURE_RES"
SUBKEY_REFRASH_RATE_SYNC	= "REFRESH_RATE_SYNC"
SUBKEY_DISPLAY_OTHERPLAYER_DAMAGE = "DISPLAY_OTHERPLAYER_DAMAGE"
SUBKEY_DISPLAY_MYPLAYER_DAMAGE = "DISPLAY_MYPLAYER_DAMAGE"

--ChatMode
SUBKEY_FILTER_NORMAL 		= "FILTER_NORMAL"
SUBKEY_FILTER_PARTY			= "FILTER_PARTY"
SUBKEY_FILTER_GUILD			= "FILTER_GUILD"
SUBKEY_FILTER_TRADE			= "FILTER_TRADE"
SUBKEY_FILTER_SYSTEM		= "FILTER_SYSTEM"
SUBKEY_FILTER_SYSTEM_DAMAGE		= "FILTER_SYSTEM_DAMAGE"
SUBKEY_FILTER_SYSTEM_ITEM		= "FILTER_SYSTEM_ITEM"
SUBKEY_FILTER_SYSTEM_GOLD		= "FILTER_SYSTEM_GOLD"
SUBKEY_FILTER_SYSTEM_EXP		= "FILTER_SYSTEM_EXP"

SUBKEY_SYSTEMOUT_WINDOW = "SYSTEMOUT_WINDOW"
SUBKEY_SYSTEMOUT_EXTEND = "SYSTEMOUT_EXTEND"
SUBKEY_SYSTEMOUT_HIDE	= "SYSTEMOUT_HIDE"

--Charactor Info
SUBKEY_CHARINFO_LEFT		= "CHARINFO_LEFT"
SUBKEY_CHARINFO_RIGHT		= "CHARINFO_RIGHT"

-----------------------------MISSION-----------------------------
PG_PHYSX_GROUP_MISSION_WALL_0 = 20
PG_PHYSX_GROUP_MISSION_WALL_1 = PG_PHYSX_GROUP_MISSION_WALL_0 + 1
PG_PHYSX_GROUP_MISSION_WALL_2 = PG_PHYSX_GROUP_MISSION_WALL_1 + 1
PG_PHYSX_GROUP_MISSION_WALL_3 = PG_PHYSX_GROUP_MISSION_WALL_2 + 1
PG_PHYSX_GROUP_MISSION_WALL_4 = PG_PHYSX_GROUP_MISSION_WALL_3 + 1
PG_PHYSX_GROUP_MISSION_WALL_5 = PG_PHYSX_GROUP_MISSION_WALL_4 + 1
PG_PHYSX_GROUP_MISSION_WALL_6 = PG_PHYSX_GROUP_MISSION_WALL_5 + 1
PG_PHYSX_GROUP_MISSION_WALL_7 = PG_PHYSX_GROUP_MISSION_WALL_6 + 1
PG_PHYSX_GROUP_MISSION_WALL_8 = PG_PHYSX_GROUP_MISSION_WALL_7 + 1
PG_PHYSX_GROUP_MISSION_WALL_9 = PG_PHYSX_GROUP_MISSION_WALL_8 + 1
PG_PHYSX_GROUP_MISSION_WALL_10 = PG_PHYSX_GROUP_MISSION_WALL_9 + 1
PG_PHYSX_GROUP_MISSION_WALL_11 = PG_PHYSX_GROUP_MISSION_WALL_10 + 1


-----------------------------CONDITION TYPE-----------------------------
CT_TOUCH=0
CT_ACTION=1
CT_SCRIPT=2

------------------ CLASS TYPE ---------------------
CT_FIGHTER = 1		-- 전사			0000 0000 0000 0002
CT_MAGICIAN = 2	-- 매지션		0000 0000 0000 0004
CT_ARCHER = 3		-- 궁수			0000 0000 0000 0008
CT_THIEF = 4		-- 도둑			0000 0000 0000 0010
CT_KNIGHT = 5		-- 기사			0000 0000 0000 0020
CT_WARRIOR = 6		-- 투사			0000 0000 0000 0040
CT_MAGE = 7		-- 메이지		0000 0000 0000 0080
CT_BATTLEMAGE = 8	-- 전투마법사	0000 0000 0000 0100
CT_HUNTER = 9		-- 사냥꾼		0000 0000 0000 0200
CT_RANGER = 10		-- 레인저		0000 0000 0000 0400
CT_CLOWN = 11		-- 광대			0000 0000 0000 0800
CT_ASSASSIN =12	-- 암살자		0000 0000 0000 1000
CT_PALADIN = 13	-- 성기사
CT_GLADIATOR = 14	-- 검투사
CT_WIZARD = 15		-- 위자드
CT_WARMAGE = 16	-- 워메이지
CT_TRAPPER = 17	-- 트랩퍼
CT_SNIPER = 18		-- 저격수
CT_DANCER = 19		-- 댄서
CT_NINJA = 20		-- 닌자
CT_DRAOON = 21		-- 용기사
CT_DESTROYER = 22	-- 파괴자
CT_ARCHMAGE = 23	-- 아크메이지
CT_WARLORD = 24		-- 워로드
CT_SENTINEL = 25	-- 수호자
CT_LAUNCHER = 26	-- 스트라이커
CT_FREEJACKER = 27	-- 매니악
CT_SHADOW = 28		-- 새도우
--Hidden
CT_DUELIST = 29	-- 무도가
CT_DOCTOR = 30		-- 닥터
CT_GUNSLINGER = 31	-- 건슬링거
CT_BARD = 32		-- 음유시인
CT_BATTLEMASTER = 33	-- 패와
CT_ANATOMIST = 34	-- 분석가
CT_DESPERADO = 35	-- 무법자
CT_VOCAL = 36		-- 보컬


--Dragonian
CT_SHAMAN = 51		-- 주술사
CT_DOUBLE_FIGHTER = 52	-- 더블 파이터
CT_SUMMONER = 53	-- 소환사
CT_TWINS = 54		-- 쌍둥이
CT_NATURE_MASTER = 55	-- 비스트마스터
CT_MIRAGE = 56		-- 미라지
CT_GAIA_MASTER = 57	-- 대지의 주인
CT_DRAGON_FIGHTER = 58	-- 드래곤 파이터

CT_COUPLE_EMOTION = 98	--커플 이모션
CT_SPECIAL_SKILL = 99 -- 스페셜 스킬
--------------------	Action Param	-----------------------
AP_CASTING = 1
AP_FIRE = 2

----------------------- QuickSlot Page Count ---------------------
QUICKSLOT_PAGE_COUNT = 6

----------------------- Minimap Name ---------------------
MINIMAP_MAIN = "MiniMap_Main"
MINIMAP_WHOLE = "MiniMap_Whole"
MINIMAP_BOOK = "MiniMap_Book"

------------------------ World Attribute(IsHaveWorldAttr(Value)) Value -----
GATTR_DEFAULT		= 0 	-- Static Ground
GATTR_INSTANCE		= 1		-- Instance Dungeon
GATTR_FLAG_PVP		= 2     -- PvP Check Flag
GATTR_PVP			= 3     -- PvP Ground
GATTR_FLAG_MISSION	= 4     -- Mission Check Flag
GATTR_MISSION		= 5     -- Mission Ground
GATTR_FLAG_BOSS		= 8     -- Boss Map Check Flag
GATTR_BOSS			= 9     -- Boss Ground
GATTR_FLAG_VILLAGE	= 16
GATTR_VILLAGE		= 16
GATTR_FLAG_EMPORIABATTLE	= 32
GATTR_EMPORIABATTLE			= 35
GATTR_FLAG_EMPORIA	= 64
GATTR_EMPORIA		= 65
GATTR_FLAG_MYHOME	= 128
GATTR_MYHOME		= 129
GATTR_FLAG_HIDDEN_F	= 256
GATTR_HIDDEN_F		= 256
GATTR_FLAG_CHAOS_F	= 512
GATTR_CHAOS_F		= 512
GATTR_CHAOS_MISSION	= 517
GATTR_CHAOS_F		= 512	-- 카오스
GATTR_STATIC_DUNGEON= 1024
GATTR_HOMETOWN_F	= 2048
GATTR_HOMETOWN		= 2048
GATTR_MARRY			= 4096
GATTR_MARRY_F		= 4096
GATTR_FLAG_BATTLESQUARE = 8192
GATTR_BATTLESQUARE	= 8192
GATTR_BATTLESQUARE_F	= 8192
GATTR_FLAG_HARDCORE_DUNGEON	= 16384
GATTR_HARDCORE_DUNGEON		= 16385
GATTR_HARDCORE_DUNGEON_BOSS	= 16393 
GATTR_FLAG_SUPER			= 32768
GATTR_SUPER_GROUND			= 32769
GATTR_SUPER_GROUND_BOSS		= 32777
GATTR_FLAG_EXPEDITION		= 65536		-- 원정대
GATTR_EXPEDITION_GROUND		= 65537
GATTR_EXPEDITION_GROUND_BOSS= 65545
GATTR_EXPEDITION_LOBBY		= 131072
GATTR_ELEMENT_GROUND			= 294913
GATTR_ELEMENT_GROUND_BOSS		= 294921
GATTR_EVENT_GROUND			= 524288
GATTR_RACE_GROUND			= 1572864

GATTR_FLAG_CONSTELLATION	= 2097152
GATTR_CONSTELLATION_GROUND	= 2097153
GATTR_CONSTELLATION_BOSS	= 2097161
	
GATTR_FLAG_PUBLIC_CHANNEL	= GATTR_FLAG_PVP+GATTR_FLAG_EMPORIABATTLE+GATTR_FLAG_EMPORIA+GATTR_FLAG_MYHOME+GATTR_HOMETOWN_F+GATTR_FLAG_BATTLESQUARE+GATTR_FLAG_HARDCORE_DUNGEON	-- Only Public Channel
GATTR_FLAG_NOPARTY			= GATTR_FLAG_PVP+GATTR_FLAG_EMPORIABATTLE+GATTR_FLAG_EMPORIA+GATTR_FLAG_MYHOME+GATTR_HOMETOWN_F+GATTR_FLAG_BATTLESQUARE						-- No Party Check
GATTR_FLAG_NOPVP			= GATTR_INSTANCE+GATTR_HOMETOWN_F+GATTR_MARRY_F+GATTR_FLAG_BATTLESQUARE+GATTR_FLAG_HARDCORE_DUNGEON
GATTR_FLAG_PVP_ABLE			= GATTR_FLAG_PVP+GATTR_FLAG_EMPORIABATTLE+GATTR_FLAG_BATTLESQUARE
GATTR_FLAG_HAVEPARTY		= GATTR_INSTANCE+GATTR_FLAG_MISSION+GATTR_FLAG_BOSS				-- Need Party or Single
GATTR_FLAG_RESULT			= GATTR_FLAG_MISSION+GATTR_FLAG_BOSS				-- Result Check Flag
GATTR_FLAG_NOEXCHANGEITEM	= GATTR_FLAG_PUBLIC_CHANNEL						-- Item Exchange
GATTR_FLAG_AUTOHEAL_ENALBE	= GATTR_VILLAGE + GATTR_FLAG_EMPORIA + GATTR_FLAG_MYHOME + GATTR_HOMETOWN_F + GATTR_BATTLESQUARE_F
GATTR_FLAG_NOWORLDMAP		= GATTR_FLAG_PVP+GATTR_FLAG_MISSION+GATTR_FLAG_BOSS+GATTR_FLAG_EMPORIABATTLE

GATTR_FLAG_RUNNING_PARTY	= GATTR_MISSION + GATTR_STATIC_DUNGEON + GATTR_FLAG_SUPER + GATTR_HIDDEN_F + GATTR_SUPER_GROUND_BOSS + GATTR_HARDCORE_DUNGEON

GATTR_FLAG_NOMINIMAP		= GATTR_INSTANCE+GATTR_BATTLESQUARE+GATTR_RACE_GROUND
GATTR_FLAG_ENABLE_KICK		= GATTR_INSTANCE+GATTR_STATIC_DUNGEON+GATTR_EVENT_GROUND

------------------------- E_CONTINENT_TYPE ---------------------
CONTINENT_OF_WIND			= 1-- 바람의 대륙
CONTINENT_OF_CHAOS			= 2-- 혼돈의 대륙
CONTINENT_OF_LAND			= 3-- 고요의 섬




------------------------- EInvType ----------------------------
IT_EQUIP = 1
IT_CONSUME = 2
IT_ETC = 3
IT_CASH = 4
IT_SAFE = 5
IT_CASH_SAFE = 6
IT_FIT = 7

------------------------- Camera -------------------------------
CMODE_NONE = 0
CMODE_FOLLOW = 1
CMODE_MOVIE = 2
CMODE_CIRCLE = 3
CMODE_SCRIPT = 4
CMODE_FREE = 5
CMODE_FOLLOW2 = 6
CMODE_FOLLOW_DEPTH_PATH = 7		-- 캐릭터 따라 깊이 들어가지 않고, 패스벽 기준으로 있는다.
CMODE_BOX = 8					-- 마그마 드레이크에서 쓰이는 카메라.
CMODE_EVENT = 9
CMODE_ANIMATION = 10			-- Actor/Nif 안에 들어있는 애니메이션이 있는 카메라 모드


--------------------------- Monster Grade -------------------------
EMGRADE_NONE		= 0
EMGRADE_NORMAL		= 1
EMGRADE_UPGRADED	= 2
EMGRADE_ELITE		= 3
EMGRADE_BOSS		= 4
--

--------------------------- User Place -------------------------
E_PLACE_NONE		= 0
E_PLACE_GROUND		= 1
E_PLACE_PVP_LOBBY	= 2
E_PLACE_PVP_ROOM	= 3
--

-------------------------- Contents Status ----------------------
STATUS_NONE			= 0
STATUS_LOBBY		= 1
STATUS_ROOM			= 2
STATUS_READY		= 3
STATUS_COUNTDOWN	= 4
STATUS_PLAYING		= 5
STATUS_ENDBEFORE	= 6
STATUS_RESULT		= 7
--

------------------------ Party List Sort ------------------------
PLS_NONE			= 0
PLS_STATEGREATER	= 1
PLS_STATELESS		= 2
PLS_NAMEGREATER		= 3
PLS_NAMELESS		= 4
PLS_ATTRGREATER		= 5
PLS_ATTRLESS		= 6
PLS_MEMBERGRATER	= 7
PLS_MEMBERLESS		= 8
PLS_END				= 9

------------------------ Find Party User List Sort ------------------

FPL_NONE			= 0
FPL_TITLEGREATER	= 1
FPL_TITLELESS		= 2
FPL_LEVELGREATER	= 3
FPL_LEVELLESS		= 4
FPL_CLASSGREATER	= 5
FPL_CLASSLESS		= 6
FPS_END				= 7

-------------------------- PvP Mode --------------------------
PVP_MODE_TEAM				= 0
PVP_MODE_PERSONAL			= 1
	
-------------------------- PVP Type --------------------------
PVP_TYPE_NONE				= 0
PVP_TYPE_DM					= 1	-- 사투전
PVP_TYPE_KTH				= 2 -- 점령전
PVP_TYPE_ANNIHILATION		= 4 -- 섬멸전
PVP_TYPE_DESTROY			= 8 -- 유물파괴전
PVP_TYPE_WINNERS			= 16-- 승자전
PVP_TYPE_WINNERS_TEAM		= 32-- 팀 승자전
PVP_TYPE_ACE				= 64-- 대장전
PVP_TYPE_LOVE				= 128-- 러브러브 대작전
PVP_TYPE_ALL				= 255
WAR_TYPE_DESTROYCORE		= 256
WAR_TYPE_PROTECTDRAGON		= 512
WAR_TYPE_ALL				= 65280
--

----- Team 
TEAM_NONE		= 0
TEAM_RED		= 1
TEAM_BLUE		= 2
TEAM_MAX		= 3
TEAM_ATTACKER	= TEAM_RED
TEAM_DEFENCER	= TEAM_BLUE

TEAM_PERSONAL_BEGIN	= 101
TEAM_PERSONAL_END	= 255-- 현재 255를 넘으면 안된다.

------------------------ Guild House Command -------------------
E_GH_C_N_REQ_INFOMATION 		= 1		-- 길드하우스에 대한 정보를 요청


-----------------------------------------------------
-- SKILL CUSTOM DATA
------------------------------------------------------
AT_SKILL_CUSTOM_DATA_01		= 60001
AT_SKILL_CUSTOM_DATA_MAX	= 60201

------------------------- EMsgBoxType -----------------------------
MBT_NONE					= 0
MBT_PARTY_JOIN				= 1
MBT_PARTY_EXIT				= 2
MBT_GUILD_JOIN				= 3
MBT_CONFIRM_EXIT			= 4
MBT_CONFIRM_BUY_SKILL		= 5
MBT_CONFIRM_COUPLE			= 6
MBT_CONFIRM_BREAK_COUPLE	= 7
MBT_CONFIRM_INSTANCE_COUPLE	= 8
MBT_CONFIRM_WARP_COUPLE		= 9
MBT_PARTYFIND_JOIN			= 10
MBT_CONFIRM_PAYMENT_MAIL	= 11
MBT_CONFIRM_GET_ANNEX_ALL	= 12
MBT_CONFIRM_RETURN_MAIL_ALL	= 13
MBT_CONFIRM_RETURN_MAIL		= 14
MBT_CONFIRM_DELETE_MAIL		= 15

MBT_EXPEDITION_DESTROY			= 80
MBT_EXPEDITION_LEAVE			= 81
MBT_EXPEDITION_JOIN				= 82
MBT_EXPEDITION_INVITE			= 83
MBT_EXPEDITION_COMPLETE_EXIT	= 84
MBT_CONFIRM_LEAVE_EXPEDITION_LOBBY = 85
MBT_EXPEDITION_CONFIRM_ENTER	= 86
MBT_EXPEDITION_KICK_MEMBER		= 87
MBT_EXPEDITION_PVPMODE_ENTER	= 88

MBT_CREATE_JUMPING_CHAR_EVENT	= 93
MBT_CREATE_DRAKAN_CHAR = 99

--EChatStationType
ECS_COMMON=0
ECS_PVPLOBBY=1
ECS_CASHSHOP=2
ECS_MYHOME=3

--ECashItemBuyType
CIBT_NONE=0
CIBT_BUY=1
CIBT_BUY_REAL=2
CIBT_RENEW=3
CIBT_RENEW_REAL=4
CIBT_GIFT=5
CIBT_GIFT_REAL=6
CIBT_IMPORTUNE=7
CIBT_IMPORTUNE_REAL=8
CIBT_PV_ALL=9
CIBT_PV_ALL_REAL=10
CIBT_BASKET=11
CIBT_BASKET_REAL=12
CIBT_BASKET_PART=13
CIBT_BASKET_PART_REAL=14
CIBT_STATIC_ITEM_BUY=15
CIBT_STATIC_ITEM_BUY_REAL=16
CIBT_STATIC_ITEM_RENEW=17
CIBT_STATIC_ITEM_RENEW_REAL=18

--ECashShopPageBtnType
PBT_NORMAL=0
PBT_JUMP=1
PBT_LAST=2

--E_EMPORIA_FUNC
EMPORIA_CONTRIBUTE	= 1-- 기부함
EMPORIA_WAREHOUSE	= 2-- 길드창고
EMPORIA_OBLATION	= 3--시주함
EMPORIA_SAFES		= 4-- 개인금고

--채팅 필터
--ChatFilter_All = 1 --전체 챗팅
ChatFilter_Normal = 1 --전체 챗팅
ChatFilter_Party = 2 --전체 챗팅
ChatFilter_Guild = 3 --전체 챗팅
ChatFilter_Trade = 4
ChatFilter_System = 5 --전체 챗팅


-- 채팅 타입
CT_Error=0			--에러 이벤트 타입
CT_None=1
CT_Normal=2 		--채팅모드 가장 기본값 (>>요기서 부터 사용자가 쓸수 있는 모드)
CT_Party=3
CT_Whisper_ByName=4
CT_Friend=5
CT_Guild=6
CT_ManToMan=7
CT_TEAM=8			--PVP에서 TEAM CHATING(<<요기까지 사용자가 사용 가능한 채팅 모드)
CT_UserSelect_Max=9	--사용자가 선택할 수 있는 채팅모드
CT_Whisper_ByGuid=10
CT_Event=11			--친구, 파티초대, 길드 등에 이벤트 메시지
CT_Event_System=12	--서버/클라이언트 시스템 이벤트 메시지(서버가 종료되었습니다, 연결이 종료 되었습니다.)
CT_Event_Game=13	--게임 메시지(공격, 스킬, ..)
CT_Notice=14		--공지
CT_Battle=15		--전투
CT_Item=16			--전투
CT_Command=17		--명령어
CT_MegaPhone_CH=18	--확성기 - 채널
CT_MegaPhone_SV=19	--확성기 - 서버
CT_RAREMONSTERGEN = 20 	--희귀 몬스터 생성
CT_RAREMONSTERDEAD = 21 --희귀 몬스터 죽음
CT_OXQUIZEVENT = 22     --OX 퀴즈 이벤트 메시지

CT_ERROR_NOTICE  = 23 	-- 오류 메세지
CT_NORMAL_NOTICE = 24	-- 일반
CT_NOTICE1		 = 25   -- 중요 정보 갱신사항
CT_NOTICE2		 = 26   -- 유저와 관계된 공지
CT_NOTICE3		 = 27	-- 정보갱신, 유저에게 알림사항
CT_LOVE_SMS 	 = 28	-- 사랑의 편지 
--채팅타입 추가<091102 조현건>
CT_Gold = 30		--골드
CT_Exp = 31			--경험치
CT_Emotion = 32		--감정표현 <20100112 조현건>

CT_Trade = 39		-- 거래

CT_MAX=40			--eChatType은 < CT_MAX 야 옳다


-- 파티 리스트 정렬 방법
PLS_NONE			= 0
PLS_STATEGREATER	= 1
PLS_STATELESS		= 2
PLS_NAMEGREATER		= 3
PLS_NAMELESS		= 4
PLS_ATTRGREATER		= 5
PLS_ATTRLESS		= 6
PLS_END				= 7

--FontFlag
XTF_OUTLINE			= 8
XTF_ALIGN_CENTER	= 16
XTF_ALIGN_RIGHT		= 32

-- Emporia Key
EMPORIA_KEY_NONE			= 0-- 소유한 엠포리아가 없다.
EMPORIA_KEY_MINE			= 1-- 소유한 엠포리아가 있다.
EMPORIA_KEY_BATTLERESERVE	= 2--엠포리아 쟁탈전을 예약했다.
EMPORIA_KEY_BATTLECONFIRM	= 3--엠포리아 쟁탈전을 예약해서 전쟁확정
EMPORIA_KEY_BATTLE_ATTACK	= 4--엠포리아 쟁탈전 진행중(공격)
EMPORIA_KEY_BATTLE_DEFENCE	= 5--엠포리아 쟁탈전 진행중(수비)

-- Emporia UI State
EMUI_ENTRANCE_BOARD					= 0
EMUI_STATE_EMPORIA_STATUSBOARD		= 1
EMUI_STATE_EMPORIA_HELP				= 2

-- ESkillFireType
EFireType_Normal				= 0		-- 단발성 보통 공격
EFireType_Dur_GoalPos			= 1		-- GoalPos 갈때까지 Fire 상태 유지 하는 스킬
EFireType_Dur_Time				= 2		-- 일정시간 Fire 상태 유지 하는 스킬 (유지시간은 SkillDef::GetAbil(AT_MAINTENANCE_TIME))
EFireType_Moving				= 4		-- Fire 동안 이동 가능(AI Unit 자신이 이동)(이동속도는 SkillDef::GetAbil(AT_R_MOVESPEED))
EFireType_Projectile			= 8		-- 발사체 발사하는 것
EFireType_TimeDelay				= 16	-- Fire 이후 얼마후에 Damage주는 스킬(폭발형같은 경우)(유지시간은 SkillDef::GetAbil(AT_MAINTENANCE_TIME))
EFireType_JumpGoalPos			= 32	-- Casting이후, Fire 이전에 GoalPos로 순간이동해야 한다.(점프해서바닥찍기 공격같은 경우 순간이동 필요)
EFireType_AttackToGoalPos		= 64	-- GoalPos 위치를 중심으로 공격범위가 설정된다.
EFireType_KeepVisionVector		= 128	-- Fire할 때 보는 방향을 유지한다.
EFireType_Ani_Moving			= 256	-- Ani Fire 동안 이동 가능(AI Unit 자신이 이동)(도착은 AT_MAINTENANCE_TIME3에 따라 달라짐)

--무기 타입
IT_FST = 0;
IT_SWORD = 1;
IT_BTS = 2;
IT_STF = 3;
IT_SPR = 4;
IT_BOW = 5;
IT_CROSSBOW = 6;
IT_CLAW = 7;
IT_KAT = 8;
IT_SPECIAL = 9;
IT_JOB_TOOL = 10;
IT_GUN_STAFF = 11;
IT_GLOVE = 12;
IT_UNKNOWN = 100;

--Window settings by reOiL
WT_WINDOW		= 0x0;
WT_BORDLESS		= 0x1;
WT_FULLSCREEN	= 0x2;
WT_MAX			= 0x3;

-- Projectile MovingType
MT_STRAIGHTLINE		= 0		-- 직선 이동(디폴트)
MT_SIN_CURVELINE	= 1		-- 사인곡선 이동
MT_BEZIER4_SPLINE	= 2		-- 베지어 곡선 이동
MT_HOMMING			= 3
		
DEGTORAD = math.pi/180.0
RADTODEG = 180.0/math.pi
-- Basic Skill Set 
-- 스킬번호, 딜레이(1/10초), 반복횟수
FIGHTER_1 = {}
FIGHTER_1[1] = {["SKILL"]= 101100601, ["CHECK_SKILL"]= 101100601, ["DELAY"]= 0, ["COUNT"]= 1 } 	--띄우기, 0.1초, 1회
FIGHTER_1[2] = {["SKILL"]= 101000801, ["CHECK_SKILL"]= 101000801, ["DELAY"]= 1, ["COUNT"]= 1 }	--스톰블레이드, 0초, 1회
FIGHTER_2 = {}
FIGHTER_2[1] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 0, ["COUNT"]= 1 }	--점프, 0.5초, 1회
FIGHTER_2[2] = {["SKILL"]= 101100703, ["CHECK_SKILL"]= 101100703, ["DELAY"]= 5, ["COUNT"]= 1 }	--내려찍기(3레벨), 0.2초, 1회
FIGHTER_2[3] = {["SKILL"]= 101000801, ["CHECK_SKILL"]= 101000801, ["DELAY"]= 2, ["COUNT"]= 1 }	--스톰블레이드, 0초, 1회

MAGICIAN_1 = {}
MAGICIAN_1[1] = {["SKILL"]= 102000101, ["CHECK_SKILL"]= 102000101, ["DELAY"]= 0, ["COUNT"]= 1 }	--포트리스, 0초, 1회
MAGICIAN_1[2] = {["SKILL"]= 103100501, ["CHECK_SKILL"]= 103100501, ["DELAY"]= 0, ["COUNT"]= 1 }	--포인트 버스터, 0초, 1회
MAGICIAN_2 = {}
MAGICIAN_2[1] = {["SKILL"]= 102000101, ["CHECK_SKILL"]= 102000101, ["DELAY"]= 0, ["COUNT"]= 1 }	--포트리스, 0초, 1회,
MAGICIAN_2[2] = {["SKILL"]= 103100501, ["CHECK_SKILL"]= 103100501, ["DELAY"]= 0, ["COUNT"]= 1 }	--포인트 버스터, 0초, 1회
MAGICIAN_2[3] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 0, ["COUNT"]= 1 }	--점프, 0.4초, 1회
MAGICIAN_2[4] = {["SKILL"]= 103100201, ["CHECK_SKILL"]= 103100201, ["DELAY"]= 4, ["COUNT"]= 1 }	--운석낙하, 0초, 1회

ARCHER_1 = {}
ARCHER_1[1] = {["SKILL"]= 102101801, ["CHECK_SKILL"]= 102101801, ["DELAY"]= 0, ["COUNT"]= 1 }	--라이징 애로우, 0.2초, 1회
ARCHER_1[2] = {["SKILL"]= 102101301, ["CHECK_SKILL"]= 102101301, ["DELAY"]= 2, ["COUNT"]= 5 }	--대공사격, 0초, 5회
ARCHER_2 = {}
ARCHER_2[1] = {["SKILL"]= 102101801, ["CHECK_SKILL"]= 102101801, ["DELAY"]= 0, ["COUNT"]= 1 }	--라이징 애로우, 0.2초, 1회
ARCHER_2[2] = {["SKILL"]= 102101301, ["CHECK_SKILL"]= 102101301, ["DELAY"]= 2, ["COUNT"]= 7 }	--대공사격, 0초, 7회
ARCHER_2[3] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 0, ["COUNT"]= 1 }	--점프, 0.4초, 1회
ARCHER_2[4] = {["SKILL"]= 102100301, ["CHECK_SKILL"]= 102100301, ["DELAY"]= 4, ["COUNT"]= 1 }	--내려쏘기, 0초, 1회

THIEF_1 = {}
THIEF_1[1] = {["SKILL"]= 103202401, ["CHECK_SKILL"]= 103202401, ["DELAY"]= 0, ["COUNT"]= 1 }	--띄우기, 0초, 1회
THIEF_1[2] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 0, ["COUNT"]= 1 }	--점프, 0.3초, 1회
THIEF_1[3] = {["SKILL"]= 103200101, ["CHECK_SKILL"]= 103201901, ["DELAY"]= 4, ["COUNT"]= 5 }	--공중난타, 0초, 5회
THIEF_2 = {}
THIEF_2[1] = {["SKILL"]= 103202401, ["CHECK_SKILL"]= 103202401, ["DELAY"]= 0, ["COUNT"]= 1 }	--띄우기, 0초, 1회
THIEF_2[2] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 0, ["COUNT"]= 1 }	--점프, 0.3초, 1회
THIEF_2[3] = {["SKILL"]= 103200101, ["CHECK_SKILL"]= 103201901, ["DELAY"]= 4, ["COUNT"]= 5 }	--공중난타, 0.2초, 5회
THIEF_2[4] = {["SKILL"]= 103202601, ["CHECK_SKILL"]= 103202601, ["DELAY"]= 2, ["COUNT"]= 1 }	--내려찍기, 0초, 1회

SHAMAN_1 = {}
SHAMAN_1[1] = {["SKILL"]= 300100201, ["CHECK_SKILL"]= 300100201, ["DELAY"]= 0, ["COUNT"]= 1 }	--그라운드 피스톤, 그라운드 피스톤, 0초, 1회
SHAMAN_1[2] = {["SKILL"]= 300100601, ["CHECK_SKILL"]= 300100601, ["DELAY"]= 3, ["COUNT"]= 1 }	--에어스프레쉬, 에어스프레쉬, 0.3초, 1회

SHAMAN_2 = {}
SHAMAN_2[1] = {["SKILL"]= 300100201, ["CHECK_SKILL"]= 300100201, ["DELAY"]= 0, ["COUNT"]= 1 }	--그라운드 피스톤, 그라운드 피스톤, 0초, 1회
SHAMAN_2[2] = {["SKILL"]= 300100601, ["CHECK_SKILL"]= 300100601, ["DELAY"]= 3, ["COUNT"]= 1 }	--에어스프레쉬, 에어스프레쉬, 0.3초, 1회
SHAMAN_2[3] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 1, ["COUNT"]= 1 }	--점프, 점프, 0.1초, 1회
SHAMAN_2[4] = {["SKILL"]= 300201201, ["CHECK_SKILL"]= 300201201, ["DELAY"]= 4, ["COUNT"]= 1 }	--에어스메쉬, 에어스메쉬, 0.4초, 1회

DOUBLE_FIGHTER_1 = {}
DOUBLE_FIGHTER_1[1] = {["SKILL"]= 400100101, ["CHECK_SKILL"]= 400100101, ["DELAY"]= 0, ["COUNT"]= 1 }	--일도양단, 일도양단, 0초, 1회
DOUBLE_FIGHTER_1[2] = {["SKILL"]= 400100901, ["CHECK_SKILL"]= 400100901, ["DELAY"]= 2, ["COUNT"]= 1 }	--로켓 블로우, 로켓 블로우, 0,2초, 1회
DOUBLE_FIGHTER_1[3] = {["SKILL"]= 400100201, ["CHECK_SKILL"]= 400100201, ["DELAY"]= 1, ["COUNT"]= 1 }	--토네이도스핀, 토네이도스핀, 0,1, 1회

DOUBLE_FIGHTER_2 = {}
DOUBLE_FIGHTER_2[1] = {["SKILL"]= 100001001, ["CHECK_SKILL"]= 100001001, ["DELAY"]= 0, ["COUNT"]= 1 }	--점프, 점프, 0초, 1회
DOUBLE_FIGHTER_2[2] = {["SKILL"]= 400300301, ["CHECK_SKILL"]= 400300301, ["DELAY"]= 3, ["COUNT"]= 1 }	--벙커 버스터, 벙커 버스터, 0,3초, 1회
DOUBLE_FIGHTER_2[3] = {["SKILL"]= 400100201, ["CHECK_SKILL"]= 400100201, ["DELAY"]= 0, ["COUNT"]= 1 }	--토네이도스핀, 토네이도스핀, 0초, 1회
DOUBLE_FIGHTER_2[4] = {["SKILL"]= 400101001, ["CHECK_SKILL"]= 400101001, ["DELAY"]= 5, ["COUNT"]= 1 }	--백열각, 백열각, 0,5초, 1회

BSS_FIGHTER = {}	--전사
BSS_FIGHTER[1] = FIGHTER_1
BSS_FIGHTER[2] = FIGHTER_2

BSS_MAGICIAN = {}	--메지션
BSS_MAGICIAN[1] = MAGICIAN_1
BSS_MAGICIAN[2] = MAGICIAN_2

BSS_ARCHER = {}		--궁수
BSS_ARCHER[1] = ARCHER_1
BSS_ARCHER[2] = ARCHER_2

BSS_THIEF = {}		--도적
BSS_THIEF[1] = THIEF_1
BSS_THIEF[2] = THIEF_2

BSS_SHAMAN = {}		--샤먼
BSS_SHAMAN[1] = SHAMAN_1
BSS_SHAMAN[2] = SHAMAN_2

BSS_DOUBLE_FIGHTER = {}		-- 쌍둥이
BSS_DOUBLE_FIGHTER[1] = DOUBLE_FIGHTER_1
BSS_DOUBLE_FIGHTER[2] = DOUBLE_FIGHTER_2

g_BasicSkillSet = {}
g_BasicSkillSet[CT_FIGHTER] = BSS_FIGHTER
g_BasicSkillSet[CT_MAGICIAN] = BSS_MAGICIAN
g_BasicSkillSet[CT_ARCHER] = BSS_ARCHER
g_BasicSkillSet[CT_THIEF] = BSS_THIEF
g_BasicSkillSet[CT_SHAMAN] = BSS_SHAMAN
g_BasicSkillSet[CT_DOUBLE_FIGHTER] = BSS_DOUBLE_FIGHTER

g_bLearnedBasicSkillSet = {}
g_bLearnedBasicSkillSet[31] = false
g_bLearnedBasicSkillSet[32] = false

g_bReadyBasicSkillSet = {}
g_bReadyBasicSkillSet[31] = false
g_bReadyBasicSkillSet[32] = false

INTERACTIVE_EMOTION_TARGET_GUID_INDEX = 100825	--2010.08.25 인터렉티브 이모션(왈츠) 대상 GUID를 저장하는 Actor의 인덱스


-------------------------------------------------- 밀어내기 시작 --------------------------------------------------
PUSH_TYPE_CIRCLE = 1;	-- 캐릭터 중심으로부터 원으로 퍼지듯
PUSH_TYPE_FRONT = 2;	-- 캐릭터가 바라보는 방향으로
PUSH_TYPE_DIR = 3;		-- 특정 방향으로
PUSH_TYPE_BACK = 4;		-- 캐릭터가 바라보는 반대 방향으로
PUSH_TYPE_TELEPORT_BACK = 5; -- 캐릭터가 바라보는 방향으로 순간이동 (DISTANCE와 방향에 영향을 받음)

-----------피격자 밀려나기 [스킬번호] = { [밀어내기 타입], [특정 방향일 경우, 방향벡터], [가속도], [밀어내는 거리] }
g_ListPushSkill={}
----한손검
g_ListPushSkill[9002110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9002210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=20}
g_ListPushSkill[9002310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListPushSkill[9002410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=35}
g_ListPushSkill[9002510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=55}

g_ListPushSkill[9002450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=55}

g_ListPushSkill[9002340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListPushSkill[9002350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9002360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}

g_ListPushSkill[9002540] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=65}
g_ListPushSkill[9002640] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9002230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListPushSkill[9002240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=85}
g_ListPushSkill[9002250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9002120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9002130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9002140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListPushSkill[9002150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListPushSkill[9002160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
--대쉬
g_ListPushSkill[9002710] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListPushSkill[9002810] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListPushSkill[9002910] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9002010] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=200, ["ACCEL"]= 250, ["DISTANCE"]=60}

g_ListPushSkill[9002940] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9002830] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9002840] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9002720] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9002730] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListPushSkill[9002740] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}
----양손검
g_ListPushSkill[9003110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=35}
g_ListPushSkill[9003210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=35}
g_ListPushSkill[9003310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}
g_ListPushSkill[9003410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9003450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9003340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=120}
g_ListPushSkill[9003350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003351] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003352] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003353] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003354] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003355] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}

g_ListPushSkill[9003230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9003240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9003250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=90}

g_ListPushSkill[9003120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListPushSkill[9003130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=10}
g_ListPushSkill[9003140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=10}
g_ListPushSkill[9003150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=10}

g_ListPushSkill[9003430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9003530] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

----대쉬
g_ListPushSkill[9003710] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003810] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003910] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003010] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}

g_ListPushSkill[9003720] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=40}
g_ListPushSkill[9003730] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9003740] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9003830] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 10, ["DISTANCE"]=50}
g_ListPushSkill[9003840] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}

----활
g_ListPushSkill[9022110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9022340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9022230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9022540] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022550] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9022130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9022160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9022150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

----석궁



---- 난사 

g_ListPushSkill[9024110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9024210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9024310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9024410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9024510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

---- 더블샷 

g_ListPushSkill[9025110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9025210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9025310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9025410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9025510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}




g_ListPushSkill[9023110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9023340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9023230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9023540] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023550] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9023130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9023160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListPushSkill[9023150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}


----클로
g_ListPushSkill[9032110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9032210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=45}
g_ListPushSkill[9032310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9032410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9032510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListPushSkill[9032450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}
g_ListPushSkill[9032460] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListPushSkill[9032340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=12}
g_ListPushSkill[9032350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=9}
g_ListPushSkill[9032360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=21}

g_ListPushSkill[9032230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9032240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListPushSkill[9032250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListPushSkill[9032260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListPushSkill[9032270] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=50}

g_ListPushSkill[9032120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=45}
g_ListPushSkill[9032130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9032140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9032150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9032160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=120}
g_ListPushSkill[9032170] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}

g_ListPushSkill[9032650] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

----카타르

g_ListPushSkill[9033110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListPushSkill[9033210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=45}
g_ListPushSkill[9033310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9033410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9033510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListPushSkill[9033450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}
g_ListPushSkill[9033460] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListPushSkill[9033340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=12}
g_ListPushSkill[9033350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=9}
g_ListPushSkill[9033360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=21}

g_ListPushSkill[9033230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9033240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListPushSkill[9033250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListPushSkill[9033260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListPushSkill[9033270] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=50}

g_ListPushSkill[9033120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=45}
g_ListPushSkill[9033130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9033140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9033150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9033160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=120}
g_ListPushSkill[9033170] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}

g_ListPushSkill[9033650] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}


----스태프
g_ListPushSkill[9014110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9015110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}


g_ListPushSkill[9012120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9012220] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9012320] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=95}
g_ListPushSkill[9012420] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9012430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=79}

g_ListPushSkill[9012340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9012440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9012450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9012260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9012360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9012370] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}


----스피어

g_ListPushSkill[9013120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9013220] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9013320] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=95}
g_ListPushSkill[9013420] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9013430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=79}

g_ListPushSkill[9013340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9013440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9013450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9013260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9013360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9013370] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}


----건틀릿
g_ListPushSkill[9512210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListPushSkill[9512450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}

g_ListPushSkill[9512340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListPushSkill[9512230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=20}
g_ListPushSkill[9512240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9512250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}

g_ListPushSkill[9512120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}
g_ListPushSkill[9512150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

----건틀릿(퓨전)
g_ListPushSkill[9512316] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512416] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512516] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListPushSkill[9512456] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListPushSkill[9512356] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListPushSkill[9512256] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}

g_ListPushSkill[9512136] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListPushSkill[9512156] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}

----건스테프

g_ListPushSkill[9502120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9502220] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListPushSkill[9502320] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=95}
g_ListPushSkill[9502420] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9502430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=79}

g_ListPushSkill[9502340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListPushSkill[9502440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9502450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9502260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=80}
g_ListPushSkill[9502360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListPushSkill[9502370] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- DB에 있는 값들
--mon_push_atot
g_ListPushSkill[3542126] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542270] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542329] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=260}

--mon_push_back
g_ListPushSkill[3401403] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3401704] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3401705] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3523902] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3525003] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3530201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3540506] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3541007] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3571304] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[3571407] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4019106] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4023109] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4038103] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4039101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4039102] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4039103] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4039104] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4039105] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4039106] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4602203] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4700605] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4701305] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=450, ["ACCEL"]= 1, ["DISTANCE"]=180}

--mon_push_back_long
g_ListPushSkill[3542119] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=650, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542241] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=650, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542242] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=650, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542243] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=650, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[3542332] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=650, ["ACCEL"]= 1, ["DISTANCE"]=260}
g_ListPushSkill[4023104] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=650, ["ACCEL"]= 1, ["DISTANCE"]=260}

--mon_push_back_short
g_ListPushSkill[1100021] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[1100122] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[2100122] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[2100222] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[2100322] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[2100422] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[2100522] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[2100622] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3542202] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3542203] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3570804] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4023100] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4041101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4500203] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4500909] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4701500] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4701502] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4701504] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[47015067] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[47015068] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=250, ["ACCEL"]= 1, ["DISTANCE"]=100}

--mon_pushmiddle
g_ListPushSkill[3571303] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4013103] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=180}
g_ListPushSkill[4700703] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=180}

--mon_pushshort
g_ListPushSkill[3000302] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3000303] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3000312] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3401503] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3401504] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3530800] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3542129] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3542210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3570610] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[3570620] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4011101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4013105] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4014202] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4016101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4019100] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4021100] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4021103] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4021107] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4025106] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4044100] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4500201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}
g_ListPushSkill[4500202] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=100}

--a_push_back_long
g_ListPushSkill[3000401] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[101300201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[101300301] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[101300401] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[101300501] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[104303201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[104302901] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[104303001] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}
g_ListPushSkill[104303101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1, ["DISTANCE"]=120}

--a_push_back_long1
g_ListPushSkill[400100801] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400100802] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400100803] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400100804] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400100805] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400405] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400501] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400502] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400503] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400504] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400401] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400402] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400403] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400404] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[400400505] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}

--a_push_back_long2
g_ListPushSkill[300301901] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[300301902] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}
g_ListPushSkill[300301903] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=225, ["ACCEL"]= 1, ["DISTANCE"]=90}

--a_push_back_short
g_ListPushSkill[3000301] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[400100701] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[400100702] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[400100703] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[400100704] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[400100705] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000210101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000210102] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000210103] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000210104] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000210105] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[101102101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[101102201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[101102301] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000810205] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[101102401] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[104302501] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[104302601] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[104302701] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[104302801] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303001] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303002] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303003] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303004] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303005] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303202] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303203] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303204] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303205] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303301] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303302] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303303] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303304] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303305] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303401] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303402] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303403] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303404] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303405] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303501] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303502] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303503] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303504] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303505] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303601] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303602] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303603] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303604] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303605] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303701] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303702] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303703] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303704] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303705] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303801] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303802] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303803] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303804] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[300303805] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[400201601] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000810201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000810202] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000810203] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}
g_ListPushSkill[2000810204] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=125, ["ACCEL"]= 1, ["DISTANCE"]=50}

--a_push_back_short1
g_ListPushSkill[400201101] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400201102] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400201103] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400201104] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400201105] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400301002] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400301003] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400301004] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[400301005] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[300302301] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[300302302] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[300302303] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[300302304] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}
g_ListPushSkill[300302305] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}

--a_push_back_short2
g_ListPushSkill[400301001] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=75, ["ACCEL"]= 1, ["DISTANCE"]=30}

--a_push_back1
g_ListPushSkill[9003410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}
g_ListPushSkill[400200201] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}
g_ListPushSkill[400200202] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}
g_ListPushSkill[400200203] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}
g_ListPushSkill[400200204] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}
g_ListPushSkill[400200205] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}

--a_push_back2
g_ListPushSkill[400000701] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 1, ["DISTANCE"]=60}
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-----------공격자 따라가기 [스킬번호] = { [따라가기 타입], [특정 방향일 경우, 방향벡터], [가속도], [밀어내는 거리] }
g_ListChaseSkill={}
----한손검
g_ListChaseSkill[9002110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9002210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}
g_ListChaseSkill[9002310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListChaseSkill[9002410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9002510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 130, ["DISTANCE"]=40}

g_ListChaseSkill[9002450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=40}

g_ListChaseSkill[9002340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=40}
g_ListChaseSkill[9002350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=50}
g_ListChaseSkill[9002360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=500, ["ACCEL"]= 500, ["DISTANCE"]=80}

g_ListChaseSkill[9002540] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=20}
g_ListChaseSkill[9002640] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=50}

g_ListChaseSkill[9002230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=50}
g_ListChaseSkill[9002240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 130, ["DISTANCE"]=50}
g_ListChaseSkill[9002250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=65}

g_ListChaseSkill[9002120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 200, ["DISTANCE"]=50}
g_ListChaseSkill[9002130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 200, ["DISTANCE"]=20}
g_ListChaseSkill[9002140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListChaseSkill[9002150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=10}
g_ListChaseSkill[9002160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}
----대쉬
g_ListChaseSkill[9002710] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}
g_ListChaseSkill[9002810] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=60}
g_ListChaseSkill[9002910] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=40}
g_ListChaseSkill[9002010] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 150, ["DISTANCE"]=70}

g_ListChaseSkill[9002940] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 400, ["DISTANCE"]=70}

g_ListChaseSkill[9002830] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListChaseSkill[9002840] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=80}

g_ListChaseSkill[9002720] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=40}
g_ListChaseSkill[9002730] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=20}
g_ListChaseSkill[9002740] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=50}
----양손검
g_ListChaseSkill[9003110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9003210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=35}
g_ListChaseSkill[9003310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9003410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}

g_ListChaseSkill[9003450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 130, ["DISTANCE"]=70}

g_ListChaseSkill[9003340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 500, ["DISTANCE"]=70}
g_ListChaseSkill[9003350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003351] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003352] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003353] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003354] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003355] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}

g_ListChaseSkill[9003230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 200, ["DISTANCE"]=30}
g_ListChaseSkill[9003240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 250, ["DISTANCE"]=40}
g_ListChaseSkill[9003250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=350, ["ACCEL"]= 650, ["DISTANCE"]=250}

g_ListChaseSkill[9003120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9003140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9003150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListChaseSkill[9003430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 100, ["DISTANCE"]=20}
g_ListChaseSkill[9003530] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 100, ["DISTANCE"]=20}
----대쉬
g_ListChaseSkill[9003710] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003810] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003910] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003010] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}

g_ListChaseSkill[9003720] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=40}
g_ListChaseSkill[9003730] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}
g_ListChaseSkill[9003740] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}

g_ListChaseSkill[9003830] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 10, ["DISTANCE"]=60}
g_ListChaseSkill[9003840] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=70}

----활
g_ListChaseSkill[9022110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 120, ["DISTANCE"]=13}
g_ListChaseSkill[9022210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 120, ["DISTANCE"]=13}
g_ListChaseSkill[9022310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=10}
g_ListChaseSkill[9022410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=8}
g_ListChaseSkill[9022440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=21}

g_ListChaseSkill[9022340] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 120, ["DISTANCE"]=20}

g_ListChaseSkill[9022230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 300, ["DISTANCE"]=120}
g_ListChaseSkill[9022240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListChaseSkill[9022250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9022540] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=120}
g_ListChaseSkill[9022120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9022130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1000, ["ACCEL"]= 800, ["DISTANCE"]=180}
g_ListChaseSkill[9022140] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=150, ["ACCEL"]= 180, ["DISTANCE"]=120}
g_ListChaseSkill[9022160] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=500, ["ACCEL"]= 400, ["DISTANCE"]=120}

g_ListChaseSkill[9022150] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=200, ["ACCEL"]= 300, ["DISTANCE"]=150}

g_ListChaseSkill[102101901] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=500, ["ACCEL"]= 500, ["DISTANCE"]=100}
g_ListChaseSkill[102101601] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=500, ["ACCEL"]= 500, ["DISTANCE"]=100}

----석궁


---- 난사 

g_ListChaseSkill[9024110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=5}
g_ListChaseSkill[9024210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=8}
g_ListChaseSkill[9024310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=20}
g_ListChaseSkill[9024410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=8}
g_ListChaseSkill[9024510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}

---- 더블샷 

g_ListChaseSkill[9025110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=5}
g_ListChaseSkill[9025210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=8}
g_ListChaseSkill[9025310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=20}
g_ListChaseSkill[9025410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=8}
g_ListChaseSkill[9025510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}

g_ListChaseSkill[9023110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 120, ["DISTANCE"]=5}
g_ListChaseSkill[9023210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 120, ["DISTANCE"]=8}
g_ListChaseSkill[9023310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=20}
g_ListChaseSkill[9023410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=8}
g_ListChaseSkill[9023440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}

g_ListChaseSkill[9023340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9023230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 300, ["DISTANCE"]=130}
g_ListChaseSkill[9023240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=30}
g_ListChaseSkill[9023250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9023540] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=120}
g_ListChaseSkill[9023120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9023130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1000, ["ACCEL"]= 800, ["DISTANCE"]=180}
g_ListChaseSkill[9023140] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 130, ["DISTANCE"]=120}
g_ListChaseSkill[9023160] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=100, ["ACCEL"]= 130, ["DISTANCE"]=120}

g_ListChaseSkill[9023150] = { ["TYPE"]= PUSH_TYPE_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=400, ["ACCEL"]= 300, ["DISTANCE"]=150}


----클로
g_ListChaseSkill[9032110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9032210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}
g_ListChaseSkill[9032310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListChaseSkill[9032410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9032510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListChaseSkill[9032450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListChaseSkill[9032460] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=15}

g_ListChaseSkill[9032340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=50}
g_ListChaseSkill[9032350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=19}
g_ListChaseSkill[9032360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=41}

g_ListChaseSkill[9032230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=45}
g_ListChaseSkill[9032240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=50}
g_ListChaseSkill[9032250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListChaseSkill[9032260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListChaseSkill[9032270] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 1600, ["DISTANCE"]=150}

g_ListChaseSkill[9032120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListChaseSkill[9032130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=80}
g_ListChaseSkill[9032140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=80}
g_ListChaseSkill[9032150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=80}
g_ListChaseSkill[9032160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1000, ["DISTANCE"]=30}
g_ListChaseSkill[9032170] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1500, ["DISTANCE"]=50}

g_ListChaseSkill[9032650] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 800, ["DISTANCE"]=100}

----카타르

g_ListChaseSkill[9033110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9033210] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=25}
g_ListChaseSkill[9033310] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListChaseSkill[9033410] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9033510] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListChaseSkill[9033450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=60}
g_ListChaseSkill[9033460] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=15}

g_ListChaseSkill[9033340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=50}
g_ListChaseSkill[9033350] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=19}
g_ListChaseSkill[9033360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=41}

g_ListChaseSkill[9033230] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=45}
g_ListChaseSkill[9033240] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=50}
g_ListChaseSkill[9033250] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListChaseSkill[9033260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 800, ["DISTANCE"]=80}
g_ListChaseSkill[9033270] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1, ["ACCEL"]= 1600, ["DISTANCE"]=150}

g_ListChaseSkill[9033120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListChaseSkill[9033130] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=80}
g_ListChaseSkill[9033140] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 100, ["DISTANCE"]=80}
g_ListChaseSkill[9033150] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 300, ["DISTANCE"]=80}
g_ListChaseSkill[9033160] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1000, ["DISTANCE"]=30}
g_ListChaseSkill[9033170] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 1500, ["DISTANCE"]=50}

g_ListChaseSkill[9033650] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 800, ["DISTANCE"]=100}


----스태프
g_ListChaseSkill[9014110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListChaseSkill[9015110] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListChaseSkill[9012120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9012220] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9012320] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9012420] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9012430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1000, ["ACCEL"]= 800, ["DISTANCE"]=120}

g_ListChaseSkill[9012340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9012440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9012450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListChaseSkill[9012260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListChaseSkill[9012360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListChaseSkill[9012370] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}



----스피어

g_ListChaseSkill[9013120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9013220] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9013320] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9013420] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9013430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1000, ["ACCEL"]= 800, ["DISTANCE"]=120}

g_ListChaseSkill[9013340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9013440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9013450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListChaseSkill[9013260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListChaseSkill[9013360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListChaseSkill[9013370] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListChaseSkill[103102001] = { ["TYPE"]= PUSH_TYPE_TELEPORT_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=3000, ["ACCEL"]= 400, ["DISTANCE"]=200}




----건스테프

g_ListChaseSkill[9502120] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=0}
g_ListChaseSkill[9502220] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9502320] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9502420] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9502430] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=1000, ["ACCEL"]= 800, ["DISTANCE"]=120}

g_ListChaseSkill[9502340] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}
g_ListChaseSkill[9502440] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=50}

g_ListChaseSkill[9502450] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=40}

g_ListChaseSkill[9502260] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}
g_ListChaseSkill[9502360] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListChaseSkill[9502370] = { ["TYPE"]= PUSH_TYPE_FRONT, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=300, ["ACCEL"]= 400, ["DISTANCE"]=39}

g_ListChaseSkill[103102001] = { ["TYPE"]= PUSH_TYPE_TELEPORT_BACK, ["DIR_X"]=0, ["DIR_Y"]=0, ["DIR_Z"]=0, ["VELOCITY"]=3000, ["ACCEL"]= 400, ["DISTANCE"]=200} 


----건틀릿



---맵 무브 타입
MMET_None		= 0
MMET_Normal		= 1
MMET_PartyWarp	= 2
MMET_CoupleWarp	= 3
MMET_GM			= 4
MMET_GM_ReCall	= 5
MMET_ClientEnd	= 6

MMET_Login		= 7
MMET_Mission	= 8
MMET_MissionDefence8 = 9
MMET_SuperGround = 10

MMET_Failed		= 11
MMET_TimeOut	= 12
MMET_PublicChannelFailed = 13

-- PvP 관련
MMET_PvP		= 14
MMET_BackToPvP	= 15-- PvP에 입장해 있는 상태이고 Lobby로 돌려보낸다.
MMET_KickToPvP	= 16

MMET_GoToPublicGround = 17	-- Public Channel Ground로 이동한다.
MMET_GoTopublicGroundParty = 18
MMET_BackToChannel = 19		-- Public Channel에서 Channel로 돌아간다.

MMET_GoToPublic_PartyOrder = 20
MMET_BATTLESQUARE = 21		-- Public Channel Ground로 이동

--	실패 관련
MMET_Failed_Access = 22	-- 접근 권한이 없습니다.
MMET_Failed_Full = 23	-- 유저가 모두 찼습니다.
MMET_Failed_Creating = 24 --그라운드 생성중
MMET_Failed_JoinTime = 25 --접근시간이 아님
MMET_Failed_Login = 26	--로그인 실패

MMET_RidingPet_Transport = 50 --라이딩펫 스킬로 전송타워 이동시 (단지 별도의 로딩화면 표시를 알리기 위해 사용)

-- 아이템교환 관련
EGM_NONE		= 0
EGM_ANTIQUE		= 6
EGM_SOCKETCARD	= 7
EGM_ELUNIUM		= 11
EGM_VALKYRIE	= 12
EGM_PURESILVER_KEY = 13

function MovingActor( kActor, action)
	if nil==kActor or true==kActor:IsNil() then
		return false
	end
	if nil==action or true==action:IsNil() then
		return false
	end
	
	local AttackActor = action:GetEffectCaster()
	if nil==AttackActor or true==AttackActor:IsNil() then
		return false
	end

	if true == AttackActor:GetAction():IsNil() then
		return false
	end
	
	local iSkillNo = AttackActor:GetAction():GetActionNo()
	local iParentSkillNo = AttackActor:GetAction():GetParentSkillNo()
	if 0 ~= iParentSkillNo then
		iSkillNo = iParentSkillNo;
	end
	--ODS("MovingActor - AttackActor's SkillNo : "..iSkillNo.."\n", false,3851)
	local kPushSkill = g_ListPushSkill[iSkillNo]
	if nil==kPushSkill then
		return false
	end
	
	local iType = kPushSkill["TYPE"]
	local fVelocity = kPushSkill["VELOCITY"]
	local fAccel = kPushSkill["ACCEL"]
	local fDistance = kPushSkill["DISTANCE"]
	
	local kMovingDir = AttackActor:GetLookingDir()
	
	if PUSH_TYPE_CIRCLE == iType then
		kMovingDir = kActor:GetPos()
		kMovingDir:Subtract( AttackActor:GetPos() )
		kMovingDir:Unitize()
	elseif PUSH_TYPE_FRONT == iType then
		kMovingDir:Unitize()
	elseif PUSH_TYPE_BACK == iType then
		kMovingDir:Multiply(-1)
		kMovingDir:Unitize()
	elseif PUSH_TYPE_DIR == iType then
		kMovingDir = Point3( kPushSkill["DIR_X"], kPushSkill["DIR_Y"], kPushSkill["DIR_Z"] )
		kMovingDir:Unitize()
	end
	kActor:PushActorDir(kMovingDir, fDistance, fVelocity, fAccel);
	return true;
	
end

-- 공격 액션에서 콤보에 의한 이동값이 있는지 체크하고 이동시키는게 필요
function ChaseActor( kActor, action)
	if nil==kActor or true==kActor:IsNil() then
		return false
	end
	if nil==action or true==action:IsNil() then
		return false
	end
	
	local iSkillNo = kActor:GetAction():GetActionNo()
	local iParentSkillNo = action:GetParentSkillNo()
	if 0 ~= iParentSkillNo then
		iSkillNo = iParentSkillNo;
	end
	--ODS("ChaseActor - kActor's SkillNo : "..iSkillNo.."\n", false,3851)
	local kChaseSkill = g_ListChaseSkill[iSkillNo]
	if nil==kChaseSkill then
		return false
	end
	
	local iType = kChaseSkill["TYPE"]
	local fVelocity = kChaseSkill["VELOCITY"]
	local fAccel = kChaseSkill["ACCEL"]
	local fDistance = kChaseSkill["DISTANCE"]
	
	local kMovingDir = kActor:GetLookingDir()
	
	local bAlreadyMove = false;
	--PUSH_TYPE_CIRCLE는 따로 이동이 없다. 필요하다면 차후 추가할 듯
	if PUSH_TYPE_FRONT == iType then
		kMovingDir:Unitize()
	elseif PUSH_TYPE_BACK == iType then
		kMovingDir:Multiply(-1)
		kMovingDir:Unitize()
	elseif PUSH_TYPE_DIR == iType then
		kMovingDir = Point3( kChaseSkill["DIR_X"], kChaseSkill["DIR_Y"], kChaseSkill["DIR_Z"] )
		kMovingDir:Unitize()
	elseif PUSH_TYPE_TELEPORT_BACK == iType then
		kMovingDir:Unitize()
		-- 여기서 거리 검사 하고 
		local fMaxMoveDist = fDistance	--최대 이동거리
		kMovingDir:Multiply(-1)	--플레이어 바라보는 반대방향
		local kPos = kActor:GetPos()
		local fHeightLimit = jumpForce * 0.7 --위로 이동 가능한 크기
		kPos:SetZ(kPos:GetZ()+fHeightLimit)
		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
	
		local ptcl = g_world:ThrowRay(kPos, kMovingDir, fMaxMoveDist,3) --뒤로 레이를 쏴서 
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then	-- 이동이 가능하면			
			local kTemp = kMovingDir
			kTemp:Multiply(fMaxMoveDist)
			kTemp:Add(kActor:GetPos())		 -- 뒤로 이동 시키고
			kTemp:SetZ(kTemp:GetZ()+fHeightLimit) -- 조금 위로 올린후에			
			ptcl = g_world:ThrowRay(kTemp, Point3(0,0,-1), 500,3)	-- 레이를 아래로 쏴서
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				--실패하면 위로도 쏴 보고
				ptcl = g_world:ThrowRay(kTemp, Point3(0,0,1),500,3);
				if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				--모두 실패 하면 MoveDelta형식으로 이동
					kMovingDir:Multiply(fDistance)
					kMovingDir:Unitize()
				end
			else
				--바닥을 찾았으면
				ptcl:SetZ(ptcl:GetZ()+30) -- 캐릭터가 서있을 위치를
				--언덕길이라 바로 위치 이동
				local kTargetPos = ptcl
				kActor:SetTranslate(kTargetPos, false)
				bAlreadyMove = true;
			end
		else
			-- 플레이어 뒤로 fMaxMoveDist 거리 이내 뭔가 있으면 MoveDelta형식으로 이동
			kMovingDir:Multiply(fDistance)
			kMovingDir:Unitize()
		end

	end
	
	if PUSH_TYPE_TELEPORT_BACK == iType then
		if false == bAlreadyMove then
			kActor:PushActorDir(kMovingDir, fDistance, fVelocity, fAccel);
		end
	else
		kActor:PushActorDir(kMovingDir, fDistance, fVelocity, fAccel);
	end
	kActor:FreeMove(false)
	return true
end

function IsHavePushActorSkillInfo( iSkillNo )
	local kInfo = g_ListPushSkill[iSkillNo]
	if nil == kInfo then
		return false
	end
	return true
end
function GetPushActorSkillInfo_Type( iSkillNo )
	local kInfo = g_ListPushSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["TYPE"]
end
function GetPushActorSkillInfo_Dir( iSkillNo )
	local kDir = Point3(0,0,0)
	local kInfo = g_ListPushSkill[iSkillNo]
	if nil == kInfo then
		return kDir
	end
	kDir = Point3(kInfo["DIR_X"], kInfo["DIR_Y"], kInfo["DIR_Z"])
	return kDir
end
function GetPushActorSkillInfo_Velocity( iSkillNo )
	local kInfo = g_ListPushSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["VELOCITY"]
end
function GetPushActorSkillInfo_Accel( iSkillNo )
	local kInfo = g_ListPushSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["ACCEL"]
end
function GetPushActorSkillInfo_Distance( iSkillNo )
	local kInfo = g_ListPushSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["DISTANCE"]
end

function IsHaveChaseActorSkillInfo( iSkillNo )
	local kInfo = g_ListChaseSkill[iSkillNo]
	if nil == kInfo then
		return false
	end
	return true
end
function GetChaseActorSkillInfo_Type( iSkillNo )
	local kInfo = g_ListChaseSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["TYPE"]
end
function GetChaseActorSkillInfo_Dir( iSkillNo )
	local kDir = Point3(0,0,0)
	local kInfo = g_ListChaseSkill[iSkillNo]
	if nil == kInfo then
		return kDir
	end
	kDir = Point3(kInfo["DIR_X"], kInfo["DIR_Y"], kInfo["DIR_Z"])
	return kDir
end
function GetChaseActorSkillInfo_Velocity( iSkillNo )
	local kInfo = g_ListChaseSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["VELOCITY"]
end
function GetChaseActorSkillInfo_Accel( iSkillNo )
	local kInfo = g_ListChaseSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["ACCEL"]
end
function GetChaseActorSkillInfo_Distance( iSkillNo )
	local kInfo = g_ListChaseSkill[iSkillNo]
	if nil == kInfo then
		return 0
	end
	return kInfo["DISTANCE"]
end

-------------------------------------------------- 밀어내기 끝 --------------------------------------------------
