
-- 메이지 (콰그마이어 : a_Quagmire) : Level 1-10
function Skill_Begin109000401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109000401(caster, kTargetArray, skillnum, result, arg)
	--InfoLog(9, "Skill_Fire109000401...--")
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire109000401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > 0 then
		InfoLog(8, "Skill_Fire109000401...Quagmire already called")
		DeleteUNIT_PTR_ARRAY(kUnitArray)
		return -1
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	
	-- Create Quagmire --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "Quagmire", iDistance)
	if kEntity:IsNil() == false then
		
		-- Effect 추가
		--kEntity:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	end
	return 1
end

function Skill_Fail109000401(caster, kTargetArray, skillnum, result, arg)
	return false
end
