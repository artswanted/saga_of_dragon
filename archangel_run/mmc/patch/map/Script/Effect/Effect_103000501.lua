
-- Archer (경쾌한 발놀림 : Wind Step) : Level 1-10
function Effect_Begin103000501(unit, iEffectNo, actarg)
	InfoLog(9, "Effect_Begin103000501 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin103000501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	kEffectMgr:AddAbil(AT_MOVESPEED, kEffect:GetAbil(AT_MOVESPEED))
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End103000501(unit, iEffectNo, actarg)
	InfoLog(9, "Effect_End103000501 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End103000501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	kEffectMgr:AddAbil(AT_MOVESPEED, 0-kEffect:GetAbil(AT_MOVESPEED))
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick103000501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick103000501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

