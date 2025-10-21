-- Fighter (스톰 블레이드 : Storm blade ) : Level 1
function Effect_Begin101000801(unit, iEffectNo, actarg)
	--[[
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	]]
	-- 무적상태가 된다.
	unit:AddAbil(AT_CANNOT_DAMAGE, 1)
	--InfoLog(9, "Effect_Begin101000801 AT_CANNOT_DAMAGE:" .. unit:GetAbil(AT_CANNOT_DAMAGE))
	return 1
end
                                                                                                                          
function Effect_End101000801(unit, iEffectNo, actarg)
	--[[
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End101000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	]]
	-- 무적상태가 된다.
	unit:AddAbil(AT_CANNOT_DAMAGE, -1)
	--InfoLog(9, "Effect_End101000801 AT_CANNOT_DAMAGE:" .. unit:GetAbil(AT_CANNOT_DAMAGE))
	return 1
end

function Effect_Tick101000801(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101000801 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
