
-- 투사 (제네식기어-공격 : Genesic Gear - Offense )(AT_EFFECTNUM1) : Level 1-10
function Effect_Begin106020201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리공격력 증가
	local iAddPhy = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAddPhy)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAddPhy)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)

	-- 마법공격력 증가
	local iAddMag = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAddMag)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAddMag)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_MAGIC_ATTACK_MIN)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End106020201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106020201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리공격력 증가
	local iAddPhy = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAddPhy)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, 0-iAddPhy)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)

	-- 마법공격력 증가
	local iAddMag = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, 0-iAddMag)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, 0-iAddMag)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_MAGIC_ATTACK_MIN)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	return 1
end

function Effect_Tick106020201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick106020201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
