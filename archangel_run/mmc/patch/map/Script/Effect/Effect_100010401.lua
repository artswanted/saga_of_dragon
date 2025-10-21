
-- 공통 : 결빙
function Effect_Begin100010401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_ATTACK_SPEED)
	kEffectMgr:AddAbil(AT_R_ATTACK_SPEED, iAdd)
	unit:NftChangedAbil(AT_R_ATTACK_SPEED, E_SENDTYPE_NONE)
		
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MOVESPEED)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_ATTACK_SPEED)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1
end

function Effect_End100010401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100010401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_ATTACK_SPEED)
	kEffectMgr:AddAbil(AT_R_ATTACK_SPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_ATTACK_SPEED, E_SENDTYPE_NONE)
		
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MOVESPEED)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_ATTACK_SPEED)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1	
end

function Effect_Tick100010401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100010401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK	
end


