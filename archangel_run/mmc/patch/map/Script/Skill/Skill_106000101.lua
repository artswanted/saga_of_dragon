
-- 투사 (룰렛 : a_Roulette) : Level 1-5
function Skill_Begin106000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106000101(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult106000101(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106000101....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), false)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			InfoLog(9, "Skill_Fire106000101 Value:" .. aresult:GetValue())
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	-- 반격당했는가를 검사
	local aresult = result:GetResult(caster:GetGuid(), false)
	if aresult:IsNil() == false and aresult:GetInvalid() == false then
		InfoLog(9, "Skill_Fire106000101 Value:" .. aresult:GetValue())
		DoFinalDamage(caster, caster, aresult:GetValue(), skillnum, arg)
	end
	
	return iIndex
end

function Skill_Fail106000101(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
