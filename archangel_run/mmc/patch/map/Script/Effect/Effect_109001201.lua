
-- 워매이지 ( 어드벤스드 스피어 마스터리: a_Advanced Spear Mastery) : Level 1-10 PASSIVE
function Effect_Begin109001201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--공격력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	--InfoLog(9, "Effect_Begin109001201 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin109001201 AT_PHY_ATTACK_MAX:" .. unit:GetAbil(AT_PHY_ATTACK_MAX) ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MAX))
	--InfoLog(9, "Effect_Begin109001201 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin109001201 iAdd:" .. iAdd ..", After:" .. unit:GetAbil(AT_C_PHY_ATTACK_MIN))
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_End109001201(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End109001201 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109001201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--공격력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end

function Effect_Tick109001201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick109001201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
