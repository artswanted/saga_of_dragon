
-- 닌자( 목둔술! 통나무 굴리기 발동: a_Rolling Log Play) : Level 1-10 PASSIVE
function Effect_Begin1700005001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1700005001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--명중률 감소
	local iAdd = kEffect:GetAbil(AT_HITRATE)
	kEffectMgr:AddAbil(AT_HITRATE, iAdd)
	unit:NftChangedAbil(AT_HITRATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	-- 이동 속도 감소
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End1700005001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End1700005001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--명중률 증가
	local iAdd = kEffect:GetAbil(AT_HITRATE)
	kEffectMgr:AddAbil(AT_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_HITRATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	-- 이동 속도 증가
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_Tick1700005001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1700005001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
