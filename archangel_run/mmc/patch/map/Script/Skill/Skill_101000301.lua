
-- Fighter (단단한 머리 : a_Increased Defence) : Level 1-10
function Skill_Begin101000301(caster, skillnum, iStatus,arg)
	InfoLog(8, "Skill_Begin101000301")
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire101000301(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult101000301(caster, kTargetArray, skillnum, result)
	InfoLog(8, "Skill_Fire101000301")
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
		
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire101000301....SkillDef is NIl" ..skillnum)
		return -1
	end

	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
InfoLog(8, "Skill_Fire101000301 End")
	return 1
end

function Skill_Fail101000301(caster, kTargetArray, skillnum, result, arg)
	InfoLog(8,"Skill_Fair101000301")
	return false
end

function Skill_CanReserve101000301(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end
