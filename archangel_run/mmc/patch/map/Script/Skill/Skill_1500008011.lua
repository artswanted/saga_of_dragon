
-- 저격수 ( WP그레네이드 설치: a_WP Granade Fire) : Level 1-5
function Skill_Begin1500008011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1500008011(caster, kTargetArray, skillnum, result, arg)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1500008011....SkillDef is NIl" ..skillnum)
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

function Skill_Fail1500008011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1500008011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1500008011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end


function Skill_Begin11500008012(caster, skillnum, iStatus,arg)
	return Skill_Begin1500008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11500008012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11500008012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11500008012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11500008012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11500008013(caster, skillnum, iStatus,arg)
	return Skill_Begin1500008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11500008013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11500008013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11500008013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11500008013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11500008014(caster, skillnum, iStatus,arg)
	return Skill_Begin1500008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11500008014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11500008014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11500008014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11500008014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin11500008015(caster, skillnum, iStatus,arg)
	return Skill_Begin1500008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire11500008015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail11500008015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1500008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire11500008015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1500008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve11500008015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1500008011(caster, kTargetArray, skillnum, actarg)
end

