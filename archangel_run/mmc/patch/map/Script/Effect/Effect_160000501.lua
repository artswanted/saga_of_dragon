
-- Dancer ( 기합 : a_Shout) : Level 1
function Effect_Begin160000501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin160000501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--물리 공격력 감소
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	-- 물리 방어력 감소
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_PHY_DEFENCE)
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	return 1
end
                                                                                                                          
function Effect_End160000501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End160000501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--물리 공격력 감소
	local iAdd = kEffect:GetAbil(AT_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	-- 물리 방어력 감소
	local iAdd = kEffect:GetAbil(AT_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_PHY_DEFENCE)
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	return 1
end

function Effect_Tick160000501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick160000501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
