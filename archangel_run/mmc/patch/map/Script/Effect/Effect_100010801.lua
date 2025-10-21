
-- 공통 : 수면
function Effect_Begin100010801(unit, iEffectNo, actarg)
	
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_CRITICAL_ONEHIT)
	unit:AddAbil(AT_CRITICAL_ONEHIT, iAdd)
	unit:NftChangedAbil(AT_CRITICAL_ONEHIT, E_SENDTYPE_BROADALL)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	iAdd = kEffect:GetAbil(AT_CANNOT_EQUIP)
	unit:AddAbil(AT_CANNOT_EQUIP, iAdd)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_CANNOT_USEITEM)
	unit:AddAbil(AT_CANNOT_USEITEM, iAdd)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)	
	return 1
end

function Effect_End100010801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--local iAdd = kEffect:GetAbil(AT_CRITICAL_ONEHIT)
	--unit:AddAbil(AT_CRITICAL_ONEHIT, 0-iAdd)
	--unit:NftChangedAbil(AT_CRITICAL_ONEHIT, E_SENDTYPE_BROADALL)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	iAdd = kEffect:GetAbil(AT_CANNOT_EQUIP)
	unit:AddAbil(AT_CANNOT_EQUIP, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_CANNOT_USEITEM)
	unit:AddAbil(AT_CANNOT_USEITEM, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)	

	return 1	
end

function Effect_Tick100010801(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100010801 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK	
end


