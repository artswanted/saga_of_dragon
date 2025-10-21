
-- Sniper (슬링 : a_Sling) Level 1-5
function Skill_Begin150000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire150000101(caster, kTargetArray, skillnum, result, arg)
	--[[
	local bReturn = GetSkillResult150000101(caster, kTargetArray, skillnum, result)
	
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

function Skill_Fail150000101(caster, kTargetArray, skillnum, result, arg)
	return false
end
