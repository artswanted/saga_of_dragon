
-- a_magician_charge_laser : 법사 응집공격
function Effect_Begin102101201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102101201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iSpeed = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffect:SetValue(iSpeed)
	-- 이동속도를 떨군다.
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iSpeed)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADCAST+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End102101201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102101201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iSpeed = kEffect:GetValue()
	-- 이동속도를 떨군다.
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iSpeed)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADCAST+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick102101201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102101201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

