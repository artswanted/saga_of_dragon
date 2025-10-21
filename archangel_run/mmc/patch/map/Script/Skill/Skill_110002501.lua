
-- 트랩퍼 (건들지마라! : a_Dont touch me! ) : Level 1-10
function Skill_Begin110002501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110002501(caster, kTargetArray, skillnum, result, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire110002501....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	while target:IsNil() == false do
		if target:IsUnitType(UT_PLAYER) == true then
			InfoLog(5, "Skill_Fire110002501 1")
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end	

	return iIndex
end

function Skill_Fail110002501(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(100)
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
