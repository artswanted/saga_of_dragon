
-- 광대 (더블 크래시: a_Double Crash) : Lv 1-5
function Skill_Begin104300701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104300701(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult104300501(caster, kTargetArray, skillnum, result)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire110001701....SkillDef is NIl" ..skillnum)
		return -1
	end
		
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		local iPer = kSkillDef:GetAbil(AT_PERCENTAGE)
		InfoLog(9, "Skill_Fire104300701 iPer:" .. iPer)
		if RAND(ABILITY_RATE_VALUE) <= iPer then
			--target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
			target:AddEffect(kSkillDef:GetAbil(AT_EFFECTNUM1), 0, arg, caster)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail104300701(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
