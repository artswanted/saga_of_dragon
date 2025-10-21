
-- Dancer ( 문 워크: a_Moon Walk) : Level 1
function Effect_Begin160000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin160000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--무적 상태 만들기
	kEffectMgr:AddAbil(AT_CANNOT_DAMAGE, 1)
	unit:NftChangedAbil(AT_CANNOT_DAMAGE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_CANNOT_ATTACK, 1)
	unit:NftChangedAbil(AT_CANNOT_ATTACK, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End160000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End160000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--무적 상태 만들기
	kEffectMgr:AddAbil(AT_CANNOT_DAMAGE, -1)
	unit:NftChangedAbil(AT_CANNOT_DAMAGE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_CANNOT_ATTACK, -1)
	unit:NftChangedAbil(AT_CANNOT_ATTACK, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick160000401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick160000401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
