
-- 배틀메이지 (파이어 엠블렘 : a_Fire Emblem) : Level 1-5
function Skill_Begin109000901(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109000901(caster, kTargetArray, skillnum, result, arg)
	-- 클라이언트에서 모든것을 처리한다. 서버는 아무것도 할 것이 없다.
	--[[
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire109000901....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > 0 then
		InfoLog(8, "Skill_Fire109000901...FireEmblem already called")
		DeleteUNIT_PTR_ARRAY(kUnitArray)
		return -1
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	
	-- Create 파이어 엠블렘 --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	local iLoc = 0
	local iCount = kSkillDef:GetAbil(AT_COUNT)	--  설치될 수 있는 개수
	local iIndex = 0
	while iIndex < iCount do
		iLoc = RAND(iDistance + 1)
		InfoLog(9, "Skill_Fire109000901 Loc:" .. iLoc)
		kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "FireEmblem", iLoc)
		if kEntity:IsNil() == false then
			local iMagic = caster:GetAbil(AT_C_MAGIC_ATTACK)
			kEntity:SetAbil( AT_ATTR_ATTACK, iMagic )
			
			-- Effect 추가
			--kEntity:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		end
		
		iIndex = iIndex + 1
	end
	]]
	return 1
end

function Skill_Fail109000901(caster, kTargetArray, skillnum, result, arg)
	return false
end
