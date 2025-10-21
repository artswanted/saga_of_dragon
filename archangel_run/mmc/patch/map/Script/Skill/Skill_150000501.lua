
-- Sniper (핀포인트 어택 : a_Pin-Point Attack) Level 1-10
function Skill_Begin150000501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire150000501(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult150000501(caster, kTargetArray, skillnum, result)
	
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

function Skill_Fail150000501(caster, kTargetArray, skillnum, result, arg)
	return false
end
