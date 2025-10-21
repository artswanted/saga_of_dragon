-- [return] : int
function Skill_BeginDefault(caster, skillnum, iStatus,arg)
	--[[
	local myskill = caster:GetSkill()
	local iNeedHP = myskill:GetOtherAbil(skillnum, AT_NEED_HP)
	local iNeedMP = myskill:GetOtherAbil(skillnum, AT_NEED_MP)
	local iHP = caster:GetAbil(AT_HP)
	local iMP = caster:GetAbil(AT_MP)
	
	if iNeedHP > 0 then
		if iHP >= iNeedHP then
			caster:SetAbil(AT_HP, iHP-iNeedHP, false)
			caster:SendAbil(AT_HP)
		else
			return 201
		end
	end

	if iNeedMP > 0 then
		if iMP >= iNeedMP then
			caster:SetAbil(AT_MP, iMP-iNeedMP, false)
			caster:SendAbil(AT_MP)
		else
			return 202
		end
	end

	myskill = caster:GetSkill()
	if myskill:Fire(skillnum, iStatus) then
		return 1	-- success
	end
	return 203
	]]
	
	return 1

end

---++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++---
function DoFinalDamage(caster, target, iDamage, iSkill, act)
	local TargetDie = false

	local kGround = Ground(act:Get(ACTARG_GROUND))
	local iHP = target:OnDamaged(iDamage, kGround, caster, iSkill)
	if iHP <= 0 then		-- target died
		TargetDie = true
	end

	return TargetDie
end

function DoTickDamage(caster, target, iDamage, iFireEffectNo, iAddEffect, actarg)
		-- 실제 데미지를 주고
		DoFinalDamage(caster, target, iDamage, 0, actarg);

		if iAddEffect ~= 0 then
			target:AddEffect(iAddEffect, 0, actarg, caster);
		end

		-- 클라이언트에 데미지를 알려준다.
		local iHP = target:GetAbil(AT_HP);
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(target:GetGuid())
		kPacket:PushShort(AT_HP)
		kPacket:PushInt(iHP)
		kPacket:PushGuid(caster:GetGuid())
		kPacket:PushInt(iFireEffectNo)
		caster:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)
end


function OnAttacked(caster, kTargetArray, iSkill, result, actarg)
	
	-- Caster는 앞으로 약간 이동
	-- Target은 뒤로 약간 이동
	--if iDamage > 0 then	-- 일단은 Damage 값으로, 클라이언트와 맞추어 다른 조건으로 변경될 수도 있다.
	local kSkillDef = GetSkillDef(skillno)
	local iCasterLen = kSkillDef:GetAbil(AT_CASTER_MOVERANGE)
	local iTargetLen = kSkillDef:GetAbil(AT_TARGET_MOVERANGE)
		
	--InfoLog(9, "OnAttacked, Caster:" .. iCasterLen .. ", Target:" .. iTargetLen)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		-- RemainHP 값 세팅
		aresult:SetRemainHP(target:GetAbil(AT_HP))
		if target:GetAbil(AT_MAX_DP) > 0 then
			aresult:SetAbil(AT_DP)
		end

		--InfoLog(9, "OnAttacked RemainHP:" ..target:GetAbil(AT_HP))
		-- 일단은 Damage>0이면, Target 밀려나도록
		if aresult:GetInvalid() == false and aresult:GetValue() > 0 then
			target:MoveTo(caster:GetPos(), 0-iTargetLen)
		end
		if iIndex == 0 then	-- caster는 처음 한번만 전진한다.
			caster:MoveTo(target:GetPos(), iCasterLen)
		end
		
		-- Damage Reflect 값이 있다면 여기서 HP값을 줄여준다.
		local iReflect = aresult:GetAbil(AT_REFLECTED_DAMAGE)
		if iReflect > 0 then
			local kGround = Ground(actarg:Get(ACTARG_GROUND))
			caster:OnDamaged(iReflect, kGround, target, iSkill)			
		--	InfoLog(9, "AT_REFLECTED_DAMAGE Reflected Damage:" .. iReflect .. ", iHP:" .. aresult:GetAbil(AT_REFLECT_DMG_HP))
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
end
	

