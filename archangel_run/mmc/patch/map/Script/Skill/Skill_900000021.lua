
-- 보쓰 
function Skill_Begin900000021(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire900000021(caster, kTargetArray, skillnum, result, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire102000301....SkillDef is Nil" ..skillnum)
		return -1
	end
	
	--InfoLog(9, "Skill_Fire900000021 1.0")
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	kUnitArray = NewUNIT_PTR_ARRAY()
	local iDetectRange = 60
	CS_GetSkillResultDefault(skillnum, caster, kUnitArray, result)
	DeleteUNIT_PTR_ARRAY(kUnitArray)
end

function Skill_Fail900000021(caster, kTargetArray, skillnum, result, arg)
	return false
end
