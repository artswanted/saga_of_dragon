
-- 검투사 (샤프니스 블레이드 : a_Sharpness Blade) : Level 1-10 PASSIVE
function Effect_Begin106500101(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_Begin106500101 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin106500101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end
                                                                                                                          
function Effect_End106500101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106500101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--공격력 감소
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)	
	
	return 1
end

function Effect_Tick106500101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick106500101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
