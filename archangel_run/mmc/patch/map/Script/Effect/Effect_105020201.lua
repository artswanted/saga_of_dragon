
-- 기사 (제네식기어-수호 : Genesic Gear - Protect )(EFFECTNUM1) : Level 1-10
function Effect_Begin105020201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105020201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리방어력 증가
	local iAddPhy = kEffectDef:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iValue)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)	
	-- 마법방어력 증가
	local iAddMag = kEffectDef:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iValue)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End105020201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105020201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리방어력 증가
	local iAddPhy = kEffectDef:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iValue)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)	
	-- 마법방어력 증가
	local iAddMag = kEffectDef:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, 0-iValue)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick105020201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105020201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
