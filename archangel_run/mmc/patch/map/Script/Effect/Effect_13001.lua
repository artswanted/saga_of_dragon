
-- PvP 방어력 보정
function Effect_Begin13001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin13001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- PvP HP
	local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
	kEffectMgr:AddAbil(AT_R_MAX_HP, iAdd)
	unit:NftChangedAbil( AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL )
	
	local iLevel = unit:GetAbil(AT_LEVEL)
	local iHandyCap = iLevel / 50
	iHandyCap = ( 5.0 - iHandyCap ) * 10000.0 -- 만분율로 올려주어야 한다.
	kEffect:SetActArgInt( AT_HANDYCAP, iHandyCap )

	if 0 ~= iHandyCap then
		-- Defence
		kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iHandyCap)
		kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iHandyCap)
		unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL )
		unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL )
	end
	return 1
end

function Effect_End13001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End13001...Cannot GetEffect(" .. iEffectNo ..")")
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_R_MAX_HP) * -1
	kEffectMgr:AddAbil( AT_R_MAX_HP, iAdd )
	unit:NftChangedAbil( AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL )
	
	local iHandyCap = kEffect:GetActArgInt(AT_HANDYCAP)
	if 0 ~= iHandyCap then
		-- Defence
		kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, -iHandyCap)
		kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, -iHandyCap)
		unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL )
		unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL )
	end
	return 1
end

function Effect_Tick13001(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end
