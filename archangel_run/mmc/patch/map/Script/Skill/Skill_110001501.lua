
-- 레인저 (자동 사격 장치 : a_Auto Shoot System) : Level 1-5
function Skill_Begin110001501(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110001501(caster, kTargetArray, skillnum, result, arg)
	-- 클라이언트에서 처리하는 것으로 수정되었음.
	--[[
	InfoLog(9, "Skill_Fire110001501 --")
	local kGround = arg:Get(ACTARG_GROUND)
	InfoLog(9, "Skill_Fire110001501 10")
	kGround = Ground(kGround)
	InfoLog(9, "Skill_Fire110001501 11")
	
	local kSkillDef = GetSkillDef(skillnum)
	InfoLog(9, "Skill_Fire110001501 12")
	if kSkillDef:IsNil() == true then
		InfoLog(9, "Skill_Fire110001501 13")
		InfoLog(2, "Skill_Fire110001501....SkillDef is NIl" ..skillnum)
		return -1
	end
	InfoLog(9, "Skill_Fire110001501 14")
	
	local kUnitArray = NewUNIT_PTR_ARRAY()
	InfoLog(9, "Skill_Fire110001501 15")
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	InfoLog(9, "Skill_Fire110001501 AT_CLASS:" .. AT_CLASS .. ", Abil:" .. iClass)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > 0 then
		DeleteUNIT_PTR_ARRAY(kUnitArray)
		return 1
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	
	-- Create AutoShootSystem --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "AutoShoot", iDistance)
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