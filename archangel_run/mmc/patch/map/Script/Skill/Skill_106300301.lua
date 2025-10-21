
-- 투사 (섬멸 : a_Annihilation) : Level 1-10
function Skill_Begin106300301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106300301(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--local bReturn = GetSkillResult106300301(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106300301....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:SetAbil(AT_1ST_ATTACK_ADDED_RATE, kSkillDef:GetAbil(AT_R_PHY_ATTACK), true, true)
	return 1
end

function Skill_Fail106300301(caster, kTargetArray, skillnum, result, arg)
	return false
end
