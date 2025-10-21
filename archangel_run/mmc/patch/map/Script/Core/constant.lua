-----------------------------------------------
-- Ability Type
-----------------------------------------------
---- UNIT
AT_NONE=0 

AT_LEVEL	= 1
AT_CLASS	= 2
AT_GENDER	= 3
AT_RACE		= 4

AT_HP					= 101
AT_DP					= 102
AT_CP					= 103
AT_MP					= 105
AT_HIT_COUNT	= 108	-- 실제로 공격한 회수 (도적:DoubleAttack에 의해서 Hit수가 가변적으로 변경될 수가 있다.)
AT_HP_RESERVED			= 109	-- 부활할때의 채워지게 될 HP양
AT_MP_RESERVED			= 110	-- 부활할때의 채워지게 될 MP양
AT_REVIVED_BY_OTHER		= 111	-- 다른 사람에 의해 부활되었는가?
AT_ATTR_ATTACK			= 201
AT_ATTR_DEFENCE			= 202
AT_LOCK_HIDDEN_ATTACK	= 203	-- Hidden상태인데 Attack 하더라도 Hidden 상태가 풀리지 않는다.
AT_UNLOCK_HIDDEN_MOVE	= 204	-- Hidden상태인데 Move 하면 Hidden상태가 풀린다.
AT_CALLER_TYPE			= 503
AT_HANDYCAP				= 504
AT_TEAM					= 505
AT_DELAY				= 506
AT_AI_TYPE				= 507
AT_DISTANCE				= 508
AT_EFFECT_SIZE			= 511
AT_EXPERIENCE			= 512
AT_MONEY				= 513
AT_GENERATE_STYLE		= 514
AT_DIE_MOTION			= 517	-- Monster Die Motion
AT_DAMAGE_PUSH_TYPE		= 518	-- Pushback by damage type
AT_END_ATTACK			= 519
AT_SKILL_EXTENDED	= 521	-- 스킬이 버프를 받아서 더욱 강력해진 상태(발사체 개수 변화)
AT_DICE_VALUE			= 525	-- 투사:Roulette 스킬 결과값(주사위값)
AT_REFLECTED_DAMAGE		= 526	-- Reflected Damage (Damage반사) 크기
AT_1ST_ATTACK_ADDED_RATE	= 527	-- 첫번째 공격에 대해서만 추가적인 공격치(최종공격력에 대한 만분율)
AT_PDMG_DEC_RATE_2ND	= 529	-- 물리Damage에 대해서 2차적으로 Damage 감소 Rate (마나쉴드 사용)
AT_MDMG_DEC_RATE_2ND	= 530	-- 마법Damage에 대해서 2차적으로 Damage 감소 Rate (마나쉴드 사용)
AT_CLAW_DBL_ATTK_RATE = 532	-- // 스킬 : Claw 평타 왼속 공격시에 더블어택 발동 확률(만분율)
AT_ADD_MONEY_PER_LV	= 533	-- Monster Level에 따라 추가적인 추가지급 Money Rate량(만분율)
									-- Unit : 도적(소매치기) 스킬은 몬스터 잡을때 추가적인 Money를 지급해야 한다.
AT_ADD_MONEY_RATE		= 534	-- 추가적인 Money Drop이 발생할 확률
AT_MAP_NUM				= 535	-- 아이템 : 마을소환 스크롤일 경우 가야할 마을 Map번호
AT_PORTAL_NUM			= 536	-- 아이템 : 마을소환 스크롤일 경우 가야할 Portal 번호
AT_POSITION_X			= 537	-- 아이템 : 마을소환 스크롤일 경우 가야할 좌표값(X)
AT_POSITION_Y			= 538	-- 아이템 : 마을소환 스크롤일 경우 가야할 좌표값(Y)
AT_POSITION_Z			= 539	-- 아이템 : 마을소환 스크롤일 경우 가야할 좌표값(Z)
AT_PARENT_SKILL_NUM		= 540	-- Skill : Parent Skill number

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

AT_REFLECT_DMG_HP		= 552 -- Refeclted Damage 발생했을때, 맞은넘의 남은 HP 값
--AT_ABS_ADDED_DMG		= 553	-- Unit : 공격할 때, 최종Dmg에 추가적으로 더해지는 Dmg
AT_DMG_DEC_RATE_COUNT	= 554	-- AT_PDMG_DEC_RATE_2ND or AT_MDMG_DEC_RATE_2ND 능력 적용 남은 개수(-1:Unlimited)

