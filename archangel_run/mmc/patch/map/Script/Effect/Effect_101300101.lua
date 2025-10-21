
-- 공통전사 (검 숙련 : Sword Mastery) : Level 1-10 PASSIVE
function Effect_Begin101300101(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_Begin101300101 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	--InfoLog(9, "Effect_Begin101300101 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	--InfoLog(9, "Effect_Begin101300101 AT_PHY_ATTACK_MAX:" .. unit:GetAbil(AT_PHY_ATTACK_MAX) ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	--InfoLog(9, "Effect_Begin101300101 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	--InfoLog(9, "Effect_Begin101300101 iAdd:" .. iAdd ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End101300101(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End101300101 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End101300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
		
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick101300101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101300101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
