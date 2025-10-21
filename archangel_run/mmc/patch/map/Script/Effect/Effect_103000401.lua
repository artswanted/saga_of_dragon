
-- Archer (버드 워칭 : a_Increased Status) : Level 1-10
function Effect_Begin103000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin103000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	kEffectMgr:AddAbil(AT_STR, kEffect:GetAbil(AT_STR))
	unit:NftChangedAbil(AT_STR, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_DEX, kEffect:GetAbil(AT_DEX))
	unit:NftChangedAbil(AT_DEX, E_SENDTYPE_NONE)
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_STR)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_DEX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_PHY_ATTACK_MIN)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_End103000401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End103000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	kEffectMgr:AddAbil(AT_STR, 0-kEffect:GetAbil(AT_STR))
	unit:NftChangedAbil(AT_STR, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_DEX, 0-kEffect:GetAbil(AT_DEX))
	unit:NftChangedAbil(AT_DEX, E_SENDTYPE_NONE)
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_STR)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_DEX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_PHY_ATTACK_MIN)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick103000401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick103000401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

