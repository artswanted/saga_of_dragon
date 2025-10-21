
-- 궁수 공통 (EMP : EMP) : Level 1-5
function Skill_Begin103000801(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire103000801(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult103000801(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire103000801....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iMPRate = kSkillDef:GetAbil(AT_MP_BURN)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		local iMP = target:GetAbil(AT_MP)
		iMP = iMP - iMP * iMPRate / ABILITY_RATE_VALUE
		target:SetAbil(AT_MP, iMP, true, false)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail103000801(caster, kTargetArray, skillnum, result, arg)
	return false
end
