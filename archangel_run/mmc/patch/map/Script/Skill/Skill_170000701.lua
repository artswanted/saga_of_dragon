
-- 닌자 ( 격! 일섬!: a_Lightning Slash) : Level 1-10
function Skill_Begin170000701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire170000701(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult170000701(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire170000701....SkillDef is NIl" ..skillnum)
		return -1
	end

	InfoLog(5, "1 ")
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	InfoLog(5, "2 ")
	while target:IsNil() == false do
InfoLog(5, "2-1 ")
		local aresult = result:GetResult(target:GetGuid())
		local iUseShadowCopy = caster:GetAbil(AT_SHADOW_COPY_USE);
		--환영분신이 있을 경우 데미지 4배가 들어간다.
		if iUseShadowCopy ~= 0 then
			InfoLog(5, "iUseShadowCopy : " ..iUseShadowCopy)
			
			aresult:SetValue(aresult:GetValue() * 4);
		end

			InfoLog(5, "2-2")
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			InfoLog(5, "2-3")
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	InfoLog(5, "3 ")

	caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster);
		InfoLog(5, "4 ")

	return iIndex
end

function Skill_Fail170000701(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
