
-- Thief (찾아보시지 : Shadow Walk) : Level 1-10	-- Toggle
function Skill_Begin104000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Toggle104000101(caster, skillnum, bToggleOn, arg)
	--GetSkillResult104000101(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(6, "Skill_Toggle104000101....SkillDef is NIl" ..skillnum)
		return -1
	end
	-- Toggle Skill
	if bToggleOn==true then
		arg:SetInt(ACTARG_TOGGLESKILL, skillnum)
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	else
		caster:DeleteEffect(kSkillDef:GetEffectNo())		
	end
	return 1
end


function Skill_Fail104000101(caster, kTargetArray, skillnum, result, arg)
	return false
end
