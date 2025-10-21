
-- PvP HandyCap
function Effect_Begin14001(unit, iEffectNo, actarg)
	
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin14001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iHandyCap = unit:GetAbil(AT_HANDYCAP)
	kEffect:SetActArgInt( AT_HANDYCAP, iHandyCap )
	
	if iHandyCap ~= 0 then
		kEffectMgr:AddAbil(AT_R_MAX_HP, iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAX_MP, iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_ATTACK, iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK, iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iHandyCap )
		
		local dwSendFlag = E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL
		unit:NftChangedAbil(AT_R_MAX_HP, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAX_MP, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_ATTACK, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_ATTACK, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_DEFENCE, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, dwSendFlag  )
	end
	return 1
end

function Effect_End14001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End14001...Cannot GetEffect(" .. iEffectNo ..")")
		return 0
	end
	
	local iHandyCap = kEffect:GetActArgInt(AT_HANDYCAP)
	if iHandyCap ~= 0 then
		kEffectMgr:AddAbil(AT_R_MAX_HP, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAX_MP, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_ATTACK, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, -iHandyCap )
		kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, -iHandyCap )
		
		local dwSendFlag = E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL
		unit:NftChangedAbil(AT_R_MAX_HP, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAX_MP, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_ATTACK, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_ATTACK, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, dwSendFlag )
		unit:NftChangedAbil(AT_R_PHY_DEFENCE, dwSendFlag )
		unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, dwSendFlag  )
	end
	return 1
end

function Effect_Tick14001(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end
