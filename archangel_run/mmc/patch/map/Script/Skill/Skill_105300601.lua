
-- 기사 (리벤지 : a_Revenge) : Level 1-5 : Toggle
function Skill_Begin105300601(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle105300601(caster, skillnum, bToggleOn, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle105300601....SkillDef is NIl" ..skillnum)
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
function Skill_Fire105300601(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult105300601(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire105300601....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(skillnum, true)
	if kEffect:IsNil() == true then
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		caster:DeleteEffect(kSkillDef:GetEffectNo())
	end	
	return iIndex
end
]]

function Skill_Fail105300601(caster, kTargetArray, skillnum, result, arg)
	return false
end
