
-- 성기사 (오라 발현 : a_Aura Activation) : Level 1 PASSIVE
function Effect_Begin105500501(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_Begin105500501 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105500501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	return 1
end
                                                                                                                          
function Effect_End105500501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105500501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	return 1
end

function Effect_Tick105500501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105500501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
