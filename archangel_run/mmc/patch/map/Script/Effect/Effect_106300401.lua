
-- 투사 (스텀블펌 : a_Stumblebum) : Level 1-5
function Effect_Begin106300401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin106300401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 명중율 감소
	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	-- 공격력 감소
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_PHY_ATTACK_MIN)
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1
end
                                                                                                                          
function Effect_End106300401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106300201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 명중율 감소
	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	-- 공격력 감소
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, 0-iAdd)	
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_PHY_ATTACK_MIN)
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick106300401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick106300401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
