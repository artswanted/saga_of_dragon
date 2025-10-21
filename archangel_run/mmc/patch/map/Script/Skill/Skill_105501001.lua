
-- Paladin (타임 리버스 : a_Time Reverse) : Level 1-10 : Toggle
function Skill_Begin105501001(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle105501001(caster, skillnum, bToggleOn, arg)
	InfoLog(9, "Skill_Toggle105501001 --")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle105501001....SkillDef is NIl" ..skillnum)
		return -1
	end
	-- Toggle Skill
	if bToggleOn==true then
		InfoLog(9, "Skill_Toggle105501001 Add")
		arg:SetInt(ACTARG_TOGGLESKILL, skillnum)
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		InfoLog(9, "Skill_Toggle105501001 Add")
		caster:DeleteEffect(kSkillDef:GetEffectNo())		
	end
	return 1
end

--[[
function Skill_Fire105501001(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult106400201(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire105501001....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == false then
		caster:DeleteEffect(iEffectNo)
	else
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	end
	
	return iIndex
end
]]

function Skill_Fail105501001(caster, kTargetArray, skillnum, result, arg)
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
