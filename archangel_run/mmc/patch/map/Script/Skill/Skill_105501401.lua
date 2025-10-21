
-- 성기사/검투사 ( 장비 수리: a_Use Repair) : Level 1
function Skill_Begin105501401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire105501401(caster, kTargetArray, skillnum, result, arg)
	return 1;
end

function Skill_Fail105501401(caster, kTargetArray, skillnum, result, arg)
	return false
end
