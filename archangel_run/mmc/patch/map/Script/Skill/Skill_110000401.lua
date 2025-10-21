
-- 궁수 (모서리로 찍으세요 : a_Bleeding) : Level 1-10
function Skill_Begin110000401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire110000401(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult110000401(caster, kTargetArray, skillnum, result)

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		local kSkillDef = GetSkillDef(skillnum)
		local iRand = RAND(ABILITY_RATE_VALUE)
		local iPer = kSkillDef:GetAbil(AT_PERCENTAGE)
		InfoLog(9, "Skill_Fire110000401 iRand:" .. iRand .. ", iPer:" .. iPer)
		if iRand < iPer then
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail110000401(caster, kTargetArray, skillnum, result, arg)
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
