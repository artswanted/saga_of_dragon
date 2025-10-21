
-- a_magician_charge_laser : 법사 응집공격
function Skill_Begin102101201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102101201(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult102101201(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire102101201....SkillDef is NIl" ..skillnum)
		return -1
	end
		
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), false)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			local iDmg = aresult:GetValue()
			DoFinalDamage(caster, target, iDmg, skillnum, arg)
			if iDmg > 0 then
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
			end			
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail102101201(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(100)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
