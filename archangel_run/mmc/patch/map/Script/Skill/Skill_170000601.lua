
-- 닌자 (환영분신 : a_Shadow Copy ) : Level 1-5
function Skill_Begin170000601(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire170000601(caster, kTargetArray, skillnum, result, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire170000601....SkillDef is NIl" ..skillnum)
		return -1
	end

	InfoLog(8, "Skill_Fire170000601....AddEffect : " ..kSkillDef:GetEffectNo())
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return 1
end

function Skill_Fail170000601(caster, kTargetArray, skillnum, result, arg)
	return false
end
