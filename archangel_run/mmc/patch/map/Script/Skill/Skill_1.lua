
-- 몬스터기본
function Skill_Begin1(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1(caster, kTargetArray, skillnum, result, actarg)
	--InfoLog(8, "Skill_Fire1 --")
	local bReturn = GetSkillResult1(caster, kTargetArray, skillnum, result)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(8, "Skill_Fire1  10")
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			--InfoLog(8, "Skill_Fire1  Damaged:" .. aresult:GetValue())
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, actarg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail1(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve1(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

