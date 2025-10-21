
-- 암살자 (난도질: a_Haphazard) : Lv 1-10
function Effect_Begin104301401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104301401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 공격속도 증가
	local iAdd = kEffect:GetAbil(AT_R_ATTAACKSPEED)
	kEffectMgr:AddAbil(AT_R_ATTAACKSPEED, iAdd)
	unit:NftChangedAbil(AT_R_ATTAACKSPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end
                                                                                                                          
function Effect_End104301401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104301401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- 공격속도 증가
	local iAdd = kEffect:GetAbil(AT_R_ATTAACKSPEED)
	kEffectMgr:AddAbil(AT_R_ATTAACKSPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_ATTAACKSPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end

function Effect_Tick104301401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104301401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
