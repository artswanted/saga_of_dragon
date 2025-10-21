
-- 데스마스터 블럭킹 : a_mon_blocking
function Skill_Begin6009603(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6009603(caster, kTargetArray, skillnum, result, actarg)
	--local bReturn = GetSkillResult6009603(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire6009603....SkillDef is Nil" ..skillnum)
		return -1
	end
		
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false then
			local kEffect = target:GetEffect(kSkillDef:GetEffectNo(), false)
			if kEffect:IsNil() == true then
				target:AddEffect(kSkillDef:GetEffectNo(), 0, actarg, caster)
			end
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail6009603(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6009603(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

