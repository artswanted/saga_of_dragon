
-- 투사 (투핸드마스터리 : a_Twohand Mastery) : Level 1-10 PASSIVE
function Effect_Begin106300701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin106300701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리공격력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End106300701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106300701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 물리공격력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick106300701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick106300701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
