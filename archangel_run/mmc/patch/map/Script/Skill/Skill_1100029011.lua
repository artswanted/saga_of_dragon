
-- 트랩퍼 ( 스모크 그레네이드 설치: a_Smoke Granade Active) : Level 1-5
function Skill_Begin1100029011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100029011(caster, kTargetArray, skillnum, result, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1100029011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster);
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail1100029011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1100029011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1100029011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end


function Skill_Begin11100029012(caster, skillnum, iStatus,arg)
	return Skill_Begin1100029011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11100029012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11100029012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11100029012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11100029012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11100029013(caster, skillnum, iStatus,arg)
	return Skill_Begin1100029011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11100029013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11100029013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11100029013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11100029013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11100029014(caster, skillnum, iStatus,arg)
	return Skill_Begin1100029011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11100029014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11100029014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11100029014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11100029014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11100029015(caster, skillnum, iStatus,arg)
	return Skill_Begin1100029011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11100029015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11100029015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100029011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11100029015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100029011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11100029015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100029011(caster, kTargetArray, skillnum, actarg)
end

