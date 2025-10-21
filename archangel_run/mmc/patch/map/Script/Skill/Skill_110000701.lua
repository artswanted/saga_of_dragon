
-- 사냥꾼 ( 크럭스 샷: a_Crux Shot) : Level 1-10
function Skill_Begin110000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110000701(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--GetSkillResult110000701(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire110000701....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:SetAbil(AT_1ST_ATTACK_ADDED_RATE, kSkillDef:GetAbil(AT_R_PHY_ATTACK))
	InfoLog(9, "Skill_Fire110000701 Added:" .. caster:GetAbil(AT_1ST_ATTACK_ADDED_RATE))
	
	return iIndex
end

function Skill_Fail110000701(caster, kTargetArray, skillnum, result, arg)
	--[[
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	]]
	return false
end
