
-- Magician (디텍션 : a_Detection) : Level 1-5
function Skill_Begin102000301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000301(caster, kTargetArray, skillnum, result, arg)
	--InfoLog(9, "Skill_Fire102000301 --")
	--GetSkillResult102000301(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire102000301....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	--InfoLog(9, "Skill_Fire102000301 10")
	local kGround = arg:Get(ACTARG_GROUND)
	--InfoLog(9, "Skill_Fire102000301 11")
	kGround = Ground(kGround)
	--InfoLog(9, "Skill_Fire102000301 12")
	local kUnitArray = NewUNIT_PTR_ARRAY()
	--InfoLog(9, "Skill_Fire102000301 20")
	local iDetectRange = kSkillDef:GetAbil(AT_DETECT_RANGE)
	InfoLog(9, "Skill_Fire102000301 21")
	kGround:GetUnitInRange(caster:GetPos(), iDetectRange, UT_MONSTER, kUnitArray)
	kGround:GetUnitInRange(caster:GetPos(), iDetectRange, UT_PLAYER, kUnitArray)
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire102000301 30")
		if caster:IsEnemy(target, false) == true then	-- Enemy에게만 적용시킨다.
			InfoLog(9, "Skill_Fire102000301 31")
			--local iRandValue = RAND(ABILITY_RATE_VALUE)
			--local aresult = result:GetResult(target:GetGuid(), true)
			--if iRandValue < iEffectPer and aresult:IsNil() == false and aresult:GetInvalid() == false then
				-- Magic Defence decrease
			--	target:AddEffect(kSkillDef:GetEffectNo(), kSkillDef:GetAbil(AT_R_MAGIC_DEFENCE), arg, caster)
			--end
			--if iRandValue < iEffectPer then
				-- Magic Defence decrease
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
				-- 10초동안 숨기 스킬 사용 금지 하기 위한 Effect
				local iEffect = kSkillDef:GetAbil(AT_EFFECTNUM1)
				if iEffect > 0 then
					target:AddEffect(iEffect, 0, arg, caster)
				end
			--end

			InfoLog(9, "Skill_Fire102000301 32")
			-- Hidden 상태인가 검사~
			if target:GetAbil(AT_UNIT_HIDDEN) > 0 then
				InfoLog(9, "Skill_Fire102000301 ..DETECTED")
				target:SetAbil(AT_UNIT_HIDDEN, 0, true)
			end		
		end
		
		iIndex = iIndex + 1
		target = kUnitArray:GetUnit(iIndex)
	end
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	return iIndex
end

function Skill_Fail102000301(caster, kTargetArray, skillnum, result, arg)
	return false
end