AT_SP					= 1001
AT_ADD_EXP_PER			= 1004
AT_ADD_MONEY_PER		= 1005
AT_MAX_TARGETNUM		= 1007
AT_SKILLPOWER_RATE	= 1008	-- Skill의 공격력 Rate
AT_SKILL_MIN_RANGE	= 1009
AT_DROPITEMGROUP		= 2008		-- Drop 하는 아이템 그룹 ID
AT_SKILL_ATT			=	2011	-- Skill 속성 (ESkillAttribute value bitmask value)
ATS_COOL_TIME			=	2012	-- Skill : Cooling Time (새로운 스킬을 사용할 수 있는 최소시간)
AT_ADD_COOL_TIME		=	2013	-- Skill의 전체적인 추가 쿨타임(스킬 기본 쿨타임 + 추가 쿨타임)
AT_MAINSKILL			=	2016	-- passive skill의 Main이 되는 active skill
AT_CASTTYPE				=	2018	-- ESkillCastType (Instant/Maintenance/CastShot)
AT_CAST_TIME			=	2019	-- 주문을 외우는데 필요한 시간
AT_ATT_DELAY			=	2020	-- 스킬쓰고 나서 Delay 시간
AT_ADD_CAST_TIME		=	2021	-- 주문을 외우는데 필요한 추가 시간(캐스팅 타임 + 추가 캐스팅 타임)
AT_TYPE					=	2022	-- Skill type
AT_WEAPON_LIMIT			=	2023	-- Weapon limit
AT_CLASSLIMIT			=	2024
AT_LEVELLIMIT			=	2025
AT_STATELIMIT			=	2026
AT_MAINTENANCE_TIME		=	2027	-- Maintenance skill일 경우 Maintenance time
AT_NEED_SP				=	2028
AT_NAMENO				= 2030			-- TB_DefRes 참조값
AT_ADDHEAL		= 2037	-- 응급치료 스킬 사용시 추가적인 HP 회복량
AT_ANIMATION_TIME		= 2039 -- 스킬의 애니메이션이 타격할 때까지의 시간
AT_DAMAGED_DELAY		= 2042 -- Monster : Damage를 입은 뒤 딜레이
AT_EFFECTNUM1			= 2051	-- 스킬에 추가적인 Effect Number #1
AT_EFFECTNUM2			= 2052	-- 스킬에 추가적인 Effect Number #2
AT_EFFECTNUM3			= 2053	-- 스킬에 추가적인 Effect Number #3
AT_EFFECTNUM4			= 2054	-- 스킬에 추가적인 Effect Number #4
AT_CUSTOMDATA1			= 2061	-- Customized data #1
AT_DROPITEMRES			= 2071	-- Drop Item Icon Resource Number (TB_DefRes)
AT_DAM_EFFECT_S			= 2072	-- Damage Motion Effect Num (Small Size)
AT_DAM_EFFECT_M			= 2073	-- Damage Motion Effect Num (Middle Size)
AT_DAM_EFFECT_L			= 2074	-- Damage Motion Effect Num (Large Size)
AT_PERCENTAGE		= 2075	-- Skill [Charging Time]
AT_ADDVALUE = 2076	-- 추가적인 값
AT_PERCENTAGE2 = 2077 -- Percentage Value #2
AT_PHY_DMG_PER = 2080 -- Skill : Phyical Damage에 대한 Percentage 값
AT_MAGIC_DMG_PER	= 2081 -- Skill : Magic Damage에 대한 Percentage 값
AT_RANGE_TYPE			= 2082 -- Skill : Range를 어떤것을 사용할 것인가? (0:SkillRange, 2:ItemRange, 3:Skill+Item, 4:Unit(AT_ATTACK_RANGE))
AT_SPECIALABIL		= 2083 -- Skill or Effect : 부수적으로 생기는 특별한 능력 Ability Number (예:침묵하기)
AT_CASTER_MOVERANGE		= 2086	-- Skill : Caster가 전진하는 길이
AT_TARGET_MOVERANGE		= 2087	-- Skill : Target이 밀리는 길이
AT_MP_BURN				= 2084
AT_GOD_TIME				= 2085
AT_DMG_CONSUME_MP		= 2088	-- Damage 받으면 HP대신 MP를 흡수하는 량(AT_C_MAGIC_DMG_DEC 값이 0이면 효과없음)
AT_COUNT				= 2089	-- 개수를 나타낼때 사용하면 좋겠다.

