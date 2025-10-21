
--	타격시 캐릭터가 잠깐 멈추는 시간 (단위 : 초)
g_fAttackHitHoldingTime	=	0.145

-- <대미지 숫자> 최초 등장시의 스케일값
g_fDmgNumInitialScale = 0.9
-- <대미지 숫자> 최종 스케일값
g_fDmgNumScale = 0.4
-- <대미지 숫자> 스케일링 되는 시간(단위 : 초)
g_fDmgNumScaleTime = 0.3
g_fDmgScaleTime_NUM = 0.3 -- 일반 데미지(알파값으로 쓰임)
g_fDmgScaleTime_CRITICAL= 0.2 -- 크리티컬 (알파값으로 쓰임) 

-- <대미지 숫자> 스케일링이 끝난 후 멈춰 있는 시간(단위 : 초)
g_fDmgNumHoldingTime = 0.2
-- <대미지 숫자> 대미지 숫자 올라가는 속도
g_fDmgNumMoveSpeed = 200.0
-- <대미지 숫자> 대미지 숫자 올라가는 총 시간(단위 : 초)
g_fDmgNumMoveTime = 0.2
g_fDmgNumMoveTimeScale_NUM = 0.4		-- 일반 데미지(알파 값으로 쓰임)
g_fDmgNumMoveTimeScale_CRITICAL = 0.8 -- 크리티컬(알파 값으로 쓰임)

-- <대미지 숫자> 대미지 숫자 올라갈때 최종 스케일링 값
g_fDmgNumMoveTargetScale = 0.2

g_fDmgNumInitialHoldingTime = 0.04;

g_fDmgNumInitialAlpha = 0.2;

g_fDmgNumHoldingAlphaStartTime = 0.5; -- 0 ~ 1 사이

g_fDmgNumScaleAcceSpeed = 0.02;
g_fDmgNumHoldingAlphaTime = 0.2;
g_fDmgNumHoldingScale = 0.8;
g_fDmgNumHoldingAfterEffectTime = 0.07;


-- 몬스터 BlowUp 시에 수평축 이동 속도
g_fBlowUpHorizonSpeed	=	30.0
-- 몬스터 BlowUp 시, 두번째 튀는 힘 비율
g_fBlowUpSecondJumpPower = 0.1

-- 낙법 제한 시간
g_fBreakFallLimitTime = 0.5

-- 3타 공격 경직시간.(단위 : 초)
g_fThirdAttackFreezeTime = 0.0

-- 마법사 3타 공격시 터지는 범위
g_fMagicianThirdShotEffectRange = 10.0

-- 마법사 기본공격 타겟 영역 폭 넓이
g_fMagicianShot_BarWidth = 20.0

-- 마법사 점프 다운 공격(메테오) 폭발 범위
g_fMagician_JumpDown_Meteor_Blow_Range = 50.0;

