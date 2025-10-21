
-- 레인저 (자동 사격 장치 발사 : a_Auto Shoot System_Fire) : Level 1-5
function Skill_Begin1100015011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100015011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1100015011(caster, kTargetArray, skillnum, result)

	--InfoLog(9, "Skill_Fire1100015011 ---------1")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1100015011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1100015011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1100015011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1100015011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end


function Skill_Begin1100015012(caster, skillnum, iStatus,arg)
	return Skill_Begin1100015011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100015012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100015012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100015012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100015012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100015013(caster, skillnum, iStatus,arg)
	return Skill_Begin1100015011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100015013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100015013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100015013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100015013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100015014(caster, skillnum, iStatus,arg)
	return Skill_Begin1100015011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100015014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100015014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100015014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100015014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100015015(caster, skillnum, iStatus,arg)
	return Skill_Begin1100015011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100015015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100015015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100015011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100015015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100015011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100015015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100015011(caster, kTargetArray, skillnum, actarg)
end