-- 밑의 것은 임시이다. 보스 몬스터 데미지에 대한 것을 결론지으면 수정하든 삭제하든 해야한다.
function SendBossDamageResult(caster,target,iDamage,arg)
	local bBossMonster = target:IsUnitType(UT_BOSSMONSTER)

	if bBossMonster == false then
		return
	end
	
	local	packet = NewPacket(PT_M_C_NFY_DAMAGE_RESULT)
	packet:PushGuid(caster:GetGuid())
	packet:PushGuid(target:GetGuid())
	packet:PushInt(iDamage)
	packet:PushInt(target:GetAbil(AT_HP))
	packet:PushInt(target:GetState())
	packet:PushInt(0)
	packet:PushInt(0)
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	kGround:Broadcast(packet,target,caster:GetGuid())

	DeletePacket(packet)
	
	return
end

--[[
-- 사용안하는 함수
function Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg)
	local kGround = Ground(actarg:Get(ACTARG_GROUND))
	local iRangeType = kSkillDef:GetAbil(AT_RANGE_TYPE)
	local iRange = 0
	if (iRangeType == 0) or (iRangeType == 1) then -- Skill Ranage
		iRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
		--InfoLog(9, "Skill_CheckRange Range" .. iRange)
	elseif iRangeType == 2 then -- Item Range
		--InfoLog(9, "Skill_CheckRange 12")
		-- Not implemented
		--InfoLog(5, "[lua:Skill_CheckRange] Cannot Get Skill Ranage, Skill[" .. kSkillDef:GetSkillNo() .. "], RangeType[2]")
		iRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
	elseif iRangeType == 3 then	--Skill + Item
		--InfoLog(9, "Skill_CheckRange 13")
		iRange = kSkillDef:GetAbil(AT_ATTACK_RANGE) + 0
	else -- iRangeType == 4 (Unit)
		--InfoLog(9, "Skill_CheckRange 14")
		iRange = caster:GetAbil(AT_C_ATTACK_RANGE)
	end
	
	if iRange == 0 then
		InfoLog(5, "[lua:Skill_CheckRange] Range is ZERO, Skill[" .. kSkillDef:GetSkillNo() .. "], RangeType[" .. iRangeType .. "]")
	end
	--return IsInRange(caster:GetPos(), target:GetPos(), iRange, iRange)
	
	local iMinRange = kSkillDef:GetAbil(AT_SKILL_MIN_RANGE)
	local iCount = 0
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(9, "Skill_CheckRange .. MinRange:" .. iMinRange .. ", Range:" .. iRange)
		if IsBetweenRange(kGround:GetUnitPos(caster), kGround:GetUnitPos(target), iMinRange, iRange) == true then
			iCount = iCount + 1
		--else
			--kTargetArray:DeleteUnit(target)
			--iIndex = iIndex - 1
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	if iCount == 0 then
		return false
	end
	return true
end
]]

-- caster : 원래 공격자, 즉 Reflect Damage 맞은 unit
-- target : 원래 Target, 즉 Reflect Damage를 발생시킨 unit
function DoReflectDamage(caster, target, iDamage, iSkill, act)
	local kGround = Ground(act:Get(ACTARG_GROUND))
	return caster:OnDamaged(iDamage, kGround, target, iSkill)
	-- 맞고 나서 남은 HP 값 return
end

function Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
	--[[
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	local iMinRange = kSkillDef:GetAbil(AT_SKILL_MIN_RANGE)
	if iMinRange <= 0 then
		return true
	end
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local iCount = 0
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		if IsInRange(kGround:GetUnitPos(caster), kGround:GetUnitPos(target), iMinRange, iMinRange) == false then
			iCount = iCount + 1
		else
			kTargetArray:DeleteUnit(target)
			iIndex = iIndex - 1
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end	
	
	if iCount == 0 then
		return false
	end
	]]

	
	return true
end
