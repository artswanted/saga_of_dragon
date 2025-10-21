
-- Dancer ( 스탠스: a_Stance) : Level 1-10 PASSIVE
function Effect_Begin160000001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin160000001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--공격력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	--InfoLog(9, "Effect_Begin160000001 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin160000001 AT_PHY_ATTACK_MAX:" .. unit:GetAbil(AT_PHY_ATTACK_MAX) ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	--InfoLog(9, "Effect_Begin160000001 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin160000001 iAdd:" .. iAdd ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))

	--방어력 증가
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	--InfoLog(9, "Effect_Begin101000301 iAdd:" ..iAdd .. ", Before[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, iAdd)
	--unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin101000301 After[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))	
	
	return 1
end
                                                                                                                          
function Effect_End160000001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End160000001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--공격력 감소
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	--InfoLog(9, "Effect_End160000001 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End160000001 AT_PHY_ATTACK_MAX:" .. unit:GetAbil(AT_PHY_ATTACK_MAX) ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	--InfoLog(9, "Effect_End160000001 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End160000001 iAdd:" .. iAdd ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))

	--방어력 감소
	iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	--InfoLog(9, "Effect_End160000001 iAdd:" ..iAdd .. ", Before[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))	
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End160000001 After[AT_C_PHY_DEFENCE]:" .. unit:GetAbil(AT_C_PHY_DEFENCE))

	return 1
end

function Effect_Tick160000001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick160000001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
