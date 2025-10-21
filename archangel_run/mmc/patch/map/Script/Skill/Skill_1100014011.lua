
-- 레인저 (클레이모어 폭발 : a_Claymore_Explosion) : Level 1-10
function Skill_Begin1100014011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1100014011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1110014011....SkillDef is NIl" ..skillnum)
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
	-- 죽는 연출은 클라이언트에서 하도록 내버려 두자
	--caster:SetAbil(AT_HP, 0)
	return iIndex
end

function Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
	-- 클레이모어는 스스로 터지지 않는다.
	return false
	--[[
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
	]]
end

function Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
	-- 클레이모어는 스스로 터지지 않는다.
	return false
	--[[
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
	]]
end


function Skill_Begin1100014012(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014013(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014014(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014015(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014016(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014016(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014016(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014016(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014016(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014017(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014017(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014017(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014017(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014017(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014018(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014018(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014018(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014018(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014018(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014019(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014019(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014019(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014019(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014019(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1100014020(caster, skillnum, iStatus,arg)
	return Skill_Begin1100014011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1100014020(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1100014020(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1100014011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1100014020(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1100014011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1100014020(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1100014011(caster, kTargetArray, skillnum, actarg)
end
