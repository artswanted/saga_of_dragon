
-- 위자드 (스트라이킹 : a_Striking ) : Level 1-10
function Effect_Begin109001301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--물리공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--마법공격력 증가
	iAdd = kEffect:GetAbil(AT_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end
                                                                                                                          
function Effect_End109001301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109001301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--물리공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--마법공격력 증가
	iAdd = kEffect:GetAbil(AT_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_Tick109001301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick109001301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