-- Monster Ability
AT_WALK_TYPE		= 3001
AT_JOB				= 3002
AT_DETECT_RANGE		= 3003
AT_CHASE_RANGE		= 3004
AT_MAX_SKILL_NUM	= 3005
AT_NORMAL_SKILL_ID	= 3006 -- 디폴트 공격
AT_MOVE_RANGE		= 3009
AT_MON_SKILL_01		= 3010
AT_MON_SKILL_RATE_01	= 3020
--AT_MONSTER_TYPE		= 3031
AT_NORMAL_SKILL_RATE	= 3032
AT_DIE_EXP = 3033
AT_UNIT_SIZE			= 3034	-- 몬스터 크기에 따라 Damage받는 모습을 다르게 하기 위한 값
AT_SKILL_SUMMON_MONBAG	= 3038
AT_DAMAGEACTION_TYPE	= 3039	-- Damaged Action type (0 = normal, 101 = no damaged delay)
AT_NOT_SEE_PLAYER		= 3043
AT_MANUAL_DIEMOTION = 3044

AT_BARRIER_AMOUNT			= 3062 --데미지를 흡수할 수 있는 양
AT_BARRIER_100PERECNT_COUNT = 3063 --100% 데미지를 흡수하는 베리어 카운터
AT_PREV_BARRIER_100PERECNT_COUNT = 3064 --이전 베리어를 카운터(변화는 것을 체크하기 위해서)
AT_ATROPINE_USE_COUNT = 3065 --아트로핀을 사용한 횟수
AT_SHADOW_COPY_USE	= 3066 --환영분신을 사용중인가?
AT_PENETRATION_COUNT = 3067 --몇개를 관통할 것인가?
AT_DOUBLE_JUMP_USE	= 3068 --더블 점프 사용

-- Pet Ability
AT_LOYALTY		= 4001
AT_BLOOD		= 4002
AT_PLAYTIME		= 4003
AT_AGE			= 4004
AT_LIFE			= 4005
AT_HUNGRY		= 4006
AT_GRADE		= 4007
AT_PETNO		= 4008
AT_PETSTATE		= 4009

-- Unit의 특수한 상태 정의의의의~~~
AT_UNIT_HIDDEN			= 4201	-- 숨어있는 상태 (다른 Player에게 보이지 않는다)
AT_ENABLE_AUTOHEAL	= 4202	-- AutoHeal 할 것인가?
AT_FIXED_MOVESPEED	= 4203	-- MoveSpeed 변경 금지~~
AT_CANNOT_DAMAGE		= 4204	-- 공격 받지 않는다.
AT_FROZEN						= 4205	-- 움직일 수 없다(그러나 한대라도 맞으면 움직일 수 있다)
AT_CANNOT_ATTACK		= 4206	-- 공격 할 수 없다.
AT_FROZEN_DMG_WAKE	= 4207	-- Frozen 상태일 때 Damage를 받으면 Frozen 상태가 풀릴것인가? (>0:풀린다.)
AT_CANNOT_USEITEM		= 4208  -- 아이템을 사용 할 수 없는 상태이다.
AT_CANNOT_EQUIP			= 4209	-- 장비아이템(장착/해제) 금지된 상태
AT_CANNOT_CASTSKILL		= 4210	-- Casting Type 스킬 사용 금지
AT_CRITICAL_ONEHIT		= 4211	-- 다음 공격에 있어 무조건 Critical 공격이 된다.
AT_CANNOT_SEE			= 4212	-- 주변을 볼수 없게 됨

-- ITEM
ATI_EXPLAINID			= 5001	-- Item Tooltip text id
AT_EQUIPPOS				= 5002
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
AT_DEFAULT_AMOUNT		= 5014	-- 수량 사용. 억단위가 0->내구도 1 ->수량

-- Boss Ability
AT_AI_DELAY			= 6001
AT_HP_GAGE			= 6002
AT_C_HP_GAGE		= 6003
AT_CURRENT_ACTION	= 6004
AT_MONSTER_APPEAR	= 6005

