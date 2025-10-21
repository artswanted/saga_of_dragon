
-- 저격수 (아드레날린 : a_Adrenalin) : Level 1-10
function Effect_Begin150000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin150000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 공격속도 증가
	local iAdd = kEffect:GetAbil(AT_R_ATTACK_SPEED)
	InfoLog(9, "Effect_Begin150000701 Before:AT_R_ATTACK_SPEED:" .. unit:GetAbil(AT_C_ATTACK_SPEED))
	kEffectMgr:AddAbil(AT_R_ATTACK_SPEED, iAdd)
	unit:NftChangedAbil(AT_R_ATTACK_SPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)	
	InfoLog(9, "Effect_Begin150000701 After:AT_R_ATTACK_SPEED:" .. unit:GetAbil(AT_C_ATTACK_SPEED))
	
	return 1
end
                                                                                                                          
function Effect_End150000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End150000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 공격속도 감소
	local iAdd = kEffect:GetAbil(AT_R_ATTACK_SPEED)
	kEffectMgr:AddAbil(AT_R_ATTACK_SPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_ATTACK_SPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)	

	return 1
end

function Effect_Tick150000701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick150000701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
