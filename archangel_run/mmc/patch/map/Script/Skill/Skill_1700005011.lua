
-- 닌자 (목둔술! 통나무 굴리기 발동 : a_Rolling Log) : Level 1
function Skill_Begin1700005011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1700005011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1700005011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1700005011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg);
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster);
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1700005011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1700005011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		InfoLog(9, "Skill_CanFire1700005011 return false")
		return false
	end
	
	return true
end

function Skill_CanReserve1700005011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end



function Skill_Begin1700005021(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1700005021(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1700005021(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1700005021(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1700005011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1700005021(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1700005011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1700005031(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1700005031(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1700005031(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1700005031(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1700005011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1700005031(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1700005011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1700005041(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1700005041(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1700005041(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1700005041(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1700005011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1700005041(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1700005011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1700005051(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1700005051(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1700005051(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1700005011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1700005051(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1700005011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1700005051(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1700005011(caster, kTargetArray, skillnum, actarg);
end

