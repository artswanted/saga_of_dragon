
-- 매지션 (마나실드 : a_Mana Shield ) : Level 1-5 : Toggle
function Skill_Begin102000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000701(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult102000701(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
		
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire102000701....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	
	-- 토글스킬일때 ----------------
	--[[
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == false then
		caster:DeleteEffect(iEffectNo)
	else
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
	end
	]]
	return 1
end

function Skill_Fail102000701(caster, kTargetArray, skillnum, result, arg)
	return false
end
