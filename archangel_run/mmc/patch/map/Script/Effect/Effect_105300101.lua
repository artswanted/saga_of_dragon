
-- 기사 (갑옷 숙련 : Armor Mastery) : Level 1-10 PASSIVE
function Effect_Begin105300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--방어력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End105300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--방어력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick105300101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105300101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
