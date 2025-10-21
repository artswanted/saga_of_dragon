
-- 돌격하기
function Skill_Begin6004201(caster, skillnum, iStatus,arg, TailPacket)
	--local kTargetGuid = caster:GetTarget()
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	local kTarget = kGround:GetUnit(caster:GetTarget())
	if kTarget:IsNil() == true then
		return -1
	end
	local kTargetPos = kTarget:GetPos()
	local fDistance = GetDistance(caster:GetPos(), kTarget:GetPos())
	local Dir = kTargetPos:GetDirectionVector(kTarget:GetPos(), caster:GetPos())
	-- 돌격 거리 계산 하기
	--if fDistance < 60 then -- 60(Minimum값)
--		fDistance = 60
	--else
	--	fDistance = fDistance + 100
--	end
--	fDistance = fDistance - 20
	local range = 20
	local kSkill = caster:GetSkill()
	if kSkill:IsNil() == false then
		local abil = kSkill:GetAbil(544)
		if abil > 0 then
			range = abil -1
		end
	end
	--InfoLog(5, "Skill_Begin6004201 Origin Dist : ".. fDistance .. " No : " .. skillnum .. " Range  : " .. range)
	if fDistance >= range then
		fDistance = fDistance - range -1 --5 - caster:GetAbil(AT_UNIT_SIZE)*3
	--	Dir = Dir:Multiply(fDistance)
	--	kTargetPos = caster:GetPos():Add(Dir)
	end
	--InfoLog(5, "Skill_Begin6004201 Dist : ".. fDistance)
	kTargetPos = kGround:GetUnitFrontPos(caster, fDistance)	-- 돌진할 위치
	-- 돌진이 가능한 거리 인지 검사
 	-- 3 = EFlags_MinDistance|EFlags_SetGoalPos
	if fDistance <= 5 or kGround:SetValidGoalPos(caster, kTargetPos, 20, 30, 3) == false then
		-- 유효한 위치가 아니므로 취소
		InfoLog(5, "Skill_Begin6004201 Cannot Dash")
		kTargetPos = kTarget:GetPos()
		-- Z축을 약간만 올리자
		kTargetPos:SetZ(kTargetPos:GetZ()+20)		
		TailPacket:PushPoint3(kTargetPos)
		return -1
	end
	-- GoalPos 세팅은 SetValidGoalPos() 안에서 처리 됨.
--	kTargetPos = caster:GetGoalPos()
	kTargetPos:SetZ(kTargetPos:GetZ()+20)		
	TailPacket:PushPoint3(kTargetPos)	-- GoalPos 값 client로 전송

	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6004201(caster, kTargetArray, skillnum, result, actarg)
	local bReturn = GetSkillResult6004201(caster, kTargetArray, skillnum, result)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			--InfoLog(8, "Skill_Fire1  Damaged:" .. aresult:GetValue())
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, actarg)
			InfoLog(5, "Skill_Fire6004201 -- ".. iIndex)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end

	return iIndex
end

function Skill_Fail6004201(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6004201(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

