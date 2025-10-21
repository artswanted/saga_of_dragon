
-- 암살자 (마인드 리딩: a_Mind Reading) : Lv 1-10
function Skill_Begin104301201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104301201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult104301201(caster, kTargetArray, skillnum, result)
	InfoLog(9, "Skill_Fire104301201 --")
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		InfoLog(9, "Skill_Fire104301201 10")
		return 1
	end
	InfoLog(9, "Skill_Fire104301201 20")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire104301201....SkillDef is NIl" ..skillnum)
		return -1
	end

	InfoLog(9, "Skill_Fire104301201 30")
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return iIndex
end

function Skill_Fail104301201(caster, kTargetArray, skillnum, result, arg)
	return false
end
