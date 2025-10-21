
-- 기사 (패링 : a_Parring ) : Level 1-5
function Skill_Begin105300201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire105300201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult105300201(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire105300201....SkillDef is NIl" ..skillnum)
		return -1
	end

	local kEffect = caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	if kEffect:IsNil() == false then
		kEffect:SetActArgInt(ACTARG_WEAPONLIMIT, kSkillDef:GetAbil(AT_WEAPON_LIMIT))
	end

	return 1
end

function Skill_Fail105300201(caster, kTargetArray, skillnum, result, arg)
	return false
end
