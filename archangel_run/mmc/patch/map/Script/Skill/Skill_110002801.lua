
-- 트랩퍼 (엠피제로 트랩 : a_MP-Zero Trap) : Level 1-5
function Skill_Begin110002801(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110002801(caster, kTargetArray, skillnum, result, arg)
	-- 클라이언트에서 모든것을 처리한다. 서버는 아무것도 할 것이 없다.
	return 1
end

function Skill_Fail110002801(caster, kTargetArray, skillnum, result, arg)
	return false
end
