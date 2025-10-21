
-- 기사 (갑옷 숙련 : Armor Mastery) : Level 1-10 PASSIVE
function Effect_Begin105300701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- BlockRate 증가
	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	InfoLog(9, "Effect_Begin105300701 AT_BLOCK_RATE:" .. iAdd .. ", Before:" .. unit:GetAbil(AT_C_BLOCK_RATE))
	kEffectMgr:AddAbil(AT_BLOCK_RATE, iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_NONE)
	--InfoLog(9, "Effect_Begin105300701 After:" .. unit:GetAbil(AT_C_BLOCK_RATE))
	--방어력 증가
	iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	--InfoLog(9, "Effect_Begin105300701 AT_PHY_DEFENCE:" .. iAdd .. ", Before:" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE)
	--InfoLog(9, "Effect_Begin105300701 AT_PHY_DEFENCE: After:" .. unit:GetAbil(AT_C_PHY_DEFENCE))

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_BLOCK_RATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_DEFENCE)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End105300701(unit, iEffectNo, actarg)
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
	--방어력 증가
	iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_BLOCK_RATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_DEFENCE)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick105300701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105300701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
