
-- Fighter (머리로 방어 : Increased Defence) : Level 1-10
function Effect_Begin101000301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--InfoLog(9, "Effect_Begin101000301....EffectNo:" .. iEffectNo)
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	InfoLog(9, "Effect_Begin101000301 iAdd:" ..iAdd .. ", Before[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin101000301 After[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))	
	return 1
end

function Effect_End101000301(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End101000301 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--InfoLog(9, "Effect_End101000301....EffectNo:" .. iEffectNo)
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	--InfoLog(9, "Effect_End101000301 iAdd:" ..iAdd .. ", Before[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))	
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End101000301 After[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	return 1
end

function Effect_Tick101000301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101000301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

