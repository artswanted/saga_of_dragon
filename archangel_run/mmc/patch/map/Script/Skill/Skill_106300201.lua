
-- 기사 (액셀레이션 : a_Acceleration) : Level 1-10
function Skill_Begin106300201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106300201(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult106300201(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106300201....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return 1
end

function Skill_Fail106300201(caster, kTargetArray, skillnum, result, arg)
	return false
end
