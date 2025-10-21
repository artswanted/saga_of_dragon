
-- War Mage (스노우 블루스 : a_Snow Blues ) : Level 1
function Skill_Begin109002301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109002301(caster, kTargetArray, skillnum, result, arg)
	return 1
end

function Skill_Fail109002301(caster, kTargetArray, skillnum, result, arg)
	return false
end
