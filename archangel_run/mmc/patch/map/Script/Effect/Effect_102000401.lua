-- Magician (바바이안! : Burberrian) : Level 1-5
function Effect_Begin102000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--unit:SetState(US_FROZEN)
	local iSpeed = unit:GetAbil(AT_C_MOVESPEED)
	kEffectMgr:AddAbil(AT_MOVESPEED, 0-iSpeed)
	kEffect:SetValue(iSpeed)
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADCAST+E_SENDTYPE_EFFECTABIL)
	unit:SetAbil(AT_FROZEN, unit:GetAbil(AT_FROZEN)+1, false)
	return 1
end

function Effect_End102000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end	
	--unit:SetState(US_IDLE)
	local iSpeed = kEffect:GetValue()
	if iSpeed > 0 then
		kEffectMgr:AddAbil(AT_MOVESPEED, iSpeed)
		unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADCAST+E_SENDTYPE_EFFECTABIL)
	end
	unit:SetAbil(AT_FROZEN, unit:GetAbil(AT_FROZEN)-1, false)
	return 1
end

function Effect_Tick102000401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102000401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
