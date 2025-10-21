
-- 사냥 (스플래쉬 트랩 : a_Splash Trap) : Level 1-5
function Skill_Begin110000801(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110000801(caster, kTargetArray, skillnum, result, arg)
	-- Client에서 알아서 생성시킨다.
	-- 서버는 아무것도 할 것이 없다.
	--[[
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire110000801....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffect = kSkillDef:GetEffectNo()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > 0 then
		--InfoLog(8, "Skill_Fire110000801...FireEmblem already called")
		-- 이미 설치된 것이 있으면 폭발 시킨다.
		local iIndex = 0
		local kTrap = kUnitArray:GetUnit(iIndex)
		while kTrap:IsNil() == false do
			kTrap:SetAbil(AT_HP, 0)	-- Trap 삭제하기

			iIndex = iIndex + 1			
			kTrap = kUnitArray:GetUnit(iIndex)
		end
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	
	-- Create 스플래쉬 트랩 --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "SplashTrap", iDistance)
	if kEntity:IsNil() == false then
		local iAdd = caster:GetAbil(AT_C_PHY_ATTACK)
		kEntity:SetAbil( AT_ATTR_ATTACK, iAdd )
	end
	]]
		
	return 1
end

function Skill_Fail110000801(caster, kTargetArray, skillnum, result, arg)
	return false
end
