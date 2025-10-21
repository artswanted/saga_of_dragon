
-- Magician (마녀의 저주 : a_Transformation) : Level 1-5
function Skill_Begin102000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult102000201(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire102000201....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	while target:IsNil() == false do
		--	Boss는 안걸리게 한다
		if false == target:IsUnitType(UT_BOSSMONSTER) then
			local iRandValue = RAND(ABILITY_RATE_VALUE)
			local aresult = result:GetResult(target:GetGuid(), true)
			if aresult:IsNil() == false and aresult:GetInvalid() == false then
				if iRandValue < iEffectPer and aresult:GetInvalid() == false then
					-- Stun ----
					target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
				else
					aresult:SetMissed(true)
				end
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail102000201(caster, kTargetArray, skillnum, result, arg)
	return false
end
