
-- 트랩퍼 ( 컨센트레이트: a_Concentration) : Level 1-10 PASSIVE
function Effect_Begin140000001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin140000001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--명중률 증가
	local iAdd = kEffect:GetAbil(AT_HITRATE)
	kEffectMgr:AddAbil(AT_HITRATE, iAdd)
	unit:NftChangedAbil(AT_HITRATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End140000001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End140000001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--명중률 증가
	local iAdd = kEffect:GetAbil(AT_HITRATE)
	kEffectMgr:AddAbil(AT_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_HITRATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_Tick140000001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick140000001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
