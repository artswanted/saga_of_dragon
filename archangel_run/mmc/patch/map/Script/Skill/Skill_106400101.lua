
-- Paladin (프레셔 : a_Pressure) : Level 1-10
function Skill_Begin106400101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106400101(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult106400101(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106400101....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			if iRandValue < iEffectPer and aresult:GetInvalid() == false then
				-- Stun ----
				target:AddEffect(kSkillDef:GetAbil(AT_EFFECTNUM1), 0, arg, caster)
				-- PhysicalAttack 이후에 호출 해야 한다.
				-- PhysicalAttack->OnDamage에서 SetAbil(AT_FRONZEN, 0) 해서, Target의 AT_FROZE 값이 초기화 되어 버린다.
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail106400101(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
