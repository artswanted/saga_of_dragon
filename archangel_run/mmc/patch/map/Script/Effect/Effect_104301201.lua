
-- 암살자 (마인드 리딩: a_Mind Reading) : Lv 1-10
function Effect_Begin104301201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104301201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end
                                                                                                                          
function Effect_End104301201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104301201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_DODGE_RATE)
	kEffectMgr:AddAbil(AT_DODGE_RATE, iAdd)
	unit:NftChangedAbil(AT_DODGE_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end

function Effect_Tick104301201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104301201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
