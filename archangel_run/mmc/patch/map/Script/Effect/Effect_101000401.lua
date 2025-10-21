
-- Fighter (망각 : Forget Pain) : Level 1-5
function Effect_Begin101000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	kEffectMgr:AddAbil(AT_DAMAGEACTION_TYPE, 101)
	unit:NftChangedAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End101000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	kEffectMgr:AddAbil(AT_DAMAGEACTION_TYPE, -101)
	unit:NftChangedAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick101000401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101000401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

