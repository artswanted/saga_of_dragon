
-- 마할카 휠윈드 (제자리) : a_mon_whirlwind
function Skill_Begin6006902(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6006902(caster, kTargetArray, skillnum, result, actarg)
	--InfoLog(8, "Skill_Fire1 --")
	local bReturn = GetSkillResult6006902(caster, kTargetArray, skillnum, result)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire6006902....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(8, "Skill_Fire1  10")
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			--InfoLog(8, "Skill_Fire1  Damaged:" .. aresult:GetValue())
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, actarg)
			if 0 < kSkillDef:GetEffectNo() then
				target:AddEffect(kSkillDef:GetEffectNo(), 0, actarg, caster)
			end
			--InfoLog(8, "Skill_Fire6006902....GetEffectNo is " .. kSkillDef:GetEffectNo() .. "skillno : " ..skillnum)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail6006902(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6006902(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

