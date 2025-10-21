
-- 검투사 (웨폰 퀴큰 : a_Weapon Quicken) : Level 1-10
function Skill_Begin106500801(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106500801(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult106500801(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106500801....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	while target:IsNil() == false do
		if target:IsUnitType(UT_PLAYER) == true then
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex

end

function Skill_Fail106500801(caster, kTargetArray, skillnum, result, arg)
	return false
end
