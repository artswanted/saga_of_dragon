
-- 삼바바 점프해서 바닥 찍기
function Skill_Begin6015903(caster, skillnum, iStatus,arg, TailPacket)
	local kTargetGuid = caster:GetTarget()
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	local kTarget = kGround:GetUnit(kTargetGuid)
	if kTarget:IsNil() == true then
		return -1
	end
	local kPos = kTarget:GetPos()
	-- Z축을 약간만 올리자
	caster:SetGoalPos(kPos)
	kPos:SetZ(kPos:GetZ()+20)
	TailPacket:PushPoint3(kPos)
	InfoLog(9, "Skill_Begin6015903 TargetPos[" .. kPos:GetX() .. ", " .. kPos:GetY() .. ", " .. kPos:GetZ() .. ")")

	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6015903(caster, kTargetArray, skillnum, result, actarg)
	--InfoLog(8, "Skill_Fire1 --")
	local bReturn = GetSkillResult6015903(caster, kTargetArray, skillnum, result)
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

function Skill_Fail6015903(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6015903(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

