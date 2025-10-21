
-- War Mage (스노우 블루스 작동 : ) : Level 1
function Skill_Begin1090023011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1090023011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1090023011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	local ipercent = kSkillDef:GetAbil(AT_PERCENTAGE);
	local iNewEffect = kSkillDef:GetAbil(AT_EFFECTNUM1);

	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg);

			local iRandValue = RAND(ABILITY_RATE_VALUE);
			if iRandValue < ipercent then
				InfoLog(9, "Skill_Fire1090023011....Add NewEffect : " ..iNewEffect.. "")
				target:AddEffect(iNewEffect, 0, arg, caster)
			end
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1090023011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1090023011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		InfoLog(9, "Skill_CanFire1090023011 return false")
		return false
	end
	
	return true
end

function Skill_CanReserve1090023011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end



function Skill_Begin1090023021(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023021(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023021(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023021(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090023011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023021(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090023011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1090023031(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023031(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023031(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023031(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090023011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023031(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090023011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1090023041(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023041(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023041(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023041(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090023011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023041(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090023011(caster, kTargetArray, skillnum, actarg);
end


function Skill_Begin1090023051(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090023051(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_Fail1090023051(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090023011(caster, kTargetArray, skillnum, result, arg);
end

function Skill_CanFire1090023051(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090023011(caster, kTargetArray, skillnum, actarg);
end

function Skill_CanReserve1090023051(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090023011(caster, kTargetArray, skillnum, actarg);
end

