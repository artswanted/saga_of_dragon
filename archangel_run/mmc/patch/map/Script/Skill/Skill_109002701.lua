
-- 위자드/워메이지 ( 장비 인챈트: a_Use Enchant) : Level 1
function Skill_Begin109002701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109002701(caster, kTargetArray, skillnum, result, arg)
	return 1;
end

function Skill_Fail109002701(caster, kTargetArray, skillnum, result, arg)
	return false
end
