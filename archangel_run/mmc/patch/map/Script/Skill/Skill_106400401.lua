
-- Paladin (기원의 오라 : a_Prayer) : Level 1-10
function Skill_Begin106400401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle106400401(caster, skillnum, bToggleOn, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle106400401....SkillDef is NIl" ..skillnum)
		return -1
	end
	-- Toggle Skill
	if bToggleOn then
		arg:SetInt(ACTARG_TOGGLESKILL, skillnum)
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		caster:DeleteEffect(kSkillDef:GetEffectNo())		
	end
	return 1
end

--[[
function Skill_Fire106400401(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult106400401(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106400401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return iIndex
end
]]

function Skill_Fail106400401(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
