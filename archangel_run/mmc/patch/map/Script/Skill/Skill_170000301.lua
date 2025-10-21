
-- 닌자 ( 미혼향: a_Sleep Smell) : Level 1-10
function Skill_Begin170000301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire170000301(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult170000301(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire170000301....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			if iRandValue < iEffectPer then
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster);
			end			
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail170000301(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
