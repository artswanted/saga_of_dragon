
-- Wizard ( 체인 라이트닝: a_Chain Lighting) : Level 1-10
function Skill_Begin120000301(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire120000301(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult120000301(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire120000301....SkillDef is NIl" ..skillnum)
		return -1
	end
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == true then
		-- Effect가 없다면 추가한다.
		-- 공격스킬이기는 하지만, 계속적으로 공격하는 스킬인지라, Effect가 필요하다.
		-- Effect 내부에서는 지속적으로 MP 소모한다.
		caster:AddEffect(iEffectNo, skillnum, arg, caster)
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		if target:IsEqual(caster) == true then
			InfoLog(9, "Skill_Fire120000301 target:IsEqual(caster) == true")
		end
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail120000301(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