-- 차징 공격 후 경직시간((단위 : 초)
g_fChargingFreezeTime = 0.2

-- 차징 공격 범위(단위 : Degree)
g_fChargingAttackAngle = 30.0

-- 차징 공격 끝난 후 화면이 밝아지는 시간(단위 : 초)
g_fChargeFilterReturnTime = 0.5

-- 포트리스 폭발 범위
g_fFortress_Blow_Range = 50

-- 띄우기 공격 범위(단위 : Degree)
g_fMeleeBlowUpAttackAngle = 60.0

-- 전사,법사,도적 기본 공격시에 공격자가 앞으로 나아가는 속도
g_fMeleeAttackPushSpeed = 150

-- 3타째 공격에서 몬스터가 밀려나는 속도
g_fPushBackMoveSpeed = 300.0

-- 대쉬 속도
g_fDashSpeed = 550.0

-- 대쉬 후 경직 시간(단위 : 초)
g_fDashFreezeTime = 0.0

-- 대쉬 점프 이동속도
g_fDashJumpSpeed = 350.0

-- 대쉬 공격 후 경직시간(단위 : 초)
g_fDashAttackFreezeTime = 0.2

-- 내려찍기 공격 후 경직시간(단위 : 초)
g_fMeleeDropFreezeTime = 0.0

-- 강공격 후 경직시간(단위 : 초)
g_fMeleeKnockBackFreezeTime = 0.15

-- 궁수 기본 공격의 타겟팅 각도(단위 : Degree)
g_fArcherDefaultAttackAngle = 20.0

-- 궁수 기본 공격 콤보 연결 가능 최대 시간(단위 : 초)
g_fArcherComboConnectMaxTime = 1.0

-- 궁수 기본 공격 쿨타임
g_fArcherDefaultAttackCoolTime = 0.3

-- 소환사 기본 공격 쿨타임
g_fSum_RifleDefaultAttackCoolTime = 0.3

-- 충돌 대미지 액션 총 밀려나는 시간(단위 : 초)
g_fTouchDmgTotalTime = 0.2

-- 충돌 대미지 액션 밀려나는 속도
g_fTouchDmgSpeed = 250.0
 
-- 충돌 대미지 액션 후 무적 시간(단위 : 초)
g_fTouchDmgGodTime = 0.5

-- 차지공격시 기모으는 동작에 따른 화면 어두워짐 효과
g_fChargeFocusFilterValue = {0x000000, 0.0, 0.9, 2};

-- 점프해서 아래로 하여 찍기 공격을 할 경우, 카메라가 아래로 흔들리는 효과 
g_fMeleeDropQuakeValue = {0.2, 7, 3, 1, 1};

-- 강공격을 할 경우, 카메라가 좌우로 흔들리는 효과 
g_fMeleeKnockBackQuakeValue_Left = {0.5, 10, 2, -1, 1};
g_fMeleeKnockBackQuakeValue_Right = {0.5, 10, 2, 1, 1};

-- 띄우기를 할 경우, 카메라가 안쪽으로 흔들리는 효과
g_fBlowUpQuakeValue = {0.2, 5, 4, 1, 3};

-- 다운공격을 할 경우, 카메라가 위쪽으로 흔들리는 효과 
g_fDownAttackQuakeValue = {0.2, 4, 3, -1, 1};

-- Bloom 파라메터값
g_fBloom_GaussBlurDev = 2.0 --  bl_gauss_dev
g_fBloom_GaussBlurMul = 1.0 --  bl_gauss_mul
g_fBloom_BloomIntensity = 0.7 -- bl_bi
g_fBloom_HilightIntensity = 0.2 --  bl_hi
g_fBloom_SceneIntensity = 1.0   --  bl_si

-- 자동 공격 딜레이
-- CDT 100ms, 애니타임 233ms 인 상황에서
-- 궁수 수동공속 분당 180일때 160의 속도를 가지는
-- 자동공속의 딜레이 : 0.36
g_fAutoFireDelayTime = {0,0,0.36,0} -- 전사,법사,궁수,도적 순으로 각 직업별로 딜레이를 다르게 입력.

-- 체력게이지바 블링크 컬러
g_fHpGaugeBlinkColor = {1,1,1,1 }

-- 저사양 포그 Start,End 값
g_fLowFogStart = 1000.0
g_fLowFogEnd = 2500.0
g_fLowFogDensity = 0.1;

-- UseDraw 스킬 디버깅 창
g_bUseSkillDraw = false;

-- 쓰러졌다가 일어날때 옆으로 구르는 기능을 쓸지 말지 결정
g_bUseBreakFallWhenStandUp = false;

-- 미션 레벨에 따른 몬스터 발사체 속도 조절
g_fProjectileSpeedByMissionLevel = {1,1.5,2,2.5,1,1,1,1,1}

-- BG 사운드 Fade In/Out 시간
g_fBgSoundFadeInTime = 5.0
g_fBgSoundFadeOutTime = 3.0

-- 공중회피 시작제한시간
-- 피격당해 띄우기 상태가 되고 해당 시간이 경과후에야
-- 공중회피가 가능해진다. (단위시간은 초)
g_fEvasionStartTime = 0.3

-- 배틀스퀘어시에 블루,레드팀에 입혀줄 유니폼 아이템 번호들
-- 사용하지 않는 Parts 의 경우에는 번호를 -1 로 설정할 것.

g_Emporia_Blue_Helmet = 49000010;	-- 블루팀 투구
g_Emporia_Blue_Jacket = 49000020;	-- 블루팀 상의
g_Emporia_Blue_Pants = 49000030;	-- 블루팀 하의
g_Emporia_Blue_Shoulder = 49000040;	-- 블루팀 어깨
g_Emporia_Blue_Glove = 49000050;	-- 블루팀 장갑
g_Emporia_Blue_Shoes = 49000060;	-- 블루팀 신발
g_Emporia_Blue_Cloak = 49000065;		-- 블루팀 백팩

g_Emporia_Red_Helmet = 49000070;	-- 레드팀 투구
g_Emporia_Red_Jacket = 49000080;	-- 레드팀 상의
g_Emporia_Red_Pants = 49000090;		-- 레드팀 하의
g_Emporia_Red_Shoulder = 49000100;	-- 레드팀 어깨
g_Emporia_Red_Glove = 49000110;		-- 레드팀 장갑
g_Emporia_Red_Shoes = 49000120;		-- 레드팀 신발
g_Emporia_Red_Cloak = 49000125;		-- 블루팀 백팩

g_SnowParticleID	=	"EF_Snowfall_002_01"	--	눈 효과 파티클 ID
g_SakuraParticleID	=	"EF_Flowerfall_002_04"	--	벚꽃 효과 파티클 ID

------------------------캐릭터 선택창 카메라 애니메이션 Seq 번호-----------------------
 g_kArraySelectCameraMode = {}
 g_kArraySelectCameraMode["NORMAL"] = { ["CAMERA_MOVE_SELECT"] 			= 1000000,  
									["CAMERA_MOVE_CREATE_HUMAN_IN"] 	= 1000101, 
									["CAMERA_MOVE_CREATE_HUMAN"] 		= 1000102, 
									["CAMERA_MOVE_CREATE_HUMAN_OUT"] 	= 1000103, 
									["CAMERA_MOVE_CREATE_DRAGON_IN"] 	= 1000201,
									["CAMERA_MOVE_CREATE_DRAGON"]		= 1000202,
									["CAMERA_MOVE_CREATE_DRAGON_OUT"]	= 1000203 }
 
 g_kArraySelectCameraMode["WIDE"] = { ["CAMERA_MOVE_SELECT"] 			= 1000000,  
									["CAMERA_MOVE_CREATE_HUMAN_IN"] 	= 1000101, 
									["CAMERA_MOVE_CREATE_HUMAN"] 		= 1000102, 
									["CAMERA_MOVE_CREATE_HUMAN_OUT"] 	= 1000103, 
									["CAMERA_MOVE_CREATE_DRAGON_IN"] 	= 1000201,
									["CAMERA_MOVE_CREATE_DRAGON"]		= 1000202,
									["CAMERA_MOVE_CREATE_DRAGON_OUT"]	= 1000203 }
									
g_kSelectCameraMode = g_kArraySelectCameraMode["NORMAL"]	--기본은 Normal로.

------------------------미리 입어보기 1:캐시 2:세트 3:어드벤스-----------------------
local MALE = 1
local FEMALE = 2
local TEST_WEAR_CASH = 1
local TEST_WEAR_SET = 2
local TEST_WEAR_ADVANCE = 3

local Test_Wear_FIGHTER_CASH = {}
Test_Wear_FIGHTER_CASH[MALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_FIGHTER_CASH[FEMALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_FIGHTER_SET = {}
Test_Wear_FIGHTER_SET[MALE] = {["HELM"]= 890100050, ["JACKET"]= 890100060, ["PANTS"]= 890100070, ["SHOULDER"]= 890100080, ["GLOVE"]= 890100090, ["SHOSE"]= 890100100, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_FIGHTER_SET[FEMALE] = {["HELM"]= 890100050, ["JACKET"]= 890100060, ["PANTS"]= 890100070, ["SHOULDER"]= 890100080, ["GLOVE"]= 890100090, ["SHOSE"]= 890100100, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_FIGHTER_ADVANCE = {}
Test_Wear_FIGHTER_ADVANCE[MALE] = {["HELM"]= 890100110, ["JACKET"]= 890100120, ["PANTS"]= 890100130, ["SHOULDER"]= 890100140, ["GLOVE"]= 890100150, ["SHOSE"]= 890100160, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_FIGHTER_ADVANCE[FEMALE] = {["HELM"]= 890100110, ["JACKET"]= 890100120, ["PANTS"]= 890100130, ["SHOULDER"]= 890100140, ["GLOVE"]= 890100150, ["SHOSE"]= 890100160, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}

local Test_Wear_MAGITION_CASH = {}
Test_Wear_MAGITION_CASH[MALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_MAGITION_CASH[FEMALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_MAGITION_SET = {}
Test_Wear_MAGITION_SET[MALE] = {["HELM"]= 890100170, ["JACKET"]= 890100180, ["PANTS"]= 890100190, ["SHOULDER"]= 890100200, ["GLOVE"]= 890100210, ["SHOSE"]= 890100220, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_MAGITION_SET[FEMALE] = {["HELM"]= 890100170, ["JACKET"]= 890100180, ["PANTS"]= 890100190, ["SHOULDER"]= 890100200, ["GLOVE"]= 890100210, ["SHOSE"]= 890100220, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_MAGITION_ADVANCE = {}
Test_Wear_MAGITION_ADVANCE[MALE] = {["HELM"]= 890100230, ["JACKET"]= 890100240, ["PANTS"]= 890100250, ["SHOULDER"]= 890100260, ["GLOVE"]= 890100270, ["SHOSE"]= 890100280, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_MAGITION_ADVANCE[FEMALE] = {["HELM"]= 890100230, ["JACKET"]= 890100240, ["PANTS"]= 890100250, ["SHOULDER"]= 890100260, ["GLOVE"]= 890100270, ["SHOSE"]= 890100280, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}

local Test_Wear_ARCHER_CASH = {}
Test_Wear_ARCHER_CASH[MALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_ARCHER_CASH[FEMALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_ARCHER_SET = {}
Test_Wear_ARCHER_SET[MALE] = {["HELM"]= 890100290, ["JACKET"]= 890100300, ["PANTS"]= 890100310, ["SHOULDER"]= 890100320, ["GLOVE"]= 890100330, ["SHOSE"]= 890100340, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_ARCHER_SET[FEMALE] = {["HELM"]= 890100290, ["JACKET"]= 890100300, ["PANTS"]= 890100310, ["SHOULDER"]= 890100320, ["GLOVE"]= 890100330, ["SHOSE"]= 890100340, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_ARCHER_ADVANCE = {}
Test_Wear_ARCHER_ADVANCE[MALE] = {["HELM"]= 890100350, ["JACKET"]= 890100360, ["PANTS"]= 890100370, ["SHOULDER"]= 890100380, ["GLOVE"]= 890100390, ["SHOSE"]= 890100400, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_ARCHER_ADVANCE[FEMALE] = {["HELM"]= 890100350, ["JACKET"]= 890100360, ["PANTS"]= 890100370, ["SHOULDER"]= 890100380, ["GLOVE"]= 890100390, ["SHOSE"]= 890100400, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}

local Test_Wear_THIEF_CASH = {}
Test_Wear_THIEF_CASH[MALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_THIEF_CASH[FEMALE] = {["HELM"]= 890100000, ["JACKET"]= 890100010, ["PANTS"]= 890100020, ["SHOULDER"]= 0, ["GLOVE"]= 890100030, ["SHOSE"]= 890100040, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_THIEF_SET = {}
Test_Wear_THIEF_SET[MALE] = {["HELM"]= 890100410, ["JACKET"]= 890100420, ["PANTS"]= 890100430, ["SHOULDER"]= 890100440, ["GLOVE"]= 890100450, ["SHOSE"]= 89010060, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_THIEF_SET[FEMALE] = {["HELM"]= 890100410, ["JACKET"]= 890100420, ["PANTS"]= 890100430, ["SHOULDER"]= 890100440, ["GLOVE"]= 890100450, ["SHOSE"]= 89010060, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_THIEF_ADVANCE = {}
Test_Wear_THIEF_ADVANCE[MALE] = {["HELM"]= 890100470, ["JACKET"]= 890100480, ["PANTS"]= 890100490, ["SHOULDER"]= 890100500, ["GLOVE"]= 890100510, ["SHOSE"]= 890100520, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_THIEF_ADVANCE[FEMALE] = {["HELM"]= 890100470, ["JACKET"]= 890100480, ["PANTS"]= 890100490, ["SHOULDER"]= 890100500, ["GLOVE"]= 890100510, ["SHOSE"]= 890100520, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}

local Test_Wear_SUMMONER_CASH = {}
Test_Wear_SUMMONER_CASH[MALE] = {["HELM"]= 890100530, ["JACKET"]= 890100540, ["PANTS"]= 890100550, ["SHOULDER"]= 0, ["GLOVE"]= 890100560, ["SHOSE"]= 890100570, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_SUMMONER_CASH[FEMALE] = {["HELM"]= 890100530, ["JACKET"]= 890100540, ["PANTS"]= 890100550, ["SHOULDER"]= 0, ["GLOVE"]= 890100560, ["SHOSE"]= 890100570, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_SUMMONER_SET = {}
Test_Wear_SUMMONER_SET[MALE] = {["HELM"]= 890100580, ["JACKET"]= 890100590, ["PANTS"]= 890100600, ["SHOULDER"]= 890100610, ["GLOVE"]= 890100620, ["SHOSE"]= 890100630, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_SUMMONER_SET[FEMALE] = {["HELM"]= 890100580, ["JACKET"]= 890100590, ["PANTS"]= 890100600, ["SHOULDER"]= 890100610, ["GLOVE"]= 890100620, ["SHOSE"]= 890100630, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_SUMMONER_ADVANCE = {}
Test_Wear_SUMMONER_ADVANCE[MALE] = {["HELM"]= 890100640, ["JACKET"]= 890100650, ["PANTS"]= 890100660, ["SHOULDER"]= 890100670, ["GLOVE"]= 890100680, ["SHOSE"]= 890100690, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_SUMMONER_ADVANCE[FEMALE] = {["HELM"]= 890100640, ["JACKET"]= 890100650, ["PANTS"]= 890100660, ["SHOULDER"]= 890100670, ["GLOVE"]= 890100680, ["SHOSE"]= 890100690, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}

local Test_Wear_TWINS_CASH = {}
Test_Wear_TWINS_CASH[MALE] = {["HELM"]= 890100530, ["JACKET"]= 890100540, ["PANTS"]= 890100550, ["SHOULDER"]= 0, ["GLOVE"]= 890100560, ["SHOSE"]= 890100570, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
Test_Wear_TWINS_CASH[FEMALE] = {["HELM"]= 890100530, ["JACKET"]= 890100540, ["PANTS"]= 890100550, ["SHOULDER"]= 0, ["GLOVE"]= 890100560, ["SHOSE"]= 890100570, ["WEAPON"]= 0, ["SHIELED"]= 0, ["BACKPACK"]= 0}
local Test_Wear_TWINS_SET = {}
Test_Wear_TWINS_SET[MALE] = {["HELM"]= 890100700, ["JACKET"]= 890100710, ["PANTS"]= 890100720, ["SHOULDER"]= 890100730, ["GLOVE"]= 890100740, ["SHOSE"]= 890100750, ["WEAPON"]= 890100760, ["SHIELED"]= 890100770, ["BACKPACK"]= 0}
Test_Wear_TWINS_SET[FEMALE] = {["HELM"]= 890100700, ["JACKET"]= 890100710, ["PANTS"]= 890100720, ["SHOULDER"]= 890100730, ["GLOVE"]= 890100740, ["SHOSE"]= 890100750, ["WEAPON"]= 890100760, ["SHIELED"]= 890100770, ["BACKPACK"]= 0}
local Test_Wear_TWINS_ADVANCE = {}
Test_Wear_TWINS_ADVANCE[MALE] = {["HELM"]= 890100780, ["JACKET"]= 890100790, ["PANTS"]= 890100800, ["SHOULDER"]= 890100810, ["GLOVE"]= 890100820, ["SHOSE"]= 890100830, ["WEAPON"]= 890100840, ["SHIELED"]= 890100850, ["BACKPACK"]= 0}
Test_Wear_TWINS_ADVANCE[FEMALE] = {["HELM"]= 890100780, ["JACKET"]= 890100790, ["PANTS"]= 890100800, ["SHOULDER"]= 890100810, ["GLOVE"]= 890100820, ["SHOSE"]= 890100830, ["WEAPON"]= 890100840, ["SHIELED"]= 890100850, ["BACKPACK"]= 0}


local Test_Wear_FIGHTER = {}
Test_Wear_FIGHTER[TEST_WEAR_CASH] = Test_Wear_FIGHTER_CASH
Test_Wear_FIGHTER[TEST_WEAR_SET] = Test_Wear_FIGHTER_SET
Test_Wear_FIGHTER[TEST_WEAR_ADVANCE] = Test_Wear_FIGHTER_ADVANCE
local Test_Wear_MAGITION = {}
Test_Wear_MAGITION[TEST_WEAR_CASH] = Test_Wear_MAGITION_CASH
Test_Wear_MAGITION[TEST_WEAR_SET] = Test_Wear_MAGITION_SET
Test_Wear_MAGITION[TEST_WEAR_ADVANCE] = Test_Wear_MAGITION_ADVANCE
local Test_Wear_ARCHER = {}
Test_Wear_ARCHER[TEST_WEAR_CASH] = Test_Wear_ARCHER_CASH
Test_Wear_ARCHER[TEST_WEAR_SET] = Test_Wear_ARCHER_SET
Test_Wear_ARCHER[TEST_WEAR_ADVANCE] = Test_Wear_ARCHER_ADVANCE
local Test_Wear_THIEF = {}
Test_Wear_THIEF[TEST_WEAR_CASH] = Test_Wear_THIEF_CASH
Test_Wear_THIEF[TEST_WEAR_SET] = Test_Wear_THIEF_SET
Test_Wear_THIEF[TEST_WEAR_ADVANCE] = Test_Wear_THIEF_ADVANCE
local Test_Wear_SUMMONER = {}
Test_Wear_SUMMONER[TEST_WEAR_CASH] = Test_Wear_SUMMONER_CASH
Test_Wear_SUMMONER[TEST_WEAR_SET] = Test_Wear_SUMMONER_SET
Test_Wear_SUMMONER[TEST_WEAR_ADVANCE] = Test_Wear_SUMMONER_ADVANCE
local Test_Wear_TWINS = {}
Test_Wear_TWINS[TEST_WEAR_CASH] = Test_Wear_TWINS_CASH
Test_Wear_TWINS[TEST_WEAR_SET] = Test_Wear_TWINS_SET
Test_Wear_TWINS[TEST_WEAR_ADVANCE] = Test_Wear_TWINS_ADVANCE


g_kCharSelect_Test_Wear = {}
g_kCharSelect_Test_Wear[CT_FIGHTER] = Test_Wear_FIGHTER
g_kCharSelect_Test_Wear[CT_MAGICIAN] = Test_Wear_MAGITION
g_kCharSelect_Test_Wear[CT_ARCHER] = Test_Wear_ARCHER
g_kCharSelect_Test_Wear[CT_THIEF] = Test_Wear_THIEF
g_kCharSelect_Test_Wear[CT_SHAMAN] = Test_Wear_SUMMONER
g_kCharSelect_Test_Wear[CT_DOUBLE_FIGHTER] = Test_Wear_TWINS

--------------------------------------------------------------------------------------------
-- 이 아래부터는 시스템 함수입니다. 프로그래머만 수정할수 있습니다.
--------------------------------------------------------------------------------------------

function	GetSnowParticleID(No)
	return	g_SnowParticleID;
end

function	GetSakuraParticleID(No)
	return	g_SakuraParticleID;
end

function	GetTeamItemNo(iTeamNo,iItemEquipPos)
	if iTeamNo == TEAM_RED then	-- Attacker, Red
		if iItemEquipPos == EQUIP_POS_HELMET then	return	g_Emporia_Red_Helmet; end
		if iItemEquipPos == EQUIP_POS_SHIRTS then	return	g_Emporia_Red_Jacket; end
		if iItemEquipPos == EQUIP_POS_PANTS then	return	g_Emporia_Red_Pants; end
		if iItemEquipPos == EQUIP_POS_SHOULDER then	return	g_Emporia_Red_Shoulder; end
		if iItemEquipPos == EQUIP_POS_GLOVE then	return	g_Emporia_Red_Glove; end
		if iItemEquipPos == EQUIP_POS_BOOTS then	return	g_Emporia_Red_Shoes; end
		if iItemEquipPos == EQUIP_POS_CLOAK then	return	g_Emporia_Red_Cloak; end
	elseif iTeamNo == TEAM_BLUE then	-- Defencer, Blue
		if iItemEquipPos == EQUIP_POS_HELMET then	return	g_Emporia_Blue_Helmet; end
		if iItemEquipPos == EQUIP_POS_SHIRTS then	return	g_Emporia_Blue_Jacket; end
		if iItemEquipPos == EQUIP_POS_PANTS then	return	g_Emporia_Blue_Pants; end
		if iItemEquipPos == EQUIP_POS_SHOULDER then	return	g_Emporia_Blue_Shoulder; end
		if iItemEquipPos == EQUIP_POS_GLOVE then	return	g_Emporia_Blue_Glove; end
		if iItemEquipPos == EQUIP_POS_BOOTS then	return	g_Emporia_Blue_Shoes; end
		if iItemEquipPos == EQUIP_POS_CLOAK then	return	g_Emporia_Blue_Cloak; end
	end
	return	-1;
end

function	GetProjectileSpeedByMissionLevel(iLevel)

	if iLevel<=0 then
		return	1
	end

	local iCount = 0
	for kKey, kVal in pairs(g_fProjectileSpeedByMissionLevel) do
		iCount = iCount + 1
	end	
	
	if iLevel>iCount then
		return	1
	end
	
	return	g_fProjectileSpeedByMissionLevel[iLevel];
	
end

function ShadowMapTest()

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	--g_bDrawBG = false;
	g_world:ChangeShader("DYNAMIC_NODE_ROOT","ShadowMapRenderScene");
	g_world:ChangeShader("STATIC_NODE_ROOT","ShadowMapRenderScene");
	g_world:ApplyShadowMap("DYNAMIC_NODE_ROOT");
	g_world:ApplyShadowMap("STATIC_NODE_ROOT");

end

function GetDashSpeed()
	return	g_fDashSpeed;
end
function GetDashFreezeTime()
	return	g_fDashFreezeTime;
end
function GetJumpForce()
	return	jumpForce;
end
function GetDashJumpSpeed()
	return	g_fDashJumpSpeed;
end
function GetAutoFireDelayTime(kActorBaseClassID)
	return	g_fAutoFireDelayTime[kActorBaseClassID];
end

function TurnOnSpotLight(bTurnOn)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:TurnOnSpotLight(bTurnOn)
end
function SetSpotLightColor(r,g,b)
	GetMyActor():SetSpotLightColor(r,g,b)
end
function SetSpotLightBGColor(r,g,b)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetSpotLightBGColor(r,g,b)
end

function SetFogAlpha(alpha)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetFogAlpha(alpha)
end
function SetFogColor(r,g,b)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetFogColor(r,g,b)
end

function GetLowFogStart()
	return	g_fLowFogStart
end

function GetLowFogEnd()
	return	g_fLowFogEnd
end
function GetLowFogDensity()
	return	g_fLowFogDensity
end

function GetDmgNumInitialHoldingTime()
	return g_fDmgNumInitialHoldingTime;
end
function GetDmgNumInitialScale()
	return	g_fDmgNumInitialScale;
end
function	GetDmgNumScale()
	return	g_fDmgNumScale;
end
function	GetDmgNumScaleTime()
	return	g_fDmgNumScaleTime;
end
function	GetDmgNumHoldingTime()
	return	g_fDmgNumHoldingTime;
end
function	GetDmgNumMoveSpeed()
	return	g_fDmgNumMoveSpeed;
end
function	GetDmgNumMoveTime()
	return	g_fDmgNumMoveTime;
end
function	GetDmgNumMoveTargetScale()
	return	g_fDmgNumMoveTargetScale;
end
function	GetDmgNumInitialAlpha()
	return	g_fDmgNumInitialAlpha;
end
function	GetDmgNumHoldingAlphaStartTime()
	return	g_fDmgNumHoldingAlphaStartTime;
end
function	GetDmgNumScaleAcceSpeed()
	return	g_fDmgNumScaleAcceSpeed;
end
function	GetDmgNumHoldingAlphaTime()
	return	g_fDmgNumHoldingAlphaTime;
end
function	GetDmgNumHoldingScale()
	return	g_fDmgNumHoldingScale;
end
function	GetDmgNumHoldingAfterEffectTime()
	return	g_fDmgNumHoldingAfterEffectTime;
end

function	GetHpGaugeBlinkColorR()
	return	g_fHpGaugeBlinkColor[1];
end
function	GetHpGaugeBlinkColorG()
	return	g_fHpGaugeBlinkColor[2];
end
function	GetHpGaugeBlinkColorB()
	return	g_fHpGaugeBlinkColor[3];
end
function	GetHpGaugeBlinkColorA()
	return	g_fHpGaugeBlinkColor[4];
end
g_iCustomEnchantLevel = -1
function GetEnchantLevel()
	return	g_iCustomEnchantLevel
end
function SetEnchantLevel(iLevel)
	g_iCustomEnchantLevel = iLevel
end

g_fSyncMoveDistance = 20.0;
function    GetSyncMoveDistance()

    return  g_fSyncMoveDistance;
end

function    GetBloom_GaussBlurDev()
    return  g_fBloom_GaussBlurDev;
end
function    GetBloom_GaussBlurMul()
    return  g_fBloom_GaussBlurMul;
end
function    GetBloom_BloomIntensity()
    return  g_fBloom_BloomIntensity;
end
function    GetBloom_HilightIntensity()
    return  g_fBloom_HilightIntensity;
end
function    GetBloom_SceneIntensity()
    return  g_fBloom_SceneIntensity;
end

g_bUseBatchRender = true
function	UseBatchRender()
	return	g_bUseBatchRender;
end

g_bUpdateSceneRoot = true;
function    UpdateSceneRoot()
    return  g_bUpdateSceneRoot;
end
g_bUpdateDynamicRoot = true;
function    UpdateDynamicRoot()
    return  g_bUpdateDynamicRoot;
end
g_bUpdateStaticRoot = true;
function    UpdateStaticRoot()
    return  g_bUpdateStaticRoot;
end
g_bUseSelectiveUpdateLOD = true;
function UseSelectiveUpdateLOD()
	return	g_bUseSelectiveUpdateLOD;
end

g_bUpdateSelectiveRoot = true;
function    UpdateSelectiveRoot()
    return  g_bUpdateSelectiveRoot;
end
g_bUpdateWorldObject = true;
function    UpdateWorldObject()
    return  g_bUpdateWorldObject;
end
g_bUpdatePhysX = true;
function    UpdatePhysX()
    return  g_bUpdatePhysX;
end

g_bUSE_PHYSX = true;
function    USE_PHYSX()
    return  g_bUSE_PHYSX;
end

g_bInvalidateUI = true
function InvalidateUI()
	return	g_bInvalidateUI;
end

g_bUseOffscreenDraw = false
function UseOffscreenDraw()
	return	g_bUseOffscreenDraw
end

g_bOptimizedUpdateDelay = 10
function	GetUpdateDelay()
	return	g_bOptimizedUpdateDelay
end


g_bUpdateActorPhysX = true;
function    UpdateActorPhysX()
    return  g_bUpdateActorPhysX;
end
g_bUpdateActorFSM = true;
function    UpdateActorFSM()
    return  g_bUpdateActorFSM;
end
g_bUpdateActorManager = true;
function    UpdateActorManager()
    return  g_bUpdateActorManager;
end
g_bUpdateController = true
function    UpdateController()
    return  g_bUpdateController
end
g_bUpdateSelected = true
function    UpdateSelected()
    return  g_bUpdateSelected
end
g_bDrawNameText = true
function    DrawNameText()
    return  g_bDrawNameText
end
g_bDrawActor = true
function DrawActor()
    return  g_bDrawActor
end
g_bUpdateActorDownwardPass = true;
function    UpdateActorDownwardPass()
    return  g_bUpdateActorDownwardPass
end

g_bDrawWorld = true;
function    DrawWorld()
    return  g_bDrawWorld;
end

g_bDrawWorldFast = false;
function    DrawWorldFast()
    return  g_bDrawWorldFast;
end

g_bDrawBG = true
function	DrawBG()
	return	g_bDrawBG;
end

function	SetDrawBG(bDrawBG)
	g_bDrawBG = bDrawBG;
end

g_bDrawObject = true
function DrawObject()
	return	g_bDrawObject
end

g_bDrawWorldText = true
function DrawWorldText()
	return	g_bDrawWorldText;
end

function SetDrawWorldText(bDrawWorldText)
	g_bDrawWorldText = bDrawWorldText;
end

g_bDrawETC  = true
function DrawETC()
	return	g_bDrawETC
end

g_bOnlyDrawSceneRoot = false;
function    OnlyDrawSceneRoot()
    return  g_bOnlyDrawSceneRoot;
end
g_bUpdateAlphaProcess = true;
function    UpdateAlphaProcess()
    return  g_bUpdateAlphaProcess;
end

g_bUseActorUpdateOptimize = true
function    UseActorUpdateOptimize()
	return	g_bUseActorUpdateOptimize;
end

g_bUpdateWorld = true;
function    UpdateWorld()
    return  g_bUpdateWorld;
end
g_iDrawWorldSkip = 0
function DrawWorldSkip()
    return  g_iDrawWorldSkip;
end
g_bBatchRender = false;
function BatchRender()
    return  g_bBatchRender
end

g_bDrawParticle = true;
function DrawParticle()
    return  g_bDrawParticle
end

g_bDrawHP = true;
function DrawHP()
    return  g_bDrawHP
end

g_bDrawTextBalloon = true;
function DrawTextBalloon()
    return  g_bDrawTextBalloon
end

g_bDrawShadow = true;
function DrawShadow()
    return  g_bDrawShadow
end

g_bUI_DRAW_UPDATE = true
function UI_DRAW_UPDATE()
    return  g_bUI_DRAW_UPDATE;
end

g_bUI_DRAW_DX = true;
function UI_DRAW_DX()
    return  g_bUI_DRAW_DX;
end
g_bDrawUI = true;
function DrawUI()
    return  g_bDrawUI;
end
g_bUpdateUnitQueue = true;
function UpdateUnitQueue()
    return  g_bUpdateUnitQueue;
end

g_bUseSphereTree = false;
function UseSphereTree()
	return	g_bUseSphereTree;
end

g_fMinSphereRadius = 100;
function GetMinSphereRadius()
	return	g_fMinSphereRadius
end

g_bUpdateWorldUpdateScript = true;
function UpdateWorldUpdateScript()
    return  g_bUpdateWorldUpdateScript;
end

g_bUpdateParticlePos = true;
function UpdateParticlePos()
    return  g_bUpdateParticlePos;
end

g_bUpdateTemporaryInput = true;
function UpdateTemporaryInput()
    return  g_bUpdateTemporaryInput;
end

g_bUpdateCamera = true;
function UpdateCamera()
    return  g_bUpdateCamera;
end

g_bUpdateProjectile = true;
function UpdateProjectile()
    return  g_bUpdateProjectile;
end

g_bUpdateTrail = true;
function UpdateTrail()
    return  g_bUpdateTrail;
end

g_bDoSimulate = true
function DoSimulate()
	return g_bDoSimulate
end

g_bDrawSkyBox = true
function DrawSkyBox()
	return	g_bDrawSkyBox
end

function ToggleSimulate()
	g_bDoSimulate = not g_bDoSimulate
end

function SetSkillDebugDialog(bUsed)
	g_bUseSkillDraw = bUsed;
end

function GetSkillDebugDialog()
	return g_bUseSkillDraw;
end

g_bUsePhysXThread = false;
function UsePhysXThread()
    return  g_bUsePhysXThread;
end

g_bDisplayAbilNo = false
function GetDisplayAbilNo()
	return g_bDisplayAbilNo
end

function SetDisplayAbilNo(bUsed)
	g_bDisplayAbilNo = bUsed;
end


function AddRidingObject(ObjNo, setmyPC, x, y, z)

	if ObjNo == nil then
		ObjNo = 5000401
	end

	--x_diff = x_diff+10;
	local guid = GUID("123")
	guid:Generate()
	--local pos = g_world:FindSpawnLoc("char_spawn_1")
	local pos = GetPlayer():GetPos()
	--pos:SetX(pos:GetX() )
	--pos:SetY(pos:GetY() )
	if x ~= nil and y ~= nil and z ~= nil then
		pos:SetX(x)
		pos:SetY(y)
		pos:SetZ(z)
	end

	local pilot = g_pilotMan:NewPilot(guid, ObjNo, 0,"OBJECT",2)
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), pos, 11)
		g_RidingObj = pilot:GetActor();
		g_RidingObj:ReserveTransitAction("a_ridingobject_move");
	end
end


function GetFSAAResize()
 return	2
end

function GetBloomDownScale()
	return	1/4
end

function GetBgSoundFadeInTime()
	return g_fBgSoundFadeInTime
end

function GetBgSoundFadeOutTime()
	return g_fBgSoundFadeOutTime
end

function SetLimitLength_CharSelect(kWnd)
	if(nil == kWnd) then return end
	if(true == kWnd:IsNil()) then  return  end
	
	--! 유럽은 12글자 
	if( GetLocale() ==  LOCALE.NC_EU
		or GetLocale() ==  LOCALE.NC_FRANCE 
		or GetLocale() ==  LOCALE.NC_GERMANY 
		or GetLocale() == LOCALE.NC_USA ) then
		kWnd:SetEditLimitLength(12); return
	end
	
	--! 그 외는 모두 10글자 
	kWnd:SetEditLimitLength(10); return
	
end

function InitSystemValues()
	g_bUseBreakFallWhenStandUp = true	--쓰러졌다가 일어날때 옆으로 구르는 기능을 쓸것인가
	g_fAutoFireDelayTime = {0,0.45,0.45,0} -- 전사,법사,궁수,도적 순으로 각 직업별로 딜레이를 다르게 입력.	
	g_fAutoFireDelayTime[51] = 0.45	-- 소환사
	g_fAutoFireDelayTime[52] = 0	-- 격투가
end

function di(fScaleTime, fMoveTimeScale, iType)
	if( nil == iType) then iType = 0; end
	if (nil == fScaleTime)  then  fScaleTime = 0; end
	if (nil == fMoveTimeScale) then fMoveTimeScale = 0; end
	
	if(0 == iType or 1 == iType) then
		g_fDmgScaleTime_NUM = fScaleTime;		
		g_fDmgNumMoveTimeScale_NUM = fMoveTimeScale;
	end
	
	if(0 == iType or 2== iType) then
		g_fDmgScaleTime_CRITICAL= fScaleTime;
		g_fDmgNumMoveTimeScale_CRITICAL = fMoveTimeScale;
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:InitDmgNum();
end
--함수로 인해 다시 초기화 시켜준다
InitSystemValues()