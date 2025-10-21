
-- Thief (보고도 못 본척 : Shadow Walk) : Level 1
function Effect_Begin104000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")")
		return 0
	end

	-- Heal Recovery Amount
	kEffectMgr:AddAbil(AT_HP_RECOVERY, kEffect:GetAbil(AT_HP_RECOVERY))
	unit:NftChangedAbil(AT_HP_RECOVERY, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	-- No Damageed
	unit:AddAbil(AT_CANNOT_DAMAGE, 1)
	unit:AddAbil(AT_CANNOT_ATTACK, 1)
	return 1
end

function Effect_End104000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104000201...Cannot GetEffect(" .. iEffectNo ..")")
		return 0
	end

	-- Heal Recovery Amount
	kEffectMgr:AddAbil(AT_HP_RECOVERY, 0-kEffect:GetAbil(AT_HP_RECOVERY))
	unit:NftChangedAbil(AT_HP_RECOVERY, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	-- Cancel : No Damage
	unit:AddAbil(AT_CANNOT_DAMAGE, -1)
	unit:AddAbil(AT_CANNOT_ATTACK, -1)
	return 1
end

function Effect_Tick104000201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104000201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

