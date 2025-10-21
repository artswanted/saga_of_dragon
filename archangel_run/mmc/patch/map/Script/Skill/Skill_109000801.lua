
-- 배틀 메이지 (디멘션 커터 : a_Dimension Cutter) : Level 1-10
function Skill_Begin109000801(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109000801(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult109000801(caster, kTargetArray, skillnum, result)
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

function Skill_Fail109000801(caster, kTargetArray, skillnum, result, arg)
	return false
end
