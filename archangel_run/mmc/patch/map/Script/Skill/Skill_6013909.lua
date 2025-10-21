
-- a_mon_shot_01 : 알비다 불가사리 비 소환
function Skill_Begin6013909(caster, skillnum, iStatus,arg, TailPacket)
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

function Skill_Fire6013909(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult6003101(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire6013909....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
		end
	return iIndex
end

function Skill_Fail6013909(caster, kTargetArray, skillnum, result, arg)
	return false
end

function Skill_CanReserve6013909(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end
