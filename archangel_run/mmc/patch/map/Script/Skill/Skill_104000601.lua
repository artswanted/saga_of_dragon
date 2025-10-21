
-- 공통도둑 (버로우 : Burrow) : Level 1-5 : Toggle
function Skill_Begin104000601(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle104000601(caster, skillnum, bToggleOn, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle104000601....SkillDef is NIl" ..skillnum)
		return -1
	end
	-- Toggle Skill
	if bToggleOn==true then
		arg:SetInt(ACTARG_TOGGLESKILL, skillnum)
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		caster:DeleteEffect(kSkillDef:GetEffectNo())		
	end
	return 1
end

--[[
function Skill_Fire104000601(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult104000601(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire104000601....SkillDef is NIl" ..skillnum)
		return -1
	end

	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(kSkillDef:GetEffectNo(), true)
  if kEffect:IsNil()== true then
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		return 0
	else
		caster:DeleteEffect(kSkillDef:GetEffectNo())	-- Effect Delete
	end

	return 1
end
]]

function Skill_Fail104000601(caster, kTargetArray, skillnum, result, arg)
	return false
end
