
-- 트랩퍼 (MP제로 트랩 작동 : a_MP-Zeri Trap Active) : Level 1-5
function Skill_Begin1100028011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100028011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1100028011(caster, kTargetArray, skillnum, result)

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
			local iDamage = aresult:GetValue();
			local iValue = iDamage * 2;
			InfoLog(5, "Skill_Fire1080001011 value : "..iValue);
			iValue = math.min(target:GetAbil(AT_C_MAX_MP), iValue);			

			target:SetMP(iValue, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL, caster:GetGuid());
			DoFinalDamage(caster, target, iDamage, skillnum, arg);
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

function Skill_Fail1100028011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1100028011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1100028011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100028012(caster, skillnum, iStatus,arg)
	return Skill_Begin1100028011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100028012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100028012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100028012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100028012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100028013(caster, skillnum, iStatus,arg)
	return Skill_Begin1100028011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100028013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100028013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100028013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100028013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100028014(caster, skillnum, iStatus,arg)
	return Skill_Begin1100028011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100028014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100028014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100028014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100028014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100028015(caster, skillnum, iStatus,arg)
	return Skill_Begin1100028011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100028015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100028015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100028011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100028015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100028011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100028015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100028011(caster, kTargetArray, skillnum, actarg)
end



