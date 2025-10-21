
-- 데스마스터 블럭킹 : a_mon_blocking
function Effect_Begin6009401(unit, iEffectNo, actarg)
	InfoLog(5, "Effect_Begin609401 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin609401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- block rate == 100%
	local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	InfoLog(5, "Effect_Begin609401 iAdd:" .. iAdd);
	kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End6009401(unit, iEffectNo, actarg)
	InfoLog(5, "Effect_End609401 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End609401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- block rate == 100%
	local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	kEffectMgr:AddAbil(AT_DODGE_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick6009401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick609401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
