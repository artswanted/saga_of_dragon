
-- 부활 무적 Effect
function Effect_Begin11201(unit, iEffectNo, actarg)
	
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin11201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iValue = kEffect:GetAbil(AT_CANNOT_DAMAGE)
	unit:SetAbil( AT_CANNOT_DAMAGE, iValue, true, true )
	return 1
end

function Effect_End11201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End11201...Cannot GetEffect(" .. iEffectNo ..")")
		return 0
	end
	
	unit:SetAbil( AT_CANNOT_DAMAGE, 0, true, true )
	return 1
end

function Effect_Tick11201(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end
