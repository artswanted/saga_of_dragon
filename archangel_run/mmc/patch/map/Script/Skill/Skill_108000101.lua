
-- 베틀메이지[전투마법사] (블리자드 : Blizzard) : Level 1-10
function Skill_Begin108000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire108000101(caster, kTargetArray, skillnum, result, arg)
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(2, "Skill_Fire108000101....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iClass = kSkillDef:GetAbil(AT_CLASS)
	local iFound = kGround:GetEntity(caster, iClass, kUnitArray)
	if iFound > 0 then
		--InfoLog(8, "Skill_Fire108000101...Blizzard already called")
		DeleteUNIT_PTR_ARRAY(kUnitArray)
		return 0
	end
	DeleteUNIT_PTR_ARRAY(kUnitArray)

	-- Create Blizzard --------
	local iDistance = kSkillDef:GetAbil(AT_DISTANCE)
	kEntity = kGround:CreateEntity(caster, iClass, kSkillDef:GetAbil(AT_LEVEL), "Blizzard", iDistance)
	if kEntity:IsNil() == false then
		-- 공격력 셋팅
		local iPercent = kSkillDef:GetAbil(AT_MAGIC_DMG_PER)
		local iMagic = caster:GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE
		kEntity:SetAbil( AT_ATTR_ATTACK, iMagic )
		
		-- Effect 추가
		--kEntity:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	end
	return 1
end

function Skill_Fail108000101(caster, kTargetArray, skillnum, result, arg)
	return false
end
