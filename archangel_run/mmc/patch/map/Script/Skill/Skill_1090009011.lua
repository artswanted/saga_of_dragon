
-- 배틀메이지 (파이어 엠블렘 폭발 : a_Fire Emblem_Explosion) : Level 1-5
function Skill_Begin1090009011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090009011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1090009011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1080001011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local bFired = false
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			bFired = true
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	-- 폭발했으므로 자신은 죽어야 한다.
	--if bFired == true then
	--	caster:SetAbil(AT_HP, 0)
	--end
	return iIndex
end

function Skill_Fail1090009011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1090009011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1090009011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end




function Skill_Begin1090009012(caster, skillnum, iStatus,arg)
	return Skill_Begin1090009011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090009012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090009012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090009012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090009012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090009013(caster, skillnum, iStatus,arg)
	return Skill_Begin1090009011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090009013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090009013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090009013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090009013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090009014(caster, skillnum, iStatus,arg)
	return Skill_Begin1090009011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090009014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090009014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090009014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090009014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090009015(caster, skillnum, iStatus,arg)
	return Skill_Begin1090009011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090009015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090009015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090009011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090009015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090009011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090009015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090009011(caster, kTargetArray, skillnum, actarg)
end



