
-- War Mage (클라우드 킬 : a_Cloud Kill ) : Level 1
function Skill_Begin130000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire130000201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult130000201(caster, kTargetArray, skillnum, result)
	return 1
end

function Skill_Fail130000201(caster, kTargetArray, skillnum, result, arg)
	return false
end
