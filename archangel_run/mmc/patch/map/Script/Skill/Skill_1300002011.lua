
-- War Mage (클라우드 킬 설치 : ) : Level 1-10
function Skill_Begin1300002011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1300002011(caster, kTargetArray, skillnum, result, arg)
	InfoLog(9, "Skill_Fire1300002011 --")
	local bReturn = GetSkillResult1300002011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1300002011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire1300002011 21")
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			InfoLog(9, "Skill_Fire1300002011 22")
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end

		InfoLog(9, "Skill_Fire1300002011 25")
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1300002011(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end

function Skill_CanFire1300002011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		InfoLog(9, "Skill_CanFire1100008011 return false")
		return false
	end
	
	return true
end

function Skill_CanReserve1300002011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end
