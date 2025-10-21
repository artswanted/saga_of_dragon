
-- 암살자 (난도질: a_Haphazard) : Lv 1-10
function Skill_Begin104301401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104301401(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult104301401(caster, kTargetArray, skillnum, result)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire104301401....SkillDef is NIl" ..skillnum)
		return -1
	end

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
	
	-- 공격속도 올리는 버프 상태
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return iIndex
end

function Skill_Fail104301401(caster, kTargetArray, skillnum, result, arg)
	return false
end
