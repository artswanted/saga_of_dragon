
-- 배틀 메이지 (스파이럴 캐논 : a_Spiral Cannon) : Level 1-5
function Skill_Begin109000601(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109000601(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult109000601(caster, kTargetArray, skillnum, result)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), false)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex	
end

function Skill_Fail109000601(caster, kTargetArray, skillnum, result, arg)
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
