
-- 트랩퍼 (사일런스 트랩 작동 : a_Silence Trap Active) : Level 1-5
function Skill_Begin1100027011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100027011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1100027011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1080001011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local bFired = false
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	local AddEffectNo = kSkillDef:GetAbil(AT_EFFECTNUM1);
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)

			if iRandValue < iEffectPer then
				target:AddEffect(AddEffectNo, 0, arg, caster)
			end			
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

function Skill_Fail1100027011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1100027011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1100027011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end




function Skill_Begin1100027012(caster, skillnum, iStatus,arg)
	return Skill_Begin1100027011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100027012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100027012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100027012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100027012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100027013(caster, skillnum, iStatus,arg)
	return Skill_Begin1100027011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100027013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100027013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100027013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100027013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100027014(caster, skillnum, iStatus,arg)
	return Skill_Begin1100027011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100027014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100027014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100027014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100027014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100027015(caster, skillnum, iStatus,arg)
	return Skill_Begin1100027011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100027015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100027015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100027011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100027015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100027011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100027015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100027011(caster, kTargetArray, skillnum, actarg)
end



