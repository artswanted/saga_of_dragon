
-- 레인저 (디토네이터 : a_Detonator) : Level 1
function Skill_Begin110001601(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110001601(caster, kTargetArray, skillnum, result, arg)
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire110001601....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end	

	-- 서버가 스스로 Target을 찾아 Claymore를 터트리는 방법
	-- 현재는 클라이언트가 Target을 찾아 공격하도록 변경되었음.
	--[[
	-- 모든 ClayMore 를 찾아 폭발시킨다.
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	local kNilTarget = Unit()
	kNilTarget:Empty()
	
	local iCount = 0
	local kCray = kUnitArray:GetUnit(iCount)
	while kCray:IsNil() == false do
		local iFireSkill = kCray:GetAbil(AT_MON_SKILL_01)
		if iFireSkill > 0 then
			local kResult = NewActionResultVector()
			kCray:SkillFire(iFireSkill, kNilTarget, kResult, arg, true)
			DeleteActionResultVector(kResult)
		end
		kCray:SetAbil(AT_HP, 0)	-- Craymore 삭제하기
		
		iCount = iCount + 1
		kCray = kUnitArray:GetUnit(iCount)
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	]]
	return 1
end

function Skill_Fail110001601(caster, kTargetArray, skillnum, result, arg)
	return false
end

