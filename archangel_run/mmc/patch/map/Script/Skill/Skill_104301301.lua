
-- 암살자 (섀도우 워크: a_Shadow Walk) : Lv 1-10 (Toggle)
function Skill_Begin104301301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle104301301(caster, skillnum, bToggleOn, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle104301301....SkillDef is NIl" ..skillnum)
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
function Skill_Fire104301301(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult104301301(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire104301301....SkillDef is NIl" ..skillnum)
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

function Skill_Fail104301301(caster, kTargetArray, skillnum, result, arg)
	return false
end
