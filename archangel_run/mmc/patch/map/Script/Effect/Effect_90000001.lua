
-- 공통(커플) (사랑의 힘으로 : a_Couple_Lover) : Level 1-10
function Effect_Begin90000001(unit, iEffectNo, actarg)	
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin90000001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_R_STR)
	kEffectMgr:AddAbil(AT_R_STR, iAdd)
	unit:NftChangedAbil(AT_R_STR, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	
	return 1
end
                                                                                                                          
function Effect_End90000001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End90000001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_R_STR)
	kEffectMgr:AddAbil(AT_R_STR, 0-iAdd)
	unit:NftChangedAbil(AT_R_STR, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	
	return 1
end

function Effect_Tick90000001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick90000001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
