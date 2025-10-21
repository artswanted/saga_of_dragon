
-- 배틀메이지 (블리자드 공격 : a_Blizzard_Attack) : Level 1-5
function Skill_Begin1080001011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1080001011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1080001011(caster, kTargetArray, skillnum, result)

	InfoLog(9, "Skill_Fire1080001011 --")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1080001011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			if caster:IsEnemy(target, false) == true then
				InfoLog(9, "Skill_Fire1080001011 iEffectPer=" .. iEffectPer)
				DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
				if iRandValue < iEffectPer then
					target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
				end
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1080001011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1080001011(caster, kTargetArray, skillnum, actarg)
	InfoLog(9, "Skill_CanFire1080001011 --")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1080001011(caster, target, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1080001012(caster, skillnum, iStatus,arg)
	InfoLog(9, "Skill_Begin1080001012 --")
	return Skill_Begin1080001011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1080001012(caster, kTargetArray, skillnum, result, arg)
	InfoLog(9, "Skill_Fire1080001012 --")
	return Skill_Fire1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1080001012(caster, kTargetArray, skillnum, result, arg)
InfoLog(9, "Skill_Fail1080001012 --")
	return Skill_Fail1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1080001012(caster, kTargetArray, skillnum, actarg)
InfoLog(9, "Skill_CanFire1080001012 --")
	return Skill_CanFire1080001011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1080001012(caster, target, skillnum, actarg)
InfoLog(9, "Skill_CanReserve1080001012 --")
	return Skill_CanReserve1080001011(caster, target, skillnum, actarg)
end

function Skill_Begin1080001013(caster, skillnum, iStatus,arg)
	return Skill_Begin1080001011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1080001013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1080001013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1080001013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1080001011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1080001013(caster, target, skillnum, actarg)
	return Skill_CanReserve1080001011(caster, target, skillnum, actarg)
end

function Skill_Begin1080001014(caster, skillnum, iStatus,arg)
	return Skill_Begin1080001011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1080001014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1080001014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1080001014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1080001011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1080001014(caster, target, skillnum, actarg)
	return Skill_CanReserve1080001011(caster, target, skillnum, actarg)
end

function Skill_Begin1080001015(caster, skillnum, iStatus,arg)
	return Skill_Begin1080001011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1080001015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1080001015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1080001011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1080001015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1080001011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1080001015(caster, target, skillnum, actarg)
	return Skill_CanReserve1080001011(caster, target, skillnum, actarg)
end
