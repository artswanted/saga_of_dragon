
-- Sniper (개틀링 러쉬 : a_Gatling Rush) Level 1-5
function Skill_Begin150001001(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire150001001(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult150001001(caster, kTargetArray, skillnum, result)
	
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

function Skill_Fail150001001(caster, kTargetArray, skillnum, result, arg)
	return false
end
