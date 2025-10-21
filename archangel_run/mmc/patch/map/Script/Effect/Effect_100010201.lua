
-- 공통 : 저주
function Effect_Begin100010201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_STR)
	kEffectMgr:AddAbil(AT_R_STR, iAdd)
	unit:NftChangedAbil(AT_R_STR, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_INT)
	kEffectMgr:AddAbil(AT_R_INT, iAdd)
	unit:NftChangedAbil(AT_R_INT, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_CON)
	kEffectMgr:AddAbil(AT_R_CON, iAdd)
	unit:NftChangedAbil(AT_R_CON, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_DEX)
	kEffectMgr:AddAbil(AT_R_DEX, iAdd)
	unit:NftChangedAbil(AT_R_DEX, E_SENDTYPE_NONE)
	
	iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_NONE)

	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_STR)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_INT)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_CON)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_PHY_ATTACK_MAX)	
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+4, AT_R_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+5, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+6, AT_R_MAGIC_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+7, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+8, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+9, AT_R_MOVESPEED)
	unit:SendAbiles(10, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1
end

function Effect_End100010201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100010201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_STR)
	kEffectMgr:AddAbil(AT_R_STR, 0-iAdd)
	unit:NftChangedAbil(AT_R_STR, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_INT)
	kEffectMgr:AddAbil(AT_R_INT, 0-iAdd)
	unit:NftChangedAbil(AT_R_INT, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_CON)
	kEffectMgr:AddAbil(AT_R_CON, 0-iAdd)
	unit:NftChangedAbil(AT_R_CON, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_DEX)
	kEffectMgr:AddAbil(AT_R_DEX, 0-iAdd)
	unit:NftChangedAbil(AT_R_DEX, E_SENDTYPE_NONE)
	
	iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_NONE)

	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_STR)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_INT)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_CON)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_PHY_ATTACK_MAX)	
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+4, AT_R_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+5, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+6, AT_R_MAGIC_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+7, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+8, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+9, AT_R_MOVESPEED)
	unit:SendAbiles(10, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1	
end

function Effect_Tick100010201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100010201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK	
end


