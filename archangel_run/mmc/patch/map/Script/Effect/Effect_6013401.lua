
-- 압둘라 넷바인더 : a_mon_shot_skill_03
function Effect_Begin6013401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin6013401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MoveSpeed = 0
	local iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	InfoLog(9, "Effect_Begin6013401 Before AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED) .. ", Added:" .. iAdd)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin6013401 After AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	return 1
end

function Effect_End6013401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End6013401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MoveSpeed = 0
	local iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick6013401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick6013401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
