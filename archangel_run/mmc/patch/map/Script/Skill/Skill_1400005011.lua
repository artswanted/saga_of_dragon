
-- Trapper ( 마나 번 작동: ) : Level 1-10
function Skill_Begin1400005011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1400005011(caster, kTargetArray, skillnum, result, arg)
	local bReturn = GetSkillResult1400005011(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1400005011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iMPBurnRate = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			-- MP소모
			local iMP = target:GetAbil(AT_MP)
			local iDecMP = iMP * iMPBurnRate * ABILITY_RATE_VALUE
			local iNewMP = math.max(0, iMP - iDecMP)
			if iNewMP ~= iMP then
				local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
				kPacket:PushGuid(target:GetGuid())
				kPacket:PushShort(AT_MP)
				kPacket:PushInt(iNewMP)
				kPacket:PushGuid(caster:GetGuid())
				kPacket:PushInt(skillnum)
				unit:Send(kPacket, E_SENDTYPE_BROADALL)
				DeletePacket(kPacket)	
			end
			target:SetAbil(AT_MP, iNewMP, true, false)
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	-- Trap이므로, 터지고 나면 사라져야 한다.
	--caster:SetAbil(AT_HP, 0)
		
	return iIndex
end

function Skill_Fail1400005011(caster, kTargetArray, skillnum, result, arg)
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

function Skill_CanFire1400005011(caster, kTargetArray, skillnum, actarg)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		InfoLog(9, "Skill_CanFire1400005011 return false")
		return false
	end
	
	return true
end

function Skill_CanReserve1400005011(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end
