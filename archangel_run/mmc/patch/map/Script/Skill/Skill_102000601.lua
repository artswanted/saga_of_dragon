-- Magician (너스 콜 : Nurse Call) : Level 1-10
function Skill_Begin102000601(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000601(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult103000301(caster, kTargetArray, skillnum, result)
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(8, "Skill_Fire102000601...Ground Is Nil")
		return -1
	end
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire102000601....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	-- Nurse Call ----------------------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	local kNurse = kGround:CreateEntity(caster, kSkillDef:GetAbil(AT_CLASS), kSkillDef:GetAbil(AT_LEVEL), "Nurse", iDistance, true)
	if kNurse:IsNil() == false then
		kNurse:SetAbil( AT_CANNOT_DAMAGE, 1 )	-- 공격을 당하지 않는다~
		kNurse:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		InfoLog(8, "Skill_Fire102000601...Nurse Already Called")
	end
	return 1
end

function Skill_Fail102000601(caster, kTargetArray, skillnum, result, arg)
	return false
end
