
-- Fighter (스톰 블레이드 : Storm blade ) : Level 1
function Skill_Begin101000801(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire101000801(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult101000801(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire101000801....SkillDef is NIl" ..skillnum)
		return -1
	end

	-- 스킬사용중에는 무적상태가 되어야 한다.
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			local iDmg = aresult:GetValue()
			DoFinalDamage(caster, target, iDmg, skillnum, arg)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end	
	return iIndex
end

function Skill_Fail101000801(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(100)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
