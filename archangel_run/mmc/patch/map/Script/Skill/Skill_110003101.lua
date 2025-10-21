
-- 트랩퍼/스나이퍼 ( 요리 하기: a_Use Cook) : Level 1
function Skill_Begin110003101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110003101(caster, kTargetArray, skillnum, result, arg)
	return 1;
end

function Skill_Fail110003101(caster, kTargetArray, skillnum, result, arg)
	return false
end
