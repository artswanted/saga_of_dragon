
-- War Mage ( 백 드래프트 발사: a_Back Draft Fire) : Level 1-5
function Skill_Begin1090024011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090024011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1090024011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1090024011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	local AddEffectNo = kSkillDef:GetAbil(AT_EFFECTNUM1);
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			target:AddEffect(AddEffectNo, 0, arg, caster)
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail1090024011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1090024011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1090024011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end


function Skill_Begin11090024012(caster, skillnum, iStatus,arg)
	return Skill_Begin1090024011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11090024012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11090024012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11090024012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11090024012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11090024013(caster, skillnum, iStatus,arg)
	return Skill_Begin1090024011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11090024013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11090024013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11090024013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11090024013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11090024014(caster, skillnum, iStatus,arg)
	return Skill_Begin1090024011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11090024014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11090024014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11090024014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11090024014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11090024015(caster, skillnum, iStatus,arg)
	return Skill_Begin1090024011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11090024015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11090024015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090024011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11090024015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090024011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11090024015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090024011(caster, kTargetArray, skillnum, actarg)
end

