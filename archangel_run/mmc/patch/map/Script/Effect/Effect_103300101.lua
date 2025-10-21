
--  궁수 (보우 마스터리 : a_Bow Mastery) : Level 1-10 PASSIVE
function Effect_Begin103300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin103300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iPhy = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iPhy)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iPhy)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End103300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End103300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iPhy = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iPhy)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iPhy)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick103300101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick103300101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
