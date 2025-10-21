
-- 데스마스터 대지의 분노 : a_mon_earthquake
function Skill_Begin6009604(caster, skillnum, iStatus,arg, TailPacket)
	--local kTargetGuid = caster:GetTarget()
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	local kTarget = kGround:GetUnit(caster:GetTarget())
	if kTarget:IsNil() == true then
		InfoLog(9, "Skill_Begin6009604 19")
		return -1
	end
	local kTargetPos = kTarget:GetPos()
	local fDistance = 260
	local kTargetPos = kGround:GetUnitFrontPos(caster, fDistance)	-- 발사체 날가갈 위치
	caster:SetGoalPos(kTargetPos)

	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6009604(caster, kTargetArray, skillnum, result, actarg)
	local bReturn = GetSkillResult6009604(caster, kTargetArray, skillnum, result)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, actarg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail6009604(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6009604(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

