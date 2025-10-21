
-- Gladiator ( 웨폰 브레이크: a_Weapon Break) : Level 1-10
function Skill_Begin106500901(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106500901(caster, kTargetArray, skillnum, result, arg)
	return 1
end

function Skill_Fail106500901(caster, kTargetArray, skillnum, result, arg)
	return false
end
