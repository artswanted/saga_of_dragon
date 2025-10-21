
-- 투사 (아이언 스킨 : a_Iron Skin) : Level 1-10 PASSIVE
function Effect_Begin106300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin106300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리방어력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	InfoLog(9, "Effect_Begin106300101 Before(AT_C_PHY_DEFENCE):" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin106300101 After(AT_C_PHY_DEFENCE):" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	return 1
end
                                                                                                                          
function Effect_End106300101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106300101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리방어력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick106300101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick106300101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
