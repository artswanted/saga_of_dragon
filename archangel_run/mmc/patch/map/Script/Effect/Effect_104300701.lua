
-- 더블 크래시 (a_Double Crash) : Level 1-5
function Effect_Begin104300701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1043000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MoveSpeed = 0
	InfoLog(9, "Effect_Begin104300701 Before MoveSpeed:" .. unit:GetAbil(AT_C_MOVESPEED))
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-ABILITY_RATE_VALUE)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADCAST+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin104300701 After MoveSpeed:" .. unit:GetAbil(AT_C_MOVESPEED))
	return 1
end

function Effect_End104300701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104300701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MoveSpeed = 0
	kEffectMgr:AddAbil(AT_R_MOVESPEED, ABILITY_RATE_VALUE)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADCAST+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick104300701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104300701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
