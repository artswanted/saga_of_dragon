
--  도둑 (소매치기 : a_Pickpoket) : Level 1-10 PASSIVE
function Effect_Begin104000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	unit:AddAbil(AT_ADD_MONEY_PER_LV, kEffect:GetAbil(AT_ADD_MONEY_PER_LV))
	unit:AddAbil(AT_ADD_MONEY_RATE, kEffect:GetAbil(AT_ADD_MONEY_RATE))
	return 1
end

function Effect_End104000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	unit:AddAbil(AT_ADD_MONEY_PER_LV, 0-kEffect:GetAbil(AT_ADD_MONEY_PER_LV))
	unit:AddAbil(AT_ADD_MONEY_RATE, 0-kEffect:GetAbil(AT_ADD_MONEY_RATE))
	return 1
end

function Effect_Tick104000801(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104000801 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

