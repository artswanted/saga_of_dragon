
-- 데스마스터 기모았다 발산
function Skill_Begin6009602(caster, skillnum, iStatus,arg, TailPacket)
	kTargetGuid = caster:GetTarget()
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	kTargetPos = kGround:GetUnitPosByGuid(kTargetGuid)
	if kTargetPos:GetX() == 0 and kTargetPos:GetY() == 0 and kTargetPos:GetZ() == 0 then
		kTargetPos = caster:GetGoalPos()
	end
	kTargetPos:SetZ(kTargetPos:GetZ()+20)		
	TailPacket:PushPoint3(kTargetPos)	-- GoalPos 값 client로 전송
	caster:SetGoalPos(kTargetPos)

	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6009602(caster, kTargetArray, skillnum, result, actarg)
	local bReturn = GetSkillResult6009602(caster, kTargetArray, skillnum, result)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(8, "Skill_Fire1  10")
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			--InfoLog(8, "Skill_Fire1  Damaged:" .. aresult:GetValue())
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, actarg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail6009602(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6009602(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

