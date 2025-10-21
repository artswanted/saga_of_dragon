
-- Archer (멋진 언니 : a_Beautiful Girl) : Level 1-5
function Skill_Begin103000301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire103000301(caster, kTargetArray, skillnum, result, arg)	
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(8, "Skill_Fire103000301...Ground Is Nil")
		return -1
	end
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire103000301....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	local kGirl = kGround:CreateEntity(caster, kSkillDef:GetAbil(AT_CLASS), kSkillDef:GetAbil(AT_LEVEL), "BeautifulGirl", iDistance, true)
	if kGirl:IsNil() == false then
		kGirl:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		InfoLog(8, "Skill_Fire103000301...BeautifulGirl Already Called")
	end
	return 1
end

function Skill_Fail103000301(caster, kTargetArray, skillnum, result, arg)
	return false
end

function Skill_CanReserve103000301(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end