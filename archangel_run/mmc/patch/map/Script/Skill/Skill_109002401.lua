
-- 워메이지 ( 백 드래프트 : a_Back Draft) : Level 1-5
function Skill_Begin109002401(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109002401(caster, kTargetArray, skillnum, result, arg)
	-- 클라이언트에서 모든것을 처리한다. 서버는 아무것도 할 것이 없다.
	return 1
end

function Skill_Fail109002401(caster, kTargetArray, skillnum, result, arg)
	return false
end
