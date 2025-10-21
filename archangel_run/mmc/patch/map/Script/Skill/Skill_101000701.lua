
-- Fighter (갑옷부수기 : Armor Bread ) : Level 1-10
function Skill_Begin101000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire101000701(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult101000701(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire101000701....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	InfoLog(9, "Skill_Fire101000701 10")
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire101000701 20")
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			local iDmg = aresult:GetValue()
			InfoLog(9, "Skill_Fire101000701 iDmg:" .. iDmg)
			DoFinalDamage(caster, target, iDmg, skillnum, arg)
			if iDmg > 0 then
				InfoLog(9, "Skill_Fire101000701 40")
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end	
	return iIndex
end

function Skill_Fail101000701(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(100)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
