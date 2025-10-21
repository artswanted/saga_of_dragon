
-- 레인저 (클레이모어 : a_Claymore) : Level 1-10
function Skill_Begin110001401(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110001401(caster, kTargetArray, skillnum, result, arg)
	--[[
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire110001401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iMax = kSkillDef:GetAbil(AT_COUNT)	-- 최대 설치 가능 개수
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > iMax then
		-- 첫번째 ClayMore는 삭제되어야 한다.
		Remove1stUnit(kUnitArray)
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	
	-- Create Craymore --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "Craymore", iDistance)
	if kEntity:IsNil() == false then
		local iAdd = caster:GetAbil(AT_C_PHY_ATTACK)
		kEntity:SetAbil( AT_ATTR_ATTACK, iAdd )
			
		-- Effect 추가
		--kEntity:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	end
	]]

	return 1
end

function Skill_Fail110001401(caster, kTargetArray, skillnum, result, arg)
	return false
end

function Remove1stUnit(kUnitArray)
	local iLowest = 999999
	local iCount = 0
	local kUnit = kUnitArray:GetUnit(iCount)
	local kChoose = kUnit
	while kUnit:IsNil() == false do
		local iIndex = kUnit:GetAbil(AT_INDEX)
		if iIndex < iLowest then
			iLowest = iIndex
			kChoose = kUnit
		end
		
		iCount = iCount + 1
		kUnit = kUnitArray:GetUnit(iCount)
	end
	
	if kChoose:IsNil() == false then
		kChoose:SetAbil(AT_HP, 0)
	end
end