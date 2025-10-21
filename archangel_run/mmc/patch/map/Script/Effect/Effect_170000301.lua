
-- 닌자 (미혼향 : a_Sleep Smell ) : Level 1-10
function Effect_Begin170000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin170000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--방어력 낮추는 어빌이 들어가야함
--	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
--	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
--	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
--	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
--	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End170000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End170000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	return 1
end

function Effect_Tick170000301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick170000301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
