
-- Ninja (그림자 도약: a_Shadow Leap) : Lv 1-5
function Skill_Begin170000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire170000101(caster, kTargetArray, skillnum, result, arg)
	-- 해당스킬은 서버가 해줄 작업이 없음
	--[[
	local bReturn = GetSkillResult170000101(caster, kTargetArray, skillnum, result)
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
	]]
	return 0
end

function Skill_Fail170000101(caster, kTargetArray, skillnum, result, arg)
	--[[
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	]]
	return false
end
