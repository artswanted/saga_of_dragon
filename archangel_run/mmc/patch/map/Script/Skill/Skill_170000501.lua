
-- 닌자 (목둔술! 통나무 굴리기 : a_Rolling Log ) : Level 1
function Skill_Begin170000501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire170000501(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--GetSkillResult170000501(caster, kTargetArray, skillnum, result)
	return 1
end

function Skill_Fail170000501(caster, kTargetArray, skillnum, result, arg)
	return false
end
