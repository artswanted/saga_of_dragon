
--  메이지(MP회복력 향상 : MP Recharge) : Level 1-10 PASSIVE
function Effect_Begin109000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MP 회복량 향상
	local iAdd = kEffect:GetAbil(AT_MP_RECOVERY)
	kEffectMgr:AddAbil(AT_MP_RECOVERY, iAdd)
	unit:NftChangedAbil(AT_MP_RECOVERY, E_SENDTYPE_NONE)
	return 1
end

function Effect_End109000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- MP 회복량 향상
	local iAdd = kEffect:GetAbil(AT_MP_RECOVERY)
	kEffectMgr:AddAbil(AT_MP_RECOVERY, 0-iAdd)
	unit:NftChangedAbil(AT_MP_RECOVERY, E_SENDTYPE_NONE)
	return 1
end

function Effect_Tick109000301(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end
