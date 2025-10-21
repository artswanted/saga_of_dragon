
-- Lavalon(BOSS) lv_lavalon_breath_c,l,r
function Skill_Begin6000904(caster, skillnum, iStatus,arg,kTailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6000904(caster, kTargetArray, skillnum, result, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire6000904....SkillDef is Nil " ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
end

function Skill_CanFire6000904(caster, kTargetArray, skillnum, actarg)
	return true
end

function Skill_CanReserve6000904(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

function Skill_Fail6000904(caster, kTargetArray, skillnum, result, arg)
	return false
end
