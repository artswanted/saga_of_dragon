
-- 기사 (제네식기어-수호 : Genesic Gear - Protect ) : Level 1-10
function Skill_Begin105000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire105000201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult105000201(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire105000201....SkillDef is NIl" ..skillnum)
		return -1
	end
	local iEffectNum = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == false then
		caster:DeleteEffect(iEffectNum)
		return 1
	end
	caster:AddEffect(iEffectNum, 0, arg, caster)
	return 1
end

function Skill_Fail105000201(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(100)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
