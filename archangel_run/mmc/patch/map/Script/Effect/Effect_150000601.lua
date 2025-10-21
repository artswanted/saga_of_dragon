
function Effect_Begin150000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	return 1
end
                                                                                                                          
function Effect_End150000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	return 1
end

function Effect_Tick150000601(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1050000601 Effect=" .. effect:GetEffectNo())

	return ECT_DOTICK
end
