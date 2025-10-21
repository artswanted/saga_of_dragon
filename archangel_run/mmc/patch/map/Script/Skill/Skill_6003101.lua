
-- 몬스터 : 거미 : 독살포 스킬
function Skill_Begin6003101(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6003101(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult6003101(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire6003101....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		target:SetTarget(caster:GetGuid())
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
		end
	return iIndex
end

function Skill_Fail6003101(caster, kTargetArray, skillnum, result, arg)
	return false
end

function Skill_CanReserve6003101(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end
