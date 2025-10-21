
-- Trapper ( 건들지마라!: a_Don’t Touch Me) : Level 1
function Skill_Begin140000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire140000201(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--local bReturn = GetSkillResult140000201(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire140000201....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), iMagicPower, arg, caster)
	return iIndex
end

function Skill_Fail140000201(caster, kTargetArray, skillnum, result, arg)
	--local iIndex = 0
	--local target = kTargetArray:GetUnit(iIndex)
	--while target:IsNil() == false do
	--	caster:SyncRandom(ABILITY_RATE_VALUE)

	--	iIndex = iIndex + 1
	--	target = kTargetArray:GetUnit(iIndex)
	--end
	return false
end