AT_MAX_HP		= 7001
AT_R_MAX_HP		= 7002
AT_C_MAX_HP		= 7003
AT_HP_RECOVERY_INTERVAL		= 7011
AT_R_HP_RECOVERY_INTERVAL	= 7012
AT_C_HP_RECOVERY_INTERVAL	= 7013
AT_MAX_MP		= 7021
AT_R_MAX_MP		= 7022
AT_C_MAX_MP		= 7023
AT_MP_RECOVERY_INTERVAL		= 7031
AT_R_MP_RECOVERY_INTERVAL	= 7032
AT_C_MP_RECOVERY_INTERVAL	= 7033
AT_STR			= 7041
AT_R_STR		= 7042
AT_C_STR		= 7043
AT_INT			= 7051
AT_R_INT		= 7052
AT_C_INT		= 7053
AT_CON			= 7061
AT_R_CON		= 7062
AT_C_CON		= 7063
AT_DEX			= 7071
AT_R_DEX		= 7072
AT_C_DEX		= 7073
AT_MOVESPEED	= 7081
AT_R_MOVESPEED	= 7082
AT_C_MOVESPEED	= 7083
AT_PHY_DEFENCE			= 7091
AT_R_PHY_DEFENCE		= 7092
AT_C_PHY_DEFENCE		= 7093
AT_MAGIC_DEFENCE		= 7101
AT_R_MAGIC_DEFENCE		= 7102
AT_C_MAGIC_DEFENCE		= 7103
AT_ATTACK_SPEED			= 7111
AT_R_ATTACK_SPEED		= 7112
AT_C_ATTACK_SPEED		= 7113
AT_BLOCK_RATE			= 7121
AT_R_BLOCK_RATE			= 7122
AT_C_BLOCK_RATE			= 7123
AT_DODGE_RATE			= 7131
AT_R_DODGE_RATE			= 7132
AT_C_DODGE_RATE			= 7133
AT_CRITICAL_RATE		= 7141
AT_R_CRITICAL_RATE		= 7142
AT_C_CRITICAL_RATE		= 7143
AT_CRITICAL_POWER		= 7151
AT_R_CRITICAL_POWER		= 7152
AT_C_CRITICAL_POWER		= 7153
AT_INVEN_SIZE			= 7161
AT_R_INVEN_SIZE			= 7162
AT_C_INVEN_SIZE			= 7163
AT_EQUIPS_SIZE			= 7171
AT_R_EQUIPS_SIZE		= 7172
AT_C_EQUIPS_SIZE		= 7173
AT_ATTACK_RANGE			= 7181	-- Item Skill
AT_R_ATTACK_RANGE		= 7182
AT_C_ATTACK_RANGE		= 7183
AT_HP_RECOVERY			= 7191
AT_R_HP_RECOVERY		= 7192
AT_C_HP_RECOVERY		= 7193
AT_MP_RECOVERY			= 7201
AT_R_MP_RECOVERY		= 7202
AT_C_MP_RECOVERY		= 7203
AT_JUMP_HEIGHT			= 7211
AT_R_JUMP_HEIGHT		= 7212
AT_C_JUMP_HEIGHT		= 7213
AT_PHY_ATTACK_MAX		= 7221	-- EquipMonsterNPC
AT_R_PHY_ATTACK_MAX		= 7222
AT_C_PHY_ATTACK_MAX		= 7223
AT_PHY_ATTACK_MIN		= 7231 -- EquipMonsterNPC
AT_R_PHY_ATTACK_MIN		= 7232
AT_C_PHY_ATTACK_MIN		= 7233
AT_TARGET_BACK_DISTANCE = 7241--밀리는 길이
AT_TARGET_FLY_DISTANCE	= 7251--뜨는 높이.
AT_NEED_MP				= 7261	-- Skill 사용 할 때 필요한 MP
AT_R_NEED_MP			= 7262
AT_C_NEED_MP			= 7263
AT_NEED_HP				= 7271
AT_R_NEED_HP			= 7272
AT_C_NEED_HP			= 7273
AT_MAGIC_ATTACK			= 7281
AT_R_MAGIC_ATTACK		= 7282
AT_C_MAGIC_ATTACK		= 7283
AT_PHY_ATTACK			= 7291
AT_R_PHY_ATTACK			= 7292
AT_C_PHY_ATTACK			= 7293
AT_CRITICAL_MPOWER		= 7301
AT_R_CRITICAL_MPOWER	= 7302
AT_C_CRITICAL_MPOWER	= 7303
AT_HITRATE				= 7311 -- 스킬 : Casting이 성공할 확률(백분율)
AT_R_HITRATE			= 7312
AT_C_HITRATE			= 7313
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
AT_R_MAGIC_DMG_DEC		= 7362
AT_C_MAGIC_DMG_DEC		= 7363
AT_HP_POTION_ADD_RATE	= 7431	--HP물약 사용시 추가적인 회복량
AT_R_HP_POTION_ADD_RATE	= 7432
AT_C_HP_POTION_ADD_RATE	= 7433
AT_MP_POTION_ADD_RATE		= 7441	--HP물약 사용시 추가적인 회복량
AT_R_MP_POTION_ADD_RATE	= 7442
AT_C_MP_POTION_ADD_RATE	= 7443
AT_DMG_REFLECT_RATE			= 7451	-- Damage 반사할 때, 반사되는 양(Damage에 대한 만분율)
AT_R_DMG_REFLECT_RATE		= 7452
AT_C_DMG_REFLECT_RATE		= 7453
AT_DMG_REFLECT_HITRATE		= 7461	-- Damage 반사할 때, 반사가 일어날 확률
AT_R_DMG_REFLECT_HITRATE	= 7462
AT_C_DMG_REFLECT_HITRATE	= 7463
AT_PENETRATION_RATE			= 7471	-- 관통할 확률
AT_R_PENETRATION_RATE		= 7472
AT_C_PENETRATION_RATE		= 7473
AT_ABS_ADDED_DMG_PHY		= 7491	-- Dmg 계산이후 절대값으로 넣어주는 추가 Dmg(Physical공격)
AT_R_ABS_ADDED_DMG_PHY		= 7492
AT_C_ABS_ADDED_DMG_PHY		= 7493
AT_ABS_ADDED_DMG_MAGIC		= 7501	-- Dmg 계산이후 절대값으로 넣어주는 추가 Dmg(Magic공격)
AT_R_ABS_ADDED_DMG_MAGIC	= 7502
AT_C_ABS_ADDED_DMG_MAGIC	= 7503
AT_MAX_DP					= 7511
AT_R_MAX_DP					= 7512
AT_C_MAX_DP					= 7513
AT_DP_RECOVERY_INTERVAL		= 7521
AT_R_DP_RECOVERY_INTERVAL	= 7522
AT_C_DP_RECOVERY_INTERVAL	= 7523
AT_DP_RECOVERY				= 7531
AT_R_DP_RECOVERY			= 7532
AT_C_DP_RECOVERY			= 7533

