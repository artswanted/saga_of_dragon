
-- 검투사 (레이징 스톰 발사 : a_Weapon Break_Fire ) : Level 1
function Skill_Begin1065009011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1065009011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1065009011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1065009011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg);
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1065009011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1065009011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		InfoLog(9, "Skill_CanFire1065009011 return false")
		return false
	end
	
	return true
end

function Skill_CanReserve1065009011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end


function Skill_Begin1090023012(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1065009011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1065009011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1090023013(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1065009011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1065009011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1090023014(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1065009011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1065009011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1090023015(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1065009011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1065009011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1065009011(caster, kTargetArray, skillnum, actarg);
end
