
-- 타겟 링크 : a_mon_melee_04
function Skill_Begin142(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire142(caster, kTargetArray, skillnum, result, actarg)
	-- GetSkillResult142(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire142....SkillDef is NIl : " ..skillnum)
		return -1
	end

	local target = kTargetArray:GetUnit(0)
	if target:IsNil() then
		InfoLog(5, "Skill_Fire142....Target is NIl : " ..skillnum)
		return -1
	end

	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	if kGround:IsNil() then
		return -1
	end

	local kPos = caster:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local Range = kSkillDef:GetAbil(AT_2ND_AREA_PARAM2)
	
	kGround:GetUnitInRangeZ(kPos, Range, 30, UT_MONSTER, kUnitArray)

	local iEffectNo = kSkillDef:GetEffectNo()
	local iIndex = 0
	local slave = kUnitArray:GetUnit(iIndex)

	if slave:IsNil() then	--주변에 잡몹이 없음
		InfoLog(5, "Skill_Fire142....Slave is NIl : " ..skillnum)
		return -1
	end

	local EffectID = kSkillDef:GetEffectNo()
	while slave:IsNil() == false do
		if false == slave:GetGuid():IsEqual(caster:GetGuid()) then
			slave:AddEffect(EffectID, 0, actarg, slave)
			slave:SetTarget(target:GetGuid())
			InfoLog(5, "Skill_Fire142....EffectID is : " ..EffectID .. " Unit Type : " .. slave:GetUnitType())
		end
		iIndex = iIndex + 1
		slave = kUnitArray:GetUnit(iIndex)
	end

	DeleteUNIT_PTR_ARRAY(kUnitArray)

	InfoLog(5, "Skill_Fire142....Monster Num is : " ..iIndex)
	return iIndex
end

function Skill_Fail142(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve142(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

