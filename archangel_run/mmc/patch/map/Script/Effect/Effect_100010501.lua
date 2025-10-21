
-- 공통 : 침묵
function Effect_Begin100010501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_CANNOT_CASTSKILL)
	unit:AddAbil(AT_CANNOT_CASTSKILL, iAdd)
	unit:NftChangedAbil(AT_CANNOT_CASTSKILL, E_SENDTYPE_SELF)
		
	return 1
end

function Effect_End100010501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100010501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_CANNOT_CASTSKILL)
	unit:AddAbil(AT_CANNOT_CASTSKILL, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_CASTSKILL, E_SENDTYPE_SELF)
	
	return 1	
end

function Effect_Tick100010501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100010501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK	
end


