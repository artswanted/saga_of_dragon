
-- 마법사 (정신일도하사불성 : a_Concentration) : Level 1-10
function Skill_Begin102000801(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000801(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult102000801(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
		
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire102000801....SkillDef is NIl" ..skillnum)
		return -1
	end
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return 1
end

function Skill_Fail102000801(caster, kTargetArray, skillnum, result, arg)
	return false
end
