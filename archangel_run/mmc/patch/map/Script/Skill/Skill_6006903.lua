
-- 마할카 휠윈드 전진하기 : a_mon_whirlwind_move
function Skill_Begin6006903(caster, skillnum, iStatus,arg, TailPacket)
	InfoLog(9, "Skill_Begin6006903 --")
	--local kTargetGuid = caster:GetTarget()
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	InfoLog(9, "Skill_Begin6006903 11")
	local kTarget = kGround:GetUnit(caster:GetTarget())
	if kTarget:IsNil() == true then
		InfoLog(9, "Skill_Begin6006903 19")
		return -1
	end
	local kTargetPos = kTarget:GetPos()
	--local fDistance = GetDistance(caster:GetPos(), kTarget:GetPos())
	-- 돌격 거리 계산 하기
	--if fDistance < 60 then -- 60(Minimum값)
	--	fDistance = 60
	--else
	--	fDistance = fDistance + 100
	--end
	local fDistance = 200
	local kTargetPos = kGround:GetUnitFrontPos(caster, fDistance)	-- 돌진할 위치
	-- 돌진이 가능한 거리 인지 검사
 	-- 3 = EFlags_MinDistance|EFlags_SetGoalPos
	if kGround:SetValidGoalPos(caster, kTargetPos, 20, 30, 3) == false then
		-- 유효한 위치가 아니므로 취소
		InfoLog(9, "Skill_Begin6004904 Cannot Dash")
		kTargetPos = kTarget:GetPos()
		-- Z축 살짝 올려준다.
		kTargetPos:SetZ(kTargetPos:GetZ()+20)
		TailPacket:PushPoint3(kTargetPos)
		return -1
	end
	-- GoalPos 세팅은 SetValidGoalPos() 안에서 처리 됨.
	kTargetPos = caster:GetGoalPos()
	-- Z축 살짝 올려준다.
	kTargetPos:SetZ(kTargetPos:GetZ()+20)
	TailPacket:PushPoint3(kTargetPos)	-- GoalPos 값 client로 전송

	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6006903(caster, kTargetArray, skillnum, result, actarg)
	--InfoLog(8, "Skill_Fire1 --")
	local bReturn = GetSkillResult6006903(caster, kTargetArray, skillnum, result)
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

function Skill_Fail6006903(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6006903(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

