
-- Paladin (크로스 컷 : a_Cross Cut) : Level 1-5
function Effect_Begin106400201(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_Begin107000701...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	
	return 1
end
                                                                                                                          
function Effect_End106400201(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_End105300601...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	
	return 1
end

function Effect_Tick106400201(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick106400201 Effect=" .. effect:GetEffectNo())
	local iDec = effect:GetAbil(AT_HP)
	local iHP = math.max(0, unit:GetAbil(AT_HP)-iDec)
	--unit:SetAbil(AT_HP, iHP, true, true)
	
	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(unit:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iHP)
	kPacket:PushGuid(effect:GetCaster())
	kPacket:PushInt(effect:GetEffectNo())
	unit:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)
	
	unit:SetHP(iHP, E_SENDTYPE_NONE, effect:GetCaster())
	return ECT_DOTICK
end
