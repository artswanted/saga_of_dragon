
--  마법사 공통(스태프 숙련 : Staff Mastery) : Level 1-10 PASSIVE
function Effect_Begin102300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--물리공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	--마법공격력 증가
	iAdd = kEffect:GetAbil(AT_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_MAGIC_ATTACK_MIN)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End102300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--물리공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	--마법공격력 증가
	iAdd = kEffect:GetAbil(AT_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_MAGIC_ATTACK_MIN)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick102300101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102300101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
