
-- 압둘라 넷바인더 : a_mon_shot_skill_03
function Skill_Begin6013903(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6013903(caster, kTargetArray, skillnum, result, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire6013903....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			InfoLog(9, "Skill_Fire6013903 AddEffect(" .. kSkillDef:GetEffectNo() .. ")")
			target:AddEffect(kSkillDef:GetEffectNo(), 0, actarg, caster)
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail6013903(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end


function Skill_CanReserve6013903(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

