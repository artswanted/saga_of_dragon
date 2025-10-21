
-- 기사 (조인트 브레이크 : a_Joint Break) : Level 1-5
function Effect_Begin105300501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105300501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 이동속도 감소
	InfoLog(9, "Effect_Begin105300501 Before:AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	kEffectMgr:AddAbil(AT_R_MOVESPEED, kEffect:GetAbil(AT_R_MOVESPEED))
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300501 After:AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	-- 공격속도 감소
	InfoLog(9, "Effect_Begin105300501 Before:AT_C_ATTACK_SPEED:" .. unit:GetAbil(AT_C_ATTACK_SPEED))
	kEffectMgr:AddAbil(AT_R_ATTACK_SPEED, kEffect:GetAbil(AT_R_ATTACK_SPEED))
	unit:NftChangedAbil(AT_R_ATTACK_SPEED, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300501 After:AT_C_ATTACK_SPEED:" .. unit:GetAbil(AT_C_ATTACK_SPEED))
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_MOVESPEED)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_ATTACK_SPEED)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1
end
                                                                                                                          
function Effect_End105300501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 이동속도 감소
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-kEffect:GetAbil(AT_R_MOVESPEED))
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	-- 공격속도 감소
	kEffectMgr:AddAbil(AT_R_ATTACK_SPEED, 0-kEffect:GetAbil(AT_R_ATTACK_SPEED))
	unit:NftChangedAbil(AT_R_ATTACK_SPEED, E_SENDTYPE_NONE)
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_MOVESPEED)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_ATTACK_SPEED)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	return 1
end

function Effect_Tick105300501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105300501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
