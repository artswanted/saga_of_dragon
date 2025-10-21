
-- Fighter (인생은 한방 : Hammer Crush) : Level 1-10
function Skill_Begin101000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire101000101(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult101000101(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire101000101....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	--InfoLog(9, "Skill_Fire101000101 11 EffectPer=" .. iEffectPer)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(9, "Skill_Fire101000101 20")
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			--InfoLog(9, "Skill_Fire101000101 40 Damage:" .. aresult:GetValue())
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			if iRandValue < iEffectPer and aresult:GetInvalid() == false then
				-- Stun ----
				--InfoLog(9, "Skill_Fire101000101 STUNNED")
				target:AddEffect(kSkillDef:GetAbil(AT_EFFECTNUM1), 0, arg, caster)
				-- PhysicalAttack 이후에 호출 해야 한다.
				-- PhysicalAttack->OnDamage에서 SetAbil(AT_FRONZEN, 0) 해서, Target의 AT_FROZE 값이 초기화 되어 버린다.
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
		--InfoLog(9, "Skill_Fire101000101 50")
	end
	
	--InfoLog(9, "Skill_Fire101000101 99")
	return iIndex
end

function Skill_Fail101000101(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(100)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
