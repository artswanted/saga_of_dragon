-- Client 에서 사용하는 함수
function GetActionResultCS(action, actor)
	if actor:IsMyActor() == false then
		return
	end
	local	iTargetCount = action:GetTargetCount();		
	local	i = 0;
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	while i<iTargetCount do
		local	kTargetGUID = action:GetTargetGUID(i);
		local	kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);		
		if kTargetPilot:IsNil() == false then
			kUnitArray:AddUnit(kTargetPilot:GetUnit())
		end
		i = i + 1
	end			
		
	local kActionResult = NewActionResultVector()
	action:Call_Skill_Result_Func(actor:GetPilot(),kUnitArray,kActionResult)
	action:SetTargetActionResult(kActionResult, actor:IsMyActor() == false)

	DeleteActionResultVector(kActionResult)
	DeleteUNIT_PTR_ARRAY(kUnitArray)
end

-----------------------------------------------------------------
--	Monster Skill
-----------------------------------------------------------------
function GetSkillResult1(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- lavalon breath skill
function GetSkillResult6000900(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult6000901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult6000902(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult6000903(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 독거리 독 스킬
function GetSkillResult6003101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 베가본드 돌진하기 : a_mon_dash_piercing_attk
function GetSkillResult6004904(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 마할카 휠윈드 (제자리) : a_mon_whirlwind
function GetSkillResult6006902(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 마할카 휠윈드 전진하기 : a_mon_whirlwind_move
function GetSkillResult6006903(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 데스마스터 기모았다 발산 : a_mon_energy_explosion
function GetSkillResult6009602(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 데스마스터 블럭킹 : a_mon_blocking
function GetSkillResult6009603(caster, kTargetArray, skillnum, result)
	return 0
end

-- 데스마스터 대지의 분노 : a_mon_earthquake
function GetSkillResult6009604(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 압둘라 넷바인더 : a_mon_shot_skill_03
function GetSkillResult6013903(caster, kTargetArray, skillnum, result)
	return 0
end

-- 압둘라 스스로 힐하기 : a_mon_melee_02
function GetSkillResult6013904(caster, kTargetArray, skillnum, result)
	return 0
end

-- 압둘라 단체 힐하기 : a_mon_melee_03
function GetSkillResult6013905(caster, kTargetArray, skillnum, result)
	return 0
end

-- -- 삼바바 점프해서 바닥 찍기 : a_mon_jumpdowncrash
function GetSkillResult6015903(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-- -- 우마이 바닥 가시 : a_mon_summon_thorn_sa
function GetSkillResult60189041(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult142(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult143(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--common Skill
-----------------------------------------------------------------
function GetSkillResult100007001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--전사  Skill
-----------------------------------------------------------------
-- Fighter (인생은 한방 : Hammer Crush) : Level 1-10
function GetSkillResult101000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (불타는 인생 : a_Fire Youth) : Level 1-10
function GetSkillResult101001101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (비정한 인생 : a_Heartless Life) : Level 1-10
function GetSkillResult101001201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (형 왔다 : Brother is Return) : Level 1-10
function GetSkillResult101000201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (머리로 방어 : Increased Defence) : Level 1-10
function GetSkillResult101000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Fighter (망각 : Forget Pain) : Level 1-5
function GetSkillResult101000401(caster, kTargetArray, skillnum, result)
	return 0
end

-- Fighter (형 간다! : Brother is Gone!) : Level 1-5
function GetSkillResult101000501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (미련 곰퉁이 : Bear Hank ) : Level 1-10
function GetSkillResult101000601(caster, kTargetArray, skillnum, result)
	return 0
end

-- Fighter (갑옷부수기 : Armor Bread ) : Level 1-10
function GetSkillResult101000701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (스톰 블레이드 : Storm blade ) : Level 1
function GetSkillResult101000801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (숄더 태클 : Shoulder Tackle ) : Level 1
function GetSkillResult101000901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Fighter (소드 댄스 : Sword dande ) : Level 1
function GetSkillResult101001001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 공통 (기본 전사 타격 2타 : a_melee_02)
function GetSkillResult101100201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101100901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101101901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 기본 전사 짧은 전진 2타 ~ 5타
function GetSkillResult101102101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101102401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 기본 전사 긴 전진 2타 ~ 5타
function GetSkillResult101300201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101300301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101300401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult101300501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--마법사  Skill
-----------------------------------------------------------------
function GetSkillResult103102001(caster, kTargetArray, skillnum, result)
	return 0
end

-- Magician (미사일 발사! : Missile) : Level 1-10
function GetSkillResult102000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Magician (마녀의 저주 : a_Transformation) : Level 1-5
function GetSkillResult102000201(caster, kTargetArray, skillnum, result)
	return 0
end

-- Magician (찾았다? : Detection) : Level 1-5
function GetSkillResult102000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Magician (바바이안! : Burberrian) : Level 1-5
function GetSkillResult102000401(caster, kTargetArray, skillnum, result)
	return 0
end

-- Magician (힐은 셀프! : Self Heal) : Level 1-5
function GetSkillResult102000501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 매지션 (마나실드 : a_Mana Shield ) : Level 1-5 : Toggle
function GetSkillResult102000701(caster, kTargetArray, skillnum, result)
	return 0
end

-- 마법사 (정신일도하사불성 : Concentration) : Level 1-10
function GetSkillResult102000801(caster, kTargetArray, skillnum, result)
	return 0
end

-- a_ThreeWay_Cast : 확산포 캐스팅
function GetSkillResult102001001(caster, kTargetArray, skillnum, result)
	return 0
end

-- a_ThreeWay_Cast : 확산포 발사
function GetSkillResult102002001(caster, kTargetArray, skillnum, result)
	--InfoLog(9, "GetSkillResult102002001 --")
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- a_lightning_armor : 라이트닝 아머
function GetSkillResult102001101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102100901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- a_magician_charge_laser : 법사 응집공격
function GetSkillResult102101201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102101801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102102101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102102201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102102301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult102102401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Magician ( 슬로우 힐: a_Slow Heal) : Level 1-5
function GetSkillResult102002401(caster, kTargetArray, skillnum, result)
	return 0
end

-----------------------------------------------------------------
--궁수  Skill
-----------------------------------------------------------------
-- Archer (다연발 사격 : Multi Shot) : Level 1-10
function GetSkillResult103000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Archer (꼬치구이 : Thrust) : Level 1-10
function GetSkillResult103000201(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_FireDefault....SkillDef is NIl" ..skillnum)
		return -1
	end
	local iDmgPercent = {}
	iDmgPercent[0] = ABILITY_RATE_VALUE
	iDmgPercent[1] = ABILITY_RATE_VALUE * 0.5
	iDmgPercent[2] = ABILITY_RATE_VALUE * 0.2
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		CS_GetDmgResult(skillnum, caster, target, iDmgPercent[iIndex], 0, aresult)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end -- while
	return iIndex
end

-- Archer (언니 이뻐? : Beautiful Girl) : Level 1-5
function GetSkillResult103000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Archer (놀림을 딛고 : Increased Status) : Level 1-10
function GetSkillResult103000401(caster, kTargetArray, skillnum, result)
	return 0
end

-- Archer (경쾌한 발놀림 : Wind Step) : Level 1-10
function GetSkillResult103000501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 궁수 (큐션샷 : Cushion Shot) : Level 1-5
function GetSkillResult103000601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 궁수 공통 (EMP : EMP) : Level 1-5
function GetSkillResult103000801(caster, kTargetArray, skillnum, result)
	return 0
end

-- 궁수 공통 (Siege Cannon : Siege Cannon Fire) : Level 1-5
function GetSkillResult103000901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103101001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103101101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103101201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 더블샷 캐스팅
function GetSkillResult103101801(caster, kTargetArray, skillnum, result)
	return 0
end
-- 더블샷 발사
function GetSkillResult103101601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103101701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (도둑 공중콤보 1단 : a_thief_float_melee_01) : 기본공격
function GetSkillResult103200101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (도둑 공중콤보 2단 : a_thief_float_melee_02) : 기본공격
function GetSkillResult103200201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (도둑 공중콤보 3단 : a_thief_float_melee_03_01) : 기본공격
function GetSkillResult103200301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (도둑 공중콤보 3단-1 : a_thief_float_melee_03_02) : 기본공격
function GetSkillResult103200401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (도둑 공중콤보 4단 : a_thief_float_melee_04) : 기본공격
function GetSkillResult103200501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (도둑 공중콤보 5단 : a_thief_float_melee_05) : 기본공격
function GetSkillResult103200601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103200701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103200801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103200901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103201001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 도둑 기본 공격 1타 : a_thi_melee_01
function GetSkillResult103201101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 도둑 기본 공격 2타 : a_thi_melee_02
function GetSkillResult103201201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end

-- 도둑 기본 공격 3타 : a_thi_melee_03
function GetSkillResult103201301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 도둑 기본 공격 4타 : a_thi_melee_04
function GetSkillResult103201401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103201501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103202201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 기본 도둑 짧은 전진 2타 ~ 5타
function GetSkillResult104302501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104302601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104302701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104302801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 기본 도둑 긴 전진 2타 ~ 5타
function GetSkillResult104302901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104303001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104303101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104303201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--도적 Skill
-----------------------------------------------------------------
-- Thief (찾아보시지 : Shadow Walk) : Level 1-10
function GetSkillResult104000101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Thief (보고도 못 본척 : Shadow Walk) : Level 1-10
function GetSkillResult104000201(caster, kTargetArray, skillnum, result)
	return 0
end

-- Thief (독 살포 : Venom) : Level 1-5
function GetSkillResult104000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Thief (로케트펀치! : Rocket Punch) : Level 1-10
function GetSkillResult104000401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (급습! : Raid) : Level 1-5
function GetSkillResult104000501(caster, kTargetArray, skillnum, result)
	local iReturn = CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)

	local kSkillDef = GetSkillDef(skillno)
	if kSkillDef:IsNil() then
		InfoLog(5, "GetSkillResult104000501....SkillDef is NIl" ..skillno)
		return -1
	end
	local iExtra = kSkillDef:GetAbil(AT_ADDVALUE)
	local iCasterDir = caster:GetFrontDirection()
	-- 뒤에서 공격한것인지 검사해서 추가 Damage를 줘야 한다.
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local iTargetDir = target:GetFrontDirection()
		--InfoLog(9, "GetSkillResult104000501 CasterDir:" .. iCasterDir .. ", TargetDir:" .. iTargetDir)
		if iCasterDir == iTargetDir then	-- 뒤에서 공격한 것이다.
			local aresult = result:GetResult(target:GetGuid(), true)
			local iDamage = aresult:GetValue()
			iDamage = iDamage + iDamage * iExtra / ABILITY_RATE_VALUE
			aresult:SetValue(iDamage)
			--InfoLog(9, "GetSkillResult104000501...ExtraDmg : " .. iDamage * iExtra / 100)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iReturn
end

-- 공통도둑 (버로우 : Burrow) : Level 1-5
function GetSkillResult104000601(caster, kTargetArray, skillnum, result)
	return 0
end
-- 공통도둑 (버로우 : Burrow) : Level 1-5

function GetSkillResult170001401(caster, kTargetArray, skillnum, result)
	return 0
end


-- 도둑 공통 (더블어택 : a_Double Attack) : Level 1-10
function GetSkillResult104000701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104100101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104100201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Thief (스트립 웨폰 : a_Strip Weapon) : Level 1-5
function GetSkillResult104300201(caster, kTargetArray, skillnum, result)
	return 0
end

-- 광대 (소닉 붐 : a_Sonic Boom) : Lv 1-10
function GetSkillResult104300401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 광대 (바이올랜스 블로우 : a_Violence Blow) : Lv 1-10
function GetSkillResult104300501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 광대 (스크류 어퍼 : a_Screw Upper) : Lv 1-5
function GetSkillResult104300601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 광대 (더블 크래시: a_Double Crash) : Lv 1-5
function GetSkillResult104300701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 광대 (체인 크래시: a_Chain Crash) : Lv 1-5
function GetSkillResult104300801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 광대 (파이널 디시전: a_Final Decision) : Lv 1-10
function GetSkillResult104300901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult104302001(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult104302101(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult104302201(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult104302301(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 암살자 (쓰로잉 대거: a_Throwing Dagger) : Lv 1-5
function GetSkillResult104301001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 암살자 (기믈렛: a_Gimmlet) : Lv 1-10
function GetSkillResult104301101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 암살자 (마인드 리딩: a_Mind Reading) : Lv 1-10
function GetSkillResult104301201(caster, kTargetArray, skillnum, result)
	return 0
end

-- 암살자 (섀도우 워크: a_Shadow Walk) : Lv 1-10 (Toggle)
function GetSkillResult104301301(caster, kTargetArray, skillnum, result)
	return 0
end

-- 암살자 (난도질: a_Haphazard) : Lv 1-10
function GetSkillResult104301401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 암살자 (베놈 크래셔: a_Venom Crasher) : Lv 1-5
function GetSkillResult104301501(caster, kTargetArray, skillnum, result)
	return 0
end

-- a_Somersaultkick : 여름소금차기 : Level 1-5
function GetSkillResult104301601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- a_Judge Dread : 저지 드레드 : Level 1-5
function GetSkillResult104301701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- a_Swiftattack : 스위프트 어택 : Level 1-10
function GetSkillResult104301801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170001101(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult170001501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170001601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170001701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170001801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170001901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170001501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult170002701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult170002801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170002901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170003001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-----------------------------------------------------------------
--기사 Skill
-----------------------------------------------------------------
-- 기사 (방패던지기 : Shield Boomerang ) : Level 1-10
function GetSkillResult105000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 기사 (제네식기어-수호 : Genesic Gear - Protect ) : Level 1-10
function GetSkillResult105000201(caster, kTargetArray, skillnum, result)
	return 0
end
-- 기사 (신성수호 : Divine Protection ) : Level 1-5
function GetSkillResult105000301(caster, kTargetArray, skillnum, result)
	return 0
end
-- 기사 (패링 : a_Parring ) : Level 1-5
function GetSkillResult105300201(caster, kTargetArray, skillnum, result)
	return 0
end
-- 기사 (웨폰 프리징 : a_Weapon Freezing) : Level 1-10
function GetSkillResult105300301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 기사 (스피어 잽 : a_Spear Jab) : Level 1-10
function GetSkillResult105300401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 기사 (조인트 브레이크 : a_Joint Break) : Level 1-5
function GetSkillResult105300501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result)
end

-- 기사 (리벤지 : a_Revenge) : Level 1-5
function GetSkillResult105300601(caster, kTargetArray, skillnum, result)
	return 0
end

-- 기사 형 돈다 시전 : a_Knight_Bro_Spin : Level 1
function GetSkillResult101103201(caster, kTargetArray, skillnum, result)
	return 0
end
--  기사 형 돈다 발사: a_Knight_Bro_Spin_Fire : Level 1
function GetSkillResult101103301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--  기사 형 돈다 발사: a_Knight_Bro_Spin_Fire : Level 2
function GetSkillResult101103302(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--  기사 형 돈다 발사: a_Knight_Bro_Spin_Fire : Level 3
function GetSkillResult101103303(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--  기사 형 돈다 발사: a_Knight_Bro_Spin_Fire : Level 4
function GetSkillResult101103304(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--  기사 형 돈다 발사: a_Knight_Bro_Spin_Fire : Level 5
function GetSkillResult101103305(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--투사 Skill
-----------------------------------------------------------------
-- 투사 (룰렛 : a_Roulette) : Level 1-5
function GetSkillResult106000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult106000101(skillnum, caster, kTargetArray, result)	
end

-- 투사 (제네식기어-공격 : Genesic Gear - Offense ) : Level 1-10
function GetSkillResult106000201(caster, kTargetArray, skillnum, result)
	return 0
end

-- 투사 (액셀레이션 : a_Acceleration) : Level 1-10
function GetSkillResult106300201(caster, kTargetArray, skillnum, result)
	return 0
end

-- 투사 (섬멸 : a_Annihilation) : Level 1-10
function GetSkillResult106300301(caster, kTargetArray, skillnum, result)
	return 0
end

-- 투사 (스텀블펌 : a_Stumblebum) : Level 1-5
function GetSkillResult106300401(caster, kTargetArray, skillnum, result)
	return 0
end

-- 투사 (와일드 레이지 : a_Wild Rage) : Level 1-10
function GetSkillResult106300501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 투사 (그로우 바디 : a_Grow Body) : Level 1-10
function GetSkillResult106300601(caster, kTargetArray, skillnum, result)
	return 0
end

-- 투사 ( 휠 윈드: a_Wheel Wind) : Level 1-10
function GetSkillResult106500201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-- 투사 ( 대지의 분노 : a_Earthquake) : Level 1-10
function GetSkillResult106500401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-----------------------------------------------------------------
--메이지 Skill
-----------------------------------------------------------------
-- 메이지 (엄마손은 약손 : Heal) : Level 1-10
function GetSkillResult107000101(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult107000201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 메이지 (와이드 힐 : a_Wide Heal) : Level 1-10
function GetSkillResult107000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- 메이지 (인스탄스 힐 : a_Instance Heal) : Level 1-5
function GetSkillResult107000401(caster, kTargetArray, skillnum, result)
	return 0
end

-- 메이지 (큐어 : a_Cure) : Level 1-5
function GetSkillResult107000501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 메이지 (에너지 버스터 : a_Energy Burster) : Level 1-5
function GetSkillResult109003001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 메이지 (감염 : a_Disease) : Level 1-10
function GetSkillResult107000701(caster, kTargetArray, skillnum, result)
	return 0
end

-- Mage ( 캐스팅 고속화: a_Fast Cast) : Level 1-5
function GetSkillResult109002901(caster, kTargetArray, skillnum, result)
	return 0
end

-- Mage ( Rain of Fire: a_Rain_Of_Fire) : Level 1-5
function GetSkillResult102003201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-----------------------------------------------------------------
--전투마법사 Skill
-----------------------------------------------------------------
-- 배틀메이지 (블리자드 공격 : a_Blizzard_Attack) : Level 1-5
function GetSkillResult1080001011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--사냥꾼 Skill
-----------------------------------------------------------------
function GetSkillResult109000201(caster, kTargetArray, skillnum, result)
	return 0
end

-- 배틀 메이지 (콰그마이어) : Level 1-10
function GetSkillResult109000401(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 배틀 메이지 (매직 미사일 : a_Magic Missile) : Level 1-10
function GetSkillResult109000501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀 메이지 (스파이럴 캐논 : a_Spiral Cannon) : Level 1-5
function GetSkillResult109000601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀 메이지 (토네이도 : a_Tornado) : Level 1-5
function GetSkillResult109000701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀 메이지 (디멘션 커터 : a_Dimension Cutter) : Level 1-10
function GetSkillResult109000801(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_FireDefault....SkillDef is NIl" ..skillnum)
		return -1
	end
	-- 물리공격력 + 스킬의절대값
	local iDmgAdd = caster:GetAbil(AT_C_PHY_ATTACK) + kSkillDef:GetAbil(AT_MAGIC_ATTACK_MAX)
	-- 스킬의 마법공격력 증폭
	local iDmgPer = kSkillDef:GetAbil(AT_MAGIC_DMG_PER)
	if iDmgPer == 0 then
		iDmgPer = ABILITY_RATE_VALUE
	end
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		CS_GetDmgResult(skillnum, caster, target, iDmgPer, iDmgAdd, aresult)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end -- while
	return iIndex
end

-- 배틀메이지 (애너지 볼 : a_Energy_Ball) : Level 1-5
function GetSkillResult109000901(caster, kTargetArray, skillnum, result)
	return 0
end
-- 배틀메이지 (에너지볼 발사 : a_Energy_Ball_Fire) : Level 1-5
function GetSkillResult1090009011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀메이지 (에너지볼 발사 : a_Energy_Ball_Fire) : Level 1-5
function GetSkillResult1090009012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀메이지 (에너지볼 발사 : a_Energy_Ball_Fire) : Level 1-5
function GetSkillResult1090009013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀메이지 (에너지볼 발사 : a_Energy_Ball_Fire) : Level 1-5
function GetSkillResult1090009014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 배틀메이지 (에너지볼 발사 : a_Energy_Ball_Fire) : Level 1-5
function GetSkillResult1090009015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 메이지 (부활 : a_Resurrection) : Level 1-5
function GetSkillResult109001001(caster, kTargetArray, skillnum, result)
	return 0
end

-----------------------------------------------------------------
--레인저 Skill
-----------------------------------------------------------------
-- 궁수 (로켓 런처 : a_Daepodong) : Level 1-5
function GetSkillResult110000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (화살비 : Arrow Shower) : Level 1-10
function GetSkillResult110000201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 궁수 (더블 스트라이크 : a_DoubleShot) : Level 1-10
function GetSkillResult110000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 궁수 (출혈 : a_Bleeding) : Level 1-10
function GetSkillResult110000401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 사냥꾼 ( 크럭스 샷: a_Crux Shot) : Level 1-10
function GetSkillResult110000701(caster, kTargetArray, skillnum, result)
	return 0
end

-- 사냥꾼 (스플래쉬 트랩 폭발 : a_Splash Trap_Explosion) : Level 1-5
function GetSkillResult1100008011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 사냥꾼 (환각 : a_Displacement) : Level 1-10
function GetSkillResult110000901(caster, kTargetArray, skillnum, result)
	return 0
end

-- 사냥꾼 (애시드 애로우 : a_Acid Arrow) : Level 1-5
function GetSkillResult110001001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 사냥꾼 (팔콘비트 : a_Falcon Beat) : Level 1-5
function GetSkillResult110001101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 사냥꾼 (울프러쉬 : a_Wolf Rush) : Level 1-10
function GetSkillResult110001201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 사냥꾼 (샤프 슈탕 : a_Sharp Shooting) : Level 1-5
function GetSkillResult110001301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (클레이모어 폭발 : a_Claymore_Explosion) : Level 1-10
function GetSkillResult1100014011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (자동 사격 장치 : a_Auto Shoot System) : Level 1-5
function GetSkillResult110001501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레인저 (자동 사격 장치 발사 : a_Auto Shoot System_Fire) : Level 1-5
function GetSkillResult1100015011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (자동 사격 장치 발사 : a_Auto Shoot System_Fire) : Level 1-5
function GetSkillResult1100015012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (자동 사격 장치 발사 : a_Auto Shoot System_Fire) : Level 1-5
function GetSkillResult1100015013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (자동 사격 장치 발사 : a_Auto Shoot System_Fire) : Level 1-5
function GetSkillResult1100015014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (자동 사격 장치 발사 : a_Auto Shoot System_Fire) : Level 1-5
function GetSkillResult1100015015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (디토네이터 : a_Detonator) : Level 1
function GetSkillResult110001601(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레인저 (카모플라쥬 : a_Camouflage) : Level 1-5
function GetSkillResult110001701(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레인저 (넷 바인더 : a_Net Binder) : Level 1-10 (Toggle)
function GetSkillResult110001801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(caster, kTargetArray, skillnum, result)
end

-- 레인저 (멀티샷 : a_Multiple Shot) : Level 1-5
function GetSkillResult110001901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (난사 캐스트 : a_Rapidly Shot_Cast) : Level 1-5
function GetSkillResult110002001(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레인저 (난사 발사 : a_Rapidly Shot_Fire)
function GetSkillResult110002101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 (공중 5연사 : a_ran_floatdownshot)
function GetSkillResult110002201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레인저 투액션 샷 캐스팅 : 103103001
function GetSkillResult103103001(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레인저 투액션 샷 : a_twoaction_fire
function GetSkillResult103104001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--성기사 Skill
-----------------------------------------------------------------
-- Paladin (프레셔 : a_Pressure) : Level 1-10
function GetSkillResult106400101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Paladin (크로스 컷 : a_Cross Cut) : Level 1-5
function GetSkillResult105500601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Paladin (프레셔 : a_Pressure) : Level 1-10
function GetSkillResult105500701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Paladin (바리케이드 : a_Barricade) : Level 1-10 (Toggle)
function GetSkillResult105500801(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (리커버리 오라 : a_Recovery Aura) : Level 1-5 (Toggle)
function GetSkillResult105500901(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (타임 리버스 : a_Time Reverse) : Level 1-10 (Toggle)
function GetSkillResult105501001(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (오펜시브 오라 : a_Offencsive Aura) : Level 1-10 (Toggle)
function GetSkillResult105501101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (디펜시브 오라 : a_Defensive Aura) : Level 1-10 (Toggle)
function GetSkillResult105501201(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (성스러운 보호 : a_Holy Armor) : Level 1-10 (Toggle)
function GetSkillResult105501301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (웨폰 크래셔 : a_Weapon Crasher) : Level 1-10
function GetSkillResult105501501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Paladin (바퀴벌레 인생 : a_Bugs Life) : Level 1-1
function GetSkillResult105501601(caster, kTargetArray, skillnum, result)
	return 0
end

-- Paladin (기원의 오라 : a_Prayer) : Level 1-10
--function GetSkillResult106400401(caster, kTargetArray, skillnum, result)
--	return 0
--end

--Paladin (도발 : a_taunt) Level 1-3
function GetSkillResult105501801(caster, kTargetArray, skillnum, result)
	return 0
end

--메가스톰 블레이드 a_mega_stormblade		
function GetSkillResult105400301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--체인쉴드 발동 a_chainshield_play
function GetSkillResult105400201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult105400202(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult105400203(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end



--쉴드스트라이크 a_shieldstrike
function GetSkillResult105400401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--필중 a_hitrate_up
function GetSkillResult105400501(caster, kTargetArray, skillnum, result)
	return 0
end
-----------------------------------------------------------------
--용기사 Skill
-----------------------------------------------------------------
-- DragonKnight (승천룡) 
function GetSkillResult2000110101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- DragonKnight (드래곤 다이브: a_dragon_dive) Level 1-5
function GetSkillResult2000120101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- DragonKnight (드래곤 다이브 막타: a_dragon_dive_finish) Level 1-5
function GetSkillResult2000120201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-----------------------------------------------------------------
--검투사 Skill
-----------------------------------------------------------------
-- Gladiator ( 거스트 슬래시: a_Gust Slash) : Level 1-10
function GetSkillResult106500501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 매그넘 브레이크: a_Magnum Break) : Level 1-5
function GetSkillResult106500601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 데스 바운드: a_Death Bound) : Level 1-5
function GetSkillResult106500701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 웨폰 퀴큰: a_Weapon Quicken) : Level 1-5
function GetSkillResult106500801(caster, kTargetArray, skillnum, result)
	return 0
end

-- Gladiator ( 웨폰 브레이크: a_Weapon Break) : Level 1-5
function GetSkillResult106500901(caster, kTargetArray, skillnum, result)
	return 0;
end

-- Gladiator ( 레이징 스톰 발사: a_Weapon Break_Fire) : Level 1-5
function GetSkillResult1065009011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 레이징 스톰 발사: a_Weapon Break_Fire) : Level 1-5
function GetSkillResult1065009012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- Gladiator ( 레이징 스톰 발사: a_Weapon Break_Fire) : Level 1-5
function GetSkillResult1065009013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- Gladiator ( 레이징 스톰 발사: a_Weapon Break_Fire) : Level 1-5
function GetSkillResult1065009014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- Gladiator ( 레이징 스톰 발사: a_Weapon Break_Fire) : Level 1-5
function GetSkillResult1065009015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 블러디 로어: a_Bloody Lore) : Level 1-5
function GetSkillResult106501201(caster, kTargetArray, skillnum, result)
	return 0
end

-- Gladiator ( 와이번 블레이드: a_Wyvern Blade) : Level 1-5
function GetSkillResult106501101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 블러디 로어 타격 1 : a_Bloody_melee_01 ) : Level 1
function GetSkillResult101102901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 블러디 로어 타격 2 : a_Bloody_melee_01 ) : Level 1
function GetSkillResult101103001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Gladiator ( 블러디 로어 타격 3 : a_Bloody_melee_01 ) : Level 1
function GetSkillResult101103101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--위자드 Skill
-----------------------------------------------------------------
-- Wizard ( 체인 라이트닝: a_Chain Lighting) : Level 1-10
function GetSkillResult109001501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard ( 스트라이킹 : a_Striking) : Level 1-10
function GetSkillResult109001301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard ( 썬더 브레이크: a_Thunder Break) : Level 1-5
function GetSkillResult109001401(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard ( 썬더 브레이크<효과>: a_Thunder Break_Target) : Level 1-5
function GetSkillResult1090014001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard ( HP 리스토어: a_HP Restore) : Level 1-10
function GetSkillResult109001601(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard ( MP 전이: a_MP Transition) : Level 1-5
function GetSkillResult109001701(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard ( 배리어 : a_Barrier) : Level 1-5
function GetSkillResult109001801(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard ( 완전한 결계 : a_Perfect Sanctuary) : Level 1-5
function GetSkillResult109001901(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard ( 블레싱 : a_Blessing) : Level 1-5
function GetSkillResult102002501(caster, kTargetArray, skillnum, result)
	return 0
end

-- Wizard (스파이크 롹 시전: a_Spark_Rock) : Lv 1-5
function GetSkillResult102003501(caster, kTargetArray, skillnum, result)
	return 0;
end

-- Wizard (스파이크 롹 발동: a_Spark_Rock_Play) : Lv 1-5
function GetSkillResult102003601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard (스파이크 롹 발동: a_Spark_Rock_Play) : Lv 1-5
function GetSkillResult102003602(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard (스파이크 롹 발동: a_Spark_Rock_Play) : Lv 1-5
function GetSkillResult102003603(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard (스파이크 롹 발동: a_Spark_Rock_Play) : Lv 1-5
function GetSkillResult102003604(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard (스파이크 롹 발동: a_Spark_Rock_Play) : Lv 1-5
function GetSkillResult102003605(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 긴급후퇴
function GetSkillResult108000201(caster, kTargetArray, skillnum, result)
	return 0;
end

-----------------------------------------------------------------
--대마법사 Skill
-----------------------------------------------------------------
-- Arcmage ( 레이스오브헤븐: a_WrathOfHeaven) 
function GetSkillResult2000320101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-----------------------------------------------------------------
--워매이지 Skill
-----------------------------------------------------------------
-- War Mage ( 백 드래프트: a_Back Draft) : Level 1-5
function GetSkillResult109002401(caster, kTargetArray, skillnum, result)
	return 0;
end

-- War Mage ( 백 드래프트: a_Back Draft Fire) : Level 1-5
function GetSkillResult1090024011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 백 드래프트: a_Back Draft Fire) : Level 1-5
function GetSkillResult1090024012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 백 드래프트: a_Back Draft Fire) : Level 1-5
function GetSkillResult1090024013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 백 드래프트: a_Back Draft Fire) : Level 1-5
function GetSkillResult1090024014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 백 드래프트: a_Back Draft Fire) : Level 1-5
function GetSkillResult1090024015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬: a_Cloud Kill) : Level 1-5
function GetSkillResult109002501(caster, kTargetArray, skillnum, result)
	return 0;
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025016(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025017(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025018(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025019(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 클라우드 킬 발사: a_Cloud Kill Fire) : Level 1-10
function GetSkillResult1090025020(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Wizard (미러 이미지 : a_Mirror Image ) : Level 1-5
function GetSkillResult109002001(caster, kTargetArray, skillnum, result)
	return 0
end

-- War Mage (리소스 컨버터 : a_Resource Converter ) : Level 1-10
function GetSkillResult109002101(caster, kTargetArray, skillnum, result)
	return 0
end

-- War Mage ( 먹물 발사!: a_Octopus Ink) : Level 1-5
function GetSkillResult109002201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 먹물 발사!: a_Octopus Ink_Fire) : Level 1-5
function GetSkillResult1031051011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage (스노우 블루스 : a_Snow Blues ) : Level 1
function GetSkillResult109002301(caster, kTargetArray, skillnum, result)
	return 0
end

-- War Mage (스노우 블루스 작동 : a_Snow Blues ) : Level 1
function GetSkillResult1090023011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage (스노우 블루스 작동 : a_Snow Blues ) : Level 1
function GetSkillResult1090023012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage (스노우 블루스 작동 : a_Snow Blues ) : Level 1
function GetSkillResult1090023013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage (스노우 블루스 작동 : a_Snow Blues ) : Level 1
function GetSkillResult1090023014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage (스노우 블루스 작동 : a_Snow Blues ) : Level 1
function GetSkillResult1090023015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- War Mage ( 메테오 스트라이크: a_Meteor Strike) : Level 1-10
function GetSkillResult109002601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--워로드 Skill
-----------------------------------------------------------------
-- WarLord ( 소울드레인 )
function GetSkillResult2000420101(caster, kTargetArray, skillnum, result)
	return 0
end
function GetSkillResult2000420201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult2000420401(caster, kTargetArray, skillnum, result)
	return 0
end

-- WarLord( 마그마피스톤 )
function GetSkillResult2000410101(caster, kTargetArray, skillnum, result)
	return 0
end

-- WarLord ( 마그마피스톤 의 마그마)
function GetSkillResult2000410201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--트랩퍼 Skill
-----------------------------------------------------------------
-- Trapper ( 건들지마라!: a_Don’t Touch Me) : Level 1
function GetSkillResult110002501(caster, kTargetArray, skillnum, result)
	return 0
end

-- Trapper ( 알 수 없는 요리!: a_Mysterious Cooking) : Level 1
function GetSkillResult140000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Trapper ( 프리징 트랩: a_Silence Trap) : Level 1-10
--function GetSkillResult110002601(caster, kTargetArray, skillnum, result)
--	return 0
--end

-- Trapper (프리징 트랩 폭발 : ) : Level 1-10
function GetSkillResult1100026011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Trapper ( 사일러스 트랩!: a_Silence Trap) : Level 1-10
--function GetSkillResult110002701(caster, kTargetArray, skillnum, result)
--	return 0
--end

-- Trapper (사일런스 트랩 폭발 : ) : Level 1-10
function GetSkillResult1100027011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Trapper ( MP제로: a_MP-Zero Trap) : Level 1-10
--function GetSkillResult110002801(caster, kTargetArray, skillnum, result)
--	return 0
--end

-- Trapper (MP제로 폭발: a_MP-Zero ) : Level 1-10 바운드 트랩으로 수정됨
--[[function GetSkillResult1100028011(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "GetSkillResult1100028011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iPercent = kSkillDef:GetAbil(AT_PERCENTAGE);
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			local iMP = target:GetAbil(AT_C_MAX_MP);
			local iValue = iMP * iPercent / ABILITY_RATE_VALUE;
			iValue = math.max(0, iValue);
			local iDamage = iValue * 0.5;
			aresult:SetValue(iDamage);
			--InfoLog(5, "GetSkillResult1100028011....TargetMp : " ..iMP.."Percent : "..iPercent)
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	--return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end]]--

-- Trapper ( 바운드 트랩: a_MP-Zero Trap) : Level 1-5
function GetSkillResult1100028011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(caster, kTargetArray, skillnum, result)
end


-- 트랩퍼 ( 스모크 그레네이드 : a_Smoke Granade) : Level 1-5
function GetSkillResult110002901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 트랩퍼 ( 스모크 그레네이드 설치: a_Smoke Granade Active) : Level 1-5
function GetSkillResult1100029011(caster, kTargetArray, skillnum, result)
	return 0
end

-- He그레네이드
function GetSkillResult110003001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 블릿츠
function GetSkillResult150002001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 테슬라코일
function GetSkillResult110003601(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 테슬라코일 발동
function GetSkillResult110003601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult110003602(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult110003603(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult110003604(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult110003605(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-----------------------------------------------------------------
--수호자 Skill
-----------------------------------------------------------------
-- Sentinel ( 윈드블레이드 : a_WindBlade) 
function GetSkillResult2000520101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-----------------------------------------------------------------
--저격수 Skill
-----------------------------------------------------------------
-- Sniper (정밀사격 : a_PinPoint Shot) Level 1-10
function GetSkillResult150000001(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "GetSkillResult150000001....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iSkillDmg = CS_GetSkillPower(caster, skillnum)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if target:GetUnitType() == UT_BOSSMONSTER then
			-- BossMonster 이면 평소 방식 대로
			CS_GetDmgResult(skillnum, caster, target, ABILITY_RATE_VALUE, 0, aresult)
		else
			-- BossMonster가 아니면, Target의 방어력을 무시한 Damage를....
			local iSkillPRate = kSkillDef:GetAbil(AT_PHY_DMG_PER)
			CS_CheckDamage(caster, target, iSkillDmg, true,aresult, iSkillPRate)
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end -- while
	return iIndex
end

-- Sniper (슬링 : a_Sling) Level 1-5
function GetSkillResult150000101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Sniper (부비트랩 : a_Booby Trap) Level 1-10
function GetSkillResult150000201(caster, kTargetArray, skillnum, result)
	return 0
end

-- Sniper (호밍 미사일 : a_Homing Missile) Level 1-5
function GetSkillResult150000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper (에이밍 : a_Aiming) Level 1-5
function GetSkillResult150000501(caster, kTargetArray, skillnum, result)
	return 0
end

-- Sniper (아드레날린 : a_Adrenalin) Level 1-10
function GetSkillResult150000701(caster, kTargetArray, skillnum, result)
	return 0
end

-- 저격수 ( WP그레네이드 : a_WP Granade) : Level 1-5
function GetSkillResult150000801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 저격수 ( WP그레네이드 설치: a_WP Granade Fire) : Level 1-5
function GetSkillResult1500008011(caster, kTargetArray, skillnum, result)
	return 0
end

-- Sniper (아트로핀 : a_Atropine) Level 1-10
function GetSkillResult150000901(caster, kTargetArray, skillnum, result)
	return 0
end

-- Sniper (융단폭격 : a_Carpet Bombing) Level 1-10
function GetSkillResult150001101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Sniper (융단폭격 발사: a_Carpet Bombing_Play) Level 1-10
function GetSkillResult1500011011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper (융단폭격 발사: a_Carpet Bombing_Play) Level 1-10
function GetSkillResult1500011012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper (융단폭격 발사: a_Carpet Bombing_Play) Level 1-10
function GetSkillResult1500011013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper (융단폭격 발사: a_Carpet Bombing_Play) Level 1-10
function GetSkillResult1500011014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper (융단폭격 발사: a_Carpet Bombing_Play) Level 1-10
function GetSkillResult1500011015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper (바주카포 지상: a_Homing Missile Ground) Level 1-5
function GetSkillResult150001401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper 플래시뱅
function GetSkillResult150000601(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "GetSkillResult150000001....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)

		local iDamageActionType = target:GetAbil(AT_DAMAGEACTION_TYPE);
		local iBitRet = BitAND(iDamageActionType, 255);
		if iBitRet == 0 then
			local iAddEffect = kSkillDef:GetAbil(AT_DAM_EFFECT_S + target:GetAbil(AT_UNIT_SIZE) - 1);
			if iAddEffect ~= 0 then
				aresult:AddEffect(iAddEffect);
			end
		end	

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end -- while
	return iIndex
end

-- Sniper 개틀링 러쉬
function GetSkillResult150001001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Sniper 스나이핑
function GetSkillResult150002101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--스트라이커 Skill
-----------------------------------------------------------------
-- Striker (셀프봄버 : a_SelfBomber)
function GetSkillResult2000602001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--댄서 Skill
-----------------------------------------------------------------
-- Dancer ( 메가 드릴 드라이브: a_MegaDrill) : Level 1-5
function GetSkillResult160000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 윈드밀: a_Wind Mill) : Level 1-10
function GetSkillResult160000201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 세븐: a_Seven) : Level 1-5
function GetSkillResult160000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 문 워크: a_Moon Walk) : Level 1
function GetSkillResult160000401(caster, kTargetArray, skillnum, result)
	return 0
end

-- Dancer ( 기합: a_Shout) : Level 1-5
function GetSkillResult160000501(caster, kTargetArray, skillnum, result)
	return 0
end

-- Dancer ( 나인틴 나이티: a_Nineteen Ninety) : Level 1-5
function GetSkillResult160000601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 코사크 댄스: a_Kozaks Dance) : Level 1-5
function GetSkillResult160000701(caster, kTargetArray, skillnum, result)
	return 0
end

-- Dancer ( 코사크 댄스: a_Kozaks Dance Fire) : Level 1-5
function GetSkillResult1600007011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 코사크 댄스: a_Kozaks Dance Fire) : Level 1-5
function GetSkillResult1600007012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 코사크 댄스: a_Kozaks Dance Fire) : Level 1-5
function GetSkillResult1600007013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 코사크 댄스: a_Kozaks Dance Fire) : Level 1-5
function GetSkillResult1600007014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer ( 코사크 댄스: a_Kozaks Dance Fire) : Level 1-5
function GetSkillResult1600007015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001201(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult160001301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160001901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult160002001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Dancer 스탭더 비트 a_step_the_beat
function GetSkillResult159999101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Dancer 헤드 스핀 a_head_spin
function GetSkillResult159999201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--매니악  Skill
-----------------------------------------------------------------
-- Maniac ( HellShout) 
function GetSkillResult2000710101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Maniac ( a_Marionette) 
function GetSkillResult2000720101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Maniac (a_Marionette_Finish) 
function GetSkillResult2000720201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------
--닌자 Skill
-----------------------------------------------------------------
-- Ninja (풍마수리검: a_Wind Suriken) : Lv 1-10
function GetSkillResult170000001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (그림자 도약: a_Shadow Leap) : Lv 1-5
function GetSkillResult170000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (미혼향: a_Sleep Smell) : Lv 1-10
function GetSkillResult170000301(caster, kTargetArray, skillnum, result)
	return 0
end

-- Ninja (압정 설치 : a_Caltrap Set) : Level 1-10
function GetSkillResult1700004011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-- Ninja (목둔술! 통나무 굴리기!: a_Rolling Log) : Lv 1-10
function GetSkillResult170000501(caster, kTargetArray, skillnum, result)
	return 0;
end

-- Ninja (목둔술! 통나무 굴리기! 발동: a_Rolling Log Play) : Lv 1-5
function GetSkillResult1700005011(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (목둔술! 통나무 굴리기! 발동: a_Rolling Log Play) : Lv 1-5
function GetSkillResult1700005012(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (목둔술! 통나무 굴리기! 발동: a_Rolling Log Play) : Lv 1-5
function GetSkillResult1700005013(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (목둔술! 통나무 굴리기! 발동: a_Rolling Log Play) : Lv 1-5
function GetSkillResult1700005014(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (목둔술! 통나무 굴리기! 발동: a_Rolling Log Play) : Lv 1-5
function GetSkillResult1700005015(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170000601(caster, kTargetArray, skillnum, result)
	return 0;
end

-- Ninja (환영 분신: a_Shadow Copy) : Lv 1-10
function GetSkillResult170000601(caster, kTargetArray, skillnum, result)
	return 0;
end

-- Ninja (격! 일섬!: a_Lightning Slash) : Lv 1-10
function GetSkillResult170000701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Ninja (질풍답보: a_windstep) : Lv 1-5
function GetSkillResult170003101(caster, kTargetArray, skillnum, result)
	return 0
end

-----------------------------------------------------------------
--쉐도우 Skill
-----------------------------------------------------------------
-- Shadow (프로텍트엣지: a_ProtectEdge)
function GetSkillResult2000802001(caster, kTargetArray, skillnum, result)
	return 0
end

-- Shadow (쉐도우블레이드: a_ShadowBlade)
function GetSkillResult2000820101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Shadow (쉐도우블레이드 소환체: a_ShadowBladeSA)
function GetSkillResult2000820201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Shadow (쉐도우블레이드 소환체 마무리: a_ShadowBladeSAFinish)
function GetSkillResult2000820301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Shadow (크로스 블레이드 시전 : a_crossblade)
function GetSkillResult2000810101(caster, kTargetArray, skillnum, result)
	return 0
end

-- Shadow (크로스 블레이드  플레이: a_crosssblade_play) LV 1-5
function GetSkillResult2000810201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult2000810202(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult2000810203(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult2000810204(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult2000810205(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-- 스페셜 스킬 ( 형 돈다: a_Bro_Spin) : Level 1
function GetSkillResult95000201(caster, kTargetArray, skillnum, result)
	return 0
end
-- 스페셜 스킬 ( 형 돈다 발사: a_Bro_Spin_Fire) : Level 1-5
function GetSkillResult95000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스페셜 스킬 ( 형 돈다 발사: a_Bro_Spin_Fire) : Level 1-5
function GetSkillResult95000302(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스페셜 스킬 ( 형 돈다 발사: a_Bro_Spin_Fire) : Level 1-5
function GetSkillResult95000303(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스페셜 스킬 ( 형 돈다 발사: a_Bro_Spin_Fire) : Level 1-5
function GetSkillResult95000304(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스페셜 스킬 ( 형 돈다 발사: a_Bro_Spin_Fire) : Level 1-5
function GetSkillResult95000305(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult170000801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103202001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103202401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103202501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103202601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103202701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult200000301(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult60446031(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 커플 (사랑의 힘으로! : a_Couple_Lover)
function GetSkillResult90000001(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult6004201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult6053100(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 상태이상 테스트 스킬

function GetSkillResult200001001(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001101(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001201(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001301(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001401(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001501(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001601(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001701(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001801(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200001901(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200002001(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200002101(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200002201(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200002301(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult200002401(caster, kTargetArray, skillnum, result)
	return 0;
end

-----------------------------------------------------------------
--각성기 Skill
-----------------------------------------------------------------

-- 데스스내치 - 땡겨오기
function GetSkillResult2000202001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result);
end

-- 데스스내치 - 피니시
function GetSkillResult2000203001(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 클로 피싱 - 땡겨오기
function GetSkillResult2000702001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result);
end
function GetSkillResult2000703001(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 와이어 액션
function GetSkillResult2000502001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result);
end
function GetSkillResult2000503001(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 타임 프리즈
function GetSkillResult2000402001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result)
end
-- 힐링 웨이브
function GetSkillResult2000302001(caster, kTargetArray, skillnum, result)
	return 0;
end

-- Nuclear
function GetSkillResult2000620101(caster, kTargetArray, skillnum, result)			           
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end


-----------------------------------------------------------------
--궁극기 Skill
-----------------------------------------------------------------
-- 드래곤 러쉬
function GetSkillResult2009110101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 드래곤 러쉬 파이날
function GetSkillResult2009110201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 데몰리션 봄버
function GetSkillResult2009210101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 데몰리션 봄버 파이날
function GetSkillResult2009210201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 라이트닝 스톰
function GetSkillResult2009310101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 라이트닝 스톰 파이날
function GetSkillResult2009310201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 리버스 그라비티 띄우기_1 (시전 스킬)
function GetSkillResult2009410101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result);
end

-- 리버스 그라비티 내리꽂기_1
function GetSkillResult2009410201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 리버스 그라비티 띄우기_2
function GetSkillResult2009410301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result);
end

-- 리버스 그라비티 내리꽂기_2
function GetSkillResult2009410401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 리버스 그라비티 띄우기_3
function GetSkillResult2009410501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result);
end

-- 리버스 그라비티 내리꽂기_3 (파이날)
function GetSkillResult2009410601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 환영분신술 - 소환
function GetSkillResult2009810101(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 환영분신술 - 소환수idle
function GetSkillResult2009810201(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 환영분신술 - 소환 닌자 idle
function GetSkillResult2009810301(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 환영분신술 - 소환 닌자 melee_01
function GetSkillResult2009810401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 환영분신술 - 소환 닌자 melee_finish
function GetSkillResult2009810501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 클럽마니아
function GetSkillResult2009710101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 클럽마니아 파이날
function GetSkillResult2009710201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 멀티 스트레이프 시전
function GetSkillResult2009610101(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 멀티 스트레이프 Fire
function GetSkillResult2009610201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 멀티 스트레이프 막타 광역
function GetSkillResult2009610301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 멀티 스트레이프 소환수 idle
function GetSkillResult2009610401(caster, kTargetArray, skillnum, result)
	return 0;
end

-- 그라운드 제로
function GetSkillResult2009510101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 그라운드 제로 막타 광역
function GetSkillResult2009510201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result);
end

-- 그라운드 제로 소환수 idle
function GetSkillResult2009510301(caster, kTargetArray, skillnum, result)
	return 0;
end
-----------------------------------------------------------------
--Pet Skill
-----------------------------------------------------------------
function GetSkillResult80000401(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult80000501(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult80000601(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult80000701(caster, kTargetArray, skillnum, result)
	return 0;
end

function GetSkillResult80000801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult80000901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 낙뢰
function GetSkillResult80001301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 물어뜯기
function GetSkillResult80001501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 푸얀 포이즌 클라우드
function GetSkillResult80001601(caster, kTargetArray, skillnum, result)
	return 0
end

-- 핑마오 포이즌 클라우드
function GetSkillResult80001701(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레오 포이즌 클라우드
function GetSkillResult80001801(caster, kTargetArray, skillnum, result)
	return 0
end

-- 푸얀 구르기
function GetSkillResult80001901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 핑마오 구르기
function GetSkillResult80002001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레오 구르기
function GetSkillResult80002101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 푸얀 다크 클라우드
function GetSkillResult80002201(caster, kTargetArray, skillnum, result)
	return 0
end

-- 핑마오 다크 클라우드
function GetSkillResult80002301(caster, kTargetArray, skillnum, result)
	return 0
end

-- 레오 다크 클라우드
function GetSkillResult80002401(caster, kTargetArray, skillnum, result)
	return 0
end

-- 푸얀 몸통 부딪히기
function GetSkillResult80002501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 핑마오 몸통 부딪히기
function GetSkillResult80002601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레오 몸통 부딪히기
function GetSkillResult80002701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 푸얀 블리자드
function GetSkillResult80002801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 핑마오 블리자드
function GetSkillResult80002901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레오 블리자드
function GetSkillResult80003001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 푸얀 메테오
function GetSkillResult80003101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 핑마오 메테오
function GetSkillResult80003201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레오 메테오
function GetSkillResult80003301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 푸얀 포이즌 클라우드 공격
function GetSkillResult80003501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 핑마오 포이즌 클라우드 공격
function GetSkillResult80003601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레오 포이즌 클라우드 공격
function GetSkillResult80003701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 푸얀 다크 클라우드 공격
function GetSkillResult80003901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 핑마오 클라우드 공격
function GetSkillResult80004001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--펫 허리케인
function GetSkillResult80004601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--펫 허리케인2
function GetSkillResult80005001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--펫 파이어 허리케인
function GetSkillResult80005301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--펫 푸드 낙하(펫 버프)
function GetSkillResult80004701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--펫 프로즌 클라우드
function GetSkillResult80004801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--펫 파이어 클라우드
function GetSkillResult80005101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 레오 클라우드 공격
function GetSkillResult80003801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 해바라기 펫 아이템 생산 스킬
function GetSkillResult80007001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 펫 탑승 공격
function GetSkillResult80004301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 펫 탑승 공격
function GetSkillResult80004401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 펫 버프 스킬
function GetSkillResult80005401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 펫 물어뜯기
function GetSkillResult80005501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 펫 레이저
function GetSkillResult80006501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 펫 수류탄
function GetSkillResult80006601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


function GetSkillResult2000102001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- Destroyer (데스 스팅거)
function GetSkillResult2000210101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- ArchMage (라이트닝 마그넷)
function GetSkillResult2000310201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult105300501(skillnum, caster, kTargetArray, result)
end

-- Destroyer (데몰리션 차지)
function GetSkillResult2000220101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult2000220201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult2000220301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult2000610101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103100401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult103101501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

----------------------------------------------------------------
--친교 버프 Skill
-----------------------------------------------------------------
function GetSkillResult180000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult180000201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult180000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult180000401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult300100901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
------------------------------------------------------------------------
--Item Skill
------------------------------------------------------------------------
--리바이어스 소환
function GetSkillResult90010101(caster, kTargetArray, skillnum, result)
	return 0;
end
--리바이어스 공격
function GetSkillResult91010101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------
--격투가
-----------------------------------------------------------
--기본공격
function GetSkillResult400000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400000201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400000401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 5콤보(z)
function GetSkillResult400000901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--mc1타 후 Z(Sub)
function GetSkillResult400000701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--sc 섬머솔트 Z(Sub)
function GetSkillResult400000801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--4콤보(봉황각)
function GetSkillResult400000501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 차지 공격(Z)
function GetSkillResult400000601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 4콤후 바닥 쓸기
function GetSkillResult400101101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 다운 공격
function GetSkillResult400001401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400001501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400001601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 공중 공격
function GetSkillResult400001101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400001201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult400001301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--대시 공격
function GetSkillResult400001001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 1차 스킬
-- 일도 양단
function GetSkillResult400100101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 토네이도 스핀
function GetSkillResult400100201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스크류 드랍킥
function GetSkillResult400100401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 래피드 스탠스
function GetSkillResult400100501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 파워 피스트(Sub)
function GetSkillResult400100601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스핀킥(Sub)
function GetSkillResult400100701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 드래곤 킥(Sub)
function GetSkillResult400100801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 로켓 블로우(Sub)
function GetSkillResult400100901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 백열각(Sub)
function GetSkillResult400101001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 무한 돌려차기
function GetSkillResult400200201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 하이퍼 니킥
function GetSkillResult400200301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 드래곤 어퍼
function GetSkillResult400200401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 지옥의 링 캐스트
function GetSkillResult400200501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 지옥의 링 발사
function GetSkillResult400200601(caster, kTargetArray, skillnum, result)
	return 0
end
-- 파워 위빙
function GetSkillResult400200701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스탬퍼 (Sub)
function GetSkillResult400200801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스핀 블레이드(Sub)
function GetSkillResult400200901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 스위시킥(Sub)
function GetSkillResult400201001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 백열진 각(Sub)
function GetSkillResult400201101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 라이징 드롭킥(Sub)
function GetSkillResult400201201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 피니쉬 콤보_5콤후_차지돌려차기(실제사용)
function GetSkillResult400201601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 절대 방어(FU)
function GetSkillResult400201801(caster, kTargetArray, skillnum, result)
	return 0
end
-- 임팩트 블로우(FU)
function GetSkillResult400201901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 투기 발산(FU)
function GetSkillResult400202001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--무한 난투 대시(FU)
function GetSkillResult400202101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--무한 난투 연타(FU)
function GetSkillResult400202201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 기사회생
function GetSkillResult400300101(caster, kTargetArray, skillnum, result)
	return 0
end
--a_twin_bunker_buster
function GetSkillResult400300301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_lion_roar
function GetSkillResult400300401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_tornado_upper
function GetSkillResult400300501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_lightning_flash
function GetSkillResult400300601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_spin_boom_s
function GetSkillResult400300701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_spirit_spear_s
function GetSkillResult400300801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_rolling_grabble_s
function GetSkillResult400300901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_sliding_batter_s
function GetSkillResult400301001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_smash_kick_s
function GetSkillResult400301101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_boost_fu
function GetSkillResult400301201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_finish_move_fu
function GetSkillResult400301301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_finish_impact_fu
function GetSkillResult400301401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--a_twin_ghost_fighter_fu
function GetSkillResult400301501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 위빙
function GetSkillResult400100301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 파워위빙
function GetSkillResult400200701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--트레이닝 타임
function GetSkillResult400400101 (caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--무한난투 지뢰진sc
function GetSkillResult400400301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--무한난투 기단파 mc(막타)
function GetSkillResult400400401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 연계강타 mc 
function GetSkillResult400400501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 연계강타 sc
function GetSkillResult400400601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--크로스 피스트 5연타 mc
function GetSkillResult400401001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--크로스 피스트 막타  mc
function GetSkillResult400401101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------
--소환사
-----------------------------------------------------------

-- 소환사 기본 공격
function GetSkillResult300000101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 소환사 기본 공중 공격
function GetSkillResult300000301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 소환사 기본 다운공격 1타
function GetSkillResult300000501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 소환사 기본 다운공격 2타
function GetSkillResult300000601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 소환사 기본 다운공격 3타
function GetSkillResult300000701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 소환사 기본 차지공격
function GetSkillResult300000801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 소환사 기본 대쉬 공격
function GetSkillResult300001001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 아이비 쏜즈
function GetSkillResult300100101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--그라운드 피스톤
function GetSkillResult300100201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--아이비 트랩 소환
function GetSkillResult300100301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 미스트 소환
function GetSkillResult300101101(caster, kTargetArray, skillnum, result)
	return 0
end

-- 에어 스프레쉬
function GetSkillResult300100601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 리게인
function GetSkillResult300101001(caster, kTargetArray, skillnum, result)
	return 0
end
-- 리콜
function GetSkillResult300200201(caster, kTargetArray, skillnum, result)
	return 0
end

--아이비 러쉬 소환
function GetSkillResult300100701(caster, kTargetArray, skillnum, result)
	return 0
end

--아이비 러쉬 소환체 공격
function GetSkillResult300100801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--에어스매시 소환체 공격
function GetSkillResult300201201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 스톤가드 (버프)
function GetSkillResult300201501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 인페르노
function GetSkillResult300201401(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 볼케이노
function GetSkillResult300201601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-- 바위감옥
function GetSkillResult300202001(caster, kTargetArray, skillnum, result)
	return 0
end

--메테오 레인 캐스트
function GetSkillResult300201801(caster, kTargetArray, skillnum, result)
	return 0
end
--메테오 레인 공격
function GetSkillResult300201901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--어스퀘이크 캐스트
function GetSkillResult300202101(caster, kTargetArray, skillnum, result)
	return 0
end
-- 어스퀘이크 발동
function GetSkillResult300202201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 포이즌 가드
function GetSkillResult300301401(caster, kTargetArray, skillnum, result)
	return 0
end
-- 마나번
function GetSkillResult300301601(caster, kTargetArray, skillnum, result)
	return 0
end
-- 메가 인페르노
function GetSkillResult300301801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 임펙트 프레셔
function GetSkillResult300301901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 메가 아이비 러쉬 캐스트
function GetSkillResult300302001(caster, kTargetArray, skillnum, result)
	return 0
end
-- 메가 아이비 러쉬
function GetSkillResult300302101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 롤링 스톤 캐스트
function GetSkillResult300302201(caster, kTargetArray, skillnum, result)
	return 0
end
-- 롤링 스톤 공격
function GetSkillResult300302301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 네이쳐 블레싱
function GetSkillResult300300301(caster, kTargetArray, skillnum, result)
	return 0
end
-- 생명의 외침
function GetSkillResult300200401(caster, kTargetArray, skillnum, result)
	return 0
end
-- 보호
function GetSkillResult300300401(caster, kTargetArray, skillnum, result)
	return 0
end
-- 임펙트 언서몬
function GetSkillResult300301101(caster, kTargetArray, skillnum, result)
	return 0
end
-- 스마트샷 캐스팅
function GetSkillResult300301301(caster, kTargetArray, skillnum, result)
	return 0
end
-- 스마트샷 발사
function GetSkillResult300304001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 프로즌 체인
function GetSkillResult300400101(caster, kTargetArray, skillnum, result)
	return 0;
end
-- 템프테이션
function GetSkillResult300400301(caster, kTargetArray, skillnum, result)
	return 0
end
--엘리멘탈 스톰 시전 준비
function GetSkillResult300400501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 바위 소환
function GetSkillResult300400601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 바위 소환체 공격
function GetSkillResult300401001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 얼음 소환
function GetSkillResult300400701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 얼음 소환체 공격
function GetSkillResult300401101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 전격 소환
function GetSkillResult300400801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 전격 소환체 공격
function GetSkillResult300401201(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 화염 소환
function GetSkillResult300400901(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--엘리멘탈 스톰 화염 소환체 공격
function GetSkillResult300401301(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--드레곤 레이지 발동 8연타
function GetSkillResult300402001(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--드레곤 레이지 막타
function GetSkillResult300402101(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

-----------------------------------------------------------------------------------------------------------------------
--펫 스킬 --
-- 할로윈 펫 스킬 (타우린 서포트)
function GetSkillResult80004501(caster, kTargetArray, skillnum, result)
	return 0
end


-----------------------------------------------------------------------------------------------------------------------
--연계콤보--
-- 한손검 1-1타
function GetSkillResult9002110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 한손검 1-2타
function GetSkillResult9002210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 한손검 1-3타
function GetSkillResult9002310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 한손검 1-4타
function GetSkillResult9002410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 한손검 1-5타
function GetSkillResult9002510(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002350(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002360(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002540(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002640(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002160(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 한손검 대쉬
function GetSkillResult9002010(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002710(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002720(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002730(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002740(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002810(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002830(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002840(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002910(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9002940(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 양손검
function GetSkillResult9003110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003350(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003430(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003530(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003351(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003352(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003353(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003354(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003355(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
-- 양손검 대쉬
function GetSkillResult9003010(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003710(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003720(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003730(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003740(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003810(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003830(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003840(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9003910(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--활
function GetSkillResult9022110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022540(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022430(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022530(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022320(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022420(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9022440(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022341(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022342(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022343(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022344(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9022345(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--석궁

function GetSkillResult9024110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9024210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9024310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9024410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9024510(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9024440(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9025110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9025210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9025310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9025410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9025510(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9025440(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9023110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023540(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023430(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023530(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023320(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023420(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023520(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023620(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9023440(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023341(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023342(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023343(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023344(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9023345(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--클로
function GetSkillResult9032110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032510(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032460(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032350(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032360(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032260(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032270(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032160(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032170(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9032650(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
--카타르
function GetSkillResult9033110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033510(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033460(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033350(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033360(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033260(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033270(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResult103201201(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033160(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033170(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9033650(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

--파워글로브
function GetSkillResult9512110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512130(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512135(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512136(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512140(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512150(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512155(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512156(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512230(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512240(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512250(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512255(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512256(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512315(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512316(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512350(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512355(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512356(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512416(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512460(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512465(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512510(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512515(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9512516(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end



--스태프
function GetSkillResult9012110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012220(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012320(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012420(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012430(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012440(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012260(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012360(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9012370(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9014110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9015110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


-- 창
function GetSkillResult9013110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013220(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013320(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013420(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013430(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013440(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013450(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013260(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013360(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end
function GetSkillResult9013370(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end



-- 건스테프

function GetSkillResult9502110(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502210(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502310(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502410(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502120(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502220(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502320(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502420(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult9502430(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult9502340(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502440(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult9502450(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult9502260(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502360(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502370(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502380(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502501(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502601(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end

function GetSkillResult9502701(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end


function GetSkillResult9502801(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)
end




-- PVP 러브모드, 긴급회피 스킬
function GetSkillResult99803700(caster, kTargetArray, skillnum, result)
	return CS_GetSkillResultDefault(skillnum, caster, kTargetArray, result)

end

-- 악세사리 스킬

function GetSkillResult531001(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531002(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531003(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531004(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531005(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531006(caster, kTargetArray, skillnum, result)
	return 0
end







function GetSkillResult531001(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531002(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531003(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531004(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531005(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531006(caster, kTargetArray, skillnum, result)
	return 0
end







function GetSkillResult531101(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531102(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531103(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531104(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531105(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531106(caster, kTargetArray, skillnum, result)
	return 0
end





function GetSkillResult531201(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531202(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531203(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531204(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531205(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531206(caster, kTargetArray, skillnum, result)
	return 0
end





function GetSkillResult531301(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531302(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531303(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531304(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531305(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531306(caster, kTargetArray, skillnum, result)
	return 0
end








function GetSkillResult531401(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531402(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531403(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531404(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531405(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531406(caster, kTargetArray, skillnum, result)
	return 0
end





function GetSkillResult531501(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531502(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531503(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531504(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531505(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531506(caster, kTargetArray, skillnum, result)
	return 0
end







function GetSkillResult531601(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531602(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531603(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531604(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531605(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531606(caster, kTargetArray, skillnum, result)
	return 0
end






function GetSkillResult531701(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531702(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531703(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531704(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531705(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531706(caster, kTargetArray, skillnum, result)
	return 0
end







function GetSkillResult531801(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531802(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531803(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531804(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531805(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531806(caster, kTargetArray, skillnum, result)
	return 0
end




function GetSkillResult531901(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531902(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531903(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531904(caster, kTargetArray, skillnum, result)
	return 0
end


function GetSkillResult531905(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult531906(caster, kTargetArray, skillnum, result)
	return 0
end






function GetSkillResult530901(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult530902(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult530903(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult530904(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult530905(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult530906(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult532501(caster, kTargetArray, skillnum, result)
	return 0
end

-- 그리스 캐쉬 아이템  스킬
function GetSkillResult532301(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult532401(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult532501(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult532601(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult532701(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult532801(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533101(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533201(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533301(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533401(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533501(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533601(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533701(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533801(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533901(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534001(caster, kTargetArray, skillnum, result)
	return 0
end

-- 참치 캐쉬 아이템  스킬
function GetSkillResult533101(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533201(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533301(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533401(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533501(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533601(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533701(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533801(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult533901(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534001(caster, kTargetArray, skillnum, result)
	return 0
end

-- 발바닥 캐쉬 아이템  스킬
function GetSkillResult534101(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534201(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534301(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534401(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534501(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534601(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534701(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534801(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult534901(caster, kTargetArray, skillnum, result)
	return 0
end

function GetSkillResult535001(caster, kTargetArray, skillnum, result)
	return 0
end