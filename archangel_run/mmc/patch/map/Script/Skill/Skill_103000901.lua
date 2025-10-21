-- 궁수 (공성병기화 : Siege Cannon Fire) : Level 1
function Skill_Begin103000901(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire103000901(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult103000901(caster, kTargetArray, skillnum, result)	
end

function Skill_Fail103000901(caster, kTargetArray, skillnum, result, arg)
	return false
end