
-- Magician (찾았다! : Detection) : Level 1-5
function Effect_Begin102000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End102000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick102000301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102000301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