ABILITY_RATE_VALUE = 10000
AT_ADD_EXPERIENCE_RATE	= 11001

AT_PENALTY_HP_RATE = 20000+AT_HP
AT_PENALTY_MP_RATE = 20000+AT_MP
AT_ADD_EXPERIENCE_RATE = 20000+AT_EXPERIENCE

-----------------------------------------------
--                      PACKET TYPE
-----------------------------------------------
PT_M_C_NFY_COOLTIME_INIT = 4135
PT_M_C_NFY_CLASS_CHANGE = 12344
PT_C_M_NFT_ITEM_STATE  = 12423
PT_M_C_NFY_DAMAGE_RESULT = 12325
PT_M_C_NFY_REMOVE_MONSTER = 12351
PT_M_C_NFY_STATE_CHANGE = 12352
PT_M_C_NFY_ATTACKDAMAGE = 12353
PT_M_C_NFY_STATE_CHANGE2 = 12419
PT_M_C_NFY_ADD_NPC = 12424
PT_M_C_ITEMCHANGED = 12433
PT_M_C_RES_LEARN_SKILL = 12561
PT_M_C_NFY_ABILCHANGED = 12563
PT_M_C_NFY_BEGIN_WORLDACTION = 12602
PT_M_C_NFY_BOSSMONSTER = 12603
PT_M_C_NFY_TARGETUNIT = 12604
PT_M_C_ANS_ERROR_MESSAGE = 13102
PT_M_C_NFY_ACTION2 = 13112
PT_M_C_NFY_CHANGE_PARTY_DMG_HP = 12541
PT_M_C_NFY_MONSTERGOAL = 13113

