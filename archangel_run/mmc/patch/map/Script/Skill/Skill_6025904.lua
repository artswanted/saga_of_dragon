
-- 포키왕 벌룬 자코 소환하기 : a_mon_shot_skill_03
function Skill_Begin6025904(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6025904(caster, kTargetArray, skillnum, result, actarg)
	local kGround = Ground(actarg:Get(ACTARG_GROUND))
	if kGround:IsNil() then
		InfoLog(5, "Skill_Fire6025904....Ground Is Nil : " .. skillnum )
		return -1
	end
	if false == kGround:CallSummonMonster(caster,skillnum) then
		InfoLog(5, "Skill_Fire6025904....Failed : " .. skillnum )
		return -1
	end
	return 0
end

function Skill_Fail6025904(caster, kTargetArray, skillnum, result, arg)
	InfoLog(5, "Skill_Fail6025904.... " ..skillnum)
	return false
end


function Skill_CanReserve6025904(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

