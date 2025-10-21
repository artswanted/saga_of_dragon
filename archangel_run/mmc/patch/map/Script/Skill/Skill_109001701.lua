
-- Wizard ( MP : a_MP Transition) : Level 1-5
function Skill_Begin109001701(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109001701(caster, kTargetArray, skillnum, result, arg)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire109001701....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iNeedMP = kSkillDef:GetAbil(AT_NEED_MP);
	local iValue = iNeedMP * kSkillDef:GetAbil(AT_PERCENTAGE) / ABILITY_RATE_VALUE;
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		if target:IsUnitType(UT_PLAYER) == true then
			local aresult = result:GetResult(target:GetGuid(), true)
			if aresult:IsNil() == false and aresult:GetInvalid() == false then

				local iMaxMP = target:GetAbil(AT_C_MAX_MP)
				local iMP = target:GetAbil(AT_MP)
				if iMP == iMaxMP then
					return 0
				end
					
				local iNewMP = math.min(iMaxMP, iMP+iValue)
	
				target:SetMP(iNewMP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL, caster:GetGuid());
				InfoLog(9, "Skill_Fire109001701....Need Mp : " ..iNeedMP.. " Value :"..iValue)	
			end
			
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail109001701(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
