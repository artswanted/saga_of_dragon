
-- 메이지 (감염 : a_Disease) : Level 1-10
function Skill_Begin107000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire107000701(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult107000701(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire107000701....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			if RAND(ABILITY_RATE_VALUE) < kSkillDef:GetAbil(AT_PERCENTAGE) then
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
				DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg, caster)
			else
				aresult:SetMissed(true)
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail107000701(caster, kTargetArray, skillnum, result, arg)
	return false
end
