
-- 보쓰 
function Skill_Begin141(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire141(caster, kTargetArray, skillnum, result, arg)
	--InfoLog(9, "Skill_Fire141 -.-")
	--GetSkillResult102000301(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire141....SkillDef is Nil" ..skillnum)
		return -1
	end
	
	--InfoLog(9, "Skill_Fire141 1.0")
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	kUnitArray = NewUNIT_PTR_ARRAY()

	-- begin the first level. (Appear)
	local kUnitItor = NewObjectMgr_UnitIterator()
	kGround:GetFirstUnitType(UT_PLAYER, kUnitItor)
	local kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor) 
	while kUnit:IsNil() == false do
		local kCurrentPos = kGround:GetUnitPos(kUnit)
		local kFloorPos = kGround:ThrowRay(kCurrentPos, Point3(0, 0, -1), 10000)

		-- 점프 했는지 체크.
		if kFloorPos:GetZ() ~= 0 and kFloorPos:GetZ() + 50 > kCurrentPos:GetZ() then
			kUnitArray:AddUnit(kUnit)
		end
		kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
	end --while문 끝
	DeleteObjectMgr_UnitIterator(kUnitItor)

	CS_GetSkillResultDefault(skillnum, caster, kUnitArray, result)

	iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(9, "Skill_Fire102000301 3.0")
		if caster:IsEnemy(target, false) then	-- Enemy에게만 적용시킨다.
			--InfoLog(9, "Skill_Fire102000301 3.1")
			local aresult = result:GetResult(target:GetGuid())
			if aresult:IsNil() == false and aresult:GetInvalid() == false then
				--InfoLog(9, "Skill_Fire102000301 3.2")
				DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			end
		end
		
		iIndex = iIndex + 1
		target = kUnitArray:GetUnit(iIndex)
		--InfoLog(9, "Skill_Fire102000301 3.0 [" .. iIndex .. "]")
	end
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
end

function Skill_Fail141(caster, kTargetArray, skillnum, result, arg)
	return false
end
