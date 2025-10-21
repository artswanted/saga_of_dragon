
-- Thief (독 살포 : Venom) : Level 1-5
function Skill_Begin104000301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104000301(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult104000301(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire104000301....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		target:SetTarget(caster:GetGuid())
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
		end
	return iIndex
end

function Skill_Fail104000301(caster, kTargetArray, skillnum, result, arg)
	return false
end
