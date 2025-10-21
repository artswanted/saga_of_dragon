
-- 트랩퍼 ( 관통: a_Penetration) : Level 1-10 PASSIVE
function Effect_Begin140000101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin140000101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--명중률 증가
	--local iAdd = kEffect:GetAbil(AT_PENETRATION_RATE)
	--kEffectMgr:AddAbil(AT_PENETRATION_RATE, iAdd)
	--unit:NftChangedAbil(AT_PENETRATION_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	local iAdd = kEffect:GetAbil(AT_COUNT);
	kEffectMgr:AddAbil(AT_PENETRATION_COUNT, iAdd);
	unit:NftChangedAbil(AT_PENETRATION_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	InfoLog(5, "Effect_Begin140000101 AT_COUNT : "..AT_COUNT.."   AT_PENETRATION_COUNT : ".. AT_PENETRATION_COUNT);


	return 1
end
                                                                                                                          
function Effect_End140000101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End140000101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--명중률 증가
	--local iAdd = kEffect:GetAbil(AT_PENETRATION_RATE)
	--kEffectMgr:AddAbil(AT_PENETRATION_RATE, 0-iAdd)
	--unit:NftChangedAbil(AT_PENETRATION_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	local iAdd = kEffect:GetAbil(AT_COUNT);
	kEffectMgr:AddAbil(AT_PENETRATION_COUNT, 0-iAdd);
	unit:NftChangedAbil(AT_PENETRATION_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);

	return 1
end

function Effect_Tick140000101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick140000101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
