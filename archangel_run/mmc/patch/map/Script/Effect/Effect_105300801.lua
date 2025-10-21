
-- 기사 (a_Adv Sword Mastery) : Level 1-10 PASSIVE
function Effect_Begin105300801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- BlockRate 증가
	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	kEffectMgr:AddAbil(AT_BLOCK_RATE, iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_NONE)
	--공격력 증가
	iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	InfoLog(9, "Effect_Begin105300801 iAdd:" ..iAdd .. ", Before[AT_C_PHY_ATTACK_MAX]:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300801 iAdd:" ..iAdd .. ", After[AT_C_PHY_ATTACK_MAX]:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))	
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_BLOCK_RATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_PHY_ATTACK_MIN)	
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End105300801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- BlockRate 증가
	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	kEffectMgr:AddAbil(AT_BLOCK_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_NONE)
	--공격력 증가
	iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAdd)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_BLOCK_RATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_PHY_ATTACK_MIN)	
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)

	return 1
end

function Effect_Tick105300801(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105300701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
