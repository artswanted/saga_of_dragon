
-- 메이지 (부활 : a_Resurrection) : Level 1-5
function Skill_Begin109001001(caster, skillnum, iStatus, arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire109001001(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult109001001(caster, kTargetArray, skillnum, result)
	--InfoLog(9, "Skill_Fire109001001 --")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		--InfoLog(5, "Skill_Fire109001001 : Cannot Get SkillDef:" .. skillnum)
		return -1
	end
	local iHPRate = kSkillDef:GetAbil(AT_R_MAX_HP)
	local iMPRate = kSkillDef:GetAbil(AT_R_MAX_MP)
	--InfoLog(9, "Skill_Fire109001001 HPRate:" .. iHPRate .. ", MPRate:" .. iMPRate)
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)

	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), false)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			-- 죽은 아군만
			--InfoLog(9, "Skill_Fire109001001 50")
			if target:GetAbil(AT_HP) <= 0 and target:IsEnemy(caster, false) == false then
				--InfoLog(9, "Skill_Fire109001001 51")
				local iHP = target:GetAbil(AT_C_MAX_HP) * iHPRate / ABILITY_RATE_VALUE
				-- MP는 현재값보다, 새로운 값이 더 클때만 올려 준다.
				local iMP = target:GetAbil(AT_C_MAX_MP) * iMPRate / ABILITY_RATE_VALUE
				local iCurMP = target:GetAbil(AT_MP)
				if iCurMP > iMP then
					--InfoLog(9, "Skill_Fire109001001 60")
					iMP = iCurMP
				end
				--InfoLog(9, "Skill_Fire109001001 61")
				--target:Alive(4, E_SENDTYPE_SELF, iHP, iMP)
				target:SetAbil(AT_REVIVED_BY_OTHER, 1)
				target:SetAbil(AT_HP_RESERVED, iHP)
				target:SetAbil(AT_MP_RESERVED, iMP)
				--InfoLog(9, "Skill_Fire109001001 62")
			end
		end
		
		--InfoLog(9, "Skill_Fire109001001 70")
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	--InfoLog(9, "Skill_Fire109001001 80")
	return iIndex	
end

function Skill_Fail109001001(caster, kTargetArray, skillnum, result, arg)
	return false
end
