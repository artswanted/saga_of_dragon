
-- 투사 (제네식기어-공격 : Genesic Gear - Offense ) : Level 1-10
function Skill_Begin106000201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106000201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult106000201(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire106000201....SkillDef is NIl" ..skillnum)
		return -1
	end
	local iEffectNum = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNum, true)
	if kEffect:IsNil() == false then
		caster:DeleteEffect(iEffectNum)
		return 1
	end
	caster:AddEffect(iEffectNum, 0, arg, caster)
	return 1
end

function Skill_Fail106000201(caster, kTargetArray, skillnum, result, arg)
	return false
end
