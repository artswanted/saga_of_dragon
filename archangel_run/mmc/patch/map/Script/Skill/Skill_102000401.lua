
-- Magician (바바리안! : Burberrian) : Level 1-5
function Skill_Begin102000401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000401(caster, kTargetArray, skillnum, result, arg)
	--InfoLog(8, "Skill_Fire102000401 --")
	--GetSkillResult102000401(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire102000401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	InfoLog(8, "Skill_Fire102000401 EffectPer:" .. iEffectPer)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--	Boss는 안걸리게 한다
		if false == target:IsUnitType(UT_BOSSMONSTER) then
			local iRandValue = RAND(100)
			local aresult = result:GetResult(target:GetGuid(), true)
			if iRandValue < iEffectPer and aresult:GetInvalid() == false then
				-- 경직시키기
				InfoLog(8, "Skill_Fire102000401 AddEffect")
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
			else
				aresult:SetMissed(true)
			end
		end
			iIndex = iIndex + 1
			target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail102000401(caster, kTargetArray, skillnum, result, arg)
	return false
end
