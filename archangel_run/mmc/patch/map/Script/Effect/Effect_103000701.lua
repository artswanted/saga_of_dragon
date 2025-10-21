
-- 궁수 (공성병기화 : Siege Cannon) : Level 1
function Effect_Begin103000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iSpeed = unit:GetAbil(AT_C_MOVESPEED)
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1, iSpeed)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iSpeed)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	local iValue = unit:GetAbil(AT_ENABLE_AUTOHEAL)
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1+1, iSpeed)
	kEffectMgr:AddAbil(AT_ENABLE_AUTOHEAL, 0-iValue)
	unit:NftChangedAbil(AT_ENABLE_AUTOHEAL, E_SENDTYPE_NONE)
	iValue = unit:GetAbil(AT_CANNOT_USEITEM)
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1+2, iSpeed)
	kEffectMgr:AddAbil(AT_CANNOT_USEITEM, 0-iValue)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)
	return 1
end

function Effect_End103000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End103000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iSpeed = kEffect:GetActArgInt(ACT_ARG_CUSTOMDATA1)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iSpeed)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	local iValue = kEffect:GetActArgInt(ACT_ARG_CUSTOMDATA1+1)
	kEffectMgr:AddAbil(AT_ENABLE_AUTOHEAL, iValue)
	unit:NftChangedAbil(AT_ENABLE_AUTOHEAL, E_SENDTYPE_NONE)
	iValue = 	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1+2, iSpeed)
	kEffectMgr:AddAbil(AT_CANNOT_USEITEM, iValue)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)
	return 1
end

function Effect_Tick103000701(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end
