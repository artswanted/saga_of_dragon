-- 사냥꾼 (쥐덫 : a_Rat Trap) : Level 1-5
function Skill_Begin109000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109000101(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult109000101(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire109000101....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail109000101(caster, kTargetArray, skillnum, result, arg)
	return false
end