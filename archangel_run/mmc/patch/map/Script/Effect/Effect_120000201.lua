
-- Wizard ( 썬더 브레이크: a_Thunder Break) : Level 1-10
function Effect_Begin120000201(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_Begin120000101...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	-- MAX_HP 증가
	--local iAdd = kEffect:GetValue(AT_MAX_HP)
	--kEffectMgr:AddAbil(AT_MAX_HP, iAdd)
	--unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End120000201(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_End106300601...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	-- MAX_HP 증가
	--local iAdd = kEffect:GetAbil(AT_MAX_HP)
	--kEffectMgr:AddAbil(AT_MAX_HP, 0-iAdd)
	--unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	--local iMaxHP = unit:GetAbil(AT_C_MAX_HP)
	-- 현재HP값이 MAX_HP를 넘지 않도록 조정
	--if unit:GetAbil(AT_HP) > iMaxHP then
	--	unit:SetAbil(AT_HP, iMaxHP, true, false)
	--end
	return 1
end

function Effect_Tick120000201(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick120000201 Effect=" .. effect:GetEffectNo())
	local iSkillNo = effect:GetAbil(AT_NORMAL_SKILL_ID)
	if iSkillNo <= 0 then
		InfoLog(6, "Effect_Tick120000201 Cannot Get AT_NORMAL_SKILL_ID, EffectNo=" .. effect:GetEffectNo())
		return ECT_MUSTDELETE
	end

	local iDelay = effect:GetValue()
	if iDelay > 0 then
		effect:SetValue(iDelay - elapsedtime)
		return ECT_DOTICK
	end
	
	local kNilTarget = Unit()
	local kAResult = NewActionResultVector()
	unit:SkillFire(iSkillNo, kNilTarget, kAResult, actarg, false)

	local iIndex = 0
	local kTargetGuid = GUID()
	local result = kAResult:GetResultIndex(iIndex, kTargetGuid)
	while result:IsNil() == false do
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(kTargetGuid)
		kPacket:PushShort(AT_HP)
		kPacket:PushInt(result:GetRemainHP())
		kPacket:PushGuid(effect:GetCaster())
		kPacket:PushInt(effect:GetEffectNo())
		unit:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)
		
		iIndex = iIndex + 1
		result = kAResult:GetResultIndex(iIndex, kTargetGuid)
	end
	DeleteActionResultVector(kAResult)
	
	if iIndex > 0 then
		-- Delay를 주어 너무 자주 공격하지 않도록 한다.
		effect:SetValue(effect:GetAbil(AT_ATT_DELAY))
	end
	return ECT_DOTICK
end
