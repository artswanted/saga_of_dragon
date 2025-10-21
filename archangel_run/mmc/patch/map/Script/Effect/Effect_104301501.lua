
-- 암살자 (베놈 크래셔: a_Venom Crasher) : Lv 1-5
function Effect_Begin104301501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104301501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_HP)
	kEffectMgr:AddAbil(AT_ABS_ADDED_DMG_PHY, iAdd)
	unit:NftChangedAbil(AT_ABS_ADDED_DMG_PHY, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_ABS_ADDED_DMG_MAGIC, iAdd)
	unit:NftChangedAbil(AT_ABS_ADDED_DMG_MAGIC, E_SENDTYPE_NONE)
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_ABS_ADDED_DMG_PHY)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_ABS_ADDED_DMG_MAGIC)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	return 1
end
                                                           
function Effect_End104301501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104301501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_HP)
	kEffectMgr:AddAbil(AT_ABS_ADDED_DMG_PHY, 0-iAdd)
	unit:NftChangedAbil(AT_ABS_ADDED_DMG_PHY, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_ABS_ADDED_DMG_MAGIC, 0-iAdd)
	unit:NftChangedAbil(AT_ABS_ADDED_DMG_MAGIC, E_SENDTYPE_NONE)
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_ABS_ADDED_DMG_PHY)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_ABS_ADDED_DMG_MAGIC)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick104301501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104301501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
