
-- 암살자 (기믈렛: a_Gimmlet) : Lv 1-10
function Skill_Begin104301101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104301101(caster, kTargetArray, skillnum, result, arg)
	-- a_Burrow 사용 상태일 때만 사용할 수 있다.
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(GetSkillNoFromActionName("a_Burrow"), true)
	if kEffect:IsNil() == true then
		InfoLog(9, "Skill_Fire104301101 cannot find a_Burrow")
		return 0
	end	
	InfoLog(9, "Skill_Fire104301101 20")
	
	local bReturn = GetSkillResult104301101(caster, kTargetArray, skillnum, result)
	
	InfoLog(9, "Skill_Fire104301101 30")
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	InfoLog(9, "Skill_Fire104301101 31")
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire104301101 32")
		local aresult = result:GetResult(target:GetGuid())
		InfoLog(9, "Skill_Fire104301101 33")
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			InfoLog(9, "Skill_Fire104301101 34")
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail104301101(caster, kTargetArray, skillnum, result, arg)
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
