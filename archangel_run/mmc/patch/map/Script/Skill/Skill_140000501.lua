
-- Trapper ( 마나 번!: a_Mana Burn) : Level 1-10
function Skill_Begin140000501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire140000501(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult140000501(caster, kTargetArray, skillnum, result)

	--[[
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire140000401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), iMagicPower, arg, caster)
	return iIndex
	]]
	return 0
end

function Skill_Fail140000501(caster, kTargetArray, skillnum, result, arg)
	--local iIndex = 0
	--local target = kTargetArray:GetUnit(iIndex)
	--while target:IsNil() == false do
	--	caster:SyncRandom(ABILITY_RATE_VALUE)

	--	iIndex = iIndex + 1
	--	target = kTargetArray:GetUnit(iIndex)
	--end
	return false
end