-----------------------------------------------
-- Unit State
-----------------------------------------------
US_BIT_CANT_MOVE			= 65536
US_BIT_CANT_DAMAGED			= 131072
US_BIT_CANT_CHANGE			= 262144
US_BIT_WAITDELAY_BYFORCE	= 524288
US_BIT_CANT_ATTACK			= 1048576		-- 공격할 수 없는 상태
US_BIT_CANT_DSKILL			= 2097152		-- 자신에게 걸린 Skill을 변경 할 수 없는 상태

US_NONE			= 0
US_DEAD			= 1 + US_BIT_CANT_MOVE + US_BIT_CANT_DAMAGED
US_ATTACK		= 2
US_MOVE	        = 3
US_IDLE			= 4
US_WATCHPLAYER      = 8
US_ATTACKTIME	=	9
US_ATTACKDELAY	=	10
US_BLOW_UP			= 11 + US_BIT_CANT_MOVE	+ US_BIT_CANT_ATTACK -- 맞아서 공중에 뜬 상태. 모든 AI 멈춤.
US_PUSHED_BY_DAMAGE = 13 + US_BIT_CANT_MOVE + US_BIT_CANT_ATTACK
US_DAMAGEDELAY			= 14 + US_BIT_CANT_MOVE + US_BIT_CANT_ATTACK	-- Damage 맞은 상태 (다른 Skill로 인한 Damage 상태로 전환 가능)
US_DAMAGEDELAY_NOCHANGE = 15 + US_BIT_CANT_MOVE + US_BIT_CANT_ATTACK + US_BIT_CANT_DSKILL -- Damage 맞은 상태 (다른 Skill로 인한 Damage 상태로 전환 불가)
US_FROZEN			= 18 + US_BIT_CANT_MOVE + US_BIT_CANT_ATTACK + US_BIT_CANT_DSKILL	-- 얼었다 (모든 액션 금지, 한대 맞으면 풀림)
US_SKILL_CAST	= 19

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
UT_ENTITY 		= 0x00000100	-- 형태가 없는 개체(지역바법같은..)
UT_OBJECT		= 0x00000200

-----------------------------------------------
-- Action Argument type
-----------------------------------------------
ACTARG_MAPOBJ		= 1
--ACTARG_ZONEOBJ		= 2
--ACTARG_UNITMNG		= 3
ACTARG_PUPPETMNG = 4
ACTARG_PUPPET = 5
ACTARG_WORLDACTION = 6
ACTARG_PUPPETGROUP = 7
ACTARG_OLD_VALUE  = 8
ACTARG_EFFECT_NUM = 9 
ACTARG_GROUND = 10
ACTARG_ACTIONINSTANCEID = 11
ACTARG_ABILITYVALUE = 12
ACTARG_WEAPONLIMIT	= 15
ACTARG_TOGGLESKILL = 17
ACTARG_CUSTOM_UNIT1 = 1001
-- RESERVER (10000 - 19999) : CUSTOM DATA
ACT_ARG_CUSTOMDATA1 = 10000


-----------------------------------------------
--Puppet Type
-----------------------------------------------
PUPPET_T_TRANSFORM = 101
PUPPET_T_PUPPETGROUP = 102
PUPPET_T_TRANSFORMCIRCLE = 103
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
WA_MOVE_TO_START_LNE = 0
WA_POSSESSION_THE_HILL = 1
WA_TIME_OVER = 2

-- 공성전
WA_MOVE_TO_START_LNE = 100
WA_POSSESSION_THE_HILL = 101
WA_TIME_OVER = 102

-- 파렐경
WA_PAREL_APPEAR	    	= 30 -- 오프닝
WA_PAREL_DIE        		= 31 -- 유저 승리
WA_PAREL_ATTACK01   	= 32 --일반 2연타
WA_PAREL_ATTACK02   	= 33 -- 보석을 채찍처럼 날린다
WA_PAREL_ATTACK03   	= 34 -- 광역 무적 공격
WA_PAREL_ATTACK04   	= 35 -- 공중 낙하 공격
WA_PAREL_ATTACK05   	= 36 -- 몸통 박치기 공격
WA_PAREL_STUN			= 37 -- 스턴
WA_PAREL_DOWN			= 38 -- 다운

-- 라발론
WA_METEOR				= 200 -- 용암탄..
WA_METEOR_BLUE			= 201 -- 용암탄 큰거.. 넘어가는 액션!
-----------------------------------------------
-- Timer Interval
-----------------------------------------------
ETICK_INTERVAL_100MS = 0
ETICK_INTERVAL_30S = 1
ETICK_INTERVAL_1S = 2

