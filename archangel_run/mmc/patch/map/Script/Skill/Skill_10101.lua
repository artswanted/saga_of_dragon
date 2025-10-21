-- a_mon_collision_01 : 몬스터 충돌 Damage
function Skill_Begin10101(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire10101(caster, kTargetArray, skillnum, result, arg)
	GetSkillResult10101(caster, kTargetArray, skillnum, result)
	
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			WA_ATTACKDAMAGE_UNIT(0,0,aresult:GetValue(),caster,target,kGround)
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail10101(caster, kTargetArray, skillnum, result, arg)
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

