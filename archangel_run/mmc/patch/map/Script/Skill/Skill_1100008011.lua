
-- 사냥꾼 (스플래쉬 트랩 폭발 : a_Splash Trap_Explosion) : Level 1-5
function Skill_Begin1100008011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100008011(caster, kTargetArray, skillnum, result, arg)
	InfoLog(9, "Skill_Fire1100008011 --")
	local bReturn = GetSkillResult1100008011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1100008011....SkillDef is NIl" ..skillnum)
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
	
	-- 폭발했으므로 자신은 죽어야 한다.
	--caster:SetAbil(AT_HP, 0)
	return iIndex
end

function Skill_Fail1100008011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1100008011(caster, kTargetArray, skillnum, actarg)
	InfoLog(9, "Skill_CanFire1100008011 --")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		InfoLog(9, "Skill_CanFire1100008011 return false")
		return false
	end
	
	InfoLog(9, "Skill_CanFire1100008011 return true")
	return true
end

function Skill_CanReserve1100008011(caster, kTargetArray, skillnum, actarg)
	InfoLog(9, "Skill_CanReserve1100008011 --")
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end


function Skill_Begin1100008012(caster, skillnum, iStatus,arg)
	return Skill_Begin1100008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100008012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100008012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100008012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100008012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100008013(caster, skillnum, iStatus,arg)
	return Skill_Begin1100008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100008013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100008013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100008013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100008013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100008014(caster, skillnum, iStatus,arg)
	return Skill_Begin1100008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100008014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100008014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100008014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100008014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100008015(caster, skillnum, iStatus,arg)
	return Skill_Begin1100008011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100008015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100008015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100008011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100008015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100008011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100008015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100008011(caster, kTargetArray, skillnum, actarg)
end