-----------------------------------------------
-- Monster
-----------------------------------------------
M_MPUMPKINRACOON			=	1	-- 겁나는 호박머리 너구리
M_MORC					=	2	-- 겁나는 오크
M_MBUMBLOW				=	3	-- 겁나는 범블로우
M_BPUMPKINRACOON			=	4	-- 무서운 호박머리 너구리
M_BORC					=	5	-- 무서운 오크
M_BBUMBLOW				=	6	-- 무서운 범블로우
M_PUMPKINRACOON				=	18	-- 호박머리 너구리
M_ORC					=	19	-- 오크
M_BUMBLOW				=	200007	-- 범블로우
M_SAPHIREBOSS				=	200008 	-- 사파이어 드래곤	그렘그리스
M_PARELBOSS				=	6000920 	-- 파렐 경

---------------------------------------------------
-- Error Message
---------------------------------------------------
-- EErrorMessageType
EMT_ITEM = 1

-- EErrorMessageCode
EMC_NOMORE_HAVE = 1
EMC_NOMORE_CONDITION = 2

----------------------------------------------
-- Skill Characteristics
----------------------------------------------
SAT_PHYSICS		= 16777216
SAT_MAGIC		= 33554432
SAT_DEFAULT	=	134217728

----------------------------------------------
-- Unit Send Type
----------------------------------------------
--E_SENDABIL_NONE = 0
--E_SENDABIL_SELF = 1
--E_SENDABIL_BROADCAST = 2
--E_SENDABIL_BROADALL	= E_SENDABIL_SELF + E_SENDABIL_BROADCAST
E_SENDTYPE_NONE = 0x00
E_SENDTYPE_SELF = 0x01
E_SENDTYPE_BROADCAST = 0x02
E_SENDTYPE_BROADCAST_GROUND =	0x04	-- Ground 전체에 BroadCase 
E_SENDTYPE_MUSTSEND =	0x08	-- 반드시 보내야 한다.
E_SENDTYPE_EFFECTABIL =	0x10	-- Effect Abil 값을 보낸다.
E_SENDTYPE_BROADALL	= E_SENDTYPE_SELF+E_SENDTYPE_BROADCAST

-----------------------------------------------------
-- TickAI
------------------------------------------------------
AI_MONSTER_MAX_MOVETIME  = 10000	-- Monster maximum move time
AI_MONSTER_MIN_MOVETIME	 = 1500	-- Monster minimum move time

-----------------------------------------------------
-- Inventory
------------------------------------------------------
-- Inven Type
IT_EQUIP = 1
IT_CONSUME = 2
IT_ETC = 3
IT_CASH = 4
IT_SAFE = 5
IT_CASH_SAFE = 6
IT_FIT = 7


-- Equip position (bit field checking)
	-- 다른 값들은 사용 안하고 있으므로 생략 하였음....
EQUIP_POS_WEAPON = 6
EQUIP_POS_SHEILD = 7

-----------------------------------------------------
-- Weapon Type
------------------------------------------------------
EWEAPON_NONE		= 0
EWEAPON_SWORD		= 1
EWEAPON_BIHANDSWORD	= 2
EWEAPON_STAFF		= 4
EWEAPON_SPEAR		= 8
EWEAPON_BOW			= 16
EWEAPON_CROSSBOW	= 32
EWEAPON_CLAW		= 64
EWEAPON_KATTAR		= 128

-----------------------------------------------------
-- Effect Tick Return value
------------------------------------------------------
ECT_NONE = 0
ECT_DOTICK = 2
ECT_MUSTDELETE = 4
ECT_TICKBREAK = 8

-----------------------------------------------------
-- Skill Statuts
------------------------------------------------------
ESS_NONE = 0
ESS_CASTTIME = 1
ESS_FIRE = 2

-----------------------------------------------------
-- Effect Type
------------------------------------------------------
EFFECT_TYPE_BLESSED = 1
EFFECT_TYPE_CURSED = 2

-----------------------------------------------------
-- ETC
------------------------------------------------------
BOUNDARY_AMOUNT_N_PERCENTAGE = 10000
ALL_UNIT_MAX_HP = 9999999

-----------------------------------------------------
-- SKILL CUSTOM DATA
------------------------------------------------------
AT_SKILL_CUSTOM_DATA_01		= 60001
AT_SKILL_CUSTOM_DATA_MAX	= 60201

