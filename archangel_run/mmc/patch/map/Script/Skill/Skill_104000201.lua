
-- Thief (비상탈출 : a_EXIT) : Level 1
function Skill_Begin104000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle104000201(caster, skillnum, bToggleOn, arg)
	--Skill_Toggle104000201(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle104000201....SkillDef is NIl" ..skillnum)
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
function Skill_Fire104000201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult104000201(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire104000201....SkillDef is NIl" ..skillnum)
		return -1
	end
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return 1
end
]]

function Skill_Fail104000201(caster, kTargetArray, skillnum, result, arg)
	return false
end
