
-- a_Rapidly Shot_Cast : 난사 캐스트 Level 1-5
function Skill_Begin110002001(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110002001(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--GetSkillResult110002001(caster, kTargetArray, skillnum, result)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire110002001....SkillDef is NIl" ..skillnum)
		return -1
	end
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return 1
end

function Skill_Fail110002001(caster, kTargetArray, skillnum, result, arg)
	return false
end
