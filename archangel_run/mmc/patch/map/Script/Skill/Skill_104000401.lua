
-- Thief (로케트펀치! : Rocket Punch) : Level 1-10
function Skill_Begin104000401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104000401(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult104000401(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire104000401....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), false)
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			if iRandValue < iEffectPer and aresult:GetInvalid() == false then
				target:AddEffect(kSkillDef:GetAbil(AT_EFFECTNUM1), 0, arg, caster)
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail104000401(caster, kTargetArray, skillnum, result, arg)
	return false
end
