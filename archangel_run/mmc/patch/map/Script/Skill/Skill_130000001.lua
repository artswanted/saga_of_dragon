
-- War Mage (스노우 블루스 : a_Snow Blues ) : Level 1
function Skill_Begin130000001(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire130000001(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--GetSkillResult130000001(caster, kTargetArray, skillnum, result)
	return 1
end

function Skill_Fail130000001(caster, kTargetArray, skillnum, result, arg)
	return false
end
