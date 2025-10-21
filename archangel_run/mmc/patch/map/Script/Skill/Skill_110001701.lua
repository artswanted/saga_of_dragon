
-- 레인저 (카모플라쥬 : a_Camouflage) : Level 1-5 (Toggle)
function Skill_Begin110001701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle110001701(caster, skillnum, bToggleOn, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle110001701....SkillDef is NIl" ..skillnum)
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
function Skill_Fire110001701(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult110001701(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire110001701....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffect = caster:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == true then
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		caster:DeleteEffect(iEffectNo)
	end
	
	return 1
end
]]

function Skill_Fail110001701(caster, kTargetArray, skillnum, result, arg)
	return false
end
