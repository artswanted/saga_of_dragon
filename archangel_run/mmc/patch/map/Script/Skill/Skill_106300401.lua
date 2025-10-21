
-- 기사 (분노의 외침 : a_Stumblebum) : Level 1-5
function Skill_Begin106300401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106300401(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult106300401(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106300401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iStunPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	InfoLog(9, "Skill_Fire106300401 Percent:" .. iStunPer)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			InfoLog(9, "Skill_Fire106300401 30")
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
			if RAND(ABILITY_RATE_VALUE) < iStunPer then
				InfoLog(5, "Skill_Fire106300401 AddEffect:" .. kSkillDef:GetAbil(AT_EFFECTNUM1))
				target:AddEffect(kSkillDef:GetAbil(AT_EFFECTNUM1), 0, arg, caster)
			else
				aresult:SetMissed(true)
			end
			
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail106300401(caster, kTargetArray, skillnum, result, arg)
	return false
end
