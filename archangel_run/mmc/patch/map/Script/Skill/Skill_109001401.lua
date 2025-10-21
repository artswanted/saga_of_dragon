
-- Wizard ( 썬더 브레이크: a_Thunder Break) : Level 1-10
function Skill_Begin109001401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109001401(caster, kTargetArray, skillnum, result, arg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire109001401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	return iIndex
end

function Skill_Fail109001401(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end

function Skill_CanReserve109001401(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

-- Wizard ( 썬더 브레이크<효과>: a_Thunder Break_Target) : Level 1-10
function Skill_Begin1090014011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090014011(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult1090014001(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1090014011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
			InfoLog(5, "Skill_Fire1090014011   4");
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			-- 데미지 마법 공격력의 %
			local iPercent = kSkillDef:GetAbil(AT_MAGIC_DMG_PER)
			local iMagic = caster:GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE
		
			InfoLog(5, "Skill_Fire1090014011...Damage : " .. iMagic ..")");
			DoFinalDamage(caster, target, iMagic, skillnum, arg)
			target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail1090014011(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
