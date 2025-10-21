
-- 사냥꾼 (사정거리 증가 : a_Increased Range) : Level 1-5 PASSIVE
function Effect_Begin110000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin110000601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 사정거리 증가
	local iAdd = kEffect:GetAbil(AT_ATTACK_RANGE)
	kEffectMgr:AddAbil(AT_ATTACK_RANGE, iAdd)
	unit:NftChangedAbil(AT_ATTACK_RANGE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End110000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110000601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 사정거리 증가
	local iAdd = kEffect:GetAbil(AT_ATTACK_RANGE)
	kEffectMgr:AddAbil(AT_ATTACK_RANGE, 0-iAdd)
	unit:NftChangedAbil(AT_ATTACK_RANGE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick110000601(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick110000601 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
