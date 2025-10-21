
-- 사냥꾼 (애시드 애로우 : a_Acid Arrow) : Level 1-5
function Effect_Begin110001001(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_Begin110001001...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	-- 회피율 증가
	--local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	--kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	--unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End110001001(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_End110000901...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	-- 회피율 증가
	--local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	--kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	--unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick110001001(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick110001001 Effect=" .. effect:GetEffectNo())
	-- HP감소
	local iDec = effect:GetAbil(AT_HP)
	local iHP = unit:GetAbil(AT_HP)
	local iMin = math.max(0, iHP+iDec)
	--unit:SetAbil(AT_HP, iMin)
	InfoLog(9, "Effect_Tick110001001 Dec:" .. iDec .. ", CurHP:" .. iMin)

	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(unit:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iMin)
	kPacket:PushGuid(effect:GetCaster())
	kPacket:PushInt(effect:GetEffectNo())
	unit:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)
	
	unit:SetHP(iMin, E_SENDTYPE_NONE, effect:GetCaster())
	
	return ECT_DOTICK
end
