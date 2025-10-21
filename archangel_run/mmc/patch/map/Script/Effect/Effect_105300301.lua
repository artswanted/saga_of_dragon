
-- 기사 (웨폰 프리징 : a_Weapon Freezing) : Level 1-10
function Effect_Begin105300301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105300301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- 공격을 할 수 없다.
	unit:AddAbil(AT_CANNOT_ATTACK, 1)
	
	return 1
end
                                                                                                                          
function Effect_End105300301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 공격을 할 수 없다.
	unit:AddAbil(AT_CANNOT_ATTACK, -1)

	return 1
end

function Effect_Tick105300301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105300301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
