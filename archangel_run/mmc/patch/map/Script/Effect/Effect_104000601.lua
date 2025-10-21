
-- 공통도둑 (버로우 : Burrow) : Level 1-5
function Effect_Begin104000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- HIDE
	unit:SetAbil(AT_UNIT_HIDDEN, 1)
	unit:AddAbil(AT_LOCK_HIDDEN_ATTACK, 1)
	-- Auto Heal 중지
	local iAbil = unit:GetAbil(AT_ENABLE_AUTOHEAL)
	if iAbil > 1 then
		unit:SetAbil(AT_ENABLE_AUTOHEAL, iAbil-1)
	else
		unit:SetAbil(AT_ENABLE_AUTOHEAL, 0)
	end
	-- 움직일 수 없다.
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-ABILITY_RATE_VALUE)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	kEffect:SetValue(kEffect:GetAbil(AT_NEED_MP))
	return 1
end
                                                                                                                          
function Effect_End104000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- HIDE
	unit:SetAbil(AT_UNIT_HIDDEN, 0)
	unit:AddAbil(AT_LOCK_HIDDEN_ATTACK, -1)
	-- Auto Heal 중지
	unit:AddAbil(AT_ENABLE_AUTOHEAL, 1)
	-- 움직일 수 없다.
	kEffectMgr:AddAbil(AT_R_MOVESPEED, ABILITY_RATE_VALUE)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_Tick104000601(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick104000601 Effect=" .. effect:GetEffectNo())
	local iNeed = effect:GetAbil(AT_MP)
	--if iNeed == -1 then
	--	return ECT_MUSTDELETE -- delete effect
	--end
	if unit:GetAbil(AT_UNIT_HIDDEN) == 0 then
		return ECT_MUSTDELETE
	end

	local iMP = unit:GetAbil(AT_MP)
	if iMP+iNeed < 0 then
		return ECT_MUSTDELETE	-- Delete Effect
	end
	unit:SetAbil(AT_MP, iMP+iNeed, true, false)
	return ECT_DOTICK
end
