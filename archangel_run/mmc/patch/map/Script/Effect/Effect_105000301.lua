
-- 기사 (신성수호 : Divine Protection ) : Level 1-5
function Effect_Begin105000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iValue = unit:GetAbil(AT_C_PHY_DMG_DEC)
	kEffectMgr:AddAbil(AT_R_PHY_DMG_DEC, iValue-100)
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1, iValue-100)
	unit:NftChangedAbil(AT_R_PHY_DMG_DEC, E_SENDTYPE_NONE)
	
	iValue = unit:GetAbil(AT_C_MAGIC_DMG_DEC)
	kEffectMgr:AddAbil(AT_R_MAGIC_DMG_DEC, iValue-100)
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1+1, iValue-100)
	unit:NftChangedAbil(AT_R_MAGIC_DMG_DEC, E_SENDTYPE_NONE)	
	
	kEffect:SetValue(kEffect:GetAbil(AT_COUNT));
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DMG_DEC)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DMG_DEC)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	
	return 1
end

function Effect_End105000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iValue = kEffect:GetActArg(ACT_ARG_CUSTOMDATA1)
	kEffectMgr:AddAbil(AT_R_PHY_DMG_DEC, 0-iValue)
	unit:NftChangedAbil(AT_R_PHY_DMG_DEC, E_SENDTYPE_NONE)
	
	local iValue = kEffect:GetActArg(ACT_ARG_CUSTOMDATA1+1)
	kEffectMgr:AddAbil(AT_R_MAGIC_DMG_DEC, 0-iValue)
	unit:NftChangedAbil(AT_R_MAGIC_DMG_DEC, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DMG_DEC)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DMG_DEC)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	
	return 1
end

function Effect_Tick105000301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105000301 Effect=" .. effect:GetEffectNo())
end

