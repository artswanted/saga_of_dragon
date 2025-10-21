
-- 메이지 (큐어 : a_Cure) : Level 1-5
function Skill_Begin107000501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire107000501(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult107000501(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire107000501....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iDelete = kSkillDef:GetAbil(AT_COUNT) -- 지워야 할 Effect 개수
	InfoLog(9, "Skill_Fire107000501 DEL_COUNT:" .. iDelete)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	InfoLog(9, "Skill_Fire107000501 29")
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire107000501 30")
		local aresult = result:GetResult(target:GetGuid(), true)
		InfoLog(9, "Skill_Fire107000501 31")
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			local iNum = 0	-- Effect Index
			local iCount = 0 -- Deleted Effect counter
			InfoLog(9, "Skill_Fire107000501 32")
			local kEffectMgr = caster:GetEffectMgr()
			InfoLog(9, "Skill_Fire107000501 33")
			local bEnemy = caster:IsEnemy(target, false)
			InfoLog(9, "Skill_Fire107000501 34:")
			while iCount < iDelete do
				InfoLog(9, "Skill_Fire107000501 41")
				local kEffect = kEffectMgr:GetEffectIndex(iNum)
				if kEffect:IsNil() == true then
					InfoLog(9, "Skill_Fire107000501 42")
					break
				end
				local iBuffType = kEffect:GetAbil(AT_TYPE)
				InfoLog(9, "Skill_Fire107000501 EffectType:" .. iBuffType)
				if (bEnemy == true) and (iBuffType == EFFECT_TYPE_BLESSED) then
					InfoLog(9, "Skill_Fire107000501 44")
					target:DeleteEffect(kEffect:GetEffectNo())
					iNum = 0
					iCount = iCount + 1
				elseif (bEnemy == false) and (iBuffType == EFFECT_TYPE_CURSED) then
					InfoLog(9, "Skill_Fire107000501 45")
					target:DeleteEffect(kEffect:GetEffectNo())
					iNum = 0
					iCount = iCount + 1
				else
					InfoLog(9, "Skill_Fire107000501 46")
					iNum = iNum + 1
				end
				
				InfoLog(9, "Skill_Fire107000501 iNum:" .. iNum)
			end	-- while
		end -- if
		
		InfoLog(9, "Skill_Fire107000501 50")
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail107000601(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end
