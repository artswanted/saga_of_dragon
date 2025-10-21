
-- WarMage ( 리스소 컨버터 : a_Resource Converter ) : Level 1-10
function Skill_Begin109002101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109002101(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire109002101....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iNeedHP = kSkillDef:GetAbil(AT_NEED_HP);
	local iValue = iNeedHP * kSkillDef:GetAbil(AT_PERCENTAGE) / ABILITY_RATE_VALUE;
	
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
				InfoLog(9, "Skill_Fire109002101....Need Hp : " ..iNeedHP.. " Value :"..iValue)	
			end
			
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail109002101(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
