
-- 공통 : 과다출혈
function Effect_Begin100010101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MaxHP 감소
	local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
	kEffectMgr:AddAbil(AT_R_MAX_HP, iAdd)
	unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end

function Effect_End100010101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100010101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MaxHP 감소
	local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
	kEffectMgr:AddAbil(AT_R_MAX_HP, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1	
end

function Effect_Tick100010101(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick100010101 Effect=" .. effect:GetEffectNo())
	local iAdd = effect:GetAbil(AT_HP)
	local iHP = unit:GetAbil(AT_HP)
	local iNewHP = math.max(0, iHP+iAdd)
	
	if iNewHP ~= iHP then
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(unit:GetGuid())
		kPacket:PushShort(AT_HP)
		kPacket:PushInt(iNewHP)
		kPacket:PushGuid(effect:GetCaster())
		kPacket:PushInt(effect:GetEffectNo())
		unit:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)	
	
		--unit:SetAbil(AT_HP, iNewHP)
		unit:SetHP(iNewHP, E_SENDTYPE_NONE, effect:GetCaster())
	end
	return ECT_DOTICK	
end

