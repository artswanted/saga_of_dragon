-- 궁수 (공성병기화 : a_Siege Cannon) : Level 1
function Skill_Begin103000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire103000701(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult103000701(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire103000701....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	if false == caster:DeleteEffect(kSkillDef:GetEffectNo()) then
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	end
	
end

function Skill_Fail103000701(caster, kTargetArray, skillnum, result, arg)
	return false
end
