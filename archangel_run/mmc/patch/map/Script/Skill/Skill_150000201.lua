
-- Sniper (부비트랩 : a_Booby Trap) Level 1-10
function Skill_Begin150000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire150000201(caster, kTargetArray, skillnum, result, arg)
	--[[
	local bReturn = GetSkillResult150000201(caster, kTargetArray, skillnum, result)
	
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
	]]
	return 0
end

function Skill_Fail150000201(caster, kTargetArray, skillnum, result, arg)
	return false
end
