
-- 사냥꾼 (크럭스 샷 : a_Crux Shot) : Level 1-10
function Skill_Begin110000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire11000701(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult11000701(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire11000701....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:SetAbil(AT_1ST_ATTACK_ADDED_RATE, kSkillDef:GetAbil(AT_R_PHY_ATTACK_MAX))
	return 1
end

function Skill_Fail11000701(caster, kTargetArray, skillnum, result, arg)
	return false
end
