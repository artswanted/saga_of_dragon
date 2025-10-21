
-- 기사 (조인트 브레이크 : a_Joint Break) : Level 1-5
function Skill_Begin105300501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire105300501(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult105300501(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire105300501....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iPercentage = kSkillDef:GetAbil(AT_PERCENTAGE)
	InfoLog(9, "Skill_Fire105300501 Percentage:" .. iPercentage)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			if RAND(ABILITY_RATE_VALUE) < iPercentage then
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
			end
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail105300501(caster, kTargetArray, skillnum, result, arg)
	return false
end
