
-- 사냥꾼 (환각 : a_Displacement) : Level 1-10
function Skill_Begin110000901(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110000901(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--GetSkillResult110000901(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire110000901....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return 1
end

function Skill_Fail110000901(caster, kTargetArray, skillnum, result, arg)
	return false
end
