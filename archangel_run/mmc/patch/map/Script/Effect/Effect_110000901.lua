
-- 사냥꾼 (환각 : a_Displacement) : Level 1-10
function Effect_Begin110000901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin110000901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 회피율 증가
	local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End110000901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110000901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 회피율 증가
	local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick110000901(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick110000901 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
