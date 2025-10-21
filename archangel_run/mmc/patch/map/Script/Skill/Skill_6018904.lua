-- 우마이 바닥치는 스킬
function Skill_Begin6018904(caster, skillnum, iStatus,arg)
	InfoLog(2, "Skill_Begin6018904" ..skillnum)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6018904(caster, kTargetArray, skillnum, result, arg)
	InfoLog(2, "Skill_Fire6018904 Start" ..skillnum)
	local kTarget = kTargetArray:GetUnit(0)
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	InfoLog(2, "Skill_Fire6018904 1 " ..skillnum)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire6018904....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > 0 then
		--InfoLog(8, "Skill_Fire6018904...SummonThron already called")
		DeleteUNIT_PTR_ARRAY(kUnitArray)
		return 0
	end

	InfoLog(2, "Skill_Fire6018904 2 " ..skillnum)
	DeleteUNIT_PTR_ARRAY(kUnitArray)

	-- Create SummonThron --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	InfoLog(2, "Skill_Fire6018904 3 iClass : ".. iClass .. " level : " .. kSkillDef:GetAbil(AT_LEVEL) .. "skillnum : " .. skillnum)

	local kTarget = kTargetArray:GetUnit(0)
	local kTargetPos = caster:GetPos()
	if false == kTarget:IsNil() then
		local TempV = Point3()
--		kTargetPos = TempV:GetDirectionVector(kTarget:GetPos(), caster:GetPos())
--		kTargetPos = kTargetPos:Multiply(iDistance)
		kTargetPos = kTarget:GetPos()
	else
		InfoLog(2, "Skill_Fire6018904....Target Count == 0 " ..skillnum)
	end

--	kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "SummonThron", iDistance)
	local kEntity = kGround:CreateEntityAtPoint(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "SummonThorn", kTargetPos)
	if kEntity:IsNil() == false then
		-- 공격력 셋팅
		local iPercent = kSkillDef:GetAbil(AT_MAGIC_DMG_PER)
		local iMagic = caster:GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE
		kEntity:SetAbil( AT_ATTR_ATTACK, iMagic )
	end
	return 1
end

function Skill_Fail6018904(caster, kTargetArray, skillnum, result, arg)
	return false
end

function Skill_CanReserve6018904(caster, target, skillnum, actarg)
	return Skill_CanReserveDefault(caster, target, skillnum, actarg)
end

