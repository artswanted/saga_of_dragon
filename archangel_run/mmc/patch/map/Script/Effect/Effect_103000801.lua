
-- 궁수 공통 (EMP : EMP) : Level 1-5
function Effect_Begin103000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin103000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAbil = kEffect:GetAbil(AT_SPECIALABIL)
	kEffectMgr:AddAbil(iAbil, 1)
	unit:NftChangedAbil(iAbil, E_SENDTYPE_NONE)
	return 1
end

function Effect_End103000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End103000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAbil = kEffect:GetAbil(AT_SPECIALABIL)
	kEffectMgr:AddAbil(iAbil, -1)
	unit:NftChangedAbil(iAbil, E_SENDTYPE_NONE)
	return 1
end

function Effect_Tick103000801(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick103000801 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

